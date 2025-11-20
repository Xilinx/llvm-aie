//===- AIEInlineSpiller.cpp - Custom AIE Inline Spiller -------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIEInlineSpiller class, which provides AIE-specific
// spilling strategies by wrapping the standard InlineSpiller.
//
//===----------------------------------------------------------------------===//

#include "AIEInlineSpiller.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/LiveInterval.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/LiveRangeEdit.h"
#include "llvm/CodeGen/LiveStacks.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/Spiller.h"
#include "llvm/CodeGen/StackMaps.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/CodeGen/VirtRegMap.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "regalloc"

STATISTIC(NumSpilledRanges, "Number of spilled live ranges");
STATISTIC(NumSnippets, "Number of spilled snippets");
STATISTIC(NumSpills, "Number of spills inserted");
STATISTIC(NumSpillsRemoved, "Number of spills removed");
STATISTIC(NumReloads, "Number of reloads inserted");
STATISTIC(NumReloadsRemoved, "Number of reloads removed");
STATISTIC(NumFolded, "Number of folded stack accesses");
STATISTIC(NumFoldedLoads, "Number of folded loads");
STATISTIC(NumRemats, "Number of rematerialized defs for spilling");
STATISTIC(NumSubRegSpills, "Number of spills for subreg");
STATISTIC(NumSubRegReloads, "Number of reloads for subreg");

AIEInlineSpiller::AIEInlineSpiller(const Spiller::RequiredAnalyses &Analyses,
                                   MachineFunction &MF, VirtRegMap &VRM,
                                   VirtRegAuxInfo &VRAI)
    : MF(MF), LIS(Analyses.LIS), LSS(Analyses.LSS), VRM(VRM),
      MRI(MF.getRegInfo()), TII(*MF.getSubtarget().getInstrInfo()),
      TRI(*MF.getSubtarget().getRegisterInfo()), Edit(nullptr),
      StackInt(nullptr), StackSlot(0), Original(0), VRAI(VRAI) {
  // AIE-specific initialization if needed
}

ArrayRef<Register> AIEInlineSpiller::getSpilledRegs() { return RegsToSpill; }

ArrayRef<Register> AIEInlineSpiller::getReplacedRegs() { return RegsReplaced; }

void AIEInlineSpiller::postOptimization() { // todo: hoist all spills
}

/// see InlineSpiller::isSibling for more details.
bool AIEInlineSpiller::isSibling(Register Reg) {
  return Reg.isVirtual() && VRM.getOriginal(Reg) == Original;
}

bool AIEInlineSpiller::isRegToSpill(Register Reg) {
  return is_contained(RegsToSpill, Reg);
}

// When spilling a virtual register, we also spill any snippets it is connected
// to. The snippets are small live ranges that only have a single real use,
// leftovers from live range splitting. Spilling them enables memory operand
// folding or tightens the live range around the single use.
//
// This minimizes register pressure and maximizes the store-to-load distance for
// spill slots which can be important in tight loops.

/// isFullCopyOf - If MI is a COPY to or from Reg, return the other register,
/// otherwise return 0.
/// see InlineSpiller::isCopyOf for more details.
static Register isCopyOf(const MachineInstr &MI, Register Reg,
                         const TargetInstrInfo &TII) {
  if (!TII.isCopyInstr(MI))
    return Register();

  const MachineOperand &DstOp = MI.getOperand(0);
  const MachineOperand &SrcOp = MI.getOperand(1);

  // TODO: Probably only worth allowing subreg copies with undef dests.
  if (DstOp.getSubReg() != SrcOp.getSubReg())
    return Register();
  if (DstOp.getReg() == Reg)
    return SrcOp.getReg();
  if (SrcOp.getReg() == Reg)
    return DstOp.getReg();
  return Register();
}

/// Check for a copy bundle as formed by SplitKit.
/// see InlineSpiller::isCopyOfBundle for more details.
static Register isCopyOfBundle(const MachineInstr &FirstMI, Register Reg,
                               const TargetInstrInfo &TII) {
  if (!FirstMI.isBundled())
    return isCopyOf(FirstMI, Reg, TII);

  assert(!FirstMI.isBundledWithPred() && FirstMI.isBundledWithSucc() &&
         "expected to see first instruction in bundle");

  Register SnipReg;
  MachineBasicBlock::const_instr_iterator I = FirstMI.getIterator();
  while (I->isBundledWithSucc()) {
    const MachineInstr &MI = *I;
    auto CopyInst = TII.isCopyInstr(MI);
    if (!CopyInst)
      return Register();

    const MachineOperand &DstOp = *CopyInst->Destination;
    const MachineOperand &SrcOp = *CopyInst->Source;
    if (DstOp.getReg() == Reg) {
      if (!SnipReg)
        SnipReg = SrcOp.getReg();
      else if (SnipReg != SrcOp.getReg())
        return Register();
    } else if (SrcOp.getReg() == Reg) {
      if (!SnipReg)
        SnipReg = DstOp.getReg();
      else if (SnipReg != DstOp.getReg())
        return Register();
    }

    ++I;
  }

  return Register();
}

