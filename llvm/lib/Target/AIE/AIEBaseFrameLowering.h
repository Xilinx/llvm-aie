//===-- AIEBaseFrameLowering.h - Define frame lowering for AIE -*- C++ -*--===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This class implements AIE-specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEBASEFRAMELOWERING_H
#define LLVM_LIB_TARGET_AIE_AIEBASEFRAMELOWERING_H

#include "AIEBaseSubtarget.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {
class AIEBaseFrameLowering : public TargetFrameLowering {
public:
  explicit AIEBaseFrameLowering(const AIEBaseSubtarget &STI,
                                Align StackAlignment)
      : TargetFrameLowering(StackGrowsUp, StackAlignment,
                            /*LocalAreaOffset=*/0),
        STI(STI) {}

  bool hasFPImpl(const MachineFunction &MF) const override;
  StackOffset getFrameIndexReference(const MachineFunction &MF, int FI,
                                     Register &FrameReg) const override;

  bool hasReservedCallFrame(const MachineFunction &MF) const override;

  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator MI) const override;

  // Map CalleeSavedGPR class Register for spilling to GPR class Register.
  // TODO getter-setter
  mutable DenseMap<unsigned /*GPR*/, Register /*CSGPR*/> GPRTOCSGPRMap;

  bool
  assignCalleeSavedSpillSlots(MachineFunction &MF,
                              const TargetRegisterInfo *TRI,
                              std::vector<CalleeSavedInfo> &CSI) const override;

  bool spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MI,
                                 ArrayRef<CalleeSavedInfo> CSI,
                                 const TargetRegisterInfo *TRI) const override;
  bool
  restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator MI,
                              MutableArrayRef<CalleeSavedInfo> CSI,
                              const TargetRegisterInfo *TRI) const override;

  bool orderFrameObjectsIncludesCalleeSaves() const override { return true; }

  virtual void emitPrologue(MachineFunction &MF,
                            MachineBasicBlock &MBB) const override;
  virtual void emitEpilogue(MachineFunction &MF,
                            MachineBasicBlock &MBB) const override;

  /// Order stack objects by alignment (highest first) to minimize padding.
  void
  orderFrameObjects(const MachineFunction &MF,
                    SmallVectorImpl<int> &ObjectsToAllocate) const override;

protected:
  const AIEBaseSubtarget &STI;

  void determineFrameLayout(MachineFunction &MF) const;

  /// Optimize callee-saved L registers by deciding whether to save as L
  /// register pair or individual GPRs based on usage and call patterns.
  void optimizeLRegCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                               const TargetRegisterClass &LRegClass,
                               unsigned SubRegIdxEven,
                               unsigned SubRegIdxOdd) const;

private:
  virtual void adjustSPReg(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MBBI, const DebugLoc &DL,
                           int64_t Val, MachineInstr::MIFlag Flag) const {
    llvm_unreachable("adjustSPReg should be overridden");
  };
  virtual void adjustReg(MachineBasicBlock &MBB,
                         MachineBasicBlock::iterator MBBI, const DebugLoc &DL,
                         unsigned Reg, int64_t Val,
                         MachineInstr::MIFlag Flag) const {
    llvm_unreachable("adjustReg should be overridden");
  };
};

} // namespace llvm
#endif // LLVM_LIB_TARGET_AIE_AIEBASEFRAMELOWERING_H
