//===- MachineInstrTest.cpp -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// The point of these tests is to verify how instructions are reordered by the
// MachineScheduler, and how the top() and bottom() iterators are maintained.
// They pay particular attention to debug instructions, as they follow specific
// rules when other instructions move past them.

#include "ScheduleDAGMITestUtils.h"

using namespace llvm;

namespace {

/// Schedule an instruction in Top without needing a move
TEST_F(ScheduleDAGMITest, SchedInTopNoMove) {
  auto *DV0 = appendDebugInstr();
  auto *MI1 = appendPlainInstr();
  auto *DV2 = appendDebugInstr();
  auto *MI3 = appendPlainInstr();

  initializeScheduler();
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 1);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 4);

  Scheduler->scheduleInstr(MI1, /*IsTop=*/true);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, MI1, DV2, MI3}));
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 3);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 4);
}

/// Schedule an instruction in Bot without needing a move
TEST_F(ScheduleDAGMITest, SchedInBotNoMove) {
  auto *DV0 = appendDebugInstr();
  auto *MI1 = appendPlainInstr();
  auto *DV2 = appendDebugInstr();
  auto *MI3 = appendPlainInstr();

  initializeScheduler();
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 1);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 4);

  Scheduler->scheduleInstr(MI3, /*IsTop=*/false);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, MI1, DV2, MI3}));
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 1);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 3);

  // Now schedule MI1, the debug instructions still stay in place.
  Scheduler->scheduleInstr(MI1, /*IsTop=*/false);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, MI1, DV2, MI3}));
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 1);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 1);
}

/// Move an instruction up into the Top zone
TEST_F(ScheduleDAGMITest, MoveInTop) {
  auto *DV0 = appendDebugInstr();
  auto *MI1 = appendPlainInstr();
  auto *DV2 = appendDebugInstr();
  auto *MI3 = appendPlainInstr();

  initializeScheduler();
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 1);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 4);

  Scheduler->scheduleInstr(MI3, /*IsTop=*/true);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, MI3, MI1, DV2}));
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 2);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 4);
}

/// Move an instruction down into the Bot zone
TEST_F(ScheduleDAGMITest, MoveInBot) {
  auto *DV0 = appendDebugInstr();
  auto *MI1 = appendPlainInstr();
  auto *DV2 = appendDebugInstr();
  auto *MI3 = appendPlainInstr();

  initializeScheduler();
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 1);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 4);

  Scheduler->scheduleInstr(MI1, /*IsTop=*/false);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, DV2, MI3, MI1}));
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 2);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 3);
}

/// Schedule an instruction in Bot with the same cycle as bottom()
/// Similar to SchedInBotNoMove but with explicit EmissionCycle.
TEST_F(ScheduleDAGMITest, SchedInBotSameCycle) {
  auto *DV0 = appendDebugInstr();
  auto *MI1 = appendPlainInstr();
  auto *DV2 = appendDebugInstr();
  auto *MI3 = appendPlainInstr();

  initializeScheduler();

  Scheduler->scheduleInstr(MI3, /*IsTop=*/false, /*EmissionCycle=*/2);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, MI1, DV2, MI3}));

  // Now schedule MI1, the debug instructions still stay in place.
  Scheduler->scheduleInstr(MI1, /*IsTop=*/false, /*EmissionCycle=*/2);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, MI1, DV2, MI3}));
}

/// Schedule an instruction in Bot with a greater cycle than bottom()
/// Similar to SchedInBotNoMove but with explicit EmissionCycle.
TEST_F(ScheduleDAGMITest, SchedInBotGreaterCycle) {
  auto *DV0 = appendDebugInstr();
  auto *MI1 = appendPlainInstr();
  auto *DV2 = appendDebugInstr();
  auto *MI3 = appendPlainInstr();

  initializeScheduler();

  Scheduler->scheduleInstr(MI3, /*IsTop=*/false, /*EmissionCycle=*/2);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, MI1, DV2, MI3}));

  // Now schedule MI1, the debug instructions still stay in place.
  Scheduler->scheduleInstr(MI1, /*IsTop=*/false, /*EmissionCycle=*/3);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, MI1, DV2, MI3}));
}

