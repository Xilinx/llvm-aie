//===-- AIEEpilogueRegRewriter.cpp - Rename epilogue definitions --------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Split LiveIntervals in Outer Loop Pipelined epilogue to remove WAR register
// dependencies.
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "AIEBaseInstrInfo.h"
#include "AIEBaseRegisterInfo.h"
#include "AIESuperRegUtils.h"
#include "Utils/AIELoopOptionOverrides.h"
#include "Utils/AIERegAllocationUtils.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/LiveDebugVariables.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/LiveRegMatrix.h"
#include "llvm/CodeGen/LiveStacks.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/CodeGen/VirtRegMap.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "aie-epilogue-reg-rewriter"

static constexpr unsigned DefaultCopyBudget = 3;
static cl::opt<unsigned>
    CopyBudget("aie-epilog-copy-budget", cl::Hidden,
               cl::init(DefaultCopyBudget),
               cl::desc("Maximum materialized copy instructions per epilogue"));

// Per-block opt-in: only rename WAR definitions in a block that carries this
// hint (see AIE::LoopOptionOverrides).
static cl::opt<bool> EnableOLPWarRename(
    "aie-olp-war-rename", cl::Hidden, cl::init(false),
    cl::desc("Enable WAR register renaming on blocks carrying the hint"));

namespace {

// All definitions of OldReg in MBB that are renamed together, followed by one
// repair copy back to OldReg at the end of MBB.
struct RewriteCandidate {
  RewriteCandidate(Register OldReg, MachineBasicBlock &MBB)
      : OldReg(OldReg), MBB(&MBB) {}

  Register OldReg;
  // Epilogue block containing all recorded definitions and uses.
  MachineBasicBlock *MBB = nullptr;
  // Definitions of OldReg in reverse program order.
  SmallVector<MachineOperand *, 4> Defs;
  // Wether the Candidate contains a use.
  bool HasUse = false;
  // Whether every read of OldReg precedes every Def. Only then does renaming
  // the Defs break the WAR edge without changing what a read observes.
  bool AllUsesBeforeDefs = true;

  // A rename pays for itself only if it breaks a WAR edge, and it is valid
  // only if the block-end repair copy is not needed by an earlier read.
  bool isValid() const { return !Defs.empty() && HasUse && AllUsesBeforeDefs; }

  const MachineOperand &getFirstDef() const {
    assert(!Defs.empty() && "Candidate without a definition");
    return *Defs.back();
  }
};

class AIEEpilogueRegRewriter : public MachineFunctionPass {
public:
  static char ID;

  AIEEpilogueRegRewriter() : MachineFunctionPass(ID) {}

  /// Registers in \p AU the analyses that this pass requires and preserves.
  void getAnalysisUsage(AnalysisUsage &AU) const override;

  /// Rewrites eligible epilogue definitions in \p MF and reports whether it
  /// changed the machine function.
  bool runOnMachineFunction(MachineFunction &MF) override;

private:
  MachineRegisterInfo *MRI = nullptr;
  const AIEBaseRegisterInfo *TRI = nullptr;
  const AIEBaseInstrInfo *TII = nullptr;
  VirtRegMap *VRM = nullptr;
  LiveIntervals *LIS = nullptr;
  LiveRegMatrix *LRM = nullptr;
  LiveDebugVariables *DebugVars = nullptr;

  /// Returns the write-after-read candidates found in \p MBB, in reverse
  /// program order.
  MapVector<Register, RewriteCandidate>
  collectCandidates(MachineBasicBlock &MBB) const;

  /// Rewrites eligible candidates using the local epilogue budget and units.
  bool rewriteEpilogue(MachineBasicBlock &MBB) const;

  /// Finds a candidate register that excludes earlier epilogue selections.
  MCPhysReg findReplacementPhysReg(const RewriteCandidate &Candidate,
                                   BitVector &ReservedRegUnits) const;

  /// Renames \p Candidate's definitions to \p NewPhys and inserts its repair
  /// copy.
  void commitRewrite(RewriteCandidate &Candidate, MCPhysReg NewPhys) const;
};

void AIEEpilogueRegRewriter::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesCFG();
  AU.addRequired<VirtRegMapWrapperLegacy>();
  AU.addPreserved<VirtRegMapWrapperLegacy>();
  AU.addRequired<SlotIndexesWrapperPass>();
  AU.addPreserved<SlotIndexesWrapperPass>();
  AU.addRequired<LiveIntervalsWrapperPass>();
  AU.addPreserved<LiveIntervalsWrapperPass>();
  AU.addRequired<LiveRegMatrixWrapperLegacy>();
  AU.addPreserved<LiveRegMatrixWrapperLegacy>();
  AU.addRequired<LiveDebugVariablesWrapperLegacy>();
  AU.addPreserved<LiveDebugVariablesWrapperLegacy>();
  AU.addRequired<LiveStacksWrapperLegacy>();
  AU.addPreserved<LiveStacksWrapperLegacy>();
  MachineFunctionPass::getAnalysisUsage(AU);
}

