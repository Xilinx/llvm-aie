//===--- StaticBitSet.h - Fully static bitset. ----------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This is an interface class on top of std::bitset, with names adjusted to
// AIEHazardrecognizer parlance and some constructors hidden to have a more
// fitting interface.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_STATICBITSET_H
#define LLVM_LIB_TARGET_AIE_STATICBITSET_H

#include <cassert>
#include <climits>

template <const int NumBits, typename Container> class StaticBitSet {
protected:
  static constexpr int twoLog(int N) { return N == 1 ? 0 : twoLog(N / 2) + 1; }
  static constexpr const int BitsPerContainer = sizeof(Container) * CHAR_BIT;
  static constexpr const int Shift = twoLog(BitsPerContainer);
  static constexpr const int Mask = ~(~0u << Shift);
  static constexpr const Container One = 1;
  static constexpr const int NumContainers =
      (NumBits + BitsPerContainer - 1) / BitsPerContainer;
  Container Bits[NumContainers];

public:
  int getNumBits() const { return NumBits; }
  StaticBitSet() { clear(); }
  // This takes the index of a resource and constructs a set containing that
  // element.
  explicit StaticBitSet(int BitNo) : StaticBitSet() {
    int BlockIdx = BitNo >> Shift;
    assert(BlockIdx < NumContainers && "BitNo out of range defined by NumBits");
    Bits[BlockIdx] |= (One << (BitNo & Mask));
  }

  StaticBitSet &operator|=(const StaticBitSet &Other) {
    for (int I = 0; I < NumContainers; I++) {
      Bits[I] |= Other.Bits[I];
    }
    return *this;
  }

  StaticBitSet operator|(const StaticBitSet &Other) {
    StaticBitSet Result(*this);
    Result |= Other;
    return Result;
  }

  StaticBitSet &operator&=(const StaticBitSet &Other) {
    for (int I = 0; I < NumContainers; I++) {
      Bits[I] &= Other.Bits[I];
    }
    return *this;
  }
  StaticBitSet operator&(const StaticBitSet &Other) const {
    StaticBitSet Result(*this);
    Result &= Other;
    return Result;
  }

  StaticBitSet operator~() const {
    StaticBitSet Result;
    for (int I = 0; I < NumContainers; I++) {
      Result.Bits[I] = ~Bits[I];
    }
    return Result;
  }

  bool operator==(const StaticBitSet &Other) const {
    for (int I = 0; I < NumContainers; I++) {
      if (Bits[I] != Other.Bits[I]) {
        return false;
      }
    }
    return true;
  }

  void clear() {
    for (auto &Block : Bits) {
      Block = 0;
    }
  }

  bool contains(int BitNo) const {
    if (BitNo >= NumBits) {
      return false;
    }
    const Container &Block = Bits[BitNo >> Shift];
    return (Block & (One << (BitNo & Mask))) != 0;
  }

  bool overlap(const StaticBitSet &Other) const {
    for (int I = 0; I < NumContainers; I++) {
      if (Bits[I] & Other.Bits[I]) {
        return true;
      }
    }
    return false;
  }

  bool empty() const {
    for (int I = 0; I < NumContainers; I++) {
      if (Bits[I]) {
        return false;
      }
    }
    return true;
  }
};

#endif // LLVM_LIB_TARGET_AIE_STATICBITSET_H
