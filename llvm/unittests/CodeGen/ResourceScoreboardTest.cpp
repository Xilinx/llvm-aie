//===- ResourceScoreboardTest.cpp -----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This tests the ResourceScoreboard template, which represents a sliding
// window of cycles around a moving reference point. It is organized as a
// circular buffer in order to minimize data movements. This is not visible in
// the external API, but defines some corner cases we test.
//===----------------------------------------------------------------------===//
#include "gtest/gtest.h"

#include "llvm/CodeGen/ResourceScoreboard.h"

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