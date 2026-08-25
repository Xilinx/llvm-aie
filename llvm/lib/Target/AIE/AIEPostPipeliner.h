//===- AIEPostPipeliner.h - Post RA Pipeliner                              ===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This file contains a simple post-RA pipeliner. It tries to wrap the linear
// schedule into a number of stages
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEPOSTPIPELINER_H
#define LLVM_LIB_TARGET_AIE_AIEPOSTPIPELINER_H

#include "AIEHazardRecognizer.h"
#include "AIERegDefUseTracker.h"
#include "AIEScheduleInterpreter.h"
#include "AIESchedulingTypes.h"
#include "AIESlotCounts.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/ResourceScoreboard.h"
#include <unordered_set>
#include <vector>

namespace llvm {
class MachineInstr;
class AIEHazardRecognizer;
} // namespace llvm

namespace llvm {
class RegLiveRangeTracker; // Forward declaration
}

namespace llvm::AIE {

namespace Solver {
class SolverData;
class SWPSolver;
} // namespace Solver

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

  /// "Tweaked" numbers for \p Earliest and \p Latest to use for the next
  /// iteration of this strategy.
  std::optional<int> TweakedEarliest;
  std::optional<int> TweakedLatest;

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

  // The length of the critical path hanging below this node, counting
  // only slack-free edges (slack: pred.Earliest + latency < succ.Earliest).
  // Edges where the successor is already driven by a longer
  // independent path are ignored.
  int EffectiveHeight = 0;

  // The transitive closure of my predecessors
  std::unordered_set<int> Ancestors;

  // The transitive closure of my successors
  std::unordered_set<int> Offspring;

  // Compute data derived from Cycle.
  // \II The initiation interval to use.
  void update(int II);

  /// Reset the node to the values computed statically
  /// If FullReset is true, also reset the accumulated dynamic data.
  void reset(bool FullReset);
};

class ScheduleInfo {
private:
  /// Pending rotation to be applied after validation succeeds.
  /// Set by peelSideEffectFree(), applied in scheduleWithStrategy().
  int PendingRotation = 0;

public:
  std::vector<NodeInfo> Nodes;
  int NInstr;
  // Effectively one more than the highest Cycle in Nodes, ultimately
  // rounded up to an integer multiple of II. Computed after scheduling
  // the first iteration.
  int Length = 0;
  void init(int NOrig) {
    NInstr = NOrig;
    Nodes.clear();
    Nodes.resize(2 * NInstr);
    Length = 0;
    PendingRotation = 0;
  }
  NodeInfo &operator[](int N) { return Nodes[N]; }
  const NodeInfo &operator[](int N) const { return Nodes[N]; }

  // Accept Nodes[Index] as part of the first iteration and update
  // the statistics.
  void commitCycle(int Index) {
    assert(Index < NInstr);
    auto &Node = Nodes[Index];
    Node.Scheduled = true;
    Length = std::max(Length, Node.Cycle + 1);
  }
  // Set the pending rotation. The actual rotation will be applied after
  // scheduleOtherIterations() succeeds, to ensure validation uses consistent
  // pre-rotation cycle values.
  // \param Rotation The number of cycles to insert
  void setRotation(int Rotation) { PendingRotation = Rotation; }
  // Apply the pending rotation by inserting empty cycles at the start,
  // shifting all cycles.
  // \param II The initiation interval used to update
  //           Stage and ModuloCycle of each node.
  void applyRotation(int II);
  // Reset the pending rotation.
  void resetRotation() { PendingRotation = 0; }
};

class PostPipelinerStrategy {
protected:
  ScheduleDAGInstrs &DAG;
  ScheduleInfo &Info;
  int LatestBias = 0;

  // Report change to the scheduling data. This can be used for iterative
  // refinement
  bool Changed = false;

public:
  void setEarliest(int Index, int Value) { Info[Index].Earliest = Value; }
  void setLatest(int Index, int Value) {
    Info[Index].Latest = Value - LatestBias;
  }

