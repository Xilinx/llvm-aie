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

// Type alias for split strategy predicates.
// A split strategy is a function that identifies "anchor" instructions
// which define the split point between peeled and kept instructions.
using SplitStrategy = std::function<bool(const Instruction *)>;

// Returns true if I is a CallInst whose return type is a 2048-bit vector.
// These are the "anchor" instructions that define the split point between
// peeled and kept instructions.
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

void AIEOuterLoopPipeliner::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<LoopInfoWrapperPass>();
  AU.addRequired<DominatorTreeWrapperPass>();
  AU.addRequired<ScalarEvolutionWrapperPass>();
  FunctionPass::getAnalysisUsage(AU);
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

  // Try to transform this loop if enabled (globally or via metadata). Each gate
  // below bails with a reason; the deeper helpers (analyzeLoopStructure,
  // isProfitableToRotate, isSafeToReorderMemoryOps) emit the specific cause.
  if (!Overrides.get(EnableOuterLoopPipelining)) {
    LLVM_DEBUG(dbgs() << "    Not pipelining: not enabled (flag/metadata)\n");
  } else if (std::optional<LoopStructure> MaybeLS = analyzeLoopStructure(L)) {
    LoopStructure &LS = *MaybeLS;
    if (!LS.isProfitableToRotate(
            *SE, Overrides.get(OuterLoopPipeliningMinTripCount))) {
      LLVM_DEBUG(dbgs() << "    Not pipelining: not profitable to rotate\n");
    } else if (!LS.isSafeToReorderMemoryOps()) {
      LLVM_DEBUG(
          dbgs() << "    Not pipelining: unsafe to reorder memory ops\n");
    } else {
      LLVM_DEBUG(dbgs() << "  Applying outer loop pipelining on ";
                 LS.getOuterHeader()->printAsOperand(dbgs(), false);
                 dbgs() << "\n");
      return performTransformation(LS, Overrides);
    }
  } else {
    LLVM_DEBUG(dbgs() << "    Not pipelining: unsupported loop structure\n");
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
  LoopStructure OrigLS(L);

  // Validate inner loop components.
  if (!OrigLS.getInnerPreheader() || !OrigLS.getInnerExit() ||
      !OrigLS.getInnerLatch()) {
    LLVM_DEBUG(dbgs() << "    Inner loop missing preheader/exit/latch\n");
    return std::nullopt;
  }

  // Epilogue must be a single block: inner exit == outer latch.
  if (OrigLS.getInnerExit() != OrigLS.getOuterLatch()) {
    LLVM_DEBUG(dbgs() << "    Inner exit != outer latch\n");
    return std::nullopt;
  }

  if (!OrigLS.discoverPrologueRegion())
    return std::nullopt;

  // Every outer-loop block must belong to the prologue region, the inner loop,
  // or the single-block epilogue (latch); anything else is an unknown shape.
  for (BasicBlock *BB : L->blocks()) {
    if (OrigLS.getInnerLoop()->contains(BB) || BB == OrigLS.getOuterLatch() ||
        OrigLS.prologueRegion().contains(BB))
      continue;
    LLVM_DEBUG(dbgs() << "    Unexpected outer-loop block: " << BB->getName()
                      << "\n");
    return std::nullopt;
  }

  LLVM_DEBUG(dbgs() << "    Prologue region: " << OrigLS.prologueRegion().size()
                    << " block(s); epilogue in outer.latch\n");

  if (!OrigLS.tryAdjustLoopBound()) {
    LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound\n");
    return std::nullopt;
  }
  return OrigLS;
}

bool LoopStructure::discoverPrologueRegion() {
  assert(isOrigLS() && "Only valid on the original LoopStructure");
  if (getInnerPreheader() != getOuterHeader()) {
    LLVM_DEBUG(dbgs() << "    Inner preheader != outer header\n");
    return false;
  }
  prologueRegion().assign({getOuterHeader()});
  return true;
}

