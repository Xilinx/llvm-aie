//===- ScheduleDAGMITestUtils.cpp -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "ScheduleDAGMITestUtils.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

namespace {

// Include helper functions to ease the manipulation of MachineFunctions.
#include "MFCommon.inc"

MCInstrDesc MCIDs[] = {
    {TargetOpcode::DBG_VALUE, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {TargetOpcode::GENERIC_OP_END + 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

} // namespace

DummyScheduleDAGMI::~DummyScheduleDAGMI() {
  if (BB) {
    exitRegion();
    finishBlock();
  }
}

void DummyScheduleDAGMI::prepareForBB(MachineBasicBlock *MBB) {
  startBlock(MBB);
  enterRegion(MBB, MBB->begin(), MBB->end(),
              std::distance(MBB->instr_begin(), MBB->instr_end()));
  buildSchedGraph(/*AA=*/nullptr);

  SchedImpl->initialize(this);
  SmallVector<SUnit *, 8> TopRoots, BotRoots;
  initQueues(TopRoots, BotRoots);
}

void DummyScheduleDAGMI::scheduleInstr(MachineInstr *MI, bool IsTop,
                                       std::optional<unsigned> EmissionCycle) {
  SUnit *SU = getSUnit(MI);
  movePickedSU(*SU, IsTop, EmissionCycle);
}

ScheduleDAGMITest::ScheduleDAGMITest() : Mod("Module", Ctx) {
  MF = createMachineFunction(Ctx, Mod);
  MBB = MF->CreateMachineBasicBlock();
}

void ScheduleDAGMITest::initializeScheduler() {
  SchedCtx.MF = MF.get();
  Scheduler = std::make_unique<DummyScheduleDAGMI>(
      &SchedCtx, std::make_unique<PostGenericScheduler>(&SchedCtx),
      /*RemoveKillFlags=*/true);
  Scheduler->prepareForBB(MBB);
}

MachineInstr *ScheduleDAGMITest::appendDebugInstr() {
  MachineInstr *MI = MF->CreateMachineInstr(MCIDs[0], DebugLoc());
  MI->addOperand(*MF, MachineOperand::CreateImm(NumMIs++));
  MBB->push_back(MI);
  return MI;
}

MachineInstr *ScheduleDAGMITest::appendPlainInstr() {
  MachineInstr *MI = MF->CreateMachineInstr(MCIDs[1], DebugLoc());
  MI->addOperand(*MF, MachineOperand::CreateImm(NumMIs++));
  MBB->push_back(MI);
  return MI;
}

std::ostream &llvm::operator<<(std::ostream &OS, const MISeq &S) {
  for (const MachineInstr *MI : S.Seq) {
    assert(MI && MI->getNumOperands() == 1);
    if (MI != S.Seq[0])
      OS << " - ";
    if (MI->isDebugValue())
      OS << "Debug ";
    else
      OS << "Instr ";
    OS << MI->getOperand(0).getImm();
  }
  return OS;
}

bool llvm::operator==(const MISeq &S1, const MISeq &S2) {
  return S1.Seq == S2.Seq;
}
