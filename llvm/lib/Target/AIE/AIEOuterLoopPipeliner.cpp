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
// Implementation File for OuterLoop Pipeliner.
//
//===----------------------------------------------------------------------===//

#include "AIEOuterLoopPipeliner.h"
#include "AIE.h"
#include "AIEBaseInstrInfo.h"
#include "Utils/AIEIRUtils.h"
#include "Utils/AIELoopOptionOverrides.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
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
using namespace llvm::OuterLoopPipelining;

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

using SplitStrategy = std::function<bool(const Instruction *)>;

static bool produces2048BitVector(const Instruction *I) {
  if (!isa<CallInst>(I))
    return false;
  auto *VT = dyn_cast<FixedVectorType>(I->getType());
  return VT && VT->getPrimitiveSizeInBits() == 2048;
}

static SmallVector<SplitStrategy, 4> getSplitStrategies() {
  // TODO: add more split strategies.
  return {
      produces2048BitVector,
  };
}

// Returns true if any split strategy marks this instruction as the point where
// stage 1 begins (e.g. a wide-vector producer; see produces2048BitVector).
static bool isStage1SplitPoint(const Instruction *I) {
  for (const auto &Strategy : getSplitStrategies()) {
    if (Strategy(I))
      return true;
  }
  return false;
}

static bool isSafePointerIncrementIntrinsic(const AIEBaseInstrInfo &TII,
                                            Intrinsic::ID IID) {
  return IID == TII.getAddrIntrinsic2D() || IID == TII.getAddrIntrinsic3D();
}

// Reroute each PHI incoming in BB from OldPred to NewPred, with value
// MapVal(oldValue) (default identity). KeepOldPred appends a new incoming
// instead of repointing — needed while OldPred still branches to BB.
static void reroutePhiIncomings(
    BasicBlock *BB, BasicBlock *OldPred, BasicBlock *NewPred, bool KeepOldPred,
    function_ref<Value *(Value *)> MapVal = [](Value *V) { return V; }) {
  for (PHINode &PHI : BB->phis()) {
    int Idx = PHI.getBasicBlockIndex(OldPred);
    if (Idx < 0)
      continue;
    Value *NewVal = MapVal(PHI.getIncomingValue(Idx));
    if (KeepOldPred) {
      PHI.addIncoming(NewVal, NewPred);
    } else {
      PHI.setIncomingValue(Idx, NewVal);
      PHI.setIncomingBlock(Idx, NewPred);
    }
  }
}

char AIEOuterLoopPipeliner::ID = 0;
char &llvm::AIEOuterLoopPipelinerID = AIEOuterLoopPipeliner::ID;

INITIALIZE_PASS_BEGIN(AIEOuterLoopPipeliner, DEBUG_TYPE,
                      "AIE Outer Loop Pipeliner", false, false)
INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
INITIALIZE_PASS_DEPENDENCY(DominatorTreeWrapperPass)
INITIALIZE_PASS_DEPENDENCY(ScalarEvolutionWrapperPass)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_END(AIEOuterLoopPipeliner, DEBUG_TYPE,
                    "AIE Outer Loop Pipeliner", false, false)

llvm::FunctionPass *llvm::createAIEOuterLoopPipelinerPass() {
  return new AIEOuterLoopPipeliner();
}

void AIEOuterLoopPipeliner::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<LoopInfoWrapperPass>();
  AU.addRequired<DominatorTreeWrapperPass>();
  AU.addRequired<ScalarEvolutionWrapperPass>();
  AU.addRequired<TargetPassConfig>();
  FunctionPass::getAnalysisUsage(AU);
}

bool AIEOuterLoopPipeliner::runOnFunction(Function &F) {
  if (skipFunction(F))
    return false;

  LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
  DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
  SE = &getAnalysis<ScalarEvolutionWrapperPass>().getSE();
  const TargetMachine &TM =
      getAnalysis<TargetPassConfig>().getTM<TargetMachine>();
  TII = static_cast<const AIEBaseInstrInfo *>(
      TM.getSubtargetImpl(F)->getInstrInfo());
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

  if (tryPipelineLoop(L, Overrides))
    return true;

  // If this loop was not transformed, recursively try its subloops.
  // This handles nested structures like: outermost { middle { innermost } }
  // where only the middle loop is annotated for pipelining.
  bool Changed = false;
  for (Loop *SubLoop : L->getSubLoops())
    Changed |= runOnLoop(SubLoop);
  return Changed;
}

bool AIEOuterLoopPipeliner::tryPipelineLoop(
    Loop *L, const AIE::LoopOptionOverrides &Overrides) {
  if (!Overrides.get(EnableOuterLoopPipelining)) {
    LLVM_DEBUG(dbgs() << "    Not pipelining: not enabled (flag/metadata)\n");
    return false;
  }

  OrigLoopStructure LS(L);
  if (!LS.isValid()) {
    LLVM_DEBUG(dbgs() << "    Not pipelining: unsupported loop structure\n");
    return false;
  }

  if (!LS.isProfitableToRotate(
          *SE, Overrides.get(OuterLoopPipeliningMinTripCount))) {
    LLVM_DEBUG(dbgs() << "    Not pipelining: not profitable to rotate\n");
    return false;
  }

  if (!LS.isSafeToReorderMemoryOps()) {
    LLVM_DEBUG(dbgs() << "    Not pipelining: unsafe to reorder memory ops\n");
    return false;
  }

  LLVM_DEBUG(dbgs() << "  Applying outer loop pipelining on ";
             LS.getOuterHeader()->printAsOperand(dbgs(), false);
             dbgs() << "\n");
  return performTransformation(LS, Overrides);
}

