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
  const SlotOccupancy Combined = *this | Other;

  // Quick pruning 1: Check if all counts are within valid range
  if (!Combined.boundedBy(SlotStructure))
    return true;

  const unsigned NumRealSlots = SlotStructure.getNumRealSlots();

  // Quick pruning 2: Sum of occupancies must not exceed number of real slots
  if (Combined.total() > NumRealSlots)
    return true;

  // Start with real slot occupancies (these are fixed)
  SlotBits RealSlotOccupancy = 0;
  for (unsigned I = 0; I < NumRealSlots; ++I) {
    if (Combined.Counts[I] > 0) {
      RealSlotOccupancy |= (SlotBits(1) << I);
    }
  }

  // Try to materialize MSPs to real slots
  return !tryMaterializeMSPs(FormatInterface, Combined, RealSlotOccupancy);
}

namespace {

/// Helper to assign MSP instances to available slots
/// \tparam AssignmentHandler Functor called for each assignment: (MSPClassIdx,
/// InstanceIdx, Bit)
/// \param AvailableSlots Bitmask of slots available for this MSP
/// \param Count Number of instances to assign
/// \param MSPClassIdx The MSP class being assigned
/// \param NewRealSlots Output: updated real slot occupancy
/// \param Handler Functor to record assignments (optional)
/// \return true if all instances were successfully assigned
template <typename AssignmentHandler>
bool assignMSPInstances(SlotBits AvailableSlots, uint8_t Count,
                        unsigned MSPClassIdx, SlotBits &NewRealSlots,
                        AssignmentHandler &&Handler) {
  SlotBits Available = AvailableSlots;
  SlotBits BitMask = 1;
  unsigned Bit = 0;

  for (unsigned Assigned = 0; Assigned < Count; ++Assigned) {
    if (Available == 0) {
      // Not enough free slots in the composition - cannot materialize
      return false;
    }

    // Find next available slot
    while (!(Available & BitMask)) {
      BitMask <<= 1;
      ++Bit;
    }

    NewRealSlots |= BitMask;
    Available &= ~BitMask; // Consume this slot

    // Record assignment if handler provided
    Handler(MSPClassIdx, Assigned, Bit);

    BitMask <<= 1; // Advance to next bit for next iteration
    ++Bit;
  }

  return true;
}

} // anonymous namespace

bool SlotOccupancy::tryMaterializeMSPs(const AIEBaseMCFormats &FormatInterface,
                                       const SlotOccupancy &RemainingOccupancy,
                                       SlotBits CurrentRealSlots) const {
  const AIESlotStructure &SlotStructure = FormatInterface.getSlotStructure();
  const unsigned NumRealSlots = SlotStructure.getNumRealSlots();

  // Find the first MSP class with non-zero count
  unsigned MSPClassIdx = MaxSlotClasses;
  for (unsigned I = NumRealSlots; I < MaxSlotClasses; ++I) {
    if (RemainingOccupancy.at(I) > 0) {
      MSPClassIdx = I;
      break;
    }
  }

  // Base case: no more MSPs to materialize, check if result is feasible
  if (MSPClassIdx == MaxSlotClasses) {
    return FormatInterface.isFormatAvailable(CurrentRealSlots);
  }

  // Get the composition for this MSP
  const SlotBits Composition = SlotStructure.getMSPComposition(MSPClassIdx);
  const uint8_t Count = RemainingOccupancy.at(MSPClassIdx);

  // Find available slots from the composition
  const SlotBits AvailableSlots = Composition & ~CurrentRealSlots;

  // Greedily assign the MSP instances to available slots
  SlotBits NewRealSlots = CurrentRealSlots;
  if (!assignMSPInstances(AvailableSlots, Count, MSPClassIdx, NewRealSlots,
                          [](unsigned, unsigned, unsigned) {})) {
    return false;
  }

  // Create new remaining occupancy with this MSP class zeroed out
  SlotOccupancy NewRemaining = RemainingOccupancy;
  NewRemaining.setCount(MSPClassIdx, 0);

  // Recursively try to materialize remaining MSPs
  return tryMaterializeMSPs(FormatInterface, NewRemaining, NewRealSlots);
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

//===----------------------------------------------------------------------===//
// MSPSlotMapping Implementation
//===----------------------------------------------------------------------===//

MSPSlotMapping::MSPSlotMapping()
    : InstanceCounters{}, NumRealSlots(0), FormatInterface(nullptr) {}

MSPSlotMapping::MSPSlotMapping(const SlotOccupancy &Occupancy,
                               const AIEBaseMCFormats &FI)
    : CurrentOccupancy(Occupancy), InstanceCounters{}, NumRealSlots(0),
      FormatInterface(&FI) {
  const bool Success = computeMapping(Occupancy, FI);
  (void)Success;
  assert(Success && "Failed to compute mapping for feasible occupancy");
}

bool MSPSlotMapping::computeMapping(const SlotOccupancy &Occupancy,
                                    const AIEBaseMCFormats &FI) {
  // Clear any existing assignments
  Assignments.clear();
  std::fill(InstanceCounters.begin(), InstanceCounters.end(), 0);
  FormatInterface = &FI;

  const AIESlotStructure &SlotStructure = FI.getSlotStructure();
  NumRealSlots = SlotStructure.getNumRealSlots();

  // Start with real slot occupancies (these are fixed)
  SlotBits RealSlotOccupancy = 0;
  for (unsigned I = 0; I < NumRealSlots; ++I) {
    if (Occupancy.at(I) > 0) {
      RealSlotOccupancy |= (SlotBits(1) << I);
    }
  }

  // Try to materialize MSPs to real slots
  return tryMaterializeMSPsWithMapping(FI, Occupancy, RealSlotOccupancy,
                                       Assignments);
}

bool MSPSlotMapping::tryMaterializeMSPsWithMapping(
    const AIEBaseMCFormats &FI, const SlotOccupancy &RemainingOccupancy,
    SlotBits CurrentRealSlots, std::vector<MSPAssignment> &Assignments) {
  const AIESlotStructure &SlotStructure = FI.getSlotStructure();
  const unsigned NumRealSlots = SlotStructure.getNumRealSlots();

  // Find the first MSP class with non-zero count
  unsigned MSPClassIdx = MaxSlotClasses;
  for (unsigned I = NumRealSlots; I < MaxSlotClasses; ++I) {
    if (RemainingOccupancy.at(I) > 0) {
      MSPClassIdx = I;
      break;
    }
  }

  // Base case: no more MSPs to materialize, check if result is feasible
  if (MSPClassIdx == MaxSlotClasses) {
    return FI.isFormatAvailable(CurrentRealSlots);
  }

  // Get the composition for this MSP
  const SlotBits Composition = SlotStructure.getMSPComposition(MSPClassIdx);
  const uint8_t Count = RemainingOccupancy.at(MSPClassIdx);

  // Find available slots from the composition
  const SlotBits AvailableSlots = Composition & ~CurrentRealSlots;

  // Greedily assign the MSP instances to available slots
  // Store the assignments for later retrieval
  SlotBits NewRealSlots = CurrentRealSlots;
  if (!assignMSPInstances(AvailableSlots, Count, MSPClassIdx, NewRealSlots,
                          [&Assignments](unsigned MSPClassIdx,
                                         unsigned InstanceIdx, unsigned Bit) {
                            Assignments.push_back(
                                {MSPClassIdx, InstanceIdx, Bit});
                          })) {
    return false;
  }

  // Create new remaining occupancy with this MSP class zeroed out
  SlotOccupancy NewRemaining = RemainingOccupancy;
  NewRemaining.setCount(MSPClassIdx, 0);

  // Recursively try to materialize remaining MSPs
  return tryMaterializeMSPsWithMapping(FI, NewRemaining, NewRealSlots,
                                       Assignments);
}

unsigned MSPSlotMapping::materializeAlternative(unsigned SlotClassIdx) {
  const AIESlotStructure &SlotStructure = FormatInterface->getSlotStructure();

  // For real slots, they materialize to themselves
  if (SlotClassIdx < NumRealSlots) {
    // Verify precondition: must have at least one instance
    assert(CurrentOccupancy.at(SlotClassIdx) > 0 &&
           "Cannot materialize slot class with zero count");

    // Real slots don't need transformation, just return the slot index
    // The occupancy already reflects the real slot
    return SlotClassIdx;
  }

  // For MSPs, find the assignment
  const uint8_t InstanceIdx = InstanceCounters[SlotClassIdx];

  // Verify precondition: must have at least one unmaterialized instance
  assert(CurrentOccupancy.at(SlotClassIdx) > InstanceIdx &&
         "Cannot materialize slot class with no remaining instances");

  // Find the assignment for this MSP class and instance
  for (const auto &Assignment : Assignments) {
    if (Assignment.MSPClassIdx == SlotClassIdx &&
        Assignment.InstanceIdx == InstanceIdx) {
      // Found the assignment - increment counter
      InstanceCounters[SlotClassIdx]++;

      // Update current occupancy: add the real slot
      CurrentOccupancy |= SlotOccupancy(SlotBits(1) << Assignment.RealSlotIdx);

      return Assignment.RealSlotIdx;
    }
  }

  // Should never reach here if precondition is satisfied
  llvm_unreachable("No assignment found for slot class instance");
}

void MSPSlotMapping::clear() {
  Assignments.clear();
  CurrentOccupancy.clear();
  std::fill(InstanceCounters.begin(), InstanceCounters.end(), 0);
}

void MSPSlotMapping::dump() const {
  dbgs() << "MSPSlotMapping:\n";
  if (Assignments.empty()) {
    dbgs() << "  (empty)\n";
    return;
  }

  for (const auto &Assignment : Assignments) {
    dbgs() << "  MSP[" << Assignment.MSPClassIdx << "]["
           << Assignment.InstanceIdx << "] -> Slot " << Assignment.RealSlotIdx
           << "\n";
  }

  dbgs() << "Current Occupancy: ";
  CurrentOccupancy.dump();

  dbgs() << "Instance Counters: [";
  bool First = true;
  for (unsigned I = 0; I < MaxSlotClasses; ++I) {
    if (InstanceCounters[I] > 0) {
      if (!First)
        dbgs() << ", ";
      dbgs() << I << ":" << static_cast<unsigned>(InstanceCounters[I]);
      First = false;
    }
  }
  dbgs() << "]\n";
}
