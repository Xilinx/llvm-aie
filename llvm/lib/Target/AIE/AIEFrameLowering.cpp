//===-- AIEFrameLowering.cpp - AIE Frame Information ------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "AIEFrameLowering.h"
#include "AIEBaseRegisterInfo.h"
#include "AIEMachineFunctionInfo.h"
#include "AIESubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "aie-frame-lowering"

using namespace llvm;

// Reg expected to be a pointer register (PTRRegClass)
void AIEFrameLowering::adjustReg(const TargetInstrInfo *TII,
                                 MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MBBI,
                                 const DebugLoc &DL, unsigned Reg, int64_t Val,
                                 MachineInstr::MIFlag Flag) {
  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();

  if (Val == 0)
    return;

  if (isInt<4+2>(Val) && (Val % 4 == 0)) {
    // Use an nrm_imm instruction if we can.  However, it encodes a 6
    // bit number with 2 LSBs being zero.
    BuildMI(MBB, MBBI, DL, TII->get(AIE::PADDA_nrm_imm), Reg)
        .addReg(Reg)
        .addImm(Val)
        .setMIFlag(Flag);
  } else if(isInt<12>(Val)) {
    Register ScratchReg = MRI.createVirtualRegister(&AIE::MODRegClass);
    BuildMI(MBB, MBBI, DL, TII->get(AIE::MOV_S12), ScratchReg)
        .addImm(Val)
        .setMIFlag(Flag);
    BuildMI(MBB, MBBI, DL, TII->get(AIE::PADDA_nrm), Reg)
        .addReg(Reg)
        .addReg(ScratchReg)
        .setMIFlag(Flag);
  } else if (isInt<20>(Val)) {
    Register ImmReg = MRI.createVirtualRegister(&AIE::GPRRegClass);
    Register ModReg = MRI.createVirtualRegister(&AIE::MODRegClass);
    BuildMI(MBB, MBBI, DL, TII->get(AIE::MOV_S20), ImmReg)
      .addImm(Val)
      .setMIFlag(Flag);
    BuildMI(MBB, MBBI, DL, TII->get(AIE::MOV), ModReg)
      .addReg(ImmReg, RegState::Kill)
      .setMIFlag(Flag);
    BuildMI(MBB, MBBI, DL, TII->get(AIE::PADDA_nrm), Reg)
        .addReg(Reg)
        .addReg(ModReg)
        .setMIFlag(Flag);
  } else {
    report_fatal_error("adjustReg cannot yet handle adjustments >20 bits");
  }
}

// Modify the stack pointer by Val.
void AIEFrameLowering::adjustSPReg(MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator MBBI,
                                   const DebugLoc &DL, int64_t Val,
                                   MachineInstr::MIFlag Flag) const {
  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  auto *TII = static_cast<const AIEInstrInfo *>(STI.getInstrInfo());

  if (Val == 0)
    return;

  // Note that we assume stack is 32-byte aligned.
  assert(Val % 32 == 0); // We only move the stack 32 bytes at a time.
  if (isInt<5+5>(Val)) {
    // Use an sp_imm instruction if we can.  However, it encodes a 10
    // bit number with 5 LSBs being zero.
    BuildMI(MBB, MBBI, DL, TII->get(AIE::PADDA_sp_imm))
        .addImm(Val)
        .setMIFlag(Flag);
  } else if(isInt<12>(Val)) {
    Register ScratchReg = MRI.createVirtualRegister(&AIE::MODRegClass);
    BuildMI(MBB, MBBI, DL, TII->get(AIE::MOV_S12), ScratchReg)
        .addImm(Val)
        .setMIFlag(Flag);
    BuildMI(MBB, MBBI, DL, TII->get(AIE::PADDA_sp))
      .addReg(ScratchReg, RegState::Kill)
      .setMIFlag(Flag);
  } else if (isInt<20>(Val)) {
    Register ImmReg = MRI.createVirtualRegister(&AIE::GPRRegClass);
    Register ModReg = MRI.createVirtualRegister(&AIE::MODRegClass);
    BuildMI(MBB, MBBI, DL, TII->get(AIE::MOV_S20), ImmReg)
      .addImm(Val)
      .setMIFlag(Flag);
    BuildMI(MBB, MBBI, DL, TII->get(AIE::MOV), ModReg)
      .addReg(ImmReg, RegState::Kill)
      .setMIFlag(Flag);
    BuildMI(MBB, MBBI, DL, TII->get(AIE::PADDA_sp))
      .addReg(ModReg, RegState::Kill)
      .setMIFlag(Flag);
  } else {
      LLVM_DEBUG(dbgs() << "Adjust Stack by " << Val << " bytes\n.");
      report_fatal_error("adjustSPReg cannot yet handle adjustments > +-2^19 bytes");
  }
}


