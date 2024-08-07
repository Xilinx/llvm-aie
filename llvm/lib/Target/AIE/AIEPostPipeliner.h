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
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/ResourceScoreboard.h"
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
  // For an LCD K1 -> K2, this holds II + Earliest(K2 - NInstr) - Latency(LCD)
  // Instructions with lower Latest have higher priority in the
  // top down scheduling
  int Latest = 0;
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

  /// Place SU in cycle Cycle; update Earliest of dependent instructions
  void scheduleNode(SUnit &SU, int Cycle);

  /// Compute the stage in which each instruction runs
  void computeStages();

  // return the first Cycle: Earliest <= Cycle < Earliest+NTries where MI fits
  // in the scoreboard, -1 if it doesn't fit. The insertion point is taken
  // module II.
  int fit(MachineInstr *MI, int Earliest, int NTries, int II);

  /// Provide some look ahead by seeing the effect of the first iteration
  /// on the second iteration.
  void computeLoopCarriedParameters();

  /// Find the first available unscheduled instruction with the highest
  /// priority
  int mostUrgent();

  /// Schedule the original instructions, taking the modulo scoreboard
  /// into account
  bool scheduleFirstIteration();

  /// Check that all copied instructions can run in the same modulo cycle
  bool scheduleOtherIterations();

public:
  PostPipeliner(const AIEHazardRecognizer &HR, int NInstr);

  /// Check whether this is a suitable loop for the PostPipeliner. It also
  /// leaves some useful information.
  bool canAccept(MachineBasicBlock &Loop);

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
