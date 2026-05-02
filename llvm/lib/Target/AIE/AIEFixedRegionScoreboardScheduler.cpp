//===- AIEFixedRegionScoreboardScheduler.cpp - Shared sched engine -===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEFixedRegionScoreboardScheduler.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "aie-fixed-region-sched"

using namespace llvm;
using namespace llvm::AIE;

FixedRegionScoreboardScheduler::FixedRegionScoreboardScheduler(
    const AIEHazardRecognizer &HR, const Config &Cfg)
    : HR(HR), Cfg(Cfg) {
  assert(Cfg.LowestCycle <= Cfg.HighestCycle &&
         "Empty scoreboard configuration");
  assert(Cfg.II >= 0 && "II must be non-negative (0 disables modulo)");
  Scoreboard.config(Cfg.LowestCycle, Cfg.HighestCycle);
}

void FixedRegionScoreboardScheduler::emitFixedBundleAt(
    const MachineBundle &Bundle, int Cycle) {
  // A fixed bundle's resource demand goes to its literal cycle only — no
  // modulo broadcast. Iterate the bundle's instructions and emit each.
  // Meta instructions carry no resource demand and are skipped by the
  // hazard recognizer's emitInScoreboard.
  for (const MachineInstr *MI : Bundle.getInstrs()) {
    if (!MI)
      continue;
    HR.emitInScoreboard(Scoreboard, *MI, MI->getDesc(), Cycle);
  }
}

void FixedRegionScoreboardScheduler::primeAllRegions() {
  // Top-fixed bundles anchored at cycle LowestCycle + i.
  for (size_t I = 0; I < Cfg.TopFixedBundles.size(); ++I) {
    const int Cycle = Cfg.LowestCycle + static_cast<int>(I);
    emitFixedBundleAt(Cfg.TopFixedBundles[I], Cycle);
  }

  // Bot-fixed bundles anchored at the high end. The first bundle in the
  // array sits earliest, so cycle = HighestCycle - (size - 1 - i).
  const int K_bot = static_cast<int>(Cfg.BotFixedBundles.size());
  for (int I = 0; I < K_bot; ++I) {
    const int Cycle = Cfg.HighestCycle - (K_bot - 1 - I);
    emitFixedBundleAt(Cfg.BotFixedBundles[I], Cycle);
  }

  // Predecessor / successor projection scoreboards — caller-owned.
  // Phase 1 leaves both null. When set, union them in. Trust modes are
  // handled by the caller for now (Conservative -> caller passes
  // nullptr); AccountForAlign widening will be added in Phase 2.
  if (Cfg.PredScoreboard)
    unionInto(Scoreboard, *Cfg.PredScoreboard);
  if (Cfg.SuccScoreboard)
    unionInto(Scoreboard, *Cfg.SuccScoreboard);
}

std::optional<int>
FixedRegionScoreboardScheduler::fitInInterval(const SUnit &SU, int Earliest,
                                              int Latest, bool BottomUp) {
  assert(Earliest <= Latest && "fitInInterval given an inverted interval");
  MachineInstr &MI = *SU.getInstr();

  const int Step = BottomUp ? -1 : 1;
  int First = Earliest;
  int Last = Latest;
  if (BottomUp)
    std::swap(First, Last);

  const int Limit = Last + Step;
  for (int C = First; C != Limit; C += Step) {
    if (!HR.checkConflict(Scoreboard, MI, conflictSlot(C)))
      return C;
  }
  return std::nullopt;
}

bool FixedRegionScoreboardScheduler::emit(const SUnit &SU, int Cycle) {
  MachineInstr *MI = SU.getInstr();
  const MCInstrDesc &Desc = MI->getDesc();
  const MemoryBankBits MemoryBanks = HR.getMemoryBanks(MI);
  const MemoryObjectsBits ObjectBits = HR.getMemoryObjectsBits(MI);
  const auto &MRI = MI->getMF()->getRegInfo();

  if (Cfg.II == 0) {
    HR.emitInScoreboard(Scoreboard, Desc, MemoryBanks, ObjectBits,
                        MI->operands(), MRI, Cycle);
    return true;
  }

  // Modulo mode: broadcast across future iteration windows up to a
  // horizon that mirrors the existing post-pipeliner logic at
  // AIEPostPipeliner.cpp:884-898 — "newly scheduled instructions have
  // ModCycle < II and have no conflict beyond ModCycle + PipelineDepth".
  const int PipelineDepth = HR.getPipelineDepth();
  const int ScoreboardSize = Cfg.HighestCycle - Cfg.LowestCycle + 1;
  const int Horizon =
      std::min(Cfg.II + PipelineDepth, ScoreboardSize - PipelineDepth);

  int Emit = Cycle % Cfg.II;
  while (Emit < Horizon) {
    if (HR.checkConflict(Scoreboard, *MI, Emit)) {
      LLVM_DEBUG(dbgs() << "FixedRegionScoreboardScheduler: future-iteration "
                           "conflict at cycle "
                        << Emit << "\n");
      return false;
    }
    HR.emitInScoreboard(Scoreboard, Desc, MemoryBanks, ObjectBits,
                        MI->operands(), MRI, Emit);
    Emit += Cfg.II;
  }
  return true;
}

void FixedRegionScoreboardScheduler::dump() const {
  dbgs() << "FixedRegionScoreboardScheduler: II=" << Cfg.II << " range=["
         << Cfg.LowestCycle << "," << Cfg.HighestCycle << "]"
         << " TopFixed=" << Cfg.TopFixedBundles.size()
         << " BotFixed=" << Cfg.BotFixedBundles.size()
         << " Pred=" << (Cfg.PredScoreboard ? "set" : "null")
         << " Succ=" << (Cfg.SuccScoreboard ? "set" : "null") << "\n";
  Scoreboard.dump();
}
