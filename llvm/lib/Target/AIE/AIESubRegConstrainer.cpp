//===----- AIESubRegConstrainer.cpp - Constrain tied sub-registers --------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2025 Advanced Micro Devices, Inc. or its affiliates
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

  const TargetRegisterInfo &TRI = *MRI.getTargetRegisterInfo();
  assert(MRI.hasOneDef(OldReg) && "OldReg expected to be in SSA form.");

  for (MachineOperand &Op : make_early_inc_range(MRI.reg_operands(OldReg))) {
    Op.setReg(NewReg);
    // Compose the sub-register index if the dst operand already has
    // subregisters.
    if (Op.getSubReg())
      Op.setSubReg(TRI.composeSubRegIndices(NewSubReg, Op.getSubReg()));
    else
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

static bool isTiedPair(const MachineInstr &MI, const OperandSubRegMapping &Op1,
                       const OperandSubRegMapping &Op2) {
  if (&Op1 == &Op2)
    return true;
  if (MI.getOperand(Op1.OpIdx).getReg() == MI.getOperand(Op2.OpIdx).getReg()) {
    assert(MI.getOperand(Op1.OpIdx).getSubReg() == Op1.SubRegIdx &&
           MI.getOperand(Op2.OpIdx).getSubReg() == Op2.SubRegIdx);
    return true;
  }
  return false;
}

void AIESubRegConstrainer::processTiedOperands(const TiedRegOperands &Ties,
                                               MachineInstr &MI) {
  assert(Ties.SrcOps.size() >= 1);
  MachineFunction &MF = *MI.getParent()->getParent();
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
  MachineRegisterInfo &MRI = MF.getRegInfo();

  auto AreOperandsTied =
      [&Ties, &MI](const SmallVector<OperandSubRegMapping, 4> &Ops) {
        return all_of(Ops, [&](const OperandSubRegMapping &DstOp) {
          return isTiedPair(MI, DstOp, Ties.SrcOps.front());
        });
      };

  if (AreOperandsTied(Ties.SrcOps) && AreOperandsTied(Ties.DstOps)) {
    // Nothing to do if all registers are already tied.
    return;
  }

  // Create a new virtual register which will be used to replace all the uses
  // of tied destination registers, and the use of SrcReg in MI.
  const TargetRegisterClass *SuperRegRC = Ties.NewSuperClass;
  if (!SuperRegRC) {
    assert(Ties.SrcOps.size() == 1);
    Register SrcReg = MI.getOperand(Ties.SrcOps.front().OpIdx).getReg();
    SuperRegRC = MRI.getRegClass(SrcReg);
  }
  auto CopyOrRegSeq = MRI.createVirtualRegister(SuperRegRC);

  // Rewrite register defs to use CopyOrRegSeq, and replace all register
  // operands using the old register (now dead).
  for (const OperandSubRegMapping &DstOp : Ties.DstOps) {
    auto DstReg = MI.getOperand(DstOp.OpIdx).getReg();
    LLVM_DEBUG(llvm::dbgs()
               << "Rewriting tied pair: Dst=" << MI.getOperand(DstOp.OpIdx));

    assert(!MI.getOperand(DstOp.OpIdx).getSubReg());
    replaceRegOperands(DstReg, CopyOrRegSeq, DstOp.SubRegIdx, MRI);
    LLVM_DEBUG(llvm::dbgs() << " to " << MI.getOperand(DstOp.OpIdx) << "\n");
  }

  // In case of a single SrcOp, insert a copy before MI from SrcReg to
  // CopyOrRegSeq. This is breaking SSA, as CopyOrRegSeq is re-defined by MI. In
  // case of multiple SrcOps, create a REG_SEQUENCE with the subregisters.
  if (Ties.SrcOps.size() == 1 && !Ties.SrcOps.front().SubRegIdx) {
    auto SrcReg = MI.getOperand(Ties.SrcOps.front().OpIdx).getReg();
    BuildMI(*MI.getParent(), MI, MI.getDebugLoc(), TII.get(TargetOpcode::COPY))
        .addReg(CopyOrRegSeq, RegState::Define)
        .addReg(SrcReg);
  } else {
    auto MIB = BuildMI(*MI.getParent(), MI, MI.getDebugLoc(),
                       TII.get(TargetOpcode::REG_SEQUENCE), CopyOrRegSeq);
    for (const OperandSubRegMapping &SrcOp : Ties.SrcOps) {
      Register SrcReg = MI.getOperand(SrcOp.OpIdx).getReg();
      const unsigned SrcSubRegIdx = MI.getOperand(SrcOp.OpIdx).getSubReg();
      MIB.addReg(SrcReg,
                 MI.getOperand(SrcOp.OpIdx).isUndef() ? RegState::Undef : 0,
                 SrcSubRegIdx);
      MIB.addImm(SrcOp.SubRegIdx);
    }
    LLVM_DEBUG(llvm::dbgs() << "Inserted: " << *MIB.getInstr());
  }

  // Rewrite register sources to use CopyOrRegSeq.
  for (const OperandSubRegMapping &SrcOp : Ties.SrcOps) {
    LLVM_DEBUG(llvm::dbgs()
               << "Rewriting tied pair: Src=" << MI.getOperand(SrcOp.OpIdx));
    MI.getOperand(SrcOp.OpIdx).setReg(CopyOrRegSeq);
    MI.getOperand(SrcOp.OpIdx).setSubReg(SrcOp.SubRegIdx);
    MI.getOperand(SrcOp.OpIdx).setIsKill(false);
    LLVM_DEBUG(llvm::dbgs() << " to " << MI.getOperand(SrcOp.OpIdx) << "\n");
  }
}

} // end anonymous namespace

char AIESubRegConstrainer::ID = 0;
char &llvm::AIESubRegConstrainerID = AIESubRegConstrainer::ID;

INITIALIZE_PASS(AIESubRegConstrainer, DEBUG_TYPE, "AIE sub-reg constrainer",
                false, false)

llvm::FunctionPass *llvm::createAIESubRegConstrainer() {
  return new AIESubRegConstrainer();
}