bool OrigLoopStructure::analyzeLoopStructure() {
  // Early validation before populating the inner-loop fields.
  if (OuterLoop->getSubLoops().size() != 1) {
    LLVM_DEBUG(dbgs() << "    Not exactly one subloop\n");
    return false;
  }
  if (!OuterLoop->getLoopLatch()) {
    LLVM_DEBUG(dbgs() << "    No single outer latch\n");
    return false;
  }

  // Validate inner loop components before deriving fields from them.
  InnerLoop = OuterLoop->getSubLoops()[0];
  if (!InnerLoop->getLoopPreheader() || !InnerLoop->getExitBlock() ||
      !InnerLoop->getLoopLatch()) {
    LLVM_DEBUG(dbgs() << "    Inner loop missing preheader/exit/latch\n");
    return false;
  }

  // Populate the prologue/epilogue regions and the derived-field backing state.
  InnerExit = InnerLoop->getExitBlock();
  InnerLoopBlocks.assign(InnerLoop->block_begin(), InnerLoop->block_end());
  OuterLoopID = OuterLoop->getLoopID();
  PrologueRegion.assign({OuterLoop->getHeader()});
  EpilogueRegion.assign({OuterLoop->getLoopLatch()});

  // Epilogue must be a single block: inner exit == outer latch.
  if (getInnerExit() != getOuterLatch()) {
    LLVM_DEBUG(dbgs() << "    Inner exit != outer latch\n");
    return false;
  }

  if (!discoverPrologueRegion())
    return false;

  // Every outer-loop block must belong to the prologue region, the inner loop,
  // or the single-block epilogue (latch); anything else is an unknown shape.
  for (BasicBlock *BB : OuterLoop->blocks()) {
    const bool BlockIsAccountedFor = getInnerLoop()->contains(BB) ||
                                     BB == getOuterLatch() ||
                                     prologueRegion().contains(BB);
    if (BlockIsAccountedFor)
      continue;
    LLVM_DEBUG(dbgs() << "    Unexpected outer-loop block: " << BB->getName()
                      << "\n");
    return false;
  }

  LLVM_DEBUG(dbgs() << "    Prologue region: " << prologueRegion().size()
                    << " block(s); epilogue in outer.latch\n");

  if (!tryAdjustLoopBound()) {
    LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound\n");
    return false;
  }
  return true;
}

bool OrigLoopStructure::discoverPrologueRegion() {
  // The derived getInnerPreheader() assumes the linear single-block prologue
  // (outer header == inner preheader); validate that against LoopInfo here.
  if (InnerLoop->getLoopPreheader() != getOuterHeader()) {
    LLVM_DEBUG(dbgs() << "    Inner preheader != outer header\n");
    return false;
  }
  prologueRegion().assign({getOuterHeader()});
  return true;
}

bool OrigLoopStructure::isInnerLoopHardwareLoop() const {
  auto *BI = dyn_cast<BranchInst>(getInnerLatch()->getTerminator());
  if (!BI || !BI->isConditional())
    return false;
  auto *Cond = dyn_cast<Instruction>(BI->getCondition());
  return Cond && AIEIRUtils::isHardwareLoopDecrement(Cond);
}

SmallVector<LoadInst *, 8> OrigLoopStructure::collectPrologueLoads() const {
  // The prologue is the single outer header block.
  SmallVector<LoadInst *, 8> Loads;
  for (Instruction &I : *getOuterHeader())
    if (auto *L = dyn_cast<LoadInst>(&I))
      Loads.push_back(L);
  return Loads;
}

SmallVector<StoreInst *, 8> OrigLoopStructure::collectEpilogueStores() const {
  // The epilogue is the single outer latch block.
  SmallVector<StoreInst *, 8> Stores;
  for (Instruction &I : *getOuterLatch())
    if (auto *S = dyn_cast<StoreInst>(&I))
      Stores.push_back(S);
  return Stores;
}

void AIEOuterLoopPipeliner::seedHeaderPhiEdge(RemapTable &Map,
                                              const LoopStructure &LS,
                                              BasicBlock *FromEdge) const {
  for (PHINode &PHI : LS.getOuterHeader()->phis()) {
    int Idx = PHI.getBasicBlockIndex(FromEdge);
    if (Idx >= 0)
      Map[&PHI] = PHI.getIncomingValue(Idx);
  }
}

bool OrigLoopStructure::isProfitableToRotate(ScalarEvolution &SE,
                                             unsigned MinTripCount) const {
  if (!isInnerLoopHardwareLoop()) {
    LLVM_DEBUG(dbgs() << "    Inner loop is not a hardware loop\n");
    return false;
  }
  std::optional<int64_t> MinTC = llvm::getMinTripCount(getOuterLoop(), &SE);
  const bool TripCountTooLow = !MinTC || *MinTC < (int64_t)MinTripCount;
  if (TripCountTooLow) {
    LLVM_DEBUG(dbgs() << "    Trip count too low\n");
    return false;
  }

  SmallVector<StoreInst *, 8> EpilogueStores = collectEpilogueStores();
  // TODO: Confirm whether a store-free epilogue can ever be profitable.
  if (EpilogueStores.empty()) {
    LLVM_DEBUG(dbgs() << "    No stores in epilogue\n");
    return false;
  }
  return true;
}

// A volatile or atomic memory op can never be reordered.
template <typename MemInstT>
static bool anyVolatileOrAtomic(ArrayRef<MemInstT *> MemOps) {
  return llvm::any_of(MemOps, [](const MemInstT *M) {
    return M->isVolatile() || M->isAtomic();
  });
}

bool OrigLoopStructure::isSafeToReorderMemoryOps() const {
  // TODO: Add alias/dependence analysis to verify that moving prologue loads
  // before epilogue stores is safe. For now, only reject volatile/atomic
  // operations which can never be reordered.
  if (anyVolatileOrAtomic<LoadInst>(collectPrologueLoads())) {
    LLVM_DEBUG(dbgs() << "    Unsafe: volatile/atomic prologue load\n");
    return false;
  }
  if (anyVolatileOrAtomic<StoreInst>(collectEpilogueStores())) {
    LLVM_DEBUG(dbgs() << "    Unsafe: volatile/atomic epilogue store\n");
    return false;
  }
  return true;
}

void OrigLoopStructure::forwardClosure(
    SmallVectorImpl<Instruction *> &Worklist,
    SmallPtrSetImpl<Instruction *> &Set) const {
  while (!Worklist.empty()) {
    Instruction *I = Worklist.pop_back_val();
    for (User *U : I->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      if (!UI || !isPipelineableValue(UI))
        continue;
      if (Set.insert(UI).second)
        Worklist.push_back(UI);
    }
  }
}

SmallPtrSet<Instruction *, 32> OrigLoopStructure::collectStage1Cone(
    function_ref<bool(const Instruction *)> IsSplitPoint) const {
  // Forward closure from the prologue loads: the candidates a stage-1 split
  // point (e.g. a wide-vector producer; see isStage1SplitPoint) is found among.
  SmallPtrSet<Instruction *, 32> ReachableFromLoad;
  SmallVector<Instruction *, 32> Worklist;
  prologueRegion().forEachInstruction([&](Instruction *I) {
    if (isa<LoadInst>(I)) {
      ReachableFromLoad.insert(I);
      Worklist.push_back(I);
    }
  });
  forwardClosure(Worklist, ReachableFromLoad);

  // Seed the cone with those split points, then take their forward closure (all
  // descendants within the prologue).
  SmallPtrSet<Instruction *, 32> Cone;
  SmallVector<Instruction *, 16> ConeWorklist;
  for (Instruction *I : ReachableFromLoad)
    if (IsSplitPoint(I) && Cone.insert(I).second)
      ConeWorklist.push_back(I);
  forwardClosure(ConeWorklist, Cone);
  return Cone;
}

void OrigLoopStructure::collectStages(
    function_ref<bool(const Instruction *)> IsSplitPoint) {
  // Stage 1 is the split-point cone. With no split point every candidate
  // backward-reachable from the inner loop is stage 0.
  const SmallPtrSet<Instruction *, 32> Stage1Set =
      collectStage1Cone(IsSplitPoint);
  if (Stage1Set.empty()) {
    collectStage0FromInnerLoop();
    return;
  }

  // Stage 0 is every other pipeline candidate (the load/address chain).
  prologueRegion().forEachInstruction([&](Instruction *I) {
    if (!isPipelineCandidate(I))
      return;
    (Stage1Set.count(I) ? stage1Insts() : stage0Insts()).push_back(I);
  });

  LLVM_DEBUG(dbgs() << "    Stages: " << stage0Insts().size() << " stage-0, "
                    << stage1Insts().size() << " stage-1 instructions\n");
}

void OrigLoopStructure::collectStage0FromInnerLoop() {
  SmallPtrSet<Instruction *, 32> Visited;
  SmallVector<Instruction *, 16> Worklist;
  auto Seed = [&](Value *V) {
    auto *I = dyn_cast<Instruction>(V);
    if (!I || !isPipelineableValue(I))
      return;
    if (Visited.insert(I).second)
      Worklist.push_back(I);
  };
  // Seed from values used inside the inner loop.
  for (BasicBlock *BB : getInnerLoop()->blocks())
    for (Instruction &I : *BB)
      for (Value *Op : I.operands())
        Seed(Op);
  // Seed from initial values of inner header PHIs (from the preheader).
  for (PHINode &PHI : getInnerHeader()->phis())
    for (unsigned I = 0; I < PHI.getNumIncomingValues(); ++I)
      if (PHI.getIncomingBlock(I) == getInnerPreheader())
        Seed(PHI.getIncomingValue(I));
  // Backward-track through operands.
  while (!Worklist.empty()) {
    Instruction *I = Worklist.pop_back_val();
    for (Value *Op : I->operands()) {
      auto *OpI = dyn_cast<Instruction>(Op);
      if (!OpI || !isPipelineableValue(OpI))
        continue;
      if (Visited.insert(OpI).second)
        Worklist.push_back(OpI);
    }
  }
  // Emit the reached candidates in region program order.
  prologueRegion().forEachInstruction([&](Instruction *I) {
    if (isPipelineCandidate(I) && Visited.count(I))
      stage0Insts().push_back(I);
  });
}

SmallVector<Instruction *, 16>
AIEOuterLoopPipeliner::remapToClone(ArrayRef<Instruction *> Insts,
                                    const RemapTable &VMap) {
  SmallVector<Instruction *, 16> Out;
  for (Instruction *I : Insts) {
    auto It = VMap.find(I);
    Out.push_back(It != VMap.end()
                      ? cast<Instruction>(static_cast<Value *>(It->second))
                      : I);
  }
  return Out;
}

void AIEOuterLoopPipeliner::clonePrologueAsPeel(const OrigLoopStructure &OrigLS,
                                                CloneLoopStructure &SteadyLS,
                                                RemapTable &PeelVMap) {
  Function *F = SteadyLS.getOuterHeader()->getParent();
  BasicBlock *Preheader = SteadyLS.getPreheader();

  // Seed PeelVMap with the entry (preheader) values of the outer header PHIs so
  // the peel's loads use the entry pointers.
  seedHeaderPhiEdge(PeelVMap, SteadyLS, Preheader);

  BasicBlock *Peel = BasicBlock::Create(F->getContext(), "steady.preheader", F,
                                        SteadyLS.getOuterHeader());
  PeelVMap[SteadyLS.getOuterHeader()] = Peel;
  cloneAndRemapInsts(remapToClone(OrigLS.stage0Insts(), SteadyLS.cloneMap()),
                     *Peel, Peel->end(), PeelVMap, ".peel");

  BranchInst::Create(SteadyLS.getOuterHeader(), Peel);
  Preheader->getTerminator()->replaceSuccessorWith(SteadyLS.getOuterHeader(),
                                                   Peel);

  // The peel is now the preheader for the steady LS.
  SteadyLS.installPreheader(Peel);
  LLVM_DEBUG(dbgs() << "    Created peel block: " << Peel->getName() << "\n");
}

Instruction *AIEOuterLoopPipeliner::cloneInstInto(Instruction &I,
                                                  BasicBlock &Dest,
                                                  BasicBlock::iterator InsertPt,
                                                  RemapTable &VMap,
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
                                        RemapTable &VMap) {
  for (Instruction *CloneI : Clones)
    RemapInstruction(CloneI, VMap,
                     RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);
}

SmallVector<Instruction *, 16> AIEOuterLoopPipeliner::cloneAndRemapInsts(
    ArrayRef<Instruction *> Insts, BasicBlock &DstBB,
    BasicBlock::iterator InsertPt, RemapTable &VMap, const Twine &Suffix) {
  SmallVector<Instruction *, 16> Clones;
  for (Instruction *I : Insts)
    Clones.push_back(cloneInstInto(*I, DstBB, InsertPt, VMap, Suffix));
  remapClones(Clones, VMap);
  return Clones;
}

CloneLoopStructure::CloneLoopStructure(const LoopStructure &Src,
                                       const Twine &Suffix) {
  Function *F = Src.getOuterHeader()->getParent();

  // Blocks in program order; CloneBasicBlock seeds CloneMap (src->clone).
  SmallVector<BasicBlock *, 8> OrigBlocks;
  OrigBlocks.push_back(Src.getOuterHeader());
  OrigBlocks.append(Src.getInnerBlocks().begin(), Src.getInnerBlocks().end());
  // OuterLatch == InnerExit (validated in analyzeLoopStructure), so it is not
  // in InnerBlocks; append it once.
  OrigBlocks.push_back(Src.getOuterLatch());

  SmallString<32> SuffixStorage;
  StringRef SuffixStr = Suffix.toStringRef(SuffixStorage);
  SmallVector<BasicBlock *, 8> CloneBlocks;
  for (BasicBlock *BB : OrigBlocks) {
    BasicBlock *CB = CloneBasicBlock(BB, CloneMap, "." + SuffixStr, F);
    // Move the clone before its original so it inherits the program-order slot
    // once the original LS is deleted.
    CB->moveBefore(BB);
    CloneMap[BB] = CB;
    CloneBlocks.push_back(CB);
  }

  // Remap within-LS references; edges leaving the LS stay pointed at the
  // originals for the caller to rewire (they are absent from CloneMap).
  remapInstructionsInBlocks(CloneBlocks, CloneMap);

  InnerExit = clonedBlock(Src.getInnerExit());
  for (BasicBlock *BB : Src.getInnerBlocks())
    InnerLoopBlocks.push_back(clonedBlock(BB));
  OuterLoopID = Src.getOuterLoopID();

  // Mirror the regions onto the clones so membership queries work on the clone.
  SmallVector<BasicBlock *, 4> CloneProBlocks;
  for (BasicBlock *BB : Src.prologueRegion().blocks())
    CloneProBlocks.push_back(clonedBlock(BB));
  PrologueRegion.assign(CloneProBlocks);
  EpilogueRegion.assign({clonedBlock(Src.getOuterLatch())});

  // Copy the cached latch-bound; its instruction pointers still reference the
  // source LS and are remapped to the clone by remapBoundToClone.
  OuterLoopCondition = Src.bound();

  // Rename clones by role; CloneBasicBlock's "<orig>.<suffix>" names read as
  // the original loop.
  getOuterHeader()->setName(SuffixStr + ".header");
  getOuterLatch()->setName(SuffixStr + ".latch");
  for (auto [Orig, Clone] : zip(Src.getInnerBlocks(), getInnerBlocks()))
    Clone->setName(SuffixStr + "." + Orig->getName());
}

