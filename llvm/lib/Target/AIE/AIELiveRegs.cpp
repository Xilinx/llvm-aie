//===- AIELiveRegs.cpp - Liveness Analysis infrastructure -----------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// Implementations of the classes used to support Liveness Analysis of all
// physical registers, including reserved registers.
//===----------------------------------------------------------------------===//

#include "AIELiveRegs.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

using namespace llvm;

namespace llvm::AIE {

void LiveRegs::addToWorkList(const MachineBasicBlock *MBB) {
  auto It = InWorkList.find(MBB);
  if (It == InWorkList.end() || !It->second) {
    WorkList.push(MBB);
    It->second = true;
  }
}

const MachineBasicBlock *LiveRegs::popFromWorkList() {
  const MachineBasicBlock *MBB = WorkList.front();
  WorkList.pop();
  InWorkList[MBB] = false;
  return MBB;
}

void LiveRegs::computeBlockLiveIns(const MachineBasicBlock *MBB,
                                   LivePhysRegs &CurrentLive) {
  CurrentLive.init(*TRI);

  // Calculates CurrentLive by iterating over each successor of the current
  // MBB and adding each successor's LiveIns to CurrentLive.
  // This ensures CurrentLive is the union of LiveIns of all successor MBBs.
  for (const MachineBasicBlock *Succ : MBB->successors()) {
    for (MCPhysReg Reg : LiveIns[Succ]) {
      CurrentLive.addReg(Reg);
    }
  }
  // Calculates CurrentLive by bottom-up traversal of the MBB
  for (const MachineInstr &MI : llvm::reverse(*MBB)) {
    CurrentLive.stepBackward(MI);
  }
}

bool LiveRegs::equal(LivePhysRegs &CurrentLive, LivePhysRegs &OldLive) {
  auto CurrentLiveIt = CurrentLive.begin();
  for (const MCPhysReg Reg : OldLive) {
    if (!CurrentLive.contains(Reg)) {
      return false;
    }
    if (CurrentLiveIt == CurrentLive.end())
      return false;
    ++CurrentLiveIt;
  }

  return CurrentLiveIt == CurrentLive.end() ? true : false;
}

void LiveRegs::updateLiveRegs(LivePhysRegs &CurrentLive,
                              LivePhysRegs &OldLive) {
  OldLive.init(*TRI);
  for (const MCPhysReg Reg : CurrentLive) {
    OldLive.addReg(Reg);
  }
}

const std::map<const MachineBasicBlock *, LivePhysRegs> &
LiveRegs::getLiveIns() const {
  return LiveIns;
}

LiveRegs::LiveRegs(const MachineFunction *MF) {
  const TargetRegisterInfo *RegisterInfo = MF->getSubtarget().getRegisterInfo();
  TRI = RegisterInfo;

  // Using post_order optimizes by minimizing re-runs, though it's not required
  // for correctness. Post-order ensures that we process the basic blocks in a
  // way that naturally accommodates dependencies and minimizes redundant work.
  for (const MachineBasicBlock *MBB : post_order(MF)) {
    LiveIns[MBB].init(*TRI);
    WorkList.push(MBB);
    InWorkList[MBB] = true;
  }
  while (!WorkList.empty()) {
    const MachineBasicBlock *MBB = popFromWorkList();
    LivePhysRegs CurrentLive;
    computeBlockLiveIns(MBB, CurrentLive);

    // If MBB's liveins changed, force a recomputation for MBB's preds
    if (equal(CurrentLive, LiveIns[MBB]))
      continue;

    updateLiveRegs(CurrentLive, LiveIns[MBB]);
    for (const MachineBasicBlock *Pred : MBB->predecessors())
      addToWorkList(Pred);
  }
}

} // namespace llvm::AIE
