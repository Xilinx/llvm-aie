//===----------------------- AIE2PSFrameLowering.cpp ----------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE2ps implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "AIE2PSFrameLowering.h"
#include "AIE2PSSubtarget.h"
#include "MCTargetDesc/aie2ps/AIE2PSMCTargetDesc.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"

#define DEBUG_TYPE "aie-frame-lowering"

using namespace llvm;

void AIE2PSFrameLowering::adjustReg(MachineBasicBlock &MBB,
                                    MachineBasicBlock::iterator MBBI,
                                    const DebugLoc &DL, unsigned Reg,
                                    int64_t StackPtrIncr,
                                    MachineInstr::MIFlag Flag) const {
  MachineRegisterInfo &MRI = MBB.getParent()->getRegInfo();
  auto *TII = static_cast<const AIE2PSInstrInfo *>(STI.getInstrInfo());

  if (StackPtrIncr == 0)
    return;

  assert(AIE2PS::ePRegClass.contains(Reg) && "Reg is not in eP");

  // Note that we assume stack is 64-byte aligned.
  assert(StackPtrIncr % 64 == 0); // We only move the stack 64 bytes at a time.
  if (isInt<4 + 6>(StackPtrIncr)) {
    BuildMI(MBB, MBBI, DL, TII->get(AIE2PS::PADD_imm_pseudo))
        .addDef(Reg)
        .addUse(Reg)
        .addImm(StackPtrIncr)
        .setMIFlag(Flag);
  } else {
    Register ModReg = MRI.createVirtualRegister(&AIE2PS::eMRegClass);
    BuildMI(MBB, MBBI, DL, TII->get(AIE2PS::MOVXM_lng_cg), ModReg)
        .addImm(StackPtrIncr)
        .setMIFlag(Flag);
    BuildMI(MBB, MBBI, DL, TII->get(AIE2PS::PADD_mod_pseudo))
        .addDef(Reg)
        .addUse(Reg)
        .addUse(ModReg)
        .setMIFlag(Flag);
  }
}

// Modify the stack pointer by Val.
void AIE2PSFrameLowering::adjustSPReg(MachineBasicBlock &MBB,
                                      MachineBasicBlock::iterator MBBI,
                                      const DebugLoc &DL, int64_t StackPtrIncr,
                                      MachineInstr::MIFlag Flag) const {
  auto *TII = static_cast<const AIE2PSInstrInfo *>(STI.getInstrInfo());

  if (StackPtrIncr == 0)
    return;
  // Note that we assume stack is 64-byte aligned.
  assert(StackPtrIncr % 64 == 0); // We only move the stack 64 bytes at a time.
  // Use an sp_imm instruction if we can.  However, it encodes an
  // 19 bit number with 6 LSBs being zero.
  if (isInt<13 + 6>(StackPtrIncr)) {
    // +- 2^18 bits for PADDB
    BuildMI(MBB, MBBI, DL, TII->get(AIE2PS::PADDXM_pstm_sp_imm))
        .addImm(StackPtrIncr)
        .setMIFlag(Flag);
  } else {
    LLVM_DEBUG(dbgs() << "Adjust Stack by " << StackPtrIncr << " bytes\n.");
    report_fatal_error(
        "adjustSPReg cannot yet handle adjustments > +-2^18 bytes");
  }
}

void AIE2PSFrameLowering::determineCalleeSaves(MachineFunction &MF,
                                               BitVector &SavedRegs,
                                               RegScavenger *RS) const {
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
  const TargetRegisterInfo *TRI = MF.getSubtarget().getRegisterInfo();
  const MachineFrameInfo &MFI = MF.getFrameInfo();

  // When both L registers and their sub-GPRs are in the CSR list, we need to
  // decide whether to save as L register or individual GPRs.
  //
  // Strategy:
  // - If only one GPR of the pair is used: save just that GPR
  // - If both GPRs are used AND function has calls: use L register save
  //   (stack spill is required, 1 L spill is more efficient than 2 GPR spills)
  // - If both GPRs are used AND no calls: use individual GPR saves
  //   (allows GPR-to-GPR spilling via scratch registers)
  // Build the list of callee-saved L registers from the callee-saved regs
  // provided by CSR list.
  SmallVector<MCPhysReg, 4> CalleeSavedLRegs;
  const MCPhysReg *CSRegs = MF.getRegInfo().getCalleeSavedRegs();
  for (unsigned I = 0; CSRegs[I]; ++I) {
    MCPhysReg Reg = CSRegs[I];
    if (AIE2PS::eLRegClass.contains(Reg))
      CalleeSavedLRegs.push_back(Reg);
  }

  for (MCPhysReg LReg : CalleeSavedLRegs) {
    // Get the two GPR subregisters of this L register
    MCPhysReg EvenGPR = TRI->getSubReg(LReg, AIE2PS::sub_l_even);
    MCPhysReg OddGPR = TRI->getSubReg(LReg, AIE2PS::sub_l_odd);

    // Check what's marked for saving by the base determineCalleeSaves.
    // This already reflects which registers are actually clobbered.
    const bool LRegMarked = SavedRegs.test(LReg);
    const bool EvenMarked = SavedRegs.test(EvenGPR);
    const bool OddMarked = SavedRegs.test(OddGPR);

    if (!LRegMarked && !EvenMarked && !OddMarked)
      continue;

    SavedRegs.reset(EvenGPR);
    SavedRegs.reset(OddGPR);
    SavedRegs.reset(LReg);

    assert((!(EvenMarked || OddMarked) || LRegMarked) &&
           "sub-reg mark without L pair mark violates invariant");

    // Determine if both subregisters actually need saving.
    // LRegMarked alone doesn't mean both - check individual GPR marks.
    const bool BothNeeded =
        (EvenMarked && OddMarked) || (LRegMarked && !EvenMarked && !OddMarked);

    // When there is calls we mark the L register so that we get a single
    // spill instead of 2. When there are no calls, we prefer marking the
    // subregisters since they can be copied to non CSR registers instead of
    // spilled to memory (There is no move instruction between L registers).
    // For the call case we have no choice but to spill anyway since we don't
    // know which registers the callee is going to use.
    if (BothNeeded) {
      // Both subregisters need saving.
      if (MFI.hasCalls()) {
        // Use L register save. Stack spill is required (scratch regs
        // clobbered by calls), so 1 L spill is more efficient than 2 GPR
        // spills.
        SavedRegs.set(LReg);
      } else {
        // No calls: use individual GPRs for GPR-to-GPR copy.
        SavedRegs.set(EvenGPR);
        SavedRegs.set(OddGPR);
      }
    } else if (EvenMarked) {
      SavedRegs.set(EvenGPR);
    } else {
      assert(OddMarked);
      SavedRegs.set(OddGPR);
    }
  }
  // If there is a frame pointer (dynamic stack allocation), p7 will be used
  // as a frame pointer. The register allocator will not be able to see the
  // redefinition of p7 as the prologue and the epilogue are emitted after the
  // register allocation. Thus, we make sure to spill p7 at the beginning of
  // the function body and restore it at the end by adding it in SavedRegs.
  const Register FPReg = TRI->getFrameRegister(MF);
  if (hasFP(MF))
    SavedRegs.set(FPReg);
}

// Although the register scavenger can often find a spare register, an
// emergency spill slot might be needed to guarantee success.
void AIE2PSFrameLowering::processFunctionBeforeFrameFinalized(
    MachineFunction &MF, RegScavenger *RS) const {
  const TargetRegisterInfo *RegInfo = MF.getSubtarget().getRegisterInfo();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const TargetRegisterClass *RC1 = &AIE2PS::ePRegClass;
  const TargetRegisterClass *RC2 = &AIE2PS::eDJRegClass;
  if (!isInt<12>(MFI.estimateStackSize(MF))) {
    // For an estimate stack size greater than isInt<12>, consevativly we
    // allocate emergency spill slots in the stack.
    int RegScavPtr = MFI.CreateStackObject(RegInfo->getSpillSize(*RC1),
                                           RegInfo->getSpillAlign(*RC1), false);
    int RegScavDj = MFI.CreateStackObject(RegInfo->getSpillSize(*RC2),
                                          RegInfo->getSpillAlign(*RC2), false);
    RS->addScavengingFrameIndex(RegScavPtr);
    RS->addScavengingFrameIndex(RegScavDj);
  }
}
