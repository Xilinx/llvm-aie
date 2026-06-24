//===- AIEOuterLoopPipeliner.cpp - Outer Loop Pipeliner Pass --------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Outer loop pipelining for AIE: overlaps the data-load chain (prologue) of
// outer iteration i+1 with the inner loop + store chain (epilogue) of
// iteration i.
//
// Produced CFG:
//
//   [Preheader]
//       |
//   [outer.header.peel.pro]   <- warm-up: DATA LOADS ONLY (no set.loop.iter)
//       |
//   [outer.header]  <---------\  <- PHIs: v0/v1 from warm-up or epilogue
//       |                      |     set.loop.iterations stays here
//   [inner loop]               |
//       |                      |
//   [outer.latch]  ------------/  <- stores + loads for NEXT iteration
//       |  (false branch)
//   [cooldown.entry]               <- set.loop.iterations (cloned)
//       |
//   [inner loop clone]             <- uses v0.epi/v1.epi from last epilogue
//       |
//   [cooldown.exit]                <- stores only, no loads
//       |
//   [exit]
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "Utils/AIEIRUtils.h"
#include "Utils/AIELoopOptionOverrides.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IntrinsicsAIE2.h"
#include "llvm/IR/IntrinsicsAIE2P.h"
#include "llvm/IR/IntrinsicsAIE2PS.h"
#include "llvm/IR/Metadata.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/ValueMapper.h"

#include <functional>

using namespace llvm;

#define DEBUG_TYPE "aie-outer-loop-pipeliner"

static cl::opt<bool> EnableOuterLoopPipelining(
    "aie-enable-outer-loop-pipelining",
    cl::desc("Enable outer loop pipelining for long-latency loops"),
    cl::init(false), cl::Hidden);

static cl::opt<unsigned> OuterLoopPipeliningMinTripCount(
    "aie-outer-loop-pipelining-min-trip-count",
    cl::desc("Minimum trip count required for outer loop pipelining"),
    cl::init(2), cl::Hidden);

static cl::opt<bool>
    SplitPrologue("aie-outer-loop-pipelining-split-prologue",
                  cl::desc("Split prologue using different strategies to reach "
                           "more compact schedules"),
                  cl::init(true), cl::Hidden);

static cl::opt<bool> EnableOuterLoopHardwareLoop(
    "aie-outer-loop-hw-loop",
    cl::desc("Convert downcounting outer loops to JNZD hardware loops after "
             "outer loop pipelining"),
    cl::init(true), cl::Hidden);

// Type alias for split strategy predicates.
// A split strategy is a function that identifies "anchor" instructions
// which define the split point between Part 1 (pipelined) and Part 2
// (stays in outer.header + cooldown.entry).
using SplitStrategy = std::function<bool(const Instruction *)>;

// Returns true if I is a CallInst whose return type is a 2048-bit vector.
// These are the "anchor" instructions that define the split point between
// Part 1 (pipelined) and Part 2 (stays in outer.header + cooldown.entry).
static bool produces2048BitVector(const Instruction *I) {
  if (!isa<CallInst>(I))
    return false;
  auto *VT = dyn_cast<FixedVectorType>(I->getType());
  return VT && VT->getPrimitiveSizeInBits() == 2048;
}

// Get all available split strategies in priority order.
// Each strategy identifies anchor instructions; if any strategy returns
// true for at least one instruction, that instruction is an anchor.
static SmallVector<SplitStrategy, 4> getSplitStrategies() {
  return {
      produces2048BitVector, // Current default: 2048-bit producing CallInsts
                             // Additional strategies can be added here, e.g.:
                             // produces1024BitVector,
                             // isAccumulatorIntrinsic,
  };
}

// Returns true if any split strategy identifies this instruction as an anchor.
static bool isAnchorInstruction(const Instruction *I) {
  for (const auto &Strategy : getSplitStrategies()) {
    if (Strategy(I))
      return true;
  }
  return false;
}

// Returns true if the intrinsic ID is a safe pointer increment intrinsic
// (2D/3D addressing) that can be lifted from epilogue to prologue.
// These intrinsics have no side effects (IntrNoMem) and are pure pointer
// computations. Any other intrinsic may have unknown side effects and should
// not be lifted.
static bool isSafePointerIncrementIntrinsic(Intrinsic::ID IID) {
  switch (IID) {
  // AIE2 2D/3D pointer increment intrinsics
  case Intrinsic::aie2_add_2d:
  case Intrinsic::aie2_add_3d:
  // AIE2P 2D/3D pointer increment intrinsics
  case Intrinsic::aie2p_add_2d:
  case Intrinsic::aie2p_add_3d:
  // AIE2PS 2D/3D pointer increment intrinsics
  case Intrinsic::aie2ps_add_2d:
  case Intrinsic::aie2ps_add_3d:
    return true;
  default:
    return false;
  }
}

namespace {

// An ordered, contiguous run of basic blocks in program order (entry first,
// exit last). Wraps the single-block (linear) and multi-block (guarded diamond)
// prologue/epilogue uniformly, so callers can test membership and iterate
// instructions without special-casing the block count.
class BlockRegion {
  SmallVector<BasicBlock *, 4> Blocks;

public:
  void assign(ArrayRef<BasicBlock *> BBs) {
    Blocks.assign(BBs.begin(), BBs.end());
  }

  size_t size() const { return Blocks.size(); }

  bool contains(const BasicBlock *BB) const {
    return llvm::is_contained(Blocks, BB);
  }
  bool contains(const Instruction *I) const { return contains(I->getParent()); }

  ArrayRef<BasicBlock *> blocks() const { return Blocks; }

  // Visit every instruction of the region in program order, block by block.
  // Works identically whether the region is one block or several.
  void forEachInstruction(function_ref<void(Instruction *)> Visit) const {
    for (BasicBlock *BB : Blocks)
      for (Instruction &I : *BB)
        Visit(&I);
  }
};

// Cached result of latch exit condition analysis.
// Populated once by canAdjustLoopBound() and reused by adjustLoopBound() and
// getDowncountingInfo() to avoid repeating the same icmp pattern-matching.
// All pointer fields are guaranteed non-null when the struct is returned.
// Latch exit condition analysis result, computed once by analyzeLoopStructure()
// and stored in LoopStructure::Bound. All pointer fields guaranteed non-null.
struct LatchBoundInfo {
  ICmpInst *Cmp = nullptr;           // latch exit icmp condition
  Value *Limit = nullptr;            // loop-invariant limit operand
  unsigned LimitIdx = 0;             // index of Limit in Cmp's operand list
  int64_t Step = 0;                  // non-zero induction step (from add)
  BinaryOperator *Counter = nullptr; // add instr feeding the icmp (phi+step)
  PHINode *OldIV = nullptr;          // counting PHI in outer header
};

// Loop structure for outer loop pipelining.
// The prologue is a single-entry/single-exit (SESE) region. Its entry is the
// outer header; its exit is the last region block, whose successor is the inner
// preheader (the region exit edge). For the common linear loop the region is
// just {OuterHeader} with OuterHeader == InnerPreheader; for a guarded prologue
// it is a diamond (e.g. outer.header → if.then → if.end). The epilogue stays a
// single block (the outer latch).
struct LoopStructure {
  // OuterLoop/InnerLoop are the LoopInfo loops for the ORIGINAL nest only. They
  // are null on cloned structures (steady-state / last-iteration), which are
  // not registered with LoopInfo. Transform-phase code must not deref these; it
  // uses the explicit fields below (InnerBlocks, OuterPreheader, LoopID)
  // instead.
  Loop *OuterLoop;
  Loop *InnerLoop;
  BasicBlock *OuterHeader;
  BasicBlock *OuterLatch;
  BasicBlock *InnerPreheader;
  BasicBlock *InnerHeader;
  BasicBlock *InnerLatch;
  BasicBlock *InnerExit;

  // The outer header's entry predecessor (the preheader edge source). For the
  // original nest this is the LoopInfo preheader; for the steady-state clone it
  // is steady.preheader. Lets transform-phase code avoid OuterLoop.
  BasicBlock *OuterPreheader;

  // The inner loop's blocks in LoopInfo order. Filled from InnerLoop->blocks()
  // for the original; mapped through the clone VMap for cloned structures, so
  // cloning code never needs the (absent) LoopInfo InnerLoop.
  SmallVector<BasicBlock *, 4> InnerBlocks;

  // The outer loop's loop-id metadata (llvm.loop on the outer latch), captured
  // so updateLoopMetadata can re-apply the adjusted id to a clone's outer latch
  // without a LoopInfo Loop. (The inner loop's own llvm.loop metadata travels
  // with the cloned inner-latch terminator automatically.)
  MDNode *OuterLoopID = nullptr;

  // The prologue region in program order, populated by analyzeLoopStructure
  // once validated. A single-block region is the degenerate linear case.
  BlockRegion PrologueRegion;

  // The epilogue region in program order, ending at the latch (back-branch).
  // Defaults to {OuterLatch}; clonePrologueIntoEpilogue widens it when it
  // splits the latch to host the prefetch diamond.
  BlockRegion EpilogueRegion;

  // Constructs a LoopStructure from an outer loop.
  // Preconditions (caller must verify before constructing):
  //   - L->getSubLoops().size() == 1
  //   - L->getLoopLatch() != nullptr
  explicit LoopStructure(Loop *L)
      : OuterLoop(L), InnerLoop(L->getSubLoops()[0]),
        OuterHeader(L->getHeader()), OuterLatch(L->getLoopLatch()),
        InnerPreheader(InnerLoop->getLoopPreheader()),
        InnerHeader(InnerLoop->getHeader()),
        InnerLatch(InnerLoop->getLoopLatch()),
        InnerExit(InnerLoop->getExitBlock()),
        OuterPreheader(L->getLoopPreheader()),
        InnerBlocks(InnerLoop->block_begin(), InnerLoop->block_end()),
        OuterLoopID(L->getLoopID()) {
    assert(L->getSubLoops().size() == 1 && "Requires exactly one subloop");
    assert(OuterLatch && "Requires single outer latch");
  }

  // Constructs a LoopStructure over explicit blocks, for clones that are not
  // registered with LoopInfo. OuterLoop/InnerLoop are null; callers must supply
  // every block, the inner-block list, and the loop-id metadata.
  LoopStructure(BasicBlock *OuterPreheader, BasicBlock *OuterHeader,
                BasicBlock *OuterLatch, BasicBlock *InnerPreheader,
                BasicBlock *InnerHeader, BasicBlock *InnerLatch,
                BasicBlock *InnerExit, ArrayRef<BasicBlock *> InnerBlocks,
                MDNode *OuterLoopID)
      : OuterLoop(nullptr), InnerLoop(nullptr), OuterHeader(OuterHeader),
        OuterLatch(OuterLatch), InnerPreheader(InnerPreheader),
        InnerHeader(InnerHeader), InnerLatch(InnerLatch), InnerExit(InnerExit),
        OuterPreheader(OuterPreheader),
        InnerBlocks(InnerBlocks.begin(), InnerBlocks.end()),
        OuterLoopID(OuterLoopID) {}

  bool isInPrologue(const Instruction *I) const {
    return PrologueRegion.contains(I);
  }

  // A region-internal PHI has every incoming block inside the prologue region;
  // it is cloned with the region. A loop-carried PHI in the entry instead has
  // incoming edges from the preheader and latch (outside the region) and is
  // resolved to a concrete value when cloning, never cloned as a PHI.
  bool isRegionInternalPhi(const PHINode *PHI) const {
    for (const BasicBlock *IB : PHI->blocks())
      if (!PrologueRegion.contains(IB))
        return false;
    return true;
  }

  // A value is pipelineable when it lives in the prologue region and is a plain
  // instruction or a region-internal PHI. Loop-carried PHIs are excluded: they
  // resolve to concrete values via the clone VMap rather than being cloned.
  bool isPipelineableValue(const Instruction *I) const {
    if (!isInPrologue(I))
      return false;
    if (const auto *PHI = dyn_cast<PHINode>(I))
      return isRegionInternalPhi(PHI);
    return true;
  }

  // A pipeline candidate is a pipelineable value that is also clonable into the
  // prefetch/cool-down sites: block terminators and hardware-loop setup are
  // excluded. Combine with PrologueRegion.forEachInstruction to walk candidates
  // in program order.
  bool isPipelineCandidate(const Instruction *I) const {
    if (I->isTerminator() || AIEIRUtils::isHardwareLoopSetup(I))
      return false;
    return isPipelineableValue(I);
  }

  // Returns true if I is in the epilogue region (outer latch).
  bool isInEpilogue(const Instruction *I) const {
    return EpilogueRegion.contains(I);
  }

  // Returns the outer loop preheader. For the original nest this queries
  // LoopInfo live (so after warm-up insertion it returns the warm-up block, the
  // current single-predecessor preheader). Clones have no LoopInfo loop and use
  // OuterPreheader directly.
  BasicBlock *getPreheader() const {
    return OuterLoop ? OuterLoop->getLoopPreheader() : OuterPreheader;
  }

  // Returns the outer latch terminator as a BranchInst.
  BranchInst *getLatchBranch() const {
    return cast<BranchInst>(OuterLatch->getTerminator());
  }

  // Latch exit condition analysis; populated by analyzeLoopStructure().
  // Always valid: LoopStructure is only returned after Bound is set.
  LatchBoundInfo Bound;
};

// Holds the pre-validated components of a downcounting outer loop exit
// condition, computed once by getDowncountingInfo and consumed by
// convertOuterLoopToHardwareLoop.
// All fields are guaranteed non-null when the struct is returned.
struct DowncountingInfo {
  ICmpInst *Cmp;           // latch exit condition (icmp eq/ne)
  BinaryOperator *Counter; // add instruction (phi + (-step))
  PHINode *OldIV;          // counting PHI in outer header feeding Counter
};

class AIEOuterLoopPipeliner : public FunctionPass {
public:
  static char ID;
  AIEOuterLoopPipeliner() : FunctionPass(ID) {}
  bool runOnFunction(Function &F) override;
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<ScalarEvolutionWrapperPass>();
    FunctionPass::getAnalysisUsage(AU);
  }
  StringRef getPassName() const override { return "AIE Outer Loop Pipeliner"; }

private:
  LoopInfo *LI = nullptr;
  DominatorTree *DT = nullptr;
  ScalarEvolution *SE = nullptr;

  bool runOnLoop(Loop *L);
  std::optional<LoopStructure> analyzeLoopStructure(Loop *L);
  // Discover and validate the SESE prologue region (entry = outer header, exit
  // = inner preheader) and populate LS.PrologueRegion in program order. Returns
  // false if the region is not a clonable SESE region (early loop exit inside
  // the prologue, a jump into the middle of the region, a prologue path that
  // bypasses the inner loop, or any block outside the region/inner-loop/latch).
  bool discoverPrologueRegion(LoopStructure &LS) const;
  bool isInnerLoopHardwareLoop(const LoopStructure &LS) const;
  bool isProfitableToRotate(const LoopStructure &LS,
                            const AIE::LoopOptionOverrides &Overrides);
  bool isSafeToReorderMemoryOps(const LoopStructure &LS);
  void collectPrologueLoads(const LoopStructure &LS,
                            SmallVectorImpl<LoadInst *> &Loads) const;
  void collectEpilogueStores(const LoopStructure &LS,
                             SmallVectorImpl<StoreInst *> &Stores) const;
  // Populate VMap with the incoming values of outer header PHIs from FromBlock.
  void populateVMapFromPHIs(ValueToValueMapTy &VMap, const LoopStructure &LS,
                            BasicBlock *FromBlock) const;
  // Collect the data-load chain instructions from the outer header that feed
  // the inner loop. Does NOT include hardware-loop setup calls
  // (@llvm.set.loop.iterations) — those stay in the outer header.
  void collectPrologueInstructions(const LoopStructure &LS,
                                   SmallVectorImpl<Instruction *> &Out) const;
  bool performTransformation(LoopStructure &LS,
                             const AIE::LoopOptionOverrides &Overrides);

  // Clone an entire loop nest (outer header + inner-loop blocks + outer latch)
  // into fresh IR blocks named "<orig><Suffix>". Records old->new in VMap and
  // remaps all cloned instructions so internal control flow and data references
  // point at the clones; edges leaving the nest (e.g. the exit successor) are
  // left pointing at the originals for the caller to rewire. Returns a
  // LoopStructure over the clone blocks (OuterLoop/InnerLoop null;
  // OuterPreheader null until the caller creates one). The clone is a faithful
  // copy of SrcLS at call time — the caller then transforms it.
  LoopStructure cloneLoopNest(const LoopStructure &SrcLS, const Twine &Suffix,
                              ValueToValueMapTy &VMap) const;

  // Swap a freshly cloned, not-yet-transformed steady-state nest in for the
  // original: rewire the original preheader to the clone's header, repoint the
  // exit's PHIs from the original latch to the clone's latch, and set
  // SteadyLS.OuterPreheader. After this the clone is the live loop (reachable
  // from the preheader, feeding the exit) and the original is unreachable,
  // ready for deletion once the transform completes.
  void swapInClonedNest(const LoopStructure &OrigLS, LoopStructure &SteadyLS,
                        const ValueToValueMapTy &SteadyVMap) const;

  // Remap SteadyLS.Bound's cached instruction pointers (Cmp/Counter/OldIV and
  // the Limit, if it is an in-nest instruction) from the original nest to their
  // clones, so adjustLoopBound / getDowncountingInfo operate on the clone.
  void remapBoundToClone(LoopStructure &SteadyLS,
                         const ValueToValueMapTy &SteadyVMap) const;

  // Delete the original (now unreachable) loop nest blocks.
  void deleteOrigNest(const LoopStructure &OrigLS) const;

  // Clone the prologue region's Part-1 instructions (PInsts) as a parallel
  // block subgraph that preserves the region's internal control flow. VMap must
  // be pre-seeded with outer-header PHI -> incoming-value entries (e.g.,
  // preheader values for warm-up, latch values for the epilogue). New blocks
  // are inserted before InsertBefore in region order and appended to NewBlocks.
  // The exit clone's terminator is NOT created (the caller wires it). Returns
  // {entryClone, exitClone}. For a single-block region this is one block.
  std::pair<BasicBlock *, BasicBlock *>
  cloneRegionSubgraph(const LoopStructure &LS,
                      const SmallVectorImpl<Instruction *> &PInsts,
                      ValueToValueMapTy &VMap, const Twine &BlockSuffix,
                      const Twine &InstSuffix, BasicBlock *InsertBefore,
                      SmallVectorImpl<BasicBlock *> &NewBlocks);

  // Clone data-load chain into a warm-up region before the outer loop. Returns
  // the warm-up exit block (the new preheader-side predecessor of the header).
  BasicBlock *
  clonePrologueAsWarmUp(const LoopStructure &LS,
                        const SmallVectorImpl<Instruction *> &PInsts,
                        ValueToValueMapTy &WarmUpVMap);

  // Clone data-load chain into the epilogue (outer latch), using the
  // NEXT-iteration pointer values so the loads prefetch for the next iteration.
  void clonePrologueIntoEpilogue(const LoopStructure &LS,
                                 const SmallVectorImpl<Instruction *> &PInsts,
                                 ValueToValueMapTy &EpiVMap);

  // Update outer header PHI predecessors (preheader -> warm-up).
  void updateOuterHeaderPHIs(const LoopStructure &LS, BasicBlock *WarmUp,
                             BasicBlock *Preheader);

  // For each data-load instruction I in the outer header, create a
  // PHI node phi_I = [WarmUpVMap[I], warm-up], [EpiVMap[I], outer.latch].
  // Replace uses of I in the inner loop with phi_I, then erase I.
  void createPipelinedPHIs(const LoopStructure &LS, BasicBlock *WarmUp,
                           const SmallVectorImpl<Instruction *> &PInsts,
                           const ValueToValueMapTy &WarmUpVMap,
                           const ValueToValueMapTy &EpiVMap);

  // Create the cool-down region (peeled epilogue for last iteration):
  //   cooldown.entry: set.loop.iterations (cloned) + PartTwoInsts clones
  //   inner loop clone: uses last epilogue load values + Part2 results
  //   cooldown.exit: epilogue stores only (no loads, no prologue clones)
  // Redirects the outer latch's false branch to cooldown.entry.
  // OrigEpiInsts: the set of instructions that were in the epilogue block
  // BEFORE clonePrologueIntoEpilogue inserted the prologue load clones.
  // PartTwoInsts: 2048-bit producing intrinsics and their descendants that must
  // be cloned into cooldown.entry so the cloned inner loop has correct initial
  // accumulator values. Empty when not in split-prologue mode.
  void
  peelLastIterationEpilogue(const LoopStructure &LS,
                            const SmallPtrSetImpl<Instruction *> &OrigEpiInsts,
                            const SmallVectorImpl<Instruction *> &PartTwoInsts);

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

  // Step helpers of peelLastIterationEpilogue, in call order.
  // The original block the latch branched to on loop exit.
  BasicBlock *getExitBlock(const LoopStructure &LS) const;

  // Clone the hardware-loop setup (set.loop.iterations) from the outer header
  // into Dest, remapping operands through CoolVMap.
  void cloneHardwareLoopSetupInto(BasicBlock *Dest, const LoopStructure &LS,
                                  ValueToValueMapTy &CoolVMap) const;

  // Creates the empty cool-down inner-loop blocks (clones of the inner-loop
  // blocks, recorded in CoolVMap; bodies filled later by
  // cloneInnerLoopIntoCooldown). Returns the cool-down exit block.
  BasicBlock *createCooldownSkeleton(const LoopStructure &LS,
                                     BasicBlock *CoolEntry,
                                     BasicBlock *OrigExit,
                                     ValueToValueMapTy &CoolVMap) const;

  // Fills the cool-down inner-loop block clones (from createCooldownSkeleton,
  // resolved via CoolVMap) with remapped instruction bodies.
  void cloneInnerLoopIntoCooldown(const LoopStructure &LS,
                                  BasicBlock *CoolEntry,
                                  ValueToValueMapTy &CoolVMap) const;

  // Populate cooldown.exit with the original epilogue stores only — no prefetch
  // loads, and none of the prologue clones inserted into the epilogue earlier.
  void populateCooldownExit(BasicBlock *CoolExit, const LoopStructure &LS,
                            const SmallPtrSetImpl<Instruction *> &OrigEpiInsts,
                            ValueToValueMapTy &CoolVMap) const;

  // Splice the cool-down into the CFG: cooldown.exit -> original exit, repoint
  // the exit's latch-predecessor PHIs to cooldown.exit, redirect latch exit.
  void wireCooldownIntoCFG(const LoopStructure &LS, BasicBlock *CoolEntry,
                           BasicBlock *CoolExit, BasicBlock *OrigExit) const;

  // Validates the latch exit condition, computes the induction step, and
  // caches all discovered components in LS.LatchBound for later use by
  // adjustLoopBound() and getDowncountingInfo(). Returns true if the loop
  // bound can be adjusted (Step != 0). Must be called before
  // performTransformation.
  // Validates the latch exit condition and returns all components needed to
  // adjust the loop bound. Called from analyzeLoopStructure(); the result is
  // stored in LS.Bound so downstream functions can use it without re-scanning.
  std::optional<LatchBoundInfo>
  canAdjustLoopBound(const LoopStructure &LS) const;

  // Adjust the outer loop trip count from N to N-1 using the pre-computed
  // LS.LatchBound. Returns the new limit Value.
  Value *adjustLoopBound(const LoopStructure &LS);

  // Repair loop metadata (trip count changed).
  void updateLoopMetadata(const LoopStructure &LS);

  // Lift pointer update instructions (add.2d,
  // add.3d, and their forward chain) from the epilogue to the end of the
  // prologue. This allows the main pipelining transformation to naturally
  // include them when cloning the prologue to warmup and epilogue.
  // Returns true if any instructions were moved.
  bool liftEpiloguePointerUpdatesToPrologue(const LoopStructure &LS);

  // Convert the outer loop to a JNZD hardware loop (optional).
  // Inserts @llvm.start.loop.iterations in the preheader, a counter PHI in
  // the outer header, and @llvm.loop.decrement.reg in the outer latch.
  // Replaces the existing downcounting icmp+branch condition.
  // WarmUp is the warm-up block that now precedes the outer header (the PHI
  // incoming block for the initial counter value).
  // AdjustedTripCount is the N-1 value produced by adjustLoopBound.
  // Info contains the pre-validated downcounting pattern components.
  void convertOuterLoopToHardwareLoop(const LoopStructure &LS,
                                      BasicBlock *WarmUp,
                                      Value *AdjustedTripCount,
                                      const DowncountingInfo &Info);

  // Returns the downcounting pattern components if the outer latch has the
  // canonical downcounting icmp pattern that can be replaced by
  // @llvm.loop.decrement.reg, or std::nullopt otherwise.
  std::optional<DowncountingInfo>
  getDowncountingInfo(const LoopStructure &LS) const;

  // Collect the "Part 1" prologue instructions for split-prologue mode.
  // Part 1 = all instructions reachable from loads (forward tracking) that
  // are matching any strategy, plus address computation chains of loads.
  // Returns true if at least one anchor was found (split is
  // meaningful by one Strategy); false if no anchors were found (caller should
  // fall back to collectPrologueInstructions).
  bool collectPrologueInstructionsForSplit(
      const LoopStructure &LS, SmallVectorImpl<Instruction *> &Out) const;

  // Collect the "Part 2" prologue instructions for split-prologue mode.
  // Part 2 = Strategy matched instructions reachable from Part 1 instructions,
  // plus all their forward-reachable descendants within the prologue.
  // These instructions stay in outer.header and are also cloned into
  // cooldown.entry so the cloned inner loop has correct initial values.
  void
  collectPartTwoInstructions(const LoopStructure &LS,
                             const SmallPtrSetImpl<Instruction *> &PartOneSet,
                             SmallVectorImpl<Instruction *> &Out) const;
};

} // end anonymous namespace

char AIEOuterLoopPipeliner::ID = 0;
char &llvm::AIEOuterLoopPipelinerID = AIEOuterLoopPipeliner::ID;

INITIALIZE_PASS_BEGIN(AIEOuterLoopPipeliner, DEBUG_TYPE,
                      "AIE Outer Loop Pipeliner", false, false)
INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
INITIALIZE_PASS_DEPENDENCY(DominatorTreeWrapperPass)
INITIALIZE_PASS_DEPENDENCY(ScalarEvolutionWrapperPass)
INITIALIZE_PASS_END(AIEOuterLoopPipeliner, DEBUG_TYPE,
                    "AIE Outer Loop Pipeliner", false, false)

llvm::FunctionPass *llvm::createAIEOuterLoopPipelinerPass() {
  return new AIEOuterLoopPipeliner();
}

bool AIEOuterLoopPipeliner::runOnFunction(Function &F) {
  if (skipFunction(F))
    return false;
  // Don't gate on EnableOuterLoopPipelining here: individual loops may opt-in
  // via !llvm.loop.hint.aie-enable-outer-loop-pipelining metadata even when
  // the global flag is off.
  LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
  DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
  SE = &getAnalysis<ScalarEvolutionWrapperPass>().getSE();
  LLVM_DEBUG(dbgs() << "AIEOuterLoopPipeliner: " << F.getName() << "\n");
  bool Changed = false;
  SmallVector<Loop *, 4> TopLevelLoops(LI->begin(), LI->end());
  for (Loop *L : TopLevelLoops)
    Changed |= runOnLoop(L);
  return Changed;
}

bool AIEOuterLoopPipeliner::runOnLoop(Loop *L) {
  LLVM_DEBUG(dbgs() << "  Considering: ";
             L->getHeader()->printAsOperand(dbgs(), false); dbgs() << "\n");
  // Build per-loop option overrides from !llvm.loop.hint.* metadata.
  AIE::LoopOptionOverrides Overrides(L->getLoopID());

  // Try to transform this loop if enabled (globally or via metadata).
  if (Overrides.get(EnableOuterLoopPipelining)) {
    std::optional<LoopStructure> MaybeLS = analyzeLoopStructure(L);
    if (MaybeLS) {
      LoopStructure &LS = *MaybeLS;
      if (isProfitableToRotate(LS, Overrides) && isSafeToReorderMemoryOps(LS)) {
        LLVM_DEBUG(dbgs() << "  Applying outer loop pipelining\n");
        return performTransformation(LS, Overrides);
      }
    }
  }

  // If this loop was not transformed, recursively try its subloops.
  // This handles nested structures like: outermost { middle { innermost } }
  // where only the middle loop is annotated for pipelining.
  bool Changed = false;
  for (Loop *SubLoop : L->getSubLoops())
    Changed |= runOnLoop(SubLoop);
  return Changed;
}

std::optional<LoopStructure>
AIEOuterLoopPipeliner::analyzeLoopStructure(Loop *L) {
  // Early validation before constructing LoopStructure.
  if (L->getSubLoops().size() != 1) {
    LLVM_DEBUG(dbgs() << "    Not exactly one subloop\n");
    return std::nullopt;
  }
  if (!L->getLoopLatch()) {
    LLVM_DEBUG(dbgs() << "    No single outer latch\n");
    return std::nullopt;
  }

  // Construct the LoopStructure (constructor asserts preconditions).
  LoopStructure LS(L);

  // Validate inner loop components.
  if (!LS.InnerPreheader || !LS.InnerExit || !LS.InnerLatch) {
    LLVM_DEBUG(dbgs() << "    Inner loop missing preheader/exit/latch\n");
    return std::nullopt;
  }

  // Epilogue must be a single block: inner exit == outer latch.
  if (LS.InnerExit != LS.OuterLatch) {
    LLVM_DEBUG(dbgs() << "    Inner exit != outer latch\n");
    return std::nullopt;
  }

  // Discover and validate the prologue region (outer.header .. inner
  // preheader). This generalizes the linear case (region == {outer.header}) to
  // a guarded SESE diamond, and rejects any other shape.
  if (!discoverPrologueRegion(LS))
    return std::nullopt;

  // Every outer-loop block must belong to the prologue region, the inner loop,
  // or the single-block epilogue (latch); anything else is an unknown shape.
  for (BasicBlock *BB : L->blocks()) {
    if (LS.InnerLoop->contains(BB) || BB == LS.OuterLatch ||
        LS.PrologueRegion.contains(BB))
      continue;
    LLVM_DEBUG(dbgs() << "    Unexpected outer-loop block: " << BB->getName()
                      << "\n");
    return std::nullopt;
  }

  // Epilogue starts as the single latch block; clonePrologueIntoEpilogue widens
  // it if it splits the latch for a guarded prologue.
  LS.EpilogueRegion.assign({LS.OuterLatch});

  LLVM_DEBUG(dbgs() << "    Prologue region: " << LS.PrologueRegion.size()
                    << " block(s); epilogue in outer.latch\n");

  // Verify and cache the latch exit condition. LoopStructure is only returned
  // when the bound can be adjusted; LS.Bound is then always valid.
  auto MaybeBound = canAdjustLoopBound(LS);
  if (!MaybeBound) {
    LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound\n");
    return std::nullopt;
  }
  LS.Bound = *MaybeBound;
  return LS;
}

bool AIEOuterLoopPipeliner::discoverPrologueRegion(LoopStructure &LS) const {
  if (LS.InnerPreheader != LS.OuterHeader) {
    LLVM_DEBUG(dbgs() << "    Inner preheader != outer header\n");
    return false;
  }
  LS.PrologueRegion.assign({LS.OuterHeader});
  return true;
}

bool AIEOuterLoopPipeliner::isInnerLoopHardwareLoop(
    const LoopStructure &LS) const {
  auto *BI = dyn_cast<BranchInst>(LS.InnerLatch->getTerminator());
  if (!BI || !BI->isConditional())
    return false;
  auto *Call = dyn_cast<CallInst>(BI->getCondition());
  if (!Call)
    return false;
  auto *Fn = Call->getCalledFunction();
  if (!Fn)
    return false;
  Intrinsic::ID IID = Fn->getIntrinsicID();
  return IID == Intrinsic::loop_decrement;
}

void AIEOuterLoopPipeliner::collectPrologueLoads(
    const LoopStructure &LS, SmallVectorImpl<LoadInst *> &Loads) const {
  // Prologue is always in OuterHeader (linear structure).
  for (Instruction &I : *LS.OuterHeader)
    if (auto *L = dyn_cast<LoadInst>(&I))
      Loads.push_back(L);
}

void AIEOuterLoopPipeliner::collectEpilogueStores(
    const LoopStructure &LS, SmallVectorImpl<StoreInst *> &Stores) const {
  // Epilogue is always in OuterLatch (linear structure).
  for (Instruction &I : *LS.OuterLatch)
    if (auto *S = dyn_cast<StoreInst>(&I))
      Stores.push_back(S);
}

// Populate VMap with the incoming values of outer header PHIs from FromBlock.
void AIEOuterLoopPipeliner::populateVMapFromPHIs(ValueToValueMapTy &VMap,
                                                 const LoopStructure &LS,
                                                 BasicBlock *FromBlock) const {
  for (PHINode &PHI : LS.OuterHeader->phis()) {
    int Idx = PHI.getBasicBlockIndex(FromBlock);
    if (Idx >= 0)
      VMap[&PHI] = PHI.getIncomingValue(Idx);
  }
}

bool AIEOuterLoopPipeliner::isProfitableToRotate(
    const LoopStructure &LS, const AIE::LoopOptionOverrides &Overrides) {
  if (!isInnerLoopHardwareLoop(LS)) {
    LLVM_DEBUG(dbgs() << "    Inner loop is not a hardware loop\n");
    return false;
  }
  std::optional<int64_t> MinTC = llvm::getMinTripCount(LS.OuterLoop, SE);
  if (!MinTC ||
      *MinTC < (int64_t)Overrides.get(OuterLoopPipeliningMinTripCount)) {
    LLVM_DEBUG(dbgs() << "    Trip count too low\n");
    return false;
  }

  SmallVector<LoadInst *, 8> PrologueLoads;
  collectPrologueLoads(LS, PrologueLoads);

  SmallVector<StoreInst *, 8> EpilogueStores;
  collectEpilogueStores(LS, EpilogueStores);
  // TODO: Do we actually need this? Do we have cases without
  // stores?
  if (EpilogueStores.empty()) {
    LLVM_DEBUG(dbgs() << "    No stores in epilogue\n");
    return false;
  }
  // Loop bound adjustability was verified in analyzeLoopStructure().
  return true;
}

bool AIEOuterLoopPipeliner::isSafeToReorderMemoryOps(const LoopStructure &LS) {
  // TODO: Add alias/dependence analysis to verify that moving prologue loads
  // before epilogue stores is safe. For now, only reject volatile/atomic
  // operations which can never be reordered.
  SmallVector<LoadInst *, 8> PrologueLoads;
  SmallVector<StoreInst *, 8> EpilogueStores;
  collectPrologueLoads(LS, PrologueLoads);
  collectEpilogueStores(LS, EpilogueStores);
  for (LoadInst *L : PrologueLoads)
    if (L->isVolatile() || L->isAtomic()) {
      LLVM_DEBUG(dbgs() << "    Unsafe: volatile/atomic prologue load\n");
      return false;
    }
  for (StoreInst *S : EpilogueStores)
    if (S->isVolatile() || S->isAtomic()) {
      LLVM_DEBUG(dbgs() << "    Unsafe: volatile/atomic epilogue store\n");
      return false;
    }
  return true;
}

// Collect the data-load chain instructions from the outer header that feed
// the inner loop. Uses backward value tracking from inner loop operands.
// Hardware-loop setup calls (@llvm.set.loop.iterations) are intentionally
// excluded — they stay in the outer header and are cloned separately into
// the cool-down block.
void AIEOuterLoopPipeliner::collectPrologueInstructions(
    const LoopStructure &LS, SmallVectorImpl<Instruction *> &Out) const {
  SmallPtrSet<Instruction *, 32> Visited;
  SmallVector<Instruction *, 16> Worklist;
  auto Seed = [&](Value *V) {
    auto *I = dyn_cast<Instruction>(V);
    if (!I || !LS.isPipelineableValue(I))
      return;
    if (Visited.insert(I).second)
      Worklist.push_back(I);
  };
  // Seed from values used inside the inner loop.
  for (BasicBlock *BB : LS.InnerLoop->blocks())
    for (Instruction &I : *BB)
      for (Value *Op : I.operands())
        Seed(Op);
  // Seed from initial values of inner header PHIs (from the preheader).
  for (PHINode &PHI : LS.InnerHeader->phis())
    for (unsigned I = 0; I < PHI.getNumIncomingValues(); ++I)
      if (PHI.getIncomingBlock(I) == LS.InnerPreheader)
        Seed(PHI.getIncomingValue(I));
  // Backward-track through operands.
  while (!Worklist.empty()) {
    Instruction *I = Worklist.pop_back_val();
    for (Value *Op : I->operands()) {
      auto *OpI = dyn_cast<Instruction>(Op);
      if (!OpI || !LS.isPipelineableValue(OpI))
        continue;
      if (Visited.insert(OpI).second)
        Worklist.push_back(OpI);
    }
  }
  // Emit the reached candidates in region program order.
  LS.PrologueRegion.forEachInstruction([&](Instruction *I) {
    if (LS.isPipelineCandidate(I) && Visited.count(I))
      Out.push_back(I);
  });
}

std::pair<BasicBlock *, BasicBlock *>
AIEOuterLoopPipeliner::cloneRegionSubgraph(
    const LoopStructure &LS, const SmallVectorImpl<Instruction *> &PInsts,
    ValueToValueMapTy &VMap, const Twine &BlockSuffix, const Twine &InstSuffix,
    BasicBlock *InsertBefore, SmallVectorImpl<BasicBlock *> &NewBlocks) {
  Function *F = LS.OuterHeader->getParent();

  // Create a clone block for each prologue-region block (currently the single
  // outer header) so branch targets and PHI incoming blocks can be remapped to
  // the clones afterwards.
  BasicBlock *CB = BasicBlock::Create(F->getContext(),
                                      LS.OuterHeader->getName() + BlockSuffix,
                                      F, InsertBefore);
  VMap[LS.OuterHeader] = CB;
  NewBlocks.push_back(CB);

  // Clone the Part-1 instructions into their block clone and remap.
  cloneAndRemapInsts(PInsts, *CB, CB->end(), VMap, InstSuffix);

  return {CB, CB};
}

BasicBlock *AIEOuterLoopPipeliner::clonePrologueAsWarmUp(
    const LoopStructure &LS, const SmallVectorImpl<Instruction *> &PInsts,
    ValueToValueMapTy &WarmUpVMap) {
  BasicBlock *Preheader = LS.getPreheader();

  // Pre-populate WarmUpVMap with the initial (preheader) values of outer header
  // PHIs so cloned loads use the entry pointer values.
  populateVMapFromPHIs(WarmUpVMap, LS, Preheader);

  SmallVector<BasicBlock *, 4> NewBlocks;
  auto [WarmUpEntry, WarmUpExit] = cloneRegionSubgraph(
      LS, PInsts, WarmUpVMap, ".peel.pro", ".peel", LS.OuterHeader, NewBlocks);

  // The warm-up region flows into the outer header.
  BranchInst::Create(LS.OuterHeader, WarmUpExit);
  // The preheader now flows into the warm-up region entry.
  Preheader->getTerminator()->replaceSuccessorWith(LS.OuterHeader, WarmUpEntry);
  LLVM_DEBUG(dbgs() << "    Created warm-up region: " << NewBlocks.size()
                    << " block(s) [" << WarmUpEntry->getName() << " .. "
                    << WarmUpExit->getName() << "]\n");
  return WarmUpExit;
}

Instruction *AIEOuterLoopPipeliner::cloneInstInto(Instruction &I,
                                                  BasicBlock &Dest,
                                                  BasicBlock::iterator InsertPt,
                                                  ValueToValueMapTy &VMap,
                                                  const Twine &Suffix) {
  Instruction *Clone = I.clone();
  SmallString<32> SuffixStorage;
  StringRef SuffixStr = Suffix.toStringRef(SuffixStorage);
  if (!SuffixStr.empty() && !Clone->getType()->isVoidTy())
    Clone->setName(I.getName() + SuffixStr);
  Clone->insertBefore(Dest, InsertPt);
  VMap[&I] = Clone;
  return Clone;
}

void AIEOuterLoopPipeliner::remapClones(ArrayRef<Instruction *> Clones,
                                        ValueToValueMapTy &VMap) {
  for (Instruction *CloneI : Clones)
    RemapInstruction(CloneI, VMap,
                     RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);
}

SmallVector<Instruction *, 16> AIEOuterLoopPipeliner::cloneAndRemapInsts(
    ArrayRef<Instruction *> Insts, BasicBlock &DstBB,
    BasicBlock::iterator InsertPt, ValueToValueMapTy &VMap,
    const Twine &Suffix) {
  SmallVector<Instruction *, 16> Clones;
  for (Instruction *I : Insts)
    Clones.push_back(cloneInstInto(*I, DstBB, InsertPt, VMap, Suffix));
  remapClones(Clones, VMap);
  return Clones;
}

LoopStructure
AIEOuterLoopPipeliner::cloneLoopNest(const LoopStructure &SrcLS,
                                     const Twine &Suffix,
                                     ValueToValueMapTy &VMap) const {
  Function *F = SrcLS.OuterHeader->getParent();

  // The nest in program order: outer header (== inner preheader for the linear
  // prologue), the inner-loop blocks, then the outer latch (== inner exit).
  // Clone each block; CloneBasicBlock copies all instructions and seeds VMap
  // with per-instruction old->new entries.
  SmallVector<BasicBlock *, 8> OrigBlocks;
  OrigBlocks.push_back(SrcLS.OuterHeader);
  OrigBlocks.append(SrcLS.InnerBlocks.begin(), SrcLS.InnerBlocks.end());
  // OuterLatch == InnerExit (validated in analyzeLoopStructure), so it is not
  // in InnerBlocks; append it once.
  OrigBlocks.push_back(SrcLS.OuterLatch);

  SmallString<32> SuffixStorage;
  StringRef SuffixStr = Suffix.toStringRef(SuffixStorage);
  SmallVector<BasicBlock *, 8> CloneBlocks;
  for (BasicBlock *BB : OrigBlocks) {
    BasicBlock *CB = CloneBasicBlock(BB, VMap, "." + SuffixStr, F);
    // CloneBasicBlock appends to the end of F; move the clone just before its
    // original so the cloned nest occupies the original's slot. After the
    // original nest is deleted the clone keeps the original program-order
    // layout (steady loop ahead of the peeled last-iteration and the exit).
    CB->moveBefore(BB);
    VMap[BB] = CB;
    CloneBlocks.push_back(CB);
  }

  // Remap intra-nest references (branch targets, PHI incoming blocks, operands)
  // to the clones. Edges to blocks outside the nest (e.g. the exit successor)
  // are absent from VMap and remain pointing at the originals for the caller to
  // rewire.
  remapInstructionsInBlocks(CloneBlocks, VMap);

  auto MapBlock = [&](BasicBlock *BB) { return cast<BasicBlock>(VMap[BB]); };
  SmallVector<BasicBlock *, 4> CloneInnerBlocks;
  for (BasicBlock *BB : SrcLS.InnerBlocks)
    CloneInnerBlocks.push_back(MapBlock(BB));

  LoopStructure CloneLS(
      /*OuterPreheader=*/nullptr, MapBlock(SrcLS.OuterHeader),
      MapBlock(SrcLS.OuterLatch), MapBlock(SrcLS.InnerPreheader),
      MapBlock(SrcLS.InnerHeader), MapBlock(SrcLS.InnerLatch),
      MapBlock(SrcLS.InnerExit), CloneInnerBlocks, SrcLS.OuterLoopID);

  // Mirror the prologue/epilogue regions onto the clone blocks so membership
  // queries (isPipelineableValue, isInEpilogue) work on the clone.
  SmallVector<BasicBlock *, 4> CloneProBlocks;
  for (BasicBlock *BB : SrcLS.PrologueRegion.blocks())
    CloneProBlocks.push_back(MapBlock(BB));
  CloneLS.PrologueRegion.assign(CloneProBlocks);
  CloneLS.EpilogueRegion.assign({MapBlock(SrcLS.OuterLatch)});

  // Copy the cached latch-bound; its instruction pointers still reference the
  // original nest and are remapped to the clone by remapBoundToClone.
  CloneLS.Bound = SrcLS.Bound;

  // Give the clone blocks clean role-based names (the "<orig>.<suffix>" names
  // from CloneBasicBlock read as if they were still the original loop).
  CloneLS.OuterHeader->setName(SuffixStr + ".header");
  CloneLS.OuterLatch->setName(SuffixStr + ".latch");
  for (auto [Orig, Clone] : zip(SrcLS.InnerBlocks, CloneLS.InnerBlocks))
    Clone->setName(SuffixStr + "." + Orig->getName());

  return CloneLS;
}

void AIEOuterLoopPipeliner::swapInClonedNest(
    const LoopStructure &OrigLS, LoopStructure &SteadyLS,
    const ValueToValueMapTy &SteadyVMap) const {
  BasicBlock *Preheader = OrigLS.getPreheader();
  BasicBlock *OrigExit = getExitBlock(OrigLS);

  // Preheader now flows into the clone's header instead of the original header.
  Preheader->getTerminator()->replaceSuccessorWith(OrigLS.OuterHeader,
                                                   SteadyLS.OuterHeader);
  SteadyLS.OuterPreheader = Preheader;

  // The clone's latch exit edge still points at OrigExit (left external by
  // cloneLoopNest); that is correct. Repoint the exit's loop-carried PHIs from
  // the original latch to the clone's latch, mapping each incoming value
  // through the clone VMap so the exit sees the clone's definitions.
  for (PHINode &PHI : OrigExit->phis()) {
    int Idx = PHI.getBasicBlockIndex(OrigLS.OuterLatch);
    if (Idx < 0)
      continue;
    Value *V = PHI.getIncomingValue(Idx);
    auto It = SteadyVMap.find(V);
    if (It != SteadyVMap.end())
      PHI.setIncomingValue(Idx, It->second);
    PHI.setIncomingBlock(Idx, SteadyLS.OuterLatch);
  }
}

void AIEOuterLoopPipeliner::remapBoundToClone(
    LoopStructure &SteadyLS, const ValueToValueMapTy &SteadyVMap) const {
  auto Map = [&](Value *V) -> Value * {
    if (!V)
      return V;
    auto It = SteadyVMap.find(V);
    return It != SteadyVMap.end() ? static_cast<Value *>(It->second) : V;
  };
  LatchBoundInfo &B = SteadyLS.Bound;
  B.Cmp = cast<ICmpInst>(Map(B.Cmp));
  B.Limit = Map(B.Limit); // loop-invariant Limit stays as-is (not in VMap)
  B.Counter = cast_or_null<BinaryOperator>(Map(B.Counter));
  B.OldIV = cast_or_null<PHINode>(Map(B.OldIV));
}

void AIEOuterLoopPipeliner::deleteOrigNest(const LoopStructure &OrigLS) const {
  // The original nest is unreachable after swapInClonedNest. Collect its blocks
  // and delete them as a set (DeleteDeadBlocks drops inter-block references
  // within the set, e.g. the back-edge and PHIs).
  SmallVector<BasicBlock *, 8> Dead;
  Dead.push_back(OrigLS.OuterHeader);
  Dead.append(OrigLS.InnerBlocks.begin(), OrigLS.InnerBlocks.end());
  Dead.push_back(OrigLS.OuterLatch);
  DeleteDeadBlocks(Dead);
}

// Clone the data-load chain into the epilogue block (outer latch), inserting
// the clones before the terminator. The clones use the NEXT-iteration pointer
// values (the latch incoming values of the outer header PHIs) so that the
// loads prefetch data for the next outer iteration.
void AIEOuterLoopPipeliner::clonePrologueIntoEpilogue(
    const LoopStructure &LS, const SmallVectorImpl<Instruction *> &PInsts,
    ValueToValueMapTy &EpiVMap) {
  // Pre-populate EpiVMap with the NEXT-iteration values of outer header PHIs.
  // This ensures that cloned loads use %a.ptr.next, %b.ptr.next, etc.
  populateVMapFromPHIs(EpiVMap, LS, LS.OuterLatch);

  // Epilogue is always in OuterLatch (linear structure): insert the flat clone
  // chain before the latch terminator.
  Instruction *LatchTerm = LS.OuterLatch->getTerminator();
  cloneAndRemapInsts(PInsts, *LS.OuterLatch, LatchTerm->getIterator(), EpiVMap,
                     ".epi");
  LLVM_DEBUG(dbgs() << "    Cloned prologue into epilogue\n");
}

// Update outer header PHI predecessors: the preheader edge now comes from
// the warm-up block.
void AIEOuterLoopPipeliner::updateOuterHeaderPHIs(const LoopStructure &LS,
                                                  BasicBlock *WarmUp,
                                                  BasicBlock *Preheader) {
  for (PHINode &PHI : LS.OuterHeader->phis()) {
    const int PreIdx = PHI.getBasicBlockIndex(Preheader);
    assert(PreIdx >= 0);
    PHI.setIncomingBlock(PreIdx, WarmUp);
  }
}

// For each data-load instruction I in the outer header, create a PHI node:
//   phi_I = phi [WarmUpVMap[I], warm-up], [EpiVMap[I], outer.latch]
// Replace all uses of I inside the inner loop with phi_I, then erase I.
void AIEOuterLoopPipeliner::createPipelinedPHIs(
    const LoopStructure &LS, BasicBlock *WarmUp,
    const SmallVectorImpl<Instruction *> &PInsts,
    const ValueToValueMapTy &WarmUpVMap, const ValueToValueMapTy &EpiVMap) {
  Instruction *InsertPt = &*LS.OuterHeader->getFirstInsertionPt();

  SmallVector<std::pair<Instruction *, PHINode *>, 8> Replacements;
  for (Instruction *I : PInsts) {
    // Void-typed instructions (stores, side-effect-only intrinsics) don't
    // produce values, so there's nothing to merge via a PHI node. Each
    // execution path simply runs its own cloned copy independently.
    if (I->getType()->isVoidTy())
      continue;
    auto WIt = WarmUpVMap.find(I);
    auto EIt = EpiVMap.find(I);
    // Both cloning functions (clonePrologueAsWarmUp, clonePrologueIntoEpilogue)
    // unconditionally add every PInsts entry to their respective maps, so every
    // non-void instruction must be present in both.
    assert(WIt != WarmUpVMap.end() && EIt != EpiVMap.end() &&
           "Prologue instruction must be in both WarmUp and Epilogue VMaps");
    Value *WarmUpVal = WIt->second;
    Value *EpiVal = EIt->second;
    PHINode *PHI = PHINode::Create(I->getType(), 2, I->getName() + ".phi");
    PHI->insertBefore(InsertPt->getIterator());
    PHI->addIncoming(WarmUpVal, WarmUp);
    PHI->addIncoming(EpiVal, LS.OuterLatch);
    Replacements.push_back({I, PHI});
  }

  // Replace ALL uses of original prologue instructions with the PHI nodes.
  // This covers both:
  //   (a) uses inside the inner loop (the primary goal), and
  //   (b) intra-prologue uses in the outer header (e.g., a shuffle that uses
  //       a load result — both are prologue instructions, and the shuffle's
  //       use of the load must be replaced so the load becomes use_empty).
  // The warm-up and epilogue clones do not use the originals (they use cloned
  // values via WarmUpVMap/EpiVMap), so replaceAllUsesWith is safe here.
  for (auto &[Orig, PHI] : Replacements)
    Orig->replaceAllUsesWith(PHI);

  // Erase original prologue instructions from the outer header (reverse order).
  for (auto It = PInsts.rbegin(); It != PInsts.rend(); ++It) {
    Instruction *I = *It;
    if (I->use_empty())
      I->eraseFromParent();
  }
}

// Create the cool-down region for the last outer iteration (N-1).
//
// The outer latch's false branch is redirected to cooldown.entry.
//   cooldown.entry: clones set.loop.iterations from outer header
//   inner loop clone: uses last epilogue load values (outer header PHI latch
//   vals) cooldown.exit: clones epilogue stores (no loads), branches to
//   original exit
//
// The cool-down VMap maps each outer header PHI to its latch incoming value:
//   %v0.phi -> %v0.epi  (last epilogue load)
//   %c.ptr  -> %c.ptr.next  (next output pointer)
//   etc.
void AIEOuterLoopPipeliner::peelLastIterationEpilogue(
    const LoopStructure &LS, const SmallPtrSetImpl<Instruction *> &OrigEpiInsts,
    const SmallVectorImpl<Instruction *> &PartTwoInsts) {
  Function *F = LS.OuterHeader->getParent();
  LLVMContext &Ctx = F->getContext();

  BasicBlock *OrigExit = getExitBlock(LS);

  // Map each outer header PHI to its latch (last-iteration) incoming value so
  // every clone below picks up the final epilogue values:
  //   %v0.phi -> %v0.epi, %c.ptr -> %c.ptr.next, etc.
  ValueToValueMapTy CoolVMap;
  populateVMapFromPHIs(CoolVMap, LS, LS.OuterLatch);

  // cooldown.entry holds the hardware-loop setup and the Part-2 accumulator
  // seeds; build it, then the inner-loop skeleton, then fill everything in.
  BasicBlock *CoolEntry =
      BasicBlock::Create(Ctx, "cooldown.entry", F, OrigExit);
  cloneHardwareLoopSetupInto(CoolEntry, LS, CoolVMap);

  BasicBlock *CoolExit =
      createCooldownSkeleton(LS, CoolEntry, OrigExit, CoolVMap);

  // Clone Part-2 instructions into cooldown.entry before the inner loop so the
  // cloned inner-loop PHIs that reference Part-2 results resolve to them.
  cloneAndRemapInsts(PartTwoInsts, *CoolEntry, CoolEntry->end(), CoolVMap,
                     ".cd");
  cloneInnerLoopIntoCooldown(LS, CoolEntry, CoolVMap);
  populateCooldownExit(CoolExit, LS, OrigEpiInsts, CoolVMap);
  wireCooldownIntoCFG(LS, CoolEntry, CoolExit, OrigExit);

  LLVM_DEBUG(dbgs() << "    Created cool-down: " << CoolEntry->getName()
                    << " -> " << CoolExit->getName() << "\n");
}

BasicBlock *AIEOuterLoopPipeliner::getExitBlock(const LoopStructure &LS) const {
  BranchInst *LatchBr = LS.getLatchBranch();
  assert(LatchBr->isConditional() && "Outer latch must be conditional");
  // The true branch goes back to outer.header; the other goes to the exit.
  BasicBlock *OrigExit = nullptr;
  for (unsigned I = 0; I < LatchBr->getNumSuccessors(); ++I) {
    BasicBlock *Succ = LatchBr->getSuccessor(I);
    if (Succ != LS.OuterHeader) {
      OrigExit = Succ;
      break;
    }
  }
  assert(OrigExit && "Outer latch must have an exit successor");
  return OrigExit;
}

