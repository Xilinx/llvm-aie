//===- AIEMachineBlockPlacement.cpp -----------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEMachineBlockPlacement.h"
#include "AIE.h"
#include "llvm/CodeGen/MachineInstrBundle.h"

using namespace llvm;

#define DEBUG_TYPE "aie-block-placement"

namespace {

/// isBlockOnlyReachableByFallthough - Return true if the basic block has
/// exactly one predecessor and the control transfer mechanism between
/// the predecessor and this block is a fall-through.
/// Note: code copied from AmsPrinter::isBlockOnlyReachableByFallthrough
bool isBlockOnlyReachableByFallthrough(const MachineBasicBlock *MBB) {
  // If this is a landing pad, it isn't a fall through.  If it has no preds,
  // then nothing falls through to it.
  if (MBB->isEHPad() || MBB->pred_empty())
    return false;

  // If there isn't exactly one predecessor, it can't be a fall through.
  if (MBB->pred_size() > 1)
    return false;

  // The predecessor has to be immediately before this block.
  MachineBasicBlock *Pred = *MBB->pred_begin();
  if (!Pred->isLayoutSuccessor(MBB))
    return false;

  // If the block is completely empty, then it definitely does fall through.
  if (Pred->empty())
    return true;

  // Check the terminators in the previous blocks
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
} // namespace
bool AIEMachineBlockPlacement::runOnMachineFunction(MachineFunction &MF) {
  // This is replacing the only net effect of branch relaxation, which
  // was removed as it isn't necessary.
  // Renumbering looks good in the assembly and it salvages some of the
  // existing references
  MF.RenumberBlocks();

  // All branch targets in AIE need to be 16-byte aligned.  We force
  // all basic blocks to be so aligned in order to meet this requirement.
  // We check whether a block is targeted by borrowing some AsmPrinter logic
  int DefaultAlignment = 4;
  for (MachineBasicBlock &MBB : MF)
    if (!isBlockOnlyReachableByFallthrough(&MBB))
      MBB.setAlignment(Align(1ULL << DefaultAlignment));
  return true;
}

INITIALIZE_PASS_BEGIN(AIEMachineBlockPlacement, DEBUG_TYPE,
                      "AIE Machine Block Placement", false, false)
INITIALIZE_PASS_END(AIEMachineBlockPlacement, DEBUG_TYPE,
                    "AIE Machine Block Placement", false, false)

char AIEMachineBlockPlacement::ID = 0;
llvm::FunctionPass *llvm::createAIEMachineBlockPlacement() {
  return new AIEMachineBlockPlacement();
}
