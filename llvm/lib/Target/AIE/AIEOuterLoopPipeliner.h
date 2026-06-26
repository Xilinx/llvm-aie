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
// Outer loop pipelining for AIE: overlaps the stage-0 chain (prologue) of outer
// iteration i+1 with the inner loop + store chain (epilogue) of iteration i.
//
// The prologue (the outer header) splits into stage-0 and stage-1 instructions
// by what the transform does with each:
//   Stage 0 = the load/address chain: loads plus the pointer/address arithmetic
//             feeding the inner loop. Moved out of the steady header — copied
//             before the loop and prefetched in the epilogue for the next
//             iteration.
//   Stage 1 = the split-point cone: the stage-1 split points (see
//             isStage1SplitPoint) reachable from the stage-0 chain, plus their
//             descendants in the prologue. Kept in the steady header and
//             re-cloned into lastiter.prologue. Only split-prologue mode
//             populates stage 1; with it off the whole prologue chain is
//             stage 0.
//
// Original CFG:
//
//   [preheader]
//       |
//   [outer.header]  <----------\  <- Prologue Content:
//       |                       |        Stage 0
//       |                       |        Stage 1 (+ set.loop.iterations)
//       |                       |
//   [outer.inner.*]             |  <- inner loop [Stage 1]
//       |                       |
//       |                       |
//   [outer.latch]  ------------/  <- Epilogue [Stage 1] + Latch
//       |  (exit branch)
//   [exit]
//
// Produced CFG:
//
//   [preheader]
//       |
//   [steady.preheader]    <- Stage 0:
//       |
//   [steady.header]  <---------\  <- Stage 1: Prologue
//       |                       |     (+ set.loop.iterations)
//       |                       |
//   [steady.inner.*]            |  <- Stage 1: steady-state inner loop
//       |                       |
//       |                       |
//   [steady.latch]  -----------/  <- Stage 1: outer.latch + Stage 0 Prologue
//       |  (exit branch)
//   [lastiter.prologue]        <- Stage 1: Prologue
//       |                             (+ set.loop.iterations)
//       |
//   [steady.inner.*.lastiter]  <- Stage 1: inner loop clone
//       |
//   [lastiter.epilogue]        <- Stage 1: Epilog
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

#include <optional>

namespace llvm {
struct AIEBaseInstrInfo;
class DominatorTree;
class Function;
class ScalarEvolution;
} // namespace llvm

namespace llvm::OuterLoopPipelining {

// A substitution table consumed by RemapInstruction in a single pass: each
// source value maps to its replacement, either a clone or the concrete
// incoming value of a header PHI resolved on a chosen edge. Instructions and
// basic blocks share one table because the remapper rewrites operands and
// branch / PHI-incoming targets together.
using RemapTable = ValueToValueMapTy;

// An ordered, contiguous run of basic blocks in program order (entry first,
// exit last). Today the prologue and epilogue are each a single block; the
// wrapper lets callers test membership and iterate instructions through one
// interface that would extend to multi-block regions without changing them.
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
};

// Holds the pre-validated components of a downcounting outer loop exit
// condition, computed once by getDowncountingInfo and consumed by
// convertOuterLoopToHardwareLoop.
// All fields are guaranteed non-null when the struct is returned.
struct DowncountingInfo {
  ICmpInst *Cmp;
  BinaryOperator *Counter;
  PHINode *OldIV;
};

class LoopStructure {
protected:
  BasicBlock *InnerExit = nullptr;

  SmallVector<BasicBlock *, 4> InnerLoopBlocks;

  MDNode *OuterLoopID = nullptr;

  // The prologue region in program order; its entry block is the outer header.
  BlockRegion PrologueRegion;

  // The epilogue region in program order; its exit block is the outer latch.
  BlockRegion EpilogueRegion;

  LatchConditionInfo OuterLoopCondition;

  // Returns true if I is in the prologue region (outer header).
  bool isInPrologue(const Instruction *I) const {
    return PrologueRegion.contains(I);
  }

public:
  LoopStructure() = default;
  virtual ~LoopStructure() = default;

  BasicBlock *getOuterHeader() const { return PrologueRegion.entry(); }
  BasicBlock *getOuterLatch() const { return EpilogueRegion.back(); }

  // The inner preheader is the prologue entry: the linear single-block prologue
  // is the outer header, which is also the inner loop's preheader.
  BasicBlock *getInnerPreheader() const { return getOuterHeader(); }
  // LoopInfo lists a loop's header first, and cloning preserves block order.
  BasicBlock *getInnerHeader() const { return InnerLoopBlocks.front(); }
  // The single inner-loop predecessor of the inner header (single-latch form).
  BasicBlock *getInnerLatch() const {
    for (BasicBlock *Pred : predecessors(getInnerHeader()))
      if (is_contained(InnerLoopBlocks, Pred))
        return Pred;
    return nullptr;
  }
  BasicBlock *getInnerExit() const { return InnerExit; }
  ArrayRef<BasicBlock *> getInnerBlocks() const { return InnerLoopBlocks; }
  MDNode *getOuterLoopID() const { return OuterLoopID; }

  BlockRegion &prologueRegion() { return PrologueRegion; }
  const BlockRegion &prologueRegion() const { return PrologueRegion; }
  BlockRegion &epilogueRegion() { return EpilogueRegion; }
  const BlockRegion &epilogueRegion() const { return EpilogueRegion; }

  LatchConditionInfo &bound() { return OuterLoopCondition; }
  const LatchConditionInfo &bound() const { return OuterLoopCondition; }

  // Returns true if I is in the epilogue region (outer latch).
  bool isInEpilogue(const Instruction *I) const {
    return EpilogueRegion.contains(I);
  }

  // The outer loop preheader. The original derives it from LoopInfo; a clone
  // returns its stored preheader.
  virtual BasicBlock *getPreheader() const = 0;

  // Returns the outer latch terminator as a BranchInst.
  BranchInst *getLatchBranch() const {
    return cast<BranchInst>(getOuterLatch()->getTerminator());
  }

  // The latch successor that leaves the loop (the non-header edge).
  BasicBlock *getExitBlock() const;
};

// The original loop, built from LoopInfo. Owns the LoopInfo loops, the validity
// flag, and the stage-0 / stage-1 instruction lists.
class OrigLoopStructure : public LoopStructure {
  Loop *OuterLoop = nullptr;
  Loop *InnerLoop = nullptr;

  // True once analyzeLoopStructure has validated the loop as a supported
  // pipelining candidate.
  bool Valid = false;

  // The stage-0 / stage-1 split of the prologue instructions, in program order.
  SmallVector<Instruction *, 16> Stage0Insts;
  SmallVector<Instruction *, 16> Stage1Insts;

  Loop *getOuterLoop() const { return OuterLoop; }

  // Populate the inner-loop fields and prologue/epilogue regions from
  // OuterLoop, validating that the loop is a supported pipelining candidate.
  // Called once by the constructor; the result is cached in Valid.
  bool analyzeLoopStructure();

  // Validate that the prologue is the linear single-block case (the outer
  // header is the inner preheader) and populate prologueRegion(). Returns false
  // for any other shape (a separate inner preheader block between the outer
  // header and the inner loop), which is not handled.
  bool discoverPrologueRegion();

  /// \return true if latch exit condition are valid and the loop bound can be
  /// adjusted (Step != 0).
  bool tryAdjustLoopBound();

  // A region-internal PHI has every incoming block inside the prologue region;
  // it is cloned with the region. A loop-carried PHI in the entry instead has
  // incoming edges from the preheader and latch (outside the region) and is
  // resolved to a concrete value when cloning, never cloned as a PHI.
  bool isRegionInternalPhi(const PHINode *PHI) const;

  // A value is pipelineable when it lives in the prologue region and is a plain
  // instruction or a region-internal PHI. Loop-carried PHIs are excluded: they
  // resolve to concrete values via the clone VMap rather than being cloned.
  bool isPipelineableValue(const Instruction *I) const;

  // A pipeline candidate is a pipelineable value that is also clonable into the
  // prefetch/last-iteration sites: block terminators and hardware-loop setup
  // are excluded. Combine with prologueRegion().forEachInstruction to walk
  // candidates in program order.
  bool isPipelineCandidate(const Instruction *I) const;

