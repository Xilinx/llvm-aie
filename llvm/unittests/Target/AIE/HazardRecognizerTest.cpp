//===- HazardRecognizerTest.cpp ---------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
#include "AIE2.h"
#include "AIE2InstrInfo.h"
#include "AIEHazardRecognizer.h"
#include "MCTargetDesc/AIEFormat.h"
#include "llvm/CodeGen/MIRParser/MIRParser.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/ResourceScoreboard.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/MCInstrItineraries.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "gtest/gtest.h"

using namespace llvm;

const auto Req = InstrStage::ReservationKinds::Required;
const auto Res = InstrStage::ReservationKinds::Reserved;

// Note: This test has a history, and bears the scratches and tears of
// a number of hazardrecognizer and scoreboard reorganizations.
// The hazard recognizer is the one shared by all AIE versions, but we supply
// it with our own mock itineraries.
// We test conflict detection on the SchedClass level.
// We only use a few resources.
// A recent overhaul changed bitsets to resource indices. This
// was backed by the way that we define itineraries, always one
// resource per stage, however, the test employed multi-resource bitsets
// as a shorthand. We made an effort to give new equivalent representations,
// which makes the result a bit clunky. Change comments are indicated by ===

static const llvm::InstrStage MockStages[] = {
    // { Number of cycles, resource index,  NextCycle, reservation kind }

    // We force 6 stages, so we should be able to check cycles in [-6, 5]
    // === This isn't used anymore. It defined 6 cycles of 8 occupied
    // resource, it has been moved down to have the space to expand it.
    {6, 10, 6, Req},

    {1, 0, 1, Req},
    {1, 0, 1, Res},
    {1, 1, 1, Req},

    // === This was empty. we use an isolated resource
    {0, 16, 4, Req},
    {1, 2, 1, Req},
    {1, 3, 2, Req},
    {1, 1, 1, Req},

    // multistage cycle, 0b111, 0b000, 0b001
    {1, 0, 0, Req},
    {1, 1, 0, Req},
    {1, 2, 2, Req},
    {1, 0, 1, Req},

    // === The expanded 6x8 resource of original entry 0
    // we economized a bit, reducing it to resource 0-3
    {6, 0, 0, Req},
    {6, 1, 0, Req},
    {6, 2, 0, Req},
    {6, 3, 0, Req},

    // Class 13: store-port resource 7 at cycle 4 (matches mem cycle 5)
    {0, 16, 4, Req},
    {1, 7, 1, Req},
    // Class 14: store-port resource 7 at cycle 5 (matches mem cycle 6)
    {0, 16, 5, Req},
    {1, 7, 1, Req},

};

