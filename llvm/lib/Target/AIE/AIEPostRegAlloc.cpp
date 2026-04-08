//===- AIEPostRegAlloc.cpp - Post-scheduling register allocator ----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements a post-scheduling register allocator for AIE targets.
//
//===----------------------------------------------------------------------===//

#include "AIEPostRegAlloc.h"
#include "AIELivenessVector.h"
#include "AIERegDefUseTracker.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <vector>

#define DEBUG_TYPE "aie-postregalloc"

using namespace llvm;
using namespace llvm::AIE;

// Initialize allocation state and compute interference graphs.
void AIEPostRegAlloc::AllocState::init(
    const TargetRegisterInfo *InTRI,
    const DenseMap<Register, AIE::LivenessVector> &LiveLanesByVReg,
    const RegLiveRangeTracker *RegTracker, const MachineRegisterInfo &MRI) {
  this->RegUnitOccupancy.clear();
  this->PhysOccupancy.clear();
  this->TRI = InTRI;

  const auto &AvailableRegs = RegTracker->getAvailablePhysRegs();

  // Build register class interference graph once.
  // Iterate over LiveRanges to get register class IDs.
  DenseSet<unsigned> UsedRCIds;
  for (const RegLiveRange &LR : RegTracker->getLiveRanges()) {
    if (const TargetRegisterClass *RC = LR.getRegisterClass())
      UsedRCIds.insert(RC->getID());
  }
  this->RCInterferenceGraph =
      AIEPostRegAlloc::buildRCInterferenceGraph(UsedRCIds, *InTRI);

  // Build virtual register interference graph once.
  this->VRegInterferenceGraph = AIEPostRegAlloc::buildVRegInterferenceGraph(
      LiveLanesByVReg, MRI, RCInterferenceGraph);

  // Pre-compute metrics for all LiveRanges.
  this->AllMetrics.clear();
  for (const RegLiveRange &LR : RegTracker->getLiveRanges()) {
    const Register VReg = LR.getVReg();
    auto It = LiveLanesByVReg.find(VReg);
    if (It == LiveLanesByVReg.end())
      continue;
    const AIE::LivenessVector &Masks = It->second;
    AllMetrics[VReg] = AIEPostRegAlloc::computeMetrics(
        LR, Masks, VRegInterferenceGraph, LiveLanesByVReg, RCInterferenceGraph,
        AvailableRegs, MRI, *InTRI);
  }
}

// Check if VReg can be placed in PhysReg without conflicts.
bool AIEPostRegAlloc::AllocState::canPlace(
    Register VReg, Register PhysReg, const AIE::LivenessVector &VRegMasks,
    const TargetRegisterClass *RC) const {

  // Check RegUnit conflicts - this handles aliasing automatically.
  // Two registers interfere if they share any RegUnits.
  for (MCRegUnitIterator Units(PhysReg.asMCReg(), TRI); Units.isValid();
       ++Units) {
    unsigned Unit = *Units;
    auto It = RegUnitOccupancy.find(Unit);
    if (It != RegUnitOccupancy.end()) {
      // This RegUnit is occupied. Check if it conflicts with our VRegMasks.
      const auto &UnitOcc = It->second;
      if (VRegMasks.overlaps(UnitOcc)) {
        LLVM_DEBUG(dbgs() << "  RegUnit conflict detected for "
                          << printReg(VReg, TRI) << " in "
                          << printReg(PhysReg, TRI) << " (unit " << Unit
                          << ")\n");
        return false;
      }
    }
  }

  return true;
}

// Place VReg in PhysReg (updates occupancy).
void AIEPostRegAlloc::AllocState::place(Register VReg, Register PhysReg,
                                        const AIE::LivenessVector &VRegMasks,
                                        const TargetRegisterClass *RC) {

  // Update lane mask occupancy for the specific register (for compatibility).
  PhysOccupancy[PhysReg] |= VRegMasks;

  // Update RegUnit occupancy - this automatically handles aliasing.
  unsigned NumUnits = 0;
  for (MCRegUnitIterator Units(PhysReg.asMCReg(), TRI); Units.isValid();
       ++Units) {
    RegUnitOccupancy[*Units] |= VRegMasks;
    NumUnits++;
  }

  LLVM_DEBUG(dbgs() << "  Placed " << printReg(VReg, TRI) << " in "
                    << printReg(PhysReg, TRI) << " (updated " << NumUnits
                    << " RegUnits)\n");
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

// Build virtual register interference graph (symmetric).
AIEPostRegAlloc::WeightedSymmetricGraph
AIEPostRegAlloc::buildVRegInterferenceGraph(
    const DenseMap<Register, AIE::LivenessVector> &LiveLanesByVReg,
    const MachineRegisterInfo &MRI,
    const WeightedAsymmetricGraph &RCInterferenceGraph) {

  WeightedSymmetricGraph Graph;

  // Build a vector of VRegs for iteration (to ensure consistent ordering).
  std::vector<Register> VRegs;
  for (const auto &[VReg, _] : LiveLanesByVReg) {
    VRegs.push_back(VReg);
  }

  // Check all pairs of virtual registers.
  // Use symmetry: only check pairs where I < J.
  for (size_t I = 0; I < VRegs.size(); ++I) {
    const Register VReg1 = VRegs[I];
    const auto &Masks1 = LiveLanesByVReg.find(VReg1)->second;
    const unsigned RCId1 = MRI.getRegClass(VReg1)->getID();

    for (size_t J = I + 1; J < VRegs.size(); ++J) {
      const Register VReg2 = VRegs[J];
      const auto &Masks2 = LiveLanesByVReg.find(VReg2)->second;
      const unsigned RCId2 = MRI.getRegClass(VReg2)->getID();

      // First check if their register classes can interfere.
      if (!RCInterferenceGraph.interferes(RCId1, RCId2))
        continue;

      // Then check if their live ranges overlap temporally.
      if (Masks1.overlaps(Masks2)) {
        Graph.addInterference(VReg1, VReg2);
      }
    }
  }

  return Graph;
}

// Compute metrics for a live range.
AIEPostRegAlloc::VRegMetrics AIEPostRegAlloc::computeMetrics(
    const RegLiveRange &LR, const AIE::LivenessVector &Masks,
    const WeightedSymmetricGraph &VRegInterferenceGraph,
    const DenseMap<Register, AIE::LivenessVector> &AllVRegs,
    const WeightedAsymmetricGraph &RCInterferenceGraph,
    const DenseSet<MCRegister> &AvailableRegs, const MachineRegisterInfo &MRI,
    const TargetRegisterInfo &TRI) {
  VRegMetrics Metrics = {0, 0, 0, 0, 0, 0};

  const Register VReg = LR.getVReg();

  // Compute basic metrics.
  for (const auto &Mask : Masks.getElements()) {
    if (Mask.any()) {
      unsigned LanesInCycle = Mask.getNumLanes();
      Metrics.TotalLanes += LanesInCycle;
      Metrics.MaxWidth = std::max(Metrics.MaxWidth, LanesInCycle);
      Metrics.Duration++;
    }
  }

  // Compute pure and aliasing interference degrees.
  // Use the register class from the LiveRange.
  const TargetRegisterClass *RC = LR.getRegisterClass();
  unsigned RCId = RC->getID();

  for (const auto &[OtherVReg, _] : AllVRegs) {
    if (OtherVReg != VReg &&
        VRegInterferenceGraph.interferes(VReg, OtherVReg)) {
      // For interference with other VRegs, we still need MRI to look up
      // their register class. A future optimization could pass a map
      // from VReg to LiveRange to avoid this MRI dependency.
      const TargetRegisterClass *OtherRC = MRI.getRegClass(OtherVReg);
      const unsigned OtherRCId = OtherRC->getID();

      if (RCId == OtherRCId) {
        // Same register class - pure interference.
        Metrics.PureInterferenceDegree++;
      } else if (RCInterferenceGraph.interferes(RCId, OtherRCId)) {
        // Different but overlapping register classes - aliasing interference.
        // Use asymmetric weight: how much does OtherVReg's class affect
        // VReg's class?
        unsigned Weight =
            RCInterferenceGraph.getInterferenceWeight(RCId, OtherRCId);
        Metrics.AliasingInterferenceDegree += Weight;
      }
    }
  }

  // Count available registers using per-LR AdmissibleRegs.
  std::vector<Register> Candidates =
      getCandidatePhysRegs(LR.getAdmissibleRegs(), AvailableRegs);
  Metrics.NumAvailableRegs = Candidates.size();

  return Metrics;
}

// Get allocatable physical registers for a live range.
// Returns the intersection of AdmissibleRegs (semantic constraint from
// instruction encoding) and AvailableRegs (global availability).
std::vector<Register> AIEPostRegAlloc::getCandidatePhysRegs(
    const DenseSet<MCRegister> &AdmissibleRegs,
    const DenseSet<MCRegister> &AvailableRegs) {

  std::vector<Register> Candidates;

  // Return the intersection of admissible and available registers.
  // AdmissibleRegs represents the semantic constraint from the LiveRange.
  // AvailableRegs represents the global set of registers available for
  // reallocation.
  for (MCRegister PhysReg : AdmissibleRegs) {
    if (AvailableRegs.count(PhysReg)) {
      Candidates.push_back(PhysReg);
    }
  }

  return Candidates;
}

// Try to allocate using a specific scoring function for ordering.
AIEPostRegAlloc::AllocResult AIEPostRegAlloc::tryAllocate(
    const DenseMap<Register, AIE::LivenessVector> &LiveLanesByVReg,
    const RegLiveRangeTracker *RegTracker, const TargetRegisterInfo &TRI,
    const MachineRegisterInfo &MRI, AllocState &State, ScoringFunction ScoreFn,
    DenseMap<Register, MCRegister> &OutAssign) {

  // Clear per-attempt state.
  State.RegUnitOccupancy.clear();
  State.PhysOccupancy.clear();
  OutAssign.clear();

  const auto &AvailableRegs = RegTracker->getAvailablePhysRegs();

  // Build sorted list of LiveRanges by difficulty.
  struct LRInfo {
    const RegLiveRange *LR;
    Register VReg;
    unsigned Score;
    const AIE::LivenessVector *Masks;
  };

  // Score and collect LiveRanges using pre-computed metrics from State.
  std::vector<LRInfo> LRInfos;
  for (const RegLiveRange &LR : RegTracker->getLiveRanges()) {
    const Register VReg = LR.getVReg();
    auto It = LiveLanesByVReg.find(VReg);
    if (It == LiveLanesByVReg.end())
      continue;

    LRInfo Info;
    Info.LR = &LR;
    Info.VReg = VReg;
    Info.Score = ScoreFn(State.AllMetrics[VReg]);
    Info.Masks = &It->second;
    LRInfos.push_back(Info);
  }

  // Sort by descending score (hardest first).
  // Use VReg index as tiebreaker for deterministic ordering when scores are
  // equal.
  llvm::sort(LRInfos, [](const LRInfo &A, const LRInfo &B) {
    if (A.Score != B.Score)
      return A.Score > B.Score;
    return A.VReg.virtRegIndex() < B.VReg.virtRegIndex();
  });

  // Try to allocate each LiveRange.
  for (const auto &Info : LRInfos) {
    const RegLiveRange &LR = *Info.LR;
    const Register VReg = Info.VReg;
    const auto &VRegMasks = *Info.Masks;
    const TargetRegisterClass *RC = LR.getRegisterClass();
    const auto &Metrics = State.AllMetrics[VReg];

    LLVM_DEBUG(dbgs() << "Allocating " << printReg(VReg, &TRI) << " class="
                      << TRI.getRegClassName(RC) << " (score=" << Info.Score
                      << ", available=" << Metrics.NumAvailableRegs
                      << ", pure_int=" << Metrics.PureInterferenceDegree
                      << ", alias_int=" << Metrics.AliasingInterferenceDegree
                      << ")\n");

    // Check for infeasible schedule: pure interference >= available registers.
    // This is a global failure - no scoring function can fix this.
    if (Metrics.PureInterferenceDegree >= Metrics.NumAvailableRegs) {
      LLVM_DEBUG(dbgs() << "  Infeasible schedule detected: pure interference ("
                        << Metrics.PureInterferenceDegree
                        << ") >= available registers ("
                        << Metrics.NumAvailableRegs << ")\n");
      return AllocResult(/*InfeasibleSchedule=*/true);
    }

    // Get candidate physical registers using AdmissibleRegs from LiveRange.
    std::vector<Register> Candidates =
        getCandidatePhysRegs(LR.getAdmissibleRegs(), AvailableRegs);

    if (Candidates.empty()) {
      LLVM_DEBUG(dbgs() << "  No candidates available!\n");
      return AllocResult(/*InfeasibleSchedule=*/false);
    }

    // Try to find a suitable physical register (first-fit).
    Register ChosenPhys = Register();

    for (Register PhysReg : Candidates) {
      LLVM_DEBUG(dbgs() << "  Trying " << printReg(PhysReg, &TRI) << "\n");
      if (State.canPlace(VReg, PhysReg, VRegMasks, RC)) {
        ChosenPhys = PhysReg;
        break;
      }
    }

    if (!ChosenPhys.isValid()) {
      LLVM_DEBUG(dbgs() << "  Failed to find suitable physreg!\n");
      return AllocResult(/*InfeasibleSchedule=*/false);
    }

    // Place the VReg and record in output.
    State.place(VReg, ChosenPhys, VRegMasks, RC);
    OutAssign[VReg] = ChosenPhys.asMCReg();
  }

  LLVM_DEBUG(dbgs() << "Allocation succeeded with " << OutAssign.size()
                    << " assignments\n");
  return AllocResult();
}

// Dump virtual register metrics for debugging.
void AIEPostRegAlloc::dumpVRegMetrics(
    const DenseMap<Register, VRegMetrics> &AllMetrics,
    const MachineRegisterInfo &MRI, const TargetRegisterInfo &TRI) {

  dbgs() << "=== Virtual Register Metrics Dump ===\n";
  dbgs() << "Total Virtual Registers: " << AllMetrics.size() << "\n\n";

  // Collect and sort VRegs for consistent output.
  std::vector<std::pair<Register, VRegMetrics>> VRegMetricsList;
  for (const auto &[VReg, Metrics] : AllMetrics) {
    VRegMetricsList.push_back({VReg, Metrics});
  }

  // Sort by VReg number for consistent output.
  llvm::sort(VRegMetricsList, [](const auto &A, const auto &B) {
    return A.first.virtRegIndex() < B.first.virtRegIndex();
  });

  // Print header.
  dbgs() << "VReg      RegClass                 Avail  Pure  Alias  "
            "TotalLanes  MaxWidth  Duration\n";
  dbgs() << "--------  -----------------------  -----  ----  -----  "
            "----------  --------  --------\n";

  // Print metrics for each VReg.
  for (const auto &[VReg, Metrics] : VRegMetricsList) {
    const TargetRegisterClass *RC = MRI.getRegClass(VReg);
    const char *Status =
        (Metrics.PureInterferenceDegree >= Metrics.NumAvailableRegs) ? " FAIL"
                                                                     : "";
    dbgs() << format("%%vreg%-4u  %-23s  %5u  %4u  %5u  %10u  %8u  %8u%s\n",
                     VReg.virtRegIndex(), TRI.getRegClassName(RC),
                     Metrics.NumAvailableRegs, Metrics.PureInterferenceDegree,
                     Metrics.AliasingInterferenceDegree, Metrics.TotalLanes,
                     Metrics.MaxWidth, Metrics.Duration, Status);
  }

  // Print summary statistics.
  dbgs() << "\n=== Summary Statistics ===\n";

  // Compute aggregate statistics.
  unsigned TotalLanesSum = 0;
  unsigned MaxWidthMax = 0;
  unsigned MaxDuration = 0;
  unsigned MaxPureInterferenceDegree = 0;
  unsigned MaxAliasingInterferenceDegree = 0;
  double AvgPureInterferenceDegree = 0.0;
  double AvgAliasingInterferenceDegree = 0.0;

  for (const auto &[_, Metrics] : VRegMetricsList) {
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

  if (!VRegMetricsList.empty()) {
    AvgPureInterferenceDegree /= VRegMetricsList.size();
    AvgAliasingInterferenceDegree /= VRegMetricsList.size();
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
  for (const auto &[VReg, _] : VRegMetricsList) {
    RCCounts[MRI.getRegClass(VReg)]++;
  }

  dbgs() << "\n=== Register Class Distribution ===\n";
  std::vector<std::pair<const TargetRegisterClass *, unsigned>> RCCountVec;
  for (const auto &[RC, Count] : RCCounts) {
    RCCountVec.push_back({RC, Count});
  }
  llvm::sort(RCCountVec, [](const auto &A, const auto &B) {
    // Sort by count descending.
    return A.second > B.second;
  });

  for (const auto &[RC, Count] : RCCountVec) {
    dbgs() << format("  %-25s: %u\n", TRI.getRegClassName(RC), Count);
  }

  dbgs() << "\n=== End Virtual Register Metrics ===\n\n";
}

// Main allocation entry point.
bool AIEPostRegAlloc::allocate(
    const DenseMap<Register, AIE::LivenessVector> &LiveLanesByVReg, int II,
    RegLiveRangeTracker &RegTracker, const MachineFunction &MF,
    const TargetRegisterInfo &TRI, const MachineRegisterInfo &MRI,
    DenseMap<Register, MCRegister> &OutAssign) {

  LLVM_DEBUG(dbgs() << "AIEPostRegAlloc::allocate for "
                    << LiveLanesByVReg.size() << " vregs, II=" << II << "\n");

  if (LiveLanesByVReg.empty()) {
    LLVM_DEBUG(dbgs() << "No vregs to allocate\n");
    return true;
  }

  LLVM_DEBUG(dbgs() << "Available " << RegTracker.getAvailablePhysRegs().size()
                    << " physical registers\n");

  // Initialize allocation state with interference graphs computed once.
  AllocState State;
  State.init(&TRI, LiveLanesByVReg, &RegTracker, MRI);

  // Dump virtual register metrics when debug output is enabled.
  LLVM_DEBUG(dumpVRegMetrics(State.AllMetrics, MRI, TRI));

  // Define the allocation strategies to try.
  struct AllocationStrategy {
    const char *Name;
    ScoringFunction ScoreFn;
  };

  std::vector<AllocationStrategy> Strategies = {
      // Try scarce register class priority scoring first.
      {"scarce register class scoring", scoreByScarceRegClass},
      // Try interference-based scoring (graph coloring inspired).
      {"interference degree scoring", scoreByInterference},
      // Try with area+width scoring (original).
      {"area+width scoring", scoreByAreaPlusWidth},
      // Try with pure area scoring.
      {"area scoring", scoreByArea},
      // Try with width-priority scoring.
      {"width scoring", scoreByWidth},
      // Try with duration scoring.
      {"duration scoring", scoreByDuration},
      // Try a custom non-linear scoring function.
      {"quadratic width scoring",
       [](const VRegMetrics &M) {
         if (M.PureInterferenceDegree >= M.NumAvailableRegs)
           return UINT_MAX;
         // Quadratic penalty for width, linear for duration.
         return M.MaxWidth * M.MaxWidth + M.Duration;
       }},
  };

  // Try each strategy in order.
  for (const auto &Strategy : Strategies) {
    LLVM_DEBUG(dbgs() << "Trying allocation with " << Strategy.Name << "\n");

    AllocResult Result = tryAllocate(LiveLanesByVReg, &RegTracker, TRI, MRI,
                                     State, Strategy.ScoreFn, OutAssign);

    if (Result) {
      LLVM_DEBUG(dbgs() << "Allocation succeeded with " << Strategy.Name
                        << "\n");
      return true;
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
  return false;
}
