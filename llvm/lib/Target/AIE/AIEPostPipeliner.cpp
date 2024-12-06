//===- AIEPostPipeliner.cpp - Post RA Pipeliner                            ===//
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

#include "AIEPostPipeliner.h"
#include "AIESlotCounts.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/CodeGen/ScheduleDAGInstrs.h"
#include "llvm/Support/MathExtras.h"

#define DEBUG_TYPE "postpipeliner"
#define DEBUG_SUMMARY(X) DEBUG_WITH_TYPE("postpipeliner-summary", X)
#define DEBUG_FULL(X) DEBUG_WITH_TYPE("postpipeliner-full", X)

namespace llvm::AIE {

static cl::opt<int>
    Heuristic("aie-postpipeliner-heuristic",
              cl::desc("Select one specific post-pipeliner heuristic"),
              cl::init(-1), cl::Hidden);

PipelineScheduleVisitor::~PipelineScheduleVisitor() {}

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

bool PostPipeliner::canAccept(MachineBasicBlock &LoopBlock) {
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

  // 3. We need to know how to update the tripcount. We check whether
  // the tripcount is pristine, otherwise the loop may have been software
  // pipelined before and we can't trust min itercount metadata.
  // Return on investment is probably low anyway.
  const bool Pristine = true;
  for (auto &MI : reverse(*Preheader)) {
    if (TII->isZOLTripCountDef(MI, Pristine)) {
      TripCountDef = &MI;
      break;
    }
  }
  if (!TripCountDef) {
    LLVM_DEBUG(dbgs() << " PostPipeliner: No tripcount def\n");
    return false;
  }

  // 4. We need to peel stages and be left with a positive tripcount.
  // This is just a minimum check to save useless work; the real stage
  // count is checked before accepting the schedule.
  auto ParsedMinTripCount = AIELoopUtils::getMinTripCount(LoopBlock);
  if (!ParsedMinTripCount) {
    LLVM_DEBUG(dbgs() << " PostPipeliner: No min tripcount\n");
    return false;
  }
  MinTripCount = *ParsedMinTripCount;
  if (MinTripCount < 2) {
    LLVM_DEBUG(dbgs() << " PostPipeliner: min tripcount < 2\n");
    return false;
  }

  return true;
}

static SlotCounts getSlotCounts(MachineInstr &MI, const AIEBaseInstrInfo *TII) {
  auto *SlotInfo = TII->getSlotInfo(TII->getSlotKind(MI.getOpcode()));
  return SlotInfo ? SlotInfo->getSlotSet() : 0;
}

int PostPipeliner::getResMII(MachineBasicBlock &LoopBlock) {
  // Add up all slot requirements and return the maximum slot count
  SlotCounts Counts;
  for (auto &MI : LoopBlock) {
    Counts += getSlotCounts(MI, TII);
  }
  int MII = Counts.max();
  LLVM_DEBUG(dbgs() << "PostPipeliner: ResMII=" << MII << "\n");
  return MII;
}

// This assigns Cycle of SU, Earliest of its successors and Latest of its
// predecessors
void PostPipeliner::scheduleNode(SUnit &SU, int Cycle) {
  LLVM_DEBUG(dbgs() << "PostPipeline " << SU.NodeNum << " in cycle " << Cycle
                    << ". ");
  Info[SU.NodeNum].Cycle = Cycle;
  for (auto &Dep : SU.Succs) {
    int Latency = Dep.getSignedLatency();
    auto *Succ = Dep.getSUnit();
    if (Succ->isBoundaryNode()) {
      continue;
    }
    const int SNum = Succ->NodeNum;
    const int NewEarliest = Cycle + Latency;
    if (NewEarliest > Info[SNum].Earliest) {
      Info[SNum].LastEarliestPusher = SU.NodeNum;
      Info[SNum].Earliest = NewEarliest;
      Info[SU.NodeNum].NumPushedEarliest++;
      LLVM_DEBUG(dbgs() << SNum << " to " << Info[SNum].Earliest << " -; ");
    }
  }
  for (auto &Dep : SU.Preds) {
    int Latency = Dep.getSignedLatency();
    auto *Pred = Dep.getSUnit();
    if (Pred->isBoundaryNode()) {
      continue;
    }
    const int PNum = Pred->NodeNum;
    const int NewLatest = Cycle - Latency;
    if (NewLatest < Info[PNum].Latest) {
      Info[PNum].LastLatestPusher = SU.NodeNum;
      Info[PNum].Latest = NewLatest;
      Info[SU.NodeNum].NumPushedLatest++;
      LLVM_DEBUG(dbgs() << PNum << " to - " << Info[PNum].Latest << "; ");
    }
  }
  LLVM_DEBUG(dbgs() << "\n");

  int Next = SU.NodeNum + NInstr;
  if (Next < NTotalInstrs) {
    Info[Next].Earliest = std::max(Info[Next].Earliest, Cycle + II);
  }
}

// Check resources. We only insert at the position modulo II. Since we insert
// all iterations separately, the resources that wrap around accumulate in the
// overflow area, causing conflicts when inserting future iterations
int PostPipeliner::fit(MachineInstr *MI, int First, int Last, int II) {
  const int Step = First > Last ? -1 : 1;
  LLVM_DEBUG(dbgs() << "   " << First << ", " << Last << ", " << Step << "\n");
  for (int C = First; C != Last; C += Step) {
    int Mod = C % II;
    LLVM_DEBUG(dbgs() << "   at " << C << " (" << Mod << ")\n");
    if (!HR.checkConflict(Scoreboard, *MI, -Depth + Mod)) {
      LLVM_DEBUG(dbgs() << "    Success\n");
      return C;
    }
  }
  LLVM_DEBUG(dbgs() << "    Fail\n");

  return -1;
}

void PostPipeliner::computeForward() {
  // The forward order defines a topological sort, so we can compute
  // Earliest and Ancestors in a single forward sweep
  for (int K = 0; K < NInstr; K++) {
    auto &Me = Info[K];
    SUnit &SU = DAG->SUnits[K];
    for (auto &Dep : SU.Preds) {
      if (Dep.getKind() != SDep::Data) {
        continue;
      }
      int P = Dep.getSUnit()->NodeNum;
      assert(P < K);
      Me.Ancestors.insert(P);
      auto &Pred = Info[P];
      for (int Anc : Pred.Ancestors) {
        Me.Ancestors.insert(Anc);
      }
    }
    for (auto &Dep : SU.Succs) {
      auto *Succ = Dep.getSUnit();
      if (Succ->isBoundaryNode()) {
        continue;
      }
      auto &SInfo = Info[Succ->NodeNum];
      const int NewEarliest = Me.Earliest + Dep.getSignedLatency();
      SInfo.Earliest = std::max(SInfo.Earliest, NewEarliest);
    }
    Me.Slots = getSlotCounts(*SU.getInstr(), TII);
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

bool PostPipeliner::computeLoopCarriedParameters() {

  // Forward properties like Earliest and Ancestors.
  computeForward();

  // Backward properties like Latest and Offspring.
  // Use a fixpoint loop, because plain reversed order may not be topological
  // for predecessors
  while (computeBackward()) {
    /* EMPTY */;
  }

  // Adjust Earliest and Latest with resource requirements.
  // FIXME: We do not account for negative latencies here. This can lead to
  // suboptimality, but we only include true dependences, where negative
  // latencies are rare.
  for (int K = 0; K < NInstr; K++) {
    auto &Me = Info[K];
    SlotCounts ASlots(Me.Slots);
    for (int A : Me.Ancestors) {
      ASlots += Info[A].Slots;
    }
    SlotCounts OSlots(Me.Slots);
    for (int O : Me.Offspring) {
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

  // Save the static values for ease of reset
  for (auto &N : Info) {
    N.StaticEarliest = N.Earliest;
    N.StaticLatest = N.Latest;
  }
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

void dumpGraph(int NInstr, const std::vector<NodeInfo> &Info,
               ScheduleDAGInstrs *DAG) {
  dbgs() << "digraph {\n";

  for (int K = 0; K < NInstr; K++) {
    auto &SU = DAG->SUnits[K];
    for (auto &Dep : SU.Succs) {
      auto *Succ = Dep.getSUnit();
      int S = Succ->NodeNum;
      if (S % NInstr == K) {
        continue;
      }

      dbgs() << "\tSU" << K << " -> "
             << "SU" << S;

      if (S >= NInstr) {
        dbgs() << "_" << S % NInstr;
      }
      if (Dep.getKind() == SDep::Data) {
        dbgs() << " [color=red] ";
      } else if (Dep.getKind() == SDep::Output) {
        dbgs() << " [color=black] ";
      } else if (Dep.getKind() == SDep::Anti) {
        dbgs() << " [color=blue] ";
      }

      dbgs() << " # L=" << Dep.getSignedLatency();
      if (Dep.getKind() == SDep::Output) {
        dbgs() << " WAW";
      }
      dbgs() << "\n";
    }
  }
  dbgs() << "}\n";
}

int PostPipeliner::mostUrgent(PostPipelinerStrategy &Strategy) {
  assert(FirstUnscheduled <= LastUnscheduled);
  while (Info[FirstUnscheduled].Scheduled) {
    FirstUnscheduled++;
  }
  while (Info[LastUnscheduled].Scheduled) {
    LastUnscheduled--;
  }
  assert(FirstUnscheduled <= LastUnscheduled);

  auto NotScheduled = [&](const auto &Dep) {
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
  for (int K = 0; K < NTotalInstrs; K++) {
    auto &N = Info[K];
    N.reset(FullReset);
    if (K < NInstr) {
      N.Earliest = N.StaticEarliest;
      N.Latest = N.StaticLatest;
    }
  }

  FirstUnscheduled = 0;
  LastUnscheduled = NInstr - 1;
}

bool PostPipeliner::scheduleFirstIteration(PostPipelinerStrategy &Strategy) {
  // Set up the basic schedule from the original instructions
  for (int K = 0; K < NInstr; K++) {
    const int N = mostUrgent(Strategy);
    LLVM_DEBUG(dbgs() << "  Trying " << N << "\n");
    SUnit &SU = DAG->SUnits[N];
    MachineInstr *MI = SU.getInstr();
    const int Earliest = Strategy.earliest(SU);
    const int Latest = Strategy.latest(SU);
    // Find the first cycle that fits. We try every position modulo II
    const int Actual = Strategy.fromTop() ? fit(MI, Earliest, Latest + 1, II)
                                          : fit(MI, Latest, Earliest - 1, II);
    if (Actual < 0) {
      // out of resources for this II;
      LLVM_DEBUG(dbgs() << "Out of resources\n");
      return false;
    }
    Strategy.selected(SU);
    const int LocalCycle = Actual % II;
    const MemoryBankBits MemoryBanks = HR.getMemoryBanks(MI);
    int Cycle = -Depth + LocalCycle;
    LLVM_DEBUG(dbgs() << "  Emit in " << Cycle << "\n");
    for (int N = 0; N < NCopies; N++) {
      if (N > 0 && HR.checkConflict(Scoreboard, *MI, Cycle)) {
        return false;
      }

      HR.emitInScoreboard(Scoreboard, MI->getDesc(), MemoryBanks,
                          MI->operands(), MI->getMF()->getRegInfo(), Cycle);
      Cycle += II;
    }

    scheduleNode(SU, Actual);
    Info[N].Scheduled = true;
    DEBUG_FULL(dbgs() << "Scoreboard\n"; Scoreboard.dumpFull(););
  }
  LLVM_DEBUG(dbgs() << "==== First iteration scheduled by " << Strategy.name()
                    << "====\n");
  return true;
}

namespace {
void dumpEarliestChain(const std::vector<NodeInfo> &Info, int N) {
  auto Prev = Info[N].LastEarliestPusher;
  if (Prev) {
    dumpEarliestChain(Info, *Prev);
  }
  dbgs() << "  --> " << N << " @" << Info[N].Cycle << "\n";
}
} // namespace

bool PostPipeliner::scheduleOtherIterations() {
  // Make sure that all the copies can be placed at II from the previous one.
  // This looks like overkill, but it accommodates dependences that span
  // multiple loop edges. Without these, the pattern should repeat after the
  // first set of copies.
  for (int L = NInstr; L < NTotalInstrs; L += NInstr) {
    for (int K = 0; K < NInstr; K++) {
      const int N = L + K;
      SUnit &SU = DAG->SUnits[N];
      // Earliest tracks the latencies of the loop carried deps
      const int Earliest = Info[N].Earliest;
      // Insert supplies the modulo condition.
      const int Insert = Info[N - NInstr].Cycle + II;

      // All iterations following the first one should fit exactly
      if (Earliest > Insert) {
        LLVM_DEBUG(dbgs() << "  Latency not met for " << N
                          << "(Earliest=" << Earliest << ")\n";
                   dumpEarliestChain(Info, N););
        return false;
      }

      scheduleNode(SU, Insert);
    }
  }
  return true;
}

class DefaultStrategy : public PostPipelinerStrategy {
public:
  DefaultStrategy(ScheduleDAGMI &DAG, std::vector<NodeInfo> &Info,
                  int LatestBias)
      : PostPipelinerStrategy(DAG, Info, LatestBias) {}
  bool better(const SUnit &A, const SUnit &B) override {
    return Info[A.NodeNum].Latest < Info[B.NodeNum].Latest;
  }
};

class ConfigStrategy : public PostPipelinerStrategy {
public:
  enum PriorityComponent {
    NodeNum,
    Latest,
    Critical,
    Sibling,
    LCDLatest,
    Size
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
    default:
      break;
    }
    return "Size - Illegal";
  }

private:
  std::string Name;
  std::set<int> SuccSiblingScheduled;
  std::function<bool(const SUnit &A, const SUnit &B)>
      Discriminators[PriorityComponent::Size] = {
          [&](const SUnit &A, const SUnit &B) { return A.NodeNum < B.NodeNum; },
          [&](const SUnit &A, const SUnit &B) {
            auto &IA = Info[A.NodeNum];
            auto &IB = Info[B.NodeNum];
            return IA.Latest < IB.Latest;
          },
          [&](const SUnit &A, const SUnit &B) {
            auto &IA = Info[A.NodeNum];
            auto &IB = Info[B.NodeNum];
            return IA.NumPushedEarliest > IB.NumPushedEarliest;
          },
          [&](const SUnit &A, const SUnit &B) {
            return SuccSiblingScheduled.count(A.NodeNum) >
                   SuccSiblingScheduled.count(B.NodeNum);
          },
          [&](const SUnit &A, const SUnit &B) {
            auto &IA = Info[A.NodeNum];
            auto &IB = Info[B.NodeNum];
            return IA.LCDLatest < IB.LCDLatest;
          },
      };
  std::vector<PriorityComponent> Priority;

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
  }

public:
  std::string name() override { return Name; }
  ConfigStrategy(ScheduleDAGInstrs &DAG, std::vector<NodeInfo> &Info,
                 int Length, ArrayRef<PriorityComponent> Components)
      : PostPipelinerStrategy(DAG, Info, Length) {
    Name = "Config_" + std::to_string(Length);
    for (auto Comp : Components) {
      Name += "_" + getPriorityName(Comp);
      Priority.emplace_back(Comp);
    }
  }
};

static const struct {
  int ExtraStages;
  bool Rerun;
  ConfigStrategy::PriorityComponent Components[3];
} Strategies[] = {
    // Loosely speaking, a lower value of the first parameter targets
    // a lower stage count, which benefits code size.
    {1, false, {ConfigStrategy::NodeNum}},
    {1, false, {ConfigStrategy::Latest}},
    {1, true, {ConfigStrategy::Critical}},
    {1, true, {ConfigStrategy::Critical, ConfigStrategy::LCDLatest}},
};

bool PostPipeliner::tryHeuristics() {
  int MinLength = computeMinScheduleLength();

  DEBUG_SUMMARY(dbgs() << "-- MinLength=" << MinLength << "\n");

  int HeuristicIndex = 0;
  for (auto &[ExtraStages, Rerun, Components] : Strategies) {
    if (Heuristic >= 0 && Heuristic != HeuristicIndex++) {
      continue;
    }
    ConfigStrategy S(*DAG, Info, MinLength + ExtraStages * II, Components);
    resetSchedule(/*FullReset=*/true);
    DEBUG_SUMMARY(dbgs() << "--- Strategy " << S.name());
    if (scheduleFirstIteration(S) && scheduleOtherIterations()) {
      DEBUG_SUMMARY(dbgs() << " found II=" << II << "\n");
      return true;
    }

    DEBUG_SUMMARY(dbgs() << " failed\n");
    if (!Rerun) {
      continue;
    }

    // Rerun with dynamic information retained
    resetSchedule(/*FullReset=*/false);
    DEBUG_SUMMARY(dbgs() << "--- Strategy " << S.name()
                         << " with critical path");
    if (scheduleFirstIteration(S) && scheduleOtherIterations()) {
      DEBUG_SUMMARY(dbgs() << " found II=" << II << "\n");
      return true;
    }
    DEBUG_SUMMARY(dbgs() << " failed\n");
  }
  DEBUG_SUMMARY(dbgs() << "=== II=" << II << " Failed ===\n");
  return false;
}

bool PostPipeliner::schedule(ScheduleDAGMI &TheDAG, int InitiationInterval) {
  NTotalInstrs = TheDAG.SUnits.size();
  assert(NTotalInstrs % NInstr == 0);
  NCopies = NTotalInstrs / NInstr;
  if (NCopies == 1) {
    LLVM_DEBUG(dbgs() << "PostPipeliner: Not feasible\n");
    return false;
  }
  II = InitiationInterval;
  DAG = &TheDAG;

  // Let's not skimp on size here. This allows us to insert any instruction
  // in the unrolled dag.
  Depth = NCopies * II + HR.getPipelineDepth();
  Scoreboard.reset(Depth);

  Info.clear();
  Info.resize(NTotalInstrs);

  LLVM_DEBUG(for (int I = 0; I < NInstr;
                  I++) { dbgs() << I << " " << *DAG->SUnits[I].getInstr(); });
  LLVM_DEBUG(dumpGraph(NInstr, Info, DAG));

  computeLoopCarriedParameters();

  if (!tryHeuristics()) {
    LLVM_DEBUG(dbgs() << "PostPipeliner: No schedule found\n");
    return false;
  }

  computeStages();
  LLVM_DEBUG(dbgs() << "PostPipeliner: Schedule found, NS=" << NStages
                    << " II=" << II << "\n");

  // Check that we don't exceed the number of copies in the DAG. In that case
  // we didn't reach steady state, and we may have missed conflicts.
  // We expect this to be rare.
  if (NStages > NCopies) {
    LLVM_DEBUG(dbgs() << "PostPipeliner: Unsafe stage count, NCopies="
                      << NCopies << "\n");
    return false;
  }

  // Check that we have a positive trip count after adjusting
  if (MinTripCount - (NStages - 1) <= 0) {
    LLVM_DEBUG(dbgs() << "PostPipeliner: MinTripCount insufficient\n");
    return false;
  }

  LLVM_DEBUG(dbgs() << "PostPipeliner: Success\n");
  return true;
}

// We only mark up the representative instructions
void PostPipeliner::computeStages() {
  NStages = 0;
  for (int K = 0; K < NInstr; K++) {
    auto &Node = Info[K];
    Node.Stage = Node.Cycle / II;
    Node.ModuloCycle = Node.Cycle % II;
    NStages = std::max(NStages, Node.Stage + 1);
  }
}

void PostPipeliner::visitPipelineSchedule(
    PipelineScheduleVisitor &Visitor) const {

  // This runs StageCount times across the original body instructions and
  // calls the bundle emission callbacks according to Filter.
  // It provide the stage and the modulo cycle in that stage
  // (both starting at zero) to the filter
  auto ExtractSection =
      [&](int StageCount,
          std::function<bool(const NodeInfo &Node, int Stage, int M)> Filter) {
        for (int Stage = 0; Stage < StageCount; Stage++) {
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
      };

  Visitor.startPrologue();
  ExtractSection(NStages - 1, [&](const NodeInfo &Node, int Stage, int M) {
    return Node.ModuloCycle == M && Node.Cycle < (Stage + 1) * II;
  });

  Visitor.startLoop();
  ExtractSection(1, [&](const NodeInfo &Node, int Stage, int M) {
    return Node.ModuloCycle == M;
  });

  Visitor.startEpilogue();
  ExtractSection(NStages - 1, [&](const NodeInfo &Node, int Stage, int M) {
    return Node.ModuloCycle == M && Node.Cycle >= (Stage + 1) * II;
  });

  Visitor.finish();
}

void PostPipeliner::dump() const {
  dbgs() << "Modulo Schedule II=" << II << " NStages=" << NStages << "\n";
  for (int I = 0; I < NInstr; I++) {
    const NodeInfo &Node = Info[I];
    dbgs() << I << " @" << Node.Cycle << " %" << Node.ModuloCycle << " S"
           << Node.Stage << " : ";
    DAG->SUnits[I].getInstr()->dump();
  }
  PostPipelineDumper Dump;
  visitPipelineSchedule(Dump);
}

void PostPipeliner::updateTripCount() const {
  int Delta = NStages - 1;
  TII->adjustTripCount(*TripCountDef, -Delta);
}

void NodeInfo::reset(bool FullReset) {
  Cycle = 0;
  Scheduled = false;
  Earliest = 0;
  Latest = -1;
  if (FullReset) {
    NumPushedEarliest = 0;
    NumPushedLatest = 0;
    LastEarliestPusher = {};
    LastLatestPusher = {};
  }
}

} // namespace llvm::AIE