class MockItineraries : public InstrItineraryData {
public:
  ArrayRef<const InstrStage> getStages(unsigned SchedClass) const override {
    switch (SchedClass) {
    case 0:
      return {MockStages + 12, 4};
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
    // Class 13: store-port resource at mem-access cycle 5.
    case 13:
      return {MockStages + 16, 2};
    // Class 14: store-port resource at mem-access cycle 6.
    case 14:
      return {MockStages + 18, 2};
    default:
      return {MockStages, 1};
    }
  }
  bool isEndMarker(unsigned SchedClass) const override {
    return SchedClass > 14;
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
            MemoryBankBits MemoryBanks = 0, MemoryObjectsBits ObjectsBits = 0,
            SmallVector<int, 2> MemoryAccessCycles = {}) {
    // Convert to MemoryObjectPair: assign same bits to both Load and Store
    // to preserve test semantics (tests don't distinguish load vs store).
    MemoryObjectPair ObjPair;
    ObjPair.Load = ObjectsBits;
    ObjPair.Store = ObjectsBits;
    enterResources(MockScoreboard, &Itins, SchedClass, SlotSet, MemoryBanks,
                   ObjPair, MemoryAccessCycles, Delta, std::nullopt);
  }
  bool hazard(unsigned SchedClass, int Delta, SlotBits SlotSet = 0,
              MemoryBankBits MemoryBanks = 0, MemoryObjectsBits ObjectsBits = 0,
              SmallVector<int, 2> MemoryAccessCycles = {}) {
    // Convert to MemoryObjectPair: assign same bits to both Load and Store
    // to preserve test semantics (tests don't distinguish load vs store).
    MemoryObjectPair ObjPair;
    ObjPair.Load = ObjectsBits;
    ObjPair.Store = ObjectsBits;
    return checkConflict(MockScoreboard, &Itins, SchedClass, SlotSet, SlotSet,
                         MemoryBanks, ObjPair, MemoryAccessCycles, Delta,
                         std::nullopt);
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

  // Load-only methods: only set LoadMemObjectsBits
  void emitLoad(unsigned SchedClass, int Delta, SlotBits SlotSet = 0,
                MemoryBankBits MemoryBanks = 0,
                MemoryObjectsBits LoadObjectsBits = 0,
                SmallVector<int, 2> MemoryAccessCycles = {}) {
    MemoryObjectPair ObjPair;
    ObjPair.Load = LoadObjectsBits;
    ObjPair.Store = 0;
    enterResources(MockScoreboard, &Itins, SchedClass, SlotSet, MemoryBanks,
                   ObjPair, MemoryAccessCycles, Delta, std::nullopt);
  }
  bool hazardLoad(unsigned SchedClass, int Delta, SlotBits SlotSet = 0,
                  MemoryBankBits MemoryBanks = 0,
                  MemoryObjectsBits LoadObjectsBits = 0,
                  SmallVector<int, 2> MemoryAccessCycles = {}) {
    MemoryObjectPair ObjPair;
    ObjPair.Load = LoadObjectsBits;
    ObjPair.Store = 0;
    return checkConflict(MockScoreboard, &Itins, SchedClass, SlotSet, SlotSet,
                         MemoryBanks, ObjPair, MemoryAccessCycles, Delta,
                         std::nullopt);
  }

  // Store-only methods: only set StoreMemObjectsBits
  void emitStore(unsigned SchedClass, int Delta, SlotBits SlotSet = 0,
                 MemoryBankBits MemoryBanks = 0,
                 MemoryObjectsBits StoreObjectsBits = 0,
                 SmallVector<int, 2> MemoryAccessCycles = {}) {
    MemoryObjectPair ObjPair;
    ObjPair.Load = 0;
    ObjPair.Store = StoreObjectsBits;
    enterResources(MockScoreboard, &Itins, SchedClass, SlotSet, MemoryBanks,
                   ObjPair, MemoryAccessCycles, Delta, std::nullopt);
  }
  bool hazardStore(unsigned SchedClass, int Delta, SlotBits SlotSet = 0,
                   MemoryBankBits MemoryBanks = 0,
                   MemoryObjectsBits StoreObjectsBits = 0,
                   SmallVector<int, 2> MemoryAccessCycles = {}) {
    MemoryObjectPair ObjPair;
    ObjPair.Load = 0;
    ObjPair.Store = StoreObjectsBits;
    return checkConflict(MockScoreboard, &Itins, SchedClass, SlotSet, SlotSet,
                         MemoryBanks, ObjPair, MemoryAccessCycles, Delta,
                         std::nullopt);
  }

  // Methods that set both Load and Store bitmaps independently (for testing
  // mixed operations like part-word stores, or testing bitmap coexistence).
  void emitLoadAndStore(unsigned SchedClass, int Delta, SlotBits SlotSet = 0,
                        MemoryBankBits MemoryBanks = 0,
                        MemoryObjectsBits LoadObjectsBits = 0,
                        MemoryObjectsBits StoreObjectsBits = 0,
                        SmallVector<int, 2> MemoryAccessCycles = {}) {
    MemoryObjectPair ObjPair;
    ObjPair.Load = LoadObjectsBits;
    ObjPair.Store = StoreObjectsBits;
    enterResources(MockScoreboard, &Itins, SchedClass, SlotSet, MemoryBanks,
                   ObjPair, MemoryAccessCycles, Delta, std::nullopt);
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

  HR.emit(1, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010, /*ObjectsBits=*/0,
          /*MemoryAccessCycle=*/{5});

  // Classes 1 and 3 have no resource conflicts in MockStages, they can only
  // conflict because of Memory Banks.
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b01,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b100,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b0101,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{5}));

  // Expected to conflict since same bank & same memory access cycle
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                        /*ObjectsBits=*/0,
                        /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1000,
                        /*ObjectsBits=*/0,
                        /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010,
                        /*ObjectsBits=*/0,
                        /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1111,
                        /*ObjectsBits=*/0,
                        /*MemoryAccessCycle=*/{5}));

  // Not Expected to conflict since same bank but differenec memory access cycle
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1000,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1111,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{6}));
}

/// Check scoreboard conflicts from bank utilization in multiple cycles
TEST(HazardRecognizer, bankConflictHazardMultiCycle) {
  AIE2InstrInfo InstrInfo;
  MockHR HR(InstrInfo);

  HR.emit(1, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010, /*ObjectsBits=*/0,
          /*MemoryAccessCycle=*/{5, 7});

  // Classes 1 and 3 have no resource conflicts in MockStages, they can only
  // conflict because of Memory Banks.
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b01,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b100,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b0101,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{5}));

  // Expected to conflict since same bank & same memory access cycle
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                        /*ObjectsBits=*/0,
                        /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1000,
                        /*ObjectsBits=*/0,
                        /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010,
                        /*ObjectsBits=*/0,
                        /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1111,
                        /*ObjectsBits=*/0,
                        /*MemoryAccessCycle=*/{5}));

  // Not Expected to conflict since same bank but differenec memory access cycle
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1000,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1111,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{6}));

  // Not Expected to conflict since different bank but same memory access cycles
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b01,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{5, 7}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b100,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{5, 7}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b0101,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{5, 7}));

  // Expected to conflict since same bank & same memory access cycles
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                        /*ObjectsBits=*/0,
                        /*MemoryAccessCycle=*/{5, 7}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1000,
                        /*ObjectsBits=*/0,
                        /*MemoryAccessCycle=*/{5, 11}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010,
                        /*ObjectsBits=*/0,
                        /*MemoryAccessCycle=*/{7, 11}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1111,
                        /*ObjectsBits=*/0,
                        /*MemoryAccessCycle=*/{5, 7}));

  // Not Expected to conflict since same bank but differenec memory access cycle
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{1, 6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1000,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{2, 8, 11}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1010,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{1, 6, 11}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b1111,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{6, 11}));
}

/// Check scoreboard to show blockResources does not touch MemoryBanks
TEST(HazardRecognizer, blockResourcesMemoryBanks) {
  AIE2InstrInfo InstrInfo;
  MockHR HR(InstrInfo);

  HR.blockResources(0);

  // Not Expected to conflict since blockResources does not touch MemoryBanks
  EXPECT_FALSE(HR.hazard(0, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{1, 6}));
  EXPECT_FALSE(HR.hazard(0, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0b010,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{5}));
}

/// Check scoreboard conflicts from object sharing
TEST(HazardRecognizer, objectConflictHazard) {
  AIE2InstrInfo InstrInfo;
  MockHR HR(InstrInfo);

  HR.emit(1, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0, /*ObjectsBits=*/0b101,
          /*MemoryAccessCycle=*/{5});

  // Classes 1 and 3 have no resource conflicts in MockStages, they can only
  // conflict because of Memory Banks and Objects.
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                         /*ObjectsBits=*/0,
                         /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                         /*ObjectsBits=*/0b01000,
                         /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                         /*ObjectsBits=*/0b010000,
                         /*MemoryAccessCycle=*/{5}));

  // Expected to conflict since same object & same memory access cycle
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                        /*ObjectsBits=*/0b01,
                        /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                        /*ObjectsBits=*/0b11,
                        /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                        /*ObjectsBits=*/0b100,
                        /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                        /*ObjectsBits=*/0b101,
                        /*MemoryAccessCycle=*/{5}));

  // Not Expected to conflict since same object but differenec memory access
  // cycle
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                         /*ObjectsBits=*/0b01,
                         /*MemoryAccessCycle=*/{6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                         /*ObjectsBits=*/0b11,
                         /*MemoryAccessCycle=*/{6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                         /*ObjectsBits=*/0b100,
                         /*MemoryAccessCycle=*/{6}));
  EXPECT_FALSE(HR.hazard(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                         /*ObjectsBits=*/0b101,
                         /*MemoryAccessCycle=*/{6}));
}

/// Two same-class stores at the same emission cycle conflict on the
/// shared store-port resource (and the slot in real AIE), even with
/// MemoryBanks=0.
TEST(HazardRecognizer, storeStoreSameMemoryCycleSerializes) {
  AIE2InstrInfo InstrInfo;
  MockHR HR(InstrInfo);

  HR.emit(13, 0, /*SlotSet=*/0b1, /*MemoryBanks=*/0, /*ObjectsBits=*/0,
          /*MemoryAccessCycle=*/{5});

  // Slot + resource both fire.
  EXPECT_TRUE(HR.hazard(13, 0, /*SlotSet=*/0b1, /*MemoryBanks=*/0,
                        /*ObjectsBits=*/0, /*MemoryAccessCycle=*/{5}));

  // Resource alone catches it without slot bits.
  EXPECT_TRUE(HR.hazard(13, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                        /*ObjectsBits=*/0, /*MemoryAccessCycle=*/{5}));

  // One cycle apart -> no overlap.
  EXPECT_FALSE(HR.hazard(13, 1, /*SlotSet=*/0b1, /*MemoryBanks=*/0,
                         /*ObjectsBits=*/0, /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazard(13, -1, /*SlotSet=*/0b1, /*MemoryBanks=*/0,
                         /*ObjectsBits=*/0, /*MemoryAccessCycle=*/{5}));
}

/// Stores in classes 13 (mem 5) and 14 (mem 6) collide on the slot at
/// the same emission cycle, and on the shared resource at delta=-1
/// where their resource cycles align at scoreboard cycle 4.
TEST(HazardRecognizer, storeStoreDifferentMemoryCycleSerializes) {
  AIE2InstrInfo InstrInfo;
  MockHR HR(InstrInfo);

  // Emit's resource lands at scoreboard cycle 4.
  HR.emit(13, 0, /*SlotSet=*/0b1, /*MemoryBanks=*/0, /*ObjectsBits=*/0,
          /*MemoryAccessCycle=*/{5});

  // Same-cycle: slot fires (resources at 4 vs 5 don't overlap).
  EXPECT_TRUE(HR.hazard(14, 0, /*SlotSet=*/0b1, /*MemoryBanks=*/0,
                        /*ObjectsBits=*/0, /*MemoryAccessCycle=*/{6}));

  // delta=-1: resource at -1+5=4 aligns with emit's resource.
  EXPECT_TRUE(HR.hazard(14, -1, /*SlotSet=*/0b1, /*MemoryBanks=*/0,
                        /*ObjectsBits=*/0, /*MemoryAccessCycle=*/{6}));

  // Same staggered case, resource alone (no slot).
  EXPECT_TRUE(HR.hazard(14, -1, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                        /*ObjectsBits=*/0, /*MemoryAccessCycle=*/{6}));

  // Same cycle without slot, resources don't overlap -> no hazard.
  EXPECT_FALSE(HR.hazard(14, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                         /*ObjectsBits=*/0, /*MemoryAccessCycle=*/{6}));
}

/// Check that two loads accessing the same object at the same cycle conflict.
TEST(HazardRecognizer, loadLoadObjectConflictHazard) {
  AIE2InstrInfo InstrInfo;
  MockHR HR(InstrInfo);

  // Emit a load with object bits 0b101 at memory cycle 5
  HR.emitLoad(1, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
              /*LoadObjectsBits=*/0b101, /*MemoryAccessCycle=*/{5});

  // Another load with overlapping objects at the same cycle should conflict
  EXPECT_TRUE(HR.hazardLoad(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                            /*LoadObjectsBits=*/0b001,
                            /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazardLoad(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                            /*LoadObjectsBits=*/0b100,
                            /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazardLoad(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                            /*LoadObjectsBits=*/0b101,
                            /*MemoryAccessCycle=*/{5}));

  // Non-overlapping objects should not conflict
  EXPECT_FALSE(HR.hazardLoad(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                             /*LoadObjectsBits=*/0b010,
                             /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazardLoad(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                             /*LoadObjectsBits=*/0b1000,
                             /*MemoryAccessCycle=*/{5}));

  // Same objects but different memory cycle should not conflict
  EXPECT_FALSE(HR.hazardLoad(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                             /*LoadObjectsBits=*/0b101,
                             /*MemoryAccessCycle=*/{6}));
}

/// Check that two stores accessing the same object at the same cycle conflict.
TEST(HazardRecognizer, storeStoreObjectConflictHazard) {
  AIE2InstrInfo InstrInfo;
  MockHR HR(InstrInfo);

  // Emit a store with object bits 0b101 at memory cycle 5
  HR.emitStore(1, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
               /*StoreObjectsBits=*/0b101, /*MemoryAccessCycle=*/{5});

  // Another store with overlapping objects at the same cycle should conflict
  EXPECT_TRUE(HR.hazardStore(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                             /*StoreObjectsBits=*/0b001,
                             /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazardStore(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                             /*StoreObjectsBits=*/0b100,
                             /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazardStore(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                             /*StoreObjectsBits=*/0b101,
                             /*MemoryAccessCycle=*/{5}));

  // Non-overlapping objects should not conflict
  EXPECT_FALSE(HR.hazardStore(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                              /*StoreObjectsBits=*/0b010,
                              /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazardStore(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                              /*StoreObjectsBits=*/0b1000,
                              /*MemoryAccessCycle=*/{5}));

  // Same objects but different memory cycle should not conflict
  EXPECT_FALSE(HR.hazardStore(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                              /*StoreObjectsBits=*/0b101,
                              /*MemoryAccessCycle=*/{6}));
}

/// Check that a load and a store accessing the same object at the same cycle
/// do NOT conflict. AIE has separate HW ports for loads and stores.
TEST(HazardRecognizer, loadStoreObjectNoConflict) {
  AIE2InstrInfo InstrInfo;
  MockHR HR(InstrInfo);

  // Emit a load with object bits 0b101 at memory cycle 5
  HR.emitLoad(1, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
              /*LoadObjectsBits=*/0b101, /*MemoryAccessCycle=*/{5});

  // A store with the same objects at the same cycle should NOT conflict
  // because loads and stores use separate HW ports on AIE.
  EXPECT_FALSE(HR.hazardStore(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                              /*StoreObjectsBits=*/0b101,
                              /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazardStore(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                              /*StoreObjectsBits=*/0b001,
                              /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazardStore(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                              /*StoreObjectsBits=*/0b100,
                              /*MemoryAccessCycle=*/{5}));

  // Similarly, emit a store and check that a load does NOT conflict
  MockHR HR2(InstrInfo);
  HR2.emitStore(1, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                /*StoreObjectsBits=*/0b101, /*MemoryAccessCycle=*/{5});

  EXPECT_FALSE(HR2.hazardLoad(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                              /*LoadObjectsBits=*/0b101,
                              /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR2.hazardLoad(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                              /*LoadObjectsBits=*/0b001,
                              /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR2.hazardLoad(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                              /*LoadObjectsBits=*/0b100,
                              /*MemoryAccessCycle=*/{5}));
}

/// Prove that Load and Store bitmaps coexist independently within one cycle.
/// Emit a load to object A and a store to object B, then verify all four
/// probe combinations match only their respective bitmaps.
TEST(HazardRecognizer, loadStoreBitmapsCoexistIndependently) {
  AIE2InstrInfo InstrInfo;
  MockHR HR(InstrInfo);

  // Use different objects for Load vs Store:
  //   Load to object A (0b01), Store to object B (0b10)
  const MemoryObjectsBits ObjA = 0b01;
  const MemoryObjectsBits ObjB = 0b10;

  HR.emitLoadAndStore(1, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                      /*LoadObjectsBits=*/ObjA, /*StoreObjectsBits=*/ObjB,
                      /*MemoryAccessCycle=*/{5});

  // Load probe on A → conflict (Load bitmap matches)
  EXPECT_TRUE(HR.hazardLoad(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                            /*LoadObjectsBits=*/ObjA,
                            /*MemoryAccessCycle=*/{5}));

  // Store probe on B → conflict (Store bitmap matches)
  EXPECT_TRUE(HR.hazardStore(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                             /*StoreObjectsBits=*/ObjB,
                             /*MemoryAccessCycle=*/{5}));

  // Load probe on B → NO conflict (Load bitmap has A, not B)
  EXPECT_FALSE(HR.hazardLoad(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                             /*LoadObjectsBits=*/ObjB,
                             /*MemoryAccessCycle=*/{5}));

  // Store probe on A → NO conflict (Store bitmap has B, not A)
  EXPECT_FALSE(HR.hazardStore(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                              /*StoreObjectsBits=*/ObjA,
                              /*MemoryAccessCycle=*/{5}));
}

/// Verify that an instruction filling both Load and Store bitmaps (e.g.,
/// part-word stores which may have mayLoad() && mayStore()) conflicts
/// with both a later load and a later store to the same object.
TEST(HazardRecognizer, bothBitmapsFilledConflictsBoth) {
  AIE2InstrInfo InstrInfo;
  MockHR HR(InstrInfo);

  // Simulate part-word store: both Load and Store set to same object bits.
  const MemoryObjectsBits SharedObj = 0b101;

  HR.emitLoadAndStore(1, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                      /*LoadObjectsBits=*/SharedObj,
                      /*StoreObjectsBits=*/SharedObj,
                      /*MemoryAccessCycle=*/{5});

  // A later load-only probe to the same object should conflict.
  EXPECT_TRUE(HR.hazardLoad(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                            /*LoadObjectsBits=*/SharedObj,
                            /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazardLoad(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                            /*LoadObjectsBits=*/0b001,
                            /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazardLoad(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                            /*LoadObjectsBits=*/0b100,
                            /*MemoryAccessCycle=*/{5}));

  // A later store-only probe to the same object should also conflict.
  EXPECT_TRUE(HR.hazardStore(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                             /*StoreObjectsBits=*/SharedObj,
                             /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazardStore(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                             /*StoreObjectsBits=*/0b001,
                             /*MemoryAccessCycle=*/{5}));
  EXPECT_TRUE(HR.hazardStore(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                             /*StoreObjectsBits=*/0b100,
                             /*MemoryAccessCycle=*/{5}));

  // Non-overlapping objects should not conflict for either load or store.
  EXPECT_FALSE(HR.hazardLoad(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                             /*LoadObjectsBits=*/0b010,
                             /*MemoryAccessCycle=*/{5}));
  EXPECT_FALSE(HR.hazardStore(3, 0, /*SlotSet=*/0b0, /*MemoryBanks=*/0,
                              /*StoreObjectsBits=*/0b010,
                              /*MemoryAccessCycle=*/{5}));
}

//===----------------------------------------------------------------------===//
// Tests for getMemoryObjectsBits() classification using real MachineInstrs
//===----------------------------------------------------------------------===//

namespace {

std::unique_ptr<TargetMachine> createAIE2TargetMachine() {
  auto TT(Triple::normalize("aie2--"));
  std::string Error;
  const Target *TheTarget = TargetRegistry::lookupTarget(TT, Error);
  if (!TheTarget)
    return nullptr;
  return std::unique_ptr<TargetMachine>(static_cast<TargetMachine *>(
      TheTarget->createTargetMachine(TT, "", "", TargetOptions(), std::nullopt,
                                     std::nullopt, CodeGenOptLevel::Default)));
}

class GetMemoryObjectsBitsTest : public testing::Test {
protected:
  static const char *MIRString;
  LLVMContext Context;
  std::unique_ptr<TargetMachine> TM;
  std::unique_ptr<MachineModuleInfo> MMI;
  std::unique_ptr<MIRParser> Parser;
  std::unique_ptr<Module> M;

  static void SetUpTestCase() {
    LLVMInitializeAIETargetInfo();
    LLVMInitializeAIETarget();
    LLVMInitializeAIETargetMC();
  }

  void SetUp() override {
    TM = createAIE2TargetMachine();
    if (!TM)
      GTEST_SKIP() << "AIE2 target not available";
    std::unique_ptr<MemoryBuffer> MBuffer =
        MemoryBuffer::getMemBuffer(MIRString);
    Parser = createMIRParser(std::move(MBuffer), Context);
    if (!Parser)
      report_fatal_error("null MIRParser");
    M = Parser->parseIRModule();
    if (!M)
      report_fatal_error("parseIRModule failed");
    M->setTargetTriple(TM->getTargetTriple().getTriple());
    M->setDataLayout(TM->createDataLayout());
    MMI = std::make_unique<MachineModuleInfo>(TM.get());
    if (Parser->parseMachineFunctions(*M, *MMI.get()))
      report_fatal_error("parseMachineFunctions failed");
  }

  MachineFunction *getMachineFunction(Module *M, StringRef Name) {
    auto F = M->getFunction(Name);
    if (!F)
      report_fatal_error("null Function");
    auto &MF = MMI->getOrCreateMachineFunction(*F);
    return &MF;
  }
};

// MIR with:
// - a pure load (LDA_S8_ag_idx_imm from %ir.a)
// - a part-word RMW store (ST_S8_ag_idx_imm to %ir.b) - has mayLoad && mayStore
// - a full-word pure store (ST_ag_idx_imm to %ir.c) - only mayStore
const char *GetMemoryObjectsBitsTest::MIRString = R"MIR(
--- |
  define void @test_load_store(ptr %a, ptr %b, ptr %c, i8 %val8, i32 %val32) {
  entry:
    %load_val = load i8, ptr %a, align 1
    store i8 %val8, ptr %b, align 1
    store i32 %val32, ptr %c, align 4
    ret void
  }

...
---
name:            test_load_store
alignment:       16
legalized:       true
regBankSelected: true
selected:        true
tracksRegLiveness: true
body:             |
  bb.0.entry (align 16):
    liveins: $p0, $p1, $p2, $r0, $r1

    renamable $r2 = LDA_S8_ag_idx_imm renamable $p0, 0 :: (load (s8) from %ir.a)
    ST_S8_ag_idx_imm renamable $r0, renamable $p1, 0 :: (store (s8) into %ir.b)
    ST_dms_sts_idx_imm renamable $r1, renamable $p2, 0 :: (store (s32) into %ir.c)
    RET implicit $lr
    DelayedSchedBarrier implicit $r2

...
)MIR";

/// Test that getMemoryObjectsBits() correctly classifies a load instruction:
/// - Load fills only Result.Load (non-zero)
/// - Store remains zero
TEST_F(GetMemoryObjectsBitsTest, LoadInstructionPopulatesOnlyLoadBits) {
  MachineFunction *MF = getMachineFunction(M.get(), "test_load_store");
  const MachineBasicBlock &MBB = MF->front();

  // Find the load instruction (LDA_S8_ag_idx_imm)
  const MachineInstr *LoadMI = nullptr;
  for (const MachineInstr &MI : MBB) {
    if (MI.mayLoad() && !MI.mayStore()) {
      LoadMI = &MI;
      break;
    }
  }
  ASSERT_NE(LoadMI, nullptr) << "Could not find load instruction";
  ASSERT_TRUE(LoadMI->mayLoad());
  ASSERT_FALSE(LoadMI->mayStore());

  // Create a HazardRecognizer with IsPreRA=false (so getMemoryObjectsBits
  // works)
  const auto *TII =
      static_cast<const AIEBaseInstrInfo *>(MF->getSubtarget().getInstrInfo());
  AIEAlternateDescriptors AltDescs;
  AIEHazardRecognizer HR(TII, MF->getSubtarget().getInstrItineraryData(),
                         AltDescs, /*IsPreRA=*/false);

  MemoryObjectPair Result = HR.getMemoryObjectsBits(LoadMI);

  // Load instruction should populate only the Load bitmap.
  EXPECT_NE(Result.Load, 0u) << "Load instruction should set Load bits";
  EXPECT_EQ(Result.Store, 0u) << "Load instruction should NOT set Store bits";
}

/// Test that a part-word RMW store (ST_S8) populates BOTH Load and Store bits.
TEST_F(GetMemoryObjectsBitsTest, PartWordStorePopulatesBothLoadAndStoreBits) {
  MachineFunction *MF = getMachineFunction(M.get(), "test_load_store");
  const MachineBasicBlock &MBB = MF->front();

  // The part-word store is the only op that both loads and stores (RMW).
  const MachineInstr *RMWStoreMI = nullptr;
  for (const MachineInstr &MI : MBB) {
    if (MI.mayLoad() && MI.mayStore()) {
      RMWStoreMI = &MI;
      break;
    }
  }
  ASSERT_NE(RMWStoreMI, nullptr) << "Could not find part-word RMW store";

  const auto *TII =
      static_cast<const AIEBaseInstrInfo *>(MF->getSubtarget().getInstrInfo());
  AIEAlternateDescriptors AltDescs;
  AIEHazardRecognizer HR(TII, MF->getSubtarget().getInstrItineraryData(),
                         AltDescs, /*IsPreRA=*/false);

  MemoryObjectPair Result = HR.getMemoryObjectsBits(RMWStoreMI);

  // A RMW store must populate BOTH bitmaps with the same object.
  EXPECT_NE(Result.Load, 0u) << "RMW store should set Load bits";
  EXPECT_NE(Result.Store, 0u) << "RMW store should set Store bits";
  EXPECT_EQ(Result.Load, Result.Store)
      << "RMW store should use the same object in both bitmaps";
}

/// Test that a full-word pure store (ST_ag_idx_imm) populates only Store bits.
TEST_F(GetMemoryObjectsBitsTest, PureStorePopulatesOnlyStoreBits) {
  MachineFunction *MF = getMachineFunction(M.get(), "test_load_store");
  const MachineBasicBlock &MBB = MF->front();

  // The full-word store is a pure store (stores but does not load).
  const MachineInstr *PureStoreMI = nullptr;
  for (const MachineInstr &MI : MBB) {
    if (MI.mayStore() && !MI.mayLoad()) {
      PureStoreMI = &MI;
      break;
    }
  }
  ASSERT_NE(PureStoreMI, nullptr) << "Could not find pure store instruction";

  const auto *TII =
      static_cast<const AIEBaseInstrInfo *>(MF->getSubtarget().getInstrInfo());
  AIEAlternateDescriptors AltDescs;
  AIEHazardRecognizer HR(TII, MF->getSubtarget().getInstrItineraryData(),
                         AltDescs, /*IsPreRA=*/false);

  MemoryObjectPair Result = HR.getMemoryObjectsBits(PureStoreMI);

  // Pure store should populate only the Store bitmap.
  EXPECT_NE(Result.Store, 0u) << "Pure store should set Store bits";
  EXPECT_EQ(Result.Load, 0u) << "Pure store should NOT set Load bits";
}

/// Test that two instructions accessing different objects get different bits.
/// The LDA_S8 loads from %ir.a, the ST_S8 (which is RMW) loads from %ir.b.
/// Both should have non-zero Load bits, and those bits should differ.
TEST_F(GetMemoryObjectsBitsTest, TwoObjectsGetDifferentLoadBits) {
  MachineFunction *MF = getMachineFunction(M.get(), "test_load_store");
  const MachineBasicBlock &MBB = MF->front();

  // Find the pure load (LDA_S8 from %ir.a) and the RMW store (ST_S8 to %ir.b)
  const MachineInstr *LoadMI = nullptr;
  const MachineInstr *StoreMI = nullptr;
  for (const MachineInstr &MI : MBB) {
    if (MI.mayLoad() && !MI.mayStore() && !LoadMI)
      LoadMI = &MI;
    if (MI.mayStore() && !StoreMI)
      StoreMI = &MI;
  }
  ASSERT_NE(LoadMI, nullptr) << "Could not find load instruction";
  ASSERT_NE(StoreMI, nullptr) << "Could not find store instruction";

  // Create a HazardRecognizer with IsPreRA=false
  const auto *TII =
      static_cast<const AIEBaseInstrInfo *>(MF->getSubtarget().getInstrInfo());
  AIEAlternateDescriptors AltDescs;
  AIEHazardRecognizer HR(TII, MF->getSubtarget().getInstrItineraryData(),
                         AltDescs, /*IsPreRA=*/false);

  MemoryObjectPair LoadResult = HR.getMemoryObjectsBits(LoadMI);
  MemoryObjectPair StoreResult = HR.getMemoryObjectsBits(StoreMI);

  // Both should have Load bits set (LDA is pure load, ST_S8 is RMW).
  EXPECT_NE(LoadResult.Load, 0u) << "LDA should have non-zero Load bits";
  EXPECT_NE(StoreResult.Load, 0u)
      << "ST_S8 (RMW) should have non-zero Load bits";

  // Since they access different objects (%ir.a vs %ir.b), bits should differ.
  EXPECT_NE(LoadResult.Load, StoreResult.Load)
      << "Different objects (%ir.a vs %ir.b) should get different Load bits";
}

} // anonymous namespace
