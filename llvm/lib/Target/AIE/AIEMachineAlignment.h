//===- AIEMachineAlignment.h --------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEMACHINEALIGNMENT_H
#define LLVM_LIB_TARGET_AIE_AIEMACHINEALIGNMENT_H

#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"

namespace llvm {

class AIEMachineAlignment : public llvm::MachineFunctionPass {

public:
  static char ID;
  AIEMachineAlignment() : MachineFunctionPass(ID) {}
  llvm::StringRef getPassName() const override {
    return "AIE Machine Alignment";
  }
  bool runOnMachineFunction(llvm::MachineFunction &MF) override;
  void applyBundlesAlignment(
      const std::vector<llvm::iterator_range<MachineBasicBlock::iterator>>
          &Regions);
};

} // end namespace llvm

#endif