void AIEOuterLoopPipeliner::cloneHardwareLoopSetupInto(
    BasicBlock *Dest, const LoopStructure &LS,
    ValueToValueMapTy &CoolVMap) const {
  SmallVector<Instruction *, 4> SetupInsts;
  for (Instruction &I : *LS.OuterHeader) {
    if (I.isTerminator())
      break;
    if (AIEIRUtils::isHardwareLoopSetup(&I))
      SetupInsts.push_back(&I);
  }
  cloneAndRemapInsts(SetupInsts, *Dest, Dest->end(), CoolVMap, /*Suffix=*/"");
}

BasicBlock *AIEOuterLoopPipeliner::createCooldownSkeleton(
    const LoopStructure &LS, BasicBlock *CoolEntry, BasicBlock *OrigExit,
    ValueToValueMapTy &CoolVMap) const {
  Function *F = LS.OuterHeader->getParent();
  LLVMContext &Ctx = F->getContext();

  // One empty clone per inner-loop block, recorded in CoolVMap so
  // cloneInnerLoopIntoCooldown can resolve each block to its clone.
  for (BasicBlock *BB : LS.InnerBlocks)
    CoolVMap[BB] = BasicBlock::Create(Ctx, BB->getName() + ".cd", F, OrigExit);

  // Inner-loop PHIs incoming from the outer header now come from
  // cooldown.entry.
  CoolVMap[LS.OuterHeader] = CoolEntry;

  // cooldown.exit receives the latch (and, in the single-block case, the inner
  // exit) so the cloned inner loop and epilogue resolve their exits to it.
  BasicBlock *CoolExit = BasicBlock::Create(Ctx, "cooldown.exit", F, OrigExit);
  CoolVMap[LS.OuterLatch] = CoolExit;
  if (LS.InnerExit == LS.OuterLatch)
    CoolVMap[LS.InnerExit] = CoolExit;
  return CoolExit;
}

void AIEOuterLoopPipeliner::cloneInnerLoopIntoCooldown(
    const LoopStructure &LS, BasicBlock *CoolEntry,
    ValueToValueMapTy &CoolVMap) const {
  // Clone every inner-loop block's body into its skeleton clone (resolved via
  // CoolVMap) first, so all cross-block references exist before any remap runs.
  SmallVector<Instruction *, 32> Clones;
  for (BasicBlock *Orig : LS.InnerBlocks) {
    auto *Clone = cast<BasicBlock>(CoolVMap[Orig]);
    for (Instruction &Inst : *Orig)
      Clones.push_back(
          cloneInstInto(Inst, *Clone, Clone->end(), CoolVMap, ".cd"));
  }
  remapClones(Clones, CoolVMap);

  BasicBlock *ClonedInnerHeader = cast<BasicBlock>(CoolVMap[LS.InnerHeader]);
  BranchInst::Create(ClonedInnerHeader, CoolEntry);
}

void AIEOuterLoopPipeliner::populateCooldownExit(
    BasicBlock *CoolExit, const LoopStructure &LS,
    const SmallPtrSetImpl<Instruction *> &OrigEpiInsts,
    ValueToValueMapTy &CoolVMap) const {
  SmallVector<Instruction *, 16> EpiInsts;
  for (Instruction &I : *LS.OuterLatch) {
    if (I.isTerminator())
      break;
    // No prefetch in the cool-down.
    if (isa<LoadInst>(&I))
      continue;
    // Skip the prologue clones inserted into the epilogue earlier.
    if (!OrigEpiInsts.count(&I))
      continue;
    EpiInsts.push_back(&I);
  }
  cloneAndRemapInsts(EpiInsts, *CoolExit, CoolExit->end(), CoolVMap, ".cd");
}

void AIEOuterLoopPipeliner::wireCooldownIntoCFG(const LoopStructure &LS,
                                                BasicBlock *CoolEntry,
                                                BasicBlock *CoolExit,
                                                BasicBlock *OrigExit) const {
  BranchInst::Create(OrigExit, CoolExit);

  for (PHINode &PHI : OrigExit->phis()) {
    const int LatchIdx = PHI.getBasicBlockIndex(LS.OuterLatch);
    assert(LatchIdx >= 0);
    PHI.setIncomingBlock(LatchIdx, CoolExit);
  }

  BranchInst *LatchBr = LS.getLatchBranch();
  for (unsigned I = 0; I < LatchBr->getNumSuccessors(); ++I) {
    if (LatchBr->getSuccessor(I) == OrigExit) {
      LatchBr->setSuccessor(I, CoolEntry);
      break;
    }
  }
}

// Validates the latch exit condition and caches all discovered components in
// LS.LatchBound. Returns true if the loop bound can be adjusted (Step != 0).
// All subsequent uses of the latch icmp pattern (adjustLoopBound,
// getDowncountingInfo) read from LS.LatchBound without re-scanning.
std::optional<LatchBoundInfo>
AIEOuterLoopPipeliner::canAdjustLoopBound(const LoopStructure &LS) const {
  auto *BI = dyn_cast<BranchInst>(LS.OuterLatch->getTerminator());
  if (!BI || !BI->isConditional())
    return std::nullopt;
  auto *Cmp = dyn_cast<ICmpInst>(BI->getCondition());
  if (!Cmp)
    return std::nullopt;
  ICmpInst::Predicate Pred = Cmp->getPredicate();

  // Find the loop-invariant limit and non-invariant counter.
  Value *Limit = nullptr;
  Value *Counter = nullptr;
  unsigned LimitIdx = 0;
  for (unsigned I = 0; I < 2; ++I) {
    Value *Op = Cmp->getOperand(I);
    if (LS.OuterLoop->isLoopInvariant(Op)) {
      Limit = Op;
      LimitIdx = I;
      Counter = Cmp->getOperand(1 - I);
      break;
    }
  }
  if (!Limit)
    return std::nullopt;

  // Determine the induction step. If Counter is an add with a constant
  // operand, extract the step directly. Otherwise, infer from the predicate.
  // EQ/NE without a visible constant step is ambiguous — bail out.
  int64_t Step = 0;
  BinaryOperator *CounterAdd = nullptr;
  if (auto *Add = dyn_cast<BinaryOperator>(Counter)) {
    if (Add->getOpcode() == Instruction::Add) {
      if (auto *C = dyn_cast<ConstantInt>(Add->getOperand(1))) {
        Step = C->getSExtValue();
        CounterAdd = Add;
      }
    }
  }
  if (Step == 0) {
    if (Pred == ICmpInst::ICMP_SLT || Pred == ICmpInst::ICMP_ULT)
      Step = 1;
    else if (Pred == ICmpInst::ICMP_SGT || Pred == ICmpInst::ICMP_UGT)
      Step = -1;
    else {
      LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound: EQ/NE with no "
                           "visible constant step\n");
      return std::nullopt;
    }
  }

  // CounterAdd must be non-null: the step must come from an explicit add
  // instruction so that we can identify the counting PHI (OldIV) and verify
  // the loop induction structure. Plain-PHI counters (no visible add) are
  // rejected to avoid relying on an unverifiable step assumption.
  if (!CounterAdd) {
    LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound: counter is not an add "
                         "instruction\n");
    return std::nullopt;
  }

  // Find the counting PHI in the outer header that feeds CounterAdd.
  // Without OldIV we cannot verify the induction structure or support JNZD.
  PHINode *OldIV = nullptr;
  for (Value *Op : CounterAdd->operands()) {
    if (auto *PHI = dyn_cast<PHINode>(Op)) {
      if (PHI->getParent() == LS.OuterHeader) {
        OldIV = PHI;
        break;
      }
    }
  }
  if (!OldIV) {
    LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound: counting PHI not "
                         "found in outer header\n");
    return std::nullopt;
  }

  return LatchBoundInfo{Cmp, Limit, LimitIdx, Step, CounterAdd, OldIV};
}

// Adjust the outer loop trip count from N to N-1 using the pre-computed
// LS.LatchBound (populated by canAdjustLoopBound). The unified formula is:
//   NewLimit = Limit - Step
// which correctly handles increment (Step > 0) and decrement (Step < 0) loops
// of any constant step magnitude.
Value *AIEOuterLoopPipeliner::adjustLoopBound(const LoopStructure &LS) {
  // LS.Bound is always valid (set by analyzeLoopStructure).
  const LatchBoundInfo &B = LS.Bound;
  IRBuilder<> Builder(LS.getPreheader()->getTerminator());
  Value *NewLimit = Builder.CreateSub(
      B.Limit, ConstantInt::getSigned(B.Limit->getType(), B.Step),
      "outer.trip.adj");
  B.Cmp->setOperand(B.LimitIdx, NewLimit);
  LLVM_DEBUG(dbgs() << "    Adjusted loop bound: N -> N-1 (step=" << B.Step
                    << ")\n");
  return NewLimit;
}

// Adjust loop metadata after a successful outer-loop pipelining transformation:
//   1. Decrement llvm.loop.itercount.range by 1 (one iteration was peeled).
//   2. Drop llvm.loop.hint.aie-enable-outer-loop-pipelining (consumed).
//   3. Insert llvm.loop.hint.aie_outerloop_pipeliner_success = i64 1.
void AIEOuterLoopPipeliner::updateLoopMetadata(const LoopStructure &LS) {
  MDNode *LoopID = LS.OuterLoopID;
  LLVMContext &Ctx = LS.OuterHeader->getContext();

  // Adjust itercount.range (N → N-1).
  MDNode *UpdatedID =
      LoopID ? updateIterCounts(
                   Ctx, LoopID, [](int64_t V) { return V - 1; }, // FixMin
                   [](int64_t V) { return V - 1; })              // FixMax
             : nullptr;

  // Rebuild the metadata node, dropping the consumed hint and
  // appending the success marker.
  MDNode *Source = UpdatedID ? UpdatedID : LoopID;
  if (!Source)
    return;

  static constexpr StringLiteral HintKey{
      "llvm.loop.hint.aie-enable-outer-loop-pipelining"};
  static constexpr StringLiteral SuccessKey{
      "llvm.loop.hint.aie_outerloop_pipeliner_success"};

  SmallVector<Metadata *, 8> MDs;
  for (unsigned I = 1, E = Source->getNumOperands(); I < E; ++I) {
    MDNode *Entry = cast<MDNode>(Source->getOperand(I));
    auto Key = AIELoopUtils::getMetadataKey(*Entry);
    if (Key && *Key == HintKey)
      continue; // drop the consumed enable hint
    MDs.push_back(Entry);
  }

  // Append the success marker:
  // !{!"llvm.loop.hint.aie_outerloop_pipeliner_success", i64 1}
  MDNode *SuccessEntry = MDNode::get(
      Ctx,
      {MDString::get(Ctx, SuccessKey),
       ConstantAsMetadata::get(ConstantInt::get(Type::getInt64Ty(Ctx), 1))});
  MDs.push_back(SuccessEntry);

  MDNode *FinalLoopID = MDNode::get(Ctx, MDs);
  FinalLoopID->replaceOperandWith(0, FinalLoopID);
  // Write llvm.loop onto the outer latch terminator (what Loop::setLoopID does
  // internally), so this works on a clone with no LoopInfo Loop.
  LS.OuterLatch->getTerminator()->setMetadata(LLVMContext::MD_loop,
                                              FinalLoopID);
}

