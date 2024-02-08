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
#include "llvm/CodeGen/MachineScheduler.h"

namespace llvm {

/// A MachineSchedStrategy implementation for AIE post RA scheduling.
class AIEPostRASchedStrategy : public PostGenericScheduler {
public:
  // Abstraction of the basic block state. It hides the order in which
  // the regions are created. We may add more state in future, e.g.
  // live out status registers.
  using Region = std::vector<AIE::MachineBundle>;
  class BlockState {
    std::vector<Region> Regions;

  public:
    // We don't supply an addBottom(), which says we can only
    // create the block state in bottom-up fashion.
    void addTop(const Region &TopRegion) { Regions.emplace_back(TopRegion); }
    const Region &getTop() const { return Regions.back(); }
    Region &getTop() { return Regions.back(); }
    const Region &getBottom() const { return Regions.front(); }
  };

  // Represents how accurate our successor information is
  enum class ScoreboardTrust {
    // The bundles represent the true start of the blocks
    Absolute,
    // The bundles are accurate, but may shift at most one cycle
    // due to alignment of a successor block
    AccountForAlign,
    // We don't have bundles for all successors
    Conservative
  };

  AIEPostRASchedStrategy(const MachineSchedContext *C);

  /// Called after each region entry.
  void initialize(ScheduleDAGMI *Dag) override;

  /// Decides which zone to schedule from, and pick a node from it.
  /// As a consequence, this can change the value of \p IsTopDown.
  /// This may also bump the cycle until a instruction is ready to be issued.
  SUnit *pickNode(bool &IsTopNode) override;

  bool isAvailableNode(SUnit &SU, SchedBoundary &Zone,
                       bool VerifyReadyCycle) const override;

  /// Called after an SU is picked and its instruction re-inserted in the BB.
  /// This essentially updates the node's ReadyCycle to CurCycle, and notifies
  /// the HazardRecognizer of the newly-emitted instruction.
  void schedNode(SUnit *SU, bool IsTopNode) override;

  void enterMBB(MachineBasicBlock *MBB) override;
  void leaveMBB() override;
  void enterRegion(MachineBasicBlock *BB, MachineBasicBlock::iterator Begin,
                   MachineBasicBlock::iterator End, unsigned RegionInstrs);
  void leaveRegion(const SUnit &ExitSU);

  /// Explicitly process regions backwards. The first scheduled region in
  /// a block connects with successors.
  bool doMBBSchedRegionsTopDown() const override { return false; }

  /// Try inserting a node into the available queue, or add/keep it in the
  /// pending queue in case of hazards.
  /// Called on all the predecessors of an SU after it is scheduled bottom-up.
  void releaseBottomNode(SUnit *SU) override;

  /// Return true if all the successor MBB are scheduled.
  bool successorsAreScheduled(MachineBasicBlock *MBB) const;

  /// Initialize Bot scoreboard by replaying all Bundles from the
  /// successor blocks of CurBB for InterBlock scheduling
  /// \param Conservative If this is set, make sure we cannot reserve resources
  /// outside of the region, i.e. block all resources.
  void initializeBotScoreBoard(ScoreboardTrust Trust);

  /// Schedule MBB in a sequence such that all the successors are scheduled
  /// before a given basic block is scheduled. This may not hold true if graph
  /// is cyclic.
  std::vector<MachineBasicBlock *>
  getMBBScheduleSeq(MachineFunction &MF) const override;

  // Return the scheduler state for this block
  const BlockState &getBlockState(MachineBasicBlock *MBB) const;
  // Return the current block
  MachineBasicBlock *getCurMBB() const { return CurMBB; }

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

  SchedBoundary Bot;

  /// Keeps track of the current zone used for scheduling. See getSchedZone().
  bool IsTopDown = true;

  MachineBasicBlock *CurMBB = nullptr;
  MachineBasicBlock::iterator RegionBegin = nullptr;
  MachineBasicBlock::iterator RegionEnd = nullptr;

  /// Minimum number of cycles to be scheduled bottom-up in the current region.
  unsigned RegionBottomUpCycles = 0;

  /// Returns true if, when "concatenated", the Top and Bot zone have resource
  /// conflicts or timing issues.
  bool checkInterZoneConflicts() const;

  /// Identify and fix issues in the scheduling region by inserting NOPs.
  /// In particular, this makes sure no instructions are in flight when leaving
  /// the region, and that the Top and Bot zones can be safely "concatenated".
  /// See checkInterZoneConflicts().
  void handleRegionConflicts(const SUnit &ExitSU);

  /// Post-process the bundles in \ref Zone to insert any required NOP
  /// and order nested instructions in a canonical order.
  void leaveSchedulingZone(SchedBoundary &Zone);

  static AIEHazardRecognizer *getAIEHazardRecognizer(const SchedBoundary &Zone);

private:
  /// Save ordered scheduled bundles for all the regions in a MBB.
  std::map<MachineBasicBlock *, BlockState> ScheduledMBB;

  /// This flag is set for the first processed region of a basic block. We force
  /// this to be the bottom one, i.e. the one which connects to successor
  /// blocks whose entry state we're going to propagate
  bool IsBottomRegion = false;
};

/// A MachineSchedStrategy implementation for AIE pre-RA scheduling.
class AIEPreRASchedStrategy : public GenericScheduler {
public:
  AIEPreRASchedStrategy(const MachineSchedContext *C) : GenericScheduler(C) {}

  void enterRegion(MachineBasicBlock *BB, MachineBasicBlock::iterator Begin,
                   MachineBasicBlock::iterator End, unsigned RegionInstrs);
  void leaveRegion(const SUnit &ExitSU);

  bool isAvailableNode(SUnit &SU, SchedBoundary &Zone,
                       bool VerifyReadyCycle) const override;

private:
  MachineBasicBlock *CurMBB = nullptr;
  MachineBasicBlock::iterator RegionBegin = nullptr;
  MachineBasicBlock::iterator RegionEnd = nullptr;
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

  // Give dag mutators access to the scheduler state
  AIEPostRASchedStrategy *getSchedImpl() const;
};

/// Similar to AIEScheduleDAGMI but for ScheduleDAGMILive.
class AIEScheduleDAGMILive final : public ScheduleDAGMILive {
public:
  using ScheduleDAGMILive::ScheduleDAGMILive;

  void enterRegion(MachineBasicBlock *BB, MachineBasicBlock::iterator Begin,
                   MachineBasicBlock::iterator End,
                   unsigned RegionInstrs) override;

  void exitRegion() override;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEMACHINESCHEDULER_H
