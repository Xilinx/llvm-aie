//===-- AIEPseudoBranchExpansion.cpp - Expand control-flow instructions ---===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEBaseInstrInfo.h"
#include "AIETiedRegOperands.h"

#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "aie-pseudobranches"

namespace {

class AIEPseudoBranchExpansion : public MachineFunctionPass {

public:
  static char ID;
  AIEPseudoBranchExpansion() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &Fn) override;

private:
  void replacePseudoBranch(
      MachineInstr &MI,
      const AIEBaseInstrInfo::PseudoBranchExpandInfo &ExpandInfo,
      const AIEBaseInstrInfo &TII);
};

bool AIEPseudoBranchExpansion::runOnMachineFunction(MachineFunction &MF) {
  auto &TII =
      *static_cast<const AIEBaseInstrInfo *>(MF.getSubtarget().getInstrInfo());

  bool MadeChange = false;
  for (MachineBasicBlock &MBB : MF) {
    for (MachineInstr &MI : MBB) {
      if (const auto &ExpandInfo = TII.getPseudoBranchExpandInfo(MI)) {
        replacePseudoBranch(MI, *ExpandInfo, TII);
        MadeChange = true;
      }
    }
  }

  return MadeChange;
}

void AIEPseudoBranchExpansion::replacePseudoBranch(
    MachineInstr &MI,
    const AIEBaseInstrInfo::PseudoBranchExpandInfo &ExpandInfo,
    const AIEBaseInstrInfo &TII) {

  // For calls and returns, implicit operands should be moved to the new
  // ExpandInfo.BarrierOpCode instruction because we want dependencies
  // to be created with the latter.
  bool MoveImplicitOps = MI.isCall() || MI.isReturn();
  const MCInstrDesc &PseudoDesc = MI.getDesc();

  MI.setDesc(TII.get(ExpandInfo.TargetInstrOpCode));
  MachineInstr &BarrierMI =
      *BuildMI(*MI.getParent(), ++MI.getIterator(), MI.getDebugLoc(),
               TII.get(ExpandInfo.BarrierOpCode))
           .getInstr();

  if (!MoveImplicitOps)
    return;

  // Move "fake" implicit operands, i.e. those added pre-RA. For example,
  // during call lowering.
  for (unsigned OpIdx = 0; OpIdx < MI.getNumOperands();) {
    MachineOperand &MO = MI.getOperand(OpIdx);
    bool IsFakeImplicitOp = MO.isReg() && MO.isImplicit() &&
                            !PseudoDesc.hasImplicitDefOfPhysReg(MO.getReg()) &&
                            !PseudoDesc.hasImplicitUseOfPhysReg(MO.getReg());
    if (IsFakeImplicitOp || MO.isRegMask()) {
      BarrierMI.addOperand(MO);
      MI.removeOperand(OpIdx);
    } else {
      ++OpIdx;
    }
  }
}

} // end anonymous namespace

char AIEPseudoBranchExpansion::ID = 0;

INITIALIZE_PASS(AIEPseudoBranchExpansion, DEBUG_TYPE,
                "AIE pseudo branch expansion", false, false)

llvm::FunctionPass *llvm::createAIEPseudoBranchExpansion() {
  return new AIEPseudoBranchExpansion();
}
