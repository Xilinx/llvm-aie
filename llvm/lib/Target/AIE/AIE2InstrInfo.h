//===-- AIE2InstrInfo.h - AIEngine V2 Instruction Information --*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIEngine V2 implementation of the TargetInstrInfo
// class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE2_AIE2INSTRINFO_H
#define LLVM_LIB_TARGET_AIE2_AIE2INSTRINFO_H

#include "AIE2.h"
#include "AIE2RegisterInfo.h"
#include "AIEBaseInstrInfo.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "AIE2GenInstrInfo.inc"

namespace llvm {

class AIE2InstrInfo : public AIE2GenInstrInfo {
public:
  AIE2InstrInfo();

  unsigned getReturnOpcode() const override;
  unsigned getCallOpcode(const MachineFunction &CallerF, bool IsIndirect,
                         bool IsTailCall) const override;
  unsigned getNopOpcode() const override;
  unsigned getOppositeBranchOpcode(unsigned Opc) const override;
  unsigned getJumpOpcode() const override;
  unsigned getPseudoMoveOpcode() const override;
  unsigned getConstantMovOpcode(MachineRegisterInfo &MRI, unsigned int Reg,
                                APInt &Val) const override;
  unsigned getScalarMovOpcode(Register DstReg, Register SrcReg) const override;
  unsigned getMvSclOpcode() const override;
  unsigned getAddrIntrinsic2D() const override;
  unsigned getAddrIntrinsic3D() const override;
  unsigned getPtrAdd2DOpcode() const override;
  unsigned getPtrAdd3DOpcode() const override;
  unsigned getMvSclMultiSlotPseudoOpcode() const override;
  unsigned getAddSclOpcode() const override;
  unsigned getMoveToMSOpcode(MachineInstr &I,
                             unsigned ConstTLastVal) const override;
  Register getSSStatusReg() const override;
  Register getMSStatusReg() const override;
  Register getPackSignCReg() const override;
  Register getUnpackSignCReg() const override;
  unsigned getGenericAddVectorEltOpcode() const override;
  unsigned getGenericInsertVectorEltOpcode() const override;
  unsigned getGenericExtractVectorEltOpcode(bool SignExt) const override;
  unsigned getGenericPadVectorOpcode() const override;
  unsigned getGenericUnpadVectorOpcode() const override;
  unsigned getGenericBroadcastVectorOpcode() const override;
  unsigned getGenericPostIncLoadOpcode() const override;
  unsigned getGenericPostIncStoreOpcode() const override;
  unsigned getCycleSeparatorOpcode() const override;
  bool isLock(unsigned Opc) const override;
  std::optional<unsigned> getDoneLatency(unsigned) const override;
  bool isDelayedSchedBarrier(const MachineInstr &MI) const override;
  bool isSchedBarrier(const MachineInstr &MI) const override;

  unsigned getScalarRegSize() const override;
  unsigned getBasicVecRegSize() const override;

  unsigned getBasicVectorBitSize() const override;
  unsigned getMaxVectorBitSize() const override;
  unsigned getMaxSupportedLdStIncSize() const override;

  virtual unsigned
  getNumReservedDelaySlots(const MachineInstr &MI) const override;

  unsigned getMachineBlockAlignmentBytes() const override { return 16; }

  bool isJNZ(unsigned Opc) const override;
  bool isJZ(unsigned Opc) const override;
  bool isCall(unsigned Opc) const override;
  bool jumpsToUnknown(unsigned Opcode) const override;
  bool isIConst(unsigned Opc) const override;
  bool isBooleanNoOp(unsigned Opc) const override;
  bool isBooleanNot(unsigned Opc) const override;
  bool isConstStep(const MachineInstr &MI, int64_t &Step) const override;
  bool isGenericOffsetMemOpcode(unsigned Opcode) const override;

  bool verifyGenericInstruction(const MachineInstr &MI,
                                StringRef &ErrInfo) const override;
  bool verifyMemOperand(const MachineInstr &MI,
                        StringRef &ErrInfo) const override;

  unsigned getOffsetMemOpcode(unsigned BaseMemOpcode) const override;
  std::optional<unsigned>
  getCombinedPostIncOpcode(MachineInstr &BaseMemI, MachineInstr &PtrAddI,
                           TypeSize Size) const override;
  unsigned getOpCode(MachineInstr &MI) const override;
  Register getVaddSignControlRegister() const override;
  Register getUPSSignControlRegister() const override;

