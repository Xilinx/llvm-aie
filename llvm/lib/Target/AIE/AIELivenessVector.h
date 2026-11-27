//===- AIELivenessVector.h - Liveness vector container ---------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines a vector-like container for liveness information that
// provides safe out-of-range access and common operations.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIELIVENESSVECTOR_H
#define LLVM_LIB_TARGET_AIE_AIELIVENESSVECTOR_H

#include "AIEMaxNumRegUnits.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"

namespace llvm {

class raw_ostream;

namespace AIE {

/// Liveness information for a single cycle/offset.
/// Tracks both occupied register units and bypass usage to detect conflicts.
class Liveness {
  // RegUnits occupied at this cycle, expressed relative to the admissible
  // registers of the live range.
  PartSet Units;
  // Set of bypass classes being read from at this cycle.
  SmallVector<unsigned, 2> BypassReads;
  // Set of bypass classes being written to at this cycle.
  SmallVector<unsigned, 2> BypassWrites;

public:
  /// Construct with no RegUnits occupied.
  Liveness() = default;

  /// Get the occupied RegUnits.
  const PartSet &getUnits() const { return Units; }

  /// Mark the RegUnits \p U as occupied.
  void addUnits(const PartSet &U);

  /// Add a bypass read for a specific forwarding class.
  void addBypassRead(unsigned ForwardingClass);

  /// Add a bypass write for a specific forwarding class.
  void addBypassWrite(unsigned ForwardingClass);

  /// Get bypass reads.
  ArrayRef<unsigned> getBypassReads() const { return BypassReads; }

  /// Get bypass writes.
  ArrayRef<unsigned> getBypassWrites() const { return BypassWrites; }

  /// Check if this liveness conflicts with another.
  /// Conflicts occur when:
  /// 1. Occupied register units overlap, OR
  /// 2. A bypass read and bypass write use the same forwarding class, OR
  /// 3. One has bypass activity and the other has occupied register units
  ///    (they share the same register address)
  bool conflictsWith(const Liveness &Other) const;

  /// Union with another liveness.
  Liveness &operator|=(const Liveness &Other);

  /// Intersection with another liveness.
  Liveness &operator&=(const Liveness &Other);

  /// Difference with another liveness.
  Liveness &operator-=(const Liveness &Other);

  /// Check if any RegUnits are occupied or any bypasses are active.
  bool any() const;

  /// Check if no RegUnits are occupied and no bypasses are active.
  bool none() const;
};

/// A vector-like container for liveness information that provides safe
/// out-of-range access and common operations.
class LivenessVector {
  SmallVector<Liveness, 8> Elements;

public:
  /// Construct with given size, all elements initialized to no liveness.
  explicit LivenessVector(size_t Size = 0);

  /// Get the size of the vector.
  size_t size() const;

  /// Check if empty.
  bool empty() const;

  /// Access element with bounds checking in debug mode.
  Liveness &operator[](size_t Index);
  const Liveness &operator[](size_t Index) const;

  /// Safe access - returns empty liveness if out of range.
  Liveness at(size_t Index) const;

  /// Get the underlying elements.
  const SmallVector<Liveness, 8> &getElements() const;

  /// Union with another vector.
  LivenessVector &operator|=(const LivenessVector &Other);

  /// Intersection with another vector.
  LivenessVector &operator&=(const LivenessVector &Other);

  /// Difference with another vector (this & ~Other).
  LivenessVector &operator-=(const LivenessVector &Other);

  /// Create union with another vector.
  LivenessVector operator|(const LivenessVector &Other) const;

  /// Create intersection with another vector.
  LivenessVector operator&(const LivenessVector &Other) const;

  /// Create difference with another vector.
  LivenessVector operator-(const LivenessVector &Other) const;

  /// True when any same-index pair conflicts via Liveness::conflictsWith
  /// (unit intersection, bypass R/W same class, or bypass vs RF units).
  bool overlaps(const LivenessVector &Other) const;

  /// Check if any element has liveness.
  bool any() const;

  /// Check if no elements have liveness.
  bool none() const;

  /// Debug dump.
  void dump() const;

  /// Print to stream.
  void print(raw_ostream &OS) const;
};

} // namespace AIE
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIELIVENESSVECTOR_H
