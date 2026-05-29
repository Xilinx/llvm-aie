//===-- AIEBaseFrameLowering.cpp - AIE Frame Information ------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the generic AIE implementation of TargetFrameLowering
// class.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseFrameLowering.h"
#include "AIEBaseRegisterInfo.h"
#include "AIEBaseSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include <cstdint>

#define DEBUG_TYPE "aie-frame-lowering"

using namespace llvm;

/* Variable length arrays (dynamic stack allocations) form the principle reason
   to use a framepointer. The stack pointer will be unstable during function
   execution, making it awkward to use it as a base address. isFrameAddressTaken
   is set by some architectures when seeing llvm.frameaddress, using the frame
   pointer value as the result. DisableFramePointerElim is similar, usually to
   supply better debugging facilities. We can change the logic so as to
   guarantee that we can compute the canonical frame address in a deterministic
   way, be it from SP or from FP. For example, llvm.frameaddress could return SP
   in functions that don't have dynamic allocation.
 */
bool AIEBaseFrameLowering::hasFPImpl(const MachineFunction &MF) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  return MF.getTarget().Options.DisableFramePointerElim(MF) ||
         MFI.hasVarSizedObjects() || MFI.isFrameAddressTaken();
}

// Determines the size of the frame and maximum call frame size.
void AIEBaseFrameLowering::determineFrameLayout(MachineFunction &MF) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const TargetRegisterInfo *RI = STI.getRegisterInfo();

  // Get the number of bytes to allocate from the FrameInfo.
  uint64_t FrameSize = MFI.getStackSize();

  // Get the alignment.
  Align StackAlign =
      RI->hasStackRealignment(MF) ? MFI.getMaxAlign() : getStackAlign();

  // Make sure the frame is aligned.
  FrameSize = alignTo(FrameSize, StackAlign);

  // Update frame info.
  MFI.setStackSize(FrameSize);
}

/// Returns the displacement from the frame register to the stack
/// frame of the specified index, along with the frame register used
/// (in output arg FrameReg).
/// For AIE, stack grows up, and the stack pointer points to the top
/// of the stack. When a function is called, the stack looks like:
/// [sp] ->
/// [sp, #-4] -> arg 5
/// [sp, #-8] -> arg 6
///
/// After the prologue, we might have something like:
/// [sp] ->
/// [fp, #16] or [sp, #-16] -> spill 2
/// [fp, # 8] or [sp, #-24] -> spill 1
/// [fp, # 4] or [sp, #-28] -> local 2
/// [fp]      or [sp, #-32] -> local 1
/// [fp, #-4] or [sp, #-36] -> arg 5
/// [fp, #-8] or [sp, #-40] -> arg 6
/// Dynamic allocations (using alloca) will end up incrementing the stack
/// pointer further. The presence of dynamic allocations also requires the
/// existance of a frame pointer that will be used to access incoming arguments,
/// local stack variables, and spilled registers
StackOffset
AIEBaseFrameLowering::getFrameIndexReference(const MachineFunction &MF, int FI,
                                             Register &FrameReg) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  const TargetRegisterInfo *RI = MF.getSubtarget().getRegisterInfo();

  FrameReg = RI->getFrameRegister(MF);

  return StackOffset::getFixed(
      MFI.getObjectOffset(FI)  // Move from start of frame to a variable.
      - MFI.getStackSize()     // Move from stack pointer to start of frame.
      - getOffsetOfLocalArea() // always zero?
      + MFI.getOffsetAdjustment()); // always zero?
}

// Not preserve stack space within prologue for outgoing variables when the
// function contains variable size objects or there are vector objects accessed
// by the frame pointer.
// Let eliminateCallFramePseudoInstr preserve stack space for it.
bool AIEBaseFrameLowering::hasReservedCallFrame(
    const MachineFunction &MF) const {
  return !MF.getFrameInfo().hasVarSizedObjects() && !hasFP(MF);
}

// Eliminate ADJCALLSTACKDOWN, ADJCALLSTACKUP pseudo instructions.
MachineBasicBlock::iterator AIEBaseFrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator MI) const {

  if (!hasReservedCallFrame(MF)) {
    // If space has not been reserved for a call frame, ADJCALLSTACKDOWN and
    // ADJCALLSTACKUP must be converted to instructions manipulating the stack
    // pointer. This is necessary when there is a variable length stack
    // allocation (e.g. alloca), which means it's not possible to allocate
    // space for outgoing arguments from within the function prologue.
    int64_t Amount = MI->getOperand(0).getImm();

    if (Amount != 0) {
      // Ensure the stack remains aligned after adjustment.
      Amount = alignSPAdjust(Amount);

      unsigned CFSetupOpcode = STI.getInstrInfo()->getCallFrameSetupOpcode();
      unsigned CFDestroyOpcode =
          STI.getInstrInfo()->getCallFrameDestroyOpcode();

      if (MI->getOpcode() == CFSetupOpcode)
        Amount = -Amount;
      else
        assert(MI->getOpcode() == CFDestroyOpcode);

      DebugLoc DL = MI->getDebugLoc();
      adjustSPReg(MBB, MI, DL, -Amount, MachineInstr::NoFlags);
    }
  }

  return MBB.erase(MI);
}

// Returns a BitVector of the intersection of GPR RegClass
// and CalleeSaved Registers
static BitVector getCalleeSavedGPRRegClass(const MachineFunction &MF,
                                           const TargetRegisterInfo *TRI) {

  auto *RI = static_cast<const AIEBaseRegisterInfo *>(
      MF.getSubtarget().getRegisterInfo());
  const TargetRegisterClass &RC = *RI->getGPRRegClass(MF);
  BitVector BVGPR(TRI->getNumRegs());
  BitVector BVCalleeSaved(TRI->getNumRegs());
  for (unsigned Reg : RC) {
    BVGPR.set(Reg);
  }
  const MachineRegisterInfo &MRI = MF.getRegInfo();
  const MCPhysReg *CSRegs = MRI.getCalleeSavedRegs();
  for (unsigned i = 0; CSRegs[i]; ++i) {
    BVCalleeSaved.set(CSRegs[i]);
  }
  return BVGPR &= BVCalleeSaved;
}

// This function checks if a callee saved GPR can be spilled to a
// GPR Register.If there are any remaining registers which were not spilled
// to GPRs, return false so the target independent code can handle them by
// assigning a FrameIdx to a stack slot.
bool AIEBaseFrameLowering::assignCalleeSavedSpillSlots(
    MachineFunction &MF, const TargetRegisterInfo *TRI,
    std::vector<CalleeSavedInfo> &CSI) const {
  if (CSI.empty())
    return true; // Early exit if no callee saved registers are modified!
  MachineFrameInfo &MFI = MF.getFrameInfo();
  auto *RI = static_cast<const AIEBaseRegisterInfo *>(STI.getRegisterInfo());

  const TargetRegisterClass &RC = *RI->getGPRRegClass(MF);
  BitVector BVCalleeSavedGPR = getCalleeSavedGPRRegClass(MF, TRI);
  if (MFI.hasCalls())
    return false;
  // Build a BitVector of GPRs that can be used for spilling eRS8s.
  BitVector BVAllocatable = TRI->getAllocatableSet(MF);
  BitVector BVCalleeSaved(TRI->getNumRegs());
  const MachineRegisterInfo &MRI = MF.getRegInfo();
  const MCPhysReg *CSRegs = MRI.getCalleeSavedRegs();
  for (unsigned i = 0; CSRegs[i]; ++i) {
    BVCalleeSaved.set(CSRegs[i]);
  }
  for (unsigned Reg : BVAllocatable.set_bits()) {
    // Set to 0 if the register is not a GPR register, or if it is
    // used in the function.
    if (BVCalleeSaved[Reg] || !RC.contains(Reg) ||
        MF.getRegInfo().isPhysRegUsed(Reg))
      BVAllocatable.reset(Reg);
  }
  bool AllSpilledToReg = true;
  for (auto &CS : CSI) {
    if (BVAllocatable.none())
      return false;

    Register Reg = CS.getReg();
    if (!BVCalleeSavedGPR.test(Reg)) {
      AllSpilledToReg = false;
      continue;
    }
    unsigned GPRReg = BVAllocatable.find_first();
    if (GPRReg < BVAllocatable.size()) {
      CS.setDstReg(GPRReg);
      BVAllocatable.reset(GPRReg);
      continue;
    } else {
      AllSpilledToReg = false;
    }
  }
  return AllSpilledToReg;
}

bool AIEBaseFrameLowering::spillCalleeSavedRegisters(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
    ArrayRef<CalleeSavedInfo> CSI, const TargetRegisterInfo *TRI) const {
  MachineFunction *MF = MBB.getParent();
  auto *TII = static_cast<const AIEBaseInstrInfo *>(STI.getInstrInfo());
  DebugLoc DL;
  if (auto MBBI = MBB.getLastNonDebugInstr(); MBBI != MBB.end())
    DL = MBBI->getDebugLoc(); // MBBI is valid

  GPRTOCSGPRMap.clear();
  for (const CalleeSavedInfo &Info : CSI) {
    if (Info.isSpilledToReg()) {
      auto &SpilledGPR = GPRTOCSGPRMap[Info.getDstReg()];
      SpilledGPR = Info.getReg();
    }
  }
  for (const CalleeSavedInfo &I : CSI) {
    Register Reg = I.getReg();
    // Add the callee-saved register as live-in; it's killed at the spill.
    // Do not do this for callee-saved registers that are live-in to the
    // function because they will already be marked live-in and this will be
    // adding it for a second time. It is an error to add the same register
    // to the set more than once.
    const MachineRegisterInfo &MRI = MF->getRegInfo();
    bool IsLiveIn = MRI.isLiveIn(Reg);
    if (!MRI.isReserved(Reg) && !IsLiveIn)
      MBB.addLiveIn(Reg);
    if (I.isSpilledToReg()) {
      unsigned Dst = I.getDstReg();

      BuildMI(MBB, MI, DL, TII->get(TII->getPseudoMoveOpcode()), Dst)
          .addReg(GPRTOCSGPRMap[Dst], getKillRegState(true));
    } else {
      const TargetRegisterClass *RC = TRI->getMinimalPhysRegClass(Reg);
      TII->storeRegToStackSlot(MBB, MI, Reg, !IsLiveIn, I.getFrameIdx(), RC,
                               TRI, Register());
    }
  }
  return true;
}

bool AIEBaseFrameLowering::restoreCalleeSavedRegisters(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
    MutableArrayRef<CalleeSavedInfo> CSI, const TargetRegisterInfo *TRI) const {
  auto *TII = static_cast<const AIEBaseInstrInfo *>(STI.getInstrInfo());
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  DebugLoc DL = MBBI->getDebugLoc();
  MachineBasicBlock::iterator I = MI;
  for (CalleeSavedInfo &CI : reverse(CSI)) {
    Register Reg = CI.getReg();
    if (CI.isSpilledToReg()) {
      unsigned Dst = CI.getDstReg();

      if (GPRTOCSGPRMap[Dst] != 0) {
        BuildMI(MBB, I, DL, TII->get(TII->getPseudoMoveOpcode()),
                GPRTOCSGPRMap[Dst])
            .addReg(Dst, getKillRegState(true));
      } else {
        llvm_unreachable("Unexpectedly reached here !");
      }
    } else {
      const TargetRegisterClass *RC = TRI->getMinimalPhysRegClass(Reg);
      TII->loadRegFromStackSlot(MBB, I, Reg, CI.getFrameIdx(), RC, TRI,
                                Register());
      assert(I != MBB.begin() &&
             "loadRegFromStackSlot didn't insert any code!");
    }
  }
  return true;
}

void AIEBaseFrameLowering::emitPrologue(MachineFunction &MF,
                                        MachineBasicBlock &MBB) const {
  assert(&MF.front() == &MBB && "Shrink-wrapping not yet supported");

  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineBasicBlock::iterator MBBI = MBB.begin();
  auto *TII = static_cast<const AIEBaseInstrInfo *>(STI.getInstrInfo());
  auto *RI = static_cast<const AIEBaseRegisterInfo *>(STI.getRegisterInfo());
  Register SPReg = RI->getStackPointerRegister();

  // Debug location must be unknown since the first debug location is used
  // to determine the end of the prologue.
  DebugLoc DL;

  // Determine the correct frame layout
  determineFrameLayout(MF);

#ifndef NDEBUG
  // Verify the full stack size is aligned to the required stack alignment.
  assert(isAligned(getStackAlign(), MFI.getStackSize()) &&
         "Stack size is not properly aligned to stack alignment!");

  // Verify all stack objects have properly aligned SP-relative offsets.
  // This catches bugs in stack slot ordering that could cause misaligned
  // accesses at runtime.
  {
    const int64_t FinalStackSize = MFI.getStackSize();
    for (int FI = MFI.getObjectIndexBegin(); FI < MFI.getObjectIndexEnd();
         ++FI) {
      if (MFI.isDeadObjectIndex(FI))
        continue;
      const int64_t Offset = MFI.getObjectOffset(FI);
      const Align ObjAlign = MFI.getObjectAlign(FI);
      // For AIE stack grows up model: SP-relative = Offset - StackSize
      const int64_t SPRelative = Offset - FinalStackSize;
      assert(isAligned(ObjAlign, std::abs(SPRelative)) &&
             "Stack object is not properly aligned relative to SP!");
    }
  }
#endif

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
  BuildMI(MBB, MBBI, DL, TII->get(TII->getMvSclOpcode()), FPReg)
      .addReg(SPReg)
      .setMIFlag(MachineInstr::FrameSetup);

  adjustReg(MBB, MBBI, DL, FPReg, -StackSize, MachineInstr::FrameSetup);
}

