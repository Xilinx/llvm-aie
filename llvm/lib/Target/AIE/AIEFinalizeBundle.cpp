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
  MachineInstr *MI = &*MII;
  if (MI->isMetaInstruction() || MII->isBundled())
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
      if (!MII->isInsideBundle()) {
        if (isBundleCandidate(MII)) {
          finalizeBundle(MBB, MII, std::next(MII));
          Changed = true;
        }
        ++MII;
      } else {
        MII = finalizeBundle(MBB, std::prev(MII));
        Changed = true;
      }
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
