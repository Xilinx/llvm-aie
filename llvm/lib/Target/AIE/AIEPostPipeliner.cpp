//===- AIEPostPipeliner.cpp - Post RA Pipeliner                            ===//
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

#include "AIEPostPipeliner.h"
#include "AIEBaseRegisterInfo.h"
#include "AIESWPSolver.h"
#include "AIESlotUtils.h"
#include "Utils/AIELoopUtils.h"
#include "Utils/AIEMachineInstrPrint.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/ResourceScoreboard.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/CodeGen/ScheduleDAGInstrs.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include <limits>
#include <string>

#define DEBUG_TYPE "postpipeliner"
#define DEBUG_SUMMARY(X) DEBUG_WITH_TYPE("postpipeliner-summary", X)
#define DEBUG_FULL(X) DEBUG_WITH_TYPE("postpipeliner-full", X)

namespace llvm::AIE {
using namespace Solver;

static cl::opt<int>
    Heuristic("aie-postpipeliner-heuristic",
              cl::desc("Select one specific post-pipeliner heuristic"),
              cl::init(-1), cl::Hidden);
static cl::opt<int>
    HeuristicRuns("aie-postpipeliner-heuristic-runs",
                  cl::desc("Number of runs for heuristics that converge"),
                  cl::init(20), cl::Hidden);

static cl::opt<int> PresetII("aie-postpipeliner-target-ii",
                             cl::desc("II for which to allow the solver"),
                             cl::init(0), cl::Hidden);

// Debug option. Setting it to one will implement the linear schedule
// without pipeline parallelism.
static cl::opt<int>
    ForcedStageCount("aie-postpipeliner-force-stagecount",
                     cl::desc("Extract a pipeline with the given stage"
                              " count. This is only granted if it divides the"
                              " computed stage count."),
                     cl::init(0), cl::Hidden);

PipelineScheduleVisitor::~PipelineScheduleVisitor() {}

std::optional<int> PostPipelinerStrategy::fitInInterval(
    const SUnit &SU, int First, int Last, int II, const AIEHazardRecognizer &HR,
    ResourceScoreboard<FuncUnitWrapper> &Scoreboard) {
  MachineInstr &MI = *SU.getInstr();
  assert(First <= Last);
  const int Step = fromTop() ? 1 : -1;
  if (Step < 0) {
    std::swap(First, Last);
  }

  const int Limit = Last + Step;
  for (int C = First; C != Limit; C += Step) {
    const int Mod = C % II;
    if (!HR.checkConflict(Scoreboard, MI, Mod)) {
      return C;
    }
  }

  return std::nullopt;
}

class PostPipelineDumper : public PipelineScheduleVisitor {
public:
  PostPipelineDumper() : PipelineScheduleVisitor() {}
  void startPrologue() override { dbgs() << "Prologue:\n"; }
  void startLoop() override { dbgs() << "Loop:\n"; }
  void startEpilogue() override { dbgs() << "Epilogue:\n"; }
  void startBundle() override { dbgs() << "\tBUNDLE {\n"; }
  void addToBundle(MachineInstr *MI) override { dbgs() << "\t\t" << *MI; }
  void endBundle() override { dbgs() << "\t}\n"; }
};

// The core of the PostPipeliner is simple. We are presented with a DAG that
// represents enough copies of the body to reach the steady state of the loop.
// NInstr is the number of instructions in the original body, the number of
// copies follows from the total number of SUnits.
// We schedule the first copy -- currently with a very simple ASAP
// heuristic. The pipelined loop replicates this schedule for each iteration,
// so the next copies are checked to fit in the same cycle modulo II.
// This checks that loop carried latencies are satisfied, and that resources
// that are still blocked from earlier copies are free.
// The resource state is maintained in a ResourceScoreboard that is advances
// by II for each copy.
// The latency state is maintained in an 'Earliest' entry for each SUnit,
// which is updated whenvever we schedule a predecessor of that SUnit.

PostPipeliner::PostPipeliner(const AIEHazardRecognizer &HR, int NInstr)
    : HR(HR), NInstr(NInstr) {}

bool PostPipeliner::isPostPipelineCandidate(MachineBasicBlock &LoopBlock) {
  // We leave the single-block loop criterion to our caller. It is fulfilled
  // by being a loopaware scheduling candidate.
  // First get us some instruments
  const auto &ST = LoopBlock.getParent()->getSubtarget();
  TII = static_cast<const AIEBaseInstrInfo *>(ST.getInstrInfo());

  // 1. We need ZOL
  auto Terminator = LoopBlock.getFirstInstrTerminator();
  if (Terminator == LoopBlock.end() ||
      !TII->isHardwareLoopEnd((*Terminator).getOpcode())) {
    LLVM_DEBUG(dbgs() << " PostPipeliner: No ZOL\n");
    return false;
  }
  // 2. We need to fix the tripcount and to push out a time-critical prologue.
  // If we don't have a dedicated preheader that is fallthrough, don't even
  // bother.
  Preheader = AIELoopUtils::getDedicatedFallThroughPreheader(LoopBlock);
  if (!Preheader) {
    LLVM_DEBUG(dbgs() << " PostPipeliner: No fallthrough preheader\n");
    return false;
  }

  // 2b. Lock instructions require special scheduling constraints (core
  // stall/resume cycles) that are not implemented for software pipelined loops.
  if (TII->hasLockInstruction(LoopBlock)) {
    LLVM_DEBUG(dbgs() << " PostPipeliner: Loop contains lock instruction\n");
    return false;
  }

  // 3. We need to know how to update the tripcount. We check whether
  // the tripcount is pristine, otherwise the loop may have been software
  // pipelined before and we can't trust min itercount metadata.
  // Return on investment is probably low anyway.
  const bool Pristine = true;
  TripCountDef = TII->findZOLTripCountDef(*Preheader, Pristine);
  if (!TripCountDef) {
    LLVM_DEBUG(dbgs() << " PostPipeliner: No tripcount def\n");
    return false;
  }

  // 4. We need to peel stages and be left with a positive tripcount.
  // This is just a minimum check to save useless work; the real stage
  // count is checked before accepting the schedule.
  using namespace AIELoopUtils;
  auto ParsedMinTripCount = getMinTripCount(LoopBlock);
  if (!ParsedMinTripCount) {
    LLVM_DEBUG(dbgs() << " PostPipeliner: No min tripcount\n");
    return false;
  }
  MinTripCount = *ParsedMinTripCount;
  if (MinTripCount < 2) {
    LLVM_DEBUG(dbgs() << " PostPipeliner: min tripcount < 2\n");
    return false;
  }

  if (PresetII) {
    TargetII = PresetII;
    return true;
  }
  auto ParsedInitiationInterval = getInitiationInterval(getLoopID(LoopBlock));
  if (ParsedInitiationInterval) {
    TargetII = *ParsedInitiationInterval;
    DEBUG_SUMMARY(dbgs() << " PostPipeliner: TargetII=" << TargetII << "\n");
  }

  return true;
}

namespace {

// Our definition of side-effect free. There are no implicit defs, no stores
// and it doesn't touch anything that is live in to the loop.
// We explicitly use the fact that out-of-bounds loads do not cause an
// exception.
bool isSideEffectFree(MachineInstr *MI) {
  if (MI->getNumImplicitOperands() != 0 || MI->mayStore() ||
      MI->hasUnmodeledSideEffects()) {
    return false;
  }

  // FIFO operations modify persistent hardware state (the FIFO
  // position register). Executing an extra copy corrupts the FIFO
  // state and is thus not side effect free.
  const auto &TRI = static_cast<const AIEBaseRegisterInfo &>(
      *MI->getMF()->getSubtarget().getRegisterInfo());

  return !any_of(MI->defs(), [MBB = MI->getParent(),
                              &TRI](MachineOperand &Def) {
    Register Reg = Def.getReg();
    unsigned SubReg = Def.getSubReg();
    // Get the lane mask for the def operand: if it has a subreg, use that
    // subreg's lane mask; otherwise assume all lanes are defined.
    LaneBitmask DefLaneMask =
        SubReg ? TRI.getSubRegIndexLaneMask(SubReg) : LaneBitmask::getAll();
    return any_of(MBB->getLiveIns(),
                  [Reg, DefLaneMask,
                   &TRI](const MachineBasicBlock::RegisterMaskPair &LiveIn) {
                    // Check if registers overlap AND the lane masks intersect.
                    return TRI.regsOverlap(Reg, LiveIn.PhysReg) &&
                           (DefLaneMask & LiveIn.LaneMask).any();
                  });
  });
}

} // namespace

int PostPipeliner::getResMII(MachineBasicBlock &LoopBlock) {
  // Add up per-slot instruction counts using primary slot sets.
  // Conflict sets are not used here because multi-slot instructions
  // (e.g. XM) accumulate conflicts from all constituent slots (X + M),
  // causing them to dominate the maximum and over-estimate ResMII.
  SlotCounts Counts;
  for (auto &MI : LoopBlock) {
    Counts += getSlotCounts(MI.getOpcode(), TII);
  }
  int MII = Counts.max();
  LLVM_DEBUG(dbgs() << "PostPipeliner: ResMII=" << MII << "\n");
  return MII;
}

// This assigns Cycle of SU, Earliest of its successors and Latest of its
// predecessors
void PostPipeliner::scheduleNode(SUnit &SU, int Cycle,
                                 PostPipelinerStrategy &Strategy) {
  LLVM_DEBUG(dbgs() << "PostPipelined SU" << SU.NodeNum << " in cycle " << Cycle
                    << ": " << *SU.getInstr());
  Info[SU.NodeNum].Cycle = Cycle;

  LLVM_DEBUG(dbgs() << "  Pushed succs Earliest: ");
  for (auto &Dep : SU.Succs) {
    int Latency = Dep.getSignedLatency();
    auto *Succ = Dep.getSUnit();
    if (Succ->isBoundaryNode()) {
      continue;
    }
    const int SNum = Succ->NodeNum;
    const int OldEarliest = Strategy.earliest(*Succ);
    const int NewEarliest = Cycle + Latency;
    if (NewEarliest > Strategy.earliest(*Succ)) {
      LLVM_DEBUG(dbgs() << "SU" << SNum << " from " << OldEarliest << " to "
                        << NewEarliest << " ; ");
      Info[SNum].LastEarliestPusher = SU.NodeNum;
      Info[SU.NodeNum].NumPushedEarliest++;
      Strategy.setEarliest(SNum, NewEarliest);
      Strategy.setChanged();
    }
  }
  LLVM_DEBUG(dbgs() << "\n  Pushed preds Latest: ");
  for (auto &Dep : SU.Preds) {
    int Latency = Dep.getSignedLatency();
    auto *Pred = Dep.getSUnit();
    if (Pred->isBoundaryNode()) {
      continue;
    }
    const int PNum = Pred->NodeNum;
    const int OldLatest = Strategy.latest(*Pred);
    const int NewLatest = Cycle - Latency;
    if (NewLatest < OldLatest) {
      LLVM_DEBUG(dbgs() << "SU" << PNum << " from " << OldLatest << " to "
                        << NewLatest << " ; ");
      Info[PNum].LastLatestPusher = SU.NodeNum;
      Info[SU.NodeNum].NumPushedLatest++;
      Strategy.setLatest(PNum, NewLatest);
      Strategy.setChanged();
    }
  }
  LLVM_DEBUG(dbgs() << "\n");

  int Next = SU.NodeNum + NInstr;
  if (Next < int(Info.Nodes.size())) {
    Info[Next].Earliest = std::max(Info[Next].Earliest, Cycle + II);
  }
}

// Account for predecessor that require the same resources by pushing Earliest
// further.
void PostPipeliner::biasForLocalResourceContention(NodeInfo &NI,
                                                   const SUnit &SU) {
  SlotCounts Slots(NI.Slots);
  int PredEarliest = std::numeric_limits<int>::max();
  SmallSet<int, 8> UniqueAncestors;
  int Count = 0;

  for (const SDep &Dep : SU.Preds) {
    if (Dep.getKind() != SDep::Data) {
      continue;
    }
    int P = Dep.getSUnit()->NodeNum;
    const NodeInfo &Pred = Info[P];
    auto [It, Inserted] = UniqueAncestors.insert(P);
    if (Inserted) {
      Slots += Pred.Slots;
      Count++;
    }
    PredEarliest = std::min(PredEarliest, Pred.Earliest);
  }

  // When we need more slots than we have data predecessors, we have local
  // resource contention that we can safely account for in Earliest.
  if (Count > 0 && Slots.max() > Count) {
    const int NewEarliest = PredEarliest + Slots.max() - 1;
    if (NewEarliest > NI.Earliest) {
      LLVM_DEBUG(dbgs() << "  SU" << SU.NodeNum << " MaxSlots=" << Slots.max()
                        << ": Earliest " << NI.Earliest << " -> " << NewEarliest
                        << "\n");
      NI.Earliest = NewEarliest;
    }
  }
}

void PostPipeliner::computeForward() {
  // The forward order defines a topological sort, so we can compute
  // Earliest and Ancestors in a single forward sweep
  for (int K = 0; K < NInstr; K++) {
    LLVM_DEBUG(dbgs() << "computeForward SU" << K << "\n");
    auto &Me = Info[K];
    SUnit &SU = DAG->SUnits[K];

    // Give a more realistic Earliest if preds require similar resources.
    biasForLocalResourceContention(Me, SU);

    // Accumulate all data predecessors.
    for (auto &Dep : SU.Preds) {
      if (Dep.getKind() != SDep::Data) {
        continue;
      }
      int P = Dep.getSUnit()->NodeNum;
      assert(P < K);
      const NodeInfo &Pred = Info[P];
      Me.Ancestors.insert(P);
      for (int Anc : Pred.Ancestors) {
        Me.Ancestors.insert(Anc);
      }
    }

    // Propagate Earliest to successors
    for (auto &Dep : SU.Succs) {
      auto *Succ = Dep.getSUnit();
      if (Succ->isBoundaryNode()) {
        continue;
      }
      auto &SInfo = Info[Succ->NodeNum];
      const int NewEarliest = Me.Earliest + Dep.getSignedLatency();
      if (NewEarliest > SInfo.Earliest) {
        LLVM_DEBUG(dbgs() << "  SU" << Succ->NodeNum << " : Earliest "
                          << SInfo.Earliest << " -> " << NewEarliest << "\n");
        SInfo.Earliest = NewEarliest;
      }
    }
  }
}

bool PostPipeliner::computeBackward() {
  bool Changed = false;

  auto AddOffspring = [&Changed](NodeInfo &Info, int E) {
    if (Info.Offspring.insert(E).second) {
      Changed = true;
    }
  };

  // Traversing backwards will speed convergence a bit
  for (int K = NInstr - 1; K >= 0; K--) {
    SUnit &SU = DAG->SUnits[K];
    auto &Me = Info[K];
    const int Latest = Info[K].Latest;
    for (auto &Dep : SU.Preds) {
      if (Dep.getKind() != SDep::Data) {
        continue;
      }
      int P = Dep.getSUnit()->NodeNum;
      auto &Pred = Info[P];
      AddOffspring(Pred, K);
      for (auto Offs : Me.Offspring) {
        AddOffspring(Pred, Offs);
      }
      int NewLatest = Latest - Dep.getSignedLatency();
      if (NewLatest < Pred.Latest) {
        Pred.Latest = NewLatest;
        Changed = true;
      }
    }
  }
  return Changed;
}

// This is a direct access cache. The outer optional says it's not present.
// the inner says it's not a circuit.
// Note that the height we compute is specific for a particular end node, so
// it would be a bit bulky to save everything in NodeInfo.
using HCache = std::vector<std::optional<std::optional<int>>>;

namespace {
std::optional<int> computeHeight(HCache &Heights, SUnit *Start, SUnit *End) {
  if (Start == End) {
    // We've reached our target, and we add nothing to the height.
    // We can't conclude this is the longest path from start to end,
    // but we will visit all paths.
    return 0;
  }
  if (Start->NodeNum > End->NodeNum) {
    // We are topologically ordered, so any forward edge to End will start
    // from an earlier one. We can prune, knowing we will not find a circuit.
    return {};
  }
  // We memoize the result, which prevents searching path suffixes
  // multiple times.
  auto Cached = Heights[Start->NodeNum];
  if (Cached) {
    return *Cached;
  }
  std::optional<int> Height;
  for (auto &Dep : Start->Succs) {
    SUnit *Dst = Dep.getSUnit();
    auto SuccHeight = computeHeight(Heights, Dst, End);
    if (SuccHeight) {
      int NewHeight = *SuccHeight + Dep.getSignedLatency();
      if (Height) {
        Height = std::max(NewHeight, *Height);
      } else {
        Height = NewHeight;
      }
    }
    // else not a cycle, so don't count
  }
  // We have definitely evaluated Height, even if it is none_opt
  Heights[Start->NodeNum] = Height;
  DEBUG_FULL(dbgs() << " Circuit height(" << Start->NodeNum << "->"
                    << End->NodeNum << ")=" << Height << "\n");
  return Height;
}
} // namespace

// We compute the Minimum Initiation Interval given by recurrences (or
// circuits) in the dependence graph.
// A recurrence is identified by a backedge for which there is a path
// between Dst and Src. Backedges are defined by edges between the
// first and the second iteration.
// The length of each circuit is computed by a depth-first traversal
// starting from Dst, trying to find Src. Revisiting paths is prevented by
// caching the height to Src.

void PostPipeliner::computeRecMII() {
  RecMII = 0;
  for (int K = 0; K < NInstr; K++) {
    SUnit &Src = DAG->SUnits[K];
    for (auto &Dep : Src.Succs) {
      SUnit *Dst = Dep.getSUnit();
      if (Dst->NodeNum >= unsigned(NInstr)) {
        NodeInfo Me = Info[K];
        int SNum = Dst->NodeNum - NInstr;
        if (Me.Ancestors.count(SNum)) {
          // The successor is represented by one of
          // my ancestors. That means we have a circuit,
          // which may be the longest one. We find the longest path between
          // that ancestor and this node, adding the latency of the
          // backedge.
          HCache Heights(NInstr);
          auto Height = computeHeight(Heights, &DAG->SUnits[SNum], &Src);
          assert(Height);

          int Circuit = *Height + Dep.getSignedLatency();
          RecMII = std::max(Circuit, RecMII);
          LLVM_DEBUG(dbgs() << "Backedge " << K << " -> " << SNum
                            << " circuit=" << Circuit << "\n");
        }
      }
    }
  }
  LLVM_DEBUG(dbgs() << "RecMII=" << RecMII << "\n");
}

void PostPipeliner::computeEffectiveHeight() {
  // Walk nodes bottom-up to compute the length of the critical path
  // below each node. Only slack-free edges contribute: if a successor
  // is already driven by a longer independent path, the edge does not
  // lie on the critical path and is ignored.
  for (int K = NInstr - 1; K >= 0; K--) {
    const SUnit &SU = DAG->SUnits[K];
    int MaxEH = 0;
    for (const SDep &Dep : SU.Succs) {
      if (Dep.getKind() != SDep::Data)
        continue;

      const int S = Dep.getSUnit()->NodeNum;
      // Only consider Instructions from the first Iteration
      const bool IsLoopCarried = S >= NInstr;
      if (IsLoopCarried)
        continue;

      const int Latency = Dep.getSignedLatency();
      // Skip edges not on the critical path: the successor is already
      // pushed later by another predecessor, so this edge has slack.
      const bool HasSlack = Info[K].Earliest + Latency < Info[S].Earliest;
      if (HasSlack)
        continue;

      // Accumulate height along this critical edge
      const int Candidate = Latency + Info[S].EffectiveHeight;
      MaxEH = std::max(MaxEH, Candidate);
    }
    Info[K].EffectiveHeight = MaxEH;
    LLVM_DEBUG(dbgs() << "SU" << K << " EffectiveHeight="
                      << Info[K].EffectiveHeight << "\n");
  }
}

bool PostPipeliner::computeLoopCarriedParameters() {

  // Initialize slot counts.
  for (int K = 0; K < NInstr; K++) {
    auto *MI = DAG->SUnits[K].getInstr();
    Info[K].Slots = getConflictCounts(MI->getOpcode(), TII);
  }

  // Forward properties like Earliest and Ancestors.
  computeForward();

  // Backward properties like Latest and Offspring.
  // Use a fixpoint loop, because plain reversed order may not be topological
  // for predecessors
  while (computeBackward()) {
    /* EMPTY */;
  }

  // Compute the Recurrence Minimum Initiation Interval. The current code
  // structure makes it difficult to use RecMII as the starting II for the
  // main pipeliner loop, but it still can be used to reject very early on.
  computeRecMII();

  // Adjust Earliest and Latest with resource requirements.
  // FIXME: We do not account for negative latencies here. This can lead to
  // suboptimality, but we only include true dependences, where negative
  // latencies are rare.
  for (int K = 0; K < NInstr; K++) {
    auto &Me = Info[K];
    SlotCounts ASlots(Me.Slots);
    for (int A : Me.Ancestors) {
      assert(A < NInstr && "Ancestor index must be < NInstr");
      ASlots += Info[A].Slots;
    }
    SlotCounts OSlots(Me.Slots);
    for (int O : Me.Offspring) {
      assert(O < NInstr && "Offspring index must be < NInstr");
      OSlots += Info[O].Slots;
    }
    LLVM_DEBUG(dbgs() << "SU" << K << " : " << Info[K].Earliest << " - "
                      << Info[K].Latest << " " << ASlots << " " << OSlots
                      << "\n");
    Me.Earliest = std::max(Me.Earliest, 0 + (ASlots.max() - 1));
    Me.Latest = std::min(Me.Latest, -1 - (OSlots.max() - 1));
    LLVM_DEBUG(dbgs() << "    -> " << Info[K].Earliest << " - "
                      << Info[K].Latest << "\n");
  }

  // Loop carried dependences will have pushed away Earliest of the second
  // iteration, which should stay in lock step with the first.
  for (int K = 0; K < NInstr; K++) {
    const int KNextIter = K + NInstr;
    const int Earliest = Info[KNextIter].Earliest - II;
    Info[K].Earliest = std::max(Info[K].Earliest, Earliest);
    LLVM_DEBUG(dbgs() << "SU" << K << " LCD: Earliest=" << Info[K].Earliest
                      << "(Modulo SU" << KNextIter
                      << " Earliest=" << Info[KNextIter].Earliest << ")\n");
  }

  // Make Earliest of the second iteration push up Latest of the first
  for (int K = 0; K < NInstr; K++) {
    auto &Me = Info[K];
    int LCDLatest = Me.Latest;
    auto &SU = DAG->SUnits[K];
    for (auto &Dep : SU.Succs) {
      const int S = Dep.getSUnit()->NodeNum;
      if (S < NInstr) {
        continue;
      }
      const int Earliest = Info[S - NInstr].Earliest;
      const int Latest = Earliest - Dep.getSignedLatency();
      LCDLatest = std::min(LCDLatest, Latest);
    }
    Me.LCDLatest = LCDLatest;
    if (LCDLatest != Me.Latest) {
      LLVM_DEBUG(dbgs() << "SU" << K << " LCDLatest=" << Me.LCDLatest << "\n");
    }
  }

  computeEffectiveHeight();

  // Save the static values for ease of reset
  for (auto &N : Info.Nodes) {
    N.StaticEarliest = N.Earliest;
    N.StaticLatest = N.Latest;
  }

  MinLength = computeMinScheduleLength();
  return true;
}

int PostPipeliner::computeMinScheduleLength() const {
  // The minimum length makes sure that every node has a range in which it
  // can be scheduled
  int MinLength = II;
  for (int K = 0; K < NInstr; K++) {
    auto &Node = Info[K];
    while (Node.Earliest > Node.Latest + MinLength) {
      MinLength += II;
    }
  }
  return MinLength;
}

namespace {

const char *getEdgeColor(SDep::Kind Kind) {
  switch (Kind) {
  case SDep::Data:
    return "red";
  case SDep::Output:
    return "black";
  case SDep::Anti:
    return "blue";
  case SDep::Order:
    return "green";
  }
  return "gray";
}

// Returns edge attributes string including label (latency + register) and
// color.
std::string edgeAttributes(const SDep &Dep, const TargetRegisterInfo *TRI) {
  std::string Label = std::to_string(Dep.getSignedLatency());
  switch (Dep.getKind()) {
  case SDep::Data:
  case SDep::Output:
  case SDep::Anti: {
    const Register Reg = Dep.getReg();
    if (Reg.isPhysical()) {
      Label += " ";
      Label += TRI->getName(Reg);
    } else if (Reg.isVirtual()) {
      Label += " VR";
      Label += std::to_string(Reg.virtRegIndex());
    }
    break;
  }
  case SDep::Order:
    break;
  }
  return "[label=\"" + Label + "\", color=" + getEdgeColor(Dep.getKind()) + "]";
}

void dumpGraph(const ScheduleInfo &Info, ScheduleDAGInstrs *DAG,
               StringRef GraphId) {
  dbgs() << "digraph " << GraphId << " {\n";
  const auto *TRI = DAG->MF.getSubtarget().getRegisterInfo();

  // Collect backedge sources and destinations for mirroring.
  // Store the full SDep to preserve latency, kind, and register information.
  SmallVector<std::tuple<int, int, SDep>, 16> Lcds;
  SmallSet<int, 16> LcdSrc;
  SmallSet<int, 16> LcdDst;

  for (int S = 0; S < Info.NInstr; S++) {
    const SUnit &SU = DAG->SUnits[S];
    for (const SDep &Dep : SU.Succs) {
      const int D = Dep.getSUnit()->NodeNum;
      if (D < Info.NInstr) {
        continue;
      }
      const int D0 = D % Info.NInstr;
      if (D0 == S) {
        continue;
      }
      // This is a backedge from S to D in the next iteration.
      // Add it to the Lcds, and register src and dst nodes.
      Lcds.emplace_back(S, D0, Dep);
      LcdSrc.insert(S);
      LcdDst.insert(D0);
    }
  }

  // Create rectangle nodes representing split sources and destinations.
  for (const auto &Src : LcdSrc) {
    dbgs() << format("\tSU%d_src [shape=rectangle, label=SU%d]\n", Src, Src);
  }
  for (const auto &Dst : LcdDst) {
    dbgs() << format("\tSU%d_dst [shape=rectangle, label=SU%d]\n", Dst, Dst);
  }

  // Add node labels with interval information (Depth and Height) and opcode.
  for (int K = 0; K < Info.NInstr; K++) {
    const SUnit &SU = DAG->SUnits[K];
    const int Depth = SU.getDepth();
    const int Height = SU.getHeight();

    dbgs() << "\tSU" << K << " [label=\"SU" << K << "\\n"
           << OpcodeOnly(*SU.getInstr(), 10) << "\\n[" << Depth << "," << Height
           << "]\\n"
           << "\"]\n";
  }

  // Emit loop-carried dependency edges (mirror edges).
  for (const auto &[Src, Dst, Dep] : Lcds) {
    const std::string Attrs = edgeAttributes(Dep, TRI);
    // Create an edge from the split source to the destination.
    dbgs() << format("\tSU%d_src -> SU%d ", Src, Dst) << Attrs << "\n";
    // Create an edge from the source to the split destination.
    dbgs() << format("\tSU%d -> SU%d_dst ", Src, Dst) << Attrs << "\n";
  }

  // Emit regular (intra-iteration) edges.
  for (int K = 0; K < Info.NInstr; K++) {
    const SUnit &SU = DAG->SUnits[K];
    for (const SDep &Dep : SU.Succs) {
      const SUnit *Succ = Dep.getSUnit();
      const int S = Succ->NodeNum;
      if (S >= Info.NInstr || S % Info.NInstr == K || Succ->isBoundaryNode()) {
        continue;
      }
      dbgs() << "\tSU" << K << " -> SU" << S << " " << edgeAttributes(Dep, TRI)
             << "\n";
    }
  }
  dbgs() << "}\n";
}

char slotLetter(const SlotCounts &Slots) {
  // Slots are sorted by name in tablegen.
  // alu, lda, ldb, lng, mov, nop, st, vec
  const char *const L = "XABLMNSVW9";

  for (int I = 0; I < 10; I++) {
    if (Slots[I] > 0) {
      return L[I];
    }
  }
  return '*';
}

void dumpSchedule(const ScheduleInfo &Info, int MinLength, int II,
                  std::function<bool(int I, int K)> Select) {
  for (int K = 0; K < Info.NInstr; K++) {
    char S = slotLetter(Info[K].Slots);
    std::string Head = "SU" + std::to_string(K);
    dbgs() << Head;
    for (int I = Head.length() - 6; I < MinLength; I++) {
      if (I >= 0 && I % II == 0) {
        dbgs() << "|";
      }
      if (Select(I, K)) {
        dbgs() << S;
      } else {
        dbgs() << " ";
      }
    }
    dbgs() << "]\n";
  }
}

void dumpIntervals(const ScheduleInfo &Info, int MinLength, int II) {
  dbgs() << "Intervals:\n";
  dumpSchedule(Info, MinLength, II, [&](int I, int K) {
    return I >= Info[K].Earliest && I <= MinLength + Info[K].Latest;
  });
}

void dumpCycles(const ScheduleInfo &Info, int II) {
  int FullStageLength = 0;
  while (FullStageLength < Info.Length) {
    FullStageLength += II;
  }

  dbgs() << "Cycles:\n";
  dumpSchedule(Info, FullStageLength, II,
               [&](int I, int K) { return I == Info[K].Cycle; });
}
} // namespace

int PostPipeliner::mostUrgent(PostPipelinerStrategy &Strategy) {
  assert(FirstUnscheduled <= LastUnscheduled);
  while (Info[FirstUnscheduled].Scheduled) {
    FirstUnscheduled++;
  }
  while (Info[LastUnscheduled].Scheduled) {
    LastUnscheduled--;
  }
  assert(FirstUnscheduled <= LastUnscheduled);

  auto NotScheduled = [this](const auto &Dep) {
    auto *SU = Dep.getSUnit();
    if (SU->isBoundaryNode()) {
      return false;
    }
    int N = SU->NodeNum;
    return N < NInstr && !Info[N].Scheduled;
  };

  int Best = -1;
  LLVM_DEBUG(dbgs() << "Available:");
  for (int K = FirstUnscheduled; K <= LastUnscheduled; K++) {
    const auto &SU = DAG->SUnits[K];
    auto &Edges = Strategy.fromTop() ? SU.Preds : SU.Succs;
    // Check whether it is available
    if (Info[K].Scheduled || any_of(Edges, NotScheduled)) {
      continue;
    }
    LLVM_DEBUG(dbgs() << " SU" << K);
    if (Best == -1 || Strategy.better(SU, DAG->SUnits[Best])) {
      Best = K;
      LLVM_DEBUG(dbgs() << "*");
    }
  }
  LLVM_DEBUG(dbgs() << "\n");
  assert(Best >= 0);
  return Best;
}

void PostPipeliner::resetSchedule(bool FullReset) {
  Scoreboard.clear();
  int K = 0;
  for (auto &N : Info.Nodes) {
    N.reset(FullReset);
    if (K < NInstr) {
      N.Earliest = N.TweakedEarliest ? *N.TweakedEarliest : N.StaticEarliest;
      N.Latest = N.TweakedLatest ? *N.TweakedLatest : N.StaticLatest;
    }
    K++;
  }

  FirstUnscheduled = 0;
  LastUnscheduled = NInstr - 1;
}

bool PostPipeliner::scheduleFirstIteration(PostPipelinerStrategy &Strategy) {
  // Set up the basic schedule from the original instructions
  const int PipelineDepth = HR.getPipelineDepth();
  for (int K = 0; K < NInstr; K++) {
    const int N = mostUrgent(Strategy);
    SUnit &SU = DAG->SUnits[N];
    MachineInstr *const MI = SU.getInstr();
    const int Earliest = Strategy.earliest(SU);
    const int Latest = Strategy.latest(SU);
    LLVM_DEBUG(
        dbgs() << format("  Trying %d in [%d, %d]\n", N, Earliest, Latest));
    if (Earliest > Latest) {
      LLVM_DEBUG(dbgs() << "  Latency violation.\n");
      return false;
    }

    auto OptCycle =
        Strategy.fitInInterval(SU, Earliest, Latest, II, HR, Scoreboard);
    if (!OptCycle) {
      LLVM_DEBUG(dbgs() << "Out of resources\n");

      // The node might have been given too tight Earliest/Latest attributes.
      // Relax those to give another chance for scheduling this II.
      Info[N].TweakedEarliest = {};
      Info[N].TweakedLatest = {};
      return false;
    }
    const int Actual = *OptCycle;
    Strategy.selected(SU);
    const int ModCycle = Actual % II;
    const MemoryBankBits MemoryBanks = HR.getMemoryBanks(MI);
    const MemoryObjectPair ObjectBits = HR.getMemoryObjectsBits(MI);
    int Cycle = ModCycle;
    // We are scheduling the first iteration, checking for conflicts with other
    // instructions that were scheduled earlier.
    // Newly scheduled instruction have ModCycle < II,
    // and have no conflict beyond
    // ModCycle + PipelineDepth
    const int Horizon =
        std::min(II + PipelineDepth, ScoreboardSize - PipelineDepth);
    LLVM_DEBUG(dbgs() << "  Emit in " << Cycle << "\n");
    int Iter = 0;
    while (Cycle < Horizon) {
      if (HR.checkConflict(Scoreboard, *MI, Cycle)) {
        LLVM_DEBUG(dbgs() << "Conflict in iteration N=" << Iter << "\n");
        return false;
      }

      HR.emitInScoreboard(Scoreboard, MI->getDesc(), MemoryBanks, ObjectBits,
                          MI->operands(), MI->getMF()->getRegInfo(), Cycle);
      Cycle += II;
      Iter++;
    }

    scheduleNode(SU, Actual, Strategy);
    Info.commitCycle(N);

    DEBUG_FULL(dbgs() << "Scoreboard\n"; Scoreboard.dumpFull(););
  }

  const bool Success = checkStages();
  DEBUG_SUMMARY(dbgs() << "==== First iteration scheduled by "
                       << Strategy.name() << "====\n");
  DEBUG_SUMMARY(dumpCycles(Info, II));
  return Success;
}

namespace {
void dumpEarliestChain(const ScheduleInfo &Info, int N) {
  auto Prev = Info[N].LastEarliestPusher;
  if (Prev) {
    dumpEarliestChain(Info, *Prev);
  }
  dbgs() << "  --> SU" << N << " @" << Info[N].Cycle << "\n";
}

/// Check whether \p Target lies on the LastEarliestPusher chain starting
/// from node \p Start. If so, delaying Target will push Start's Earliest
/// by the same amount, making the delay futile for resolving a modulo
/// constraint between them.
bool isOnEarliestChain(const ScheduleInfo &Info, int Start, int Target) {
  auto Prev = Info[Start].LastEarliestPusher;
  while (Prev) {
    if (*Prev == Target)
      return true;
    Prev = Info[*Prev].LastEarliestPusher;
  }
  return false;
}

#ifndef NDEBUG
/// Recompute Earliest from direct predecessors only.
int computeEarliestFromPreds(const SUnit &SU, const ScheduleInfo &Info) {
  int Earliest = 0;
  for (const SDep &Dep : SU.Preds) {
    const SUnit *Pred = Dep.getSUnit();
    if (Pred->isBoundaryNode())
      continue;
    const NodeInfo &PredNode = Info[Pred->NodeNum];
    Earliest = std::max(Earliest, PredNode.Cycle + Dep.getSignedLatency());
  }
  return Earliest;
}
#endif
} // namespace

bool PostPipeliner::scheduleOtherIterations(PostPipelinerStrategy &Strategy) {
  // Make sure that copies can be placed at II from the previous one.
  // We only check the second iteration. This may have had earliest
  // pushed by LCDs from the first iteration. Since the dag doesn't change,
  // the third iteration behaves the same.
  for (int K = 0; K < NInstr; K++) {
    const int N = K + NInstr;
    SUnit &SU = DAG->SUnits[N];
    NodeInfo &Node = Info[N];
    const SUnit &ModuloSU = DAG->SUnits[N - NInstr];
    NodeInfo &ModuloNode = Info[N - NInstr];

#ifndef NDEBUG
    // Assert that Earliest is still consistent with scheduled predecessors.
    const int RecomputedEarliest = computeEarliestFromPreds(SU, Info);
    assert(Node.Earliest >= RecomputedEarliest &&
           "Earliest is stale: predecessor pushes a later cycle");
#endif

    // Earliest tracks the latencies of the loop carried deps
    const int Earliest = Node.Earliest;
    // Insert supplies the modulo condition.
    const int Insert = ModuloNode.Cycle + II;

    // All iterations following the first one should fit exactly
    if (Earliest > Insert) {
      LLVM_DEBUG(dbgs() << "Latency not met for SU" << N << " in cycle "
                        << Insert << " (Earliest=" << Earliest
                        << " ModuloNode=SU" << N - NInstr << ")\n";
                 dumpEarliestChain(Info, N));
      // Check whether the modulo node can be delayed to resolve the
      // violation. HasScheduleSlack means the current schedule still
      // has room. CanPlaceLaterInOriginalInterval means scheduling
      // tightened Latest beyond the original interval -- retrying
      // with a higher TweakedEarliest can produce a different
      // successor layout that doesn't squeeze as aggressively.
      // DelayReducesGap guards against futile delays where the modulo
      // node drives Node's Earliest through an LCD chain.

      // The current schedule still has room for the modulo node.
      const bool HasScheduleSlack = Strategy.mobility(ModuloSU) > 0;

      // The strategy's schedule length shifts all Latest values by a
      // constant offset. Recover it to translate StaticLatest into
      // the strategy's coordinate system.
      const int ScheduleLengthOffset =
          Strategy.latest(ModuloSU) - Info[ModuloSU.NodeNum].Latest;

      // The node's upper bound before scheduling tightened it.
      const int OriginalLatest = ModuloNode.StaticLatest + ScheduleLengthOffset;

      // The node hasn't reached the boundary of its pre-scheduling
      // interval -- there is room to push it later.
      const bool CanPlaceLaterInOriginalInterval =
          ModuloNode.Earliest < OriginalLatest;

      // The delay actually reduces the Earliest-Insert gap. If the
      // modulo node drives Node's Earliest through an LCD chain,
      // both sides advance equally and the gap stays constant.
      const bool DelayReducesGap =
          !isOnEarliestChain(Info, N, ModuloSU.NodeNum);

      const bool CanDelayModuloNode =
          HasScheduleSlack ||
          (CanPlaceLaterInOriginalInterval && DelayReducesGap);
      if (CanDelayModuloNode) {
        ModuloNode.TweakedEarliest = ModuloNode.Cycle + 1;
        Strategy.setChanged();
        LLVM_DEBUG(dbgs() << "  Try to delay SU" << N - NInstr
                          << " with TweakedEarliest= "
                          << ModuloNode.TweakedEarliest << "\n");
        return false;
      }
      if (Node.LastEarliestPusher && *Node.LastEarliestPusher < NInstr) {
        // The modulo Node cannot be delayed.
        // Instead, prioritise whatever pushed us.
        NodeInfo &Pusher = Info[*Node.LastEarliestPusher];
        if (Strategy.mobility(DAG->SUnits[*Node.LastEarliestPusher]) > 0) {
          ModuloNode.TweakedEarliest = {};
          Pusher.TweakedLatest = Pusher.Latest - 1;
          Strategy.setChanged();
          LLVM_DEBUG(dbgs()
                     << "  Try to prioritise SU" << *Node.LastEarliestPusher
                     << " with TweakedLatest= " << Pusher.TweakedLatest
                     << "\n");
          return false;
        }
      }
      return false;
    }

    scheduleNode(SU, Insert, Strategy);
  }

  // Make a final check on the resources. We bring a pristine scoreboard
  // to steady state by checking and inserting NStages. Since the steady
  // state is the busiest, we can shift the scoreboard by II after each stage.
  // We repeat the resource schedule often enough to make the final one land
  // after the conflict horizon of the first one.
  const int PipelineDepth = HR.getPipelineDepth();
  ResourceScoreboard<FuncUnitWrapper> Resources;
  Resources.config(0, 2 * II + PipelineDepth);
  for (int Start = 0; Start < II + PipelineDepth; Start += II) {
    for (int I = 0; I < NInstr; I++) {
      SUnit &SU = DAG->SUnits[I];
      MachineInstr &MI = *SU.getInstr();
      int ModCycle = Info[I].ModuloCycle;
      if (HR.checkConflict(Resources, MI, ModCycle)) {
        return false;
      }
      HR.emitInScoreboard(Resources, MI, MI.getDesc(), ModCycle);
    }
    for (int I = 0; I < II; I++) {
      Resources.advance();
    }
  }

  return true;
}

bool PostPipeliner::scheduleWithStrategy(PostPipelinerStrategy &S) {
  DEBUG_SUMMARY(dbgs() << "Starting " << S.name() << "\n");
  if (!scheduleFirstIteration(S)) {
    return false;
  }
  DEBUG_SUMMARY(dbgs() << "   First iteration successful\n");
  if (!scheduleOtherIterations(S)) {
    Info.resetRotation();
    return false;
  }
  DEBUG_SUMMARY(dbgs() << "   Other iterations successful\n");

  // Apply pending rotation from peelSideEffectFree() now that validation
  // has succeeded. This rotates the schedule to the SEF form, prefixing it
  // with empty cycles so that the first stage only contains SEF instructions.
  Info.applyRotation(II);
  Info.resetRotation();

  return true;
}

namespace {
int getMinOutputLat(ArrayRef<SDep> Edges) {
  int Min = std::numeric_limits<int>::max();
  for (const SDep &Dep : Edges) {
    if (Dep.getKind() != SDep::Output)
      continue;
    Min = std::min(Min, Dep.getSignedLatency());
  }
  return Min;
}
} // namespace

class DefaultStrategy : public PostPipelinerStrategy {
public:
  DefaultStrategy(ScheduleDAGMI &DAG, ScheduleInfo &Info, int LatestBias)
      : PostPipelinerStrategy(DAG, Info, LatestBias) {}
  bool better(const SUnit &A, const SUnit &B) override {
    return Info[A.NodeNum].Latest < Info[B.NodeNum].Latest;
  }
};

// This is a strategy that follows a pre-computed schedule. it picks
// instructions in the order of the final schedule and nudges earliest and
// latest so as to have no slack.
// It still checks latencies and resources
class CheckFixedSchedule : public PostPipelinerStrategy {
  std::vector<int> Schedule;
  // We schedule in strict top-down order, and we leave only one cycle
  // to schedule it in.
  bool better(const SUnit &A, const SUnit &B) override {
    return Schedule[A.NodeNum] < Schedule[B.NodeNum];
  }
  int earliest(const SUnit &N) override {
    int Result = PostPipelinerStrategy::earliest(N);
    unsigned NodeNum = N.NodeNum;
    if (NodeNum < Schedule.size()) {
      Result = std::max(Result, Schedule[NodeNum]);
    }
    return Result;
  }
  int latest(const SUnit &N) override {
    int Result = PostPipelinerStrategy::latest(N);
    unsigned NodeNum = N.NodeNum;
    if (NodeNum < Schedule.size()) {
      Result = std::min(Result, Schedule[NodeNum]);
    }
    return Result;
  }

public:
  CheckFixedSchedule(ScheduleDAGInstrs &DAG, ScheduleInfo &Info, int Length,
                     std::vector<int> Schedule)
      : PostPipelinerStrategy(DAG, Info, Length), Schedule(Schedule) {}
  std::string name() override { return "CheckFixedSchedule"; }
};

// This strategy is specifically to have a high chance of success in peeling
// off a side-effect-free first stage, so that we can have some relaxation
// on minitercount
// The plan is to target one full stage more than the minimum,
// select side-effect free instructions on an initial top-down phase, and do the
// rest bottom-up. This improves the chances to have a gap between those
// two regions, which is exactly what we need in order to separate that first
// stage from the others.
// For now, we assume the extra slack stage means we don't have to be too
// clever, we just use earliest
class IterCountSlackStrategy : public PostPipelinerStrategy {
  bool TopDown = true;

private:
  bool fromTop() override { return TopDown; }

  bool better(const SUnit &A, const SUnit &B) override {
    if (!TopDown) {
      // Something simple suitable for bottom-up.
      return Info[A.NodeNum].Earliest > Info[B.NodeNum].Earliest;
    }
    const bool SEFA = isSideEffectFree(A.getInstr());
    const bool SEFB = isSideEffectFree(B.getInstr());
    if (SEFA > SEFB) {
      return true;
    }
    if (SEFA < SEFB) {
      return false;
    }
    // Both are equal, use a simple Top-Down heuristic
    return Info[A.NodeNum].Latest > Info[B.NodeNum].Latest;
  }

  void selected(const SUnit &N) override {
    // The once-only transition to bottom-up.
    if (TopDown && !isSideEffectFree(N.getInstr())) {
      TopDown = false;
    }
  }

public:
  std::string name() override { return "IterCountSlackStrategy"; }
  IterCountSlackStrategy(ScheduleDAGInstrs &DAG, ScheduleInfo &Info, int Length)
      : PostPipelinerStrategy(DAG, Info, Length) {}
};

class ConfigStrategy : public PostPipelinerStrategy {
  bool TopDown = true;
  bool Alternate = false;

public:
  enum PriorityComponent {
    NodeNum,
    Latest,
    Critical,
    Sibling,
    LCDLatest,
    DepLength, // Schedule "as deep as possible" first
    Liveness,  // Minimise liveness by looking at output deps
    EffHeight, // Prefer nodes with more critical path below them
    Size
  };