void AIEBaseFrameLowering::emitEpilogue(MachineFunction &MF,
                                        MachineBasicBlock &MBB) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  auto *TII = static_cast<const AIEBaseInstrInfo *>(STI.getInstrInfo());
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

    BuildMI(MBB, LastFrameDestroy, DL, TII->get(TII->getMvSclOpcode()), SPReg)
        .addReg(FPReg)
        .setMIFlag(MachineInstr::FrameSetup);

    adjustSPReg(MBB, MBBI, DL, -StackSize, MachineInstr::FrameDestroy);
    return;
  }

  // Deallocate stack
  adjustSPReg(MBB, MBBI, DL, -StackSize, MachineInstr::FrameDestroy);
}

/// Order stack objects by alignment descending to minimize padding.
///
/// Stack layout on AIE (stack grows downward toward lower addresses):
///   High addresses (allocated first, farther from SP)
///   +------------------------+
///   | High-alignment objects |  <- First in sorted order (alignment
///   descending)
///   +------------------------+
///   | Medium-alignment objs  |
///   +------------------------+
///   | Low-alignment objects  |  <- Last in sorted order (closer to SP)
///   +------------------------+
///   Low addresses (SP points here)
///
/// Objects with smaller alignment end up closer to SP after layout, which is
/// ideal for callee-saved registers that have limited immediate offset range.
void AIEBaseFrameLowering::orderFrameObjects(
    const MachineFunction &MF, SmallVectorImpl<int> &ObjectsToAllocate) const {
  if (ObjectsToAllocate.empty())
    return;

  const MachineFrameInfo &MFI = MF.getFrameInfo();

  // Find the first variable-sized object; only sort fixed-size objects before
  // it.
  const auto FixedSizeEnd = llvm::find_if(
      ObjectsToAllocate, [&MFI](int FI) { return MFI.getObjectSize(FI) <= 0; });

  // Sort fixed-size objects in-place by alignment descending, then size
  // descending, then index.
  llvm::stable_sort(llvm::make_range(ObjectsToAllocate.begin(), FixedSizeEnd),
                    [&MFI](int A, int B) {
                      const Align AlignA = MFI.getObjectAlign(A);
                      const Align AlignB = MFI.getObjectAlign(B);
                      if (AlignA != AlignB)
                        return AlignA > AlignB;
                      const uint64_t SizeA = MFI.getObjectSize(A);
                      const uint64_t SizeB = MFI.getObjectSize(B);
                      if (SizeA != SizeB)
                        return SizeA > SizeB;
                      return A < B;
                    });

  LLVM_DEBUG({
    dbgs() << "AIE orderFrameObjects: ";
    for (int FI : ObjectsToAllocate)
      dbgs() << FI << " ";
    dbgs() << "\n";
  });
}

void AIEBaseFrameLowering::optimizeLRegCalleeSaves(
    MachineFunction &MF, BitVector &SavedRegs,
    const TargetRegisterClass &LRegClass, unsigned SubRegIdxEven,
    unsigned SubRegIdxOdd) const {
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
    if (LRegClass.contains(Reg))
      CalleeSavedLRegs.push_back(Reg);
  }

  for (MCPhysReg LReg : CalleeSavedLRegs) {
    // Get the two GPR subregisters of this L register
    MCPhysReg EvenGPR = TRI->getSubReg(LReg, SubRegIdxEven);
    MCPhysReg OddGPR = TRI->getSubReg(LReg, SubRegIdxOdd);

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
}