bool LoopStructure::isInnerLoopHardwareLoop() const {
  assert(isOrigLS() && "Only valid on the original LoopStructure");
  auto *BI = dyn_cast<BranchInst>(getInnerLatch()->getTerminator());
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

SmallVector<LoadInst *, 8> LoopStructure::collectPrologueLoads() const {
  assert(isOrigLS() && "Only valid on the original LoopStructure");
  // The prologue is the single outer header block.
  SmallVector<LoadInst *, 8> Loads;
  for (Instruction &I : *getOuterHeader())
    if (auto *L = dyn_cast<LoadInst>(&I))
      Loads.push_back(L);
  return Loads;
}

SmallVector<StoreInst *, 8> LoopStructure::collectEpilogueStores() const {
  assert(isOrigLS() && "Only valid on the original LoopStructure");
  // The epilogue is the single outer latch block.
  SmallVector<StoreInst *, 8> Stores;
  for (Instruction &I : *getOuterLatch())
    if (auto *S = dyn_cast<StoreInst>(&I))
      Stores.push_back(S);
  return Stores;
}

// Populate VMap with the incoming values of outer header PHIs from FromBlock.
void AIEOuterLoopPipeliner::populateVMapFromPHIs(ValueToValueMapTy &VMap,
                                                 const LoopStructure &LS,
                                                 BasicBlock *FromBlock) const {
  for (PHINode &PHI : LS.getOuterHeader()->phis()) {
    int Idx = PHI.getBasicBlockIndex(FromBlock);
    if (Idx >= 0)
      VMap[&PHI] = PHI.getIncomingValue(Idx);
  }
}

bool LoopStructure::isProfitableToRotate(ScalarEvolution &SE,
                                         unsigned MinTripCount) const {
  assert(isOrigLS() && "Only valid on the original LoopStructure");
  if (!isInnerLoopHardwareLoop()) {
    LLVM_DEBUG(dbgs() << "    Inner loop is not a hardware loop\n");
    return false;
  }
  std::optional<int64_t> MinTC = llvm::getMinTripCount(getOuterLoop(), &SE);
  if (!MinTC || *MinTC < (int64_t)MinTripCount) {
    LLVM_DEBUG(dbgs() << "    Trip count too low\n");
    return false;
  }

  SmallVector<StoreInst *, 8> EpilogueStores = collectEpilogueStores();
  // TODO: Do we actually need this? Do we have cases without
  // stores?
  if (EpilogueStores.empty()) {
    LLVM_DEBUG(dbgs() << "    No stores in epilogue\n");
    return false;
  }
  return true;
}

bool LoopStructure::isSafeToReorderMemoryOps() const {
  assert(isOrigLS() && "Only valid on the original LoopStructure");
  // TODO: Add alias/dependence analysis to verify that moving prologue loads
  // before epilogue stores is safe. For now, only reject volatile/atomic
  // operations which can never be reordered.
  SmallVector<LoadInst *, 8> PrologueLoads = collectPrologueLoads();
  SmallVector<StoreInst *, 8> EpilogueStores = collectEpilogueStores();
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

// Collect the peeled instructions from the outer header that feed the inner
// loop. Uses backward value tracking from inner loop operands. Hardware-loop
// setup calls (@llvm.set.loop.iterations) are intentionally excluded — they
// stay in the outer header and are cloned separately into the last-iteration
// block.
void LoopStructure::collectPeeledInstructions() {
  assert(isOrigLS() && "Only valid on the original LoopStructure");
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
      peeledInsts().push_back(I);
  });
}

SmallVector<Instruction *, 16>
AIEOuterLoopPipeliner::remapToClone(ArrayRef<Instruction *> Insts,
                                    const ValueToValueMapTy &VMap) {
  SmallVector<Instruction *, 16> Out;
  for (Instruction *I : Insts) {
    auto It = VMap.find(I);
    Out.push_back(It != VMap.end()
                      ? cast<Instruction>(static_cast<Value *>(It->second))
                      : I);
  }
  return Out;
}

void AIEOuterLoopPipeliner::clonePrologueAsPeel(
    const LoopStructure &OrigLS, LoopStructure &SteadyLS,
    const ValueToValueMapTy &SteadyVMap, ValueToValueMapTy &PeelVMap) {
  Function *F = SteadyLS.getOuterHeader()->getParent();
  BasicBlock *Preheader = SteadyLS.getPreheader();

  // Seed PeelVMap with the entry (preheader) values of the outer header PHIs so
  // the peel's loads use the entry pointers.
  populateVMapFromPHIs(PeelVMap, SteadyLS, Preheader);

  BasicBlock *Peel = BasicBlock::Create(F->getContext(), "steady.preheader", F,
                                        SteadyLS.getOuterHeader());
  PeelVMap[SteadyLS.getOuterHeader()] = Peel;
  cloneAndRemapInsts(remapToClone(OrigLS.peeledInsts(), SteadyVMap), *Peel,
                     Peel->end(), PeelVMap, ".peel");

  BranchInst::Create(SteadyLS.getOuterHeader(), Peel);
  Preheader->getTerminator()->replaceSuccessorWith(SteadyLS.getOuterHeader(),
                                                   Peel);

  // The peel is now the preheader for the steady nest.
  SteadyLS.adoptPeelAsPreheader(Peel);
  LLVM_DEBUG(dbgs() << "    Created peel block: " << Peel->getName() << "\n");
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
  Function *F = SrcLS.getOuterHeader()->getParent();

  // The nest in program order: outer header (== inner preheader for the linear
  // prologue), the inner-loop blocks, then the outer latch (== inner exit).
  // Clone each block; CloneBasicBlock copies all instructions and seeds VMap
  // with per-instruction old->new entries.
  SmallVector<BasicBlock *, 8> OrigBlocks;
  OrigBlocks.push_back(SrcLS.getOuterHeader());
  OrigBlocks.append(SrcLS.getInnerBlocks().begin(),
                    SrcLS.getInnerBlocks().end());
  // OuterLatch == InnerExit (validated in analyzeLoopStructure), so it is not
  // in InnerBlocks; append it once.
  OrigBlocks.push_back(SrcLS.getOuterLatch());

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
  for (BasicBlock *BB : SrcLS.getInnerBlocks())
    CloneInnerBlocks.push_back(MapBlock(BB));

  LoopStructure CloneLS(
      /*OuterPreheader=*/nullptr, MapBlock(SrcLS.getOuterHeader()),
      MapBlock(SrcLS.getOuterLatch()), MapBlock(SrcLS.getInnerPreheader()),
      MapBlock(SrcLS.getInnerHeader()), MapBlock(SrcLS.getInnerLatch()),
      MapBlock(SrcLS.getInnerExit()), CloneInnerBlocks, SrcLS.getOuterLoopID());

  // Mirror the prologue/epilogue regions onto the clone blocks so membership
  // queries (isPipelineableValue, isInEpilogue) work on the clone.
  SmallVector<BasicBlock *, 4> CloneProBlocks;
  for (BasicBlock *BB : SrcLS.prologueRegion().blocks())
    CloneProBlocks.push_back(MapBlock(BB));
  CloneLS.prologueRegion().assign(CloneProBlocks);
  CloneLS.epilogueRegion().assign({MapBlock(SrcLS.getOuterLatch())});

  // Copy the cached latch-bound; its instruction pointers still reference the
  // original nest and are remapped to the clone by remapBoundToClone.
  CloneLS.bound() = SrcLS.bound();

  // Give the clone blocks clean role-based names (the "<orig>.<suffix>" names
  // from CloneBasicBlock read as if they were still the original loop).
  CloneLS.getOuterHeader()->setName(SuffixStr + ".header");
  CloneLS.getOuterLatch()->setName(SuffixStr + ".latch");
  for (auto [Orig, Clone] :
       zip(SrcLS.getInnerBlocks(), CloneLS.getInnerBlocks()))
    Clone->setName(SuffixStr + "." + Orig->getName());

  return CloneLS;
}

