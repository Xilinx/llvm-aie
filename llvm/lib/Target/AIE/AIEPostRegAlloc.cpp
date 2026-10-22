//===- AIEPostRegAlloc.cpp - Post-scheduling register allocator ----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements a post-scheduling register allocator for AIE targets.
//
//===----------------------------------------------------------------------===//

#include "AIEPostRegAlloc.h"
#include "AIELivenessVector.h"
#include "AIERegDefUseTracker.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <optional>
#include <vector>

#define DEBUG_TYPE "aie-postregalloc"

using namespace llvm;
using namespace llvm::AIE;

static cl::opt<unsigned> PostRegAllocExactBudget(
    "aie-postregalloc-exact-budget", cl::Hidden, cl::init(0),
    cl::desc("Node budget for the exact post-regalloc search. "
             "0 uses the heuristic strategies"));

// Initialize allocation state and compute interference graphs.
void AIEPostRegAlloc::AllocState::init(
    const TargetRegisterInfo *InTRI,
    const DenseMap<unsigned, AIE::LivenessVector> &LiveLanesByLRIndex,
    const RegLiveRangeTracker *RegTracker) {
  this->TRI = InTRI;

  const unsigned II = LiveLanesByLRIndex.begin()->second.size();
  this->CycleOccupancy.assign(II, PartSet());

  // Pre-compute the per-physreg PartSet cache and the per-LR admissible
  // RegUnit set. The admissible set is the union of RegUnits of all admissible
  // physical registers, i.e., all RegUnits that this live range could
  // potentially occupy.
  this->PhysRegUnitSets.clear();
  this->AdmissibleRegUnits.clear();
  for (const RegLiveRange &LR : RegTracker->getLiveRanges()) {
    PartSet LRUnits;
    for (MCRegister Reg : LR.getAdmissibleRegs()) {
      PartSet &PhysRegUnits = this->PhysRegUnitSets[Reg];
      if (PhysRegUnits.empty())
        for (MCRegUnitIterator UI(Reg, InTRI); UI.isValid(); ++UI)
          PhysRegUnits.set(*UI);
      LRUnits |= PhysRegUnits;
    }
    this->AdmissibleRegUnits[LR.getIndex()] = LRUnits;
  }

  const auto &AvailableRegs = RegTracker->getAvailablePhysRegs();

  // Build register class interference graph once.
  DenseSet<unsigned> UsedRCIds;
  for (const RegLiveRange &LR : RegTracker->getLiveRanges()) {
    if (const TargetRegisterClass *RC = LR.getRegisterClass())
      UsedRCIds.insert(RC->getID());
  }
  this->RCInterferenceGraph =
      AIEPostRegAlloc::buildRCInterferenceGraph(UsedRCIds, *InTRI);

  // Build live range interference graph once.
  this->VRegInterferenceGraph = AIEPostRegAlloc::buildVRegInterferenceGraph(
      LiveLanesByLRIndex, AdmissibleRegUnits);

  // Pre-compute metrics for all live ranges that have been virtualized.
  // Non-virtualized ranges (RESERVED or policy-excluded) have no VReg and
  // are skipped.
  this->AllMetrics.clear();
  for (const RegLiveRange &LR : RegTracker->getLiveRanges()) {
    if (!LR.getVReg().isValid())
      continue;
    const unsigned LRIndex = LR.getIndex();
    auto It = LiveLanesByLRIndex.find(LRIndex);
    if (It == LiveLanesByLRIndex.end())
      continue;
    const AIE::LivenessVector &Masks = It->second;
    AllMetrics[LRIndex] = AIEPostRegAlloc::computeMetrics(
        LR, Masks, VRegInterferenceGraph, LiveLanesByLRIndex,
        RCInterferenceGraph, AvailableRegs, *RegTracker, *InTRI);
  }
}

