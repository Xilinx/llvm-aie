//===- StaticBitSetTest.cpp -------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "gtest/gtest.h"

#include "StaticBitSet.h"

// I like primes.
static const int Size = 137;
using S = StaticBitSet<Size, uint32_t>;

namespace {
bool checkEmpty(S X) {
  for (int I = 0; I < Size; I++) {
    S BitI(I);
    if (X.overlap(BitI)) {
      return false;
    }
  }
  return X.empty();
}

bool checkTautologies(S X) {
  S Empty;
  S Block = ~S();

  // Any non-empty set overlaps with itself
  if (!checkEmpty(X) && !X.overlap(X)) {
    return false;
  }
  // No set overlaps with the empty set
  if (X.overlap(Empty)) {
    return false;
  }

  // Any non-empty set overlaps with the universe
  if (!checkEmpty(X) && !X.overlap(Block)) {
    return false;
  }
  return true;
}
} // namespace

TEST(StaticBitSet, Construct) {
  S Empty;
  S EmptyTwo;

  EXPECT_TRUE(checkEmpty(Empty));
  EXPECT_FALSE(Empty.overlap(Empty));
  EXPECT_FALSE(Empty.overlap(EmptyTwo));

  S Bit136(136);
  S Bit100(100);
  EXPECT_FALSE(checkEmpty(Bit136));
  EXPECT_TRUE(Bit136.overlap(Bit136));
  EXPECT_FALSE(Bit136.overlap(Empty));
  EXPECT_FALSE(Bit136.overlap(Bit100));
  EXPECT_FALSE(Bit100.overlap(Bit136));

  EXPECT_TRUE(checkTautologies(Empty));
  EXPECT_TRUE(checkTautologies(Bit136));
  EXPECT_TRUE(checkTautologies(Bit100));
}

TEST(StaticBitSet, Ops) {
  S Bit1(1);
  S Bit2(2);
  S Bit3(3);

  EXPECT_TRUE(checkTautologies(Bit1));
  EXPECT_TRUE(checkTautologies(Bit2));
  EXPECT_TRUE(checkTautologies(Bit3));

  EXPECT_FALSE(Bit1.overlap(Bit3));

  S Bit1or3(Bit1 | Bit3);
  EXPECT_TRUE(checkTautologies(Bit1or3));

  // We shouldn't have changed anything in the operands.
  EXPECT_FALSE(Bit1.overlap(Bit3));

  EXPECT_TRUE(Bit1or3.overlap(Bit1));
  EXPECT_TRUE(Bit1or3.overlap(Bit3));

  S Bit1or2(Bit1 | Bit2);
  EXPECT_TRUE(checkTautologies(Bit1or2));

  S Intersect = Bit1or3 & Bit1or2;
  EXPECT_TRUE(checkTautologies(Intersect));
  EXPECT_TRUE(Intersect.overlap(Bit1));
  EXPECT_FALSE(Intersect.overlap(Bit2));
  EXPECT_FALSE(Intersect.overlap(Bit3));

  S Block = ~S();
  EXPECT_TRUE(checkTautologies(Block));
  S Empty;
  EXPECT_TRUE(Block.overlap(Bit1));
  EXPECT_TRUE(Block.overlap(Bit2));
  EXPECT_TRUE(Block.overlap(Bit3));
  EXPECT_TRUE(Block.overlap(Bit1or2));
  EXPECT_TRUE(Block.overlap(Bit1or3));
  EXPECT_FALSE(Block.overlap(Empty));

  Bit1or2 &= Bit2;
  Bit1or2 &= Bit1;
  EXPECT_TRUE(Bit1or2.empty());
  Bit1or2 |= Bit1;
  EXPECT_FALSE(Bit1or2.empty());
  EXPECT_TRUE(Bit1or2.overlap(Bit1));
  EXPECT_FALSE(Bit1or2.overlap(Bit2));
  Bit1or2 |= Bit2;

  EXPECT_FALSE(Bit1or2.empty());
  EXPECT_TRUE(Bit1or2.overlap(Bit1));
  EXPECT_TRUE(Bit1or2.overlap(Bit2));
}
