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
#include "Utils/AIELoopUtils.h"
#include "llvm/CodeGen/ScheduleDAGInstrs.h"
#include "llvm/Support/MathExtras.h"

#define DEBUG_TYPE "postpipeliner"

namespace llvm::AIE {

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

int PostPipeliner::getResMII(MachineBasicBlock &LoopBlock) {
  // For each instruction, find the first cycle in which it fits and collect the
  // maximum
  std::vector<uint64_t> Scoreboard(NInstr, 0);
  int MII = 1;
  for (auto &MI : LoopBlock) {
    auto *SlotInfo = TII->getSlotInfo(TII->getSlotKind(MI.getOpcode()));
    SlotBits Slots = SlotInfo ? SlotInfo->getSlotSet() : 0;

    int C = 0;
    while (C < NInstr && (Scoreboard[C] & Slots)) {
      C++;
    }
    if (C >= NInstr) {
      MII = NInstr;
      break;
    }
    Scoreboard[C] |= Slots;
    MII = std::max(MII, C + 1);
  }
  LLVM_DEBUG(dbgs() << "PostPipeliner: ResMII=" << MII << "\n");
  return MII;
}

// This assigns Cycle of SU, Earliest of its predecessors and Earliest of
// the next instance of SU.
void PostPipeliner::scheduleNode(SUnit &SU, int Cycle) {
  LLVM_DEBUG(dbgs() << "PostPipeline " << SU.NodeNum << " in cycle " << Cycle
                    << "\n");
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
      Info[SNum].Earliest = NewEarliest;
      LLVM_DEBUG(dbgs() << "   Earliest(" << SNum << ") to "
                        << Info[SNum].Earliest << "\n");
    }
  }

  int Next = SU.NodeNum + NInstr;
  if (Next < NTotalInstrs) {
    Info[Next].Earliest = std::max(Info[Next].Earliest, Cycle + II);
  }
}

// Check resources. We only insert at the position modulo II. Since we insert
// all iterations separately, the resources that wrap around accumulate in the
// overflow area, causing conflicts when inserting future iterations
int PostPipeliner::fit(MachineInstr *MI, int Earliest, int NTries, int II) {
  for (int C = Earliest; C < Earliest + NTries; C++) {
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

void PostPipeliner::computeLoopCarriedParameters() {
  // We schedule the first iteration, only using earliest. This updates
  // earliest of the successors. Any successor in the second iteration
  // represents a loop carried dependence, and we account for that by
  // propagating its Earliest back to the first iteration
  // Note that we don't have to clean the effects of this exploration,
  // since the real scheduling will overwrite Cycle, and the ultimate Earliest
  // will never be less than we compute here.

  for (int K = 0; K < NInstr; K++) {
    SUnit &SU = DAG->SUnits[K];
    const int Earliest = Info[K].Earliest;
    scheduleNode(SU, Earliest);
  }

  // Propagate Earliest upstream, initialize Latest
  for (int K = 0; K < NInstr; K++) {
    const int K2 = K + NInstr;
    const int Earliest = Info[K2].Earliest - II;
    Info[K].Earliest = std::max(Info[K].Earliest, Earliest);
    // Unrestricted: Beyond the last stage.
    Info[K].Latest = NCopies * II;
  }
  // Propagate Latest upstream. Latest is the latest
  // that is admissible for Earliest to be achievable within II
  for (int K = 0; K < NInstr; K++) {
    const int K2 = K + NInstr;
    const int Earliest = Info[K2].Earliest;
    const auto &SU = DAG->SUnits[K2];
    for (auto &Dep : SU.Preds) {
      const auto *Pred = Dep.getSUnit();
      // Any predecessor in the first iteration
      int K1 = Pred->NodeNum;
      if (K1 < NInstr) {
        const int Latest = Earliest - Dep.getSignedLatency();
        Info[K1].Latest = std::min(Info[K1].Latest, Latest);
      }
    }
  }
  LLVM_DEBUG(for (int K = 0; K < NInstr; K++) {
    dbgs() << "SU" << K << " : " << Info[K].Earliest << " - " << Info[K].Latest
           << "\n";
  });
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
      dbgs() << "# L=" << Dep.getSignedLatency() << "\n";
    }
  }
  dbgs() << "}\n";
}

int PostPipeliner::mostUrgent() {
  assert(FirstUnscheduled < NInstr);
  while (Info[FirstUnscheduled].Scheduled) {
    FirstUnscheduled++;
  }
  assert(FirstUnscheduled < NInstr);

  int Best = -1;
  LLVM_DEBUG(dbgs() << "Available:");
  for (int K = FirstUnscheduled; K < NInstr; K++) {
    const auto &SU = DAG->SUnits[K];
    // Check whether it is available
    if (any_of(SU.Preds, [&](const auto &Dep) {
          return !Info[Dep.getSUnit()->NodeNum].Scheduled;
        })) {
      continue;
    }
    LLVM_DEBUG(dbgs() << " SU" << K);
    // Yeah, I know. This is a difficult way to schedule in the original
    // node order. Have patience, my friend.
    if (Best == -1) {
      Best = K;
      LLVM_DEBUG(dbgs() << "*");
    }
  }
  LLVM_DEBUG(dbgs() << "\n");
  assert(Best >= 0);
  return Best;
}

bool PostPipeliner::scheduleFirstIteration() {
  // Set up the basic schedule from the original instructions
  for (int K = 0; K < NInstr; K++) {
    const int N = mostUrgent();
    LLVM_DEBUG(dbgs() << "  Trying " << N << "\n");
    SUnit &SU = DAG->SUnits[N];
    MachineInstr *MI = SU.getInstr();
    const int Earliest = Info[N].Earliest;
    // Find the first cycle that fits. We try every position modulo II
    const int Actual = fit(MI, Earliest, II, II);
    if (Actual < 0) {
      // out of resources for this II;
      LLVM_DEBUG(dbgs() << "Out of resources\n");
      return false;
    }
    const int LocalCycle = Actual % II;
    const MemoryBankBits MemoryBanks = HR.getMemoryBanks(MI);
    LLVM_DEBUG(dbgs() << "  Emit in " << -Depth + LocalCycle << "\n");
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
    LLVM_DEBUG(dbgs() << "Scoreboard\n"; Scoreboard.dumpFull(););
  }
  LLVM_DEBUG(dbgs() << "==== First iteration scheduled ======\n");
  return true;
}

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
        LLVM_DEBUG(dbgs() << "  Latency not met (Earliest=" << Earliest
                          << ")\n");
        return false;
      }

      scheduleNode(SU, Insert);
    }
  }
  return true;
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
  FirstUnscheduled = 0;

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
  if (!scheduleFirstIteration() || !scheduleOtherIterations()) {
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

} // namespace llvm::AIE
