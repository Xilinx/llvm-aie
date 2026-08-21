//===-- AIEEpilogueRegRewriter.cpp - Rename epilogue definitions --------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "AIEBaseInstrInfo.h"
#include "AIEBaseRegisterInfo.h"
#include "AIESuperRegUtils.h"
#include "Utils/AIELoopOptionOverrides.h"
#include "Utils/AIERegAllocationUtils.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/LiveDebugVariables.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/LiveRegMatrix.h"
#include "llvm/CodeGen/LiveStacks.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/CodeGen/VirtRegMap.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "aie-epilogue-reg-rewriter"

static cl::opt<unsigned> EpilogueCopyBudget(
    "aie-epilogue-reg-rewrite-copy-budget", cl::Hidden, cl::init(4),
    cl::desc("Maximum materialized copy instructions per epilogue"));

// Per-block opt-in: only rename WAR definitions in a block that carries this
// hint (see AIE::LoopOptionOverrides).
static cl::opt<bool> EnableOLPWarRename(
    "aie-olp-war-rename", cl::Hidden, cl::init(false),
    cl::desc("Enable WAR register renaming on blocks carrying the hint"));

namespace {

class AIEEpilogueRegRewriter : public MachineFunctionPass {
public:
  static char ID;

  AIEEpilogueRegRewriter() : MachineFunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    AU.addRequired<VirtRegMapWrapperLegacy>();
    AU.addPreserved<VirtRegMapWrapperLegacy>();
    AU.addRequired<SlotIndexesWrapperPass>();
    AU.addPreserved<SlotIndexesWrapperPass>();
    AU.addRequired<LiveIntervalsWrapperPass>();
    AU.addPreserved<LiveIntervalsWrapperPass>();
    AU.addRequired<LiveRegMatrixWrapperLegacy>();
    AU.addPreserved<LiveRegMatrixWrapperLegacy>();
    AU.addRequired<LiveDebugVariablesWrapperLegacy>();
    AU.addPreserved<LiveDebugVariablesWrapperLegacy>();
    AU.addRequired<LiveStacksWrapperLegacy>();
    AU.addPreserved<LiveStacksWrapperLegacy>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &MF) override { return false; }
};

} // namespace

char AIEEpilogueRegRewriter::ID = 0;
char &llvm::AIEEpilogueRegRewriterID = AIEEpilogueRegRewriter::ID;

INITIALIZE_PASS_BEGIN(AIEEpilogueRegRewriter, DEBUG_TYPE,
                      "AIE epilogue register rewrite", false, false)
INITIALIZE_PASS_DEPENDENCY(VirtRegMapWrapperLegacy)
INITIALIZE_PASS_DEPENDENCY(SlotIndexesWrapperPass)
INITIALIZE_PASS_DEPENDENCY(LiveIntervalsWrapperPass)
INITIALIZE_PASS_DEPENDENCY(LiveRegMatrixWrapperLegacy)
INITIALIZE_PASS_DEPENDENCY(LiveDebugVariablesWrapperLegacy)
INITIALIZE_PASS_DEPENDENCY(LiveStacksWrapperLegacy)
INITIALIZE_PASS_END(AIEEpilogueRegRewriter, DEBUG_TYPE,
                    "AIE epilogue register rewrite", false, false)

FunctionPass *llvm::createAIEEpilogueRegRewriter() {
  return new AIEEpilogueRegRewriter();
}