MapVector<Register, RewriteCandidate>
AIEEpilogueRegRewriter::collectCandidates(MachineBasicBlock &MBB) const {
  MapVector<Register, RewriteCandidate> CandidatesByReg;
  auto GetOrCreateCandidate = [&](Register Reg) -> RewriteCandidate & {
    return CandidatesByReg.try_emplace(Reg, Reg, MBB).first->second;
  };

  // Scanning backwards, an instruction's reads follow its writes, so a read in
  // the defining instruction also clears AllUsesBeforeDefs.
  for (MachineInstr &MI : reverse(MBB)) {
    for (const MachineOperand &MO : MI.all_uses())
      if (MO.getReg().isVirtual())
        GetOrCreateCandidate(MO.getReg()).HasUse = true;

    for (MachineOperand &MO : MI.all_defs()) {
      if (!MO.getReg().isVirtual())
        continue;
      RewriteCandidate &Candidate = GetOrCreateCandidate(MO.getReg());
      Candidate.AllUsesBeforeDefs &= !Candidate.HasUse;
      Candidate.Defs.push_back(&MO);
    }
  }

  CandidatesByReg.remove_if(
      [](const auto &Entry) { return !Entry.second.isValid(); });
  return CandidatesByReg;
}

MCPhysReg AIEEpilogueRegRewriter::findReplacementPhysReg(
    const RewriteCandidate &Candidate, BitVector &ReservedRegUnits) const {
  // Boundary conservatively places the planned repair copy at the block end.
  const SlotIndex Boundary = LIS->getMBBEndIdx(Candidate.MBB);
  const MachineOperand &FirstDefMO = Candidate.getFirstDef();
  const SlotIndex FirstDef = LIS->getInstructionIndex(*FirstDefMO.getParent())
                                 .getRegSlot(FirstDefMO.isEarlyClobber());

  // A single prospective interval rejects every physical overlap before repair.
  LiveInterval ProspectiveLI(Candidate.OldReg, 0.0F);
  VNInfo *MainValue =
      ProspectiveLI.getNextValue(FirstDef, LIS->getVNInfoAllocator());
  ProspectiveLI.addSegment(LiveRange::Segment(FirstDef, Boundary, MainValue));

  // Temporary intervals reuse their stack address; invalidate the pointer-keyed
  // matrix cache and refresh it after preceding rewrites changed live ranges.
  LRM->invalidateVirtRegs();
  const TargetRegisterClass *RC = MRI->getRegClass(Candidate.OldReg);
  const MCPhysReg OldPhys = VRM->getPhys(Candidate.OldReg);
  BitVector ForbiddenRegUnits = ReservedRegUnits;
  AIERegAllocationUtils::reserveRegUnits(OldPhys, *TRI, ForbiddenRegUnits);
  MCPhysReg NewPhys = AIERegAllocationUtils::findFreeNonOverlappingPhysReg(
      ProspectiveLI, *RC, RC->getRegisters(), ForbiddenRegUnits, *TRI, *LRM);
  LLVM_DEBUG(dbgs() << "Epilogue register rewrite: replacement for "
                    << printReg(Candidate.OldReg, TRI) << " ("
                    << printReg(OldPhys, TRI) << ") is "
                    << printReg(NewPhys, TRI) << '\n');
  return NewPhys;
}

void AIEEpilogueRegRewriter::commitRewrite(RewriteCandidate &Candidate,
                                           MCPhysReg NewPhys) const {
  Register NewReg = MRI->cloneVirtualRegister(Candidate.OldReg);
  VRM->grow();
  for (MachineOperand *Def : Candidate.Defs)
    Def->setReg(NewReg);

  MachineInstr *Copy =
      BuildMI(*Candidate.MBB, Candidate.MBB->getFirstTerminator(), DebugLoc(),
              TII->get(TargetOpcode::COPY), Candidate.OldReg)
          .addReg(NewReg)
          .getInstr();
  LIS->InsertMachineInstrInMaps(*Copy);
  LLVM_DEBUG(
      dbgs() << "Epilogue register rewrite: rewrite "
             << printReg(Candidate.OldReg, MRI->getTargetRegisterInfo(), 0, MRI)
             << " to " << printReg(NewReg, MRI->getTargetRegisterInfo(), 0, MRI)
             << " in " << Candidate.MBB->getFullName() << '\n');

  // Repair after each rewrite because the next replacement search queries
  // LiveRegMatrix using the updated LiveIntervals. Delaying repair until all
  // candidates were rewritten would leave both analyses stale and could select
  // a physical register that now interferes with an earlier replacement.
  SmallSet<Register, 8> RegistersToRepair;
  RegistersToRepair.insert(Candidate.OldReg);
  AIESuperRegUtils::repairLiveIntervals(RegistersToRepair, *VRM, *LRM, *LIS);

  LiveInterval &NewLI = LIS->createAndComputeVirtRegInterval(NewReg);
  LIS->shrinkToUses(&NewLI);

#ifndef NDEBUG
  assert(LRM->checkInterference(NewLI, NewPhys) == LiveRegMatrix::IK_Free &&
         "Prevalidated physical register became unavailable");
#endif

  VRM->setRequiredPhys(NewReg, NewPhys);
  LRM->assign(NewLI, NewPhys);
  SmallVector<Register, 2> SplitRegs{Candidate.OldReg, NewReg};
  DebugVars->splitRegister(Candidate.OldReg, SplitRegs, *LIS);
}

