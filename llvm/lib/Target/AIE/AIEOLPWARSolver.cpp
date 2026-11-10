//===- AIEOLPWARSolver.cpp - WAR dependency solver for OLP --*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Implementation of WAR dependency resolution for outer-loop pipelined
// epilogue blocks. See AIEOLPWARSolver.h for the algorithm description.
//
//===----------------------------------------------------------------------===//

#include "AIEOLPWARSolver.h"
#include "AIEBaseInstrInfo.h"
#include "AIEInterBlockScheduling.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "aie-olp-war-solver"

using namespace llvm;

static cl::opt<bool> EnableOLPWARSolver(
    "aie-enable-olp-war-solver", cl::Hidden, cl::init(true),
    cl::desc("Resolve WAR register dependencies in OLP epilogue blocks "
             "by renaming accumulator registers"));

namespace llvm::AIE {

OLPWARSolver::OLPWARSolver(InterBlockScheduling &IBS,
                           const AIEBaseInstrInfo &TII,
                           const TargetRegisterInfo &TRI)
    : IBS(IBS), TII(TII), TRI(TRI) {}

/// Map an operand's register from OldSuperReg's register hierarchy to
/// NewSuperReg's hierarchy. If the operand uses a sub-register of
/// OldSuperReg, find the corresponding sub-register of NewSuperReg.
/// Returns MCRegister::NoRegister if the mapping fails.
static MCRegister mapRegister(MCRegister OpReg, MCRegister OldSuperReg,
                              MCRegister NewSuperReg,
                              const TargetRegisterInfo &TRI) {
  if (OpReg == OldSuperReg)
    return NewSuperReg;

  const unsigned SubIdx = TRI.getSubRegIndex(OldSuperReg, OpReg);
  if (SubIdx == 0)
    return MCRegister::NoRegister;

  return TRI.getSubReg(NewSuperReg, SubIdx);
}

bool OLPWARSolver::isCompletelyRedefinedLater(
    MCRegister Reg, const MachineInstr *StoreMI,
    ArrayRef<MachineInstr *> FreeInstrs) const {
  // Collect all register units that make up Reg.
  SmallDenseSet<MCRegUnit, 8> RequiredUnits;
  for (MCRegUnit Unit : TRI.regunits(Reg))
    RequiredUnits.insert(Unit);

  // Walk free instructions after StoreMI and track which units are defined.
  SmallDenseSet<MCRegUnit, 8> DefinedUnits;
  bool PastStore = false;
  for (const MachineInstr *MI : FreeInstrs) {
    if (MI == StoreMI) {
      PastStore = true;
      continue;
    }
    if (!PastStore)
      continue;

    for (const MachineOperand &MO : MI->all_defs()) {
      if (!MO.isReg() || !MO.getReg().isPhysical())
        continue;
      for (MCRegUnit Unit : TRI.regunits(MO.getReg().asMCReg()))
        DefinedUnits.insert(Unit);
    }
  }

  // Check that every unit of Reg is covered by a later definition.
  for (MCRegUnit Unit : RequiredUnits) {
    if (!DefinedUnits.count(Unit))
      return false;
  }
  return true;
}

DenseMap<MCRegister, MachineOperand *>
OLPWARSolver::buildLastDefMap(const BlockState &BS) const {
  DenseMap<MCRegister, MachineOperand *> LastDefByReg;
  for (auto &Bundle : BS.TopInsert.Bundles)
    for (MachineInstr *MI : Bundle.getInstrs())
      for (MachineOperand &MO : MI->all_defs())
        if (MO.getReg().isPhysical())
          LastDefByReg[MO.getReg().asMCReg()] = &MO;
  return LastDefByReg;
}

std::pair<MCRegister, MachineOperand *> OLPWARSolver::findTopInsertDef(
    MCRegister UseReg,
    const DenseMap<MCRegister, MachineOperand *> &LastDefByReg) const {
  // Try exact match first.
  if (auto It = LastDefByReg.find(UseReg); It != LastDefByReg.end())
    return {UseReg, It->second};

  // Walk super-registers looking for a matching def.
  for (MCRegister Super : TRI.superregs(UseReg)) {
    if (auto It = LastDefByReg.find(Super); It != LastDefByReg.end())
      return {Super, It->second};
  }

  return {MCRegister(), nullptr};
}

bool OLPWARSolver::hasPostDefUseInFixed(MCRegister DefReg,
                                        const MachineOperand *LastDef,
                                        const BlockState &BS) const {
  bool PastLastDef = false;
  for (auto &Bundle : BS.TopInsert.Bundles) {
    for (MachineInstr *MI : Bundle.getInstrs()) {
      if (MI == LastDef->getParent()) {
        PastLastDef = true;
        continue;
      }
      if (!PastLastDef)
        continue;

      for (const MachineOperand &MO : MI->all_uses()) {
        if (MO.isReg() && MO.getReg().isPhysical() &&
            TRI.regsOverlap(MO.getReg(), DefReg))
          return true;
      }
    }
  }
  return false;
}

OLPWARSolver::UsesAndLoads OLPWARSolver::collectFreeUsesAndConflictingLoads(
    MCRegister DefReg, ArrayRef<MachineInstr *> FreeInstrs) const {
  UsesAndLoads Result;
  bool SeenRedef = false;

  for (MachineInstr *MI : FreeInstrs) {
    // Check if this instruction redefines DefReg (a conflicting load).
    bool IsRedef = false;
    for (const MachineOperand &MO : MI->all_defs()) {
      if (MO.getReg().isPhysical() && TRI.regsOverlap(MO.getReg(), DefReg)) {
        Result.ConflictingLoads.push_back(MI);
        IsRedef = true;
        break;
      }
    }
    if (IsRedef)
      SeenRedef = true;

    // Only collect uses before the first redefinition — those are
    // the ones reading the TopInsert value.
    if (!SeenRedef) {
      for (MachineOperand &MO : MI->all_uses())
        if (MO.getReg().isPhysical() && TRI.regsOverlap(MO.getReg(), DefReg))
          Result.FreeUses.push_back(&MO);
    }
  }

  return Result;
}

const TargetRegisterClass *
OLPWARSolver::getRegClassForDef(const MachineOperand *Def,
                                MCRegister DefReg) const {
  const MachineInstr *DefMI = Def->getParent();
  const unsigned DefOpIdx = DefMI->getOperandNo(Def);

  const TargetRegisterClass *RC = TII.getRegClass(DefMI->getDesc(), DefOpIdx);
  if (!RC)
    RC = TRI.getMinimalPhysRegClass(DefReg);
  return RC;
}

SmallVector<OLPWARSolver::WARCandidate, 4>
OLPWARSolver::collectCandidates(BlockState &BS) const {
  SmallVector<WARCandidate, 4> Candidates;

  if (BS.TopInsert.empty())
    return Candidates;

  const auto &FreeInstrs = BS.getTop().getFreeInstructions();

  DenseMap<MCRegister, MachineOperand *> LastDefByReg = buildLastDefMap(BS);
  if (LastDefByReg.empty())
    return Candidates;

  // Walk free instructions top-down. For each store use whose value comes
  // from TopInsert, check if the register is completely redefined later.
  DenseSet<MCRegister> ProcessedRegs;

  for (MachineInstr *StoreMI : FreeInstrs) {
    for (MachineOperand &StoreUse : StoreMI->all_uses()) {
      if (!StoreUse.isReg() || !StoreUse.getReg().isPhysical())
        continue;
      const MCRegister UseReg = StoreUse.getReg().asMCReg();

      // The stored value must come from TopInsert.
      const auto [DefReg, LastDef] = findTopInsertDef(UseReg, LastDefByReg);
      if (!LastDef || !ProcessedRegs.insert(DefReg).second)
        continue;

      // The register must be completely redefined by later free
      // instructions — either the register itself directly, or all
      // its sub-registers covering every register unit.
      if (!isCompletelyRedefinedLater(DefReg, StoreMI, FreeInstrs)) {
        LLVM_DEBUG(dbgs() << "  Skip " << printReg(DefReg, &TRI)
                          << ": not completely redefined later\n");
        continue;
      }

      // Verify that no TopInsert instruction after the last def reads
      // DefReg. Renaming the last def would leave those consumers with
      // a stale value, breaking the data flow within the fixed region.
      if (hasPostDefUseInFixed(DefReg, LastDef, BS)) {
        LLVM_DEBUG(
            dbgs() << "  Skip " << printReg(DefReg, &TRI)
                   << ": TopInsert instruction after last def reads it\n");
        continue;
      }

      // Collect free uses that consume the TopInsert value and conflicting
      // loads that redefine the register.
      auto [FreeUses, ConflictingLoads] =
          collectFreeUsesAndConflictingLoads(DefReg, FreeInstrs);

      if (FreeUses.empty() || ConflictingLoads.empty())
        continue;

      const TargetRegisterClass *RC = getRegClassForDef(LastDef, DefReg);
      if (!RC)
        continue;

      Candidates.emplace_back(DefReg, RC, LastDef, std::move(FreeUses),
                              std::move(ConflictingLoads));
    }
  }

  return Candidates;
}

bool OLPWARSolver::canRename(const WARCandidate &C) const {
  assert(C.LastDefInFixed && "Candidate without a last def");
  const MachineInstr *MI = C.LastDefInFixed->getParent();
  const unsigned DefOpIdx = MI->getOperandNo(C.LastDefInFixed);

  if (MI->isRegTiedToUseOperand(DefOpIdx))
    return false;

  return true;
}

DenseSet<MCRegister> OLPWARSolver::computeAvailableRegs(
    const BlockState &BS, const TargetRegisterClass *RC,
    const AIELoopUtils::OuterLoopStructure &OLP) const {
  const MachineRegisterInfo &MRI = BS.TheBlock->getParent()->getRegInfo();

  // Start with all non-reserved registers in the class.
  DenseSet<MCRegister> Available;
  for (MCRegister Reg : *RC)
    if (!MRI.isReserved(Reg))
      Available.insert(Reg);

  // Helper to exclude a register and all its aliases.
  auto ExcludeReg = [&](MCRegister Reg) {
    Available.erase(Reg);
    for (MCRegister Super : TRI.superregs(Reg))
      Available.erase(Super);
    for (MCRegister Sub : TRI.subregs(Reg))
      Available.erase(Sub);
  };

  // Helper to exclude all registers mentioned in an instruction.
  auto ExcludeRegsInMI = [&](const MachineInstr &MI) {
    for (const MachineOperand &MO : MI.operands()) {
      if (!MO.isReg() || !MO.getReg().isPhysical())
        continue;
      ExcludeReg(MO.getReg().asMCReg());
    }
  };

  // Helper to exclude all registers in a block's free instructions.
  auto ExcludeFreeInstrs = [&](MachineBasicBlock *MBB) {
    const BlockState &BlockBS = IBS.getBlockState(MBB);
    if (!BlockBS.getRegions().empty())
      for (MachineInstr *MI : BlockBS.getTop().getFreeInstructions())
        ExcludeRegsInMI(*MI);
  };

  // Exclude live-ins of SteadyTop — these are registers flowing into
  // the outer loop from outside and must not be clobbered.
  for (const auto &LI : OLP.SteadyTop->liveins())
    ExcludeReg(LI.PhysReg);

  // Exclude registers defined or used in TopInsert (the fixed drain).
  // Picking a NewReg that is already used in TopInsert would corrupt
  // an existing drain value.
  for (auto &Bundle : BS.TopInsert.Bundles)
    for (MachineInstr *MI : Bundle.getInstrs())
      ExcludeRegsInMI(*MI);

  // Exclude registers defined or used in free instructions of all
  // OLP steady-state blocks. This covers the entire register footprint
  // of the outer loop iteration.
  ExcludeFreeInstrs(OLP.SteadyTop);
  ExcludeFreeInstrs(OLP.SteadyInner);
  ExcludeFreeInstrs(OLP.SteadyBottom);

  LLVM_DEBUG(dbgs() << "  computeAvailableRegs: " << Available.size()
                    << " registers in class " << TRI.getRegClassName(RC)
                    << " after OLP scan\n");

  return Available;
}

bool OLPWARSolver::renameCandidate(WARCandidate &C, MCRegister NewReg) const {
  const MCRegister OldReg = C.ConflictReg;
  assert(C.LastDefInFixed && "No last def to rename");

  // Phase 1: Verify all use mappings before mutating anything.
  SmallVector<MCRegister, 4> MappedRegs;
  MappedRegs.reserve(C.FreeUses.size());
  for (MachineOperand *MO : C.FreeUses) {
    const MCRegister OpReg = MO->getReg().asMCReg();
    const MCRegister MappedReg = mapRegister(OpReg, OldReg, NewReg, TRI);
    if (!MappedReg.isValid()) {
      LLVM_DEBUG(dbgs() << "  Abort rename: could not map "
                        << printReg(OpReg, &TRI) << " from "
                        << printReg(OldReg, &TRI) << " to "
                        << printReg(NewReg, &TRI) << "\n");
      return false;
    }
    MappedRegs.push_back(MappedReg);
  }

  // Phase 2: All mappings verified — commit the rename.
  LLVM_DEBUG(dbgs() << "OLPWARSolver: renaming " << printReg(OldReg, &TRI)
                    << " -> " << printReg(NewReg, &TRI) << "\n");

  LLVM_DEBUG(dbgs() << "  Fixed def: " << *C.LastDefInFixed->getParent());
  C.LastDefInFixed->setReg(NewReg);
  LLVM_DEBUG(dbgs() << "    -> " << *C.LastDefInFixed->getParent());

  for (auto [MO, MappedReg] : zip(C.FreeUses, MappedRegs)) {
    LLVM_DEBUG(dbgs() << "  Free use: " << *MO->getParent());
    MO->setReg(MappedReg);
    LLVM_DEBUG(dbgs() << "    -> " << *MO->getParent());
  }

  return true;
}

bool OLPWARSolver::tryResolve(BlockState &BS) const {
  if (!EnableOLPWARSolver)
    return false;

  // Only apply to SteadyBottom blocks in an OLP structure.
  const auto *OLP = BS.getOLPMembership();
  if (!OLP)
    return false;
  if (BS.TheBlock != OLP->SteadyBottom)
    return false;
  if (BS.TopInsert.empty())
    return false;

  LLVM_DEBUG(dbgs() << "OLPWARSolver: analyzing SteadyBottom "
                    << BS.TheBlock->getFullName() << "\n");

  SmallVector<WARCandidate, 4> Candidates = collectCandidates(BS);
  if (Candidates.empty()) {
    LLVM_DEBUG(dbgs() << "  No WAR candidates\n");
    return false;
  }

  LLVM_DEBUG(dbgs() << "  " << Candidates.size() << " WAR candidate(s)\n");

  // Track registers assigned to previous candidates so they are not
  // reused by later ones within the same tryResolve call.
  DenseSet<MCRegister> AssignedRegs;

  bool Changed = false;
  for (auto &C : Candidates) {
    LLVM_DEBUG(dbgs() << "  Candidate: " << printReg(C.ConflictReg, &TRI)
                      << " (" << C.FreeUses.size() << " free uses, "
                      << C.ConflictingLoads.size() << " conflicting loads)\n");

    if (!canRename(C)) {
      LLVM_DEBUG(dbgs() << "  Skip: last def operand is tied\n");
      continue;
    }

    DenseSet<MCRegister> Available = computeAvailableRegs(BS, C.RC, *OLP);

    // Exclude registers already assigned to earlier candidates.
    for (MCRegister Reg : AssignedRegs) {
      Available.erase(Reg);
      for (MCRegister Super : TRI.superregs(Reg))
        Available.erase(Super);
      for (MCRegister Sub : TRI.subregs(Reg))
        Available.erase(Sub);
    }

    if (Available.empty()) {
      LLVM_DEBUG(dbgs() << "  Skip: no available registers in class "
                        << TRI.getRegClassName(C.RC) << "\n");
      continue;
    }

    // Iterate the register class in its tablegen-defined allocation order
    // and pick the first available register. This guarantees deterministic
    // codegen and respects the target's preferred register order.
    MCRegister NewReg;
    for (MCRegister Reg : *C.RC) {
      if (Available.contains(Reg)) {
        NewReg = Reg;
        break;
      }
    }
    if (!NewReg.isValid())
      continue;

    LLVM_DEBUG(dbgs() << "  Available: " << Available.size()
                      << " registers, picking " << printReg(NewReg, &TRI)
                      << "\n");

    if (renameCandidate(C, NewReg)) {
      AssignedRegs.insert(NewReg);
      Changed = true;
    }
  }

  return Changed;
}

} // namespace llvm::AIE
