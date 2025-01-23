//===- AIE1MachineBlockPlacement.cpp ----------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE1MachineBlockPlacement.h"
#include "AIE.h"
#include "Utils/AIEMachineBasicBlockUtils.h"
#include "llvm/CodeGen/MachineInstrBundle.h"

using namespace llvm;

#define DEBUG_TYPE "aie-block-placement"

bool AIE1MachineBlockPlacement::runOnMachineFunction(MachineFunction &MF) {
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
    if (!AIEMachineBasicBlockUtils::isBlockOnlyReachableByFallthrough(&MBB))
      MBB.setAlignment(Align(1ULL << DefaultAlignment));
  return true;
}

INITIALIZE_PASS_BEGIN(AIE1MachineBlockPlacement, DEBUG_TYPE,
                      "AIE Machine Block Placement", false, false)
INITIALIZE_PASS_END(AIE1MachineBlockPlacement, DEBUG_TYPE,
                    "AIE Machine Block Placement", false, false)

char AIE1MachineBlockPlacement::ID = 0;
llvm::FunctionPass *llvm::createAIE1MachineBlockPlacement() {
  return new AIE1MachineBlockPlacement();
}
