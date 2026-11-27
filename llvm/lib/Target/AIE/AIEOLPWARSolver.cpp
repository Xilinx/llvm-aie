//===- AIEOLPWARSolver.cpp - WAR dependency solver for OLP ------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEOLPWARSolver.h"
#include "AIEDataDependenceHelper.h"
#include "AIEInterBlockScheduling.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/CodeGen/LiveRegUnits.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "aie-olp-war-solver"

using namespace llvm;

static cl::opt<bool> EnableOLPWARSolver(
    "aie-enable-olp-war-solver", cl::Hidden, cl::init(true),
    cl::desc("Rename the last-iteration def of recurrences in OLP epilogues "
             "to break WAR dependences in the inner loop exit"));

namespace llvm::AIE {

OLPWARSolver::OLPWARSolver(InterBlockScheduling &IBS,
                           const TargetRegisterInfo &TRI)
    : IBS(IBS), TRI(TRI) {}

/// True if \p Dep is a register dependence on a register overlapping \p Reg.
static bool isRegDepOn(const SDep &Dep, MCRegister Reg,
                       const TargetRegisterInfo &TRI) {
  if (Dep.getKind() == SDep::Order)
    return false;
  const Register DepReg = Dep.getReg();
  return DepReg.isPhysical() && TRI.regsOverlap(DepReg, Reg);
}

/// Map \p OpReg, which is \p OldReg or one of its sub-registers, to the
/// matching register of \p NewReg. Returns an invalid register if there is
/// none.
static MCRegister mapRegister(MCRegister OpReg, MCRegister OldReg,
                              MCRegister NewReg,
                              const TargetRegisterInfo &TRI) {
  if (OpReg == OldReg)
    return NewReg;
  const unsigned SubIdx = TRI.getSubRegIndex(OldReg, OpReg);
  return SubIdx ? TRI.getSubReg(NewReg, SubIdx) : MCRegister();
}

bool OLPWARSolver::collectReaderUses(
    const SUnit &DefSU, const SUnit &Reader, MCRegister DefReg,
    SmallVectorImpl<MachineOperand *> &Uses) const {
  // Every unit of DefReg that the reader takes must come from DefSU, so that
  // renaming its operands cannot mix the old and the new register.
  for (const SDep &Dep : Reader.Preds)
    if (Dep.getKind() == SDep::Data && isRegDepOn(Dep, DefReg, TRI) &&
        Dep.getSUnit() != &DefSU)
      return false;

  for (MachineOperand &MO : Reader.getInstr()->all_uses()) {
    if (!MO.getReg().isPhysical() || !TRI.regsOverlap(MO.getReg(), DefReg))
      continue;
    if (MO.isImplicit() || MO.isTied())
      return false;
    if (MO.getReg() != DefReg && !TRI.isSubRegister(DefReg, MO.getReg()))
      return false;
    Uses.push_back(&MO);
  }
  return true;
}

std::optional<OLPWARSolver::Candidate>
OLPWARSolver::analyzeDef(const InterBlockEdges &DDG, const SUnit &DefSU,
                         MCRegister UseReg) const {
  MachineInstr &DefMI = *DefSU.getInstr();

  // The operand that feeds UseReg must be the only def of that register in
  // the instruction, explicit and untied.
  MachineOperand *DefMO = nullptr;
  for (MachineOperand &MO : DefMI.all_defs()) {
    if (!MO.getReg().isPhysical() || !TRI.regsOverlap(MO.getReg(), UseReg))
      continue;
    if (DefMO)
      return std::nullopt;
    DefMO = &MO;
  }
  if (!DefMO || DefMO->isImplicit() ||
      DefMI.isRegTiedToUseOperand(DefMI.getOperandNo(DefMO)))
    return std::nullopt;
  const MCRegister DefReg = DefMO->getReg().asMCReg();

  // Only the recurrence is renamed: DefMI reads all of DefReg. The epilogue
  // also holds earlier-iteration clones of DefMI, which the DDG does not model
  // and which keep writing DefReg. Their writes land before this read, and
  // the read stays ordered before the redefinitions through anti edges.
  SmallDenseSet<MCRegUnit, 8> ReadUnits;
  for (const MachineOperand &MO : DefMI.all_uses())
    if (MO.getReg().isPhysical())
      for (MCRegUnit Unit : TRI.regunits(MO.getReg().asMCReg()))
        ReadUnits.insert(Unit);
  if (!all_of(TRI.regunits(DefReg),
              [&](MCRegUnit Unit) { return ReadUnits.contains(Unit); }))
    return std::nullopt;

  // The edges between two epilogue clones are kept regardless of their issue
  // order. Rename only when no other clone touches DefReg.
  for (const SDep &Dep : concat<const SDep>(DefSU.Preds, DefSU.Succs))
    if (isRegDepOn(Dep, DefReg, TRI) &&
        DDG.isFixedPostBoundaryNode(Dep.getSUnit()))
      return std::nullopt;

  auto IsFree = [&](const SUnit *SU) {
    return !SU->isBoundaryNode() && DDG.isPostBoundaryNode(SU) &&
           !DDG.isFixedNode(*SU);
  };

  // Data successors are the readers of the value, output successors the
  // first redefinitions of each unit. The value is dead at the end of the
  // region once every unit of DefReg is redefined.
  SmallVector<MachineOperand *, 4> Uses;
  SmallPtrSet<const SUnit *, 4> Readers;
  SmallDenseSet<MCRegUnit, 8> RedefinedUnits;
  for (const SDep &Dep : DefSU.Succs) {
    if (!isRegDepOn(Dep, DefReg, TRI))
      continue;
    const SUnit *Succ = Dep.getSUnit();
    if (Dep.getKind() == SDep::Data) {
      if (!IsFree(Succ))
        return std::nullopt;
      if (Readers.insert(Succ).second &&
          !collectReaderUses(DefSU, *Succ, DefReg, Uses))
        return std::nullopt;
    } else if (Dep.getKind() == SDep::Output) {
      if (Succ->isBoundaryNode() || !DDG.isPostBoundaryNode(Succ))
        return std::nullopt;
      for (MCRegUnit Unit : TRI.regunits(Dep.getReg().asMCReg()))
        RedefinedUnits.insert(Unit);
    }
  }
  if (Uses.empty())
    return std::nullopt;
  for (MCRegUnit Unit : TRI.regunits(DefReg))
    if (!RedefinedUnits.contains(Unit))
      return std::nullopt;

  // Renaming only pays off if a reader blocks a redefinition.
  const bool HasWAR = any_of(Readers, [&](const SUnit *Reader) {
    return any_of(Reader->Succs, [&](const SDep &Dep) {
      return Dep.getKind() == SDep::Anti && isRegDepOn(Dep, DefReg, TRI);
    });
  });
  if (!HasWAR)
    return std::nullopt;

  const MachineFunction &MF = *DefMI.getMF();
  const TargetRegisterClass *RC = DefMI.getRegClassConstraint(
      DefMI.getOperandNo(DefMO), MF.getSubtarget().getInstrInfo(), &TRI);
  if (!RC)
    return std::nullopt;
  return Candidate{DefMO, RC, std::move(Uses)};
}

void OLPWARSolver::collectUnavailableRegs(const BlockState &Loop,
                                          const BlockState &Exit,
                                          LiveRegUnits &Unavailable) const {
  Unavailable.addLiveIns(*Exit.TheBlock);
  Unavailable.addLiveOuts(*Exit.TheBlock);
  for (const MachineInstr &MI : Exit.TheBlock->instrs())
    Unavailable.accumulate(MI);
  // The epilogue and prologue bundles are not all emitted yet.
  for (const InsertDescriptor *Insert : {&Exit.TopInsert, &Exit.BottomInsert})
    for (const MachineBundle &Bundle : Insert->Bundles)
      for (const MachineInstr *MI : Bundle.getInstrs())
        Unavailable.accumulate(*MI);
  for (const MachineInstr &MI : Loop.TheBlock->instrs())
    Unavailable.accumulate(MI);
}

void OLPWARSolver::rename(Candidate &C, MCRegister NewReg) const {
  const MCRegister OldReg = C.Def->getReg().asMCReg();
  LLVM_DEBUG(dbgs() << "OLPWARSolver: renaming " << printReg(OldReg, &TRI)
                    << " -> " << printReg(NewReg, &TRI) << " in "
                    << *C.Def->getParent());
  C.Def->setReg(NewReg);
  for (MachineOperand *MO : C.Uses) {
    MO->setReg(mapRegister(MO->getReg().asMCReg(), OldReg, NewReg, TRI));
    LLVM_DEBUG(dbgs() << "  use: " << *MO->getParent());
  }
}

MachineBasicBlock *OLPWARSolver::run(const BlockState &Loop) const {
  if (!EnableOLPWARSolver)
    return nullptr;
  const auto *OLP = Loop.getOLPMembership();
  if (!OLP || Loop.TheBlock != OLP->SteadyInner)
    return nullptr;

  // The epilogue and the free instructions of the exit only stay in one block
  // if the exit is dedicated. Otherwise the epilogue moves to a new block when
  // the exit is entered.
  MachineBasicBlock *ExitMBB = OLP->SteadyBottom;
  if (ExitMBB->pred_size() != 1 || !Loop.TheBlock->isSuccessor(ExitMBB))
    return nullptr;
  const BlockState &Exit = IBS.getBlockState(ExitMBB);
  if (Exit.TopInsert.empty() || Exit.getRegions().empty())
    return nullptr;

  const auto EdgeIt = find_if(Loop.getPerSuccEdges(), [&](const auto &Edges) {
    return Edges->getSucc() == ExitMBB;
  });
  assert(EdgeIt != Loop.getPerSuccEdges().end() && "Missing loop exit edge");
  const InterBlockEdges &DDG = **EdgeIt;

  // Follow the data edges of the free instructions back into the epilogue, in
  // program order. Earlier candidates claim their new register first.
  SmallVector<Candidate, 4> Candidates;
  SmallPtrSet<const MachineOperand *, 8> SeenDefs;
  for (MachineInstr *MI : Exit.getTop().getFreeInstructions()) {
    for (const SDep &Dep : DDG.getPostBoundaryNode(MI)->Preds) {
      if (Dep.getKind() != SDep::Data ||
          !DDG.isFixedPostBoundaryNode(Dep.getSUnit()))
        continue;
      std::optional<Candidate> C =
          analyzeDef(DDG, *Dep.getSUnit(), Dep.getReg().asMCReg());
      if (C && SeenDefs.insert(C->Def).second)
        Candidates.push_back(std::move(*C));
    }
  }
  if (Candidates.empty())
    return nullptr;

  const MachineRegisterInfo &MRI = ExitMBB->getParent()->getRegInfo();
  LiveRegUnits Unavailable(TRI);
  collectUnavailableRegs(Loop, Exit, Unavailable);

  bool Changed = false;
  for (Candidate &C : Candidates) {
    const MCRegister OldReg = C.Def->getReg().asMCReg();
    auto CanUse = [&](MCRegister Reg) {
      return MRI.isAllocatable(Reg) && !MRI.isReserved(Reg) &&
             Unavailable.available(Reg) && all_of(C.Uses, [&](auto *MO) {
               return mapRegister(MO->getReg().asMCReg(), OldReg, Reg, TRI)
                   .isValid();
             });
    };
    const auto NewReg = find_if(*C.RC, CanUse);
    if (NewReg == C.RC->end()) {
      LLVM_DEBUG(dbgs() << "OLPWARSolver: no free register for "
                        << printReg(OldReg, &TRI) << "\n");
      continue;
    }
    rename(C, *NewReg);
    Unavailable.addReg(*NewReg);
    Changed = true;
  }
  return Changed ? ExitMBB : nullptr;
}

} // namespace llvm::AIE
