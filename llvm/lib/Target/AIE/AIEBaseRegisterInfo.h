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

#include "llvm/ADT/SmallSet.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include <set>

namespace llvm {

struct AIEBaseRegisterInfo : public TargetRegisterInfo {
  using TargetRegisterInfo::TargetRegisterInfo;
  virtual Register getStackPointerRegister() const = 0;
  /// Returns a TargetRegisterClass used for GPR RegClass.
  virtual const TargetRegisterClass *
  getGPRRegClass(const MachineFunction &MF) const {
    llvm_unreachable("Target didn't implement getGPRRegClass!");
  }

  /// Returns the SubReg indices that can be used to split a vreg in
  /// \p RegClassId into multiple smaller registers.
  /// Returns {NoSubRegister} if splitting is not possible.
  virtual const std::set<int> &getSubRegSplit(int RegClassId) const {
    llvm_unreachable("Target didn't implement getSubRegSplit()");
  }

  virtual SmallSet<int, 8>
  getCoveringSubRegs(const TargetRegisterClass &RC) const {
    return {};
  }

  // Whether redundant assignments to reserved registers can be simplified by
  // WAWEdges
  bool isSimplifiableReservedReg(MCRegister PhysReg) const override {
    return false;
  }

  virtual bool isVecOrAccRegClass(const TargetRegisterClass &RC) const {
    llvm_unreachable("Target didn't implement isVecOrAccRegClass()");
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

  virtual const TargetRegisterClass *get2DIteratorRegClass() const {
    llvm_unreachable("Target didn't implement get2DIteratorRegClass!");
  }

  virtual const TargetRegisterClass *get3DIteratorRegClass() const {
    llvm_unreachable("Target didn't implement get3DIteratorRegClass!");
  }
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEBASEREGISTERINFO_H
