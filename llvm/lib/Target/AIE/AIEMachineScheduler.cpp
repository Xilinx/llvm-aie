//===- AIEMachineScheduler.cpp - MI Scheduler for AIE ---------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEMachineScheduler.h"
#include "AIEBaseAliasAnalysis.h"
#include "AIEBaseInstrInfo.h"
#include "AIEBundle.h"
#include "AIEHazardRecognizer.h"
#include "AIEInterBlockScheduling.h"
#include "AIEMaxLatencyFinder.h"
#include "AIEPostPipeliner.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/ResourceScoreboard.h"
#include "llvm/Support/Debug.h"
#include <memory>

#define DEBUG_TYPE "machine-scheduler"

// This is a more specific debug classes, separating the function and block
// level logging from the detailed scheduling info.
// useful combis:
// --debug-only=sched-blocks,loop-aware
// --debug-only=sched-blocks,machine-scheduler
#define DEBUG_BLOCKS(X) DEBUG_WITH_TYPE("sched-blocks", X)

using namespace llvm;
using namespace llvm::AIE;

static cl::opt<bool> InsertCycleSeparators(
    "aie-prera-cycle-separators", cl::init(false),
    cl::desc("[AIE] Insert CYCLE_SEPARATOR meta instructions"));
static cl::opt<bool>
    EnableFinerRPTracking("aie-premisched-finer-rp-tracking", cl::init(true),
                          cl::desc("Track reg pressure more accurately and "
                                   "delay some instructions to avoid spills."));
static cl::opt<unsigned> NumCriticalFreeRegs(
    "aie-premisched-near-critical-regs", cl::init(2),
    cl::desc("Number of free registers below which premisched should actively "
             "try to reduce the pressure."));

static cl::opt<unsigned> BottomUpCycles(
    "aie-bottomup-cycles", cl::init(std::numeric_limits<int>::max()),
    cl::desc("[AIE] Min number of cycles to be scheduled bottom-up"));
static cl::opt<int> BottomUpDelta(
    "aie-bottomup-delta", cl::init(128),
    cl::desc("[AIE] Max cycles delta relative to current for bottom-up sched"));

// Note: For AIE2 the latest register access is in E9, this can create negative
// latencies of -8 with E1 accessors.
// Still, does not hurt to be a bit more conservative with -10.
static cl::opt<int> NegativeLatencyLowerBound(
    "aie-neglatency-lowerbound", cl::init(-10),
    cl::desc("[AIE] Lower bound for negative-latency dependencies. Used to "
             "bump the window of schedulable cycles without hampering "
             "scheduling opportunities."));

static cl::opt<bool>
    AllowNegativeLatencies("aie-negative-latencies", cl::init(true),
                           cl::desc("[AIE] Allow negative-latency scheduling"));
static cl::opt<unsigned> ReservedDelaySlots(
    "aie-reserved-delay-slots", cl::init(0),
    cl::desc("[AIE] Number of delay slots to be left empty"));

static cl::opt<bool> EnableDelaySlotTopDown(
    "aie-delay-slot-topdown", cl::init(true),
    cl::desc("[AIE] When top-fixed bundles and a delay slot instruction "
             "coexist, schedule the whole region top-down and fix up the "
             "branch position in leaveRegion() instead of forcing bottom-up "
             "cycles"));

/// This is a testing option. Resetting it prevents inter-block conflicts from
/// the scoreboard, so that all interblock scheduling effects can be blamed on
/// the latencies.
static cl::opt<bool>
    InterBlockScoreboard("aie-interblock-scoreboard", cl::init(true),
                         cl::desc("Initialize the interblock scoreboard"));

/// This option indicates that there may be an alignment nop cycle inserted
/// in a successor block. It can be reset for testing purposes.
static cl::opt<bool>
    InterBlockAlignment("aie-interblock-alignment", cl::init(true),
                        cl::desc("Allow for alignment of successor blocks"));

static cl::opt<bool> UseLoopHeuristics(
    "aie-loop-sched-heuristics", cl::init(true),
    cl::desc("Use special picking heuristics when scheduling a loop region"));

static cl::opt<bool> PreSchedFollowsSkipPipeliner(
    "aie-presched-follows-skip-pipeliner", cl::init(true),
    cl::desc("Don't run the prescheduler if the pipeliner is skipped"));

namespace {
// A sentinel value to represent an unknown SUnit.
const constexpr unsigned UnknownSUNum = ~0;
} // namespace

static AIEHazardRecognizer *getAIEHazardRecognizer(const SchedBoundary &Zone) {
  return static_cast<AIEHazardRecognizer *>(Zone.HazardRec);
}

AIEPostRASchedStrategy::AIEPostRASchedStrategy(const MachineSchedContext *C)
    : PostGenericScheduler(C), InterBlock(C, InterBlockScoreboard) {
  assert(PostRADirection != MISched::Direction::Bidirectional &&
         "-misched-topdown incompatible with -misched-bottomup");
  if (PostRADirection == MISched::Direction::TopDown)
    this->IsTopDown = true;
  else if (PostRADirection == MISched::Direction::BottomUp)
    this->IsTopDown = false;
}

namespace {
/// Shorthand to get TargetInstrInfo from a MachineBasicBlock
const AIEBaseInstrInfo *getTII(MachineBasicBlock *MBB) {
  return static_cast<const AIEBaseInstrInfo *>(
      MBB->getParent()->getSubtarget().getInstrInfo());
}
const AIEBaseInstrInfo *getTII(const ScheduleDAGMI &DAG) {
  return static_cast<const AIEBaseInstrInfo *>(DAG.TII);
}

void bumpCycleForBundles(unsigned ToCycle,
                         std::vector<AIE::MachineBundle> &Bundles,
                         AIE::MachineBundle &CurrBundle) {
  unsigned CurrCycle = Bundles.size();
  assert(ToCycle > CurrCycle);

  auto BumpCycle = [&CurrCycle, &Bundles](const AIE::MachineBundle &NewCycle) {
    Bundles.push_back(NewCycle);
    ++CurrCycle;
    LLVM_DEBUG(dbgs() << "  Bump to CurrCycle=" << CurrCycle << "\n");
  };

  // Push the CurrentBundle
  BumpCycle(CurrBundle);
  CurrBundle.clear();

  // Push empty bundles until making ToCycle the current cycle.
  const AIE::MachineBundle EmptyBundle(CurrBundle.FormatInterface);
  while (ToCycle != CurrCycle) {
    BumpCycle(EmptyBundle);
  }
}

} // namespace

std::vector<AIE::MachineBundle>
llvm::AIE::computeAndFinalizeBundles(SchedBoundary &Zone) {
  LLVM_DEBUG(dbgs() << "Computing Bundles for Zone "
                    << (Zone.isTop() ? "Top\n" : "Bot\n"));
  const ScheduleDAGMI &DAG = *Zone.DAG;
  bool ComputeSlots = !DAG.hasVRegLiveness();
  std::vector<AIE::MachineBundle> Bundles;
  AIE::MachineBundle CurrBundle(getTII(DAG)->getFormatInterface());

  auto AddInBundles = [&](auto Range, const AIEHazardRecognizer &HazardRec) {
    // Iterate over all instructions between Begin and End to create
    // the sequence of MachineBundles.
    for (MachineInstr &MI : Range) {
      SUnit *SU = DAG.getSUnit(&MI);
      if (!SU)
        continue;
      unsigned EmitCycle = Zone.isTop() ? SU->TopReadyCycle : SU->BotReadyCycle;

      if (!ComputeSlots && EmitCycle < Bundles.size()) {
        // The pre-RA scheduler can actually re-order copies and immediate
        // moves, disregarding the emission cycle.
        // See GenericScheduler::reschedulePhysReg().
        EmitCycle = Bundles.size();
      }

      if (EmitCycle != Bundles.size())
        bumpCycleForBundles(EmitCycle, Bundles, CurrBundle);

      for (MachineInstr &BundledMI : bundled_instrs(MI, /*IncludeRoot=*/true)) {
        LLVM_DEBUG(dbgs() << "  Add to CurrBundle: " << BundledMI);
        CurrBundle.add(&BundledMI,
                       HazardRec.getSelectedAltDescs().getOpcode(&BundledMI),
                       ComputeSlots);
      }
    }
  };

  if (Zone.isTop())
    AddInBundles(make_range(DAG.begin(), DAG.top()),
                 *getAIEHazardRecognizer(Zone));
  else
    AddInBundles(reverse(make_range(DAG.bottom(), DAG.end())),
                 *getAIEHazardRecognizer(Zone));

  // Flush any non-empty CurrBundle
  if (!CurrBundle.empty()) {
    bumpCycleForBundles(Bundles.size() + 1, Bundles, CurrBundle);
    LLVM_DEBUG(dbgs() << "  Finalized Bundle. NumBundles=" << Bundles.size()
                      << "\n");
  }

  // Make sure the zone's cycle is greater or equal to the number of Bundles
  // In particular for Bot, instructions can be emitted in cycles greater than
  // CurrCycle.
  if (Bundles.size() > Zone.getCurrCycle()) {
    Zone.bumpCycle(Bundles.size());
    LLVM_DEBUG(dbgs() << "  Updated zone CurrCycle=" << Zone.getCurrCycle()
                      << "\n");
  }

  // Push NOP Bundles until reaching Zone's current cycle.
  // In particular for Top, the CurrCycle might have been bumped beyond the
  // emission cycle of the last bundle.
  if (Zone.getCurrCycle() != Bundles.size())
    bumpCycleForBundles(Zone.getCurrCycle(), Bundles, CurrBundle);

  // Re-order bundles and instructions within so they appear in the same order
  // as in their parent basic block. This canonical order is required by
  // applyBundles, and facilitates generic NOP insertion.
  if (!Zone.isTop()) {
    std::reverse(Bundles.begin(), Bundles.end());
    for (AIE::MachineBundle &Bundle : Bundles) {
      std::reverse(Bundle.Instrs.begin(), Bundle.Instrs.end());
    }
  }
  return Bundles;
}

namespace {
/// Search for instructions that might jump to an unknown target block
bool hasUnknownSuccessors(
    llvm::iterator_range<MachineBasicBlock::iterator> Region,
    MachineBasicBlock *MBB) {
  if (MBB->succ_empty()) {
    // This includes pure return blocks, i.e. blocks that unconditionally
    // fall in a return.  ATM, we don't have true control sinks like HALT
    // which could be special-cased to return false. Probably not worth it.
    return true;
  }
  const AIEBaseInstrInfo *TII = getTII(MBB);
  for (auto &Flow : reverse(Region)) {
    if (TII->jumpsToUnknown(Flow.getOpcode())) {
      return true;
    }
  }
  return false;
}
} // namespace

bool AIEPostRASchedStrategy::successorsAreScheduled(
    MachineBasicBlock *MBB) const {
  return !hasUnknownSuccessors(llvm::make_range(RegionBegin, RegionEnd), MBB) &&
         llvm::all_of(MBB->successors(), [&](MachineBasicBlock *B) {
           const auto &BS = InterBlock.getBlockState(B);
           return BS.isScheduled();
         });
}

void AIEPostRASchedStrategy::initializeBotScoreBoard(ScoreboardTrust Trust) {

  if (!InterBlockScoreboard) {
    DEBUG_BLOCKS(dbgs() << "Interblock scoreboard explicitly disabled\n");
    return;
  }

  DEBUG_BLOCKS(dbgs() << "Compute bottom scoreboard of MBB "
                      << CurMBB->getNumber() << "\n");

  // Check that we have forced bottom up regions. This makes sure that
  // getTop() used below behaves sanely.
  assert(!doMBBSchedRegionsTopDown());
  AIEHazardRecognizer *BotHazardRec = getAIEHazardRecognizer(Bot);
  const int Depth = BotHazardRec->getMaxLookAhead();
  assert(unsigned(Depth) >= BotHazardRec->getPipelineDepth());

  /// These lambdas are an abstraction of the scoreboard manipulations,
  /// hiding the details of the implementation. In particular, we need to
  /// make sure we always have enough lookahead available. We arrange for that
  /// by starting in the earliest possible cycle, -Depth
  auto InsertInCycle = [=](MachineInstr &MI, int Cycle) {
    BotHazardRec->emitInScoreboard(MI, MI.getDesc(), Cycle - Depth);
  };
  auto BlockCycle = [=](int Cycle) {
    BotHazardRec->blockCycleInScoreboard(Cycle - Depth);
  };

  /// Do the final alignment of the scoreboard to the position where we
  /// want it. We started it at -Depth representing Cycle 0. The scoreboard
  /// should have CurrentCycle representing the last cycle of the current
  /// block/region so we have to shift it to be in Cycle 1.
  auto AlignScoreboardToCycleOne = [=]() {
    BotHazardRec->recedeScoreboard(Depth + 1);
  };

  // This tracks unknown cycles resulting from blocks that are too short.
  // The conservative estimate is to declare all cycles unknown.
  // The conservative case includes the fixpoint iteration on a loop. For that
  // case we are not actually conservative; we assume a number of empty cycles
  // in the scoreboard given by the fixpoint parameters.
  int FirstBlockedCycle = 0;
  if (Trust != ScoreboardTrust::Conservative) {
    // The pipeline depth is a suitable supremum for the minimum we compute.
    // Note that if the loop isn't entered (we have no successors), the
    // responsibilty lies with our caller not setting Conservative.
    // This may be legitimate to represent a 'done' or 'flush_pipeline'
    // instruction in future
    FirstBlockedCycle = BotHazardRec->getPipelineDepth();
    for (llvm::MachineBasicBlock *SuccMBB : CurMBB->successors()) {
      // Replay bundles into scoreboard.
      DEBUG_BLOCKS(dbgs() << " SuccBB " << SuccMBB->getNumber() << "\n");
      auto &SBS = InterBlock.getBlockState(SuccMBB);
      if (SBS.getRegions().empty()) {
        DEBUG_BLOCKS(dbgs() << " Empty Successor\n");
        FirstBlockedCycle = 0;
        continue;
      }
      DEBUG_BLOCKS(dbgs() << " Replay bundles\n");
      int Cycle = 0;
      const std::vector<MachineBundle> &TopBundles = SBS.getTop().Bundles;
      for (auto &Bundle : TopBundles) {
        // There's only so much future we need.
        if (Cycle >= FirstBlockedCycle) {
          break;
        }
        DEBUG_BLOCKS(dbgs() << "Cycle " << Cycle << " has "
                            << Bundle.getInstrs().size() << " instrs\n");
        for (MachineInstr *MI : Bundle.getInstrs()) {
          InsertInCycle(*MI, Cycle);
          if (Trust == ScoreboardTrust::AccountForAlign && Cycle + 1 < Depth) {
            InsertInCycle(*MI, Cycle + 1);
          }
        }
        Cycle++;
      }
      FirstBlockedCycle = std::min(FirstBlockedCycle, Cycle);
    }
  }

  auto Cap = InterBlock.getBlockedResourceCap(CurMBB);
  if (Cap && IsBottomRegion) {
    int Margin = BotHazardRec->getPipelineDepth() - *Cap;
    FirstBlockedCycle = std::max(FirstBlockedCycle, Margin);
    DEBUG_BLOCKS(dbgs() << "FirstBlockedCycle = " << Margin << "\n");
  }

  // We have to assume the worst for any cycle we haven't completely seen
  for (int Cycle = FirstBlockedCycle; Cycle < Depth; Cycle++) {
    BlockCycle(Cycle);
  }

  AlignScoreboardToCycleOne();
  DEBUG_BLOCKS(BotHazardRec->dumpScoreboard());
}

void AIEPostRASchedStrategy::initializeTopScoreBoard() {

  // Top Scoreboard is only used for Post-SWP epilogue merging.
  // If we have a multiregion MBB, the epilogue will be scheduled
  // in the first topological region. In this case, ignore scoreboard
  // initialization for other regions.
  const bool IsFirstRegion = DAG->begin() == CurMBB->begin();
  if (!IsFirstRegion)
    return;

  auto EpilogueContextOpt = InterBlock.getSWPEpilogueContext(CurMBB);

  if (!EpilogueContextOpt)
    return;

  AIEHazardRecognizer *TopHazardRec = getAIEHazardRecognizer(Top);
  auto EmitInstr = [=](MachineInstr &MI) {
    TopHazardRec->emitInScoreboard(MI, MI.getDesc(), 0);
  };

  const unsigned ConflictHorizon = TopHazardRec->getConflictHorizon();
  ArrayRef<MachineBundle> LoopBundles = EpilogueContextOpt->Loop;
  const unsigned LoopSize = LoopBundles.size();

  // ceil(LoopSize / ConflictHorizon)
  const int LoopReplayTimes =
      (LoopSize + ConflictHorizon - 1) / ConflictHorizon;

  // Replay SWP loop enough times (right before the epilogue) until the
  // scoreboard reaches a steady state.
  for (int I = 0; I < LoopReplayTimes; I++) {
    for (auto &Bundle : LoopBundles) {
      for (MachineInstr *MI : Bundle.getInstrs()) {
        EmitInstr(*MI);
      }
      TopHazardRec->AdvanceCycle();
    }
  }

  DEBUG_BLOCKS(TopHazardRec->dumpScoreboard());
}

static MachineInstr *getDelaySlotInstr(MachineBasicBlock::iterator RegionBegin,
                                       MachineBasicBlock::iterator RegionEnd) {
  auto HasDelaySlot = [](const MachineInstr &MI) { return MI.hasDelaySlot(); };
  auto It = std::find_if(RegionBegin, RegionEnd, HasDelaySlot);
  if (It == RegionEnd)
    return nullptr;
  assert(std::find_if(std::next(It), RegionEnd, HasDelaySlot) == RegionEnd &&
         "Region has multiple delay slots.");
  return &(*It);
}

void AIEPostRASchedStrategy::initialize(ScheduleDAGMI *Dag) {
  PostGenericScheduler::initialize(Dag);
  assert(PostRADirection == MISched::Direction::Unspecified);

  // Update Bot scoreboard of the bottom region with the foreseeable future
  // as found in the top regions of the successor blocks. If we don't know,
  // assume the worst.
  const bool Conservative = !(IsBottomRegion && successorsAreScheduled(CurMBB));
  const ScoreboardTrust NonConservative = InterBlockAlignment
                                              ? ScoreboardTrust::AccountForAlign
                                              : ScoreboardTrust::Absolute;
  initializeBotScoreBoard(Conservative ? ScoreboardTrust::Conservative
                                       : NonConservative);
  initializeTopScoreBoard();

  // Compute RegionTopDownCycles first so the delay slot logic can inspect it.
  const Region &Reg = InterBlock.getBlockState(CurMBB).getCurrentRegion();
  RegionTopDownCycles = Reg.getTopFixedBundles().size();

  // Delay slots are normally scheduled bottom-up so the control-flow
  // instruction is issued exactly TII->getNumDelaySlots() before the end of
  // the region.  However, when top-fixed bundles are present and
  // -aie-delay-slot-topdown is enabled, we schedule the whole region top-down
  // and rely on fixupDelaySlotPosition() (called from leaveRegion) to move
  // the branch to the correct position afterwards.
  unsigned DelaySlotCycles = 0;
  PersistentTopDown = false;
  if (MachineInstr *MI = getDelaySlotInstr(RegionBegin, RegionEnd)) {
    auto *TII = getTII(CurMBB);
    assert(RegionEnd != MI->getParent()->instr_end() &&
           TII->isDelayedSchedBarrier(*RegionEnd));
    unsigned Reserved = std::max(ReservedDelaySlots.getValue(),
                                 TII->getNumReservedDelaySlots(*MI));
    getAIEHazardRecognizer(Bot)->setReservedCycles(Reserved);

    if (RegionTopDownCycles > 0 && EnableDelaySlotTopDown &&
        Reg.getBotFixedBundles().empty()) {
      // Top-fixed bundles present, no bot-fixed bundles, and the option is
      // enabled: schedule fully top-down and fix up the branch position in
      // leaveRegion(). Bot-fixed bundles are incompatible with this path
      // because fixupDelaySlotPosition requires BotBundles to be empty.
      PersistentTopDown = true;
    } else {
      // Normal case: force enough bottom-up cycles to place the branch at the
      // correct distance from the end of the region.
      DelaySlotCycles = TII->getNumDelaySlots(*MI) + 1;
    }
  }

  RegionBottomUpCycles = std::max(BottomUpCycles.getValue(), DelaySlotCycles);
  // Start with top-down when we have TopInsert bundles.
  IsTopDown = (RegionBottomUpCycles == 0) || (RegionTopDownCycles > 0);
  if (!IsTopDown) {
    LLVM_DEBUG(dbgs() << "*** Using bottom-up scheduling for the region ***\n");
  } else {
    LLVM_DEBUG(dbgs() << "*** Using top-down scheduling for the region ***\n");
  }
}

