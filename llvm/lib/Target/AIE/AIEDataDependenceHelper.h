//===- AIEDataDependenceHelper.h - Inter-block DAG --------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Class providing an inter-block dependence graph
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEDATADEPENDENCEHELPER_H
#define LLVM_LIB_TARGET_AIE_AIEDATADEPENDENCEHELPER_H

#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/ScheduleDAGInstrs.h"
#include <algorithm>
#include <map>
#include <optional>

namespace llvm {

class raw_ostream;

namespace AIE {

/// Class to derive actual helpers from. It is a placeholder for the future
/// stand-alone DDG class, it just implements the unrelated schedule() as
/// a dummy
/// It copies the Mutations mechanism from ScheduleDAGMI; this represents a
/// change of perspective on DAGMutations: They are target-dependent ways to
/// modify the dependence graph, not target-dependent ways to tweak the
/// scheduler.
class DataDependenceHelper : public ScheduleDAGInstrs {
  /// Ordered list of DAG postprocessing steps.
  std::vector<std::unique_ptr<ScheduleDAGMutation>> Mutations;
  const MachineSchedContext &Context;
  void schedule() override {};

protected:
  bool mayAlias(SUnit *SUa, SUnit *SUb, bool TBAA) override;

public:
  DataDependenceHelper(const MachineSchedContext &Context, bool AddMutators,
                       bool ExactLatencies);
  void buildEdges();

  // Compute the maximum depth of all nodes. The depth is the earliest cycle
  // in which an instruction can run when considering all latencies leading
  // up to it.
  int maxDepth() const;

  // Dump a graphviz representation of the graph to OS.
  // IncludeBoundaries controls whether edges to the artificial boundary node
  // are printed.
  void dumpDot(raw_ostream &OS, bool IncludeBoundaries) const;
};

/// This class generates all edges between nodes in two flow-adjacent regions.
/// The nodes are added in forward flow order, marking the boundary at the
/// appropriate point. Since the same MachineInstruction may be present in the
/// predecessor and the successor in case of a self-edge, we keep separate maps
/// of pre- and post- boundary nodes. They disambiguate the pre- and
/// post-boundary SUnits, identified by their NodeNum.
///
/// When SafeToIgnoreMemDeps is set, memory-alias edges that cross the
/// pre/post boundary are suppressed via a mayAlias() override.
///
/// The class also provides depth values (keyed by SUnit NodeNum):
///
///   PreDepths — bottom-up cycle of each pre-boundary node, negated: the
///               last predecessor cycle is -1.
///   PostDepths — top-down cycle of each post-boundary node.
///
/// Both sides share one cycle axis through the CFG boundary. PreDepth grows
/// upward through the predecessor and is stored negative, so the boundary
/// stays at 0. PostDepth grows downward through the successor. depth + latency
/// and latency - depth then work on either side. The last predecessor cycle
/// is -1. Successor cycle 0 is the first cycle after the boundary.
///
///   ^
///   | PreDepth   -3  -2  -1
/// ----- Boundary
///   | PostDepth   0   1   2
///   v
///
/// Each is a NodeValues object: a per-node map plus a region maximum.
/// Recording an instruction that is not in the matching pre/post index is
/// ignored for the per-node map but still updates the region maximum. The
/// no-instruction overload exists for empty bundles (NOPs). Negative values
/// are allowed.
///
/// In practice, SUnits and their dependences are invariant after first
/// construction. PostDepth is variant, and lazily re-evaluated by algorithms
/// working on the predecessor block. The depth values may change each time
/// the successor block is scheduled.
class InterBlockEdges : public DataDependenceHelper {
  /// MachineInstr to SUnit NodeNum. The same instruction may appear on both
  /// sides of the boundary, so predecessor and successor keep separate maps.
  using IndexMap = std::map<MachineInstr *, unsigned>;

public:
  /// Per-node integers for one side of the boundary, plus the maximum value
  /// recorded for that region. \p Nodes maps a MachineInstr to its SUnit
  /// NodeNum.
  class NodeValues {
    const IndexMap &Nodes;
    std::map<unsigned, int> Values;
    int RegionMax = 0;

  public:
    explicit NodeValues(const IndexMap &Nodes) : Nodes(Nodes) {}

    void clear();

    /// Record \p Val for \p MI. An instruction missing from Nodes is ignored
    /// for the per-node map, but the region maximum is still updated: the
    /// caller has found code at this cycle (a bundle interleaved with Fixed
    /// instructions, or a BottomFixed instruction outside the first
    /// iteration).
    void record(MachineInstr *MI, int Val);

    /// Record a cycle that is not represented by a MachineInstr, for instance
    /// an empty bundle.
    void record(int Val) { RegionMax = std::max(RegionMax, Val); }

    /// Recorded value of \p SU, or \p Default if none has been recorded (for
    /// example, the instruction is beyond the conflict horizon).
    int getValueOr(const SUnit *SU, int Default) const {
      return getValue(SU).value_or(Default);
    }

    /// Recorded value of \p SU, or nullopt if none has been recorded.
    std::optional<int> getValue(const SUnit *SU) const;

    int getRegionMax() const { return RegionMax; }
  };

private:
  // The predecessor block feeding instructions before the boundary.
  MachineBasicBlock *Pred = nullptr;
  // The successor block feeding instructions after the boundary.
  MachineBasicBlock *Succ = nullptr;
  // The boundary between Pred and Succ nodes. Boundary holds the index of
  // the first post-boundary node. This is equal to its NodeNum.
  std::optional<unsigned> Boundary;
  // When true, memory edges crossing the boundary are suppressed.
  bool SafeToIgnoreMemDeps = false;

  IndexMap PredMap;
  IndexMap SuccMap;

  /// Bottom-up cycle of pre-boundary SUnits, negated (last cycle is -1).
  NodeValues PreDepths{PredMap};
  /// Top-down cycle of post-boundary SUnits.
  NodeValues PostDepths{SuccMap};

  bool mayAlias(SUnit *SUa, SUnit *SUb, bool TBAA) override;

public:
  InterBlockEdges(const MachineSchedContext &Context,
                  bool SafeToIgnoreMemDeps = false,
                  MachineBasicBlock *Pred = nullptr,
                  MachineBasicBlock *Succ = nullptr)
      : DataDependenceHelper(Context, true, true), Pred(Pred), Succ(Succ),
        SafeToIgnoreMemDeps(SafeToIgnoreMemDeps) {}

  MachineBasicBlock *getPred() const { return Pred; }
  MachineBasicBlock *getSucc() const { return Succ; }

  /// Add a MI as a Node to the DAG.
  void addNode(MachineInstr *MI);

  /// Mark the boundary between the predecessor block and the successor block.
  /// Nodes added before are part of the predecessor, nodes added after are
  /// part of the successor.
  void markBoundary();

  /// To iterate forward across the SUnits of the underlying DDG.
  auto begin() { return SUnits.begin(); }
  auto begin() const { return SUnits.begin(); }
  auto end() { return SUnits.end(); }
  auto end() const { return SUnits.end(); }

  /// The following two methods are used to find the cross-boundary edges,
  /// by starting from a pre-boundary node and selecting its successor edges
  /// that connect to a post-boundary node.
  /// ---
  /// Retrieve the SUnit that represents MI's instance before the
  /// boundary, null if not found.
  const SUnit *getPreBoundaryNode(MachineInstr *MI) const;

  /// Retrieve the SUnit that represents MI's instance after the
  /// boundary, null if not found.
  const SUnit *getPostBoundaryNode(MachineInstr *MI) const;

  /// Check whether SU represents an instruction before the boundary.
  bool isPreBoundaryNode(const SUnit *SU) const;

  /// Check whether SU represents an instruction after the boundary.
  bool isPostBoundaryNode(const SUnit *SU) const;

  /// Pre-boundary height interface.
  /// Return the longest dependence chain that leads from each pre-boundary
  /// node to the end of the predecessor block, keyed by NodeNum. This is the
  /// mirror of PostDepths: a producer with height H has H cycles of its
  /// latency already covered by the predecessor before control reaches the
  /// successor.
  ///
  /// Chains are cut at the boundary, so a node whose only consumers are in
  /// the successor has no entry, which is equivalent to a height of zero.
  /// The values are static lower bounds derived from the DDG alone; they do
  /// not depend on the predecessor being scheduled.
  std::map<unsigned, int> computePreHeights() const;

  NodeValues &getPreDepths() { return PreDepths; }
  const NodeValues &getPreDepths() const { return PreDepths; }
  NodeValues &getPostDepths() { return PostDepths; }
  const NodeValues &getPostDepths() const { return PostDepths; }

  // Clear DAG and recorded depths.
  void clear();
};

} // namespace AIE
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEDATADEPENDENCEHELPER_H
