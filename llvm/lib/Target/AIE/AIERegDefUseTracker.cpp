//===- AIERegDefUseTracker.cpp - Track Register Live Ranges --------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements tracking and analysis of register live ranges in a
// MachineBasicBlock. The tracker performs the following:
// - Identifies register definitions and uses that form live ranges
// - Merges aliasing register accesses into unified live ranges
// - Filters out unsafe ranges (tied operands, live-in/out)
// - Marks ranges with implicit operands as non-virtualizable
// - Computes appropriate register classes for each live range
// - Optionally replaces physical registers with virtual registers for testing
//
//===----------------------------------------------------------------------===//

#include "AIERegDefUseTracker.h"
#include "AIEBaseInstrInfo.h"
#include "AIEBaseRegisterInfo.h"
#include "Utils/AIEMachineInstrPrint.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/MC/LaneBitmask.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "aie-reg-liverange"

using namespace llvm;

namespace {

/// Return the sub-register index of AccessReg within BaseReg, or 0 if
/// AccessReg equals BaseReg or is not a sub-register of BaseReg.
unsigned getSubRegIdx(MCRegister AccessReg, MCRegister BaseReg,
                      const TargetRegisterInfo *TRI) {
  if (AccessReg == BaseReg)
    return 0;
  for (MCSubRegIndexIterator SubRegIdxIt(BaseReg, TRI); SubRegIdxIt.isValid();
       ++SubRegIdxIt) {
    if (SubRegIdxIt.getSubReg() == AccessReg)
      return SubRegIdxIt.getSubRegIndex();
  }
  return 0;
}

/// Return true if every register in Regs equals Candidate or is a
/// sub-register of Candidate.
bool containsAll(MCRegister Candidate, ArrayRef<MCRegister> Regs,
                 const TargetRegisterInfo *TRI) {
  return llvm::all_of(Regs, [Candidate, TRI](MCRegister R) {
    return R == Candidate || getSubRegIdx(R, Candidate, TRI);
  });
}

/// Return the smallest strict super-register of Start that contains all
/// registers in Regs, or NoRegister if none exists.
MCRegister findSmallestSuperRegContaining(MCRegister Start,
                                          ArrayRef<MCRegister> Regs,
                                          const TargetRegisterInfo *TRI) {
  for (MCSuperRegIterator SuperIt(Start, TRI); SuperIt.isValid(); ++SuperIt) {
    if (containsAll(*SuperIt, Regs, TRI))
      return *SuperIt;
  }
  return MCRegister::NoRegister;
}

} // end anonymous namespace

void RegLiveRange::dumpBrief(const TargetRegisterInfo *TRI) const {
  StringRef Name =
      (BaseReg != MCRegister::NoRegister) ? TRI->getName(BaseReg) : "unknown";

  dbgs() << "  - LR#" << ID << " Base=" << Name << " defs=" << getNumDefs()
         << " uses=" << getNumUses();

  if (IsReserved) {
    dbgs() << " [RESERVED]";
  }

  // Print first def if available
  if (!Defs.empty()) {
    const MachineInstr *MI = Defs[0].getOperand()->getParent();
    assert(MI && "Def operand must have a parent instruction");
    dbgs() << " firstDef: " << AIE::NoDebug(*MI);
  }

  dbgs() << "\n";
}

static cl::opt<std::string> ExcludeLiveRangesByRegClass(
    "aie-exclude-liveranges-by-regclass", cl::Hidden, cl::init(""),
    cl::desc("[AIE] Exclude live ranges of the specified register class name. "
             "Empty string means no filtering."));

static cl::opt<bool> AddUnusedCallerSavedRegs(
    "aie-add-unused-caller-saved-regs", cl::Hidden, cl::init(false),
    cl::desc("[AIE] Add unused caller-saved registers to the available "
             "register pool for pipelining. Only safe when loops with calls "
             "are excluded from pipelining."));

static cl::opt<unsigned> MaxScarceRangeCandidates(
    "aie-max-scarce-range-candidates", cl::Hidden, cl::init(4),
    cl::desc("[AIE] Maximum number of scarce range scheduling candidates. "
             "Bounds the complexity of the scarce range scheduler."));

RegLiveRangeTracker::RegLiveRangeTracker(MachineFunction &MF)
    : MF(&MF), TRI(static_cast<const AIEBaseRegisterInfo *>(
                   MF.getSubtarget().getRegisterInfo())),
      TII(static_cast<const AIEBaseInstrInfo *>(
          MF.getSubtarget().getInstrInfo())) {
  assert(TRI && "TargetRegisterInfo cannot be null");
  assert(TII && "TargetInstrInfo cannot be null");
}

void RegLiveRange::addDef(MachineOperand *DefOp, unsigned SubRegIdx) {
  Defs.emplace_back(DefOp, SubRegIdx);
}

void RegLiveRange::addUse(MachineOperand *UseOp, unsigned SubRegIdx) {
  Uses.emplace_back(UseOp, SubRegIdx);
}

bool RegLiveRange::mergeFrom(const RegLiveRange &Other,
                             const TargetRegisterInfo *TRI) {
  assert(BaseReg != MCRegister::NoRegister &&
         "mergeFrom requires a valid base register on this range");
  assert(Other.BaseReg != MCRegister::NoRegister &&
         "mergeFrom requires a valid base register on Other");

  // Collect all operand registers from both ranges.
  SmallVector<MCRegister, 8> AllOperandRegs;
  for (const auto &OpInfo : operands()) {
    AllOperandRegs.push_back(OpInfo.getOperand()->getReg().asMCReg());
  }
  for (const auto &OpInfo : Other.operands()) {
    AllOperandRegs.push_back(OpInfo.getOperand()->getReg().asMCReg());
  }

  // Compute the new base register: the smallest register that contains all
  // operand registers. Start with the current base register and upgrade to
  // Other.BaseReg if it is a strict super-register.
  MCRegister NewBaseReg = BaseReg;
  if (getSubRegIdx(NewBaseReg, Other.BaseReg, TRI)) {
    NewBaseReg = Other.BaseReg;
  }

  // If the current NewBaseReg doesn't contain all operands (e.g., sibling
  // registers like cml4 and cmh4), find the smallest common super-register.
  if (!containsAll(NewBaseReg, AllOperandRegs, TRI)) {
    // Search for the smallest super-register that contains all operands.
    // MCSuperRegIterator yields them from smallest to largest.
    NewBaseReg =
        findSmallestSuperRegContaining(NewBaseReg, AllOperandRegs, TRI);
    if (NewBaseReg == MCRegister::NoRegister)
      return false;
  }

  // Re-add existing operands with updated sub-register indices if base
  // changed.
  if (NewBaseReg != BaseReg) {
    SmallVector<RegOperandInfo, 4> OldDefs = std::move(Defs);
    SmallVector<RegOperandInfo, 4> OldUses = std::move(Uses);
    Defs.clear();
    Uses.clear();

    for (const auto &DefInfo : OldDefs) {
      const MCRegister DefReg = DefInfo.getOperand()->getReg().asMCReg();
      Defs.emplace_back(DefInfo.getOperand(),
                        getSubRegIdx(DefReg, NewBaseReg, TRI));
    }
    for (const auto &UseInfo : OldUses) {
      const MCRegister UseReg = UseInfo.getOperand()->getReg().asMCReg();
      Uses.emplace_back(UseInfo.getOperand(),
                        getSubRegIdx(UseReg, NewBaseReg, TRI));
    }

    BaseReg = NewBaseReg;
  }

  // Merge defs from Other with computed sub-register indices.
  for (const auto &DefInfo : Other.defs()) {
    const MCRegister DefReg = DefInfo.getOperand()->getReg().asMCReg();
    Defs.emplace_back(DefInfo.getOperand(),
                      getSubRegIdx(DefReg, NewBaseReg, TRI));
  }

  // Merge uses from Other with computed sub-register indices.
  for (const auto &UseInfo : Other.uses()) {
    const MCRegister UseReg = UseInfo.getOperand()->getReg().asMCReg();
    Uses.emplace_back(UseInfo.getOperand(),
                      getSubRegIdx(UseReg, NewBaseReg, TRI));
  }

  // Propagate reserved status: if Other is reserved, this becomes reserved.
  if (Other.IsReserved) {
    IsReserved = true;
  }
  return true;
}

bool RegLiveRange::expandBaseToInclude(MCRegister ExtReg,
                                       const TargetRegisterInfo *TRI) {
  assert(BaseReg != MCRegister::NoRegister &&
         "expandBaseToInclude requires a valid base register");
  assert(ExtReg != MCRegister::NoRegister &&
         "expandBaseToInclude requires a valid ExtReg");

  // If ExtReg is already contained by BaseReg, nothing to do.
  if (ExtReg == BaseReg || getSubRegIdx(ExtReg, BaseReg, TRI))
    return true;

  // If BaseReg is contained by ExtReg, upgrade to ExtReg.
  if (getSubRegIdx(BaseReg, ExtReg, TRI)) {
    // Recompute SubRegIdx for existing operands.
    SmallVector<RegOperandInfo, 4> OldDefs = std::move(Defs);
    SmallVector<RegOperandInfo, 4> OldUses = std::move(Uses);
    Defs.clear();
    Uses.clear();

    for (const auto &DefInfo : OldDefs) {
      const MCRegister DefReg = DefInfo.getOperand()->getReg().asMCReg();
      Defs.emplace_back(DefInfo.getOperand(),
                        getSubRegIdx(DefReg, ExtReg, TRI));
    }
    for (const auto &UseInfo : OldUses) {
      const MCRegister UseReg = UseInfo.getOperand()->getReg().asMCReg();
      Uses.emplace_back(UseInfo.getOperand(),
                        getSubRegIdx(UseReg, ExtReg, TRI));
    }

    BaseReg = ExtReg;
    return true;
  }

  // Neither is a subreg of the other - find the smallest common super-register.
  // Collect all operand registers plus ExtReg.
  SmallVector<MCRegister, 8> AllRegs;
  AllRegs.push_back(ExtReg);
  for (const auto &OpInfo : operands()) {
    AllRegs.push_back(OpInfo.getOperand()->getReg().asMCReg());
  }

  // Search for the smallest super-register that contains all.
  const MCRegister NewBaseReg =
      findSmallestSuperRegContaining(BaseReg, AllRegs, TRI);
  if (NewBaseReg == MCRegister::NoRegister)
    return false;

  // Recompute SubRegIdx for existing operands.
  if (NewBaseReg != BaseReg) {
    SmallVector<RegOperandInfo, 4> OldDefs = std::move(Defs);
    SmallVector<RegOperandInfo, 4> OldUses = std::move(Uses);
    Defs.clear();
    Uses.clear();

    for (const auto &DefInfo : OldDefs) {
      const MCRegister DefReg = DefInfo.getOperand()->getReg().asMCReg();
      Defs.emplace_back(DefInfo.getOperand(),
                        getSubRegIdx(DefReg, NewBaseReg, TRI));
    }
    for (const auto &UseInfo : OldUses) {
      const MCRegister UseReg = UseInfo.getOperand()->getReg().asMCReg();
      Uses.emplace_back(UseInfo.getOperand(),
                        getSubRegIdx(UseReg, NewBaseReg, TRI));
    }

    BaseReg = NewBaseReg;
  }
  return true;
}

void RegLiveRange::clear() {
  Defs.clear();
  Uses.clear();
  BaseReg = MCRegister::NoRegister;
  RegisterClass = nullptr;
  AdmissibleRegs.clear();
  VReg = Register();
  IsScarce = false;
  IsReserved = false;
  ID = -1;
}

unsigned RegLiveRangeTracker::getSubRegIndex(MCRegister AccessReg,
                                             MCRegister BaseReg) const {
  return ::getSubRegIdx(AccessReg, BaseReg, TRI);
}

PartSet RegLiveRangeTracker::getRegUnits(MCRegister Reg) const {
  PartSet Units;
  for (MCRegUnitIterator UI(Reg, TRI); UI.isValid(); ++UI)
    Units.set(RegUnit(*UI));
  return Units;
}

PartSet
RegLiveRangeTracker::getRegUnits(const DenseSet<MCRegister> &Regs) const {
  PartSet Units;
  for (const MCRegister Reg : Regs)
    Units |= getRegUnits(Reg);
  return Units;
}

PartSet RegLiveRangeTracker::getRegUnits(MCRegister Reg,
                                         LaneBitmask Lanes) const {
  PartSet Units;
  for (MCRegUnitMaskIterator UI(Reg, TRI); UI.isValid(); ++UI) {
    const auto [Unit, UnitMask] = *UI;
    if (UnitMask.none() || (UnitMask & Lanes).any())
      Units.set(RegUnit(Unit));
  }
  return Units;
}

PartSet
RegLiveRangeTracker::getLiveInUnits(const MachineBasicBlock &MBB) const {
  PartSet Units;
  for (const auto &LiveIn : MBB.liveins())
    Units |= getRegUnits(LiveIn.PhysReg, LiveIn.LaneMask);
  return Units;
}

PartSet
RegLiveRangeTracker::getLiveOutUnits(const MachineBasicBlock &MBB) const {
  PartSet Units;
  for (const auto &LiveOut : MBB.liveouts())
    Units |= getRegUnits(LiveOut.PhysReg, LiveOut.LaneMask);
  return Units;
}

PartSet
RegLiveRangeTracker::getTopLiveUnits(const LivenessScanState &State) const {
  PartSet Units = State.OutLive;
  for (const PartSet &LiveUnits : State.LiveUnits)
    Units |= LiveUnits;
  return Units;
}

bool RegLiveRangeTracker::isFullyDefined(const PartSet &TopLiveUnits,
                                         const PartSet &LiveInUnits) {
  // A live range is fully defined if the register units that are still live
  // at the top of the block do not intersect with the live-in units of the
  // block.
  //
  // This is more precise than just checking register overlap: it allows
  // ranges where the live units are disjoint from the live-in units.
  //
  // Importantly, this can discriminate between a truly undefined register
  // (which is not in the live-in set and is safe to virtualize) and a
  // register that was defined outside of the loop (which is in the live-in
  // set and should be rejected because changing it would affect loop-carried
  // values).
  return !TopLiveUnits.overlap(LiveInUnits);
}

bool RegLiveRangeTracker::hasTiedOperands(const RegLiveRange &LR) const {
  assert(TII);

  // Check if any operand in this live range is tied
  for (const auto &Def : LR.defs()) {
    MachineOperand *MO = Def.getOperand();
    if (MO->isTied())
      return true;

    MachineInstr *MI = MO->getParent();
    assert(MI);

    // Get the operand index for this def
    unsigned OpIdx = MO->getOperandNo();

    // Check AIE-specific tied register info
    const auto TiedInfo = TII->getTiedRegInfo(*MI);
    for (const auto &TiedSet : TiedInfo) {
      // Check if this operand is in the destination operands of a tied set
      for (const auto &DstOp : TiedSet.DstOps) {
        if (DstOp.OpIdx == OpIdx)
          return true;
      }
      // Check if this operand is in the source operands of a tied set
      for (const auto &SrcOp : TiedSet.SrcOps) {
        if (SrcOp.OpIdx == OpIdx)
          return true;
      }
    }

    const MCRegister R = MO->getReg().asMCReg();
    const int DefIdx = MI->findRegisterDefOperandIdx(R, TRI);
    if (DefIdx >= 0 && MI->isRegTiedToUseOperand(DefIdx))
      return true;
  }

  // Also check uses for tied operands
  for (const auto &Use : LR.uses()) {
    MachineOperand *MO = Use.getOperand();

    // Get the operand index for this use
    unsigned OpIdx = MO->getOperandNo();

    MachineInstr *MI = Use.getOperand()->getParent();
    assert(MI);

    // Check AIE-specific tied register info.
    // Skip use-only tied sets (empty DstOps): those tie source operands
    // together (e.g., two sub-registers of the same superregister) but do
    // not constrain any def, so they do not glue live ranges together.
    const auto TiedInfo = TII->getTiedRegInfo(*MI);
    for (const auto &TiedSet : TiedInfo) {
      if (TiedSet.DstOps.empty())
        continue;
      for (const auto &SrcOp : TiedSet.SrcOps) {
        if (SrcOp.OpIdx == OpIdx)
          return true;
      }
    }
  }

  return false;
}

void RegLiveRangeTracker::pruneByFullCoverage() {
  LLVM_DEBUG(dbgs() << "\nPrune by full coverage: " << LiveRanges.size()
                    << " ranges before pruning\n");

  // We run this in a fixed point loop, since pruning a range may uncover ranges
  // that were previously covered by it.
  bool Changed = true;
  while (Changed) {
    Changed = false;

    // Build coverage map from current LiveRanges
    DenseSet<MachineOperand *> CoveredOps;
    for (const RegLiveRange &LR : LiveRanges) {
      for (const auto &R : LR.operands()) {
        CoveredOps.insert(R.getOperand());
      }
    }

    // Check if there are any uncovered operands that alias with this LR's
    // registers
    auto HasUncoveredAlias = [&](const PartSet &LRUnits,
                                 MCRegister *SampleUncovered = nullptr) {
      for (MachineOperand *MO : AllPhysRegOperands) {
        if (CoveredOps.contains(MO))
          continue;
        // Check if this uncovered operand shares units with this LR.
        const MCRegister UncoveredReg = MO->getReg().asMCReg();
        if (getRegUnits(UncoveredReg).overlap(LRUnits)) {
          if (SampleUncovered)
            *SampleUncovered = UncoveredReg;
          return true;
        }
      }
      return false;
    };

    // For each live range, check if ALL operands of its register group are
    // covered
    SmallVector<RegLiveRange, 16> NewLiveRanges;
    for (const RegLiveRange &LR : LiveRanges) {
      MCRegister SampleUncovered = MCRegister::NoRegister;
      if (!HasUncoveredAlias(getOperandUnits(LR), &SampleUncovered)) {
        NewLiveRanges.push_back(LR);
      } else {
        LLVM_DEBUG({
          dbgs() << "Reject: pruned by full coverage";
          if (SampleUncovered != MCRegister::NoRegister)
            dbgs() << " (uncovered alias " << TRI->getName(SampleUncovered)
                   << ")";
          dbgs() << ": ";
          LR.dumpBrief(TRI);
        });
        Changed = true;
      }
    }

    LiveRanges = std::move(NewLiveRanges);
  }

  LLVM_DEBUG(dbgs() << "After pruning: " << LiveRanges.size() << " ranges\n");

  assert(isFullyCovered() &&
         "Uncovered operand overlaps with kept live range!");
  assert(isBaseCoveringOperands() &&
         "Operands of a live range exceed its base register!");
}

PartSet RegLiveRangeTracker::getOperandUnits(const RegLiveRange &LR) const {
  PartSet Units;
  for (const RegOperandInfo &Op : LR.operands())
    Units |= getRegUnits(Op.getOperand()->getReg().asMCReg());
  return Units;
}

bool RegLiveRangeTracker::isFullyCovered() const {
  PartSet KeptUnits;
  DenseSet<const MachineOperand *> CoveredOps;
  for (const RegLiveRange &LR : LiveRanges) {
    KeptUnits |= getOperandUnits(LR);
    for (const RegOperandInfo &Op : LR.operands())
      CoveredOps.insert(Op.getOperand());
  }
  return llvm::all_of(AllPhysRegOperands, [&](const MachineOperand *MO) {
    return CoveredOps.contains(MO) ||
           !getRegUnits(MO->getReg().asMCReg()).overlap(KeptUnits);
  });
}

bool RegLiveRangeTracker::isBaseCoveringOperands() const {
  return llvm::all_of(LiveRanges, [&](const RegLiveRange &LR) {
    return getRegUnits(LR.getBaseReg()).covers(getOperandUnits(LR));
  });
}

std::optional<unsigned>
RegLiveRangeTracker::findLiveRangeOfOperand(const MachineOperand *MO) const {
  for (const auto &Entry : llvm::enumerate(LiveRanges)) {
    const bool HoldsOperand =
        llvm::any_of(Entry.value().operands(), [MO](const RegOperandInfo &Op) {
          return Op.getOperand() == MO;
        });
    if (HoldsOperand)
      return Entry.index();
  }
  return std::nullopt;
}

bool RegLiveRangeTracker::absorbLiveRange(unsigned TargetIdx, unsigned SrcIdx,
                                          LivenessScanState &State) {
  assert(TargetIdx != SrcIdx && "Cannot absorb a range into itself");
  if (!LiveRanges[TargetIdx].mergeFrom(LiveRanges[SrcIdx], TRI))
    return false;
  LiveRanges[SrcIdx].clear();
  State.LiveUnits[TargetIdx] |= State.LiveUnits[SrcIdx];
  State.LiveUnits[SrcIdx].clear();
  return true;
}

bool RegLiveRangeTracker::absorbLiveRanges(unsigned TargetIdx,
                                           ArrayRef<unsigned> SrcIdxs,
                                           LivenessScanState &State) {
  for (const unsigned SrcIdx : SrcIdxs) {
    LLVM_DEBUG(dbgs() << "LR#" << LiveRanges[TargetIdx].getID() << " + LR#"
                      << LiveRanges[SrcIdx].getID() << " -> LR#"
                      << LiveRanges[TargetIdx].getID() << "\n");
    if (!absorbLiveRange(TargetIdx, SrcIdx, State)) {
      discardLiveRange(TargetIdx, State);
      discardLiveRange(SrcIdx, State);
      return false;
    }
  }
  return true;
}

void RegLiveRangeTracker::discardLiveRange(unsigned LRIdx,
                                           LivenessScanState &State) {
  LLVM_DEBUG(dbgs() << "Discard LR#" << LiveRanges[LRIdx].getID()
                    << " (no common base register)\n");

  State.OutLive |= State.LiveUnits[LRIdx];
  State.LiveUnits[LRIdx].clear();
  LiveRanges[LRIdx].clear();
}

bool RegLiveRangeTracker::areLiveUnitsDisjoint(
    const LivenessScanState &State) const {
  if (State.LiveUnits.size() != LiveRanges.size())
    return false;
  for (const auto &Entry : llvm::enumerate(State.LiveUnits)) {
    const PartSet &Units = Entry.value();
    if (!LiveRanges[Entry.index()].isValid() && Units.any())
      return false;
    const ArrayRef<PartSet> Earlier =
        ArrayRef(State.LiveUnits).take_front(Entry.index());
    for (const PartSet &Other : Earlier)
      if (Units.overlap(Other))
        return false;
  }
  return true;
}

void RegLiveRangeTracker::mergeAliasingLiveRanges(unsigned LRIdx,
                                                  const MachineOperand &MO,
                                                  LivenessScanState &State) {
  const PartSet Units = getRegUnits(MO.getReg().asMCReg());
  RegLiveRange &TargetLR = LiveRanges[LRIdx];

  // Live-out units and units of discarded ranges that a def overlaps reserve
  // the range and widen its base register over the units that stay live. A
  // read does not interact with the out-live units: without a covering def its
  // units are live-in, and the def clashes with the out-live units.
  if (MO.isDef() && State.OutLive.overlap(Units)) {
    TargetLR.setIsReserved(true);
    expandBaseWithin(TargetLR, State.OutLive);
  }

  // Overlapping ranges that observe the same value as the access are merged,
  // the others only lose the units of the access.
  SmallVector<unsigned, 4> ToMerge;
  for (const unsigned OtherIdx :
       findOverlappingLiveRanges(LRIdx, Units, State)) {
    if (isJoinableAccess(OtherIdx, Units, MO, State))
      ToMerge.push_back(OtherIdx);
    else
      State.LiveUnits[OtherIdx] -= Units;
  }
  if (!absorbLiveRanges(LRIdx, ToMerge, State))
    return;

  // A read makes all units of its register live.
  if (!MO.isDef()) {
    State.LiveUnits[LRIdx] |= Units;
    return;
  }

  // The def kills its units. This is critical for separating live ranges:
  // after x10 is defined, any y5 (containing x10) should only have x11's
  // units live, and a subsequent x10 def should NOT merge into that y5 range.
  State.LiveUnits[LRIdx] -= Units;
  State.OutLive -= Units;
}

DenseSet<MCRegister> RegLiveRangeTracker::collectReservedBaseRegs() const {
  DenseSet<MCRegister> ReservedRegs;
  for (const RegLiveRange &LR : LiveRanges) {
    if (LR.isReserved()) {
      ReservedRegs.insert(LR.getBaseReg());
    }
  }
  return ReservedRegs;
}

void RegLiveRangeTracker::computeAvailableUnits(
    const DenseSet<MCRegister> &ReservedRegs) {

  const PartSet ReservedUnits = getRegUnits(ReservedRegs);

  // Build AvailableUnits from non-reserved ranges, excluding any range whose
  // base register overlaps with a reserved register.
  AvailableUnits.clear();
  for (const RegLiveRange &LR : LiveRanges) {
    // Virtualizable ranges must have an RC; non-virtualizable ranges
    // intentionally bypass RC computation and have null RC.
    assert((!LR.isVirtualizable() || LR.getRegisterClass()) &&
           "Virtualizable live range must have a valid register class");
    assert(LR.getBaseReg() != MCRegister::NoRegister &&
           "Live range must have a base register");
    assert(LR.getBaseReg().isPhysical() &&
           "BaseReg must be a physical register");

    // Skip if this range is reserved.
    if (LR.isReserved()) {
      continue;
    }

    // Skip if base register overlaps with any reserved register.
    // Sub-registers are contained within the base, so if the base doesn't
    // overlap with reserved, neither will any sub-register.
    if (getRegUnits(LR.getBaseReg()).overlap(ReservedUnits)) {
      continue;
    }

    // The units of the base register cover those of all its sub-registers.
    AvailableUnits |= getRegUnits(LR.getBaseReg());
  }
}

bool RegLiveRangeTracker::isAvailable(MCRegister Reg) const {
  const PartSet Units = getRegUnits(Reg);
  return Units.any() && AvailableUnits.covers(Units);
}

void RegLiveRangeTracker::addUnusedCallerSavedRegs(
    MachineBasicBlock &MBB, const PartSet &ImplicitUnits,
    const DenseSet<MCRegister> &ReservedRegs) {

  // This feature is controlled by a command-line option because it changes
  // the available register pool, which can affect register allocation results.
  if (!AddUnusedCallerSavedRegs)
    return;

  // Augment AvailableUnits with caller-saved registers that are completely
  // unused in this block. Since pipelining excludes loops with calls, these
  // registers are safe to use as additional allocation candidates.
  //
  // A caller-saved register is safe to add if:
  // 1. It is allocatable (not reserved by the target)
  // 2. It belongs to a register class used by at least one live range
  // 3. It does not overlap with any register used in the block (explicit ops)
  // 4. It does not overlap with any register used implicitly
  // 5. It does not overlap with any live-in register
  // 6. It does not overlap with any live-out register
  // 7. It does not overlap with any reserved live range

  // Collect the set of register classes used by live ranges.
  SmallPtrSet<const TargetRegisterClass *, 8> UsedRegClasses;
  for (const RegLiveRange &LR : LiveRanges) {
    if (LR.getRegisterClass()) {
      UsedRegClasses.insert(LR.getRegisterClass());
    }
  }

  // If no live ranges have register classes, nothing to add.
  if (UsedRegClasses.empty())
    return;

  const auto *AIERII = static_cast<const AIEBaseRegisterInfo *>(TRI);

  // Get the call-preserved mask. clobbersPhysReg returns true for caller-saved
  // registers (those NOT preserved across calls).
  const uint32_t *PreservedMask =
      AIERII->getCallPreservedMask(*MF, MF->getFunction().getCallingConv());
  const BitVector AllocatableRegs = TRI->getAllocatableSet(*MF);

  // Helper to check if Reg is caller-saved (clobbered by calls).
  auto IsCallerSaved = [PreservedMask](MCRegister Reg) {
    return MachineOperand::clobbersPhysReg(PreservedMask, Reg);
  };

  // The units that make a register unusable: those of the explicit operands,
  // the implicit registers, the live-ins, the live-outs and the reserved base
  // registers.
  PartSet UsedUnits = ImplicitUnits | getLiveInUnits(MBB) |
                      getLiveOutUnits(MBB) | getRegUnits(ReservedRegs);
  for (const MachineOperand *MO : AllPhysRegOperands)
    UsedUnits |= getRegUnits(MO->getReg().asMCReg());

  // Iterate over allocatable registers and add unused caller-saved ones.
  unsigned NumUnusedCallerSavedAdded = 0;
  for (unsigned RegIdx = 0, E = TRI->getNumRegs(); RegIdx < E; ++RegIdx) {
    const MCRegister Reg = MCRegister::from(RegIdx);

    // Skip if already available.
    if (isAvailable(Reg))
      continue;

    // Must be allocatable.
    if (!AllocatableRegs.test(RegIdx))
      continue;

    // Must be caller-saved (clobbered by calls).
    if (!IsCallerSaved(Reg))
      continue;

    // Must belong to at least one register class used by live ranges.
    bool BelongsToUsedClass = llvm::any_of(
        UsedRegClasses, [Reg](auto *RC) { return RC->contains(Reg); });
    if (!BelongsToUsedClass)
      continue;

    // Must not overlap with any explicit, implicit, live-in, live-out or
    // reserved register.
    if (getRegUnits(Reg).overlap(UsedUnits))
      continue;

    // This register is safe to use as an additional allocation candidate.
    AvailableUnits |= getRegUnits(Reg);
    ++NumUnusedCallerSavedAdded;

    LLVM_DEBUG(dbgs() << "Added unused caller-saved register: "
                      << TRI->getName(Reg) << "\n");
  }

  LLVM_DEBUG(dbgs() << "Added " << NumUnusedCallerSavedAdded
                    << " unused caller-saved registers to available set\n");
}

