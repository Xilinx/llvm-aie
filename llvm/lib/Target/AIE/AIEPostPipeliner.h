//===- AIEPostPipeliner.h - Post RA Pipeliner                              ===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This file contains a simple post-RA pipeliner. It tries to wrap the linear
// schedule into a number of stages
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEPOSTPIPELINER_H
#define LLVM_LIB_TARGET_AIE_AIEPOSTPIPELINER_H

#include "AIEHazardRecognizer.h"
#include "AIESlotCounts.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/ResourceScoreboard.h"
#include <unordered_set>
#include <vector>

namespace llvm {
class MachineInstr;
class AIEHazardRecognizer;
} // namespace llvm

namespace llvm::AIE {

/// This is a dedicated softwarepipeliner. Its schedule method takes an
/// augmented DAG that represents a number of copies of a loop body.
/// These copies are scheduled 'synchronously', i.e. the copies are checked
/// to fit into the same cycle modulo II.

class NodeInfo {
public:
  // Keep track of being scheduled. Only maintained for the
  // representative instructions.
  bool Scheduled = false;
  // The linear cycle in which any instruction is scheduled.
  // only valid if Scheduled is set.
  int Cycle = 0;
  // Cycle % II
  int ModuloCycle = 0;
  // Cycle / II
  int Stage = 0;

  // The earliest cycle at which this can be scheduled to meet latencies
  // This includes the lowerbound of the modulo condition, i.e.
  // Earliest(N) >= Cycle(N - NInstr) + II
  int Earliest = 0;

  // The latest cycle at which this can be scheduled. This is a negative value
  // relative to the length of the linear schedule.
  // So -1 is the last cycle of the linear schedule, -Length is the first cycle
  // of the linear schedule. Note that this length is usually rounded up to
  // the next multiple of the initiation interval
  int Latest = -1;

  // These are the values of Earliest and Latest as computed from the a-priori
  // computations. During scheduling Earliest and Latest may be adjusted to
  // more accurate values. The two values are cached here to facilitate cheaper
  // reset before trying a new strategy for the same II.
  int StaticEarliest = 0;
  int StaticLatest = -1;

  // Slots necessary for this instruction.
  SlotCounts Slots;

  // Record critical path components
  // The Pred/Succ that pushed my Earliest/Latest
  std::optional<int> LastEarliestPusher;
  std::optional<int> LastLatestPusher;
  // The number of Succs/Preds whose Earliest/Latest I have pushed.
  int NumPushedEarliest = 0;
  int NumPushedLatest = 0;

  // Latest corrected by taking Earliest of an LCD successor into account
  int LCDLatest = -1;

  // The transitive closure of my predecessors
  std::unordered_set<int> Ancestors;

  // The transitive closure of my successors
  std::unordered_set<int> Offspring;

  /// Reset the node to the values computed statically
  /// If FullReset is true, also reset the accumulated dynamic data.
  void reset(bool FullReset);
};

class PostPipelinerStrategy {
protected:
  ScheduleDAGInstrs &DAG;
  std::vector<NodeInfo> &Info;
  int LatestBias = 0;

public:
  PostPipelinerStrategy(ScheduleDAGInstrs &DAG, std::vector<NodeInfo> &Info,
                        int LatestBias)
      : DAG(DAG), Info(Info), LatestBias(LatestBias) {};
  virtual ~PostPipelinerStrategy() {};
  // Provide a name for logging purposes
  virtual std::string name() { return "PostPipelinerStrategy"; }
  // Choose among available alternatives
  virtual bool better(const SUnit &A, const SUnit &B) { return false; }
  // Define the earliest cycle in which to insert \p N
  virtual int earliest(const SUnit &N) { return Info[N.NodeNum].Earliest; }
  // Define the latest cycle in which to insert \p N
  virtual int latest(const SUnit &N) {
    return Info[N.NodeNum].Latest + LatestBias;
  }
  // Select from top or from bottom.
  virtual bool fromTop() { return true; }
  // Report a final selection. This marks the start of selecting a new node.
  // fromTop() should be invariant between calls to selected()
  virtual void selected(const SUnit &N) {};
};

class PipelineScheduleVisitor {
public:
  virtual ~PipelineScheduleVisitor();
  virtual void startPrologue() {};
  virtual void startLoop() {};
  virtual void startEpilogue() {};
  virtual void finish() {};
  virtual void startBundle() {};
  virtual void addToBundle(MachineInstr *MI) = 0;
  virtual void endBundle() {};
};

class PostPipeliner {
  const AIEHazardRecognizer &HR;
  ScheduleDAGMI *DAG = nullptr;
  const AIEBaseInstrInfo *TII = nullptr;

