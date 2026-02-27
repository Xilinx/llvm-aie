//===- AIESlotUtils.cpp - Utilities related to slots ----------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIESlotUtils.h"
#include "AIEBaseInstrInfo.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm/CodeGen/MachineInstr.h"

namespace llvm::AIE {

SlotCounts getSlotCounts(unsigned Opcode, const AIEBaseInstrInfo *TII) {
  const auto *SlotInfo = TII->getSlotInfo(TII->getSlotKind(Opcode));
  return SlotInfo ? SlotCounts{SlotInfo->getSlotSet()} : SlotCounts{};
}

SlotCounts getConflictCounts(unsigned Opcode, const AIEBaseInstrInfo *TII) {
  const auto *SlotInfo = TII->getSlotInfo(TII->getSlotKind(Opcode));
  return SlotInfo ? SlotCounts{SlotInfo->getConflictSet()} : SlotCounts{};
}

} // namespace llvm::AIE
