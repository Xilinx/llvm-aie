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
#include "AIEMaxNumRegUnits.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallBitVector.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/MC/MCRegister.h"
#include <functional>
#include <string>
#include <vector>

namespace llvm {

class TargetRegisterInfo;
class TargetRegisterClass;
class RegLiveRangeTracker;
class RegLiveRange;

namespace AIE {

/// Result of a post-scheduling register allocation attempt.
/// On failure the object evaluates to false and holds no useful data.
/// On success it holds the winning scoring strategy name and the
/// virtual-to-physical register assignment.
class PostRegAllocResult {
  bool Succeeded = false;
  std::string WinningStrategyName;
  DenseMap<Register, MCRegister> Assignments;

public:
  PostRegAllocResult() = default;
  PostRegAllocResult(std::string WinningStrategyName,
                     DenseMap<Register, MCRegister> Assignments)
      : Succeeded(true), WinningStrategyName(std::move(WinningStrategyName)),
        Assignments(std::move(Assignments)) {}

  explicit operator bool() const { return Succeeded; }

  const std::string &getWinningStrategyName() const {
    return WinningStrategyName;
  }
  const DenseMap<Register, MCRegister> &getAssignments() const {
    return Assignments;
  }
  DenseMap<Register, MCRegister> &getAssignments() { return Assignments; }
};

/// Post-scheduling register allocator for AIE targets.
///
/// This allocator performs modulo-aware register allocation. Occupancy is
/// tracked per modulo cycle as the set of register units in use. Placing a
/// physical register marks all of its units occupied at every cycle where the
/// live range is non-empty. Registers that
/// share a unit therefore alias. It is transactional (does not modify MRI
/// until a complete solution is found) and does not spill (returns false if
/// allocation fails).
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
    // Per-cycle RegUnit occupancy, sized to II: CycleOccupancy[c] has a bit
    // set for each RegUnit that is occupied at modulo cycle c.
    std::vector<PartSet> CycleOccupancy;

    // Cached RegUnit set per physical register.
    const DenseMap<MCRegister, PartSet> *PhysRegUnitSets = nullptr;

    /// Pre-computed interference graphs (reused across scoring attempts).
    WeightedAsymmetricGraph RCInterferenceGraph;
    WeightedSymmetricGraph VRegInterferenceGraph;

    /// Pre-computed metrics for all LiveRanges (reused across scoring
    /// attempts). Keyed by LRIndex (RegLiveRange::getIndex()).
    DenseMap<unsigned, VRegMetrics> AllMetrics;

    /// Target register info for RegUnit computation.
    const TargetRegisterInfo *TRI = nullptr;

    /// Initialize occupancy and compute interference graphs.
    /// The RegTracker provides the problem description (LiveRanges,
    /// AvailableRegs, AdmissibleRegs per LR). LiveLanesByLRIndex provides the
    /// temporal liveness data computed during scheduling.
    void init(const TargetRegisterInfo *TRI,
              const DenseMap<unsigned, AIE::LivenessVector> &LiveLanesByLRIndex,
              const RegLiveRangeTracker *RegTracker);

    /// Check if PhysReg can accommodate VRegMasks without conflicts.
    /// A conflict occurs when the candidate register's RegUnits are already
    /// occupied at any live cycle.
    bool canPlace(Register PhysReg, const AIE::LivenessVector &VRegMasks) const;

    /// Mark PhysReg's RegUnits occupied per cycle according to VRegMasks.
    void place(Register PhysReg, const AIE::LivenessVector &VRegMasks);

    /// Undo place(): clear the RegUnits place() occupied for VRegMasks.
    void unplace(Register PhysReg, const AIE::LivenessVector &VRegMasks);

    /// Pin non-virtualizable live ranges into CycleOccupancy. Returns false
    /// when two fixed ranges conflict. Virtualizable ranges left physical
    /// (RESERVED / reserved-overlap) are skipped: they constrain nothing.
    bool pinFixedRanges(
        const DenseMap<unsigned, AIE::LivenessVector> &LiveLanesByLRIndex,
        const RegLiveRangeTracker &RegTracker);
  };

  /// Comparator type for ordering live ranges during allocation.
  /// Returns true if A should be allocated before B (A has higher priority).
  /// This is the strict-weak-ordering predicate used directly in std::sort.
  using ComparatorFunction =
      std::function<bool(const VRegMetrics &, const VRegMetrics &)>;

  /// Live ranges under allocation and the occupancy they share. Used by both
  /// the heuristic and the exact search.
  struct Placement {
    const DenseMap<unsigned, AIE::LivenessVector> &LiveLanes;
    const DenseSet<MCRegister> &AvailableRegs;
    AllocState &State;
    DenseMap<Register, MCRegister> &OutAssign;
    SmallBitVector Placed;
    SmallVector<const RegLiveRange *, 32> Ranges;

    /// Per live range index: the admissible, available registers.
    std::vector<std::vector<Register>> AllCands;
    /// Per live range index: the positions in AllCands of the registers that
    /// the occupancy admits. Kept current for the unplaced ranges.
    std::vector<BitVector> Cands;
    /// Per live range index: the number of bits set in Cands.
    std::vector<unsigned> NumCands;
    /// Per live range index: the cycles with live lanes.
    std::vector<BitVector> LiveCycles;

    /// Collect the live ranges that take part in allocation. The occupancy
    /// of State must not change outside place() and unplace() while this
    /// object is used.
    Placement(const DenseMap<unsigned, AIE::LivenessVector> &Lanes,
              const RegLiveRangeTracker &Tracker, AllocState &InState,
              DenseMap<Register, MCRegister> &InAssign);

    /// Per-cycle lane masks of LR.
    const AIE::LivenessVector &masks(const RegLiveRange &LR) const;

    /// Number of admissible, available registers that LR can still occupy.
    unsigned numCandidates(const RegLiveRange &LR) const;

    /// The first admissible, available register that LR can still occupy, or
    /// an invalid register when there is none.
    Register firstCandidate(const RegLiveRange &LR) const;

    /// Assign PhysReg to LR and mark its units occupied.
    void place(const RegLiveRange &LR, Register PhysReg);

    /// Undo place() for LR.
    void unplace(const RegLiveRange &LR);

  private:
    /// Whether placing LR in PhysReg can change the candidates of Other.
    bool mayBlock(const RegLiveRange &LR, MCRegister PhysReg,
                  const RegLiveRange &Other) const;

    /// Recompute Cands of LR from AllCands.
    void refresh(const RegLiveRange &LR);
  };

public:
  /// Allocate physical registers for live ranges.
  ///
  /// \param LiveLanesByLRIndex Map from live range index to per-cycle lane
  ///        masks. Keys are RegLiveRange::getIndex() values.
  /// \param II Initiation interval for pipelined loops (>= 1).
  /// \param RegTracker RegLiveRangeTracker providing register information.
  /// \param TRI Target register info.
  /// \return PostRegAllocResult holding the assignment and winning strategy
  ///         name on success, or an empty (false) result on failure.
  static PostRegAllocResult
  allocate(const DenseMap<unsigned, AIE::LivenessVector> &LiveLanesByLRIndex,
           int II, const RegLiveRangeTracker &RegTracker,
           const TargetRegisterInfo &TRI);

private:
  /// Try to allocate using a specific comparator for ordering.
  /// Returns AllocResult which implicitly converts to bool (true = success).
  /// On success, OutAssign contains the virtual to physical register mapping.
  /// The RegTracker provides the problem description (LiveRanges,
  /// AvailableRegs, AdmissibleRegs per LR). The next live range is selected by
  /// Minimum Remaining Values (fewest effective physical-register candidates),
  /// with OrderFn as tie-break.
  static AllocResult
  tryAllocate(const DenseMap<unsigned, AIE::LivenessVector> &LiveLanesByLRIndex,
              const RegLiveRangeTracker *RegTracker,
              const TargetRegisterInfo &TRI, AllocState &State,
              ComparatorFunction OrderFn,
              DenseMap<Register, MCRegister> &OutAssign);

  /// Result of the budgeted exact permutation search.
  /// Feasible: a register assignment was found.
  /// Infeasible: every order was tried and none succeeded.
  /// Unknown: the node budget was exhausted. The assignment is
  /// kept only when the result is Feasible.
  enum class ExactResult { Feasible, Infeasible, Unknown };

  /// Budgeted exact allocator. Tries every order of the virtual live ranges,
  /// assigning the first legal register in each position. Backtracking keeps a
  /// placed prefix, and a prefix with no legal register is pruned. Stops when
  /// the node budget is spent. Used instead of the heuristics when
  /// -aie-postregalloc-exact-budget is non-zero.
  static ExactResult tryAllocateExact(
      const DenseMap<unsigned, AIE::LivenessVector> &LiveLanesByLRIndex,
      const RegLiveRangeTracker &RegTracker, const TargetRegisterInfo &TRI,
      AllocState &State, DenseMap<Register, MCRegister> &OutAssign);

  /// Compute metrics for a live range.
  /// \param LR The live range to compute metrics for.
  /// \param Masks The lane masks for this live range.
  /// \param VRegInterferenceGraph Pre-computed live range interference graph.
  /// \param LiveLanesByLRIndex All live ranges' liveness data.
  /// \param RCInterferenceGraph Register class interference graph with
  ///                            weights.
  /// \param AvailableRegs Available physical registers.
  /// \param RegTracker Tracker for looking up live range register classes.
  /// \param TRI Target register info.
  static VRegMetrics computeMetrics(
      const RegLiveRange &LR, const AIE::LivenessVector &Masks,
      const WeightedSymmetricGraph &VRegInterferenceGraph,
      const DenseMap<unsigned, AIE::LivenessVector> &LiveLanesByLRIndex,
      const WeightedAsymmetricGraph &RCInterferenceGraph,
      const DenseSet<MCRegister> &AvailableRegs,
      const RegLiveRangeTracker &RegTracker, const TargetRegisterInfo &TRI);

  /// Build register class interference graph with asymmetric weights.
  static WeightedAsymmetricGraph
  buildRCInterferenceGraph(const DenseSet<unsigned> &UsedRCIds,
                           const TargetRegisterInfo &TRI);

  /// Build live range interference graph (symmetric). Two live ranges
  /// interfere when their admissible RegUnit sets overlap and they are live
  /// at a common cycle.
  /// \param LiveLanesByLRIndex Per-cycle lane masks keyed by live range index.
  /// \param LiveRanges All live ranges of the tracker, indexed by
  ///        RegLiveRange::getIndex().
  static WeightedSymmetricGraph buildVRegInterferenceGraph(
      const DenseMap<unsigned, AIE::LivenessVector> &LiveLanesByLRIndex,
      ArrayRef<RegLiveRange> LiveRanges);

  /// Predefined ordering comparators.
  /// Each returns true if A should be allocated before B.
  static bool orderByArea(const VRegMetrics &A, const VRegMetrics &B) {
    return A.TotalLanes > B.TotalLanes;
  }
  static bool orderByWidth(const VRegMetrics &A, const VRegMetrics &B) {
    return A.MaxWidth > B.MaxWidth;
  }
  static bool orderByDuration(const VRegMetrics &A, const VRegMetrics &B) {
    return A.Duration > B.Duration;
  }
  // Area, then width.
  static bool orderByAreaPlusWidth(const VRegMetrics &A, const VRegMetrics &B) {
    if (A.TotalLanes != B.TotalLanes)
      return A.TotalLanes > B.TotalLanes;
    return A.MaxWidth > B.MaxWidth;
  }
  // Most-interfering range first: pure degree, then aliasing degree, then area.
  static bool orderByInterference(const VRegMetrics &A, const VRegMetrics &B) {
    if (A.PureInterferenceDegree != B.PureInterferenceDegree)
      return A.PureInterferenceDegree > B.PureInterferenceDegree;
    if (A.AliasingInterferenceDegree != B.AliasingInterferenceDegree)
      return A.AliasingInterferenceDegree > B.AliasingInterferenceDegree;
    return A.TotalLanes > B.TotalLanes;
  }
  // Fewer available registers first. Within the same scarceness, the
  // most-interfering range goes first.
  static bool orderByScarceRegClass(const VRegMetrics &A,
                                    const VRegMetrics &B) {
    if (A.NumAvailableRegs != B.NumAvailableRegs)
      return A.NumAvailableRegs < B.NumAvailableRegs;
    return orderByInterference(A, B);
  }
  // Widest register class first (e.g. eEYs, MaxWidth=12, before MaxWidth=6),
  // then pure interference degree, then aliasing interference degree.
  // Within a width tier the most interfering range goes first, mirroring the
  // graph-coloring heuristic of colouring high-degree nodes first.
  static bool orderByWidthThenInterference(const VRegMetrics &A,
                                           const VRegMetrics &B) {
    if (A.MaxWidth != B.MaxWidth)
      return A.MaxWidth > B.MaxWidth;
    if (A.PureInterferenceDegree != B.PureInterferenceDegree)
      return A.PureInterferenceDegree > B.PureInterferenceDegree;
    return A.AliasingInterferenceDegree > B.AliasingInterferenceDegree;
  }

  /// Dump live range metrics for debugging.
  static void dumpVRegMetrics(const DenseMap<unsigned, VRegMetrics> &AllMetrics,
                              const RegLiveRangeTracker &RegTracker,
                              const TargetRegisterInfo &TRI);
};

} // namespace AIE
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEPOSTREGALLOC_H
