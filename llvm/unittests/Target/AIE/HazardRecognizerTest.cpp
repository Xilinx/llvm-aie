//===- HazardRecognizerTest.cpp ---------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
#include "AIE2.h"
#include "AIE2InstrInfo.h"
#include "AIEHazardRecognizer.h"
#include "MCTargetDesc/AIEFormat.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/ResourceScoreboard.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/MC/MCInstrItineraries.h"
#include "llvm/MC/TargetRegistry.h"
#include "gtest/gtest.h"

using namespace llvm;

const auto Req = InstrStage::ReservationKinds::Required;
const auto Res = InstrStage::ReservationKinds::Reserved;

static const llvm::InstrStage MockStages[] = {
    // { Number of cycles, bitvec of resources,  NextCycle, reservation kind }
    // We force 6 stages, so we should be able to check cycles in [-6, 5]
    {6, 0b11111111, 6, Req},
    {1, 0b00000001, 1, Req},
    {1, 0b00000001, 1, Res},
    {1, 0b00000010, 1, Req},

    {0, 0b00000000, 4, Req},
    {1, 0b00000100, 1, Req},
    {1, 0b00001000, 2, Req},
    {1, 0b00000001, 1, Req},

    // multistage cycle, 0b111, 0b000, 0b001
    {1, 0b00000001, 0, Req},
    {1, 0b00000010, 0, Req},
    {1, 0b00000100, 2, Req},
    {1, 0b00000001, 1, Req},

};

class MockItineraries : public InstrItineraryData {
public:
  ArrayRef<const InstrStage> getStages(unsigned SchedClass) const override {
    switch (SchedClass) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 5:
    case 8:
    case 9:
    case 10:
    case 11:
      return {MockStages + SchedClass, 1};
    case 4:
      return {MockStages + 4, 2};
    case 6:
      return {MockStages + 6, 2};
    case 12:
      return {MockStages + 8, 4};
    default:
      return {MockStages, 1};
    }
  }
  bool isEndMarker(unsigned SchedClass) const override {
    return SchedClass > 12;
  }
  bool isEmpty() const override { return false; }
  std::optional<unsigned> getOperandCycle(unsigned SchedClass,
                                          unsigned OpIdx) const override {
    return {};
  }
};

// Not really used, except to get a FormatInterface.
AIE2InstrInfo DummyInstrInfo;

MockItineraries Itins;

// Derived class to access protected methods
class MockHR : public AIEHazardRecognizer {
  AIEAlternateDescriptors AlternateDescriptors;
  ResourceScoreboard<FuncUnitWrapper> MockScoreboard;

public:
  ~MockHR() = default;
  MockHR()
      : AIEHazardRecognizer(&DummyInstrInfo, &Itins, AlternateDescriptors,
                            /*IsPreRA=*/false) {
    MockScoreboard.reset(computeScoreboardDepth());
  }
  MockHR(const AIEBaseInstrInfo &InstrInfo)
      : AIEHazardRecognizer(&InstrInfo, &Itins, AlternateDescriptors,
                            /*IsPreRA=*/false) {
    MockScoreboard.reset(computeScoreboardDepth());
  }
  void emit(unsigned SchedClass, int Delta, SlotBits SlotSet = 0,
            MemoryBankBits MemoryBanks = 0,
            SmallVector<int, 2> MemoryAccessCycles = {}) {
    enterResources(MockScoreboard, &Itins, SchedClass, SlotSet, MemoryBanks,
                   MemoryAccessCycles, Delta, std::nullopt);
  }
  bool hazard(unsigned SchedClass, int Delta, SlotBits SlotSet = 0,
              MemoryBankBits MemoryBanks = 0,
              SmallVector<int, 2> MemoryAccessCycles = {}) {
    return checkConflict(MockScoreboard, &Itins, SchedClass, SlotSet,
                         MemoryBanks, MemoryAccessCycles, Delta, std::nullopt);
  }
  void AdvanceCycle() override { MockScoreboard.advance(); }
  void RecedeCycle() override { MockScoreboard.recede(); }
  void Reset() override {
    AIEHazardRecognizer::Reset();
    MockScoreboard.clear();
  }
  bool conflict(const MockHR &Other, int DeltaCycles) const {
    return MockScoreboard.conflict(Other.MockScoreboard, DeltaCycles);
  }
  void blockResources(int DeltaCycles) {
    AIEHazardRecognizer::blockCycleInScoreboard(DeltaCycles);
  }
};

TEST(HazardRecognizer, empty) {
  MockHR HR;

  bool Conflict = false;
  for (unsigned SC = 0; !Itins.isEndMarker(SC); SC++) {
    // Never a conflict in an empty scoreboard
    Conflict |= HR.hazard(SC, 0);
  }
  EXPECT_FALSE(Conflict);
  for (unsigned SC = 1; !Itins.isEndMarker(SC); SC++) {
    // Neither in the past
    Conflict |= HR.hazard(SC, -4);
  }
  EXPECT_FALSE(Conflict);
  for (int C = 0; C < 4; C++) {
    HR.AdvanceCycle();
    for (unsigned SC = 1; !Itins.isEndMarker(SC); SC++) {
      // Nor in the future
      Conflict |= HR.hazard(SC, 0);
    }
  }
  EXPECT_FALSE(Conflict);
}

TEST(HazardRecognizer, simple) {
  MockHR HR;

  // Fill [-6, -1]
  HR.emit(0, -6);
  EXPECT_TRUE(HR.hazard(0, -6));
  EXPECT_TRUE(HR.hazard(1, -6));
  EXPECT_TRUE(HR.hazard(1, -5));
  EXPECT_TRUE(HR.hazard(1, -1));
  EXPECT_FALSE(HR.hazard(1, 0));
  HR.RecedeCycle();
  EXPECT_TRUE(HR.hazard(1, 0));
}

TEST(HazardRecognizer, past) {
  MockHR HR;

  // Emit slightly in the past
  // The resource usage will land in cycle 1
  // 0 (4x4) ---
  // 1       ---
  // 2       ---
  // 3       ---
  // 4 (1x5) 1--

  HR.emit(4, -3);
  EXPECT_TRUE(HR.hazard(4, -3));
  EXPECT_FALSE(HR.hazard(5, -3));
  EXPECT_FALSE(HR.hazard(5, -2));
  EXPECT_FALSE(HR.hazard(5, -1));
  EXPECT_FALSE(HR.hazard(5, 0));
  HR.AdvanceCycle();
  EXPECT_TRUE(HR.hazard(5, 0));
  HR.AdvanceCycle();
  EXPECT_FALSE(HR.hazard(5, 0));
  HR.AdvanceCycle();
  EXPECT_FALSE(HR.hazard(5, 0));

  // Emit further in the past.
  // The resource usage will land in cycle -2
  MockHR HR2;
  HR2.emit(4, -6);
  EXPECT_FALSE(HR2.hazard(5, -3));
  EXPECT_TRUE(HR2.hazard(5, -2));
  EXPECT_FALSE(HR2.hazard(5, -1));
  EXPECT_FALSE(HR2.hazard(5, 0));
  HR2.RecedeCycle();
  EXPECT_FALSE(HR2.hazard(5, -3));
  EXPECT_FALSE(HR2.hazard(5, -2));
  EXPECT_TRUE(HR2.hazard(5, -1));
  EXPECT_FALSE(HR2.hazard(5, 0));
  HR2.RecedeCycle();
  EXPECT_FALSE(HR2.hazard(5, -3));
  EXPECT_FALSE(HR2.hazard(5, -2));
  EXPECT_FALSE(HR2.hazard(5, -1));
  EXPECT_TRUE(HR2.hazard(5, 0));
}

TEST(HazardRecognizer, flush) {
  MockHR HR;

  auto Fill = [&HR]() {
    // Fill it up
    HR.emit(0, -6);
    HR.AdvanceCycle();
    HR.emit(0, -1);
  };

  // Make sure that it is empty
  auto CheckEmpty = [&HR]() {
    for (unsigned SC = 0; SC <= 6; SC++) {
      for (int C = -6; C <= 0; C++) {
        if (HR.hazard(SC, C)) {
          return false;
        }
      }
    }
    return true;
  };

  Fill();
  EXPECT_FALSE(CheckEmpty());
  // Flush it. Note that flushing more than the depth doesn't harm.
  for (int C = 0; C < 16; C++) {
    HR.AdvanceCycle();
  }
  EXPECT_TRUE(CheckEmpty());

  HR.Reset();
  Fill();
  EXPECT_FALSE(CheckEmpty());
  // Flush it in reverse
  for (int I = 0; I < 16; I++) {
    HR.RecedeCycle();
  }
  EXPECT_TRUE(CheckEmpty());

  Fill();
  HR.Reset();
  EXPECT_TRUE(CheckEmpty());
}

