//===--- AIESlotOccupancy.cpp - Generalized Slot Occupancy Model ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIESlotOccupancy.h"
#include "AIESlotStructure.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm/Support/Debug.h"
#include <algorithm>

#define DEBUG_TYPE "slot-occupancy"

using namespace llvm;

SlotOccupancy::SlotOccupancy(SlotBits Slots) : Counts{} {
  // Expand each bit in Slots to an occupation count of 1
  // This unifies regular slots (capacity 1) and MSP slot classes (capacity > 1)
  // Each bit position corresponds to a slot class index
  for (unsigned ClassIdx = 0; ClassIdx < MaxSlotClasses && Slots != 0;
       ++ClassIdx) {
    if (Slots & (SlotBits(1) << ClassIdx)) {
      Counts[ClassIdx] = 1;
    }
  }
}

SlotOccupancy::SlotOccupancy(unsigned ClassIdx, uint8_t Count) : Counts{} {
  assert(ClassIdx < MaxSlotClasses && "Class index out of bounds");
  Counts[ClassIdx] = Count;
}

bool SlotOccupancy::isEmpty() const {
  return std::all_of(Counts.begin(), Counts.end(),
                     [](uint8_t C) { return C == 0; });
}

void SlotOccupancy::clear() { std::fill(Counts.begin(), Counts.end(), 0); }

void SlotOccupancy::blockResources() {
  // Set all class counts to maximum to prevent any scheduling
  std::fill(Counts.begin(), Counts.end(), UINT8_MAX);
}

SlotOccupancy &SlotOccupancy::operator|=(const SlotOccupancy &Other) {
  // Sum occupation counts element-wise
  // With max occupation count ~7 and feasible inputs, no overflow occurs
  for (unsigned I = 0; I < MaxSlotClasses; ++I) {
    Counts[I] += Other.Counts[I];
  }
  return *this;
}

SlotOccupancy SlotOccupancy::operator|(const SlotOccupancy &Other) const {
  SlotOccupancy Result(*this);
  Result |= Other;
  return Result;
}

bool SlotOccupancy::conflict(const SlotOccupancy &Other,
                             const AIEBaseMCFormats &FormatInterface) const {
  // Get slot structure from format interface
  const AIESlotStructure &SlotStructure = FormatInterface.getSlotStructure();

  // Combine the two occupancies
  SlotOccupancy Combined = *this | Other;

  // Quick pruning 1: Check if all counts are within valid range
  if (!Combined.boundedBy(SlotStructure))
    return true;

  unsigned NumRealSlots = SlotStructure.getNumRealSlots();

  // Quick pruning 2: Sum of occupancies must not exceed number of real slots
  if (Combined.total() > NumRealSlots)
    return true;

  // Now we need to check if we can materialize the MSPs to real slots
  // in a way that produces a feasible format.
  // With at most 7 MSPs, we can enumerate all possible materializations.

  // Start with real slot occupancies (these are fixed)
  SlotBits RealSlotOccupancy = 0;
  for (unsigned I = 0; I < NumRealSlots; ++I) {
    if (Combined.Counts[I] > 0) {
      RealSlotOccupancy |= (SlotBits(1) << I);
    }
  }

  // Collect MSP demands
  std::array<unsigned, MaxSlotClasses> MSPIndices;
  std::array<uint8_t, MaxSlotClasses> MSPCounts;
  unsigned NumMSPs = 0;
  for (unsigned I = NumRealSlots; I < MaxSlotClasses; ++I) {
    if (Combined.Counts[I] > 0) {
      MSPIndices[NumMSPs] = I;
      MSPCounts[NumMSPs] = Combined.Counts[I];
      ++NumMSPs;
    }
  }

  // If no MSPs, just check if the real slot pattern is feasible
  if (NumMSPs == 0) {
    return !FormatInterface.isFormatAvailable(RealSlotOccupancy);
  }

  // Try to materialize MSPs to real slots
  // For each MSP, we need to assign its count to real slots from its
  // composition This is a constraint satisfaction problem, but with max 7 MSPs
  // it's tractable
  return !tryMaterializeMSPs(SlotStructure, FormatInterface, RealSlotOccupancy,
                             MSPIndices, MSPCounts, NumMSPs, 0);
}

bool SlotOccupancy::tryMaterializeMSPs(
    const AIESlotStructure &SlotStructure,
    const AIEBaseMCFormats &FormatInterface, SlotBits CurrentRealSlots,
    const std::array<unsigned, MaxSlotClasses> &MSPIndices,
    const std::array<uint8_t, MaxSlotClasses> &MSPCounts, unsigned NumMSPs,
    unsigned MSPIdx) const {
  // Base case: all MSPs materialized, check if result is feasible
  if (MSPIdx == NumMSPs) {
    return FormatInterface.isFormatAvailable(CurrentRealSlots);
  }

  // Get the composition for this MSP
  unsigned MSPClassIdx = MSPIndices[MSPIdx];
  SlotBits Composition = SlotStructure.getMSPComposition(MSPClassIdx);
  uint8_t Count = MSPCounts[MSPIdx];

  // Find available slots from the composition
  SlotBits AvailableSlots = Composition & ~CurrentRealSlots;

  // Check if we have enough available slots for this MSP's count
  if (llvm::popcount(AvailableSlots) < Count) {
    // Not enough free slots in the composition - cannot materialize
    return false;
  }

  // For simplicity, greedily assign the MSP instances to available slots
  // A more sophisticated approach would try all combinations
  SlotBits NewRealSlots = CurrentRealSlots;
  unsigned Assigned = 0;
  for (unsigned Bit = 0; Bit < 64 && Assigned < Count; ++Bit) {
    if (AvailableSlots & (1ULL << Bit)) {
      NewRealSlots |= (1ULL << Bit);
      ++Assigned;
    }
  }

  // Recursively try to materialize remaining MSPs
  return tryMaterializeMSPs(SlotStructure, FormatInterface, NewRealSlots,
                            MSPIndices, MSPCounts, NumMSPs, MSPIdx + 1);
}

void SlotOccupancy::dump() const {
  dbgs() << "SlotOccupancy: [";
  bool First = true;
  for (unsigned I = 0; I < MaxSlotClasses; ++I) {
    if (Counts[I] > 0) {
      if (!First)
        dbgs() << ", ";
      dbgs() << I << ":" << static_cast<unsigned>(Counts[I]);
      First = false;
    }
  }
  dbgs() << "]\n";
}

bool SlotOccupancy::operator==(const SlotOccupancy &Other) const {
  return Counts == Other.Counts;
}

bool SlotOccupancy::boundedBy(const AIESlotStructure &SlotStructure) const {
  for (unsigned I = 0; I < MaxSlotClasses; ++I) {
    if (Counts[I] > SlotStructure.getCapacity(I))
      return false;
  }
  return true;
}

unsigned SlotOccupancy::total() const {
  unsigned Total = 0;
  for (unsigned I = 0; I < MaxSlotClasses; ++I) {
    Total += Counts[I];
  }
  return Total;
}
