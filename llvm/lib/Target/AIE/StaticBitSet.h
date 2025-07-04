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

#include <bitset>

template <const int NumBits> class StaticBitSet : public std::bitset<NumBits> {
public:
  constexpr int getNumBits() const { return NumBits; }
  StaticBitSet() : std::bitset<NumBits>() {}
  // This takes the index of a resource and constructs a set containing that
  // element.
  explicit StaticBitSet(int BitNo) : std::bitset<NumBits>() {
    this->set(BitNo, true);
  }

  StaticBitSet operator|(const StaticBitSet &Other) {
    StaticBitSet Result(*this);
    Result |= Other;
    return Result;
  }

  StaticBitSet operator&(const StaticBitSet &Other) const {
    StaticBitSet Result(*this);
    Result &= Other;
    return Result;
  }

  StaticBitSet operator~() const {
    StaticBitSet Result(*this);
    Result.flip();
    return Result;
  }

  void clear() { this->reset(); }
  bool contains(int BitNo) const { return this->test(BitNo); }
  bool overlap(const StaticBitSet &Other) const {
    return (*this & Other).any();
  }
  bool empty() const { return this->none(); }
};

#endif // LLVM_LIB_TARGET_AIE_STATICBITSET_H
