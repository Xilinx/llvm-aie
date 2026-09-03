//===-- AIESiblingLoopRegAligner.cpp - Align sibling loop registers -------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass aligns register allocations between sibling inner loops created
// by the Outer Loop Pipeliner (OLP). OLP creates a "steady" inner loop and
// a "peeled iteration" inner loop that execute the same code, but may receive
// different register allocations. This pass rewrites the peeled iteration
// loop's allocations to match the steady loop where possible.
//
// By aligning registers, we maximize common code between the sibling loops,
// enabling the post-pipeliner to generate more compact prologues through
// better code reuse.
//
// The alignment strategy uses three phases:
// 1. All-or-nothing (Peeled -> Steady): Try to reassign ALL peeled registers
//    to match the steady loop. Only commit if ALL succeed.
// 2. All-or-nothing (Steady -> Peeled): If phase 1 fails, try the reverse -
//    reassign ALL steady registers to match the peeled loop.
// 3. Best-effort fallback: If both fail, reassign as many peeled registers
//    as possible to match steady (partial alignment).
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "AIEBaseInstrInfo.h"
#include "AIEBaseRegisterInfo.h"
#include "Utils/AIELoopUtils.h"

#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/LiveRegMatrix.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/CodeGen/VirtRegMap.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "aie-sibling-loop-reg-align"

cl::opt<bool> EnableSiblingLoopRegAlign(
    "aie-sibling-loop-reg-align", cl::Hidden, cl::init(true),
    cl::desc("Align register allocations between OLP sibling loops"));

namespace {

/// Represents a potential register reassignment.
struct ReassignmentCandidate {
  Register VReg;        // Virtual register to reassign
  MCPhysReg TargetPhys; // Target physical register
};

class AIESiblingLoopRegAligner : public MachineFunctionPass {
public:
  static char ID;
  AIESiblingLoopRegAligner() : MachineFunctionPass(ID) {
    initializeAIESiblingLoopRegAlignerPass(*PassRegistry::getPassRegistry());
  }

  StringRef getPassName() const override {
    return "AIE sibling loop register alignment";
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    AU.setPreservesAll();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

private:
  MachineFunction *MF = nullptr;
  MachineRegisterInfo *MRI = nullptr;
  const AIEBaseRegisterInfo *TRI = nullptr;
  VirtRegMap *VRM = nullptr;
  LiveRegMatrix *LRM = nullptr;
  LiveIntervals *LIS = nullptr;

  /// Initialize analysis passes. Returns false if required analyses
  /// unavailable.
  bool initializeAnalyses(MachineFunction &Fn);

  /// Check if an OLP structure is valid for register alignment.
  bool isValidForAlignment(const AIELoopUtils::OuterLoopStructure &OLS);

  /// Build instruction mapping between sibling loops.
  SmallVector<std::pair<MachineInstr *, MachineInstr *>, 64>
  buildInstructionMapping(const AIELoopUtils::OuterLoopStructure &OLS);

  /// Check if a register's live range is entirely within a single basic block.
  /// This ensures we only rename loop-local registers (not live-ins/live-outs).
  bool isLoopLocalRegister(Register VReg,
                           const MachineBasicBlock *LoopMBB) const;

  /// Check if VReg can be reassigned to TargetPhysReg.
  /// Does NOT perform the reassignment.
  bool canReassignReg(Register VReg, MCPhysReg TargetPhysReg) const;

  /// Reassign VReg to TargetPhysReg. Assumes canReassignReg returned true.
  void reassignReg(Register VReg, MCPhysReg TargetPhysReg);

  /// Collect all potential reassignment candidates from instruction pairs.
  /// If SteadyToTarget is true: collect (PeeledReg -> SteadyPhys)
  /// If SteadyToTarget is false: collect (SteadyReg -> PeeledPhys)
  SmallVector<ReassignmentCandidate, 32> collectReassignmentCandidates(
      const SmallVectorImpl<std::pair<MachineInstr *, MachineInstr *>> &InstMap,
      const MachineBasicBlock *SteadyLoop, const MachineBasicBlock *PeeledLoop,
      bool SteadyIsTarget) const;

  /// Check if all candidates can be reassigned without interference.
  bool canReassignAll(
      const SmallVectorImpl<ReassignmentCandidate> &Candidates) const;

  /// Commit all reassignments. Call only after canReassignAll returns true.
  void
  commitReassignments(const SmallVectorImpl<ReassignmentCandidate> &Candidates);

  /// Apply best-effort reassignments (reassign as many as possible).
  unsigned
  applyBestEffort(const SmallVectorImpl<ReassignmentCandidate> &Candidates);

  /// Align register allocations for one OLP structure using 3-phase strategy.
  bool alignSiblingLoops(const AIELoopUtils::OuterLoopStructure &OLS);
};

char AIESiblingLoopRegAligner::ID = 0;

bool AIESiblingLoopRegAligner::initializeAnalyses(MachineFunction &Fn) {
  MF = &Fn;
  MRI = &Fn.getRegInfo();
  TRI = static_cast<const AIEBaseRegisterInfo *>(MRI->getTargetRegisterInfo());

  auto *VRMW = getAnalysisIfAvailable<VirtRegMapWrapperLegacy>();
  auto *LRMW = getAnalysisIfAvailable<LiveRegMatrixWrapperLegacy>();
  auto *LISW = getAnalysisIfAvailable<LiveIntervalsWrapperPass>();

  if (!VRMW || !LRMW || !LISW) {
    LLVM_DEBUG(dbgs() << "Required analyses not available, skipping\n");
    return false;
  }

  VRM = &VRMW->getVRM();
  LRM = &LRMW->getLRM();
  LIS = &LISW->getLIS();
  return true;
}

bool AIESiblingLoopRegAligner::isValidForAlignment(
    const AIELoopUtils::OuterLoopStructure &OLS) {
  if (!OLS.hasPeeledIterRegion()) {
    LLVM_DEBUG(dbgs() << "Skipping speculative OLP (no peeled iteration)\n");
    return false;
  }
  if (!OLS.hasMatchingInnerLoops()) {
    LLVM_DEBUG(dbgs() << "Inner loops structure mismatch, skipping\n");
    return false;
  }
  return true;
}

bool AIESiblingLoopRegAligner::runOnMachineFunction(MachineFunction &Fn) {
  if (!EnableSiblingLoopRegAlign)
    return false;

  LLVM_DEBUG(dbgs() << "*** Sibling Loop Register Alignment: " << Fn.getName()
                    << " ***\n");

  // First pass: collect valid OLP structures before touching analyses
  SmallVector<AIELoopUtils::OuterLoopStructure, 2> OLPStructures;

  for (MachineBasicBlock &MBB : Fn) {
    if (!AIELoopUtils::isOuterLoopPipelined(MBB))
      continue;

    auto OLS = AIELoopUtils::OuterLoopStructure::tryBuildFrom(MBB);
    if (!OLS) {
      LLVM_DEBUG(dbgs() << "Failed to build OLP structure from "
                        << MBB.getName() << "\n");
      continue;
    }

    if (isValidForAlignment(*OLS))
      OLPStructures.push_back(*OLS);
  }

  if (OLPStructures.empty()) {
    LLVM_DEBUG(dbgs() << "No OLP structures found, skipping\n");
    return false;
  }

  if (!initializeAnalyses(Fn))
    return false;

  bool Modified = false;
  for (auto &OLS : OLPStructures) {
    Modified |= alignSiblingLoops(OLS);
  }

  return Modified;
}

SmallVector<std::pair<MachineInstr *, MachineInstr *>, 64>
AIESiblingLoopRegAligner::buildInstructionMapping(
    const AIELoopUtils::OuterLoopStructure &OLS) {
  SmallVector<std::pair<MachineInstr *, MachineInstr *>, 64> Mapping;

  auto SI = OLS.SteadyInner->begin();
  auto PI = OLS.PeeledIterInner->begin();

  for (; SI != OLS.SteadyInner->end() && PI != OLS.PeeledIterInner->end();
       ++SI, ++PI) {
    Mapping.emplace_back(&*SI, &*PI);
  }

  return Mapping;
}

bool AIESiblingLoopRegAligner::isLoopLocalRegister(
    Register VReg, const MachineBasicBlock *LoopMBB) const {
  if (!VReg.isVirtual() || !LIS->hasInterval(VReg))
    return false;

  const LiveInterval &LI = LIS->getInterval(VReg);

  // Get the slot index range for the loop block
  const SlotIndex LoopStart = LIS->getMBBStartIdx(LoopMBB);
  const SlotIndex LoopEnd = LIS->getMBBEndIdx(LoopMBB);

  // All segments must be within the loop block
  return llvm::all_of(LI, [&](const LiveRange::Segment &Seg) {
    return Seg.start >= LoopStart && Seg.end <= LoopEnd;
  });
}

bool AIESiblingLoopRegAligner::canReassignReg(Register VReg,
                                              MCPhysReg TargetPhysReg) const {
  if (!VRM->hasPhys(VReg))
    return false;

  const MCPhysReg CurrentPhys = VRM->getPhys(VReg);
  if (CurrentPhys == TargetPhysReg)
    return false; // Already at target, no change needed

  // Validate register class compatibility
  const TargetRegisterClass *RC = MRI->getRegClass(VReg);
  if (!RC->contains(TargetPhysReg))
    return false;

  // Check for interference with other live ranges
  const LiveInterval &LI = LIS->getInterval(VReg);
  return LRM->checkInterference(LI, TargetPhysReg) == LiveRegMatrix::IK_Free;
}

void AIESiblingLoopRegAligner::reassignReg(Register VReg,
                                           MCPhysReg TargetPhysReg) {
  const MCPhysReg CurrentPhys = VRM->getPhys(VReg);
  const LiveInterval &LI = LIS->getInterval(VReg);

  // Perform reassignment: update both LRM and VRM consistently
  LRM->unassign(LI);
  LRM->assign(LI, TargetPhysReg);
  VRM->clearVirt(VReg);
  VRM->assignVirt2Phys(VReg, TargetPhysReg);

  LLVM_DEBUG(dbgs() << "Realigned " << printReg(VReg, TRI) << " from "
                    << TRI->getName(CurrentPhys) << " to "
                    << TRI->getName(TargetPhysReg) << "\n");
}

SmallVector<ReassignmentCandidate, 32>
AIESiblingLoopRegAligner::collectReassignmentCandidates(
    const SmallVectorImpl<std::pair<MachineInstr *, MachineInstr *>> &InstMap,
    const MachineBasicBlock *SteadyLoop, const MachineBasicBlock *PeeledLoop,
    bool SteadyIsTarget) const {
  SmallVector<ReassignmentCandidate, 32> Candidates;

  for (const auto &[SteadyMI, PeeledMI] : InstMap) {
    const unsigned NumDefs = SteadyMI->getNumDefs();

    for (unsigned I = 0; I < NumDefs; ++I) {
      const MachineOperand &SteadyOp = SteadyMI->getOperand(I);
      const MachineOperand &PeeledOp = PeeledMI->getOperand(I);

      if (!SteadyOp.isReg() || !PeeledOp.isReg())
        continue;

      const Register SteadyReg = SteadyOp.getReg();
      const Register PeeledReg = PeeledOp.getReg();

      // Both must be virtual with physical assignments
      if (!SteadyReg.isVirtual() || !PeeledReg.isVirtual())
        continue;
      if (!VRM->hasPhys(SteadyReg) || !VRM->hasPhys(PeeledReg))
        continue;

      const MCPhysReg SteadyPhys = VRM->getPhys(SteadyReg);
      const MCPhysReg PeeledPhys = VRM->getPhys(PeeledReg);

      // Skip if already aligned
      if (SteadyPhys == PeeledPhys)
        continue;

      // Check locality constraints
      if (!isLoopLocalRegister(SteadyReg, SteadyLoop) ||
          !isLoopLocalRegister(PeeledReg, PeeledLoop))
        continue;

      // Determine which register to reassign based on direction
      if (SteadyIsTarget) {
        // Reassign PeeledReg to SteadyPhys
        Candidates.push_back({PeeledReg, SteadyPhys});
      } else {
        // Reassign SteadyReg to PeeledPhys
        Candidates.push_back({SteadyReg, PeeledPhys});
      }
    }
  }

  return Candidates;
}

bool AIESiblingLoopRegAligner::canReassignAll(
    const SmallVectorImpl<ReassignmentCandidate> &Candidates) const {
  for (const auto &C : Candidates) {
    if (!canReassignReg(C.VReg, C.TargetPhys))
      return false;
  }
  return true;
}

void AIESiblingLoopRegAligner::commitReassignments(
    const SmallVectorImpl<ReassignmentCandidate> &Candidates) {
  for (const auto &C : Candidates) {
    reassignReg(C.VReg, C.TargetPhys);
  }
}

unsigned AIESiblingLoopRegAligner::applyBestEffort(
    const SmallVectorImpl<ReassignmentCandidate> &Candidates) {
  unsigned Count = 0;
  for (const auto &C : Candidates) {
    if (canReassignReg(C.VReg, C.TargetPhys)) {
      reassignReg(C.VReg, C.TargetPhys);
      Count++;
    }
  }
  return Count;
}

bool AIESiblingLoopRegAligner::alignSiblingLoops(
    const AIELoopUtils::OuterLoopStructure &OLS) {
  LLVM_DEBUG(dbgs() << "Aligning sibling loops: steady="
                    << OLS.SteadyInner->getName()
                    << " peeled=" << OLS.PeeledIterInner->getName() << "\n");

  const auto InstMap = buildInstructionMapping(OLS);
  const MachineBasicBlock *SteadyLoop = OLS.SteadyInner;
  const MachineBasicBlock *PeeledLoop = OLS.PeeledIterInner;

  // Phase 1: Try all-or-nothing with Steady as target (Peeled -> Steady)
  auto CandidatesPeeledToSteady =
      collectReassignmentCandidates(InstMap, SteadyLoop, PeeledLoop,
                                    /*SteadyIsTarget=*/true);

  if (!CandidatesPeeledToSteady.empty() &&
      canReassignAll(CandidatesPeeledToSteady)) {
    LLVM_DEBUG(dbgs() << "Phase 1 success: all "
                      << CandidatesPeeledToSteady.size()
                      << " registers aligned (Peeled -> Steady)\n");
    commitReassignments(CandidatesPeeledToSteady);
    return true;
  }

  LLVM_DEBUG(dbgs() << "Phase 1 failed, trying Phase 2\n");

  // Phase 2: Try all-or-nothing with Peeled as target (Steady -> Peeled)
  auto CandidatesSteadyToPeeled =
      collectReassignmentCandidates(InstMap, SteadyLoop, PeeledLoop,
                                    /*SteadyIsTarget=*/false);

  if (!CandidatesSteadyToPeeled.empty() &&
      canReassignAll(CandidatesSteadyToPeeled)) {
    LLVM_DEBUG(dbgs() << "Phase 2 success: all "
                      << CandidatesSteadyToPeeled.size()
                      << " registers aligned (Steady -> Peeled)\n");
    commitReassignments(CandidatesSteadyToPeeled);
    return true;
  }

  LLVM_DEBUG(dbgs() << "Phase 2 failed, falling back to best-effort\n");

  // Phase 3: Best-effort fallback (Peeled -> Steady, partial alignment)
  unsigned Aligned = applyBestEffort(CandidatesPeeledToSteady);

  LLVM_DEBUG(dbgs() << "Phase 3 (best-effort): " << Aligned << " of "
                    << CandidatesPeeledToSteady.size() << " aligned\n");

  return Aligned > 0;
}

} // end anonymous namespace

char &llvm::AIESiblingLoopRegAlignerID = AIESiblingLoopRegAligner::ID;

INITIALIZE_PASS(AIESiblingLoopRegAligner, DEBUG_TYPE,
                "AIE sibling loop register alignment", false, false)

FunctionPass *llvm::createAIESiblingLoopRegAligner() {
  return new AIESiblingLoopRegAligner();
}
