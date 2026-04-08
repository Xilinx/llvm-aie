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

LiveRangeScheduleResult
computeMinimalSchedule(const RegLiveRange &LR, const ScheduleDAG &DAG,
                       const AIEHazardRecognizer &HR,
                       const AIEScheduleInterpreter &Interp) {
  // TODO: Determine optimal scoreboard bounds based on pipeline depth
  // and latencies. For now, use a fixed range.
  constexpr int ScoreboardLowerBound = -32;
  constexpr int ScoreboardUpperBound = 31;

  // Create a local scoreboard for this scheduling attempt.
  ResourceScoreboard<FuncUnitWrapper> LocalScoreboard;
  LocalScoreboard.config(ScoreboardLowerBound, ScoreboardUpperBound);

  // Collect instructions from the live range, defs first, then uses.
  // This provides a natural topological ordering for most cases.
  SmallVector<const MachineInstr *, 8> Instructions;
  DenseSet<const MachineInstr *> Seen;

  // Collect def instructions.
  for (const auto &DefInfo : LR.defs()) {
    const MachineInstr *MI = DefInfo.getOperand()->getParent();
    if (Seen.insert(MI).second)
      Instructions.push_back(MI);
  }

  // Collect use instructions.
  for (const auto &UseInfo : LR.uses()) {
    const MachineInstr *MI = UseInfo.getOperand()->getParent();
    if (Seen.insert(MI).second)
      Instructions.push_back(MI);
  }

  // Build a map from MachineInstr to SUnit for dependency tracking.
  // The DAG may contain multiple copies of instructions (for pipelining).
  // Use try_emplace to only map the first occurrence of each instruction.
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
          if (PredMI && Seen.count(PredMI)) {
            if (!Scheduled.count(PredMI)) {
              CanSchedule = false;
              break;
            }
            // Account for latency (can be negative).
            int PredCycle = IssueCycles[PredMI];
            int MinCycle = PredCycle + static_cast<int>(Pred.getLatency());
            EarliestCycle = std::max(EarliestCycle, MinCycle);
          }
        }
      }

      if (!CanSchedule)
        continue;

      // Find the earliest cycle without structural hazards.
      // Start from EarliestCycle (which can be negative).
      int IssueCycle = EarliestCycle;
      while (HR.getHazardType(LocalScoreboard, MI, IssueCycle) !=
             ScheduleHazardRecognizer::NoHazard) {
        ++IssueCycle;
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
        dbgs()
            << "Failed to make scheduling progress. Remaining instructions:\n";
        for (const MachineInstr *MI : Instructions) {
          if (!Scheduled.count(MI)) {
            dbgs() << "  Unscheduled: " << *MI;
            SUnit *SU = MIToSUnit.lookup(MI);
            if (SU) {
              dbgs() << "    Waiting for predecessors:\n";
              for (const SDep &Pred : SU->Preds) {
                if (SUnit *PredSU = Pred.getSUnit()) {
                  const MachineInstr *PredMI = PredSU->getInstr();
                  if (PredMI && Seen.count(PredMI) &&
                      !Scheduled.count(PredMI)) {
                    dbgs() << "      " << *PredMI;
                  }
                }
              }
            }
          }
        }
      });
    }
    assert(MadeProgress && "Failed to make scheduling progress");
  }

  // Generate events for all scheduled instructions.
  EventSchedule Schedule;
  for (const MachineInstr *MI : Instructions) {
    int IssueCycle = IssueCycles[MI];
    Interp.addInstructionEvents(*MI, IssueCycle, Schedule);
  }

  // Compute the minimal live length from the event schedule.
  // Find the earliest def event and latest use event for this live range.
  int MinDefCycle = INT_MAX;
  int MaxUseCycle = INT_MIN;

  for (size_t Cycle = 0; Cycle < Schedule.size(); ++Cycle) {
    for (const auto &Event : Schedule[Cycle]) {
      // Check if this event belongs to an instruction in our live range.
      if (!Seen.count(Event.MI))
        continue;

      if (Event.Type == EventType::Write) {
        // This is a def event - update earliest def cycle.
        MinDefCycle = std::min(MinDefCycle, static_cast<int>(Cycle));
      } else if (Event.Type == EventType::Read) {
        // This is a use event - update latest use cycle.
        MaxUseCycle = std::max(MaxUseCycle, static_cast<int>(Cycle));
      }
    }
  }

  // The minimal live length is the distance from first def event to the cycle
  // before the last use event (the value is live from def until consumed).
  unsigned MinimalLength = 0;
  if (MinDefCycle != INT_MAX && MaxUseCycle != INT_MIN) {
    MinimalLength = MaxUseCycle - MinDefCycle;
  }

  return LiveRangeScheduleResult(MinimalLength);
}

} // end namespace llvm::AIE
