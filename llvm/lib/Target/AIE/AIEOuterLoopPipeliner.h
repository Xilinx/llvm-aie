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
// Outer loop pipelining for AIE: overlaps the peeled chain (prologue) of outer
// iteration i+1 with the inner loop + store chain (epilogue) of iteration i.
//
// The prologue (the outer header) splits into peeled and kept instructions by
// what the transform does with each:
//   Peeled = the load/address chain: loads plus the pointer/address arithmetic
//            feeding the inner loop. Peeled out of the steady header — copied
//            before the loop and prefetched in the epilogue for the next
//            iteration.
//   Kept   = the anchor cone: the anchor instructions (see isAnchorInstruction)
//            reachable from the peeled chain, plus their descendants in the
//            prologue. Kept in the steady header and re-cloned into
//            lastiter.prologue. Only split-prologue mode keeps anything; with
//            it off the whole prologue chain is peeled.
//
// Original CFG:
//
//   [preheader]
//       |
//   [outer.header]  <----------\  <- Prologue Content:
//       |                       |        peeled (load/address chain)
//       |                       |        kept (anchor cone)
//       |                       |        set.loop.iterations
//       |                       |
//   [outer.inner.*]             |  <- inner loop
//       |                       |
//   [outer.latch]  ------------/  <- Epilogue
//       |  (exit branch)
//   [exit]
//
// Produced CFG:
//
//   [preheader]
//       |
//   [steady.preheader]    <- peel: DATA LOADS ONLY (no set.loop.iter)
//       |
//   [steady.header]  <---------\  <- PHIs: v0/v1 from peel or epilogue
//       |                       |     set.loop.iterations stays here
//   [steady.inner.*]            |  <- steady-state inner loop
//       |                       |
//   [steady.latch]  -----------/  <- stores + loads for NEXT iteration
//       |  (exit branch)
//   [lastiter.prologue]        <- set.loop.iterations (cloned)
//       |
//   [steady.inner.*.lastiter]  <- inner loop clone, uses last epilogue values
//       |
//   [lastiter.epilogue]        <- stores only, no loads
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

// One LS instance, describing the outer loop and its single inner loop. Used
// both for the original loop and for the steady-state / last-iteration clones
// produced during the transform.
class LoopStructure {
  // Original Loop Structure Exclusive Attributes
  // OuterLoop/InnerLoop are the LoopInfo loops for the ORIGINAL LS only. They
  // are null on cloned structures (steady-state / last-iteration).
  Loop *OuterLoop = nullptr;
  Loop *InnerLoop = nullptr;
  bool IsOrigLS = false;

  // True once the constructor's analyzeLoopStructure has validated the loop as
  // a supported pipelining candidate. Always false on cloned structures.
  bool Valid = false;

  // Generic Attributes
  BasicBlock *InnerPreheader = nullptr;
  BasicBlock *InnerHeader = nullptr;
  BasicBlock *InnerLatch = nullptr;
  BasicBlock *InnerExit = nullptr;

  BasicBlock *OuterPreheader = nullptr;

  SmallVector<BasicBlock *, 4> InnerLoopBlocks;

  MDNode *OuterLoopID = nullptr;

  // The prologue region in program order; its entry block is the outer header.
  BlockRegion PrologueRegion;

  // The epilogue region in program order; its exit block is the outer latch.
  BlockRegion EpilogueRegion;

  LatchConditionInfo OuterLoopCondition;

  // The peeled / kept split of the prologue instructions, in program order.
  // Populated on the original LS only; clones never store these (see
  // IsOrigLS).
  SmallVector<Instruction *, 16> PeeledInsts;
  SmallVector<Instruction *, 16> KeptInsts;

  // Snapshot of the epilogue (outer latch) contents taken before
  // clonePrologueIntoEpilogue inserts prefetch clones, so the last-iteration
  // epilogue can be filtered back to the original stores only.
  SmallPtrSet<Instruction *, 32> EpilogueSnapshot;

  // The outer LoopInfo loop (original LS only; null on clones).
  Loop *getOuterLoop() const { return OuterLoop; }

  // True for the original LS; false for steady / last-iteration clones.
  bool isOrigLS() const { return IsOrigLS; }

  // Populate the inner-loop fields and prologue/epilogue regions from
  // OuterLoop, validating that the loop is a supported pipelining candidate.
  // Called once by the LoopStructure(Loop *) constructor; the result is cached
  // in Valid.
  bool analyzeLoopStructure();

  // Validate that the prologue is the linear single-block case (the outer
  // header is the inner preheader) and populate prologueRegion(). Returns false
  // for any other shape (a separate inner preheader block between the outer
  // header and the inner loop), which is not handled.
  bool discoverPrologueRegion();