void RegLiveRangeTracker::markScarceRanges() {
  // Mark live ranges as scarce if they have exactly 1 available register.
  for (RegLiveRange &LR : LiveRanges) {
    const TargetRegisterClass *RC = LR.getRegisterClass();
    if (!RC) {
      continue;
    }

    // Scarce ranges will get special attention in the virtual mode of the
    // postpipeliner, in order to be able to reorder these live ranges.
    unsigned AvailableCount = 0;
    for (MCPhysReg PhysReg : *RC) {
      if (isAvailable(PhysReg)) {
        ++AvailableCount;
        if (AvailableCount > 1) {
          break;
        }
      }
    }

    LR.setIsScarce(AvailableCount == 1);
  }
}

//===----------------------------------------------------------------------===//
// Analyze helper methods (decomposition of analyze())
//===----------------------------------------------------------------------===//

void RegLiveRangeTracker::buildInstructionOrderAndCollectOperands(
    ArrayRef<MachineInstr *> SemanticOrder, LivenessScanState &State) {
  State.FirstDefOrder.assign(TotalNumRegUnits, LivenessScanState::NotDefined);
  unsigned InstrIdx = 0;
  for (MachineInstr *MI : SemanticOrder) {
    const unsigned Order = InstrIdx++;
    InstrOrder[MI] = Order;

    for (MachineOperand &MO : MI->operands()) {
      if (!MO.isReg() || !MO.getReg().isPhysical()) {
        continue;
      }
      if (MO.isDef()) {
        for (const RegUnit Unit : getRegUnits(MO.getReg().asMCReg())) {
          unsigned &FirstDef = State.FirstDefOrder[Unit];
          FirstDef = std::min(FirstDef, Order);
        }
      }
      if (MO.isImplicit()) {
        // Track the units of implicit registers so that
        // addUnusedCallerSavedRegs can exclude them from the unused
        // caller-saved register pool. Implicit operands are not added to
        // AllPhysRegOperands because full-coverage pruning applies only to
        // explicit operands.
        State.ImplicitUnits |= getRegUnits(MO.getReg().asMCReg());
      } else {
        AllPhysRegOperands.push_back(&MO);
      }
    }
  }
}

void RegLiveRangeTracker::initOutLive(const MachineBasicBlock &MBB,
                                      LivenessScanState &State) {
  State.OutLive |= getLiveOutUnits(MBB);
}

bool RegLiveRangeTracker::isJoinableAccess(
    unsigned LRIdx, const PartSet &RegUnits, const MachineOperand &MO,
    const LivenessScanState &State) const {
  if (MO.isDef() || LiveRanges[LRIdx].getBaseReg() != MO.getReg().asMCReg())
    return true;

  // A read consumes every unit of its register. Units already redefined below
  // carry the value of that later def, not the one this read observes, so the
  // read belongs to an earlier range. Joining here would glue two distinct
  // values into one range and orphan the earlier producer. Splitting is only
  // safe when the still-live units have no producer above: otherwise both
  // reads share that producer.
  const PartSet &LiveUnits = State.LiveUnits[LRIdx];
  return LiveUnits.covers(RegUnits) ||
         areUnitsDefinedBefore(LiveUnits, *MO.getParent(), State);
}

std::optional<unsigned> RegLiveRangeTracker::selectLiveRangeToJoin(
    const PartSet &RegUnits, const MachineOperand &MO,
    const LivenessScanState &State) const {
  for (const auto &Entry : llvm::enumerate(State.LiveUnits))
    if (RegUnits.overlap(Entry.value()) &&
        isJoinableAccess(Entry.index(), RegUnits, MO, State))
      return static_cast<unsigned>(Entry.index());
  return std::nullopt;
}

SmallVector<unsigned, 4> RegLiveRangeTracker::findOverlappingLiveRanges(
    unsigned ExceptIdx, const PartSet &Units,
    const LivenessScanState &State) const {
  SmallVector<unsigned, 4> Overlapping;
  for (const auto &Entry : llvm::enumerate(State.LiveUnits))
    if (Entry.index() != ExceptIdx && Units.overlap(Entry.value()))
      Overlapping.push_back(static_cast<unsigned>(Entry.index()));
  return Overlapping;
}

void RegLiveRangeTracker::expandBaseForAccess(unsigned LRIdx, MCRegister Reg) {
  const MCRegister CurrentBase = LiveRanges[LRIdx].getBaseReg();
  assert(CurrentBase.isPhysical() && "CurrentBase must be physical");
  assert(Reg.isPhysical() && "Reg must be physical");
  if (getSubRegIndex(Reg, CurrentBase) == 0 &&
      getSubRegIndex(CurrentBase, Reg) != 0)
    LiveRanges[LRIdx].expandBaseToInclude(Reg, TRI);
}

void RegLiveRangeTracker::expandBaseWithin(RegLiveRange &LR,
                                           const PartSet &Live) const {
  const PartSet Allowed = Live | getRegUnits(LR.getBaseReg());
  MCRegister Widest = MCRegister::NoRegister;
  for (MCSuperRegIterator SuperIt(LR.getBaseReg(), TRI); SuperIt.isValid();
       ++SuperIt)
    if (Allowed.covers(getRegUnits(*SuperIt)))
      Widest = *SuperIt;
  if (Widest != MCRegister::NoRegister)
    LR.expandBaseToInclude(Widest, TRI);
}

unsigned RegLiveRangeTracker::createLiveRange(MCRegister Reg,
                                              const MachineOperand &MO,
                                              LivenessScanState &State) {
  const unsigned NewLRIdx = LiveRanges.size();
  LiveRanges.emplace_back(NextLiveRangeID++, Reg, /*IsReserved=*/false);
  State.LiveUnits.emplace_back();
  LLVM_DEBUG(dbgs() << "LR#" << LiveRanges[NewLRIdx].getID()
                    << (MO.isDef() ? " Def " : " Use ") << TRI->getName(Reg)
                    << "\n");
  return NewLRIdx;
}

unsigned RegLiveRangeTracker::getOrCreateLiveRangeForOperand(
    MCRegister Reg, MachineOperand *MO, LivenessScanState &State) {
  // Check if this register or an aliasing register is already live. We need a
  // range whose live units actually overlap, not just the registers. This is
  // critical for separating live ranges: after x10 is defined, any y5
  // (containing x10) should only have x11's units live, and a subsequent x10
  // access should NOT merge into that y5 range.
  const PartSet RegUnits = getRegUnits(Reg);

  // A def that overlaps the out-live units starts a fresh range. The merge of
  // the aliasing ranges then widens it before absorbing the overlapping ranges.
  std::optional<unsigned> Join;
  if (!MO->isDef() || !State.OutLive.overlap(RegUnits))
    Join = selectLiveRangeToJoin(RegUnits, *MO, State);

  if (!Join)
    return createLiveRange(Reg, *MO, State);
  expandBaseForAccess(*Join, Reg);
  return *Join;
}

bool RegLiveRangeTracker::areUnitsDefinedBefore(
    const PartSet &Units, const MachineInstr &MI,
    const LivenessScanState &State) const {
  const unsigned Order = InstrOrder.lookup(&MI);
  for (const RegUnit Unit : Units)
    if (State.FirstDefOrder[Unit] < Order)
      return true;
  return false;
}

void RegLiveRangeTracker::processOperand(MachineOperand &MO,
                                         LivenessScanState &State) {
  const MCRegister Reg = MO.getReg().asMCReg();
  const unsigned LRIdx = getOrCreateLiveRangeForOperand(Reg, &MO, State);

  // Add the operand to the live range with SubRegIdx relative to base.
  const MCRegister CurrentBase = LiveRanges[LRIdx].getBaseReg();
  const unsigned SubRegIdx = getSubRegIndex(Reg, CurrentBase);
  if (MO.isDef())
    LiveRanges[LRIdx].addDef(&MO, SubRegIdx);
  else
    LiveRanges[LRIdx].addUse(&MO, SubRegIdx);

  // Merge with any aliasing live ranges and update the live units.
  mergeAliasingLiveRanges(LRIdx, MO, State);
  assert(areLiveUnitsDisjoint(State) &&
         "Live units of live ranges must be disjoint after an access");
}

void RegLiveRangeTracker::processDefsInInstruction(MachineInstr &MI,
                                                   LivenessScanState &State) {
  // Process all def operands — both explicit and implicit. Implicit defs
  // produce non-virtualizable live ranges used for scheduling relaxation.
  for (MachineOperand &MO : MI.operands())
    if (MO.isReg() && MO.getReg().isPhysical() && MO.isDef())
      processOperand(MO, State);
}

void RegLiveRangeTracker::processUsesInInstruction(MachineInstr &MI,
                                                   LivenessScanState &State) {
  // Process all use operands — both explicit and implicit. Implicit uses
  // produce non-virtualizable live ranges used for scheduling relaxation.
  for (MachineOperand &MO : MI.operands())
    if (MO.isReg() && MO.getReg().isPhysical() && !MO.isDef())
      processOperand(MO, State);
  // Merge composite sub-register LRs: use-use tied groups encode sub-registers
  // of the same composite physical register that must be allocated together.
  mergeCompositeSubregLRs(MI, State);
  assert(areLiveUnitsDisjoint(State) &&
         "Live units of live ranges must be disjoint after composite merges");
}