  virtual std::optional<ZOLSupport> getZOLSupport() const override;
  virtual std::optional<JNZDSupport> getJNZDSupport() const override;
  virtual std::optional<IfConvSupport> getIfConvSupport() const override;

  virtual bool
  isOffsetInImmediateRange(unsigned Opcode, unsigned LoadStoreSize,
                           std::optional<APInt> Immediate) const override;

  virtual unsigned getPseudoJNZDOpcode() const override;

  unsigned getNumBypassedCycles(const InstrItineraryData *ItinData,
                                const MachineInstr &DefMI, unsigned DefIdx,
                                const MachineInstr &UseMI,
                                unsigned UseIdx) const override;

  std::optional<int> getFirstMemoryCycle(unsigned SchedClass) const override;

  std::optional<int> getLastMemoryCycle(unsigned SchedClass) const override;

  int getMinFirstMemoryCycle() const override;
  int getMaxFirstMemoryCycle() const override;
  int getMinLastMemoryCycle() const override;
  int getMaxLastMemoryCycle() const override;

  SmallVector<int, 2> getMemoryCycles(unsigned SchedClass) const override;

  VarItinInterface getVarItinInterface() const override;

  SmallVector<TiedRegOperands, 4>
  getTiedRegInfo(unsigned Opcode) const override;

  std::optional<unsigned>
  getOpcodeWithAtomicOperands(unsigned Opcode) const override;

  TiedRegOperands getTiedRegInfoForSplitting(unsigned Opcode) const override;

  std::optional<unsigned>
  getOpcodeWithTupleOperands(unsigned Opcode) const override;

  std::optional<PseudoBranchExpandInfo>
  getPseudoBranchExpandInfo(const MachineInstr &MI) const override;

  /// Allocate and return a hazard recognizer to use for this target when
  /// scheduling the machine instructions after register allocation.
  ScheduleHazardRecognizer *
  CreateTargetPostRAHazardRecognizer(const InstrItineraryData *II,
                                     const ScheduleDAG *DAG) const override;
  ScheduleHazardRecognizer *
  CreateTargetMIHazardRecognizer(const InstrItineraryData *II,
                                 const ScheduleDAGMI *DAG) const override;

  /// insertNoop - Insert a noop into the instruction stream at the specified
  /// point.
  void insertNoop(MachineBasicBlock &MBB,
                  MachineBasicBlock::iterator MI) const override;

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI,
                   const DebugLoc &DL, MCRegister DstReg, MCRegister SrcReg,
                   bool KillSrc, bool RenamableDest = false,
                   bool RenamableSrc = false) const override;

  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MBBI, Register SrcReg,
                           bool IsKill, int FrameIndex,
                           const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI, Register VReg,
                           MachineInstr::MIFlag Flags) const override;

  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI, Register DstReg,
                            int FrameIndex, const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI, Register VReg,
                            MachineInstr::MIFlag Flags) const override;

  bool expandPostRAPseudo(MachineInstr &MI) const override;

  // Implement MIR serialization of target flags
  std::pair<unsigned, unsigned>
  decomposeMachineOperandsTargetFlags(unsigned TF) const override;

  ArrayRef<std::pair<unsigned, const char *>>
  getSerializableDirectMachineOperandTargetFlags() const override;

  bool canHoistCheapInst(const MachineInstr &MI) const override;

  std::optional<const VConcatOpInfo>
  getVConcatOpInfo(const MachineInstr &MI) const override;

  std::optional<const VUpdateOpInfo>
  getVUpdateOpInfo(const MachineInstr &MI) const override;

  std::optional<const VExtractOpInfo>
  getVExtractOpInfo(const MachineInstr &MI) const override;

  unsigned getMaxLoadStoreSize() const override;

  bool canCombineWithLoadStore(const MachineInstr &MI) const override;

  bool isProfitableToSplitType(const LLT Ty) const override;

  std::optional<const AbstractOp>
  parseAbstractOp(const MachineInstr &MI) const override;

protected:
  SmallVector<AIEPseudoExpandInfo, 4>
  getSpillPseudoExpandInfo(const TargetRegisterInfo &TRI,
                           MachineInstr &MI) const override;
  SmallVector<AIEPseudoExpandInfo, 4>
  getSpillPseudoExpandInfoByOpcode(unsigned Opcode) const override;
};
} // namespace llvm
#endif