void AIEFrameLowering::emitPrologue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  assert(&MF.front() == &MBB && "Shrink-wrapping not yet supported");

  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineRegisterInfo &MRI = MF.getRegInfo();
  MachineBasicBlock::iterator MBBI = MBB.begin();
  auto *TII = static_cast<const AIEInstrInfo *>(STI.getInstrInfo());
  auto *RI = static_cast<const AIEBaseRegisterInfo *>(STI.getRegisterInfo());
  Register SPReg = RI->getStackPointerRegister();

  // Debug location must be unknown since the first debug location is used
  // to determine the end of the prologue.
  DebugLoc DL;

  // Determine the correct frame layout
  determineFrameLayout(MF);

  // FIXME (note copied from Lanai): This appears to be overallocating.  Needs
  // investigation. Get the number of bytes to allocate from the FrameInfo.
  uint64_t StackSize = MFI.getStackSize();

  // Early exit if there is no need to allocate on the stack
  if (StackSize == 0 && !MFI.adjustsStack())
    return;

  // Allocate space on the stack if necessary.
  adjustSPReg(MBB, MBBI, DL, StackSize, MachineInstr::FrameSetup);

  // The frame pointer is callee-saved, and code has been generated for us to
  // save it to the stack. We need to skip over the storing of callee-saved
  // registers as the frame pointer must be modified after it has been saved
  // to the stack, not before.
  // FIXME: assumes exactly one instruction is used to save each callee-saved
  // register.
  const std::vector<CalleeSavedInfo> &CSI = MFI.getCalleeSavedInfo();
  std::advance(MBBI, CSI.size());

  // Generate new FP.
  if (hasFP(MF)) {
    Register FPReg = STI.getRegisterInfo()->getFrameRegister(MF);
    Register ScratchReg = MRI.createVirtualRegister(&AIE::GPRRegClass);
    BuildMI(MBB, MBBI, DL, TII->get(AIE::MV_SPECIAL2R), ScratchReg)
        .addReg(SPReg)
        .setMIFlag(MachineInstr::FrameSetup);
    BuildMI(MBB, MBBI, DL, TII->get(AIE::MOV), FPReg)
        .addReg(ScratchReg)
        .setMIFlag(MachineInstr::FrameSetup);

    adjustReg(TII, MBB, MBBI, DL, FPReg, -StackSize, MachineInstr::FrameSetup);
  }
}

void AIEFrameLowering::emitEpilogue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineRegisterInfo &MRI = MF.getRegInfo();

  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  auto *TII = static_cast<const AIEInstrInfo *>(STI.getInstrInfo());
  auto *RI = static_cast<const AIEBaseRegisterInfo *>(STI.getRegisterInfo());

  DebugLoc DL = MBBI->getDebugLoc();
  Register SPReg = RI->getStackPointerRegister();
  uint64_t StackSize = MFI.getStackSize();

  // Deallocate the dynamic part of the stack frame, bringing SP back to
  // its normal position. We use the value of the framepointer, which must
  // exist.
  if (MFI.hasVarSizedObjects()) {
    assert(hasFP(MF) && "frame pointer should not have been eliminated");
    // Skip to before the restores of callee-saved registers
    // FIXME: assumes exactly one instruction is used to restore each
    // callee-saved register.
    auto LastFrameDestroy = std::prev(MBBI, MFI.getCalleeSavedInfo().size());
    Register FPReg = STI.getRegisterInfo()->getFrameRegister(MF);
    Register ScratchReg = MRI.createVirtualRegister(&AIE::GPRRegClass);

    BuildMI(MBB, MBBI, DL, TII->get(AIE::MV_SPECIAL2R), ScratchReg)
        .addReg(FPReg)
        .setMIFlag(MachineInstr::FrameSetup);
    BuildMI(MBB, MBBI, DL, TII->get(AIE::MV_R2SPECIAL), SPReg)
    .addReg(ScratchReg)
    .setMIFlag(MachineInstr::FrameSetup);

    adjustSPReg(MBB, LastFrameDestroy, DL, -StackSize,
                MachineInstr::FrameDestroy);
  }

  // Deallocate stack
  adjustSPReg(MBB, MBBI, DL, -StackSize, MachineInstr::FrameDestroy);
}