void RegLiveRangeTracker::mergeCompositeSubregLRs(MachineInstr &MI,
                                                  LivenessScanState &State) {
  for (const TiedRegOperands &TiedSet : TII->getTiedRegInfo(MI)) {
    // Use-use tied groups (DstOps empty) represent sub-registers of the same
    // composite physical register appearing as separate explicit use operands.
    // Merge their per-sub-register LRs into one so that PostRegAlloc assigns
    // both halves from the same composite physical register.
    if (!TiedSet.DstOps.empty())
      continue;
    if (TiedSet.SrcOps.size() < 2)
      continue;

    // Collect (LRIndex, PhysReg) for each operand in the tied group.
    SmallVector<std::pair<unsigned, MCRegister>, 4> SubRegLRs;
    for (const OperandSubRegMapping &SrcOp : TiedSet.SrcOps) {
      if (SrcOp.OpIdx >= MI.getNumOperands())
        continue;
      const MachineOperand &SubMO = MI.getOperand(SrcOp.OpIdx);
      if (!SubMO.isReg() || !SubMO.getReg().isPhysical())
        continue;
      const std::optional<unsigned> LRIdx = findLiveRangeOfOperand(&SubMO);
      if (!LRIdx)
        continue;
      SubRegLRs.push_back({*LRIdx, SubMO.getReg().asMCReg()});
    }

    if (SubRegLRs.size() < 2)
      continue;

    const unsigned TargetIdx = SubRegLRs.front().first;
    if (llvm::all_of(SubRegLRs, [TargetIdx](const auto &P) {
          return P.first == TargetIdx;
        }))
      continue;

    bool MergeFailed = false;
    for (size_t I = 1; I < SubRegLRs.size(); ++I) {
      const unsigned SrcIdx = SubRegLRs[I].first;
      if (SrcIdx == TargetIdx)
        continue;
      if (!absorbLiveRange(TargetIdx, SrcIdx, State)) {
        MergeFailed = true;
        break;
      }
    }

    if (MergeFailed) {
      discardLiveRange(TargetIdx, State);
      continue;
    }

    LLVM_DEBUG(dbgs() << "Merged composite sub-reg LRs into LR#"
                      << LiveRanges[TargetIdx].getID() << " base="
                      << TRI->getName(LiveRanges[TargetIdx].getBaseReg())
                      << "\n");
  }
}

void RegLiveRangeTracker::performLivenessScan(
    ArrayRef<MachineInstr *> SemanticOrder, LivenessScanState &State) {
  // Process instructions in reverse semantic order (backward pass).
  for (MachineInstr *MI : llvm::reverse(SemanticOrder)) {
    // In backward pass: process defs first (they kill liveness), then uses
    // (they start liveness). This order is critical for read-modify-write
    // instructions where the same register is both read and written.
    // The def terminates the current live range, and the use starts a new one.
    processDefsInInstruction(*MI, State);
    processUsesInInstruction(*MI, State);
  }
}

void RegLiveRangeTracker::applySafetyFiltering(const MachineBasicBlock &MBB,
                                               const LivenessScanState &State) {
  LLVM_DEBUG({ dump("CANDIDATE LIVE RANGES\n", /*ShowAvailableRegs=*/false); });
  const PartSet LiveInUnits = getLiveInUnits(MBB);
  const PartSet TopLiveUnits = getTopLiveUnits(State);
  LLVM_DEBUG(dbgs() << "\nFirst-stage filtering: " << LiveRanges.size()
                    << " candidate ranges\n");

  SmallVector<RegLiveRange, 16> SafeRanges;
  for (const RegLiveRange &LR : LiveRanges) {
    // Skip invalid/cleared ranges from merging.
    if (!LR.isValid())
      continue;

    // Filter out live ranges whose base register is not fully defined.
    // This checks that the range doesn't read from live-in values, which
    // would make it unsafe to virtualize (we'd be changing loop-carried
    // values). This also implicitly handles use-before-def cases.
    const PartSet BaseUnits = getRegUnits(LR.getBaseReg());
    if (!isFullyDefined(TopLiveUnits & BaseUnits, LiveInUnits)) {
      LLVM_DEBUG({
        dbgs() << "Reject: base register not fully defined in block: ";
        LR.dumpBrief(TRI);
      });
      continue;
    }

    // Reject tied operands.
    if (hasTiedOperands(LR)) {
      LLVM_DEBUG({
        dbgs() << "Reject: has tied operands: ";
        LR.dumpBrief(TRI);
      });
      continue;
    }

    LLVM_DEBUG({
      dbgs() << "Keep: ";
      LR.dumpBrief(TRI);
    });
    SafeRanges.push_back(LR);
  }

  LLVM_DEBUG(dbgs() << "After first-stage: " << SafeRanges.size()
                    << " safe ranges\n");

  LiveRanges = std::move(SafeRanges);
}

void RegLiveRangeTracker::computeRegisterClassesAndFilter() {
  LLVM_DEBUG(dbgs() << "\nRegister class computation and filtering\n");

  SmallVector<RegLiveRange, 16> ValidRanges;
  for (RegLiveRange &LR : LiveRanges) {
    // Non-virtualizable ranges (containing implicit operands) bypass RC
    // computation: they may reference registers outside any allocatable class.
    if (!LR.isVirtualizable()) {
      ValidRanges.push_back(std::move(LR));
      continue;
    }

    computeRegisterClass(LR);

    // Filter out ranges with no valid register class.
    if (!LR.getRegisterClass()) {
      LLVM_DEBUG({
        dbgs() << "Reject: no valid register class: ";
        LR.dumpBrief(TRI);
      });
      continue;
    }

    ValidRanges.push_back(std::move(LR));
  }
  LiveRanges = std::move(ValidRanges);

  LLVM_DEBUG(dbgs() << "After register class filtering: " << LiveRanges.size()
                    << " ranges\n");

  // Refine register classes to preserve schedule class determinism.
  refineRegisterClassesForSchedClass();

  // Apply register class name exclusion after refinement, so it operates
  // on the final register classes.
  if (!ExcludeLiveRangesByRegClass.empty()) {
    SmallVector<RegLiveRange, 16> FilteredRanges;
    for (RegLiveRange &LR : LiveRanges) {
      auto *RC = LR.getRegisterClass();
      if (RC && TRI->getRegClassName(RC) == ExcludeLiveRangesByRegClass) {
        LLVM_DEBUG({
          dbgs() << "Reject: excluded register class "
                 << TRI->getRegClassName(LR.getRegisterClass()) << ": ";
          LR.dumpBrief(TRI);
        });
        continue;
      }
      FilteredRanges.push_back(std::move(LR));
    }
    LiveRanges = std::move(FilteredRanges);
  }
}

//===----------------------------------------------------------------------===//
// Schedule class-preserving register class refinement
//===----------------------------------------------------------------------===//

void RegLiveRangeTracker::collectVariableItinInstructions(
    VarItinInstrMap &VarItinInstrs) const {

  // Collect all instructions connected to live ranges that have variable
  // itineraries (more than one schedule class variant).
  for (unsigned LRIdx = 0; LRIdx < LiveRanges.size(); ++LRIdx) {
    const RegLiveRange &LR = LiveRanges[LRIdx];

    for (const auto &OpInfo : LR.operands()) {
      MachineInstr *MI = OpInfo.getOperand()->getParent();
      if (!MI)
        continue;

      // Skip if already processed.
      if (VarItinInstrs.count(MI))
        continue;

      // Check if this instruction has variable itineraries.
      const unsigned NumVariants = TII->getNumSchedClassVariants(MI->getDesc());
      if (NumVariants <= 1)
        continue;

      // This instruction has variable itineraries — record its operand RC
      // requirements from the original physreg match.
      VarItinInstrInfo &Info = VarItinInstrs[MI];
      Info.OrigOperandRCs = TII->getMatchingOperandRCs(
          MI->getDesc(), MI->operands(), MF->getRegInfo());
    }
  }

  LLVM_DEBUG(dbgs() << "Collected " << VarItinInstrs.size()
                    << " instructions with variable itineraries\n");
}

