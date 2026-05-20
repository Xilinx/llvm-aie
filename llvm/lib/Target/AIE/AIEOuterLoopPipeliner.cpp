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

// Loop structure for outer loop pipelining.
// We only support the linear structure:
//   outer.header (prologue) → inner loop → outer.latch (epilogue)
// where prologue instructions are in the outer header and epilogue
// instructions are in the outer latch (no separate blocks).
struct LoopStructure {
  Loop *OuterLoop;
  Loop *InnerLoop;
  BasicBlock *OuterHeader; // Contains prologue instructions
  BasicBlock *OuterLatch;  // Contains epilogue instructions
  BasicBlock *InnerPreheader;
  BasicBlock *InnerHeader;
  BasicBlock *InnerLatch;
  BasicBlock *InnerExit;

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

  // Returns true if I is in the prologue block (outer header).
  bool isInPrologue(const Instruction *I) const {
    return I->getParent() == OuterHeader;
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

  // Clone data-load chain into a warm-up block before the outer loop.
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

  // Adjust the outer loop trip count from N to N-1.
  // Returns the new limit Value (the adjusted trip count), or nullptr if the
  // bound could not be adjusted.
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
  void convertOuterLoopToHardwareLoop(const LoopStructure &LS,
                                      BasicBlock *WarmUp,
                                      Value *AdjustedTripCount);

  // Returns true if the outer latch has a downcounting icmp pattern that can
  // be replaced by @llvm.loop.decrement.reg.
  bool isOuterLoopDowncounting(const LoopStructure &LS) const;

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

  // We only support the linear structure:
  //   outer.header (prologue) → inner loop → outer.latch (epilogue)
  // Check that there are no separate prologue/epilogue blocks.
  for (BasicBlock *BB : L->blocks()) {
    if (LS.InnerLoop->contains(BB))
      continue;
    // BB is in the outer loop but not the inner loop.
    // It must be either the outer header or the outer latch.
    if (BB != LS.OuterHeader && BB != LS.OuterLatch) {
      LLVM_DEBUG(dbgs() << "    Separate prologue/epilogue blocks not "
                           "supported (found "
                        << BB->getName() << ")\n");
      return std::nullopt;
    }
  }

  // Validate: prologue is in outer header (inner preheader == outer header
  // or inner preheader is the unique successor of outer header).
  // For simplicity, we require inner preheader == outer header.
  if (LS.InnerPreheader != LS.OuterHeader) {
    LLVM_DEBUG(dbgs() << "    Inner preheader != outer header\n");
    return std::nullopt;
  }

  // Validate: epilogue is in outer latch (inner exit == outer latch).
  if (LS.InnerExit != LS.OuterLatch) {
    LLVM_DEBUG(dbgs() << "    Inner exit != outer latch\n");
    return std::nullopt;
  }

  LLVM_DEBUG(dbgs() << "    Linear structure: outer.header -> inner loop -> "
                       "outer.latch\n");
  return LS;
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
  // Prologue is always in OuterHeader (linear structure).
  SmallPtrSet<Instruction *, 32> Visited;
  SmallVector<Instruction *, 16> Worklist;
  auto Seed = [&](Value *V) {
    auto *I = dyn_cast<Instruction>(V);
    if (!I || isa<PHINode>(I) || I->getParent() != LS.OuterHeader)
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
      if (!OpI || isa<PHINode>(OpI) || OpI->getParent() != LS.OuterHeader)
        continue;
      if (Visited.insert(OpI).second)
        Worklist.push_back(OpI);
    }
  }
  // Emit in program order, excluding terminators.
  // Hardware-loop setup calls are excluded (they stay in the outer header).
  for (Instruction &I : *LS.OuterHeader) {
    if (isa<PHINode>(&I) || I.isTerminator())
      continue;
    if (!Visited.count(&I))
      continue;
    Out.push_back(&I);
  }
}

