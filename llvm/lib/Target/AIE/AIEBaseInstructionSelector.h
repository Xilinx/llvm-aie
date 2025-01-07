//===- AIEBaseInstructionSelector.h---------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements the targeting of the InstructionSelector class for
/// AIEngine.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEBASEINSTRUCTIONSELECTOR_H
#define LLVM_LIB_TARGET_AIE_AIEBASEINSTRUCTIONSELECTOR_H

#include "AIEBaseInstrInfo.h"
#include "AIEBaseRegisterBankInfo.h"
#include "AIEBaseRegisterInfo.h"
#include "AIEBaseSubtarget.h"
#include "AIECombinerHelper.h"
#include "AIEMachineFunctionInfo.h"
#include "llvm/ADT/APInt.h"
#include "llvm/CodeGen/GlobalISel/GIMatchTableExecutorImpl.h"
#include "llvm/CodeGen/GlobalISel/GenericMachineInstrs.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelector.h"
#include "llvm/CodeGen/GlobalISel/MIPatternMatch.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/GlobalISel/Utils.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IntrinsicsAIE.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TypeSize.h"
#include <optional>
#include <utility>

#define DEBUG_TYPE "aiebase-isel"

namespace llvm {

/// Hold the information needed to instruction select memory operations.
struct AddressingModeInfo {
  MachineInstr &MemI;
  MachineOperand &SrcDstOp;
  MachineOperand &PtrOp;
  std::optional<Register> OffsetReg;
  std::optional<APInt> ImmediateOffset;
};

/// Hold the information needed to select instructions that deal with multiple
/// addressing modes and might need to be split
struct LoadStoreOpcodes {
  // The main Opcode with the correct addressing mode
  unsigned ISelOpcode;
  // Indicates whether the provided immediate offset fits in the immedate range
  // of the selected instruction
  bool FitsImmediateRange;
  // Opcode with an immediate indexed addressing mode to split larger loads and
  // stores into smaller ones
  // E.g. Instruction selection for $x0, $p0 = G_AIE_POSTINC_LOAD $p0, #128:
  // $wh0 = VLDA_dmw_lda_w_ag_idx_imm $p0, #32
  // $wl0, $p0 = VLDA_dmw_lda_w_ag_pstm_nrm_imm $p0, #128
  std::optional<unsigned> OffsetOpcode;
};

class AIEBaseInstructionSelector : public InstructionSelector {
public:
  AIEBaseInstructionSelector(const AIEBaseSubtarget &STI,
                             const AIEBaseRegisterBankInfo &RBI);
  bool selectCopy(MachineInstr &I, MachineRegisterInfo &MRI);
  bool selectG_IMPLICIT_DEF(MachineInstr &I, MachineRegisterInfo &MRI);
  bool selectG_PHI(MachineInstr &I, MachineRegisterInfo &MRI);
  bool selectG_PTR_ADD(MachineIRBuilder &MIB, MachineInstr &I,
                       MachineRegisterInfo &MRI);
  void renderFrameIndex(MachineInstrBuilder &MIB, const MachineInstr &MI,
                        int OpIdx) const;
  void renderNegateImm(MachineInstrBuilder &MIB, const MachineInstr &MI,
                       int OpIdx) const;
  /// set \a CRReg based on \a ValueReg before \a I and set \a CRReg based on \a
  /// DefaultCRVal after \a I.
  void setUnsetCtrlRegister(MachineIRBuilder &MIB, MachineInstr &I,
                            MachineRegisterInfo &MRI, Register CRReg,
                            Register ValueReg, unsigned DefaultCRVal = 0);
  void setUnsetCtrlRegister(MachineIRBuilder &MIB, MachineInstr &I,
                            MachineInstr &EndI, MachineRegisterInfo &MRI,
                            Register CRReg, Register ValueReg,
                            unsigned DefaultCRVal = 0);
  AddressingModeInfo createAddressModeInfo(MachineInstr &MemI,
                                           MachineOperand &SrcDstOp,
                                           MachineOperand &PtrOp,
                                           std::optional<Register> OffsetReg,
                                           MachineRegisterInfo &MRI);
  /// Split the memory operand from I by the factor defined in SplitFactor, add
  /// the Offset and add the new memory operands to Higher and Lower.
  void addSplitMemOperands(MachineInstr &I, MachineInstrBuilder &Higher,
                           MachineInstrBuilder &Lower, unsigned Offset,
                           unsigned SplitFactor);
  /// Given the AddressingModeInfo, FitsImmediateRange and whether the
  /// instruction supports frame index rendering add the correct operands to MIB
  void addAddressingMode(MachineInstrBuilder &MIB, AddressingModeInfo &AMI,
                         bool FitsImmediateRange, bool RenderFrameIndex,
                         MachineRegisterInfo &MRI);
  /// Select into a REG_SEQUENCE instruction of two 32-bit R registers to
  /// one 64-bit L register.
  bool selectLRegSequence(MachineIRBuilder &MIB, MachineInstr &I,
                          MachineRegisterInfo &MRI);

  bool selectG_UNMERGE_VALUES(MachineIRBuilder &MIB, MachineInstr &MI,
                              MachineRegisterInfo &MRI);
  virtual Register createDSRegSequence(Register ModifierReg, Register Incr1Reg,
                                       Register Incr2Reg, Register Size1Reg,
                                       Register Count1Reg, Register Size2Reg,
                                       Register Count2Reg,
                                       MachineRegisterInfo &MRI) = 0;
  virtual Register createDRegSequence(Register ModifierReg, Register IncrReg,
                                      Register SizeReg, Register CountReg,
                                      MachineRegisterInfo &MRI) = 0;
  bool selectAddrInsn(MachineIRBuilder &MIB, MachineInstr &I,
                      MachineRegisterInfo &MRI);
  bool selectGetSS(MachineInstr &I, MachineRegisterInfo &MRI,
                   MachineIRBuilder &MIB);
  bool selectPutMSB(MachineInstr &I, MachineRegisterInfo &MRI,
                    MachineIRBuilder &MIB);
  bool selectPutMSNB(MachineInstr &I, MachineRegisterInfo &MRI,
                     MachineIRBuilder &MIB);
  virtual void buildUnpack(MachineInstr &I, MachineRegisterInfo &MRI,
                           MachineIRBuilder &MIB, MachineInstrBuilder &MI);
  void buildPack(MachineInstr &I, MachineRegisterInfo &MRI,
                 MachineIRBuilder &MIB, MachineInstrBuilder &MI);

  bool selectVMAXDIFF_LT(MachineInstr &I, MachineRegisterInfo &MRI,
                         MachineIRBuilder &MIB);
  bool selectVABS_GTZ(MachineInstr &I, MachineRegisterInfo &MRI,
                      MachineIRBuilder &MIB);
  bool selectVSUB_LTGE(MachineInstr &I, MachineRegisterInfo &MRI,
                       MachineIRBuilder &MIB);

  bool selectVCompare(MachineInstr &I, MachineRegisterInfo &MRI,
                      MachineIRBuilder &MIB);

  bool selectVSUB_MIN_MAX(MachineInstr &I, MachineRegisterInfo &MRI,
                          MachineIRBuilder &MIB);

protected:
  void makeDeadMI(MachineInstr &MI, MachineRegisterInfo &MRI);
  virtual std::optional<AddressingModeInfo>
  getOrDefineAddressingRegister(MachineInstr &MemI, MachineRegisterInfo &MRI);
  virtual std::optional<LoadStoreOpcodes>
  getCombinedOpcodeCONV(const MachineInstr &MemOp, const MachineInstr &CombOp,
                        std::optional<APInt> Immediate);
  bool canCombineCONV(MachineInstr &MemOp, MachineInstr &CombOp);
  virtual std::optional<LoadStoreOpcodes>
  getCombinedOpcodeCONVLoad(const MachineInstr &MemOp,
                            const MachineInstr &CombOp,
                            const std::optional<APInt> Immediate);
  bool canCombineCONVLoad(MachineInstr &MemOp, MachineInstr &CombOp);
  bool selectG_AIE_LOAD_CONV(MachineInstr &CONVI, MachineRegisterInfo &MRI);
  bool selectVCONV(MachineInstr &I, MachineRegisterInfo &MRI);

protected:
  MachineIRBuilder MIB;

private:
  virtual bool selectImpl(MachineInstr &I,
                          CodeGenCoverage &CoverageInfo) const = 0;

private:
  const AIEBaseInstrInfo &TII;
  const AIEBaseRegisterInfo &TRI;
  const AIEBaseRegisterBankInfo &RBI;
};

inline unsigned getLoadStoreSize(const MachineInstr &MI) {
  // We are guaranteed to have MMOs during Instruction Selection.
  // We need them to select the correct instruction when they depend on the
  // size in memory and not on the register size. E.g.: part word stores.
  return (*MI.memoperands_begin())->getSizeInBits().getValue();
}

template <unsigned NumEncodingBits, unsigned Step>
bool checkImmediateRange(std::optional<APInt> Immediate) {
  unsigned MaxPow2 = NumEncodingBits + llvm::Log2_64(Step);
  if (Immediate && isIntN(MaxPow2, Immediate->getSExtValue()) &&
      Immediate->getSExtValue() % Step == 0) {
    LLVM_DEBUG(dbgs() << "Immediate " << Immediate << " is valid for MaxPow2 "
                      << MaxPow2 << " and Step " << Step << ".\n");
    return true;
  }
  return false;
}

template <unsigned NumEncodingBits, unsigned Step, unsigned SplitOffset>
bool checkImmediateRangeSplitting(std::optional<APInt> Immediate) {
  return Immediate && checkImmediateRange<NumEncodingBits, Step>(Immediate) &&
         checkImmediateRange<NumEncodingBits, Step>(*Immediate + SplitOffset);
}

inline unsigned deriveRegBankID(Register Reg, const MachineRegisterInfo &MRI,
                                const RegisterBankInfo &RBI) {
  const RegisterBank *RB = MRI.getRegBankOrNull(Reg);
  if (RB) {
    return RB->getID();
  }
  const TargetRegisterClass *RC = MRI.getRegClassOrNull(Reg);
  if (RC) {
    return RBI.getRegBankFromRegClass(*RC, MRI.getType(Reg)).getID();
  }
  llvm_unreachable("Cannot derive RegBankID from Register");
}

/// Create a memory operand representing Tile Memory
/// \param I Instruction to build it for
/// \param Mode Store or Load
/// \return The machine memory operand
inline MachineMemOperand *getTileMemOperand(MachineInstr &I,
                                            MachineMemOperand::Flags Mode) {
  MachineFunction *MF = I.getMF();
  const auto *MFI = MF->getInfo<AIEMachineFunctionInfo>();
  MachinePointerInfo PtrInfo = MachinePointerInfo(MFI->getTileMemory());
  return MF->getMachineMemOperand(PtrInfo, Mode, 4, Align(4));
}

} // namespace llvm

#undef DEBUG_TYPE
#endif // LLVM_LIB_TARGET_AIE_AIEBASEINSTRUCTIONSELECTOR_H
