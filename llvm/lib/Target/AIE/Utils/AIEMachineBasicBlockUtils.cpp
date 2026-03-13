//===- AIEMachineBasicBlockUtils.cpp -----------------------------*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
#include "AIEMachineBasicBlockUtils.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBundle.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"

namespace llvm::AIEMachineBasicBlockUtils {

bool isBlockOnlyReachableByFallthrough(const MachineBasicBlock *MBB) {
  if (!MBB)
    return false;

  // If this is a landing pad, it isn't a fall through.
  if (MBB->isEHPad())
    return false;

  // If there isn't exactly one predecessor, it can't be a fall through.
  if (MBB->pred_size() != 1)
    return false;

  // The predecessor has to be immediately before this block.
  MachineBasicBlock *Pred = *MBB->pred_begin();
  if (!Pred->isLayoutSuccessor(MBB))
    return false;

  // If the block is completely empty, the Pred will be optimized away,
  // therefore continue checking for fallthrough check.
  if (Pred->empty())
    return isBlockOnlyReachableByFallthrough(Pred);

  // On AIE, branches are not marked as terminators, as there will be regular
  // instructions scheduled in the branch delay slots. Thus, we need to check
  // whether a basic block is part of a jump table and conservatively not
  // consider it to be only reachable as a fall-through.
  auto JumpsToBB = [MBB](const MachineJumpTableEntry &JTE) {
    return is_contained(JTE.MBBs, MBB);
  };
  const MachineJumpTableInfo *JTI = MBB->getParent()->getJumpTableInfo();
  if (JTI && any_of(JTI->getJumpTables(), JumpsToBB)) {
    return false;
  }

  // Check the terminators in the previous blocks
  // NOTE: for targets that use delay slot filling (all except AIE1),
  // branch instructions are not terminators, thus the code below is dead.
  for (const auto &MI : Pred->terminators()) {
    // If it is not a simple branch, we are in a table somewhere.
    if (!MI.isBranch() || MI.isIndirectBranch())
      return false;

    // If we are the operands of one of the branches, this is not a fall
    // through. Note that targets with delay slots will usually bundle
    // terminators with the delay slot instruction.
    for (ConstMIBundleOperands OP(MI); OP.isValid(); ++OP) {
      if (OP->isJTI())
        return false;
      if (OP->isMBB() && OP->getMBB() == MBB)
        return false;
    }
  }

  return true;
}

MachineBasicBlock *getPrevNonEmptyMBB(MachineBasicBlock *MBB) {
  if (!MBB)
    return nullptr;

  while ((MBB = MBB->getPrevNode())) {
    if (MBB->empty())
      continue;

    return MBB;
  }
  return nullptr;
}

} // namespace llvm::AIEMachineBasicBlockUtils