  int NTotalInstrs = 0;
  int FirstUnscheduled = 0;
  int LastUnscheduled = -1;

  /// Holds the cycle of each SUnit. The following should hold:
  /// Cycle(N) mod II == Cycle(N % NInstr) mod II
  std::vector<NodeInfo> Info;

  // The scoreboard and its depth
  ResourceScoreboard<FuncUnitWrapper> Scoreboard;
  int Depth;

  /// The minimum tripcount, read from the pragma, or from an LC initialization
  int MinTripCount = 0;

  /// The Preheader of the loop
  MachineBasicBlock *Preheader = nullptr;

  // The instruction defining the tripcount
  MachineInstr *TripCountDef = nullptr;

  // Basic modulo scheduling parameters
  int NInstr;
  int NCopies;
  int II = 1;
  int NStages = 0;

  /// Place SU in cycle Cycle; update Earliest of successors and Latest
  /// of predecessors
  void scheduleNode(SUnit &SU, int Cycle);

  /// Compute the stage in which each instruction runs
  void computeStages();

  // return the first Cycle: Earliest <= Cycle < Earliest+NTries where MI fits
  // in the scoreboard, -1 if it doesn't fit. The insertion point is taken
  // module II.
  int fit(MachineInstr *MI, int Earliest, int NTries, int II);

  /// Provide some look ahead by seeing the effect of the first iteration
  /// on the second iteration. May return false if the II isn't feasible.
  bool computeLoopCarriedParameters();

  /// Helpers of computeLoopCarriedParameters()
  void computeForward();
  bool computeBackward();

  // Given Earliest and Latest of each node in the first iteration,
  // compute the smallest length of the linear schedule that is feasible.
  // this length will be a multiple of the InitiationInterval
  int computeMinScheduleLength();

  /// Try all heuristics, stop at the first that fits the II
  /// If it returns true, a valid schedule is laid down in Info.
  bool tryHeuristics();

  /// Find the first available unscheduled instruction with the highest
  /// priority
  int mostUrgent(PostPipelinerStrategy &Strategy);

  /// Schedule the original instructions, taking the modulo scoreboard
  /// into account
  bool scheduleFirstIteration(PostPipelinerStrategy &Strategy);

  /// Check that all copied instructions can run in the same modulo cycle
  bool scheduleOtherIterations();

  /// Reset dynamic scheduling data.
  /// If FullReset is set, also reset information collected from earlier
  /// data mining scheduling rounds
  void resetSchedule(bool FullReset);

public:
  PostPipeliner(const AIEHazardRecognizer &HR, int NInstr);

  /// Check whether this is a suitable loop for the PostPipeliner. It also
  /// leaves some useful information.
  bool canAccept(MachineBasicBlock &LoopBlock);

  /// Get a lowerbound for the II required to accommodate the slots.
  /// \pre canAccept has returned true
  int getResMII(MachineBasicBlock &LoopBlock);

  // Schedule using the given InitiationInterval. Return true when successful.
  // In that case calls to the query methods below are legitimate
  bool schedule(ScheduleDAGMI &DAG, int InitiationInterval);

  // quick query for the stage count
  int getStageCount() { return NStages; }

  // After scheduling, interpret the results and call the appropriate methods
  // in the Visitor interface object.
  // There are section delimitor methods for prologue, loop, and epilogue
  // end end-of-epilogue.
  // Between those delimitors, it will call emit() with instructions that need
  // to be cloned and placed in the appropriate sections. These calls are
  // bracketed with start and end methods to indicate cycles.
  void visitPipelineSchedule(PipelineScheduleVisitor &Visitor) const;

  // Modify the tripcount to run StageCount-1 less iterations.
  void updateTripCount() const;

  void dump() const;
};

} // namespace llvm::AIE
#endif // LLVM_LIB_TARGET_AIE_AIEPOSTPIPELINER_H
