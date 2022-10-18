//===-- AIEBaseRegisterInfo.h - Common AIE Register Information -*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains common register code between AIE versions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEBASEREGISTERINFO_H
#define LLVM_LIB_TARGET_AIE_AIEBASEREGISTERINFO_H

#include "llvm/CodeGen/TargetRegisterInfo.h"

namespace llvm {

struct AIEBaseRegisterInfo : public TargetRegisterInfo {
  using TargetRegisterInfo::TargetRegisterInfo;
  virtual Register getStackPointerRegister() const = 0;
  /// Returns a TargetRegisterClass used for GPR RegClass.
  virtual const TargetRegisterClass *
  getGPRRegClass(const MachineFunction &MF) const {
    llvm_unreachable("Target didn't implement getGPRRegClass!");
  }
#if 0
  /// Returns a BitVector of the intersection of GPR RegClass
  /// and CalleeSaved Registers
  virtual BitVector
  getCalleeSavedGPRRegClass(const MachineFunction &MF,
                            const TargetRegisterInfo *TRI) const {
    llvm_unreachable("Target didn't implement getGPRCalleeSavedRegClass!");
  }
#endif
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEBASEREGISTERINFO_H