/// Compute the minimum cycle for Zone in which one can ever find
/// an instruction to schedule.
static unsigned getMinSchedulableCycle(SchedBoundary &Zone) {
  unsigned MinSchedulableCycle = std::numeric_limits<unsigned>::max();
  auto SchedCycle = [&Zone](const SUnit *SU) -> unsigned {
    if (Zone.isTop())
      return SU->TopReadyCycle;
    // For bottom-up, dependent instructions can actually be scheduled in a
    // cycle smaller than SU->BotReadyCycle due to negative latencies.
    // Instead, compute the minimum cycle in which a dependent can be emitted.
    // Use a static lower bound to avoid traversing the predecessor tree.
    int EarliestPredSchedCycle =
        int(SU->BotReadyCycle) + NegativeLatencyLowerBound;
    return std::max(EarliestPredSchedCycle, 0);
  };
  for (const SUnit *SU : Zone.Available) {
    MinSchedulableCycle = std::min(MinSchedulableCycle, SchedCycle(SU));
  }
  for (const SUnit *SU : Zone.Pending) {
    MinSchedulableCycle = std::min(MinSchedulableCycle, SchedCycle(SU));
  }
  assert(MinSchedulableCycle != std::numeric_limits<unsigned>::max());
  return MinSchedulableCycle;
}

bool AIEPostRASchedStrategy::doesNotProgressInZone(const SchedBoundary &Zone,
                                                   const SUnit &SU) const {
  // If SU is a fixed instruction in the other zone, it isn't available.
  if (isFixedSU(SU, !Zone.isTop()))
    return true;

  // We cannot proceed with delay slot instructions in the top zone, unless we
  // are using the post-scheduling fixup path (PersistentTopDown).  In that
  // case the branch is allowed to be scheduled anywhere and its position will
  // be corrected in leaveRegion().
  return Zone.isTop() && SU.getInstr()->hasDelaySlot() && !PersistentTopDown;
}

// This function returns true when it is impossible to continue with top-down
// without entering an infinite loop because the only remaining instructions
// cannot be scheduled in the top zone.
bool AIEPostRASchedStrategy::mustSwitchToBottomUp() {
  assert(IsTopDown);
  SchedBoundary &Zone = getSchedZone();
  ReadyQueue &PQ = Zone.Pending;
  ReadyQueue &AQ = Zone.Available;

  // We must switch when we have an empty AQ and instructions that cannot
  // progress in the PQ.
  if (AQ.size() == 0 && all_of(PQ, [&](const SUnit *SU) {
        return doesNotProgressInZone(Zone, *SU);
      }))
    return true;

  // Also switch when the AQ is empty and all progressable pending instructions
  // would only become ready at a cycle >= RegionTopDownCycles (i.e., beyond
  // the top-fixed bundle region). Scheduling them top-down would waste cycles
  // and push anti-dependent instructions into the bottom zone as standalone
  // instructions, degrading bundle packing.
  if (RegionTopDownCycles && AQ.size() == 0) {
    for (const SUnit *SU : PQ) {
      if (!doesNotProgressInZone(Zone, *SU) &&
          SU->TopReadyCycle < RegionTopDownCycles)
        return false;
    }
    return true;
  }

  return false;
}

SUnit *AIEPostRASchedStrategy::pickNodeAndCycle(
    bool &IsTopNode, std::optional<unsigned> &EmissionCycle) {
  LLVM_DEBUG(dbgs() << "** AIEPostRASchedStrategy::pickNode TopCycle="
                    << Top.getCurrCycle() << " BotCycle=" << Bot.getCurrCycle()
                    << "\n");
  if (!IsTopDown && Bot.getCurrCycle() >= RegionBottomUpCycles) {
    // Note that there is no guarantee we can issue an available instruction
    // in the current cycle. In case of hazards, PostGenericScheduler::pickNode
    // will bump the cycle until it finds a schedulable instruction. As a
    // consequence, the picked instruction can issue in a cycle greater than
    // RegionBottomUpCycles.
    LLVM_DEBUG(dbgs() << "*** Switching to top-down ***\n");
    IsTopDown = true;
  } else if (IsTopDown && RegionTopDownCycles && !PersistentTopDown &&
             (Top.getCurrCycle() >= RegionTopDownCycles ||
              mustSwitchToBottomUp())) {
    // We have scheduled all top-fixed instructions, filling as many slots as
    // possible. Now it is time to proceed with the bottom-up approach.
    // Note: when PersistentTopDown is true (top-fixed bundles + delay slot),
    // we stay top-down for the entire region and fix up the branch position
    // in leaveRegion().
    LLVM_DEBUG(dbgs() << "*** Switching to bottom-up ***\n");
    IsTopDown = false;
  }

  SchedBoundary &Zone = getSchedZone();
  if (DAG->top() == DAG->bottom()) {
    assert(Zone.Available.empty() && Zone.Pending.empty() && "ReadyQ garbage");
    return nullptr;
  }

  // Bump the cycle as much as possible to ensure the window of DeltaCycles is
  // as big as possible.
  if (unsigned MinSchedCycle = getMinSchedulableCycle(Zone);
      Zone.getCurrCycle() < MinSchedCycle) {
    Zone.bumpCycle(MinSchedCycle);
  }

  SUnit *SU;
  do {
    SU = pickNodeUnidirectional(Zone);
  } while (SU->isScheduled);

  IsTopNode = Zone.isTop();

  if (SU->isTopReady())
    Top.removeReady(SU);
  if (SU->isBottomReady())
    Bot.removeReady(SU);

  // For bottom-up scheduling, we might have picked an instruction to be
  // scheduled in a cycle greater than CurrCycle. Alternatively, for top-down
  // scheduling, we might have picked an instruction to be scheduled in a cycle
  // lesser than CurrCycle. See isAvailableNode(). Make sure to set the
  // EmissionCycle right.
  if (IsTopNode) {
    assert(SU->TopReadyCycle <= Zone.getCurrCycle());
    EmissionCycle = SU->TopReadyCycle;
  } else {
    assert(SU->BotReadyCycle >= Zone.getCurrCycle());
    EmissionCycle = SU->BotReadyCycle;
  }

  LLVM_DEBUG(dbgs() << "Scheduling SU(" << SU->NodeNum << ") "
                    << *SU->getInstr());
  return SU;
}

int AIEPostRASchedStrategy::getMaxDeltaCycles(const SchedBoundary &Zone) const {
  // DeltaCycles for top-down are handled directly in isAvailableNode via the
  // TopReadyCycle loop; this function only governs the bottom-up delta window.
  if (Zone.isTop() || Zone.getCurrCycle() >= RegionBottomUpCycles - 1)
    return 0;
  return std::min({int(RegionBottomUpCycles - 1 - Zone.getCurrCycle()),
                   int(getAIEHazardRecognizer(Zone)->getMaxLookAhead()),
                   BottomUpDelta.getValue()});
}

/// Returns the number of emitted instructions in the Top or Bot zone.
unsigned getNumEmittedInstrs(ScheduleDAGMI *DAG, bool IsTop) {
  if (IsTop)
    return DAG->top().isValid() ? std::distance(DAG->begin(), DAG->top()) : 0;
  return DAG->bottom().isValid() ? std::distance(DAG->bottom(), DAG->end()) : 0;
}

SUnit *AIEPostRASchedStrategy::getNextUnscheduledFixedInstr(
    const SchedBoundary &Zone) const {
  const Region &CurRegion = InterBlock.getBlockState(CurMBB).getCurrentRegion();
  const unsigned NumEmitted = getNumEmittedInstrs(DAG, Zone.isTop());
  // If the zone still has unscheduled fixed instructions, the next one to
  // pick is (DAG->bottom() - 1) for bottom-up, or DAG->top() for top-down.
  if (Zone.isTop()) {
    if (NumEmitted < CurRegion.getTopFixedBundles().size()) {
      MachineInstr &NextMI =
          *(DAG->top().isValid() ? DAG->top() : DAG->begin());
      SUnit *NextSU = DAG->getSUnit(&NextMI);
      assert(NextSU);
      assert(NextSU->TopReadyCycle == NextSU->getDepth() &&
             "Fixed instruction won't be placed at the correct cycle");
      assert(Zone.getCurrCycle() <= NextSU->TopReadyCycle);
      return NextSU;
    }
  } else {
    if (NumEmitted < CurRegion.getBotFixedBundles().size()) {
      MachineInstr &NextMI =
          *std::prev(DAG->bottom().isValid() ? DAG->bottom() : DAG->end());
      SUnit *NextSU = DAG->getSUnit(&NextMI);
      assert(NextSU);
      assert(NextSU->BotReadyCycle == NextSU->getHeight() &&
             "Fixed instruction won't be placed at the correct cycle");
      assert(Zone.getCurrCycle() <= NextSU->BotReadyCycle);
      return NextSU;
    }
  }

  return nullptr;
}

bool AIEPostRASchedStrategy::isFixedSU(const SUnit &SU, bool IsTop) const {
  if (IsTop) {
    return FirstTopFixedSU && SU.NodeNum >= *FirstTopFixedSU &&
           SU.NodeNum < FirstBotFixedSU.value_or(DAG->SUnits.size());
  }
  return FirstBotFixedSU && SU.NodeNum >= *FirstBotFixedSU &&
         SU.NodeNum <= LastBotFixedSU.value();
}

bool AIEPostRASchedStrategy::isFreeSU(const SUnit &SU) const {
  const unsigned NumUpperBound = DAG->SUnits.size();
  return SU.NodeNum < FirstTopFixedSU.value_or(NumUpperBound) &&
         SU.NodeNum < FirstBotFixedSU.value_or(NumUpperBound);
}