  // Placement modifiers affect which cycle a node lands in, as opposed
  // to PriorityComponents which affect which node is picked next.
  enum PlacementModifier {
    // Defer non-critical nodes (EffectiveHeight == 0) by one cycle,
    // leaving their earliest modulo slot free for critical-path nodes.
    DeferNonCritical,
    PlacementSize
  };
  static std::string getPlacementName(PlacementModifier Mod) {
    switch (Mod) {
    case PlacementModifier::DeferNonCritical:
      return "Defer";
    default:
      break;
    }
    return "PlacementSize - Illegal";
  };
  static std::string getPriorityName(PriorityComponent Component) {
    switch (Component) {
    case PriorityComponent::NodeNum:
      return "NodeNum";
    case PriorityComponent::Latest:
      return "Latest";
    case PriorityComponent::Critical:
      return "Critical";
    case PriorityComponent::Sibling:
      return "Sibling";
    case PriorityComponent::LCDLatest:
      return "LcdLatest";
    case PriorityComponent::DepLength:
      return "DepLength";
    case PriorityComponent::Liveness:
      return "Liveness";
    case PriorityComponent::EffHeight:
      return "EffHeight";
    default:
      break;
    }
    return "Size - Illegal";
  }
  struct Configuration {
    int ExtraStages = 0;
    bool TopDown = true;
    bool Alternate = false;
    int Runs = 0;
    SmallVector<PriorityComponent, 4> Components;
    SmallVector<PlacementModifier, 2> Modifiers;
  };

private:
  std::string Name;
  SmallVector<PlacementModifier, 2> Modifiers;
  std::set<int> SuccSiblingScheduled;
  std::set<int> PredSiblingScheduled;
  std::function<bool(const SUnit &A, const SUnit &B)>
      Discriminators[PriorityComponent::Size] = {
          [&](const SUnit &A, const SUnit &B) {
            return TopDown ? A.NodeNum < B.NodeNum : A.NodeNum > B.NodeNum;
          },
          [&](const SUnit &A, const SUnit &B) {
            auto &IA = Info[A.NodeNum];
            auto &IB = Info[B.NodeNum];
            return TopDown ? IA.Latest < IB.Latest : IA.Earliest > IB.Earliest;
          },
          [&](const SUnit &A, const SUnit &B) {
            auto &IA = Info[A.NodeNum];
            auto &IB = Info[B.NodeNum];
            return TopDown ? IA.NumPushedEarliest > IB.NumPushedEarliest
                           : IA.NumPushedLatest > IB.NumPushedLatest;
          },
          [&](const SUnit &A, const SUnit &B) {
            std::set<int> &Sibling =
                TopDown ? SuccSiblingScheduled : PredSiblingScheduled;
            return Sibling.count(A.NodeNum) > Sibling.count(B.NodeNum);
          },
          [&](const SUnit &A, const SUnit &B) {
            auto &IA = Info[A.NodeNum];
            auto &IB = Info[B.NodeNum];
            return IA.LCDLatest < IB.LCDLatest;
          },
          [&](const SUnit &A, const SUnit &B) {
            return A.getDepth() > B.getDepth();
          },
          [&](const SUnit &A, const SUnit &B) {
            // This tries to minimise live ranges of registers by favouring
            // nodes that have successors with negative latencies.
            return getMinOutputLat(A.Succs) < getMinOutputLat(B.Succs);
          },
          [&](const SUnit &A, const SUnit &B) {
            // Prefer nodes with more critical path below them.
            // Deferring these risks extending the schedule length.
            const auto &IA = Info[A.NodeNum];
            const auto &IB = Info[B.NodeNum];
            return IA.EffectiveHeight > IB.EffectiveHeight;
          },
      };
  std::vector<PriorityComponent> Priority;

  bool fromTop() override { return TopDown; }

  bool better(const SUnit &A, const SUnit &B) override {
    for (auto P : Priority) {
      if (Discriminators[P](A, B)) {
        return true;
      }
    }
    return false;
  }

  void selected(const SUnit &N) override {
    // Promote the critical path
    NodeInfo *Pushed = &Info[N.NodeNum];
    while (Pushed->LastEarliestPusher) {
      Pushed = &Info[*Pushed->LastEarliestPusher];
      Pushed->NumPushedEarliest++;
      setChanged();
    }

    // Promote my siblings
    for (auto &SDep : N.Succs) {
      if (SDep.getKind() != SDep::Data) {
        continue;
      }
      for (auto &PDep : SDep.getSUnit()->Preds) {
        if (PDep.getKind() != SDep::Data) {
          continue;
        }
        SuccSiblingScheduled.insert(PDep.getSUnit()->NodeNum);
      }
    }
    for (auto &PDep : N.Preds) {
      if (PDep.getKind() != SDep::Data) {
        continue;
      }
      for (auto &SDep : PDep.getSUnit()->Succs) {
        if (SDep.getKind() != SDep::Data) {
          continue;
        }
        PredSiblingScheduled.insert(PDep.getSUnit()->NodeNum);
      }
    }
    if (Alternate) {
      TopDown = !TopDown;
    }
  }

public:
  std::string name() override { return Name; }
  ConfigStrategy(ScheduleDAGInstrs &DAG, ScheduleInfo &Info, int Length,
                 bool TopDown, bool Alternate,
                 ArrayRef<PriorityComponent> Components,
                 ArrayRef<PlacementModifier> Modifiers = {})
      : PostPipelinerStrategy(DAG, Info, Length), TopDown(TopDown),
        Alternate(Alternate), Modifiers(Modifiers.begin(), Modifiers.end()) {
    Name = "Config_" + std::to_string(Length) + "_" + std::to_string(TopDown) +
           "_" + std::to_string(Alternate);
    for (auto Comp : Components) {
      Name += "_" + getPriorityName(Comp);
      Priority.emplace_back(Comp);
    }
    for (auto Mod : this->Modifiers)
      Name += "_" + getPlacementName(Mod);
  }

