//===- AIEPostRegAlloc.h - Post-scheduling register allocator ------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines a post-scheduling register allocator for AIE targets.
// It performs modulo-aware register allocation for pipelined loops and can
// also be used for non-loop blocks. The allocator is transactional and does
// not spill - it returns false if allocation fails.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEPOSTREGALLOC_H
#define LLVM_LIB_TARGET_AIE_AIEPOSTREGALLOC_H

#include "AIELivenessVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/MC/LaneBitmask.h"
#include "llvm/MC/MCRegister.h"
#include <functional>
#include <vector>

namespace llvm {

class MachineFunction;
class MachineRegisterInfo;
class TargetRegisterInfo;
class TargetRegisterClass;
class RegLiveRangeTracker;
class RegLiveRange;

namespace AIE {

/// Post-scheduling register allocator for AIE targets.
///
/// This allocator performs modulo-aware register allocation using lane masks
/// to track sub-register liveness. It properly handles physical register
/// aliasing, ensuring that allocating a register blocks all its aliases
/// (sub-registers and super-registers). It is transactional (does not modify
/// MRI until a complete solution is found) and does not spill (returns false
/// if allocation fails).
class AIEPostRegAlloc {
private:
  /// Interference graph with configurable weight type and symmetry.
  /// @tparam WeightT Type of edge weights (bool for simple, unsigned for
  ///                 weighted).
  /// @tparam IsSymmetric Whether the graph is symmetric (undirected) or
  ///                     asymmetric (directed).
  template <typename WeightT = bool, bool IsSymmetric = true>
  class InterferenceGraph {
    // For symmetric graphs, store upper triangle; for asymmetric, store
    // full matrix. Key is (from, to) pair - order matters for asymmetric.
    DenseMap<std::pair<unsigned, unsigned>, WeightT> Edges;

  public:
    /// Add an interference edge with optional weight.
    /// For symmetric graphs, order doesn't matter.
    /// For asymmetric graphs, this is the weight from A to B.
    void addInterference(unsigned A, unsigned B, WeightT Weight = WeightT(1)) {
      if constexpr (IsSymmetric) {
        if (A > B)
          std::swap(A, B);
      }
      Edges[std::make_pair(A, B)] = Weight;
    }

    /// Check if A and B interfere.
    bool interferes(unsigned A, unsigned B) const {
      if (A == B)
        return true; // A node interferes with itself.
      if constexpr (IsSymmetric) {
        if (A > B)
          std::swap(A, B);
      }
      auto It = Edges.find(std::make_pair(A, B));
      if constexpr (std::is_same_v<WeightT, bool>) {
        return It != Edges.end() && It->second;
      } else {
        return It != Edges.end() && It->second > 0;
      }
    }

    /// Get the weight of interference from A to B.
    /// For asymmetric graphs, this is directional.
    WeightT getInterferenceWeight(unsigned A, unsigned B) const {
      if (A == B)
        return WeightT(0); // No weight for self-interference.
      if constexpr (IsSymmetric) {
        if (A > B)
          std::swap(A, B);
      }
      auto It = Edges.find(std::make_pair(A, B));
      return (It != Edges.end()) ? It->second : WeightT(0);
    }
  };

  // Type aliases for common use cases.
  using SimpleSymmetricGraph = InterferenceGraph<bool, true>;
  using WeightedSymmetricGraph = InterferenceGraph<unsigned, true>;
  using WeightedAsymmetricGraph = InterferenceGraph<unsigned, false>;

  /// Pre-computed metrics for a virtual register.
  struct VRegMetrics {
    // Sum of lanes across all cycles.
    unsigned TotalLanes;
    // Maximum lanes in any single cycle.
    unsigned MaxWidth;
    // Number of cycles where register is live.
    unsigned Duration;
    // Number of other VRegs in the SAME register class that interfere.
    unsigned PureInterferenceDegree;
    // Weighted interference from VRegs in aliasing register classes.
    unsigned AliasingInterferenceDegree;
    // Number of available registers in this register class.
    unsigned NumAvailableRegs;
  };

  /// Result of an allocation attempt.
  /// Default construction indicates success.
  /// Construction with bool parameter indicates failure (true = infeasible).
  class AllocResult {
    bool Success = true;
    bool InfeasibleSchedule = false;

  public:
    // Default constructor - indicates success.
    AllocResult() = default;

