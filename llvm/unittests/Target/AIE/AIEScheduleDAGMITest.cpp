//===- AIEScheduleDAGMITest.cpp -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEHazardRecognizer.h"
#include "AIEMachineScheduler.h"
#include "AIETestTarget.h"
#include "ScheduleDAGMITestUtils.h"

using namespace llvm;

namespace llvm::AIE {

void PrintTo(const MachineBundle &B, std::ostream *OS) {
  // Re-use MISeq to print something useful from MIs with dummy descriptors.
  *OS << "Bundle{ " << MISeq(B.Instrs) << " }";
}

} // namespace llvm::AIE

namespace {

class DummyAIEHazardRecognizer : public AIEHazardRecognizer {
  AIEAlternateDescriptors AlternateDescriptors;

public:
  DummyAIEHazardRecognizer(const ScheduleDAG *DAG)
      : AIEHazardRecognizer(nullptr, nullptr, AlternateDescriptors, DAG,
                            /*ScoreboardDepth=*/0) {}

  ~DummyAIEHazardRecognizer() override {}

  HazardType getHazardType(SUnit *SU, int DeltaCycles) override {
    return NoHazard;
  }
};

/// A simple wrapper around ScheduleDAGMITest that initializes a scheduler
/// and a scheduling zone (SchedBoundary) with a DummyAIEHazardRecognizer.
class AIEScheduleDAGMITest : public ScheduleDAGMITest {
public:
  AIEScheduleDAGMITest()
      : ScheduleDAGMITest(AIE::createAIETestTargetMachine()) {}

protected:
  void initializeScheduler(bool IsPreRA = false) override {
    ScheduleDAGMITest::initializeScheduler(IsPreRA);
    Scheduler->getSchedZone().HazardRec =
        new DummyAIEHazardRecognizer(Scheduler.get());
  }
};

/// Case where all instructions are scheduled in Bot.getCurrCycle()
TEST_F(AIEScheduleDAGMITest, SchedNoDelta) {
  auto *MI0 = appendPlainInstr();
  auto *MI1 = appendPlainInstr();
  auto *MI2 = appendPlainInstr();

  initializeScheduler();
  SchedBoundary &Bot = Scheduler->getSchedZone();

  // Mark all instructions as available.
  for (auto *MI : {MI0, MI1, MI2})
    Bot.releaseNode(Scheduler->getSUnit(MI), /*ReadyCycle=*/0, false);

  Scheduler->scheduleInstr(MI2, Bot);
  Bot.bumpCycle(2);
  Scheduler->scheduleInstr(MI0, Bot);
  Bot.bumpCycle(3);
  Scheduler->scheduleInstr(MI1, Bot);

  EXPECT_EQ(MISeq(*MBB), MISeq({MI1, MI0, MI2}));
}

/// Case where instructions are scheduled with a delta from Bot.getCurrCycle().
TEST_F(AIEScheduleDAGMITest, SchedWithDelta) {
  auto *MI0 = appendPlainInstr();
  auto *MI1 = appendPlainInstr();
  auto *MI2 = appendPlainInstr();

  initializeScheduler();
  SchedBoundary &Bot = Scheduler->getSchedZone();

  // Mark all instructions as available.
  for (auto *MI : {MI0, MI1, MI2})
    Bot.releaseNode(Scheduler->getSUnit(MI), /*ReadyCycle=*/0, false);

  Scheduler->scheduleInstr(MI2, Bot, -8); // Emit in cycle 0+8
  Bot.bumpCycle(2);
  Scheduler->scheduleInstr(MI1, Bot, -5); // Emit in cycle 2+5
  Bot.bumpCycle(3);
  Scheduler->scheduleInstr(MI0, Bot); // Emit in cycle 3

  EXPECT_EQ(MISeq(*MBB), MISeq({MI2, MI1, MI0}));
}

/// Verify the behavior of computeAndFinalizeBundles. In pre-RA scheduling,
/// it should also support the case where the SU's ReadyCycle is out-of-date
/// due to it being moved by GenericScheduler::reschedulePhysReg().
TEST_F(AIEScheduleDAGMITest, SchedThenMove) {
  auto *MI0 = appendPlainInstr();
  auto *MI1 = appendPlainInstr();
  auto *MI2 = appendPlainInstr();

  initializeScheduler(/*IsPreRA=*/true);
  SchedBoundary &Bot = Scheduler->getSchedZone();

  // Mark all instructions as available.
  for (auto *MI : {MI0, MI1, MI2})
    Bot.releaseNode(Scheduler->getSUnit(MI), /*ReadyCycle=*/0, false);

  Scheduler->scheduleInstr(MI2, Bot);
  Bot.bumpCycle(2);
  Scheduler->scheduleInstr(MI1, Bot);
  Bot.bumpCycle(3);
  Scheduler->scheduleInstr(MI0, Bot);

  EXPECT_EQ(MISeq(*MBB), MISeq({MI0, MI1, MI2}));

  // Make sure the bundles are computed as expected, c1 should be empty.
  const AIEBaseMCFormats *Fmts = nullptr;
  std::vector<AIE::MachineBundle> ExpectedBundles = {
      AIE::MachineBundle({MI0}, Fmts),  // c3
      AIE::MachineBundle({MI1}, Fmts),  // c2
      AIE::MachineBundle({}, Fmts),     // c1
      AIE::MachineBundle({MI2}, Fmts)}; // c0
  EXPECT_EQ(AIE::computeAndFinalizeBundles(Bot), ExpectedBundles);

  // Now move an instruction to simulate GenericScheduler::reschedulePhysReg.
  // This causes the SU's ReadyCycle to be out-of-sync with its position in MBB.
  Scheduler->moveInstruction(MI2, MI1);
  ExpectedBundles = {AIE::MachineBundle({MI0}, Fmts),      // c3
                     AIE::MachineBundle({MI2, MI1}, Fmts), // c2
                     AIE::MachineBundle({}, Fmts),         // c1
                     AIE::MachineBundle({}, Fmts)};        // c0
  EXPECT_EQ(Scheduler->getSUnit(MI2)->BotReadyCycle, 0U);
  EXPECT_EQ(AIE::computeAndFinalizeBundles(Bot), ExpectedBundles);
}

} // end namespace
