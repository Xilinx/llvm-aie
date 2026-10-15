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
                                     const AIEBaseInstrInfo *TII) {
  SlotStatistics Result;
  for (auto *MI : Instrs)
    Result.addInstruction(*MI, TII);
  return Result;
}

static cl::opt<int> SlotDemandMargin(
    "aie-slot-demand-margin", cl::Hidden, cl::init(30),
    cl::desc("Remaining slot demand (60 per instruction) by which another "
             "alternative must beat the default to be tried first"));

void SlotDemandTracker::init(ArrayRef<MachineInstr *> FreeInstrs,
                             const AIEBaseInstrInfo *TII) {
  Stats = computeSlotStatistics(FreeInstrs, TII);
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
