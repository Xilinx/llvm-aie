//===- AIESlotCounts.cpp - SlotCount utility ------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
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

int SlotCounts::max() {
  int Max = 0;
  for (int I = 0; I < Size; I++) {
    Max = std::max(Max, int(Counts[I]));
  }
  return Max;
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

SlotCounts SlotCounts::operator+(const SlotCounts &Other) const {
  SlotCounts Result(*this);
  return Result += Other;
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
