//===--- AIESlotStructure.h - Slot Structure Query Interface ----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines an interface for querying slot structure information
// needed by the SlotOccupancy conflict detection system.
//
// The interface provides generator-supplied data:
// - Slot definitions (real slots come first, then MSP classes)
// - MSP compositions (which real slots each MSP can use)
// - Feasible formats (valid combinations of real slots)
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIESLOTSTRUCTURE_H
#define LLVM_LIB_TARGET_AIE_AIESLOTSTRUCTURE_H

#include "MCTargetDesc/AIEFormat.h"
#include <array>
#include <cstdint>

namespace llvm {

// MaxSlotClasses is defined in AIESlotOccupancy.h
// We use a forward declaration here to avoid circular dependencies
constexpr unsigned MaxSlotClasses = 32;

/// Interface for querying slot structure information.
/// This provides generator-supplied data about slot classes, MSP compositions,
/// and feasible formats. The feasibility logic itself is implemented in
/// SlotOccupancy::conflict().
///
/// Complete unification: Real slots are MSPs with composition = themselves.
/// For example, slot A (index 0) has composition = (1 << 0).
/// Capacity is computed as popcount(composition).
class AIESlotStructure {
public:
  virtual ~AIESlotStructure() = default;

  /// Get the number of real slots (non-MSP slot classes)
  /// Real slots always come first in the slot class indexing
  virtual unsigned getNumRealSlots() const = 0;

  /// Get the composition for any slot class (real or MSP)
  /// For real slots: returns (1 << ClassIdx)
  /// For MSP classes: returns bitmask of real slots it can use
  /// \param ClassIdx The slot class index
  /// \return Bitmask of real slots this class can occupy
  virtual SlotBits getMSPComposition(unsigned ClassIdx) const = 0;

  /// Get the capacity (max occupancy) for a slot class
  /// Computed as popcount(getMSPComposition(ClassIdx))
  /// \param ClassIdx The slot class index
  /// \return The maximum occupation count for this class
  uint8_t getCapacity(unsigned ClassIdx) const {
    return llvm::popcount(getMSPComposition(ClassIdx));
  }
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIESLOTSTRUCTURE_H
