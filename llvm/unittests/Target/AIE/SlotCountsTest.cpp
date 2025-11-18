//===- SlotCountsTest.cpp ---------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIESlotCounts.h"
#include "gtest/gtest.h"

#include <algorithm>
#include <cstdlib>

using namespace llvm;
using namespace llvm::AIE;

// Helper: Build SlotCounts using default constructor and non-const indexing.
static SlotCounts makeCounts(std::initializer_list<int> Vals) {
  SlotCounts C;
  int I = 0;
  for (int V : Vals)
    C[I++] = V;
  return C;
}

// Metric: using SlotCounts::distance method directly.

static bool equalByDistance(const SlotCounts &A, const SlotCounts &B) {
  return A.distance(B) == 0;
}

// Tautologies expected from a vector space over integers:
// - Additive identity: X + 0 == X
// - Commutativity: X + Y == Y + X
// - Associativity: (X + Y) + Z == X + (Y + Z)
// - Additive inverse: X + (-1)*X == 0
// - Distributivity: a*(X + Y) == a*X + a*Y
// - Distributivity over scalars: (a + b)*X == a*X + b*X
// - Scalar associativity: a*(b*X) == (a*b)*X
static void checkVectorSpaceTautologies(const SlotCounts &X,
                                        const SlotCounts &Y,
                                        const SlotCounts &Z) {
  SlotCounts Zero; // default constructed is the zero vector

  // Additive identity
  EXPECT_TRUE(equalByDistance(X + Zero, X));
  EXPECT_TRUE(equalByDistance(Zero + X, X));

  // Commutativity
  EXPECT_TRUE(equalByDistance(X + Y, Y + X));

  // Associativity
  EXPECT_TRUE(equalByDistance((X + Y) + Z, X + (Y + Z)));

  // Additive inverse
  EXPECT_TRUE(equalByDistance(X + (-1) * X, Zero));
  EXPECT_TRUE(equalByDistance((-1) * X + X, Zero));

  // Distributivity a*(X+Y) == a*X + a*Y
  int a = 3;
  EXPECT_TRUE(equalByDistance(a * (X + Y), a * X + a * Y));

  // Distributivity over scalars (a + b)*X == a*X + b*X
  int b = -2;
  EXPECT_TRUE(equalByDistance((a + b) * X, a * X + b * X));

  // Scalar associativity a*(b*X) == (a*b)*X
  EXPECT_TRUE(equalByDistance(a * (b * X), (a * b) * X));

  // Subtraction tautologies
  EXPECT_TRUE(equalByDistance((X - Y) + Y, X));
  EXPECT_TRUE(equalByDistance(X - X, Zero));

  // Zero scalar
  EXPECT_TRUE(equalByDistance(0 * X, Zero));
}

TEST(SlotCounts, ConstructAndBasics) {
  // Zero vector via default constructor.
  SlotCounts Zero;
  EXPECT_EQ(Zero.size(), 0);
  EXPECT_EQ(Zero.distance(Zero), 0);
  EXPECT_EQ(Zero.max(), 0);
  EXPECT_EQ(Zero.totals(), 0);

  // Build via non-const indexing (expands size and initializes intermediates).
  SlotCounts A;
  A[3] = 4;  // {0,0,0,4}
  A[0] = 1;  // {1,0,0,4}
  A[1] = 2;  // {1,2,0,4}
  A[15] = 2; // expand up to MaxSlots (16), last index allowed

  EXPECT_GE(A.size(), 4);
  EXPECT_LE(A.size(), 16);
  EXPECT_EQ(A.at(0), 1);
  EXPECT_EQ(A.at(1), 2);
  EXPECT_EQ(A.at(2), 0);
  EXPECT_EQ(A.at(3), 4);
  EXPECT_EQ(A.at(15), 2);

  // max, maxIndex, totals
  EXPECT_EQ(A.max(), 4);
  EXPECT_EQ(A.maxIndex(), 3);
  EXPECT_EQ(A.totals(), 1 + 2 + 0 + 4 + 2);

  // Copy constructor
  SlotCounts B(A);
  EXPECT_TRUE(equalByDistance(A, B));
  EXPECT_EQ(A.distance(B), 0);
}

TEST(SlotCounts, DistanceAxioms) {
  SlotCounts X = makeCounts({1, 2, 0, 4});
  SlotCounts Y = makeCounts({1, 0, 0, 1});
  SlotCounts Z = makeCounts({0, 2, 3});

  // Non-negativity
  EXPECT_GE(X.distance(Y), 0);
  EXPECT_GE(Y.distance(Z), 0);
  EXPECT_GE(Z.distance(X), 0);

  // Identity of indiscernibles: d(X, X) == 0 and d(X, Y) == 0 iff X == Y
  EXPECT_EQ(X.distance(X), 0);
  EXPECT_EQ(Y.distance(Y), 0);
  EXPECT_EQ(Z.distance(Z), 0);

  SlotCounts Ysame(Y);
  EXPECT_EQ(Y.distance(Ysame), 0);
  EXPECT_TRUE(equalByDistance(Y, Ysame));

  EXPECT_GT(X.distance(Y), 0);
  EXPECT_GT(Y.distance(Z), 0);
  EXPECT_GT(Z.distance(X), 0);

  // Symmetry
  EXPECT_EQ(X.distance(Y), Y.distance(X));
  EXPECT_EQ(Y.distance(Z), Z.distance(Y));
  EXPECT_EQ(Z.distance(X), X.distance(Z));

  // Triangle inequality: d(X, Z) <= d(X, Y) + d(Y, Z)
  EXPECT_LE(X.distance(Z), X.distance(Y) + Y.distance(Z));
  EXPECT_LE(Y.distance(X), Y.distance(Z) + Z.distance(X));
  EXPECT_LE(Z.distance(Y), Z.distance(X) + X.distance(Y));
}

TEST(SlotCounts, VectorSpaceTautologies) {
  SlotCounts X = makeCounts({1, 2, 3});
  SlotCounts Y = makeCounts({0, 5, -1, 2});
  SlotCounts Z = makeCounts({3, -2});

  checkVectorSpaceTautologies(X, Y, Z);

  // Additional checks using different scalars and shapes.
  int a = -4, b = 7;
  EXPECT_TRUE(equalByDistance(a * (X + Z), a * X + a * Z));
  EXPECT_TRUE(equalByDistance((a + b) * Y, a * Y + b * Y));
  EXPECT_TRUE(equalByDistance(a * (b * Z), (a * b) * Z));
}
