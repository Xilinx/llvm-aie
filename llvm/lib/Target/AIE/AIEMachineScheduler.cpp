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
    "aie-bottomup-cycles", cl::init(0),
    cl::desc("[AIE] Min number of cycles to be scheduled bottom-up"));
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

/// Flush any in-flight bundle in the zone's HazardRecognizer
static void finalizeCurrentBundle(SchedBoundary &Zone) {
  LLVM_DEBUG(Zone.dumpScheduledState());
  AIEHazardRecognizer *HazardRec = getAIEHazardRecognizer(Zone);
  if (!HazardRec) {
    return;
  }

  // Finalize the current bundle, if any.
  if (HazardRec->currentCycleHasInstr()) {
    Zone.bumpCycle(Zone.getCurrCycle() + 1);
    LLVM_DEBUG(dbgs() << "  Finalized Bundle. CurrentCycle="
                      << Zone.getCurrCycle() << "\n");
  }
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
    BotHazardRec->emitInScoreboard(MI.getDesc().getSchedClass(), Cycle - Depth,
                                   std::nullopt);
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

  Bot.init(DAG, SchedModel, &Rem);
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

SUnit *AIEPostRASchedStrategy::pickNode(bool &IsTopNode) {
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
  SUnit *SU;
  do {
    SU = pickNodeUnidirectional(Zone);
  } while (SU->isScheduled);

  IsTopNode = Zone.isTop();

  if (SU->isTopReady())
    Top.removeReady(SU);
  if (SU->isBottomReady())
    Bot.removeReady(SU);

  LLVM_DEBUG(dbgs() << "Scheduling SU(" << SU->NodeNum << ") "
                    << *SU->getInstr());
  return SU;
}

/// Called after ScheduleDAGMI has scheduled an instruction and updated
/// scheduled/remaining flags in the DAG nodes.
void AIEPostRASchedStrategy::schedNode(SUnit *SU, bool IsTopNode) {
  if (IsTopNode) {
    PostGenericScheduler::schedNode(SU, IsTopNode);
  } else {
    SU->BotReadyCycle = std::max(SU->BotReadyCycle, Bot.getCurrCycle());
    Bot.bumpNode(SU);
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
  finalizeCurrentBundle(Top);
  finalizeCurrentBundle(Bot);
  handleRegionConflicts(ExitSU);
  leaveSchedulingZone(Top);
  leaveSchedulingZone(Bot);
  RegionBegin = nullptr;
  RegionEnd = nullptr;
  IsBottomRegion = false;
}

bool AIEPostRASchedStrategy::checkInterZoneConflicts() const {
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
  for (const AIE::MachineBundle &Bundle : reverse(BotHazardRec->getBundles())) {
    for (MachineInstr *MI : Bundle.getInstrs()) {
      SUnit *SU = DAG->getSUnit(MI);
      if (SU->TopReadyCycle > CurTopCycle)
        return true;
    }
    ++CurTopCycle;
  }
  return false;
}

void AIEPostRASchedStrategy::handleRegionConflicts(const SUnit &ExitSU) {

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
  while (checkInterZoneConflicts()) {
    LLVM_DEBUG(dbgs() << "** checkInterZoneConflicts: Bump current cycle\n");
    Top.bumpCycle(Top.getCurrCycle() + 1);
  }
}

void AIEPostRASchedStrategy::leaveSchedulingZone(SchedBoundary &Zone) {
  AIEHazardRecognizer *HazardRec = getAIEHazardRecognizer(Zone);
  if (!HazardRec) {
    return;
  }

  LLVM_DEBUG(dbgs() << "  NumBundles=" << HazardRec->getBundles().size()
                    << "\n");

  // Re-order bundles and instructions within so they appear in the same order
  // as in their parent basic block. This canonical order is required by
  // applyBundles, and facilitates generic NOP insertion.
  std::vector<AIE::MachineBundle> OrderedBundles = HazardRec->getBundles();
  if (!Zone.isTop()) {
    std::reverse(OrderedBundles.begin(), OrderedBundles.end());
    for (AIE::MachineBundle &Bundle : OrderedBundles) {
      std::reverse(Bundle.Instrs.begin(), Bundle.Instrs.end());
    }
  }

  // Contrary to PRAS, the MachineScheduler does not automatically insert NOPs.
  // That isn't a problem, since the callbacks to the HazardRecognizer were a
  // bit flaky (e.g. when to call emitNoop vs advanceCycle).
  // MachineScheduler just calls advanceCycle, and this is enough for us to
  // insert NOPs because the sequence of Bundles gives us the full picture.
  const TargetInstrInfo *TII = getTII(CurMBB);
  auto It = RegionBegin;
  for (AIE::MachineBundle &Bundle : OrderedBundles) {
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

  HazardRec->getBundles().clear();
}

AIEHazardRecognizer *
AIEPostRASchedStrategy::getAIEHazardRecognizer(const SchedBoundary &Zone) {
  return static_cast<AIEHazardRecognizer *>(Zone.HazardRec);
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
  if (Cand.Policy.ReduceLatency && tryLatency(TryCand, Cand, Zone)) {
    return TryCand.Reason != NoCand;
  }

  // Fall through to original instruction order.
  if ((Zone.isTop() && TryCand.SU->NodeNum < Cand.SU->NodeNum) ||
      (!Zone.isTop() && TryCand.SU->NodeNum > Cand.SU->NodeNum)) {
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

  finalizeCurrentBundle(Bot);

  if (InsertCycleSeparators) {
    AIEHazardRecognizer &HazardRec = *getAIEHazardRecognizer(Bot);
    auto *TII = static_cast<const AIEBaseInstrInfo *>(
        CurMBB->getParent()->getSubtarget().getInstrInfo());
    auto It = RegionBegin;
    for (AIE::MachineBundle &Bundle : reverse(HazardRec.getBundles())) {
      if (!Bundle.empty())
        It = std::next(Bundle.getInstrs().front()->getIterator());
      BuildMI(*CurMBB, It, DebugLoc(),
              TII->get(TII->getCycleSeparatorOpcode()));
    }
  }

  CurMBB = nullptr;
  RegionBegin = nullptr;
  RegionEnd = nullptr;
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
  // correctness.
  if (RegionInstrs == 1) {
    LLVM_DEBUG(dbgs() << "Force scheduling for single instruction\n");
    ScheduleDAGMI::schedule();
  }
}

void AIEScheduleDAGMI::exitRegion() {
  // AIEPostRASchedStrategy doesn't get callbacks from the MachineScheduler
  // to enter/exit regions. Let's give it some.
  getSchedImpl()->leaveRegion(ExitSU);
  ScheduleDAGMI::exitRegion();
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
}

void AIEScheduleDAGMILive::exitRegion() {
  // AIEPreRASchedStrategy doesn't get callbacks from the MachineScheduler
  // to enter/exit regions. Let's give it some.
  static_cast<AIEPreRASchedStrategy *>(SchedImpl.get())->leaveRegion(ExitSU);
  ScheduleDAGMILive::exitRegion();
}
