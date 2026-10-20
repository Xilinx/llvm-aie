//===----------------------- AIE2FrameLowering.cpp ------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE2 implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "AIE2FrameLowering.h"
#include "AIE2Subtarget.h"
#include "AIEBaseRegisterInfo.h"
#include "AIEMachineFunctionInfo.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "aie-frame-lowering"

using namespace llvm;

// Modify the stack pointer by Val.
void AIE2FrameLowering::adjustSPReg(MachineBasicBlock &MBB,
                                    MachineBasicBlock::iterator MBBI,
                                    const DebugLoc &DL, int64_t StackPtrIncr,
                                    MachineInstr::MIFlag Flag) const {
  auto *TII = static_cast<const AIE2InstrInfo *>(STI.getInstrInfo());

  if (StackPtrIncr == 0)
    return;

  // Note that we assume stack is 32-byte aligned.
  assert(StackPtrIncr % 32 == 0); // We only move the stack 32 bytes at a time.
  // Use an sp_imm instruction if we can.  However, it encodes an
  // 18 (17 for PADDB) bit number with 5 LSBs being zero.
  if (isInt<12 + 5>(StackPtrIncr)) {
    // +- 2^16 bits for PADDB
    BuildMI(MBB, MBBI, DL, TII->get(AIE2::PADD_sp_imm_pseudo))
        .addImm(StackPtrIncr)
        .setMIFlag(Flag);
  } else if (isInt<13 + 5>(StackPtrIncr)) {
    // +-2^17 bits for PADDA
    BuildMI(MBB, MBBI, DL, TII->get(AIE2::PADDA_sp_imm))
        .addImm(StackPtrIncr)
        .setMIFlag(Flag);
  } else {
    // AIE2 has no register-form SP pointer add (only the sp_imm forms), and SP
    // is reserved, so it cannot be an explicit operand of the general pointer
    // add. Copy SP into a scratch pointer register, add the materialized offset
    // there, and copy the result back to SP. This is the same sequence the
    // backend already uses to update SP for a dynamic stack allocation. The
    // scratch registers are resolved by the register scavenger.
    MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
    auto *RI = static_cast<const AIEBaseRegisterInfo *>(STI.getRegisterInfo());
    Register SPReg = RI->getStackPointerRegister();
    Register ScratchP = MRI.createVirtualRegister(&AIE2::ePRegClass);
    Register ModReg = MRI.createVirtualRegister(&AIE2::eMRegClass);
    BuildMI(MBB, MBBI, DL, TII->get(TII->getMvSclOpcode()), ScratchP)
        .addReg(SPReg)
        .setMIFlag(Flag);
    BuildMI(MBB, MBBI, DL, TII->get(AIE2::MOVXM_lng_cg), ModReg)
        .addImm(StackPtrIncr)
        .setMIFlag(Flag);
    BuildMI(MBB, MBBI, DL, TII->get(AIE2::PADDA_lda_ptr_inc_idx))
        .addDef(ScratchP)
        .addUse(ScratchP)
        .addUse(ModReg)
        .setMIFlag(Flag);
    BuildMI(MBB, MBBI, DL, TII->get(TII->getMvSclOpcode()), SPReg)
        .addReg(ScratchP, RegState::Kill)
        .setMIFlag(Flag);
  }
}

void AIE2FrameLowering::adjustReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator MBBI,
                                  const DebugLoc &DL, unsigned Reg,
                                  int64_t StackPtrIncr,
                                  MachineInstr::MIFlag Flag) const {
  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  auto *TII = static_cast<const AIE2InstrInfo *>(STI.getInstrInfo());

  if (StackPtrIncr == 0)
    return;

  assert(AIE2::ePRegClass.contains(Reg) && "Reg is not in eP");

  // Note that we assume stack is 32-byte aligned.
  assert(StackPtrIncr % 32 == 0); // We only move the stack 32 bytes at a time.
  if (isInt<10 + 2>(StackPtrIncr)) {
    BuildMI(MBB, MBBI, DL, TII->get(AIE2::PADD_imm10_pseudo))
        .addDef(Reg)
        .addUse(Reg)
        .addImm(StackPtrIncr)
        .setMIFlag(Flag);
  } else {
    Register ModReg = MRI.createVirtualRegister(&AIE2::eMRegClass);
    BuildMI(MBB, MBBI, DL, TII->get(AIE2::MOVXM_lng_cg), ModReg)
        .addImm(StackPtrIncr)
        .setMIFlag(Flag);
    BuildMI(MBB, MBBI, DL, TII->get(AIE2::PADDA_lda_ptr_inc_idx))
        .addDef(Reg)
        .addUse(Reg)
        .addUse(ModReg)
        .setMIFlag(Flag);
  }
}

void AIE2FrameLowering::determineCalleeSaves(MachineFunction &MF,
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
