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

  /// Construct with a single class occupation (for MSPs)
  SlotOccupancy(unsigned ClassIdx, uint8_t Count);

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

  /// Check if all counts are bounded by their respective capacities
  /// \param SlotStructure Interface for querying capacity limits
  /// \return true if all counts are within valid range
  bool boundedBy(const AIESlotStructure &SlotStructure) const;

  /// Get the total of all occupation counts
  /// \return Sum of all counts
  unsigned total() const;

private:
  /// Occupation counts per slot class
  /// For regular slots: 0 or 1
  /// For MSP classes: 0 to capacity
  std::array<uint8_t, MaxSlotClasses> Counts;

  /// Helper to recursively try materializing MSPs to real slots
  /// \param SlotStructure Interface for querying slot compositions
  /// \param FormatInterface Interface for checking format feasibility
  /// \param CurrentRealSlots Current real slot occupancy
  /// \param MSPIndices Array of MSP class indices to materialize
  /// \param MSPCounts Array of counts for each MSP
  /// \param NumMSPs Number of MSPs to materialize
  /// \param MSPIdx Current MSP being materialized
  /// \return true if a feasible materialization exists
  bool
  tryMaterializeMSPs(const AIESlotStructure &SlotStructure,
                     const AIEBaseMCFormats &FormatInterface,
                     SlotBits CurrentRealSlots,
                     const std::array<unsigned, MaxSlotClasses> &MSPIndices,
                     const std::array<uint8_t, MaxSlotClasses> &MSPCounts,
                     unsigned NumMSPs, unsigned MSPIdx) const;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIESLOTOCCUPANCY_H
