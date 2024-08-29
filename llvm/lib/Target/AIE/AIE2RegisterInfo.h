//===-- AIE2RegisterInfo.h -AIEngine V2 Register Information Impl -*- C++*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIEngine V2 implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE2_AIE2REGISTERINFO_H
#define LLVM_LIB_TARGET_AIE2_AIE2REGISTERINFO_H

#include "AIEBaseRegisterInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "AIE2GenRegisterInfo.inc"

namespace llvm {

class RegisterBank;

struct AIE2RegisterInfo : public AIE2GenRegisterInfo {

  AIE2RegisterInfo(unsigned HwMode);
  const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID) const override;

  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  const uint32_t *getNoPreservedMask() const override;

  bool eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  Register getFrameRegister(const MachineFunction &MF) const override;

  const TargetRegisterClass *getPointerRegClass(const MachineFunction &MF,
                                                unsigned Kind) const override;

  bool requiresRegisterScavenging(const MachineFunction &MF) const override {
    return true;
  }

  bool requiresFrameIndexScavenging(const MachineFunction &MF) const override {
    return true;
  }

  bool trackLivenessAfterRegAlloc(const MachineFunction &) const override {
    return true;
  }

  bool isTypeLegalForClass(const TargetRegisterClass &RC, LLT T) const override;

  /// Given a register bank and operand type, return the smallest register class
  /// that can hold a value on that bank.
  const TargetRegisterClass &getMinClassForRegBank(const RegisterBank &RB,
                                                   LLT Ty) const;

  const TargetRegisterClass *getConstrainedRegClassForOperand(
      const MachineOperand &MO, const MachineRegisterInfo &MRI) const override;

  Register getStackPointerRegister() const override;
  Register getControlRegister(unsigned Idx) const;

  const TargetRegisterClass *
  getLargestLegalSuperClass(const TargetRegisterClass *RC,
                            const MachineFunction &MF) const override;
  const TargetRegisterClass *
  getGPRRegClass(const MachineFunction &MF) const override;
  const std::set<int> &getSubRegSplit(int RegClassId) const override;
  SmallSet<int, 8>
  getCoveringSubRegs(const TargetRegisterClass &RC) const override;
  bool isSimplifiableReservedReg(MCRegister PhysReg) const override;

  const TargetRegisterClass *get2DIteratorRegClass() const override {
    return &AIE2::eDRegClass;
  }

  const TargetRegisterClass *get3DIteratorRegClass() const override {
    return &AIE2::eDSRegClass;
  }
  bool isVecOrAccRegClass(const TargetRegisterClass &RC) const override;
};
} // namespace llvm

#endif
