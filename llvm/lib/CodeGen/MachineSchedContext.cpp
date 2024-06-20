//===- MachineSchedConetxt.cpp - Machine Scheduler Context ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its
// affiliates
//
//===----------------------------------------------------------------------===//
//
// Common functionality between different MachineScheduler-like passes
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/MachineSchedContext.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/Pass.h"



using namespace llvm;

namespace llvm {

void MachineSchedContext::initializeContext(MachineFunction &MFunc, bool NeedAA,
                                            bool NeedRCI, bool NeedLIS) {
  MF = &MFunc;
  MLI = &getAnalysis<MachineLoopInfo>();
  MDT = &getAnalysis<MachineDominatorTree>();
  PassConfig = &getAnalysis<TargetPassConfig>();
  if (NeedLIS) {
    LIS = &getAnalysis<LiveIntervals>();
  }
  if (NeedAA) {
    AA = &getAnalysis<AAResultsWrapperPass>().getAAResults();
  }
  if (NeedRCI) {
    RegClassInfo.runOnMachineFunction(*MF);
  }
}

} // namespace llvm
