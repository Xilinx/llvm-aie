//===- AIEMachineScheduler.cpp - MI Scheduler for AIE ---------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEMachineScheduler.h"
#include "AIEBaseInstrInfo.h"
#include "AIEHazardRecognizer.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "machine-scheduler"

using namespace llvm;

static cl::opt<bool> InsertCycleSeparators(
    "aie-prera-cycle-separators", cl::init(false),
    cl::desc("[AIE] Insert CYCLE_SEPARATOR meta instructions"));
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

static AIEHazardRecognizer *getAIEHazardRecognizer(const SchedBoundary &Zone) {
  return static_cast<AIEHazardRecognizer *>(Zone.HazardRec);
}

AIEPostRASchedStrategy::AIEPostRASchedStrategy(const MachineSchedContext *C)
    : PostGenericScheduler(C), Bot(SchedBoundary::BotQID, "BotQ") {
  assert((!ForceBottomUp || !ForceTopDown) &&
         "-misched-topdown incompatible with -misched-bottomup");
  if (ForceTopDown)
    this->IsTopDown = true;
  else if (ForceBottomUp)
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

std::vector<AIE::MachineBundle> computeAndFinalizeBundles(SchedBoundary &Zone) {
  LLVM_DEBUG(dbgs() << "Computing Bundles for Zone "
                    << (Zone.isTop() ? "Top\n" : "Bot\n"));
  const ScheduleDAGMI &DAG = *Zone.DAG;
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
      if (EmitCycle != Bundles.size())
        bumpCycleForBundles(EmitCycle, Bundles, CurrBundle);

      LLVM_DEBUG(dbgs() << "  Add to CurrBundle: " << MI);
      CurrBundle.add(&MI, HazardRec.getSelectedAltOpcode(&MI));
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
           return ScheduledMBB.count(B) > 0;
         });
}

void AIEPostRASchedStrategy::initializeBotScoreBoard(ScoreboardTrust Trust) {

  if (!InterBlockScoreboard) {
    LLVM_DEBUG(dbgs() << "Interblock scoreboard explicitly disabled\n");
    return;
  }

  // Check that we have forced bottom up regions. This makes sure that
  // getTop() used below behaves sanely.
  assert(!doMBBSchedRegionsTopDown());
  AIEHazardRecognizer *BotHazardRec = getAIEHazardRecognizer(Bot);
  const int Depth = BotHazardRec->getMaxLookAhead();

  /// These lambdas are an abstraction of the scoreboard manipulations,
  /// hiding the details of the implementation. In particular, we need to
  /// make sure we always have enough lookahead available. We arrange for that
  /// by starting in the earliest possible cycle, -Depth
  auto InsertInCycle = [=](MachineInstr &MI, int Cycle) {
    BotHazardRec->emitInScoreboard(MI.getDesc(), Cycle - Depth, std::nullopt);
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
  int FirstBlockedCycle = 0;

  if (Trust != ScoreboardTrust::Conservative) {
    // Depth is a suitable supremum for the minimum we compute.
    // Note that if the loop isn't entered, the responsibilty lies with
    // our caller not setting Conservative. This may be legitimate to
    // represent a done or flush_pipeline instruction in future
    FirstBlockedCycle = Depth;
    for (llvm::MachineBasicBlock *SuccMBB : CurMBB->successors()) {
      // Replay bundles into scoreboard.
      LLVM_DEBUG(dbgs() << "**** Replay bundles of MBBs : "
                        << SuccMBB->getNumber() << " "
                        << "for MBB : " << CurMBB->getNumber() << " ****\n");
      const Region &TopRegion = ScheduledMBB[SuccMBB].getTop();
      int Cycle = 0;
      for (auto &Bundle : TopRegion) {
        // There's only so much future we need.
        if (Cycle >= FirstBlockedCycle) {
          break;
        }
        LLVM_DEBUG(dbgs() << "Cycle " << Cycle << " has "
                          << Bundle.getInstrs().size() << "instr\n");
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

  // We have to assume the worst for any cycle we haven't completely seen
  for (int Cycle = FirstBlockedCycle; Cycle < Depth; Cycle++) {
    BlockCycle(Cycle);
  }

  AlignScoreboardToCycleOne();
  LLVM_DEBUG(BotHazardRec->dumpScoreboard());
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
  assert(!ForceBottomUp && !ForceTopDown);

  Bot.init(DAG, this, SchedModel, &Rem);
  const InstrItineraryData *Itin = SchedModel->getInstrItineraries();
  if (!Bot.HazardRec) {
    Bot.HazardRec =
        DAG->MF.getSubtarget().getInstrInfo()->CreateTargetMIHazardRecognizer(
            Itin, DAG);
  }

  // Update Bot scoreboard of the bottom region with the foreseeable future
  // as found in the top regions of the successor blocks. If we don't know,
  // assume the worst.
  // TODO: When we include alignment in the schedule, we can change
  // AccountForAlign to Absolute
  const bool Conservative = !(IsBottomRegion && successorsAreScheduled(CurMBB));
  const ScoreboardTrust NonConservative = InterBlockAlignment
                                              ? ScoreboardTrust::AccountForAlign
                                              : ScoreboardTrust::Absolute;
  initializeBotScoreBoard(Conservative ? ScoreboardTrust::Conservative
                                       : NonConservative);

  // TODO:
  // We could set something like 'InterBlockCycles' here and include
  // that in RegionBottomUpCycles below to force (some) backward
  // scheduling.
  // Currently we rely on checkInterZoneConflicts to resolve the conflicts
  // with the bottom scoreboard representing the successor blocks, and
  // backwards scheduling doesn't seem to give a net gain.

  // Delay slots are scheduled bottom up to be sure the control-flow instruction
  // is issued exactly TII->getNumDelaySlots() before the end of the region.
  unsigned DelaySlotCycles = 0;
  if (MachineInstr *MI = getDelaySlotInstr(RegionBegin, RegionEnd)) {
    auto *TII = getTII(CurMBB);
    assert(RegionEnd != MI->getParent()->instr_end() &&
           TII->isDelayedSchedBarrier(*RegionEnd));
    // Schedule bottom-up for at least getNumDelaySlots() cycles, and an extra
    // one for the delay slot instruction itself.
    DelaySlotCycles = TII->getNumDelaySlots(*MI) + 1;
    unsigned Reserved = std::max(ReservedDelaySlots.getValue(),
                                 TII->getNumReservedDelaySlots(*MI));
    getAIEHazardRecognizer(Bot)->setReservedCycles(Reserved);
  }

  RegionBottomUpCycles = std::max(BottomUpCycles.getValue(), DelaySlotCycles);
  IsTopDown = (RegionBottomUpCycles == 0);
  if (!IsTopDown)
    LLVM_DEBUG(dbgs() << "*** Using bottom-up scheduling for the region ***\n");
  else
    LLVM_DEBUG(dbgs() << "*** Using top-down scheduling for the region ***\n");
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

SUnit *AIEPostRASchedStrategy::pickNodeAndCycle(
    bool &IsTopNode, std::optional<unsigned> &BotEmissionCycle) {
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
  // scheduled in a cycle greater than CurrCycle. See isAvailableNode().
  // Make sure to set the EmissionCycle right.
  if (!Zone.isTop()) {
    assert(SU->BotReadyCycle >= Zone.getCurrCycle());
    BotEmissionCycle = SU->BotReadyCycle;
  }

  LLVM_DEBUG(dbgs() << "Scheduling SU(" << SU->NodeNum << ") "
                    << *SU->getInstr());
  return SU;
}

int AIEPostRASchedStrategy::getMaxDeltaCycles(const SchedBoundary &Zone) const {
  assert(!Zone.isTop());
  if (Zone.getCurrCycle() >= RegionBottomUpCycles - 1)
    return 0;
  return std::min({int(RegionBottomUpCycles - 1 - Zone.getCurrCycle()),
                   int(getAIEHazardRecognizer(Zone)->getMaxLookAhead()),
                   BottomUpDelta.getValue()});
}

bool AIEPostRASchedStrategy::isAvailableNode(SUnit &SU, SchedBoundary &Zone,
                                             bool /*VerifyReadyCycle*/) const {
  // Whether or not the zone is Top or Bot, verify if SU is ready to be
  // scheduled in terms of cycle.
  if (Zone.isTop())
    return MachineSchedStrategy::isAvailableNode(SU, Zone,
                                                 /*VerifyReadyCycle=*/true);

  // Note we use signed integers to avoid wrap-around behavior.
  const int MinDelta = -getMaxDeltaCycles(Zone);
  const int ReadyCycle = std::max(Zone.getCurrCycle(), SU.BotReadyCycle);
  const int CurrCycle = Zone.getCurrCycle();

  for (int DeltaCycles = CurrCycle - ReadyCycle; DeltaCycles >= MinDelta;
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
  SU.BotReadyCycle = std::max(ReadyCycle, CurrCycle - MinDelta);
  return false;
}

/// Called after ScheduleDAGMI has scheduled an instruction and updated
/// scheduled/remaining flags in the DAG nodes.
void AIEPostRASchedStrategy::schedNode(SUnit *SU, bool IsTopNode) {
  if (IsTopNode) {
    PostGenericScheduler::schedNode(SU, IsTopNode);
  } else {
    int DeltaCycles = int(Bot.getCurrCycle()) - int(SU->BotReadyCycle);
    assert(DeltaCycles <= 0);
    Bot.bumpNode(SU, DeltaCycles);
  }
}

void AIEPostRASchedStrategy::enterMBB(MachineBasicBlock *MBB) {
  CurMBB = MBB;
  // We force bottom up region processing, so the first region
  // from a block is the bottom one. We reset this when leaving any
  // region
  IsBottomRegion = true;
  LLVM_DEBUG(dbgs() << "Enter MBB" << CurMBB->getName() << "\n");
}

void AIEPostRASchedStrategy::leaveMBB() {
  LLVM_DEBUG(dbgs() << "Leave MBB" << CurMBB->getName() << "\n");
  CurMBB = nullptr;
}

std::vector<MachineBasicBlock *>
AIEPostRASchedStrategy::getMBBScheduleSeq(MachineFunction &MF) const {

  std::vector<MachineBasicBlock *> MBBSequence;
  for (MachineBasicBlock *MBB : post_order(&MF)) {
    MBBSequence.push_back(MBB);
  }

  LLVM_DEBUG(dbgs() << "MBB scheduling sequence : ";
             for (const auto &MBBSeq
                  : MBBSequence) dbgs()
             << MBBSeq->getNumber() << " -> ";
             dbgs() << "\n";);

  assert(MF.size() == MBBSequence.size() &&
         "Missing MBB in scheduling sequence");

  return MBBSequence;
}
const AIEPostRASchedStrategy::BlockState &
AIEPostRASchedStrategy::getBlockState(MachineBasicBlock *MBB) const {
  return ScheduledMBB.at(MBB);
}

void AIEPostRASchedStrategy::enterRegion(MachineBasicBlock *BB,
                                         MachineBasicBlock::iterator Begin,
                                         MachineBasicBlock::iterator End,
                                         unsigned RegionInstrs) {
  RegionBegin = Begin;
  RegionEnd = End;
}

void AIEPostRASchedStrategy::leaveRegion(const SUnit &ExitSU) {
  LLVM_DEBUG(dbgs() << "Leave Region\n");
  materializeMultiOpcodeInstrs();
  std::vector<AIE::MachineBundle> TopBundles = computeAndFinalizeBundles(Top);
  std::vector<AIE::MachineBundle> BotBundles = computeAndFinalizeBundles(Bot);
  handleRegionConflicts(ExitSU, TopBundles, BotBundles);
  leaveSchedulingZone(Top, TopBundles);
  leaveSchedulingZone(Bot, BotBundles);
  RegionBegin = nullptr;
  RegionEnd = nullptr;
  IsBottomRegion = false;
}

void AIEPostRASchedStrategy::materializeMultiOpcodeInstrs() {
  const TargetInstrInfo *TII = getTII(CurMBB);
  const AIEHazardRecognizer &TopHazardRec = *getAIEHazardRecognizer(Top);
  const AIEHazardRecognizer &BotHazardRec = *getAIEHazardRecognizer(Bot);

  auto MaterializePseudo = [&TII](MachineInstr &MI,
                                  const AIEHazardRecognizer &HazardRec) {
    // Materialize instructions with multiple opcode options
    if (std::optional<unsigned> AltOpcode =
            HazardRec.getSelectedAltOpcode(&MI)) {
      MI.setDesc(TII->get(*AltOpcode));
    }
  };

  assert(DAG->top() == DAG->bottom());
  for (MachineInstr &MI : make_range(DAG->begin(), DAG->top()))
    MaterializePseudo(MI, TopHazardRec);
  for (MachineInstr &MI : make_range(DAG->bottom(), DAG->end()))
    MaterializePseudo(MI, BotHazardRec);
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
      SUnit *SU = DAG->getSUnit(MI);
      if (SU->TopReadyCycle > CurTopCycle)
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

void AIEPostRASchedStrategy::leaveSchedulingZone(
    SchedBoundary &Zone,
    const std::vector<AIE::MachineBundle> &OrderedBundles) {
  LLVM_DEBUG(dbgs() << "  NumBundles=" << OrderedBundles.size() << "\n");

  // Contrary to PRAS, the MachineScheduler does not automatically insert NOPs.
  // That isn't a problem, since the callbacks to the HazardRecognizer were a
  // bit flaky (e.g. when to call emitNoop vs advanceCycle).
  // MachineScheduler just calls advanceCycle, and this is enough for us to
  // insert NOPs because the sequence of Bundles gives us the full picture.
  const TargetInstrInfo *TII = getTII(CurMBB);
  auto It = RegionBegin;
  for (const AIE::MachineBundle &Bundle : OrderedBundles) {
    if (Bundle.empty()) {
      // Empty bundle means 1-cycle stall.
      TII->insertNoop(*CurMBB, It);
      continue;
    }
    It = std::next(Bundle.getInstrs().back()->getIterator());
  }

  // Bundle real instructions and move meta instructions.
  // TODO: this should be done here instead of in a HazardRecognizer.
  AIEHazardRecognizer::applyBundles(OrderedBundles, CurMBB);

  // ScheduledMBB is populated with Bundles for MBB after they are scheduled.
  // Every scheduled region is saved under the same key/MBB.
  // Note : Bundles from Top & Bot zone combined created complete bundles for a
  // region.
  if (Zone.isTop())
    ScheduledMBB[CurMBB].addTop(OrderedBundles);
  else {
    std::vector<AIE::MachineBundle> &TopBundles = ScheduledMBB[CurMBB].getTop();
    TopBundles.insert(TopBundles.end(), OrderedBundles.begin(),
                      OrderedBundles.end());
  }
}

void AIEPostRASchedStrategy::releaseBottomNode(SUnit *SU) {
  PostGenericScheduler::releaseBottomNode(SU);
  if (SU->isScheduled)
    return;
  Bot.releaseNode(SU, SU->BotReadyCycle, false);
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

  SchedBoundary &Zone = getSchedZone();

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

void AIEPreRASchedStrategy::enterRegion(MachineBasicBlock *BB,
                                        MachineBasicBlock::iterator Begin,
                                        MachineBasicBlock::iterator End,
                                        unsigned RegionInstrs) {
  CurMBB = BB;
  RegionBegin = Begin;
  RegionEnd = End;
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
}

bool AIEPreRASchedStrategy::isAvailableNode(SUnit &SU, SchedBoundary &Zone,
                                            bool /*VerifyReadyCycle*/) const {
  // Force verifying if SU is ready to be scheduled in terms of cycle.
  return MachineSchedStrategy::isAvailableNode(SU, Zone,
                                               /*VerifyReadyCycle=*/true);
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
    ScheduleDAGMI::schedule();
  }
}

void AIEScheduleDAGMI::exitRegion() {
  // AIEPostRASchedStrategy doesn't get callbacks from the MachineScheduler
  // to enter/exit regions. Let's give it some.
  getSchedImpl()->leaveRegion(ExitSU);
  ScheduleDAGMI::exitRegion();
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
    ScheduleDAGMILive::schedule();
  }
}

void AIEScheduleDAGMILive::exitRegion() {
  // AIEPreRASchedStrategy doesn't get callbacks from the MachineScheduler
  // to enter/exit regions. Let's give it some.
  static_cast<AIEPreRASchedStrategy *>(SchedImpl.get())->leaveRegion(ExitSU);
  ScheduleDAGMILive::exitRegion();
}