  /// \return true if latch exit condition are valid and the loop bound can be
  /// adjusted (Step != 0).
  bool tryAdjustLoopBound();

  // Returns true if I is in the prologue region (outer header).
  bool isInPrologue(const Instruction *I) const {
    return PrologueRegion.contains(I);
  }

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
  explicit LoopStructure(Loop *L);

  // Constructor for Copied LoopStructures that do not provide valid LoopInfo
  LoopStructure(BasicBlock *OuterPreheader, BasicBlock *OuterHeader,
                BasicBlock *OuterLatch, BasicBlock *InnerPreheader,
                BasicBlock *InnerHeader, BasicBlock *InnerLatch,
                BasicBlock *InnerExit, ArrayRef<BasicBlock *> InnerBlocks,
                MDNode *OuterLoopID);

  // True if the loop was validated as a supported pipelining candidate by the
  // LoopStructure(Loop *) constructor. Always false on cloned structures.
  bool isValid() const {
    assert(isOrigLS() && "Only valid on the original LoopStructure");
    return Valid;
  }

  BasicBlock *getOuterHeader() const { return PrologueRegion.entry(); }
  BasicBlock *getOuterLatch() const { return EpilogueRegion.back(); }

  Loop *getInnerLoop() const { return InnerLoop; }
  BasicBlock *getInnerPreheader() const { return InnerPreheader; }
  BasicBlock *getInnerHeader() const { return InnerHeader; }
  BasicBlock *getInnerLatch() const { return InnerLatch; }
  BasicBlock *getInnerExit() const { return InnerExit; }
  ArrayRef<BasicBlock *> getInnerBlocks() const { return InnerLoopBlocks; }
  MDNode *getOuterLoopID() const { return OuterLoopID; }

  BlockRegion &prologueRegion() { return PrologueRegion; }
  const BlockRegion &prologueRegion() const { return PrologueRegion; }
  BlockRegion &epilogueRegion() { return EpilogueRegion; }
  const BlockRegion &epilogueRegion() const { return EpilogueRegion; }

  LatchConditionInfo &bound() { return OuterLoopCondition; }
  const LatchConditionInfo &bound() const { return OuterLoopCondition; }

  // The peeled / kept lists live only on the original LS.
  SmallVectorImpl<Instruction *> &peeledInsts();
  const SmallVectorImpl<Instruction *> &peeledInsts() const;
  SmallVectorImpl<Instruction *> &keptInsts();
  const SmallVectorImpl<Instruction *> &keptInsts() const;
  SmallPtrSetImpl<Instruction *> &epilogueSnapshot() {
    return EpilogueSnapshot;
  }
  const SmallPtrSetImpl<Instruction *> &epilogueSnapshot() const {
    return EpilogueSnapshot;
  }

  // Only clones store the preheader; the original derives it from LoopInfo.
  void setOuterPreheader(BasicBlock *BB);

  // Returns true if I is in the epilogue region (outer latch).
  bool isInEpilogue(const Instruction *I) const {
    return EpilogueRegion.contains(I);
  }

  // Returns the outer loop preheader.
  BasicBlock *getPreheader() const {
    return OuterLoop ? OuterLoop->getLoopPreheader() : OuterPreheader;
  }

  // Returns the outer latch terminator as a BranchInst.
  BranchInst *getLatchBranch() const {
    return cast<BranchInst>(getOuterLatch()->getTerminator());
  }

  // The latch successor that leaves the loop (the non-header edge).
  BasicBlock *getExitBlock() const;

  // Splice a freshly created peel block in as this LS's preheader: repoint
  // the header PHIs' incoming edge from the current preheader to Peel, then
  // record Peel. The current preheader is read before it is overwritten, so the
  // two steps must stay in this order.
  void adoptPeelAsPreheader(BasicBlock *Peel);

  // Returns true if this LS is profitable to rotate: the inner loop is a
  // hardware loop, the outer trip count meets MinTripCount, and the epilogue
  // has stores. SE supplies the outer-loop minimum trip count.
  bool isProfitableToRotate(ScalarEvolution &SE, unsigned MinTripCount) const;

  // Returns true if it is safe to reorder the prologue loads before the
  // epilogue stores (rejects volatile/atomic memory ops).
  bool isSafeToReorderMemoryOps() const;

  // Collect the peeled instructions from the outer header that feed the inner
  // loop into peeledInsts(). Does NOT include hardware-loop setup calls
  // (@llvm.set.loop.iterations) — those stay in the outer header.
  void collectPeeledInstructions();

  // Collect the peeled prologue instructions for split-prologue mode into
  // peeledInsts(): the load/address chain, i.e. prologue pipeline candidates
  // that are not anchors or anchor descendants. IsAnchor identifies anchor
  // instructions. Returns true if at least one anchor was found (the split is
  // meaningful); false if no anchors were found (caller should fall back to
  // collectPeeledInstructions).
  bool collectPeeledForSplit(function_ref<bool(const Instruction *)> IsAnchor);

  // Collect the kept prologue instructions for split-prologue mode into
  // keptInsts(), reading the peeled set from peeledInsts(): the anchors
  // reachable from the peeled chain plus all their forward-reachable
  // descendants within the prologue. IsAnchor identifies anchor instructions.
  // These stay in outer.header and are also cloned into lastiter.prologue so
  // the cloned inner loop has correct initial values.
  void
  collectKeptInstructions(function_ref<bool(const Instruction *)> IsAnchor);

  // Delete this (now unreachable) LS's blocks.
  void removeFromCFG() const;

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
  // Populate VMap with the incoming values of outer header PHIs from FromBlock.
  void populateVMapFromPHIs(ValueToValueMapTy &VMap, const LoopStructure &LS,
                            BasicBlock *FromBlock) const;
  bool performTransformation(LoopStructure &OrigLS,
                             const AIE::LoopOptionOverrides &Overrides);

  // Clone an entire LS (outer header + inner-loop blocks + outer latch)
  // into fresh IR blocks named "<orig><Suffix>". Records old->new in VMap and
  // remaps all cloned instructions so internal control flow and data references
  // point at the clones; edges leaving the LS (e.g. the exit successor) are
  // left pointing at the originals for the caller to rewire. Returns a
  // LoopStructure over the clone blocks (OuterLoop/InnerLoop null;
  // OuterPreheader null until the caller creates one). The clone is a faithful
  // copy of SrcLS at call time — the caller then transforms it.
  LoopStructure cloneLS(const LoopStructure &SrcLS, const Twine &Suffix,
                        ValueToValueMapTy &VMap) const;

  // Swap a freshly cloned, not-yet-transformed steady-state LS in for the
  // original: rewire the original preheader to the clone's header, repoint the
  // exit's PHIs from the original latch to the clone's latch, and set the
  // clone's outer preheader. After this the clone is the live loop (reachable
  // from the preheader, feeding the exit) and the original is unreachable,
  // ready for deletion once the transform completes.
  void swapInClonedLS(const LoopStructure &OrigLS, LoopStructure &SteadyLS,
                      const ValueToValueMapTy &SteadyVMap) const;

  // Remap SteadyLS.bound()'s cached instruction pointers (Cmp/Counter/OldIV and
  // the Limit, if it is an instruction in the LS) from the original LS to their
  // clones, so adjustLoopBound / getDowncountingInfo operate on the clone.
  void remapBoundToClone(LoopStructure &SteadyLS,
                         const ValueToValueMapTy &SteadyVMap) const;

  // Clone OrigLS's peeled instructions (translated to steady clones via
  // SteadyVMap) into a peel block before the steady loop, then adopt the peel
  // as the steady preheader.
  void clonePrologueAsPeel(const LoopStructure &OrigLS, LoopStructure &SteadyLS,
                           const ValueToValueMapTy &SteadyVMap,
                           ValueToValueMapTy &PeelVMap);

  // Clone OrigLS's peeled instructions (translated to steady clones via
  // SteadyVMap) into the epilogue (outer latch), using the NEXT-iteration
  // pointer values so the loads prefetch for the next iteration.
  void clonePrologueIntoEpilogue(const LoopStructure &OrigLS,
                                 const LoopStructure &SteadyLS,
                                 const ValueToValueMapTy &SteadyVMap,
                                 ValueToValueMapTy &EpiVMap);

  // For each steady peeled instruction (translated from OrigLS via SteadyVMap),
  // create a PHI selecting the peel value on the entry edge and the epilogue
  // value on the back edge, replace the instruction's uses with it, and erase
  // the instruction.
  void createPipelinedPHIs(const LoopStructure &OrigLS,
                           const LoopStructure &SteadyLS,
                           const ValueToValueMapTy &SteadyVMap,
                           const ValueToValueMapTy &PeelVMap,
                           const ValueToValueMapTy &EpiVMap);

  // Create the last-iteration region (peeled epilogue for last iteration):
  //   lastiter.prologue: set.loop.iterations (cloned) + kept clones
  //   inner loop clone: uses last epilogue load values + kept results
  //   lastiter.epilogue: epilogue stores only (no loads, no prologue clones)
  // Redirects the outer latch's false branch to lastiter.prologue. OrigLS's
  // kept list is translated to steady clones via SteadyVMap; the epilogue
  // snapshot is read off SteadyLS.
  void peelLastIteration(const LoopStructure &OrigLS,
                         const LoopStructure &SteadyLS,
                         const ValueToValueMapTy &SteadyVMap);