bool AIEPostRASchedStrategy::isAvailableNode(SUnit &SU, SchedBoundary &Zone,
                                             bool /*VerifyReadyCycle*/) {
  // Note we use signed integers to avoid wrap-around behavior.
  const int MinDelta = -getMaxDeltaCycles(Zone);
  const int BotReadyCycle = std::max(Zone.getCurrCycle(), SU.BotReadyCycle);
  const int TopReadyCycle = SU.TopReadyCycle;
  const int CurrCycle = Zone.getCurrCycle();

  // If the Zone has remaining fixed instructions, only one SU is available.
  if (SUnit *FixedSU = getNextUnscheduledFixedInstr(Zone)) {
    if (FixedSU != &SU)
      return false;
    if (Zone.isTop()) {
      return CurrCycle == TopReadyCycle;
    }
    const int DeltaCycles = CurrCycle - BotReadyCycle;
    return DeltaCycles >= MinDelta;
  }

  if (doesNotProgressInZone(Zone, SU))
    return false;

  // Whether or not the zone is Top or Bot, verify if SU is ready to be
  // scheduled in terms of cycle.
  if (Zone.isTop()) {
    // This SU should be scheduled after CurrCycle.
    if (TopReadyCycle > CurrCycle)
      return false;
    // Clamp the search start to the scoreboard's valid backward range.
    // On large basic blocks TopReadyCycle can be much smaller than
    // CurrCycle - MaxLookAhead. Cycles before the scoreboard window are
    // guaranteed empty: resources are always inserted forward in time, so
    // once the scoreboard has advanced past a cycle its entries have been
    // cleared. Attempting to access those out-of-window cycles would
    // assert in checkConflict/enterResources. When no slot is found within
    // the valid window the callers' bump mechanism rotates the scoreboard
    // forward to open fresh slots, mirroring the !isTop path.
    const int MaxLookAhead =
        int(getAIEHazardRecognizer(Zone)->getMaxLookAhead());
    const int Start = std::max(TopReadyCycle - CurrCycle, -MaxLookAhead);
    for (int DeltaCycles = Start; DeltaCycles <= 0; ++DeltaCycles) {
      // TopReadyCycle is always less or equal to the current cycle here,
      // (if not, we could violate dependencies) so DeltaCycles will
      // always be less or equal to 0.
      if (Zone.checkHazard(&SU, DeltaCycles))
        continue;
      SU.TopReadyCycle = CurrCycle + DeltaCycles;
      return true;
    }
    // No slot found in the scoreboard window. The callers' bump loop will
    // advance the scoreboard by one cycle so the next call sees a fresh slot.
    SU.TopReadyCycle = CurrCycle + 1;
    return false;
  }

  for (int DeltaCycles = CurrCycle - BotReadyCycle; DeltaCycles >= MinDelta;
       --DeltaCycles) {
    // ReadyCycle is always greater or equal to the current cycle,
    // so DeltaCycles will always be less or equal to 0.
    if (Zone.checkHazard(&SU, DeltaCycles))
      continue;
    SU.BotReadyCycle = CurrCycle - DeltaCycles;
    return true;
  }

  // Didn't find a cycle in which to emit SU, move it to the Pending queue.
  // Still, update BotReadyCycle so next calls to isAvailableNode are quicker
  SU.BotReadyCycle = std::max(BotReadyCycle, CurrCycle - MinDelta);
  return false;
}

/// Called after ScheduleDAGMI has scheduled an instruction and updated
/// scheduled/remaining flags in the DAG nodes.
void AIEPostRASchedStrategy::schedNode(SUnit *SU, bool IsTopNode) {
  if (IsTopNode) {
    const int DeltaCycles = int(SU->TopReadyCycle) - int(Top.getCurrCycle());
    assert(DeltaCycles <= 0);
    Top.bumpNode(SU, DeltaCycles);
  } else {
    int DeltaCycles = int(Bot.getCurrCycle()) - int(SU->BotReadyCycle);
    assert(DeltaCycles <= 0);
    Bot.bumpNode(SU, DeltaCycles);
  }
}

static void removeMetaInstructions(MachineFunction &MF) {
  // Remove KILL and IMPLICIT_DEF pseudo-instructions before scheduling. These
  // meta instructions confuse the dependence analysis: their defs absorb the
  // entries in the Defs/Uses maps inside buildEdges, breaking direct dependency
  // edges between real instructions. For example:
  //   $r0 = MOV ...          (Insn A)
  //   $r0 = KILL $r0         (Insn B)
  //   $r0 = ADD ...          (Insn C)
  // Insn B causes buildEdges to create A->B->C instead of a direct A->C
  // output dependence. Since these are zero-latency meta instructions that
  // emit no code, the transitive chain allows A and C to be placed in the
  // same VLIW bundle, which is incorrect. IMPLICIT_DEF has the same problem
  // when its def sits between two real writers of the same register.
  // Removing KILL before VLIW scheduling is the same approach used by the
  // Hexagon packetizer (HexagonVLIWPacketizer.cpp).
  for (MachineBasicBlock &MBB : MF)
    for (MachineInstr &MI : llvm::make_early_inc_range(MBB))
      if (MI.isKill() || MI.isImplicitDef())
        MI.eraseFromParent();
}

void AIEPostRASchedStrategy::enterFunction(MachineFunction *MF) {
  LLVM_DEBUG(dbgs() << "enterFunction " << MF->getName() << "\n");
  removeMetaInstructions(*MF);
  InterBlock.enterFunction(MF);
}

void AIEPostRASchedStrategy::leaveFunction() { InterBlock.leaveFunction(); }

void AIEPostRASchedStrategy::enterMBB(MachineBasicBlock *MBB) {
  InterBlock.enterBlock(MBB);
  CurMBB = MBB;
  // We force bottom up region processing, so the first region
  // from a block is the bottom one. We reset this when leaving any
  // region.
  IsBottomRegion = true;
}

static MachineBasicBlock::iterator
materializeEmptyBundles(std::vector<MachineBundle> Bundles,
                        MachineBasicBlock *BB, MachineBasicBlock::iterator It,
                        const TargetInstrInfo *TII) {
  for (const AIE::MachineBundle &Bundle : Bundles) {
    if (Bundle.empty()) {
      // Empty bundle means 1-cycle stall. Insert NOP before position It.
      // Note: insertNoop inserts *before* It, so consecutive empty bundles
      // naturally create sequential NOPs without needing to advance It.
      TII->insertNoop(*BB, It);
      continue;
    }
    // Advance iterator to the position after this bundle's last
    // instruction, ready for the next bundle or region.
    It = getBundleEnd(Bundle.getInstrs().back()->getIterator());
  }
  return It;
}

void AIEPostRASchedStrategy::commitBlockSchedule(MachineBasicBlock *BB) {
  auto &BS = InterBlock.getBlockState(BB);

  assert(BS.getRegions().empty() ||
         BS.TopInsert.size() == BS.getTop().getTopFixedBundles().size());
  assert(BS.BottomInsert.empty() ||
         BS.BottomInsert.size() == BS.getBottom().getBotFixedBundles().size());

  // Safety margin that is applied to non-pipelined loops.
  // Note that the swp epilogue and prologue are handled in a different way.
  // See enterMBB.
  InterBlock.emitTopSafetyMargin(BS);

  if (BS.isPipelined()) {
    assert(BS.getRegions().size() == 1);
    MachineBasicBlock::iterator It = BB->getFirstTerminator();
    InterBlock.emitBundles(BS.getRegions().front().Bundles, BB, It,
                           /*Move=*/true, /*EmitNops=*/true);
  } else {
    // Emit bundles for each region in the block. Regions are stored in the
    // order they were scheduled (bottom-up), so we must reverse them to emit
    // NOPs in the correct region.
    MachineBasicBlock::iterator It = BB->begin();
    const TargetInstrInfo *TII = getTII(BB);
    // Reverse iteration: regions were added bottom-up (last region first),
    // but must be emitted top-down (first region first) to match MIR order.
    for (auto &Region : reverse(BS.getRegions())) {
      // Contrary to PRAS, the MachineScheduler does not automatically insert
      // NOPs. That isn't a problem, since the callbacks to the
      // HazardRecognizer were a bit flaky (e.g. when to call emitNoop vs
      // advanceCycle). MachineScheduler just calls advanceCycle, and this is
      // enough for us to insert NOPs because the sequence of Bundles gives us
      // the full picture.
      It = materializeEmptyBundles(Region.Bundles, BB, It, TII);

      AIEHazardRecognizer::applyBundles(Region.Bundles, BS.TheBlock);
    }
  }
}

void AIEPostRASchedStrategy::leaveMBB() {
  if (InterBlock.leaveBlock()) {
    // Finish it off and move to the next block.
    commitBlockSchedule(CurMBB);
  }
  CurMBB = nullptr;
}

MachineBasicBlock *AIEPostRASchedStrategy::nextBlock() {
  return InterBlock.nextBlock();
}

void AIEPostRASchedStrategy::enterRegion(MachineBasicBlock *BB,
                                         MachineBasicBlock::iterator Begin,
                                         MachineBasicBlock::iterator End,
                                         unsigned RegionInstrs) {
  InterBlock.enterRegion(BB, Begin, End);
  RegionBegin = Begin;
  RegionEnd = End;
}

/// Return the index of the first bundle in \p Bundles that contains \p MI,
/// or -1 if not found.
static int findInBundles(ArrayRef<AIE::MachineBundle> Bundles,
                         const MachineInstr *MI) {
  auto It = llvm::find_if(Bundles, [MI](const AIE::MachineBundle &B) {
    return llvm::is_contained(B.getInstrs(), MI);
  });
  assert(It != Bundles.end() && "MI not found in any bundle");
  return static_cast<unsigned>(std::distance(Bundles.begin(), It));
}

/// Return the MBB iterator at which \p BranchMI should be spliced: just after
/// the last non-BranchMI instruction in bundles[0..\p PlacedIdx], searching
/// backward from \p PlacedIdx.
static MachineBasicBlock::iterator
computeSplicePoint(ArrayRef<AIE::MachineBundle> Bundles, unsigned PlacedIdx,
                   MachineInstr *BranchMI, MachineBasicBlock *MBB) {
  for (int I = static_cast<int>(PlacedIdx); I >= 0; --I) {
    const auto &Instrs = Bundles[I].getInstrs();
    auto It =
        llvm::find_if(llvm::reverse(Instrs),
                      [BranchMI](MachineInstr *MI) { return MI != BranchMI; });
    if (It != Instrs.rend())
      return getBundleEnd((*It)->getIterator());
  }
  return MBB->end();
}

