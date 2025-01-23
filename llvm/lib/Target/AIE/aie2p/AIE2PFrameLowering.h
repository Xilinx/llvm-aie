//===--------------------- AIE2PFrameLowering.h ----------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This class implements AIE2p-specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE2PFRAMELOWERING_H
#define LLVM_LIB_TARGET_AIE_AIE2PFRAMELOWERING_H

#include "AIEBaseFrameLowering.h"

namespace llvm {
class AIE2PSubtarget;

class AIE2PFrameLowering : public AIEBaseFrameLowering {
public:
  explicit AIE2PFrameLowering(const AIEBaseSubtarget &STI)
      : AIEBaseFrameLowering(STI,
                             /*StackAlignment=*/Align(64)) {}
  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                            RegScavenger *RS = nullptr) const override;
  void processFunctionBeforeFrameFinalized(MachineFunction &MF,
                                           RegScavenger *RS) const override;

private:
  void adjustReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI,
                 const DebugLoc &DL, unsigned Reg, int64_t Val,
                 MachineInstr::MIFlag Flag) const override;
  void adjustSPReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI,
                   const DebugLoc &DL, int64_t Val,
                   MachineInstr::MIFlag Flag) const override;
};
} // namespace llvm
#endif // LLVM_LIB_TARGET_AIE_AIE2PFRAMELOWERING_H
