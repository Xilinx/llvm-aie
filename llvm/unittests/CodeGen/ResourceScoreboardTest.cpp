//===- ResourceScoreboardTest.cpp -----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This tests the ResourceScoreboard template, which represents a sliding
// window of cycles around a moving reference point. It is organized as a
// circular buffer in order to minimize data movements. This is not visible in
// the external API, but defines some corner cases we test.
//===----------------------------------------------------------------------===//
#include "gtest/gtest.h"

#include "llvm/CodeGen/ResourceScoreboard.h"

namespace {

// Something basic that can accommodate some methods.
class RC {
  int Set = 0;

public:
  RC() = default;
  RC(int Value) : Set(Value) {}
  void clearResources() { Set = 0; }
  bool empty() const { return Set == 0; }
  bool conflict(RC &Other) const { return (Set & Other.Set) != 0; }
  bool operator==(RC Other) const { return Set == Other.Set; }
};

using SB = llvm::ResourceScoreboard<RC>;

// Compare two modulo scoreboards slot-by-slot over the full period.
// Both scoreboards must have the same period (getSize()). Every slot is
// compared so that tests also verify that unintended slots were not modified.
void compareModulo(const SB &Actual, const SB &Expected) {
  const int Period = Actual.getSize();
  ASSERT_EQ(Period, Expected.getSize());
  for (int I = 0; I < Period; I++) {
    EXPECT_EQ(Actual[I], Expected[I]) << "slot " << I << " differs";
  }
}

} // namespace

TEST(ResourceScoreboard, Construct) {
  SB Empty;
  Empty.config(-7, 4);
  for (int I = -7; I <= 4; I++) {
    EXPECT_TRUE(Empty[I].empty());
  }
  // Size should grow to the next power of two.
  EXPECT_EQ(Empty.getSize(), 16);

  // We should be able to reconfigure
  Empty.config(-5, 1);
  EXPECT_EQ(Empty.getSize(), 8);
}

TEST(ResourceScoreboard, Populate) {
  SB Scoreboard;
  Scoreboard.config(-7, 4);
  for (int I = -7; I <= 4; I++) {
    Scoreboard[I] = RC(I);
  }
  // Check that none was overwritten
  for (int I = -7; I <= 4; I++) {
    EXPECT_TRUE(Scoreboard[I] == RC(I));
  }
}

TEST(ResourceScoreboard, Advance) {
  SB Scoreboard;

  // We want the boundary case where we have no slack in the power of two
  Scoreboard.config(-8, 7);

  // Avoid 0, since that is checked as a special value
  for (int I = -8; I <= 7; I++) {
    Scoreboard[I] = RC(I + 100);
  }

  Scoreboard.advance();

  // Check that everything shifted as expected
  for (int I = -8; I <= 6; I++) {
    EXPECT_TRUE(Scoreboard[I] == RC(I + 101));
  }
  // With empty landing in the slot that scrolled into view.
  EXPECT_TRUE(Scoreboard[7] == RC(0));
}

TEST(ResourceScoreboard, Recede) {
  SB Scoreboard;

  // We want the boundary case where we have no slack in the power of two
  Scoreboard.config(-8, 7);

  // Avoid 0, since that is checked as a special value
  for (int I = -8; I <= 7; I++) {
    Scoreboard[I] = RC(I + 100);
  }

  Scoreboard.recede();

  // Check that everything shifted as expected
  for (int I = -7; I <= 7; I++) {
    EXPECT_TRUE(Scoreboard[I] == RC(I + 99));
  }
  // With empty landing in the slot that scrolled into view.
  EXPECT_TRUE(Scoreboard[-8] == RC(0));
}

TEST(ResourceScoreboard, Clear) {
  SB Scoreboard;

  Scoreboard.config(-5, 13);

  // Avoid 0, since that is checked as a special value
  for (int I = -5; I <= 13; I++) {
    Scoreboard[I] = RC(I + 100);
  }

  Scoreboard.clear();

  // Check that everything gets cleared
  for (int I = -5; I <= 13; I++) {
    EXPECT_TRUE(Scoreboard[I] == RC(0));
  }
}

TEST(ResourceScoreboard, SlidingWindow) {
  SB Scoreboard;

  Scoreboard.config(0, 11);

  // Some more elaborate insertions
  for (int I = 0; I <= 10; I += 2) {
    Scoreboard.advance();
    Scoreboard.advance();
    Scoreboard[10] = RC(I);
    Scoreboard[11] = RC(I + 1);
  }

  // Check the range
  for (int I = 0; I <= 11; I++) {
    EXPECT_EQ(Scoreboard[I], I);
  }

  for (int K = 12; K < 32; K++) {
    Scoreboard.advance();
    Scoreboard[11] = RC(K);
  }
  // Check the range
  for (int I = 0; I <= 11; I++) {
    EXPECT_EQ(Scoreboard[11 - I], 31 - I);
  }
}