// Clone the data-load chain into a new warm-up block inserted between the
// preheader and the outer header. The warm-up block contains only the data
// loads (no set.loop.iterations). PHI initial values are used so that the
// cloned loads use the entry pointer values, not the PHI nodes.
BasicBlock *AIEOuterLoopPipeliner::clonePrologueAsWarmUp(
    const LoopStructure &LS, const SmallVectorImpl<Instruction *> &PInsts,
    ValueToValueMapTy &WarmUpVMap) {
  Function *F = LS.OuterHeader->getParent();
  BasicBlock *Preheader = LS.getPreheader();
  BasicBlock *WarmUp = BasicBlock::Create(
      F->getContext(), LS.OuterHeader->getName() + ".peel.pro", F,
      LS.OuterHeader);

  // Pre-populate WarmUpVMap with the initial (preheader) values of outer
  // header PHI nodes so that cloned loads use the entry pointer values.
  populateVMapFromPHIs(WarmUpVMap, LS, Preheader);

  for (Instruction *I : PInsts) {
    Instruction *Clone = I->clone();
    if (!Clone->getType()->isVoidTy())
      Clone->setName(I->getName() + ".peel");
    Clone->insertInto(WarmUp, WarmUp->end());
    WarmUpVMap[I] = Clone;
  }
  for (Instruction &I : *WarmUp)
    RemapInstruction(&I, WarmUpVMap,
                     RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);
  BranchInst::Create(LS.OuterHeader, WarmUp);
  Preheader->getTerminator()->replaceSuccessorWith(LS.OuterHeader, WarmUp);
  LLVM_DEBUG(dbgs() << "    Created warm-up: " << WarmUp->getName() << "\n");
  return WarmUp;
}

