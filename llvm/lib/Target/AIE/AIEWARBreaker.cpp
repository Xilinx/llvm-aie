//===- AIEWARBreaker.cpp - Post-RA WAR breaker via vreg rename ---*- C++ -*-=//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Breaks write-after-read hazards introduced by AIEOuterLoopPipeliner in
// the steady-state epilog. A rotated def can land on a (sub-)register
// that the surrounding epilog code (or a back-edge consumer) also reads,
// forming a WAR that pushes the rotated write several bundles late.
//
// Detection (`WARScanner`) is a single intra-MBB walk over reg-units. The
// scanner seeds a "blocked" reg-unit set with everything live-in to the
// block and folds use operands in as it goes. A conflicting def is recorded
// only when its vreg appeared as a prior use in the block; candidates are
// coalesced per def-vreg via a DenseMap index.
//
// A candidate whose vreg was used earlier in the block is resolved by
// splitting it at the touched defs and pinning the fresh vreg to a
// non-blocked physreg. Glue COPYs restore the original vreg before the
// terminator.
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "AIEBaseInstrInfo.h"
#include "Utils/AIELoopUtils.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/LiveDebugVariables.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/LiveRegMatrix.h"
#include "llvm/CodeGen/LiveStacks.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/VirtRegMap.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "aie-war-breaker"

static cl::opt<bool> RestrictToOuterLoopEpilog(
    "aie-warbreaker-outer-loop-epilog-only", cl::Hidden, cl::init(true),
    cl::desc("Only break WARs in outer-loop pipeliner epilogs (the default). "
             "When false, AIEWARBreaker runs on every MBB."));

namespace {

/// Per-pass cache of COPY costs keyed on (DstPhys, SrcPhys). Each
/// miss scratch-emits TII->copyPhysReg into a throwaway MBB and
/// counts the emitted instructions. This is the only architecture-
/// agnostic way to estimate COPY cost: LLVM upstream has no
/// TargetInstrInfo::estimateCopyCost hook, and walking the register
/// class manually would duplicate the target's decomposition rules.
class CopyCostCache {
  DenseMap<std::pair<MCRegister, MCRegister>, int> Cache;

public:
  void clear() { Cache.clear(); }

  int costOf(MachineFunction &MF, const TargetInstrInfo &TII,
             MCRegister DstPhys, MCRegister SrcPhys) {
    if (!DstPhys || !SrcPhys)
      return 0;
    auto Key = std::make_pair(DstPhys, SrcPhys);
    auto It = Cache.find(Key);
    if (It != Cache.end())
      return It->second;

    MachineBasicBlock *Scratch = MF.CreateMachineBasicBlock();
    TII.copyPhysReg(*Scratch, Scratch->end(), DebugLoc(), DstPhys, SrcPhys,
                    /*KillSrc=*/true);
    const int Cost = std::distance(Scratch->begin(), Scratch->end());
    MF.deleteMachineBasicBlock(Scratch);
    Cache[Key] = Cost;
    return Cost;
  }
};

struct WARCandidate {
  Register DefVReg;
  SmallVector<MachineOperand *, 4> TouchedDefOps;
  LaneBitmask TouchedLanes;
  bool HasInterveningUse = false;
  bool IsRejected = false;
};

/// The glue-COPY emission plan for a same-LI split: either one
/// full-class COPY (CoversFull=true, SubRegs empty) or one COPY per
/// listed touched sub-reg index, in first-seen program order.
struct GlueCopyPlan {
  bool CoversFull = false;
  SmallVector<unsigned, 4> SubRegs;
};

static GlueCopyPlan planGlueCopies(const MachineRegisterInfo &MRI,
                                   const WARCandidate &C) {
  GlueCopyPlan Plan;
  const LaneBitmask FullMask = MRI.getMaxLaneMaskForVReg(C.DefVReg);
  if ((C.TouchedLanes & FullMask) == FullMask) {
    Plan.CoversFull = true;
    return Plan;
  }
  DenseSet<unsigned> Seen;
  for (const MachineOperand *DefMO : C.TouchedDefOps) {
    const unsigned SubIdx = DefMO->getSubReg();
    if (!SubIdx || !Seen.insert(SubIdx).second)
      continue;
    Plan.SubRegs.push_back(SubIdx);
  }
  return Plan;
}

/// Add the reg-units of \p Phys to \p Out. \p Out is expected to be sized
/// TRI.getNumRegUnits(); bits are added, never cleared.
static void addRegUnits(const TargetRegisterInfo &TRI, MCRegister Phys,
                        BitVector &Out) {
  for (MCRegUnit RU : TRI.regunits(Phys))
    Out.set(RU);
}

/// Seed \p Out with the reg-units of every physreg live-in to \p MBB.
static void addLiveInUnits(const MachineBasicBlock &MBB,
                           const TargetRegisterInfo &TRI, BitVector &Out) {
  for (const auto &LI : MBB.liveins())
    addRegUnits(TRI, LI.PhysReg, Out);
}

/// Resolve \p MO to its concrete (sub-)physreg via VRM. Returns 0 if the
/// operand has no register, the register is not VRM-assigned, or the
/// sub-register index does not exist.
static MCRegister resolveOperandToPhys(const MachineOperand &MO,
                                       const TargetRegisterInfo &TRI,
                                       const VirtRegMap &VRM) {
  if (!MO.isReg() || !MO.getReg())
    return MCRegister();
  const Register R = MO.getReg();
  MCRegister Phys;
  if (R.isVirtual()) {
    if (!VRM.hasPhys(R))
      return MCRegister();
    Phys = VRM.getPhys(R);
  } else {
    Phys = R.asMCReg();
  }
  if (const unsigned SubIdx = MO.getSubReg())
    Phys = TRI.getSubReg(Phys, SubIdx);
  return Phys;
}

/// Lane mask covered by the operand's sub-reg index (or full reg if none).
static LaneBitmask laneMaskOf(const MachineOperand &MO,
                              const TargetRegisterInfo &TRI) {
  if (const unsigned SubIdx = MO.getSubReg())
    return TRI.getSubRegIndexLaneMask(SubIdx);
  return LaneBitmask::getAll();
}

/// Recompute the "blocked" reg-unit set for a candidate whose chain
/// head is \p HeadMI: block everything live-in to the block plus every
/// physreg read by an instruction strictly before the head. Resolved
/// against live VRM so it reflects any splits applied to earlier
/// candidates -- there is no stored snapshot to fall out of sync.
static BitVector computeBlockedUnits(const MachineBasicBlock &MBB,
                                     const MachineInstr *HeadMI,
                                     const TargetRegisterInfo &TRI,
                                     const VirtRegMap &VRM) {
  BitVector Blocked(TRI.getNumRegUnits());
  addLiveInUnits(MBB, TRI, Blocked);
  for (const MachineInstr &MI : MBB) {
    if (&MI == HeadMI)
      break;
    if (MI.isDebugInstr())
      continue;
    for (const MachineOperand &UseMO : MI.uses())
      if (MCRegister Phys = resolveOperandToPhys(UseMO, TRI, VRM))
        addRegUnits(TRI, Phys, Blocked);
  }
  return Blocked;
}

/// Single-pass scanner over an MBB. Produces candidates that need an SSA
/// split, coalesced per def-vreg. The outer-loop-epilog filter lives in caller.
class WARScanner {
  const MachineRegisterInfo &MRI;
  const TargetRegisterInfo &TRI;
  const VirtRegMap &VRM;

  BitVector BlockedUnits;
  DenseSet<Register> UsedVRegs;

  SmallVector<WARCandidate, 4> Candidates;
  DenseMap<Register, size_t> CandidateIndex;

  /// Reject partial-lane candidates whose value is read on an untouched
  /// lane after the chain (see comment at the call site).
  void rejectUntouchedLaneReads(MachineBasicBlock &MBB);

public:
  WARScanner(const MachineBasicBlock &MBB, const MachineRegisterInfo &MRI,
             const TargetRegisterInfo &TRI, const VirtRegMap &VRM)
      : MRI(MRI), TRI(TRI), VRM(VRM), BlockedUnits(TRI.getNumRegUnits()) {
    addLiveInUnits(MBB, TRI, BlockedUnits);
  }