bool AIEOuterLoopPipeliner::collectPrologueInstructionsForSplit(
    const LoopStructure &LS, SmallVectorImpl<Instruction *> &Out) const {
  // Find Part 2 anchors = relevant producing CallInsts within the
  // prologue that are reachable from loads (forward-tracking).
  // We forward-track from loads to find anchors, then collect all their
  // descendants. Part 1 = everything else (all prologue instructions that are
  // not Part 2 anchors or their descendants, and not set.loop.iterations).
  SmallPtrSet<Instruction *, 32> ReachableFromLoad;
  SmallVector<Instruction *, 32> FwdWorklist;
  LS.PrologueRegion.forEachInstruction([&](Instruction *I) {
    if (isa<LoadInst>(I)) {
      ReachableFromLoad.insert(I);
      FwdWorklist.push_back(I);
    }
  });
  while (!FwdWorklist.empty()) {
    Instruction *I = FwdWorklist.pop_back_val();
    for (User *U : I->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      if (!UI || !LS.isPipelineableValue(UI))
        continue;
      if (ReachableFromLoad.insert(UI).second)
        FwdWorklist.push_back(UI);
    }
  }

  // Find anchors = instructions matching any split strategy in
  // ReachableFromLoad.
  SmallPtrSet<Instruction *, 16> Anchors;
  for (Instruction *I : ReachableFromLoad)
    if (isAnchorInstruction(I))
      Anchors.insert(I);

  if (Anchors.empty())
    return false;

  // Find all descendants of anchors within the prologue (Part 2 set).
  SmallPtrSet<Instruction *, 32> PartTwoSet;
  PartTwoSet.insert(Anchors.begin(), Anchors.end());
  SmallVector<Instruction *, 16> DescWorklist(Anchors.begin(), Anchors.end());
  while (!DescWorklist.empty()) {
    Instruction *I = DescWorklist.pop_back_val();
    for (User *U : I->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      if (!UI || !LS.isPipelineableValue(UI))
        continue;
      if (PartTwoSet.insert(UI).second)
        DescWorklist.push_back(UI);
    }
  }

  // Part 1 = region pipeline candidates not in Part 2 (the load/address chain;
  // the post-anchor cone stays in Part 2). Loop-carried PHIs, terminators, and
  // hardware-loop setup are excluded by isPipelineCandidate.
  LS.PrologueRegion.forEachInstruction([&](Instruction *I) {
    if (LS.isPipelineCandidate(I) && !PartTwoSet.count(I))
      Out.push_back(I);
  });

  LLVM_DEBUG(dbgs() << "    Split-prologue: " << Anchors.size()
                    << " Number of anchor(s), " << Out.size()
                    << " Part-1 instructions\n");
  return true;
}

// Lift update instructions from the epilogue to the end of the prologue.
// This allows the main pipelining transformation to naturally include them
// when cloning the prologue to warmup and epilogue.
//
// Approach: For each outer header PHI node, find the latch incoming value.
// If it's an instruction defined in the epilogue, backward-track to find
// the entire computation chain. Validate each chain independently and only
// lift chains that can be lifted:
//   1. No inner loop dependencies
//   2. No uses by other epilogue instructions (outside the chain)
//
// This ensures we don't lift chains used by stores or loop control logic.
bool AIEOuterLoopPipeliner::liftEpiloguePointerUpdatesToPrologue(
    const LoopStructure &LS) {
  Instruction *InsertPt = LS.OuterHeader->getTerminator();

  // Collect all liftable chains across all PHIs.
  SmallPtrSet<Instruction *, 32> AllLiftable;

  // Process each PHI independently to allow partial lifting.
  for (PHINode &PHI : LS.OuterHeader->phis()) {
    // Get the incoming value from the latch (back-edge).
    Value *LatchVal = PHI.getIncomingValueForBlock(LS.OuterLatch);
    auto *LatchInst = dyn_cast<Instruction>(LatchVal);
    if (!LatchInst || !LS.isInEpilogue(LatchInst))
      continue;

    // Backward-track from LatchInst to find all epilogue instructions that
    // form the computation chain for this PHI's next-iteration value.
    SmallPtrSet<Instruction *, 16> Chain;
    SmallVector<Instruction *, 16> Worklist;
    Chain.insert(LatchInst);
    Worklist.push_back(LatchInst);

    bool CanLift = true;
    while (!Worklist.empty() && CanLift) {
      Instruction *I = Worklist.pop_back_val();

      // If this is an intrinsic call, check if it's a safe pointer increment.
      // We only allow lifting chains that contain 2D/3D pointer intrinsics;
      // any other intrinsic may have unknown side effects.
      if (auto *II = dyn_cast<IntrinsicInst>(I)) {
        if (!isSafePointerIncrementIntrinsic(II->getIntrinsicID())) {
          CanLift = false;
          LLVM_DEBUG(dbgs() << "    PHI " << PHI.getName()
                            << ": cannot lift (unsafe intrinsic "
                            << II->getName() << ")\n");
          break;
        }
      }

      for (Value *Op : I->operands()) {
        auto *OpI = dyn_cast<Instruction>(Op);
        if (!OpI)
          continue;
        // If the operand is defined in the inner loop, we cannot lift.
        if (LS.InnerLoop->contains(OpI->getParent())) {
          CanLift = false;
          break;
        }
        // If the operand is defined in the epilogue, add to the chain.
        if (LS.isInEpilogue(OpI)) {
          if (Chain.insert(OpI).second)
            Worklist.push_back(OpI);
        }
        // Operands from outer header (PHIs) or preheader are OK - they're
        // already available before the inner loop.
      }
    }

    // Additional check: if any instruction in the chain has uses by other
    // epilogue instructions (outside the chain), we cannot lift this chain.
    // This excludes chains used by stores, loop control (icmp), etc.
    if (CanLift) {
      for (Instruction *I : Chain) {
        for (User *U : I->users()) {
          auto *UI = dyn_cast<Instruction>(U);
          if (!UI)
            continue;
          // If the user is in the epilogue AND not part of this chain,
          // this chain has external epilogue uses - don't lift it.
          if (LS.isInEpilogue(UI) && !Chain.count(UI)) {
            CanLift = false;
            LLVM_DEBUG(dbgs() << "    PHI " << PHI.getName()
                              << ": cannot lift (epilogue use by "
                              << UI->getName() << ")\n");
            break;
          }
        }
        if (!CanLift)
          break;
      }
    }

    if (CanLift) {
      AllLiftable.insert(Chain.begin(), Chain.end());
      LLVM_DEBUG(dbgs() << "    PHI " << PHI.getName() << ": lifting chain of "
                        << Chain.size() << " instructions\n");
    }
  }

  if (AllLiftable.empty())
    return false;

  // Collect liftable instructions in program order.
  SmallVector<Instruction *, 32> ToLift;
  for (Instruction &I : *LS.OuterLatch)
    if (AllLiftable.count(&I))
      ToLift.push_back(&I);

  // Move each instruction to the end of the prologue block (before terminator).
  for (Instruction *I : ToLift)
    I->moveBefore(InsertPt->getIterator());

  LLVM_DEBUG(dbgs() << "    Lifted " << ToLift.size()
                    << " instructions from epilogue to prologue\n");
  return true;
}

void AIEOuterLoopPipeliner::collectPartTwoInstructions(
    const LoopStructure &LS, const SmallPtrSetImpl<Instruction *> &PartOneSet,
    SmallVectorImpl<Instruction *> &Out) const {
  // Find anchors: instructions matching any split strategy that are direct
  // users of Part 1 instructions (or transitively reachable from Part 1 within
  // the prologue).
  SmallPtrSet<Instruction *, 32> PartTwoSet;
  SmallVector<Instruction *, 16> Worklist;

  // Seed: forward-track from Part 1 instructions to find anchor instructions.
  for (Instruction *P1 : PartOneSet) {
    for (User *U : P1->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      if (!UI || !LS.isPipelineableValue(UI))
        continue;
      if (!PartOneSet.count(UI) && isAnchorInstruction(UI)) {
        if (PartTwoSet.insert(UI).second)
          Worklist.push_back(UI);
      }
    }
  }

  // Forward-track from anchors to collect all descendants within the prologue.
  while (!Worklist.empty()) {
    Instruction *I = Worklist.pop_back_val();
    for (User *U : I->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      if (!UI || !LS.isPipelineableValue(UI))
        continue;
      if (!PartOneSet.count(UI) && PartTwoSet.insert(UI).second)
        Worklist.push_back(UI);
    }
  }

  // Emit PartTwoSet in region program order.
  LS.PrologueRegion.forEachInstruction([&](Instruction *I) {
    if (PartTwoSet.count(I))
      Out.push_back(I);
  });

  LLVM_DEBUG(dbgs() << "    Split-prologue: " << Out.size()
                    << " Part-2 instructions (stay in outer.header + "
                       "cooldown.entry)\n");
}