  // Apply placement modifiers to adjust the cycle chosen for SU.
  // Currently supports DeferNonCritical, which nudges non-critical
  // nodes one cycle later so their earliest modulo slot stays free
  // for critical-path nodes.
  std::optional<int>
  fitInInterval(const SUnit &SU, int Earliest, int Latest, int II,
                const AIEHazardRecognizer &HR,
                ResourceScoreboard<FuncUnitWrapper> &Scoreboard) override {
    const bool ShouldDefer = llvm::is_contained(Modifiers, DeferNonCritical) &&
                             Info[SU.NodeNum].EffectiveHeight == 0;
    if (ShouldDefer && Earliest + 1 <= Latest) {
      // Try the deferred range [Earliest+1, Latest] first. If no
      // resource-free cycle exists there, fall back to the original
      // range so the node is never left unscheduled.
      auto Result = PostPipelinerStrategy::fitInInterval(
          SU, Earliest + 1, Latest, II, HR, Scoreboard);
      if (Result)
        return Result;
      // No need to retry [Earliest+1, Latest]
      return PostPipelinerStrategy::fitInInterval(SU, Earliest, Earliest, II,
                                                  HR, Scoreboard);
    }
    return PostPipelinerStrategy::fitInInterval(SU, Earliest, Latest, II, HR,
                                                Scoreboard);
  }
};

using Prio = ConfigStrategy::PriorityComponent;
using Plc = ConfigStrategy::PlacementModifier;
static const ConfigStrategy::Configuration Heuristics[] = {
    // Loosely speaking, a lower value of the first parameter targets
    // a lower stage count, which benefits code size.
    // Runs>1 is only useful for heuristics that use it, e.g. Critical
    // {ExtraStages, TopDown, Alternate, Runs, PriorityComponents, Modifiers}
    {1, true, false, 1, {Prio::NodeNum}, {}},
    // Tight schedule window: ExtraStages=0 keeps NS low, which is needed
    // when MinTripCount barely exceeds RecMII.
    {0, true, false, HeuristicRuns, {Prio::NodeNum}, {}},
    {1, true, false, HeuristicRuns, {Prio::Latest}, {}},
    {1, true, false, HeuristicRuns, {Prio::Critical}, {}},
    {1, true, false, HeuristicRuns, {Prio::Latest, Prio::Sibling}, {}},
    {1, true, false, HeuristicRuns, {Prio::DepLength, Prio::Latest}, {}},
    {1, true, false, HeuristicRuns, {Prio::Critical, Prio::LCDLatest}, {}},
    {1, true, false, HeuristicRuns, {Prio::Liveness, Prio::Latest}, {}},
    {1, true, false, HeuristicRuns, {Prio::Latest, Prio::Liveness}, {}},
    // EffectiveHeight: prefer nodes with more critical path below them.
    // DeferNonCritical defers non-critical nodes (EH=0) to leave early
    // modulo cycles free for the critical path.
    {1,
     true,
     false,
     HeuristicRuns,
     {Prio::EffHeight, Prio::Latest},
     {Plc::DeferNonCritical}},
    {1,
     true,
     false,
     HeuristicRuns,
     {Prio::EffHeight, Prio::Critical},
     {Plc::DeferNonCritical}},
    {1, true, false, HeuristicRuns, {Prio::EffHeight, Prio::Latest}, {}},
    {1, true, false, HeuristicRuns, {Prio::EffHeight, Prio::Critical}, {}},
    // Bottom-up strategies
    {0, false, false, 2, {Prio::Critical, Prio::LCDLatest}, {}},
    {1, false, false, 2, {Prio::Critical, Prio::LCDLatest}, {}},
    {1, false, false, 1, {Prio::NodeNum}, {}}, // pure bottom up
};

bool PostPipeliner::tryApproaches() {
  DEBUG_SUMMARY(dbgs() << "-- MinLength=" << MinLength << "\n");
  int HeuristicIndex = 0;
  for (const auto &Config : Heuristics) {
    if (Heuristic >= 0 && Heuristic != HeuristicIndex++) {
      continue;
    }
    const int StrategyLength = MinLength + Config.ExtraStages * II;
    ConfigStrategy S(*DAG, Info, StrategyLength, Config.TopDown,
                     Config.Alternate, Config.Components, Config.Modifiers);
    resetSchedule(/*FullReset=*/true);
    for (int Run = 0; Run < Config.Runs && Run < HeuristicRuns; Run++) {
      DEBUG_SUMMARY(dbgs() << "--- Strategy " << S.name() << " run=" << Run
                           << " trying II=" << II << "\n");
      if (scheduleWithStrategy(S)) {
        DEBUG_SUMMARY(dbgs()
                      << "    Strategy " << S.name() << " run=" << Run
                      << " found NS=" << NStages << " II=" << II << "\n");
        return true;
      }
      if (!S.checkAndResetChanged()) {
        // If nothing changed, there's no use in rerunning.
        break;
      }
      resetSchedule(/*FullReset=*/false);
    }
    DEBUG_SUMMARY(dbgs() << "    Strategy " << S.name() << " failed\n");
  }
  IterCountSlackStrategy Relaxed(*DAG, Info, MinLength + II);
  resetSchedule(/*FullReset=*/true);
  if (scheduleWithStrategy(Relaxed)) {
    return true;
  }

  // TargetII is the OK from the user to spend some time reaching this II.
  // Therefore, if we haven't found a solution yet, bring in the big guns.
  if (II == TargetII) {
    const SolverData Data = createSolverData();
    int NS = MinLength / II;
    if (solve(Data, NS, false)) {
      return true;
    }
    if (NS == MinTripCount) {
      // Only try this at the boundary case
      if (solve(Data, NS + 1, true)) {
        return true;
      }
    }
  }

  DEBUG_SUMMARY(dbgs() << "=== II=" << II << " Failed ===\n");
  return false;
}

bool PostPipeliner::solve(const SolverData &Data, int NS, bool SEFStage) {
  auto Solvers = getSolvers();
  for (auto &Solver : Solvers) {
    if (applySolver(Data, *Solver, NS, SEFStage)) {
      return true;
    }
  }
  return false;
}

SolverData PostPipeliner::createSolverData() {
  SolverData Data;
  // Add the forward dependence edges within the first iteration
  for (int N = 0; N < NInstr; N++) {
    const SUnit &SU = DAG->SUnits[N];
    MachineInstr *const MI = SU.getInstr();
    auto SlotKind = TII->getSlotKind(MI->getOpcode());

    const uint64_t MemoryBanks = HR.getMemoryBanks(MI);
    const int Id =
        Data.addInstruction(SlotKind, MemoryBanks, !isSideEffectFree(MI));
    assert(unsigned(Id) == SU.NodeNum);
    for (auto Dep : SU.Preds) {
      const int From = Dep.getSUnit()->NodeNum;
      assert(From < NInstr);
      Data.addLatency(From, N, Dep.getSignedLatency());
    }
  }

  // Add loop-carried dependences to future iterations. The iteration
  // distance is taken into account
  for (int N = 0; N < NInstr; N++) {
    const SUnit &SU = DAG->SUnits[N];
    for (auto Dep : SU.Succs) {
      const int To = Dep.getSUnit()->NodeNum;
      const bool IsLoopCarried = To >= NInstr;
      const bool IsSelfEdge = To % NInstr == N;
      if (IsLoopCarried && !IsSelfEdge) {
        Data.addLatency(N, To % NInstr, Dep.getSignedLatency(), To / NInstr);
      }
    }
  }
  Data.finalize(II);
  return Data;
}

bool PostPipeliner::applySolver(const SolverData &Data, SWPSolver &Solver,
                                int NS, bool SEFStage) {

  // We don't model the resource hazards. They would be very tedious to express,
  // since resource uses are offset relative to the instruction cycle. We would
  // need to interpret raw itinerary data, and the modulo constraints on those
  // would lead to very awkard expressions.
  Solver.setScheduleSize(II, NS);
  Solver.genModel(Data, SEFStage);
  if (!Solver.solveModel()) {
    return false;
  }

  // We have a solution of our model, but this is missing some constraints, in
  // order to save solver time. We extract the cycles, and make a final check
  // for all constraints using a dedicated strategy.
  auto Schedule = Solver.getSUCycles();
  DEBUG_SUMMARY(dbgs() << "Solver found "; for (auto C
                                                : Schedule) dbgs()
                                           << C << ", ";
                dbgs() << "\n";);
  CheckFixedSchedule S{*DAG, Info, II * NS, Schedule};
  resetSchedule(/*FullReset=*/true);
  DEBUG_SUMMARY(dbgs() << "--- Strategy " << S.name() << "\n");
  if (scheduleWithStrategy(S)) {
    DEBUG_SUMMARY(dbgs() << "    Strategy " << S.name() << " found II=" << II
                         << "\n");
    return true;
  }

  return false;
}

bool PostPipeliner::schedule(ScheduleDAGMI &TheDAG, int InitiationInterval) {

  II = InitiationInterval;
  DAG = &TheDAG;

  // We need to set up a scoreboard that gives us some look-ahead.
  // The look-ahead is used heuristically, to see conflicts with future
  // iterations of nodes scheduled earlier.
  // We will check conflicts in cycle [0, II) and we want to insert the future
  // iterations that can conflict with it.
  const int InsertRange = std::max(II, int(HR.getPipelineDepth()));

  ScoreboardSize = InsertRange + HR.getPipelineDepth();
  Scoreboard.config(0, ScoreboardSize - 1);

  Info.init(NInstr);

  LLVM_DEBUG(for (int I = 0; I < NInstr; I++) {
    dbgs() << I << " " << NoDebug(*DAG->SUnits[I].getInstr()) << "\n";
  });

  computeLoopCarriedParameters();

  LLVM_DEBUG(dumpGraph(Info, DAG, "PostPipeliner_II" + std::to_string(II)));

  if (II < RecMII) {
    return false;
  }
  LLVM_DEBUG(dumpIntervals(Info, MinLength, II));
  if (!tryApproaches()) {
    LLVM_DEBUG(dbgs() << "PostPipeliner: No schedule found\n");
    return false;
  }

  LLVM_DEBUG(dbgs() << "PostPipeliner: Success\n");
  return true;
}

// Pipelining reduces the iteration count by NS - 1
// The result should be > 0, because ZOL doesn't support zero iterations.
bool PostPipeliner::hasSufficientMinTripCount(int NS) const {
  return MinTripCount - (NS - 1) > 0;
}

// This visitor counts the initial bundles without any side-effect,
// typically only on the first stage.
class CountSideEffectFreeBundles : public PipelineScheduleVisitor {
  bool SEF = true;
  int NumSEFBundles = 0;
  void addToBundle(MachineInstr *MI) override { SEF &= isSideEffectFree(MI); }
  void endBundle() override { NumSEFBundles += SEF; }

public:
  int operator()() { return NumSEFBundles; }
};

bool PostPipeliner::peelSideEffectFree() {
  // The plan: If an instruction has no side-effect, it doesn't matter whether
  // we execute it too often. That means we can construct a side-effect-free
  // first stage as a preamble to the true modulo loop/prologue.
  // That stage has one more copy than the others, and will produce unused
  // values in the last iteration.
  // The extracted pipeline for a 2.5 stage pipeline looks like this:
  //
  // Prolog:
  //   S-1
  //   S0 S-1
  // Loop:
  //   S1 S0 S-1
  // Epilog:
  //      S1

  // Try whether peeling one SEF stage would help.
  const int OneStageFewer = NStages - 1;
  if (!hasSufficientMinTripCount(OneStageFewer)) {
    // still no luck
    return false;
  }

  CountSideEffectFreeBundles SEFCounter;
  visitPipelineSection(SEFCounter, 1, [](const NodeInfo &Node, int S, int M) {
    return Node.Stage == 0 && Node.ModuloCycle == M;
  });

  const int NSEF = SEFCounter();
  LLVM_DEBUG(dbgs() << "SEFPeeler: Length=" << Info.Length << " NSEF=" << NSEF
                    << " NStages=" << NStages << "\n");
  // If we exclude the side-effect-free part of the schedule, it may fit in one
  // stage fewer, which we have shown not to exceed the tripcount constraint
  if (Info.Length - NSEF <= OneStageFewer * II) {
    DEBUG_SUMMARY(dbgs() << "Can peel SEF stage. " << Info.Length << " - "
                         << NSEF << " <= " << NStages << " * " << II << "\n");
    // Store the pending rotation. The actual rotation will be applied after
    // scheduleOtherIterations() succeeds, to ensure validation uses consistent
    // pre-rotation cycle values.
    const int Rotation = II - NSEF;
    Info.setRotation(Rotation);
    NStages--;
    return true;
  }

  return false;
}

bool PostPipeliner::checkStages() {
  // We compute the stage in which each representative instruction runs,
  // and take the maximum to decide on the stage count
  int MaxStage = 0;
  for (int K = 0; K < NInstr; K++) {
    auto &Node = Info[K];
    Node.update(II);
    MaxStage = std::max(MaxStage, Node.Stage);
  }
  NStages = MaxStage + 1;
  NPrologueStages = NStages - 1;
  // Check that we have a positive trip count after adjusting
  if (!hasSufficientMinTripCount(NStages) && !peelSideEffectFree()) {
    DEBUG_SUMMARY(dbgs() << "PostPipeliner: MinTripCount insufficient\n");
    return false;
  }
  return true;
}

void PostPipeliner::visitPipelineSection(
    PipelineScheduleVisitor &Visitor, int Repeat,
    std::function<bool(const NodeInfo &Node, int Stage, int M)> Filter) const {

  // This runs Repeat times across the original body instructions and
  // calls the bundle emission callbacks according to Filter.
  // It provide the stage and the modulo cycle in that stage
  // (both starting at zero) to the filter
  for (int Stage = 0; Stage < Repeat; Stage++) {
    for (int M = 0; M < II; M++) {
      Visitor.startBundle();
      for (int K = 0; K < NInstr; K++) {
        auto &Node = Info[K];
        if (Filter(Node, Stage, M)) {
          Visitor.addToBundle(DAG->SUnits[K].getInstr());
        }
      }
      Visitor.endBundle();
    }
  }
}

void PostPipeliner::visitPipelineSchedule(
    PipelineScheduleVisitor &Visitor) const {

  DEBUG_SUMMARY(dbgs() << "Extracting NP=" << NPrologueStages
                       << " NS=" << NStages << "\n");
  Visitor.startPrologue();
  visitPipelineSection(
      Visitor, NPrologueStages, [&](const NodeInfo &Node, int Stage, int M) {
        return Node.ModuloCycle == M && Node.Cycle < (Stage + 1) * II;
      });

  Visitor.startLoop();
  visitPipelineSection(Visitor, 1, [&](const NodeInfo &Node, int Stage, int M) {
    return Node.ModuloCycle == M;
  });

  Visitor.startEpilogue();
  // The epilogue normally starts at stage 1. However, if we have a SEF stage,
  // it occupies stage 0, the actual pipeline has shifted to start at
  // stage 1, and the epilogue starts at stage 2. We recognize the case by
  // comparing the main NStages with the secondary NPrologueStages.
  const int EpiBase = NPrologueStages == NStages ? 2 : 1;
  visitPipelineSection(
      Visitor, NStages - 1, [&](const NodeInfo &Node, int Stage, int M) {
        return Node.ModuloCycle == M && Node.Cycle >= (EpiBase + Stage) * II;
      });

  Visitor.finish();
}

void PostPipeliner::dump() const {
  dbgs() << "Modulo Schedule II=" << II << " NStages=" << NStages << "\n";
  for (int I = 0; I < NInstr; I++) {
    const NodeInfo &Node = Info[I];
    dbgs() << I << " @" << Node.Cycle << " %" << Node.ModuloCycle << " S"
           << Node.Stage << " : ";
    LLVM_DEBUG(DAG->SUnits[I].getInstr()->dump());
  }
  PostPipelineDumper Dump;
  visitPipelineSchedule(Dump);
}

void PostPipeliner::updateTripCount() const {
  int Delta = NStages - 1;
  TII->adjustTripCount(*TripCountDef, -Delta);
}

int PostPipeliner::getFinalMinTripCount() const {
  const int Delta = NStages - 1;
  return MinTripCount - Delta;
}

void PostPipeliner::materializePipeline(PipelineScheduleVisitor &Visitor) {
  // A schedule NS=N, II=L is compatible with NS=1, II=N*L.
  // In general, we can take any divisor of N.
  // We provide an actual implementation of such less dense
  // schedules, since it can provide debugging insights.
  // We avoid the complication of a SEF stage, recognised by the
  // NPrologueStages vs NStages relation.
  if (ForcedStageCount && NStages % ForcedStageCount == 0 &&
      NPrologueStages == NStages - 1) {
    // Fix the II, recompute ModuloCycle and Stage, fix stagecount and
    // prologue stages count
    const int Factor = NStages / ForcedStageCount;
    II *= Factor;
    for (int K = 0; K < NInstr; K++) {
      auto &Node = Info[K];
      Node.update(II);
    }
    NStages = ForcedStageCount;
    NPrologueStages = NStages - 1;
  }

  visitPipelineSchedule(Visitor);
  updateTripCount();
}

void NodeInfo::reset(bool FullReset) {
  Cycle = 0;
  Scheduled = false;
  Earliest = 0;
  Latest = -1;
  if (FullReset) {
    TweakedEarliest = {};
    TweakedLatest = {};
    NumPushedEarliest = 0;
    NumPushedLatest = 0;
    LastEarliestPusher = {};
    LastLatestPusher = {};
  }
}

void NodeInfo::update(int II) {
  ModuloCycle = Cycle % II;
  Stage = Cycle / II;
}

void ScheduleInfo::applyRotation(int II) {
  if (PendingRotation == 0)
    return;

  for (int N = 0; N < NInstr; N++) {
    Nodes[N].Cycle = Nodes[N].Cycle + PendingRotation;
    Nodes[N].update(II);

    commitCycle(N);
  }
}

} // namespace llvm::AIE
