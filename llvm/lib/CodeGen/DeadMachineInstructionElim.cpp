//===- DeadMachineInstructionElim.cpp - Remove dead machine instructions --===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its
// affiliates
//
//===----------------------------------------------------------------------===//
//
// This is an extremely simple MachineInstr-level dead-code-elimination pass.
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/DeadMachineInstructionElim.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/LiveRegUnits.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "dead-mi-elimination"

STATISTIC(NumDeletes,          "Number of dead instructions deleted");

namespace {
class DeadMachineInstructionElimImpl {
  const MachineRegisterInfo *MRI = nullptr;
  const TargetInstrInfo *TII = nullptr;
  LiveRegUnits LivePhysRegs;
  bool KeepLifetimeInstructions;

public:
  bool runImpl(MachineFunction &MF, bool KeepLifetimeInstructions = false);

private:
  bool eliminateDeadMI(MachineFunction &MF);
};

class DeadMachineInstructionElim : public MachineFunctionPass {
public:
  static char ID; // Pass identification, replacement for typeid
  bool KeepLifetimeInstructions;

  DeadMachineInstructionElim(bool KeepLifetimeInstructions = false)
      : MachineFunctionPass(ID),
        KeepLifetimeInstructions(KeepLifetimeInstructions) {
    initializeDeadMachineInstructionElimPass(*PassRegistry::getPassRegistry());
  }

  bool runOnMachineFunction(MachineFunction &MF) override {
    if (skipFunction(MF.getFunction()))
      return false;
    return DeadMachineInstructionElimImpl().runImpl(MF,
                                                    KeepLifetimeInstructions);
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    MachineFunctionPass::getAnalysisUsage(AU);
  }
};
} // namespace

PreservedAnalyses
DeadMachineInstructionElimPass::run(MachineFunction &MF,
                                    MachineFunctionAnalysisManager &) {
  if (!DeadMachineInstructionElimImpl().runImpl(MF))
    return PreservedAnalyses::all();
  PreservedAnalyses PA = getMachineFunctionPassPreservedAnalyses();
  PA.preserveSet<CFGAnalyses>();
  return PA;
}

char DeadMachineInstructionElim::ID = 0;
char &llvm::DeadMachineInstructionElimID = DeadMachineInstructionElim::ID;

INITIALIZE_PASS(DeadMachineInstructionElim, DEBUG_TYPE,
                "Remove dead machine instructions", false, false)

bool DeadMachineInstructionElimImpl::runImpl(MachineFunction &MF,
                                             bool LifetimeInstructions) {
  MRI = &MF.getRegInfo();
  const TargetSubtargetInfo &ST = MF.getSubtarget();
  TII = ST.getInstrInfo();
  LivePhysRegs.init(*ST.getRegisterInfo());
  KeepLifetimeInstructions = LifetimeInstructions;

  bool AnyChanges = eliminateDeadMI(MF);
  while (AnyChanges && eliminateDeadMI(MF))
    ;
  return AnyChanges;
}

SmallVector<MCPhysReg, 16>
getSimplifiableReservedRegs(const MachineRegisterInfo *MRI) {
  BitVector ReservedRegs = MRI->getReservedRegs();
  SmallVector<MCPhysReg, 16> SimplifiableReservedRegs;
  for (MCPhysReg PhysReg : ReservedRegs.set_bits()) {
    if (MRI->canSimplifyPhysReg(PhysReg)) {
      SimplifiableReservedRegs.push_back(PhysReg);
    }
  }
  return SimplifiableReservedRegs;
}

bool DeadMachineInstructionElimImpl::eliminateDeadMI(MachineFunction &MF) {
  bool AnyChanges = false;

  SmallVector<MCPhysReg, 16> SimplifiableReservedRegs =
      getSimplifiableReservedRegs(MRI);

  // Loop over all instructions in all blocks, from bottom to top, so that it's
  // more likely that chains of dependent but ultimately dead instructions will
  // be cleaned up.
  for (MachineBasicBlock *MBB : post_order(&MF)) {
    LivePhysRegs.addLiveOuts(*MBB);

    // Reserved registers are considered always live, so consider them as
    // live-outs for MBB. Inside MBB, dead assignments can still be detected.
    for (MCPhysReg PhysReg : SimplifiableReservedRegs) {
      LivePhysRegs.addReg(PhysReg);
    }

    // Now scan the instructions and delete dead ones, tracking physreg
    // liveness as we go.
    for (MachineInstr &MI : make_early_inc_range(reverse(*MBB))) {
      // If the instruction is dead, delete it!
      if (MI.isDead(*MRI, &LivePhysRegs)) {
        LLVM_DEBUG(dbgs() << "DeadMachineInstructionElim: DELETING: " << MI);
        // It is possible that some DBG_VALUE instructions refer to this
        // instruction. They will be deleted in the live debug variable
        // analysis.
        MI.eraseFromParent();
        AnyChanges = true;
        ++NumDeletes;
        continue;
      }
      LivePhysRegs.stepBackward(MI);

      // If the instruction is a call, conservatively assume that it reads
      // reserved registers.
      if (MI.isCall()) {
        for (MCPhysReg PhysReg : SimplifiableReservedRegs) {
          LivePhysRegs.addReg(PhysReg);
        }
      }
    }
  }
  LivePhysRegs.clear();
  return AnyChanges;
}

namespace llvm {
MachineFunctionPass *
createDeadMachineInstructionElim(bool KeepLifetimeInstructions = false) {
  return new DeadMachineInstructionElim(KeepLifetimeInstructions);
}
} // end namespace llvm
