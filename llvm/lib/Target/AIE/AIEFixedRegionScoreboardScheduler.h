//===- AIEFixedRegionScoreboardScheduler.h - Shared sched engine -*- C++ -*-=//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// FixedRegionScoreboardScheduler is the shared scoreboard-driven scheduling
// engine used by the AIE backend. It owns one ResourceScoreboard, primes it
// with fixed-region bundle demands, and answers fitInInterval / emit queries
// against the merged occupancy.
//
// Phase 1 (refactor): the post-pipeliner uses the engine in modulo mode
// (Cfg.II > 0) with empty fixed-region arrays — bit-equivalent to its
// previous inline scoreboard path. Phase 2 will populate Pred/Succ
// projection scoreboards for cross-MBB awareness.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_AIE_AIEFIXEDREGIONSCOREBOARDSCHEDULER_H
#define LLVM_LIB_TARGET_AIE_AIEFIXEDREGIONSCOREBOARDSCHEDULER_H

#include "AIEHazardRecognizer.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/CodeGen/ResourceScoreboard.h"
#include <optional>

namespace llvm {
class SUnit;
} // namespace llvm

namespace llvm::AIE {

/// Trust level for an external (predecessor / successor) projection
/// scoreboard. Mirrors the enum in AIEInterBlockScheduling.h:296 — duplicated
/// here so this header doesn't drag in the heavier InterBlockScheduling
/// types. Keep the values in sync.
enum class FixedRegionScoreboardTrust {
  Absolute,
  AccountForAlign,
  Conservative
};

class FixedRegionScoreboardScheduler {
public:
  struct Config {
    /// 0 disables modulo (regular scheduling); >0 enables modulo
    /// scheduling at this initiation interval.
    int II = 0;

    /// Inclusive cycle bounds for the central scoreboard. PostPipeliner
    /// uses [0, ScoreboardSize-1]; the regular scheduler uses a centered
    /// frame.
    int LowestCycle = 0;
    int HighestCycle = 0;

    /// Intra-MBB fixed bundles. TopFixedBundles[i] is anchored at cycle
    /// LowestCycle + i; BotFixedBundles[i] is anchored at
    /// HighestCycle - (BotFixedBundles.size() - 1 - i).
    /// Phase 1: post-pipeliner leaves both empty; the regular scheduler
    /// migration in a later step populates them.
    ArrayRef<MachineBundle> TopFixedBundles;
    ArrayRef<MachineBundle> BotFixedBundles;

    /// External-MBB resource projections — caller-owned. Phase 1 leaves
    /// both null; Phase 2 wires them from BlockState. See plan file.
    const ResourceScoreboard<FuncUnitWrapper> *PredScoreboard = nullptr;
    const ResourceScoreboard<FuncUnitWrapper> *SuccScoreboard = nullptr;
    FixedRegionScoreboardTrust PredTrust =
        FixedRegionScoreboardTrust::Conservative;
    FixedRegionScoreboardTrust SuccTrust =
        FixedRegionScoreboardTrust::Conservative;
  };

  /// Owned-scoreboard mode: engine allocates and configures its own
  /// ResourceScoreboard from Cfg.LowestCycle/HighestCycle. Used by the
  /// post-pipeliner — the scoreboard is private to one schedule()
  /// invocation and discarded afterward.
  FixedRegionScoreboardScheduler(const AIEHazardRecognizer &HR,
                                 const Config &Cfg);

  /// Borrowed-scoreboard mode: engine operates on \p Scoreboard, which
  /// the caller owns and has already configured. Used by the regular
  /// post-RA scheduler so the engine and the AIEHazardRecognizer's
  /// scheduling-loop usage share one scoreboard.
  ///
  /// Cfg.LowestCycle / HighestCycle are not used to (re)configure the
  /// borrowed scoreboard — the caller is responsible for sizing.
  /// Cfg.II is still consulted for modulo behaviour in fitInInterval/
  /// emit, but the regular scheduler always passes Cfg.II = 0.
  FixedRegionScoreboardScheduler(const AIEHazardRecognizer &HR,
                                 ResourceScoreboard<FuncUnitWrapper> &Borrowed,
                                 const Config &Cfg);

  /// Emit the fixed-region demands (Top/Bot bundles + Pred/Succ
  /// projections) into the central scoreboard at their literal cycle
  /// positions. Idempotent on a freshly-cleared scoreboard.
  ///
  /// Fixed-region cycles are NOT modulo-broadcast — they execute exactly
  /// once per outer iteration even when Cfg.II > 0. Free instructions
  /// (driven through emit()) get the modulo broadcast.
  ///
  /// Returns true on success, false if a Top-fixed bundle and a Bot-fixed
  /// bundle would collide on the same modulo slot (only possible when
  /// Cfg.II > 0 and both fixed regions are non-empty). On failure the
  /// central scoreboard is left in a partially-primed state — the caller
  /// should discard the engine and rebuild with a larger scoreboard
  /// (the "shift Bot down by one cycle, grow L" loop described in the
  /// plan file) or report unsatisfiable resource demand to its own
  /// caller.
  ///
  /// In Cfg.II == 0 mode the only way for Top and Bot to collide is a
  /// length error caught by the Region-construction precondition
  /// L >= K_top + K_bot, so this method always returns true in that mode
  /// for well-formed inputs.
  [[nodiscard]] bool primeAllRegions();

