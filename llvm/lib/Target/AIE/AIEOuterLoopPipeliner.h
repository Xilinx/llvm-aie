//===-- AIEOuterLoopPipeliner.h - Outer Loop Pipeliner Structures -*- C++ -*-=//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Outer loop pipelining for AIE: overlaps the stage-0 chain of outer iteration
// i+1 with the inner loop + store chain of iteration i.
//
// Positions are named top / inner / bottom (the outer loop's header, inner
// loop, and latch). The top block splits into stage-0 and stage-1 instructions
// by role (the partitioning strategy lives in collectStages):
//   Stage 0 = the prefetched chain: cloned into the stage0.top preheader above
//             the steady loop and prefetched in the bottom block for the next
//             iteration.
//   Stage 1 = the rest of the top: kept in the steady top and re-cloned into
//             the last-iteration top. May be empty, in which case the whole top
//             is stage 0.
//
// Original CFG:
//
//   [preheader]
//       |
//   [top]  <------------------\  <- stage 0 + stage 1 (+ set.loop.iterations)
//       |                      |
//   [inner.*]                  |  <- inner loop
//       |                      |
//   [bottom]  ----------------/   <- stage 1 + latch
//       |  (exit branch)
//   [exit]
//
// Produced CFG:
//
//   [preheader]
//       |
//   [stage0.top]                             <- stage 0 (preheader copy)
//       |
//   [steady.stage1.top]  <---------------\   <- stage 1 (+ set.loop.iterations)
//       |                                 |
//   [steady.stage1.inner.*]              |   <- steady-state inner loop
//       |                                 |
//   [steady.stage1.bottom.and.stage0.top]-/  <- stage 1 + next-iter stage 0
//       |  (exit branch)
//   [lastiter.stage1.top]                    <- stage 1 (+ set.loop.iterations)
//       |
//   [lastiter.stage1.inner.*]                <- inner loop clone
//       |
//   [lastiter.stage1.bottom]                 <- stage 1 stores only
//       |
//   [exit]
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEOUTERLOOPPIPELINER_H
#define LLVM_LIB_TARGET_AIE_AIEOUTERLOOPPIPELINER_H

#include "Utils/AIELoopOptionOverrides.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/STLFunctionalExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/ValueMapper.h"

#include <memory>
#include <optional>

namespace llvm {
struct AIEBaseInstrInfo;
class DominatorTree;
class Function;
class ScalarEvolution;
} // namespace llvm

namespace llvm::OuterLoopPipelining {

// Source value -> replacement (clone, or a header PHI's resolved incoming),
// consumed by RemapInstruction. Blocks and instructions share one table.
using RemapTable = ValueToValueMapTy;

// A single-entry/single-exit run of basic blocks in program order, queried and
// iterated through one interface that extends to multi-block regions.
class BlockRegion {
  SmallVector<BasicBlock *, 4> Blocks;

  // No edge enters a non-entry block and none leaves a non-exit block: the run
  // is single-entry/single-exit. Vacuously true for a single-block region.
  [[maybe_unused]] bool isSingleEntrySingleExit() const;

public:
  void assign(ArrayRef<BasicBlock *> BBs);

  size_t size() const { return Blocks.size(); }
  bool empty() const { return Blocks.empty(); }

  BasicBlock *entry() const {
    return Blocks.empty() ? nullptr : Blocks.front();
  }
  BasicBlock *back() const { return Blocks.empty() ? nullptr : Blocks.back(); }

  bool contains(const BasicBlock *BB) const {
    return llvm::is_contained(Blocks, BB);
  }
  bool contains(const Instruction *I) const { return contains(I->getParent()); }

  ArrayRef<BasicBlock *> blocks() const { return Blocks; }

  // Visit every instruction of the region in program order, block by block.
  // Works identically whether the region is one block or several.
  void forEachInstruction(function_ref<void(Instruction *)> Visit) const;
};

struct LatchConditionInfo {
  ICmpInst *Cmp = nullptr;
  Value *Limit = nullptr;
  unsigned LimitIdx = 0;
  int64_t Step = 0;
  BinaryOperator *Counter = nullptr;
  PHINode *OldIV = nullptr;

  // A unit-downcounting latch (step -1), convertible to
  // @llvm.loop.decrement.reg.
  bool isDowncounting() const { return Step == -1; }
};

class LoopStructure {
protected:
  BasicBlock *InnerExit = nullptr;

  SmallVector<BasicBlock *, 4> InnerLoopBlocks;

  MDNode *OuterLoopID = nullptr;

  // The top region in program order; its entry is the top block.
  BlockRegion TopRegion;

  // The bottom region in program order; its exit is the bottom block.
  BlockRegion BottomRegion;

  LatchConditionInfo OuterLoopCondition;

  // Returns true if I is in the top region.
  bool isInTop(const Instruction *I) const { return TopRegion.contains(I); }

public:
  LoopStructure() = default;
  virtual ~LoopStructure() = default;

  BasicBlock *getTop() const { return TopRegion.entry(); }
  BasicBlock *getBottom() const { return BottomRegion.back(); }

  // LoopInfo lists a loop's header first, and cloning preserves block order.
  BasicBlock *getInnerHeader() const { return InnerLoopBlocks.front(); }
  // The single inner-loop predecessor of the inner header (single-latch form).
  BasicBlock *getInnerLatch() const;
  BasicBlock *getInnerExit() const { return InnerExit; }
  ArrayRef<BasicBlock *> getInnerBlocks() const { return InnerLoopBlocks; }
  MDNode *getOuterLoopID() const { return OuterLoopID; }

  // The LS blocks in program order: top, inner-loop blocks, bottom.
  SmallVector<BasicBlock *, 8> blocksInProgramOrder() const;

  const BlockRegion &topRegion() const { return TopRegion; }
  const BlockRegion &bottomRegion() const { return BottomRegion; }

  const LatchConditionInfo &latchCondition() const {
    return OuterLoopCondition;
  }

  // Returns true if I is in the bottom region.
  bool isInBottom(const Instruction *I) const {
    return BottomRegion.contains(I);
  }

  // The outer loop preheader. The original derives it from LoopInfo; a clone
  // returns its stored preheader.
  virtual BasicBlock *getPreheader() const = 0;

  // Returns the bottom-block terminator as a BranchInst.
  BranchInst *getLatchBranch() const {
    return cast<BranchInst>(getBottom()->getTerminator());
  }

  // The bottom successor that leaves the loop (the non-top edge).
  BasicBlock *getExitBlock() const;
};

// The original loop, built from LoopInfo. Owns the LoopInfo loops and the
// stage-0 / stage-1 instruction lists. Only obtainable validated, via
// analyze().
class OrigLoopStructure : public LoopStructure {
  Loop *OuterLoop = nullptr;
  Loop *InnerLoop = nullptr;

  // The stage-0 / stage-1 split of the top-block instructions, in program
  // order.
  SmallVector<Instruction *, 16> Stage0Insts;
  SmallVector<Instruction *, 16> Stage1Insts;

  explicit OrigLoopStructure(Loop *L) : OuterLoop(L) {}

  Loop *getOuterLoop() const { return OuterLoop; }

  // Populate the inner-loop fields and top/bottom regions from
  // OuterLoop; returns false unless the loop is a supported pipelining
  // candidate.
  bool analyzeLoopStructure();

  // Every outer-loop block belongs to the top, the inner loop, or the
  // single-block bottom; false on any other (unsupported) shape.
  bool allOuterBlocksAccountedFor() const;

  // Validate the linear single-block top (top block == inner preheader)
  // and populate the top region; false for any other shape.
  bool discoverTopRegion();

  /// \return true if latch exit condition are valid and the loop bound can be
  /// adjusted (Step != 0).
  bool tryAdjustLoopBound();

  // True if every incoming block is inside the top region (cloned with
  // it), as opposed to a loop-carried PHI resolved to a concrete value when
  // cloning.
  bool isRegionInternalPhi(const PHINode *PHI) const;

  // True if I lives in the top and is a plain instruction or a
  // region-internal PHI; loop-carried PHIs are excluded.
  bool isPipelineableValue(const Instruction *I) const;

  // A pipelineable value also clonable into the prefetch/last-iteration sites;
  // excludes terminators and hardware-loop setup.
  bool isPipelineCandidate(const Instruction *I) const;

  // Returns true if the inner loop is a hardware (JNZD) loop, i.e. its latch is
  // controlled by an @llvm.loop.decrement intrinsic.
  bool isInnerLoopHardwareLoop() const;

  // Returns the top-block loads.
  SmallVector<LoadInst *, 8> collectTopLoads() const;

  // Returns the bottom-block stores.
  SmallVector<StoreInst *, 8> collectBottomStores() const;

  // Worklist closure over pipelineable neighbours (forward via users, backward
  // via operands), each added to Set once; Set must already hold the seeds.
  void forwardClosure(SmallVectorImpl<Instruction *> &Worklist,
                      SmallPtrSetImpl<Instruction *> &Set) const;
  void backwardClosure(SmallVectorImpl<Instruction *> &Worklist,
                       SmallPtrSetImpl<Instruction *> &Set) const;

  // The stage-1 set: the split points (IsSplitPoint, reachable from the
  // top-block loads) and their top-block descendants. Empty if none is found.
  SmallPtrSet<Instruction *, 32>
  collectStage1Cone(function_ref<bool(const Instruction *)> IsSplitPoint) const;

  // The fallback stage-0 collection (no split point): pipeline candidates
  // backward-reachable from inner-loop uses, in program order.
  void collectStage0FromInnerLoop();

public:
  // Build and validate the LS for L; nullptr if L is not a supported candidate.
  static std::unique_ptr<OrigLoopStructure> analyze(Loop *L);

  Loop *getInnerLoop() const { return InnerLoop; }

  BasicBlock *getPreheader() const override {
    return OuterLoop->getLoopPreheader();
  }

  const SmallVectorImpl<Instruction *> &stage0Insts() const {
    return Stage0Insts;
  }
  const SmallVectorImpl<Instruction *> &stage1Insts() const {
    return Stage1Insts;
  }

  // True if rotating pays off: hardware inner loop, outer trip count (from SE)
  // meets MinTripCount, and the bottom has stores.
  bool isProfitableToRotate(ScalarEvolution &SE, unsigned MinTripCount) const;

  // Returns true if it is safe to reorder the top-block loads before the
  // bottom-block stores (rejects volatile/atomic memory ops).
  bool isSafeToReorderMemoryOps() const;

  // Split the top-block pipeline candidates into the stage-1 split-point cone
  // and the stage-0 remainder (the load/address chain).
  void collectStages(function_ref<bool(const Instruction *)> IsSplitPoint);

  // Delete this (now unreachable) LS's blocks.
  void removeFromCFG() const;
};

// A steady-state or last-iteration clone. Owns its source->clone CloneMap and a
// stored outer preheader.
class CloneLoopStructure : public LoopStructure {
  BasicBlock *OuterPreheader = nullptr;

  // Each source value mapped to its clone in this LS.
  RemapTable CloneMap;

public:
  // Deep-clone Src into "<name>.<Suffix>" IR; internal references are remapped,
  // edges leaving the LS stay at Src's externals for the caller to rewire.
  CloneLoopStructure(const LoopStructure &Src, const Twine &Suffix);

  // Tag for the last-iteration skeleton constructor below.
  struct LastIterSkeletonTag {};

  // Create the empty last-iteration blocks spliced before Src's exit and record
  // the Src->lastiter block mappings; the caller fills the bodies. Src is the
  // structural source (the original loop), so the last iteration is a clone of
  // the original body.
  CloneLoopStructure(const LoopStructure &Src, LastIterSkeletonTag);

  // Clone of V in this LS, or V itself if it has none (invariants/args pass
  // through). Direction is source->clone, matching RemapInstruction.
  Value *cloneOf(Value *V) const {
    auto It = CloneMap.find(V);
    return It != CloneMap.end() ? static_cast<Value *>(It->second) : V;
  }
  // Clone of block BB in this LS. BB must have been cloned into this LS.
  BasicBlock *clonedBlock(BasicBlock *BB) const {
    return cast<BasicBlock>(cloneOf(BB));
  }
  RemapTable &cloneMap() { return CloneMap; }
  const RemapTable &cloneMap() const { return CloneMap; }

  // Repoint Src's recorded clone to NewClone. Used when a clone is superseded,
  // e.g. a stage-0 clone replaced by a pipelined header PHI, so the map keeps
  // naming the value that actually represents Src in this loop.
  void retargetClone(Value *Src, Value *NewClone) { CloneMap[Src] = NewClone; }

  // The value Src carries into the final (peeled) iteration: if Src's clone is
  // a header PHI of this loop, the value on its back edge (the prefetched /
  // next-iteration value); otherwise the clone itself.
  Value *lastIterInputFor(Value *Src) const;

  BasicBlock *getPreheader() const override { return OuterPreheader; }

  // Record a preheader the header PHIs already reference (its branch is rewired
  // by the caller). Use installPreheader to splice in a fresh block instead.
  void recordExistingPreheader(BasicBlock *BB);

  // Splice a fresh block in as this LS's preheader, repointing the header PHIs'
  // edge from the current preheader to NewPreheader.
  void installPreheader(BasicBlock *NewPreheader);

  // Retarget the cached latch condition's pointers from the source LS to this
  // clone, so adjustLoopBound / getDowncountingInfo operate on the clone.
  void remapBoundThroughCloneMap();

  // Adjust the outer loop trip count from N to N-1. Returns the new limit
  // Value.
  Value *adjustLoopBound() const;

  // Repair loop metadata (trip count changed): decrement itercount.range, drop
  // the consumed enable hint, and append the success marker.
  void updateLoopMetadata() const;
};

class AIEOuterLoopPipeliner : public FunctionPass {
public:
  static char ID;
  AIEOuterLoopPipeliner() : FunctionPass(ID) {}
  bool runOnFunction(Function &F) override;
  void getAnalysisUsage(AnalysisUsage &AU) const override;
  StringRef getPassName() const override { return "AIE Outer Loop Pipeliner"; }

private:
  LoopInfo *LI = nullptr;
  DominatorTree *DT = nullptr;
  ScalarEvolution *SE = nullptr;
  const AIEBaseInstrInfo *TII = nullptr;

  bool runOnLoop(Loop *L);
  // Run every pipelining precondition as a flat early-return guard and
  // transform L iff all pass. True if L was pipelined.
  bool tryPipelineLoop(Loop *L, const AIE::LoopOptionOverrides &Overrides);
  // Seed Map with each outer-header PHI's incoming value on the FromEdge edge,
  // collapsing loop-carried PHIs to the values seen entering via that edge.
  void seedHeaderPhiEdge(RemapTable &Map, const LoopStructure &LS,
                         BasicBlock *FromEdge) const;
  bool performTransformation(OrigLoopStructure &OrigLS,
                             const AIE::LoopOptionOverrides &Overrides);

  // Swap a freshly cloned steady-state LS in for the original (rewire preheader
  // and exit PHIs), leaving the original unreachable and ready for deletion.
  void swapInClonedLS(const OrigLoopStructure &OrigLS,
                      CloneLoopStructure &SteadyLS) const;

  // Clone OrigLS's stage-0 chain into a new stage-0 top block before the steady
  // loop and adopt it as the steady preheader (entry pointer values).
  void cloneStage0IntoPreheader(const OrigLoopStructure &OrigLS,
                                CloneLoopStructure &SteadyLS,
                                RemapTable &PreheaderVMap);

  // Clone OrigLS's stage-0 chain into the bottom block using next-iteration
  // pointer values, so the loads prefetch for the next iteration.
  void cloneStage0IntoBottom(const OrigLoopStructure &OrigLS,
                             const CloneLoopStructure &SteadyLS,
                             RemapTable &BottomVMap);

  // For each steady stage-0 instruction, merge its preheader (entry-edge) and
  // bottom (back-edge) clones via a header PHI, erase the instruction, and
  // retarget SteadyLS's clone map so the slot now resolves to the merged PHI.
  void createPipelinedPHIs(const OrigLoopStructure &OrigLS,
                           CloneLoopStructure &SteadyLS,
                           const RemapTable &PreheaderVMap,
                           const RemapTable &BottomVMap);

