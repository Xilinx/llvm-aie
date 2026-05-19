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
/// The class also provides a depth map (keyed by SUnit NodeNum) that represents
/// the depth of an instruction after the boundary.
///
///   PostDepths — top-down cycle of each post-boundary node.
///
///   PostRegionMaxDepth — The maximum of the recorded depths.
///
/// In practice, SUnits and their dependences are invariant after first
/// construction. PostDepth is variant, and lazily re-evaluated by algorithms
/// working on the predecessor block. The depth values may change each time
/// the successor block is scheduled.
class InterBlockEdges : public DataDependenceHelper {
  // The predecessor block feeding instructions before the boundary.
  MachineBasicBlock *Pred = nullptr;
  // The successor block feeding instructions after the boundary.
  MachineBasicBlock *Succ = nullptr;
  // The boundary between Pred and Succ nodes. 'Boundary holds the index of
  // the first post-boundary node. This is equal to its NodeNum.
  std::optional<unsigned> Boundary;
  // When true, memory edges crossing the boundary are suppressed.
  bool SafeToIgnoreMemDeps = false;

  /// We can add the same instruction on both sides of the boundary.
  /// We maintain explicit maps to retrieve the corresponding SUnit.
  using IndexMap = std::map<MachineInstr *, unsigned>;
  IndexMap PredMap;
  IndexMap SuccMap;

  /// Depth (top-down cycle) of post-boundary SUnits, keyed by NodeNum.
  std::map<unsigned, int> PostDepths;

  /// Maximum depth of any PostBoundary node
  int PostRegionMaxDepth = 0;

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
  /// In normal operation, there should just be one call to this method.
  /// Nodes added before are part of the predecessor, nodes added after are
  /// part of the successor.
  void markBoundary();

  /// To iterate forward across the SUnits of the underlying DDG.
  auto begin() const { return SUnits.begin(); }
  auto end() const { return SUnits.end(); }

  /// The following two methods are used to find the cross-boundary edges,
  /// by starting from a pre-boundary node and selecting its successor edges
  /// that connect to a post-boundary node.
  /// ---
  /// Retrieve the SUnit that represents MI's instance before the
  /// boundary, null if not found.
  const SUnit *getPreBoundaryNode(MachineInstr *MI) const;

  /// Check whether SU represents an instruction after the boundary.
  bool isPostBoundaryNode(SUnit *SU) const;

  /// Post-boundary depth interface.
  /// Record the top-down cycle of a post-boundary instruction. If MI is not
  /// in our post-boundary nodes, it is silently ignored. This facilitates
  /// recording scheduled bundles, which may be interleaved with Fixed
  /// instructions.
  /// The variant without MI just records the maximum.
  void recordPostDepth(MachineInstr *MI, int Depth);
  void recordPostDepth(int Depth);
  /// Get the recorded top-down cycle of a post-boundary SUnit, or \p Default
  /// if no depth has been recorded (e.g. the instruction is beyond the
  /// conflict horizon).
  int getPostDepthOr(const SUnit *SU, int Default) const;

  /// Clear all recorded post-boundary depths.  Call before repopulating.
  void clearPostDepths();

  // Post-boundary maximum depth. This is one less than the 'depth'
  // of the next region.
  int getPostRegionMaxDepth() const { return PostRegionMaxDepth; }

  // Clear DAG and local extensions like PostDepths
  void clear();
};

} // namespace AIE
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEDATADEPENDENCEHELPER_H
