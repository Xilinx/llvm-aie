//===- AIESlotCounts.h - Resource computation utility ---------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This defines a class that can be used to tally up the slots required for
// one or more instructions
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIESLOTCOUNTS_H
#define LLVM_LIB_TARGET_AIE_AIESLOTCOUNTS_H

#include "MCTargetDesc/AIEFormat.h"

namespace llvm {
namespace AIE {

/// Efficient representation of slot requirements
class SlotCounts {
  static const int MaxSlots = 16;
  int Counts[MaxSlots];
  // The number of valid Counts. Further counts are assumed to be zero.
  int Size = 0;

public:
  // Useful constructors
  SlotCounts() = default;
  SlotCounts(SlotBits Bits);
  SlotCounts(const SlotCounts &Org);
  SlotCounts &operator=(const SlotCounts &Rhs) = default;

  // Compute the number of required cycles
  int max() const;

  // Compute the index the max element
  int maxIndex() const;

  // Compute the total of the counts
  int totals() const;

  // Add slot counts of Other to this
  SlotCounts &operator+=(const SlotCounts &Other);

  // By-value addition.
  SlotCounts operator+(const SlotCounts &Other) const;

  // Subtract slot counts of Other from this
  SlotCounts &operator-=(const SlotCounts &Other);

  // By-value subtraction.
  SlotCounts operator-(const SlotCounts &Other) const;

  // Multiply all elements with Scalar
  SlotCounts &operator*=(int Scalar);

  // By-value scalar multiplication
  SlotCounts operator*(int Scalar) const;

  // Indexing
  int &operator[](int I) {
    while (I >= Size) {
      Counts[Size++] = 0;
    }
    return Counts[I];
  };

  const int &operator[](int I) const {
    assert(I < Size);
    return Counts[I];
  };

  // Retrieve value
  int at(int I) const { return I >= Size ? 0 : Counts[I]; }

  int size() const { return Size; }
};

// Symmetric version of scalar multiplication

SlotCounts operator*(int Scalar, const SlotCounts &Slots);

} // namespace AIE

raw_ostream &operator<<(raw_ostream &OS, const AIE::SlotCounts &Val);

} // namespace llvm
#endif // LLVM_LIB_TARGET_AIE_AIESLOTCOUNTS_H