  // Create the last-iteration region (top + inner-loop clone + stores-only
  // bottom) and redirect the steady latch exit into it.
  void peelLastIteration(const OrigLoopStructure &OrigLS,
                         const CloneLoopStructure &SteadyLS);

  // Clone I into Dest before InsertPt, record orig->clone in VMap, and return
  // the clone. A non-empty Suffix renames non-void clones to "<orig><Suffix>".
  static Instruction *cloneInstInto(Instruction &I, BasicBlock &Dest,
                                    BasicBlock::iterator InsertPt,
                                    RemapTable &VMap, const Twine &Suffix);
  // Clone Insts (in order) into DstBB before InsertPt, then remap operands of
  // all the clones through VMap. Returns the clones, parallel to Insts.
  static SmallVector<Instruction *, 16>
  cloneAndRemapInsts(ArrayRef<Instruction *> Insts, BasicBlock &DstBB,
                     BasicBlock::iterator InsertPt, RemapTable &VMap,
                     const Twine &Suffix);
  // Second-pass remap of freshly inserted clones, once all are in place.
  static void remapClones(ArrayRef<Instruction *> Clones, RemapTable &VMap);

  // Translate each instruction of the original LS to its clone via VMap;
  // unmapped entries (e.g. loop-invariant operands) pass through unchanged.
  static SmallVector<Instruction *, 16>
  remapToClone(ArrayRef<Instruction *> Insts, const RemapTable &VMap);

  // Step helpers of peelLastIteration, in call order. The last iteration is
  // cloned structurally from OrigLS (orig->lastiter map); its live-in values
  // come from SteadyLS via seedLastIterInputs / lastIterInputFor.

  // Seed Map so each stage-0 slot and loop-carried header PHI of OrigLS
  // resolves to the value SteadyLS carries into the last iteration (its
  // prefetch / next-iteration value).
  void seedLastIterInputs(RemapTable &Map, const OrigLoopStructure &OrigLS,
                          const CloneLoopStructure &SteadyLS) const;

  // Clone the hardware-loop setup (set.loop.iterations) from Src's top into the
  // last-iteration top.
  void cloneHardwareLoopSetupInto(CloneLoopStructure &LastIterLS,
                                  const LoopStructure &Src) const;

  // Fill the last-iteration inner-loop block clones with remapped OrigLS
  // instruction bodies and wire the top into the inner header.
  void cloneInnerLoopIntoLastIter(const OrigLoopStructure &OrigLS,
                                  CloneLoopStructure &LastIterLS) const;

  // Fill the last-iteration bottom with the pristine original stores and
  // pointer updates only (no prefetch loads), cloned directly from OrigLS.
  void populateLastIterBottom(const OrigLoopStructure &OrigLS,
                              CloneLoopStructure &LastIterLS) const;

  // Splice the last-iteration into the CFG: last-iteration bottom -> original
  // exit, repoint the exit's bottom-predecessor PHIs to it, redirect the
  // steady bottom exit to the last-iteration top.
  void wireLastIterIntoCFG(const OrigLoopStructure &OrigLS,
                           const CloneLoopStructure &SteadyLS,
                           const CloneLoopStructure &LastIterLS) const;

  // The bottom instructions forming PHI's next-iteration pointer-update
  // chain, or nullopt if it cannot be safely lifted.
  std::optional<SmallPtrSet<Instruction *, 16>>
  collectLiftableBottomChain(const OrigLoopStructure &OrigLS,
                             PHINode &PHI) const;

  // Lift the bottom pointer-update chains (add.2d/add.3d and descendants) to
  // the top-block end so top-block cloning naturally covers them. True if
  // moved.
  bool liftBottomPointerUpdatesToTop(const OrigLoopStructure &OrigLS);

  // Convert the steady loop to a JNZD hardware loop: start.loop.iterations in
  // the preheader, a counter PHI in the header, loop.decrement.reg in the
  // latch. Requires latchCondition().isDowncounting().
  void convertOuterLoopToHardwareLoop(const CloneLoopStructure &SteadyLS);
};

} // namespace llvm::OuterLoopPipelining

#endif // LLVM_LIB_TARGET_AIE_AIEOUTERLOOPPIPELINER_H