/// Move an instruction (MI1) to the Bot zone, and then move
/// another one (MI3) below because it has a lower EmissionCycle.
TEST_F(ScheduleDAGMITest, MoveBelowBot) {
  auto *DV0 = appendDebugInstr();
  auto *MI1 = appendPlainInstr();
  auto *DV2 = appendDebugInstr();
  auto *MI3 = appendPlainInstr();

  initializeScheduler();
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 1);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 4);

  // Insert MI1 in bottom-up cycle 6
  Scheduler->scheduleInstr(MI1, /*IsTop=*/false, /*EmissionCycle=*/6);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, DV2, MI3, MI1}));
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 2);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 3);

  // Insert MI3 in bottom-up cycle 2. This is less than MI1's cycle so MI3
  // should actually be inserted below MI1. top() and bottom() should now
  // both point to MI1.
  Scheduler->scheduleInstr(MI3, /*IsTop=*/false, /*EmissionCycle=*/2);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, DV2, MI1, MI3}));
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 2);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 2);
}

/// Schedule an instruction (MI3) which is already at bottom() in the Bot zone.
/// This then maintains the position of DV4.
/// Then move MI1 below because it has a lower EmissionCycle.
TEST_F(ScheduleDAGMITest, MoveInBotBelowDbg) {
  auto *DV0 = appendDebugInstr();
  auto *MI1 = appendPlainInstr();
  auto *DV2 = appendDebugInstr();
  auto *MI3 = appendPlainInstr();
  auto *DV4 = appendDebugInstr();

  initializeScheduler();
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 1);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 5);

  // Insert MI3 in bottom-up cycle 6
  Scheduler->scheduleInstr(MI3, /*IsTop=*/false, /*EmissionCycle=*/6);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, MI1, DV2, MI3, DV4}));
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 1);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 3);

  // Insert MI1 in bottom-up cycle 2. This is less than MI3's cycle so MI1
  // should actually be inserted below.
  // Make sure it's also inserted below DV4.
  Scheduler->scheduleInstr(MI1, /*IsTop=*/false, /*EmissionCycle=*/2);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, DV2, MI3, DV4, MI1}));
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 2);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 2);
}

/// Move an instruction to the Bot zone between two already
/// scheduled instructions
TEST_F(ScheduleDAGMITest, MoveInBopInBetween) {
  auto *DV0 = appendDebugInstr();
  auto *MI1 = appendPlainInstr();
  auto *DV2 = appendDebugInstr();
  auto *MI3 = appendPlainInstr();
  auto *DV4 = appendDebugInstr();
  auto *MI5 = appendPlainInstr();

  initializeScheduler();
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 1);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 6);

  // Insert MI5 in bottom-up cycle 1
  Scheduler->scheduleInstr(MI5, /*IsTop=*/false, /*EmissionCycle=*/1);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, MI1, DV2, MI3, DV4, MI5}));
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 1);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 5);

  // Insert MI3 in bottom-up cycle 6
  Scheduler->scheduleInstr(MI3, /*IsTop=*/false, /*EmissionCycle=*/6);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, MI1, DV2, MI3, DV4, MI5}));
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 1);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 3);

  // Insert MI1 in bottom-up cycle 2. It should end up between MI3 and MI5
  Scheduler->scheduleInstr(MI1, /*IsTop=*/false, /*EmissionCycle=*/2);
  EXPECT_EQ(MISeq(*MBB), MISeq({DV0, DV2, MI3, DV4, MI1, MI5}));
  EXPECT_POS_IN_BB(Scheduler->top(), MBB, 2);
  EXPECT_POS_IN_BB(Scheduler->bottom(), MBB, 2);
}

} // end namespace