// Modulo mode: configModulo(P) does not round size up to a power of two.
// All slots must start empty.
TEST(ResourceScoreboard, ModuloConstruct) {
  SB Scoreboard;
  Scoreboard.configModulo(7);
  EXPECT_EQ(Scoreboard.getSize(), 7);

  SB Expected;
  Expected.configModulo(7);
  // Expected is all-empty by construction.
  compareModulo(Scoreboard, Expected);
}

// Modulo mode: all slots in [0, P) can be independently written and read.
// No slot bleeds into its neighbours.
TEST(ResourceScoreboard, ModuloPopulate) {
  SB Scoreboard;
  Scoreboard.configModulo(6);
  for (int I = 0; I < 6; I++) {
    Scoreboard[I] = RC(I + 1);
  }

  SB Expected;
  Expected.configModulo(6);
  for (int I = 0; I < 6; I++) {
    Expected[I] = RC(I + 1);
  }
  compareModulo(Scoreboard, Expected);
}

// Modulo mode: cycle K and cycle K+P reach the same physical slot.
// Writing via cycle K+P must not disturb any other slot.
TEST(ResourceScoreboard, ModuloWrapping) {
  SB Scoreboard;
  Scoreboard.configModulo(5);

  // Write at cycle 2; verify via cycle 2+5=7 and check no other slot changed.
  Scoreboard[2] = RC(42);
  EXPECT_EQ(Scoreboard[7], RC(42));

  SB Expected;
  Expected.configModulo(5);
  Expected[2] = RC(42);
  compareModulo(Scoreboard, Expected);

  // Overwrite via cycle 7; only slot 2 must change.
  Scoreboard[7] = RC(99);
  Expected[2] = RC(99);
  compareModulo(Scoreboard, Expected);
}

// Modulo mode: negative indices wrap correctly.
// Cycle -1 aliases slot P-1; cycle -P aliases slot 0.
// No other slot must be affected.
TEST(ResourceScoreboard, ModuloNegativeIndices) {
  SB Scoreboard;
  Scoreboard.configModulo(4);

  Scoreboard[-1] = RC(10);
  SB Expected;
  Expected.configModulo(4);
  Expected[3] = RC(10);
  compareModulo(Scoreboard, Expected);

  Scoreboard[-4] = RC(20);
  Expected[0] = RC(20);
  compareModulo(Scoreboard, Expected);
}

// Modulo mode: clear() resets every slot to the empty state.
TEST(ResourceScoreboard, ModuloClear) {
  SB Scoreboard;
  Scoreboard.configModulo(5);
  for (int I = 0; I < 5; I++) {
    Scoreboard[I] = RC(I + 1);
  }
  Scoreboard.clear();

  // Expected is all-empty.
  SB Expected;
  Expected.configModulo(5);
  compareModulo(Scoreboard, Expected);
}

// Modulo mode: isInRange() accepts [-Period, INT_MAX - Period] and rejects
// anything below -Period.
TEST(ResourceScoreboard, ModuloIsInRange) {
  SB Scoreboard;
  Scoreboard.configModulo(4);
  // Every non-negative cycle is in range.
  EXPECT_TRUE(Scoreboard.isInRange(0));
  EXPECT_TRUE(Scoreboard.isInRange(3));
  EXPECT_TRUE(Scoreboard.isInRange(100));
  // Negative indices down to -Period are in range.
  EXPECT_TRUE(Scoreboard.isInRange(-1));
  EXPECT_TRUE(Scoreboard.isInRange(-4));
  // Indices below -Period are out of range.
  EXPECT_FALSE(Scoreboard.isInRange(-5));
  EXPECT_FALSE(Scoreboard.isInRange(-100));
}

// Modulo mode: a resource use at the last cycle of the period and at the first
// cycle of the next iteration (period + 0) both map to the same slot, so an
// itinerary that straddles the stage boundary is captured correctly.
// The full scoreboard state is verified after each write.
TEST(ResourceScoreboard, ModuloStageBoundaryWrap) {
  SB Scoreboard;
  // Period of 4: slots 0, 1, 2, 3.
  Scoreboard.configModulo(4);

  // Occupy the last slot of the period.
  Scoreboard[3] = RC(1);
  SB Expected;
  Expected.configModulo(4);
  Expected[3] = RC(1);
  compareModulo(Scoreboard, Expected);

  // Cycle 4 wraps to slot 0; only slot 0 must change.
  Scoreboard[4] = RC(2);
  Expected[0] = RC(2);
  compareModulo(Scoreboard, Expected);

  // Cycle -1 aliases slot 3; only slot 3 must change.
  Scoreboard[-1] = RC(3);
  Expected[3] = RC(3);
  compareModulo(Scoreboard, Expected);
}
