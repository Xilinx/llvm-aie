//==========-- AIEPHISorterPass.cpp - Sort PHI nodes in loops --==============//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This Pass reorders PHI nodes in a more friendly way to Greedy Allocator
//
//===----------------------------------------------------------------------===//

#include "AIE2.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/IntrinsicsAArch64.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Utils/Cloning.h"

using namespace llvm;

#define DEBUG_TYPE "aie-phi-sorter"

namespace {
struct AIEPHISorter : public FunctionPass {
  static char ID; // Pass identification, replacement for typeid
  AIEPHISorter() : FunctionPass(ID) {
    initializeAIEPHISorterPass(*PassRegistry::getPassRegistry());
  }

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<LoopInfoWrapperPass>();
    AU.setPreservesAll();
  }
};
} // end anonymous namespace

char AIEPHISorter::ID = 0;
static const char *name = "AIE PHI Sorter Pass";
INITIALIZE_PASS_BEGIN(AIEPHISorter, DEBUG_TYPE, name, false, false)
INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass);
INITIALIZE_PASS_END(AIEPHISorter, DEBUG_TYPE, name, false, false)

FunctionPass *llvm::createAIEPHISorterPass() { return new AIEPHISorter(); }

bool AIEPHISorter::runOnFunction(Function &F) {

  const LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
  bool Changed = false;

  for (BasicBlock &B : F) {
    if (LI.getLoopFor(&B)) {
      SmallVector<PHINode *> WorkList;
      std::map<const Instruction *, unsigned> InstrOrder;
      // Collect all interesting recursive PHI nodes.
      for (auto &PHI : B.phis()) {
        for (BasicBlock *BB : PHI.blocks()) {
          // Check basically if this loop block
          // is part of the body.
          if (BB == &B)
            WorkList.push_back(&PHI);
        }
      }

      if (WorkList.empty())
        continue;

      Changed = true;
      unsigned PositionCounter = 0;

      // Collect instruction ordering.
      for (auto &I : B)
        InstrOrder[&I] = PositionCounter++;

      // Reorder out-of-place the collected PHI nodes.
      std::sort(WorkList.begin(), WorkList.end(),
                [&](const PHINode *PhiA, const PHINode *PhiB) {
                  const Instruction *InstA =
                      dyn_cast<Instruction>(PhiA->getIncomingValueForBlock(&B));
                  const Instruction *InstB =
                      dyn_cast<Instruction>(PhiB->getIncomingValueForBlock(&B));
                  return InstrOrder[InstA] < InstrOrder[InstB];
                });

      // Commit new ordering.
      for (unsigned i = 0; i < WorkList.size() - 1; i++)
        WorkList[i + 1]->moveAfter(WorkList[i]);
    }
  }
  return Changed;
}