bool AIEOuterLoopPipeliner::performTransformation(
    LoopStructure &OrigLS, const AIE::LoopOptionOverrides &Overrides) {
  // Lift pointer update instructions from epilogue to prologue.
  // This must happen BEFORE collecting prologue instructions so that the
  // lifted instructions are included in the data-load chain.
  liftEpiloguePointerUpdatesToPrologue(OrigLS);

  // Collect the data-load chain instructions from the outer header.
  // Hardware-loop setup calls (set.loop.iterations) are excluded.
  // In split-prologue mode, we split the prologue in two parts.
  SmallVector<Instruction *, 16> PInsts;
  if (Overrides.get(SplitPrologue)) {
    if (!collectPrologueInstructionsForSplit(OrigLS, PInsts)) {
      LLVM_DEBUG(dbgs() << "    Split-prologue: no split points\n");
      collectPrologueInstructions(OrigLS, PInsts);
    } else {
      LLVM_DEBUG(dbgs() << "    Split-prologue: pipelining " << PInsts.size()
                        << " Part-1 instructions\n");
    }
  } else {
    collectPrologueInstructions(OrigLS, PInsts);
  }
  if (PInsts.empty()) {
    LLVM_DEBUG(dbgs() << "    No prologue instructions found\n");
    return false;
  }

  // Collect Part 2 instructions (split-prologue mode only) on the ORIGINAL nest
  // while Part 1 instructions still exist for forward-tracking.
  SmallVector<Instruction *, 16> PartTwoInsts;
  if (Overrides.get(SplitPrologue)) {
    SmallPtrSet<Instruction *, 32> PartOneSet;
    PartOneSet.insert(PInsts.begin(), PInsts.end());
    collectPartTwoInstructions(OrigLS, PartOneSet, PartTwoInsts);
  }

  // Snapshot original epilogue instructions (the latch contents) so the
  // last-iteration epilogue can be filtered to original stores only.
  SmallPtrSet<Instruction *, 32> OrigEpiInsts;
  for (Instruction &I : *OrigLS.OuterLatch)
    OrigEpiInsts.insert(&I);

  // Clone the whole nest into a fresh, steady-state copy and swap it into the
  // CFG in the original's place (preheader -> clone header -> ... -> exit). The
  // original nest is now unreachable; all transform steps below run on the
  // clone so the original is never mutated and is deleted at the end.
  ValueToValueMapTy SteadyVMap;
  LoopStructure SteadyLS = cloneLoopNest(OrigLS, "steady", SteadyVMap);
  swapInClonedNest(OrigLS, SteadyLS, SteadyVMap);

  // Translate the collected instruction lists / bound from the original nest to
  // the steady clone via the clone VMap (loop-invariant operands not in the map
  // stay as-is). All subsequent transform steps operate on the clone.
  auto MapInst = [&](Instruction *I) -> Instruction * {
    auto It = SteadyVMap.find(I);
    return It != SteadyVMap.end()
               ? cast<Instruction>(static_cast<Value *>(It->second))
               : I;
  };
  for (Instruction *&I : PInsts)
    I = MapInst(I);
  for (Instruction *&I : PartTwoInsts)
    I = MapInst(I);
  SmallPtrSet<Instruction *, 32> SteadyEpiInsts;
  for (Instruction *I : OrigEpiInsts)
    SteadyEpiInsts.insert(MapInst(I));
  remapBoundToClone(SteadyLS, SteadyVMap);

  // Clone data-load chain into a warm-up block (before the steady header).
  // The warm-up uses the entry pointer values (PHI initial values).
  ValueToValueMapTy WarmUpVMap;
  BasicBlock *WarmUp = clonePrologueAsWarmUp(SteadyLS, PInsts, WarmUpVMap);
  WarmUp->setName("steady.preheader");

  // Clone data-load chain into the epilogue (steady latch).
  // The clones use the NEXT-iteration pointer values (latch incoming values).
  ValueToValueMapTy EpiVMap;
  clonePrologueIntoEpilogue(SteadyLS, PInsts, EpiVMap);

  // Update steady header PHI predecessors: preheader -> warm-up.
  updateOuterHeaderPHIs(SteadyLS, WarmUp, SteadyLS.getPreheader());
  // The warm-up block is now the steady loop's preheader.
  SteadyLS.OuterPreheader = WarmUp;

  // Create pipelined PHI nodes in the steady header for each data-load value.
  createPipelinedPHIs(SteadyLS, WarmUp, PInsts, WarmUpVMap, EpiVMap);

  // Adjust the outer loop trip count from N to N-1. Must happen before the peel
  // so the hardware-loop conversion can find the right icmp to replace.
  Value *AdjustedTripCount = adjustLoopBound(SteadyLS);

  // Optionally convert the steady loop to a JNZD hardware loop. MUST run before
  // peelLastIterationEpilogue so the counting add/icmp are erased from the
  // latch before the peel step clones it.
  if (EnableOuterLoopHardwareLoop && AdjustedTripCount)
    if (auto Info = getDowncountingInfo(SteadyLS))
      convertOuterLoopToHardwareLoop(SteadyLS, WarmUp, AdjustedTripCount,
                                     *Info);

  // Create the last-iteration (cool-down) region from the steady loop.
  peelLastIterationEpilogue(SteadyLS, SteadyEpiInsts, PartTwoInsts);

  // Adjust itercount metadata to reflect the reduced trip count.
  updateLoopMetadata(SteadyLS);

  // The original nest is unreachable; delete it.
  deleteOrigNest(OrigLS);

  return true;
}

// Returns the downcounting pattern components for @llvm.loop.decrement.reg
// conversion if the latch has the canonical pattern (icmp eq/ne, step -1).
// All LatchBound fields are guaranteed non-null, so only the step is checked.
// Returns the downcounting pattern components for JNZD hardware loop
// conversion. LS.Bound is always valid; only the step is checked.
std::optional<DowncountingInfo>
AIEOuterLoopPipeliner::getDowncountingInfo(const LoopStructure &LS) const {
  // Hardware loop requires step == -1 (loop.decrement.reg decrements by 1).
  if (LS.Bound.Step != -1)
    return std::nullopt;
  return DowncountingInfo{LS.Bound.Cmp, LS.Bound.Counter, LS.Bound.OldIV};
}

// Convert the outer loop to a JNZD hardware loop.
//
// Before (after adjustLoopBound):
//   preheader:
//     %outer.trip.minus1 = sub i32 %N, 1
//     br outer.header
//
//   outer.header:
//     %phi = phi i32 [%init, %warm_up], [%next, %outer.latch]
//     ...
//
//   outer.latch:
//     %counter = add i32 %phi, -1
//     %cond    = icmp eq i32 %counter, %limit   ; limit = 1 after
//     adjustLoopBound br i1 %cond, label %cooldown.entry, label %outer.header
//
// After:
//   preheader:
//     %outer.trip.minus1 = sub i32 %N, 1        ; already there
//     %ctr.init = call i32 @llvm.start.loop.iterations.i32(i32
//     %outer.trip.minus1) br outer.header
//
//   outer.header:
//     %phi = phi i32 [%init, %warm_up], [%next, %outer.latch]
//     %ctr = phi i32 [%ctr.init, %warm_up], [%ctr.next, %outer.latch]
//     ...
//
//   outer.latch:
//     %ctr.next = call i32 @llvm.loop.decrement.reg.i32(i32 %ctr, i32 1)
//     %loop.cond = icmp ne i32 %ctr.next, 0
//     br i1 %loop.cond, label %outer.header, label %cooldown.entry
//     ; (old %counter add and %cond icmp become dead and are deleted)
//
void AIEOuterLoopPipeliner::convertOuterLoopToHardwareLoop(
    const LoopStructure &LS, BasicBlock *WarmUp, Value *AdjustedTripCount,
    const DowncountingInfo &Info) {
  LLVMContext &Ctx = LS.OuterHeader->getContext();
  Type *I32Ty = Type::getInt32Ty(Ctx);

  // Unpack the pre-validated downcounting pattern from Info.
  // Note: AdjustedTripCount (returned by adjustLoopBound) is the new icmp
  // *threshold* (e.g., the constant 1 for a decrement loop), NOT the loop
  // trip count.  The actual trip count for JNZD is:
  //   OldIV_initial_value - 1
  // where OldIV_initial_value is the preheader/warm-up incoming of OldIV.
  PHINode *OldIV = Info.OldIV;

  // Compute the JNZD trip count = OldIV_initial_value - 1.
  // OldIV's entry predecessor is WarmUp (set by updateOuterHeaderPHIs).
  // For a decrement loop starting at N (the initial IV value), after peeling
  // one iteration the loop runs N-1 times, so the JNZD counter = N-1.
  BasicBlock *Preheader = LS.getPreheader();
  IRBuilder<> PreBuilder(Preheader->getTerminator());

  // OldIV is guaranteed non-null (getDowncountingInfo ensures this).
  Value *InitN = OldIV->getIncomingValueForBlock(WarmUp);
  Value *TripCount = PreBuilder.CreateSub(
      InitN, ConstantInt::get(InitN->getType(), 1), "outer.jnzd.tc");
  // Ensure i32.
  if (TripCount->getType() != I32Ty)
    TripCount =
        PreBuilder.CreateZExtOrTrunc(TripCount, I32Ty, "outer.jnzd.tc.i32");

  Value *CtrInit = PreBuilder.CreateIntrinsic(
      Intrinsic::start_loop_iterations, {I32Ty}, {TripCount},
      /*FMFSource=*/nullptr, "outer.ctr.init");

  // Insert counter PHI in the outer header.
  // The outer header now has WarmUp as its entry predecessor (set by
  // updateOuterHeaderPHIs). The back-edge comes from OuterLatch.
  Instruction *InsertPt = &*LS.OuterHeader->getFirstInsertionPt();
  PHINode *CtrPHI =
      PHINode::Create(I32Ty, 2, "outer.ctr", InsertPt->getIterator());
  CtrPHI->addIncoming(CtrInit, WarmUp);
  // The latch incoming value will be set after we create %ctr.next below.

  // Replace the latch icmp+add with loop.decrement.reg.
  // Use the pre-validated components from Info.
  BranchInst *LatchBr = LS.getLatchBranch();
  BinaryOperator *OldCounter = Info.Counter;

  IRBuilder<> LatchBuilder(LatchBr);
  Value *CtrNext =
      LatchBuilder.CreateIntrinsic(Intrinsic::loop_decrement_reg, {I32Ty},
                                   {CtrPHI, ConstantInt::get(I32Ty, 1)},
                                   /*FMFSource=*/nullptr, "outer.ctr.next");

  // The loop continues while CtrNext != 0.
  Value *NewCond = LatchBuilder.CreateICmpNE(
      CtrNext, ConstantInt::get(I32Ty, 0), "outer.loop.cond");

  // Update the branch: true successor must be the loop header.
  Value *OldCond = LatchBr->getCondition();
  LatchBr->setCondition(NewCond);

  // Ensure the true branch goes back to the outer header (loop continues).
  if (LatchBr->getSuccessor(0) != LS.OuterHeader)
    LatchBr->swapSuccessors();

  // Complete the counter PHI with the latch back-edge value.
  CtrPHI->addIncoming(CtrNext, LS.OuterLatch);

  // Delete the old icmp (now dead: branch condition was replaced above).
  RecursivelyDeleteTriviallyDeadInstructions(OldCond);

  // OldCounter (the add) and OldIV (the counting PHI) form a use cycle:
  //   OldIV  = phi [..., OldCounter, outer.latch]
  //   OldCounter = add OldIV, -1
  // RecursivelyDeleteTriviallyDeadInstructions cannot break this cycle because
  // neither is trivially dead in isolation.
  //
  // If OldIV is a pure counting PHI (only used by OldCounter), break the
  // cycle: replace OldCounter's latch slot in OldIV with PoisonValue, then
  // both become dead and are deleted.
  // If OldIV has other uses (e.g., address computation), leave the PHI alive.
  //
  // NOTE: This conversion runs BEFORE peelLastIterationEpilogue.  That
  // ordering is critical: in EpilogueInLatch mode the peel step clones all
  // instructions currently in the outer latch.  By erasing OldCounter and
  // OldIV here first, the peel step never sees them and never creates live
  // cooldown clones that would keep OldCounter alive.
  if (OldCounter) {
    // If OldIV is a pure counting PHI (no uses outside OldCounter), break
    // the cycle so both the PHI and the add can be deleted.
    if (OldIV->hasOneUse()) {
      int LatchIdx = OldIV->getBasicBlockIndex(LS.OuterLatch);
      if (LatchIdx >= 0)
        OldIV->setIncomingValue(LatchIdx, PoisonValue::get(OldIV->getType()));
    }
    // RecursivelyDeleteTriviallyDeadInstructions(OldCounter) will also delete
    // OldIV transitively if OldIV becomes dead (0 users) after OldCounter is
    // removed.  Do NOT reference OldIV after this call — it may be a dangling
    // pointer.
    RecursivelyDeleteTriviallyDeadInstructions(OldCounter);
  }

  LLVM_DEBUG(dbgs() << "    Converted outer loop to JNZD hardware loop\n");
}
