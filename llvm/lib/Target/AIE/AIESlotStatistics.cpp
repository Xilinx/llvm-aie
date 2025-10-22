//===- AIESlotStatistics.cpp - Statistics on slot occupations -------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEBaseInstrInfo.h"
#include "AIESlotUtils.h"
#include "llvm/CodeGen/MachineInstr.h"

namespace llvm::AIE {

SlotStatistics::SlotStatistics(ArrayRef<int> FixedValue,
                               ArrayRef<int> FreeValue) {
  for (unsigned I = 0; I < FixedValue.size(); I++) {
    Fixed[I] = FixedValue[I];
  }
  for (unsigned I = 0; I < FreeValue.size(); I++) {
    Free[I] = FreeValue[I];
  }
}

void SlotStatistics::addInstruction(MachineInstr &MI,
                                    const AIEBaseInstrInfo *TII) {
  const unsigned Opcode = MI.getOpcode();
  const AIEBaseMCFormats *const Formats = TII->getFormatInterface();
  const auto *const Alternatives = Formats->getAlternateInstsOpcode(Opcode);
  if (Alternatives) {
    MSPs.push_back(&MI);
    SlotCounts Term;
    const int Scale = SlotStatistics::Unit / Alternatives->size();
    for (unsigned AltOpcode : *Alternatives) {
      Term += Scale * getSlotCounts(AltOpcode, TII);
    }
    MSPSlotCounts.emplace(&MI, Term);
    Free += Term;
  } else {
    const int Scale = SlotStatistics::Unit;
    const SlotCounts Term = Scale * getSlotCounts(Opcode, TII);
    Fixed += Term;
  }
}

int SlotStatistics::distance(const SlotStatistics &Other) const {
  return Fixed.distance(Other.Fixed) + Free.distance(Other.Free);
}

void SlotStatistics::dump() const {
  dbgs() << "Fixed:\n  " << Fixed << "\n";

  dbgs() << "MSPs:\n";
  for (auto &[MSP, Counts] : MSPSlotCounts) {
    dbgs() << "  " << Counts << "\n";
  }
  dbgs() << "Free:\n  " << Free << "\n";
}

void SlotStatistics::dumpShort() const {
  dbgs() << "{ " << Fixed << ", " << Free << " },\n";
}

SlotStatistics computeSlotStatistics(MachineBasicBlock &MBB,
                                     const AIEBaseInstrInfo *TII) {
  SlotStatistics Result;
  for (auto &MI : MBB) {
    Result.addInstruction(MI, TII);
  }
  return Result;
}

int SlotStatistics::getMinII() const {
  SlotCounts Total = Fixed + Free;
  // This rounds down, but any fractional part results from a probability
  return Total.max() / Unit;
}

} // namespace llvm::AIE