bool AIEEpilogueRegRewriter::rewriteEpilogue(MachineBasicBlock &MBB) const {
  MapVector<Register, RewriteCandidate> Candidates = collectCandidates(MBB);
  if (Candidates.empty())
    return false;

  LLVM_DEBUG(dbgs() << "Epilogue register rewrite: " << Candidates.size()
                    << " candidate(s) in " << MBB.getFullName() << '\n');

  BitVector ReservedRegUnits(TRI->getNumRegUnits());
  unsigned Spent = 0;
  bool Changed = false;
  // The reverse scan discovered candidates back to front; the copy budget is
  // spent in program order.
  for (auto &[Reg, Candidate] : reverse(Candidates)) {
    LLVM_DEBUG(dbgs() << "  target " << printReg(Candidate.OldReg, TRI, 0, MRI)
                      << ", old physical register "
                      << printReg(VRM->getPhys(Candidate.OldReg), TRI) << '\n');
    if (Spent >= CopyBudget) {
      LLVM_DEBUG(dbgs() << "  skip " << printReg(Candidate.OldReg, TRI, 0, MRI)
                        << ": epilogue copy budget exhausted\n");
      break;
    }

    MCPhysReg NewPhys = findReplacementPhysReg(Candidate, ReservedRegUnits);
    if (!NewPhys) {
      LLVM_DEBUG(dbgs() << "  skip " << printReg(Candidate.OldReg, TRI, 0, MRI)
                        << ": no free non-overlapping physical register\n");
      continue;
    }

    const std::optional<unsigned> CopyCost =
        TII->getCopyCost(VRM->getPhys(Candidate.OldReg), NewPhys);
    if (!CopyCost || *CopyCost > CopyBudget - Spent) {
      LLVM_DEBUG(dbgs() << "  skip " << printReg(Candidate.OldReg, TRI, 0, MRI)
                        << ": copy cost does not fit the remaining budget\n");
      continue;
    }

    AIERegAllocationUtils::reserveRegUnits(NewPhys, *TRI, ReservedRegUnits);
    Spent += *CopyCost;
    commitRewrite(Candidate, NewPhys);
    Changed = true;
  }

  return Changed;
}

bool AIEEpilogueRegRewriter::runOnMachineFunction(MachineFunction &MF) {
  MRI = &MF.getRegInfo();
  TRI = static_cast<const AIEBaseRegisterInfo *>(MRI->getTargetRegisterInfo());
  TII = static_cast<const AIEBaseInstrInfo *>(MF.getSubtarget().getInstrInfo());
  VRM = &getAnalysis<VirtRegMapWrapperLegacy>().getVRM();
  LIS = &getAnalysis<LiveIntervalsWrapperPass>().getLIS();
  LRM = &getAnalysis<LiveRegMatrixWrapperLegacy>().getLRM();
  DebugVars = &getAnalysis<LiveDebugVariablesWrapperLegacy>().getLDV();

  bool Changed = false;
  for (MachineBasicBlock &MBB : MF) {
    AIE::LoopOptionOverrides Overrides(MBB);
    if (Overrides.get(EnableOLPWarRename))
      Changed |= rewriteEpilogue(MBB);
  }

  return Changed;
}

} // namespace

char AIEEpilogueRegRewriter::ID = 0;
char &llvm::AIEEpilogueRegRewriterID = AIEEpilogueRegRewriter::ID;

INITIALIZE_PASS_BEGIN(AIEEpilogueRegRewriter, DEBUG_TYPE,
                      "AIE epilogue register rewrite", false, false)
INITIALIZE_PASS_DEPENDENCY(VirtRegMapWrapperLegacy)
INITIALIZE_PASS_DEPENDENCY(SlotIndexesWrapperPass)
INITIALIZE_PASS_DEPENDENCY(LiveIntervalsWrapperPass)
INITIALIZE_PASS_DEPENDENCY(LiveRegMatrixWrapperLegacy)
INITIALIZE_PASS_DEPENDENCY(LiveDebugVariablesWrapperLegacy)
INITIALIZE_PASS_DEPENDENCY(LiveStacksWrapperLegacy)
INITIALIZE_PASS_END(AIEEpilogueRegRewriter, DEBUG_TYPE,
                    "AIE epilogue register rewrite", false, false)

FunctionPass *llvm::createAIEEpilogueRegRewriter() {
  return new AIEEpilogueRegRewriter();
}