// RegUnits of PhysReg covered by Mask. Empty when Mask is empty.
// Only units whose coverage intersects Mask are included, so non-overlapping
// sub-registers of a composite can be co-allocated. Bypass cycles carry the
// same lane mask as the RF write (same def, same register).
static PartSet laneActiveUnits(Register PhysReg, LaneBitmask Mask,
                               const TargetRegisterInfo *TRI) {
  PartSet Units;
  if (!Mask.any())
    return Units;
  for (MCRegUnitMaskIterator MUI(PhysReg.asMCReg(), TRI); MUI.isValid();
       ++MUI) {
    auto [Unit, UnitMask] = *MUI;
    if ((UnitMask & Mask).any())
      Units.set(Unit);
  }
  return Units;
}

// Check if PhysReg can accommodate VRegMasks without conflicts.
bool AIEPostRegAlloc::AllocState::canPlace(
    Register PhysReg, const AIE::LivenessVector &VRegMasks) const {
  for (size_t C = 0; C < VRegMasks.size(); ++C) {
    const PartSet Active =
        laneActiveUnits(PhysReg, VRegMasks[C].getLanes(), TRI);
    if (Active.overlap(CycleOccupancy[C]))
      return false;
  }
  return true;
}

// Place VReg in PhysReg (marks lane-accurate RegUnits occupied per cycle).
void AIEPostRegAlloc::AllocState::place(Register VReg, Register PhysReg,
                                        const AIE::LivenessVector &VRegMasks,
                                        const TargetRegisterClass *RC) {
  for (size_t C = 0; C < VRegMasks.size(); ++C)
    CycleOccupancy[C] |= laneActiveUnits(PhysReg, VRegMasks[C].getLanes(), TRI);

  LLVM_DEBUG(dbgs() << "  Placed " << printReg(VReg, TRI) << " in "
                    << printReg(PhysReg, TRI) << "\n");
}

// Build register class interference graph with asymmetric weights.
AIEPostRegAlloc::WeightedAsymmetricGraph
AIEPostRegAlloc::buildRCInterferenceGraph(const DenseSet<unsigned> &UsedRCIds,
                                          const TargetRegisterInfo &TRI) {
  WeightedAsymmetricGraph Graph;

  // Check all ordered pairs of register classes.
  for (unsigned RCId1 : UsedRCIds) {
    const TargetRegisterClass *RC1 = TRI.getRegClass(RCId1);

    for (unsigned RCId2 : UsedRCIds) {
      if (RCId1 == RCId2)
        continue;

      const TargetRegisterClass *RC2 = TRI.getRegClass(RCId2);
      unsigned RC2Size = std::distance(RC2->begin(), RC2->end());

      // Count how many RC1 registers are blocked by each RC2 register.
      // For asymmetric weight: if I allocate one register from RC2,
      // how many RC1 registers become unavailable on average?
      unsigned TotalRC1Blocked = 0;

      for (MCPhysReg Reg2 : *RC2) {
        unsigned RC1BlockedByThisReg2 = 0;
        for (MCPhysReg Reg1 : *RC1) {
          if (TRI.regsOverlap(Reg1, Reg2)) {
            RC1BlockedByThisReg2++;
          }
        }
        TotalRC1Blocked += RC1BlockedByThisReg2;
      }

      if (TotalRC1Blocked > 0) {
        // Weight = average number of RC1 registers blocked per RC2 register.
        // Scale by 100 to preserve precision.
        // This gives asymmetric weights:
        // - eY -> VEC512: each VEC512 blocks ~0.5 eY registers
        // - VEC512 -> eY: each eY blocks ~2 VEC512 registers
        unsigned Weight = (TotalRC1Blocked * 100) / RC2Size;
        // Ensure minimum weight of 1 for any overlap.
        Weight = std::max(1u, Weight);
        Graph.addInterference(RCId1, RCId2, Weight);

        LLVM_DEBUG(dbgs() << "RC interference: " << TRI.getRegClassName(RC1)
                          << " -> " << TRI.getRegClassName(RC2)
                          << " weight=" << Weight << " (avg " << TotalRC1Blocked
                          << "/" << RC2Size << ")\n");
      }
    }
  }

  return Graph;
}

// Build live range interference graph (symmetric).
AIEPostRegAlloc::WeightedSymmetricGraph
AIEPostRegAlloc::buildVRegInterferenceGraph(
    const DenseMap<unsigned, AIE::LivenessVector> &LiveLanesByLRIndex,
    const DenseMap<unsigned, PartSet> &AdmissibleRegUnits) {

  WeightedSymmetricGraph Graph;

  // Live ranges with a non-empty admissible RegUnit set.
  std::vector<std::pair<unsigned, const PartSet *>> Candidates;
  for (const auto &[LRIndex, _] : LiveLanesByLRIndex) {
    const auto It = AdmissibleRegUnits.find(LRIndex);
    if (It == AdmissibleRegUnits.end() || It->second.empty())
      continue;
    Candidates.emplace_back(LRIndex, &It->second);
  }

  // Check all pairs of live ranges.
  // Use symmetry: only check pairs where I < J.
  for (size_t I = 0; I < Candidates.size(); ++I) {
    const auto &[LRIndex1, Units1] = Candidates[I];
    const auto &Masks1 = LiveLanesByLRIndex.find(LRIndex1)->second;

    for (size_t J = I + 1; J < Candidates.size(); ++J) {
      const auto &[LRIndex2, Units2] = Candidates[J];

      // First check if their admissible RegUnit sets overlap.
      if (!Units1->overlap(*Units2))
        continue;

      // Then check temporal concurrency: are they live at the same cycle?
      const auto &Masks2 = LiveLanesByLRIndex.find(LRIndex2)->second;
      if (Masks1.overlaps(Masks2))
        Graph.addInterference(LRIndex1, LRIndex2);
    }
  }

  return Graph;
}

namespace {

// Intersection of a live range's admissible registers and the allocatable pool.
std::vector<Register>
getCandidatePhysRegs(const DenseSet<MCRegister> &AdmissibleRegs,
                     const DenseSet<MCRegister> &AvailableRegs) {
  std::vector<Register> Candidates;
  for (MCRegister PhysReg : AdmissibleRegs)
    if (AvailableRegs.count(PhysReg))
      Candidates.push_back(PhysReg);
  return Candidates;
}

} // namespace

AIEPostRegAlloc::Placement::Placement(
    const DenseMap<unsigned, AIE::LivenessVector> &Lanes,
    const RegLiveRangeTracker &Tracker, AllocState &InState,
    DenseMap<Register, MCRegister> &InAssign)
    : LiveLanes(Lanes), AvailableRegs(Tracker.getAvailablePhysRegs()),
      State(InState), OutAssign(InAssign),
      Placed(Tracker.getLiveRanges().size()) {
  // Non-virtualizable ranges are kept: they hold a register the pool can hand
  // out, so they must occupy it. A virtualizable range without a VReg was left
  // physical because it is RESERVED or overlaps a RESERVED base. That register
  // is not in the pool, so the range constrains nothing.
  for (const RegLiveRange &LR : Tracker.getLiveRanges()) {
    if (!LiveLanes.count(LR.getIndex()))
      continue;
    if (LR.isVirtualizable() && !LR.getVReg().isValid())
      continue;
    Ranges.push_back(&LR);
  }
}

const AIE::LivenessVector &
AIEPostRegAlloc::Placement::masks(const RegLiveRange &LR) const {
  return LiveLanes.find(LR.getIndex())->second;
}

std::vector<Register>
AIEPostRegAlloc::Placement::candidates(const RegLiveRange &LR) const {
  std::vector<Register> Cands;
  for (Register PhysReg :
       getCandidatePhysRegs(LR.getAdmissibleRegs(), AvailableRegs))
    if (State.canPlace(PhysReg, masks(LR)))
      Cands.push_back(PhysReg);
  return Cands;
}

void AIEPostRegAlloc::Placement::place(const RegLiveRange &LR,
                                       Register PhysReg) {
  State.place(LR.getVReg(), PhysReg, masks(LR), LR.getRegisterClass());
  if (LR.getVReg().isValid())
    OutAssign[LR.getVReg()] = PhysReg.asMCReg();
  Placed.set(LR.getIndex());
}

