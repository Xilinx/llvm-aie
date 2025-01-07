//===----------------------- AIE2PFrameLowering.cpp ----------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE2p implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "AIE2PFrameLowering.h"
#include "AIE2PSubtarget.h"
#include "MCTargetDesc/aie2p/AIE2PMCTargetDesc.h"

#define DEBUG_TYPE "aie-frame-lowering"

using namespace llvm;

void AIE2PFrameLowering::adjustReg(MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator MBBI,
                                   const DebugLoc &DL, unsigned Reg,
                                   int64_t StackPtrIncr,
                                   MachineInstr::MIFlag Flag) const {
  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  auto *TII = static_cast<const AIEInstrInfo *>(STI.getInstrInfo());

  if (StackPtrIncr == 0)
    return;

  assert(AIE2P::ePRegClass.contains(Reg) && "Reg is not in eP");

  // Note that we assume stack is 64-byte aligned.
  assert(StackPtrIncr % 64 == 0); // We only move the stack 64 bytes at a time.
  if (isInt<4 + 6>(StackPtrIncr)) {
    BuildMI(MBB, MBBI, DL, TII->get(AIE2P::PADD_imm_pseudo))
        .addDef(Reg)
        .addUse(Reg)
        .addImm(StackPtrIncr)
        .setMIFlag(Flag);
  } else {
    Register ModReg = MRI.createVirtualRegister(&AIE2P::eMRegClass);
    BuildMI(MBB, MBBI, DL, TII->get(AIE2P::MOVXM), ModReg)
        .addImm(StackPtrIncr)
        .setMIFlag(Flag);
    BuildMI(MBB, MBBI, DL, TII->get(AIE2P::PADD_mod_pseudo))
        .addDef(Reg)
        .addUse(Reg)
        .addUse(ModReg)
        .setMIFlag(Flag);
  }
}

// Modify the stack pointer by Val.
void AIE2PFrameLowering::adjustSPReg(MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator MBBI,
                                     const DebugLoc &DL, int64_t StackPtrIncr,
                                     MachineInstr::MIFlag Flag) const {

  auto *TII = static_cast<const AIEInstrInfo *>(STI.getInstrInfo());

  if (StackPtrIncr == 0)
    return;
  // Note that we assume stack is 64-byte aligned.
  assert(StackPtrIncr % 64 == 0); // We only move the stack 64 bytes at a time.
  // Use an sp_imm instruction if we can.  However, it encodes an
  // 19 bit number with 6 LSBs being zero.
  if (isInt<13 + 6>(StackPtrIncr)) {
    // +- 2^18 bits for PADDB
    BuildMI(MBB, MBBI, DL, TII->get(AIE2P::PADDXM_pstm_sp_imm))
        .addImm(StackPtrIncr)
        .setMIFlag(Flag);
  } else {
    LLVM_DEBUG(dbgs() << "Adjust Stack by " << StackPtrIncr << " bytes\n.");
    report_fatal_error(
        "adjustSPReg cannot yet handle adjustments > +-2^18 bytes");
  }
}
void AIE2PFrameLowering::determineCalleeSaves(MachineFunction &MF,
                                              BitVector &SavedRegs,
                                              RegScavenger *RS) const {
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
  Register FPReg = STI.getRegisterInfo()->getFrameRegister(MF);

  // If there is a frame pointer (dynamic stack allocation), p7 will be used as
  // a frame pointer. The register allocator will not be able to see the
  // redefinition of p7 as the prologue and the epilogue are emitted after the
  // register allocation. Thus, we make sure to spill p7 at the beginning of the
  // function body and restore it at the end by adding it in SavedRegs.
  if (hasFP(MF))
    SavedRegs.set(FPReg);
}
