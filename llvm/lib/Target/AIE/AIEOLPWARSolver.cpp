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

DenseMap<MCRegister, MachineOperand *>
OLPWARSolver::findLastDefsInClones(const BlockState &BS) const {
  DenseMap<MCRegister, MachineOperand *> LastDefs;

  // Collect all defs from TopInsert clones.
  for (MachineInstr *Clone : BS.TopInsert.Clones) {
    for (MachineOperand &MO : Clone->all_defs()) {
      if (!MO.isReg() || !MO.getReg().isPhysical())
        continue;
      LastDefs[MO.getReg().asMCReg()] = &MO;
    }
  }

  // Validate each def: it must come from a single defining clone,
  // and no other clone may read the register. The only allowed reader
  // of the register is the defining instruction itself (e.g. for tied
  // operands or implicit sub-register modeling).
  auto IsInvalidDef = [&](MCRegister DefReg,
                          const MachineInstr *DefMI) -> bool {
    for (MachineInstr *Clone : BS.TopInsert.Clones) {
      if (Clone == DefMI)
        continue;
      // Another clone defines an overlapping register — multiple writers.
      for (const MachineOperand &MO : Clone->all_defs()) {
        if (MO.isReg() && MO.getReg().isPhysical() &&
            TRI.regsOverlap(MO.getReg(), DefReg))
          return true;
      }
      // Another clone reads an overlapping register — other reader.
      for (const MachineOperand &MO : Clone->all_uses()) {
        if (MO.isReg() && MO.getReg().isPhysical() &&
            TRI.regsOverlap(MO.getReg(), DefReg))
          return true;
      }
    }
    return false;
  };

  SmallVector<MCRegister, 4> ToRemove;
  for (auto &[DefReg, DefOp] : LastDefs) {
    if (IsInvalidDef(DefReg, DefOp->getParent())) {
      LLVM_DEBUG(dbgs() << "  Skip " << printReg(DefReg, &TRI)
                        << ": multiple writers or other reader in clones\n");
      ToRemove.push_back(DefReg);
    }
  }
  for (MCRegister Reg : ToRemove)
    LastDefs.erase(Reg);

  return LastDefs;
}

