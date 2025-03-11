//===- AIESlotCounts.cpp - SlotCount utility ------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIESlotCounts.h"

namespace llvm {
namespace AIE {

SlotCounts::SlotCounts(SlotBits Bits) {
  while (Bits) {
    assert(Size < MaxSlots);
    Counts[Size] = Bits & 1;
    Size++;
    Bits >>= 1;
  }
}

SlotCounts::SlotCounts(const SlotCounts &Org) : Size(Org.Size) {
  for (int I = 0; I < Size; I++) {
    Counts[I] = Org.Counts[I];
  }
}

int &SlotCounts::operator[](int I) {
  // Grow to the required size, zero-filling new elements
  while (I >= Size) {
    Counts[Size++] = 0;
  }
  return Counts[I];
}

const int &SlotCounts::operator[](int I) const {
  assert(I < Size);
  return Counts[I];
}

int SlotCounts::max() const {
  int Max = 0;
  for (int I = 0; I < Size; I++) {
    Max = std::max(Max, Counts[I]);
  }
  return Max;
}

int SlotCounts::maxIndex() const {
  int MaxIdx = 0;
  for (int I = 1; I < Size; I++) {
    if (Counts[I] > Counts[MaxIdx]) {
      MaxIdx = I;
    }
  }
  return MaxIdx;
}

int SlotCounts::totals() const {
  int Totals = 0;
  for (int I = 0; I < Size; I++) {
    Totals += Counts[I];
  }
  return Totals;
}

int SlotCounts::distance(const SlotCounts &Other) const {
  int Sum = 0;
  const int N = std::max(Size, Other.Size);
  for (int I = 0; I < N; ++I) {
    Sum += abs(at(I) - Other.at(I));
  }
  return Sum;
}

SlotCounts &SlotCounts::operator+=(const SlotCounts &Other) {
  // The common part
  for (int I = 0; I < Size && I < Other.Size; I++) {
    Counts[I] += Other.Counts[I];
  }
  // Any excess from the other
  while (Size < Other.Size) {
    Counts[Size] = Other.Counts[Size];
    Size++;
  }
  assert(Size >= Other.Size);
  assert(Size < MaxSlots);
  return *this;
}

SlotCounts &SlotCounts::operator-=(const SlotCounts &Other) {
  // The common part
  for (int I = 0; I < Size && I < Other.Size; I++) {
    Counts[I] -= Other.Counts[I];
  }
  // Any excess from the other
  while (Size < Other.Size) {
    Counts[Size] = -Other.Counts[Size];
    Size++;
  }
  assert(Size >= Other.Size);
  assert(Size < MaxSlots);
  return *this;
}

SlotCounts SlotCounts::operator+(const SlotCounts &Other) const {
  SlotCounts Result(*this);
  return Result += Other;
}

SlotCounts SlotCounts::operator-(const SlotCounts &Other) const {
  SlotCounts Result(*this);
  return Result -= Other;
}

SlotCounts &SlotCounts::operator*=(int Scalar) {
  for (int I = 0; I < Size; I++) {
    Counts[I] *= Scalar;
  }
  return *this;
}

SlotCounts SlotCounts::operator*(int Scalar) const {
  SlotCounts Result(*this);
  return Result *= Scalar;
}

SlotCounts operator*(int Scalar, const SlotCounts &Slots) {
  return Slots * Scalar;
}

} // namespace AIE

raw_ostream &operator<<(raw_ostream &OS, const AIE::SlotCounts &Val) {
  OS << "{ ";
  const char *Sep = "";
  for (int I = 0; I < Val.size(); I++) {
    OS << Sep << Val[I];
    Sep = ", ";
  }
  OS << " }";
  return OS;
}

} // namespace llvm
