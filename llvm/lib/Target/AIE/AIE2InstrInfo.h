//===-- AIE2InstrInfo.h - AIEngine V2 Instruction Information --*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
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
  MCSlotKind getSlotKind(unsigned Opcode) const override;
  const MCSlotInfo *getSlotInfo(const MCSlotKind Kind) const override;
  const PacketFormats &getPacketFormats() const override;
  unsigned getNopOpcode(size_t Size = 0) const override;
  unsigned getOppositeBranchOpcode(unsigned Opc) const override;
  unsigned getJumpOpcode() const override;
  unsigned getPseudoMoveOpcode() const override;
  unsigned getConstantMovOpcode(MachineRegisterInfo &MRI, unsigned int Reg,
                                APInt &Val) const override;
  unsigned getScalarMovOpcode(Register DstReg, Register SrcReg) const override;
  unsigned getGenericAddVectorEltOpcode() const override;
  unsigned getGenericInsertVectorEltOpcode() const override;
  unsigned getGenericExtractVectorEltOpcode(bool SignExt) const override;
  unsigned getGenericPadVectorOpcode() const override;
  unsigned getGenericUnpadVectorOpcode() const override;
  unsigned getCycleSeparatorOpcode() const override;
  bool isLock(unsigned Opc) const override;
  bool isDelayedSchedBarrier(const MachineInstr &MI) const override;
  bool isSchedBarrier(const MachineInstr &MI) const override;

  virtual unsigned
  getNumReservedDelaySlots(const MachineInstr &MI) const override;

  bool isJNZ(unsigned Opc) const override;
  bool isJZ(unsigned Opc) const override;
  bool isCall(unsigned Opc) const override;
  bool jumpsToUnknown(unsigned Opcode) const override;
  bool isIConst(unsigned Opc) const override;
  bool isBooleanNoOp(unsigned Opc) const override;
  bool isBooleanNot(unsigned Opc) const override;
  bool isConstStep(const MachineInstr &MI, int64_t &Step) const override;

  bool verifyGenericInstruction(const MachineInstr &MI,
                                StringRef &ErrInfo) const override;
  bool verifyMemOperand(const MachineInstr &MI,
                        StringRef &ErrInfo) const override;

  unsigned getOffsetMemOpcode(unsigned BaseMemOpcode) const override;
  std::optional<unsigned>
  getCombinedPostIncOpcode(MachineInstr &BaseMemI, MachineInstr &PtrAddI,
                           TypeSize Size) const override;

  virtual bool isHardwareLoopDec(unsigned Opcode) const override;
  virtual bool isHardwareLoopJNZ(unsigned Opcode) const override;
  virtual bool isHardwareLoopStart(unsigned Opcode) const override;
  virtual bool isHardwareLoopEnd(unsigned Opcode) const override;

  bool isZOLTripCountDef(const MachineInstr &MI,
                         bool Pristine = false) const override;
  void adjustTripCount(MachineInstr &MI, int Adjustment) const override;

  virtual bool
  isZeroOverheadLoopSetupInstr(const MachineInstr &) const override;
  virtual std::vector<MachineBasicBlock::iterator>
  getAlignmentBoundaries(MachineBasicBlock &MBB) const override;

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

  virtual unsigned
  getSchedClass(const MCInstrDesc &Desc,
                iterator_range<const MachineOperand *> Operands,
                const MachineRegisterInfo &MRI) const override;

  SmallVector<TiedRegOperands, 4>
  getTiedRegInfo(unsigned Opcode) const override;

  std::optional<unsigned>
  getOpcodeWithAtomicOperands(unsigned Opcode) const override;

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
                   bool KillSrc) const override;

  Register isLoadFromStackSlot(const MachineInstr &MI,
                               int &FrameIndex) const override;
  Register isStoreToStackSlot(const MachineInstr &MI,
                              int &FrameIndex) const override;

  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MBBI, Register SrcReg,
                           bool IsKill, int FrameIndex,
                           const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI,
                           Register VReg) const override;

  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI, Register DstReg,
                            int FrameIndex, const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI,
                            Register VReg) const override;

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
  getSpillPseudoExpandInfo(const MachineInstr &MI) const override;
};
} // namespace llvm
#endif