TEST(HazardRecognizer, scoreboardConflict) {
  MockHR Top;
  MockHR Bot;

  // Empty scoreboards never conflict
  for (int I = 0; I < 6; I++) {
    EXPECT_FALSE(Top.conflict(Bot, I));
    EXPECT_FALSE(Bot.conflict(Top, I));
  }

  // Put the same resource in different cycles
  Top.emit(1, -2);
  Bot.emit(1, 0);

  EXPECT_FALSE(Top.conflict(Bot, -3));
  EXPECT_TRUE(Top.conflict(Bot, -2));
  EXPECT_FALSE(Top.conflict(Bot, -1));
  EXPECT_FALSE(Top.conflict(Bot, 0));
  EXPECT_FALSE(Top.conflict(Bot, 1));

  EXPECT_FALSE(Bot.conflict(Top, -1));
  EXPECT_FALSE(Bot.conflict(Top, 0));
  EXPECT_FALSE(Bot.conflict(Top, 1));
  EXPECT_TRUE(Bot.conflict(Top, 2));
  EXPECT_FALSE(Bot.conflict(Top, 3));

  // conflict shouldn't be very picky about out of range
  // deltas, and should check both past and future
  MockHR One;
  MockHR Two;
  One.emit(5, -6);
  // Can't emit in the future. But we know it fits, so emit now
  // and travel into the past.
  Two.emit(5, 0);
  for (int C = 0; C < 5; C++) {
    Two.RecedeCycle();
  }
  for (int C = -20; C < 20; C++) {
    EXPECT_EQ(One.conflict(Two, C), C == -11);
    EXPECT_EQ(Two.conflict(One, C), C == 11);
  }
}

TEST(HazardRecognizer, copy) {
  MockHR HR;
  MockHR Copy0(HR);

  // Check that we can copy and use methods on const objects
  EXPECT_FALSE(HR.conflict(Copy0, 0));

  HR.emit(1, 0);
  const MockHR Cst(HR);
  MockHR Copy1(Cst);
  EXPECT_TRUE(HR.conflict(Cst, 0));
  EXPECT_TRUE(HR.conflict(Copy1, 0));
  EXPECT_TRUE(Cst.conflict(Copy1, 0));
  EXPECT_TRUE(Cst.conflict(Cst, 0));
}

TEST(HazardRecognizer, splitCycles) {
  MockHR HR;

  // Check that cycles split in multiple stages are treated correctly
  // (1x{8, 9, 10}) 111
  // -              ---
  // (1x11)         001
  HR.emit(12, -4);

  EXPECT_TRUE(HR.hazard(12, -4));
  EXPECT_FALSE(HR.hazard(12, -3));
  EXPECT_TRUE(HR.hazard(12, -2));
  EXPECT_FALSE(HR.hazard(12, -1));

  // HR is equivalent to itinerary 12
  EXPECT_TRUE(HR.conflict(HR, 0));
  EXPECT_FALSE(HR.conflict(HR, 1));
  EXPECT_TRUE(HR.conflict(HR, 2));
  EXPECT_FALSE(HR.conflict(HR, 3));

  EXPECT_TRUE(HR.hazard(8, -4));
  EXPECT_TRUE(HR.hazard(9, -4));
  EXPECT_TRUE(HR.hazard(10, -4));

  EXPECT_FALSE(HR.hazard(8, -3));
  EXPECT_FALSE(HR.hazard(9, -3));
  EXPECT_FALSE(HR.hazard(10, -3));

  EXPECT_TRUE(HR.hazard(8, -2));
  EXPECT_FALSE(HR.hazard(9, -2));
  EXPECT_FALSE(HR.hazard(10, -2));
}

/// Check scoreboard conflicts from slot utilization
TEST(HazardRecognizer, slotHazard) {
  // We use a hidden property of  AIE2's FormatInterface here, which is
  // that any combination of the first two slots is valid.
  // We construct it here locally, so that we are sure that the format
  // interface is set up correctly. Note that other unit tests in this
  // same executable seem to instantiate the AIE1 interface, and they clash
  // on a static implementation pointer.
  AIE2InstrInfo InstrInfo;
  MockHR HR(InstrInfo);

  HR.emit(1, -2, /*SlotSet=*/0b01);

  // Classes 1 and 3 have no resource conflicts in MockStages, they can only
  // conflict because of slots.
  EXPECT_FALSE(HR.hazard(3, -2, /*SlotSet=*/0b00));
  EXPECT_FALSE(HR.hazard(3, -2, /*SlotSet=*/0b10));
  EXPECT_TRUE(HR.hazard(3, -2, /*SlotSet=*/0b11));
  EXPECT_TRUE(HR.hazard(3, -2, /*SlotSet=*/0b01));
}

