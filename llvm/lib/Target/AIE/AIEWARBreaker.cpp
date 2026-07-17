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
// Breaks write-after-read hazards that AIEOuterLoopPipeliner's epilog
// rotation can introduce, by splitting the rotated def into a fresh vreg.
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "AIEBaseInstrInfo.h"
#include "Utils/AIELoopUtils.h"
#include "Utils/AIERegUnitUtils.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/LiveDebugVariables.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/LiveRegMatrix.h"
#include "llvm/CodeGen/LiveRegUnits.h"
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

#include <optional>

using namespace llvm;

#define DEBUG_TYPE "aie-war-breaker"

static cl::opt<bool> RestrictToOuterLoopEpilog(
    "aie-warbreaker-outer-loop-epilog-only", cl::Hidden, cl::init(true),
    cl::desc("Only break WARs in outer-loop pipeliner epilogs (the default). "
             "When false, AIEWARBreaker runs on every MBB."));

namespace {

/// Per-pass cache of COPY costs keyed on (DstPhys, SrcPhys): copyPhysReg's
/// own emission is the only source of truth, so cost is measured by emitting.
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

/// Whether \p Touched already covers every lane of \p Full.
static bool coversFullRegister(LaneBitmask Touched, LaneBitmask Full) {
  return (Touched & Full) == Full;
}

/// Glue-COPY emission plan for a same-LI split: full-class or per-lane.
struct GlueCopyPlan {
  bool CoversFull = false;
  SmallVector<unsigned, 4> SubRegsInProgramOrder;
};

static GlueCopyPlan planGlueCopies(const MachineRegisterInfo &MRI,
                                   const WARCandidate &C) {
  GlueCopyPlan Plan;
  const LaneBitmask FullMask = MRI.getMaxLaneMaskForVReg(C.DefVReg);
  if (coversFullRegister(C.TouchedLanes, FullMask)) {
    Plan.CoversFull = true;
    return Plan;
  }
  DenseSet<unsigned> Seen;
  for (const MachineOperand *DefMO : C.TouchedDefOps) {
    const unsigned SubIdx = DefMO->getSubReg();
    if (!SubIdx || !Seen.insert(SubIdx).second)
      continue;
    Plan.SubRegsInProgramOrder.push_back(SubIdx);
  }
  return Plan;
}

using AIERegUnitUtils::addRegUnits;

/// Seed \p Out with the reg-units of every physreg live-in to \p MBB.
static void addLiveInUnits(const MachineBasicBlock &MBB,
                           const TargetRegisterInfo &TRI, BitVector &Out) {
  LiveRegUnits LiveIns(TRI);
  LiveIns.addLiveIns(MBB);
  Out |= LiveIns.getBitVector();
}

/// Resolves \p MO to its concrete (sub-)physreg via VRM; null if unresolved.
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

/// Blocked reg-units before \p HeadMI: live-ins plus every physreg read
/// earlier, re-resolved against live VRM so earlier splits are reflected.
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
  MachineBasicBlock &MBB;
  const MachineRegisterInfo &MRI;
  const TargetRegisterInfo &TRI;
  const VirtRegMap &VRM;

  BitVector BlockedUnits;
  BitVector DefUnitsScratch;
  DenseSet<Register> UsedVRegs;

  SmallVector<WARCandidate, 4> Candidates;
  DenseMap<Register, size_t> CandidateIndex;

  void recordCandidateDef(MachineOperand &DefMO);
  void foldUse(const MachineOperand &UseMO, const MachineInstr &MI);

  /// Rejects partial-lane candidates whose value is read on an untouched lane.
  void rejectUntouchedLaneReads();

public:
  WARScanner(MachineBasicBlock &MBB, const MachineRegisterInfo &MRI,
             const TargetRegisterInfo &TRI, const VirtRegMap &VRM)
      : MBB(MBB), MRI(MRI), TRI(TRI), VRM(VRM),
        BlockedUnits(TRI.getNumRegUnits()),
        DefUnitsScratch(TRI.getNumRegUnits()) {
    addLiveInUnits(MBB, TRI, BlockedUnits);
  }

  void scan();
  ArrayRef<WARCandidate> candidates() const { return Candidates; }
};

void WARScanner::scan() {
  for (MachineInstr &MI : MBB) {
    if (MI.isDebugInstr())
      continue;

    for (MachineOperand &DefMO : MI.all_defs())
      if (!DefMO.isTied())
        recordCandidateDef(DefMO);

    // Fold uses (after def-check, so same-instr uses are not "prior").
    for (const MachineOperand &UseMO : MI.uses())
      foldUse(UseMO, MI);
  }

  rejectUntouchedLaneReads();
}