/// Remove \p MI from \p Bundle, keeping Instrs, SlotMap and OccupiedSlots
/// in sync.
static void removeFromBundle(AIE::MachineBundle &Bundle, MachineInstr *MI) {
  auto &Instrs = Bundle.Instrs;
  Instrs.erase(std::remove(Instrs.begin(), Instrs.end(), MI), Instrs.end());
  // Remove from SlotMap and update OccupiedSlots.
  auto MapIt = llvm::find_if(Bundle.SlotMap,
                             [MI](const auto &P) { return P.second == MI; });
  assert(MapIt != Bundle.SlotMap.end() && "MI not found in bundle SlotMap");
  const auto *SlotInfo = Bundle.FormatInterface->getSlotInfo(MapIt->first);
  assert(SlotInfo && "No SlotInfo for slot containing MI");
  Bundle.OccupiedSlots &= ~SlotInfo->getSlotSet();
  Bundle.SlotMap.erase(MapIt);
}

void AIEPostRASchedStrategy::fixupDelaySlotPosition(
    std::vector<AIE::MachineBundle> &TopBundles,
    std::vector<AIE::MachineBundle> &BotBundles, MachineInstr *BranchMI,
    unsigned NumDelaySlots) {

  // Only reached when PersistentTopDown is true: fully top-down region,
  // no bot-fixed bundles. All NOPs go into TopBundles, keeping
  // Top.getCurrCycle() in sync.
  assert(BotBundles.empty() &&
         "fixupDelaySlotPosition: BotBundles must be empty on entry");

  const AIEBaseMCFormats *FmtIface = getTII(CurMBB)->getFormatInterface();
  AIEHazardRecognizer *TopHR = getAIEHazardRecognizer(Top);
  // Appends one empty NOP to TopBundles and advances Top's scoreboard.
  auto AppendNop = [&]() {
    Top.bumpCycle(Top.getCurrCycle() + 1);
    TopBundles.emplace_back(FmtIface);
  };

  const int BranchIdx = findInBundles(TopBundles, BranchMI);

  // May increase as NOPs are appended or before the re-placement runs.
  unsigned BundlesAfterBranch =
      TopBundles.size() - static_cast<unsigned>(BranchIdx) - 1;

  LLVM_DEBUG({
    dbgs() << "fixupDelaySlotPosition: BranchIdx=" << BranchIdx
           << " BundlesAfterBranch=" << BundlesAfterBranch
           << " NumDelaySlots=" << NumDelaySlots << "\n";
    for (unsigned I = 0; I < TopBundles.size(); ++I) {
      dbgs() << "  Bundle[" << I << "]:";
      for (MachineInstr *MI : TopBundles[I].getInstrs())
        dbgs() << " "
               << MI->getMF()->getSubtarget().getInstrInfo()->getName(
                      MI->getOpcode());
      dbgs() << "\n";
    }
  });

  // Append NOPs until exactly NumDelaySlots bundles follow the branch.
  // If the inter-zone scoreboard is also clean afterwards, we are done.
  // Otherwise fall through to resolve the conflict by moving the branch
  // forward.
  while (BundlesAfterBranch < NumDelaySlots) {
    LLVM_DEBUG(dbgs() << "fixupDelaySlotPosition: appending empty bundle\n");
    AppendNop();
    BundlesAfterBranch++;
  }

  if (BundlesAfterBranch == NumDelaySlots &&
      !checkInterZoneConflicts(BotBundles)) {
    LLVM_DEBUG(dbgs() << "fixupDelaySlotPosition: done, position is correct "
                         "and scoreboard are aligned.\n");
    return;
  }

  // Branch too early (or inter-zone conflict after NOP padding) – extract it
  // and re-place at a conflict-free slot. Each conflict appends a NOP
  // (advancing Top's scoreboard), maintaining exactly NumDelaySlots bundles
  // after the final placement.
  const unsigned MoveDown = BundlesAfterBranch - NumDelaySlots;
  const unsigned TargetIdx = static_cast<unsigned>(BranchIdx) + MoveDown;

  LLVM_DEBUG(dbgs() << "fixupDelaySlotPosition: extracting branch from bundle "
                    << BranchIdx << " and placing at/after bundle " << TargetIdx
                    << "\n");

  // Remove BranchMI from its current bundle.
  removeFromBundle(TopBundles[BranchIdx], BranchMI);

  // Scan forward from TargetIdx. Delta = -(NumDelaySlots + 1) is constant:
  // each AppendNop advances the Top scoreboard ring by one slot, so successive
  // checks probe cycles TargetIdx, TargetIdx+1, ... The branch never conflicts
  // with its own old booking at a later cycle.
  const int Delta = -(static_cast<int>(NumDelaySlots) + 1);
  unsigned PlacedIdx = TargetIdx;
  while (TopHR->checkConflict(*BranchMI, Delta) ||
         checkInterZoneConflicts(BotBundles)) {
    LLVM_DEBUG(dbgs() << "fixupDelaySlotPosition: conflict at bundle "
                      << PlacedIdx << ", appending empty bundle\n");
    AppendNop();
    ++PlacedIdx;
  }

  LLVM_DEBUG(dbgs() << "fixupDelaySlotPosition: placing branch at bundle "
                    << PlacedIdx << "\n");

  // Add BranchMI to the chosen bundle and record its resource bookings in the
  // Top scoreboard so subsequent inter-zone checks are accurate.
  TopBundles[PlacedIdx].add(BranchMI);
  TopHR->emitInScoreboard(*BranchMI, BranchMI->getDesc(), Delta);

  // Physically move BranchMI to its correct position in the MBB.
  CurMBB->splice(computeSplicePoint(TopBundles, PlacedIdx, BranchMI, CurMBB),
                 CurMBB, BranchMI->getIterator());

  // Now that the branch's resource bookings are in the Top scoreboard,
  // re-check for inter-zone conflicts caused by the branch itself. If one is
  // found, append a NOP and recurse (MoveDown = 1). Terminates because the
  // scoreboard has finite depth.
  if (checkInterZoneConflicts(BotBundles)) {
    LLVM_DEBUG(dbgs() << "fixupDelaySlotPosition: post-placement inter-zone "
                         "conflict, appending NOP and retrying\n");
    AppendNop();
    fixupDelaySlotPosition(TopBundles, BotBundles, BranchMI, NumDelaySlots);
  }
}

void AIEPostRASchedStrategy::leaveRegion(const SUnit &ExitSU) {
  LLVM_DEBUG(dbgs() << "    << leaveRegion\n");

  auto &BS = InterBlock.getBlockState(CurMBB);
  if (InterBlock.isGatheringPhase() ||
      BS.FixPoint.Stage != SchedulingStage::Scheduling) {
    return;
  }
  materializeMultiOpcodeInstrs();
  InterBlock.getSelectedAltDescs().clear();
  if (IsBottomRegion) {
    // This is the earliest point where we can destroy the recorded
    // schedule in iterative scheduling. enterMBB and enterRegion are too early,
    // since then the schedule can't be used to compute interblock latencies on
    // the backedge of a loop. Note that this is done in a DAG mutator, which
    // is called after enterRegion.
    BS.clearSchedule();
  }

  std::vector<AIE::MachineBundle> TopBundles = computeAndFinalizeBundles(Top);
  std::vector<AIE::MachineBundle> BotBundles = computeAndFinalizeBundles(Bot);
  handleRegionConflicts(ExitSU, TopBundles, BotBundles);

  // When the delay slot instruction was scheduled top-down (because top-fixed
  // bundles were present), fix up its position in the bundle sequence so that
  // exactly NumDelaySlots bundles follow it.
  if (PersistentTopDown) {
    if (MachineInstr *BranchMI = getDelaySlotInstr(RegionBegin, RegionEnd)) {
      const auto *TII = getTII(CurMBB);
      const unsigned NumDelaySlots = TII->getNumDelaySlots(*BranchMI);
      fixupDelaySlotPosition(TopBundles, BotBundles, BranchMI, NumDelaySlots);
    }
    PersistentTopDown = false;
  }

  assert(BS.getCurrentRegion().Bundles.empty());
  BS.addBundles(TopBundles);
  BS.addBundles(BotBundles);
  RegionBegin = nullptr;
  RegionEnd = nullptr;
  IsBottomRegion = false;
  FirstTopFixedSU = {};
  FirstBotFixedSU = {};
  LastBotFixedSU = {};
  BS.advanceRegion();
  DEBUG_BLOCKS(dbgs() << "    << leaveRegion\n");
}

void AIEPostRASchedStrategy::materializeMultiOpcodeInstrs() {
  const TargetInstrInfo *TII = getTII(CurMBB);
  const AIEHazardRecognizer &TopHazardRec = *getAIEHazardRecognizer(Top);
  const AIEHazardRecognizer &BotHazardRec = *getAIEHazardRecognizer(Bot);

  auto MaterializePseudo = [&TII](MachineInstr &MI,
                                  const AIEHazardRecognizer &HazardRec) {
    // Materialize instructions with multiple opcode options
    if (std::optional<unsigned> AltOpcode =
            HazardRec.getSelectedAltDescs().getSelectedOpcode(&MI)) {
      MI.setDesc(TII->get(*AltOpcode));
    }
  };

  assert(DAG->top() == DAG->bottom());
  for (MachineInstr &MI : make_range(DAG->begin(), DAG->top()))
    MaterializePseudo(MI, TopHazardRec);
  for (MachineInstr &MI : make_range(DAG->bottom(), DAG->end()))
    MaterializePseudo(MI, BotHazardRec);
}

const SUnit &getBundledSUnit(const ScheduleDAGMI *DAG, MachineInstr *MI) {
  if (const SUnit *SU = DAG->getSUnit(MI))
    return *SU;
  auto BundleStart = getBundleStart(MI->getIterator());
  return *DAG->getSUnit(&*BundleStart);
}

bool AIEPostRASchedStrategy::checkInterZoneConflicts(
    const std::vector<AIE::MachineBundle> &BotBundles) const {
  const AIEHazardRecognizer *TopHazardRec = getAIEHazardRecognizer(Top);
  const AIEHazardRecognizer *BotHazardRec = getAIEHazardRecognizer(Bot);

  // Make sure there's no conflict in the overlap of Top and Bottom.
  // Both zones have completed their last scheduled bundles by advance-receding
  // to an empty cycle. That means that bottom scoreboard[0]
  // represents cycle -1, which we want to line up with top scoreboard[-1]
  if (TopHazardRec->conflict(*BotHazardRec, -1)) {
    return true;
  }

  // Verify if each instruction in the Bot zone has its timing requirements met
  // for dependencies with the Top zone.
  unsigned CurTopCycle = Top.getCurrCycle();
  for (const AIE::MachineBundle &Bundle : BotBundles) {
    for (MachineInstr *MI : Bundle.getInstrs()) {
      const SUnit &SU = getBundledSUnit(DAG, MI);
      if (SU.TopReadyCycle > CurTopCycle)
        return true;
    }
    ++CurTopCycle;
  }
  return false;
}

