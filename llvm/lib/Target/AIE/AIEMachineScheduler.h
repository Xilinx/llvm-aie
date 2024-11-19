//===- AIEMachineScheduler.h - Custom AIE MI scheduler ----------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Custom AIE MI scheduler.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEMACHINESCHEDULER_H
#define LLVM_LIB_TARGET_AIE_AIEMACHINESCHEDULER_H

#include "AIEHazardRecognizer.h"
#include "AIEInterBlockScheduling.h"
#include "AIEPostPipeliner.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include <memory>

namespace llvm {

using BlockState = AIE::BlockState;
using BlockType = AIE::BlockType;
using Region = AIE::Region;
using ScoreboardTrust = AIE::ScoreboardTrust;

namespace AIE {
std::vector<AIE::MachineBundle> computeAndFinalizeBundles(SchedBoundary &Zone);
} // namespace AIE

/// A MachineSchedStrategy implementation for AIE post RA scheduling.
class AIEPostRASchedStrategy : public PostGenericScheduler {
  /// Maintain the state of interblock/loop-aware scheduling
  AIE::InterBlockScheduling InterBlock;

public:
  AIEPostRASchedStrategy(const MachineSchedContext *C);

  /// Called after each region entry.
  void initialize(ScheduleDAGMI *Dag) override;

  /// Decides which zone to schedule from, and pick a node from it.
  /// As a consequence, this can change the value of \p IsTopDown.
  /// This may also bump the cycle until a instruction is ready to be issued.
  SUnit *pickNodeAndCycle(bool &IsTopNode,
                          std::optional<unsigned> &BotEmissionCycle) override;

  bool isAvailableNode(SUnit &SU, SchedBoundary &Zone,
                       bool VerifyReadyCycle) override;

  /// Called after an SU is picked and its instruction re-inserted in the BB.
  /// This essentially updates the node's ReadyCycle to CurCycle, and notifies
  /// the HazardRecognizer of the newly-emitted instruction.
  void schedNode(SUnit *SU, bool IsTopNode) override;

  void enterFunction(MachineFunction *MF) override;
  void leaveFunction() override;

  void enterMBB(MachineBasicBlock *MBB) override;
  void leaveMBB() override;
  void enterRegion(MachineBasicBlock *BB, MachineBasicBlock::iterator Begin,
                   MachineBasicBlock::iterator End, unsigned RegionInstrs);
  void leaveRegion(const SUnit &ExitSU);

  /// We build the graph ourselves from the (original) semantical order
  void buildGraph(ScheduleDAGMI &DAG, AAResults *AA,
                  RegPressureTracker *RPTracker, PressureDiffs *PDiffs,
                  LiveIntervals *LIS, bool TrackLaneMasks) override;

  /// Adds a SUnit for the given fixed instruction
  /// \param IsTop Whether MI is fixed at the top or bottom of the region
  SUnit &addFixedSUnit(MachineInstr &MI, bool IsTop);

  /// Whether \p SU is fixed in a specific cycle of the given zone.
  bool isFixedSU(const SUnit &SU, bool IsTop) const;

  /// Whether \p SU is free to be scheduled anywhere in the region.
  /// (modulo dependencies and resource conflicts)
  bool isFreeSU(const SUnit &SU) const;

  /// Explicitly process regions backwards. The first scheduled region in
  /// a block connects with successors.
  bool doMBBSchedRegionsTopDown() const override { return false; }

  /// Return true if all the successor MBB are scheduled and converged.
  bool successorsAreScheduled(MachineBasicBlock *MBB) const;

  /// Initialize Bot scoreboard by replaying all Bundles from the
  /// successor blocks of CurBB for InterBlock scheduling
  /// \param Conservative If this is set, make sure we cannot reserve resources
  /// outside of the region, i.e. block all resources.
  void initializeBotScoreBoard(ScoreboardTrust Trust);

  MachineBasicBlock *nextBlock() override;

  // Return the current block
  MachineBasicBlock *getCurMBB() const { return CurMBB; }

  const AIE::InterBlockScheduling &getInterBlock() const { return InterBlock; }
  AIE::InterBlockScheduling &getInterBlock() { return InterBlock; }

  AIEAlternateDescriptors &getSelectedAltDescs() {
    return InterBlock.getSelectedAltDescs();
  }

protected:
  /// Apply a set of heuristics to a new candidate for PostRA scheduling.
  ///
  /// \param Cand provides the policy and current best candidate.
  /// \param TryCand refers to the next SUnit candidate, otherwise
  /// uninitialized.
  /// \return \c true if TryCand is better than Cand (Reason is
  /// NOT NoCand)
  bool tryCandidate(SchedCandidate &Cand, SchedCandidate &TryCand) override;

  /// Returns the current zone to be used for scheduling.
  /// The zone may be switched at any moment when scheduling a region, e.g.
  ///  - Scheduling from Top might give better results
  ///  - Scheduling from Bot might be required to schedule a node exactly
  ///    5 cycles before the end of the region.
  /// See \p pickNode()
  SchedBoundary &getSchedZone() { return IsTopDown ? Top : Bot; }

  /// Maximum (absolute) distance between the current cycle and the emission
  /// cycle of instructions to be scheduled.
  int getMaxDeltaCycles(const SchedBoundary &Zone) const;

  /// Return the next "fixed" instruction to place down.
  SUnit *getNextUnscheduledFixedInstr(const SchedBoundary &Zone) const;

  /// SU numbers for fixed instructions.
  /// "top" fixed SUnits belong in [FirstTopFixedSU,FirstBotFixedSU)
  /// "bot" fixed SUnits belong in [FirstBotFixedSU,LastBotFixedSU]
  std::optional<unsigned> FirstTopFixedSU;
  std::optional<unsigned> FirstBotFixedSU;
  std::optional<unsigned> LastBotFixedSU;

  /// Keeps track of the current zone used for scheduling. See getSchedZone().
  bool IsTopDown = true;

  MachineBasicBlock *CurMBB = nullptr;
  MachineBasicBlock::iterator RegionBegin = nullptr;
  MachineBasicBlock::iterator RegionEnd = nullptr;

  /// Minimum number of cycles to be scheduled bottom-up in the current region.
  unsigned RegionBottomUpCycles = 0;

  /// Materialize "multi-opcode" instructions into the option that was selected
  /// at schedule time. See AIEHazardRecognizer::getSelectedAltOpcode().
  void materializeMultiOpcodeInstrs();

  /// Returns true if, when "concatenated", the Top and Bot zone have resource
  /// conflicts or timing issues.
  bool checkInterZoneConflicts(
      const std::vector<AIE::MachineBundle> &BotBundles) const;

  /// Identify and fix issues in the scheduling region by inserting NOPs.
  /// In particular, this makes sure no instructions are in flight when leaving
  /// the region, and that the Top and Bot zones can be safely "concatenated".
  /// See checkInterZoneConflicts().
  void handleRegionConflicts(const SUnit &ExitSU,
                             std::vector<AIE::MachineBundle> &TopBundles,
                             const std::vector<AIE::MachineBundle> &BotBundles);

  // After scheduling a block, fill in nops, apply bundling, etc.
  void commitBlockSchedule(MachineBasicBlock *BB);

private:
  /// This flag is set for the first processed region of a basic block. We force
  /// this to be the bottom one, i.e. the one which connects to successor
  /// blocks whose entry state we're going to propagate
  bool IsBottomRegion = false;
};

/// A MachineSchedStrategy implementation for AIE pre-RA scheduling.
class AIEPreRASchedStrategy : public GenericScheduler {
public:
  AIEPreRASchedStrategy(const MachineSchedContext *C) : GenericScheduler(C) {}

  void initialize(ScheduleDAGMI *DAG) override;

  // We override to be able to skip the prescheduler for specific blocks
  MachineBasicBlock *nextBlock() override;

  void enterRegion(MachineBasicBlock *BB, MachineBasicBlock::iterator Begin,
                   MachineBasicBlock::iterator End, unsigned RegionInstrs);
  void leaveRegion(const SUnit &ExitSU);

  bool isAvailableNode(SUnit &SU, SchedBoundary &Zone,
                       bool VerifyReadyCycle) override;

  AIEAlternateDescriptors &getSelectedAltDescs() { return SelectedAltDescs; }

protected:
  /// Whether \p DelayedSU can be safely delayed without forming a cycle
  /// of SUs delaying each other indefinitely.
  bool canBeDelayed(const SUnit &DelayedSU, const SUnit &Delayer) const;

  /// Apply a set of heuristics to a new candidate for scheduling.
  ///
  /// \param Cand provides the policy and current best candidate.
  /// \param TryCand refers to the next SUnit candidate, otherwise
  /// uninitialized.
  /// \return \c true if TryCand is better than Cand (Reason is
  /// NOT NoCand)
  bool tryCandidate(SchedCandidate &Cand, SchedCandidate &TryCand,
                    SchedBoundary *Zone) const override;

private:
  MachineBasicBlock *CurMBB = nullptr;
  MachineBasicBlock::iterator RegionBegin = nullptr;
  MachineBasicBlock::iterator RegionEnd = nullptr;

  /// Keeps track of SUs that have been delayed, waiting on another
  /// pressure-reducing SU to be scheduled first.
  /// SUDelayerMap[0] = 2 means that SU(0) is waiting on SU(2).
  std::vector<unsigned> SUDelayerMap;

  std::vector<unsigned> PSetThresholds;

  AIEAlternateDescriptors SelectedAltDescs;
};

/// An extension to ScheduleDAGMI that provides callbacks on region entry/exit
/// to the HazardRecognizer, and forces scheduling of single-instruction
/// regions.
class AIEScheduleDAGMI final : public ScheduleDAGMI {
public:
  using ScheduleDAGMI::ScheduleDAGMI;

  void enterRegion(MachineBasicBlock *BB, MachineBasicBlock::iterator Begin,
                   MachineBasicBlock::iterator End,
                   unsigned RegionInstrs) override;

  void exitRegion() override;

  void finalizeSchedule() override;
  void recordDbgInstrs(const Region &CurrentRegion);

  // We hook into the alias analysis to exploit knowledge about virtual unroll
  bool mayAlias(SUnit *SUa, SUnit *SUb, bool UseTBAA) override;

  // We override the default schedule method in order to perform PostRASWP
  void schedule() override;

  // Give dag mutators access to the scheduler state
  AIEPostRASchedStrategy *getSchedImpl() const;

protected:
  void releasePred(SUnit *SU, SDep *PredEdge) override;
};

/// Similar to AIEScheduleDAGMI but for ScheduleDAGMILive.
class AIEScheduleDAGMILive final : public ScheduleDAGMILive {
public:
  using ScheduleDAGMILive::ScheduleDAGMILive;

  void enterRegion(MachineBasicBlock *BB, MachineBasicBlock::iterator Begin,
                   MachineBasicBlock::iterator End,
                   unsigned RegionInstrs) override;

  void exitRegion() override;

  // Give dag mutators access to the scheduler state
  AIEPreRASchedStrategy *getSchedImpl() const;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEMACHINESCHEDULER_H
