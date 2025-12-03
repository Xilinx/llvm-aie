//===- SlotOccupancyTest.cpp -----------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Unit tests for SlotOccupancy.
//
//===----------------------------------------------------------------------===//

#include "AIESlotOccupancy.h"
#include "AIESlotStructure.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm/Support/MathExtras.h"
#include "gtest/gtest.h"

using namespace llvm;

namespace {

// Mock slot structure mimicking AIE2P architecture
// Real slots (indices 0-6): A, B, X, M, S, V, L
// MSP classes (indices 7-10):
//   - MSP_AB (index 7): can use A OR B
//   - MSP_AXM (index 8): can use A OR X OR M
//   - MSP_ABS (index 9): can use A OR B OR S
//   - MSP_ABXM (index 10): can use A OR B OR X OR M
class MockSlotStructure : public AIESlotStructure {
public:
  unsigned getNumRealSlots() const override { return 7; }

  SlotBits getMSPComposition(unsigned ClassIdx) const override {
    // Real slots: composition = (1 << ClassIdx)
    if (ClassIdx < 7)
      return (1ULL << ClassIdx);

    // MSP classes
    switch (ClassIdx) {
    case 7: // MSP_AB: A + B
      return (1ULL << 0) | (1ULL << 1);
    case 8: // MSP_AXM: A + X + M
      return (1ULL << 0) | (1ULL << 2) | (1ULL << 3);
    case 9: // MSP_ABS: A + B + S
      return (1ULL << 0) | (1ULL << 1) | (1ULL << 4);
    case 10: // MSP_ABXM: A + B + X + M
      return (1ULL << 0) | (1ULL << 1) | (1ULL << 2) | (1ULL << 3);
    default:
      return 0;
    }
  }
};

// Mock format interface
class MockFormatInterface : public AIEBaseMCFormats {
  MockSlotStructure SlotStruct;

public:
  const AIESlotStructure &getSlotStructure() const override {
    return SlotStruct;
  }

  bool isFormatAvailable(uint64_t SlotSet) const override {
    // Format rules:
    // - Any combination of A, B, S, X, M, V is valid
    // - L conflicts with X and M only

    bool HasL = (SlotSet & (1ULL << 6)) != 0;
    bool HasX = (SlotSet & (1ULL << 2)) != 0;
    bool HasM = (SlotSet & (1ULL << 3)) != 0;

    // L conflicts with X or M
    if (HasL && (HasX || HasM))
      return false;

    return true;
  }

  // Stub implementations for pure virtual methods
  std::optional<unsigned int>
  getFormatDescIndex(unsigned int Opcode) const override {
    return std::nullopt;
  }

  const std::vector<unsigned int> *
  getAlternateInstsOpcode(unsigned int Opcode) const override {
    return nullptr;
  }

  const MCSlotInfo *getSlotInfo(const MCSlotKind Kind) const override {
    return nullptr;
  }

  const MCFormatDesc *getMCFormats() const override { return nullptr; }

  const PacketFormats &getPacketFormats() const override {
    llvm_unreachable("Not implemented in mock");
  }

  ArrayRef<bool> getIsFormatAvailable() const override { return {}; }
};

} // anonymous namespace

// Basic functionality tests
TEST(SlotOccupancy, ConstructorFromSlotBits) {
  SlotOccupancy Occ(0b1010);
  EXPECT_FALSE(Occ.isEmpty());
}

TEST(SlotOccupancy, ConstructorFromClassIndex) {
  SlotOccupancy Occ(7, 2);
  EXPECT_FALSE(Occ.isEmpty());
}

TEST(SlotOccupancy, Total) {
  SlotOccupancy Occ1(0b0111);
  EXPECT_EQ(Occ1.total(), 3u);

  SlotOccupancy Occ2(7, 2);
  EXPECT_EQ(Occ2.total(), 2u);
}

TEST(SlotOccupancy, CapacityComputation) {
  MockSlotStructure SS;

  // Real slots have capacity 1 (composition = single bit)
  EXPECT_EQ(SS.getCapacity(0), 1u); // A
  EXPECT_EQ(SS.getCapacity(1), 1u); // B

  // MSPs have capacity = popcount(composition)
  EXPECT_EQ(SS.getCapacity(7), 2u);  // MSP_AB {A,B}
  EXPECT_EQ(SS.getCapacity(8), 3u);  // MSP_AXM {A,X,M}
  EXPECT_EQ(SS.getCapacity(9), 3u);  // MSP_ABS {A,B,S}
  EXPECT_EQ(SS.getCapacity(10), 4u); // MSP_ABXM {A,B,X,M}
}

// MSP Materialization - MSP can use ANY slot from its composition

TEST(SlotOccupancy, MSP_AB_NoConflictWithFixedA) {
  MockFormatInterface FI;
  // MSP_AB can use B since A is occupied
  SlotOccupancy FixedA(0b0001);
  SlotOccupancy MSP_AB(7, 1);
  EXPECT_FALSE(MSP_AB.conflict(FixedA, FI));
}

TEST(SlotOccupancy, MSP_AB_NoConflictWithFixedB) {
  MockFormatInterface FI;
  // MSP_AB can use A since B is occupied
  SlotOccupancy FixedB(0b0010);
  SlotOccupancy MSP_AB(7, 1);
  EXPECT_FALSE(MSP_AB.conflict(FixedB, FI));
}

