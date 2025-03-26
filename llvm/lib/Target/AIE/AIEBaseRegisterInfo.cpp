//=== AIEBaseRegisterInfo.cpp - Common AIE Register Information -*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains common register code between AIE versions.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseRegisterInfo.h"
#include "AIEBaseInstrInfo.h"
#include "AIEBaseSubtarget.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/CodeGen/LiveIntervals.h"

using namespace llvm;
extern cl::opt<bool> EnableCoalescingForWideCopy;

bool AIEBaseRegisterInfo::shouldCoalesce(
    MachineInstr *MI, const TargetRegisterClass *SrcRC, unsigned SubReg,
    const TargetRegisterClass *DstRC, unsigned DstSubReg,
    const TargetRegisterClass *NewRC, LiveIntervals &LIS) const {
  MachineBasicBlock *MBB = MI->getParent();
  if (EnableCoalescingForWideCopy || AIELoopUtils::isSingleMBBLoop(MBB)) {
    return TargetRegisterInfo::shouldCoalesce(MI, SrcRC, SubReg, DstRC,
                                              DstSubReg, NewRC, LIS);
  }
  const unsigned SrcSize = getRegSizeInBits(*SrcRC);
  const unsigned DstSize = getRegSizeInBits(*DstRC);
  MachineFunction *MF = MI->getMF();
  const AIEBaseInstrInfo *TII =
      static_cast<const AIEBaseInstrInfo *>(MF->getSubtarget().getInstrInfo());
  const unsigned BasicVectorSize = TII->getBasicVecRegSize();
  // Should not coalesce if copying from bigger source.
  if (SrcSize < DstSize &&
      (SrcSize >= BasicVectorSize || DstSize >= BasicVectorSize)) {
    LiveInterval &LI = LIS.getInterval(MI->getOperand(1).getReg());
    const MachineInstr *FirstMI =
        LI.empty() ? nullptr : LIS.getInstructionFromIndex(LI.beginIndex());
    const MachineInstr *LastMI =
        LI.empty() ? nullptr : LIS.getInstructionFromIndex(LI.endIndex());
    // Coalescing inside the same basic block found beneficial. So, check that
    // the LiveInterval is not just local to MBB.
    if (!FirstMI || FirstMI->getParent() != MBB || !LastMI ||
        LastMI->getParent() != MBB)
      return false;
  }

  return TargetRegisterInfo::shouldCoalesce(MI, SrcRC, SubReg, DstRC, DstSubReg,
                                            NewRC, LIS);
}
