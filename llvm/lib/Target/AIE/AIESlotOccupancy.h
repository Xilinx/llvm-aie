//===--- AIESlotOccupancy.h - Generalized Slot Occupancy Model ---*- C++
//-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines a generalized slot occupancy model that unifies regular
// slots and multi-slot pseudos (MSPs) using occupation counts.
//
// Key concepts:
// - Regular slots are treated as slot classes with capacity 1
// - MSPs are slot classes with capacity > 1
// - Conflict detection uses precomputed capacity bounds and format feasibility
// - No local materialization decisions during hazard checking
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIESLOTOCCUPANCY_H
#define LLVM_LIB_TARGET_AIE_AIESLOTOCCUPANCY_H

#include "AIEBaseSubtarget.h"
#include "AIESlotStructure.h"
#include "llvm/Support/raw_ostream.h"
#include <array>
#include <cstdint>

namespace llvm {

// Forward declarations
class AIEBaseMCFormats;

/// SlotOccupancy represents the occupancy state of slots in a cycle.
/// It unifies regular slots (capacity 1) and multi-slot pseudos (capacity > 1)
/// using occupation counts per slot class.
class SlotOccupancy {
public:
  SlotOccupancy() : Counts{} {}

  /// Construct with concrete slot bits (for regular instructions)
  explicit SlotOccupancy(SlotBits Slots);

  /// Check if this occupancy is empty
  bool isEmpty() const;

  /// Clear all occupancy
  void clear();

  /// Block all resources (for reserved cycles)
  void blockResources();

  /// Merge another occupancy into this one
  SlotOccupancy &operator|=(const SlotOccupancy &Other);

  /// Combine two occupancies (by-value)
  SlotOccupancy operator|(const SlotOccupancy &Other) const;

  /// Check if adding Other would create a conflict
  /// Uses three-layer checking:
  /// 1. Quick prune: All counts within capacity bounds
  /// 2. Quick prune: Total occupancy within real slot count
  /// 3. Materialization check: MSPs can be placed on real slots in feasible
  /// format
  /// \param Other The occupancy to check for conflict
  /// \param FormatInterface Interface providing slot structure and format
  /// feasibility
  bool conflict(const SlotOccupancy &Other,
                const AIEBaseMCFormats &FormatInterface) const;

  /// Dump a human-readable representation
  void dump() const;

  /// Equality comparison (for testing/debugging)
  bool operator==(const SlotOccupancy &Other) const;

  /// Check if all counts are bounded by the given bounds
  /// \param Bounds SlotOccupancy representing the upper bounds for each slot
  /// class
  /// \return true if all counts are within the bounds
  bool boundedBy(const SlotOccupancy &Bounds) const;

  /// Get the total of all occupation counts
  /// \return Sum of all counts
  unsigned total() const;

  /// Get the occupation count for a specific slot class (STL-style)
  /// \param ClassIdx The slot class index to query
  /// \return The occupation count for that class
  uint8_t at(unsigned ClassIdx) const { return Counts.at(ClassIdx); }

  /// Set the occupation count for a specific slot class
  /// \param ClassIdx The slot class index to set
  /// \param Count The new count value
  void setCount(unsigned ClassIdx, uint8_t Count) {
    assert(ClassIdx < MaxSlotClasses && "Class index out of bounds");
    Counts[ClassIdx] = Count;
  }

private:
  /// Occupation counts per slot class
  /// For regular slots: 0 or 1
  /// For MSP classes: 0 to capacity
  std::array<uint8_t, MaxSlotClasses> Counts;

  /// Helper to recursively try materializing MSPs to real slots
  /// \param FormatInterface Interface for checking format feasibility
  /// \param RemainingOccupancy Occupancy still to be materialized
  /// \param CurrentRealSlots Current real slot occupancy (bit mask)
  /// \return true if a feasible materialization exists
  bool tryMaterializeMSPs(const AIEBaseMCFormats &FormatInterface,
                          const SlotOccupancy &RemainingOccupancy,
                          SlotBits CurrentRealSlots) const;
};

/// MSPSlotMapping stores the materialization of MSP instances to real slots.
/// This allows iterative querying to determine which real slot each MSP
/// instance should use. The class maintains internal counters to track which
/// instance of each MSP class is being queried, and updates a SlotOccupancy
/// to reflect materializations as they are retrieved.
class MSPSlotMapping {
  /// Structure to hold assignment information for a single MSP instance
  struct MSPAssignment {
    unsigned MSPClassIdx; // Which MSP class
    unsigned InstanceIdx; // Which instance of that class (0-based)
    unsigned RealSlotIdx; // Which real slot it's assigned to
  };

  /// List of all MSP assignments
  /// Ordered by MSP class index, then instance index
  std::vector<MSPAssignment> Assignments;

  /// Current occupancy state reflecting materializations retrieved so far
  SlotOccupancy CurrentOccupancy;

  /// Counters tracking how many instances of each MSP class have been queried
  std::array<uint8_t, MaxSlotClasses> InstanceCounters;

  /// Number of real slots (cached from SlotStructure)
  unsigned NumRealSlots;

  /// Reference to format interface (needed for materializeAlternative)
  const AIEBaseMCFormats *FormatInterface;

  /// Compute a legal materialization for the given occupancy
  /// \param Occupancy The slot occupancy to materialize
  /// \param FormatInterface Interface for checking format feasibility and
  ///        providing slot structure
  /// \return true if a legal materialization was found
  bool computeMapping(const SlotOccupancy &Occupancy,
                      const AIEBaseMCFormats &FormatInterface);

  /// Helper to recursively compute MSP materializations
  /// \param FormatInterface Interface for checking format feasibility
  /// \param RemainingOccupancy Occupancy still to be materialized
  /// \param CurrentRealSlots Current real slot occupancy (bit mask)
  /// \param Assignments Output vector to store assignments
  /// \return true if a feasible materialization exists
  static bool
  tryMaterializeMSPsWithMapping(const AIEBaseMCFormats &FormatInterface,
                                const SlotOccupancy &RemainingOccupancy,
                                SlotBits CurrentRealSlots,
                                std::vector<MSPAssignment> &Assignments);

public:
  /// Default constructor creates an empty mapping
  MSPSlotMapping();

  /// Construct and compute mapping for the given occupancy
  /// \param Occupancy The slot occupancy to materialize
  /// \param FormatInterface Interface for checking format feasibility and
  ///        providing slot structure
  MSPSlotMapping(const SlotOccupancy &Occupancy,
                 const AIEBaseMCFormats &FormatInterface);

  /// Materialize the next instance of a slot class (MSP or real slot) to a
  /// real slot. This decrements the count for the slot class and increments
  /// the count for the materialized real slot in the current occupancy.
  /// \param SlotClassIdx The slot class index (can be MSP or real slot)
  /// \return The real slot index this instance materializes to
  /// \pre The slot class must have at least one unmaterialized instance
  unsigned materializeAlternative(unsigned SlotClassIdx);

  /// Get the current occupancy state reflecting all materializations retrieved
  /// so far
  /// \return The current occupancy with MSP instances materialized to real
  /// slots
  const SlotOccupancy &getCurrentOccupancy() const { return CurrentOccupancy; }

  /// Check if the mapping is empty (no MSPs materialized)
  bool isEmpty() const { return Assignments.empty(); }

  /// Clear all mappings and reset state
  void clear();

  /// Dump a human-readable representation
  void dump() const;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIESLOTOCCUPANCY_H
