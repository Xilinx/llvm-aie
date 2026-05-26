//===----- AIE2PSInstrInfo.h -AIE2ps Instruction Information *- C++ -----*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE2ps implementation of the TargetInstrInfo
// class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE2PS_AIE2PSINSTRINFO_H
#define LLVM_LIB_TARGET_AIE2PS_AIE2PSINSTRINFO_H

#include "AIE2PSRegisterInfo.h"
#include "AIEBaseInstrInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "AIE2PSGenInstrInfo.inc"

namespace llvm {

class AIE2PSInstrInfo : public AIE2PSGenInstrInfo {
public:
  AIE2PSInstrInfo();

  unsigned getReturnOpcode() const override;
  unsigned getAddrIntrinsic2D() const override;
  unsigned getAddrIntrinsic3D() const override;
  unsigned getPtrAdd2DOpcode() const override;
  unsigned getPtrAdd3DOpcode() const override;
  unsigned getMvSclMultiSlotPseudoOpcode() const override;
  unsigned getOppositeBranchOpcode(unsigned Opc) const override;
  unsigned getJumpOpcode() const override;

  unsigned getOffsetMemOpcode(unsigned BaseMemOpcode) const override;
  std::optional<unsigned>
  getCombinedPostIncOpcode(MachineInstr &BaseMemI, MachineInstr &PtrAddI,
                           TypeSize Size) const override;
  Register getVaddSignControlRegister() const override;
  unsigned getOpCode(MachineInstr &MI) const override;

  bool isOffsetInImmediateRange(unsigned Opcode, unsigned LoadStoreSize,
                                std::optional<APInt> Immediate) const override;
  bool isGenericOffsetMemOpcode(unsigned Opcode) const override;

  bool isFifoStoreConvOpcode(unsigned Opcode) const override;
  std::optional<unsigned>
  getStoreFlushConvOpcode(unsigned StoreFlushOpcode) const override;

  std::optional<int> getFirstMemoryCycle(unsigned SchedClass) const override;

  std::optional<int> getLastMemoryCycle(unsigned SchedClass) const override;

  int getMinFirstMemoryCycle() const override;
  int getMaxFirstMemoryCycle() const override;
  int getMinLastMemoryCycle() const override;
  int getMaxLastMemoryCycle() const override;

  SmallVector<int, 2> getMemoryCycles(unsigned SchedClass) const override;

  VarItinInterface getVarItinInterface() const override;

  ScheduleHazardRecognizer *
  CreateTargetMIHazardRecognizer(const InstrItineraryData *II,
                                 const ScheduleDAGMI *DAG) const override;

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI,
                   const DebugLoc &DL, MCRegister DstReg, MCRegister SrcReg,
                   bool KillSrc, bool RenamableDest = false,
                   bool RenamableSrc = false) const override;

  Register isLoadFromStackSlot(const MachineInstr &MI,
                               int &FrameIndex) const override;
  Register isStoreToStackSlot(const MachineInstr &MI,
                              int &FrameIndex) const override;

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

  unsigned getNopOpcode() const override;
  unsigned getMvSclOpcode() const override;
  unsigned getAddSclOpcode() const override;
  unsigned getPseudoMoveOpcode() const override;
  unsigned getConstantMovOpcode(MachineRegisterInfo &MRI, unsigned int Reg,
                                APInt &Val) const override;
  unsigned getScalarMovOpcode(Register DstReg, Register SrcReg) const override;

  bool isLock(unsigned Opc) const override;
  std::optional<unsigned> getDoneLatency(unsigned) const override;
  bool isCall(unsigned Opc) const override;
  bool jumpsToUnknown(unsigned Opcode) const override;
  bool isIConst(unsigned Opc) const override;

  std::optional<PseudoBranchExpandInfo>
  getPseudoBranchExpandInfo(const MachineInstr &MI) const override;

  unsigned getNumReservedDelaySlots(const MachineInstr &MI) const override;
  bool isDelayedSchedBarrier(const MachineInstr &MI) const override;
  bool isSchedBarrier(const MachineInstr &MI) const override;

  unsigned getScalarRegSize() const override;
  unsigned getBasicVecRegSize() const override;

  unsigned getBasicVectorBitSize() const override;
  unsigned getMaxVectorBitSize() const override;
  unsigned getMaxSupportedLdStIncSize() const override;

  unsigned getMachineBlockAlignmentBytes() const override { return 16; }

  unsigned getCallOpcode(const MachineFunction &CallerF, bool IsIndirect,
                         bool IsTailCall) const override;
  Register getPackSignCReg() const override;
  Register getUnpackSignCReg() const override;
  Register getSSStatusReg() const override;
  Register getMSStatusReg() const override;
  unsigned getMoveToMSOpcode(MachineInstr &I,
                             unsigned ConstTLastVal) const override;
  unsigned getCycleSeparatorOpcode() const override;
  unsigned getGenericAddVectorEltOpcode() const override;
  unsigned getGenericInsertVectorEltOpcode() const override;
  unsigned getGenericExtractVectorEltOpcode(bool SignExt) const override;
  unsigned getGenericUnpadVectorOpcode() const override;
  unsigned getGenericPadVectorOpcode() const override;
  unsigned getGenericBroadcastVectorOpcode() const override;
  unsigned getGenericVSelOpcode() const override;
  unsigned getGenericVShiftOpcode() const override;
  unsigned getGenericShuffleVectorOpcode() const override;
  unsigned getGenericPostIncLoadOpcode() const override;
  unsigned getGenericPostIncStoreOpcode() const override;
  std::optional<PtrPostIncOpInfo>
  getPtrPostIncOpInfo(unsigned Opcode) const override;
  unsigned getGenericExtractSubvectorOpcode() const override;
  unsigned getGenericIntegerComparisonOpcode() const override;

  std::optional<ZOLSupport> getZOLSupport() const override;
  std::optional<JNZDSupport> getJNZDSupport() const override;
  std::optional<IfConvSupport> getIfConvSupport() const override;

  // Implement MIR serialization of target flags
  std::pair<unsigned, unsigned>
  decomposeMachineOperandsTargetFlags(unsigned TF) const override;

  ArrayRef<std::pair<unsigned, const char *>>
  getSerializableDirectMachineOperandTargetFlags() const override;

  std::optional<const AbstractOp>
  parseAbstractOp(const MachineInstr &MI) const override;

  SmallVector<TiedRegOperands, 4>
  getTiedRegInfo(unsigned Opcode) const override;

  std::optional<unsigned>
  getOpcodeWithAtomicOperands(unsigned Opcode) const override;

  TiedRegOperands getTiedRegInfoForSplitting(unsigned Opcode) const override;

  std::optional<unsigned>
  getOpcodeWithTupleOperands(unsigned Opcode) const override;

  ArrayRef<WidenNarrowConversionPair>
  getWidenNarrowConversionPairs() const override;

protected:
  SmallVector<AIEPseudoExpandInfo, 4>
  getSpillPseudoExpandInfo(const TargetRegisterInfo &TRI,
                           MachineInstr &MI) const override;
  AIERegOffsetSpillInstrInfo
  getRegOffsetSpillInstrInfoFromImmOffset(const unsigned Opcode) const override;

  int isRoundRobinSlotCandidate(MachineInstr &MI) const override;
};
} // namespace llvm
#endif
