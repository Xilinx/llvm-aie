//===-- AIESwitchLowering.cpp - Lower switches to OR-of-icmp chains -------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Branches are expensive on AIE (mandatory delay slots), so we want to avoid
// them as much as possible. The default GlobalISel switch lowering emits one
// compare + conditional branch per (non-contiguous) case cluster, which for a
// switch whose several case values reach the same label results in multiple
// conditional branches.
//
// This target-specific IR pass rewrites such switches into a chain of
// OR-of-`icmp eq` conditions, emitting a single conditional branch per
// destination. For example:
//
//   switch i8 %x, label %else [ i8 0, label %then
//                              i8 2, label %then ]
//
// becomes:
//
//   %c0 = icmp eq i8 %x, 0
//   %c2 = icmp eq i8 %x, 2
//   %cond = or i1 %c0, %c2
//   br i1 %cond, label %then, label %else
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/MDBuilder.h"
#include "llvm/IR/ProfDataUtils.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include <algorithm>
#include <cstdint>

using namespace llvm;

#define DEBUG_TYPE "aie-switch-lowering"

STATISTIC(NumSwitchesLowered,
          "Number of switches lowered to OR-of-icmp chains");

// The number of *conditional branches* our OR-chain emits equals the number of
// distinct case destinations (one branch per destination). Leaving the switch
// in place instead hands it to the default GlobalISel lowering
// (SwitchLoweringUtils), which bisects it into a balanced compare-and-branch
// tree needing only ~O(log2 #cases) branches on any path. Since branches are
// expensive on AIE (mandatory delay slots), the OR-chain wins only when there
// are *few* destinations; with many destinations the linear branch-per-
// destination chain loses to bisection. So the primary gate is a cap on the
// number of destinations (default 2 -- the "several values dispatch to one or
// two labels" pattern this transform targets).
static cl::opt<unsigned> MaxDestsForOrChain(
    "aie-switch-or-chain-max-dests", cl::Hidden, cl::init(2),
    cl::desc("Maximum number of distinct case destinations for which a switch "
             "is lowered to an OR-of-icmp chain on AIE. Switches with more "
             "destinations are left to the generic (bisected) lowering."));

// Secondary guard on the *total* number of cases. Even with few destinations,
// the OR-chain evaluates the comparisons linearly (O(N) on a path) and grows
// code size by ~N, so we also cap the case count to keep the chain short.
static cl::opt<unsigned> MaxCasesForOrChain(
    "aie-switch-or-chain-max-cases", cl::Hidden, cl::init(8),
    cl::desc("Maximum number of switch cases lowered to an OR-of-icmp chain "
             "on AIE. Larger switches are left to the generic (bisected) "
             "lowering."));

static cl::opt<bool>
    EnableAIESwitchLowering("aie-switch-or-chain", cl::Hidden, cl::init(true),
                            cl::desc("Enable lowering of switches to "
                                     "OR-of-icmp chains on AIE."));

namespace {

// Note: this pass rewrites the CFG (it splits the switch block and inserts new
// comparison blocks), so it does not preserve any analyses. As a legacy
// FunctionPass with no getAnalysisUsage override, it conservatively invalidates
// DominatorTree/LoopInfo/etc., which is the intended behavior here.
class AIESwitchLowering : public FunctionPass {
public:
  static char ID;
  AIESwitchLowering() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  StringRef getPassName() const override {
    return "AIE Switch to OR-of-icmp Lowering";
  }
};

} // end anonymous namespace

char AIESwitchLowering::ID = 0;

INITIALIZE_PASS(AIESwitchLowering, DEBUG_TYPE,
                "AIE Switch to OR-of-icmp Lowering", false, false)

