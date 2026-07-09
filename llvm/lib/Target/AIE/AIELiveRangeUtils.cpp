//===- AIELiveRangeUtils.cpp - Live Range Utilities -----------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIELiveRangeUtils.h"
#include "AIEHazardRecognizer.h"
#include "AIERegDefUseTracker.h"
#include "AIEScheduleInterpreter.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/ResourceScoreboard.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "aie-live-range-utils"

using namespace llvm;

namespace llvm::AIE {

// Compute an approximate lowerbound for the schedule length of LR.
// It accounts for all internal latencies and resource conflicts.
unsigned computeMinimalSchedule(const RegLiveRange &LR, const ScheduleDAG &DAG,
                                const AIEHazardRecognizer &HR,
                                const AIEScheduleInterpreter &Interp) {

  // Create a local scoreboard for this scheduling attempt. We will not
  // allow negative latencies to cause negative issue cycles.
  const int ScoreboardUpperBound = 63;

  ResourceScoreboard<FuncUnitWrapper> LocalScoreboard;
  LocalScoreboard.config(0, ScoreboardUpperBound + HR.getPipelineDepth());

  // Collect instructions from the live range, defs first, then uses.
  // This provides a natural topological ordering for most cases.
  // Seen represents the relevant subgraph we want to schedule.
  SmallVector<const MachineInstr *, 8> Instructions;
  DenseSet<const MachineInstr *> Seen;

  auto CollectInstr = [&](iterator_range<const RegOperandInfo *> Opers) {
    for (const auto &Oper : Opers) {
      const MachineInstr *MI = Oper.getOperand()->getParent();
      if (Seen.insert(MI).second)
        Instructions.push_back(MI);
    }
  };

  CollectInstr(LR.defs());
  CollectInstr(LR.uses());

  // Build a map from MachineInstr to SUnit for dependency tracking.
  // The DAG may contain multiple copies of instructions (for pipelining).
  // try_emplace retains the first one, which is the representative one.
  DenseMap<const MachineInstr *, SUnit *> MIToSUnit;
  for (SUnit &SU : const_cast<ScheduleDAG &>(DAG).SUnits) {
    MachineInstr *MI = SU.getInstr();
    assert(MI && "SUnit must have a MachineInstr");
    MIToSUnit.try_emplace(MI, &SU);
  }

  // Schedule instructions with multiple scans.
  // Track which instructions have been scheduled.
  DenseMap<const MachineInstr *, int> IssueCycles;
  DenseSet<const MachineInstr *> Scheduled;

  // Keep scanning until all instructions are scheduled.
  while (Scheduled.size() < Instructions.size()) {
    bool MadeProgress = false;

    for (const MachineInstr *MI : Instructions) {
      if (Scheduled.count(MI))
        continue;

      SUnit *SU = MIToSUnit.lookup(MI);
      assert(SU && "Could not find SUnit for instruction in live range");

      // Check if all predecessors within the live range are scheduled.
      bool CanSchedule = true;
      int EarliestCycle = 0;

      for (const SDep &Pred : SU->Preds) {
        if (SUnit *PredSU = Pred.getSUnit()) {
          const MachineInstr *PredMI = PredSU->getInstr();
          if (!Seen.count(PredMI)) {
            continue;
          }
          if (!Scheduled.count(PredMI)) {
            CanSchedule = false;
            break;
          }
          // Account for latency (can be negative).
          int PredCycle = IssueCycles[PredMI];
          int MinCycle = PredCycle + static_cast<int>(Pred.getSignedLatency());
          EarliestCycle = std::max(EarliestCycle, MinCycle);
        }
      }

      if (!CanSchedule)
        continue;

      // Find the earliest cycle without structural hazards.
      // Start from EarliestCycle.
      int IssueCycle = EarliestCycle;
      while (HR.getHazardType(LocalScoreboard, MI, IssueCycle) !=
             ScheduleHazardRecognizer::NoHazard) {
        ++IssueCycle;
        if (IssueCycle > ScoreboardUpperBound) {
          // We can't check resources this far, but will still lead to a
          // valid lowerbound.
          break;
        }
      }

      // Schedule the instruction.
      IssueCycles[MI] = IssueCycle;
      Scheduled.insert(MI);
      MadeProgress = true;

      // Update local scoreboard.
      HR.emitInScoreboard(LocalScoreboard, *MI, MI->getDesc(), IssueCycle);
    }

    // We must make progress in each iteration.
    if (!MadeProgress) {
      LLVM_DEBUG({
        dbgs() << "Scheduling Live range failed. Remaining instructions:\n";
        for (const MachineInstr *MI : Instructions) {
          if (!Scheduled.count(MI)) {
            dbgs() << "  Unscheduled: " << *MI;
          }
        }
      });
    }
    assert(MadeProgress && "Failed to make scheduling progress");
  }

  // Compute the minimal live length directly from the live range's operands.
  // This avoids building an event schedule for all instructions and correctly
  // accounts for per-operand timing rather than instruction-level timing.
  int MinDefCycle = INT_MAX;
  int MaxUseCycle = INT_MIN;

  for (const auto &DefInfo : LR.defs()) {
    const MachineInstr *MI = DefInfo.getOperand()->getParent();
    auto It = IssueCycles.find(MI);
    if (It == IssueCycles.end())
      continue;
    const int Cycle = Interp.getOperandAccessCycle(
        *MI, It->second, DefInfo.getOperand()->getOperandNo());
    MinDefCycle = std::min(MinDefCycle, Cycle);
  }
  for (const auto &UseInfo : LR.uses()) {
    const MachineInstr *MI = UseInfo.getOperand()->getParent();
    auto It = IssueCycles.find(MI);
    if (It == IssueCycles.end())
      continue;
    const int Cycle = Interp.getOperandAccessCycle(
        *MI, It->second, UseInfo.getOperand()->getOperandNo());
    MaxUseCycle = std::max(MaxUseCycle, Cycle);
  }

  // The minimal live length is the distance from first def event to the cycle
  // before the last use event (the value is live from def until consumed).
  if (MinDefCycle != INT_MAX && MaxUseCycle != INT_MIN)
    return MaxUseCycle - MinDefCycle;
  return 0;
}

} // end namespace llvm::AIE
