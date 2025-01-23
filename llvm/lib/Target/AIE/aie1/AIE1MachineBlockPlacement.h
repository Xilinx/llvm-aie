//===- AIE1MachineBlockPlacement.h -------------------------------*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE1_AIE1MACHINEBLOCKPLACEMENT_H
#define LLVM_LIB_TARGET_AIE_AIE1_AIE1MACHINEBLOCKPLACEMENT_H

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"

class AIE1MachineBlockPlacement : public llvm::MachineFunctionPass {

public:
  static char ID;
  AIE1MachineBlockPlacement() : MachineFunctionPass(ID) {}
  llvm::StringRef getPassName() const override {
    return "AIE Machine Block Alignment";
  }
  bool runOnMachineFunction(llvm::MachineFunction &MF) override;
};

#endif
