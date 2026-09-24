//===- AIEScarceRegScheduling.h - Scarce Register Scheduling Strategy ----===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This file contains a PostPipelinerStrategy that prioritizes scheduling
// decisions based on scarce register pressure.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIESCARCEREGSCHEDULING_H
#define LLVM_LIB_TARGET_AIE_AIESCARCEREGSCHEDULING_H

#include "AIEPostPipeliner.h"
#include "llvm/ADT/SmallVector.h"
#include <vector>

namespace llvm {
class RegLiveRange;
class RegLiveRangeTracker;
class SUnit;
} // namespace llvm

namespace llvm::AIE {

// Represents a scarce range to be scheduled atomically.
struct ScarceRange {
  // SUnit indices that are part of this scarce range.
  SmallVector<int, 4> Members;

  // Indices of scarce ranges that must precede this range (scarce-only DAG).
  SmallVector<int, 4> PredRanges;

  // Reference to the corresponding RegLiveRange with def/use operand info.
  // The LiveRange provides the MachineOperand pointers and indices needed for
  // anti-dependence simulation in BurstMostUrgentStrategy.
  const RegLiveRange &LiveRange;

  // Constructor computes Members from LiveRange's defs and uses via DAG.
  ScarceRange(const RegLiveRange &LR, const ScheduleDAGInstrs &DAG);
};

// Strategy for burst scheduling: prioritize predecessors of the current
// scarce range, then atomically place the scarce range members.
class BurstMostUrgentStrategy : public PostPipelinerStrategy {
  // The ordered sequence of scarce ranges to schedule.
  const std::vector<ScarceRange> &ScarceRanges;

  // Ordered sets to schedule (built by init() for the given range ordering).
  // For each burst i:
  //   OrderedMembers[2*i]
  //     Partitioned ancestors for ScarceRanges[RangeOrder[i]]
  //   OrderedMembers[2*i+1]
  //     ScarceRanges[RangeOrder[i]].Members
  std::vector<SmallVector<int, 4>> OrderedMembers;

  // Current index into OrderedMembers (which set we're working on).
  size_t CurrentSet = 0;

  // The range order supplied to the last init() call.
  // Stored so that simulateAntiDependences can iterate ranges in the
  // correct (order-specified) sequence rather than the original array order.
  SmallVector<int, 4> CurrentOrder;

  // The initiation interval for the current pipelining attempt.
  // Used in simulateAntiDependences to apply the correct iteration offset.
  int II;

public:
  BurstMostUrgentStrategy(ScheduleDAGInstrs &DAG, ScheduleInfo &Info,
                          const std::vector<ScarceRange> &ScarceRanges,
                          int LatestBias, int II);

  // Initialize OrderedMembers based on the given range order.
  void init(const SmallVector<int, 4> &RangeOrder);

  std::string name() override { return "BurstMostUrgentStrategy"; }

  bool better(const SUnit &A, const SUnit &B) override;

  void selected(const SUnit &N) override;

  bool fromTop() override { return true; }

private:
  // Log the ancestors and members of the burst that starts at set index Set.
  // Set must be even and in-bounds within OrderedMembers.
  [[maybe_unused]] void logBurstStart(unsigned Set) const;

  // Advance CurrentSet past any empty ancestors sets.
  void skipEmptySets();

  // Simulate anti-dependences from the burst at position BurstPos in
  // CurrentOrder to every other range. For each (use in burst, def in other
  // range) pair, the constraint applied depends on topological order:
  //   - Intra-iteration (def.NodeNum > use.NodeNum):
  //       Earliest[def] = max(Earliest[def], Cycle[use] + antiLat)
  //   - Loop-carried (def.NodeNum < use.NodeNum):
  //       Earliest[def] = max(Earliest[def], Cycle[use] + antiLat - II)
  void simulateAntiDependences(size_t BurstPos);
};

// Build a mapping from SUnit index to scarce range index.
// RangeOfSUnit[i] = range index if SUnit i is in a scarce range, -1 otherwise.
void buildScarceRangeMapping(const std::vector<ScarceRange> &Ranges,
                             const ScheduleInfo &Info,
                             std::vector<int> &RangeOfSUnit);

// Build the scarce-only DAG by populating PredRanges for each range.
void buildScarceDAG(std::vector<ScarceRange> &Ranges, const ScheduleInfo &Info,
                    const ScheduleDAGInstrs &DAG);

// Check that the scarce-only DAG is acyclic using Kahn's algorithm.
// Returns true if acyclic, false if a cycle is detected.
bool checkAcyclic(const std::vector<ScarceRange> &Ranges);

// Enumerate range orders compatible with the DAG.
// OnOrder returns true to stop enumeration (success), false to continue.
// Returns true if OnOrder returned true for any order, false otherwise.
bool enumerateRangeOrders(
    const std::vector<ScarceRange> &Ranges,
    llvm::function_ref<bool(const SmallVector<int, 4> &Order)> OnOrder);

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIESCARCEREGSCHEDULING_H
