//===- AIEBaseInstructionSelector.h---------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
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
  bool selectG_SEXT_INREG(MachineInstr &I, MachineRegisterInfo &MRI,
                          const std::pair<unsigned, unsigned> &Opcodes);
  bool selectG_TRUNC(MachineInstr &I, MachineRegisterInfo &MRI,
                     unsigned SubRegIdx);
  bool selectGetCoreID(MachineInstr &I, MachineRegisterInfo &MRI,
                       Register CoreID);
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
  MachineInstrBuilder setCtrlRegister(MachineIRBuilder &MIB, Register CRReg,
                                      unsigned Val);
  MachineInstrBuilder setStatusRegister(MachineIRBuilder &MIB,
                                        Register StatusReg, unsigned Val);
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
  virtual unsigned getSub256LoIdx() const = 0;
  virtual unsigned getNoSubRegIdx() const = 0;
  virtual const TargetRegisterClass &getVEC128RegClass() const = 0;
  virtual const TargetRegisterClass &getVEC256RegClass() const = 0;
  virtual const TargetRegisterClass &getVEC512RegClass() const = 0;
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
  bool selectSetLoopIterations(MachineInstr &I, MachineRegisterInfo &MRI,
                               MachineIRBuilder &MIB);
  bool selectBrCondLoopDecrement(MachineInstr &BrCond,
                                 MachineRegisterInfo &MRI);
  bool selectBrCondLoopDecrementReg(MachineInstr &BrCond,
                                    MachineRegisterInfo &MRI);
  bool selectG_BRCOND(MachineInstr &I, MachineRegisterInfo &MRI);

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

  void insertPtrAddForOffset(MachineRegisterInfo &MRI, MachineInstr &MemI);

  bool selectG_CONSTANT(MachineInstr &I, MachineRegisterInfo &MRI);

  bool selectWriteTM(MachineInstr &I, MachineRegisterInfo &MRI,
                     unsigned Opcode);
  bool selectReadTM(MachineInstr &I, MachineRegisterInfo &MRI, unsigned Opcode);

  bool selectExtractI128(MachineInstr &I, Register DstReg, Register SrcReg,
                         MachineRegisterInfo &MRI, MachineFunction &MF);
  bool selectG_AIE_UNPAD_VECTOR(MachineInstr &I, Register DstReg,
                                Register SrcReg, MachineRegisterInfo &MRI,
                                MachineFunction &MF);

  bool selectSetI128(MachineInstr &I, MachineOperand &DstReg,
                     MachineOperand &SrcReg, MachineRegisterInfo &MRI,
                     MachineFunction &MF);
  bool selectG_AIE_PAD_VECTOR_UNDEF(MachineInstr &I, MachineOperand &DstReg,
                                    MachineOperand &SrcReg,
                                    MachineRegisterInfo &MRI,
                                    MachineFunction &MF);