/// isSnippet - Identify if a live interval is a snippet that should be spilled.
/// It is assumed that SnipLI is a virtual register with the same original as
/// Edit->getReg().
/// see InlineSpiller::isSnippet for more details.
bool AIEInlineSpiller::isSnippet(const LiveInterval &SnipLI) {
  Register Reg = Edit->getReg();

  // A snippet is a tiny live range with only a single instruction using it
  // besides copies to/from Reg or spills/fills.
  // Exception is done for statepoint instructions which will fold fills
  // into their operands.
  // We accept:
  //
  //   %snip = COPY %Reg / FILL fi#
  //   %snip = USE %snip
  //   %snip = STATEPOINT %snip in var arg area
  //   %Reg = COPY %snip / SPILL %snip, fi#
  //
  if (!LIS.intervalIsInOneMBB(SnipLI))
    return false;

  // Number of defs should not exceed 2 not accounting defs coming from
  // statepoint instructions.
  unsigned NumValNums = SnipLI.getNumValNums();
  for (auto *VNI : SnipLI.vnis()) {
    MachineInstr *MI = LIS.getInstructionFromIndex(VNI->def);
    if (MI->getOpcode() == TargetOpcode::STATEPOINT)
      --NumValNums;
  }
  if (NumValNums > 2)
    return false;

  MachineInstr *UseMI = nullptr;

  // Check that all uses satisfy our criteria.
  for (MachineRegisterInfo::reg_bundle_nodbg_iterator
           RI = MRI.reg_bundle_nodbg_begin(SnipLI.reg()),
           E = MRI.reg_bundle_nodbg_end();
       RI != E;) {
    MachineInstr &MI = *RI++;

    // Allow copies to/from Reg.
    if (isCopyOfBundle(MI, Reg, TII))
      continue;

    // Allow stack slot loads.
    int FI;
    if (SnipLI.reg() == TII.isLoadFromStackSlot(MI, FI) && FI == StackSlot)
      continue;

    // Allow stack slot stores.
    if (SnipLI.reg() == TII.isStoreToStackSlot(MI, FI) && FI == StackSlot)
      continue;

    if (StatepointOpers::isFoldableReg(&MI, SnipLI.reg()))
      continue;

    // Allow a single additional instruction.
    if (UseMI && &MI != UseMI)
      return false;
    UseMI = &MI;
  }
  return true;
}

void AIEInlineSpiller::collectRegsToSpill() {
  Register Reg = Edit->getReg();

  // Main register always spills.
  RegsToSpill.assign(1, Reg);
  SnippetCopies.clear();
  RegsReplaced.clear();

  // Snippets all have the same original, so there can't be any for an original
  // register.
  if (Original == Reg)
    return;

  for (MachineInstr &MI : llvm::make_early_inc_range(MRI.reg_bundles(Reg))) {
    Register SnipReg = isCopyOfBundle(MI, Reg, TII);
    if (!isSibling(SnipReg))
      continue;
    LiveInterval &SnipLI = LIS.getInterval(SnipReg);
    if (!isSnippet(SnipLI))
      continue;
    SnippetCopies.insert(&MI);
    if (isRegToSpill(SnipReg))
      continue;
    RegsToSpill.push_back(SnipReg);
    LLVM_DEBUG(dbgs() << "\talso spill snippet " << SnipLI << '\n');
    ++NumSnippets;
  }
}

void AIEInlineSpiller::spill(LiveRangeEdit &Edit) {
  ++NumSpilledRanges;
  this->Edit = &Edit;
  assert(!Register::isStackSlot(Edit.getReg()) &&
         "Trying to spill a stack slot.");
  // Share a stack slot among all descendants of Original.
  Original = VRM.getOriginal(Edit.getReg());
  StackSlot = VRM.getStackSlot(Original);
  StackInt = nullptr;

  collectRegsToSpill();
  // todo: reMaterializeAll();

  // Remat may handle everything.
  if (!RegsToSpill.empty())
    spillAll();

  this->Edit->calculateRegClassAndHint(MF, VRAI);
}

void AIEInlineSpiller::spillAll() {
  SpillInfo SI = collectSpillInfo();

  SI.calcStack(MRI, TRI, VRM, LSS);

  // todo: FIXME: perform optimizations

  LLVM_DEBUG(SI.dump()); // MRI will be auto-fetched from
                         // SpillLocations/ReloadLocations

  SI.insertSpills(MRI, TII, TRI, VRM, LIS);
  SI.insertReloads(MRI, TII, TRI, VRM, LIS);
  SpillInfos.push_back(SI);
  RegsToSpill.emplace_back(SI.getReg());
  LLVM_DEBUG(MF.dump());
}

SpillInfo AIEInlineSpiller::collectSpillInfo() const {
  SpillInfo SI(Original);
  for (Register Reg : RegsToSpill) {
    SI.update(Reg, MRI);
  }
  return SI;
}

void SpillInfo::calcStack(MachineRegisterInfo &MRI,
                          const TargetRegisterInfo &TRI, VirtRegMap &VRM,
                          LiveStacks &LSS) {

  for (auto DefOp : DefOps) {
    // get Original register from the defining operand
    const TargetRegisterClass *RC = MRI.getRegClass(DefOp.getReg());
    if (DefOp.getSubReg()) {
      RC = TRI.getSubClassWithSubReg(RC, DefOp.getSubReg());
    }

    StackSlots.push_back(VRM.createSpillSlot(RC));
  }
}

void SpillInfo::updateWriteDefs(
    SmallVector<std::pair<MachineInstr *, unsigned>, 8> &Ops) {
  for (const auto &Op : Ops) {
    MachineOperand &MO = Op.first->getOperand(Op.second);
    if (MO.isDef()) {
      // Use isIdenticalTo for comparison instead of is_contained, since
      // MachineOperand does not have operator== defined for direct container
      // search.
      bool Found = false;
      for (const auto &DefOp : DefOps) {
        if (DefOp.isIdenticalTo(MO)) {
          Found = true;
          break;
        }
      }
      if (!Found) {
        LLVM_DEBUG(dbgs() << "Adding write def: " << MO << '\n');
        DefOps.push_back(MO);
      } else {
        LLVM_DEBUG(dbgs() << "Write def already exists: " << MO << '\n');
      }
    }
  }
}

void SpillInfo::update(Register Reg, MachineRegisterInfo &MRI) {
  // Iterate over instructions using Reg.
  for (MachineInstr &MI : llvm::make_early_inc_range(MRI.reg_bundles(Reg))) {
    if (MI.isDebugValue()) {
      LLVM_DEBUG(dbgs() << "Skipping debug value: " << MI << '\n');
      continue;
    }

    // Analyze instruction.
    SmallVector<std::pair<MachineInstr *, unsigned>, 8> Ops;
    VirtRegInfo RI = AnalyzeVirtRegInBundle(MI, Reg, &Ops);

    if (RI.Writes) {
      // Save the defining operands of the write.
      updateWriteDefs(Ops);
      LLVM_DEBUG(dbgs() << "Adding spill location: " << MI << '\n');
      SpillLocations.push_back(&MI);
    }

    if (RI.Reads) {
      LLVM_DEBUG(dbgs() << "Adding reload location: " << MI << '\n');
      ReloadLocations.push_back(&MI);
    }
  }
}

void SpillInfo::insertSpill(MachineBasicBlock::iterator MI, bool IsKill,
                            MachineRegisterInfo &MRI,
                            const TargetInstrInfo &TII,
                            const TargetRegisterInfo &TRI, VirtRegMap &VRM,
                            LiveIntervals &LIS) {
  MachineBasicBlock &MBB = *MI->getParent();
  MachineInstrSpan MIS(MI, &MBB);

  for (unsigned I = 0; I < StackSlots.size(); I++) {
    Register OrigReg = DefOps[I].getReg();
    int StackSlot = StackSlots[I];
    const TargetRegisterClass *RC = MRI.getRegClass(OrigReg);

    // Create a new virtual register
    Register NewVReg = MRI.createVirtualRegister(RC);
    SpillVRegs.push_back(NewVReg);

    // Create COPY: NewVReg = COPY OrigReg
    BuildMI(MBB, MI, MI->getDebugLoc(), TII.get(TargetOpcode::COPY), NewVReg)
        .addReg(OrigReg, getKillRegState(IsKill));

    // Assign the new virtual register to the stack slot
    VRM.assignVirt2StackSlot(NewVReg, StackSlot);

    // Store the new virtual register to the stack slot
    TII.storeRegToStackSlot(MBB, MI, NewVReg, true, StackSlot, RC, &TRI,
                            Register());
  }
  LIS.InsertMachineInstrRangeInMaps(MIS.begin(), MI);
}