  // Returns true if the inner loop is a hardware (JNZD) loop, i.e. its latch is
  // controlled by an @llvm.loop.decrement intrinsic.
  bool isInnerLoopHardwareLoop() const;

  // Returns the prologue (outer header) loads.
  SmallVector<LoadInst *, 8> collectPrologueLoads() const;

  // Returns the epilogue (outer latch) stores.
  SmallVector<StoreInst *, 8> collectEpilogueStores() const;

public:
  explicit OrigLoopStructure(Loop *L);

  Loop *getInnerLoop() const { return InnerLoop; }

  BasicBlock *getPreheader() const override {
    return OuterLoop->getLoopPreheader();
  }

  // True if the loop was validated as a supported pipelining candidate by the
  // constructor.
  bool isValid() const { return Valid; }

  SmallVectorImpl<Instruction *> &stage0Insts() { return Stage0Insts; }
  const SmallVectorImpl<Instruction *> &stage0Insts() const {
    return Stage0Insts;
  }
  SmallVectorImpl<Instruction *> &stage1Insts() { return Stage1Insts; }
  const SmallVectorImpl<Instruction *> &stage1Insts() const {
    return Stage1Insts;
  }

  // Returns true if this LS is profitable to rotate: the inner loop is a
  // hardware loop, the outer trip count meets MinTripCount, and the epilogue
  // has stores. SE supplies the outer-loop minimum trip count.
  bool isProfitableToRotate(ScalarEvolution &SE, unsigned MinTripCount) const;

  // Returns true if it is safe to reorder the prologue loads before the
  // epilogue stores (rejects volatile/atomic memory ops).
  bool isSafeToReorderMemoryOps() const;

  // Drain Worklist, adding each pipelineable user of a popped instruction to
  // Set (and the worklist) once. Set must already contain the seeds.
  void forwardClosure(SmallVectorImpl<Instruction *> &Worklist,
                      SmallPtrSetImpl<Instruction *> &Set) const;

  // The split-point cone: the stage-1 split points (IsSplitPoint, reachable
  // from the prologue loads) plus all their forward-reachable descendants
  // within the prologue. Empty when no split point is found. This is the
  // stage-1 set.
  SmallPtrSet<Instruction *, 32>
  collectStage1Cone(function_ref<bool(const Instruction *)> IsSplitPoint) const;

  // The non-split stage-0 collection: pipeline candidates backward-reachable
  // from inner-loop uses, in program order, into stage0Insts(). Used by
  // collectStages when no split point is found. Does NOT include hardware-loop
  // setup calls (@llvm.set.loop.iterations) — those stay in the outer header.
  void collectStage0FromInnerLoop();

  // Split the prologue pipeline candidates into stage0Insts() and
  // stage1Insts(): stage 1 is the split-point cone, stage 0 is the rest (the
  // load/address chain). IsSplitPoint identifies where stage 1 begins; with no
  // split point the whole chain is stage 0 (via collectStage0FromInnerLoop).
  // Hardware-loop setup (@llvm.set.loop.iterations), loop-carried PHIs, and
  // terminators are excluded by isPipelineCandidate.
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
  // Deep-clone Src's blocks into "<name>.<Suffix>" IR, recording Src->clone in
  // CloneMap and remapping internal references to the clones. Edges leaving the
  // LS are left pointing at Src's externals for the caller to rewire. Built in
  // place: RemapTable is non-movable.
  CloneLoopStructure(const LoopStructure &Src, const Twine &Suffix);

  // Tag for the last-iteration skeleton constructor below.
  struct LastIterSkeletonTag {};

  // Create the empty last-iteration blocks spliced just before Steady's exit,
  // recording the Steady->lastiter block mappings in CloneMap. The caller fills
  // the instruction bodies afterwards. Built in place: RemapTable is
  // non-movable.
  CloneLoopStructure(const LoopStructure &Steady, LastIterSkeletonTag);

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

  BasicBlock *getPreheader() const override { return OuterPreheader; }

  void setOuterPreheader(BasicBlock *BB) { OuterPreheader = BB; }