  void scan(MachineBasicBlock &MBB);
  ArrayRef<WARCandidate> candidates() const { return Candidates; }
};

void WARScanner::scan(MachineBasicBlock &MBB) {
  BitVector DefUnits(TRI.getNumRegUnits());
  for (MachineInstr &MI : MBB) {
    if (MI.isDebugInstr())
      continue;

    // Check non-tied renameable defs against the running blocked set.
    for (MachineOperand &DefMO : MI.all_defs()) {
      if (DefMO.isTied())
        continue;
      const Register DefReg = DefMO.getReg();
      const bool IsRenameableVReg = DefReg.isVirtual() && VRM.hasPhys(DefReg) &&
                                    !VRM.hasRequiredPhys(DefReg);
      if (!IsRenameableVReg)
        continue;
      const MCRegister Phys = resolveOperandToPhys(DefMO, TRI, VRM);
      if (!Phys)
        continue;
      DefUnits.reset();
      addRegUnits(TRI, Phys, DefUnits);
      if (!DefUnits.anyCommon(BlockedUnits))
        continue;
      if (!UsedVRegs.contains(DefReg))
        continue;

      // Inline coalesce into per-vreg candidate.
      auto [It, Inserted] =
          CandidateIndex.try_emplace(DefReg, Candidates.size());
      if (Inserted) {
        WARCandidate New;
        New.DefVReg = DefReg;
        New.TouchedDefOps.push_back(&DefMO);
        New.TouchedLanes = laneMaskOf(DefMO, TRI);
        Candidates.push_back(std::move(New));
      } else {
        WARCandidate &Existing = Candidates[It->second];
        if (Existing.HasInterveningUse) {
          Existing.IsRejected = true;
          continue;
        }
        if (Existing.IsRejected)
          continue;
        Existing.TouchedDefOps.push_back(&DefMO);
        Existing.TouchedLanes |= laneMaskOf(DefMO, TRI);
      }
    }

    // Fold uses (after def-check, so same-instr uses are not "prior").
    for (const MachineOperand &UseMO : MI.uses()) {
      const MCRegister Phys = resolveOperandToPhys(UseMO, TRI, VRM);
      if (Phys)
        addRegUnits(TRI, Phys, BlockedUnits);
      if (!UseMO.isReg() || !UseMO.getReg().isVirtual())
        continue;
      const Register UsedVReg = UseMO.getReg();
      UsedVRegs.insert(UsedVReg);
      auto It = CandidateIndex.find(UsedVReg);
      if (It == CandidateIndex.end())
        continue;
      WARCandidate &Existing = Candidates[It->second];
      if (Existing.TouchedDefOps.back()->getParent() != &MI)
        Existing.HasInterveningUse = true;
    }
  }

  rejectUntouchedLaneReads(MBB);
}

void WARScanner::rejectUntouchedLaneReads(MachineBasicBlock &MBB) {
  // splitAndRenameVReg rewrites *every* post-chain operand of the vreg
  // to the fresh vreg, which only defines the touched lanes. A read of
  // an untouched lane after the last touched def would therefore become
  // an undefined read of the fresh vreg's un-defined lanes. Full-coverage
  // chains (TouchedLanes == the vreg's full mask) can never trip this,
  // so the common case is unaffected; only partial-lane chains with a
  // downstream read outside the touched lanes are dropped.
  for (WARCandidate &C : Candidates) {
    if (C.IsRejected)
      continue;
    const LaneBitmask FullMask = MRI.getMaxLaneMaskForVReg(C.DefVReg);
    if ((C.TouchedLanes & FullMask) == FullMask)
      continue;
    const LaneBitmask UntouchedLanes = ~C.TouchedLanes & FullMask;
    const MachineInstr *LastDef = C.TouchedDefOps.back()->getParent();
    for (MachineBasicBlock::const_iterator
             It = std::next(LastDef->getIterator()),
             E = MBB.end();
         It != E && !C.IsRejected; ++It)
      for (const MachineOperand &MO : It->operands()) {
        if (!MO.isReg() || MO.getReg() != C.DefVReg || !MO.readsReg())
          continue;
        if ((laneMaskOf(MO, TRI) & UntouchedLanes).any()) {
          C.IsRejected = true;
          break;
        }
      }
  }
}

class AIEWARBreaker : public MachineFunctionPass {
  MachineRegisterInfo *MRI = nullptr;
  const TargetRegisterInfo *TRI = nullptr;
  const AIEBaseInstrInfo *TII = nullptr;
  VirtRegMap *VRM = nullptr;
  LiveRegMatrix *LRM = nullptr;
  LiveIntervals *LIS = nullptr;
  CopyCostCache CostCache;
  BitVector CSRRegs;

