//===- AIEInterBlockScheduling.cpp - Inter-block scheduling infrastructure ===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// Implementations of the classes used to support inter-block scheduling
//===----------------------------------------------------------------------===//

#include "AIEInterBlockScheduling.h"
#include "AIEBaseInstrInfo.h"
#include "AIEBaseSubtarget.h"
#include "AIEBundle.h"
#include "AIEHazardRecognizer.h"
#include "AIELiveRegs.h"
#include "AIEMachineScheduler.h"
#include "AIEMaxLatencyFinder.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/Support/ErrorHandling.h"
#include <memory>

#define DEBUG_TYPE "machine-scheduler"

// These are more specific debug classes, separating the function and block
// level logging from the detailed scheduling info.
// useful combis:
// --debug-only=sched-blocks,loop-aware
// --debug-only=sched-blocks,machine-scheduler
#define DEBUG_LOOPAWARE(X) DEBUG_WITH_TYPE("loop-aware", X)
#define DEBUG_BLOCKS(X) DEBUG_WITH_TYPE("sched-blocks", X)

using namespace llvm;

static cl::opt<bool>
    LoopAware("aie-loop-aware", cl::init(true),
              cl::desc("[AIE] Schedule single block loops iteratively"));

static cl::opt<bool> LoopEpilogueAnalysis(
    "aie-loop-epilogue-analysis", cl::init(true),
    cl::desc("[AIE] Perform Loop/Epilogue analysis with loop scheduling"));

static cl::opt<int> MaxExpensiveIterations(
    "aie-loop-aware-expensive-iterations", cl::init(35),
    cl::desc("[AIE] Maximum iterations for fine-grained convergence in "
             "iterative loop scheduling"));

static cl::opt<bool>
    BiasDepth("aie-loop-aware-bias-depth", cl::init(true),
              cl::desc("[AIE] Try to bias the depth for hazard avoidance in "
                       "iterative loop scheduling"));

static cl::opt<int> PostPipelinerMaxII(
    "aie-postpipeliner-maxii", cl::init(40),
    cl::desc("[AIE] Maximum II to be tried in the post-ra pipeliner"));

namespace llvm::AIE {

void dumpInterBlock(const InterBlockEdges &Edges) {
  for (const SUnit &SU : Edges) {
    dbgs() << "SU" << SU.NodeNum << ": " << *SU.getInstr();
  }
}

ResourceScoreboard<FuncUnitWrapper>
createTopDownScoreboard(ArrayRef<MachineBundle> Bundles,
                        const AIEHazardRecognizer &HR,
                        const AIEAlternateDescriptors &SelectedDescriptors) {
  ResourceScoreboard<FuncUnitWrapper> Scoreboard;
  Scoreboard.reset(HR.getMaxLookAhead());

  const int TotalBundles = Bundles.size();
  const int AmountToEmit = std::min(TotalBundles, HR.getConflictHorizon());
  // Do not emit more than the specified by the conflict horizon. More
  // then this will not cause conflicts.
  for (int I = TotalBundles - AmountToEmit; I < TotalBundles; I++) {
    for (MachineInstr *MI : Bundles[I].getInstrs())
      HR.emitInScoreboard(Scoreboard, *SelectedDescriptors.getDesc(MI),
                          HR.getMemoryBanks(MI), MI->operands(),
                          MI->getMF()->getRegInfo(), 0);
    Scoreboard.advance();
  }

  DEBUG_LOOPAWARE(dbgs() << "*** Emitted " << TotalBundles << " top-down\n");

  // If an iteration contains less bundles than the number of resources that
  // stick out into the next one, this means that the first cycles of the
  // scoreboard could potentially be "clobbered" by previous iterations.
  // We conservatively block those cycles.
  const int MaxResourceExtent = Scoreboard.lastOccupied();
  assert(MaxResourceExtent <= HR.getConflictHorizon());
  if (MaxResourceExtent > AmountToEmit) {
    const int NumBlockedCycles = MaxResourceExtent - AmountToEmit;
    const int FirstBlockedCycle = -AmountToEmit;
    const int LastBlockedCycle = FirstBlockedCycle + NumBlockedCycles - 1;
    for (int C = FirstBlockedCycle; C <= LastBlockedCycle; ++C) {
      Scoreboard[C].blockResources();
    }
  }

  return Scoreboard;
}

ResourceScoreboard<FuncUnitWrapper>
createBottomUpScoreboard(ArrayRef<MachineBundle> Bundles,
                         const AIEHazardRecognizer &HR,
                         const AIEAlternateDescriptors &SelectedDescriptors) {
  const unsigned NumBundles = Bundles.size();
  const unsigned RequiredCycles = HR.getConflictHorizon();

  ResourceScoreboard<FuncUnitWrapper> Scoreboard;
  Scoreboard.reset(HR.getMaxLookAhead());

  // We have less known bundles than the minimum number required for
  // correctness. Conservatively block some cycles.
  if (NumBundles < RequiredCycles) {
    unsigned CyclesToBlock = RequiredCycles - Bundles.size();
    for (unsigned Num = 0; Num < CyclesToBlock; ++Num) {
      Scoreboard[0].blockResources();
      Scoreboard.recede();
    }
  }

  // Do not emit more Bundles than required for correctness.
  ArrayRef<MachineBundle> MinBundles(
      Bundles.begin(), Bundles.begin() + std::min(NumBundles, RequiredCycles));
  for (const MachineBundle &B : reverse(MinBundles)) {
    for (MachineInstr *MI : B.getInstrs())
      HR.emitInScoreboard(Scoreboard, *SelectedDescriptors.getDesc(MI),
                          HR.getMemoryBanks(MI), MI->operands(),
                          MI->getMF()->getRegInfo(), 0);
    Scoreboard.recede();
  }
  return Scoreboard;
}

/// Replay the \p PredBundles bottom-up into \p ScoreBoard.
/// If that causes a resource conflict, return the instruction
/// from \p PredBundles that is responsible for it.
MachineInstr *checkResourceConflictsBottomUp(
    const ResourceScoreboard<FuncUnitWrapper> &Scoreboard,
    const std::vector<MachineBundle> &PredBundles,
    const AIEHazardRecognizer &HR,
    const AIEAlternateDescriptors &SelectedDescriptors) {
  DEBUG_LOOPAWARE(dbgs() << "Interblock Successor scoreboard:\n";
                  Scoreboard.dump());

  int BottomUpCycle = 0;
  for (const MachineBundle &B : reverse(PredBundles)) {
    if (BottomUpCycle >= HR.getConflictHorizon())
      break;
    for (MachineInstr *MI : B.getInstrs()) {
      if (HR.getHazardType(Scoreboard, *SelectedDescriptors.getDesc(MI),
                           HR.getMemoryBanks(MI), MI->operands(),
                           MI->getMF()->getRegInfo(), -BottomUpCycle)) {
        DEBUG_LOOPAWARE(dbgs() << "Conflicting MI at Bottom-up cycle="
                               << BottomUpCycle << ": " << *MI);
        return MI;
      }
    }
    ++BottomUpCycle;
  }

  // All instructions in Bot could be emitted straight after those in Top.
  return nullptr;
}

/// Replay the \p SuccBundles top-down into \p ScoreBoard.
/// If that causes a resource conflict, return an instruction
/// from \p SuccBundles that is responsible for it.
/// Note that \p Scoreboard will be modified.
MachineInstr *checkResourceConflictsTopDown(
    ResourceScoreboard<FuncUnitWrapper> &Scoreboard,
    const std::vector<MachineBundle> &SuccBundles,
    const AIEHazardRecognizer &HR,
    const AIEAlternateDescriptors &SelectedDescriptors,
    const FixedpointState &Fixedpoint) {
  DEBUG_LOOPAWARE(dbgs() << "Interblock Predecessor scoreboard:\n";
                  Scoreboard.dump());

  int TopCycle = 0;
  MachineInstr *ConflictMI = nullptr;
  for (const MachineBundle &B : SuccBundles) {
    for (MachineInstr *MI : B.getInstrs()) {
      if (HR.getHazardType(Scoreboard, *SelectedDescriptors.getDesc(MI),
                           HR.getMemoryBanks(MI), MI->operands(),
                           MI->getMF()->getRegInfo(), 0)) {
        DEBUG_LOOPAWARE(dbgs() << "Conflicting MI at Top cycle=" << TopCycle
                               << ": " << *MI);
        ConflictMI = MI;
        if (!Fixedpoint.PerMIExtraDepth.contains(MI))
          return MI;
      }
    }
    ++TopCycle;
    Scoreboard.advance();
  }

  return ConflictMI;
}

MachineBasicBlock *getLoopPredecessor(const MachineBasicBlock &MBB) {
  if (MBB.pred_size() == 1) {
    // if we have only one, it must be the loop
    return *MBB.predecessors().begin();
  }
  // Otherwise, the loop is the fallthrough predecessor by construction
  for (auto *Pred : MBB.predecessors()) {
    if (Pred->isLayoutSuccessor(&MBB)) {
      return Pred;
    }
  }
  return nullptr;
}

InterBlockScheduling::InterBlockScheduling(const MachineSchedContext *C,
                                           bool InterBlock)
    : Context(C), InterBlockScoreboard(InterBlock) {}

// This sets up the scheduling mode for each block. It defines which
// CFG edges will be prioritized for interblock scheduling and which blocks
// should take care of the latency repair.
void InterBlockScheduling::markEpilogueBlocks() {
  // Mark up the epilogues of the Loops we have found
  for (auto &[MBB, BS] : Blocks) {
    if (BS.Kind != BlockType::Loop) {
      continue;
    }
    llvm::for_each(MBB->successors(), [L = MBB, this](auto *S) {
      if (S != L) {
        getBlockState(S).Kind = BlockType::Epilogue;
      }
    });
  }
}

void InterBlockScheduling::enterFunction(MachineFunction *MF) {
  DEBUG_BLOCKS(dbgs() << ">> enterFunction " << MF->getName() << "\n");

  // Get ourselves a hazard recognizer
  const auto &Subtarget = MF->getSubtarget();
  HR = std::make_unique<AIEHazardRecognizer>(Subtarget, SelectedAltDescs);

  // And a native InstrInfo
  TII = static_cast<const AIEBaseInstrInfo *>(Subtarget.getInstrInfo());

  const TargetRegisterInfo *TRI = MF->getSubtarget().getRegisterInfo();
  LiveRegs MBBLiveness(MF);
  const std::map<const MachineBasicBlock *, LivePhysRegs> &LiveIns =
      MBBLiveness.getLiveIns();

  // Define our universe of blocks
  for (MachineBasicBlock &MBB : *MF) {
    auto Itr = Blocks.emplace(&MBB, &MBB).first;
    BlockState &BS = Itr->second;
    BS.LiveOuts.init(*TRI);
    // Calculating LiveOuts by iterating over each successor of the MBB and
    // adding each successor's LiveIns to LiveOuts.
    for (const MachineBasicBlock *Succ : MBB.successors()) {
      const LivePhysRegs &MBBLiveins = LiveIns.at(Succ);
      for (const MCPhysReg Reg : MBBLiveins) {
        BS.LiveOuts.addReg(Reg);
      }
    }
    LLVM_DEBUG({
      dbgs() << MBB.getFullName() << " LiveOuts\n";
      BS.LiveOuts.dump();
    });
  }
  if (LoopAware) {
    // Mark epilogues of the loops we found. This is only necessary if
    // we have created Loops in the first place, as indicated by LoopAware.
    markEpilogueBlocks();
  }

  defineSchedulingOrder(MF);
}

void InterBlockScheduling::leaveFunction() {
  DEBUG_BLOCKS(dbgs() << "<< leaveFunction\n");
  Blocks.clear();
}

void InterBlockScheduling::enterBlock(MachineBasicBlock *BB) {
  CurrentBlockState = &getBlockState(BB);
  CurrentBlockState->resetRegion();
  DEBUG_BLOCKS(dbgs() << "  >> enterBlock " << BB->getNumber() << " "
                      << CurrentBlockState->kindAsString() << " FixPointIter="
                      << CurrentBlockState->FixPoint.NumIters
                      << " II=" << CurrentBlockState->FixPoint.II << "\n");
}
namespace {
/// This implements the interface to the postpipeliner to extract the
/// prologue, steady state and epilogue of the pipelined loop
/// Each of these code segments is collected in TimedRegion, and then copied
/// into the appropriate blockstate region.
/// TimedRegion is built one bundle at the time
class PipelineExtractor : public PipelineScheduleVisitor {
  BlockState &Loop;
  BlockState *Prologue = nullptr;
  BlockState *Epilogue = nullptr;
  MachineBundle CurrentBundle;
  std::vector<MachineBundle> TimedRegion;
  bool InLoop = false;

  void startPrologue() override {
    // Nothing at this time, but let's keep the override around
  }
  void startLoop() override {
    assert(Prologue->BottomInsert.empty() &&
           "PreHeader already has a timed region at Bottom.");
    Prologue->BottomInsert = TimedRegion;
    TimedRegion.clear();
    InLoop = true;
  }
  void startEpilogue() override {
    Loop.getTop().Bundles = TimedRegion;
    TimedRegion.clear();
    InLoop = false;
  }
  void finish() override {
    assert(Epilogue->TopInsert.empty() &&
           "Epilogue already has a timed region at Top.");
    Epilogue->TopInsert = TimedRegion;
    TimedRegion.clear();
  }
  void startBundle() override { CurrentBundle.clear(); }
  void addToBundle(MachineInstr *MI) override {
    // We re-emit the original instructions into the loop body.
    // Prologue and epilogue obtain copies.
    MachineInstr *ToBeEmitted =
        InLoop ? MI : Loop.TheBlock->getParent()->CloneMachineInstr(MI);
    CurrentBundle.add(ToBeEmitted);
  }
  void endBundle() override { TimedRegion.emplace_back(CurrentBundle); }

public:
  PipelineExtractor(InterBlockScheduling &InterBlock, BlockState &BS,
                    const AIEBaseInstrInfo &TII)
      : Loop(BS), CurrentBundle(TII.getFormatInterface()) {
    MachineBasicBlock *LoopBlock = Loop.TheBlock;
    for (auto *P : LoopBlock->predecessors()) {
      if (P == LoopBlock) {
        continue;
      }
      Prologue = &InterBlock.getBlockState(P);
    }
    for (auto *S : LoopBlock->successors()) {
      if (S == LoopBlock) {
        continue;
      }
      Epilogue = &InterBlock.getBlockState(S);
    }
  }
};

} // namespace
bool InterBlockScheduling::leaveBlock() {
  DEBUG_BLOCKS(dbgs() << "  << leaveBlock "
                      << CurrentBlockState->TheBlock->getNumber() << "\n");
  // After scheduling a basic block, check convergence to determine which block
  // to schedule next and with what parameters
  auto &BS = *CurrentBlockState;
  switch (updateFixPoint(BS)) {
  case SchedulingStage::GatheringRegions:
  case SchedulingStage::SchedulingNotConverged:
  case SchedulingStage::Scheduling:
  case SchedulingStage::Pipelining:
    // Iterate on CurrentBlock
    // When Scheduling, we have increased the latency margin, at first
    // per instruction, later for all instructions at once.
    // When Pipelining, we have incremented II
    return false;
  case SchedulingStage::PipeliningDone: {
    // When pipelined, we need to materialize the schedule
    BS.clearSchedule();
    PipelineExtractor GenSchedule(*this, BS, *TII);
    auto &PostSWP = BS.getPostSWP();
    PostSWP.visitPipelineSchedule(GenSchedule);
    PostSWP.updateTripCount();
    break;
  }
  case SchedulingStage::SchedulingDone:
  case SchedulingStage::PipeliningFailed:
    break;
  }

  CurrentBlockState = nullptr;
  return true;
}

MachineInstr *
InterBlockScheduling::resourcesConverged(BlockState &BS,
                                         bool FindInBottomRegion) const {
  assert(!BS.getRegions().empty());

  // We are a single-block loop body. Check that there is no resource conflict
  // on the backedge, by overlaying top and bottom region
  if (FindInBottomRegion) {
    if (MachineInstr *MICausingConflict = checkResourceConflictsBottomUp(
            createBottomUpScoreboard(BS.getTop().Bundles, *HR,
                                     SelectedAltDescs),
            BS.getBottom().Bundles, *HR, SelectedAltDescs))
      return MICausingConflict;
  }

  // Bottom represents the resources that are sticking out of the block.
  // The last non-empty cycle is a safe upperbound for the resource
  // safety margin.
  ResourceScoreboard<FuncUnitWrapper> Bottom =
      createTopDownScoreboard(BS.getBottom().Bundles, *HR, SelectedAltDescs);
  BS.FixPoint.MaxResourceExtent = Bottom.lastOccupied();

  if (!FindInBottomRegion) {
    if (MachineInstr *MICausingConflict = checkResourceConflictsTopDown(
            Bottom, BS.getTop().Bundles, *HR, SelectedAltDescs, BS.FixPoint))
      return MICausingConflict;
  }

  return nullptr;
}

MachineInstr *InterBlockScheduling::latencyConverged(BlockState &BS) const {
  const auto &SubTarget = BS.TheBlock->getParent()->getSubtarget();
  auto *TII = static_cast<const AIEBaseInstrInfo *>(SubTarget.getInstrInfo());
  auto *ItinData = SubTarget.getInstrItineraryData();

  assert(!BS.getRegions().empty());

  // BackEdges represents all dependence edges that span the loop edge
  // We will iterate over all backedge dependences by running over the
  // SUnits connected to instructions in the bottom bundles and check
  // successor SUnits to be in the Top region (using a boundary check)
  // If the successor is in Top, we lookup its depth in TopDepth
  const Region &Bottom = BS.getBottom();
  const Region &Top = BS.getTop();
  const InterBlockEdges &BackEdges = BS.getBoundaryEdges();

  // Record the depth of all instructions in Top. Don't record the ones that
  // can't cause problems
  std::map<MachineInstr *, int> TopDepth;
  int Depth = 0;
  for (auto &Bundle : Top.Bundles) {
    for (auto *MI : Bundle.getInstrs()) {
      TopDepth[MI] = Depth;
    }
    if (++Depth > HR->getConflictHorizon()) {
      break;
    }
  }

  // Now check all inter-block edges. We prune by checking whether
  // max latency reaches the successor at all
  int MaxExtent = 0;
  int Height = 1;
  for (auto &Bundle : reverse(Bottom.Bundles)) {
    DEBUG_LOOPAWARE(dbgs() << "--- Height=" << Height << "---\n");
    for (auto *MI : Bundle.getInstrs()) {
      int Extending = AIE::maxLatency(MI, *TII, *ItinData, false) - Height;
      DEBUG_LOOPAWARE(dbgs()
                      << "Extending=" << Extending << " " << *MI << "\n");
      if (Extending <= 0) {
        continue;
      }
      MaxExtent = std::max(MaxExtent, Extending);
      const SUnit *Pred = BackEdges.getPreBoundaryNode(MI);
      for (auto &SDep : Pred->Succs) {
        auto *Succ = SDep.getSUnit();
        if (!BackEdges.isPostBoundaryNode(Succ)) {
          continue;
        }
        DEBUG_LOOPAWARE(dbgs() << "  Backedge to " << Succ->NodeNum << "\n");
        auto DepthIt = TopDepth.find(Succ->getInstr());
        if (DepthIt == TopDepth.end()) {
          // Over the horizon
          continue;
        }
        DEBUG_LOOPAWARE(dbgs() << "  Depth=" << DepthIt->second << "\n");
        int Latency = SDep.getSignedLatency();
        int Distance = Height + DepthIt->second;
        if (Distance < Latency) {
          DEBUG_LOOPAWARE(dbgs() << "  Latency(" << Pred->NodeNum << "->"
                                 << Succ->NodeNum << ")=" << Latency
                                 << " not met (" << Distance << ")\n");
          DEBUG_LOOPAWARE(dbgs() << "  " << Succ->NodeNum << ": "
                                 << *Succ->getInstr());
          return Pred->getInstr();
        }
      }
    }
    if (++Height > HR->getConflictHorizon()) {
      break;
    }
  }

  // MaxExtent tracks anything sticking out of the block, so is a safe
  // upperbound of the latency safety margin that should be provided by
  // the epilogue
  BS.FixPoint.MaxLatencyExtent = MaxExtent;
  return nullptr;
}

SchedulingStage InterBlockScheduling::updateFixPoint(BlockState &BS) {
  if (BS.Kind != BlockType::Loop) {
    return BS.FixPoint.Stage = SchedulingStage::SchedulingDone;
  }

  if (BS.FixPoint.Stage == SchedulingStage::GatheringRegions) {
    // This is the first time we schedule this loop. In that first
    // iteration, we have recorded the region decomposition.
    // Now we can create the interblock edges between the top and the bottom
    // region
    BS.initInterBlock(*Context, *HR);
    return BS.FixPoint.Stage = SchedulingStage::Scheduling;
  }

  BS.FixPoint.NumIters++;
  if (BS.FixPoint.Stage == SchedulingStage::Scheduling) {
    return updateScheduling(BS);
  }

  return updatePipelining(BS);
}

SchedulingStage InterBlockScheduling::updateScheduling(BlockState &BS) {
  if (BS.FixPoint.NumIters >
      MaxExpensiveIterations + 2 * HR->getConflictHorizon()) {
    report_fatal_error("Inter-block scheduling did not converge.");
    return BS.FixPoint.Stage = SchedulingStage::SchedulingNotConverged;
  }

  if (MachineInstr *MINeedsHigherCap = latencyConverged(BS)) {
    auto Res = BS.FixPoint.PerMILatencyMargin.try_emplace(MINeedsHigherCap, 0);
    // Increase the latency margin per instruction, unless we already iterated
    // more than MaxExpensiveIterations without converging.
    if (BS.FixPoint.NumIters <= MaxExpensiveIterations) {
      ++Res.first->second;
    } else {
      BS.FixPoint.LatencyMargin++;
    }
    DEBUG_LOOPAWARE(dbgs() << "  not converged: latency RM="
                           << BS.FixPoint.ResourceMargin
                           << " LM=" << BS.FixPoint.LatencyMargin
                           << " MIM=" << Res.first->second << "\n");
    // Iterate on CurMBB
    return BS.FixPoint.Stage = SchedulingStage::Scheduling;
  }

  // Before pushing BS.getBottom() instructions up to avoid resource hazards,
  // try and bias the depth of some instructions in BS.getTop()
  if (BiasDepth && BS.FixPoint.NumIters <= MaxExpensiveIterations) {
    if (MachineInstr *MINeedsHigherCap =
            resourcesConverged(BS, /*FindInBottomRegion=*/false);
        InterBlockScoreboard && MINeedsHigherCap) {
      auto Res = BS.FixPoint.PerMIExtraDepth.try_emplace(MINeedsHigherCap, 1);
      int &ExtraDepth = Res.first->second;
      if (ExtraDepth >= 0) {
        if (!Res.second) // Depth was already biased, try a negative bias
          ExtraDepth = -1;
        DEBUG_LOOPAWARE(dbgs() << "  not converged: resources ExtraDepth="
                               << ExtraDepth << "\n");
        // Iterate on CurMBB
        return BS.FixPoint.Stage = SchedulingStage::Scheduling;
      }
      DEBUG_LOOPAWARE(dbgs() << "  not converged: Depth biasing failed\n");
    }
  }

  // If biasing did not help, actively push instructions from BS.getBottom() up.
  if (MachineInstr *MINeedsHigherCap =
          resourcesConverged(BS, /*FindInBottomRegion=*/true);
      InterBlockScoreboard && MINeedsHigherCap) {
    auto Res = BS.FixPoint.PerMILatencyMargin.try_emplace(MINeedsHigherCap, 0);
    if (BS.FixPoint.NumIters <= MaxExpensiveIterations) {
      ++Res.first->second;
    } else {
      BS.FixPoint.PerMIExtraDepth.clear();
      BS.FixPoint.ResourceMargin++;
    }
    DEBUG_LOOPAWARE(dbgs() << "  not converged: resources RM="
                           << BS.FixPoint.ResourceMargin
                           << " LM=" << BS.FixPoint.LatencyMargin
                           << " MIM=" << Res.first->second << "\n");
    // Iterate on CurMBB
    return BS.FixPoint.Stage = SchedulingStage::Scheduling;
  }

  DEBUG_LOOPAWARE(dbgs() << "Converged,"
                         << " LatencyExtent=" << BS.FixPoint.MaxLatencyExtent
                         << " ResourceExtent=" << BS.FixPoint.MaxResourceExtent
                         << "\n");

  // The loop schedule has converged, so we could declare our work done.
  // But first try SWP
  if (BS.getRegions().size() == 1) {
    auto &PostSWP = BS.getPostSWP();
    if (PostSWP.canAccept(*BS.TheBlock)) {
      BS.FixPoint.II = PostSWP.getResMII(*BS.TheBlock);
      return BS.FixPoint.Stage = SchedulingStage::Pipelining;
    }
  }
  return BS.FixPoint.Stage = SchedulingStage::SchedulingDone;
}

SchedulingStage InterBlockScheduling::updatePipelining(BlockState &BS) {
  // We have been pipelining. Check whether we were successful.
  if (BS.FixPoint.Stage == SchedulingStage::PipeliningDone) {
    return BS.FixPoint.Stage;
  }

  // Otherwise try a larger II.
  // We cut off at larger IIs to prevent excessive compilation time.
  if (++BS.FixPoint.II <= PostPipelinerMaxII) {
    return BS.FixPoint.Stage = SchedulingStage::Pipelining;
  }

  // Fall back to the loop schedule. Note that we can only have II != 0
  // after the loop schedule has stabilized.
  return BS.FixPoint.Stage = SchedulingStage::PipeliningFailed;
}

bool InterBlockScheduling::successorsAreScheduled(
    MachineBasicBlock *MBB) const {
  return llvm::all_of(MBB->successors(), [&](MachineBasicBlock *B) {
    const auto &BS = getBlockState(B);
    return BS.isScheduled();
  });
}

std::optional<int> InterBlockScheduling::getLatencyCap(MachineInstr &MI) const {
  auto &BS = getBlockState(MI.getParent());
  if (BS.Kind != BlockType::Loop) {
    return {};
  }
  if (BS.FixPoint.LatencyMargin)
    return BS.FixPoint.LatencyMargin;
  if (const auto *It = BS.FixPoint.PerMILatencyMargin.find(&MI);
      It != BS.FixPoint.PerMILatencyMargin.end()) {
    return It->second;
  }
  return 0;
}

std::optional<int>
InterBlockScheduling::getBlockedResourceCap(MachineBasicBlock *BB) const {
  auto &BS = getBlockState(BB);
  if (BS.Kind != BlockType::Loop) {
    return {};
  }
  return BS.FixPoint.ResourceMargin;
}

void InterBlockScheduling::defineSchedulingOrder(MachineFunction *MF) {
  // First do the (single-block) loops. We don't want these to be constrained
  // by their epilogues
  for (auto &[MBB, BS] : Blocks) {
    if (BS.Kind == BlockType::Loop) {
      MBBSequence.push_back(MBB);
    }
  }

  // Then the rest in postorder to optimize the number of already scheduled
  // successors
  for (MachineBasicBlock *MBB : post_order(MF)) {
    if (getBlockState(MBB).Kind != BlockType::Loop) {
      MBBSequence.push_back(MBB);
    }
  }

  // Now initialize the index to the start.
  NextInOrder = 0;
  DEBUG_BLOCKS(dbgs() << "MBB scheduling sequence : ";
               for (const auto &MBBSeq
                    : MBBSequence) dbgs()
               << MBBSeq->getNumber() << " -> ";
               dbgs() << "\n";);

  assert(MF->size() == MBBSequence.size() &&
         "Missing MBB in scheduling sequence");
}

MachineBasicBlock *InterBlockScheduling::nextBlock() {
  auto &BS = getBlockState(MBBSequence[NextInOrder]);
  if (!BS.isScheduled() ||
      (BS.FixPoint.II && !BS.isPipelined() && !BS.pipeliningFailed())) {
    return MBBSequence[NextInOrder];
  }

  ++NextInOrder;
  if (NextInOrder >= MBBSequence.size()) {
    return nullptr;
  }
  return MBBSequence[NextInOrder];
}

const BlockState &
InterBlockScheduling::getBlockState(MachineBasicBlock *BB) const {
  return Blocks.at(BB);
}

BlockState &InterBlockScheduling::getBlockState(MachineBasicBlock *BB) {
  return Blocks.at(BB);
}

void InterBlockScheduling::enterRegion(MachineBasicBlock *BB,
                                       MachineBasicBlock::iterator RegionBegin,
                                       MachineBasicBlock::iterator RegionEnd) {
  auto &BS = getBlockState(BB);
  DEBUG_BLOCKS(dbgs() << "    >> enterRegion, Iter=" << BS.FixPoint.NumIters
                      << "\n");

  // Only add regions of loops when in the GatheringRegions phase
  if (BS.Kind != BlockType::Loop ||
      BS.FixPoint.Stage == SchedulingStage::GatheringRegions) {
    ArrayRef<MachineBundle> TopFixedBundles;
    ArrayRef<MachineBundle> BotFixedBundles;
    BS.addRegion(BB, RegionBegin, RegionEnd, TopFixedBundles, BotFixedBundles);
  }
}

// Create a block, insert it before Succ, and route the control flow edge
// between Pred and Succ through it.
// Since we don't add any control flow instructions, the edge should be a
// fallthrough edge; it will be replaced with two fallthrough edges and a block
MachineBasicBlock *splitEdge(MachineBasicBlock *Pred, MachineBasicBlock *Succ) {
  auto *MF = Pred->getParent();
  MachineBasicBlock *NewBB = MF->CreateMachineBasicBlock(Succ->getBasicBlock());
  MF->insert(Succ->getIterator(), NewBB);
  for (auto *Edge : make_early_inc_range(Pred->successors())) {
    if (Edge == Succ) {
      Pred->removeSuccessor(Succ);
    }
  }
  NewBB->addSuccessor(Succ);
  Pred->addSuccessor(NewBB);
  return NewBB;
}

int InterBlockScheduling::getSafetyMargin(MachineBasicBlock *Loop,
                                          MachineBasicBlock *Epilogue) const {
  auto &LBS = getBlockState(Loop);
  auto &EBS = getBlockState(Epilogue);

  // We can only analyze non-empty epilogue blocks because we need
  // to build a DDG, which is not possible.
  // For empty ones, we need to be conservative because we are not aware of
  // content of epilogues' successor.
  int SafetyMargin = LBS.getSafetyMargin();
  if (LoopEpilogueAnalysis && Epilogue->size() > 0) {
    int ExistingLatency = getCyclesToRespectTiming(EBS, LBS);
    // Start the next step only after clearing latencies.
    SafetyMargin = getCyclesToAvoidResourceConflicts(ExistingLatency, EBS, LBS);
  }

  return SafetyMargin;
}

void InterBlockScheduling::emitBundles(
    const std::vector<MachineBundle> &Bundles, MachineBasicBlock *BB,
    MachineBasicBlock::iterator Before, bool Move, bool EmitNops) const {
  for (auto &Bundle : Bundles) {
    if (Bundle.empty()) {
      if (EmitNops)
        TII->insertNoop(*BB, Before);
      else {
        DebugLoc DL;
        BuildMI(*BB, Before, DL, TII->get(TargetOpcode::BUNDLE));
      }
      continue;
    }
    for (auto *MI : Bundle.getInstrs()) {
      if (Move) {
        BB->remove_instr(MI);
      }
      BB->insert(Before, MI);
    }
  }
  AIEHazardRecognizer::applyBundles(Bundles, BB);
}

void InterBlockScheduling::emitInterBlockTop(const BlockState &BS) const {
  if (BS.Kind != BlockType::Epilogue) {
    return;
  }

  MachineBasicBlock *BB = BS.TheBlock;
  MachineBasicBlock *Loop = getLoopPredecessor(*BB);
  assert(Loop);
  const BlockState &LBS = getBlockState(Loop);

  // Epilogues should supply the safety margin for their loop.
  // Epilogues of pipelined loops should emit the bundles swp epilog
  // Both need a dedicated exit. If there isn't one, spawn a new block
  auto MakeDedicated = [Loop](MachineBasicBlock *BB) {
    auto *DedicatedExit = BB;
    if (BB->pred_size() > 1) {
      // The loop is a fallthrough predecessor by construction. We insert a
      // new block that will be a dedicated exit to the loop.
      DEBUG_LOOPAWARE(dbgs() << "New dedicated exit\n");
      DedicatedExit = splitEdge(Loop, BB);
    }
    return DedicatedExit;
  };

  if (LBS.isPipelined()) {
    auto *DedicatedExit = MakeDedicated(BB);
    emitBundles(BS.TopInsert, DedicatedExit, DedicatedExit->begin(),
                /*Move=*/false, /*EmitNops=*/true);
  } else {
    if (int SafetyMargin = getSafetyMargin(Loop, BB)) {
      auto *DedicatedExit = MakeDedicated(BB);
      DEBUG_LOOPAWARE(dbgs()
                      << "Emitting " << SafetyMargin << " safety nops\n");
      while (SafetyMargin--) {
        TII->insertNoop(*DedicatedExit, DedicatedExit->begin());
      }
    }
  }
}

void InterBlockScheduling::emitInterBlockBottom(const BlockState &BS) const {
  if (BS.BottomInsert.empty()) {
    return;
  }
  MachineBasicBlock *PreHeader = BS.TheBlock;
  assert(PreHeader->end() == PreHeader->getFirstTerminator() &&
         "PreHeader is not fall-through");
  emitBundles(BS.BottomInsert, PreHeader, PreHeader->end(), /*Move=*/false,
              /*EmitNops=*/true);
}

int InterBlockScheduling::getCyclesToRespectTiming(
    const BlockState &EpilogueBS, const BlockState &LoopBS) const {

  const MachineBasicBlock &EpilogueMBB = *EpilogueBS.TheBlock;

  DEBUG_LOOPAWARE(dbgs() << "** Loop/Epilogue-carried latency dependencies:"
                         << " Original Loop " << *LoopBS.TheBlock
                         << " Original Epilogue " << EpilogueMBB << "\n");

  InterBlockEdges Edges(*Context);
  std::map<const MachineInstr *, int> DistancesFromLoopEntry;
  int DistFromLoopEntry = 0;
  int EntryNops = 0;

  auto AddRegionToEdges = [&](const Region &R) {
    for (auto &Bundle : R.Bundles) {
      for (MachineInstr *MI : Bundle.getInstrs()) {
        DistancesFromLoopEntry[MI] = DistFromLoopEntry;
      }
      ++DistFromLoopEntry;
    }
    // Here we need to iterate using semantic order.
    assert(R.top_fixed_instrs().empty() && "SWP epilogue already emitted?");
    for (MachineInstr *MI : R.getFreeInstructions()) {
      Edges.addNode(MI);
    }
  };

  // Construction of the superblock containing Loop+Epilogue
  // First part is the loop
  AddRegionToEdges(LoopBS.getBottom());
  Edges.markBoundary();
  // Second part is the epilogue itself
  AddRegionToEdges(EpilogueBS.getTop());
  Edges.buildEdges();

  DEBUG_LOOPAWARE(dumpInterBlock(Edges));
  // Check cross-boundary latencies.
  int Height = 1;
  for (auto &Bundle : reverse(LoopBS.getBottom().Bundles)) {
    for (auto *PreBoundaryMI : Bundle.getInstrs()) {
      const SUnit *Pred = Edges.getPreBoundaryNode(PreBoundaryMI);

      for (auto &SDep : Pred->Succs) {
        auto *Succ = SDep.getSUnit();

        if (!Edges.isPostBoundaryNode(Succ))
          continue;

        const MachineInstr *PostBoundaryMI = Succ->getInstr();

        const int PostBoundOrExitDist =
            (PostBoundaryMI != nullptr)
                ? DistancesFromLoopEntry[PostBoundaryMI]
                // When getInstr returns nullptr, we reached
                // ExitSU. We can consider the DistFromLoopEntry as
                // depth of the ExitSU.
                : DistFromLoopEntry;

        const int Latency = SDep.getSignedLatency();
        const int Distance =
            PostBoundOrExitDist - DistancesFromLoopEntry[PreBoundaryMI];

        DEBUG_LOOPAWARE(dbgs() << "Data dependency found:\n"
                               << " Loop instruction SU: " << *PreBoundaryMI);
        DEBUG_LOOPAWARE(dbgs() << " Epilogue instruction: ";
                        if (PostBoundaryMI) PostBoundaryMI->dump();
                        else dbgs() << "nullptr (ExitSU)";);
        DEBUG_LOOPAWARE(dbgs() << "\n Latency: " << Latency
                               << "\n Distance: " << Distance << "\n");

        EntryNops = std::max(EntryNops, Latency - Distance);
      }
    }
    if (++Height > HR->getConflictHorizon()) {
      break;
    }
  }

  DEBUG_LOOPAWARE(
      dbgs() << "Timing constraints between loop and epilogue require "
             << EntryNops << " Nops\n");
  return EntryNops;
}

int InterBlockScheduling::getCyclesToAvoidResourceConflicts(
    int ExistingLatency, const BlockState &EpilogueBS,
    const BlockState &LoopBS) const {

  const MachineBasicBlock &EpilogueMBB = *EpilogueBS.TheBlock;
  MachineBasicBlock *LoopMBB = LoopBS.TheBlock;
  DEBUG_LOOPAWARE(dbgs() << "* Loop/Epilogue-carried resource conflicts:"
                         << " Original Loop " << *LoopMBB << " Original Epilog "
                         << EpilogueMBB << "\n");

  ResourceScoreboard<FuncUnitWrapper> Scoreboard = createBottomUpScoreboard(
      EpilogueBS.getTop().Bundles, *HR, SelectedAltDescs);

  // We know how many latency cycles we need to respect, and we can advance
  // the scoreboard to the first possible cycle that can accommodate another
  // instruction and start the resource verification from this point, tracking
  // the number of NOPS.
  int NopCounter = 0;
  for (NopCounter = 0; NopCounter < ExistingLatency; ++NopCounter)
    Scoreboard.recede();
  DEBUG_LOOPAWARE(dbgs() << "Epilogue scoreboard\n"; Scoreboard.dump());

  // Increment the number of intermediate nops until there are no resource
  // conflicts between the last iteration of the loop and the epilogue.
  while (checkResourceConflictsBottomUp(Scoreboard, LoopBS.getBottom().Bundles,
                                        *HR, SelectedAltDescs)) {
    Scoreboard.recede();
    ++NopCounter;
  }

  DEBUG_LOOPAWARE(dbgs() << "Resource conflict avoidance between"
                         << " loop: " << *LoopMBB
                         << " And epilogue: " << EpilogueMBB << " Requires "
                         << NopCounter << " Nops\n");

  return NopCounter;
}

void InterBlockEdges::addNode(MachineInstr *MI) {
  if (auto Index = DDG.initSUnit(*MI)) {
    IndexMap &TheMap = Boundary ? SuccMap : PredMap;
    TheMap.emplace(MI, *Index);
  }
}

// Mark the boundary between the predecessor block and the successor block
void InterBlockEdges::markBoundary() { Boundary = DDG.SUnits.size(); }

const SUnit *InterBlockEdges::getPreBoundaryNode(MachineInstr *MI) const {
  auto Found = PredMap.find(MI);
  if (Found == PredMap.end()) {
    return nullptr;
  }

  return &DDG.SUnits.at(Found->second);
}

bool InterBlockEdges::isPostBoundaryNode(SUnit *SU) const {
  return Boundary ? SU->NodeNum >= *Boundary : false;
}

Region::Region(MachineBasicBlock *BB, MachineBasicBlock::iterator Begin,
               MachineBasicBlock::iterator End,
               ArrayRef<MachineBundle> TopFixedBundles,
               ArrayRef<MachineBundle> BotFixedBundles)
    : BB(BB), TopFixedBundles(TopFixedBundles),
      BotFixedBundles(BotFixedBundles) {
  MachineBasicBlock::iterator FreeBegin =
      std::next(Begin, TopFixedBundles.size());
  MachineBasicBlock::iterator FreeEnd = std::prev(End, BotFixedBundles.size());

  // Verify that all fixed instructions are at the right place in the MBB
  assert(TopFixedBundles.empty() || Begin == BB->begin());
  assert(TopFixedBundles.empty() ||
         all_of(TopFixedBundles.back().Instrs, [FreeBegin](
                                                   const MachineInstr *MI) {
           return getBundleStart(MI->getIterator()) == std::prev(FreeBegin);
         }));
  assert(BotFixedBundles.empty() || End == BB->end());
  assert(
      BotFixedBundles.empty() ||
      all_of(BotFixedBundles.front().Instrs, [FreeEnd](const MachineInstr *MI) {
        return getBundleStart(MI->getIterator()) == FreeEnd;
      }));

  // When the region is created, its instructions haven't been re-ordered yet,
  // so this is effectively saving the semantic order.
  for (auto It = FreeBegin; It != FreeEnd; ++It) {
    SemanticOrder.push_back(&*It);
  }
  if (End != BB->end()) {
    ExitInstr = &*End;
  }
}

BlockState::BlockState(MachineBasicBlock *Block) : TheBlock(Block) {
  classify();
}

// This safety margin is independent of the successor block, and is therefore
// conservative
int BlockState::getSafetyMargin() const {
  assert(Kind == BlockType::Loop);
  assert(isScheduled());
  auto Margin = std::max(FixPoint.MaxLatencyExtent, FixPoint.MaxResourceExtent);
  DEBUG_LOOPAWARE(dbgs() << "Epilogue margin=" << Margin << "\n");
  return Margin;
}

void BlockState::setPipelined() {
  FixPoint.Stage = SchedulingStage::PipeliningDone;
}

int BlockState::getScheduleLength() const {
  int Length = 0;
  for (auto &R : Regions) {
    Length += R.Bundles.size();
  }
  return Length;
}

void BlockState::clearSchedule() {
  // We are rescheduling this block. Clear the results of the previous
  // iteration, to prepare for the next round.
  for (auto &R : Regions) {
    R.Bundles.clear();
  }
  CurrentRegion = 0;
}

void BlockState::classify() {
  // Detect whether this block is amenable to loop-aware scheduling.
  // We must push the safety margin to our epilogue block(s)
  // This can only be done if we have an epilogue and the epilogue is not itself
  // a loop.
  auto IsLoop = [](const MachineBasicBlock *MBB) {
    int NumLoopEdges = 0;
    int NumExitEdges = 0;
    for (auto *S : MBB->successors()) {
      if (S == MBB) {
        NumLoopEdges++;
      } else {
        NumExitEdges++;
      }
    }
    return NumLoopEdges == 1 && NumExitEdges == 1;
  };

  // We generalize slightly; we require the epilogue to be a dedicated exit of
  // the loop or a fallthrough block that is not a loop, so that we can
  // squeeze in a dedicated exit.
  auto CanFixLoopSchedule = [L = TheBlock,
                             &IsLoop](const MachineBasicBlock *S) {
    // Either the backedge, or a dedicated loop exit, or a fallthrough loop exit
    return S == L || S->pred_size() == 1 ||
           (L->isLayoutSuccessor(S) && !IsLoop(S));
  };

  // If we don't mark up any loops, we will iterate in the same order and apply
  // the same safety margins as before.
  if (LoopAware && IsLoop(TheBlock) &&
      llvm::all_of(TheBlock->successors(), CanFixLoopSchedule)) {
    Kind = BlockType::Loop;
    FixPoint.Stage = SchedulingStage::GatheringRegions;
  }

  // We will mark the epilogues in a second sweep, when all states have been
  // constructed. That sweep is driven by the Loops we've classified on
  // construction.
}

void BlockState::initInterBlock(const MachineSchedContext &Context,
                                const AIEHazardRecognizer &HR) {
  assert(!BoundaryEdges);
  assert(Kind == BlockType::Loop);
  assert(all_of(Regions,
                [](const Region &R) {
                  return R.top_fixed_instrs().empty() &&
                         R.bot_fixed_instrs().empty();
                }) &&
         "Loop cannot have fixed instructions");
  BoundaryEdges = std::make_unique<InterBlockEdges>(Context);
  if (Regions.size() == 1) {
    // Don't worry, this just constructs a mostly empty container class
    auto NumInstrs = getTop().getFreeInstructions().size();
    PostSWP = std::make_unique<PostPipeliner>(HR, NumInstrs);
  }

  // We are called just after the first round of scheduling a block.
  // These loops run over the original 'semantical order' that was collected
  // in this first fixpoint iteration
  for (auto *MI : getBottom().getFreeInstructions()) {
    BoundaryEdges->addNode(MI);
  }
  BoundaryEdges->markBoundary();
  for (auto *MI : getTop().getFreeInstructions()) {
    BoundaryEdges->addNode(MI);
  }
  BoundaryEdges->buildEdges();
  DEBUG_LOOPAWARE(dumpInterBlock(*BoundaryEdges));
}

} // namespace llvm::AIE
