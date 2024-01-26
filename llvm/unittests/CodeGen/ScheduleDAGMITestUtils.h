//===- ScheduleDAGMITestUtils.h -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/IR/Module.h"
#include "gtest/gtest.h"

using namespace llvm;

namespace llvm {

class DummyScheduleDAGMI : public ScheduleDAGMI {
public:
  using ScheduleDAGMI::ScheduleDAGMI;
  ~DummyScheduleDAGMI() override;

  /// Initialize enough stuff in a similar manner to ScheduleDAGMI::schedule()
  /// so one can do "manual" scheduling.
  void prepareForBB(MachineBasicBlock *MBB);

  /// Move \p MI to the Top or Bot Zone
  void scheduleInstr(MachineInstr *MI, bool IsTop,
                     std::optional<unsigned> EmissionCycle = std::nullopt);
};

class ScheduleDAGMITest : public testing::Test {
protected:
  ScheduleDAGMITest();

  /// Initialize a DummyScheduleDAGMI so it is ready to schedule instructions
  /// in \p MBB
  void initializeScheduler();

  /// Create a dummy instruction for which MachineInstr::isDebugValue() is true
  /// It is pushed at the end of \p MBB
  MachineInstr *appendDebugInstr();

  /// Create a dummy instruction for which MachineInstr::isDebugValue() is false
  /// It is pushed at the end of \p MBB
  MachineInstr *appendPlainInstr();

  LLVMContext Ctx;
  Module Mod;
  std::unique_ptr<MachineFunction> MF;
  MachineBasicBlock *MBB = nullptr;
  unsigned NumMIs = 0;

  MachineSchedContext SchedCtx;
  std::unique_ptr<DummyScheduleDAGMI> Scheduler;
};

/// Wrapper to compare iterators that belong to MBB. This will show a useful
/// message in case of inequality.
#define EXPECT_POS_IN_BB(It, MBB, Pos)                                         \
  EXPECT_EQ(std::distance(MBB->begin(), It), Pos) << #It " != " #MBB "+" << Pos;

/// Wrapper around a list of MachineInstr to allow printing of dummy
/// instructions without actual name or valid MCDesc.
struct MISeq {
  std::vector<const MachineInstr *> Seq;
  MISeq(std::initializer_list<const MachineInstr *> L) : Seq(L) {}
  MISeq(const MachineBasicBlock &MBB) {
    for (const MachineInstr &MI : MBB)
      Seq.push_back(&MI);
  }
};

std::ostream &operator<<(std::ostream &OS, const MISeq &S);
bool operator==(const MISeq &S1, const MISeq &S2);

} // end namespace llvm
