//===-- AIE2PInstrInfo.h - AIE2p Instruction Information *- C++ -------*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE2p implementation of the TargetInstrInfo
// class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE2P_AIE2PINSTRINFO_H
#define LLVM_LIB_TARGET_AIE2P_AIE2PINSTRINFO_H

#include "AIE2PRegisterInfo.h"
#include "AIEBaseInstrInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "AIE2PGenInstrInfo.inc"

namespace llvm {

class AIE2PInstrInfo : public AIE2PGenInstrInfo {
public:
  AIE2PInstrInfo();

  unsigned getReturnOpcode() const override;
  unsigned getCallOpcode(const MachineFunction &CallerF, bool IsIndirect,
                         bool IsTailCall) const override;
  unsigned getNopOpcode(size_t Size = 0) const override;
  unsigned getMvSclOpcode() const override;
  unsigned getAddrIntrinsic2D() const override;
  unsigned getAddrIntrinsic3D() const override;
  unsigned getPtrAdd2DOpcode() const override;
  unsigned getPtrAdd3DOpcode() const override;
  unsigned getMvSclMultiSlotPseudoOpcode() const override;
  unsigned getAddSclOpcode() const override;
  unsigned getOppositeBranchOpcode(unsigned Opc) const override;
  unsigned getJumpOpcode() const override;
  unsigned getPseudoMoveOpcode() const override;
  unsigned getConstantMovOpcode(MachineRegisterInfo &MRI, unsigned int Reg,
                                APInt &Val) const override;
  unsigned getMvScl2MS(unsigned ConstTLastVal) const override;
  unsigned getMvNBScl2MS(unsigned ConstTLastVal) const override;
  unsigned getMvScl2MSTlastRegOpcode() const override;
  unsigned getMvNBScl2MSTlastRegOpcode() const override;
  Register getSSStatusReg() const override;
  Register getMSStatusReg() const override;
  Register getPackSignCReg() const override;
  Register getUnpackSignCReg() const override;
  unsigned getCycleSeparatorOpcode() const override;
  unsigned getGenericAddVectorEltOpcode() const override;
  unsigned getGenericInsertVectorEltOpcode() const override;
  unsigned getGenericExtractVectorEltOpcode(bool SignExt) const override;
  unsigned getGenericPadVectorOpcode() const override;
  unsigned getGenericUnpadVectorOpcode() const override;
  unsigned getGenericBroadcastVectorOpcode() const override;
  unsigned getGenericVSelOpcode() const override;
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
  unsigned getOpCode(MachineInstr &MI) const override;
  Register getVaddSignControlRegister() const override;

  virtual bool isHardwareLoopDec(unsigned Opcode) const override;
  virtual bool isHardwareLoopJNZ(unsigned Opcode) const override;
  virtual bool isHardwareLoopStart(unsigned Opcode) const override;
  virtual bool isHardwareLoopEnd(unsigned Opcode) const override;

  virtual bool
  isZeroOverheadLoopSetupInstr(const MachineInstr &) const override;

  // Between writing to zero-overhead registers (lc, ls and le) and
  // the end of the loop, must be a distance of 112 bytes
  // (7 fully-expanded 128-bit instructions) in terms of PM addresses
  unsigned getLoopSetupDistance() const override { return 7; }
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

protected:
  SmallVector<AIEPseudoExpandInfo, 4>
  getSpillPseudoExpandInfo(const MachineInstr &MI) const override;
};
} // namespace llvm
#endif