  // Clone I into Dest before InsertPt, record orig->clone in VMap, and return
  // the clone. A non-empty Suffix renames non-void clones to "<orig><Suffix>".
  static Instruction *cloneInstInto(Instruction &I, BasicBlock &Dest,
                                    BasicBlock::iterator InsertPt,
                                    ValueToValueMapTy &VMap,
                                    const Twine &Suffix);
  // Clone Insts (in order) into DstBB before InsertPt, then remap operands of
  // all the clones through VMap. Returns the clones, parallel to Insts.
  static SmallVector<Instruction *, 16>
  cloneAndRemapInsts(ArrayRef<Instruction *> Insts, BasicBlock &DstBB,
                     BasicBlock::iterator InsertPt, ValueToValueMapTy &VMap,
                     const Twine &Suffix);
  // Second-pass remap of freshly inserted clones, once all are in place.
  static void remapClones(ArrayRef<Instruction *> Clones,
                          ValueToValueMapTy &VMap);

  // Translate each instruction of the original LS to its clone via VMap
  // (entries not in the map, e.g. loop-invariant operands, pass through
  // unchanged). Used to turn OrigLS's peeled / kept lists into steady-resident
  // instructions at the point each transform step needs them.
  static SmallVector<Instruction *, 16>
  remapToClone(ArrayRef<Instruction *> Insts, const ValueToValueMapTy &VMap);

  // Step helpers of peelLastIteration, in call order.
  // Creates the empty last-iteration LS: lastiter.prologue (outer header),
  // one clone per inner-loop block, and lastiter.epilogue (outer latch), all
  // spliced before the steady loop's exit successor and recorded in
  // LastIterVMap. Bodies are filled later by the helpers below. Returns a
  // LoopStructure over the new blocks.
  LoopStructure createLastIterSkeleton(const LoopStructure &SteadyLS,
                                       ValueToValueMapTy &LastIterVMap) const;

  // Clone the hardware-loop setup (set.loop.iterations) from the steady outer
  // header into the last-iteration prologue, remapping through LastIterVMap.
  void cloneHardwareLoopSetupInto(const LoopStructure &LastIterLS,
                                  const LoopStructure &SteadyLS,
                                  ValueToValueMapTy &LastIterVMap) const;

  // Fills the last-iteration inner-loop block clones (created by
  // createLastIterSkeleton, resolved via LastIterVMap) with remapped
  // instruction bodies and wires the prologue into the inner header.
  void cloneInnerLoopIntoLastIter(const LoopStructure &SteadyLS,
                                  const LoopStructure &LastIterLS,
                                  ValueToValueMapTy &LastIterVMap) const;

  // Populate the last-iteration epilogue with the original epilogue stores only
  // — no prefetch loads, and none of the prologue clones inserted into the
  // epilogue earlier. The original stores are recovered from
  // SteadyLS.epilogueSnapshot().
  void populateLastIterEpilogue(const LoopStructure &LastIterLS,
                                const LoopStructure &SteadyLS,
                                ValueToValueMapTy &LastIterVMap) const;

  // Splice the last-iteration into the CFG: last-iteration epilogue -> original
  // exit, repoint the exit's latch-predecessor PHIs to it, redirect the steady
  // latch exit to the last-iteration prologue.
  void wireLastIterIntoCFG(const LoopStructure &SteadyLS,
                           const LoopStructure &LastIterLS) const;

  // Lift pointer update instructions (add.2d,
  // add.3d, and their forward chain) from the epilogue to the end of the
  // prologue. This allows the main pipelining transformation to naturally
  // include them when cloning the prologue to peel and epilogue.
  // Returns true if any instructions were moved.
  bool liftEpiloguePointerUpdatesToPrologue(const LoopStructure &OrigLS);

  // Convert the steady loop to a JNZD hardware loop (optional).
  // Inserts @llvm.start.loop.iterations in the preheader, a counter PHI in
  // the outer header, and @llvm.loop.decrement.reg in the outer latch.
  // Replaces the existing downcounting icmp+branch condition. The initial
  // counter value is computed from the counting PHI's peel incoming value
  // (SteadyLS.getPreheader() is the peel block at this point).
  // Info contains the pre-validated downcounting pattern components.
  void convertOuterLoopToHardwareLoop(const LoopStructure &SteadyLS,
                                      const DowncountingInfo &Info);
};

} // namespace llvm::OuterLoopPipelining

#endif // LLVM_LIB_TARGET_AIE_AIEOUTERLOOPPIPELINER_H