CloneLoopStructure::CloneLoopStructure(const LoopStructure &Steady,
                                       LastIterSkeletonTag) {
  OuterLoopID = Steady.getOuterLoopID();
  Function *F = Steady.getOuterHeader()->getParent();
  LLVMContext &Ctx = F->getContext();

  // The last-iteration LS is spliced just before the steady loop's exit
  // successor; all its blocks are created before that block.
  BasicBlock *Exit = Steady.getExitBlock();

  // lastiter.prologue is the last-iteration outer header (and, for the linear
  // LS, its inner preheader); inner-loop PHIs incoming from the outer header
  // now come from it.
  BasicBlock *LastIterPrologue =
      BasicBlock::Create(Ctx, "lastiter.prologue", F, Exit);
  CloneMap[Steady.getOuterHeader()] = LastIterPrologue;

  // One empty clone per inner-loop block, recorded in CloneMap so the body
  // cloners can resolve each block to its clone.
  for (BasicBlock *BB : Steady.getInnerBlocks()) {
    BasicBlock *Clone =
        BasicBlock::Create(Ctx, BB->getName() + ".lastiter", F, Exit);
    CloneMap[BB] = Clone;
    InnerLoopBlocks.push_back(Clone);
  }

  // lastiter.epilogue is the last-iteration outer latch; it receives the steady
  // latch (and, in the single-block case, the inner exit) so the cloned inner
  // loop and epilogue resolve their exits to it.
  BasicBlock *LastIterEpilogue =
      BasicBlock::Create(Ctx, "lastiter.epilogue", F, Exit);
  CloneMap[Steady.getOuterLatch()] = LastIterEpilogue;
  if (Steady.getInnerExit() == Steady.getOuterLatch())
    CloneMap[Steady.getInnerExit()] = LastIterEpilogue;

  InnerExit = LastIterEpilogue;
  PrologueRegion.assign({LastIterPrologue});
  EpilogueRegion.assign({LastIterEpilogue});
}

void AIEOuterLoopPipeliner::swapInClonedLS(const OrigLoopStructure &OrigLS,
                                           CloneLoopStructure &SteadyLS) const {
  BasicBlock *Preheader = OrigLS.getPreheader();
  BasicBlock *OrigExit = OrigLS.getExitBlock();

  // Redirecting leaves the original LS unreachable, so capture the preheader on
  // the clone now — LoopInfo can no longer recover it afterwards.
  Preheader->getTerminator()->replaceSuccessorWith(OrigLS.getOuterHeader(),
                                                   SteadyLS.getOuterHeader());
  SteadyLS.setOuterPreheader(Preheader);

  // Add a clone-latch incoming to the exit PHIs so the exit sees the clone's
  // defs. KeepOldPred: the original latch still branches here until
  // removeFromCFG, so its entry must survive.
  reroutePhiIncomings(
      OrigExit, OrigLS.getOuterLatch(), SteadyLS.getOuterLatch(),
      /*KeepOldPred=*/true, [&](Value *V) { return SteadyLS.cloneOf(V); });
}

void AIEOuterLoopPipeliner::remapBoundToClone(
    CloneLoopStructure &SteadyLS) const {
  auto Map = [&](Value *V) -> Value * { return V ? SteadyLS.cloneOf(V) : V; };
  LatchConditionInfo &B = SteadyLS.bound();
  B.Cmp = cast<ICmpInst>(Map(B.Cmp));
  // A loop-invariant Limit is not in the clone map and maps to itself.
  B.Limit = Map(B.Limit);
  B.Counter = cast_or_null<BinaryOperator>(Map(B.Counter));
  B.OldIV = cast_or_null<PHINode>(Map(B.OldIV));
}

void OrigLoopStructure::removeFromCFG() const {
  SmallVector<BasicBlock *, 8> Dead;
  Dead.push_back(getOuterHeader());
  Dead.append(getInnerBlocks().begin(), getInnerBlocks().end());
  Dead.push_back(getOuterLatch());
  DeleteDeadBlocks(Dead);
}

void AIEOuterLoopPipeliner::clonePrologueIntoEpilogue(
    const OrigLoopStructure &OrigLS, const CloneLoopStructure &SteadyLS,
    RemapTable &EpiVMap) {
  // Seed EpiVMap with the next-iteration (latch incoming) values of the outer
  // header PHIs so the cloned loads prefetch the next iteration's pointers.
  seedHeaderPhiEdge(EpiVMap, SteadyLS, SteadyLS.getOuterLatch());

  SmallVector<Instruction *, 16> Stage0Insts =
      remapToClone(OrigLS.stage0Insts(), SteadyLS.cloneMap());
  Instruction *LatchTerm = SteadyLS.getOuterLatch()->getTerminator();
  cloneAndRemapInsts(Stage0Insts, *SteadyLS.getOuterLatch(),
                     LatchTerm->getIterator(), EpiVMap, ".epi");
  LLVM_DEBUG(dbgs() << "    Cloned prologue into epilogue\n");
}