void AIEPostRASchedStrategy::handleRegionConflicts(
    const SUnit &ExitSU, std::vector<AIE::MachineBundle> &TopBundles,
    const std::vector<AIE::MachineBundle> &BotBundles) {

  // Make sure no instructions are in flight after leaving the region.
  unsigned ExitReadyCycle = ExitSU.TopReadyCycle;
  unsigned TopFinalCycle = Top.getCurrCycle() + Bot.getCurrCycle();
  LLVM_DEBUG(dbgs() << "** checkInterZoneConflicts: ExitReadyCycle="
                    << ExitReadyCycle << " TopFinalCycle=" << TopFinalCycle
                    << "\n");
  if (ExitReadyCycle > TopFinalCycle)
    Top.bumpCycle(ExitReadyCycle - Bot.getCurrCycle());

  // Add NOPs between the two scheduling zones until:
  // - Their scoreboards do not overlap
  // - All register dependencies are met
  while (checkInterZoneConflicts(BotBundles)) {
    LLVM_DEBUG(dbgs() << "** checkInterZoneConflicts: Bump Top cycle\n");
    Top.bumpCycle(Top.getCurrCycle() + 1);
  }

  // Top's cycle may have been bumped, update TopBundles to reflect the change
  if (Top.getCurrCycle() != TopBundles.size()) {
    AIE::MachineBundle DummyCurrBundle(getTII(*DAG)->getFormatInterface());
    bumpCycleForBundles(Top.getCurrCycle(), TopBundles, DummyCurrBundle);
  }
}

/// The earliest use of this instruction in the next iteration.
/// Note that we reason with "bottom-up" cycle, so a larger cycle means it's
/// used earlier in topological order. If the SU has no loop-carried dependency,
/// this will be MAX_INT.
int getEarliestLoopCarriedUse(const SUnit &SU,
                              const InterBlockEdges &LoopEdges) {
  const SUnit *SUInCurrentIteration =
      LoopEdges.getPreBoundaryNode(SU.getInstr());
  assert(SUInCurrentIteration);
  assert(SUInCurrentIteration->getHeight() >= SU.getHeight());

  // Look at loop-carried dependencies to see how early the instruction will be
  // needed in the next iteration.
  int EarliestCycle = std::numeric_limits<int>::max();
  for (const SDep &Succ : SUInCurrentIteration->Succs) {
    if (!LoopEdges.isPostBoundaryNode(Succ.getSUnit()))
      continue;
    EarliestCycle = std::min(EarliestCycle, int(Succ.getSUnit()->getHeight()));
  }
  return EarliestCycle;
}

/// Apply a set of heuristics to a new candidate for PostRA scheduling.
///
/// \param Cand provides the policy and current best candidate.
/// \param TryCand refers to the next SUnit candidate, otherwise uninitialized.
/// \return \c true if TryCand is better than Cand (Reason is NOT NoCand)
bool AIEPostRASchedStrategy::tryCandidate(SchedCandidate &Cand,
                                          SchedCandidate &TryCand) {
  // Initialize the candidate if needed.
  if (!Cand.isValid()) {
    TryCand.Reason = NodeOrder;
    return true;
  }

  SchedBoundary &Zone = getSchedZone();
  assert(!getNextUnscheduledFixedInstr(Zone) &&
         "More than one available SUnit while not all fixed instructions have "
         "been emitted.");

  // Instructions with delay slots are critical and should be scheduled
  // as soon as they are ready.
  if (TryCand.SU->getInstr()->hasDelaySlot()) {
    assert(!Cand.SU->getInstr()->hasDelaySlot() &&
           "Best candidate already has delay slot.");
    TryCand.Reason = NodeOrder;
    return true;
  }
  if (Cand.SU->getInstr()->hasDelaySlot()) {
    return false;
  }

  // Avoid serializing long latency dependence chains.
  if (Cand.Policy.ReduceLatency && Zone.isTop() &&
      tryLatency(TryCand, Cand, Zone)) {
    return TryCand.Reason != NoCand;
  }

  // Custom heuristics for Bot zone due to the introduction of DeltaCycles.
  // The following relies on BotReadyCycle for comparisons, as this corresponds
  // to the actual cycle in which the SU will be emitted.
  if (!Zone.isTop()) {

    // Prefer placing down instructions with long dependence chains, regardless
    // of their emission cycle. This helps scheduling the critical path first.
    if (tryGreater(TryCand.SU->getDepth(), Cand.SU->getDepth(), TryCand, Cand,
                   BotPathReduce)) {
      return TryCand.Reason != NoCand;
    }

    // Special heuristics for loops.
    // Note that they aren't used for the first fixpoint iteration: this is
    // currently a workaround because we want a very optimistic schedule in that
    // first iteration. That is because it decides the slot assignments for
    // multi-slot instructions. This rule can probably be deleted once the
    // loop-aware scheduler knows how to reassign those.
    const BlockState &BS = getInterBlock().getBlockState(CurMBB);
    if (UseLoopHeuristics && BS.Kind == AIE::BlockType::Loop &&
        BS.getRegions().size() == 1 && BS.FixPoint.NumIters > 0) {
      if (const InterBlockEdges *LoopEdgesPtr = BS.getLoopSelfEdge()) {
        const InterBlockEdges &LoopEdges = *LoopEdgesPtr;
        // For instructions with equal dependence chains, prioritize scheduling
        // instructions that are used later in the next iteration. The point is
        // to teach our heuristics a tiny bit about LCDs.
        if (tryLess(getEarliestLoopCarriedUse(*TryCand.SU, LoopEdges) +
                        TryCand.SU->BotReadyCycle,
                    getEarliestLoopCarriedUse(*Cand.SU, LoopEdges) +
                        Cand.SU->BotReadyCycle,
                    TryCand, Cand, BotPathReduce)) {
          return TryCand.Reason != NoCand;
        }
      }
    }

    // Prefer the instruction whose dependent chain is estimated to
    // finish executing later. This can help reducing the overall height
    // of the region.
    if (tryGreater(TryCand.SU->BotReadyCycle + TryCand.SU->getDepth(),
                   Cand.SU->BotReadyCycle + Cand.SU->getDepth(), TryCand, Cand,
                   BotHeightReduce)) {
      return TryCand.Reason != NoCand;
    }

    // Otherwise, prefer instructions booking resources close to CurrCycle.
    // This helps "packing" the scoreboard.
    auto ReverseEmitCycle = [](const SUnit &SU) -> int {
      // Compute the first Bot cycle where the instruction books resources.
      // Note: The result might be negative due to interblock scheduling
      return int(SU.BotReadyCycle) - int(SU.Latency) + 1;
    };
    if (tryLess(ReverseEmitCycle(*TryCand.SU), ReverseEmitCycle(*Cand.SU),
                TryCand, Cand, ResourceDemand)) {
      return TryCand.Reason != NoCand;
    }
  }

  // Fall through to original instruction order.
  if ((Zone.isTop() && TryCand.SU->NodeNum < Cand.SU->NodeNum) ||
      (!Zone.isTop() &&
       (TryCand.SU->NodeNum > Cand.SU->NodeNum) ==
           (TryCand.SU->BotReadyCycle <= Cand.SU->BotReadyCycle))) {
    TryCand.Reason = NodeOrder;
    return true;
  }

  return false;
}

void AIEPreRASchedStrategy::initialize(ScheduleDAGMI *DAG) {
  GenericScheduler::initialize(DAG);

  // Cache the threshold for each pressure set.
  const std::vector<unsigned> &RegionMaxPressure =
      static_cast<ScheduleDAGMILive *>(DAG)->getRegPressure().MaxSetPressure;
  PSetThresholds.clear();
  for (unsigned PSet = 0, EndPSet = RegionMaxPressure.size(); PSet < EndPSet;
       ++PSet) {
    unsigned MaxPressure = RegionMaxPressure[PSet];
    unsigned Limit = Context->RegClassInfo->getRegPressureSetLimit(PSet);

    // If the region has a maximum pressure that exceeds the target threshold,
    // artificially reduce that threshold to force more conservative scheduling.
    if (MaxPressure > Limit) {
      unsigned ExtraPressure = MaxPressure - Limit;
      if (Limit > ExtraPressure)
        Limit -= ExtraPressure;
      else
        Limit = 0;
      LLVM_DEBUG(dbgs() << TRI->getRegPressureSetName(PSet)
                        << " Decreased Threshold to " << Limit << "\n");
    }
    PSetThresholds.push_back(Limit);
  }
}

MachineBasicBlock *AIEPreRASchedStrategy::nextBlock() {
  MachineBasicBlock *Next = nullptr;

  // The pipeliner is usually disabled to give the postpipeliner a chance.
  // The prescheduler also clutters the view of the postpipeliner, so we skip
  // such blocks here.
  auto Skip = [](MachineBasicBlock *Block) {
    if (!Block)
      return false;
    bool PrePipelinerDisabled =
        AIELoopUtils::getPipelinerDisabled(*Block) ||
        !Block->getParent()->getSubtarget().enableMachinePipeliner();
    return PreSchedFollowsSkipPipeliner &&
           AIELoopUtils::isSingleMBBLoop(Block) && PrePipelinerDisabled;
  };

  do {
    Next = MachineSchedStrategy::nextBlock();
  } while (Skip(Next));
  return Next;
}

void AIEPreRASchedStrategy::enterRegion(MachineBasicBlock *BB,
                                        MachineBasicBlock::iterator Begin,
                                        MachineBasicBlock::iterator End,
                                        unsigned RegionInstrs) {
  CurMBB = BB;
  RegionBegin = Begin;
  RegionEnd = End;
  SUDelayerMap.resize(std::distance(Begin, End), UnknownSUNum);
}