SmallVector<MachineOperand *, 4>
OLPWARSolver::collectFreeStoreUses(MCRegister DefReg,
                                   ArrayRef<MachineInstr *> FreeInstrs) const {
  SmallVector<MachineOperand *, 4> FreeUses;
  SmallDenseSet<MCRegUnit, 8> DefinedUnits;
  bool SeenRedef = false;

  for (MachineInstr *MI : FreeInstrs) {
    bool ReadsReg = false;
    for (const MachineOperand &MO : MI->all_uses())
      if (MO.isReg() && MO.getReg().isPhysical() &&
          TRI.regsOverlap(MO.getReg(), DefReg)) {
        ReadsReg = true;
        break;
      }

    bool WritesReg = false;
    for (const MachineOperand &MO : MI->all_defs())
      if (MO.isReg() && MO.getReg().isPhysical() &&
          TRI.regsOverlap(MO.getReg(), DefReg)) {
        WritesReg = true;
        break;
      }

    if (!ReadsReg && !WritesReg)
      continue;

    if (!SeenRedef) {
      // Phase 1: expecting stores that read DefReg.
      if (WritesReg) {
        // Transition to Phase 2: this is the first redefinition.
        // It must be a load instruction; any other writer type
        // (e.g. vmac, vaddmac) is not safe to rename around.
        if (!MI->mayLoad())
          return {};
        for (const MachineOperand &MO : MI->all_defs())
          if (MO.isReg() && MO.getReg().isPhysical())
            for (MCRegUnit Unit : TRI.regunits(MO.getReg().asMCReg()))
              DefinedUnits.insert(Unit);
        SeenRedef = true;
        continue;
      }
      // A reader in Phase 1 must be a store instruction.
      if (!MI->mayStore())
        return {};
      for (MachineOperand &MO : MI->all_uses())
        if (MO.isReg() && MO.getReg().isPhysical() &&
            TRI.regsOverlap(MO.getReg(), DefReg))
          FreeUses.push_back(&MO);
    } else {
      // Phase 2: expecting loads that redefine DefReg.
      // A reader after the first redefinition is invalid — all stores
      // must come before all loads. The redefining load itself may
      // read the register (tied input / sub-register modeling), so
      // only reject non-redefining readers.
      if (ReadsReg && !WritesReg)
        return {};
      if (WritesReg) {
        if (!MI->mayLoad())
          return {};
        for (const MachineOperand &MO : MI->all_defs())
          if (MO.isReg() && MO.getReg().isPhysical())
            for (MCRegUnit Unit : TRI.regunits(MO.getReg().asMCReg()))
              DefinedUnits.insert(Unit);
      }
    }
  }

  // Verify that the loads completely redefine DefReg by checking
  // that every register unit is covered.
  if (FreeUses.empty() || DefinedUnits.empty())
    return {};

  for (MCRegUnit Unit : TRI.regunits(DefReg))
    if (!DefinedUnits.count(Unit))
      return {};

  return FreeUses;
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

  // Find defs in TopInsert clones that are valid renaming candidates:
  // each register must have a single defining clone and no other clone
  // may read it (except the defining instruction itself).
  DenseMap<MCRegister, MachineOperand *> LastDefByReg =
      findLastDefsInClones(BS);
  if (LastDefByReg.empty())
    return Candidates;

  // Walk free instructions top-down in program order to discover
  // candidates deterministically. The iteration order matters because
  // earlier candidates claim registers that become unavailable for
  // later ones.
  DenseSet<MCRegister> ProcessedRegs;

  for (MachineInstr *StoreMI : FreeInstrs) {
    for (MachineOperand &StoreUse : StoreMI->all_uses()) {
      if (!StoreUse.isReg() || !StoreUse.getReg().isPhysical())
        continue;
      const MCRegister UseReg = StoreUse.getReg().asMCReg();

      // Look up in the clone-derived map: exact match first, then
      // super-registers.
      MCRegister DefReg;
      MachineOperand *LastDef = nullptr;
      if (auto It = LastDefByReg.find(UseReg); It != LastDefByReg.end()) {
        DefReg = UseReg;
        LastDef = It->second;
      } else {
        for (MCRegister Super : TRI.superregs(UseReg)) {
          if (auto It = LastDefByReg.find(Super); It != LastDefByReg.end()) {
            DefReg = Super;
            LastDef = It->second;
            break;
          }
        }
      }
      if (!LastDef || !ProcessedRegs.insert(DefReg).second)
        continue;

      // Collect free store uses that consume the TopInsert value.
      // This validates store-then-load ordering and that the loads
      // completely redefine the register.
      auto FreeUses = collectFreeStoreUses(DefReg, FreeInstrs);
      if (FreeUses.empty())
        continue;

      const TargetRegisterClass *RC = getRegClassForDef(LastDef, DefReg);
      if (!RC)
        continue;

      Candidates.emplace_back(DefReg, RC, LastDef, std::move(FreeUses));
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

  // Never rename implicit operands — their register is fixed by the
  // instruction encoding and cannot be changed.
  if (C.LastDefInFixed->isImplicit())
    return false;

  return true;
}

DenseSet<MCRegister> OLPWARSolver::computeAvailableRegs(
    const BlockState &BS, const TargetRegisterClass *RC,
    const AIELoopUtils::OuterLoopStructure &OLP) const {
  const MachineFunction &MF = *BS.TheBlock->getParent();
  const MachineRegisterInfo &MRI = MF.getRegInfo();

  // Build a set of callee-saved registers for quick lookup.
  DenseSet<MCRegister> CalleeSaved;
  if (const MCPhysReg *CSRegs = TRI.getCalleeSavedRegs(&MF))
    for (unsigned I = 0; CSRegs[I]; ++I)
      CalleeSaved.insert(CSRegs[I]);

  // Start with allocatable, non-reserved, non-callee-saved registers
  // in the class. Using a callee-saved register would require the
  // function to save and restore it, which is not worth the rename.
  DenseSet<MCRegister> Available;
  for (MCRegister Reg : *RC)
    if (!MRI.isReserved(Reg) && MRI.isAllocatable(Reg) &&
        !CalleeSaved.count(Reg))
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
                      << " (" << C.FreeUses.size() << " free uses)\n");

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