  /// Scan [Earliest, Latest] for a cycle whose modulo slot has no
  /// scoreboard conflict with MI's demand. BottomUp = true scans high
  /// to low; otherwise low to high. Returns the absolute cycle on
  /// success, std::nullopt if no slot fits.
  ///
  /// Modulo handling: when Cfg.II > 0 the conflict check is performed
  /// at cycle % Cfg.II; otherwise at the literal cycle.
  std::optional<int> fitInInterval(const SUnit &SU, int Earliest, int Latest,
                                   bool BottomUp);

  /// Emit MI's demand at Cycle. When Cfg.II > 0 the demand is broadcast
  /// across all future modulo windows up to the scoreboard's
  /// HighestCycle. Returns false if a future-iteration emission detected
  /// a conflict (matches the post-pipeliner's "future-iteration
  /// conflict" detection at AIEPostPipeliner.cpp:888-892); returns true
  /// otherwise.
  bool emit(const SUnit &SU, int Cycle);

  /// Read-only access for diagnostics and validation passes (the
  /// post-pipeliner's replay loop reads the scoreboard directly).
  const ResourceScoreboard<FuncUnitWrapper> &getScoreboard() const {
    return Scoreboard;
  }
  ResourceScoreboard<FuncUnitWrapper> &getScoreboard() { return Scoreboard; }

  /// Reset the central scoreboard to all-empty. Pred/Succ projection
  /// scoreboards are caller-owned and untouched.
  void clear() { Scoreboard.clear(); }

  /// Emit one MachineInstr's resource demand at \p Cycle (no modulo
  /// broadcast). Counterpart to AIEHazardRecognizer's no-extra-args
  /// emitInScoreboard for use by callers that prime cycle-by-cycle
  /// (e.g. the regular scheduler replaying successor heads).
  void emitInstr(const MachineInstr &MI, int Cycle);

  /// Mark the cycle's resources as fully blocked. Counterpart to
  /// AIEHazardRecognizer::blockCycleInScoreboard.
  void blockCycle(int Cycle);

  /// Advance / recede the scoreboard by one cycle. Counterparts to
  /// the same-named methods on AIEHazardRecognizer (which forward to
  /// the underlying ResourceScoreboard). Side note: HR's AdvanceCycle/
  /// RecedeCycle additionally decrement its ReservedCycles counter;
  /// the engine forwarders don't, because they're meant for priming
  /// contexts where ReservedCycles is zero.
  void advanceCycle();
  void recedeCycle();

  /// Recede the scoreboard by \p N cycles. Counterpart to
  /// AIEHazardRecognizer::recedeScoreboard.
  void recedeScoreboard(int N);

  /// HR pass-through accessors used by the regular scheduler at sizing
  /// and horizon-decision time. Provided here so callers that route
  /// scoreboard ops through the engine can also read these without
  /// keeping a separate HR reference around. Signatures match HR's
  /// (unsigned for getMaxLookAhead/getPipelineDepth, int for
  /// getConflictHorizon).
  unsigned getMaxLookAhead() const;
  int getConflictHorizon() const;
  unsigned getPipelineDepth() const;

  /// Dump the scoreboard. Counterpart to
  /// AIEHazardRecognizer::dumpScoreboard.
  void dumpScoreboard() const { Scoreboard.dump(); }

  void dump() const;

private:
  const AIEHazardRecognizer &HR;
  Config Cfg;

  /// Owned in owned-scoreboard mode; left unused (default-constructed)
  /// in borrowed-scoreboard mode.
  ResourceScoreboard<FuncUnitWrapper> OwnedScoreboard;

  /// References either OwnedScoreboard or the caller-owned scoreboard
  /// passed to the borrow constructor. All engine internal logic talks
  /// to this reference; mode (owned vs borrowed) is invisible.
  ResourceScoreboard<FuncUnitWrapper> &Scoreboard;

  /// Emit one bundle's resource demand at the given absolute cycle (no
  /// modulo broadcast — used for fixed-region priming).
  void emitFixedBundleAt(const MachineBundle &Bundle, int Cycle);

  /// Return true iff any instruction in \p Bundle would see a scoreboard
  /// hazard if emitted at \p Cycle. Used by primeAllRegions() in modulo
  /// mode to detect Top/Bot fixed-region collisions before committing
  /// the Bot emission.
  bool bundleConflictsAt(const MachineBundle &Bundle, int Cycle) const;

  /// Map an absolute cycle to the scoreboard slot used for conflict
  /// queries (Cycle % II when modulo, else Cycle).
  int conflictSlot(int Cycle) const {
    return Cfg.II > 0 ? Cycle % Cfg.II : Cycle;
  }
};

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIEFIXEDREGIONSCOREBOARDSCHEDULER_H
