//===- AIEScarceRegScheduling.cpp - Scarce Register Scheduling Strategy --===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This file implements a PostPipelinerStrategy that prioritizes scheduling
// decisions based on scarce register pressure.
//===----------------------------------------------------------------------===//

#include "AIEScarceRegScheduling.h"
#include "AIEBaseInstrInfo.h"
#include "AIERegDefUseTracker.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/CodeGen/TargetSchedule.h"

#define DEBUG_TYPE "scarce-reg-sched"

namespace llvm::AIE {

ScarceRange::ScarceRange(const RegLiveRange &LR, const ScheduleDAGInstrs &DAG)
    : LiveRange(LR) {
  // Collect all unique MachineInstr pointers from defs and uses.
  DenseSet<const MachineInstr *> UniqueInstrs;

  for (const auto &DefInfo : LR.defs()) {
    MachineOperand *const DefOp = DefInfo.getOperand();
    assert(DefOp && "DefOp should be valid");
    MachineInstr *const DefMI = DefOp->getParent();
    assert(DefMI && "Every operand should have a parent MachineInstr");
    UniqueInstrs.insert(DefMI);
  }

  for (const auto &UseInfo : LR.uses()) {
    MachineOperand *const UseOp = UseInfo.getOperand();
    assert(UseOp && "UseOp should be valid");
    MachineInstr *const UseMI = UseOp->getParent();
    assert(UseMI && "Every operand should have a parent MachineInstr");
    UniqueInstrs.insert(UseMI);
  }

  // Iterate over all SUnits and collect those whose instruction is in the set.
  // This handles the case where multiple SUnits reference the same instruction.
  // We only need the first (representative) SUnit for each instruction.
  for (const auto &SU : DAG.SUnits) {
    const MachineInstr *const MI = SU.getInstr();
    assert(MI && "Every SUnit should have a MachineInstr");
    if (UniqueInstrs.count(MI)) {
      Members.push_back(SU.NodeNum);
      // Early break when we've found all unique instructions.
      if (Members.size() == UniqueInstrs.size()) {
        break;
      }
    }
  }

  // Members are in SUnit order, which is deterministic.
}

ScarceRegScheduling::ScarceRegScheduling(ScheduleDAGInstrs &DAG,
                                         ScheduleInfo &Info,
                                         RegLiveRangeTracker &RegTracker,
                                         int II)
    : PostPipelinerStrategy(DAG, Info, /*LatestBias=*/0),
      RegTracker(RegTracker), II(II) {}

// BurstMostUrgentStrategy — design invariants
// ============================================
//
// Scarce register liveness:
//   Each ScarceRange holds the def and use instructions of a scarce physical
//   register (e.g. an accumulator). The members set is never empty.
//
// Topological order of SUnits:
//   SUnits are numbered such that every predecessor (via any dep kind) has a
//   strictly lower NodeNum than its successor. A single backward linear sweep
//   therefore suffices to compute any transitive predecessor set.
//
// Ordering-dependent partitioned ancestor set (computed in init()):
//   The strategy is called once per valid range ordering (one init() call per
//   ordering tried by enumerateRangeOrders()). For each ordering, init()
//   computes the partitioned ancestor set of each burst in a single backward
//   sweep.  The sweep seeds IsAncestor with the current range's own members
//   and stops at:
//     (a) members of OTHER scarce ranges (IsScarceRangeMember && !IsAncestor),
//         guaranteed to be scheduled before this range starts; and
//     (b) non-scarce nodes already claimed by an earlier range in this ordering
//         (AlreadyClaimed && !IsAncestor), whose own ancestors are also
//         claimed.
//   This directly produces the partitioned ancestor set without needing to
//   precompute or store full ancestor sets between init() calls.
//
// Admissible range ordering invariant:
//   enumerateRangeOrders() only produces orderings consistent with the
//   dependency DAG between scarce ranges. If a member of range S is a
//   predecessor of a member of range R, S appears before R in every ordering.
//   Consequently, when R's burst begins, all earlier ranges are fully
//   scheduled, including their members and all their ancestor nodes.
//   By the topological property, ancestors of a scheduled node are also
//   scheduled. Therefore no node claimed by an earlier range can be
//   unscheduled when R's burst starts.
//
// Intra-range dep exception:
//   A non-scarce instruction A may be sandwiched between two members of the
//   same range (e.g. def→A→use). A is included in the ancestor set (it is a
//   predecessor of the use member), but A depends on the def member, which is
//   not yet scheduled during the ancestors phase. In that case better() may
//   fall through to a non-ancestor. selected() handles this with an all_of
//   check rather than requiring strict membership.

BurstMostUrgentStrategy::BurstMostUrgentStrategy(
    ScheduleDAGInstrs &DAG, ScheduleInfo &Info,
    const std::vector<ScarceRange> &ScarceRanges, int LatestBias)
    : PostPipelinerStrategy(DAG, Info, LatestBias), ScarceRanges(ScarceRanges),
      CurrentSet(0) {

  assert(!ScarceRanges.empty() &&
         "BurstMostUrgentStrategy requires at least one scarce range");

  // Validate member indices.
  const size_t NumSUnits = Info.NInstr;
  for (const auto &Range : ScarceRanges) {
    for (int MemberIdx : Range.Members) {
      assert(MemberIdx >= 0 && static_cast<size_t>(MemberIdx) < NumSUnits &&
             "Scarce range member index out of bounds");
    }
  }

  // Pre-size OrderedMembers (will be populated by init()).
  OrderedMembers.resize(ScarceRanges.size() * 2);
}

void BurstMostUrgentStrategy::logBurstStart() const {
  assert(CurrentSet % 2 == 0 && CurrentSet < OrderedMembers.size() &&
         "logBurstStart called at invalid CurrentSet");
  const size_t BurstIdx = CurrentSet / 2;
  dbgs() << "Current burst " << BurstIdx << ": ";
  for (size_t SubSet = CurrentSet; SubSet <= CurrentSet + 1; ++SubSet) {
    if (SubSet > CurrentSet)
      dbgs() << " -> ";
    dbgs() << "[";
    llvm::interleaveComma(OrderedMembers[SubSet], dbgs());
    dbgs() << "]";
  }
  dbgs() << "\n";
}

void BurstMostUrgentStrategy::skipEmptySets() {
  // Only ancestors sets (even-indexed) can be empty — members sets (odd) are
  // guaranteed non-empty by construction (every scarce range has members). So
  // at most one set needs to be skipped.
  if (CurrentSet < OrderedMembers.size() &&
      OrderedMembers[CurrentSet].empty()) {
    assert(CurrentSet % 2 == 0 &&
           "Members sets (odd-indexed) must be non-empty");
    LLVM_DEBUG(
        dbgs() << format("Skipping empty ancestors set %zu\n", CurrentSet));
    ++CurrentSet;
    // After skipping an empty ancestors set we land on a members set, which
    // must be non-empty.
    assert(CurrentSet < OrderedMembers.size() &&
           !OrderedMembers[CurrentSet].empty() &&
           "Members set after empty ancestors set must be non-empty");
  }
  // Members sets (odd-indexed) are always non-empty.
  assert((CurrentSet >= OrderedMembers.size() ||
          !OrderedMembers[CurrentSet].empty()) &&
         "Current set must be non-empty");
}

void BurstMostUrgentStrategy::init(const SmallVector<int, 4> &RangeOrder) {
  assert(RangeOrder.size() == ScarceRanges.size() &&
         "RangeOrder must have the same size as ScarceRanges");

  // Reset state.
  CurrentSet = 0;

  // Build a bit vector of all scarce range members.
  const size_t NumSUnits = Info.NInstr;
  BitVector IsScarceRangeMember(NumSUnits, false);
  for (const auto &Range : ScarceRanges)
    for (int M : Range.Members)
      IsScarceRangeMember.set(M);

  // For each range in the given ordering, compute its partitioned ancestor set
  // directly via a single backward linear sweep. The sweep stops at:
  //   (a) members of other scarce ranges (they are scheduled in their own
  //   burst) (b) nodes already claimed by an earlier range (their ancestors are
  //   also
  //       claimed and thus already scheduled)
  // Current range members are seeds and are traversed through. The check
  //   (IsScarceRangeMember[P] || AlreadyClaimed[P]) && !IsAncestor[P]
  // distinguishes stopping cases from the traversal-through case.
  BitVector AlreadyClaimed(NumSUnits, false);

  for (size_t I = 0; I < RangeOrder.size(); ++I) {
    const int RangeIdx = RangeOrder[I];
    const auto &Range = ScarceRanges[RangeIdx];

    // Seed IsAncestor with the current range's members.
    BitVector IsAncestor(NumSUnits, false);
    for (int M : Range.Members)
      IsAncestor.set(M);

    // Backward sweep: collect partitioned ancestors.
    SmallVector<int, 4> PartitionedPreds;
    for (int K = static_cast<int>(NumSUnits) - 1; K >= 0; --K) {
      if (!IsAncestor.test(K))
        continue;
      for (const auto &Dep : DAG.SUnits[K].Preds) {
        const SUnit *PredSU = Dep.getSUnit();
        if (!PredSU || PredSU->isBoundaryNode())
          continue;
        const int PredIdx = static_cast<int>(PredSU->NodeNum);
        if (PredIdx < 0 || static_cast<size_t>(PredIdx) >= NumSUnits)
          continue;
        if (IsAncestor.test(PredIdx))
          continue;
        // Stop at other scarce range members or already-claimed nodes.
        // Exception: current range's members are seeded (IsAncestor) and
        // must be traversed through to reach their non-scarce predecessors.
        if (IsScarceRangeMember.test(PredIdx) || AlreadyClaimed.test(PredIdx))
          continue;
        IsAncestor.set(PredIdx);
        PartitionedPreds.push_back(PredIdx);
        AlreadyClaimed.set(PredIdx);
      }
    }

    // Mark this range's members as claimed after their ancestors.
    for (int M : Range.Members)
      AlreadyClaimed.set(M);

    OrderedMembers[2 * I] = std::move(PartitionedPreds);
    OrderedMembers[2 * I + 1] = Range.Members;
  }

  // Log the first burst.
  if (!OrderedMembers.empty()) {
    LLVM_DEBUG(logBurstStart());
  }

  // Find the first non-empty set.
  skipEmptySets();
}

bool BurstMostUrgentStrategy::better(const SUnit &A, const SUnit &B) {
  const int AIdx = A.NodeNum;
  const int BIdx = B.NodeNum;

  // Check if either is in the current set.
  if (CurrentSet < OrderedMembers.size()) {
    const auto &CurrentMembers = OrderedMembers[CurrentSet];
    const bool AInSet = std::find(CurrentMembers.begin(), CurrentMembers.end(),
                                  AIdx) != CurrentMembers.end();
    const bool BInSet = std::find(CurrentMembers.begin(), CurrentMembers.end(),
                                  BIdx) != CurrentMembers.end();

    // Prefer members of the current set.
    if (AInSet != BInSet) {
      return AInSet;
    }
  }

  // Default: prefer earlier earliest.
  return Info[AIdx].Earliest < Info[BIdx].Earliest;
}

void BurstMostUrgentStrategy::selected(const SUnit &N) {
  if (CurrentSet >= OrderedMembers.size())
    return; // All sets done; free scheduling.

  // The intent is to give priority to scarce ranges and schedule them without
  // interruption. Ideally every selected() call delivers a member of the
  // current set. However, a non-scarce ancestor A may itself depend on another
  // member of the same range (e.g. def→A→use), making A unavailable during
  // the ancestors phase. In that case better() falls through to a non-ancestor.
  // We therefore check whether all current-set members are now scheduled
  // (counting N as already scheduled) rather than requiring strict membership.
  const auto &CurrentMembers = OrderedMembers[CurrentSet];
  const bool AllScheduled =
      llvm::all_of(CurrentMembers, [this, &N](int MemberIdx) {
        return Info[MemberIdx].Scheduled ||
               MemberIdx == static_cast<int>(N.NodeNum);
      });

  if (AllScheduled) {
    ++CurrentSet;
    LLVM_DEBUG(dbgs() << format("Completed set %zu, advancing to %zu\n",
                                CurrentSet - 1, CurrentSet));

    // If we just advanced to a predecessors set (even index), a new burst
    // is starting.
    if (CurrentSet % 2 == 0 && CurrentSet < OrderedMembers.size())
      LLVM_DEBUG(logBurstStart());

    // If we just completed a members set (odd index), simulate
    // anti-dependences.
    if ((CurrentSet - 1) % 2 == 1) {
      const size_t BurstIdx = (CurrentSet - 1) / 2;
      const int RangeIdx =
          (BurstIdx < ScarceRanges.size()) ? static_cast<int>(BurstIdx) : -1;
      if (RangeIdx >= 0)
        simulateAntiDependences(RangeIdx);
    }

    // Skip any empty sets.
    skipEmptySets();
  }
}

void BurstMostUrgentStrategy::simulateAntiDependences(int CompletedRangeIdx) {
  const auto &CompletedRange = ScarceRanges[CompletedRangeIdx];
  const auto *const SchedModel = DAG.getSchedModel();
  const auto *const TII =
      static_cast<const AIEBaseInstrInfo *>(SchedModel->getInstrInfo());
  const auto *const ItinData = SchedModel->getInstrItineraries();

  LLVM_DEBUG(dbgs() << format("Simulating anti-dependences for range %d\n",
                              CompletedRangeIdx));

  // For each Use in the completed range's LiveRange.
  for (const auto &UseInfo : CompletedRange.LiveRange.uses()) {
    MachineOperand *const UseOp = UseInfo.getOperand();
    assert(UseOp && "UseOp should be valid");
    MachineInstr *const UseMI = UseOp->getParent();
    assert(UseMI && "Every operand should have a parent MachineInstr");

    const unsigned UseOpIdx = UseOp->getOperandNo();

    // Find the corresponding SUnit index.
    int UseSUIdx = -1;
    for (const int MemberIdx : CompletedRange.Members) {
      if (DAG.SUnits[MemberIdx].getInstr() == UseMI) {
        UseSUIdx = MemberIdx;
        break;
      }
    }
    assert(UseSUIdx >= 0 && "Use instruction should be in completed range");

    const int UseCycle = Info[UseSUIdx].Cycle;

    // For each subsequent range.
    for (size_t LaterRangeIdx = CompletedRangeIdx + 1;
         LaterRangeIdx < ScarceRanges.size(); ++LaterRangeIdx) {
      const auto &LaterRange = ScarceRanges[LaterRangeIdx];

      // For each Def in the later range's LiveRange.
      for (const auto &DefInfo : LaterRange.LiveRange.defs()) {
        MachineOperand *const DefOp = DefInfo.getOperand();
        assert(DefOp && "DefOp should be valid");
        MachineInstr *const DefMI = DefOp->getParent();
        assert(DefMI && "Every operand should have a parent MachineInstr");

        const unsigned DefOpIdx = DefOp->getOperandNo();

        // Find the corresponding SUnit index.
        int DefSUIdx = -1;
        for (const int MemberIdx : LaterRange.Members) {
          if (DAG.SUnits[MemberIdx].getInstr() == DefMI) {
            DefSUIdx = MemberIdx;
            break;
          }
        }
        assert(DefSUIdx >= 0 && "Def instruction should be in later range");

        // Compute the anti-dependence latency using the signed variant to
        // support negative latencies.
        const std::optional<int> Latency = TII->getSignedOperandLatency(
            ItinData, *UseMI, UseOpIdx, *DefMI, DefOpIdx, SDep::Anti);
        if (!Latency)
          continue;

        // Update Earliest[Def] = max(Earliest[Def], Cycle[Use] + L).
        const int NewEarliest = UseCycle + *Latency;
        Info[DefSUIdx].Earliest =
            std::max(Info[DefSUIdx].Earliest, NewEarliest);
      }
    }
  }
}

void buildScarceRangeMapping(const std::vector<ScarceRange> &Ranges,
                             const ScheduleInfo &Info,
                             std::vector<int> &RangeOfSUnit) {
  RangeOfSUnit.assign(Info.NInstr, -1);

  for (size_t RangeIdx = 0; RangeIdx < Ranges.size(); ++RangeIdx) {
    const auto &Range = Ranges[RangeIdx];
    for (int MemberIdx : Range.Members) {
      assert(MemberIdx >= 0 && MemberIdx < Info.NInstr &&
             "Scarce range member index out of bounds");
      assert(RangeOfSUnit[MemberIdx] == -1 &&
             "SUnit cannot belong to multiple scarce ranges");
      RangeOfSUnit[MemberIdx] = RangeIdx;
    }
  }
}

void buildScarceDAG(std::vector<ScarceRange> &Ranges, const ScheduleInfo &Info,
                    const ScheduleDAGInstrs &DAG) {
  // Build the mapping from SUnit to range index.
  std::vector<int> RangeOfSUnit;
  buildScarceRangeMapping(Ranges, Info, RangeOfSUnit);

  // Populate PredRanges for each range using direct predecessors from the DAG.
  for (size_t RangeIdx = 0; RangeIdx < Ranges.size(); ++RangeIdx) {
    auto &Range = Ranges[RangeIdx];
    Range.PredRanges.clear();

    // Use a small set to deduplicate predecessor ranges.
    SmallVector<int, 4> PredSet;

    // For each member of this range.
    for (int MemberIdx : Range.Members) {
      assert(MemberIdx >= 0 && MemberIdx < Info.NInstr &&
             "Scarce range member index out of bounds");

      const auto &SU = DAG.SUnits[MemberIdx];

      // For each direct predecessor of this member.
      for (const auto &PredEdge : SU.Preds) {
        const SUnit *PredSU = PredEdge.getSUnit();
        if (!PredSU || PredSU->isBoundaryNode()) {
          continue;
        }

        const int PredIdx = PredSU->NodeNum;
        const int PredRange = RangeOfSUnit[PredIdx];

        // If the predecessor is in a different scarce range, record the edge.
        if (PredRange != -1 && PredRange != static_cast<int>(RangeIdx)) {
          // Add to PredSet if not already present.
          if (std::find(PredSet.begin(), PredSet.end(), PredRange) ==
              PredSet.end()) {
            PredSet.push_back(PredRange);
          }
        }
      }
    }

    // Copy deduplicated predecessors to PredRanges.
    Range.PredRanges = PredSet;
  }
}

bool checkAcyclic(const std::vector<ScarceRange> &Ranges) {
  const size_t K = Ranges.size();

  // Compute indegrees (PredRanges.size() for each range).
  SmallVector<unsigned, 4> Indegree;
  Indegree.reserve(K);
  for (const auto &Range : Ranges) {
    Indegree.push_back(Range.PredRanges.size());
  }

  // Kahn's algorithm: process ranges with indegree 0.
  SmallVector<int, 4> Ready;
  for (size_t I = 0; I < K; ++I) {
    if (Indegree[I] == 0) {
      Ready.push_back(I);
    }
  }

  unsigned ProcessedCount = 0;
  while (!Ready.empty()) {
    const int Current = Ready.pop_back_val();
    ++ProcessedCount;

    // For each range that has Current as a predecessor, decrement indegree.
    for (size_t J = 0; J < K; ++J) {
      const auto &Range = Ranges[J];
      if (std::find(Range.PredRanges.begin(), Range.PredRanges.end(),
                    Current) != Range.PredRanges.end()) {
        --Indegree[J];
        if (Indegree[J] == 0) {
          Ready.push_back(J);
        }
      }
    }
  }

  // If we processed all ranges, the DAG is acyclic.
  return ProcessedCount == K;
}

bool enumerateRangeOrders(
    const std::vector<ScarceRange> &Ranges,
    llvm::function_ref<bool(const SmallVector<int, 4> &Order)> OnOrder) {

  const size_t K = Ranges.size();

  // Track which ranges have been placed in the current order.
  SmallVector<bool, 4> Placed(K, false);

  // Current partial order being built.
  SmallVector<int, 4> Order;
  Order.reserve(K);

  // Recursive DFS to enumerate linear extensions.
  const auto Enumerate = [&](auto &EnumerateRef) -> bool {
    // Base case: complete order found.
    if (Order.size() == K) {
      LLVM_DEBUG(dbgs() << "\nEntering burst scheduling with order ";
                 for (auto Ord : Order) { dbgs() << Ord << ", "; } dbgs()
                 << "\n";);
      return OnOrder(Order);
    }

    // Find ready ranges (all predecessors are in Order).
    for (size_t RangeIdx = 0; RangeIdx < K; ++RangeIdx) {
      if (Placed[RangeIdx]) {
        continue;
      }

      const auto &Range = Ranges[RangeIdx];

      // Check if all predecessors are placed.
      const bool AllPredsPlaced = llvm::all_of(
          Range.PredRanges, [&Placed](int PredIdx) { return Placed[PredIdx]; });

      if (AllPredsPlaced) {
        // This range is ready; add it to the order and recurse.

        Order.push_back(RangeIdx);
        Placed[RangeIdx] = true;

        if (EnumerateRef(EnumerateRef)) {
          return true;
        }

        // Backtrack.
        Placed[RangeIdx] = false;
        Order.pop_back();
      }
    }

    return false;
  };

  LLVM_DEBUG(dbgs() << "Enumerating scarce ranges\n");

  return Enumerate(Enumerate);
}

} // namespace llvm::AIE
