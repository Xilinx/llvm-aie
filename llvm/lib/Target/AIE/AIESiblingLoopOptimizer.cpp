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

#define DEBUG_TYPE "machine-scheduler"
#define DEBUG_BLOCKS(X) DEBUG_WITH_TYPE("sched-blocks", X)

using namespace llvm;

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
static int calculateSchedulingSlack(const Region &EpilogueRegion,
                                    const BlockState &PrologueBS,
                                    const MachineSchedContext &Context) {
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

  // Add pre-boundary nodes (Epilogue instructions in semantic order)
  for (MachineInstr *MI : EpilogueRegion.getFreeInstructions()) {
    Edges.addNode(MI);
  }

  Edges.markBoundary();

  // Add post-boundary nodes (Prologue instructions in semantic order)
  for (MachineInstr *MI : PrologueRegion.getFreeInstructions()) {
    Edges.addNode(MI);
  }

  // Add post-boundary nodes from BottomInsertSemanticOrder (SWP prologue
  // clones)
  for (MachineInstr *MI : PrologueBS.BottomInsertSemanticOrder) {
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

  // If no cross-boundary edges were found, return 0 (conservative)
  if (MinSlack == INT_MAX) {
    DEBUG_BLOCKS(dbgs() << "    No cross-boundary edges found\n");
    return 0;
  }

  DEBUG_BLOCKS(dbgs() << "    Computed slack: " << MinSlack << "\n");

  return std::max(0, MinSlack);
}

/// Check how many leading bundles from both sibling prologues can be merged
/// into trailing bundles of EpilogueBundles without resource conflicts.
///
/// The epilogue has two successors: the steady-state prologue and the
/// last-iteration prologue. When prologue bundles are merged into the
/// epilogue, the merged bundles create a denser resource footprint.
/// This function builds a progressive scoreboard that includes both epilogue
/// AND merged prologue resources, then validates the remaining bundles of
/// BOTH prologues against it.
///
/// \param EpilogueBundles The epilogue bundles to merge into
/// \param SteadyPrologue The steady-state prologue bundles
/// \param LastIterPrologue The last-iteration prologue bundles
/// \param OptimizationLimit Maximum number of bundles to try merging
/// \param HR Hazard recognizer for resource checking
/// \param SelectedAltDescs Selected alternate descriptors
/// \returns The maximum number of bundles that can be merged without conflicts
static int
checkResourceMergeability(ArrayRef<MachineBundle> EpilogueBundles,
                          ArrayRef<MachineBundle> SteadyPrologue,
                          ArrayRef<MachineBundle> LastIterPrologue,
                          int OptimizationLimit, const AIEHazardRecognizer &HR,
                          const AIEAlternateDescriptors &SelectedAltDescs) {

  const int EpilogueSize = EpilogueBundles.size();
  const int SteadySize = SteadyPrologue.size();
  const int LastIterSize = LastIterPrologue.size();

  // Can't merge more bundles than exist in any region
  const int MaxMerge =
      std::min({OptimizationLimit, EpilogueSize, SteadySize, LastIterSize});
  if (MaxMerge <= 0)
    return 0;

  // Build ONE scoreboard from ALL epilogue bundles (top-down).
  // This gives us the resource state after executing the entire epilogue.
  ResourceScoreboard<FuncUnitWrapper> EpilogueScoreboard =
      createTopDownScoreboard(EpilogueBundles, HR, SelectedAltDescs);

  DEBUG_BLOCKS(dbgs() << "    checkResourceMergeability: EpilogueSize="
                      << EpilogueSize << " SteadySize=" << SteadySize
                      << " LastIterSize=" << LastIterSize
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

  // Check a range of bundles [StartIdx, Bundles.size()) against a scoreboard.
  auto HasConflictInRange =
      [&](const ResourceScoreboard<FuncUnitWrapper> &Scoreboard,
          ArrayRef<MachineBundle> Bundles, int StartIdx,
          int MergeCount) -> bool {
    for (int I = StartIdx, E = Bundles.size(); I < E; ++I) {
      if (BundleHasConflict(Scoreboard, Bundles[I], I - MergeCount))
        return true;
    }
    return false;
  };

  // Try MergeCount from MaxMerge down to 1
  for (int MergeCount = MaxMerge; MergeCount >= 1; --MergeCount) {
    DEBUG_BLOCKS(dbgs() << "    Trying MergeCount=" << MergeCount << "\n");

    // Start with a copy of the epilogue-only scoreboard for this attempt.
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

    // Phase 2: Check remaining bundles of BOTH prologues against the
    // merged scoreboard (epilogue + merged prologue resources).
    if (!HasConflict)
      HasConflict = HasConflictInRange(TrialScoreboard, SteadyPrologue,
                                       MergeCount, MergeCount);
    if (!HasConflict)
      HasConflict = HasConflictInRange(TrialScoreboard, LastIterPrologue,
                                       MergeCount, MergeCount);

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
      calculateSchedulingSlack(EpilogueRegion, SteadyTopBS, Context);
  DEBUG_BLOCKS(dbgs() << "  Slack (Epilogue->SteadyPrologue): " << SlackToSteady
                      << " cycles\n");
  if (SlackToSteady == 0) {
    DEBUG_BLOCKS(dbgs() << "  No slack to steady prologue, skipping\n");
    return 0;
  }

  // Calculate scheduling slack for Epilogue -> LastIterPrologue
  const int SlackToLastIter =
      calculateSchedulingSlack(EpilogueRegion, LastIterTopBS, Context);
  DEBUG_BLOCKS(dbgs() << "  Slack (Epilogue->LastIterPrologue): "
                      << SlackToLastIter << " cycles\n");
  if (SlackToLastIter == 0) {
    DEBUG_BLOCKS(dbgs() << "  No slack to last iter prologue, skipping\n");
    return 0;
  }

  // Check resource mergeability (for correctness, do last)
  const int MaxPossibleMerge = std::min(
      {static_cast<int>(MatchingBundles), SlackToSteady, SlackToLastIter});
  const int MergeableBundles = checkResourceMergeability(
      EpilogueBundles, SteadyPrologue, LastIterPrologue, MaxPossibleMerge, HR,
      SelectedAltDescs);

  if (MergeableBundles == 0) {
    DEBUG_BLOCKS(dbgs() << "  Resource mergeability check failed\n");
    return 0;
  }

  DEBUG_BLOCKS(dbgs() << "  Can merge " << MergeableBundles
                      << " bundles into epilogue\n");
  return MergeableBundles;
}

unsigned
SiblingLoopOptimizer::getPreheaderMergeCount(const BlockState &EntryBS,
                                             const BlockState &SteadyTopBS,
                                             unsigned NumTransferred) {
  const auto &EntryBundles = EntryBS.getTop().Bundles;
  if (EntryBundles.empty() || NumTransferred == 0)
    return 0;

  // Latency check: compute slack between preheader and steady prologue.
  // The transferred bundles are the first N bundles of the steady prologue,
  // so the slack tells us how many cycles of overlap are safe.
  const int LatencySlack =
      calculateSchedulingSlack(EntryBS.getTop(), SteadyTopBS, Context);
  DEBUG_BLOCKS(dbgs() << "  Preheader merge: LatencySlack=" << LatencySlack
                      << "\n");
  if (LatencySlack <= 0)
    return 0;

  // Resource check: use the first NumTransferred bundles of the steady
  // prologue as the source. Pass the same array for both prologue arguments
  // since the preheader has a single successor path.
  const auto &SteadyPrologue = SteadyTopBS.getTop().Bundles;
  ArrayRef<MachineBundle> TransferredSlice(SteadyPrologue.data(),
                                           NumTransferred);
  const int MaxLimit = std::min(static_cast<int>(NumTransferred), LatencySlack);
  const int ResourceMerge = checkResourceMergeability(
      EntryBundles, TransferredSlice, TransferredSlice, MaxLimit, HR,
      SelectedAltDescs);
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

  // Compute preheader merge count BEFORE transfer (needs full SteadyTopBS
  // to build inter-block DAG for latency slack computation).
  const unsigned PreheaderMerge =
      getPreheaderMergeCount(EntryBS, SteadyTopBS, NumBundles);

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

    DEBUG_BLOCKS(dbgs() << "optimizeSiblingLoops: Processing outer loop latch "
                        << BB->getNumber() << "\n");

    BlockState &EntryBS = IBS.getBlockState(OLS.OuterPreheader);
    BlockState &SteadyTopBS = IBS.getBlockState(OLS.SteadyTop);
    BlockState &LastIterTopBS = IBS.getBlockState(OLS.PeeledIterTop);

    // Verify BS is the epilogue (SteadyBottom)
    assert(BS.TheBlock == OLS.SteadyBottom && "BS must match OLS.SteadyBottom");

    // Validate all required regions and bundles exist
    assert(!SteadyTopBS.getRegions().empty() &&
           !LastIterTopBS.getRegions().empty() &&
           "Empty regions in prologue blocks");
    assert(!SteadyTopBS.getTop().Bundles.empty() &&
           !LastIterTopBS.getTop().Bundles.empty() && "Empty prologue bundles");
    assert(!BS.getRegions().empty() && "Empty epilogue region");

    tryMergePrologues(EntryBS, SteadyTopBS, BS, LastIterTopBS);
  }
}

} // namespace llvm::AIE
