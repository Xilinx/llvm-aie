//===- AIELoopVersioning.cpp - Trip-count loop versioning -----------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Loop versioning for the AIE postpipeliner.
//
// A loop carrying the !llvm.loop.hint.aie-loop-versioning metadata is split
// into two copies guarded by a runtime trip-count check, so the postpipeliner
// can pipeline the fast copy as aggressively as it likes while small trip
// counts stay correct on the slow copy:
//
//   [preheader / guard]   if (tripcount < threshold) -> slow else -> fast
//          /     \
//   [slow.ph]   [fast.ph]
//       |           |
//    [slow]       [fast]     fast keeps the hint (postpipeliner pipelines it);
//       \           /        slow is the verbatim, un-pipelined loop.
//        \         /
//         [exit]
//
// The guard threshold is a placeholder here (the hint's trip-count target, or
// the minimum stage count when the hint asks us to derive it). The
// postpipeliner overwrites it with the real stage count once it has scheduled
// the fast copy (the IR marker does not survive ISel, so the patch is done
// structurally on the lowered guard). Running before HardwareLoops lets both
// copies lower to ZOL uniformly and keeps constant trip counts foldable to a
// single version.
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Metadata.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"

using namespace llvm;

#define DEBUG_TYPE "aie-loop-versioning"

namespace {

class AIELoopVersioning : public FunctionPass {
public:
  static char ID;
  AIELoopVersioning() : FunctionPass(ID) {}
  bool runOnFunction(Function &F) override;
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<ScalarEvolutionWrapperPass>();
    FunctionPass::getAnalysisUsage(AU);
  }
  StringRef getPassName() const override { return "AIE Loop Versioning"; }

private:
  LoopInfo *LI = nullptr;
  DominatorTree *DT = nullptr;
  ScalarEvolution *SE = nullptr;

  // True when the loop carries the versioning enable hint.
  bool isVersioningEnabled(const Loop *L) const;
  // Materialize the loop trip count at the preheader terminator, or nullptr if
  // it cannot be computed/expanded there.
  Value *expandTripCount(Loop *L, Instruction *InsertPt) const;
  // Clone L into a slow copy and route a runtime guard to either copy.
  bool versionLoop(Loop *L, int64_t Threshold);
  // Merge loop-defined values used after the loop across the two copies.
  void addExitPHIs(Loop *FastLoop, Loop *SlowLoop, BasicBlock *ExitBlock,
                   ValueToValueMapTy &VMap,
                   ArrayRef<Instruction *> DefsUsedOutside) const;
  // Remove the versioning hint so the postpipeliner leaves the slow copy alone.
  void stripVersioningHint(Loop *L) const;
};

} // end anonymous namespace

char AIELoopVersioning::ID = 0;

INITIALIZE_PASS_BEGIN(AIELoopVersioning, DEBUG_TYPE, "AIE Loop Versioning",
                      false, false)
INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
INITIALIZE_PASS_DEPENDENCY(DominatorTreeWrapperPass)
INITIALIZE_PASS_DEPENDENCY(ScalarEvolutionWrapperPass)
INITIALIZE_PASS_END(AIELoopVersioning, DEBUG_TYPE, "AIE Loop Versioning", false,
                    false)

FunctionPass *llvm::createAIELoopVersioningPass() {
  return new AIELoopVersioning();
}

bool AIELoopVersioning::isVersioningEnabled(const Loop *L) const {
  std::optional<int64_t> Hint = AIELoopUtils::getLoopHintInt(
      L->getLoopID(), AIELoopUtils::LoopVersioningHintKey);
  return Hint && *Hint > 0;
}

Value *AIELoopVersioning::expandTripCount(Loop *L,
                                          Instruction *InsertPt) const {
  const SCEV *BackedgeCount = SE->getBackedgeTakenCount(L);
  if (isa<SCEVCouldNotCompute>(BackedgeCount))
    return nullptr;
  const SCEV *TripCount =
      SE->getAddExpr(BackedgeCount, SE->getOne(BackedgeCount->getType()));
  SCEVExpander Exp(*SE, InsertPt->getDataLayout(), "lver.tc");
  if (!Exp.isSafeToExpandAt(TripCount, InsertPt))
    return nullptr;
  return Exp.expandCodeFor(TripCount, TripCount->getType(), InsertPt);
}

void AIELoopVersioning::addExitPHIs(
    Loop *FastLoop, Loop *SlowLoop, BasicBlock *ExitBlock,
    ValueToValueMapTy &VMap, ArrayRef<Instruction *> DefsUsedOutside) const {
  PHINode *PN;
  // Give every value used after the loop a merge PHI fed by the fast copy.
  for (Instruction *Inst : DefsUsedOutside) {
    for (auto I = ExitBlock->begin(); (PN = dyn_cast<PHINode>(I)); ++I)
      if (PN->getIncomingValue(0) == Inst)
        break;
    if (PN)
      continue;
    PN = PHINode::Create(Inst->getType(), 2, Inst->getName() + ".lver");
    PN->insertBefore(ExitBlock->begin());
    SmallVector<User *, 8> UsersToUpdate;
    for (User *U : Inst->users())
      if (!FastLoop->contains(cast<Instruction>(U)->getParent()))
        UsersToUpdate.push_back(U);
    for (User *U : UsersToUpdate)
      U->replaceUsesOfWith(Inst, PN);
    PN->addIncoming(Inst, FastLoop->getExitingBlock());
  }
  // Add the matching incoming value from the slow copy.
  for (auto I = ExitBlock->begin(); (PN = dyn_cast<PHINode>(I)); ++I) {
    Value *FastValue = PN->getIncomingValue(0);
    auto Mapped = VMap.find(FastValue);
    Value *SlowValue =
        Mapped != VMap.end() ? cast<Value>(Mapped->second) : FastValue;
    PN->addIncoming(SlowValue, SlowLoop->getExitingBlock());
  }
}

