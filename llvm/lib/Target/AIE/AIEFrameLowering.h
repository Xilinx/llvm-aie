//===-- AIEFrameLowering.h - Define frame lowering for AIE -*- C++ -*--===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This class implements AIE-specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEFRAMELOWERING_H
#define LLVM_LIB_TARGET_AIE_AIEFRAMELOWERING_H

#include "AIEBaseFrameLowering.h"
namespace llvm {
class AIEFrameLowering : public AIEBaseFrameLowering {
public:
  explicit AIEFrameLowering(const AIEBaseSubtarget &STI)
      : AIEBaseFrameLowering(STI) {}

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  static void adjustReg(const TargetInstrInfo *TII, MachineBasicBlock &MBB,
                        MachineBasicBlock::iterator MBBI, const DebugLoc &DL,
                        unsigned Reg, int64_t Val,
                        MachineInstr::MIFlag Flag = MachineInstr::NoFlags);

private:
  void adjustSPReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI,
                   const DebugLoc &DL, int64_t Val,
                   MachineInstr::MIFlag Flag) const override;
};
} // namespace llvm
#endif // LLVM_LIB_TARGET_AIE_AIEFRAMELOWERING_H
