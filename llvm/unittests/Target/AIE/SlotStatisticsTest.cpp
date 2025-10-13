//===- SlotStatisticsTest.cpp ----------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Unit tests for SlotStatistics distance axioms.
//
//===----------------------------------------------------------------------===//

#include "AIESlotStatistics.h"
#include "llvm/ADT/ArrayRef.h"
#include "gtest/gtest.h"

#include <cstdlib>

using namespace llvm;
using namespace llvm::AIE;

static bool equalByDistance(const SlotStatistics &A, const SlotStatistics &B) {
  return A.distance(B) == 0;
}

TEST(SlotStatistics, DistanceAxioms) {
  // Construct a few SlotStatistics with different Fixed/Free shapes.
  SlotStatistics X({1, 2, 3}, {0, 1}); // Fixed: [1,2,3], Free: [0,1]
  SlotStatistics Y({1, 0, 3}, {2});    // Fixed: [1,0,3], Free: [2]
  SlotStatistics Z({0, 2}, {3, 1, 0}); // Fixed: [0,2],   Free: [3,1,0]
  SlotStatistics Zero({}, {});         // Both Fixed and Free empty

  // Non-negativity
  EXPECT_GE(X.distance(Y), 0);
  EXPECT_GE(Y.distance(Z), 0);
  EXPECT_GE(Z.distance(X), 0);
  EXPECT_GE(X.distance(Zero), 0);
  EXPECT_GE(Zero.distance(X), 0);
  EXPECT_GE(Zero.distance(Zero), 0);

  // Identity of indiscernibles
  EXPECT_EQ(X.distance(X), 0);
  EXPECT_EQ(Y.distance(Y), 0);
  EXPECT_EQ(Z.distance(Z), 0);
  EXPECT_EQ(Zero.distance(Zero), 0);

  EXPECT_TRUE(equalByDistance(X, X));
  EXPECT_TRUE(equalByDistance(Y, Y));
  EXPECT_TRUE(equalByDistance(Z, Z));
  EXPECT_TRUE(equalByDistance(Zero, Zero));

  // Distinct pairs should be at positive distance
  EXPECT_GT(X.distance(Y), 0);
  EXPECT_GT(Y.distance(Z), 0);
  EXPECT_GT(Z.distance(X), 0);
  EXPECT_GT(X.distance(Zero), 0);
  EXPECT_GT(Zero.distance(X), 0);

  // Symmetry
  EXPECT_EQ(X.distance(Y), Y.distance(X));
  EXPECT_EQ(Y.distance(Z), Z.distance(Y));
  EXPECT_EQ(Z.distance(X), X.distance(Z));
  EXPECT_EQ(X.distance(Zero), Zero.distance(X));

  // Triangle inequality
  EXPECT_LE(X.distance(Z), X.distance(Y) + Y.distance(Z));
  EXPECT_LE(Y.distance(X), Y.distance(Z) + Z.distance(X));
  EXPECT_LE(Z.distance(Y), Z.distance(X) + X.distance(Y));

  // Triangle inequality with Zero should also hold
  EXPECT_LE(X.distance(Zero), X.distance(Y) + Y.distance(Zero));
  EXPECT_LE(Y.distance(Zero), Y.distance(Z) + Z.distance(Zero));
  EXPECT_LE(Z.distance(Zero), Z.distance(X) + X.distance(Zero));
}