void AIEOuterLoopPipeliner::createPipelinedPHIs(
    const OrigLoopStructure &OrigLS, const CloneLoopStructure &SteadyLS,
    const RemapTable &PeelVMap, const RemapTable &EpiVMap) {
  BasicBlock *Peel = SteadyLS.getPreheader();
  Instruction *InsertPt = &*SteadyLS.getOuterHeader()->getFirstInsertionPt();

  SmallVector<Instruction *, 16> Stage0Insts =
      remapToClone(OrigLS.stage0Insts(), SteadyLS.cloneMap());
  SmallVector<std::pair<Instruction *, PHINode *>, 8> Replacements;
  for (Instruction *I : Stage0Insts) {
    // Void-typed instructions produce no value to merge; each path runs its
    // own clone.
    if (I->getType()->isVoidTy())
      continue;
    auto WIt = PeelVMap.find(I);
    auto EIt = EpiVMap.find(I);
    // Both cloners add every stage-0 entry, so every non-void inst is in both.
    assert(WIt != PeelVMap.end() && EIt != EpiVMap.end() &&
           "Prologue instruction must be in both Peel and Epilogue VMaps");
    Value *PeelVal = WIt->second;
    Value *EpiVal = EIt->second;
    PHINode *PHI = PHINode::Create(I->getType(), 2, I->getName() + ".phi");
    PHI->insertBefore(InsertPt->getIterator());
    PHI->addIncoming(PeelVal, Peel);
    PHI->addIncoming(EpiVal, SteadyLS.getOuterLatch());
    Replacements.push_back({I, PHI});
  }

  // Replace all uses of the originals (inner-loop and intra-prologue) with the
  // merge PHIs; the peel/epilogue clones use mapped values, not the originals.
  for (auto &[Orig, PHI] : Replacements)
    Orig->replaceAllUsesWith(PHI);

  // Erase original prologue instructions from the outer header (reverse order).
  for (Instruction *I : reverse(Stage0Insts)) {
    if (I->use_empty())
      I->eraseFromParent();
  }
}

void AIEOuterLoopPipeliner::peelLastIteration(
    const OrigLoopStructure &OrigLS, const CloneLoopStructure &SteadyLS) {
  // Build the empty last-iteration LS. It owns its steady->lastiter clone map;
  // the skeleton constructor records the block mappings.
  CloneLoopStructure LastIterLS(SteadyLS,
                                CloneLoopStructure::LastIterSkeletonTag{});
  RemapTable &LastIterMap = LastIterLS.cloneMap();

  // Seed each steady outer-header PHI to its latch-incoming value, so every
  // clone below picks up the final epilogue values.
  seedHeaderPhiEdge(LastIterMap, SteadyLS, SteadyLS.getOuterLatch());

  // Fill each block. The prologue holds the hardware-loop setup and the
  // stage-1 accumulator seeds; both must be in place before the inner loop is
  // filled so its PHIs that reference stage-1 results resolve.
  cloneHardwareLoopSetupInto(LastIterLS, SteadyLS);
  SmallVector<Instruction *, 16> Stage1Insts =
      remapToClone(OrigLS.stage1Insts(), SteadyLS.cloneMap());
  cloneAndRemapInsts(Stage1Insts, *LastIterLS.getOuterHeader(),
                     LastIterLS.getOuterHeader()->end(), LastIterMap,
                     ".lastiter");
  cloneInnerLoopIntoLastIter(SteadyLS, LastIterLS);
  populateLastIterEpilogue(OrigLS, SteadyLS, LastIterLS);
  wireLastIterIntoCFG(OrigLS, SteadyLS, LastIterLS);

  LLVM_DEBUG(dbgs() << "    Created last-iteration: "
                    << LastIterLS.getOuterHeader()->getName() << " -> "
                    << LastIterLS.getOuterLatch()->getName() << "\n");
}

void AIEOuterLoopPipeliner::cloneHardwareLoopSetupInto(
    CloneLoopStructure &LastIterLS, const CloneLoopStructure &SteadyLS) const {
  SmallVector<Instruction *, 4> SetupInsts;
  for (Instruction &I : *SteadyLS.getOuterHeader()) {
    if (I.isTerminator())
      break;
    if (AIEIRUtils::isHardwareLoopSetup(&I))
      SetupInsts.push_back(&I);
  }
  BasicBlock *Dest = LastIterLS.getOuterHeader();
  cloneAndRemapInsts(SetupInsts, *Dest, Dest->end(), LastIterLS.cloneMap(),
                     /*Suffix=*/"");
}

void AIEOuterLoopPipeliner::cloneInnerLoopIntoLastIter(
    const CloneLoopStructure &SteadyLS, CloneLoopStructure &LastIterLS) const {
  RemapTable &LastIterMap = LastIterLS.cloneMap();
  // Clone every inner-loop block's body into its skeleton clone (resolved via
  // the clone map) first, so all cross-block references exist before any remap
  // runs.
  SmallVector<Instruction *, 32> Clones;
  for (BasicBlock *Orig : SteadyLS.getInnerBlocks()) {
    auto *Clone = cast<BasicBlock>(LastIterMap[Orig]);
    for (Instruction &Inst : *Orig)
      Clones.push_back(
          cloneInstInto(Inst, *Clone, Clone->end(), LastIterMap, ".lastiter"));
  }
  remapClones(Clones, LastIterMap);

  BranchInst::Create(LastIterLS.getInnerHeader(), LastIterLS.getOuterHeader());
}

void AIEOuterLoopPipeliner::populateLastIterEpilogue(
    const OrigLoopStructure &OrigLS, const CloneLoopStructure &SteadyLS,
    CloneLoopStructure &LastIterLS) const {
  // Clone the whole pristine original latch: a value accumulated there is read
  // after the loop, so omitting it would compute a wrong last iteration.
  // Back-edge control is the exception — dead without a back-edge, and its
  // steady clone may already be freed by convertOuterLoopToHardwareLoop.
  const LatchConditionInfo &Bound = OrigLS.bound();
  SmallVector<Instruction *, 16> OrigEpiInsts;
  for (Instruction &I : *OrigLS.getOuterLatch()) {
    if (I.isTerminator())
      break;
    // No prefetch in the last-iteration.
    if (isa<LoadInst>(&I))
      continue;
    // The back-edge control is dead without a back-edge.
    const bool IsBackEdgeControl = &I == Bound.Counter || &I == Bound.Cmp;
    if (IsBackEdgeControl)
      continue;
    OrigEpiInsts.push_back(&I);
  }
  // Translate Orig -> Steady, then clone Steady -> last-iteration.
  SmallVector<Instruction *, 16> SteadyEpiInsts =
      remapToClone(OrigEpiInsts, SteadyLS.cloneMap());
  BasicBlock *LastIterEpilogue = LastIterLS.getOuterLatch();
  cloneAndRemapInsts(SteadyEpiInsts, *LastIterEpilogue, LastIterEpilogue->end(),
                     LastIterLS.cloneMap(), ".lastiter");
}