void AIEPreRASchedStrategy::leaveRegion(const SUnit &ExitSU) {
  LLVM_DEBUG(dbgs() << "Leave Region\n");
  assert(RegionPolicy.OnlyBottomUp);

  std::vector<AIE::MachineBundle> BotBundles = computeAndFinalizeBundles(Bot);

  // If requested, insert a CYCLE_SEPARATOR after each bundle.
  if (InsertCycleSeparators) {
    auto *TII = static_cast<const AIEBaseInstrInfo *>(
        CurMBB->getParent()->getSubtarget().getInstrInfo());
    auto It = RegionBegin;
    for (const AIE::MachineBundle &Bundle : BotBundles) {
      if (!Bundle.empty())
        It = std::next(Bundle.getInstrs().back()->getIterator());
      BuildMI(*CurMBB, It, DebugLoc(),
              TII->get(TII->getCycleSeparatorOpcode()));
    }
  }

  CurMBB = nullptr;
  RegionBegin = nullptr;
  RegionEnd = nullptr;
  SUDelayerMap.clear();
  SelectedAltDescs.clear();
}

PressureDiff estimatePressureDiff(const SUnit &SU,
                                  const RegPressureTracker &RPT) {
  const MachineInstr &MI = *SU.getInstr();
  const MachineRegisterInfo &MRI = MI.getMF()->getRegInfo();
  PressureDiff PDiff;
  const LiveRegSet &LiveRegs = RPT.getLiveRegs();
  LiveRegSet DefinedRegs;
  DefinedRegs.init(MRI);

  for (const MachineOperand &D : MI.defs()) {
    if (D.isReg() && D.getReg().isVirtual()) {
      // Note that we aren't in SSA anymore, so D.getReg() might already be live
      PDiff.addPressureChange(D.getReg(), /*IsDec=*/true, &MRI);
      DefinedRegs.insert(VRegMaskOrUnit(D.getReg(), LaneBitmask::getAll()));
    }
  }
  for (const MachineOperand &U : MI.uses()) {
    if (!U.isReg() || !U.getReg().isVirtual())
      continue;
    // Note that newly-defined registers make in/out regs live again.
    // e.g. %0 should still be live after receding over `%0 = FOO %0`
    LaneBitmask LiveLanes =
        LiveRegs.contains(U.getReg()) & ~DefinedRegs.contains(U.getReg());
    if (LiveLanes.none())
      PDiff.addPressureChange(U.getReg(), /*IsDec=*/false, &MRI);
  }
  LLVM_DEBUG(dbgs() << "EstPDiff SU(" << SU.NodeNum << "): ");
  LLVM_DEBUG(PDiff.dump(*MRI.getTargetRegisterInfo()));
  return PDiff;
}

/// Return the worst (or best if \p FindMin is false) pressure change
/// within \p PD.
PressureChange getPressureChange(const PressureDiff &PD, bool FindMin = true) {
  if (PD.begin() == PD.end())
    return {};
  auto Cmp = [](const PressureChange &Lhs, const PressureChange &Rhs) {
    return Lhs.getUnitInc() < Rhs.getUnitInc();
  };
  return FindMin ? *std::min_element(PD.begin(), PD.end(), Cmp)
                 : *std::max_element(PD.begin(), PD.end(), Cmp);
}

/// Try and find a SUnit within \p Nodes that can help reduce the pressure
/// for \p CriticalPSet. Returns nullptr if not successful.
const SUnit *findPressureReducer(unsigned CriticalPSet, ArrayRef<SUnit *> Nodes,
                                 const RegPressureTracker &RPT) {
  for (const SUnit *SU : Nodes) {
    PressureDiff PDiff = estimatePressureDiff(*SU, RPT);
    for (const PressureChange &PC : PDiff) {
      if (PC.isValid() && PC.getPSet() == CriticalPSet && PC.getUnitInc() < 0)
        return SU;
    }
  }
  return nullptr;
}

bool AIEPreRASchedStrategy::isAvailableNode(SUnit &SU, SchedBoundary &Zone,
                                            bool /*VerifyReadyCycle*/) {
  // Force verifying if SU is ready to be scheduled in terms of cycle.
  bool Avail = MachineSchedStrategy::isAvailableNode(SU, Zone,
                                                     /*VerifyReadyCycle=*/true);
  if (!EnableFinerRPTracking)
    return Avail;
  if (!Avail)
    return false;

  // The node can be scheduled, but check if it increases the pressure too much.
  // If so, try to delay it until another instruction decreases the pressure.
  const RegPressureTracker &BotRPT = DAG->getBotRPTracker();
  PressureChange WorstPC =
      getPressureChange(estimatePressureDiff(SU, BotRPT), false);
  if (WorstPC.getUnitInc() <= 0) {
    // Improving register pressure, keep node as available
    return true;
  }

  unsigned CurrPressure = BotRPT.getRegSetPressureAtPos()[WorstPC.getPSet()];
  if (CurrPressure + WorstPC.getUnitInc() +
          (NumCriticalFreeRegs * WorstPC.getUnitInc()) <
      PSetThresholds[WorstPC.getPSet()]) {
    // Worsening pressure, but still within limits, keep node as available
    return true;
  }

  // The node will likely cause a spill, only consider it schedule-able if
  // there is no pending node that can reduce the register pressure.
  if (const SUnit *PendingPressureReducer = findPressureReducer(
          WorstPC.getPSet(), Zone.Pending.elements(), BotRPT);
      PendingPressureReducer && canBeDelayed(SU, *PendingPressureReducer)) {
    LLVM_DEBUG(dbgs() << "** Delaying SU(" << SU.NodeNum << "): Waiting for SU("
                      << PendingPressureReducer->NodeNum << ")\n");

    // Keep track of PendingPressureReducer to avoid cycles of SUs
    // delaying each other.
    SUDelayerMap[SU.NodeNum] = PendingPressureReducer->NodeNum;
    return false;
  }

  // Can't prove a pending SU will help reduce reg pressure, keep as available.
  return true;
}

bool AIEPreRASchedStrategy::canBeDelayed(const SUnit &DelayedSU,
                                         const SUnit &Delayer) const {
  std::function<bool(unsigned)> Impl = [&](unsigned SUNum) {
    if (SUNum == UnknownSUNum)
      return true;
    if (SUNum == DelayedSU.NodeNum)
      return false;
    return Impl(SUDelayerMap[SUNum]);
  };
  // If SU is delayed by another instruction that is eventually waiting on SU
  // itself, do not keep delaying SU otherwise this creates an infinite loop.
  return Impl(Delayer.NodeNum);
}

bool AIEPreRASchedStrategy::tryCandidate(SchedCandidate &Cand,
                                         SchedCandidate &TryCand,
                                         SchedBoundary *Zone) const {
  if (!EnableFinerRPTracking)
    return GenericScheduler::tryCandidate(Cand, TryCand, Zone);

  // Note: Most of the heuristics below are taken from the default
  // GenericScheduler strategy. However, we then also try to better estimate
  // the pressure change of both candidates (based on what regs are live,
  // or made live), and use those to check if the threshold of a pressure set
  // would be exceeded.

  // Initialize the candidate if needed.
  if (!Cand.isValid()) {
    TryCand.Reason = NodeOrder;
    return true;
  }

  // Bias PhysReg Defs and copies to their uses and defined respectively.
  if (tryGreater(biasPhysReg(TryCand.SU, TryCand.AtTop),
                 biasPhysReg(Cand.SU, Cand.AtTop), TryCand, Cand, PhysReg))
    return TryCand.Reason != NoCand;

  // Avoid exceeding the target's limit.
  if (DAG->isTrackingPressure() &&
      tryPressure(TryCand.RPDelta.Excess, Cand.RPDelta.Excess, TryCand, Cand,
                  RegExcess, TRI, DAG->MF))
    return TryCand.Reason != NoCand;

  // Avoid increasing the max critical pressure in the scheduled region.
  if (DAG->isTrackingPressure() &&
      tryPressure(TryCand.RPDelta.CriticalMax, Cand.RPDelta.CriticalMax,
                  TryCand, Cand, RegCritical, TRI, DAG->MF))
    return TryCand.Reason != NoCand;

  // Weak edges are for clustering and other constraints.
  if (tryLess(getWeakLeft(TryCand.SU, TryCand.AtTop),
              getWeakLeft(Cand.SU, Cand.AtTop), TryCand, Cand, Weak))
    return TryCand.Reason != NoCand;

  // Main change from GenericScheduler: try and better estimate the
  // pressure changes for both candidates.
  if (DAG->isTrackingPressure()) {
    const RegPressureTracker &BotRPT = DAG->getBotRPTracker();
    auto IsNearCritical = [&](const PressureChange &PC) {
      if (!PC.isValid())
        return false;
      unsigned CurrPressure = BotRPT.getRegSetPressureAtPos()[PC.getPSet()];
      unsigned Threshold = PSetThresholds[PC.getPSet()];
      unsigned NumCriticalFreeUnits =
          NumCriticalFreeRegs * std::abs(PC.getUnitInc());
      return Threshold <= NumCriticalFreeUnits ||
             CurrPressure >= Threshold - NumCriticalFreeUnits;
    };
    PressureChange TryCandPC =
        getPressureChange(estimatePressureDiff(*TryCand.SU, BotRPT));
    PressureChange CandPC =
        getPressureChange(estimatePressureDiff(*Cand.SU, BotRPT));
    if ((IsNearCritical(TryCandPC) || IsNearCritical(CandPC)) &&
        tryPressure(TryCandPC, CandPC, TryCand, Cand, RegMax, TRI, DAG->MF))
      return TryCand.Reason != NoCand;

    // Avoid increasing the max pressure of the entire region.
    if (tryPressure(TryCand.RPDelta.CurrentMax, Cand.RPDelta.CurrentMax,
                    TryCand, Cand, RegMax, TRI, DAG->MF))
      return TryCand.Reason != NoCand;
  }

  // Fall through to original instruction order.
  if ((Zone->isTop() && TryCand.SU->NodeNum < Cand.SU->NodeNum) ||
      (!Zone->isTop() && TryCand.SU->NodeNum > Cand.SU->NodeNum)) {
    TryCand.Reason = NodeOrder;
    return true;
  }

  return false;
}

AIEPostRASchedStrategy *AIEScheduleDAGMI::getSchedImpl() const {
  return static_cast<AIEPostRASchedStrategy *>(SchedImpl.get());
}