bool RegLiveRangeTracker::checkAndRefineSchedClasses(
    const VarItinInstrMap &VarItinInstrs) {
  bool Changed = false;

  // Iterate over live ranges as the primary loop. All information needed to
  // refine a live range's RC is reachable from the LR itself: its operands
  // carry parent-instruction links and the SubRegIdx relative to the base
  // register. VarItinInstrs is used as an auxiliary lookup.
  for (RegLiveRange &LR : LiveRanges) {
    // Skip already-rejected live ranges.
    if (!LR.getRegisterClass())
      continue;

    for (const auto &OpInfo : LR.operands()) {
      MachineInstr *const MI = OpInfo.getOperand()->getParent();
      if (!MI)
        continue;

      // Skip instructions without variable itineraries.
      const auto MIIt = VarItinInstrs.find(MI);
      if (MIIt == VarItinInstrs.end())
        continue;

      const VarItinInstrInfo &Info = MIIt->second;
      const unsigned OpIdx = OpInfo.getOperand()->getOperandNo();

      // Find the required register class for this operand from the original
      // physreg match.
      const TargetRegisterClass *RequiredRC = nullptr;
      for (const OperandRCRequirement &Req : Info.OrigOperandRCs) {
        if (Req.OpIdx == OpIdx) {
          RequiredRC = Req.RC;
          break;
        }
      }
      if (!RequiredRC)
        continue;

      const TargetRegisterClass *CurrentRC = LR.getRegisterClass();
      const unsigned SubRegIdx = OpInfo.getSubRegIdx();

      // Compute the narrowed RC. For base-register operands (SubRegIdx == 0),
      // directly intersect CurrentRC with RequiredRC. For sub-register operands
      // (SubRegIdx != 0), the itinerary discriminator is the sub-register's
      // bypass class (e.g. eBMLH for bmlh0), while the LR's RC is for the
      // super-register (e.g. ACC2048 for dm0). These belong to disjoint class
      // hierarchies, so getCommonSubClass would always return null. Use
      // getMatchingSuperRegClass instead: it finds the subclass of the
      // super-register RC whose sub-register at SubRegIdx satisfies RequiredRC.
      const TargetRegisterClass *NarrowedRC =
          (SubRegIdx == 0)
              ? TRI->getCommonSubClass(CurrentRC, RequiredRC)
              : TRI->getMatchingSuperRegClass(CurrentRC, RequiredRC, SubRegIdx);

      if (NarrowedRC && NarrowedRC != CurrentRC) {
        LLVM_DEBUG(dbgs() << "  Narrowing LR#" << LR.getID() << " from "
                          << TRI->getRegClassName(CurrentRC) << " to "
                          << TRI->getRegClassName(NarrowedRC) << "\n");
        LR.setRegisterClass(NarrowedRC);
        Changed = true;
      } else if (!NarrowedRC) {
        // Narrowing is impossible: the RC constraint from the original physreg
        // match is incompatible with the LR's current class even when the
        // sub-register hierarchy is accounted for. Reject this LR from
        // virtualization so it stays physical and gives deterministic schedule
        // class behavior.
        LLVM_DEBUG(dbgs() << "  Rejecting LR#" << LR.getID()
                          << " from virtualization: no common subclass between "
                          << TRI->getRegClassName(CurrentRC) << " and "
                          << TRI->getRegClassName(RequiredRC)
                          << " (SubRegIdx=" << SubRegIdx << ")\n");
        LR.setRegisterClass(nullptr);
        Changed = true;
        break;
      }
    }
  }

  return Changed;
}

void RegLiveRangeTracker::refineRegisterClassesForSchedClass() {
  LLVM_DEBUG(dbgs() << "\nRefining register classes for schedule class "
                       "determinism\n");

  // Stage 1: Current register classes are already computed (optimistic).
  // computeRegisterClass() was called in the loop above, giving us the
  // broadest RCs that satisfy operand constraints.

  // Stage 2: Collect variable-itinerary instructions and record original
  // schedule classes.
  VarItinInstrMap VarItinInstrs;
  collectVariableItinInstructions(VarItinInstrs);

  if (VarItinInstrs.empty()) {
    LLVM_DEBUG(dbgs() << "No variable-itinerary instructions - skipping "
                         "refinement\n");
    return;
  }

  // Stage 3: Fixed-point loop to narrow RCs when schedule classes change.
  // When narrowing is impossible, the LR's RC is set to null (rejected).
  // After the loop, rejected LRs are removed from LiveRanges so they stay
  // physical and give deterministic schedule class behavior.
  unsigned Iteration = 0;
  const unsigned MaxIterations = 10;
  while (Iteration < MaxIterations) {
    LLVM_DEBUG(dbgs() << "Refinement iteration " << Iteration << "\n");

    const bool Changed = checkAndRefineSchedClasses(VarItinInstrs);
    if (!Changed) {
      LLVM_DEBUG(dbgs() << "Fixed point reached after " << Iteration + 1
                        << " iterations\n");
      break;
    }

    ++Iteration;
  }

  if (Iteration == MaxIterations) {
    LLVM_DEBUG(dbgs() << "Warning: max iterations reached in register class "
                         "refinement\n");
  }

  // Remove live ranges rejected during the fixpoint loop (RC set to null),
  // but always keep non-virtualizable ranges regardless of RC.
  SmallVector<RegLiveRange, 16> KeptRanges;
  for (RegLiveRange &LR : LiveRanges) {
    if (!LR.isVirtualizable()) {
      KeptRanges.push_back(std::move(LR));
      continue;
    }
    if (!LR.getRegisterClass()) {
      LLVM_DEBUG({
        dbgs() << "Reject: schedule class mismatch unresolvable by RC"
                  " narrowing: ";
        LR.dumpBrief(TRI);
      });
      continue;
    }
    KeptRanges.push_back(std::move(LR));
  }
  LiveRanges = std::move(KeptRanges);
}

void RegLiveRangeTracker::finalizeAvailabilityAndScarcity(
    MachineBasicBlock &MBB, const LivenessScanState &State) {
  // Second-stage full coverage pruning.
  // This happens AFTER register class filtering.
  pruneByFullCoverage();

  // Assign compact indices now that all filtering is finalized, and
  // precompute the admissible RegUnit PartSet for each live range.
  PhysRegUnitSets.clear();
  for (unsigned I = 0; I < LiveRanges.size(); ++I) {
    RegLiveRange &LR = LiveRanges[I];
    LR.setIndex(I);
    LR.fillAdmissibleRegUnits(TRI, PhysRegUnitSets);
  }

  // Compute the register units available for reallocation.
  const DenseSet<MCRegister> ReservedRegs = collectReservedBaseRegs();
  computeAvailableUnits(ReservedRegs);

  addUnusedCallerSavedRegs(MBB, State.ImplicitUnits, ReservedRegs);
  markScarceRanges();

  // Compute and cache the most promising scarce range set.
  MostPromisingScarceRanges = findMostPromisingScarceRanges();
}

void RegLiveRangeTracker::analyze(MachineBasicBlock &MBB,
                                  ArrayRef<MachineInstr *> SemanticOrder) {
  assert(!AnalysisDone &&
         "analyze() may only be called once per tracker instance.");
  AnalysisDone = true;
  assert(!SemanticOrder.empty() && "SemanticOrder must be provided - MBB order "
                                   "is unreliable after scheduling");

  // Initialize state for liveness scan.
  LivenessScanState State;
  State.SemanticOrder = SemanticOrder;

  // Build instruction order map and collect operands.
  buildInstructionOrderAndCollectOperands(SemanticOrder, State);

  // Initialize the out-live units from live-outs.
  initOutLive(MBB, State);

  // Perform the liveness scan to build live ranges.
  performLivenessScan(SemanticOrder, State);

  // Apply first-stage safety filtering.
  applySafetyFiltering(MBB, State);

  // Classify ranges with any implicit operand as non-virtualizable early,
  // before RC computation.  Implicit operands encode the physical register in
  // the instruction opcode; the register cannot be substituted.  RC
  // computation is skipped for non-virtualizable ranges in the next step.
  // AdmissibleRegs is set to {BaseReg} so the allocator's uniform candidate
  // computation (getCandidatePhysRegs) works without special-casing.
  for (RegLiveRange &LR : LiveRanges)
    if (llvm::any_of(LR.operands(), [](const RegOperandInfo &OpInfo) {
          return OpInfo.getOperand()->isImplicit();
        })) {
      LR.setIsVirtualizable(false);
      LR.setAdmissibleRegsToBaseReg();
    }

  // Compute register classes and apply filtering.
  // Non-virtualizable ranges bypass RC computation and are kept as-is.
  computeRegisterClassesAndFilter();

  // Discard non-virtualizable ranges unless their base register has the
  // configured property. Ranges without the property are dropped so they
  // neither appear in the live range list nor inflate AvailableUnits.
  {
    SmallVector<RegLiveRange, 16> Filtered;
    for (RegLiveRange &LR : LiveRanges) {
      if (LR.isVirtualizable() ||
          (ImplicitRangesProperty &&
           TRI->hasPhysRegProperty(LR.getBaseReg(), *ImplicitRangesProperty)))
        Filtered.push_back(std::move(LR));
    }
    LiveRanges = std::move(Filtered);
  }

  // Finalize availability and scarcity.  Must run after any filtering so that
  // discarded ranges do not inflate AvailableUnits.
  finalizeAvailabilityAndScarcity(MBB, State);
}

void RegLiveRange::fillAdmissibleRegUnits(
    const TargetRegisterInfo *TRI,
    DenseMap<MCRegister, PartSet> &PhysRegUnits) {
  AdmissibleRegUnits.clear();
  for (MCRegister Reg : AdmissibleRegs) {
    PartSet &RegUnits = PhysRegUnits[Reg];
    if (RegUnits.empty())
      for (MCRegUnitIterator UI(Reg, TRI); UI.isValid(); ++UI)
        RegUnits.set(RegUnit(*UI));

    // Admissible registers must not share register units. Lane-derived units
    // of a live range are only equivalent to the per-register lane-derived
    // units under this condition.
    assert(!AdmissibleRegUnits.overlap(RegUnits) &&
           "Admissible registers of a live range must not overlap");
    AdmissibleRegUnits |= RegUnits;
  }
}

void RegLiveRange::setRegisterClass(const TargetRegisterClass *RC) {
  RegisterClass = RC;

  // Populate AdmissibleRegs from RegisterClass.
  // This is initially equivalent to the RC membership, but can be further
  // constrained later by per-LR requirements (e.g., bypass constraints).
  AdmissibleRegs.clear();
  if (RC) {
    for (MCPhysReg Reg : *RC) {
      AdmissibleRegs.insert(Reg);
    }
  }
}