/// Rewrite a single \p SI into an OR-of-icmp chain. Returns true on success.
static bool lowerSwitch(SwitchInst *SI) {
  BasicBlock *BB = SI->getParent();
  BasicBlock *DefaultBB = SI->getDefaultDest();
  Value *Cond = SI->getCondition();
  Function *F = BB->getParent();
  LLVMContext &Ctx = F->getContext();

  // Per-successor branch weights, if the switch carries profile metadata.
  // Index 0 is the default successor; the rest follow case successor order.
  SmallVector<uint32_t, 8> SuccWeights;
  const bool HasProf = extractBranchWeights(*SI, SuccWeights);

  // Group the case values by their destination, preserving first-seen order
  // for determinism, and accumulate each destination's summed branch weight.
  MapVector<BasicBlock *, SmallVector<ConstantInt *, 4>> DestToVals;
  DenseMap<BasicBlock *, uint64_t> DestWeight;
  for (const auto &Case : SI->cases()) {
    BasicBlock *Dest = Case.getCaseSuccessor();
    DestToVals[Dest].push_back(Case.getCaseValue());
    if (HasProf)
      DestWeight[Dest] += SuccWeights[Case.getSuccessorIndex()];
  }

  // A case destination that is also the default block is a degenerate shape we
  // do not model; leave it to the generic lowering.
  if (DestToVals.count(DefaultBB))
    return false;

  // The OR-chain emits one conditional branch per distinct destination, so it
  // only beats the generic balanced bisection when there are few destinations.
  // Bail out for switches that would fan out to too many destinations (this
  // also subsumes the fully-distinct case, where #dests == #cases).
  if (DestToVals.size() > MaxDestsForOrChain)
    return false;

  // Capture the ordered groups before mutating the CFG.
  SmallVector<std::pair<BasicBlock *, SmallVector<ConstantInt *, 4>>, 4> Groups(
      DestToVals.begin(), DestToVals.end());
  const uint64_t DefaultWeight = HasProf ? SuccWeights[0] : 0;

  // Remove the switch. Its successor PHIs still reference BB as an incoming
  // block; we fix them up below as we recreate the edges.
  SI->eraseFromParent();

  // Rewire the PHIs of a successor \p Succ that was reached from the switch
  // block \p BB so that the (single) incoming edge now originates from \p Pred.
  // A switch may reference the same successor from several case values, which
  // creates several predecessor edges (and therefore several incoming PHI
  // entries) for BB. After this transform there is exactly one edge, so we keep
  // one entry (retargeted to Pred) and drop the duplicates. The duplicated
  // entries are required by the verifier to carry the same value, so dropping
  // them preserves semantics.
  auto RewirePHIs = [&](BasicBlock *Succ, BasicBlock *Pred) {
    for (PHINode &PN : Succ->phis()) {
      bool KeptOne = false;
      for (int I = PN.getNumIncomingValues() - 1; I >= 0; --I) {
        if (PN.getIncomingBlock(I) != BB)
          continue;
        if (!KeptOne) {
          PN.setIncomingBlock(I, Pred);
          KeptOne = true;
        } else {
          PN.removeIncomingValue(I, /*DeletePHIIfEmpty=*/false);
        }
      }
    }
  };

  BasicBlock *CondBB = BB;
  const unsigned NumGroups = Groups.size();
  for (unsigned Idx = 0; Idx != NumGroups; ++Idx) {
    const bool IsLast = (Idx + 1 == NumGroups);
    BasicBlock *Dest = Groups[Idx].first;
    ArrayRef<ConstantInt *> Vals = Groups[Idx].second;

    IRBuilder<> Builder(CondBB);

    // Build the OR-of-`icmp eq` condition for this destination.
    Value *ChainCond = nullptr;
    for (ConstantInt *CI : Vals) {
      Value *Eq = Builder.CreateICmpEQ(Cond, CI, "sw.eq");
      ChainCond = ChainCond ? Builder.CreateOr(ChainCond, Eq, "sw.or") : Eq;
    }

    // The false edge goes either to a fresh block that tests the next
    // destination, or (for the last group) to the default block.
    BasicBlock *FalseBB =
        IsLast ? DefaultBB
               : BasicBlock::Create(Ctx, "sw.or.next", F, DefaultBB);

    BranchInst *BI = Builder.CreateCondBr(ChainCond, Dest, FalseBB);

    // Preserve profile info: the taken edge inherits this destination's summed
    // case weight; the fall-through inherits the remaining destinations plus
    // the default weight. Weights are saturated to the uint32 metadata range.
    if (HasProf) {
      uint64_t TrueW = DestWeight.lookup(Dest);
      uint64_t FalseW = DefaultWeight;
      for (unsigned J = Idx + 1; J != NumGroups; ++J)
        FalseW += DestWeight.lookup(Groups[J].first);
      auto Sat = [](uint64_t W) -> uint32_t {
        return static_cast<uint32_t>(std::min<uint64_t>(W, UINT32_MAX));
      };
      BI->setMetadata(LLVMContext::MD_prof, MDBuilder(Ctx).createBranchWeights(
                                                Sat(TrueW), Sat(FalseW)));
    }

    // The predecessor edge for Dest now comes from CondBB (was BB).
    RewirePHIs(Dest, CondBB);

    // On the last group, the default block is reached from CondBB (was BB).
    if (IsLast)
      RewirePHIs(DefaultBB, CondBB);

    CondBB = FalseBB;
  }

  ++NumSwitchesLowered;
  return true;
}

bool AIESwitchLowering::runOnFunction(Function &F) {
  if (!EnableAIESwitchLowering)
    return false;

  // Collect candidate switches first; lowering mutates the CFG.
  SmallVector<SwitchInst *, 4> Candidates;
  for (BasicBlock &BB : F) {
    auto *SI = dyn_cast<SwitchInst>(BB.getTerminator());
    if (!SI)
      continue;
    // Only handle reasonably small switches; larger ones are better left to the
    // generic bisected compare-tree lowering (see MaxCasesForOrChain above for
    // the linear-chain vs. bisection trade-off).
    if (SI->getNumCases() == 0 || SI->getNumCases() > MaxCasesForOrChain)
      continue;
    Candidates.push_back(SI);
  }

  bool Changed = false;
  for (SwitchInst *SI : Candidates)
    Changed |= lowerSwitch(SI);
  return Changed;
}

FunctionPass *llvm::createAIESwitchLowering() {
  return new AIESwitchLowering();
}
