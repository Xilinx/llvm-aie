//===- MachineSchedContext.h - Function-level Scheduling -    ----- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2024 Advanced Micro Devices, Inc. or its
// affiliates
//
//===----------------------------------------------------------------------===//
//
// This file holds common functionality between MachinePipeliner and
// MachineScheduler
//===----------------------------------------------------------------------===//

#ifndef LLVM_CODEGEN_MACHINESCHEDCONTEXT_H
#define LLVM_CODEGEN_MACHINESCHEDCONTEXT_H

#include "llvm/CodeGen/RegisterClassInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"

namespace llvm {

class MachineFunction;
class MachineLoopInfo;
class MachineDominatorTree;
class TargetPassConfig;
class AAResults;
class LiveIntervals;


/// MachineSchedContext provides enough context from the MachineScheduler pass
/// for the target to instantiate a scheduler.
class MachineSchedContext : public MachineFunctionPass {
public:
  MachineFunction *MF = nullptr;
  const MachineLoopInfo *MLI = nullptr;
  const MachineDominatorTree *MDT = nullptr;
  const TargetPassConfig *PassConfig = nullptr;
  AAResults *AA = nullptr;
  LiveIntervals *LIS = nullptr;

  RegisterClassInfo RegClassInfo;

  MachineSchedContext(char &ID) : MachineFunctionPass(ID) {};
  virtual ~MachineSchedContext() = default;
  MachineSchedContext &operator=(const MachineSchedContext &Other) = delete;
  MachineSchedContext(const MachineSchedContext &Other) = delete;

  // Implementation of MachineFunctionPass's pure virtuals to make
  // MachineSchedContext a concrete class that can be instantiated from
  // a testing mockup.
  void getAnalysisUsage(AnalysisUsage &AU) const override{};
  bool runOnMachineFunction(MachineFunction &) override { return false; };

  // For dynamic type checking. Biggest difference now is that a pipeliner
  // only visits loops.
  virtual bool isPipeliner() const { return false; }

protected:
  // Initialize the context with analysis result handles, some of them
  // optional.
  void initializeContext(MachineFunction &MFunc, bool NeedAA = false,
                         bool NeedRCI = false, bool NeedLIS = false);
};

} // namespace llvm

#endif // LLVM_CODEGEN_MACHINESCHEDCONTEXT_H