void AIEOuterLoopPipeliner::swapInClonedNest(
    const LoopStructure &OrigLS, LoopStructure &SteadyLS,
    const ValueToValueMapTy &SteadyVMap) const {
  BasicBlock *Preheader = OrigLS.getPreheader();
  BasicBlock *OrigExit = OrigLS.getExitBlock();

  // Redirect the preheader to the clone's header. This leaves the original nest
  // unreachable, so capture the preheader on the clone now — afterwards
  // LoopInfo can no longer recover it for the original.
  Preheader->getTerminator()->replaceSuccessorWith(OrigLS.getOuterHeader(),
                                                   SteadyLS.getOuterHeader());
  SteadyLS.setOuterPreheader(Preheader);

  // The clone's latch exit edge still points at OrigExit (left external by
  // cloneLoopNest); that is correct. Repoint the exit's loop-carried PHIs from
  // the original latch to the clone's latch, mapping each incoming value
  // through the clone VMap so the exit sees the clone's definitions.
  for (PHINode &PHI : OrigExit->phis()) {
    int Idx = PHI.getBasicBlockIndex(OrigLS.getOuterLatch());
    if (Idx < 0)
      continue;
    Value *V = PHI.getIncomingValue(Idx);
    auto It = SteadyVMap.find(V);
    if (It != SteadyVMap.end())
      PHI.setIncomingValue(Idx, It->second);
    PHI.setIncomingBlock(Idx, SteadyLS.getOuterLatch());
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
  LatchConditionInfo &B = SteadyLS.bound();
  B.Cmp = cast<ICmpInst>(Map(B.Cmp));
  // A loop-invariant Limit is not in the VMap and maps to itself.
  B.Limit = Map(B.Limit);
  B.Counter = cast_or_null<BinaryOperator>(Map(B.Counter));
  B.OldIV = cast_or_null<PHINode>(Map(B.OldIV));
}

void LoopStructure::removeFromCFG() const {
  assert(isOrigLS() && "Only valid on the original LoopStructure");
  SmallVector<BasicBlock *, 8> Dead;
  Dead.push_back(getOuterHeader());
  Dead.append(getInnerBlocks().begin(), getInnerBlocks().end());
  Dead.push_back(getOuterLatch());
  DeleteDeadBlocks(Dead);
}

// Clone the peeled instructions into the epilogue block (outer latch) before
// its terminator. The clones use the next-iteration pointer values (the latch
// incoming values of the outer header PHIs) so the loads prefetch for the next
// outer iteration.
void AIEOuterLoopPipeliner::clonePrologueIntoEpilogue(
    const LoopStructure &OrigLS, const LoopStructure &SteadyLS,
    const ValueToValueMapTy &SteadyVMap, ValueToValueMapTy &EpiVMap) {
  // Seed EpiVMap with the next-iteration (latch incoming) values of the outer
  // header PHIs so the cloned loads prefetch the next iteration's pointers.
  populateVMapFromPHIs(EpiVMap, SteadyLS, SteadyLS.getOuterLatch());

  SmallVector<Instruction *, 16> PeeledInsts =
      remapToClone(OrigLS.peeledInsts(), SteadyVMap);
  Instruction *LatchTerm = SteadyLS.getOuterLatch()->getTerminator();
  cloneAndRemapInsts(PeeledInsts, *SteadyLS.getOuterLatch(),
                     LatchTerm->getIterator(), EpiVMap, ".epi");
  LLVM_DEBUG(dbgs() << "    Cloned prologue into epilogue\n");
}

// For each steady peeled instruction, create a PHI selecting the peel value on
// the entry edge and the epilogue value on the back edge, then replace the
// instruction's uses with it and erase it.
void AIEOuterLoopPipeliner::createPipelinedPHIs(
    const LoopStructure &OrigLS, const LoopStructure &SteadyLS,
    const ValueToValueMapTy &SteadyVMap, const ValueToValueMapTy &PeelVMap,
    const ValueToValueMapTy &EpiVMap) {
  BasicBlock *Peel = SteadyLS.getPreheader();
  Instruction *InsertPt = &*SteadyLS.getOuterHeader()->getFirstInsertionPt();

  SmallVector<Instruction *, 16> PeeledInsts =
      remapToClone(OrigLS.peeledInsts(), SteadyVMap);
  SmallVector<std::pair<Instruction *, PHINode *>, 8> Replacements;
  for (Instruction *I : PeeledInsts) {
    // Void-typed instructions (stores, side-effect-only intrinsics) don't
    // produce values, so there's nothing to merge via a PHI node. Each
    // execution path simply runs its own cloned copy independently.
    if (I->getType()->isVoidTy())
      continue;
    auto WIt = PeelVMap.find(I);
    auto EIt = EpiVMap.find(I);
    // Both cloning functions (clonePrologueAsPeel, clonePrologueIntoEpilogue)
    // unconditionally add every peeled entry to their respective maps, so
    // every non-void instruction must be present in both.
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
  for (Instruction *I : reverse(PeeledInsts)) {
    if (I->use_empty())
      I->eraseFromParent();
  }
}

// Create the last-iteration region for the last outer iteration (N-1).
void AIEOuterLoopPipeliner::peelLastIteration(
    const LoopStructure &OrigLS, const LoopStructure &SteadyLS,
    const ValueToValueMapTy &SteadyVMap) {
  // Seed the last-iteration map with each outer header PHI's latch incoming
  // value, so every clone below picks up the final epilogue values.
  ValueToValueMapTy LastIterVMap;
  populateVMapFromPHIs(LastIterVMap, SteadyLS, SteadyLS.getOuterLatch());

  // Create the empty last-iteration nest (prologue + inner-loop skeleton +
  // epilogue), then fill each block in. The prologue holds the hardware-loop
  // setup and the kept accumulator seeds; both must be in place before the
  // inner loop is filled so its PHIs that reference kept results resolve.
  LoopStructure LastIterLS = createLastIterSkeleton(SteadyLS, LastIterVMap);
  cloneHardwareLoopSetupInto(LastIterLS, SteadyLS, LastIterVMap);
  SmallVector<Instruction *, 16> KeptInsts =
      remapToClone(OrigLS.keptInsts(), SteadyVMap);
  cloneAndRemapInsts(KeptInsts, *LastIterLS.getOuterHeader(),
                     LastIterLS.getOuterHeader()->end(), LastIterVMap,
                     ".lastiter");
  cloneInnerLoopIntoLastIter(SteadyLS, LastIterLS, LastIterVMap);
  populateLastIterEpilogue(LastIterLS, SteadyLS, LastIterVMap);
  wireLastIterIntoCFG(SteadyLS, LastIterLS);

  LLVM_DEBUG(dbgs() << "    Created last-iteration: "
                    << LastIterLS.getOuterHeader()->getName() << " -> "
                    << LastIterLS.getOuterLatch()->getName() << "\n");
}

void AIEOuterLoopPipeliner::cloneHardwareLoopSetupInto(
    const LoopStructure &LastIterLS, const LoopStructure &SteadyLS,
    ValueToValueMapTy &LastIterVMap) const {
  SmallVector<Instruction *, 4> SetupInsts;
  for (Instruction &I : *SteadyLS.getOuterHeader()) {
    if (I.isTerminator())
      break;
    if (AIEIRUtils::isHardwareLoopSetup(&I))
      SetupInsts.push_back(&I);
  }
  BasicBlock *Dest = LastIterLS.getOuterHeader();
  cloneAndRemapInsts(SetupInsts, *Dest, Dest->end(), LastIterVMap,
                     /*Suffix=*/"");
}

LoopStructure AIEOuterLoopPipeliner::createLastIterSkeleton(
    const LoopStructure &SteadyLS, ValueToValueMapTy &LastIterVMap) const {
  Function *F = SteadyLS.getOuterHeader()->getParent();
  LLVMContext &Ctx = F->getContext();

  // The last-iteration nest is spliced just before the steady loop's exit
  // successor; all its blocks are created before that block.
  BasicBlock *OrigExit = SteadyLS.getExitBlock();

  // lastiter.prologue is the last-iteration outer header (and, for the linear
  // nest, its inner preheader); inner-loop PHIs incoming from the outer header
  // now come from it.
  BasicBlock *LastIterPrologue =
      BasicBlock::Create(Ctx, "lastiter.prologue", F, OrigExit);
  LastIterVMap[SteadyLS.getOuterHeader()] = LastIterPrologue;

  // One empty clone per inner-loop block, recorded in LastIterVMap so
  // cloneInnerLoopIntoLastIter can resolve each block to its clone.
  SmallVector<BasicBlock *, 4> LastIterInnerBlocks;
  for (BasicBlock *BB : SteadyLS.getInnerBlocks()) {
    BasicBlock *Clone =
        BasicBlock::Create(Ctx, BB->getName() + ".lastiter", F, OrigExit);
    LastIterVMap[BB] = Clone;
    LastIterInnerBlocks.push_back(Clone);
  }

  // lastiter.epilogue is the last-iteration outer latch; it receives the steady
  // latch (and, in the single-block case, the inner exit) so the cloned inner
  // loop and epilogue resolve their exits to it.
  BasicBlock *LastIterEpilogue =
      BasicBlock::Create(Ctx, "lastiter.epilogue", F, OrigExit);
  LastIterVMap[SteadyLS.getOuterLatch()] = LastIterEpilogue;
  if (SteadyLS.getInnerExit() == SteadyLS.getOuterLatch())
    LastIterVMap[SteadyLS.getInnerExit()] = LastIterEpilogue;

  auto MapBlock = [&](BasicBlock *BB) {
    return cast<BasicBlock>(LastIterVMap[BB]);
  };
  return LoopStructure(
      /*OuterPreheader=*/nullptr, LastIterPrologue, LastIterEpilogue,
      /*InnerPreheader=*/LastIterPrologue, MapBlock(SteadyLS.getInnerHeader()),
      MapBlock(SteadyLS.getInnerLatch()), LastIterEpilogue, LastIterInnerBlocks,
      SteadyLS.getOuterLoopID());
}

void AIEOuterLoopPipeliner::cloneInnerLoopIntoLastIter(
    const LoopStructure &SteadyLS, const LoopStructure &LastIterLS,
    ValueToValueMapTy &LastIterVMap) const {
  // Clone every inner-loop block's body into its skeleton clone (resolved via
  // LastIterVMap) first, so all cross-block references exist before any remap
  // runs.
  SmallVector<Instruction *, 32> Clones;
  for (BasicBlock *Orig : SteadyLS.getInnerBlocks()) {
    auto *Clone = cast<BasicBlock>(LastIterVMap[Orig]);
    for (Instruction &Inst : *Orig)
      Clones.push_back(
          cloneInstInto(Inst, *Clone, Clone->end(), LastIterVMap, ".lastiter"));
  }
  remapClones(Clones, LastIterVMap);

  BranchInst::Create(LastIterLS.getInnerHeader(), LastIterLS.getOuterHeader());
}

void AIEOuterLoopPipeliner::populateLastIterEpilogue(
    const LoopStructure &LastIterLS, const LoopStructure &SteadyLS,
    ValueToValueMapTy &LastIterVMap) const {
  const SmallPtrSetImpl<Instruction *> &EpiSnapshot =
      SteadyLS.epilogueSnapshot();
  SmallVector<Instruction *, 16> EpiInsts;
  for (Instruction &I : *SteadyLS.getOuterLatch()) {
    if (I.isTerminator())
      break;
    // No prefetch in the last-iteration.
    if (isa<LoadInst>(&I))
      continue;
    // Skip the prologue clones inserted into the epilogue earlier.
    if (!EpiSnapshot.count(&I))
      continue;
    EpiInsts.push_back(&I);
  }
  BasicBlock *LastIterEpilogue = LastIterLS.getOuterLatch();
  cloneAndRemapInsts(EpiInsts, *LastIterEpilogue, LastIterEpilogue->end(),
                     LastIterVMap, ".lastiter");
}

void AIEOuterLoopPipeliner::wireLastIterIntoCFG(
    const LoopStructure &SteadyLS, const LoopStructure &LastIterLS) const {
  BasicBlock *OrigExit = SteadyLS.getExitBlock();
  BasicBlock *LastIterEpilogue = LastIterLS.getOuterLatch();
  BranchInst::Create(OrigExit, LastIterEpilogue);

  for (PHINode &PHI : OrigExit->phis()) {
    const int LatchIdx = PHI.getBasicBlockIndex(SteadyLS.getOuterLatch());
    assert(LatchIdx >= 0);
    PHI.setIncomingBlock(LatchIdx, LastIterEpilogue);
  }

  BranchInst *LatchBr = SteadyLS.getLatchBranch();
  for (unsigned I = 0; I < LatchBr->getNumSuccessors(); ++I) {
    if (LatchBr->getSuccessor(I) == OrigExit) {
      LatchBr->setSuccessor(I, LastIterLS.getOuterHeader());
      break;
    }
  }
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

LoopStructure::LoopStructure(Loop *L)
    : OuterLoop(L), InnerLoop(L->getSubLoops()[0]), IsOrigLS(true),
      InnerPreheader(InnerLoop->getLoopPreheader()),
      InnerHeader(InnerLoop->getHeader()),
      InnerLatch(InnerLoop->getLoopLatch()),
      InnerExit(InnerLoop->getExitBlock()),
      OuterPreheader(L->getLoopPreheader()),
      InnerLoopBlocks(InnerLoop->block_begin(), InnerLoop->block_end()),
      OuterLoopID(L->getLoopID()) {
  assert(L->getSubLoops().size() == 1 && "Requires exactly one subloop");
  assert(L->getLoopLatch() && "Requires single outer latch");
  PrologueRegion.assign({L->getHeader()});
  EpilogueRegion.assign({L->getLoopLatch()});
}

LoopStructure::LoopStructure(BasicBlock *OuterPreheader,
                             BasicBlock *OuterHeader, BasicBlock *OuterLatch,
                             BasicBlock *InnerPreheader,
                             BasicBlock *InnerHeader, BasicBlock *InnerLatch,
                             BasicBlock *InnerExit,
                             ArrayRef<BasicBlock *> InnerBlocks,
                             MDNode *OuterLoopID)
    : IsOrigLS(false), InnerPreheader(InnerPreheader), InnerHeader(InnerHeader),
      InnerLatch(InnerLatch), InnerExit(InnerExit),
      OuterPreheader(OuterPreheader),
      InnerLoopBlocks(InnerBlocks.begin(), InnerBlocks.end()),
      OuterLoopID(OuterLoopID) {
  PrologueRegion.assign({OuterHeader});
  EpilogueRegion.assign({OuterLatch});
}

SmallVectorImpl<Instruction *> &LoopStructure::peeledInsts() {
  assert(IsOrigLS && "peeled/kept lists exist only on the original nest");
  return PeeledInsts;
}

const SmallVectorImpl<Instruction *> &LoopStructure::peeledInsts() const {
  assert(IsOrigLS && "peeled/kept lists exist only on the original nest");
  return PeeledInsts;
}

SmallVectorImpl<Instruction *> &LoopStructure::keptInsts() {
  assert(IsOrigLS && "peeled/kept lists exist only on the original nest");
  return KeptInsts;
}

const SmallVectorImpl<Instruction *> &LoopStructure::keptInsts() const {
  assert(IsOrigLS && "peeled/kept lists exist only on the original nest");
  return KeptInsts;
}

void LoopStructure::setOuterPreheader(BasicBlock *BB) {
  assert(!IsOrigLS && "Original derives its preheader from LoopInfo");
  OuterPreheader = BB;
}

bool LoopStructure::isRegionInternalPhi(const PHINode *PHI) const {
  for (const BasicBlock *IB : PHI->blocks())
    if (!PrologueRegion.contains(IB))
      return false;
  return true;
}

bool LoopStructure::isPipelineableValue(const Instruction *I) const {
  if (!isInPrologue(I))
    return false;
  if (const auto *PHI = dyn_cast<PHINode>(I))
    return isRegionInternalPhi(PHI);
  return true;
}

bool LoopStructure::isPipelineCandidate(const Instruction *I) const {
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

void LoopStructure::adoptPeelAsPreheader(BasicBlock *Peel) {
  BasicBlock *OldPreheader = getPreheader();
  for (PHINode &PHI : getOuterHeader()->phis()) {
    const int PreIdx = PHI.getBasicBlockIndex(OldPreheader);
    assert(PreIdx >= 0);
    PHI.setIncomingBlock(PreIdx, Peel);
  }
  setOuterPreheader(Peel);
}

bool LoopStructure::tryAdjustLoopBound() {
  auto *BI = dyn_cast<BranchInst>(getOuterLatch()->getTerminator());
  if (!BI || !BI->isConditional())
    return false;
  auto *Cmp = dyn_cast<ICmpInst>(BI->getCondition());
  if (!Cmp)
    return false;
  ICmpInst::Predicate Pred = Cmp->getPredicate();

  // Find the loop-invariant limit and non-invariant counter.
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
      return false;
    }
  }

  // CounterAdd must be non-null: the step must come from an explicit add
  // instruction so that we can identify the counting PHI (OldIV) and verify
  // the loop induction structure. Plain-PHI counters (no visible add) are
  // rejected to avoid relying on an unverifiable step assumption.
  if (!CounterAdd) {
    LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound: counter is not an add "
                         "instruction\n");
    return false;
  }

  // Find the counting PHI in the outer header that feeds CounterAdd.
  // Without OldIV we cannot verify the induction structure or support JNZD.
  PHINode *OldIV = nullptr;
  for (Value *Op : CounterAdd->operands()) {
    if (auto *PHI = dyn_cast<PHINode>(Op)) {
      if (PHI->getParent() == getOuterHeader()) {
        OldIV = PHI;
        break;
      }
    }
  }
  if (!OldIV) {
    LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound: counting PHI not "
                         "found in outer header\n");
    return false;
  }

  OuterLoopCondition = {Cmp, Limit, LimitIdx, Step, CounterAdd, OldIV};
  return true;
}

