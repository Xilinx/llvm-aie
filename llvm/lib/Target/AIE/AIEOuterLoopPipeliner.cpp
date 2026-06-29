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

// How reroutePhiIncomings treats OldPred's existing incoming entry.
enum class PhiEdge {
  Repoint,   // overwrite OldPred's entry to point at NewPred
  AppendNew, // keep OldPred's entry and add a parallel one for NewPred
};

// Reroute each PHI incoming in BB from OldPred to NewPred, value MapVal(old).
// AppendNew adds a parallel incoming (OldPred still live); Repoint overwrites.
static void reroutePhiIncomings(
    BasicBlock *BB, BasicBlock *OldPred, BasicBlock *NewPred, PhiEdge Edge,
    function_ref<Value *(Value *)> MapVal = [](Value *V) { return V; }) {
  for (PHINode &PHI : BB->phis()) {
    int Idx = PHI.getBasicBlockIndex(OldPred);
    if (Idx < 0)
      continue;
    Value *NewVal = MapVal(PHI.getIncomingValue(Idx));
    if (Edge == PhiEdge::AppendNew) {
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

  // Untransformed: recurse into subloops, e.g. a nested middle loop is the only
  // one annotated for pipelining.
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

  std::unique_ptr<OrigLoopStructure> LS = OrigLoopStructure::analyze(L);
  if (!LS) {
    LLVM_DEBUG(dbgs() << "    Not pipelining: unsupported loop structure\n");
    return false;
  }

  if (!LS->isProfitableToRotate(
          *SE, Overrides.get(OuterLoopPipeliningMinTripCount))) {
    LLVM_DEBUG(dbgs() << "    Not pipelining: not profitable to rotate\n");
    return false;
  }

  if (!LS->isSafeToReorderMemoryOps()) {
    LLVM_DEBUG(dbgs() << "    Not pipelining: unsafe to reorder memory ops\n");
    return false;
  }

  LLVM_DEBUG(dbgs() << "  Applying outer loop pipelining on ";
             LS->getTop()->printAsOperand(dbgs(), false); dbgs() << "\n");
  return performTransformation(*LS, Overrides);
}

bool OrigLoopStructure::analyzeLoopStructure() {
  // Early validation before populating the inner-loop fields.
  if (OuterLoop->getSubLoops().size() != 1) {
    LLVM_DEBUG(dbgs() << "    Not exactly one subloop\n");
    return false;
  }
  if (!OuterLoop->getLoopLatch()) {
    LLVM_DEBUG(dbgs() << "    No single bottom block\n");
    return false;
  }

  // Validate inner loop components before deriving fields from them.
  InnerLoop = OuterLoop->getSubLoops()[0];
  if (!InnerLoop->getLoopPreheader() || !InnerLoop->getExitBlock() ||
      !InnerLoop->getLoopLatch()) {
    LLVM_DEBUG(dbgs() << "    Inner loop missing preheader/exit/latch\n");
    return false;
  }

  // Populate the prologue/bottom regions and the derived-field backing state.
  InnerExit = InnerLoop->getExitBlock();
  InnerLoopBlocks.assign(InnerLoop->block_begin(), InnerLoop->block_end());
  OuterLoopID = OuterLoop->getLoopID();
  TopRegion.assign({OuterLoop->getHeader()});
  BottomRegion.assign({OuterLoop->getLoopLatch()});

  // Epilogue must be a single block: inner exit == bottom block.
  if (getInnerExit() != getBottom()) {
    LLVM_DEBUG(dbgs() << "    Inner exit != bottom block\n");
    return false;
  }

  if (!discoverTopRegion())
    return false;

  if (!allOuterBlocksAccountedFor())
    return false;

  LLVM_DEBUG(dbgs() << "    top region: " << topRegion().size()
                    << " block(s); epilogue in bottom block\n");

  if (!tryAdjustLoopBound()) {
    LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound\n");
    return false;
  }
  return true;
}

bool OrigLoopStructure::discoverTopRegion() {
  // The derived getTop() assumes the linear single-block prologue
  // (top block == inner preheader); validate that against LoopInfo here.
  if (InnerLoop->getLoopPreheader() != getTop()) {
    LLVM_DEBUG(dbgs() << "    Inner preheader != top block\n");
    return false;
  }
  TopRegion.assign({getTop()});
  return true;
}

bool OrigLoopStructure::allOuterBlocksAccountedFor() const {
  for (BasicBlock *BB : OuterLoop->blocks()) {
    const bool Accounted = getInnerLoop()->contains(BB) || BB == getBottom() ||
                           topRegion().contains(BB);
    if (Accounted)
      continue;
    LLVM_DEBUG(dbgs() << "    Unexpected outer-loop block: " << BB->getName()
                      << "\n");
    return false;
  }
  return true;
}

bool OrigLoopStructure::isInnerLoopHardwareLoop() const {
  auto *BI = dyn_cast<BranchInst>(getInnerLatch()->getTerminator());
  if (!BI || !BI->isConditional())
    return false;
  auto *Cond = dyn_cast<Instruction>(BI->getCondition());
  return Cond && AIEIRUtils::isHardwareLoopDecrement(Cond);
}

SmallVector<LoadInst *, 8> OrigLoopStructure::collectTopLoads() const {
  // The prologue is the single top block.
  SmallVector<LoadInst *, 8> Loads;
  for (Instruction &I : *getTop())
    if (auto *L = dyn_cast<LoadInst>(&I))
      Loads.push_back(L);
  return Loads;
}

SmallVector<StoreInst *, 8> OrigLoopStructure::collectBottomStores() const {
  // The epilogue is the single bottom block.
  SmallVector<StoreInst *, 8> Stores;
  for (Instruction &I : *getBottom())
    if (auto *S = dyn_cast<StoreInst>(&I))
      Stores.push_back(S);
  return Stores;
}

void AIEOuterLoopPipeliner::seedHeaderPhiEdge(RemapTable &Map,
                                              const LoopStructure &LS,
                                              BasicBlock *FromEdge) const {
  for (PHINode &PHI : LS.getTop()->phis()) {
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

  SmallVector<StoreInst *, 8> EpilogueStores = collectBottomStores();
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
  // TODO(outer-loop-pipeliner): add alias/dependence analysis for load/store
  // reordering; for now only reject never-reorderable volatile/atomic ops.
  if (anyVolatileOrAtomic<LoadInst>(collectTopLoads())) {
    LLVM_DEBUG(dbgs() << "    Unsafe: volatile/atomic top-block load\n");
    return false;
  }
  if (anyVolatileOrAtomic<StoreInst>(collectBottomStores())) {
    LLVM_DEBUG(dbgs() << "    Unsafe: volatile/atomic bottom-block store\n");
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

void OrigLoopStructure::backwardClosure(
    SmallVectorImpl<Instruction *> &Worklist,
    SmallPtrSetImpl<Instruction *> &Set) const {
  while (!Worklist.empty()) {
    Instruction *I = Worklist.pop_back_val();
    for (Value *Op : I->operands()) {
      auto *OpI = dyn_cast<Instruction>(Op);
      if (!OpI || !isPipelineableValue(OpI))
        continue;
      if (Set.insert(OpI).second)
        Worklist.push_back(OpI);
    }
  }
}

SmallPtrSet<Instruction *, 32> OrigLoopStructure::collectStage1Cone(
    function_ref<bool(const Instruction *)> IsSplitPoint) const {
  // Forward closure from the top-block loads: the candidates a stage-1 split
  // point (e.g. a wide-vector producer; see isStage1SplitPoint) is found among.
  SmallPtrSet<Instruction *, 32> ReachableFromLoad;
  SmallVector<Instruction *, 32> Worklist;
  topRegion().forEachInstruction([&](Instruction *I) {
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
  topRegion().forEachInstruction([&](Instruction *I) {
    if (!isPipelineCandidate(I))
      return;
    (Stage1Set.count(I) ? Stage1Insts : Stage0Insts).push_back(I);
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
      if (PHI.getIncomingBlock(I) == getTop())
        Seed(PHI.getIncomingValue(I));

  backwardClosure(Worklist, Visited);

  // Emit the reached candidates in region program order.
  topRegion().forEachInstruction([&](Instruction *I) {
    if (isPipelineCandidate(I) && Visited.count(I))
      Stage0Insts.push_back(I);
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

void AIEOuterLoopPipeliner::cloneStage0AsPeel(const OrigLoopStructure &OrigLS,
                                              CloneLoopStructure &SteadyLS,
                                              RemapTable &PeelVMap) {
  Function *F = SteadyLS.getTop()->getParent();
  BasicBlock *Preheader = SteadyLS.getPreheader();

  // Seed PeelVMap with the entry (preheader) values of the top block PHIs so
  // the peel's loads use the entry pointers.
  seedHeaderPhiEdge(PeelVMap, SteadyLS, Preheader);

  BasicBlock *Peel =
      BasicBlock::Create(F->getContext(), "stage0.top", F, SteadyLS.getTop());
  PeelVMap[SteadyLS.getTop()] = Peel;
  cloneAndRemapInsts(remapToClone(OrigLS.stage0Insts(), SteadyLS.cloneMap()),
                     *Peel, Peel->end(), PeelVMap, ".peel");

  BranchInst::Create(SteadyLS.getTop(), Peel);
  Preheader->getTerminator()->replaceSuccessorWith(SteadyLS.getTop(), Peel);

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
  Function *F = Src.getTop()->getParent();

  // Blocks in program order; CloneBasicBlock seeds CloneMap (src->clone).
  SmallVector<BasicBlock *, 8> OrigBlocks = Src.blocksInProgramOrder();

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
  for (BasicBlock *BB : Src.topRegion().blocks())
    CloneProBlocks.push_back(clonedBlock(BB));
  TopRegion.assign(CloneProBlocks);
  BottomRegion.assign({clonedBlock(Src.getBottom())});

  // Copy the cached latch condition; its pointers still reference the source LS
  // until remapBoundThroughCloneMap retargets them to this clone.
  OuterLoopCondition = Src.latchCondition();

  // Label clones by <copy>.stage1.<position>; the bottom block also hosts the
  // next iteration's stage-0 prefetch, hence the compound name.
  getTop()->setName(SuffixStr + ".stage1.top");
  getBottom()->setName(SuffixStr + ".stage1.bottom.and.stage0.top");
  for (auto [Orig, Clone] : zip(Src.getInnerBlocks(), getInnerBlocks()))
    Clone->setName(SuffixStr + ".stage1.inner." + Orig->getName());
}

CloneLoopStructure::CloneLoopStructure(const LoopStructure &Src,
                                       LastIterSkeletonTag) {
  OuterLoopID = Src.getOuterLoopID();
  Function *F = Src.getTop()->getParent();
  LLVMContext &Ctx = F->getContext();

  // The last-iteration LS is spliced just before the source loop's exit
  // successor; all its blocks are created before that block.
  BasicBlock *Exit = Src.getExitBlock();

  // The last-iteration top is also its inner preheader; inner-loop PHIs
  // incoming from the source top now come from it.
  BasicBlock *LastIterTop =
      BasicBlock::Create(Ctx, "lastiter.stage1.top", F, Exit);
  CloneMap[Src.getTop()] = LastIterTop;

  // One empty clone per inner-loop block, named lastiter.stage1.inner.<orig>,
  // recorded in CloneMap so the body cloners can resolve each block to its
  // clone.
  for (BasicBlock *BB : Src.getInnerBlocks()) {
    BasicBlock *Clone = BasicBlock::Create(
        Ctx, "lastiter.stage1.inner." + BB->getName(), F, Exit);
    CloneMap[BB] = Clone;
    InnerLoopBlocks.push_back(Clone);
  }

  // The last-iteration bottom is also the inner exit, so the cloned inner loop
  // and stores resolve their exits to it. It carries no stage-0 prefetch.
  BasicBlock *LastIterBottom =
      BasicBlock::Create(Ctx, "lastiter.stage1.bottom", F, Exit);
  CloneMap[Src.getBottom()] = LastIterBottom;
  if (Src.getInnerExit() == Src.getBottom())
    CloneMap[Src.getInnerExit()] = LastIterBottom;

  InnerExit = LastIterBottom;
  TopRegion.assign({LastIterTop});
  BottomRegion.assign({LastIterBottom});
}

void AIEOuterLoopPipeliner::swapInClonedLS(const OrigLoopStructure &OrigLS,
                                           CloneLoopStructure &SteadyLS) const {
  BasicBlock *Preheader = OrigLS.getPreheader();
  BasicBlock *OrigExit = OrigLS.getExitBlock();

  // Redirecting leaves the original LS unreachable, so capture the preheader on
  // the clone now — LoopInfo can no longer recover it afterwards.
  Preheader->getTerminator()->replaceSuccessorWith(OrigLS.getTop(),
                                                   SteadyLS.getTop());
  SteadyLS.recordExistingPreheader(Preheader);

  // Add a clone-latch incoming to the exit PHIs. AppendNew: the original latch
  // still branches here until removeFromCFG, so its entry must survive.
  reroutePhiIncomings(OrigExit, OrigLS.getBottom(), SteadyLS.getBottom(),
                      PhiEdge::AppendNew,
                      [&](Value *V) { return SteadyLS.cloneOf(V); });
}

Value *CloneLoopStructure::lastIterInputFor(Value *Src) const {
  Value *Clone = cloneOf(Src);
  // A header PHI of this loop carries the next-iteration value on its back
  // edge; that is what the peeled final iteration consumes.
  if (auto *PHI = dyn_cast<PHINode>(Clone); PHI && PHI->getParent() == getTop())
    return PHI->getIncomingValueForBlock(getBottom());
  return Clone;
}

void CloneLoopStructure::remapBoundThroughCloneMap() {
  auto Map = [&](Value *V) -> Value * { return V ? cloneOf(V) : V; };
  LatchConditionInfo &B = OuterLoopCondition;
  B.Cmp = cast<ICmpInst>(Map(B.Cmp));
  // A loop-invariant Limit is not in the clone map and maps to itself.
  B.Limit = Map(B.Limit);
  B.Counter = cast_or_null<BinaryOperator>(Map(B.Counter));
  B.OldIV = cast_or_null<PHINode>(Map(B.OldIV));
}

void OrigLoopStructure::removeFromCFG() const {
  SmallVector<BasicBlock *, 8> Dead = blocksInProgramOrder();
  DeleteDeadBlocks(Dead);
}

void AIEOuterLoopPipeliner::cloneStage0IntoBottom(
    const OrigLoopStructure &OrigLS, const CloneLoopStructure &SteadyLS,
    RemapTable &EpiVMap) {
  // Seed EpiVMap with the next-iteration (latch incoming) values of the outer
  // header PHIs so the cloned loads prefetch the next iteration's pointers.
  seedHeaderPhiEdge(EpiVMap, SteadyLS, SteadyLS.getBottom());

  SmallVector<Instruction *, 16> Stage0Insts =
      remapToClone(OrigLS.stage0Insts(), SteadyLS.cloneMap());
  Instruction *LatchTerm = SteadyLS.getBottom()->getTerminator();
  cloneAndRemapInsts(Stage0Insts, *SteadyLS.getBottom(),
                     LatchTerm->getIterator(), EpiVMap, ".epi");
  LLVM_DEBUG(dbgs() << "    Cloned stage-0 into bottom block\n");
}

void AIEOuterLoopPipeliner::createPipelinedPHIs(const OrigLoopStructure &OrigLS,
                                                CloneLoopStructure &SteadyLS,
                                                const RemapTable &PeelVMap,
                                                const RemapTable &EpiVMap) {
  BasicBlock *Peel = SteadyLS.getPreheader();
  Instruction *InsertPt = &*SteadyLS.getTop()->getFirstInsertionPt();

  // Walk OrigLS stage-0 and its steady clones together: the merged PHI replaces
  // the steady clone, and SteadyLS's map is retargeted Orig -> PHI so the slot
  // resolves to the merged PHI afterwards (e.g. for lastIterInputFor).
  SmallVector<Instruction *, 16> SteadyClones;
  for (Instruction *OrigI : OrigLS.stage0Insts()) {
    Instruction *I = cast<Instruction>(SteadyLS.cloneOf(OrigI));
    SteadyClones.push_back(I);
    // Void-typed instructions produce no value to merge; each path runs its
    // own clone.
    if (I->getType()->isVoidTy())
      continue;
    auto WIt = PeelVMap.find(I);
    auto EIt = EpiVMap.find(I);
    // Both cloners add every stage-0 entry, so every non-void inst is in both.
    assert(WIt != PeelVMap.end() && EIt != EpiVMap.end() &&
           "Prologue instruction must be in both Peel and Epilogue VMaps");
    PHINode *PHI = PHINode::Create(I->getType(), 2, I->getName() + ".phi");
    PHI->insertBefore(InsertPt->getIterator());
    PHI->addIncoming(WIt->second, Peel);
    PHI->addIncoming(EIt->second, SteadyLS.getBottom());

    // The inner-loop and intra-top uses now read the merge PHI; the peel /
    // bottom clones keep using their own mapped values.
    I->replaceAllUsesWith(PHI);
    SteadyLS.retargetClone(OrigI, PHI);
  }

  // Erase the now-dead steady stage-0 clones (reverse order).
  for (Instruction *I : reverse(SteadyClones))
    if (I->use_empty())
      I->eraseFromParent();
}

void AIEOuterLoopPipeliner::peelLastIteration(
    const OrigLoopStructure &OrigLS, const CloneLoopStructure &SteadyLS) {
  // Build the empty last-iteration LS as a structural clone of OrigLS; it owns
  // its orig->lastiter clone map.
  CloneLoopStructure LastIterLS(OrigLS,
                                CloneLoopStructure::LastIterSkeletonTag{});
  RemapTable &LastIterMap = LastIterLS.cloneMap();

  // Seed the live-ins: stage-0 slots and loop-carried PHIs enter the last
  // iteration as the values SteadyLS prefetched / carried on its back edge.
  seedLastIterInputs(LastIterMap, OrigLS, SteadyLS);

  // Fill the top (hardware-loop setup + stage-1) before the inner loop, so its
  // PHIs referencing stage-1 results resolve.
  cloneHardwareLoopSetupInto(LastIterLS, OrigLS);
  cloneAndRemapInsts(OrigLS.stage1Insts(), *LastIterLS.getTop(),
                     LastIterLS.getTop()->end(), LastIterMap, ".lastiter");
  cloneInnerLoopIntoLastIter(OrigLS, LastIterLS);
  populateLastIterBottom(OrigLS, LastIterLS);
  wireLastIterIntoCFG(OrigLS, SteadyLS, LastIterLS);

  LLVM_DEBUG(dbgs() << "    Created last-iteration: "
                    << LastIterLS.getTop()->getName() << " -> "
                    << LastIterLS.getBottom()->getName() << "\n");
}

void AIEOuterLoopPipeliner::seedLastIterInputs(
    RemapTable &Map, const OrigLoopStructure &OrigLS,
    const CloneLoopStructure &SteadyLS) const {
  // Each stage-0 slot enters as its steady prefetch value.
  for (Instruction *I : OrigLS.stage0Insts())
    if (!I->getType()->isVoidTy())
      Map[I] = SteadyLS.lastIterInputFor(I);

  // Each loop-carried header PHI enters as its steady next-iteration value.
  // Skip the outer counter: it has no role in the single last iteration and its
  // steady clone may have been removed by JNZD conversion.
  const PHINode *Counter = OrigLS.latchCondition().OldIV;
  for (PHINode &PHI : OrigLS.getTop()->phis())
    if (&PHI != Counter)
      Map[&PHI] = SteadyLS.lastIterInputFor(&PHI);
}

void AIEOuterLoopPipeliner::cloneHardwareLoopSetupInto(
    CloneLoopStructure &LastIterLS, const LoopStructure &Src) const {
  SmallVector<Instruction *, 4> SetupInsts;
  for (Instruction &I : *Src.getTop()) {
    if (I.isTerminator())
      break;
    if (AIEIRUtils::isHardwareLoopSetup(&I))
      SetupInsts.push_back(&I);
  }
  BasicBlock *Dest = LastIterLS.getTop();
  cloneAndRemapInsts(SetupInsts, *Dest, Dest->end(), LastIterLS.cloneMap(),
                     /*Suffix=*/"");
}

void AIEOuterLoopPipeliner::cloneInnerLoopIntoLastIter(
    const OrigLoopStructure &OrigLS, CloneLoopStructure &LastIterLS) const {
  RemapTable &LastIterMap = LastIterLS.cloneMap();
  // Clone every inner-loop body into its skeleton clone first, so all
  // cross-block references exist before any remap runs.
  SmallVector<Instruction *, 32> Clones;
  for (BasicBlock *Orig : OrigLS.getInnerBlocks()) {
    auto *Clone = cast<BasicBlock>(LastIterMap[Orig]);
    for (Instruction &Inst : *Orig)
      Clones.push_back(
          cloneInstInto(Inst, *Clone, Clone->end(), LastIterMap, ".lastiter"));
  }
  remapClones(Clones, LastIterMap);

  BranchInst::Create(LastIterLS.getInnerHeader(), LastIterLS.getTop());
}

void AIEOuterLoopPipeliner::populateLastIterBottom(
    const OrigLoopStructure &OrigLS, CloneLoopStructure &LastIterLS) const {
  // Clone the pristine original latch except its back-edge control: accumulated
  // values are read after the loop, but the counter and compare are now dead.
  const LatchConditionInfo &Bound = OrigLS.latchCondition();
  SmallVector<Instruction *, 16> OrigEpiInsts;
  for (Instruction &I : *OrigLS.getBottom()) {
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
  BasicBlock *LastIterBottom = LastIterLS.getBottom();
  cloneAndRemapInsts(OrigEpiInsts, *LastIterBottom, LastIterBottom->end(),
                     LastIterLS.cloneMap(), ".lastiter");
}

void AIEOuterLoopPipeliner::wireLastIterIntoCFG(
    const OrigLoopStructure &OrigLS, const CloneLoopStructure &SteadyLS,
    const CloneLoopStructure &LastIterLS) const {
  BasicBlock *OrigExit = OrigLS.getExitBlock();
  BasicBlock *LastIterBottom = LastIterLS.getBottom();
  BranchInst::Create(OrigExit, LastIterBottom);

  // Replace the steady scaffolding edge (added by swapInClonedLS) with the real
  // last-iteration edge. The live-out value is read from the still-present
  // orig-latch incoming and mapped to its lastiter clone in a single hop.
  for (PHINode &PHI : OrigExit->phis()) {
    int SteadyIdx = PHI.getBasicBlockIndex(SteadyLS.getBottom());
    if (SteadyIdx < 0)
      continue;
    Value *OrigVal = PHI.getIncomingValueForBlock(OrigLS.getBottom());
    PHI.setIncomingBlock(SteadyIdx, LastIterBottom);
    PHI.setIncomingValue(SteadyIdx, LastIterLS.cloneOf(OrigVal));
  }

  // Retarget non-PHI live-outs (LCSSA values rematerialized into the dedicated
  // exit block); they reference orig values, mapped in one hop.
  for (Instruction &I : *OrigExit) {
    if (isa<PHINode>(&I))
      continue;
    for (Use &U : I.operands())
      if (auto *OpI = dyn_cast<Instruction>(U.get()))
        U.set(LastIterLS.cloneOf(OpI));
  }

  SteadyLS.getLatchBranch()->replaceSuccessorWith(OrigExit,
                                                  LastIterLS.getTop());
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

std::unique_ptr<OrigLoopStructure> OrigLoopStructure::analyze(Loop *L) {
  // Private constructor: a validated LS is only reachable through here.
  auto LS = std::unique_ptr<OrigLoopStructure>(new OrigLoopStructure(L));
  if (!LS->analyzeLoopStructure())
    return nullptr;
  return LS;
}

bool OrigLoopStructure::isRegionInternalPhi(const PHINode *PHI) const {
  for (const BasicBlock *IB : PHI->blocks())
    if (!TopRegion.contains(IB))
      return false;
  return true;
}

bool OrigLoopStructure::isPipelineableValue(const Instruction *I) const {
  if (!isInTop(I))
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

BasicBlock *LoopStructure::getInnerLatch() const {
  for (BasicBlock *Pred : predecessors(getInnerHeader()))
    if (is_contained(InnerLoopBlocks, Pred))
      return Pred;
  return nullptr;
}

SmallVector<BasicBlock *, 8> LoopStructure::blocksInProgramOrder() const {
  SmallVector<BasicBlock *, 8> Blocks;
  Blocks.push_back(getTop());
  Blocks.append(getInnerBlocks().begin(), getInnerBlocks().end());
  // OuterLatch == InnerExit (validated in analyzeLoopStructure), so it is not
  // in InnerBlocks; append it once.
  Blocks.push_back(getBottom());
  return Blocks;
}

BasicBlock *LoopStructure::getExitBlock() const {
  BranchInst *LatchBr = getLatchBranch();
  assert(LatchBr->isConditional() && "Bottom block must be conditional");
  for (BasicBlock *Succ : LatchBr->successors())
    if (Succ != getTop())
      return Succ;
  llvm_unreachable("Bottom block must have an exit successor");
}

void CloneLoopStructure::recordExistingPreheader(BasicBlock *BB) {
  assert(llvm::all_of(
             getTop()->phis(),
             [&](PHINode &PHI) { return PHI.getBasicBlockIndex(BB) >= 0; }) &&
         "header PHIs must already reference the recorded preheader");
  OuterPreheader = BB;
}

void CloneLoopStructure::installPreheader(BasicBlock *NewPreheader) {
  BasicBlock *OldPreheader = getPreheader();
  reroutePhiIncomings(getTop(), OldPreheader, NewPreheader, PhiEdge::Repoint);
  OuterPreheader = NewPreheader;
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

namespace {
// Cmp's operands split into its loop-invariant limit (with operand index) and
// the remaining counter operand.
struct LimitCounterSplit {
  Value *Limit;
  unsigned LimitIdx;
  Value *Counter;
};
} // namespace

// Split Cmp into its loop-invariant limit and counter operand, or nullopt when
// neither operand is loop-invariant.
static std::optional<LimitCounterSplit>
splitCompareIntoLimitAndCounter(const Loop *L, ICmpInst *Cmp) {
  for (unsigned I = 0; I < 2; ++I) {
    Value *Op = Cmp->getOperand(I);
    if (L->isLoopInvariant(Op))
      return LimitCounterSplit{Op, I, Cmp->getOperand(1 - I)};
  }
  return std::nullopt;
}

bool OrigLoopStructure::tryAdjustLoopBound() {
  auto *BI = dyn_cast<BranchInst>(getBottom()->getTerminator());
  if (!BI || !BI->isConditional())
    return false;
  auto *Cmp = dyn_cast<ICmpInst>(BI->getCondition());
  if (!Cmp)
    return false;

  std::optional<LimitCounterSplit> Split =
      splitCompareIntoLimitAndCounter(getOuterLoop(), Cmp);
  if (!Split)
    return false;
  auto [Limit, LimitIdx, Counter] = *Split;

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

  PHINode *OldIV = findCountingPhi(CounterAdd, getTop());
  if (!OldIV) {
    LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound: counting PHI not "
                         "found in top block\n");
    return false;
  }

  OuterLoopCondition = {Cmp, Limit, LimitIdx, Step, CounterAdd, OldIV};
  return true;
}

// NewLimit = Limit - Step covers increment (Step > 0) and decrement (Step < 0)
// loops of any constant step magnitude.
Value *CloneLoopStructure::adjustLoopBound() const {
  const LatchConditionInfo &B = latchCondition();
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
  LLVMContext &Ctx = getTop()->getContext();

  // Decrement itercount.range (N -> N-1) to reflect the peeled iteration.
  MDNode *AdjustedID = updateIterCounts(
      Ctx, LoopID, /*FixMin=*/[](int64_t V) { return V - 1; },
      /*FixMax=*/[](int64_t V) { return V - 1; });
  MDNode *Source = AdjustedID ? AdjustedID : LoopID;

  // Drop the consumed enable hint and append the success marker.
  MDNode *FinalLoopID = rebuildPipelinedLoopID(Ctx, Source);

  // Write onto the latch terminator (what Loop::setLoopID does internally) so
  // this works on a clone with no LoopInfo Loop.
  getBottom()->getTerminator()->setMetadata(LLVMContext::MD_loop, FinalLoopID);
}

// An intrinsic other than a safe 2D/3D pointer increment, whose unknown side
// effects forbid moving it out of the epilogue.
static bool isUnsafeIntrinsicToLift(const AIEBaseInstrInfo &TII,
                                    const Instruction *I) {
  const auto *II = dyn_cast<IntrinsicInst>(I);
  return II && !isSafePointerIncrementIntrinsic(TII, II->getIntrinsicID());
}

// True if a chain instruction is used by an epilogue instruction outside the
// chain (a store, the exit icmp, ...), which pins the chain to the epilogue.
static bool
hasExternalEpilogueUser(const LoopStructure &OrigLS,
                        const SmallPtrSetImpl<Instruction *> &Chain) {
  for (Instruction *I : Chain)
    for (User *U : I->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      if (UI && OrigLS.isInBottom(UI) && !Chain.contains(UI))
        return true;
    }
  return false;
}

std::optional<SmallPtrSet<Instruction *, 16>>
AIEOuterLoopPipeliner::collectLiftableBottomChain(
    const OrigLoopStructure &OrigLS, PHINode &PHI) const {
  Value *LatchVal = PHI.getIncomingValueForBlock(OrigLS.getBottom());
  auto *LatchInst = dyn_cast<Instruction>(LatchVal);
  const bool EpilogueDefinedBackEdge =
      LatchInst && OrigLS.isInBottom(LatchInst);
  if (!EpilogueDefinedBackEdge)
    return std::nullopt;

  // Collect the epilogue operands feeding the back-edge value; bail if the
  // chain reaches an inner-loop value or an unsafe intrinsic.
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
      const bool ExtendsChain = OrigLS.isInBottom(OpI);
      if (!ExtendsChain)
        continue;
      const bool NewlyReached = Chain.insert(OpI).second;
      if (NewlyReached)
        Worklist.push_back(OpI);
    }
  }

  if (hasExternalEpilogueUser(OrigLS, Chain))
    return std::nullopt;

  LLVM_DEBUG(dbgs() << "    PHI " << PHI.getName() << ": lifting chain of "
                    << Chain.size() << " instructions\n");
  return Chain;
}

bool AIEOuterLoopPipeliner::liftBottomPointerUpdatesToTop(
    const OrigLoopStructure &OrigLS) {
  // Union the liftable chains of every PHI (each lifts independently).
  SmallPtrSet<Instruction *, 32> AllLiftable;
  for (PHINode &PHI : OrigLS.getTop()->phis())
    if (auto Chain = collectLiftableBottomChain(OrigLS, PHI))
      AllLiftable.insert(Chain->begin(), Chain->end());

  if (AllLiftable.empty())
    return false;

  // Move the liftable instructions to the top-block end in program order.
  Instruction *InsertPt = OrigLS.getTop()->getTerminator();
  SmallVector<Instruction *, 32> ToLift;
  for (Instruction &I : *OrigLS.getBottom())
    if (AllLiftable.count(&I))
      ToLift.push_back(&I);
  for (Instruction *I : ToLift)
    I->moveBefore(InsertPt->getIterator());

  LLVM_DEBUG(dbgs() << "    Lifted " << ToLift.size()
                    << " instructions from bottom block to top block\n");
  return true;
}

bool AIEOuterLoopPipeliner::performTransformation(
    OrigLoopStructure &OrigLS, const AIE::LoopOptionOverrides &Overrides) {
  liftBottomPointerUpdatesToTop(OrigLS);

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
  // slot; transform steps below run on the clone, leaving OrigLS pristine.
  CloneLoopStructure SteadyLS(OrigLS, "steady");
  swapInClonedLS(OrigLS, SteadyLS);
  SteadyLS.remapBoundThroughCloneMap();

  // Peel the stage-0 chain before the steady header (entry pointer values);
  // this also adopts the peel as the steady preheader.
  RemapTable PeelVMap;
  cloneStage0AsPeel(OrigLS, SteadyLS, PeelVMap);

  // Prefetch the stage-0 chain in the epilogue (next-iteration pointer values).
  RemapTable EpiVMap;
  cloneStage0IntoBottom(OrigLS, SteadyLS, EpiVMap);

  // Merge the peel and epilogue copies into the steady header via PHIs.
  createPipelinedPHIs(OrigLS, SteadyLS, PeelVMap, EpiVMap);

  // Adjust the outer loop trip count from N to N-1. Must happen before the peel
  // so the hardware-loop conversion can find the right icmp to replace.
  SteadyLS.adjustLoopBound();

  // Convert to a JNZD hardware loop. MUST run before peelLastIteration so the
  // counting add/icmp are erased from the latch before the peel clones it.
  if (EnableOuterLoopHardwareLoop && SteadyLS.latchCondition().isDowncounting())
    convertOuterLoopToHardwareLoop(SteadyLS);

  // Create the last-iteration region from the steady loop.
  peelLastIteration(OrigLS, SteadyLS);

  // Adjust itercount metadata to reflect the reduced trip count.
  SteadyLS.updateLoopMetadata();

  OrigLS.removeFromCFG();

  return true;
}

// The i32 JNZD trip count materialized in the peel: peeling one iteration from
// a decrement loop starting at N leaves N-1 to run (NOT the adjusted
// threshold).
static Value *computeJNZDTripCount(BasicBlock *Peel, PHINode &OldIV,
                                   Type *I32Ty) {
  IRBuilder<> PreBuilder(Peel->getTerminator());
  Value *InitN = OldIV.getIncomingValueForBlock(Peel);
  Value *TripCount = PreBuilder.CreateSub(
      InitN, ConstantInt::get(InitN->getType(), 1), "outer.jnzd.tc");
  if (TripCount->getType() != I32Ty)
    TripCount =
        PreBuilder.CreateZExtOrTrunc(TripCount, I32Ty, "outer.jnzd.tc.i32");
  return TripCount;
}

// Create the outer-header counter PHI seeded from start_loop_iterations in the
// peel; its back-edge incoming is filled by rewriteLatchToDecrement.
static PHINode *createOuterCounterPHI(const CloneLoopStructure &SteadyLS,
                                      Value *TripCount, Type *I32Ty) {
  BasicBlock *Peel = SteadyLS.getPreheader();
  IRBuilder<> PreBuilder(Peel->getTerminator());
  Value *CtrInit = PreBuilder.CreateIntrinsic(
      Intrinsic::start_loop_iterations, {I32Ty}, {TripCount},
      /*FMFSource=*/nullptr, "outer.ctr.init");
  Instruction *InsertPt = &*SteadyLS.getTop()->getFirstInsertionPt();
  PHINode *CtrPHI =
      PHINode::Create(I32Ty, 2, "outer.ctr", InsertPt->getIterator());
  CtrPHI->addIncoming(CtrInit, Peel);
  return CtrPHI;
}

// Replace the latch condition with loop.decrement.reg(CtrPHI) != 0, keep the
// loop-continue edge as the true successor, close CtrPHI, and DCE the old icmp.
static void rewriteLatchToDecrement(const CloneLoopStructure &SteadyLS,
                                    PHINode *CtrPHI, Type *I32Ty) {
  BranchInst *LatchBr = SteadyLS.getLatchBranch();
  IRBuilder<> LatchBuilder(LatchBr);
  Value *CtrNext =
      LatchBuilder.CreateIntrinsic(Intrinsic::loop_decrement_reg, {I32Ty},
                                   {CtrPHI, ConstantInt::get(I32Ty, 1)},
                                   /*FMFSource=*/nullptr, "outer.ctr.next");
  Value *NewCond = LatchBuilder.CreateICmpNE(
      CtrNext, ConstantInt::get(I32Ty, 0), "outer.loop.cond");

  Value *OldCond = LatchBr->getCondition();
  LatchBr->setCondition(NewCond);
  const bool TopIsTakenSuccessor =
      LatchBr->getSuccessor(0) == SteadyLS.getTop();
  if (!TopIsTakenSuccessor)
    LatchBr->swapSuccessors();
  CtrPHI->addIncoming(CtrNext, SteadyLS.getBottom());
  RecursivelyDeleteTriviallyDeadInstructions(OldCond);
}

// Break the dead OldCounter/OldIV use cycle that trivial-DCE cannot: poison
// OldIV's latch slot when it feeds only OldCounter, then DCE the add.
static void eraseOldCounterCycle(const CloneLoopStructure &SteadyLS,
                                 const LatchConditionInfo &Info) {
  PHINode *OldIV = Info.OldIV;
  if (OldIV->hasOneUse()) {
    int LatchIdx = OldIV->getBasicBlockIndex(SteadyLS.getBottom());
    const bool HasLatchIncoming = LatchIdx >= 0;
    if (HasLatchIncoming)
      OldIV->setIncomingValue(LatchIdx, PoisonValue::get(OldIV->getType()));
  }
  // OldIV may be deleted transitively here; do not reference it afterwards.
  RecursivelyDeleteTriviallyDeadInstructions(Info.Counter);
}

void AIEOuterLoopPipeliner::convertOuterLoopToHardwareLoop(
    const CloneLoopStructure &SteadyLS) {
  const LatchConditionInfo &Info = SteadyLS.latchCondition();
  assert(Info.isDowncounting() && Info.Cmp && Info.Counter && Info.OldIV &&
         "JNZD conversion needs a fully validated downcounting latch");
  Type *I32Ty = Type::getInt32Ty(SteadyLS.getTop()->getContext());

  Value *TripCount =
      computeJNZDTripCount(SteadyLS.getPreheader(), *Info.OldIV, I32Ty);
  PHINode *CtrPHI = createOuterCounterPHI(SteadyLS, TripCount, I32Ty);
  rewriteLatchToDecrement(SteadyLS, CtrPHI, I32Ty);
  eraseOldCounterCycle(SteadyLS, Info);

  LLVM_DEBUG(dbgs() << "    Converted outer loop to JNZD hardware loop\n");
}