void AIEScheduleDAGMI::enterRegion(MachineBasicBlock *BB,
                                   MachineBasicBlock::iterator Begin,
                                   MachineBasicBlock::iterator End,
                                   unsigned RegionInstrs) {
  ScheduleDAGMI::enterRegion(BB, Begin, End, RegionInstrs);

  // AIEPostRASchedStrategy doesn't get callbacks from the MachineScheduler
  // to enter/exit regions. Let's give it some.
  getSchedImpl()->enterRegion(BB, Begin, End, RegionInstrs);

  // The MachineScheduler skips regions with a single instruction.
  // AIE has an exposed pipeline and some NOPs might still be needed for
  // correctness. This also ensures that the state within SchedImpl is
  // correctly initialized...
  if (RegionInstrs <= 1) {
    LLVM_DEBUG(dbgs() << "Force scheduling for skipped region\n");
    schedule();
  }
}

void AIEScheduleDAGMI::exitRegion() {
  // AIEPostRASchedStrategy doesn't get callbacks from the MachineScheduler
  // to enter/exit regions. Let's give it some.
  getSchedImpl()->leaveRegion(ExitSU);
  ScheduleDAGMI::exitRegion();
}

void AIEScheduleDAGMI::recordDbgInstrs(const Region &CurrentRegion) {
  // Remove any stale debug info; sometimes BuildSchedGraph is called again
  // without emitting the info from the previous call.
  DbgValues.clear();
  FirstDbgValue = nullptr;

  // We connect any Debug machine instruction to the instruction before it.
  // if there is no instruction before it, it is recorded in FirstDbgValue;
  MachineInstr *DbgMI = nullptr;
  for (MachineInstr *MI : reverse(CurrentRegion.getFreeInstructions())) {
    if (DbgMI) {
      DbgValues.emplace_back(DbgMI, MI);
      DbgMI = nullptr;
    }

    if (MI->isDebugValue() || MI->isDebugPHI()) {
      DbgMI = MI;
    }
  }
  if (DbgMI)
    FirstDbgValue = DbgMI;
}

void AIEScheduleDAGMI::finalizeSchedule() {
  if (AllowNegativeLatencies) {
    // Negative latencies can make it seem that one reads undefined registers
    // if not accounting for timing.
    MRI.invalidateLiveness();
  }
  ScheduleDAGMI::finalizeSchedule();
}

void AIEScheduleDAGMI::releasePred(SUnit *SU, SDep *PredEdge) {
  if (PredEdge->isWeak()) {
    return ScheduleDAGMI::releasePred(SU, PredEdge);
  }

  // Update the ready cycle of SU's predecessor
  SUnit *PredSU = PredEdge->getSUnit();
  int Latency = AllowNegativeLatencies ? PredEdge->getSignedLatency()
                                       : PredEdge->getLatency();
  PredSU->BotReadyCycle =
      std::max(int(PredSU->BotReadyCycle), int(SU->BotReadyCycle) + Latency);

  --PredSU->NumSuccsLeft;
  if (PredSU->NumSuccsLeft == 0 && PredSU != &EntrySU)
    SchedImpl->releaseBottomNode(PredSU);
}

void AIEScheduleDAGMI::releaseSucc(SUnit *SU, SDep *SuccEdge) {

  if (SuccEdge->isWeak())
    return ScheduleDAGMI::releaseSucc(SU, SuccEdge);

  // Update the ready cycle of SU's successor. When AllowNegativeLatencies is
  // enabled, use the signed latency so that a successor with a negative-latency
  // edge can be scheduled in a cycle earlier than SU->TopReadyCycle + latency.
  // This is the top-down counterpart of the negative-latency handling in
  // releasePred: just as a predecessor can be pulled closer to its consumer
  // (bottom-up), a successor can be pulled closer to its producer (top-down).
  SUnit *SuccSU = SuccEdge->getSUnit();
  const int Latency = AllowNegativeLatencies ? SuccEdge->getSignedLatency()
                                             : (int)SuccEdge->getLatency();
  // Use signed arithmetic to correctly handle negative latencies. The result
  // is clamped to zero since TopReadyCycle is unsigned and a negative ready
  // cycle has no meaning (the instruction is ready from the very first cycle).
  const int NewReadyCycle = int(SU->TopReadyCycle) + Latency;
  if (NewReadyCycle > int(SuccSU->TopReadyCycle))
    SuccSU->TopReadyCycle = NewReadyCycle;

  --SuccSU->NumPredsLeft;
  if (SuccSU->NumPredsLeft == 0 && SuccSU != &ExitSU)
    SchedImpl->releaseTopNode(SuccSU);
}

AIEPreRASchedStrategy *AIEScheduleDAGMILive::getSchedImpl() const {
  return static_cast<AIEPreRASchedStrategy *>(SchedImpl.get());
}

void AIEScheduleDAGMILive::enterRegion(MachineBasicBlock *BB,
                                       MachineBasicBlock::iterator Begin,
                                       MachineBasicBlock::iterator End,
                                       unsigned RegionInstrs) {
  ScheduleDAGMILive::enterRegion(BB, Begin, End, RegionInstrs);

  // AIEPreRASchedStrategy doesn't get callbacks from the MachineScheduler
  // to enter/exit regions. Let's give it some.
  static_cast<AIEPreRASchedStrategy *>(SchedImpl.get())
      ->enterRegion(BB, Begin, End, RegionInstrs);

  // Similar to AIEScheduleDAGMI, ensure correct state for SchedImpl.
  if (RegionInstrs <= 1) {
    LLVM_DEBUG(dbgs() << "Force scheduling for skipped region\n");
    schedule();
  }
}

void AIEScheduleDAGMILive::exitRegion() {
  // AIEPreRASchedStrategy doesn't get callbacks from the MachineScheduler
  // to enter/exit regions. Let's give it some.
  static_cast<AIEPreRASchedStrategy *>(SchedImpl.get())->leaveRegion(ExitSU);
  ScheduleDAGMILive::exitRegion();
}

void llvm::AIEPostRASchedStrategy::buildGraph(ScheduleDAGMI &DAG, AAResults *AA,
                                              RegPressureTracker *RPTracker,
                                              PressureDiffs *PDiffs,
                                              LiveIntervals *LIS,
                                              bool TrackLaneMasks) {

  // Let's save the DAG already instead of waiting for initialize().
  // Some DAG mutators might require a DAG to be set.
  this->DAG = &DAG;

  /// We are called after enterRegion, which will have recorded the semantic
  /// order. We can't use the basic block order, since this may have changed
  /// in earlier iterations of scheduling
  DAG.clearDAG();

  auto &BS = InterBlock.getBlockState(CurMBB);
  const auto &Region = BS.getCurrentRegion();
  int NCopies = 1;
  if (BS.FixPoint.II) {
    assert(BS.Kind == BlockType::Loop);
    assert(BS.getRegions().size() == 1);
    assert(Region.getBotFixedBundles().empty());
    assert(Region.getTopFixedBundles().empty());
    // Try to wrap the linear schedule within II.
    // We create two copies of the loop body, which will make the loop-carried
    // dependences appear as forward dependences between the first and the
    // second iteration.
    NCopies = 2;
  }
  DEBUG_BLOCKS(dbgs() << "    buildGraph, NCopies=" << NCopies << "\n");
  for (int S = 0; S < NCopies; S++) {
    // Only add SUnits for "free" instructions, fixed instructions will be added
    // later in a DAGMutator.
    for (MachineInstr *I : Region.getFreeInstructions()) {
      DAG.initSUnit(*I);
    }
  }
  DAG.ExitSU.setInstr(Region.getExitInstr());
  DAG.makeMaps();
  DAG.buildEdges(Context->AA);
  static_cast<AIEScheduleDAGMI &>(DAG).recordDbgInstrs(Region);
}

SUnit &AIEPostRASchedStrategy::addFixedSUnit(MachineInstr &MI, bool IsTop) {
  DEBUG_BLOCKS(dbgs() << "Adding Fixed MI: " << MI);
  DEBUG_BLOCKS(dbgs() << "  DAG size=" << DAG->SUnits.size()
                      << " capacity=" << DAG->SUnits.capacity() << "\n");
  assert(!(IsTop && FirstBotFixedSU) && "Top-fixed SUnits must be added first");
  assert(DAG->SUnits.size() < DAG->SUnits.capacity() &&
         "SUnits need to be re-allocated.");
  unsigned SUNum = DAG->initSUnit(MI).value();
  SUnit &SU = DAG->SUnits[SUNum];

  if (IsTop) {
    if (!FirstTopFixedSU)
      FirstTopFixedSU = SUNum;
  } else {
    if (!FirstBotFixedSU)
      FirstBotFixedSU = SUNum;
    LastBotFixedSU = SUNum;
  }

  return SU;
}

bool AIEScheduleDAGMI::mayAlias(SUnit *SUa, SUnit *SUb, bool UseTBAA) {
  BlockState &BS = getSchedImpl()->getInterBlock().getBlockState(getBB());

  if (BS.isSafeToIgnoreMemDeps())
    return false;

  if (BS.FixPoint.Stage == SchedulingStage::Pipelining) {
    int NInstr = BS.getCurrentRegion().getFreeInstructions().size();
    int IterA = SUa->NodeNum / NInstr;
    int IterB = SUb->NodeNum / NInstr;
    if (aliasAcrossVirtualUnrolls(SUa->getInstr(), SUb->getInstr(), IterA,
                                  IterB) == AliasResult::NoAlias) {
      return false;
    }
  }

  return ScheduleDAGMI::mayAlias(SUa, SUb, UseTBAA);
}

void AIEScheduleDAGMI::schedule() {
  BlockState &BS = getSchedImpl()->getInterBlock().getBlockState(getBB());

  if (getSchedImpl()->getInterBlock().isGatheringPhase()) {
    // We are only gathering regions in the MBB, no scheduling to do.
    return;
  }
  switch (BS.FixPoint.Stage) {
  case SchedulingStage::Pipelining: {
    // We've gone past regular scheduling. Try to find a valid modulo schedule
    // If it succeeds, we need to implement it, if we fail we fall back on the
    // normal loop schedule
    SchedImpl->buildGraph(*this, AA);
    postProcessDAG();

    auto &PostSWP = BS.getPostSWP();

    if (PostSWP.schedule(*this, BS.FixPoint.II)) {
      BS.setPipelined();
      LLVM_DEBUG(PostSWP.dump());
    }
    return;
  }
  default:
    ScheduleDAGMI::schedule();
    return;
  }
}
