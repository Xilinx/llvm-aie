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
#include "Utils/AIELoopOptionOverrides.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
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

// True for @llvm.set.loop.iterations / @llvm.start.loop.iterations: the
// hardware-loop setup calls that must stay in the loop (the inner preheader)
// and be cloned only into the cool-down — never pipelined into the prefetch
// sites.
static bool isHardwareLoopSetup(const Instruction *I) {
  const auto *Call = dyn_cast<CallInst>(I);
  if (!Call)
    return false;
  const auto *Fn = Call->getCalledFunction();
  if (!Fn)
    return false;
  Intrinsic::ID IID = Fn->getIntrinsicID();
  return IID == Intrinsic::set_loop_iterations ||
         IID == Intrinsic::start_loop_iterations;
}

namespace {

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
// The prologue is a single-entry/single-exit (SESE) region of blocks between
// the outer header (region entry) and the inner preheader (region exit). For
// the common linear loop this region is just {OuterHeader} and
// OuterHeader == InnerPreheader; for a guarded prologue it is a diamond
// (e.g. outer.header → if.then → if.end). The epilogue stays a single block
// (the outer latch).
struct LoopStructure {
  Loop *OuterLoop;
  Loop *InnerLoop;
  BasicBlock *OuterHeader; // Prologue region entry
  BasicBlock *OuterLatch;  // Contains epilogue instructions
  BasicBlock *InnerPreheader;
  BasicBlock *InnerHeader;
  BasicBlock *InnerLatch;
  BasicBlock *InnerExit;

  // The prologue region in program order: entry (OuterHeader) first, exit
  // (InnerPreheader) last. Populated by analyzeLoopStructure once the region is
  // validated. A single-element vector is the degenerate linear case.
  SmallVector<BasicBlock *, 4> PrologueRegion;

  // The epilogue blocks in program order, ending at the latch (back-branch).
  // Defaults to {OuterLatch}; clonePrologueIntoEpilogue widens it when it
  // splits the latch to host the prefetch diamond.
  SmallVector<BasicBlock *, 4> EpilogueRegion;

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
        InnerExit(InnerLoop->getExitBlock()) {
    assert(L->getSubLoops().size() == 1 && "Requires exactly one subloop");
    assert(OuterLatch && "Requires single outer latch");
  }

  bool isInPrologue(const Instruction *I) const {
    return llvm::is_contained(PrologueRegion, I->getParent());
  }

  BasicBlock *getPrologueExit() const { return InnerPreheader; }

  bool hasMultiBlockPrologue() const { return PrologueRegion.size() > 1; }

  // A region-internal merge PHI merges values produced inside the prologue
  // region (every incoming block is a region block). It is pipelined with the
  // region. A loop-carried PHI in the entry instead has incoming edges from the
  // preheader and latch (outside the region) and is resolved to a concrete
  // value when cloning, never cloned as a PHI.
  bool isRegionInternalPhi(const PHINode *PN) const {
    for (const BasicBlock *IB : PN->blocks())
      if (!llvm::is_contained(PrologueRegion, IB))
        return false;
    return true;
  }

  // A value is pipelineable when it lives in the region and is a plain
  // instruction or a region-internal merge PHI. Loop-carried PHIs are excluded:
  // they resolve to concrete values via the clone VMap rather than being
  // pipelined.
  bool isPipelineableValue(const Instruction *I) const {
    if (!isInPrologue(I))
      return false;
    if (const auto *PN = dyn_cast<PHINode>(I))
      return isRegionInternalPhi(PN);
    return true;
  }

  // Returns true if I is in the epilogue block (outer latch).
  bool isInEpilogue(const Instruction *I) const {
    return I->getParent() == OuterLatch;
  }

  // Returns the outer loop preheader.
  BasicBlock *getPreheader() const { return OuterLoop->getLoopPreheader(); }

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

  // Clone the prologue region's Part-1 instructions (PInsts) as a parallel
  // block subgraph that preserves the region's internal control flow. VMap must
  // be pre-seeded with loop-carried PHI -> concrete value entries. New blocks
  // are inserted before InsertBefore in region order and appended to NewBlocks.
  // The exit clone's terminator is NOT created (the caller wires it). Returns
  // {entryClone, exitClone}. For a single-block region this is one block.
  std::pair<BasicBlock *, BasicBlock *>
  cloneRegionSubgraph(const LoopStructure &LS,
                      const SmallVectorImpl<Instruction *> &PInsts,
                      ValueToValueMapTy &VMap, const Twine &BlockSuffix,
                      const Twine &InstSuffix, BasicBlock *InsertBefore,
                      SmallVectorImpl<BasicBlock *> &NewBlocks);

  void cloneInstChainBefore(Instruction *InsertBefore,
                            const SmallVectorImpl<Instruction *> &PInsts,
                            ValueToValueMapTy &VMap, const Twine &InstSuffix);

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
  //   cooldown.entry: set.loop.iterations (cloned) + Part2Insts clones
  //   inner loop clone: uses last epilogue load values + Part2 results
  //   cooldown.exit: epilogue stores only (no loads, no prologue clones)
  // Redirects the outer latch's false branch to cooldown.entry.
  // OrigEpiInsts: the set of instructions that were in the epilogue block
  // BEFORE clonePrologueIntoEpilogue inserted the prologue load clones.
  // Part2Insts: 2048-bit producing intrinsics and their descendants that must
  // be cloned into cooldown.entry so the cloned inner loop has correct initial
  // accumulator values. Empty when not in split-prologue mode.
  void
  peelLastIterationEpilogue(const LoopStructure &LS,
                            const SmallPtrSetImpl<Instruction *> &OrigEpiInsts,
                            const SmallVectorImpl<Instruction *> &Part2Insts);

  // Step helpers of peelLastIterationEpilogue, in call order.
  BasicBlock *getLatchExitSuccessor(const LoopStructure &LS) const;
  void cloneHardwareLoopSetupInto(BasicBlock *Dest, const LoopStructure &LS,
                                  ValueToValueMapTy &CoolVMap) const;
  BasicBlock *
  createCooldownSkeleton(const LoopStructure &LS, BasicBlock *CoolEntry,
                         BasicBlock *OrigExit,
                         SmallVectorImpl<BasicBlock *> &ClonedInnerBlocks,
                         ValueToValueMapTy &CoolVMap) const;
  void
  clonePart2InstructionsInto(BasicBlock *Dest,
                             const SmallVectorImpl<Instruction *> &Part2Insts,
                             ValueToValueMapTy &CoolVMap) const;
  void
  cloneInnerLoopInto(const LoopStructure &LS, BasicBlock *CoolEntry,
                     const SmallVectorImpl<BasicBlock *> &ClonedInnerBlocks,
                     ValueToValueMapTy &CoolVMap) const;
  void populateCooldownExit(BasicBlock *CoolExit, const LoopStructure &LS,
                            const SmallPtrSetImpl<Instruction *> &OrigEpiInsts,
                            ValueToValueMapTy &CoolVMap) const;
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
  void collectPart2Instructions(const LoopStructure &LS,
                                const SmallPtrSetImpl<Instruction *> &Part1Set,
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
        llvm::is_contained(LS.PrologueRegion, BB))
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

// Discover and validate the prologue region; only the linear case is accepted.
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

static void forEachRegionInstruction(const LoopStructure &LS,
                                     function_ref<void(Instruction *)> Visit) {
  for (Instruction &I : *LS.OuterHeader)
    Visit(&I);
}

// Iterate the region's pipelining candidates: drops terminators, hardware-loop
// setup, and loop-carried PHIs; passes region-internal merge PHIs through.
static void
forEachRegionPipelineCandidate(const LoopStructure &LS,
                               function_ref<void(Instruction *)> Keep) {
  forEachRegionInstruction(LS, [&](Instruction *I) {
    if (I->isTerminator() || isHardwareLoopSetup(I))
      return;
    if (isa<PHINode>(I))
      return;
    Keep(I);
  });
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
  forEachRegionPipelineCandidate(LS, [&](Instruction *I) {
    if (Visited.count(I))
      Out.push_back(I);
  });
}

// Build a parallel subgraph for the region; the exit clone's terminator is
// left for the caller to wire.
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

  // Clone the Part-1 instructions into their block clone.
  for (Instruction *I : PInsts) {
    Instruction *Clone = I->clone();
    if (!Clone->getType()->isVoidTy())
      Clone->setName(I->getName() + InstSuffix);
    Clone->insertInto(CB, CB->end());
    VMap[I] = Clone;
  }

  // Remap operands to the clones.
  for (BasicBlock *NB : NewBlocks)
    for (Instruction &I : *NB)
      RemapInstruction(&I, VMap,
                       RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);

  return {CB, CB};
}

// Clone the flat Part-1 instruction chain immediately before InsertBefore,
// recording the clones in VMap; remap after inserting all of them (avoids
// iterator invalidation).
void AIEOuterLoopPipeliner::cloneInstChainBefore(
    Instruction *InsertBefore, const SmallVectorImpl<Instruction *> &PInsts,
    ValueToValueMapTy &VMap, const Twine &InstSuffix) {
  SmallVector<Instruction *, 16> Clones;
  for (Instruction *I : PInsts) {
    Instruction *Clone = I->clone();
    if (!Clone->getType()->isVoidTy())
      Clone->setName(I->getName() + InstSuffix);
    Clone->insertBefore(InsertBefore->getIterator());
    VMap[I] = Clone;
    Clones.push_back(Clone);
  }
  for (Instruction *Clone : Clones)
    RemapInstruction(Clone, VMap,
                     RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);
}

// Clone the data-load chain into a warm-up region inserted between the
// preheader and the outer header, using the entry (preheader) pointer values.
// Returns the warm-up exit block, which becomes the header's new
// preheader-side predecessor.
BasicBlock *AIEOuterLoopPipeliner::clonePrologueAsWarmUp(
    const LoopStructure &LS, const SmallVectorImpl<Instruction *> &PInsts,
    ValueToValueMapTy &WarmUpVMap) {
  BasicBlock *Preheader = LS.getPreheader();

  // Pre-populate WarmUpVMap with the initial (preheader) values of outer header
  // PHIs so cloned loads use the entry pointer values.
  populateVMapFromPHIs(WarmUpVMap, LS, Preheader);

  SmallVector<BasicBlock *, 4> NewBlocks;
  auto [EntryClone, ExitClone] = cloneRegionSubgraph(
      LS, PInsts, WarmUpVMap, ".peel.pro", ".peel", LS.OuterHeader, NewBlocks);

  // The warm-up region flows into the outer header; the preheader now flows
  // into the warm-up region entry.
  BranchInst::Create(LS.OuterHeader, ExitClone);
  Preheader->getTerminator()->replaceSuccessorWith(LS.OuterHeader, EntryClone);
  LLVM_DEBUG(dbgs() << "    Created warm-up region: " << NewBlocks.size()
                    << " block(s)\n");
  return ExitClone;
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
  cloneInstChainBefore(LS.OuterLatch->getTerminator(), PInsts, EpiVMap, ".epi");
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
    const SmallVectorImpl<Instruction *> &Part2Insts) {
  Function *F = LS.OuterHeader->getParent();
  LLVMContext &Ctx = F->getContext();

  BasicBlock *OrigExit = getLatchExitSuccessor(LS);

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

  SmallVector<BasicBlock *, 8> ClonedInnerBlocks;
  BasicBlock *CoolExit = createCooldownSkeleton(LS, CoolEntry, OrigExit,
                                                ClonedInnerBlocks, CoolVMap);

  clonePart2InstructionsInto(CoolEntry, Part2Insts, CoolVMap);
  cloneInnerLoopInto(LS, CoolEntry, ClonedInnerBlocks, CoolVMap);
  populateCooldownExit(CoolExit, LS, OrigEpiInsts, CoolVMap);
  wireCooldownIntoCFG(LS, CoolEntry, CoolExit, OrigExit);

  LLVM_DEBUG(dbgs() << "    Created cool-down: " << CoolEntry->getName()
                    << " -> " << CoolExit->getName() << "\n");
}

// The outer latch's non-header successor: the loop's original exit block.
BasicBlock *
AIEOuterLoopPipeliner::getLatchExitSuccessor(const LoopStructure &LS) const {
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

// Clone the hardware-loop setup (set.loop.iterations) from the outer header
// into Dest, remapping operands through CoolVMap.
void AIEOuterLoopPipeliner::cloneHardwareLoopSetupInto(
    BasicBlock *Dest, const LoopStructure &LS,
    ValueToValueMapTy &CoolVMap) const {
  for (Instruction &I : *LS.OuterHeader) {
    if (I.isTerminator())
      break;
    if (!isHardwareLoopSetup(&I))
      continue;
    Instruction *Clone = I.clone();
    Clone->insertInto(Dest, Dest->end());
    RemapInstruction(Clone, CoolVMap,
                     RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);
    CoolVMap[&I] = Clone;
  }
}

// Create empty cool-down blocks (one per inner-loop block plus cooldown.exit)
// and register the block remaps; returns cooldown.exit.
BasicBlock *AIEOuterLoopPipeliner::createCooldownSkeleton(
    const LoopStructure &LS, BasicBlock *CoolEntry, BasicBlock *OrigExit,
    SmallVectorImpl<BasicBlock *> &ClonedInnerBlocks,
    ValueToValueMapTy &CoolVMap) const {
  Function *F = LS.OuterHeader->getParent();
  LLVMContext &Ctx = F->getContext();

  // One empty clone per inner-loop block, in block_begin..block_end order so it
  // aligns with cloneInnerLoopInto's re-derived block list.
  for (BasicBlock *BB : LS.InnerLoop->blocks()) {
    BasicBlock *ClonedBB =
        BasicBlock::Create(Ctx, BB->getName() + ".cd", F, OrigExit);
    CoolVMap[BB] = ClonedBB;
    ClonedInnerBlocks.push_back(ClonedBB);
  }

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

// Clone Part-2 instructions into Dest. Must run before cloneInnerLoopInto so
// the cloned inner-loop PHIs that reference Part-2 results resolve to them.
void AIEOuterLoopPipeliner::clonePart2InstructionsInto(
    BasicBlock *Dest, const SmallVectorImpl<Instruction *> &Part2Insts,
    ValueToValueMapTy &CoolVMap) const {
  SmallVector<Instruction *, 16> Part2Clones;
  for (Instruction *I : Part2Insts) {
    Instruction *Clone = I->clone();
    if (!Clone->getType()->isVoidTy())
      Clone->setName(I->getName() + ".cd");
    Clone->insertInto(Dest, Dest->end());
    CoolVMap[I] = Clone;
    Part2Clones.push_back(Clone);
  }
  for (Instruction *Clone : Part2Clones)
    RemapInstruction(Clone, CoolVMap,
                     RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);
}

// Clone the inner-loop bodies into the skeleton blocks and branch
// cooldown.entry into the cloned inner-loop header.
void AIEOuterLoopPipeliner::cloneInnerLoopInto(
    const LoopStructure &LS, BasicBlock *CoolEntry,
    const SmallVectorImpl<BasicBlock *> &ClonedInnerBlocks,
    ValueToValueMapTy &CoolVMap) const {
  SmallVector<BasicBlock *, 8> InnerBlocks(LS.InnerLoop->block_begin(),
                                           LS.InnerLoop->block_end());
  for (unsigned I = 0; I < InnerBlocks.size(); ++I) {
    BasicBlock *Orig = InnerBlocks[I];
    BasicBlock *Clone = ClonedInnerBlocks[I];
    for (Instruction &Inst : *Orig) {
      Instruction *CloneI = Inst.clone();
      if (!CloneI->getType()->isVoidTy())
        CloneI->setName(Inst.getName() + ".cd");
      CloneI->insertInto(Clone, Clone->end());
      CoolVMap[&Inst] = CloneI;
    }
  }
  for (BasicBlock *BB : ClonedInnerBlocks)
    for (Instruction &Inst : *BB)
      RemapInstruction(&Inst, CoolVMap,
                       RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);

  BasicBlock *ClonedInnerHeader = cast<BasicBlock>(CoolVMap[LS.InnerHeader]);
  BranchInst::Create(ClonedInnerHeader, CoolEntry);
}

// Populate cooldown.exit with the original epilogue stores only — no prefetch
// loads, and none of the prologue clones inserted into the epilogue earlier
// (OrigEpiInsts is the snapshot taken before those clones were inserted).
void AIEOuterLoopPipeliner::populateCooldownExit(
    BasicBlock *CoolExit, const LoopStructure &LS,
    const SmallPtrSetImpl<Instruction *> &OrigEpiInsts,
    ValueToValueMapTy &CoolVMap) const {
  SmallVector<Instruction *, 16> EpiClones;
  for (Instruction &I : *LS.OuterLatch) {
    if (I.isTerminator())
      break;
    if (isa<LoadInst>(&I))
      continue; // no prefetch in cool-down
    if (!OrigEpiInsts.count(&I))
      continue; // skip prologue clones inserted by clonePrologueIntoEpilogue
    Instruction *Clone = I.clone();
    if (!Clone->getType()->isVoidTy())
      Clone->setName(I.getName() + ".cd");
    Clone->insertInto(CoolExit, CoolExit->end());
    CoolVMap[&I] = Clone;
    EpiClones.push_back(Clone);
  }
  for (Instruction *Clone : EpiClones)
    RemapInstruction(Clone, CoolVMap,
                     RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);
}

// Splice the cool-down into the CFG: cooldown.exit -> original exit, repoint
// the exit's latch-predecessor PHIs to cooldown.exit, and redirect the latch's
// exit edge to cooldown.entry.
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
  MDNode *LoopID = LS.OuterLoop->getLoopID();
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
  LS.OuterLoop->setLoopID(FinalLoopID);
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
  forEachRegionInstruction(LS, [&](Instruction *I) {
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
  SmallPtrSet<Instruction *, 32> Part2Set;
  Part2Set.insert(Anchors.begin(), Anchors.end());
  SmallVector<Instruction *, 16> DescWorklist(Anchors.begin(), Anchors.end());
  while (!DescWorklist.empty()) {
    Instruction *I = DescWorklist.pop_back_val();
    for (User *U : I->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      if (!UI || isa<PHINode>(UI) || !LS.isInPrologue(UI))
        continue;
      if (Part2Set.insert(UI).second)
        DescWorklist.push_back(UI);
    }
  }

  // Part 1 = region pipeline candidates not in Part 2 (the load/address chain;
  // the post-anchor cone stays in Part 2). Loop-carried PHIs, terminators, and
  // hardware-loop setup are excluded by forEachRegionPipelineCandidate.
  forEachRegionPipelineCandidate(LS, [&](Instruction *I) {
    if (!Part2Set.count(I))
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

void AIEOuterLoopPipeliner::collectPart2Instructions(
    const LoopStructure &LS, const SmallPtrSetImpl<Instruction *> &Part1Set,
    SmallVectorImpl<Instruction *> &Out) const {
  // Find anchors: instructions matching any split strategy that are direct
  // users of Part 1 instructions (or transitively reachable from Part 1 within
  // the prologue).
  SmallPtrSet<Instruction *, 32> Part2Set;
  SmallVector<Instruction *, 16> Worklist;

  // Seed: forward-track from Part 1 instructions to find anchor instructions.
  for (Instruction *P1 : Part1Set) {
    for (User *U : P1->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      if (!UI || isa<PHINode>(UI) || !LS.isInPrologue(UI))
        continue;
      if (!Part1Set.count(UI) && isAnchorInstruction(UI)) {
        if (Part2Set.insert(UI).second)
          Worklist.push_back(UI);
      }
    }
  }

  // Forward-track from anchors to collect all descendants within the prologue.
  while (!Worklist.empty()) {
    Instruction *I = Worklist.pop_back_val();
    for (User *U : I->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      if (!UI || isa<PHINode>(UI) || !LS.isInPrologue(UI))
        continue;
      if (!Part1Set.count(UI) && Part2Set.insert(UI).second)
        Worklist.push_back(UI);
    }
  }

  // Emit Part2Set in region program order.
  forEachRegionInstruction(LS, [&](Instruction *I) {
    if (Part2Set.count(I))
      Out.push_back(I);
  });

  LLVM_DEBUG(dbgs() << "    Split-prologue: " << Out.size()
                    << " Part-2 instructions (stay in outer.header + "
                       "cooldown.entry)\n");
}

bool AIEOuterLoopPipeliner::performTransformation(
    LoopStructure &LS, const AIE::LoopOptionOverrides &Overrides) {
  BasicBlock *Preheader = LS.getPreheader();

  // Lift pointer update instructions from epilogue to prologue.
  // This must happen BEFORE collecting prologue instructions so that the
  // lifted instructions are included in the data-load chain.
  liftEpiloguePointerUpdatesToPrologue(LS);

  // Collect the data-load chain instructions from the prologue region.
  // Hardware-loop setup calls (set.loop.iterations) are excluded.
  // Split-prologue keeps the 2048-bit producers in the branch-free steady
  // header and clones them into cool-down; otherwise the whole region is
  // pipelined. collectPrologueInstructionsForSplit returns false when there is
  // no split point, requesting the whole-region path.
  SmallVector<Instruction *, 16> PInsts;
  bool SplitApplied = false;
  if (Overrides.get(SplitPrologue) &&
      collectPrologueInstructionsForSplit(LS, PInsts)) {
    SplitApplied = true;
    LLVM_DEBUG(dbgs() << "    Split-prologue: pipelining " << PInsts.size()
                      << " Part-1 instructions\n");
  } else {
    collectPrologueInstructions(LS, PInsts);
  }
  if (PInsts.empty()) {
    LLVM_DEBUG(dbgs() << "    No prologue instructions found\n");
    return false;
  }

  // Snapshot original epilogue instructions BEFORE prologue clones are
  // inserted into the epilogue block. This set is used later by
  // peelLastIterationEpilogue to filter out the prologue clones from
  // cooldown.exit. Epilogue is always in OuterLatch (linear structure).
  SmallPtrSet<Instruction *, 32> OrigEpiInsts;
  for (Instruction &I : *LS.OuterLatch)
    OrigEpiInsts.insert(&I);

  // Clone data-load chain into a warm-up block (before the outer loop).
  // The warm-up uses the entry pointer values (PHI initial values).
  ValueToValueMapTy WarmUpVMap;
  BasicBlock *WarmUp = clonePrologueAsWarmUp(LS, PInsts, WarmUpVMap);

  // Clone data-load chain into the epilogue (outer latch).
  // The clones use the NEXT-iteration pointer values (latch incoming values).
  ValueToValueMapTy EpiVMap;
  clonePrologueIntoEpilogue(LS, PInsts, EpiVMap);

  // Update outer header PHI predecessors: preheader -> warm-up.
  updateOuterHeaderPHIs(LS, WarmUp, Preheader);

  // Collect Part 2 instructions (split-prologue mode only) BEFORE
  // createPipelinedPHIs erases the Part 1 instructions from outer.header.
  // Part 2 = matched producing intrinsics + descendants that stay in
  // outer.header and must also be cloned into cooldown.entry.
  // Must be done while Part 1 instructions still exist for forward-tracking.
  SmallVector<Instruction *, 16> Part2Insts;
  if (SplitApplied) {
    SmallPtrSet<Instruction *, 32> Part1Set;
    Part1Set.insert(PInsts.begin(), PInsts.end());
    collectPart2Instructions(LS, Part1Set, Part2Insts);
  }

  // Create pipelined PHI nodes in the outer header for each data-load
  // value. Replace uses of Part 1 instructions with PHI nodes, erase originals.
  // After this step, Part 2 instructions in outer.header automatically use the
  // new PHI nodes (via replaceAllUsesWith), so cloning them into cooldown.entry
  // with CoolVMap will correctly use the last epilogue values.
  createPipelinedPHIs(LS, WarmUp, PInsts, WarmUpVMap, EpiVMap);

  // Adjust the outer loop trip count from N to N-1.
  // Must happen before peelLastIterationEpilogue so that the hardware loop
  // conversion can find the right icmp to replace.
  Value *AdjustedTripCount = adjustLoopBound(LS);

  // Optionally convert the outer loop to a JNZD hardware loop.
  //
  // IMPORTANT: This MUST run before peelLastIterationEpilogue.
  // In EpilogueInLatch mode the outer latch is also the epilogue block.
  // peelLastIterationEpilogue clones all original epilogue instructions into
  // cooldown.exit, which would include OldCounter (the counting `add`) and
  // OldCond (the `icmp`). Those clones would keep OldCounter alive and
  // prevent RecursivelyDeleteTriviallyDeadInstructions from cleaning up the
  // cycle.  By running convertOuterLoopToHardwareLoop first, we erase
  // OldCond, OldCounter, and the pure-counter PHI (OldIV) from the latch
  // BEFORE the peel step iterates over it, so they are never cloned.
  if (EnableOuterLoopHardwareLoop && AdjustedTripCount)
    if (auto Info = getDowncountingInfo(LS))
      convertOuterLoopToHardwareLoop(LS, WarmUp, AdjustedTripCount, *Info);

  // Create the cool-down region (peeled last iteration):
  //   set.loop.iterations + Part2 clones + inner loop clone + epilogue stores.
  // OrigEpiInsts ensures only original epilogue instructions are copied into
  // cooldown.exit (not the prologue clones inserted earlier).
  // Redirects outer latch false branch to cooldown.entry.
  peelLastIterationEpilogue(LS, OrigEpiInsts, Part2Insts);

  // Adjust itercount metadata to reflect the reduced trip count.
  updateLoopMetadata(LS);

  // Emit a remark with the chosen strategy and the prologue block count.
  OptimizationRemarkEmitter ORE(LS.OuterHeader->getParent());
  ORE.emit([&]() {
    return OptimizationRemark(DEBUG_TYPE, "pipelined",
                              LS.OuterHeader->getTerminator())
           << "pipelined outer loop "
           << ore::NV("Mode", StringRef(SplitApplied ? "split-prologue"
                                                     : "whole-region"))
           << ore::NV("PrologueBlocks",
                      static_cast<int64_t>(LS.PrologueRegion.size()));
  });

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
