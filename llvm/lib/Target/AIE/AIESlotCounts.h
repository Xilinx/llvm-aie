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
  int max();

  // Add slot counts of Other to this
  SlotCounts &operator+=(const SlotCounts &Other);

  // By-value addition.
  SlotCounts operator+(const SlotCounts &Other) const;

  // Indexing
  const int &operator[](int I) const { return Counts[I]; };

  int size() const { return Size; }
};
} // namespace AIE

raw_ostream &operator<<(raw_ostream &OS, const AIE::SlotCounts &Val);

} // namespace llvm
#endif // LLVM_LIB_TARGET_AIE_AIESLOTCOUNTS_H
