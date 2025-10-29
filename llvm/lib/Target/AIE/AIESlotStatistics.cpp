//===- AIESlotStatistics.cpp - Statistics on slot occupations -------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIESlotStatistics.h"
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

void SlotStatistics::dump() {
  dbgs() << "Fixed:\n  " << Fixed << "\n";

  dbgs() << "MSPs:\n";
  for (auto &[MSP, Counts] : MSPSlotCounts) {
    dbgs() << "  " << Counts << "\n";
  }
  dbgs() << "Free:\n  " << Free << "\n";
}

SlotStatistics computeSlotStatistics(MachineBasicBlock &MBB,
                                     const AIEBaseInstrInfo *TII) {
  SlotStatistics Result;

  const AIEBaseMCFormats *Formats = TII->getFormatInterface();
  for (auto &MI : MBB) {
    unsigned Opcode = MI.getOpcode();
    const auto *Alternatives = Formats->getAlternateInstsOpcode(Opcode);
    if (Alternatives) {
      Result.MSPs.push_back(&MI);
      SlotCounts Term;
      for (unsigned AltOpcode : *Alternatives) {
        Term += getSlotCounts(AltOpcode, TII);
      }
      Term *= (SlotStatistics::Unit / Alternatives->size());
      Result.MSPSlotCounts.emplace(&MI, Term);
      Result.Free += Term;
    } else {
      Result.Fixed += SlotStatistics::Unit * getSlotCounts(Opcode, TII);
    }
  }
  return Result;
}

int SlotStatistics::getMinII() const {
  SlotCounts Total = Fixed + Free;
  // This rounds down, but any fractional part results from a probability
  return Total.max() / Unit;
}

} // namespace llvm::AIE