protected:
  void makeDeadMI(MachineInstr &MI, MachineRegisterInfo &MRI);
  virtual std::optional<AddressingModeInfo>
  getOrDefineAddressingRegister(MachineInstr &MemI, MachineRegisterInfo &MRI);
  virtual std::optional<LoadStoreOpcodes>
  getCombinedOpcodeCONVStore(const MachineInstr &MemOp,
                             const MachineInstr &CombOp,
                             std::optional<APInt> Immediate);
  bool canCombineCONVStore(MachineInstr &MemOp, MachineInstr &CombOp);
  virtual std::optional<LoadStoreOpcodes>
  getCombinedOpcodeCONVLoad(const MachineInstr &MemOp,
                            const MachineInstr &CombOp,
                            const std::optional<APInt> Immediate);
  bool canCombineCONVLoad(MachineInstr &MemOp, MachineInstr &CombOp);
  bool selectG_AIE_LOAD_CONV(MachineInstr &CONVI, MachineRegisterInfo &MRI);
  bool selectVCONV(MachineInstr &I, MachineRegisterInfo &MRI);
  bool selectStartLoop(MachineInstr &I, MachineRegisterInfo &MRI);
  bool selectLockPtrIntrinsic(MachineInstr &I);
  bool selectSetControlRegister(MachineInstr &I, MachineRegisterInfo &MRI);
  bool selectGetControlRegister(MachineInstr &I, MachineRegisterInfo &MRI);
  bool selectSetStatusRegister(MachineInstr &I, MachineRegisterInfo &MRI);
  bool selectGetStatusRegister(MachineInstr &I, MachineRegisterInfo &MRI);
  bool selectVUPS(MachineInstr &I, MachineRegisterInfo &MRI,
                  std::optional<unsigned> crUPSModeVal = std::nullopt);
  virtual bool selectG_AIE_LOAD_UPS(MachineInstr &StoreI,
                                    MachineRegisterInfo &MRI,
                                    std::optional<unsigned> crUPSModeVal) {
    return false;
  }
  virtual bool setUnpackSizeRegister(MachineIRBuilder &MIB,
                                     Intrinsic::ID IntrinsicID) {
    return false;
  }
  virtual bool canCombineUNPACKLoad(MachineInstr &MemOp, MachineInstr &CombOp,
                                    MachineRegisterInfo &MRI) = 0;
  virtual std::optional<LoadStoreOpcodes> getCombinedOpcodeUNPACKLoad(
      const MachineInstr &MemOp, const MachineInstr &CombOp,
      std::optional<APInt> Immediate, bool IsSigned) = 0;
  bool selectG_AIE_LOAD_UNPACK(MachineInstr &UNPACKI, MachineRegisterInfo &MRI);

  // Virtual methods for PACK combine
  virtual bool canCombinePACK(MachineInstr &MemOp, MachineInstr &CombOp,
                              MachineRegisterInfo &MRI) {
    return false;
  }
  virtual std::optional<LoadStoreOpcodes>
  getCombinedOpcodePACK(const MachineInstr &MemOp, const MachineInstr &CombOp,
                        std::optional<APInt> Immediate, bool IsSigned) {
    return {};
  }
  /// Returns true if the intrinsic produces 8-bit output (vs 4-bit).
  virtual bool isPackI8Intrinsic(Intrinsic::ID IntrinsicID) const {
    llvm_unreachable("Target didn't implement isPackI8Intrinsic!");
  }
  virtual bool selectG_AIE_STORE_PACK(MachineInstr &StoreI,
                                      MachineRegisterInfo &MRI);

  /// Shared implementation for G_AIE_STORE_SRS selection.
  /// Returns true if SRS combine was successful.
  virtual bool selectG_AIE_STORE_SRS(MachineInstr &StoreI,
                                     MachineRegisterInfo &MRI);

  /// Check if SRS combine is possible. Override in derived classes.
  virtual bool canCombineSRS(MachineInstr &MemOp, MachineInstr &CombOp,
                             MachineRegisterInfo &MRI) {
    return false;
  }

  /// Get combined opcode for SRS store. Override in derived classes.
  virtual std::optional<LoadStoreOpcodes>
  getCombinedOpcodeSRS(const MachineInstr &MemOp, const MachineInstr &CombOp,
                       std::optional<APInt> Immediate, bool IsSigned) {
    return std::nullopt;
  }

  /// Get SRS mode (0 for 32-bit acc, 1 for 64-bit acc) based on intrinsic.
  virtual std::optional<unsigned> getSRSModeForIntrinsic(Intrinsic::ID ID) {
    return std::nullopt;
  }

  // FIFO store selection helpers - common implementation for derived classes
  bool selectVST_FIFO_Push(MachineInstr &I, MachineRegisterInfo &MRI);
  bool selectVST_FIFO_Flush(MachineInstr &I, MachineRegisterInfo &MRI);
  bool selectVST_FIFO_Flush1D(MachineInstr &I, MachineRegisterInfo &MRI);
  bool selectVST_FIFO_Flush2D(MachineInstr &I, MachineRegisterInfo &MRI);
  bool selectVST_FIFO_Flush3D(MachineInstr &I, MachineRegisterInfo &MRI);

  bool prepareLockPtrIntrinsic(MachineInstr &I, MachineMemOperand *&SavedMMO);
  bool attachLockPtrMemOperand(MachineBasicBlock &MBB,
                               MachineMemOperand *SavedMMO);

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

/// Get a constant VReg value or call llvm_unreachable with the provided message
/// \param VReg The virtual register to lookup
/// \param MRI The machine register info
/// \param ErrorMsg The error message to display if constant is not found
/// \return The ValueAndVReg containing the constant value
inline ValueAndVReg getIConstantVRegValWithLookThroughOrFail(
    Register VReg, const MachineRegisterInfo &MRI, const char *ErrorMsg) {
  auto Result = getIConstantVRegValWithLookThrough(VReg, MRI);
  if (!Result)
    llvm_unreachable(ErrorMsg);
  return *Result;
}

} // namespace llvm

#undef DEBUG_TYPE
#endif // LLVM_LIB_TARGET_AIE_AIEBASEINSTRUCTIONSELECTOR_H
