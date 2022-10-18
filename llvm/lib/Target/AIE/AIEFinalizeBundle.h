//===- AIEFinalizeBundle.h --------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEFINALIZEBUNDLE_H
#define LLVM_LIB_TARGET_AIE_AIEFINALIZEBUNDLE_H

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"

// This pass provides the same functionality as the generic
// Bundle Finalization in MachineInstrBundle, except that it
// also bundles standalone instructions.
class AIEFinalizeBundle : public llvm::MachineFunctionPass {

public:
  static char ID;
  AIEFinalizeBundle() : MachineFunctionPass(ID) {}
  llvm::StringRef getPassName() const override {
    return "AIE Bundle Finalization";
  }
  bool runOnMachineFunction(llvm::MachineFunction &MF) override;
};

#endif // LLVM_LIB_TARGET_AIE_AIEFINALIZEBUNDLE_H