void RegLiveRangeTracker::computeRegisterClass(RegLiveRange &LR) const {
  if (LR.getBaseReg() == MCRegister::NoRegister)
    return;

  LLVM_DEBUG(dbgs() << "  computeRegisterClass for LR#" << LR.getID()
                    << " (base=" << TRI->getName(LR.getBaseReg()) << "):\n");

  // createVirtualRegister requires an allocatable class. Operand constraints
  // such as AIE2 mMvBMXDst are intentionally non-allocatable unions used only
  // for encoding. Project them to the largest allocatable subclass that still
  // contains the physical base before intersecting, so CommonRC is always
  // allocatable (or null) by construction.
  auto getAllocatableContainingBase =
      [&](const TargetRegisterClass *RC) -> const TargetRegisterClass * {
    const TargetRegisterClass *Best = nullptr;
    for (BitMaskClassIterator It(RC->getSubClassMask(), *TRI); It.isValid();
         ++It) {
      const TargetRegisterClass *SubRC = TRI->getRegClass(It.getID());
      if (!SubRC->isAllocatable() || !SubRC->contains(LR.getBaseReg()))
        continue;
      if (!Best || SubRC->getNumRegs() > Best->getNumRegs())
        Best = SubRC;
    }
    return Best;
  };

  // Start with nullptr, representing the universe of all register classes.
  // Intersection with nullptr is identity: intersect(nullptr, X) = X
  const TargetRegisterClass *CommonRC = nullptr;

  // Process all operands (defs and uses) to compute register class constraints
  for (const auto &OpInfo : LR.operands()) {
    MachineInstr *MI = OpInfo.getOperand()->getParent();
    const unsigned OpIdx = OpInfo.getOperand()->getOperandNo();

    // Get the register class constraint for this operand
    const TargetRegisterClass *OpRC =
        MI->getRegClassConstraint(OpIdx, TII, TRI);

    LLVM_DEBUG({
      dbgs() << "    Op#" << OpIdx << " "
             << (OpInfo.getOperand()->isDef() ? "def" : "use") << " in "
             << AIE::NoDebug(*MI);
      dbgs() << "      getRegClassConstraint -> "
             << (OpRC ? TRI->getRegClassName(OpRC) : "(null)");
      if (OpInfo.getSubRegIdx() != 0)
        dbgs() << " (subRegIdx=" << OpInfo.getSubRegIdx() << ")";
      dbgs() << "\n";
    });

    if (OpRC) {
      // Account for subregister access: the operand constrains a sub-register
      // of the LR's base register. Lift this constraint to the base register
      // level using getMatchingSuperRegClass, which finds the subclass of the
      // current base RC where the sub-register at SubRegIdx satisfies OpRC.
      // Note: getSubClassWithSubReg(OpRC, SubRegIdx) is the wrong direction —
      // it asks whether OpRC itself has a sub-register at SubRegIdx, not
      // whether the base RC does.
      //
      // If getMatchingSuperRegClass returns null, no defined subclass of the
      // base RC can satisfy the sub-register constraint. This means the
      // constraint cannot be expressed through any valid register class, so
      // the live range cannot be safely virtualized.
      if (OpInfo.getSubRegIdx() != 0) {
        const TargetRegisterClass *OrigOpRC = OpRC;
        const TargetRegisterClass *BaseRC =
            CommonRC ? CommonRC : TRI->getMinimalPhysRegClass(LR.getBaseReg());
        OpRC =
            TRI->getMatchingSuperRegClass(BaseRC, OpRC, OpInfo.getSubRegIdx());
        LLVM_DEBUG(dbgs() << "      getMatchingSuperRegClass("
                          << TRI->getRegClassName(BaseRC) << ", "
                          << TRI->getRegClassName(OrigOpRC) << ", "
                          << OpInfo.getSubRegIdx() << ") -> "
                          << (OpRC ? TRI->getRegClassName(OpRC) : "(null)")
                          << "\n");
        if (!OpRC) {
          // Constraint is inexpressible at the base register class level.
          LR.setRegisterClass(nullptr);
          return;
        }
      }

      if (OpRC && !OpRC->isAllocatable()) {
        const TargetRegisterClass *OrigOpRC = OpRC;
        OpRC = getAllocatableContainingBase(OpRC);
        LLVM_DEBUG(dbgs() << "      Narrow non-allocatable "
                          << TRI->getRegClassName(OrigOpRC) << " -> "
                          << (OpRC ? TRI->getRegClassName(OpRC) : "(null)")
                          << "\n");
        if (!OpRC) {
          LR.setRegisterClass(nullptr);
          return;
        }
      }

      // Intersect: nullptr is identity, otherwise find common subclass
      if (!CommonRC) {
        CommonRC = OpRC;
        LLVM_DEBUG(dbgs() << "      Initial RC = "
                          << TRI->getRegClassName(CommonRC) << "\n");
      } else {
        const TargetRegisterClass *PrevRC = CommonRC;
        CommonRC = TRI->getCommonSubClass(CommonRC, OpRC);
        LLVM_DEBUG({
          dbgs() << "      Intersect(" << TRI->getRegClassName(PrevRC) << ", "
                 << TRI->getRegClassName(OpRC) << ") -> "
                 << (CommonRC ? TRI->getRegClassName(CommonRC) : "(null)")
                 << "\n";
        });
        if (!CommonRC) {
          // No common class possible - this live range is illegal.
          LR.setRegisterClass(nullptr);
          return;
        }
      }
    }
  }

  // If no operand constraints were found, fall back to minimal class.
  if (!CommonRC) {
    CommonRC = TRI->getMinimalPhysRegClass(LR.getBaseReg());
    assert(CommonRC && "Physical register must have a register class");
    LLVM_DEBUG(dbgs() << "    No operand constraints, fallback MinRC = "
                      << TRI->getRegClassName(CommonRC) << "\n");
  }

  LLVM_DEBUG(dbgs() << "    After operand constraints: "
                    << TRI->getRegClassName(CommonRC) << "\n");

  // Canonicalize to the lowest-ID equivalent class.
  // Multiple register classes can have the same set of registers (e.g., eDM
  // and ACC2048 both contain {dm0..dm4}). We pick the one with the lowest
  // TableGen ID for determinism: regardless of which equivalent class we
  // start from, we always converge to the same canonical representative.
  for (const TargetRegisterClass *RC : TRI->regclasses()) {
    if (RC->getNumRegs() != CommonRC->getNumRegs())
      continue;
    if (RC->getID() >= CommonRC->getID())
      continue;
    // Check if RC is a subclass of CommonRC with the same registers.
    const TargetRegisterClass *Sub = TRI->getCommonSubClass(CommonRC, RC);
    if (Sub == RC) {
      LLVM_DEBUG(dbgs() << "    Canonicalize: "
                        << TRI->getRegClassName(CommonRC) << " -> "
                        << TRI->getRegClassName(RC) << " (same "
                        << RC->getNumRegs() << " regs, lower ID)\n");
      CommonRC = RC;
    }
  }

  LLVM_DEBUG(dbgs() << "    Final RC = " << TRI->getRegClassName(CommonRC)
                    << "\n");

  LR.setRegisterClass(CommonRC);
}

void RegLiveRangeTracker::virtualizeFilteredPhysRegs(OverlapPolicy Policy) {
  assert(!RegistersVirtualized && "Registers are already virtualized");

  MachineRegisterInfo &MRI = MF->getRegInfo();

  // Clear the NoVRegs property.
  MF->getProperties().reset(MachineFunctionProperties::Property::NoVRegs);

  // Build the set of units of the RESERVED base registers.
  const PartSet ReservedUnits = getRegUnits(collectReservedBaseRegs());

  // Create and rewrite virtual registers. Live ranges are created in reverse,
  // so we run this loop in reverse order to make the dumps more intuitive.
  for (RegLiveRange &LR : reverse(LiveRanges)) {
    // Non-virtualizable ranges keep their physical operands unchanged.
    if (!LR.isVirtualizable())
      continue;

    // The analysis should have filtered out any live ranges without a valid
    // register class.
    assert(LR.getRegisterClass() &&
           "Live range must have a valid register class");

    // The analysis should have assigned a base register to every live range.
    assert(LR.getBaseReg() != MCRegister::NoRegister &&
           "Live range must have a base register");

    // Never virtualize RESERVED ranges themselves.
    if (LR.isReserved()) {
      continue;
    }

    // Apply the overlap policy.
    if (Policy == OverlapPolicy::DisallowOverlapWithReservedBase) {
      // Skip virtualization if this LR's base register overlaps any RESERVED
      // base.
      if (getRegUnits(LR.getBaseReg()).overlap(ReservedUnits))
        continue;
    }
    // If Policy == AllowOverlapWithReservedBase, we proceed to virtualize.

    // Create a virtual register for this live range.
    const Register VReg = MRI.createVirtualRegister(LR.getRegisterClass());

    // Store the VReg in the LiveRange for later mapping.
    LR.setVReg(VReg);

    // Replace all operands in this live range with the virtual register.
    const auto RewriteOperand = [VReg](const RegOperandInfo &Info) {
      MachineOperand *MO = Info.getOperand();
      MO->setReg(VReg);
      MO->setSubReg(Info.getSubRegIdx());
    };

    // Rewrite all operands.
    for (const auto &OpInfo : LR.operands()) {
      RewriteOperand(OpInfo);
    }
  }

  // Mark as virtualized even if no live ranges were virtualized.
  RegistersVirtualized = true;
}

void RegLiveRangeTracker::rewriteToPhysRegs(
    const DenseMap<Register, MCRegister> &VRegToPhysMap) {
  assert(RegistersVirtualized && "Registers are not virtualized");

  MachineRegisterInfo &MRI = MF->getRegInfo();

  for (const RegLiveRange &LR : LiveRanges) {
    const Register VReg = LR.getVReg();

    // Skip live ranges that were not virtualized (partial virtualization).
    if (!VReg.isValid()) {
      continue;
    }

    // Look up the physical register for this virtual register.
    auto It = VRegToPhysMap.find(VReg);
    assert(It != VRegToPhysMap.end() &&
           "VReg must have a mapping in VRegToPhysMap");

    const MCRegister PhysReg = It->second;

    // Rewrite all operands in this live range to the physical register.
    for (const auto &OpInfo : LR.operands()) {
      MachineOperand *MO = OpInfo.getOperand();
      if (MO->getReg() == VReg) {
        // Compute the actual physical register considering subregs.
        Register FinalReg = PhysReg;
        if (OpInfo.getSubRegIdx() != 0) {
          FinalReg = TRI->getSubReg(PhysReg, OpInfo.getSubRegIdx());
          assert(FinalReg && "Invalid subregister index for physical register");
        }
        MO->setReg(FinalReg);
        MO->setSubReg(0);
      }
    }
  }

  // Clear virtual registers from MRI and restore NoVRegs property.
  MRI.clearVirtRegs();
  MF->getProperties().set(MachineFunctionProperties::Property::NoVRegs);

  // Mark as no longer virtualized.
  RegistersVirtualized = false;

  LLVM_DEBUG(dbgs() << "Rewritten virtual registers to physical registers\n");
}

void RegLiveRangeTracker::restoreOriginalPhysRegs() {
  // Build the mapping from VRegs to their original PhysRegs
  DenseMap<Register, MCRegister> VRegToPhysMap;
  for (const RegLiveRange &LR : LiveRanges) {
    if (LR.getVReg().isValid()) {
      VRegToPhysMap[LR.getVReg()] = LR.getBaseReg();
    }
  }

  // Use the general rewrite method
  rewriteToPhysRegs(VRegToPhysMap);
  LLVM_DEBUG(dbgs() << "Restored original physical registers\n");
}

bool RegLiveRangeTracker::areRegistersVirtualized() const {
  return RegistersVirtualized;
}

std::optional<unsigned>
RegLiveRangeTracker::getIndexForOperand(const MachineOperand *MO) const {
  for (const RegLiveRange &LR : LiveRanges) {
    for (const auto &OpInfo : LR.operands()) {
      if (OpInfo.getOperand() == MO)
        return LR.getIndex();
    }
  }
  return std::nullopt;
}

void RegLiveRangeTracker::dump(const char *Header,
                               bool ShowAvailableRegs) const {
  if (Header) {
    dbgs() << Header;
  }
  dbgs() << "================================\n";
  dbgs() << "Total live ranges: " << LiveRanges.size() << "\n\n";

  // Create a sorted index array to ensure deterministic output
  SmallVector<size_t, 16> SortedIndices;
  for (size_t LRIdx = 0; LRIdx < LiveRanges.size(); ++LRIdx) {
    SortedIndices.push_back(LRIdx);
  }

  // Sort by base register ID first, then by first def instruction pointer
  // This ensures a stable, deterministic order
  llvm::sort(SortedIndices, [this](size_t A, size_t B) {
    const RegLiveRange &LRA = LiveRanges[A];
    const RegLiveRange &LRB = LiveRanges[B];

    // First sort by base register ID
    if (LRA.getBaseReg() != LRB.getBaseReg()) {
      return LRA.getBaseReg() < LRB.getBaseReg();
    }

    // Then by first def instruction address (if any)
    if (!LRA.defs().empty() && !LRB.defs().empty()) {
      const MachineInstr *MIA = LRA.defs().begin()->getOperand()->getParent();
      const MachineInstr *MIB = LRB.defs().begin()->getOperand()->getParent();
      if (MIA != MIB) {
        // Use instruction order if available
        auto ItA = InstrOrder.find(MIA);
        auto ItB = InstrOrder.find(MIB);
        if (ItA != InstrOrder.end() && ItB != InstrOrder.end()) {
          return ItA->second < ItB->second;
        }
      }
    }

    // Finally by original index for stability
    return A < B;
  });

  for (size_t SortedIdx = 0; SortedIdx < SortedIndices.size(); ++SortedIdx) {
    const size_t LRIdx = SortedIndices[SortedIdx];
    const RegLiveRange &LR = LiveRanges[LRIdx];

    // Skip invalid/cleared ranges.
    if (!LR.isValid())
      continue;

    // Use the stored base register
    const MCRegister BaseReg = LR.getBaseReg();
    StringRef PrimaryReg = "unknown";
    if (BaseReg != MCRegister::NoRegister) {
      PrimaryReg = TRI->getName(BaseReg);
    }

    dbgs() << "Live Range #" << LR.getID() << " for " << PrimaryReg;
    if (LR.isReserved()) {
      dbgs() << " [RESERVED]";
    }
    dbgs() << ":\n";

    dbgs() << "  Definitions (" << LR.getNumDefs() << "):\n";
    size_t DefIdx = 0;
    for (const RegOperandInfo &DefInfo : LR.defs()) {
      dbgs() << "    [" << DefIdx++ << "] ";
      Register Reg = DefInfo.getOperand()->getReg();
      if (Reg.isPhysical()) {
        dbgs() << "Register: " << TRI->getName(Reg);
      } else {
        dbgs() << "Register: %vreg" << Reg.virtRegIndex();
      }
      if (DefInfo.getSubRegIdx() != 0) {
        dbgs() << " (SubRegIdx: " << DefInfo.getSubRegIdx() << ")";
      }
      dbgs() << " ";
      if (MachineInstr *DefInstr = DefInfo.getOperand()->getParent()) {
        dbgs() << AIE::NoDebug(*DefInstr) << "\n";
      } else {
        dbgs() << "<orphaned operand>\n";
      }
    }

    dbgs() << "  Uses (" << LR.getNumUses() << "):\n";
    size_t UseIdx = 0;
    for (const RegOperandInfo &UseInfo : LR.uses()) {
      dbgs() << "    [" << UseIdx++ << "] ";
      Register Reg = UseInfo.getOperand()->getReg();
      if (Reg.isPhysical()) {
        dbgs() << "Register: " << TRI->getName(Reg);
      } else {
        dbgs() << "Register: %vreg" << Reg.virtRegIndex();
      }
      if (UseInfo.getSubRegIdx() != 0) {
        dbgs() << " (SubRegIdx: " << UseInfo.getSubRegIdx() << ")";
      }
      dbgs() << " ";
      if (MachineInstr *UseInstr = UseInfo.getOperand()->getParent()) {
        dbgs() << AIE::NoDebug(*UseInstr) << "\n";
      } else {
        dbgs() << "<orphaned operand>\n";
      }
    }
    dbgs() << "\n";
  }

  if (ShowAvailableRegs) {
    dbgs() << "Available Physical Registers for Reallocation:\n";
    dbgs() << "==============================================\n";
    unsigned NumAvailable = 0;
    for (unsigned RegIdx = 1, E = TRI->getNumRegs(); RegIdx < E; ++RegIdx) {
      const MCRegister Reg = MCRegister::from(RegIdx);
      if (!isAvailable(Reg))
        continue;
      ++NumAvailable;
      dbgs() << "  " << TRI->getName(Reg) << "\n";
    }
    dbgs() << "Total: " << NumAvailable << " registers\n\n";
  }

  // Emit end marker if header was provided
  if (Header) {
    dbgs() << "=== END " << Header;
  }
}

std::vector<const RegLiveRange *>
RegLiveRangeTracker::findMostPromisingScarceRanges() const {

  const unsigned MaxCandidates = MaxScarceRangeCandidates;

  // Group live ranges by base register (not register class).
  DenseMap<MCRegister, std::vector<const RegLiveRange *>> RangesByBaseReg;

  for (const auto &LR : LiveRanges) {
    // Only consider ranges that are marked as scarce.
    if (!LR.isScarce()) {
      continue;
    }

    const MCRegister BaseReg = LR.getBaseReg();
    assert(BaseReg != MCRegister::NoRegister &&
           "LiveRange must have a BaseReg after analysis");

    RangesByBaseReg[BaseReg].push_back(&LR);
  }

  // Helper to check if a set of ranges has overlapping instructions.
  auto HasOverlap = [](const std::vector<const RegLiveRange *> &Ranges) {
    DenseSet<const MachineInstr *> SeenInstrs;
    for (const RegLiveRange *LR : Ranges) {
      for (const auto &Info : LR->operands()) {
        if (!SeenInstrs.insert(Info.getOperand()->getParent()).second) {
          return true;
        }
      }
    }
    return false;
  };

  // Select the best complete candidate set (size <= MaxCandidates).
  //
  // Sets larger than MaxCandidates ("partial" sets) are rejected entirely:
  // the scarce range scheduler uses exhaustive enumeration, which is
  // factorial in the number of ranges, and crude truncation of a large set
  // yields little benefit over no scarce scheduling at all. Partial sets
  // will be revisited once a pruning mechanism is available.
  //
  // Among complete sets, larger is better (more competing ranges means more
  // optimization opportunity). Need at least 2 ranges for actual competition.
  const std::vector<const RegLiveRange *> *BestSet = nullptr;
  size_t BestSize = 0;

  for (const auto &Entry : RangesByBaseReg) {
    const auto &Ranges = Entry.second;

    // Need at least 2 ranges for actual competition.
    if (Ranges.size() <= 1)
      continue;

    // A set where no range has a use offers no reordering benefit.
    if (llvm::all_of(Ranges, [](const RegLiveRange *LR) {
          return LR->getNumUses() == 0;
        }))
      continue;

    // Skip sets that exceed the candidate limit: exhaustive enumeration of
    // their orderings would be prohibitively expensive.
    if (Ranges.size() > MaxCandidates) {
      LLVM_DEBUG(dbgs() << "Skipping scarce range set for "
                        << TRI->getName(Ranges.front()->getBaseReg()) << " ("
                        << Ranges.size() << " ranges, limit " << MaxCandidates
                        << ")\n");
      continue;
    }

    // No overlapping instructions allowed.
    if (HasOverlap(Ranges))
      continue;

    // Prefer larger complete sets.
    if (Ranges.size() > BestSize) {
      BestSet = &Ranges;
      BestSize = Ranges.size();
    }
  }

  if (!BestSet)
    return {};

  return std::vector<const RegLiveRange *>(BestSet->begin(), BestSet->end());
}