  /// Pick a non-CSR physreg in \p RC whose reg-units do not overlap
  /// \p BlockedUnits and that is free over [Start, End). Returns
  /// NoRegister if none survives.
  MCPhysReg pickRenamePhysReg(const TargetRegisterClass &RC,
                              const BitVector &BlockedUnits, SlotIndex Start,
                              SlotIndex End) const;

  /// SSA-level split for a same-LI candidate. Allocates a
  /// fresh vreg, rewrites the touched defs (and any uses strictly
  /// after the last touched def) to the new vreg, emits glue COPYs
  /// before the terminator to restore the original vreg's name, and
  /// pins the fresh vreg via LRM.
  void splitAndRenameVReg(MachineBasicBlock &MBB, const WARCandidate &C,
                          MCPhysReg RenamePhys);

  /// Rewrite the touched def operands of \p OldVReg to \p NewVReg,
  /// plus any operands of \p OldVReg in instructions strictly after
  /// the last touched def. Uses on the same instructions as a
  /// rewritten def are left on OldVReg (they read the pre-rename
  /// value).
  void rewriteTouchedDefs(MachineBasicBlock &MBB, const WARCandidate &C,
                          Register NewVReg) const;

  /// Emit one COPY per touched sub-reg index of \p OldVReg from
  /// \p NewVReg before \p InsertPt. When the touched lanes cover the
  /// full register class, emit a single full-class COPY instead.
  void insertGlueCopies(MachineBasicBlock &MBB,
                        MachineBasicBlock::iterator InsertPt,
                        const WARCandidate &C, Register OldVReg,
                        Register NewVReg) const;

  /// Recompute live intervals for OldVReg and NewVReg after the split.
  /// Clears stale `dead` flags, removes + recomputes both intervals,
  /// re-pins OldVReg to \p OldPhys in LiveRegMatrix (the glue COPYs
  /// extend OldVReg's range, so its LRM assignment must be dropped and
  /// redone against the recomputed interval), and splits disconnected
  /// components of OldVReg into fresh vregs.
  void refreshIntervals(Register OldVReg, Register NewVReg, MCPhysReg OldPhys);

  bool tryBreakWARsInMBB(MachineBasicBlock &MBB);

public:
  static char ID;
  AIEWARBreaker() : MachineFunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    AU.addRequired<VirtRegMapWrapperLegacy>();
    AU.addPreserved<VirtRegMapWrapperLegacy>();
    AU.addRequired<SlotIndexesWrapperPass>();
    AU.addPreserved<SlotIndexesWrapperPass>();
    AU.addRequired<LiveDebugVariablesWrapperLegacy>();
    AU.addPreserved<LiveDebugVariablesWrapperLegacy>();
    AU.addRequired<LiveStacksWrapperLegacy>();
    AU.addPreserved<LiveStacksWrapperLegacy>();
    AU.addRequired<LiveIntervalsWrapperPass>();
    AU.addPreserved<LiveIntervalsWrapperPass>();
    AU.addRequired<LiveRegMatrixWrapperLegacy>();
    AU.addPreserved<LiveRegMatrixWrapperLegacy>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &MF) override;
};

MCPhysReg AIEWARBreaker::pickRenamePhysReg(const TargetRegisterClass &RC,
                                           const BitVector &BlockedUnits,
                                           SlotIndex Start,
                                           SlotIndex End) const {
  BitVector Scratch(TRI->getNumRegUnits());
  for (MCPhysReg P : RC.getRegisters()) {
    if (CSRRegs.test(P))
      continue;
    if (!MRI->isAllocatable(P))
      continue;
    Scratch.reset();
    addRegUnits(*TRI, P, Scratch);
    if (Scratch.anyCommon(BlockedUnits))
      continue;
    if (LRM->checkInterference(Start, End, P))
      continue;
    return P;
  }
  return MCRegister::NoRegister;
}

void AIEWARBreaker::rewriteTouchedDefs(MachineBasicBlock &MBB,
                                       const WARCandidate &C,
                                       Register NewVReg) const {
  const Register OldVReg = C.DefVReg;

  // Rewrite the touched def operands themselves. The first touched
  // def's IsUndefinied flag is set so the recomputed LI knows there is no
  // prior value of NewVReg's other lanes to preserve. Without this
  // anchor, partial-lane defs leave NewVReg's main range
  // underdefined and the verifier rejects subsequent sub-reg reads.
  bool First = true;
  for (MachineOperand *DefMO : C.TouchedDefOps) {
    DefMO->setReg(NewVReg);
    if (First && DefMO->getSubReg()) {
      DefMO->setIsUndef(true);
      First = false;
    }
  }

  // Find the last touched def's MI, then rewrite any operands of
  // OldVReg in strictly later instructions. Operands on the touched-
  // def MIs themselves (including uses of OldVReg there) are left
  // alone -- those uses read the pre-rename value.
  MachineInstr *LastDef = C.TouchedDefOps.back()->getParent();

  for (MachineBasicBlock::iterator It = std::next(LastDef->getIterator()),
                                   E = MBB.end();
       It != E; ++It)
    for (MachineOperand &MO : It->operands())
      if (MO.isReg() && MO.getReg() == OldVReg)
        MO.setReg(NewVReg);
}

void AIEWARBreaker::insertGlueCopies(MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator InsertPt,
                                     const WARCandidate &C, Register OldVReg,
                                     Register NewVReg) const {
  const GlueCopyPlan Plan = planGlueCopies(*MRI, C);

  if (Plan.CoversFull) {
    // Touched lanes cover the full register: one full-class COPY is
    // both simpler and gives the same-bank coalescer the best shot
    // at folding it away.
    MachineInstr *Glue = BuildMI(MBB, InsertPt, DebugLoc(),
                                 TII->get(TargetOpcode::COPY), OldVReg)
                             .addReg(NewVReg)
                             .getInstr();
    LIS->InsertMachineInstrInMaps(*Glue);
    return;
  }

  // Partial-lane case: emit one sub-reg COPY per touched sub-reg
  // index, in program order from the touched defs. The def-side of
  // each COPY is a partial def of OldVReg's sub-reg, leaving the
  // un-rewritten lanes intact on OldVReg's original physreg. The
  // use-side reads only the named sub-reg lane of NewVReg, so
  // NewVReg's un-touched lanes are never read.
  for (unsigned SubIdx : Plan.SubRegs) {
    MachineInstrBuilder MIB =
        BuildMI(MBB, InsertPt, DebugLoc(), TII->get(TargetOpcode::COPY));
    MIB.addReg(OldVReg, RegState::Define, SubIdx);
    MIB.addReg(NewVReg, 0, SubIdx);
    LIS->InsertMachineInstrInMaps(*MIB.getInstr());
  }
}