  // Splice a freshly created block in as this LS's preheader: repoint the
  // header PHIs' incoming edge from the current preheader to NewPreheader, then
  // record it. The current preheader is read before it is overwritten, so the
  // two steps must stay in this order.
  void installPreheader(BasicBlock *NewPreheader);

  // Adjust the outer loop trip count from N to N-1 using the pre-computed
  // bound(). Returns the new limit Value.
  Value *adjustLoopBound();

  // Repair loop metadata (trip count changed): decrement itercount.range, drop
  // the consumed enable hint, and append the success marker.
  void updateLoopMetadata() const;

  // Returns the downcounting pattern components if the outer latch has the
  // canonical downcounting icmp pattern that can be replaced by
  // @llvm.loop.decrement.reg, or std::nullopt otherwise.
  std::optional<DowncountingInfo> getDowncountingInfo() const;
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
  // Build the LoopStructure for L and run every pipelining precondition as a
  // flat early-return guard; performs the transform iff all pass. Returns true
  // if L was pipelined.
  bool tryPipelineLoop(Loop *L, const AIE::LoopOptionOverrides &Overrides);
  // Seed Map with each outer-header PHI's incoming value on the FromEdge edge,
  // collapsing loop-carried PHIs to the concrete values seen when entering a
  // region via that edge.
  void seedHeaderPhiEdge(RemapTable &Map, const LoopStructure &LS,
                         BasicBlock *FromEdge) const;
  bool performTransformation(OrigLoopStructure &OrigLS,
                             const AIE::LoopOptionOverrides &Overrides);

  // Swap a freshly cloned, not-yet-transformed steady-state LS in for the
  // original: rewire the original preheader to the clone's header, repoint the
  // exit's PHIs from the original latch to the clone's latch, and set the
  // clone's outer preheader. After this the clone is the live loop (reachable
  // from the preheader, feeding the exit) and the original is unreachable,
  // ready for deletion once the transform completes.
  void swapInClonedLS(const OrigLoopStructure &OrigLS,
                      CloneLoopStructure &SteadyLS) const;

  // Remap SteadyLS.bound()'s cached instruction pointers (Cmp/Counter/OldIV and
  // the Limit, if it is an instruction in the LS) from the original LS to their
  // clones (via SteadyLS.cloneOf), so adjustLoopBound / getDowncountingInfo
  // operate on the clone.
  void remapBoundToClone(CloneLoopStructure &SteadyLS) const;

  // Clone OrigLS's stage-0 instructions (translated to steady clones via
  // SteadyLS.cloneMap()) into a peel block before the steady loop, then adopt
  // the peel as the steady preheader.
  void clonePrologueAsPeel(const OrigLoopStructure &OrigLS,
                           CloneLoopStructure &SteadyLS, RemapTable &PeelVMap);

  // Clone OrigLS's stage-0 instructions (translated to steady clones via
  // SteadyLS.cloneMap()) into the epilogue (outer latch), using the
  // NEXT-iteration pointer values so the loads prefetch for the next iteration.
  void clonePrologueIntoEpilogue(const OrigLoopStructure &OrigLS,
                                 const CloneLoopStructure &SteadyLS,
                                 RemapTable &EpiVMap);

  // For each steady stage-0 instruction (translated from OrigLS via
  // SteadyLS.cloneMap()), create a PHI selecting the peel value on the entry
  // edge and the epilogue value on the back edge, replace the instruction's
  // uses with it, and erase the instruction.
  void createPipelinedPHIs(const OrigLoopStructure &OrigLS,
                           const CloneLoopStructure &SteadyLS,
                           const RemapTable &PeelVMap,
                           const RemapTable &EpiVMap);

  // Create the last-iteration region (peeled epilogue for last iteration):
  //   lastiter.prologue: set.loop.iterations (cloned) + stage-1 clones
  //   inner loop clone: uses last epilogue load values + stage-1 results
  //   lastiter.epilogue: epilogue stores only (no loads, no prologue clones)
  // Redirects the outer latch's false branch to lastiter.prologue. OrigLS's
  // stage-1 list is translated to steady clones via SteadyLS.cloneMap(); the
  // epilogue snapshot is read off SteadyLS.
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