void SpillInfo::insertReload(MachineInstr *MI, MachineRegisterInfo &MRI,
                             const TargetInstrInfo &TII,
                             const TargetRegisterInfo &TRI, VirtRegMap &VRM,
                             LiveIntervals &LIS) {
  MachineBasicBlock &MBB = *MI->getParent();
  MachineInstrSpan MIS(MI, &MBB);

  // Create a new virtual register
  const TargetRegisterClass *RC = MRI.getRegClass(Reg);
  Register NewVReg = MRI.createVirtualRegister(RC);

  for (unsigned I = 0; I < StackSlots.size(); I++) {
    Register OrigReg = DefOps[I].getReg();
    unsigned SubRegIdx = DefOps[I].getSubReg();
    int StackSlot = StackSlots[I];

    const bool IsSubReg = DefOps[I].getSubReg() != 0;
    unsigned AdditionalFlag = IsSubReg && I == 0 ? getUndefRegState(true) : 0;

    // Assign the new virtual register to the stack slot
    const TargetRegisterClass *SubRC = MRI.getRegClass(OrigReg);
    Register TempReg = MRI.createVirtualRegister(SubRC);
    VRM.assignVirt2StackSlot(TempReg, StackSlot);

    TII.loadRegFromStackSlot(MBB, MI, TempReg, StackSlot, SubRC, &TRI,
                             Register());

    // Copy from the temporary to the parent register's subregister
    BuildMI(MBB, MI, MI->getDebugLoc(), TII.get(TargetOpcode::COPY))
        .addReg(NewVReg, RegState::Define | AdditionalFlag, SubRegIdx)
        .addReg(TempReg, RegState::Kill);

    if (IsSubReg)
      NumSubRegReloads++;
    else
      NumReloads++;
  }

  LIS.InsertMachineInstrRangeInMaps(MIS.begin(), MI);
}

void SpillInfo::insertSpills(MachineRegisterInfo &MRI,
                             const TargetInstrInfo &TII,
                             const TargetRegisterInfo &TRI, VirtRegMap &VRM,
                             LiveIntervals &LIS) {
  const bool IsKill = true;
  for (MachineInstr *MI : SpillLocations) {
    MachineBasicBlock::iterator SpillBefore = std::next(MI->getIterator());
    insertSpill(SpillBefore, IsKill, MRI, TII, TRI, VRM, LIS);
  }
}

void SpillInfo::insertReloads(MachineRegisterInfo &MRI,
                              const TargetInstrInfo &TII,
                              const TargetRegisterInfo &TRI, VirtRegMap &VRM,
                              LiveIntervals &LIS) {
  for (MachineInstr *MI : ReloadLocations) {
    insertReload(MI, MRI, TII, TRI, VRM, LIS);
  }
}

void SpillInfo::dump() const {
  // Lambda to get MRI from any available instruction pointer
  auto GetMRI = [this]() -> const MachineRegisterInfo * {
    const MachineInstr *MI = nullptr;
    if (!SpillLocations.empty())
      MI = SpillLocations[0];
    else if (!ReloadLocations.empty())
      MI = ReloadLocations[0];

    if (MI)
      if (const MachineFunction *MF = MI->getMF())
        return &MF->getRegInfo();
    return nullptr;
  };

  const MachineRegisterInfo *MRI = GetMRI();
  const TargetRegisterInfo *TRI = MRI ? MRI->getTargetRegisterInfo() : nullptr;

  dbgs() << "SpillInfo for register: " << printReg(Reg);
  if (MRI && Reg.isVirtual()) {
    const TargetRegisterClass *RC = MRI->getRegClass(Reg);
    dbgs() << ", RC: " << TRI->getRegClassName(RC);
  }
  dbgs() << "\n";

  dbgs() << "  DefOps (" << DefOps.size() << "):\n";
  for (const auto &MO : DefOps) {
    dbgs() << "    Reg: " << printReg(MO.getReg());
    if (MRI && MO.getReg().isVirtual()) {
      const TargetRegisterClass *RC = MRI->getRegClass(MO.getReg());
      dbgs() << ", RC: " << TRI->getRegClassName(RC);
    }
    dbgs() << ", SubReg: " << MO.getSubReg() << "\n";
  }

  dbgs() << "  StackSlots (" << StackSlots.size() << "):\n";
  for (auto StackSlot : StackSlots) {
    dbgs() << "    StackSlot: " << StackSlot << "\n";
  }

  dbgs() << "  SpillVRegs (" << SpillVRegs.size() << "):\n";
  for (auto VReg : SpillVRegs) {
    dbgs() << "    " << printReg(VReg);
    if (MRI && VReg.isVirtual()) {
      const TargetRegisterClass *RC = MRI->getRegClass(VReg);
      dbgs() << ", RC: " << TRI->getRegClassName(RC);
    }
    dbgs() << "\n";
  }

  dbgs() << "  SpillLocations (" << SpillLocations.size() << "):\n";
  for (const auto *MI : SpillLocations) {
    dbgs() << "    " << *MI;
  }

  dbgs() << "  ReloadLocations (" << ReloadLocations.size() << "):\n";
  for (const auto *MI : ReloadLocations) {
    dbgs() << "    " << *MI;
  }
}