void AIEWARBreaker::refreshIntervals(Register OldVReg, Register NewVReg,
                                     MCPhysReg OldPhys) {
  // Stale `dead` flags on defs that now feed the glue COPY would
  // truncate the recomputed range short of the COPY's use. Clear
  // them; createAndComputeVirtRegInterval will re-derive both range
  // and dead-ness from the current operand graph.
  for (MachineOperand &Def : MRI->def_operands(OldVReg))
    Def.setIsDead(false);
  for (MachineOperand &Def : MRI->def_operands(NewVReg))
    Def.setIsDead(false);

  // The glue COPYs extend OldVReg's live range up to the terminator,
  // so LiveRegMatrix's union for OldPhys still records the pre-split
  // (shorter) range and a soon-to-be-freed LiveInterval*. Drop the
  // LRM assignment before removing the interval, then re-pin OldVReg
  // to OldPhys against the recomputed interval so the follow-up
  // greedy allocator sees an accurate occupancy for OldPhys.
  LRM->unassign(LIS->getInterval(OldVReg));
  LIS->removeInterval(OldVReg);
  LIS->createAndComputeVirtRegInterval(OldVReg);
  LRM->assign(LIS->getInterval(OldVReg), OldPhys);
  LIS->createAndComputeVirtRegInterval(NewVReg);

  // After the split, either vreg may end up with disconnected
  // components in its recomputed LI: OldVReg if the chain head's use
  // killed it and the glue COPY restarted it; NewVReg if multiple
  // touched defs feed disjoint use regions. Peel both into separate
  // vregs so the verifier (and follow-up greedy) see one vreg per
  // component. Each cloned vreg must be grown into VRM.
  auto SplitComponents = [&](Register V) {
    SmallVector<LiveInterval *, 2> SplitLIs;
    LIS->splitSeparateComponents(LIS->getInterval(V), SplitLIs);
    for (LiveInterval *NewLI : SplitLIs) {
      (void)NewLI;
      VRM->grow();
    }
  };
  SplitComponents(OldVReg);
  SplitComponents(NewVReg);
}

/// Compute the COPY cost that splitAndRenameVReg + insertGlueCopies
/// would incur for candidate \p C with the given physreg pair. The
/// computation mirrors insertGlueCopies's emission: a single
/// full-class COPY when touched lanes cover the full register, else
/// the sum of per-touched-sub-reg COPY costs.
static int costOfFullRename(CopyCostCache &Cache, MachineFunction &MF,
                            const TargetInstrInfo &TII,
                            const TargetRegisterInfo &TRI,
                            const MachineRegisterInfo &MRI,
                            const WARCandidate &C, MCRegister OldPhys,
                            MCRegister NewPhys) {
  const GlueCopyPlan Plan = planGlueCopies(MRI, C);
  if (Plan.CoversFull)
    return Cache.costOf(MF, TII, OldPhys, NewPhys);
  int Sum = 0;
  for (unsigned SubIdx : Plan.SubRegs) {
    const MCRegister OldSub = TRI.getSubReg(OldPhys, SubIdx);
    const MCRegister NewSub = TRI.getSubReg(NewPhys, SubIdx);
    Sum += Cache.costOf(MF, TII, OldSub, NewSub);
  }
  return Sum;
}

void AIEWARBreaker::splitAndRenameVReg(MachineBasicBlock &MBB,
                                       const WARCandidate &C,
                                       MCPhysReg RenamePhys) {
  const Register OldVReg = C.DefVReg;
  assert(OldVReg.isVirtual() && VRM->hasPhys(OldVReg) &&
         "DefVReg must be a VRM-assigned virtual register");
  const MCPhysReg OrigPhys = VRM->getPhys(OldVReg);
  const TargetRegisterClass *RC = MRI->getRegClass(OldVReg);

  // The rename target was already chosen by the caller against the same
  // blocked set used for cost estimation; reuse the terminator iterator
  // for the glue-COPY insertion point.
  const MachineBasicBlock::iterator GlueInsertPt = MBB.getFirstTerminator();

  const Register NewVReg = MRI->createVirtualRegister(RC);
  VRM->grow();
  rewriteTouchedDefs(MBB, C, NewVReg);
  insertGlueCopies(MBB, GlueInsertPt, C, OldVReg, NewVReg);
  refreshIntervals(OldVReg, NewVReg, OrigPhys);
  LRM->assign(LIS->getInterval(NewVReg), RenamePhys);
  LLVM_DEBUG(dbgs() << "  split " << printReg(OldVReg, TRI) << " -> "
                    << printReg(NewVReg, TRI) << " pinned to "
                    << printReg(RenamePhys, TRI) << "\n");
}