void AIELoopVersioning::stripVersioningHint(Loop *L) const {
  MDNode *LoopID = L->getLoopID();
  if (!LoopID)
    return;
  LLVMContext &Ctx = L->getHeader()->getContext();
  // Operand 0 of a loop ID is a self-reference; reserve it with a placeholder.
  SmallVector<Metadata *, 8> MDs(1, nullptr);
  for (unsigned I = 1, E = LoopID->getNumOperands(); I < E; ++I) {
    MDNode *Entry = cast<MDNode>(LoopID->getOperand(I));
    auto Key = AIELoopUtils::getMetadataKey(*Entry);
    if (Key && *Key == AIELoopUtils::LoopVersioningHintKey)
      continue;
    MDs.push_back(Entry);
  }
  MDNode *NewLoopID = MDNode::getDistinct(Ctx, MDs);
  NewLoopID->replaceOperandWith(0, NewLoopID);
  L->setLoopID(NewLoopID);
}

bool AIELoopVersioning::versionLoop(Loop *L, int64_t Threshold) {
  // We need simplify form for a unique preheader and a single merge point for
  // the two copies' results.
  if (!L->isLoopSimplifyForm())
    return false;
  // A unique exit and exiting block let addExitPHIs merge the two copies.
  BasicBlock *ExitBlock = L->getUniqueExitBlock();
  if (!ExitBlock || !L->getExitingBlock())
    return false;

  BasicBlock *GuardBB = L->getLoopPreheader();
  Value *TripCount = expandTripCount(L, GuardBB->getTerminator());
  if (!TripCount)
    return false;

  // Build the runtime check in the (soon to be) guard block: take the slow copy
  // when the trip count is below what the pipelined fast copy needs.
  IRBuilder<> Builder(GuardBB->getTerminator());
  Value *ThresholdVal = ConstantInt::get(TripCount->getType(), Threshold);
  Value *TakeSlow = Builder.CreateICmpULT(TripCount, ThresholdVal, "lver.slow");
  GuardBB->setName(L->getHeader()->getName() + ".lver.guard");

  SmallVector<Instruction *, 8> DefsUsedOutside = findDefsUsedOutsideOfLoop(L);

  // Split off an empty preheader for the fast (original) copy, then clone the
  // loop into the slow copy dominated by the guard block.
  BasicBlock *FastPH = SplitBlock(GuardBB, GuardBB->getTerminator(), DT, LI,
                                  nullptr, L->getHeader()->getName() + ".ph");
  ValueToValueMapTy VMap;
  SmallVector<BasicBlock *, 8> SlowBlocks;
  Loop *SlowLoop = cloneLoopWithPreheader(FastPH, GuardBB, L, VMap,
                                          ".lver.slow", LI, DT, SlowBlocks);
  remapInstructionsInBlocks(SlowBlocks, VMap);

  // Replace the guard's fall-through with the trip-count branch.
  Instruction *OrigTerm = GuardBB->getTerminator();
  Builder.SetInsertPoint(OrigTerm);
  Builder.CreateCondBr(TakeSlow, SlowLoop->getLoopPreheader(), FastPH);
  OrigTerm->eraseFromParent();

  DT->changeImmediateDominator(ExitBlock, GuardBB);
  addExitPHIs(L, SlowLoop, ExitBlock, VMap, DefsUsedOutside);
  formDedicatedExitBlocks(SlowLoop, DT, LI, nullptr, /*PreserveLCSSA=*/true);
  formDedicatedExitBlocks(L, DT, LI, nullptr, /*PreserveLCSSA=*/true);

  // The fast copy keeps the hint and gets pipelined; the slow copy is the
  // verbatim un-pipelined fallback.
  stripVersioningHint(SlowLoop);

  LLVM_DEBUG(dbgs() << "AIELoopVersioning: versioned " << L->getName()
                    << " with guard threshold " << Threshold << "\n");
  return true;
}

bool AIELoopVersioning::runOnFunction(Function &F) {
  if (skipFunction(F))
    return false;
  LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
  DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
  SE = &getAnalysis<ScalarEvolutionWrapperPass>().getSE();

  // Collect candidates before mutating, so cloning does not disturb iteration.
  SmallVector<Loop *, 4> Candidates;
  for (Loop *L : LI->getLoopsInPreorder())
    if (isVersioningEnabled(L))
      Candidates.push_back(L);

  // Distinct placeholder per loop so MachineCSE can't merge sibling guards'
  // threshold constants; the postpipeliner overwrites each with the stage
  // count.
  bool Changed = false;
  int64_t Placeholder = AIELoopUtils::DefaultLoopVersionGuardThreshold;
  for (Loop *L : Candidates)
    if (versionLoop(L, Placeholder)) {
      ++Placeholder;
      Changed = true;
    }
  return Changed;
}
