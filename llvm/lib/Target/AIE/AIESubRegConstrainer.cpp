//===----- AIESubRegConstrainer.cpp - Constrain tied sub-registers --------===//
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

#define DEBUG_TYPE "aie-subregs"

namespace {

class AIESubRegConstrainer : public MachineFunctionPass {

public:
  static char ID;
  AIESubRegConstrainer() : MachineFunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &Fn) override;

private:
  void replaceRegOperands(Register OldReg, Register NewReg, unsigned NewSubReg,
                          MachineRegisterInfo &MRI);
  void processTiedOperands(const TiedRegOperands &Regs, MachineInstr &MI);
};

bool AIESubRegConstrainer::runOnMachineFunction(MachineFunction &MF) {
  LLVM_DEBUG(llvm::dbgs() << "Constraining sub-registers: " << MF.getName()
                          << "\n");

  // We are generating copies for tied operands. The VReg for those copies
  // will be used both as input and output of the instruction with tied
  // operands. This breaks SSA.
  MF.getRegInfo().leaveSSA();

  auto &TII =
      *static_cast<const AIEBaseInstrInfo *>(MF.getSubtarget().getInstrInfo());

  for (MachineBasicBlock &MBB : MF) {
    for (MachineInstr &MI : MBB) {
      for (const TiedRegOperands &Regs : TII.getTiedRegInfo(MI)) {
        processTiedOperands(Regs, MI);
      }
    }
  }

  return true;
}

void AIESubRegConstrainer::replaceRegOperands(Register OldReg, Register NewReg,
                                              unsigned NewSubReg,
                                              MachineRegisterInfo &MRI) {
  for (MachineOperand &Op : make_early_inc_range(MRI.reg_operands(OldReg))) {
    Op.setReg(NewReg);
    // TODO: We could compose sub-registers, but that's not needed at this point
    // because AIE2's DC registers have no sub-registers.
    assert(!Op.getSubReg() && "Cannot rewrite operand with existing subreg.");
    Op.setSubReg(NewSubReg);

    // We are changing OldReg operands into NewReg.NewSubReg. This changes the
    // semantics of kill and dead flags, because they would now apply to the
    // whole NewReg super-register. Below, we conservatively remove those flags.
    if (Op.isUse())
      Op.setIsKill(false);
    if (Op.isDef())
      Op.setIsDead(false);
  }
}

static bool isTiedPair(const MachineInstr &MI,
                       const OperandSubRegMapping &DstOp,
                       const OperandSubRegMapping &SrcOp) {
  if (MI.getOperand(DstOp.OpIdx).getReg() ==
      MI.getOperand(SrcOp.OpIdx).getReg()) {
    assert(MI.getOperand(DstOp.OpIdx).getSubReg() == DstOp.SubRegIdx &&
           MI.getOperand(SrcOp.OpIdx).getSubReg() == SrcOp.SubRegIdx);
    return true;
  }
  return false;
}

void AIESubRegConstrainer::processTiedOperands(const TiedRegOperands &Regs,
                                               MachineInstr &MI) {
  assert(!MI.getOperand(Regs.SrcOp.OpIdx).getSubReg() && !Regs.SrcOp.SubRegIdx);
  MachineFunction &MF = *MI.getParent()->getParent();
  auto *TII = MF.getSubtarget().getInstrInfo();
  MachineRegisterInfo &MRI = MF.getRegInfo();

  if (all_of(Regs.DstOps, [&Regs, &MI](const OperandSubRegMapping &DstOp) {
        return isTiedPair(MI, DstOp, Regs.SrcOp);
      })) {
    // Nothing to do if all registers are already tied.
    return;
  }

  // Create a new virtual register which will be used to replace all the uses
  // of tied destination registers, and the use of SrcReg in MI.
  auto SrcReg = MI.getOperand(Regs.SrcOp.OpIdx).getReg();
  auto CopyReg = MRI.createVirtualRegister(MRI.getRegClass(SrcReg));

  for (const OperandSubRegMapping &DstOp : Regs.DstOps) {
    auto DstReg = MI.getOperand(DstOp.OpIdx).getReg();
    LLVM_DEBUG(llvm::dbgs()
               << "Rewriting tied pair: Dst=" << MI.getOperand(DstOp.OpIdx)
               << " Src=" << MI.getOperand(Regs.SrcOp.OpIdx) << "\n");
    // MI should be in SSA form and fully define its destination registers.
    // Existing subregs make rewriting operands harder.
    assert(!MI.getOperand(DstOp.OpIdx).getSubReg());
    replaceRegOperands(DstReg, CopyReg, DstOp.SubRegIdx, MRI);
  }
  MI.getOperand(Regs.SrcOp.OpIdx).setReg(CopyReg);
  MI.getOperand(Regs.SrcOp.OpIdx).setIsKill(false);

  // Insert a copy before MI from SrcReg to CopyReg. This is breaking SSA, as
  // CopyReg is re-defined by MI.
  BuildMI(*MI.getParent(), MI, MI.getDebugLoc(), TII->get(TargetOpcode::COPY))
      .addReg(CopyReg, RegState::Define)
      .addReg(SrcReg);
}

} // end anonymous namespace

char AIESubRegConstrainer::ID = 0;
char &llvm::AIESubRegConstrainerID = AIESubRegConstrainer::ID;

INITIALIZE_PASS(AIESubRegConstrainer, DEBUG_TYPE, "AIE sub-reg constrainer",
                false, false)

llvm::FunctionPass *llvm::createAIESubRegConstrainer() {
  return new AIESubRegConstrainer();
}