void WARScanner::recordCandidateDef(MachineOperand &DefMO) {
  const Register DefReg = DefMO.getReg();
  const bool IsRenameableVReg =
      DefReg.isVirtual() && VRM.hasPhys(DefReg) && !VRM.hasRequiredPhys(DefReg);
  if (!IsRenameableVReg)
    return;
  const MCRegister Phys = resolveOperandToPhys(DefMO, TRI, VRM);
  if (!Phys)
    return;
  DefUnitsScratch.reset();
  addRegUnits(TRI, Phys, DefUnitsScratch);
  if (!DefUnitsScratch.anyCommon(BlockedUnits))
    return;
  if (!UsedVRegs.contains(DefReg))
    return;

  // Inline coalesce into per-vreg candidate.
  auto [It, Inserted] = CandidateIndex.try_emplace(DefReg, Candidates.size());
  if (Inserted) {
    WARCandidate New;
    New.DefVReg = DefReg;
    New.TouchedDefOps.push_back(&DefMO);
    New.TouchedLanes = laneMaskOf(DefMO, TRI);
    Candidates.push_back(std::move(New));
    return;
  }
  WARCandidate &Existing = Candidates[It->second];
  if (Existing.HasInterveningUse) {
    Existing.IsRejected = true;
    return;
  }
  if (Existing.IsRejected)
    return;
  Existing.TouchedDefOps.push_back(&DefMO);
  Existing.TouchedLanes |= laneMaskOf(DefMO, TRI);
}

void WARScanner::foldUse(const MachineOperand &UseMO, const MachineInstr &MI) {
  const MCRegister Phys = resolveOperandToPhys(UseMO, TRI, VRM);
  if (Phys)
    addRegUnits(TRI, Phys, BlockedUnits);
  if (!UseMO.isReg() || !UseMO.getReg().isVirtual())
    return;
  const Register UsedVReg = UseMO.getReg();
  UsedVRegs.insert(UsedVReg);
  auto It = CandidateIndex.find(UsedVReg);
  if (It == CandidateIndex.end())
    return;
  WARCandidate &Existing = Candidates[It->second];
  if (Existing.TouchedDefOps.back()->getParent() != &MI)
    Existing.HasInterveningUse = true;
}

void WARScanner::rejectUntouchedLaneReads() {
  // The fresh vreg only defines the touched lanes, so a downstream read of
  // an untouched lane would become undefined. Drop such candidates.
  for (WARCandidate &C : Candidates) {
    if (C.IsRejected)
      continue;
    const LaneBitmask FullMask = MRI.getMaxLaneMaskForVReg(C.DefVReg);
    if (coversFullRegister(C.TouchedLanes, FullMask))
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

  /// The rename physreg chosen for a candidate and its glue-COPY cost.
  struct RenamePlan {
    MCPhysReg Phys;
    int Cost;
  };

  /// Picks a non-CSR physreg in \p RC free of \p BlockedUnits and free
  /// over [Start, End); NoRegister if none qualifies.
  MCPhysReg pickRenamePhysReg(const TargetRegisterClass &RC,
                              const BitVector &BlockedUnits, SlotIndex Start,
                              SlotIndex End) const;

  /// Picks candidate \p C's rename physreg and its cost under \p Plan, or
  /// nullopt if no physreg survives blocking and interference.
  std::optional<RenamePlan> planRename(MachineBasicBlock &MBB,
                                       const WARCandidate &C,
                                       const GlueCopyPlan &Plan,
                                       SlotIndex MBBEnd) const;

  /// Splits \p C's def-vreg into a fresh vreg pinned to \p RenamePhys,
  /// with glue COPYs restoring the original name before the terminator.
  void splitAndRenameVReg(MachineBasicBlock &MBB, const WARCandidate &C,
                          const GlueCopyPlan &Plan, MCPhysReg RenamePhys);

  /// Rewrites \p C's touched defs to \p NewVReg and anchors the first
  /// partial-lane def as undef.
  void rewriteTouchedDefs(const WARCandidate &C, Register NewVReg) const;

  /// Renames operands of \p OldVReg strictly after \p LastDef to \p NewVReg;
  /// same-instruction uses are left on OldVReg (pre-rename value).
  void renameOperandsAfter(MachineBasicBlock &MBB, MachineInstr *LastDef,
                           Register OldVReg, Register NewVReg) const;

  /// Emits full-class or per-sub-reg glue COPYs from \p NewVReg back to
  /// \p OldVReg before \p InsertPt, per \p Plan.
  void insertGlueCopies(MachineBasicBlock &MBB,
                        MachineBasicBlock::iterator InsertPt,
                        const GlueCopyPlan &Plan, Register OldVReg,
                        Register NewVReg) const;

  /// Recompute live intervals for OldVReg and NewVReg after the split.
  void refreshIntervals(Register OldVReg, Register NewVReg, MCPhysReg OldPhys);

  /// Clears stale `dead` flags so recomputed live ranges aren't
  /// truncated at defs that now feed the glue COPY.
  void clearStaleDeadFlags(Register OldVReg, Register NewVReg) const;

  /// Re-derives OldVReg's live interval and LiveRegMatrix assignment
  /// after glue COPYs extended its range past OldPhys's recorded union.
  void reassignOldVRegInterval(Register OldVReg, MCPhysReg OldPhys);

  /// Peels any disconnected components of \p VReg's interval into
  /// fresh vregs grown into VRM (one per component, for the verifier).
  void splitDisconnectedComponents(Register VReg);

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

/// COPY cost for candidate \p C given the shared \p Plan and physreg pair;
/// the same \p Plan insertGlueCopies uses, so cost and emission agree.
static int costOfFullRename(CopyCostCache &Cache, MachineFunction &MF,
                            const TargetInstrInfo &TII,
                            const TargetRegisterInfo &TRI,
                            const GlueCopyPlan &Plan, MCRegister OldPhys,
                            MCRegister NewPhys) {
  if (Plan.CoversFull)
    return Cache.costOf(MF, TII, OldPhys, NewPhys);
  int Sum = 0;
  for (unsigned SubIdx : Plan.SubRegsInProgramOrder) {
    const MCRegister OldSub = TRI.getSubReg(OldPhys, SubIdx);
    const MCRegister NewSub = TRI.getSubReg(NewPhys, SubIdx);
    Sum += Cache.costOf(MF, TII, OldSub, NewSub);
  }
  return Sum;
}

std::optional<AIEWARBreaker::RenamePlan>
AIEWARBreaker::planRename(MachineBasicBlock &MBB, const WARCandidate &C,
                          const GlueCopyPlan &Plan, SlotIndex MBBEnd) const {
  const MCRegister OldPhys = VRM->getPhys(C.DefVReg);

  // Re-derived from scratch (not the scanner's own BlockedUnits): VRM may
  // have moved since an earlier candidate in this block was split.
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
  const MCPhysReg Phys = pickRenamePhysReg(*RC, Blocked, Start, End);
  if (!Phys)
    return std::nullopt;

  MachineFunction &MF = *MBB.getParent();
  const int Cost =
      costOfFullRename(CostCache, MF, *TII, *TRI, Plan, OldPhys, Phys);
  return RenamePlan{Phys, Cost};
}

void AIEWARBreaker::rewriteTouchedDefs(const WARCandidate &C,
                                       Register NewVReg) const {
  for (MachineOperand *DefMO : C.TouchedDefOps)
    DefMO->setReg(NewVReg);

  // The first partial-lane def is anchored undef so the recomputed LI
  // knows there is no prior value of NewVReg's other lanes to preserve.
  for (MachineOperand *DefMO : C.TouchedDefOps) {
    if (DefMO->getSubReg()) {
      DefMO->setIsUndef(true);
      break;
    }
  }
}

void AIEWARBreaker::renameOperandsAfter(MachineBasicBlock &MBB,
                                        MachineInstr *LastDef, Register OldVReg,
                                        Register NewVReg) const {
  for (MachineBasicBlock::iterator It = std::next(LastDef->getIterator()),
                                   E = MBB.end();
       It != E; ++It)
    for (MachineOperand &MO : It->operands())
      if (MO.isReg() && MO.getReg() == OldVReg)
        MO.setReg(NewVReg);
}

void AIEWARBreaker::insertGlueCopies(MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator InsertPt,
                                     const GlueCopyPlan &Plan, Register OldVReg,
                                     Register NewVReg) const {
  if (Plan.CoversFull) {
    // Full-class COPY: simpler and gives the coalescer its best shot.
    MachineInstr *Glue = BuildMI(MBB, InsertPt, DebugLoc(),
                                 TII->get(TargetOpcode::COPY), OldVReg)
                             .addReg(NewVReg)
                             .getInstr();
    LIS->InsertMachineInstrInMaps(*Glue);
    return;
  }

  // Partial lanes: one sub-reg COPY per touched index; untouched
  // lanes of NewVReg are never read by these COPYs.
  for (unsigned SubIdx : Plan.SubRegsInProgramOrder) {
    MachineInstrBuilder MIB =
        BuildMI(MBB, InsertPt, DebugLoc(), TII->get(TargetOpcode::COPY));
    MIB.addReg(OldVReg, RegState::Define, SubIdx);
    MIB.addReg(NewVReg, 0, SubIdx);
    LIS->InsertMachineInstrInMaps(*MIB.getInstr());
  }
}

void AIEWARBreaker::refreshIntervals(Register OldVReg, Register NewVReg,
                                     MCPhysReg OldPhys) {
  clearStaleDeadFlags(OldVReg, NewVReg);
  reassignOldVRegInterval(OldVReg, OldPhys);
  LIS->createAndComputeVirtRegInterval(NewVReg);
  splitDisconnectedComponents(OldVReg);
  splitDisconnectedComponents(NewVReg);
}

void AIEWARBreaker::clearStaleDeadFlags(Register OldVReg,
                                        Register NewVReg) const {
  for (MachineOperand &Def : MRI->def_operands(OldVReg))
    Def.setIsDead(false);
  for (MachineOperand &Def : MRI->def_operands(NewVReg))
    Def.setIsDead(false);
}

void AIEWARBreaker::reassignOldVRegInterval(Register OldVReg,
                                            MCPhysReg OldPhys) {
  LRM->unassign(LIS->getInterval(OldVReg));
  LIS->removeInterval(OldVReg);
  LIS->createAndComputeVirtRegInterval(OldVReg);
  LRM->assign(LIS->getInterval(OldVReg), OldPhys);
}

void AIEWARBreaker::splitDisconnectedComponents(Register VReg) {
  // OldVReg can split if the head's use killed it before the glue COPY
  // restarted it; NewVReg can split across disjoint use regions.
  SmallVector<LiveInterval *, 2> SplitLIs;
  LIS->splitSeparateComponents(LIS->getInterval(VReg), SplitLIs);
  for (size_t I = 0, E = SplitLIs.size(); I != E; ++I)
    VRM->grow();
}

void AIEWARBreaker::splitAndRenameVReg(MachineBasicBlock &MBB,
                                       const WARCandidate &C,
                                       const GlueCopyPlan &Plan,
                                       MCPhysReg RenamePhys) {
  const Register OldVReg = C.DefVReg;
  assert(OldVReg.isVirtual() && VRM->hasPhys(OldVReg) &&
         "DefVReg must be a VRM-assigned virtual register");
  const MCPhysReg OrigPhys = VRM->getPhys(OldVReg);
  const TargetRegisterClass *RC = MRI->getRegClass(OldVReg);
  const MachineBasicBlock::iterator GlueInsertPt = MBB.getFirstTerminator();

  const Register NewVReg = MRI->createVirtualRegister(RC);
  VRM->grow();
  rewriteTouchedDefs(C, NewVReg);
  renameOperandsAfter(MBB, C.TouchedDefOps.back()->getParent(), OldVReg,
                      NewVReg);
  insertGlueCopies(MBB, GlueInsertPt, Plan, OldVReg, NewVReg);
  refreshIntervals(OldVReg, NewVReg, OrigPhys);
  LRM->assign(LIS->getInterval(NewVReg), RenamePhys);
  LLVM_DEBUG(dbgs() << "  split " << printReg(OldVReg, TRI) << " -> "
                    << printReg(NewVReg, TRI) << " pinned to "
                    << printReg(RenamePhys, TRI) << "\n");
}

bool AIEWARBreaker::tryBreakWARsInMBB(MachineBasicBlock &MBB) {
  LLVM_DEBUG(dbgs() << "Try WAR break on " << MBB.getFullName() << "\n");

  WARScanner Scanner(MBB, *MRI, *TRI, *VRM);
  Scanner.scan();

  // Resolve candidates against a shared fixed slack budget; each
  // accepted split debits its glue-COPY cost from Budget.
  bool Changed = false;
  int Budget = TII->getOuterLoopEpilogCopySlack();
  const SlotIndex MBBEnd = LIS->getMBBEndIdx(&MBB);
  for (const WARCandidate &C : Scanner.candidates()) {
    if (C.IsRejected) {
      LLVM_DEBUG(dbgs() << "  rejected interrupted write chain for "
                        << printReg(C.DefVReg, TRI) << "\n");
      continue;
    }

    const GlueCopyPlan Plan = planGlueCopies(*MRI, C);
    const std::optional<RenamePlan> Rename = planRename(MBB, C, Plan, MBBEnd);
    if (!Rename) {
      LLVM_DEBUG(dbgs() << "  cost gate: no rename target for "
                        << printReg(C.DefVReg, TRI) << "\n");
      continue;
    }
    if (Rename->Cost > Budget) {
      LLVM_DEBUG(dbgs() << "  cost gate: skipping " << printReg(C.DefVReg, TRI)
                        << " (cost=" << Rename->Cost << " > budget=" << Budget
                        << ")\n");
      continue;
    }
    splitAndRenameVReg(MBB, C, Plan, Rename->Phys);
    Budget -= Rename->Cost;
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
  CSRRegs = AIERegUnitUtils::computeCalleeSavedRegSet(*TRI, *MRI);

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
