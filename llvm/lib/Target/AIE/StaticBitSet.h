//===--- StaticBitSet.h - Fully static bitset. ----------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This is an interface class on top of std::bitset, with names adjusted to
// AIEHazardrecognizer parlance and some constructors hidden to have a more
// fitting interface.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_STATICBITSET_H
#define LLVM_LIB_TARGET_AIE_STATICBITSET_H

#include <bitset>
#include <cstddef>
#include <iterator>

// Elem is the type of the elements of the set, as yielded by iteration. It must
// be explicitly constructible from unsigned and implicitly convertible to
// unsigned, which is how it indexes the set.
template <const int NumBits, typename Elem = unsigned>
class StaticBitSet : public std::bitset<NumBits> {
public:
  // Forward iterator over the indices of the elements in the set.
  class const_iterator {
    const StaticBitSet *Set;
    std::size_t Pos;

    void skipToElement() {
      while (Pos < static_cast<std::size_t>(NumBits) && !Set->test(Pos))
        ++Pos;
    }

  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Elem;
    using difference_type = std::ptrdiff_t;
    using pointer = const Elem *;
    using reference = Elem;

    const_iterator(const StaticBitSet *Set, std::size_t Pos)
        : Set(Set), Pos(Pos) {
      skipToElement();
    }
    unsigned operator*() const { return static_cast<unsigned>(Pos); }
    const_iterator &operator++() {
      ++Pos;
      skipToElement();
      return *this;
    }
    const_iterator operator++(int) {
      const_iterator Previous = *this;
      ++*this;
      return Previous;
    }
    bool operator==(const const_iterator &Other) const {
      return Pos == Other.Pos;
    }
    bool operator!=(const const_iterator &Other) const {
      return Pos != Other.Pos;
    }
  };

  const_iterator begin() const { return const_iterator(this, 0); }
  const_iterator end() const { return const_iterator(this, NumBits); }

  constexpr int getNumBits() const { return NumBits; }
  StaticBitSet() : std::bitset<NumBits>() {}
  // This takes the index of a resource and constructs a set containing that
  // element.
  explicit StaticBitSet(int BitNo) : std::bitset<NumBits>() {
    this->set(BitNo, true);
  }

  StaticBitSet operator|(const StaticBitSet &Other) const {
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

  // Set difference: the elements of this set that are not in Other.
  StaticBitSet operator-(const StaticBitSet &Other) const {
    StaticBitSet Result(*this);
    Result &= ~Other;
    return Result;
  }

  StaticBitSet &operator-=(const StaticBitSet &Other) {
    *this &= ~Other;
    return *this;
  }

  void clear() { this->reset(); }
  bool contains(int BitNo) const { return this->test(BitNo); }
  bool overlap(const StaticBitSet &Other) const {
    return (*this & Other).any();
  }
  bool empty() const { return this->none(); }
  // Return true if every element of Other is also in this set.
  bool covers(const StaticBitSet &Other) const {
    return (Other - *this).none();
  }
};

#endif // LLVM_LIB_TARGET_AIE_STATICBITSET_H