void AIEOuterLoopPipeliner::wireLastIterIntoCFG(
    const OrigLoopStructure &OrigLS, const CloneLoopStructure &SteadyLS,
    const CloneLoopStructure &LastIterLS) const {
  BasicBlock *OrigExit = SteadyLS.getExitBlock();
  BasicBlock *LastIterEpilogue = LastIterLS.getOuterLatch();
  BranchInst::Create(OrigExit, LastIterEpilogue);

  // Compose orig -> steady -> lastiter: a PHI incoming is a steady value (one
  // hop), a rematerialized live-out is an orig value (two hops); cloneOf passes
  // non-clones through, so both resolve correctly.
  auto ToLastIter = [&](Value *V) {
    return LastIterLS.cloneOf(SteadyLS.cloneOf(V));
  };

  // Repoint each exit PHI's latch edge to the last-iteration epilogue,
  // retargeting the value to its last-iteration clone.
  reroutePhiIncomings(OrigExit, SteadyLS.getOuterLatch(), LastIterEpilogue,
                      /*KeepOldPred=*/false, ToLastIter);

  // Retarget non-PHI live-outs (LCSSA values rematerialized into the dedicated
  // exit block) so their operands read the last-iteration clones.
  for (Instruction &I : *OrigExit) {
    if (isa<PHINode>(&I))
      continue;
    for (Use &U : I.operands())
      if (auto *OpI = dyn_cast<Instruction>(U.get()))
        U.set(ToLastIter(OpI));
  }

  BranchInst *LatchBr = SteadyLS.getLatchBranch();
  LatchBr->replaceSuccessorWith(OrigExit, LastIterLS.getOuterHeader());
}

bool BlockRegion::isSingleEntrySingleExit() const {
  for (BasicBlock *BB : Blocks) {
    if (BB != Blocks.front())
      for (BasicBlock *Pred : predecessors(BB))
        if (!contains(Pred))
          return false;
    if (BB != Blocks.back())
      for (BasicBlock *Succ : successors(BB))
        if (!contains(Succ))
          return false;
  }
  return true;
}

void BlockRegion::assign(ArrayRef<BasicBlock *> BBs) {
  Blocks.assign(BBs.begin(), BBs.end());
  assert(isSingleEntrySingleExit() && "BlockRegion must be single-entry "
                                      "single-exit");
}

void BlockRegion::forEachInstruction(
    function_ref<void(Instruction *)> Visit) const {
  for (BasicBlock *BB : Blocks)
    for (Instruction &I : *BB)
      Visit(&I);
}

OrigLoopStructure::OrigLoopStructure(Loop *L) : OuterLoop(L) {
  Valid = analyzeLoopStructure();
}

bool OrigLoopStructure::isRegionInternalPhi(const PHINode *PHI) const {
  for (const BasicBlock *IB : PHI->blocks())
    if (!PrologueRegion.contains(IB))
      return false;
  return true;
}

bool OrigLoopStructure::isPipelineableValue(const Instruction *I) const {
  if (!isInPrologue(I))
    return false;
  if (const auto *PHI = dyn_cast<PHINode>(I))
    return isRegionInternalPhi(PHI);
  return true;
}

bool OrigLoopStructure::isPipelineCandidate(const Instruction *I) const {
  if (I->isTerminator() || AIEIRUtils::isHardwareLoopSetup(I))
    return false;
  return isPipelineableValue(I);
}

BasicBlock *LoopStructure::getExitBlock() const {
  BranchInst *LatchBr = getLatchBranch();
  assert(LatchBr->isConditional() && "Outer latch must be conditional");
  for (BasicBlock *Succ : LatchBr->successors())
    if (Succ != getOuterHeader())
      return Succ;
  llvm_unreachable("Outer latch must have an exit successor");
}

void CloneLoopStructure::installPreheader(BasicBlock *NewPreheader) {
  reroutePhiIncomings(getOuterHeader(), getPreheader(), NewPreheader,
                      /*KeepOldPred=*/false);
  setOuterPreheader(NewPreheader);
}

// The `add Counter, C` (with constant C) that defines Counter, or nullptr.
static BinaryOperator *getConstantStepAdd(Value *Counter) {
  auto *Add = dyn_cast<BinaryOperator>(Counter);
  const bool IsConstantStepAdd = Add && Add->getOpcode() == Instruction::Add &&
                                 isa<ConstantInt>(Add->getOperand(1));
  return IsConstantStepAdd ? Add : nullptr;
}

// The induction step the predicate implies for a unit-stride loop, or nullopt
// for EQ/NE (ambiguous without a visible constant step).
static std::optional<int64_t> stepFromPredicate(ICmpInst::Predicate Pred) {
  if (Pred == ICmpInst::ICMP_SLT || Pred == ICmpInst::ICMP_ULT)
    return 1;
  if (Pred == ICmpInst::ICMP_SGT || Pred == ICmpInst::ICMP_UGT)
    return -1;
  return std::nullopt;
}

// The counting PHI in Header that feeds Add, or nullptr.
static PHINode *findCountingPhi(BinaryOperator *Add, const BasicBlock *Header) {
  for (Value *Op : Add->operands()) {
    auto *PHI = dyn_cast<PHINode>(Op);
    if (PHI && PHI->getParent() == Header)
      return PHI;
  }
  return nullptr;
}

bool OrigLoopStructure::tryAdjustLoopBound() {
  auto *BI = dyn_cast<BranchInst>(getOuterLatch()->getTerminator());
  if (!BI || !BI->isConditional())
    return false;
  auto *Cmp = dyn_cast<ICmpInst>(BI->getCondition());
  if (!Cmp)
    return false;

  // Split the compare into its loop-invariant limit and its counter operand.
  Value *Limit = nullptr;
  Value *Counter = nullptr;
  unsigned LimitIdx = 0;
  for (unsigned I = 0; I < 2; ++I) {
    Value *Op = Cmp->getOperand(I);
    if (getOuterLoop()->isLoopInvariant(Op)) {
      Limit = Op;
      LimitIdx = I;
      Counter = Cmp->getOperand(1 - I);
      break;
    }
  }
  if (!Limit)
    return false;

  // The step comes from the counting add's constant; a zero/absent constant
  // falls back to the predicate's unit stride.
  BinaryOperator *CounterAdd = getConstantStepAdd(Counter);
  int64_t Step =
      CounterAdd ? cast<ConstantInt>(CounterAdd->getOperand(1))->getSExtValue()
                 : 0;
  if (Step == 0) {
    std::optional<int64_t> PredStep = stepFromPredicate(Cmp->getPredicate());
    if (!PredStep) {
      LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound: EQ/NE with no "
                           "visible constant step\n");
      return false;
    }
    Step = *PredStep;
  }

  // A plain-PHI counter has no add to identify OldIV from, so its step is
  // unverifiable and JNZD conversion is impossible; reject it.
  if (!CounterAdd) {
    LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound: counter is not an add "
                         "instruction\n");
    return false;
  }

  PHINode *OldIV = findCountingPhi(CounterAdd, getOuterHeader());
  if (!OldIV) {
    LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound: counting PHI not "
                         "found in outer header\n");
    return false;
  }

  OuterLoopCondition = {Cmp, Limit, LimitIdx, Step, CounterAdd, OldIV};
  return true;
}

