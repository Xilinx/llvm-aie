//===----------------------- AIE2FrameLowering.cpp ------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
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
  auto *TII = static_cast<const AIEInstrInfo *>(STI.getInstrInfo());

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
    LLVM_DEBUG(dbgs() << "Adjust Stack by " << StackPtrIncr << " bytes\n.");
    report_fatal_error(
        "adjustSPReg cannot yet handle adjustments > +-2^17 bytes");
  }
}

void AIE2FrameLowering::adjustReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator MBBI,
                                  const DebugLoc &DL, unsigned Reg,
                                  int64_t StackPtrIncr,
                                  MachineInstr::MIFlag Flag) const {
  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  auto *TII = static_cast<const AIEInstrInfo *>(STI.getInstrInfo());

  if (StackPtrIncr == 0)
    return;

  assert(AIE2::ePRegClass.contains(Reg) && "Reg is not in eP");

  // Note that we assume stack is 32-byte aligned.
  assert(StackPtrIncr % 32 == 0); // We only move the stack 32 bytes at a time.
  if (isInt<10 + 2>(StackPtrIncr)) {
    BuildMI(MBB, MBBI, DL, TII->get(AIE2::PADDA_lda_ptr_inc_idx_imm))
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

void AIE2FrameLowering::emitPrologue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {
  assert(&MF.front() == &MBB && "Shrink-wrapping not yet supported");

  MachineFrameInfo &MFI = MF.getFrameInfo();
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

  if (!hasFP(MF)) {
    return;
  }

  // The frame pointer is callee-saved, and code has been generated for us to
  // save it to the stack. We need to skip over the storing of callee-saved
  // registers as the frame pointer must be modified after it has been saved
  // to the stack, not before.
  // FIXME: assumes exactly one instruction is used to save each callee-saved
  // register.
  const std::vector<CalleeSavedInfo> &CSI = MFI.getCalleeSavedInfo();
  std::advance(MBBI, CSI.size());

  // Generate new FP.
  // Copy the stack pointer to the frame pointer, adjust it with the appropriate
  // amount
  Register FPReg = STI.getRegisterInfo()->getFrameRegister(MF);
  BuildMI(MBB, MBBI, DL, TII->get(AIE2::MOV_mv_scl), FPReg)
      .addReg(SPReg)
      .setMIFlag(MachineInstr::FrameSetup);

  adjustReg(MBB, MBBI, DL, FPReg, -StackSize, MachineInstr::FrameSetup);
}

void AIE2FrameLowering::emitEpilogue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  auto *TII = static_cast<const AIEInstrInfo *>(STI.getInstrInfo());
  auto *RI = static_cast<const AIEBaseRegisterInfo *>(STI.getRegisterInfo());

  DebugLoc DL = MBBI->getDebugLoc();
  Register SPReg = RI->getStackPointerRegister();
  uint64_t StackSize = MFI.getStackSize();

  // Restore the stack pointer using the value of the frame pointer. Only
  // necessary if the stack pointer was modified, meaning the stack size is
  // unknown.
  if (MFI.hasVarSizedObjects()) {
    assert(hasFP(MF) && "frame pointer should not have been eliminated");
    // Skip to before the restores of callee-saved registers
    // FIXME: assumes exactly one instruction is used to restore each
    // callee-saved register.
    auto LastFrameDestroy = std::prev(MBBI, MFI.getCalleeSavedInfo().size());
    Register FPReg = STI.getRegisterInfo()->getFrameRegister(MF);

    BuildMI(MBB, LastFrameDestroy, DL, TII->get(AIE2::MOV_mv_scl), SPReg)
        .addReg(FPReg)
        .setMIFlag(MachineInstr::FrameSetup);

    adjustSPReg(MBB, MBBI, DL, -StackSize, MachineInstr::FrameDestroy);
    return;
  }

  // Deallocate stack
  adjustSPReg(MBB, MBBI, DL, -StackSize, MachineInstr::FrameDestroy);
}

void AIE2FrameLowering::determineCalleeSaves(MachineFunction &MF,
                                             BitVector &SavedRegs,
                                             RegScavenger *RS) const {
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);

  // If there is a frame pointer (dynamic stack allocation), p7 will be used as
  // a frame pointer. The register allocator will not be able to see the
  // redefinition of p7 as the prologue and the epilogue are emitted after the
  // register allocation. Thus, we make sure to spill p7 at the beginning of the
  // function body and restore it at the end by adding it in SavedRegs.
  if (hasFP(MF))
    SavedRegs.set(AIE2::p7);
}