// Compute metrics for a live range.
AIEPostRegAlloc::VRegMetrics AIEPostRegAlloc::computeMetrics(
    const RegLiveRange &LR, const AIE::LivenessVector &Masks,
    const WeightedSymmetricGraph &VRegInterferenceGraph,
    const DenseMap<unsigned, AIE::LivenessVector> &LiveLanesByLRIndex,
    const WeightedAsymmetricGraph &RCInterferenceGraph,
    const DenseSet<MCRegister> &AvailableRegs,
    const RegLiveRangeTracker &RegTracker, const TargetRegisterInfo &TRI) {
  VRegMetrics Metrics = {0, 0, 0, 0, 0, 0};

  const unsigned LRIndex = LR.getIndex();

  // Compute basic metrics.
  for (const auto &Mask : Masks.getElements()) {
    if (Mask.any()) {
      const unsigned LanesInCycle = Mask.getNumLanes();
      Metrics.TotalLanes += LanesInCycle;
      Metrics.MaxWidth = std::max(Metrics.MaxWidth, LanesInCycle);
      Metrics.Duration++;
    }
  }

  // Compute pure and aliasing interference degrees.
  const TargetRegisterClass *RC = LR.getRegisterClass();
  const unsigned RCId = RC->getID();

  for (const auto &[OtherLRIndex, _] : LiveLanesByLRIndex) {
    if (OtherLRIndex == LRIndex)
      continue;
    if (!VRegInterferenceGraph.interferes(LRIndex, OtherLRIndex))
      continue;

    const RegLiveRange &OtherLR = RegTracker[OtherLRIndex];
    if (!OtherLR.getRegisterClass())
      continue;
    const unsigned OtherRCId = OtherLR.getRegisterClass()->getID();

    if (RCId == OtherRCId) {
      // Same register class - pure interference.
      Metrics.PureInterferenceDegree++;
    } else if (RCInterferenceGraph.interferes(RCId, OtherRCId)) {
      // Different but overlapping register classes - aliasing interference.
      const unsigned Weight =
          RCInterferenceGraph.getInterferenceWeight(RCId, OtherRCId);
      Metrics.AliasingInterferenceDegree += Weight;
    }
  }

  // Count available registers using per-LR AdmissibleRegs.
  const std::vector<Register> Candidates =
      getCandidatePhysRegs(LR.getAdmissibleRegs(), AvailableRegs);
  Metrics.NumAvailableRegs = Candidates.size();

  return Metrics;
}

// Try to allocate using a specific comparator for ordering.
// Each iteration of the main loop first drains all forced (choice-1)
// decisions, then places exactly one scored entry before restarting.
// The free (non-forced) pick is the unplaced VReg with the fewest effective
// candidates (Minimum Remaining Values); OrderFn is only used as a tie-break.
AIEPostRegAlloc::AllocResult AIEPostRegAlloc::tryAllocate(
    const DenseMap<unsigned, AIE::LivenessVector> &LiveLanesByLRIndex,
    const RegLiveRangeTracker *RegTracker, const TargetRegisterInfo &TRI,
    AllocState &State, ComparatorFunction OrderFn,
    DenseMap<Register, MCRegister> &OutAssign) {

  // Reset per-attempt state.
  std::fill(State.CycleOccupancy.begin(), State.CycleOccupancy.end(),
            PartSet());
  OutAssign.clear();

  Placement Work(LiveLanesByLRIndex, *RegTracker, State, OutAssign);

  // Place every range that has exactly one legal register, repeating until no
  // new forced choice appears. Returns false when a range has none.
  const auto DrainForcedChoices = [&]() -> bool {
    bool Progress = true;
    while (Progress) {
      Progress = false;
      for (const RegLiveRange *LR : Work.Ranges) {
        if (Work.Placed.test(LR->getIndex()))
          continue;
        const std::vector<Register> Cands = Work.candidates(*LR);
        if (Cands.empty()) {
          LLVM_DEBUG(dbgs() << "  Choice-0 on LR#" << LR->getIndex()
                            << " - infeasible\n");
          return false;
        }
        if (Cands.size() == 1) {
          LLVM_DEBUG(dbgs() << "  Forced: LR#" << LR->getIndex() << " -> "
                            << printReg(Cands[0], &TRI) << "\n");
          Work.place(*LR, Cands[0]);
          Progress = true;
        }
      }
    }
    return true;
  };

  // Unplaced virtual range with the fewest legal registers. OrderFn breaks
  // ties. nullopt when every virtual range is already placed.
  struct Choice {
    const RegLiveRange *LR = nullptr;
    std::vector<Register> Cands;
  };
  const auto SelectMinRemaining = [&]() -> std::optional<Choice> {
    std::optional<Choice> Best;
    for (const RegLiveRange *LR : Work.Ranges) {
      if (!LR->getVReg().isValid() || Work.Placed.test(LR->getIndex()))
        continue;
      std::vector<Register> Cands = Work.candidates(*LR);
      const unsigned Count = Cands.size();
      const VRegMetrics &Metrics =
          State.AllMetrics.find(LR->getIndex())->second;
      if (!Best || Count < Best->Cands.size() ||
          (Count == Best->Cands.size() &&
           OrderFn(Metrics,
                   State.AllMetrics.find(Best->LR->getIndex())->second)))
        Best = Choice{LR, std::move(Cands)};
    }
    return Best;
  };

  // Main allocation loop. Each iteration drains all forced choices first, then
  // places exactly one scored entry before restarting. A choice-0 before any
  // scored placement means the schedule is truly infeasible; after a scored
  // placement it may be a scoring artifact that another tie-break avoids.
  // NumScoredPlacements == 0 iff no scored placement has been made yet, which
  // is the condition for reporting a truly-infeasible schedule.
  unsigned NumScoredPlacements = 0;

  while (true) {
    if (!DrainForcedChoices())
      return AllocResult(/*InfeasibleSchedule=*/NumScoredPlacements == 0);

    const std::optional<Choice> Best = SelectMinRemaining();
    if (!Best)
      break; // All placed.

    const RegLiveRange &LR = *Best->LR;
    const std::vector<Register> &EffectiveCands = Best->Cands;
    const unsigned LRIndex = LR.getIndex();
    const TargetRegisterClass *RC = LR.getRegisterClass();
    const auto &Metrics = State.AllMetrics[LRIndex];

    LLVM_DEBUG(dbgs() << "Allocating LR#" << LRIndex
                      << " class=" << TRI.getRegClassName(RC)
                      << " (available=" << Metrics.NumAvailableRegs
                      << ", pure_int=" << Metrics.PureInterferenceDegree
                      << ", alias_int=" << Metrics.AliasingInterferenceDegree
                      << ")\n");

    if (EffectiveCands.empty()) {
      LLVM_DEBUG(dbgs() << "  Failed to find suitable physreg!\n");
      return AllocResult(/*InfeasibleSchedule=*/false);
    }

    Register ChosenPhys = EffectiveCands.front();
    LLVM_DEBUG(dbgs() << "  Trying " << printReg(ChosenPhys, &TRI) << "\n");
    Work.place(LR, ChosenPhys);
    ++NumScoredPlacements;
  }

  LLVM_DEBUG(dbgs() << "Allocation succeeded with " << OutAssign.size()
                    << " assignments\n");
  return AllocResult();
}

AIEPostRegAlloc::ExactResult AIEPostRegAlloc::tryAllocateExact(
    const DenseMap<unsigned, AIE::LivenessVector> &LiveLanesByLRIndex,
    const RegLiveRangeTracker &RegTracker, const TargetRegisterInfo &TRI,
    AllocState &State, DenseMap<Register, MCRegister> &OutAssign) {

  if (PostRegAllocExactBudget == 0)
    return ExactResult::Unknown;

  std::fill(State.CycleOccupancy.begin(), State.CycleOccupancy.end(),
            PartSet());
  OutAssign.clear();

  Placement Work(LiveLanesByLRIndex, RegTracker, State, OutAssign);

  // Pin non-virtual ranges that have exactly one legal register. They already
  // occupy a register the pool can hand out and are not part of the
  // permutation.
  for (const RegLiveRange *LR : Work.Ranges) {
    if (LR->getVReg().isValid() || Work.Placed.test(LR->getIndex()))
      continue;
    const std::vector<Register> Cands = Work.candidates(*LR);
    if (Cands.empty()) {
      LLVM_DEBUG(dbgs() << "Exact: infeasible, pinned range has no register\n");
      return ExactResult::Infeasible;
    }
    if (Cands.size() == 1) {
      LLVM_DEBUG(dbgs() << "  Pinned: LR#" << LR->getIndex() << " -> "
                        << printReg(Cands[0], &TRI) << "\n");
      Work.place(*LR, Cands[0]);
    }
  }

  SmallVector<const RegLiveRange *, 32> Order;
  for (const RegLiveRange *LR : Work.Ranges) {
    if (!LR->getVReg().isValid() || Work.Placed.test(LR->getIndex()))
      continue;
    Order.push_back(LR);
  }
  llvm::sort(Order, [](const RegLiveRange *A, const RegLiveRange *B) {
    return A->getIndex() < B->getIndex();
  });

  const SmallBitVector Pinned = Work.Placed;
  const SmallVector<PartSet, 8> SavedOccupancy = State.CycleOccupancy;
  const DenseMap<Register, MCRegister> SavedAssign = OutAssign;

  unsigned Nodes = 0;
  ExactResult Outcome = ExactResult::Infeasible;
  const auto ByIndex = [](const RegLiveRange *A, const RegLiveRange *B) {
    return A->getIndex() < B->getIndex();
  };

  do {
    State.CycleOccupancy = SavedOccupancy;
    Work.Placed = Pinned;
    OutAssign = SavedAssign;

    bool Failed = false;
    for (const RegLiveRange *LR : Order) {
      if (Nodes == PostRegAllocExactBudget) {
        Outcome = ExactResult::Unknown;
        Failed = true;
        break;
      }
      ++Nodes;

      const std::vector<Register> Cands = Work.candidates(*LR);
      if (Cands.empty()) {
        Failed = true;
        break;
      }
      Work.place(*LR, Cands.front());
    }

    if (Outcome == ExactResult::Unknown)
      break;
    if (!Failed) {
      Outcome = ExactResult::Feasible;
      break;
    }
  } while (std::next_permutation(Order.begin(), Order.end(), ByIndex));

  const char *Name = Outcome == ExactResult::Feasible     ? "feasible"
                     : Outcome == ExactResult::Infeasible ? "infeasible"
                                                          : "unknown";
  LLVM_DEBUG(dbgs() << "Exact: " << Name << " nodes=" << Nodes << "\n");
  if (Outcome != ExactResult::Feasible)
    OutAssign.clear();
  return Outcome;
}

// Dump virtual register metrics for debugging.
void AIEPostRegAlloc::dumpVRegMetrics(
    const DenseMap<unsigned, VRegMetrics> &AllMetrics,
    const RegLiveRangeTracker &RegTracker, const TargetRegisterInfo &TRI) {

  dbgs() << "=== Virtual Register Metrics Dump ===\n";
  dbgs() << "Total Virtual Registers: " << AllMetrics.size() << "\n\n";

  // Collect and sort LRIndices for consistent output.
  std::vector<std::pair<unsigned, VRegMetrics>> MetricsList;
  for (const auto &[LRIndex, Metrics] : AllMetrics)
    MetricsList.push_back({LRIndex, Metrics});
  llvm::sort(MetricsList,
             [](const auto &A, const auto &B) { return A.first < B.first; });

  // Print header.
  dbgs() << "LRIdx     RegClass                 Avail  Pure  Alias  "
            "TotalLanes  MaxWidth  Duration\n";
  dbgs() << "--------  -----------------------  -----  ----  -----  "
            "----------  --------  --------\n";

  // Print metrics for each LRIndex.
  for (const auto &[LRIndex, Metrics] : MetricsList) {
    const char *RCName =
        RegTracker[LRIndex].getRegisterClass()
            ? TRI.getRegClassName(RegTracker[LRIndex].getRegisterClass())
            : "unknown";
    dbgs() << format("LR#%-5u   %-23s  %5u  %4u  %5u  %10u  %8u  %8u\n",
                     LRIndex, RCName, Metrics.NumAvailableRegs,
                     Metrics.PureInterferenceDegree,
                     Metrics.AliasingInterferenceDegree, Metrics.TotalLanes,
                     Metrics.MaxWidth, Metrics.Duration);
  }

  // Print summary statistics.
  dbgs() << "\n=== Summary Statistics ===\n";

  unsigned TotalLanesSum = 0;
  unsigned MaxWidthMax = 0;
  unsigned MaxDuration = 0;
  unsigned MaxPureInterferenceDegree = 0;
  unsigned MaxAliasingInterferenceDegree = 0;
  double AvgPureInterferenceDegree = 0.0;
  double AvgAliasingInterferenceDegree = 0.0;

  for (const auto &[_, Metrics] : MetricsList) {
    TotalLanesSum += Metrics.TotalLanes;
    MaxWidthMax = std::max(MaxWidthMax, Metrics.MaxWidth);
    MaxDuration = std::max(MaxDuration, Metrics.Duration);
    MaxPureInterferenceDegree =
        std::max(MaxPureInterferenceDegree, Metrics.PureInterferenceDegree);
    MaxAliasingInterferenceDegree = std::max(
        MaxAliasingInterferenceDegree, Metrics.AliasingInterferenceDegree);
    AvgPureInterferenceDegree += Metrics.PureInterferenceDegree;
    AvgAliasingInterferenceDegree += Metrics.AliasingInterferenceDegree;
  }

  if (!MetricsList.empty()) {
    AvgPureInterferenceDegree /= MetricsList.size();
    AvgAliasingInterferenceDegree /= MetricsList.size();
  }

  dbgs() << "Total Lanes (sum):              " << TotalLanesSum << "\n";
  dbgs() << "Max Width (max):                " << MaxWidthMax << "\n";
  dbgs() << "Max Duration:                   " << MaxDuration << "\n";
  dbgs() << "Max Pure Interference Degree:   " << MaxPureInterferenceDegree
         << "\n";
  dbgs() << "Max Aliasing Interference Deg:  " << MaxAliasingInterferenceDegree
         << "\n";
  dbgs() << format("Avg Pure Interference Degree:   %.2f\n",
                   AvgPureInterferenceDegree);
  dbgs() << format("Avg Aliasing Interference Deg:  %.2f\n",
                   AvgAliasingInterferenceDegree);

  // Count register classes used.
  DenseMap<const TargetRegisterClass *, unsigned> RCCounts;
  for (const auto &[LRIndex, _] : MetricsList) {
    if (const TargetRegisterClass *RC = RegTracker[LRIndex].getRegisterClass())
      RCCounts[RC]++;
  }

  dbgs() << "\n=== Register Class Distribution ===\n";
  std::vector<std::pair<const TargetRegisterClass *, unsigned>> RCCountVec;
  for (const auto &[RC, Count] : RCCounts)
    RCCountVec.push_back({RC, Count});
  llvm::sort(RCCountVec,
             [](const auto &A, const auto &B) { return A.second > B.second; });

  for (const auto &[RC, Count] : RCCountVec)
    dbgs() << format("  %-25s: %u\n", TRI.getRegClassName(RC), Count);

  dbgs() << "\n=== End Virtual Register Metrics ===\n\n";
}

// Main allocation entry point.
PostRegAllocResult AIEPostRegAlloc::allocate(
    const DenseMap<unsigned, AIE::LivenessVector> &LiveLanesByLRIndex, int II,
    const RegLiveRangeTracker &RegTracker, const TargetRegisterInfo &TRI) {

  LLVM_DEBUG(dbgs() << "AIEPostRegAlloc::allocate for "
                    << LiveLanesByLRIndex.size() << " live ranges, II=" << II
                    << "\n");

  DenseMap<Register, MCRegister> Assignments;

  if (LiveLanesByLRIndex.empty()) {
    LLVM_DEBUG(dbgs() << "No live ranges to allocate\n");
    return PostRegAllocResult("Trivial", std::move(Assignments));
  }

  LLVM_DEBUG(dbgs() << "Available " << RegTracker.getAvailablePhysRegs().size()
                    << " physical registers\n");

  // Initialize allocation state with interference graphs computed once.
  AllocState State;
  State.init(&TRI, LiveLanesByLRIndex, &RegTracker);

  // Dump virtual register metrics when debug output is enabled.
  LLVM_DEBUG(dumpVRegMetrics(State.AllMetrics, RegTracker, TRI));

  // A non-zero budget selects the exact search and skips the heuristics.
  if (PostRegAllocExactBudget > 0) {
    LLVM_DEBUG(dbgs() << "Trying exact search\n");
    ExactResult ER = tryAllocateExact(LiveLanesByLRIndex, RegTracker, TRI,
                                      State, Assignments);
    if (ER == ExactResult::Feasible) {
      LLVM_DEBUG(dbgs() << "Allocation succeeded with exact search\n");
      return PostRegAllocResult("exact search", std::move(Assignments));
    }
    LLVM_DEBUG(dbgs() << "Exact search failed\n");
    return PostRegAllocResult();
  }

  // Define the allocation strategies to try.
  struct AllocationStrategy {
    const char *Name;
    ComparatorFunction OrderFn;
  };

  // Every strategy selects by Minimum Remaining Values. OrderFn is the
  // tie-break, tried in order until one allocation succeeds.
  std::vector<AllocationStrategy> Strategies = {
      // Fewer available registers first, then highest interference degree.
      {"scarce register class scoring", orderByScarceRegClass},
      // Lexicographic: widest register class first (absolute precedence),
      // then highest interference degree within the same width tier.
      {"width then interference scoring", orderByWidthThenInterference},
      // Pure interference degree (graph-coloring inspired).
      {"interference degree scoring", orderByInterference},
      // Area + width combined.
      {"area+width scoring", orderByAreaPlusWidth},
      // Pure area (total lane-cycles).
      {"area scoring", orderByArea},
      // Width only.
      {"width scoring", orderByWidth},
      // Duration only.
      {"duration scoring", orderByDuration},
      // Quadratic width + duration.
      {"quadratic width scoring",
       [](const VRegMetrics &A, const VRegMetrics &B) {
         return (A.MaxWidth * A.MaxWidth + A.Duration) >
                (B.MaxWidth * B.MaxWidth + B.Duration);
       }},
  };

  // Try each strategy in order.
  for (const auto &Strategy : Strategies) {
    LLVM_DEBUG(dbgs() << "Trying allocation with " << Strategy.Name << "\n");

    AllocResult Result = tryAllocate(LiveLanesByLRIndex, &RegTracker, TRI,
                                     State, Strategy.OrderFn, Assignments);

    if (Result) {
      LLVM_DEBUG(dbgs() << "Allocation succeeded with " << Strategy.Name
                        << "\n");
      return PostRegAllocResult(Strategy.Name, std::move(Assignments));
    }

    LLVM_DEBUG(dbgs() << Strategy.Name << " failed\n");

    // If the schedule is infeasible, no other scoring function will succeed.
    if (Result.isInfeasibleSchedule()) {
      LLVM_DEBUG(dbgs() << "Schedule is infeasible - skipping remaining "
                        << "allocation strategies\n");
      break;
    }
  }

  LLVM_DEBUG(dbgs() << "All allocation attempts failed\n");
  return PostRegAllocResult();
}
