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

} // end namespace
