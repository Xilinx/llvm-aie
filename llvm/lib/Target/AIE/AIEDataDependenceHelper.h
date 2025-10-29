//===- AIEDataDependenceHelper.h - Inter-block DAG --------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2025 Advanced Micro Devices, Inc. or its affiliates
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
  bool mayAlias(SUnit *SUa, SUnit *SUb, bool TBAA) override;

public:
  DataDependenceHelper(const MachineSchedContext &Context,
                       bool AddMutators = true);

  // After adding the nodes, create the edges, using the order in which the
  // nodes were added.
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
} // namespace AIE
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEDATADEPENDENCEHELPER_H
