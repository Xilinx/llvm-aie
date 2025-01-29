//===AIE2PRegisterInfo.h -AIE2p Register Information Impl --------*- C++*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE2p implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE2P_AIE2PREGISTERINFO_H
#define LLVM_LIB_TARGET_AIE2P_AIE2PREGISTERINFO_H

#include "AIEBaseRegisterInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "AIE2PGenRegisterInfo.inc"

namespace llvm {

class RegisterBank;

struct AIE2PRegisterInfo : public AIE2PGenRegisterInfo {

  AIE2PRegisterInfo(unsigned HwMode);

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
                                                   LLT Ty) const override;
  const std::set<int> &getSubRegSplit(int RegClassId) const override;
  const TargetRegisterClass *getConstrainedRegClassForOperand(
      const MachineOperand &MO, const MachineRegisterInfo &MRI) const override;

  Register getStackPointerRegister() const override;
  Register getControlRegister(unsigned Idx) const;

  const TargetRegisterClass *
  getLargestLegalSuperClass(const TargetRegisterClass *RC,
                            const MachineFunction &MF) const override;
  const TargetRegisterClass *
  getGPRRegClass(const MachineFunction &MF) const override;

  unsigned getVectorRegBankID() const override;
  unsigned getGPRRegBankID() const override;

  void getTargetSubRegs(std::vector<unsigned> &, unsigned Size,
                        const RegisterBank &RB) const override;

  bool isReservedStickyReg(MCRegister PhysReg) const override;

  const TargetRegisterClass *get2DIteratorRegClass() const override {
    return &AIE2P::eDRegClass;
  }

  const TargetRegisterClass *get3DIteratorRegClass() const override {
    return &AIE2P::eDSRegClass;
  }

  const TargetRegisterClass *getAddrCountRegClass() const override {
    return &AIE2P::eDCRegClass;
  }
  bool isVecOrAccRegClass(const TargetRegisterClass &RC) const override;

  bool isFifoPhysReg(const Register Reg) const override;

  bool isSimplifiableReservedReg(MCRegister PhysReg) const override;
};
} // namespace llvm

#endif