TEST(SlotOccupancy, MSP_AB_ConflictsWithFixedAB) {
  MockFormatInterface FI;
  // MSP_AB cannot materialize if both A and B are occupied
  SlotOccupancy FixedAB(0b0011);
  SlotOccupancy MSP_AB(7, 1);
  EXPECT_TRUE(MSP_AB.conflict(FixedAB, FI));
}

TEST(SlotOccupancy, TwoMSP_AB_NoConflict) {
  MockFormatInterface FI;
  // Two MSP_ABs can use A and B respectively
  SlotOccupancy MSP1(7, 1);
  SlotOccupancy MSP2(7, 1);
  EXPECT_FALSE(MSP1.conflict(MSP2, FI));
}

TEST(SlotOccupancy, ThreeMSP_AB_ExceedsCapacity) {
  MockFormatInterface FI;
  // Three MSP_ABs exceed capacity of 2
  SlotOccupancy MSP1(7, 2);
  SlotOccupancy MSP2(7, 1);
  EXPECT_TRUE(MSP1.conflict(MSP2, FI));
}

TEST(SlotOccupancy, MSP_AXM_WithOneSlotOccupied) {
  MockFormatInterface FI;
  // MSP_AXM {A,X,M} can still use 2 other slots if one is occupied
  SlotOccupancy FixedA(0b0001);
  SlotOccupancy MSP_AXM(8, 1);
  EXPECT_FALSE(MSP_AXM.conflict(FixedA, FI));
}

TEST(SlotOccupancy, MSP_AXM_WithTwoSlotsOccupied) {
  MockFormatInterface FI;
  // MSP_AXM can still use M if A and X are occupied
  SlotOccupancy FixedAX(0b0101);
  SlotOccupancy MSP_AXM(8, 1);
  EXPECT_FALSE(MSP_AXM.conflict(FixedAX, FI));
}

TEST(SlotOccupancy, MSP_AXM_ConflictsWithAllSlotsOccupied) {
  MockFormatInterface FI;
  // MSP_AXM cannot materialize if A, X, and M are all occupied
  SlotOccupancy FixedAXM(0b1101);
  SlotOccupancy MSP_AXM(8, 1);
  EXPECT_TRUE(MSP_AXM.conflict(FixedAXM, FI));
}

TEST(SlotOccupancy, TwoMSPs_AB_AXM_NoConflict) {
  MockFormatInterface FI;
  // MSP_AB can use B, MSP_AXM can use X or M - no conflict
  SlotOccupancy MSP_AB(7, 1);
  SlotOccupancy MSP_AXM(8, 1);
  EXPECT_FALSE(MSP_AB.conflict(MSP_AXM, FI));
}

TEST(SlotOccupancy, MSP_ABXM_WithThreeSlotsOccupied) {
  MockFormatInterface FI;
  // MSP_ABXM can use the one remaining slot
  SlotOccupancy FixedABX(0b0111);
  SlotOccupancy MSP_ABXM(10, 1);
  EXPECT_FALSE(MSP_ABXM.conflict(FixedABX, FI));
}

TEST(SlotOccupancy, FourMSP_ABXM_FillAllSlots) {
  MockFormatInterface FI;
  // Four MSP_ABXMs can each use one of A, B, X, M
  SlotOccupancy MSP(10, 4);
  EXPECT_FALSE(MSP.isEmpty());
  // Should not conflict with itself at capacity
  MockSlotStructure SS;
  EXPECT_TRUE(MSP.boundedBy(SS));
}

// L slot conflicts

TEST(SlotOccupancy, LConflictsWithX) {
  MockFormatInterface FI;
  SlotOccupancy FixedL(0b1000000);
  SlotOccupancy FixedX(0b0000100);
  EXPECT_TRUE(FixedL.conflict(FixedX, FI));
}

TEST(SlotOccupancy, LConflictsWithM) {
  MockFormatInterface FI;
  SlotOccupancy FixedL(0b1000000);
  SlotOccupancy FixedM(0b0001000);
  EXPECT_TRUE(FixedL.conflict(FixedM, FI));
}

TEST(SlotOccupancy, MSP_AXM_NoConflictWithFixedL) {
  MockFormatInterface FI;
  // MSP_AXM can materialize to A, which doesn't conflict with L
  SlotOccupancy FixedL(0b1000000);
  SlotOccupancy MSP_AXM(8, 1);
  EXPECT_FALSE(MSP_AXM.conflict(FixedL, FI));
}

TEST(SlotOccupancy, MSP_AB_NoConflictWithFixedL) {
  MockFormatInterface FI;
  // MSP_AB uses A or B, which don't conflict with L
  SlotOccupancy FixedL(0b1000000);
  SlotOccupancy MSP_AB(7, 1);
  EXPECT_FALSE(MSP_AB.conflict(FixedL, FI));
}

TEST(SlotOccupancy, PureMSPs_AB_ABS_CannotMaterialize) {
  MockFormatInterface FI;
  // 2x MSP_AB will use A and B
  // 2x MSP_ABS needs 2 slots from {A,B,S}, but A and B are taken
  // Only S is available, which is insufficient for 2 instances
  SlotOccupancy TwoMSP_AB(7, 2);  // Uses A and B
  SlotOccupancy TwoMSP_ABS(9, 2); // Needs 2 from {A,B,S}, only S left
  EXPECT_TRUE(TwoMSP_AB.conflict(TwoMSP_ABS, FI));
}
