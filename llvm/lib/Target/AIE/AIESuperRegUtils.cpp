//===- AIESuperRegUtils.cpp -----------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
#include "AIESuperRegUtils.h"
#include "AIEBaseInstrInfo.h"
#include "AIEBaseRegisterInfo.h"
#include "llvm/CodeGen/LiveDebugVariables.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/LiveRegMatrix.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "aie-ra"

namespace llvm::AIESuperRegUtils {

/// Returns the subreg indices that can be used to rewrite \p Reg into smaller
/// regs. Returns {} if the rewrite isn't possible.
SmallSet<int, 8> getRewritableSubRegs(Register Reg,
                                      const MachineRegisterInfo &MRI,
                                      const AIEBaseRegisterInfo &TRI,
                                      std::set<Register> &VisitedVRegs) {
  if (Reg.isPhysical()) {
    // TODO: One could use collectSubRegs() in AIEBaseInstrInfo.cpp
    // But given that MOD registers are not part of the ABI, they should
    // not appear as physical registers before RA.
    LLVM_DEBUG(dbgs() << "  Cannot rewrite physreg " << printReg(Reg, &TRI)
                      << "\n");
    return {};
  }

  auto &SubRegSplit = TRI.getSubRegSplit(MRI.getRegClass(Reg)->getID());
  if (SubRegSplit.size() <= 1) {
    // Register does not have multiple subregs to be rewritten into.
    LLVM_DEBUG(dbgs() << "  Cannot rewrite " << printReg(Reg, &TRI, 0, &MRI)
                      << ": no sub-reg split\n");
    return {};
  }

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

  if (!VRM.hasPhys(DstReg)) {
    // FIXME: This pass may cause verification failures. The fix should
    // be in the MachineVerifier. This is a very uncommon case where the
    // destination register was not allocated yet.
    // The machine verifier does not properly handle the semantics of:
    // 1. **Partial register definitions with `undefined`**: When the first
    // subregister is defined with `undefined`, it doesn't expect subsequent
    // definitions to implicitly read that lane.
    // 2. **Lane-based liveness for composite registers**: The verifier expects
    // a continuous live range for the entire register, but with subregister
    // definitions, different lanes have different live ranges that are being
    // built up incrementally.
    // 3. **Implicit reads in partial definitions**: The verifier doesn't
    // recognize that `%18.sub_dim_size:ed = COPY ...` implicitly reads the
    // previously defined `%18.sub_dim_count` lane.
    CopyMI.getMF()->getProperties().set(
        MachineFunctionProperties::Property::FailsVerification);
  }

  MachineInstr *FirstMI = nullptr;
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
            .addReg(DstReg, RegState::Define, SubRegIdx)
            .addReg(SrcReg, 0, SubRegIdx)
            .getInstr();

    // Only set undefined on the first partial copy. The first copy doesn't read
    // other lanes, but subsequent copies do read the previously written lanes.
    // Setting undefined on all copies breaks live interval tracking and causes
    // machine verifier errors.
    if (!FirstMI) {
      PartCopy->getOperand(0).setIsUndef();
      FirstMI = PartCopy;
    }
    LLVM_DEBUG(dbgs() << "        to " << *PartCopy);
    LIS.InsertMachineInstrInMaps(*PartCopy);
    // We need to repair only the Src register. For the Dst register,
    // we don't need to do anything explicit, because we will replace the
    // original copy by the first lane copy in LIS. We avoid the explicit repair
    // of Dst reg because LIS will create a exclusive range for each copy,
    // because it considers that every sub-lane copy will make the preceding
    // one dead, what is not true for composite registers.
    // TODO: investigate why subregister liveness is being ignored by LIS
    // at this point.
    RegistersToRepair.insert(PartCopy->getOperand(1).getReg());
  }