// NewLimit = Limit - Step covers increment (Step > 0) and decrement (Step < 0)
// loops of any constant step magnitude.
Value *CloneLoopStructure::adjustLoopBound() {
  const LatchConditionInfo &B = bound();
  IRBuilder<> Builder(getPreheader()->getTerminator());
  Value *NewLimit = Builder.CreateSub(
      B.Limit, ConstantInt::getSigned(B.Limit->getType(), B.Step),
      "outer.trip.adj");
  B.Cmp->setOperand(B.LimitIdx, NewLimit);
  LLVM_DEBUG(dbgs() << "    Adjusted loop bound: N -> N-1 (step=" << B.Step
                    << ")\n");
  return NewLimit;
}

// Copy Source's hint entries dropping the consumed enable hint, append the
// pipeliner success marker, and self-reference operand 0 as a loop ID requires.
static MDNode *rebuildPipelinedLoopID(LLVMContext &Ctx, MDNode *Source) {
  static constexpr StringLiteral HintKey{
      "llvm.loop.hint.aie-enable-outer-loop-pipelining"};
  static constexpr StringLiteral SuccessKey{
      "llvm.loop.hint.aie_outerloop_pipeliner_success"};

  SmallVector<Metadata *, 8> MDs;
  for (unsigned I = 1, E = Source->getNumOperands(); I < E; ++I) {
    MDNode *Entry = cast<MDNode>(Source->getOperand(I));
    auto Key = AIELoopUtils::getMetadataKey(*Entry);
    // Drop the consumed enable hint.
    if (Key && *Key == HintKey)
      continue;
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
  return FinalLoopID;
}

void CloneLoopStructure::updateLoopMetadata() const {
  MDNode *LoopID = getOuterLoopID();
  if (!LoopID)
    return;
  LLVMContext &Ctx = getOuterHeader()->getContext();

  // Decrement itercount.range (N -> N-1) to reflect the peeled iteration.
  MDNode *AdjustedID = updateIterCounts(
      Ctx, LoopID, /*FixMin=*/[](int64_t V) { return V - 1; },
      /*FixMax=*/[](int64_t V) { return V - 1; });
  MDNode *Source = AdjustedID ? AdjustedID : LoopID;

  // Drop the consumed enable hint and append the success marker.
  MDNode *FinalLoopID = rebuildPipelinedLoopID(Ctx, Source);

  // Write onto the latch terminator (what Loop::setLoopID does internally) so
  // this works on a clone with no LoopInfo Loop.
  getOuterLatch()->getTerminator()->setMetadata(LLVMContext::MD_loop,
                                                FinalLoopID);
}

// An intrinsic other than a safe 2D/3D pointer increment, whose unknown side
// effects forbid moving it out of the epilogue.
static bool isUnsafeIntrinsicToLift(const AIEBaseInstrInfo &TII,
                                    const Instruction *I) {
  const auto *II = dyn_cast<IntrinsicInst>(I);
  return II && !isSafePointerIncrementIntrinsic(TII, II->getIntrinsicID());
}

// A chain instruction used by an epilogue instruction outside the chain (a
// store, the exit icmp, ...), or nullptr if none. Such a use pins the chain to
// the epilogue, so it cannot be lifted.
static Instruction *
findExternalEpilogueUser(const LoopStructure &OrigLS,
                         const SmallPtrSetImpl<Instruction *> &Chain) {
  for (Instruction *I : Chain)
    for (User *U : I->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      const bool IsExternalEpilogueUse =
          UI && OrigLS.isInEpilogue(UI) && !Chain.contains(UI);
      if (IsExternalEpilogueUse)
        return UI;
    }
  return nullptr;
}

std::optional<SmallPtrSet<Instruction *, 16>>
AIEOuterLoopPipeliner::collectLiftableEpilogueChain(
    const OrigLoopStructure &OrigLS, PHINode &PHI) const {
  Value *LatchVal = PHI.getIncomingValueForBlock(OrigLS.getOuterLatch());
  auto *LatchInst = dyn_cast<Instruction>(LatchVal);
  const bool EpilogueDefinedBackEdge =
      LatchInst && OrigLS.isInEpilogue(LatchInst);
  if (!EpilogueDefinedBackEdge)
    return std::nullopt;

  // Backward-track from the back-edge value, collecting the epilogue operands
  // that feed it; bail if the chain reaches an inner-loop value or an unsafe
  // intrinsic.
  SmallPtrSet<Instruction *, 16> Chain;
  SmallVector<Instruction *, 16> Worklist;
  Chain.insert(LatchInst);
  Worklist.push_back(LatchInst);
  while (!Worklist.empty()) {
    Instruction *I = Worklist.pop_back_val();
    if (isUnsafeIntrinsicToLift(*TII, I))
      return std::nullopt;

    for (Value *Op : I->operands()) {
      auto *OpI = dyn_cast<Instruction>(Op);
      if (!OpI)
        continue;
      // A value computed in the inner loop cannot be lifted above it.
      const bool DependsOnInnerLoopValue =
          OrigLS.getInnerLoop()->contains(OpI->getParent());
      if (DependsOnInnerLoopValue)
        return std::nullopt;
      const bool ExtendsChain = OrigLS.isInEpilogue(OpI);
      if (!ExtendsChain)
        continue;
      const bool NewlyReached = Chain.insert(OpI).second;
      if (NewlyReached)
        Worklist.push_back(OpI);
    }
  }

  const bool ChainPinnedToEpilogue = findExternalEpilogueUser(OrigLS, Chain);
  if (ChainPinnedToEpilogue)
    return std::nullopt;

  LLVM_DEBUG(dbgs() << "    PHI " << PHI.getName() << ": lifting chain of "
                    << Chain.size() << " instructions\n");
  return Chain;
}

bool AIEOuterLoopPipeliner::liftEpiloguePointerUpdatesToPrologue(
    const OrigLoopStructure &OrigLS) {
  // Union the liftable chains of every PHI (each lifts independently).
  SmallPtrSet<Instruction *, 32> AllLiftable;
  for (PHINode &PHI : OrigLS.getOuterHeader()->phis())
    if (auto Chain = collectLiftableEpilogueChain(OrigLS, PHI))
      AllLiftable.insert(Chain->begin(), Chain->end());

  if (AllLiftable.empty())
    return false;

  // Move the liftable instructions to the prologue end in program order.
  Instruction *InsertPt = OrigLS.getOuterHeader()->getTerminator();
  SmallVector<Instruction *, 32> ToLift;
  for (Instruction &I : *OrigLS.getOuterLatch())
    if (AllLiftable.count(&I))
      ToLift.push_back(&I);
  for (Instruction *I : ToLift)
    I->moveBefore(InsertPt->getIterator());

  LLVM_DEBUG(dbgs() << "    Lifted " << ToLift.size()
                    << " instructions from epilogue to prologue\n");
  return true;
}

bool AIEOuterLoopPipeliner::performTransformation(
    OrigLoopStructure &OrigLS, const AIE::LoopOptionOverrides &Overrides) {
  liftEpiloguePointerUpdatesToPrologue(OrigLS);

  const bool SplitMode = Overrides.get(SplitPrologue);
  const auto IsSplitPoint = [SplitMode](const Instruction *I) {
    return SplitMode && isStage1SplitPoint(I);
  };
  OrigLS.collectStages(IsSplitPoint);
  if (OrigLS.stage0Insts().empty()) {
    LLVM_DEBUG(dbgs() << "    Could not extract Stage 0\n");
    return false;
  }

  // Clone the LS into a steady-state copy and swap it into the original's CFG
  // slot; all transform steps below run on the clone, leaving OrigLS pristine
  // for removal at the end.
  CloneLoopStructure SteadyLS(OrigLS, "steady");
  swapInClonedLS(OrigLS, SteadyLS);
  remapBoundToClone(SteadyLS);

  // Peel the stage-0 chain before the steady header (entry pointer values);
  // this also adopts the peel as the steady preheader.
  RemapTable PeelVMap;
  clonePrologueAsPeel(OrigLS, SteadyLS, PeelVMap);

  // Prefetch the stage-0 chain in the epilogue (next-iteration pointer values).
  RemapTable EpiVMap;
  clonePrologueIntoEpilogue(OrigLS, SteadyLS, EpiVMap);

  // Merge the peel and epilogue copies into the steady header via PHIs.
  createPipelinedPHIs(OrigLS, SteadyLS, PeelVMap, EpiVMap);

  // Adjust the outer loop trip count from N to N-1. Must happen before the peel
  // so the hardware-loop conversion can find the right icmp to replace.
  SteadyLS.adjustLoopBound();

  // Optionally convert the steady loop to a JNZD hardware loop. MUST run before
  // peelLastIteration so the counting add/icmp are erased from the
  // latch before the peel step clones it.
  if (EnableOuterLoopHardwareLoop)
    if (auto Info = SteadyLS.getDowncountingInfo())
      convertOuterLoopToHardwareLoop(SteadyLS, *Info);

  // Create the last-iteration region from the steady loop.
  peelLastIteration(OrigLS, SteadyLS);

  // Adjust itercount metadata to reflect the reduced trip count.
  SteadyLS.updateLoopMetadata();

  OrigLS.removeFromCFG();

  return true;
}

std::optional<DowncountingInfo>
CloneLoopStructure::getDowncountingInfo() const {
  // loop.decrement.reg only decrements by 1, so JNZD needs step == -1.
  if (bound().Step != -1)
    return std::nullopt;
  return DowncountingInfo{bound().Cmp, bound().Counter, bound().OldIV};
}

void AIEOuterLoopPipeliner::convertOuterLoopToHardwareLoop(
    const CloneLoopStructure &SteadyLS, const DowncountingInfo &Info) {
  LLVMContext &Ctx = SteadyLS.getOuterHeader()->getContext();
  Type *I32Ty = Type::getInt32Ty(Ctx);
  PHINode *OldIV = Info.OldIV;

  // For a decrement loop starting at N, peeling one iteration leaves N-1 to
  // run; the JNZD trip count is therefore the peel-incoming value of OldIV - 1
  // (NOT the adjusted icmp threshold), zero-extended/truncated to i32.
  BasicBlock *Peel = SteadyLS.getPreheader();
  IRBuilder<> PreBuilder(Peel->getTerminator());
  Value *InitN = OldIV->getIncomingValueForBlock(Peel);
  Value *TripCount = PreBuilder.CreateSub(
      InitN, ConstantInt::get(InitN->getType(), 1), "outer.jnzd.tc");
  if (TripCount->getType() != I32Ty)
    TripCount =
        PreBuilder.CreateZExtOrTrunc(TripCount, I32Ty, "outer.jnzd.tc.i32");

  Value *CtrInit = PreBuilder.CreateIntrinsic(
      Intrinsic::start_loop_iterations, {I32Ty}, {TripCount},
      /*FMFSource=*/nullptr, "outer.ctr.init");

  // Counter PHI in the outer header: entry value from the peel, back-edge value
  // filled in once the decrement is created below.
  Instruction *InsertPt = &*SteadyLS.getOuterHeader()->getFirstInsertionPt();
  PHINode *CtrPHI =
      PHINode::Create(I32Ty, 2, "outer.ctr", InsertPt->getIterator());
  CtrPHI->addIncoming(CtrInit, Peel);

  // Replace the latch icmp+add with loop.decrement.reg.
  BranchInst *LatchBr = SteadyLS.getLatchBranch();
  BinaryOperator *OldCounter = Info.Counter;

  IRBuilder<> LatchBuilder(LatchBr);
  Value *CtrNext =
      LatchBuilder.CreateIntrinsic(Intrinsic::loop_decrement_reg, {I32Ty},
                                   {CtrPHI, ConstantInt::get(I32Ty, 1)},
                                   /*FMFSource=*/nullptr, "outer.ctr.next");

  Value *NewCond = LatchBuilder.CreateICmpNE(
      CtrNext, ConstantInt::get(I32Ty, 0), "outer.loop.cond");

  Value *OldCond = LatchBr->getCondition();
  LatchBr->setCondition(NewCond);
  const bool TrueBranchContinuesLoop =
      LatchBr->getSuccessor(0) == SteadyLS.getOuterHeader();
  if (!TrueBranchContinuesLoop)
    LatchBr->swapSuccessors();

  CtrPHI->addIncoming(CtrNext, SteadyLS.getOuterLatch());

  // The condition was just replaced, so the old icmp is now dead.
  RecursivelyDeleteTriviallyDeadInstructions(OldCond);

  // OldCounter (add) and OldIV (counting PHI) form a use cycle that trivial-DCE
  // cannot break. If OldIV is used only by OldCounter, poison its latch slot so
  // both become dead; otherwise leave the PHI for its other users.
  assert(OldCounter && "DowncountingInfo guarantees a non-null Counter");
  if (OldIV->hasOneUse()) {
    int LatchIdx = OldIV->getBasicBlockIndex(SteadyLS.getOuterLatch());
    if (LatchIdx >= 0)
      OldIV->setIncomingValue(LatchIdx, PoisonValue::get(OldIV->getType()));
  }
  // OldIV may be deleted transitively here; do not reference it afterwards.
  RecursivelyDeleteTriviallyDeadInstructions(OldCounter);

  LLVM_DEBUG(dbgs() << "    Converted outer loop to JNZD hardware loop\n");
}
