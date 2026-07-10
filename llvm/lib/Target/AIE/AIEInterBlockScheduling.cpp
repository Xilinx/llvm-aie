//===- AIEInterBlockScheduling.cpp - Inter-block scheduling infrastructure ===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
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
#include "AIEMultiSlotInstrMaterializer.h"
#include "Utils/AIELoopOptionOverrides.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineOptimizationRemarkEmitter.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/Support/ErrorHandling.h"
#include <memory>
#include <optional>

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

static cl::opt<int> PostPipelinerMinII(
    "aie-postpipeliner-minii", cl::init(1),
    cl::desc("[AIE] Minimum II to be tried in the post-ra pipeliner"));

static cl::opt<int> PostPipelinerMaxII(
    "aie-postpipeliner-maxii", cl::init(60),
    cl::desc("[AIE] Maximum II to be tried in the post-ra pipeliner"));

static cl::opt<bool> EnableMultiSlotInstrMaterialization(
    "aie-preassign-multi-slot-instr", cl::Hidden, cl::init(true),
    cl::desc("Statically materialize Multi-Slot Pseudo Instructions in "
             "loops."));

static cl::opt<bool> MaterializePipeline(
    "aie-materialize-pipeline", cl::Hidden, cl::init(true),
    cl::desc("Materialize all Multi-Slot Pseudo Instructions in "
             "post-pipeline candidate loops."));

static cl::opt<int> PostPipelinerMaxTryII(
    "aie-postpipeliner-maxtry-ii", cl::init(20),
    cl::desc("[AIE] Maximum II steps to be tried in the post-ra pipeliner"));

static cl::opt<bool> PostSchedIgnoreMemoryDeps(
    "aie-safe-to-ignore-memory-deps", cl::init(false),
    cl::desc("Ignore memory deps when we know that it is safe."));

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
      HR.emitInScoreboard(Scoreboard, *MI, *SelectedDescriptors.getDesc(MI), 0);
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
      HR.emitInScoreboard(Scoreboard, *MI, *SelectedDescriptors.getDesc(MI), 0);
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
      if (HR.getHazardType(Scoreboard, MI, -BottomUpCycle)) {
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
      if (HR.getHazardType(Scoreboard, MI, 0)) {
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

InterBlockScheduling::InterBlockScheduling(const MachineSchedContext *C,
                                           bool InterBlock)
    : Context(C), InterBlockScoreboard(InterBlock) {}

void InterBlockScheduling::classifyBlock(BlockState &BS) {
  // Detect whether this block is amenable to loop-aware scheduling.
  // We must push the safety margin to our epilogue block(s)
  // This can only be done if we have an epilogue and the epilogue is not itself
  // a loop.
  auto IsLoop = [](const MachineBasicBlock *MBB) {
    return AIELoopUtils::isSingleMBBLoop(MBB);
  };

  // We generalize slightly; we require the epilogue to be a dedicated exit of
  // the loop or a fallthrough block that is not a loop, so that we can
  // squeeze in a dedicated exit.
  auto CanFixLoopSchedule = [LBB = BS.TheBlock,
                             &IsLoop](const MachineBasicBlock *S) {
    // Either the backedge, or a dedicated loop exit, or a fallthrough loop exit
    return S == LBB || S->pred_size() == 1 ||
           (LBB->isLayoutSuccessor(S) && !IsLoop(S));
  };

  // If we don't mark up any loops, we will iterate in the same order and apply
  // the same safety margins as before.
  if (LoopAware && IsLoop(BS.TheBlock) &&
      llvm::all_of(BS.TheBlock->successors(), CanFixLoopSchedule)) {
    BS.Kind = BlockType::Loop;
    return;
  }
  // A block is an Epilogue iff at least one of its predecessors is a Loop
  // block.
  for (MachineBasicBlock *Pred : BS.TheBlock->predecessors()) {
    if (Blocks.count(Pred) && getBlockState(Pred).Kind == BlockType::Loop) {
      BS.Kind = BlockType::Epilogue;
      return;
    }
  }
  BS.Kind = BlockType::Regular;
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
    // This is the first classification, picking out the loops. It may pick out
    // some epilogues already, but we make an explicit second classification
    // sweep once we have established the set of loops.
    classifyBlock(BS);
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
    for (auto &[_, BS] : Blocks) {
      classifyBlock(BS);
    }
  }

  // Compute the scheduling order once up-front. We use it for both the
  // region gathering phase and the scheduling phase, even if the former
  // is independent of order.
  defineSchedulingOrder(MF);
  IsGatheringPhase = true;
}

/// Emit a loop scheduling optimization remark with pipeliner kind, II, NS,
/// loop name, and prologue/epilogue MBB names and bundle counts.
static void emitPipelinerRemark(MachineOptimizationRemarkEmitter &More,
                                const char *Pipeliner,
                                MachineBasicBlock *LoopBB, int II, unsigned NS,
                                const BlockState *PrologueBS,
                                unsigned PrologueBundles,
                                const BlockState *EpilogueBS,
                                unsigned EpilogueBundles) {
  const auto DbgLoc = LoopBB->begin()->getDebugLoc();
  auto MBBLabel = [](const MachineBasicBlock *MBB) {
    // return "bb.<N>.<irname>" when an IR name is present, or "bb.<N>".
    std::string Buf;
    raw_string_ostream OS(Buf);
    MBB->printName(OS, MachineBasicBlock::PrintNameIr);
    return Buf;
  };
  More.emit([&]() {
    auto R = MachineOptimizationRemark("pipeliner", "schedule", DbgLoc, LoopBB);
    R << "Schedule found" << ore::NV("Pipeliner", Pipeliner)
      << ore::NV("II", II) << ore::NV("NS", NS)
      << ore::NV("Loop", MBBLabel(LoopBB));
    if (PrologueBS)
      R << ore::NV("Prologue", MBBLabel(PrologueBS->TheBlock))
        << ore::NV("PrologueBundles", PrologueBundles);
    if (EpilogueBS)
      R << ore::NV("Epilogue", MBBLabel(EpilogueBS->TheBlock))
        << ore::NV("EpilogueBundles", EpilogueBundles);
    return R;
  });
}

/// Emit scheduling remarks for single-MBB loop blocks. Each loop gets
/// exactly one remark reporting II, NS, and prologue/epilogue bundle counts.
/// The "Pipeliner" field distinguishes which pipeliner (if any) handled it.
void InterBlockScheduling::emitLoopRemarks() {
  if (Blocks.empty())
    return;

  // Iterate single-MBB loops in MachineFunction layout order so remark
  // output is deterministic and independent of the pointer-keyed
  // `Blocks` map. Only loops tracked here and classified as
  // BlockType::Loop receive a remark.
  MachineFunction &MF = *Blocks.begin()->first->getParent();
  MachineOptimizationRemarkEmitter More(MF, nullptr);
  for (MachineBasicBlock *LoopBB : AIELoopUtils::getSingleBlockLoopMBBs(MF)) {
    if (!Blocks.count(LoopBB))
      continue;
    const BlockState &BS = getBlockState(LoopBB);
    if (BS.Kind != BlockType::Loop)
      continue;

    auto [PrologueMBB, EpilogueMBB] =
        AIELoopUtils::findPrologueEpilogue(*LoopBB);

    // Skip loops without a proper prologue/epilogue or whose
    // prologue/epilogue blocks are not tracked (e.g. entry-block loops
    // or isolated --run-pass=postmisched on partial MIR).
    if (!PrologueMBB || !EpilogueMBB)
      continue;
    if (!Blocks.count(PrologueMBB) || !Blocks.count(EpilogueMBB))
      continue;

    const BlockState &PrologueBS = getBlockState(PrologueMBB);
    const BlockState &EpilogueBS = getBlockState(EpilogueMBB);

    // Determine the pipeliner kind, II and NS.
    const char *Pipeliner = nullptr;
    // For a non-pipelined loop, "II" is the unpipelined loop body length;
    // we keep the field name "II" for tooling compatibility.
    int II = BS.getScheduleLength();
    unsigned NS = 1;
    if (BS.isPipelined()) {
      Pipeliner = "postpipeliner";
      const auto &SWP = BS.getPostSWP();
      II = SWP.getII();
      NS = SWP.getStageCount();
    } else if (auto SWP_NS = AIELoopUtils::getSWPStageCount(*LoopBB, *TII)) {
      Pipeliner = "prepipeliner";
      NS = *SWP_NS;
    }

    emitPipelinerRemark(More, Pipeliner, LoopBB, II, NS, &PrologueBS,
                        PrologueBS.getScheduleLength(), &EpilogueBS,
                        EpilogueBS.getScheduleLength());
  }
}

void InterBlockScheduling::leaveFunction() {
  DEBUG_BLOCKS(dbgs() << "<< leaveFunction\n");
  emitLoopRemarks();
  Blocks.clear();
}

void InterBlockScheduling::enterBlock(MachineBasicBlock *BB) {
  CurrentBlockState = &getBlockState(BB);
  CurrentBlockState->resetRegion();
  DEBUG_BLOCKS(dbgs() << "  >> enterBlock " << BB->getNumber() << " "
                      << CurrentBlockState->kindAsString() << " FixPointIter="
                      << CurrentBlockState->FixPoint.NumIters
                      << " II=" << CurrentBlockState->FixPoint.II << "\n");

  if (IsGatheringPhase) {
    return;
  }

  // When relevant, pick up the fixed fragments left by scheduling other
  // blocks, in particular the pipeliner's prologue and epilogue.
  emitInterBlockTop(*CurrentBlockState);
  emitInterBlockBottom(*CurrentBlockState);
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
  // True while visiting the prologue section.
  bool InPrologue = false;
  // Maps each original loop instruction to its first-iteration clone in the
  // prologue. Only the first occurrence of each original is recorded.
  DenseMap<const MachineInstr *, MachineInstr *> PrologueFirstIterClones;

  void startPrologue() override { InPrologue = true; }
  void startLoop() override {
    auto &CopyTo = Prologue->BottomInsert;
    assert(CopyTo.empty() && "PreHeader already has a timed region at Bottom.");
    bool CopyEmpty = false;
    for (auto &B : TimedRegion) {
      if (!B.empty() || CopyEmpty) {
        CopyTo.emplace_back(B);
        CopyEmpty = true;
      }
    }
    TimedRegion.clear();

    // We finished the prologue and collected the first copy of each
    // instruction emitted there. Now populate the semantic order of BotFixed
    // by following the original semantic order of the loop body. Only
    // instructions that were actually cloned into the prologue are recorded;
    // loop body instructions without a prologue copy are omitted.
    for (MachineInstr *OrigMI : Loop.getTop().getFreeInstructions()) {
      const auto It = PrologueFirstIterClones.find(OrigMI);
      if (It != PrologueFirstIterClones.end())
        Prologue->BottomInsertSemanticOrder.push_back(It->second);
    }

    InPrologue = false;
    InLoop = true;
  }
  void startEpilogue() override {
    Loop.getTop().Bundles = TimedRegion;
    TimedRegion.clear();
    InLoop = false;
  }
  void finish() override {
    auto &CopyTo = Epilogue->TopInsert;
    assert(CopyTo.empty() && "Epilogue already has a timed region at Top.");

    // Establish the number of bundles to copy. Note that std::distance on a
    // vector is O(1)
    int NonEmpty = std::distance(
        find_if(reverse(TimedRegion), [](const auto &B) { return !B.empty(); }),
        TimedRegion.rend());
    // And copy them.
    for (int I = 0; I < NonEmpty; I++) {
      CopyTo.push_back(TimedRegion[I]);
    }
    TimedRegion.clear();
  }
  void startBundle() override { CurrentBundle.clear(); }
  void addToBundle(MachineInstr *MI) override {
    // We re-emit the original instructions into the loop body.
    // Prologue and epilogue obtain copies.
    MachineInstr *ToBeEmitted =
        InLoop ? MI : Loop.TheBlock->getParent()->CloneMachineInstr(MI);
    CurrentBundle.add(ToBeEmitted);

    // Record the first-iteration prologue clone for each original instruction.
    // try_emplace only inserts when the key is absent, so only the first
    // occurrence (i.e. the first-iteration copy) is kept.
    if (InPrologue)
      PrologueFirstIterClones.try_emplace(MI, ToBeEmitted);
  }
  void endBundle() override { TimedRegion.emplace_back(CurrentBundle); }

public:
  PipelineExtractor(InterBlockScheduling &InterBlock, BlockState &BS,
                    const AIEBaseInstrInfo &TII)
      : Loop(BS), CurrentBundle(TII.getFormatInterface()) {
    auto [PrologueMBB, EpilogueMBB] =
        AIELoopUtils::findPrologueEpilogue(*Loop.TheBlock);
    assert(PrologueMBB && EpilogueMBB &&
           "Pipelined loop must have a unique prologue and epilogue");
    Prologue = &InterBlock.getBlockState(PrologueMBB);
    Epilogue = &InterBlock.getBlockState(EpilogueMBB);
  }
  const BlockState *getPrologue() const { return Prologue; }
  const BlockState *getEpilogue() const { return Epilogue; }
};

/// Recursive post-order walker for epilogue scheduling in
/// defineSchedulingOrder.
class EpilogueProcessor {
  InterBlockScheduling &IBS;
  SmallPtrSet<const MachineBasicBlock *, 16> &Emitted;
  SmallPtrSet<const MachineBasicBlock *, 16> &Processing;
  llvm::function_ref<void(MachineBasicBlock *)> Push;

public:
  EpilogueProcessor(InterBlockScheduling &IBS,
                    SmallPtrSet<const MachineBasicBlock *, 16> &Emitted,
                    SmallPtrSet<const MachineBasicBlock *, 16> &Processing,
                    llvm::function_ref<void(MachineBasicBlock *)> Push)
      : IBS(IBS), Emitted(Emitted), Processing(Processing), Push(Push) {}

  void operator()(MachineBasicBlock *EpilogueRoot) const {
    if (Emitted.contains(EpilogueRoot) ||
        !Processing.insert(EpilogueRoot).second)
      return;

    for (MachineBasicBlock *Sub : post_order(EpilogueRoot)) {
      if (Sub != EpilogueRoot &&
          IBS.getBlockState(Sub).Kind == BlockType::Epilogue) {
        (*this)(Sub);
      }
      Push(Sub);
    }

    Processing.erase(EpilogueRoot);
  }
};

} // namespace

bool InterBlockScheduling::leaveBlock() {
  DEBUG_BLOCKS(dbgs() << "  << leaveBlock "
                      << CurrentBlockState->TheBlock->getNumber() << "\n");
  // After scheduling a basic block, check convergence to determine which block
  // to schedule next and with what parameters
  auto &BS = *CurrentBlockState;
  if (IsGatheringPhase) {
    // This is the first visit to this block. The region decomposition has been
    // gathered. Now transition to Scheduling so the next pass actually
    // schedules the gathered regions.
    //
    // The machine scheduler may skip enterRegion entirely for blocks that have
    // no schedulable region (empty blocks or single-instruction blocks). Ensure
    // the invariant that every block has at least one region by creating one
    // that covers the full block content.
    if (BS.getRegions().empty())
      BS.addRegion(BS.TheBlock, BS.TheBlock->begin(), BS.TheBlock->end());
    if (BS.Kind == BlockType::Loop) {
      // For loops, also create the interblock edges between the top and the
      // bottom region.
      BS.initInterBlock(*Context, *HR);
    }
    return false;
  }

  const auto Stage = updateFixPoint(BS);
  BS.FixPoint.Stage = Stage;
  switch (Stage) {
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
    PostSWP.materializePipeline(GenSchedule);
    break;
  }
  case SchedulingStage::SchedulingDone:
  case SchedulingStage::PipeliningFailed:
    break;
  }

  // After scheduling a block that contains BotFixed (SWP prologue) bundles,
  // update the PerSuccEdges entry for this block in each predecessor's edge
  // set. By now emitInterBlockBottom has run and the prologue clones are
  // physically in their MBB, so they carry valid MF context for DAG edge
  // building. Only the edge to this specific block is rebuilt; other successor
  // edges of each predecessor are preserved.
  if (!BS.getBottom().getBotFixedBundles().empty()) {
    for (MachineBasicBlock *PredBB : BS.TheBlock->predecessors()) {
      if (Blocks.count(PredBB))
        updatePerSuccEdges(PredBB, /*For=*/BS.TheBlock);
    }
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

MachineInstr *InterBlockScheduling::latencyConverged(BlockState &BS) {
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
  InterBlockEdges *BackEdge = BS.getLoopSelfEdge();
  assert(BackEdge && "Loop block must have a self-edge in PerSuccEdges");

  // Repopulate the post-boundary depths from the current scheduled bundles of
  // the top region, capped at the conflict horizon.  Clear first so that stale
  // values from a previous fixpoint iteration are not retained.
  BackEdge->clearPostDepths();
  int Depth = 0;
  for (auto &Bundle : Top.Bundles) {
    for (auto *MI : Bundle.getInstrs()) {
      BackEdge->recordPostDepth(MI, Depth);
    }
    // For empty bundles...
    BackEdge->recordPostDepth(Depth);
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
      const SUnit *Pred = BackEdge->getPreBoundaryNode(MI);
      for (auto &SDep : Pred->Succs) {
        auto *Succ = SDep.getSUnit();
        if (!BackEdge->isPostBoundaryNode(Succ)) {
          continue;
        }
        DEBUG_LOOPAWARE(dbgs() << "  Backedge to " << Succ->NodeNum << "\n");
        // Instructions beyond the conflict horizon default to ConflictHorizon,
        // so that Distance = Height + ConflictHorizon >= 1 + ConflictHorizon,
        // which is always >= Latency, naturally avoiding false positives.
        const int SuccDepth =
            BackEdge->getPostDepthOr(Succ, HR->getConflictHorizon());
        DEBUG_LOOPAWARE(dbgs() << "  Depth=" << SuccDepth << "\n");
        int Latency = SDep.getSignedLatency();
        int Distance = Height + SuccDepth;
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
  assert(!IsGatheringPhase);

  if (BS.Kind != BlockType::Loop) {
    return SchedulingStage::SchedulingDone;
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

    return SchedulingStage::SchedulingNotConverged;
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
    return SchedulingStage::Scheduling;
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
        return SchedulingStage::Scheduling;
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
    return SchedulingStage::Scheduling;
  }

  DEBUG_LOOPAWARE(dbgs() << "Converged,"
                         << " LatencyExtent=" << BS.FixPoint.MaxLatencyExtent
                         << " ResourceExtent=" << BS.FixPoint.MaxResourceExtent
                         << "\n");

  // The loop schedule has converged, so we could declare our work done.
  // But first try SWP
  if (BS.getRegions().size() == 1) {
    auto &PostSWP = BS.getPostSWP();
    if (PostSWP.isPostPipelineCandidate(*BS.TheBlock)) {
      const int ResMII = PostSWP.getResMII(*BS.TheBlock);
      const int StartII = std::max(ResMII, PostPipelinerMinII.getValue());
      if (StartII <= PostPipelinerMaxII) {
        BS.FixPoint.II = StartII;
        BS.FixPoint.IITries = 1;
        return SchedulingStage::Pipelining;
      }
    }
  }
  return SchedulingStage::SchedulingDone;
}

SchedulingStage InterBlockScheduling::updatePipelining(BlockState &BS) {
  // We have been pipelining. Check whether we were successful.
  if (BS.FixPoint.Stage == SchedulingStage::PipeliningDone) {
    return BS.FixPoint.Stage;
  }

  // Otherwise try a larger II.
  // We cut off at larger IIs to prevent excessive compilation time.
  if (++BS.FixPoint.II <= PostPipelinerMaxII &&
      ++BS.FixPoint.IITries <= PostPipelinerMaxTryII) {
    return SchedulingStage::Pipelining;
  }

  // Fall back to the loop schedule. Note that we can only enter pipeline mode
  // after the loop schedule has stabilized. Failure is observable by the
  // absence of a "Schedule found" remark on this loop.
  return SchedulingStage::PipeliningFailed;
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
  // Membership-only set used to deduplicate pushes across phases.
  SmallPtrSet<const MachineBasicBlock *, 16> Emitted;
  auto Push = [&](MachineBasicBlock *MBB) {
    if (Emitted.insert(MBB).second) {
      // Insertion into the Scheduling Order.
      MBBSequence.push_back(MBB);
    }
  };

  // Phase 1: schedule loops first so the loop's own schedule is available
  // when prologues and epilogues are scheduled.
  for (MachineBasicBlock &MBB : *MF) {
    if (getBlockState(&MBB).Kind == BlockType::Loop)
      Push(&MBB);
  }

  // Phase 2: for each epilogue, walk its CFG-successor sub-tree in
  // post-order. post_order(E) ends with E itself, so E is scheduled AFTER
  // everything reachable from it (its successors are already done -> precise
  // inter-block latency for E) and BEFORE its non-loop predecessors.
  //
  // When an outer epilogue's forward walk reaches a nested epilogue inside a
  // cyclic region, post-order rooted at the outer epilogue can emit the inner
  // epilogue before its prologue (loop-back). Recursively process nested
  // epilogues with their own root so post_order picks the correct order.
  SmallPtrSet<const MachineBasicBlock *, 16> Processing;
  EpilogueProcessor ProcessEpilogue{*this, Emitted, Processing, Push};

  for (MachineBasicBlock *MBB : post_order(MF)) {
    if (getBlockState(MBB).Kind != BlockType::Epilogue)
      continue;
    ProcessEpilogue(MBB);
  }

  // Phase 3: everything else in post-order to optimize the number of already
  // scheduled successors.
  for (MachineBasicBlock *MBB : post_order(MF)) {
    Push(MBB);
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
  if (IsGatheringPhase) {
    if (NextInOrder < MBBSequence.size()) {
      // Each call returns the next block to gather; NextInOrder tracks which
      // blocks have already been returned (and thus gathered).
      return MBBSequence[NextInOrder++];
    }

    // All blocks gathered. Build per-successor inter-block DDG edges for
    // every block.
    for (MachineBasicBlock *MBB : MBBSequence) {
      getBlockState(MBB).resetRegion();
      buildPerSuccEdges(MBB);
    }

    // Now run the scheduling phase. We reset the iterator and switch the mode.
    NextInOrder = 0;
    IsGatheringPhase = false;
  }

  auto &BS = getBlockState(MBBSequence[NextInOrder]);
  if (!BS.isScheduled() ||
      (BS.FixPoint.II && !BS.isPipelined() && !BS.pipeliningFailed())) {
    return MBBSequence[NextInOrder];
  }

  if (++NextInOrder < MBBSequence.size()) {
    return MBBSequence[NextInOrder];
  }
  return nullptr;
}

const BlockState &
InterBlockScheduling::getBlockState(MachineBasicBlock *BB) const {
  return Blocks.at(BB);
}

BlockState &InterBlockScheduling::getBlockState(MachineBasicBlock *BB) {
  return Blocks.at(BB);
}

void InterBlockScheduling::buildGraph(InterBlockEdges &DAG) {

  MachineBasicBlock *PredBB = DAG.getPred();

  const BlockState &BS = getBlockState(PredBB);
  const Region &Bot = BS.getBottom();

  // Pre-boundary: free instructions of the current region.
  for (MachineInstr *MI : Bot.getFreeInstructions())
    DAG.addNode(MI);

  DAG.markBoundary();

  // Post-boundary: free instructions. Empty regions signify empty basic
  // blocks; in that case no post-boundary nodes are added.
  MachineBasicBlock *SuccBB = DAG.getSucc();
  const BlockState &SuccBS = getBlockState(SuccBB);
  if (!SuccBS.getRegions().empty()) {
    for (MachineInstr *MI : SuccBS.getTop().getFreeInstructions())
      DAG.addNode(MI);
  }

  // Post-boundary: BotFixed first-iteration copies (SWP prologue clones),
  // in the semantic order of the original loop instructions. This vector is
  // empty for non-pipelined blocks.
  for (MachineInstr *MI : SuccBS.BottomInsertSemanticOrder) {
    DAG.addNode(MI);
    // Some queries in edge building require a parent to get to SubTarget.
    // We push them in the corresponding block. edge building uses the
    // insertion order in the DAG, not the block, so position within the block
    // is irrelevant. The instructions will be removed again before the regular
    // reinsertion that is part of scheduling Fixed regions.
    if (!MI->getParent()) {
      SuccBB->push_back(MI);
    }
  }

  DAG.buildEdges();
}

void InterBlockScheduling::buildPerSuccEdges(MachineBasicBlock *BB) {
  BlockState &BS = getBlockState(BB);
  assert(!BS.getRegions().empty() &&
         "Every block in Blocks must have at least one region.");

  const MachineSchedContext &C = *Context;
  const bool SafeToIgnoreMemDeps = BS.isSafeToIgnoreMemDeps();

  auto &PerSuccEdges = BS.getPerSuccEdges();
  PerSuccEdges.clear();

  for (MachineBasicBlock *SuccBB : BB->successors()) {
    DEBUG_BLOCKS(dbgs() << "Building InterBlockEdge: Pred=" << BB->getNumber()
                        << " Succ=" << SuccBB->getNumber() << "\n");

    InterBlockEdges &SE = *PerSuccEdges.emplace_back(
        std::make_unique<InterBlockEdges>(C, SafeToIgnoreMemDeps, BB, SuccBB));
    buildGraph(SE);
  }
}

void InterBlockScheduling::updatePerSuccEdges(MachineBasicBlock *BB,
                                              MachineBasicBlock *For) {
  BlockState &BS = getBlockState(BB);
  assert(!BS.getRegions().empty() &&
         "Every block in Blocks must have at least one region.");

  auto &PerSuccEdges = BS.getPerSuccEdges();
  auto It = llvm::find_if(PerSuccEdges, [For](const auto &SEPtr) {
    return SEPtr->getSucc() == For;
  });
  assert(It != PerSuccEdges.end());
  InterBlockEdges &DAG = **It;
  DAG.clear();
  buildGraph(DAG);
}

void InterBlockScheduling::recordPostDepths(MachineBasicBlock *BB) {
  for (const auto &SEPtr : getBlockState(BB).getPerSuccEdges()) {
    InterBlockEdges &SE = *SEPtr;
    MachineBasicBlock *SuccBB = SE.getSucc();
    if (!SuccBB)
      continue;
    const BlockState &SBS = getBlockState(SuccBB);
    assert(!SBS.getRegions().empty() &&
           "Every block in Blocks must have at least one region.");
    SE.clearPostDepths();
    if (!SBS.isScheduled()) {
      // Compute a static lower-bound on each instruction's cycle position
      // within the successor block, using the inter-block DDG latencies.
      for (auto &SU : SE.SUnits) {
        if (!SE.isPostBoundaryNode(&SU))
          continue;
        int Depth = 0;
        for (auto &Dep : SU.Preds) {
          const int NewDepth =
              Dep.getLatency() + SE.getPostDepthOr(Dep.getSUnit(), 0);
          Depth = std::max(Depth, NewDepth);
        }
        SE.recordPostDepth(SU.getInstr(), Depth);
      }
    } else {
      int Cycle = 0;
      for (const MachineBundle &Bundle : SBS.getTop().Bundles) {
        for (MachineInstr *MI : Bundle.getInstrs())
          SE.recordPostDepth(MI, Cycle);

        // For empty bundles...
        SE.recordPostDepth(Cycle);
        ++Cycle;
      }
    }
  }
}

void InterBlockScheduling::enterRegion(MachineBasicBlock *BB,
                                       MachineBasicBlock::iterator RegionBegin,
                                       MachineBasicBlock::iterator RegionEnd) {
  auto &BS = getBlockState(BB);
  DEBUG_BLOCKS(dbgs() << "    >> enterRegion, Iter=" << BS.FixPoint.NumIters
                      << "\n");

  if (IsGatheringPhase) {
    // Gather region boundaries and capture the invariant SemanticOrder for all
    // block types. Fixed bundles are NOT set here: they result from loop
    // pipelining, which happens during Scheduling, and are applied via the
    // setTopFixedBundles / setBotFixedBundles calls in the Scheduling pass.
    BS.addRegion(BB, RegionBegin, RegionEnd);
    return;
  }

  if (BS.Kind == BlockType::Loop) {
    return;
  }

  // Scheduling pass for non-loop blocks: set fixed bundles on the
  // pre-gathered region now that emitInterBlockTop / emitInterBlockBottom
  // has physically inserted the SWP instructions into the block.
  assert(!BS.getRegions().empty() &&
         "Every block in Blocks must have at least one region.");
  if (RegionBegin == BB->begin() && !BS.TopInsert.empty())
    BS.getCurrentRegion().setTopFixedBundles(BS.TopInsert);
  if (RegionEnd == BB->end() && !BS.BottomInsert.empty())
    BS.getCurrentRegion().setBotFixedBundles(BS.BottomInsert);
}

namespace {

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

} // namespace

MachineBasicBlock *
InterBlockScheduling::makeDedicatedLoopExit(MachineBasicBlock *LoopMBB,
                                            MachineBasicBlock *CurrentMBB) {
  if (CurrentMBB->pred_size() > 1) {
    MachineBasicBlock *DedicatedExit = splitEdge(LoopMBB, CurrentMBB);
    Blocks.emplace(DedicatedExit, DedicatedExit);
    // Add an empty region so the invariant holds: every block in Blocks has
    // at least one region.
    BlockState &NewBS = Blocks.at(DedicatedExit);
    NewBS.addRegion(DedicatedExit, DedicatedExit->begin(),
                    DedicatedExit->end());
    // Re-classify both affected blocks based on the updated CFG.
    classifyBlock(NewBS);
    BlockState &CurrentBS = getBlockState(CurrentMBB);
    classifyBlock(CurrentBS);
    // Rebuild successor edges now that the CFG has been updated.
    buildPerSuccEdges(LoopMBB);
    buildPerSuccEdges(DedicatedExit);
    return DedicatedExit;
  }
  return CurrentMBB;
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
    MachineBasicBlock::iterator Before, bool Move, bool EmitNops,
    bool ApplyBundling) const {
  for (auto &Bundle : Bundles) {
    if (Bundle.empty()) {
      // Without bundling the band is reserved as standalone instructions and
      // empty (NOP) bundles are dropped: their cycles become gaps, re-encoded
      // as DAG edge latencies from the band geometry.
      if (!ApplyBundling)
        continue;
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
  if (ApplyBundling)
    AIEHazardRecognizer::applyBundles(Bundles, BB);
}

std::optional<std::pair<MachineBasicBlock *, MachineBasicBlock *>>
getMBBAndParentLoopMBB(const BlockState &EpilogueBS,
                       const InterBlockScheduling &InterBlock,
                       bool IsLoopPipelined) {

  if (EpilogueBS.Kind != BlockType::Epilogue)
    return std::nullopt;

  MachineBasicBlock *EpilogueBB = EpilogueBS.TheBlock;
  MachineBasicBlock *LoopMBB = AIELoopUtils::getLoopPredecessor(*EpilogueBB);
  assert(LoopMBB);
  const BlockState &LoopBS = InterBlock.getBlockState(LoopMBB);

  if (LoopBS.isPipelined() != IsLoopPipelined)
    return std::nullopt;

  return {std::make_pair(EpilogueBB, LoopMBB)};
}

void InterBlockScheduling::emitTopSafetyMargin(const BlockState &BS) {

  auto EpilogueAndParentLoopMBBs =
      getMBBAndParentLoopMBB(BS, *this, /*bool IsLoopPipelined=*/false);

  if (!EpilogueAndParentLoopMBBs)
    return;

  auto [EpilogueBB, ParentLoopMBB] = *EpilogueAndParentLoopMBBs;

  // Epilogues of non-pipelined loops should supply the safety margin for their
  // loops. If this block is not a dedicated exit, spawn a new exclusive exit
  // block.
  if (int SafetyMargin = getSafetyMargin(ParentLoopMBB, EpilogueBB)) {
    auto *DedicatedExit = makeDedicatedLoopExit(ParentLoopMBB, EpilogueBB);
    DEBUG_LOOPAWARE(dbgs() << "Emitting " << SafetyMargin << " safety nops\n");
    while (SafetyMargin--) {
      TII->insertNoop(*DedicatedExit, DedicatedExit->begin());
    }
  }
}

void InterBlockScheduling::emitInterBlockTop(BlockState &BS) {

  auto EpilogueAndParentLoopMBBs =
      getMBBAndParentLoopMBB(BS, *this, /*bool IsLoopPipelined=*/true);

  if (!EpilogueAndParentLoopMBBs)
    return;

  auto [EpilogueBB, ParentLoopMBB] = *EpilogueAndParentLoopMBBs;

  // Emit the bundles of the swp epilogue in a dedicated exit.
  // If there isn't one, spawn a new block, add a new block state and put
  // this block to be scheduled later. Some maintenance of the original block
  // state is also necessary.
  auto *DedicatedExit = makeDedicatedLoopExit(ParentLoopMBB, EpilogueBB);
  if (DedicatedExit == EpilogueBB) {

    // Trim excedent empty bundles. Empty TopInsert means 1-stage pipeline.
    if (!BS.TopInsert.empty()) {
      while (BS.TopInsert.back().empty()) {
        assert(BS.TopInsert.back().getMetaInstrs().empty());
        BS.TopInsert.pop_back();
      }
    }

    // Reserve the epilogue standalone (NOP bundles dropped);
    // commitBlockSchedule re-bundles it after scheduling.
    emitBundles(BS.TopInsert, DedicatedExit, DedicatedExit->begin(),
                /*Move=*/false, /*EmitNops=*/false, /*ApplyBundling=*/false);
  } else {
    // If not, transfer the timed region to the new block state created
    // by makeDedicatedLoopExit. The Kind of both blocks was already updated
    // there via classifyNonLoop.
    MBBSequence.push_back(DedicatedExit);
    BlockState &NewBS = getBlockState(DedicatedExit);
    NewBS.TopInsert = BS.TopInsert;
    BS.TopInsert.clear();
  }
}

void InterBlockScheduling::emitInterBlockBottom(const BlockState &BS) const {
  if (BS.BottomInsert.empty()) {
    return;
  }
  MachineBasicBlock *PreHeader = BS.TheBlock;
  assert(PreHeader->end() == PreHeader->getFirstTerminator() &&
         "PreHeader is not fall-through");
  // BottomInsertSemanticOrder instructions may have been temporarily placed in
  // the block by buildPerSuccEdges to enable dependency analysis. Remove them
  // before emitBundles re-inserts all BottomInsert instructions properly.
  for (MachineInstr *MI : BS.BottomInsertSemanticOrder) {
    if (MI->getParent() == PreHeader)
      PreHeader->remove_instr(MI);
  }
  // Reserve the prologue standalone (NOP bundles dropped); commitBlockSchedule
  // re-bundles it after scheduling.
  emitBundles(BS.BottomInsert, PreHeader, PreHeader->end(), /*Move=*/false,
              /*EmitNops=*/false, /*ApplyBundling=*/false);
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

  DEBUG_LOOPAWARE(dbgs() << "Resource conflict avoidance between" << " loop: "
                         << *LoopMBB << " And epilogue: " << EpilogueMBB
                         << " Requires " << NopCounter << " Nops\n");

  return NopCounter;
}

Region::Region(MachineBasicBlock *BB, MachineBasicBlock::iterator Begin,
               MachineBasicBlock::iterator End)
    : BB(BB) {
  // When the region is created, its instructions haven't been re-ordered yet,
  // so this is effectively saving the semantic order. Fixed bundles (if any)
  // are set separately via setTopFixedBundles / setBotFixedBundles, which
  // will trim the corresponding entries from SemanticOrder.
  for (auto It = Begin; It != End; ++It) {
    SemanticOrder.push_back(&*It);
  }
  if (End != BB->end()) {
    ExitInstr = &*End;
  }
}

FixedInstrs computeFixedInstrs(ArrayRef<MachineBundle> Band) {
  FixedInstrs G;
  for (const MachineBundle &B : Band) {
    for (MachineInstr *MI : B.getInstrs())
      G.MIToCycle[MI] = G.NumCycles;
    // Advance the cycle even for an empty (NOP) bundle: every bundle, real or
    // NOP, occupies exactly one cycle.
    ++G.NumCycles;
  }
  return G;
}

static unsigned countBandInstrs(ArrayRef<MachineBundle> Bundles) {
  unsigned Count = 0;
  for (const MachineBundle &B : Bundles)
    Count += B.getInstrs().size();
  return Count;
}

void Region::setTopFixedBundles(ArrayRef<MachineBundle> Bundles) {
  assert(TopFixedBundles.empty() && "TopFixedBundles already set.");
  TopFixedBundles = Bundles;
  TopFixedInstrCount = countBandInstrs(Bundles);
#ifndef NDEBUG
  // The top-fixed instructions are reserved as the first TopFixedInstrCount
  // standalone MIs of the block (NOP bundles dropped); verify the last real
  // band instruction sits exactly at that offset, matching what
  // top_fixed_instrs() assumes.
  if (TopFixedInstrCount > 0) {
    const MachineInstr *LastBandMI = nullptr;
    for (const MachineBundle &B : reverse(Bundles))
      if (!B.getInstrs().empty()) {
        LastBandMI = B.getInstrs().back();
        break;
      }
    assert(&*std::next(BB->begin(), TopFixedInstrCount - 1) == LastBandMI &&
           "Top-fixed instructions are not at the block start.");
  }
#endif
  // SemanticOrder was captured during the gathering phase before the fixed
  // instructions were inserted into the block, so it already contains only the
  // free instructions. No adjustment is needed.
}

void Region::setBotFixedBundles(ArrayRef<MachineBundle> Bundles) {
  assert(BotFixedBundles.empty() && "BotFixedBundles already set.");
  BotFixedBundles = Bundles;
  BotFixedInstrCount = countBandInstrs(Bundles);
#ifndef NDEBUG
  // The bot-fixed instructions are reserved as the last BotFixedInstrCount
  // standalone MIs of the block (NOP bundles dropped); verify the first real
  // band instruction sits exactly at that offset, matching what
  // bot_fixed_instrs() assumes.
  if (BotFixedInstrCount > 0) {
    const MachineInstr *FirstBandMI = nullptr;
    for (const MachineBundle &B : Bundles)
      if (!B.getInstrs().empty()) {
        FirstBandMI = B.getInstrs().front();
        break;
      }
    assert(&*std::prev(BB->end(), BotFixedInstrCount) == FirstBandMI &&
           "Bot-fixed instructions are not at the block bottom.");
  }
#endif
  // SemanticOrder was captured during the gathering phase before the fixed
  // bundles were inserted into the block, so it already contains only the
  // free instructions. No adjustment is needed.
}

BlockState::BlockState(MachineBasicBlock *Block) : TheBlock(Block) {
  setBlockProperties();
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

void BlockState::setBlockProperties() {
  // We use the classification engine as a place to determine if this block
  // is the epilogue of an outerloop pipelined loop.
  IsEpilogueOfOuterPipelinedLoop =
      AIELoopUtils::isOuterLoopPipelined(*TheBlock);

  // We never skip AA check. Except for epilogues of outer-pipelined loops.
  // This is a pre-condition of the optimization (sometimes restrict information
  // may not help).
  auto Overrides = AIE::LoopOptionOverrides(*TheBlock);
  IsSafeToIgnoreMemDeps = IsEpilogueOfOuterPipelinedLoop
                              ? true
                              : Overrides.get(PostSchedIgnoreMemoryDeps);
}

InterBlockEdges *BlockState::getLoopSelfEdge() {
  for (auto &SEPtr : PerSuccEdges)
    if (SEPtr->getSucc() == TheBlock)
      return SEPtr.get();
  return nullptr;
}

const InterBlockEdges *BlockState::getLoopSelfEdge() const {
  for (auto &SEPtr : PerSuccEdges)
    if (SEPtr->getSucc() == TheBlock)
      return SEPtr.get();
  return nullptr;
}

void BlockState::initInterBlock(const MachineSchedContext &Context,
                                const AIEHazardRecognizer &HR) {
  assert(Kind == BlockType::Loop);
  assert(all_of(Regions,
                [](const Region &R) {
                  return R.top_fixed_instrs().empty() &&
                         R.bot_fixed_instrs().empty();
                }) &&
         "Loop cannot have fixed instructions");
  if (Regions.size() == 1) {
    // Don't worry, this just constructs a mostly empty container class
    auto NumInstrs = getTop().getFreeInstructions().size();
    PostSWP = std::make_unique<PostPipeliner>(HR, NumInstrs);

    // Perform static assignment of multi-slot pseudos.
    if (EnableMultiSlotInstrMaterialization &&
        PostSWP->isPostPipelineCandidate(*TheBlock)) {
      staticallyMaterializeMultiSlotInstructions(*TheBlock, HR,
                                                 MaterializePipeline);
    }
  }
}

std::optional<SWPEpilogueContext>
InterBlockScheduling::getSWPEpilogueContext(MachineBasicBlock *MBB) {

  BlockState &BS = getBlockState(MBB);
  if (BS.Kind != BlockType::Epilogue)
    return std::nullopt;

  BlockState &LoopBS = getBlockState(*MBB->pred_begin());

  if (!LoopBS.isPipelined())
    return std::nullopt;

  return SWPEpilogueContext{LoopBS.getTop().Bundles,
                            LoopBS.getPostSWP().getFinalMinTripCount()};
}

} // namespace llvm::AIE