// Clone the data-load chain into the epilogue block (outer latch), inserting
// the clones before the terminator. The clones use the NEXT-iteration pointer
// values (the latch incoming values of the outer header PHIs) so that the
// loads prefetch data for the next outer iteration.
void AIEOuterLoopPipeliner::clonePrologueIntoEpilogue(
    const LoopStructure &LS, const SmallVectorImpl<Instruction *> &PInsts,
    ValueToValueMapTy &EpiVMap) {
  // Epilogue is always in OuterLatch (linear structure).
  Instruction *InsertBefore = LS.OuterLatch->getTerminator();

  // Pre-populate EpiVMap with the NEXT-iteration values of outer header PHIs.
  // This ensures that cloned loads use %a.ptr.next, %b.ptr.next, etc.
  populateVMapFromPHIs(EpiVMap, LS, LS.OuterLatch);

  SmallVector<Instruction *, 16> EpiClones;
  for (Instruction *I : PInsts) {
    Instruction *Clone = I->clone();
    if (!Clone->getType()->isVoidTy())
      Clone->setName(I->getName() + ".epi");
    Clone->insertBefore(InsertBefore->getIterator());
    EpiVMap[I] = Clone;
    EpiClones.push_back(Clone);
  }
  // Remap after all clones are inserted to avoid iterator invalidation.
  for (Instruction *Clone : EpiClones)
    RemapInstruction(Clone, EpiVMap,
                     RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);
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

  // Find the original exit block (false branch of outer latch).
  BranchInst *LatchBr = LS.getLatchBranch();
  assert(LatchBr->isConditional() && "Outer latch must be conditional");
  // The true branch goes back to outer.header; false branch goes to exit.
  BasicBlock *OrigExit = nullptr;
  for (unsigned I = 0; I < LatchBr->getNumSuccessors(); ++I) {
    BasicBlock *Succ = LatchBr->getSuccessor(I);
    if (Succ != LS.OuterHeader) {
      OrigExit = Succ;
      break;
    }
  }

  assert(OrigExit && "Outer latch must have an exit successor");

  // Build the cool-down VMap: map each outer header PHI to its latch value.
  // This gives us: %v0.phi -> %v0.epi, %c.ptr -> %c.ptr.next, etc.
  ValueToValueMapTy CoolVMap;
  populateVMapFromPHIs(CoolVMap, LS, LS.OuterLatch);

  // Create cooldown.entry.
  BasicBlock *CoolEntry =
      BasicBlock::Create(Ctx, "cooldown.entry", F, OrigExit);

  // Clone set.loop.iterations from the outer header into cooldown.entry.
  for (Instruction &I : *LS.OuterHeader) {
    if (I.isTerminator())
      break;
    auto *Call = dyn_cast<CallInst>(&I);
    if (!Call)
      continue;
    auto *Fn = Call->getCalledFunction();
    if (!Fn)
      continue;
    Intrinsic::ID IID = Fn->getIntrinsicID();
    if (IID != Intrinsic::set_loop_iterations &&
        IID != Intrinsic::start_loop_iterations)
      continue;
    Instruction *Clone = Call->clone();
    Clone->insertInto(CoolEntry, CoolEntry->end());
    RemapInstruction(Clone, CoolVMap,
                     RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);
    CoolVMap[&I] = Clone;
  }

  // Clone the inner loop blocks.
  // Collect inner loop blocks in RPO order.
  SmallVector<BasicBlock *, 8> InnerBlocks(LS.InnerLoop->block_begin(),
                                           LS.InnerLoop->block_end());

  // Create cloned blocks.
  SmallVector<BasicBlock *, 8> ClonedInnerBlocks;
  for (BasicBlock *BB : InnerBlocks) {
    BasicBlock *ClonedBB =
        BasicBlock::Create(Ctx, BB->getName() + ".cd", F, OrigExit);
    CoolVMap[BB] = ClonedBB;
    ClonedInnerBlocks.push_back(ClonedBB);
  }

  // Map outer.header -> cooldown.entry (for PHI incoming blocks in inner loop).
  CoolVMap[LS.OuterHeader] = CoolEntry;
  // Map outer.latch -> cooldown.exit (will be created below; use placeholder).
  // We'll fix this after creating cooldown.exit.

  // Create cooldown.exit.
  BasicBlock *CoolExit = BasicBlock::Create(Ctx, "cooldown.exit", F, OrigExit);
  CoolVMap[LS.OuterLatch] = CoolExit;
  // Also map inner exit (which is outer.latch in the single-block case).
  if (LS.InnerExit == LS.OuterLatch)
    CoolVMap[LS.InnerExit] = CoolExit;

  // Clone Part 2 instructions into cooldown.entry (split-prologue mode).
  // Part 2 = matched intrinsics + their descendants. They stay in
  // outer.header and must also appear in cooldown.entry so the cloned inner
  // loop has correct initial accumulator values.
  // IMPORTANT: This must happen BEFORE cloning inner loop instructions and
  // remapping, so that CoolVMap[Part2_inst] is set when the cloned inner loop
  // PHIs (which reference Part 2 results from outer.header) are remapped.
  // CoolVMap already maps Part 1 PHI nodes → their latch values (= epilogue
  // clones), so remapping Part 2 clones automatically uses the last epilogue
  // values for Part 1 operands.
  SmallVector<Instruction *, 16> Part2Clones;
  for (Instruction *I : Part2Insts) {
    Instruction *Clone = I->clone();
    if (!Clone->getType()->isVoidTy())
      Clone->setName(I->getName() + ".cd");
    Clone->insertInto(CoolEntry, CoolEntry->end());
    CoolVMap[I] = Clone;
    Part2Clones.push_back(Clone);
  }
  for (Instruction *Clone : Part2Clones)
    RemapInstruction(Clone, CoolVMap,
                     RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);

  // Clone instructions into inner loop clone blocks.
  for (unsigned I = 0; I < InnerBlocks.size(); ++I) {
    BasicBlock *Orig = InnerBlocks[I];
    BasicBlock *Clone = ClonedInnerBlocks[I];
    for (Instruction &I : *Orig) {
      Instruction *CloneI = I.clone();
      if (!CloneI->getType()->isVoidTy())
        CloneI->setName(I.getName() + ".cd");
      CloneI->insertInto(Clone, Clone->end());
      CoolVMap[&I] = CloneI;
    }
  }

  // Remap all cloned inner loop instructions.
  // CoolVMap now contains Part 2 entries, so inner loop PHIs that reference
  // Part 2 results from outer.header will correctly use the cooldown.entry
  // Part 2 clones.
  for (BasicBlock *BB : ClonedInnerBlocks)
    for (Instruction &I : *BB)
      RemapInstruction(&I, CoolVMap,
                       RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);

  // Branch from cooldown.entry to the cloned inner loop header.
  BasicBlock *ClonedInnerHeader = cast<BasicBlock>(CoolVMap[LS.InnerHeader]);
  BranchInst::Create(ClonedInnerHeader, CoolEntry);

  // Populate cooldown.exit with epilogue stores (no loads, no prologue
  // clones). Only copy instructions that were in the original epilogue block
  // BEFORE clonePrologueIntoEpilogue inserted the prologue load clones.
  // Epilogue is always in OuterLatch (linear structure).
  SmallVector<Instruction *, 16> EpiClones;
  for (Instruction &I : *LS.OuterLatch) {
    if (I.isTerminator())
      break;
    if (isa<LoadInst>(&I))
      continue; // Skip loads -- no prefetch in cool-down
    if (!OrigEpiInsts.count(&I))
      continue; // Skip prologue clones inserted by clonePrologueIntoEpilogue
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

  // Branch from cooldown.exit to the original exit.
  BranchInst::Create(OrigExit, CoolExit);

  // Update PHI nodes in OrigExit: replace outer.latch predecessor with
  // cooldown.exit.
  for (PHINode &PHI : OrigExit->phis()) {
    const int LatchIdx = PHI.getBasicBlockIndex(LS.OuterLatch);
    assert(LatchIdx >= 0);
    PHI.setIncomingBlock(LatchIdx, CoolExit);
  }

  // Redirect the outer latch's false branch to cooldown.entry.
  for (unsigned I = 0; I < LatchBr->getNumSuccessors(); ++I) {
    if (LatchBr->getSuccessor(I) == OrigExit) {
      LatchBr->setSuccessor(I, CoolEntry);
      break;
    }
  }

  LLVM_DEBUG(dbgs() << "    Created cool-down: " << CoolEntry->getName()
                    << " -> " << CoolExit->getName() << "\n");
}

// Adjust the outer loop trip count from N to N-1.
//
// Two canonical forms are handled:
//
//  Increment loop:  icmp slt/ult %counter, %N
//    → decrement limit: %N - 1
//
//  Decrement loop:  icmp eq %counter, 0   (counter = phi - 1)
//    → increment limit: 0 + 1 = 1
//    (loop exits when counter reaches 1 instead of 0)
//
// For icmp eq/ne we detect the counter direction by inspecting the add
// instruction that produces the non-invariant operand.
//
// Returns the new limit Value (the adjusted trip count).
Value *AIEOuterLoopPipeliner::adjustLoopBound(const LoopStructure &LS) {
  BranchInst *BI = dyn_cast<BranchInst>(LS.OuterLatch->getTerminator());

  assert(BI->isConditional() && "Outer latch must have conditional branch");

  ICmpInst *Cmp = dyn_cast<ICmpInst>(BI->getCondition());
  assert(Cmp && "Outer latch condition must be icmp");

  // Find the loop-invariant limit operand and the non-invariant counter.
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

  assert(Limit &&
         "Loop exit condition must have a loop-invariant limit operand");

  // Determine whether to increment or decrement the limit.
  //   Increment loop (icmp slt/ult %i, N)  → decrement limit (N-1)
  //   Decrement loop (icmp eq %i, 0)       → increment limit (0+1=1)
  //   icmp sgt/ugt                         → increment limit
  ICmpInst::Predicate Pred = Cmp->getPredicate();
  bool IncrementLimit = false;

  if (Pred == ICmpInst::ICMP_EQ || Pred == ICmpInst::ICMP_NE) {
    // Detect counter direction from the add that produces the counter value.
    if (auto *Add = dyn_cast<BinaryOperator>(Counter)) {
      if (Add->getOpcode() == Instruction::Add) {
        if (auto *C = dyn_cast<ConstantInt>(Add->getOperand(1)))
          IncrementLimit =
              C->isNegative(); // decrement counter → increment limit
      }
    }
  } else if (Pred == ICmpInst::ICMP_SGT || Pred == ICmpInst::ICMP_UGT) {
    IncrementLimit = true;
  }
  // ICMP_SLT / ICMP_ULT: decrement limit (IncrementLimit = false, default)

  IRBuilder<> Builder(LS.getPreheader()->getTerminator());
  Value *NewLimit;
  if (IncrementLimit) {
    NewLimit = Builder.CreateAdd(Limit, ConstantInt::get(Limit->getType(), 1),
                                 "outer.trip.plus1");
  } else {
    NewLimit = Builder.CreateSub(Limit, ConstantInt::get(Limit->getType(), 1),
                                 "outer.trip.minus1");
  }
  Cmp->setOperand(LimitIdx, NewLimit);
  LLVM_DEBUG(dbgs() << "    Adjusted loop bound: N -> N-1\n");
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

  for (Instruction &I : *LS.OuterHeader)
    if (isa<LoadInst>(&I)) {
      ReachableFromLoad.insert(&I);
      FwdWorklist.push_back(&I);
    }

  while (!FwdWorklist.empty()) {
    Instruction *I = FwdWorklist.pop_back_val();
    for (User *U : I->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      if (!UI || isa<PHINode>(UI) || !LS.isInPrologue(UI))
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

  // Part 1 = all prologue instructions that are NOT in Part 2 and NOT
  // set.loop.iterations. This naturally includes loads, address computation
  // (GEPs, addrspacecasts, add.3d, extractvalues, truncs, zexts), ups
  // conversions, bitcasts, shufflevectors — everything except matched
  // intrinsics producing chains.
  for (Instruction &I : *LS.OuterHeader) {
    if (isa<PHINode>(&I) || I.isTerminator())
      continue;
    if (Part2Set.count(&I))
      continue;
    // Exclude set.loop.iterations / start.loop.iterations — they stay in
    // outer.header and are cloned separately into cooldown.entry.
    if (auto *Call = dyn_cast<CallInst>(&I)) {
      if (auto *Fn = Call->getCalledFunction()) {
        Intrinsic::ID IID = Fn->getIntrinsicID();
        if (IID == Intrinsic::set_loop_iterations ||
            IID == Intrinsic::start_loop_iterations)
          continue;
      }
    }
    Out.push_back(&I);
  }

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

  // Emit Part2Set in program order.
  for (Instruction &I : *LS.OuterHeader)
    if (Part2Set.count(&I))
      Out.push_back(&I);

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

  // Collect the data-load chain instructions from the outer header.
  // Hardware-loop setup calls (set.loop.iterations) are excluded.
  // In split-prologue mode, we split the in the prologue in two parts
  SmallVector<Instruction *, 16> PInsts;
  if (Overrides.get(SplitPrologue)) {
    if (!collectPrologueInstructionsForSplit(LS, PInsts)) {
      LLVM_DEBUG(dbgs() << "    Split-prologue: no split points\n");
      collectPrologueInstructions(LS, PInsts);
    } else {
      LLVM_DEBUG(dbgs() << "    Split-prologue: pipelining " << PInsts.size()
                        << " Part-1 instructions\n");
    }
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
  if (Overrides.get(SplitPrologue) && !PInsts.empty()) {
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
  if (EnableOuterLoopHardwareLoop && AdjustedTripCount &&
      isOuterLoopDowncounting(LS))
    convertOuterLoopToHardwareLoop(LS, WarmUp, AdjustedTripCount);

  // Create the cool-down region (peeled last iteration):
  //   set.loop.iterations + Part2 clones + inner loop clone + epilogue stores.
  // OrigEpiInsts ensures only original epilogue instructions are copied into
  // cooldown.exit (not the prologue clones inserted earlier).
  // Redirects outer latch false branch to cooldown.entry.
  peelLastIterationEpilogue(LS, OrigEpiInsts, Part2Insts);

  // Adjust itercount metadata to reflect the reduced trip count.
  updateLoopMetadata(LS);

  return true;
}

// Returns true if the outer latch has a downcounting icmp pattern:
//   %counter = add i32 %phi, -1
//   %cond    = icmp eq i32 %counter, <limit>
// This is the pattern that can be replaced by @llvm.loop.decrement.reg.
bool AIEOuterLoopPipeliner::isOuterLoopDowncounting(
    const LoopStructure &LS) const {
  auto *BI = dyn_cast<BranchInst>(LS.OuterLatch->getTerminator());
  if (!BI || !BI->isConditional())
    return false;
  auto *Cmp = dyn_cast<ICmpInst>(BI->getCondition());
  if (!Cmp)
    return false;
  // We need icmp eq or icmp ne (the canonical downcounting exit condition).
  ICmpInst::Predicate Pred = Cmp->getPredicate();
  if (Pred != ICmpInst::ICMP_EQ && Pred != ICmpInst::ICMP_NE)
    return false;
  // Find the non-invariant operand (the counter).
  Value *Counter = nullptr;
  for (unsigned I = 0; I < 2; ++I) {
    if (!LS.OuterLoop->isLoopInvariant(Cmp->getOperand(I))) {
      Counter = Cmp->getOperand(I);
      break;
    }
  }
  if (!Counter)
    return false;
  // The counter must be produced by an add with a negative constant step.
  auto *Add = dyn_cast<BinaryOperator>(Counter);
  if (!Add || Add->getOpcode() != Instruction::Add)
    return false;
  auto *Step = dyn_cast<ConstantInt>(Add->getOperand(1));
  return Step && Step->isNegative();
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
    const LoopStructure &LS, BasicBlock *WarmUp, Value *AdjustedTripCount) {
  LLVMContext &Ctx = LS.OuterHeader->getContext();
  Type *I32Ty = Type::getInt32Ty(Ctx);

  // Find OldIV (the counting PHI feeding OldCounter) early.
  // We need OldIV to:
  //   (a) compute the correct JNZD trip count (OldIV's initial value - 1), and
  //   (b) clean up the OldIV/OldCounter use cycle after replacing the icmp.
  //
  // Note: AdjustedTripCount (returned by adjustLoopBound) is the new icmp
  // *threshold* (e.g., the constant 1 for a decrement loop), NOT the loop
  // trip count.  The actual trip count for JNZD is:
  //   OldIV_initial_value - 1
  // where OldIV_initial_value is the preheader/warm-up incoming of OldIV.
  BranchInst *LatchBrEarly = LS.getLatchBranch();
  ICmpInst *OldCmpEarly = cast<ICmpInst>(LatchBrEarly->getCondition());

  // Find the non-invariant counter operand of the icmp.
  Value *OldCounterEarly = nullptr;
  for (unsigned I = 0; I < 2; ++I) {
    if (!LS.OuterLoop->isLoopInvariant(OldCmpEarly->getOperand(I))) {
      OldCounterEarly = OldCmpEarly->getOperand(I);
      break;
    }
  }

  // Find the IV PHI in the outer header that feeds OldCounter.
  PHINode *OldIVEarly = nullptr;
  if (OldCounterEarly) {
    for (Value *Op : cast<Instruction>(OldCounterEarly)->operands()) {
      if (auto *PHI = dyn_cast<PHINode>(Op)) {
        if (PHI->getParent() == LS.OuterHeader) {
          OldIVEarly = PHI;
          break;
        }
      }
    }
  }

  // Compute the JNZD trip count = OldIV_initial_value - 1.
  // OldIV's entry predecessor is WarmUp (set by updateOuterHeaderPHIs).
  // For a decrement loop starting at N (the initial IV value), after peeling
  // one iteration the loop runs N-1 times, so the JNZD counter = N-1.
  BasicBlock *Preheader = LS.getPreheader();
  IRBuilder<> PreBuilder(Preheader->getTerminator());

  Value *TripCount;
  if (OldIVEarly) {
    Value *InitN = OldIVEarly->getIncomingValueForBlock(WarmUp);
    TripCount = PreBuilder.CreateSub(
        InitN, ConstantInt::get(InitN->getType(), 1), "outer.jnzd.tc");
    // Ensure i32.
    if (TripCount->getType() != I32Ty)
      TripCount =
          PreBuilder.CreateZExtOrTrunc(TripCount, I32Ty, "outer.jnzd.tc.i32");
  } else {
    // Fallback: use AdjustedTripCount (should not happen for decrement loops
    // that passed isOuterLoopDowncounting, but be safe).
    TripCount = AdjustedTripCount;
    if (TripCount->getType() != I32Ty)
      TripCount =
          PreBuilder.CreateZExtOrTrunc(TripCount, I32Ty, "outer.trip.i32");
  }

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
  // Re-use the latch branch and counter pointers identified earlier.
  BranchInst *LatchBr = LatchBrEarly;
  Value *OldCounter = OldCounterEarly;
  PHINode *OldIV = OldIVEarly;

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
    if (OldIV && OldIV->hasOneUse()) {
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
