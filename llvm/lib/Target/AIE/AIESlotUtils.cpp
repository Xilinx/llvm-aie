//===- AIESlotUtils.cpp - Utilities related to slots ----------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIESlotUtils.h"
#include "AIEBaseInstrInfo.h"
#include "llvm/CodeGen/MachineInstr.h"

namespace llvm::AIE {

uint64_t getSlotSet(unsigned Opcode, const AIEBaseInstrInfo *TII) {
  auto *SlotInfo = TII->getSlotInfo(TII->getSlotKind(Opcode));
  return SlotInfo ? SlotInfo->getSlotSet() : 0;
}

SlotCounts getSlotCounts(unsigned Opcode, const AIEBaseInstrInfo *TII) {
  return SlotCounts{getSlotSet(Opcode, TII)};
}

} // namespace llvm::AIE
