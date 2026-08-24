//===- AIESiblingLoopOptimizer.cpp - Sibling loop optimization --*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Implementation of sibling loop optimization: merges matching leading
// prologue bundles into the epilogue and transfers them to the entry block.
//
//===----------------------------------------------------------------------===//

#include "AIESiblingLoopOptimizer.h"
#include "AIEBaseInstrInfo.h"
#include "AIEHazardRecognizer.h"
#include "AIEInterBlockScheduling.h"
#include "Utils/AIEMachineBundleUtils.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/Support/CommandLine.h"

#define DEBUG_TYPE "sibling-scheduler"
#define DEBUG_BLOCKS(X) DEBUG_WITH_TYPE("sched-blocks", X)

using namespace llvm;

static llvm::cl::opt<bool> TryMergePrologueIntoPreheader(
    "aie-sibling-try-merge-prologue-preheader", cl::Hidden, cl::init(false),
    cl::desc("Try to merge instead of appending (experimental)"));

namespace llvm::AIE {

SiblingLoopOptimizer::SiblingLoopOptimizer(
    InterBlockScheduling &IBS, const AIEHazardRecognizer &HR,
    const AIEBaseInstrInfo &TII,
    const AIEAlternateDescriptors &SelectedAltDescs,
    const MachineSchedContext &Context)
    : IBS(IBS), HR(HR), TII(TII), SelectedAltDescs(SelectedAltDescs),
      Context(Context) {}

/// Calculate how many cycles of slack exist between Epilogue and Prologue.
/// Builds an inter-block DAG and computes min(Distance - Latency) for all
/// cross-boundary edges. Returns the maximum safe overlap (0 if none).
///
/// For any in-flight register value from the inner loop body, exactly one of
/// two things is true related to WAW deps:

/// * TopInsert contains the write itself - The SWP epilogue drain issues the
/// same instruction (from a later pipeline stage), so the register def appears
/// directly in TopInsert -> our DAG captures the WAW edge to any prologue
/// instruction that also defines it.
///
/// * TopInsert consumes the value - The drain reads that register (RAW within
/// the pipeline). The TopInsert instruction that consumes it will itself
/// produce results or have its own timing constraints -> our DAG captures those
/// edges (WAR/WAW) transitively.
static int calculateSchedulingSlack(const BlockState &EpilogueBS,
                                    const BlockState &PrologueBS,
                                    const MachineSchedContext &Context) {
  const Region &EpilogueRegion = EpilogueBS.getTop();
  const Region &PrologueRegion = PrologueBS.getTop();
  const auto &EpilogueBundles = EpilogueRegion.Bundles;
  const auto &PrologueBundles = PrologueRegion.Bundles;
  const int PrologueLength = PrologueBundles.size();

  DEBUG_BLOCKS(dbgs() << "    calculateSchedulingSlack: EpilogueLen="
                      << EpilogueBundles.size()
                      << " PrologueLen=" << PrologueLength << "\n");

  if (EpilogueBundles.empty() || PrologueBundles.empty())
    return 0;

  // Build an InterBlockEdges DAG from Epilogue -> Prologue
  InterBlockEdges Edges(Context);

  // Map prologue instructions to their scheduled cycle positions
  std::map<const MachineInstr *, int> PrologueCycles;
  int Cycle = 0;
  for (const auto &Bundle : PrologueBundles) {
    for (MachineInstr *MI : Bundle.getInstrs()) {
      PrologueCycles[MI] = Cycle;
    }
    ++Cycle;
  }

  // Add pre-boundary nodes from TopInsert clones (SWP epilogue clones).
  // These execute at the top of the epilogue block, before the free
  // instructions, and may have dependencies to prologue instructions.
  // Including them ensures WAW/WAR edges from in-flight inner-loop results
  // to prologue instructions are captured — see the NStages ≥ 2 comment
  // in run() for the correctness argument.
  for (MachineInstr *MI : EpilogueBS.TopInsert.Clones) {
    Edges.addNode(MI);
  }

  // Add pre-boundary nodes (Epilogue instructions in semantic order)
  for (MachineInstr *MI : EpilogueRegion.getFreeInstructions()) {
    Edges.addNode(MI);
  }

  Edges.markBoundary();

  // Add post-boundary nodes (Prologue instructions in semantic order)
  for (MachineInstr *MI : PrologueRegion.getFreeInstructions()) {
    Edges.addNode(MI);
  }

  // Add post-boundary nodes from BottomInsert clones (SWP prologue clones)
  for (MachineInstr *MI : PrologueBS.BottomInsert.Clones) {
    Edges.addNode(MI);
  }

  Edges.buildEdges();

  // Compute min(Distance - Latency) for all cross-boundary edges
  // Start with a large slack (no constraint means infinite overlap possible)
  int MinSlack = INT_MAX;

  // For each pre-boundary instruction (in Epilogue), from end to start
  int Height = 1;
  for (const auto &Bundle : reverse(EpilogueBundles)) {
    for (MachineInstr *PreBoundaryMI : Bundle.getInstrs()) {
      const SUnit *Pred = Edges.getPreBoundaryNode(PreBoundaryMI);
      if (!Pred)
        continue;

      for (const auto &SDep : Pred->Succs) {
        SUnit *Succ = SDep.getSUnit();

        // Only consider cross-boundary edges (to post-boundary nodes)
        if (!Edges.isPostBoundaryNode(Succ))
          continue;

        const MachineInstr *PostBoundaryMI = Succ->getInstr();

        // Get prologue depth (cycle position of post-boundary instruction)
        int Depth;
        if (PostBoundaryMI) {
          auto It = PrologueCycles.find(PostBoundaryMI);
          if (It != PrologueCycles.end())
            Depth = It->second;
          else
            Depth = PrologueLength; // Instruction not in prologue bundles
        } else {
          // ExitSU - use prologue length as depth
          Depth = PrologueLength;
        }

        // Distance = Height + Depth (total cycles between them)
        const int Distance = Height + Depth;
        const int Latency = SDep.getSignedLatency();
        const int EdgeSlack = Distance - Latency;

        DEBUG_BLOCKS(dbgs()
                     << "      Edge: Height=" << Height << " Depth=" << Depth
                     << " Lat=" << Latency << " Slack=" << EdgeSlack << "\n");

        MinSlack = std::min(MinSlack, EdgeSlack);
      }
    }
    ++Height;
  }

  // Also check TopInsert bundles (SWP epilogue at the top of the block).
  // These execute before the free instructions, so they have larger Height
  // values — continuing the count from where the free instructions ended.
  for (const auto &Bundle : reverse(EpilogueBS.TopInsert.Bundles)) {
    for (MachineInstr *PreBoundaryMI : Bundle.getInstrs()) {
      const SUnit *Pred = Edges.getPreBoundaryNode(PreBoundaryMI);
      if (!Pred)
        continue;

      for (const auto &SDep : Pred->Succs) {
        SUnit *Succ = SDep.getSUnit();

        if (!Edges.isPostBoundaryNode(Succ))
          continue;

        const MachineInstr *PostBoundaryMI = Succ->getInstr();

        int Depth;
        if (PostBoundaryMI) {
          auto It = PrologueCycles.find(PostBoundaryMI);
          Depth = (It != PrologueCycles.end()) ? It->second : PrologueLength;
        } else {
          Depth = PrologueLength;
        }

        const int Distance = Height + Depth;
        const int Latency = SDep.getSignedLatency();
        const int EdgeSlack = Distance - Latency;

        DEBUG_BLOCKS(dbgs() << "      TopInsert Edge: Height=" << Height
                            << " Depth=" << Depth << " Lat=" << Latency
                            << " Slack=" << EdgeSlack << "\n");

        MinSlack = std::min(MinSlack, EdgeSlack);
      }
    }
    ++Height;
  }

  // If no cross-boundary edges were found, return 0 (conservative)
  if (MinSlack == INT_MAX) {
    DEBUG_BLOCKS(dbgs() << "    No cross-boundary edges found\n");
    return 0;
  }

  DEBUG_BLOCKS(dbgs() << "    Computed slack: " << MinSlack << "\n");

  return std::max(0, MinSlack);
}

/// Build a top-down resource scoreboard seeded with the trailing resources
/// of a preceding loop body. The loop body is replayed enough times to fill
/// the conflict horizon, then the block's own bundles are replayed on top.
/// This mirrors initializeTopScoreBoard in the machine scheduler.
///
/// \param BlockBundles The bundles of the block (e.g. epilogue)
/// \param LoopBodyBundles The inner loop body that precedes the block.
///        Pass empty to get the same result as createTopDownScoreboard.
/// \param HR Hazard recognizer
/// \param SelectedAltDescs Alternate descriptor map
static ResourceScoreboard<FuncUnitWrapper>
createTopDownScoreboardWithLoopContext(
    ArrayRef<MachineBundle> BlockBundles,
    ArrayRef<MachineBundle> LoopBodyBundles, const AIEHazardRecognizer &HR,
    const AIEAlternateDescriptors &SelectedAltDescs) {
  ResourceScoreboard<FuncUnitWrapper> Scoreboard;
  Scoreboard.reset(HR.getMaxLookAhead());

  // Replay the loop body N times so that trailing resources that extend
  // into the block are captured, just like initializeTopScoreBoard.
  if (!LoopBodyBundles.empty()) {
    const int LoopSize = LoopBodyBundles.size();
    const int ReplayTimes = (HR.getConflictHorizon() + LoopSize - 1) / LoopSize;
    for (int I = 0; I < ReplayTimes; I++) {
      for (const auto &Bundle : LoopBodyBundles) {
        for (MachineInstr *MI : Bundle.getInstrs())
          HR.emitInScoreboard(Scoreboard, *MI, *SelectedAltDescs.getDesc(MI),
                              0);
        Scoreboard.advance();
      }
    }
  }

  // Then replay the block's own bundles on top.
  for (const auto &Bundle : BlockBundles) {
    for (MachineInstr *MI : Bundle.getInstrs())
      HR.emitInScoreboard(Scoreboard, *MI, *SelectedAltDescs.getDesc(MI), 0);
    Scoreboard.advance();
  }

  return Scoreboard;
}

/// Check how many leading bundles from both sibling prologues can be merged
/// into trailing bundles of EpilogueBundles without resource conflicts.
///
/// The epilogue is preceded by an inner loop whose trailing resources extend
/// into the epilogue, and each prologue is followed by its own inner loop.
/// The scoreboard is seeded with the preceding loop context and the check
/// also validates that the remaining prologue bundles do not conflict with
/// the inner loop that follows them.
///
/// \param EpilogueBundles The epilogue bundles to merge into
/// \param SteadyPrologue The steady-state prologue bundles
/// \param LastIterPrologue The last-iteration prologue bundles
/// \param PrecedingLoopBundles Inner loop body preceding the epilogue
/// \param SteadyInnerBundles Inner loop body following the steady prologue
/// \param PeeledInnerBundles Inner loop body following the last-iter prologue
/// \param MaxPossibleMerge Maximum number of bundles to try merging
/// \param HR Hazard recognizer for resource checking
/// \param SelectedAltDescs Selected alternate descriptors
/// \returns The maximum number of bundles that can be merged without conflicts
static int
checkResourceMergeability(ArrayRef<MachineBundle> EpilogueBundles,
                          ArrayRef<MachineBundle> SteadyPrologue,
                          ArrayRef<MachineBundle> LastIterPrologue,
                          ArrayRef<MachineBundle> PrecedingLoopBundles,
                          ArrayRef<MachineBundle> SteadyInnerBundles,
                          ArrayRef<MachineBundle> PeeledInnerBundles,
                          int MaxPossibleMerge, const AIEHazardRecognizer &HR,
                          const AIEAlternateDescriptors &SelectedAltDescs) {

  const int EpilogueSize = EpilogueBundles.size();
  const int SteadyPrologueSize = SteadyPrologue.size();
  const int LastPrologueIterSize = LastIterPrologue.size();

  // Can't merge more bundles than exist in any region
  const int MaxMerge = std::min({MaxPossibleMerge, EpilogueSize,
                                 SteadyPrologueSize, LastPrologueIterSize});
  if (MaxMerge <= 0)
    return 0;

  // Build the epilogue scoreboard WITH the preceding inner loop context.
  // This captures resources from the loop body that extend into the epilogue.
  ResourceScoreboard<FuncUnitWrapper> EpilogueScoreboard =
      createTopDownScoreboardWithLoopContext(
          EpilogueBundles, PrecedingLoopBundles, HR, SelectedAltDescs);

  DEBUG_BLOCKS(dbgs() << "    checkResourceMergeability: EpilogueSize="
                      << EpilogueSize
                      << " SteadyPrologueSize=" << SteadyPrologueSize
                      << " LastPrologueIterSize=" << LastPrologueIterSize
                      << " MaxMerge=" << MaxMerge << "\n");

  // Check whether a bundle has a resource conflict at a given delta.
  auto BundleHasConflict =
      [&](const ResourceScoreboard<FuncUnitWrapper> &Scoreboard,
          const MachineBundle &Bundle, int Delta) -> bool {
    for (MachineInstr *MI : Bundle.getInstrs()) {
      if (HR.getHazardType(Scoreboard, MI, Delta)) {
        DEBUG_BLOCKS(dbgs()
                     << "      Conflict (delta=" << Delta << "): " << *MI);
        return true;
      }
    }
    return false;
  };

  // Emit a bundle's resources into the scoreboard at a given delta.
  auto EmitBundle = [&](ResourceScoreboard<FuncUnitWrapper> &Scoreboard,
                        const MachineBundle &Bundle, int Delta) {
    for (MachineInstr *MI : Bundle.getInstrs())
      HR.emitInScoreboard(Scoreboard, *MI, *SelectedAltDescs.getDesc(MI),
                          Delta);
  };

  // Check a range of bundles [StartIdx, EndIdx) against a scoreboard,
  // using offsets relative to MergeCount.
  auto HasConflictInRange =
      [&](const ResourceScoreboard<FuncUnitWrapper> &Scoreboard,
          ArrayRef<MachineBundle> Bundles, int StartIdx, int EndIdx,
          int MergeCount) -> bool {
    for (int I = StartIdx; I < EndIdx; ++I) {
      if (BundleHasConflict(Scoreboard, Bundles[I], I - MergeCount))
        return true;
    }
    return false;
  };

  // Check a prologue path: remaining prologue bundles followed by the
  // successor inner loop bundles (up to conflict horizon).
  auto HasConflictInProloguePath =
      [&](const ResourceScoreboard<FuncUnitWrapper> &Scoreboard,
          ArrayRef<MachineBundle> Prologue, ArrayRef<MachineBundle> InnerLoop,
          int MergeCount) -> bool {
    const int PrologueSize = Prologue.size();
    // Check remaining prologue bundles [MergeCount, PrologueSize)
    if (HasConflictInRange(Scoreboard, Prologue, MergeCount, PrologueSize,
                           MergeCount))
      return true;
    // Check the inner loop bundles that follow the remaining prologue,
    // up to the conflict horizon.
    const int ConflictHorizon = HR.getConflictHorizon();
    const int RemainingPrologueLen = PrologueSize - MergeCount;
    const int InnerBundlesToCheck =
        std::min(static_cast<int>(InnerLoop.size()),
                 std::max(0, ConflictHorizon - RemainingPrologueLen));
    for (int I = 0; I < InnerBundlesToCheck; ++I) {
      if (BundleHasConflict(Scoreboard, InnerLoop[I], RemainingPrologueLen + I))
        return true;
    }
    return false;
  };

  // Try MergeCount from MaxMerge down to 1
  for (int MergeCount = MaxMerge; MergeCount >= 1; --MergeCount) {
    DEBUG_BLOCKS(dbgs() << "    Trying MergeCount=" << MergeCount << "\n");

    // Start with a copy of the epilogue scoreboard (with loop context).
    auto TrialScoreboard = EpilogueScoreboard;
    bool HasConflict = false;

    // Phase 1: Check merged bundles and progressively emit them.
    // Merged prologue bundles overlay the trailing epilogue bundles,
    // creating a denser resource footprint. We emit each merged bundle
    // into the scoreboard so subsequent checks see the combined state.
    for (int I = 0; I < MergeCount && !HasConflict; ++I) {
      const int Delta = I - MergeCount;
      HasConflict =
          BundleHasConflict(TrialScoreboard, SteadyPrologue[I], Delta);
      if (!HasConflict)
        EmitBundle(TrialScoreboard, SteadyPrologue[I], Delta);
    }

    // Phase 2: Check remaining bundles of BOTH prologue paths against the
    // merged scoreboard. Each path includes the remaining prologue bundles
    // followed by the inner loop bundles that succeed them.
    if (!HasConflict)
      HasConflict = HasConflictInProloguePath(TrialScoreboard, SteadyPrologue,
                                              SteadyInnerBundles, MergeCount);
    if (!HasConflict)
      HasConflict = HasConflictInProloguePath(TrialScoreboard, LastIterPrologue,
                                              PeeledInnerBundles, MergeCount);

    if (!HasConflict) {
      DEBUG_BLOCKS(dbgs() << "    Resource mergeability check passed with "
                          << MergeCount << " bundles\n");
      return MergeCount;
    }
  }

  DEBUG_BLOCKS(dbgs() << "    No bundles can be merged without conflicts\n");
  return 0;
}

unsigned SiblingLoopOptimizer::getNumberOfMergeableBundles(
    const BlockState &EpilogueBS, const BlockState &SteadyTopBS,
    const BlockState &LastIterTopBS) {
  // Get the prologue bundles from both regions
  const auto &SteadyPrologue = SteadyTopBS.getTop().Bundles;
  const auto &LastIterPrologue = LastIterTopBS.getTop().Bundles;

  // Create filter to stop at loop setup instructions
  auto LoopSetupFilter = [this](const MachineInstr &MI) {
    return TII.isZeroOverheadLoopSetupInstr(MI) && !TII.isZOLTripCountDef(MI);
  };

  // Count matching leading bundles, stopping at loop setup instructions
  const unsigned MatchingBundles =
      AIEMachineBundleUtils::countMatchingLeadingBundles(
          SteadyPrologue, LastIterPrologue, LoopSetupFilter);

  if (MatchingBundles == 0) {
    DEBUG_BLOCKS(dbgs() << "  No matching bundles between prologues\n");
    return 0;
  }

  DEBUG_BLOCKS(dbgs() << "  Found " << MatchingBundles
                      << " matching bundles\n");

  // Get the epilogue bundles
  const Region &EpilogueRegion = EpilogueBS.getTop();
  const auto &EpilogueBundles = EpilogueRegion.Bundles;

  // Calculate scheduling slack for Epilogue -> SteadyPrologue
  const int SlackToSteady =
      calculateSchedulingSlack(EpilogueBS, SteadyTopBS, Context);
  DEBUG_BLOCKS(dbgs() << "  Slack (Epilogue->SteadyPrologue): " << SlackToSteady
                      << " cycles\n");
  if (SlackToSteady == 0) {
    DEBUG_BLOCKS(dbgs() << "  No slack to steady prologue, skipping\n");
    return 0;
  }

  // Calculate scheduling slack for Epilogue -> LastIterPrologue
  const int SlackToLastIter =
      calculateSchedulingSlack(EpilogueBS, LastIterTopBS, Context);
  DEBUG_BLOCKS(dbgs() << "  Slack (Epilogue->LastIterPrologue): "
                      << SlackToLastIter << " cycles\n");
  if (SlackToLastIter == 0) {
    DEBUG_BLOCKS(dbgs() << "  No slack to last iter prologue, skipping\n");
    return 0;
  }

  // Get the inner loop body bundles for resource context.
  // SteadyInner precedes the epilogue and follows SteadyTop.
  // PeeledIterInner follows PeeledIterTop.
  const auto &OLS = *EpilogueBS.getOuterLoopContext();
  const auto &SteadyInnerBundles =
      IBS.getBlockState(OLS.SteadyInner).getTop().Bundles;
  const auto &PeeledInnerBundles =
      IBS.getBlockState(OLS.PeeledIterInner).getTop().Bundles;

  // Check resource mergeability (for correctness, do last)
  const int MaxPossibleMerge = std::min(
      {static_cast<int>(MatchingBundles), SlackToSteady, SlackToLastIter});
  const int MergeableBundles = checkResourceMergeability(
      EpilogueBundles, SteadyPrologue, LastIterPrologue, SteadyInnerBundles,
      SteadyInnerBundles, PeeledInnerBundles, MaxPossibleMerge, HR,
      SelectedAltDescs);

  if (MergeableBundles == 0) {
    DEBUG_BLOCKS(dbgs() << "  Resource mergeability check failed\n");
    return 0;
  }

  DEBUG_BLOCKS(dbgs() << "  Can merge " << MergeableBundles
                      << " bundles into epilogue\n");
  return MergeableBundles;
}

unsigned SiblingLoopOptimizer::getPreheaderMergeCount(
    const BlockState &EntryBS, const BlockState &SteadyTopBS,
    ArrayRef<MachineBundle> SteadyInnerBundles, unsigned NumTransferred) {
  const auto &EntryBundles = EntryBS.getTop().Bundles;

  // TODO: Resource checks are not complete. Check and fix them before enabling
  if (!TryMergePrologueIntoPreheader)
    return 0;

  if (EntryBundles.empty() || NumTransferred == 0)
    return 0;

  // Latency check: compute slack between preheader and steady prologue.
  // The transferred bundles are the first N bundles of the steady prologue,
  // so the slack tells us how many cycles of overlap are safe.
  const int LatencySlack =
      calculateSchedulingSlack(EntryBS, SteadyTopBS, Context);
  DEBUG_BLOCKS(dbgs() << "  Preheader merge: LatencySlack=" << LatencySlack
                      << "\n");
  if (LatencySlack <= 0)
    return 0;

  // Resource check: use the first NumTransferred bundles of the steady
  // prologue as the source. Pass the same array for both prologue arguments
  // since the preheader has a single successor path.
  // No loop precedes the preheader, so PrecedingLoopBundles is empty.
  // The inner loop that follows the remaining prologue is passed as follow-up
  // context to detect resource conflicts that extend into the loop.
  const auto &SteadyPrologue = SteadyTopBS.getTop().Bundles;
  ArrayRef<MachineBundle> TransferredSlice(SteadyPrologue.data(),
                                           NumTransferred);
  const int MaxLimit = std::min(static_cast<int>(NumTransferred), LatencySlack);
  ArrayRef<MachineBundle> NoPrecedingLoop;
  const int ResourceMerge = checkResourceMergeability(
      EntryBundles, TransferredSlice, TransferredSlice, NoPrecedingLoop,
      SteadyInnerBundles, SteadyInnerBundles, MaxLimit, HR, SelectedAltDescs);
  DEBUG_BLOCKS(dbgs() << "  Preheader merge: ResourceMerge=" << ResourceMerge
                      << "\n");

  return static_cast<unsigned>(std::max(0, ResourceMerge));
}

void SiblingLoopOptimizer::mergeBundles(BlockState &EntryBS,
                                        BlockState &SteadyTopBS,
                                        BlockState &EpilogueBS,
                                        BlockState &LastIterTopBS,
                                        unsigned NumBundles) {
  const auto &SteadyPrologue = SteadyTopBS.getTop().Bundles;
  const auto &LastIterPrologue = LastIterTopBS.getTop().Bundles;
  const auto &EpilogueBundles = EpilogueBS.getTop().Bundles;

  // Merge prologue into epilogue and update BlockState
  auto MergedBundles = AIEMachineBundleUtils::mergeBundlesIntoMBB(
      *EpilogueBS.TheBlock, EpilogueBundles, SteadyPrologue, NumBundles, TII);
  EpilogueBS.getTop().Bundles = std::move(MergedBundles);

  // Get inner loop bundles for preheader merge resource check.
  const auto &OLS = *EpilogueBS.getOuterLoopContext();
  const auto &SteadyInnerBundles =
      IBS.getBlockState(OLS.SteadyInner).getTop().Bundles;

  // Compute preheader merge count BEFORE transfer (needs full SteadyTopBS
  // to build inter-block DAG for latency slack computation).
  const unsigned PreheaderMerge = getPreheaderMergeCount(
      EntryBS, SteadyTopBS, SteadyInnerBundles, NumBundles);

  // Transfer leading bundles from SteadyTop to Entry (OuterPreheader)
  auto [TransferredBundles, RemainingSteady] =
      AIEMachineBundleUtils::transferLeadingBundles(
          *EntryBS.TheBlock, *SteadyTopBS.TheBlock, SteadyPrologue, NumBundles,
          TII);

  // Update preheader: merge overlapping bundles into trailing preheader
  // cycles, or just append if no merge is possible.
  auto &EntryBundles = EntryBS.getTop().Bundles;
  if (PreheaderMerge > 0) {
    EntryBundles = AIEMachineBundleUtils::mergeAndAppendBundlesIntoMBB(
        *EntryBS.TheBlock, EntryBundles, TransferredBundles, PreheaderMerge,
        TII);
    DEBUG_BLOCKS(dbgs() << "  Preheader merge: merged " << PreheaderMerge
                        << " of " << NumBundles
                        << " transferred bundles into preheader\n");
  } else {
    llvm::append_range(EntryBundles, std::move(TransferredBundles));
  }
  SteadyTopBS.getTop().Bundles = std::move(RemainingSteady);

  // Remove leading bundles from LastIterTop and update BlockState
  auto RemainingLastIter = AIEMachineBundleUtils::removeLeadingBundles(
      *LastIterTopBS.TheBlock, LastIterPrologue, NumBundles, TII);
  LastIterTopBS.getTop().Bundles = std::move(RemainingLastIter);

  DEBUG_BLOCKS(dbgs() << "  Sibling loop optimization complete: moved "
                      << NumBundles << " bundles\n");
}

void SiblingLoopOptimizer::tryMergePrologues(BlockState &EntryBS,
                                             BlockState &SteadyTopBS,
                                             BlockState &EpilogueBS,
                                             BlockState &LastIterTopBS) {
  unsigned NumBundles =
      getNumberOfMergeableBundles(EpilogueBS, SteadyTopBS, LastIterTopBS);
  if (NumBundles == 0)
    return;
  mergeBundles(EntryBS, SteadyTopBS, EpilogueBS, LastIterTopBS, NumBundles);
}

void SiblingLoopOptimizer::run() {
  for (auto &[BB, BS] : IBS.getBlocks()) {
    if (!BS.getOuterLoopContext())
      continue;

    const auto &OLS = *BS.getOuterLoopContext();
    if (OLS.Speculative)
      continue;

    if (!OLS.isSafeForSiblingMerge()) {
      DEBUG_BLOCKS(dbgs() << "  Skipping: OLP blocks are not in linear "
                             "layout order\n");
      continue;
    }

    DEBUG_BLOCKS(dbgs() << "optimizeSiblingLoops: Processing outer loop latch "
                        << BB->getNumber() << "\n");

    BlockState &EntryBS = IBS.getBlockState(OLS.OuterPreheader);
    BlockState &SteadyTopBS = IBS.getBlockState(OLS.SteadyTop);
    BlockState &LastIterTopBS = IBS.getBlockState(OLS.PeeledIterTop);

    // Verify BS is the epilogue (SteadyBottom)
    assert(BS.TheBlock == OLS.SteadyBottom && "BS must match OLS.SteadyBottom");

    // Bail out unless each of P, ST, SB and PT has exactly one region.
    // Multi-region blocks may contain non-bundled instructions between
    // regions that the bundle merge infrastructure cannot handle safely.
    if (EntryBS.getRegions().size() != 1 ||
        SteadyTopBS.getRegions().size() != 1 || BS.getRegions().size() != 1 ||
        LastIterTopBS.getRegions().size() != 1) {
      DEBUG_BLOCKS(dbgs() << "  Skipping: not all blocks have exactly one "
                             "region\n");
      continue;
    }

    // Bail out unless both inner loops are pipelined with at least 2 stages.
    // Non-pipelined inner loops may have safety-margin NOPs inserted by
    // emitTopSafetyMargin directly into the MBB without recording them in
    // Region.Bundles. replaceMBBWithBundles would silently lose those NOPs.
    // With ≥2 stages the epilogue/prologue inserts go through TopInsert/
    // BottomInsert which are tracked as fixed bundles.
    //
    // The ≥2 stages requirement also guarantees that TopInsert is non-empty.
    // This is essential for WAW correctness: in-flight results from the
    // inner loop's last iteration(s) may still be landing in the epilogue
    // while we merge prologue instructions. Every such in-flight register
    // either has its write replicated in TopInsert (the SWP drain catches
    // it) or is consumed by a TopInsert instruction. Either way, including
    // TopInsert clones in the inter-block DAG (see calculateSchedulingSlack)
    // ensures all WAW/WAR edges are captured and the slack calculation
    // correctly blocks merges that would violate write-ordering constraints.
    const BlockState &SteadyInnerBS = IBS.getBlockState(OLS.SteadyInner);
    const BlockState &PeeledInnerBS = IBS.getBlockState(OLS.PeeledIterInner);
    if (!SteadyInnerBS.isPipelined() || !PeeledInnerBS.isPipelined() ||
        SteadyInnerBS.getPostSWP().getStageCount() < 2 ||
        PeeledInnerBS.getPostSWP().getStageCount() < 2) {
      DEBUG_BLOCKS(dbgs() << "  Skipping: inner loops not pipelined with "
                             "at least 2 stages\n");
      continue;
    }

    // Validate prologue bundles exist (required for matching/merging)
    if (SteadyTopBS.getTop().Bundles.empty() ||
        LastIterTopBS.getTop().Bundles.empty()) {
      DEBUG_BLOCKS(dbgs() << "  Skipping: empty prologue bundles\n");
      continue;
    }

    tryMergePrologues(EntryBS, SteadyTopBS, BS, LastIterTopBS);
  }
}

} // namespace llvm::AIE
