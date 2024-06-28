//===- AIEFinalizeBundle.cpp -----------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEFinalizeBundle.h"
#include "AIE.h"
#include "AIEBundle.h"
#include "llvm/CodeGen/MachineInstrBundle.h"

using namespace llvm;

#define DEBUG_TYPE "aie-finalize-mi-bundles"

namespace {

bool isBundleCandidate(MachineBasicBlock::instr_iterator MII) {
  const MachineBasicBlock *MBB = MII->getParent();
  auto *MF = MBB->getParent();
  auto &Subtarget = MF->getSubtarget();
  const AIEBaseInstrInfo *TII =
      static_cast<const AIEBaseInstrInfo *>(Subtarget.getInstrInfo());
  MachineInstr *MI = &*MII;
  // LoopStart should be lowered before reaching here
  assert(!TII->isHardwareLoopStart(MI->getOpcode()) &&
         "LoopStart is not lowered \n\r");
  if (MI->isMetaInstruction() || MII->isBundled() ||
      TII->isHardwareLoopEnd(MI->getOpcode()))
    return false;
  return true;
}

} // namespace

bool AIEFinalizeBundle::runOnMachineFunction(MachineFunction &MF) {
  bool Changed = false;
  for (MachineBasicBlock &MBB : MF) {
    MachineBasicBlock::instr_iterator MII = MBB.instr_begin();
    MachineBasicBlock::instr_iterator MIE = MBB.instr_end();
    if (MII == MIE)
      continue;
    assert(!MII->isInsideBundle() && "First instr cannot be inside bundle!");

    while (MII != MIE) {
      // Check if MI is a standalone instruction
      if (!MII->isInsideBundle() && isBundleCandidate(MII)) {
        finalizeBundle(MBB, MII, std::next(MII));
        Changed = true;
      }
      ++MII;
    }
  }

  return Changed;
}

INITIALIZE_PASS_BEGIN(AIEFinalizeBundle, DEBUG_TYPE, "AIE Bundle Finalization",
                      false, false)
INITIALIZE_PASS_END(AIEFinalizeBundle, DEBUG_TYPE, "AIE Bundle Finalization",
                    false, false)

char AIEFinalizeBundle::ID = 0;
llvm::FunctionPass *llvm::createAIEFinalizeBundle() {
  return new AIEFinalizeBundle();
}