TEST(HazardRecognizer, composeConflicting) {
  // Check that we can add conflicting itineraries
  // without crashing. This allows replaying/merging scoreboards from
  // different successor blocks
  MockHR HR;

  HR.emit(8, -2);
  HR.emit(8, 0);
  HR.emit(9, -1, /*SlotSet=*/0b1);
  HR.emit(9, 0, /*SlotSet=*/0b1);

  // redundant, but harmless
  HR.emit(8, -2);
  HR.emit(9, 0, /*SlotSet=*/0b1);

  for (int C = -4; C <= 0; C++) {
    EXPECT_EQ(HR.hazard(8, C), C == -2 || C == 0);
    EXPECT_EQ(HR.hazard(9, C), C == -1 || C == 0);
  }
}

/// Check scoreboard conflicts from bank utilization
TEST(HazardRecognizer, bankConflictHazard) {
  AIE2InstrInfo InstrInfo;
  MockHR HR(InstrInfo);

  HR.emit(1, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010,
          /*MemoryAccessCycle=*/{5});

  // Classes 1 and 3 have no resource conflicts in MockStages, they can only
  // conflict because of Memory Banks.
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b01,
                         /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b100,
                         /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b0101,
                         /*MemoryAccessCycle=*/{5}));

  // Expected to conflict since same bank & same memory access cycle
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                        /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1000,
                        /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010,
                        /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1111,
                        /*MemoryAccessCycle=*/{5}));

  // Not Expected to conflict since same bank but differenec memory access cycle
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                         /*MemoryAccessCycle=*/{6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1000,
                         /*MemoryAccessCycle=*/{6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010,
                         /*MemoryAccessCycle=*/{6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1111,
                         /*MemoryAccessCycle=*/{6}));
}

/// Check scoreboard conflicts from bank utilization in multiple cycles
TEST(HazardRecognizer, bankConflictHazardMultiCycle) {
  AIE2InstrInfo InstrInfo;
  MockHR HR(InstrInfo);

  HR.emit(1, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010,
          /*MemoryAccessCycle=*/{5, 7});

  // Classes 1 and 3 have no resource conflicts in MockStages, they can only
  // conflict because of Memory Banks.
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b01,
                         /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b100,
                         /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b0101,
                         /*MemoryAccessCycle=*/{5}));

  // Expected to conflict since same bank & same memory access cycle
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                        /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1000,
                        /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010,
                        /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1111,
                        /*MemoryAccessCycle=*/{5}));

  // Not Expected to conflict since same bank but differenec memory access cycle
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                         /*MemoryAccessCycle=*/{6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1000,
                         /*MemoryAccessCycle=*/{6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010,
                         /*MemoryAccessCycle=*/{6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1111,
                         /*MemoryAccessCycle=*/{6}));

  // Not Expected to conflict since different bank but same memory access cycles
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b01,
                         /*MemoryAccessCycle=*/{5, 7}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b100,
                         /*MemoryAccessCycle=*/{5, 7}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b0101,
                         /*MemoryAccessCycle=*/{5, 7}));

  // Expected to conflict since same bank & same memory access cycles
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                        /*MemoryAccessCycle=*/{5, 7}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1000,
                        /*MemoryAccessCycle=*/{5, 11}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010,
                        /*MemoryAccessCycle=*/{7, 11}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1111,
                        /*MemoryAccessCycle=*/{5, 7}));

  // Not Expected to conflict since same bank but differenec memory access cycle
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                         /*MemoryAccessCycle=*/{1, 6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1000,
                         /*MemoryAccessCycle=*/{2, 8, 11}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010,
                         /*MemoryAccessCycle=*/{1, 6, 11}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1111,
                         /*MemoryAccessCycle=*/{6, 11}));
}

/// Check scoreboard to show blockResources does not touch MemoryBanks
TEST(HazardRecognizer, blockResourcesMemoryBanks) {
  AIE2InstrInfo InstrInfo;
  MockHR HR(InstrInfo);

  HR.blockResources(0);

  // Not Expected to conflict since blockResources does not touch MemoryBanks
  EXPECT_FALSE(HR.hazard(0, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                         /*MemoryAccessCycle=*/{1, 6}));
  EXPECT_FALSE(HR.hazard(0, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                         /*MemoryAccessCycle=*/{5}));
}