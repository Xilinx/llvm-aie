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

bool AIEBaseRegisterInfo::shouldCoalesce(
    MachineInstr *MI, const TargetRegisterClass *SrcRC, unsigned SubReg,
    const TargetRegisterClass *DstRC, unsigned DstSubReg,
    const TargetRegisterClass *NewRC, LiveIntervals &LIS) const {
  return TargetRegisterInfo::shouldCoalesce(MI, SrcRC, SubReg, DstRC, DstSubReg,
                                            NewRC, LIS);
}