  // Replace the original copy by the first one, so we automatically repair
  // DstReg's LI. This will ensure that the LR will start on first instruction
  // and will not end because the next instruction's slot index.
  LIS.ReplaceMachineInstrInMaps(CopyMI, *FirstMI);
  CopyMI.eraseFromParent();
  // As we don't handle all registers now (selective LI filter),
  // We should make sure that all LiveIntervals are correct.
  // If we don't repair, MI will compose the LIs of some registers,
  // what is not correct because MI was deleted.
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

void rewriteSuperReg(Register Reg, Register AssignedPhysReg,
                     SmallSet<int, 8> &SubRegs, MachineRegisterInfo &MRI,
                     const AIEBaseRegisterInfo &TRI, VirtRegMap &VRM,
                     LiveRegMatrix &LRM, LiveIntervals &LIS,
                     SlotIndexes &Indexes, LiveDebugVariables &DebugVars) {
  LLVM_DEBUG(dbgs() << "Rewriting " << printReg(Reg, &TRI, 0, &MRI) << '\n');
  auto *TII = static_cast<const AIEBaseInstrInfo *>(
      VRM.getMachineFunction().getSubtarget().getInstrInfo());

  // Collect all the subreg indices to rewrite as independent vregs.
  SmallMapVector<int, Register, 8> SubRegToVReg;
  const TargetRegisterClass *SuperRC = MRI.getRegClass(Reg);
  assert(!SubRegs.empty());
  for (int SubReg : SubRegs) {
    const TargetRegisterClass *SubRC = TRI.getSubRegisterClass(SuperRC, SubReg);
    SubRegToVReg[SubReg] = MRI.createVirtualRegister(SubRC);
  }

  // Rewrite full copies into multiple copies using subregs
  for (MachineInstr &MI : make_early_inc_range(MRI.reg_instructions(Reg))) {
    if (MI.isFullCopy())
      rewriteFullCopy(MI, LIS, *TII, TRI, VRM, LRM);
  }

  LLVM_DEBUG(dbgs() << "  Splitting range " << LIS.getInterval(Reg) << "\n");
  for (MachineOperand &RegOp : make_early_inc_range(MRI.reg_operands(Reg))) {
    LLVM_DEBUG(dbgs() << "  Changing " << *RegOp.getParent());
    int SubReg = RegOp.getSubReg();
    assert(SubReg);
    RegOp.setReg(SubRegToVReg[SubReg]);
    RegOp.setSubReg(0);

    // There might have been a write-undefined due to only writing one sub-lane.
    // Now that each sub-lane has its own VReg, the qualifier is invalid.
    if (RegOp.isDef()) {
      RegOp.setIsUndef(false);
      // Also unset correctly the dead flag if the instruction
      // is not the dead slot in the live range (the def is still alive).
      LiveInterval &LI = LIS.getInterval(Reg);
      MachineInstr *DefMI = RegOp.getParent();
      SlotIndex Def = LIS.getInstructionIndex(*DefMI);
      LiveRange::iterator I = LI.FindSegmentContaining(Def);
      if (I->end != Def.getDeadSlot())
        RegOp.setIsDead(false);
    }

    // Make sure the right reg class is applied, some MIs might use compound
    // classes with both 20 and 32 bits registers.
    const TargetRegisterClass *OpRC = TII->getRegClass(
        RegOp.getParent()->getDesc(), RegOp.getParent()->getOperandNo(&RegOp),
        &TRI, VRM.getMachineFunction());
    MRI.constrainRegClass(SubRegToVReg[SubReg], OpRC);

    LLVM_DEBUG(dbgs() << "        to " << *RegOp.getParent());
  }

  VRM.grow();
  LIS.removeInterval(Reg);

  for (auto &[SubRegIdx, VReg] : SubRegToVReg) {
    MCRegister SubPhysReg = TRI.getSubReg(AssignedPhysReg, SubRegIdx);
    LiveInterval &SubRegLI = LIS.getInterval(VReg);
    LLVM_DEBUG(dbgs() << "  Assigning Range: " << SubRegLI << '\n');

    // By giving an independent VReg to each lane, we might have created
    // multiple separate components. Give a VReg to each separate component.
    SmallVector<LiveInterval *, 4> LIComponents;
    LIS.splitSeparateComponents(SubRegLI, LIComponents);
    LIComponents.push_back(&SubRegLI);
    VRM.grow();

    for (LiveInterval *LI : LIComponents) {
      LRM.assign(*LI, SubPhysReg);
      VRM.setRequiredPhys(LI->reg(), SubPhysReg);
      LLVM_DEBUG(dbgs() << "  Assigned " << printReg(LI->reg()) << "\n");
    }
  }

  // Announce new VRegs so DBG locations can be updated.
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