bool AIEWARBreaker::tryBreakWARsInMBB(MachineBasicBlock &MBB) {
  LLVM_DEBUG(dbgs() << "Try WAR break on " << MBB.getFullName() << "\n");

  WARScanner Scanner(MBB, *MRI, *TRI, *VRM);
  Scanner.scan(MBB);

  // Resolve candidates against a shared fixed slack budget; each
  // accepted split debits its glue-COPY cost from Budget.
  bool Changed = false;
  int Budget = TII->getOuterLoopEpilogCopySlack();
  MachineFunction &MF = *MBB.getParent();
  const SlotIndex MBBEnd = LIS->getMBBEndIdx(&MBB);
  for (const WARCandidate &C : Scanner.candidates()) {
    if (C.IsRejected) {
      LLVM_DEBUG(dbgs() << "  rejected interrupted write chain for "
                        << printReg(C.DefVReg, TRI) << "\n");
      continue;
    }

    // Pick the rename physreg for cost estimation. If it fits the
    // budget the same physreg is passed to splitAndRenameVReg, so the
    // estimated COPY pair is exactly the one emitted.
    const MCRegister OldPhys = VRM->getPhys(C.DefVReg);
    BitVector Blocked = computeBlockedUnits(
        MBB, C.TouchedDefOps.front()->getParent(), *TRI, *VRM);
    addRegUnits(*TRI, OldPhys, Blocked);
    const TargetRegisterClass *RC = MRI->getRegClass(C.DefVReg);
    const SlotIndex Start =
        LIS->getInstructionIndex(*C.TouchedDefOps.front()->getParent());
    const SlotIndex End =
        MBB.getFirstTerminator() == MBB.end()
            ? MBBEnd
            : LIS->getInstructionIndex(*MBB.getFirstTerminator());
    const MCPhysReg TentativePhys = pickRenamePhysReg(*RC, Blocked, Start, End);
    if (!TentativePhys) {
      LLVM_DEBUG(dbgs() << "  cost gate: no rename target for "
                        << printReg(C.DefVReg, TRI) << "\n");
      continue;
    }
    const int Cost = costOfFullRename(CostCache, MF, *TII, *TRI, *MRI, C,
                                      OldPhys, TentativePhys);
    if (Cost > Budget) {
      LLVM_DEBUG(dbgs() << "  cost gate: skipping " << printReg(C.DefVReg, TRI)
                        << " (cost=" << Cost << " > budget=" << Budget
                        << ")\n");
      continue;
    }
    splitAndRenameVReg(MBB, C, TentativePhys);
    Budget -= Cost;
    Changed = true;
  }
  return Changed;
}

bool AIEWARBreaker::runOnMachineFunction(MachineFunction &MF) {
  MRI = &MF.getRegInfo();
  TRI = MRI->getTargetRegisterInfo();
  TII = static_cast<const AIEBaseInstrInfo *>(MF.getSubtarget().getInstrInfo());
  CostCache.clear();
  VRM = &getAnalysis<VirtRegMapWrapperLegacy>().getVRM();
  LRM = &getAnalysis<LiveRegMatrixWrapperLegacy>().getLRM();
  LIS = &getAnalysis<LiveIntervalsWrapperPass>().getLIS();
  CSRRegs = BitVector(TRI->getNumRegs());
  for (const MCPhysReg *CSR = MRI->getCalleeSavedRegs(); CSR && *CSR; ++CSR)
    CSRRegs.set(*CSR);

  LLVM_DEBUG(dbgs() << "*** AIE WAR Breaker: " << MF.getName() << " ***\n");

  bool Changed = false;
  for (MachineBasicBlock &MBB : MF)
    if (!RestrictToOuterLoopEpilog || AIELoopUtils::isOuterLoopEpilog(MBB))
      Changed |= tryBreakWARsInMBB(MBB);
  return Changed;
}

} // end anonymous namespace

char AIEWARBreaker::ID = 0;
INITIALIZE_PASS(AIEWARBreaker, DEBUG_TYPE, "AIE WAR breaker", false, false)

llvm::FunctionPass *llvm::createAIEWARBreaker() { return new AIEWARBreaker(); }
