//===- AIESlotStatistics.cpp - Statistics on slot occupations -------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEBaseInstrInfo.h"
#include "AIESlotUtils.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineInstr.h"

namespace llvm::AIE {

static SlotBits getConflictBits(unsigned Opcode, const AIEBaseInstrInfo *TII) {
  const auto *SlotInfo = TII->getSlotInfo(TII->getSlotKind(Opcode));
  return SlotInfo ? SlotInfo->getConflictSet() : 0;
}

/// \return the alternatives whose conflict set is not a strict superset of
/// another alternative's conflict set. A dominated alternative blocks
/// everything a dominating one blocks and more, so it is never preferred.
static SmallVector<unsigned, 4>
getNonDominatedAlternatives(ArrayRef<unsigned> Alternatives,
                            const AIEBaseInstrInfo *TII) {
  SmallVector<unsigned, 4> Result;
  for (unsigned AltOpcode : Alternatives) {
    const SlotBits Bits = getConflictBits(AltOpcode, TII);
    const bool IsDominated = any_of(Alternatives, [&](unsigned Other) {
      const SlotBits OtherBits = getConflictBits(Other, TII);
      return OtherBits != Bits && (OtherBits & ~Bits) == 0;
    });
    if (!IsDominated)
      Result.push_back(AltOpcode);
  }
  return Result;
}

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
                                    const AIEBaseInstrInfo *TII,
                                    bool SkipDominatedAlternatives) {
  const unsigned Opcode = MI.getOpcode();
  const AIEBaseMCFormats *const Formats = TII->getFormatInterface();
  const auto *const Alternatives = Formats->getAlternateInstsOpcode(Opcode);
  if (Alternatives) {
    MSPs.push_back(&MI);
    SmallVector<unsigned, 4> Spread(Alternatives->begin(), Alternatives->end());
    if (SkipDominatedAlternatives)
      Spread = getNonDominatedAlternatives(Spread, TII);
    SlotCounts Term;
    const int Scale = SlotStatistics::Unit / Spread.size();
    for (unsigned AltOpcode : Spread) {
      Term += Scale * getConflictCounts(AltOpcode, TII);
    }
    MSPSlotCounts.emplace(&MI, Term);
    Free += Term;
  } else {
    const int Scale = SlotStatistics::Unit;
    const SlotCounts Term = Scale * getConflictCounts(Opcode, TII);
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

SlotStatistics computeSlotStatistics(ArrayRef<MachineInstr *> Instrs,
                                     const AIEBaseInstrInfo *TII,
                                     bool SkipDominatedAlternatives) {
  SlotStatistics Result;
  for (auto *MI : Instrs)
    Result.addInstruction(*MI, TII, SkipDominatedAlternatives);
  return Result;
}

int SlotStatistics::getMinII() const {
  SlotCounts Total = Fixed + Free;
  // This rounds down, but any fractional part results from a probability
  return Total.max() / Unit;
}

} // namespace llvm::AIE