// Adjust the outer loop trip count from N to N-1 using the pre-computed
// LS.bound() (populated by canAdjustLoopBound). The unified formula is:
//   NewLimit = Limit - Step
// which correctly handles increment (Step > 0) and decrement (Step < 0) loops
// of any constant step magnitude.
Value *LoopStructure::adjustLoopBound() {
  // bound() is always valid (set by analyzeLoopStructure).
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

// Adjust loop metadata after a successful outer-loop pipelining transformation:
//   1. Decrement llvm.loop.itercount.range by 1 (one iteration was peeled).
//   2. Drop llvm.loop.hint.aie-enable-outer-loop-pipelining (consumed).
//   3. Insert llvm.loop.hint.aie_outerloop_pipeliner_success = i64 1.
void LoopStructure::updateLoopMetadata() const {
  MDNode *LoopID = getOuterLoopID();
  LLVMContext &Ctx = getOuterHeader()->getContext();

  // Adjust itercount.range (N → N-1).
  MDNode *UpdatedID =
      LoopID ? updateIterCounts(
                   Ctx, LoopID, /*FixMin=*/[](int64_t V) { return V - 1; },
                   /*FixMax=*/[](int64_t V) { return V - 1; })
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
  // Write llvm.loop onto the outer latch terminator (what Loop::setLoopID does
  // internally), so this works on a clone with no LoopInfo Loop.
  getOuterLatch()->getTerminator()->setMetadata(LLVMContext::MD_loop,
                                                FinalLoopID);
}

bool LoopStructure::collectPeeledForSplit(
    function_ref<bool(const Instruction *)> IsAnchor) {
  assert(isOrigLS() && "Only valid on the original LoopStructure");
  // Forward-track from loads to find anchors, then collect all their
  // descendants (the kept set). Peeled = everything else: prologue pipeline
  // candidates that are neither anchors nor anchor descendants.
  SmallPtrSet<Instruction *, 32> ReachableFromLoad;
  SmallVector<Instruction *, 32> FwdWorklist;
  prologueRegion().forEachInstruction([&](Instruction *I) {
    if (isa<LoadInst>(I)) {
      ReachableFromLoad.insert(I);
      FwdWorklist.push_back(I);
    }
  });
  while (!FwdWorklist.empty()) {
    Instruction *I = FwdWorklist.pop_back_val();
    for (User *U : I->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      if (!UI || !isPipelineableValue(UI))
        continue;
      if (ReachableFromLoad.insert(UI).second)
        FwdWorklist.push_back(UI);
    }
  }

  // Find anchors = instructions matching any split strategy in
  // ReachableFromLoad.
  SmallPtrSet<Instruction *, 16> Anchors;
  for (Instruction *I : ReachableFromLoad)
    if (IsAnchor(I))
      Anchors.insert(I);

  if (Anchors.empty())
    return false;

  // Find all descendants of anchors within the prologue (the kept set).
  SmallPtrSet<Instruction *, 32> KeptSet;
  KeptSet.insert(Anchors.begin(), Anchors.end());
  SmallVector<Instruction *, 16> DescWorklist(Anchors.begin(), Anchors.end());
  while (!DescWorklist.empty()) {
    Instruction *I = DescWorklist.pop_back_val();
    for (User *U : I->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      if (!UI || !isPipelineableValue(UI))
        continue;
      if (KeptSet.insert(UI).second)
        DescWorklist.push_back(UI);
    }
  }

  // Peeled = region pipeline candidates not in the kept set (the load/address
  // chain; the post-anchor cone is kept). Loop-carried PHIs, terminators, and
  // hardware-loop setup are excluded by isPipelineCandidate.
  prologueRegion().forEachInstruction([&](Instruction *I) {
    if (isPipelineCandidate(I) && !KeptSet.count(I))
      peeledInsts().push_back(I);
  });

  LLVM_DEBUG(dbgs() << "    Split-prologue: " << Anchors.size()
                    << " Number of anchor(s), " << peeledInsts().size()
                    << " peeled instructions\n");
  return true;
}

// Lift each outer-header PHI's epilogue-defined pointer-update chain to the end
// of the prologue, so the main transform clones them into peel and epilogue.
// Only chains with no inner-loop dependency and no external epilogue use lift.
bool AIEOuterLoopPipeliner::liftEpiloguePointerUpdatesToPrologue(
    const LoopStructure &OrigLS) {
  Instruction *InsertPt = OrigLS.getOuterHeader()->getTerminator();

  // Collect all liftable chains across all PHIs.
  SmallPtrSet<Instruction *, 32> AllLiftable;

  // Process each PHI independently to allow partial lifting.
  for (PHINode &PHI : OrigLS.getOuterHeader()->phis()) {
    // Get the incoming value from the latch (back-edge).
    Value *LatchVal = PHI.getIncomingValueForBlock(OrigLS.getOuterLatch());
    auto *LatchInst = dyn_cast<Instruction>(LatchVal);
    if (!LatchInst || !OrigLS.isInEpilogue(LatchInst))
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
        if (OrigLS.getInnerLoop()->contains(OpI->getParent())) {
          CanLift = false;
          break;
        }
        // If the operand is defined in the epilogue, add to the chain.
        if (OrigLS.isInEpilogue(OpI)) {
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
          if (OrigLS.isInEpilogue(UI) && !Chain.count(UI)) {
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
  for (Instruction &I : *OrigLS.getOuterLatch())
    if (AllLiftable.count(&I))
      ToLift.push_back(&I);

  // Move each instruction to the end of the prologue block (before terminator).
  for (Instruction *I : ToLift)
    I->moveBefore(InsertPt->getIterator());

  LLVM_DEBUG(dbgs() << "    Lifted " << ToLift.size()
                    << " instructions from epilogue to prologue\n");
  return true;
}

void LoopStructure::collectKeptInstructions(
    function_ref<bool(const Instruction *)> IsAnchor) {
  assert(isOrigLS() && "Only valid on the original LoopStructure");
  SmallPtrSet<Instruction *, 32> PeeledSet;
  PeeledSet.insert(peeledInsts().begin(), peeledInsts().end());

  // Find anchors: instructions matching any split strategy that are direct
  // users of peeled instructions (or transitively reachable from the peeled set
  // within the prologue).
  SmallPtrSet<Instruction *, 32> KeptSet;
  SmallVector<Instruction *, 16> Worklist;

  // Seed: forward-track from peeled instructions to find anchor instructions.
  for (Instruction *P1 : PeeledSet) {
    for (User *U : P1->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      if (!UI || !isPipelineableValue(UI))
        continue;
      if (!PeeledSet.count(UI) && IsAnchor(UI)) {
        if (KeptSet.insert(UI).second)
          Worklist.push_back(UI);
      }
    }
  }

  // Forward-track from anchors to collect all descendants within the prologue.
  while (!Worklist.empty()) {
    Instruction *I = Worklist.pop_back_val();
    for (User *U : I->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      if (!UI || !isPipelineableValue(UI))
        continue;
      if (!PeeledSet.count(UI) && KeptSet.insert(UI).second)
        Worklist.push_back(UI);
    }
  }

  // Emit KeptSet in region program order.
  prologueRegion().forEachInstruction([&](Instruction *I) {
    if (KeptSet.count(I))
      keptInsts().push_back(I);
  });

  LLVM_DEBUG(dbgs() << "    Split-prologue: " << keptInsts().size()
                    << " kept instructions (stay in outer.header + "
                       "lastiter.prologue)\n");
}

bool AIEOuterLoopPipeliner::performTransformation(
    LoopStructure &OrigLS, const AIE::LoopOptionOverrides &Overrides) {
  // Lift pointer update instructions from epilogue to prologue.
  // This must happen BEFORE collecting prologue instructions so that the
  // lifted instructions are included in the peeled chain.
  liftEpiloguePointerUpdatesToPrologue(OrigLS);

  // Collect the peeled (and, in split-prologue mode, kept) instructions into
  // OrigLS. Hardware-loop setup calls (set.loop.iterations) are excluded.
  const bool SplitMode = Overrides.get(SplitPrologue);
  if (SplitMode && OrigLS.collectPeeledForSplit(isAnchorInstruction)) {
    LLVM_DEBUG(dbgs() << "    Split-prologue: pipelining "
                      << OrigLS.peeledInsts().size()
                      << " peeled instructions\n");
    // The kept set forward-tracks from the peeled set, so collect it while both
    // still live on the original nest.
    OrigLS.collectKeptInstructions(isAnchorInstruction);
  } else {
    if (SplitMode)
      LLVM_DEBUG(dbgs() << "    Split-prologue: no split points\n");
    OrigLS.collectPeeledInstructions();
  }
  if (OrigLS.peeledInsts().empty()) {
    LLVM_DEBUG(dbgs() << "    No prologue instructions found\n");
    return false;
  }

  // Clone the whole nest into a fresh, steady-state copy and swap it into the
  // CFG in the original's place (preheader -> clone header -> ... -> exit). The
  // clone inherits the cached bound (remapped to its blocks); all transform
  // steps below run on the clone so the original is never mutated and is
  // deleted at the end.
  ValueToValueMapTy SteadyVMap;
  LoopStructure SteadyLS = cloneLoopNest(OrigLS, "steady", SteadyVMap);
  swapInClonedNest(OrigLS, SteadyLS, SteadyVMap);
  remapBoundToClone(SteadyLS, SteadyVMap);

  // Snapshot the steady epilogue (latch) contents now, before any prefetch
  // clones are inserted into it, so the last-iteration epilogue can later be
  // filtered back to the original stores.
  for (Instruction &I : *SteadyLS.getOuterLatch())
    SteadyLS.epilogueSnapshot().insert(&I);

  // Peel the peeled chain before the steady header (entry pointer values); this
  // also adopts the peel as the steady preheader.
  ValueToValueMapTy PeelVMap;
  clonePrologueAsPeel(OrigLS, SteadyLS, SteadyVMap, PeelVMap);

  // Prefetch the peeled chain in the epilogue (next-iteration pointer values).
  ValueToValueMapTy EpiVMap;
  clonePrologueIntoEpilogue(OrigLS, SteadyLS, SteadyVMap, EpiVMap);

  // Merge the peel and epilogue copies into the steady header via PHIs.
  createPipelinedPHIs(OrigLS, SteadyLS, SteadyVMap, PeelVMap, EpiVMap);

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
  peelLastIteration(OrigLS, SteadyLS, SteadyVMap);

  // Adjust itercount metadata to reflect the reduced trip count.
  SteadyLS.updateLoopMetadata();

  OrigLS.removeFromCFG();

  return true;
}

// Returns the downcounting pattern for JNZD conversion if the latch has the
// canonical pattern (icmp eq/ne, step -1). bound() is always valid here, so
// only the step is checked.
std::optional<DowncountingInfo> LoopStructure::getDowncountingInfo() const {
  // Hardware loop requires step == -1 (loop.decrement.reg decrements by 1).
  if (bound().Step != -1)
    return std::nullopt;
  return DowncountingInfo{bound().Cmp, bound().Counter, bound().OldIV};
}

// Convert the outer loop to a JNZD hardware loop: replace the latch's
// downcounting add+icmp with start.loop.iterations / loop.decrement.reg.
void AIEOuterLoopPipeliner::convertOuterLoopToHardwareLoop(
    const LoopStructure &SteadyLS, const DowncountingInfo &Info) {
  LLVMContext &Ctx = SteadyLS.getOuterHeader()->getContext();
  Type *I32Ty = Type::getInt32Ty(Ctx);

  // Unpack the pre-validated downcounting pattern from Info. The JNZD trip
  // count is NOT the adjusted icmp threshold (the constant 1 for a decrement
  // loop); it is:
  //   OldIV_initial_value - 1
  // where OldIV_initial_value is the peel incoming of OldIV.
  PHINode *OldIV = Info.OldIV;

  // The JNZD trip count is the peel-incoming value of OldIV minus 1: for a
  // decrement loop starting at N, peeling one iteration leaves N-1 to run.
  BasicBlock *Peel = SteadyLS.getPreheader();
  IRBuilder<> PreBuilder(Peel->getTerminator());

  Value *InitN = OldIV->getIncomingValueForBlock(Peel);
  Value *TripCount = PreBuilder.CreateSub(
      InitN, ConstantInt::get(InitN->getType(), 1), "outer.jnzd.tc");
  // Ensure i32.
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
  // Use the pre-validated components from Info.
  BranchInst *LatchBr = SteadyLS.getLatchBranch();
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
  if (LatchBr->getSuccessor(0) != SteadyLS.getOuterHeader())
    LatchBr->swapSuccessors();

  // Complete the counter PHI with the latch back-edge value.
  CtrPHI->addIncoming(CtrNext, SteadyLS.getOuterLatch());

  // Delete the old icmp (now dead: branch condition was replaced above).
  RecursivelyDeleteTriviallyDeadInstructions(OldCond);

  // OldCounter (add) and OldIV (counting PHI) form a use cycle that trivial-DCE
  // cannot break. If OldIV is used only by OldCounter, poison its latch slot so
  // both become dead; otherwise leave the PHI for its other users.
  if (OldCounter) {
    if (OldIV->hasOneUse()) {
      int LatchIdx = OldIV->getBasicBlockIndex(SteadyLS.getOuterLatch());
      if (LatchIdx >= 0)
        OldIV->setIncomingValue(LatchIdx, PoisonValue::get(OldIV->getType()));
    }
    // OldIV may be deleted transitively here; do not reference it afterwards.
    RecursivelyDeleteTriviallyDeadInstructions(OldCounter);
  }

  LLVM_DEBUG(dbgs() << "    Converted outer loop to JNZD hardware loop\n");
}
