//===- AIESuperRegUtils.cpp -----------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
#include "AIESuperRegUtils.h"
#include "AIEBaseInstrInfo.h"
#include "AIEBaseRegisterInfo.h"
#include "AIEMachineFunctionInfo.h"
#include "llvm/CodeGen/LiveDebugVariables.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/LiveRegMatrix.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/VirtRegMap.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "aie-ra"

namespace llvm::AIESuperRegUtils {

bool isExpandableRegister(Register Reg, const MachineRegisterInfo &MRI,
                          const AIEBaseRegisterInfo &TRI) {
  if (Reg.isPhysical())
    return false;

  auto &SubRegSplit = TRI.getSubRegSplit(MRI.getRegClass(Reg)->getID());
  return SubRegSplit.size() > 1;
}

/// Returns the subreg indices that can be used to rewrite \p Reg into smaller
/// regs. Returns {} if the rewrite isn't possible.
SmallSet<int, 8> getRewritableSubRegs(Register Reg,
                                      const MachineRegisterInfo &MRI,
                                      const AIEBaseRegisterInfo &TRI,
                                      std::set<Register> &VisitedVRegs) {

  if (!isExpandableRegister(Reg, MRI, TRI)) {
    LLVM_DEBUG(dbgs() << "  Cannot rewrite " << printReg(Reg, &TRI, 0, &MRI)
                      << ": no sub-reg split\n");
    return {};
  }

  auto &SubRegSplit = TRI.getSubRegSplit(MRI.getRegClass(Reg)->getID());

  VisitedVRegs.insert(Reg);
  SmallSet<int, 8> UsedSubRegs;
  for (MachineOperand &RegOp : MRI.reg_operands(Reg)) {
    const unsigned SubReg = RegOp.getSubReg();
    if (SubReg && SubRegSplit.count(SubReg)) {
      UsedSubRegs.insert(SubReg);
    } else if (RegOp.getParent()->isFullCopy()) {
      // To rewrite a full copy, both operands need to be rewritable using
      // their subregs.
      Register DstReg = RegOp.getParent()->getOperand(0).getReg();
      if (!VisitedVRegs.count(DstReg) &&
          getRewritableSubRegs(DstReg, MRI, TRI, VisitedVRegs).empty()) {
        LLVM_DEBUG(dbgs() << "  Cannot rewrite "
                          << printReg(DstReg, &TRI, 0, &MRI) << " in "
                          << *RegOp.getParent());
        return {};
      }
      Register SrcReg = RegOp.getParent()->getOperand(1).getReg();
      if (!VisitedVRegs.count(SrcReg) &&
          getRewritableSubRegs(SrcReg, MRI, TRI, VisitedVRegs).empty()) {
        LLVM_DEBUG(dbgs() << "  Cannot rewrite "
                          << printReg(SrcReg, &TRI, 0, &MRI) << " in "
                          << *RegOp.getParent());
        return {};
      }
      UsedSubRegs.insert(SubRegSplit.begin(), SubRegSplit.end());
    } else {
      // TODO: could we move further and handle cases like:
      // %A.sub_hi_dim:eds = COPY %B:ed.
      LLVM_DEBUG(dbgs() << "  Cannot rewrite " << RegOp << " in "
                        << *RegOp.getParent());
      return {};
    }
  }

  return UsedSubRegs;
}

SmallSet<int, 8> getRewritableSubRegs(Register Reg,
                                      const MachineRegisterInfo &MRI,
                                      const AIEBaseRegisterInfo &TRI) {
  std::set<Register> VisitedVRegs;
  return getRewritableSubRegs(Reg, MRI, TRI, VisitedVRegs);
}

/// Rewrite a full copy into multiple copies using the subregs in \p CopySubRegs
void rewriteFullCopy(MachineInstr &CopyMI, LiveIntervals &LIS,
                     const TargetInstrInfo &TII, const AIEBaseRegisterInfo &TRI,
                     VirtRegMap &VRM, LiveRegMatrix &LRM) {
  assert(CopyMI.isFullCopy());
  SlotIndex CopyIndex = LIS.getInstructionIndex(CopyMI);
  LLVM_DEBUG(dbgs() << "  Changing full copy at " << CopyIndex << ": "
                    << CopyMI);
  Register DstReg = CopyMI.getOperand(0).getReg();
  Register SrcReg = CopyMI.getOperand(1).getReg();
  LaneBitmask LiveSrcLanes = getLiveLanesAt(CopyIndex, SrcReg, LIS);
  MachineRegisterInfo &MRI = CopyMI.getMF()->getRegInfo();
  const std::set<int> CopySubRegs =
      TRI.getSubRegSplit(MRI.getRegClass(DstReg)->getID());

  unsigned AdditionalFlags = RegState::Undef;
  SmallSet<Register, 8> RegistersToRepair;
  for (int SubRegIdx : CopySubRegs) {
    if ((LiveSrcLanes & TRI.getSubRegIndexLaneMask(SubRegIdx)).none()) {
      LLVM_DEBUG(dbgs() << "        Skip undefined subreg "
                        << TRI.getSubRegIndexName(SubRegIdx) << "\n");
      continue;
    }

    MachineInstr *PartCopy =
        BuildMI(*CopyMI.getParent(), CopyMI, CopyMI.getDebugLoc(),
                TII.get(TargetOpcode::COPY))
            .addReg(DstReg, RegState::Define | AdditionalFlags, SubRegIdx)
            .addReg(SrcReg, 0, SubRegIdx)
            .getInstr();
    // Only for the first copy set the undefined flag
    AdditionalFlags = 0;

    LLVM_DEBUG(dbgs() << "        to " << *PartCopy);
    LIS.InsertMachineInstrInMaps(*PartCopy);
    // Since we modified Source and Destination registers, we need to repair
    // both LiveIntervals
    RegistersToRepair.insert(PartCopy->getOperand(1).getReg());
    RegistersToRepair.insert(PartCopy->getOperand(0).getReg());
  }

  LLVM_DEBUG(dbgs() << "  Erasing copy at " << CopyIndex << ": " << CopyMI
                    << "\n");
  LIS.RemoveMachineInstrFromMaps(CopyMI);
  CopyMI.eraseFromParent();

  // Update Liveinterval of all modified Registers
  repairLiveIntervals(RegistersToRepair, VRM, LRM, LIS);
}

/// Return a mask of all the lanes that are live at \p Index
LaneBitmask getLiveLanesAt(SlotIndex Index, Register Reg,
                           const LiveIntervals &LIS) {
  const LiveInterval &LI = LIS.getInterval(Reg);
  if (!LI.hasSubRanges())
    return LaneBitmask::getAll();

  LaneBitmask LiveLanes;
  for (const LiveInterval::SubRange &SubLI : LI.subranges()) {
    if (SubLI.liveAt(Index))
      LiveLanes |= SubLI.LaneMask;
  }
  return LiveLanes;
}

/// Check if a live interval is effectively empty.
/// Returns true if:
/// - The interval has no segments, or
/// - All segments are confined to a single instruction (e.g. start == end or
///   go from R to D in the same SlotIndex)
static bool isLiveIntervalEffectiveEmpty(const LiveInterval &LI) {
  // Check if the main interval is empty (no segments)
  if (LI.empty() || LI.begin() == LI.end())
    return true;

  // Check if all segments have start == end (R to D at same slot)
  // If any segment has start != end, the interval is not empty
  for (const LiveInterval::Segment &Seg : LI.segments) {
    // If the segment stays within the same instruction, treat it as empty
    // (this covers R->D transitions at the same SlotIndex).
    if (Seg.start == Seg.end || SlotIndex::isSameInstr(Seg.start, Seg.end))
      continue;

    if (Seg.start != Seg.end) {
      LLVM_DEBUG(dbgs() << "  Segment " << Seg << " is not empty\n"
                        << " start: " << Seg.start << " end: " << Seg.end
                        << "\n");
      return false;
    }
  }

  // All segments have start == end, so interval is effectively empty
  return true;
}

/// Create virtual registers for each subregister index.
static SmallMapVector<int, Register, 8>
createSubRegisterVRegs(Register Reg, const SmallSet<int, 8> &SubRegs,
                       std::optional<Register> AssignedPhysReg,
                       MachineRegisterInfo &MRI, const AIEBaseRegisterInfo &TRI,
                       MachineFunction &MF) {
  SmallMapVector<int, Register, 8> SubRegToVReg;
  const TargetRegisterClass *SuperRC = MRI.getRegClass(Reg);
  assert(!SubRegs.empty());

  for (int SubReg : SubRegs) {
    const TargetRegisterClass *SubRC =
        AssignedPhysReg.has_value()
            ? TRI.getSubRegisterClass(SuperRC, SubReg)
            : TRI.getLargestLegalSuperClass(
                  TRI.getSubRegisterClass(SuperRC, SubReg), MF);
    SubRegToVReg[SubReg] = MRI.createVirtualRegister(SubRC);
  }

  return SubRegToVReg;
}

/// Rewrite operands to use the new subregister virtual registers.
static void rewriteOperandsToSubRegs(
    Register Reg, SmallMapVector<int, Register, 8> &SubRegToVReg,
    MachineRegisterInfo &MRI, const AIEBaseRegisterInfo &TRI,
    const TargetInstrInfo &TII, VirtRegMap &VRM) {
  for (MachineOperand &RegOp : make_early_inc_range(MRI.reg_operands(Reg))) {
    LLVM_DEBUG(dbgs() << printReg(RegOp.getReg(), &TRI, 0, &MRI)
                      << "  Changing " << *RegOp.getParent());
    int SubReg = RegOp.getSubReg();
    assert(SubReg);
    RegOp.setReg(SubRegToVReg[SubReg]);
    RegOp.setSubReg(0);

    // There might have been a write-undefined due to only writing one sub-lane.
    // Now that each sub-lane has its own VReg, the qualifier is invalid.
    if (RegOp.isDef()) {
      RegOp.setIsUndef(false);
    }

    // Make sure the right reg class is applied, some MIs might use compound
    // classes with both 20 and 32 bits registers.
    const TargetRegisterClass *OpRC = TII.getRegClass(
        RegOp.getParent()->getDesc(), RegOp.getParent()->getOperandNo(&RegOp),
        &TRI, VRM.getMachineFunction());
    MRI.constrainRegClass(SubRegToVReg[SubReg], OpRC);

    LLVM_DEBUG(dbgs() << "        to " << *RegOp.getParent());
  }
}

/// Collect effectivly empty subregisters and copy instructions that define
/// them. These are characterized by a liveinterval From R to D of the same
/// slotindex and originate in bundled Instructions.
static void collectEffectiveEmptyCopies(
    SmallMapVector<int, Register, 8> &SubRegToVReg,
    SmallVector<MachineInstr *, 8> &CopiesToMarkDead, MachineRegisterInfo &MRI,
    const AIEBaseRegisterInfo &TRI, LiveIntervals &LIS) {
  for (auto &[SubRegIdx, VReg] : SubRegToVReg) {
    // Ensure the interval is computed before checking
    if (!LIS.hasInterval(VReg)) {
      LIS.createAndComputeVirtRegInterval(VReg);
    }

    LiveInterval &SubRegLI = LIS.getInterval(VReg);

    // Check if this subregister has an empty live interval
    if (!isLiveIntervalEffectiveEmpty(SubRegLI)) {
      LLVM_DEBUG(dbgs() << "  Subregister " << SubRegLI
                        << " is not empty, skipping\n");
      continue;
    }

    LLVM_DEBUG(dbgs() << "  Found empty subregister " << SubRegIdx
                      << " with VReg " << printReg(VReg, &TRI, 0, &MRI)
                      << " Range: " << SubRegLI << '\n');

    // Find all copy instructions that define effective empty subregisters.
    // Effective empty: Liveinterval from R to D within the same SlotIndex,
    // occurs with bundled MachineInstructions. For COPY instructions, operand 0
    // is always the destination (def)
    for (MachineInstr &MI : MRI.reg_nodbg_instructions(VReg)) {
      if (!MI.isCopy())
        continue;
      if (!MI.getOperand(0).isDef())
        continue;
      if (MI.getOperand(0).getReg() != VReg)
        continue;

      LLVM_DEBUG(dbgs() << "    Marking copy for deletion: " << MI << '\n');
      CopiesToMarkDead.push_back(&MI);
    }
  }
}

/// Mark copy instructions that define empty live intervals as dead.
static void markCopiesDead(SmallVector<MachineInstr *, 8> &CopiesToDelete) {
  for (MachineInstr *CopyMI : CopiesToDelete) {
    assert(CopyMI->isCopy() && "Expected copy instruction");

    LLVM_DEBUG(dbgs() << "  Marking copy as dead: " << *CopyMI << '\n');
    CopyMI->getOperand(0).setIsDead(true);
  }
}

static void markEffectiveEmptyCopiesDead(
    SmallMapVector<int, Register, 8> &SubRegToVReg, MachineRegisterInfo &MRI,
    const AIEBaseRegisterInfo &TRI, LiveIntervals &LIS) {

  // Collect copies MI that define empty live intervals
  SmallVector<MachineInstr *, 8> CopiesToDelete;
  collectEffectiveEmptyCopies(SubRegToVReg, CopiesToDelete, MRI, TRI, LIS);

  // Mark the copies as dead
  markCopiesDead(CopiesToDelete);
}

/// Process non-empty subregisters: split components and assign physical
/// registers.
static void
splitAndAssignSubPhysRegs(SmallMapVector<int, Register, 8> &SubRegToVReg,
                          std::optional<Register> AssignedPhysReg,
                          const AIEBaseRegisterInfo &TRI, VirtRegMap &VRM,
                          LiveRegMatrix &LRM, LiveIntervals &LIS) {
  for (auto &[SubRegIdx, VReg] : SubRegToVReg) {
    LiveInterval &SubRegLI = LIS.getInterval(VReg);
    LLVM_DEBUG(dbgs() << "  Assigning Range: " << SubRegLI << '\n');

    // By giving an independent VReg to each lane, we might have created
    // multiple separate components. Give a VReg to each separate component.
    SmallVector<LiveInterval *, 4> LIComponents;
    LIS.splitSeparateComponents(SubRegLI, LIComponents);
    LIComponents.push_back(&SubRegLI);
    // todo: there is a bug in splitSeparateComponents, so we have to manually
    // grow the VRM (due to abstraction complexity on MRI::Delegate "protocol")
    VRM.grow();

    if (!AssignedPhysReg.has_value())
      continue;

    MCRegister SubPhysReg = TRI.getSubReg(*AssignedPhysReg, SubRegIdx);
    for (LiveInterval *LI : LIComponents) {
      LRM.assign(*LI, SubPhysReg);
      VRM.setRequiredPhys(LI->reg(), SubPhysReg);
      LLVM_DEBUG(dbgs() << "  Assigned " << *LI << " \n");
    }
  }
}

void rewriteSuperReg(Register Reg, std::optional<Register> AssignedPhysReg,
                     SmallSet<int, 8> &SubRegs, MachineRegisterInfo &MRI,
                     const AIEBaseRegisterInfo &TRI, VirtRegMap &VRM,
                     LiveRegMatrix &LRM, LiveIntervals &LIS,
                     SlotIndexes &Indexes, LiveDebugVariables &DebugVars) {
  LLVM_DEBUG({
    dbgs() << "Rewriting " << printReg(Reg, &TRI, 0, &MRI)
           << " with sublanes:\n";
    if (LIS.hasInterval(Reg))
      LIS.getInterval(Reg).dump();
    for (int SubRegIdx : SubRegs) {
      dbgs() << "  Sublane Index: " << SubRegIdx
             << ", Name: " << TRI.getSubRegIndexName(SubRegIdx)
             << ", Mask: " << TRI.getSubRegIndexLaneMask(SubRegIdx) << "\n";
    }
  });
  MachineFunction &MF = VRM.getMachineFunction();
  auto *TII =
      static_cast<const AIEBaseInstrInfo *>(MF.getSubtarget().getInstrInfo());

  // Step 1: Create virtual registers for each subregister
  SmallMapVector<int, Register, 8> SubRegToVReg =
      createSubRegisterVRegs(Reg, SubRegs, AssignedPhysReg, MRI, TRI, MF);

  // Step 2: Rewrite full copies into multiple copies using subregs
  for (MachineInstr &MI : make_early_inc_range(MRI.reg_instructions(Reg))) {
    if (MI.isFullCopy())
      rewriteFullCopy(MI, LIS, *TII, TRI, VRM, LRM);
  }

  // Step 3: Rewrite operands to use the new subregister virtual registers
  LLVM_DEBUG(dbgs() << "  Splitting range " << LIS.getInterval(Reg) << "\n");
  rewriteOperandsToSubRegs(Reg, SubRegToVReg, MRI, TRI, *TII, VRM);

  // Step 4: Remove the original register's live interval
  LIS.removeInterval(Reg);

  // Step 5: Filter out empty subregisters
  markEffectiveEmptyCopiesDead(SubRegToVReg, MRI, TRI, LIS);

  // Step 6: Process remaining non-empty subregisters
  splitAndAssignSubPhysRegs(SubRegToVReg, AssignedPhysReg, TRI, VRM, LRM, LIS);

  // Step 7: Update debug variables with non-empty subregisters
  auto NewVRegs = SmallVector<Register, 8>(llvm::map_range(
      SubRegToVReg, [&](auto &Mapping) { return Mapping.second; }));
  DebugVars.splitRegister(Reg, NewVRegs, LIS);
}

bool isRegUsedBy2DOr3DInstruction(const MachineRegisterInfo &MRI,
                                  const Register &R) {

  return llvm::any_of(
      MRI.use_nodbg_instructions(R), [&](const MachineInstr &MI) {
        auto &TII = *static_cast<const AIEBaseInstrInfo *>(
            MI.getMF()->getSubtarget().getInstrInfo());

        // We should recognize both cases, with and without splitting. A 2D/3D
        // instruction will always be split or splittable.
        return TII.getOpcodeWithTupleOperands(MI.getOpcode()).has_value() ||
               TII.getOpcodeWithAtomicOperands(MI.getOpcode()).has_value();
      });
}

void clearStaleSplitFromMappings(const SmallSet<Register, 8> &TaintedOriginals,
                                 MachineFunction &MF, MachineRegisterInfo &MRI,
                                 VirtRegMap &VRM) {
  if (TaintedOriginals.empty())
    return;

  // Record the pre-severance "logical group original" in the target-side
  // side map so that InlineSpiller (via TargetSubtargetInfo::
  // getSpillGroupOriginal) can still merge sibling spills onto a shared
  // stack slot after we cut the VRM split-from chain below.
  auto *MFI = MF.getInfo<AIEMachineFunctionInfo>();

  for (unsigned I = 0, E = MRI.getNumVirtRegs(); I != E; ++I) {
    const Register V = Register::index2VirtReg(I);
    if (MRI.reg_nodbg_empty(V))
      continue;
    const Register Orig = VRM.getPreSplitReg(V);
    if (!Orig || !TaintedOriginals.count(Orig))
      continue;

    LLVM_DEBUG({
      const TargetRegisterInfo *TRI = MRI.getTargetRegisterInfo();
      dbgs() << "  Clearing stale split-from for " << printReg(V, TRI, 0, &MRI)
             << " (was split from " << printReg(Orig, TRI, 0, &MRI)
             << "); recorded for spill-group sharing\n";
    });
    // Remember the chain so InlineSpiller can still group V's spills with
    // the rest of Orig's descendants on a shared stack slot.
    if (MFI)
      MFI->recordSpillGroupOriginal(V, Orig);
    // Restore V to the canonical "no split parent" state so getOriginal(V)==V
    // and SplitKit::defFromParent stops consulting the (stale) ancestor LI.
    VRM.clearSplitFromReg(V);
  }
}

void repairLiveIntervals(SmallSet<Register, 8> &RegistersToRepair,
                         VirtRegMap &VRM, LiveRegMatrix &LRM,
                         LiveIntervals &LIS) {
  for (Register R : RegistersToRepair) {

    if (!LIS.hasInterval(R))
      continue;

    if (VRM.hasPhys(R)) {
      const MCRegister PhysReg = VRM.getPhys(R);
      const LiveInterval &OldLI = LIS.getInterval(R);
      LRM.unassign(OldLI);
      LIS.removeInterval(R);
      const LiveInterval &LI = LIS.createAndComputeVirtRegInterval(R);
      LRM.assign(LI, PhysReg);
    } else {
      LIS.removeInterval(R);
      LIS.createAndComputeVirtRegInterval(R);
    }

    // After recomputing, shrink the interval to remove any invalid segments
    // This is important for registers with undefined definitions.
    LIS.shrinkToUses(&LIS.getInterval(R));
  }
}

} // namespace llvm::AIESuperRegUtils