  // Translate each instruction of the original LS to its clone via VMap
  // (entries not in the map, e.g. loop-invariant operands, pass through
  // unchanged). Used to turn OrigLS's stage-0 / stage-1 lists into
  // steady-resident instructions at the point each transform step needs them.
  static SmallVector<Instruction *, 16>
  remapToClone(ArrayRef<Instruction *> Insts, const RemapTable &VMap);

  // Step helpers of peelLastIteration, in call order. The last-iteration LS is
  // built by its skeleton constructor and owns its steady->lastiter clone map;
  // these helpers fill its blocks through that map.

  // Clone the hardware-loop setup (set.loop.iterations) from the steady outer
  // header into the last-iteration prologue.
  void cloneHardwareLoopSetupInto(CloneLoopStructure &LastIterLS,
                                  const CloneLoopStructure &SteadyLS) const;

  // Fill the last-iteration inner-loop block clones with remapped instruction
  // bodies and wire the prologue into the inner header.
  void cloneInnerLoopIntoLastIter(const CloneLoopStructure &SteadyLS,
                                  CloneLoopStructure &LastIterLS) const;

  // Populate the last-iteration epilogue with the original epilogue stores and
  // pointer updates only — no prefetch loads. The instructions are read from
  // the pristine OrigLS latch (never touched by the prefetch-cloning steps),
  // translated Orig -> Steady via SteadyLS.cloneMap(), then cloned into the
  // last-iteration epilogue via LastIterLS.cloneMap().
  void populateLastIterEpilogue(const OrigLoopStructure &OrigLS,
                                const CloneLoopStructure &SteadyLS,
                                CloneLoopStructure &LastIterLS) const;

  // Splice the last-iteration into the CFG: last-iteration epilogue -> original
  // exit, redirect the steady latch exit to the last-iteration prologue, and
  // remap the exit block's live-out values to their last-iteration clones.
  // The exit's loop-carried live-outs (LCSSA PHIs or instructions
  // rematerialized into a dedicated exit block) still reference the
  // steady/original latch defs; since the final outer iteration now executes in
  // lastiter.epilogue, they are retargeted to its clones via the composed
  // lookup LastIterLS.cloneOf(SteadyLS.cloneOf(v)), otherwise the value read
  // after the loop omits the last iteration (or dangles once the original loop
  // is deleted).
  void wireLastIterIntoCFG(const OrigLoopStructure &OrigLS,
                           const CloneLoopStructure &SteadyLS,
                           const CloneLoopStructure &LastIterLS) const;

  // The epilogue instructions forming PHI's next-iteration pointer-update
  // chain, or nullopt if the chain cannot be safely lifted: it depends on an
  // inner-loop value, runs an unsafe (non-2D/3D-pointer) intrinsic, or is used
  // by an epilogue instruction outside the chain (a store, the exit icmp, ...).
  std::optional<SmallPtrSet<Instruction *, 16>>
  collectLiftableEpilogueChain(const OrigLoopStructure &OrigLS,
                               PHINode &PHI) const;

  // Lift pointer update instructions (add.2d,
  // add.3d, and their forward chain) from the epilogue to the end of the
  // prologue. This allows the main pipelining transformation to naturally
  // include them when cloning the prologue to peel and epilogue.
  // Returns true if any instructions were moved.
  bool liftEpiloguePointerUpdatesToPrologue(const OrigLoopStructure &OrigLS);

  // Convert the steady loop to a JNZD hardware loop (optional).
  // Inserts @llvm.start.loop.iterations in the preheader, a counter PHI in
  // the outer header, and @llvm.loop.decrement.reg in the outer latch.
  // Replaces the existing downcounting icmp+branch condition. The initial
  // counter value is computed from the counting PHI's peel incoming value
  // (SteadyLS.getPreheader() is the peel block at this point).
  // Info contains the pre-validated downcounting pattern components.
  void convertOuterLoopToHardwareLoop(const CloneLoopStructure &SteadyLS,
                                      const DowncountingInfo &Info);
};

} // namespace llvm::OuterLoopPipelining

#endif // LLVM_LIB_TARGET_AIE_AIEOUTERLOOPPIPELINER_H