    // Constructor for failure cases.
    // InfeasibleSchedule=true means no scoring function can succeed.
    // InfeasibleSchedule=false means this scoring function failed but another
    // might work.
    explicit AllocResult(bool InfeasibleSchedule)
        : Success(false), InfeasibleSchedule(InfeasibleSchedule) {}

    // Check if the schedule is provably infeasible.
    bool isInfeasibleSchedule() const { return InfeasibleSchedule; }

    // Implicit conversion to bool - true if allocation succeeded.
    operator bool() const { return Success; }
  };

  /// Internal allocation state with RegUnit-based interference tracking.
  struct AllocState {
    /// RegUnit occupancy - tracks lane masks for each register unit.
    /// RegUnits are the fundamental units of register interference in LLVM.
    /// Two registers interfere if they share any RegUnits.
    DenseMap<unsigned /*RegUnit*/, AIE::LivenessVector> RegUnitOccupancy;

    /// Physical register occupancy - tracks lane masks for each allocated
    /// physical register (kept for compatibility with existing code).
    DenseMap<Register, AIE::LivenessVector> PhysOccupancy;

    /// Pre-computed interference graphs (reused across scoring attempts).
    WeightedAsymmetricGraph RCInterferenceGraph;
    WeightedSymmetricGraph VRegInterferenceGraph;

    /// Pre-computed metrics for all LiveRanges (reused across scoring
    /// attempts). Keyed by VReg since there is a 1:1 mapping.
    DenseMap<Register, VRegMetrics> AllMetrics;

    /// Target register info for RegUnit computation.
    const TargetRegisterInfo *TRI = nullptr;

    /// Initialize occupancy and compute interference graphs.
    /// The RegTracker provides the problem description (LiveRanges,
    /// AvailableRegs, AdmissibleRegs per LR). LiveLanesByVReg provides the
    /// temporal liveness data computed during scheduling.
    void init(const TargetRegisterInfo *TRI,
              const DenseMap<Register, AIE::LivenessVector> &LiveLanesByVReg,
              const RegLiveRangeTracker *RegTracker,
              const MachineRegisterInfo &MRI);

    /// Check if VReg can be placed in PhysReg without conflicts.
    /// This checks RegUnit conflicts to handle aliasing properly.
    bool canPlace(Register VReg, Register PhysReg,
                  const AIE::LivenessVector &VRegMasks,
                  const TargetRegisterClass *RC) const;

    /// Place VReg in PhysReg (updates RegUnit occupancy).
    void place(Register VReg, Register PhysReg,
               const AIE::LivenessVector &VRegMasks,
               const TargetRegisterClass *RC);
  };

  /// Scoring function type - takes pre-computed metrics and returns a score.
  using ScoringFunction = std::function<unsigned(const VRegMetrics &)>;

public:
  /// Allocate physical registers for virtual registers.
  ///
  /// \param LiveLanesByVReg Map from virtual register to per-cycle lane masks.
  /// \param II Initiation interval for pipelined loops (>= 1).
  ///        For non-pipelined blocks, use 0 or the schedule length.
  /// \param RegTracker RegLiveRangeTracker providing register information.
  /// \param MF Machine function being processed.
  /// \param TRI Target register info.
  /// \param MRI Machine register info (not modified).
  /// \param OutAssign Output map from virtual to physical registers.
  /// \return True if allocation succeeded, false if no solution found.
  static bool
  allocate(const DenseMap<Register, AIE::LivenessVector> &LiveLanesByVirtReg,
           int II, RegLiveRangeTracker &RegTracker, const MachineFunction &MF,
           const TargetRegisterInfo &TRI, const MachineRegisterInfo &MRI,
           DenseMap<Register /*VReg*/, MCRegister /*Phys*/> &OutAssign);

private:
  /// Try to allocate using a specific scoring function for ordering.
  /// Returns AllocResult which implicitly converts to bool (true = success).
  /// On success, OutAssign contains the virtual to physical register mapping.
  /// The RegTracker provides the problem description (LiveRanges,
  /// AvailableRegs, AdmissibleRegs per LR).
  static AllocResult
  tryAllocate(const DenseMap<Register, AIE::LivenessVector> &LiveLanesByVReg,
              const RegLiveRangeTracker *RegTracker,
              const TargetRegisterInfo &TRI, const MachineRegisterInfo &MRI,
              AllocState &State, ScoringFunction ScoreFn,
              DenseMap<Register, MCRegister> &OutAssign);

  /// Compute metrics for a live range.
  /// \param LR The live range to compute metrics for.
  /// \param Masks The lane masks for this live range.
  /// \param VRegInterferenceGraph Pre-computed virtual register interference
  ///                               graph.
  /// \param AllVRegs All virtual registers to compute degree against.
  /// \param RCInterferenceGraph Register class interference graph with
  ///                            weights.
  /// \param AvailableRegs Available physical registers.
  /// \param MRI Machine register info (for looking up other VRegs' RCs).
  /// \param TRI Target register info.
  static VRegMetrics
  computeMetrics(const RegLiveRange &LR, const AIE::LivenessVector &Masks,
                 const WeightedSymmetricGraph &VRegInterferenceGraph,
                 const DenseMap<Register, AIE::LivenessVector> &AllVRegs,
                 const WeightedAsymmetricGraph &RCInterferenceGraph,
                 const DenseSet<MCRegister> &AvailableRegs,
                 const MachineRegisterInfo &MRI, const TargetRegisterInfo &TRI);

  /// Build register class interference graph with asymmetric weights.
  static WeightedAsymmetricGraph
  buildRCInterferenceGraph(const DenseSet<unsigned> &UsedRCIds,
                           const TargetRegisterInfo &TRI);

  /// Build virtual register interference graph (symmetric).
  static WeightedSymmetricGraph buildVRegInterferenceGraph(
      const DenseMap<Register, AIE::LivenessVector> &LiveLanesByVirtReg,
      const MachineRegisterInfo &MRI,
      const WeightedAsymmetricGraph &RCInterferenceGraph);

  /// Predefined scoring functions.
  /// All return infinite score when pure degree >= available registers.
  static unsigned scoreByArea(const VRegMetrics &M) {
    if (M.PureInterferenceDegree >= M.NumAvailableRegs)
      return UINT_MAX;
    return M.TotalLanes;
  }
  static unsigned scoreByWidth(const VRegMetrics &M) {
    if (M.PureInterferenceDegree >= M.NumAvailableRegs)
      return UINT_MAX;
    return M.MaxWidth;
  }
  static unsigned scoreByDuration(const VRegMetrics &M) {
    if (M.PureInterferenceDegree >= M.NumAvailableRegs)
      return UINT_MAX;
    return M.Duration;
  }
  static unsigned scoreByAreaPlusWidth(const VRegMetrics &M) {
    if (M.PureInterferenceDegree >= M.NumAvailableRegs)
      return UINT_MAX;
    return M.TotalLanes * 10 + M.MaxWidth;
  }
  // Score by interference degree - considers both pure and aliasing.
  static unsigned scoreByInterference(const VRegMetrics &M) {
    if (M.PureInterferenceDegree >= M.NumAvailableRegs)
      return UINT_MAX;
    // Pure interference is critical, aliasing interference is secondary.
    return M.PureInterferenceDegree * 1000 + M.AliasingInterferenceDegree * 10 +
           M.TotalLanes;
  }
  // Score prioritizing scarce register classes (fewer available registers).
  // Register classes with fewer available registers get HIGHER scores,
  // so they are allocated FIRST, giving them first pick of registers.
  static unsigned scoreByScarceRegClass(const VRegMetrics &M) {
    if (M.PureInterferenceDegree >= M.NumAvailableRegs)
      return UINT_MAX;
    // Fewer available registers = higher scarceness bonus.
    // This ensures scarce register classes are allocated first.
    // Use a large multiplier to make this the dominant factor.
    unsigned ScarcenessBonus = (100 - M.NumAvailableRegs) * 10000;
    // Add interference as secondary factor.
    unsigned InterferenceScore = M.PureInterferenceDegree * 1000 +
                                 M.AliasingInterferenceDegree * 10 +
                                 M.TotalLanes;
    return ScarcenessBonus + InterferenceScore;
  }

  /// Get allocatable physical registers for a live range.
  /// Returns the intersection of AdmissibleRegs (semantic constraint from
  /// instruction encoding) and AvailableRegs (global availability).
  static std::vector<Register>
  getCandidatePhysRegs(const DenseSet<MCRegister> &AdmissibleRegs,
                       const DenseSet<MCRegister> &AvailableRegs);

  /// Dump virtual register metrics for debugging.
  static void dumpVRegMetrics(const DenseMap<Register, VRegMetrics> &AllMetrics,
                              const MachineRegisterInfo &MRI,
                              const TargetRegisterInfo &TRI);
};

} // namespace AIE
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEPOSTREGALLOC_H