  // Register a change
  void setChanged() { Changed = true; }
  // Return changed and reset it
  bool checkAndResetChanged() {
    bool Old = Changed;
    Changed = false;
    return Old;
  }

public:
  PostPipelinerStrategy(ScheduleDAGInstrs &DAG, ScheduleInfo &Info,
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
  virtual int mobility(const SUnit &N) { return latest(N) - earliest(N); }
  // Select from top or from bottom.
  virtual bool fromTop() { return true; }
  // Report a final selection. This marks the start of selecting a new node.
  // fromTop() should be invariant between calls to selected()
  virtual void selected(const SUnit &N) {};

  // Decide a cycle in [Earliest, Latest] to insert MI, based on resource
  // hazards. Returns the chosen cycle on success, or an empty optional if none
  // fits. The default implementation uses fromTop() to determine scan
  // direction.
  virtual std::optional<int>
  fitInInterval(const SUnit &SU, int Earliest, int Latest, int II,
                const AIEHazardRecognizer &HR,
                ResourceScoreboard<FuncUnitWrapper> &Scoreboard);
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
  RegLiveRangeTracker &RegTracker;
  ScheduleDAGMI *DAG = nullptr;
  const AIEBaseInstrInfo *TII = nullptr;

  // Schedule interpreter for computing modulo live ranges
  AIEScheduleInterpreter Interpreter;

  // Event schedule populated during scheduling
  EventSchedule EventSched;

  int FirstUnscheduled = 0;
  int LastUnscheduled = -1;

  /// Holds the scheduling information for each instruction. The following
  /// should hold:
  /// Cycle(N) mod II == Cycle(N % NInstr) mod II
  ScheduleInfo Info;
  int MinLength;

  /// The length of the longest circuit in the graph.
  int RecMII = 0;

  // The scoreboard.
  ResourceScoreboard<FuncUnitWrapper> Scoreboard;

  /// The minimum tripcount, read from the pragma, or from an LC initialization.
  int MinTripCount = 0;

  /// Set when the loop is a runtime-guarded ("versioned") loop, in which case
  /// the minimum trip-count requirement is lifted and the guard threshold is
  /// patched with the required minimum after a schedule is found.
  bool IsVersionGuarded = false;

  /// The II requested by a pragma. This will trigger expensive algorithms
  /// like solvers or exhaustive searches to be run if the heuristic methods
  /// don't find a solution.
  int TargetII = 0;

  /// The Preheader of the loop.
  MachineBasicBlock *Preheader = nullptr;

  /// The instruction defining the tripcount.
  MachineInstr *TripCountDef = nullptr;

  /// Basic modulo scheduling parameters.
  int NInstr;
  int ScoreboardSize;
  int II = 1;
  int NStages = 0;
  int NPrologueStages = 0;

  /// The pipeliner mode passed from InterBlockScheduling.
  PostPipelinerMode Mode = PostPipelinerMode::None;

  /// Place SU in cycle Cycle; update Earliest of successors and Latest
  /// of predecessors.
  void scheduleNode(SUnit &SU, int Cycle, PostPipelinerStrategy &Strategy);

  /// Check the stage count against the tripcount to see whether this schedule
  /// can be trivially extracted into a pipelined loop.
  bool hasSufficientMinTripCount(int NS) const;

  /// If the iteration count is too low, we may be able to peel off some
  /// side-effect-free initial stage and rotate the schedule accordingly.
  /// When the method returns true, this was successful, and all relevant
  /// data has been updated to perform the modulo extraction.
  /// \pre !hasSufficientTripCount()
  bool peelSideEffectFree();

  /// Computes the stage in which each instruction runs and check the resulting
  /// stage count against MinIterCount and the number of copies in the DAG.
  /// Returns true if these checks indicate that the schedule can be implmented.
  bool checkStages();

  /// Provide some look ahead by seeing the effect of the first iteration
  /// on the second iteration. May return false if the II isn't feasible.
  bool computeLoopCarriedParameters();

  /// Helpers of computeLoopCarriedParameters()
  void biasForLocalResourceContention(NodeInfo &NI, const SUnit &SU);
  void computeForward();
  bool computeBackward();
  void computeRecMII();
  void computeEffectiveHeight();
  int computeScarceRegMII();

  /// Given Earliest and Latest of each node in the first iteration,
  /// compute the smallest length of the linear schedule that is feasible.
  /// this length will be a multiple of the InitiationInterval.
  int computeMinScheduleLength() const;

  // Set up the raw data of the problem, instructions (with their atrributes)
  // and latencies.
  Solver::SolverData createSolverData();

  /// Try to find a solution using a solver
  bool solve(const Solver::SolverData &Data, int NS, bool SEFStage);

  /// Helper of solve, applying one specific solver
  bool applySolver(const Solver::SolverData &Data, Solver::SWPSolver &Solver,
                   int NS, bool SEFStage);

  /// Try all approaches to arrive at a SWP schedule for the current II.
  /// If it returns true, a valid schedule is laid down in Info.
  bool tryApproaches();

  /// Find the first available unscheduled instruction with the highest
  /// priority.
  int mostUrgent(PostPipelinerStrategy &Strategy);

  /// Schedule the original instructions, taking the modulo scoreboard
  /// into account.
  bool scheduleFirstIteration(PostPipelinerStrategy &Strategy);

  /// Check that all copied instructions can run in the same modulo cycle
  bool scheduleOtherIterations(PostPipelinerStrategy &Strategy);

  /// Top level strategy scheduler
  bool scheduleWithStrategy(PostPipelinerStrategy &Strategy);

  /// Try to schedule scarce ranges by enumerating orders and using
  /// BurstMostUrgentStrategy.
  /// Checks applicability, finds scarce ranges, and attempts scheduling.
  /// Returns true if scheduling succeeded, false otherwise.
  bool tryScarceRangePacking();

  /// Reset dynamic scheduling data.
  /// If FullReset is set, also reset information collected from earlier
  /// data mining scheduling rounds.
  void resetSchedule(bool FullReset);

  /// Try to allocate registers for the current schedule
  /// Returns true if register allocation succeeds
  bool tryAllocateRegisters();

public:
  PostPipeliner(const AIEHazardRecognizer &HR, int NInstr,
                RegLiveRangeTracker &RegTracker, const MachineFunction &MF);

  /// Check whether this is a suitable loop for the PostPipeliner. It also
  /// leaves some useful information.
  bool isPostPipelineCandidate(MachineBasicBlock &LoopBlock);

  /// Get a lowerbound for the II required to accommodate the slots.
  /// \pre isPostPipelineCandidate has returned true
  int getResMII(MachineBasicBlock &LoopBlock);

  /// Schedule using the given InitiationInterval. Return true when successful.
  /// In that case calls to the query methods below are legitimate.
  /// \param PipelinerMode The mode the postpipeliner is operating in.
  bool schedule(ScheduleDAGMI &DAG, int InitiationInterval,
                PostPipelinerMode PipelinerMode);

  // Quick query for the stage count.
  int getStageCount() const { return NStages; }

  // Quick query for the achieved initiation interval.
  int getII() const { return II; }

  // After scheduling, interpret the results and call the appropriate methods
  // in the Visitor interface object.
  // There are section delimitor methods for prologue, loop, and epilogue
  // end end-of-epilogue.
  // Between those delimitors, it will call addToBundle() with instructions
  // that need to be cloned and placed in the appropriate sections. These calls
  // are bracketed with startBundle and endBundle methods to indicate cycles.
  void visitPipelineSchedule(PipelineScheduleVisitor &Visitor) const;

  // Core helper of visitPipelineSchedule to process a single section.
  // It will not call the section delimitor methods.
  // \param Filter will decide on calling Visitor.addToBundle().
  void visitPipelineSection(
      PipelineScheduleVisitor &Visitor, int Repeat,
      std::function<bool(const NodeInfo &Node, int Stage, int M)> Filter) const;

  // Modify the tripcount to run StageCount-1 less iterations.
  void updateTripCount() const;

  void materializePipeline(PipelineScheduleVisitor &Visitor);

  /// For a versioned loop, patch the guard's placeholder threshold pseudo with
  /// the actual stage count. No-op when not versioned.
  void updateVersionGuard() const;

  void dump() const;

  /// Set information about registers with simplified dependencies in the
  /// register tracker.
  void setSimplifiedRegsInfo(SimplifiedRegsInfo &&Info) {
    RegTracker.setSimplifiedRegsInfo(std::forward<SimplifiedRegsInfo>(Info));
  }
};

} // namespace llvm::AIE
#endif // LLVM_LIB_TARGET_AIE_AIEPOSTPIPELINER_H
