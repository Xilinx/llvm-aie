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
#include "llvm/Support/CommandLine.h"

namespace llvm::AIE {

/// \return the alternatives whose conflict set is not a strict superset of
/// another alternative's conflict set. A wider alternative blocks everything a
/// narrower one blocks and more, so it only wins if the narrower one hits a
/// hazard that is not slot related. Note that "wider" refers to the conflict
/// sets, not to the instruction encoding.
static SmallVector<unsigned, 4>
getMinimalConflictAlternatives(ArrayRef<unsigned> Alternatives,
                               const AIEBaseInstrInfo *TII) {
  SmallVector<unsigned, 4> Result;
  for (unsigned AltOpcode : Alternatives) {
    const SlotBits Bits = getConflictSet(AltOpcode, TII);
    // Multi-slot pseudos only list instructions that have slot info.
    assert(Bits && "alternative of a multi-slot pseudo without slot info");
    const bool IsWider = any_of(Alternatives, [&](unsigned Other) {
      const SlotBits OtherBits = getConflictSet(Other, TII);
      return OtherBits != Bits && (OtherBits & ~Bits) == 0;
    });
    if (!IsWider)
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
                                    bool SkipWiderAlternatives) {
  const unsigned Opcode = MI.getOpcode();
  const AIEBaseMCFormats *const Formats = TII->getFormatInterface();
  const auto *const Alternatives = Formats->getAlternateInstsOpcode(Opcode);
  if (Alternatives) {
    MSPs.push_back(&MI);
    SmallVector<unsigned, 4> Spread(Alternatives->begin(), Alternatives->end());
    if (SkipWiderAlternatives)
      Spread = getMinimalConflictAlternatives(Spread, TII);
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
                                     bool SkipWiderAlternatives) {
  SlotStatistics Result;
  for (auto *MI : Instrs)
    Result.addInstruction(*MI, TII, SkipWiderAlternatives);
  return Result;
}

static cl::opt<int> SlotDemandMargin(
    "aie-slot-demand-margin", cl::Hidden, cl::init(30),
    cl::desc("Remaining slot demand (60 per instruction) by which another "
             "alternative must beat the default to be tried first"));

void SlotDemandTracker::init(ArrayRef<MachineInstr *> FreeInstrs,
                             const AIEBaseInstrInfo *TII,
                             bool SkipWiderAlternatives) {
  Stats = computeSlotStatistics(FreeInstrs, TII, SkipWiderAlternatives);
  Remaining = Stats.Fixed + Stats.Free;
  Active = true;
}

void SlotDemandTracker::removeDemand(MachineInstr &MI,
                                     const AIEBaseInstrInfo *TII) {
  if (!Active)
    return;
  // A multi-slot pseudo leaves the demand it spread over its alternatives,
  // any other instruction the demand of its own conflict set.
  if (auto It = Stats.MSPSlotCounts.find(&MI); It != Stats.MSPSlotCounts.end())
    Remaining -= It->second;
  else
    Remaining -= SlotStatistics::Unit * getConflictCounts(MI.getOpcode(), TII);
}

void SlotDemandTracker::orderAlternatives(
    SmallVectorImpl<unsigned> &Alternatives,
    const AIEBaseMCFormats *Formats) const {
  if (!Active)
    return;
  auto Demand = [&](unsigned Opcode) {
    return Remaining.at(int(Formats->getSlotKind(Opcode)));
  };
  // Fill each position with the least demanded remaining alternative if it
  // beats the next one in TableGen order by at least the margin. Otherwise
  // that one keeps its place. A margin-based comparator is not a strict weak
  // ordering, so this is a selection procedure.
  for (auto *Pos = Alternatives.begin(); Pos != Alternatives.end(); ++Pos) {
    auto *Best =
        std::min_element(Pos, Alternatives.end(), [&](unsigned A, unsigned B) {
          return Demand(A) < Demand(B);
        });
    if (Demand(*Pos) - Demand(*Best) >= SlotDemandMargin)
      std::rotate(Pos, Best, Best + 1);
  }
}

int SlotStatistics::getMinII() const {
  SlotCounts Total = Fixed + Free;
  // This rounds down, but any fractional part results from a probability
  return Total.max() / Unit;
}

} // namespace llvm::AIE
