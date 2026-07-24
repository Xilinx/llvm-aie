//===-- AIE1InstrInfo.h - AIE Instruction Information --------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE1INSTRINFO_H
#define LLVM_LIB_TARGET_AIE_AIE1INSTRINFO_H

#include "AIE.h"
#include "AIE1RegisterInfo.h"
#include "AIEBaseInstrInfo.h"
#include "MCTargetDesc/AIEFormat.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "AIEGenInstrInfo.inc"

namespace llvm {

class AIEInstrInfo : public AIEGenInstrInfo {
public:
  AIEInstrInfo();

  /// Allocate and return a hazard recognizer to use for this target when
  /// scheduling the machine instructions after register allocation.
  ScheduleHazardRecognizer *
  CreateTargetPostRAHazardRecognizer(const InstrItineraryData *II,
                                     const ScheduleDAG *DAG) const override;

  Register isLoadFromStackSlot(const MachineInstr &MI,
                               int &FrameIndex) const override;
  Register isStoreToStackSlot(const MachineInstr &MI,
                              int &FrameIndex) const override;

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI,
                   const DebugLoc &DL, Register DstReg, Register SrcReg,
                   bool KillSrc, bool RenamableDest = false,
                   bool RenamableSrc = false) const override;

  bool expandPostRAPseudo(MachineInstr &MI) const override;

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

  unsigned getInstSizeInBytes(const MachineInstr &MI) const override;

  /// Return an opcode that reverses the condition of the given one
  /// \param Opc Conditional branch opcode to reverse
  unsigned getOppositeBranchOpcode(unsigned Opc) const override;

  unsigned getJumpOpcode() const override;

  unsigned getOffsetMemOpcode(unsigned BaseMemOpcode) const override;
  std::optional<unsigned>
  getCombinedPostIncOpcode(MachineInstr &BaseMemI, MachineInstr &PtrAddI,
                           TypeSize Size) const override;

  // Implement MIR serialization of target flags
  std::pair<unsigned, unsigned>
  decomposeMachineOperandsTargetFlags(unsigned TF) const override;

  ArrayRef<std::pair<unsigned, const char *>>
  getSerializableDirectMachineOperandTargetFlags() const override;

  unsigned getReturnOpcode() const override;
  unsigned getCallOpcode(const MachineFunction &CallerF, bool IsIndirect,
                         bool IsTailCall) const override;
  bool isCall(unsigned Opc) const override;
  unsigned getNopOpcode() const override;
  unsigned getMvSclOpcode() const override;

  bool canHoistCheapInst(const MachineInstr &MI) const override;
  unsigned getBasicVecRegSize() const override { return 256; };

  unsigned getMachineBlockAlignmentBytes() const override { return 16; }

  // AIE1 has no variable itineraries, return empty interface.
  VarItinInterface getVarItinInterface() const override { return {}; }

protected:
  SmallVector<AIEPseudoExpandInfo, 4>
  getSpillPseudoExpandInfo(const TargetRegisterInfo &TRI,
                           MachineInstr &MI) const override;
};
} // namespace llvm
#endif // LLVM_LIB_TARGET_AIE_AIE1INSTRINFO_H
