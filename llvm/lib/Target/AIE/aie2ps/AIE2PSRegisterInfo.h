//=== AIE2PSRegisterInfo.h -AIE2ps Register Information Impl ------*- C++*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE2ps implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE2PS_AIE2PSREGISTERINFO_H
#define LLVM_LIB_TARGET_AIE2PS_AIE2PSREGISTERINFO_H

#include "AIEBaseRegisterInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "AIE2PSGenRegisterInfo.inc"

namespace llvm {

class RegisterBank;

struct AIE2PSRegisterInfo : public AIE2PSGenRegisterInfo {

  AIE2PSRegisterInfo(unsigned HwMode);
  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;
  BitVector getReservedRegs(const MachineFunction &MF) const override;
  const uint32_t *getNoPreservedMask() const override;
  bool eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  Register getFrameRegister(const MachineFunction &MF) const override;
  Register getStackPointerRegister() const override;

  bool requiresRegisterScavenging(const MachineFunction &MF) const override {
    return true;
  }

  bool requiresFrameIndexScavenging(const MachineFunction &MF) const override {
    return true;
  }

  const TargetRegisterClass *
  getLargestLegalSuperClass(const TargetRegisterClass *RC,
                            const MachineFunction &MF) const override;
  bool isTypeLegalForClass(const TargetRegisterClass &RC, LLT T) const override;
  const TargetRegisterClass *
  getGPRRegClass(const MachineFunction &MF) const override;
  Register getControlRegister(unsigned Idx) const override;

  /// Given a register bank and operand type, return the smallest register class
  /// that can hold a value on that bank.
  const TargetRegisterClass &getMinClassForRegBank(const RegisterBank &RB,
                                                   LLT Ty) const override;
  const std::set<int> &getSubRegSplit(int RegClassId) const override;
  const TargetRegisterClass *getConstrainedRegClassForOperand(
      const MachineOperand &MO, const MachineRegisterInfo &MRI) const override;

  const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID) const override;

  unsigned getGPRRegBankID() const override;
  unsigned getMODRegBankID() const override;
  unsigned getPTRRegBankID() const override;

  unsigned matchControlRegisterBitwidth(Register CtrlReg,
                                        unsigned SrcConstVal) const override;

  Register getUnpackSignCtrlReg() const override;

  void getTargetSubRegs(std::vector<unsigned> &, unsigned Size,
                        const RegisterBank &RB) const override;

  bool isReservedStickyReg(MCRegister PhysReg) const override;

  const TargetRegisterClass *get2DIteratorRegClass() const override {
    return &AIE2PS::eDRegClass;
  }

  const TargetRegisterClass *get3DIteratorRegClass() const override {
    return &AIE2PS::eDSRegClass;
  }

  const TargetRegisterClass *getAddrCountRegClass() const override {
    return &AIE2PS::eDCRegClass;
  }

  bool isVecOrAccRegClass(const TargetRegisterClass &RC) const override;

  bool isFifoPhysReg(const Register Reg) const override;

  bool shouldCoalesce(MachineInstr *MI, const TargetRegisterClass *SrcRC,
                      unsigned SubReg, const TargetRegisterClass *DstRC,
                      unsigned DstSubReg, const TargetRegisterClass *NewRC,
                      LiveIntervals &LIS) const override;

  bool isSimplifiableReservedReg(MCRegister PhysReg) const override;
};
} // namespace llvm

#endif
