//===--- AIEMultiSlotInstrMaterializer.cpp - -Multi Slot Instr materializer===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// \file assigns an issue slot to multi-slot pseudo instructions within a single
// block loop to help loop pipelining.
//
//===----------------------------------------------------------------------===//

#include "AIEMultiSlotInstrMaterializer.h"
#include "AIEHazardRecognizer.h"
#include "AIESlotStatistics.h"
#include "AIESlotUtils.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOptimizationRemarkEmitter.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "aie-multi-slot-pseudo"

static cl::opt<bool> SkipSingleSlotAssignment(
    "aie-skip-single-slot-assignment", cl::Hidden, cl::init(true),
    cl::desc("Skip preassigning if all multi-slot instr are assigned to the "
             "same Slot."));

namespace llvm::AIE {

/// Emit a missed-optimization remark for a load without a memory bank.
static void reportMissingMemoryBank(MachineInstr &MI) {
  MachineBasicBlock *MBB = MI.getParent();
  MachineFunction &MF = *MBB->getParent();

  MachineOptimizationRemarkEmitter MORE(MF, nullptr);
  MORE.emit([&]() {
    return MachineOptimizationRemarkMissed(DEBUG_TYPE, "missing-memory-bank",
                                           MI.getDebugLoc(), MBB)
           << ore::MNV("Instruction", MI);
  });
}

class SlotMapping {
public:
  SlotMapping(const AIEBaseInstrInfo *TII) : TII(TII) {}

  /// update \p MemBankBits assigned to \p Slot . Create the Slot mapping, if
  /// necessary.
  void update(const MCSlotKind &Slot, const MemoryBankBits MemBankBits) {
    SlotToBanks[Slot] |= MemBankBits;
  }

  /// \return first Slot where MemoryBankBits overlap with \p MemBankBits .
  std::optional<MCSlotKind>
  getAssignedSlot(const MemoryBankBits MemBankBits) const {
    auto IT =
        find_if(SlotToBanks,
                [MemBankBits](
                    const std::pair<MCSlotKind, MemoryBankBits> &SlotBankPair) {
                  auto [Slot, Banks] = SlotBankPair;
                  return (Banks & MemBankBits) != 0;
                });

    if (IT == SlotToBanks.end())
      return {};

    const auto Slot = IT->first;
    return Slot;
  }

  /// \return whether no MemoryBank has multiple Slots assigned to it in the
  /// current mapping.
  bool hasUniqueSlotForBank() const {
    MemoryBankBits AccumulatedBanks = {};
    for (auto &[Slot, Banks] : SlotToBanks) {
      if (Banks & AccumulatedBanks) {
        LLVM_DEBUG(dbgs() << "Conflict detected at Slot " << Slot << "\n");
        return false;
      }
      AccumulatedBanks |= Banks;
    }
    return true;
  }

  /// \return whether a Slot can be assigned to \b MI and assign it in the
  /// mapping.
  bool isSlotAssignable(MachineInstr &MI, const AIEHazardRecognizer &HR) {
    auto MemBankBits = HR.getMemoryBanks(&MI);
    LLVM_DEBUG(dbgs() << "Memory Bank: " << MemBankBits << " " << MI);
    if (!MemBankBits) {
      reportMissingMemoryBank(MI);
      return false;
    }

    std::optional<MCSlotKind> SelectedSlot = getAssignedSlot(MemBankBits);
    if (!SelectedSlot)
      SelectedSlot = getUnusedLoadSlot();
    if (!SelectedSlot) {
      LLVM_DEBUG(dbgs() << "Reassigning existing Slot to MemoryBankBits "
                        << MemBankBits << "\n");
      SelectedSlot = getLeastRecentlyUsedSlot();
    }

    update(*SelectedSlot, MemBankBits);

    return true;
  }

  bool hasSingleMapping() const { return SlotToBanks.size() == 1; }

private:
  /// Mapping between a Slot and the MemoryBanks that occupy the Slot.
  std::map<MCSlotKind, MemoryBankBits> SlotToBanks;
  /// If Slots have to be reassigned (because every Slot has already been
  /// assigned to a Memory Bank), use an Index to cycle through already
  /// used Slots.
  /// The initial value is tuned for specific benchmarks for aie2p architecture.
  unsigned ReassignIndex = 1;
  const AIEBaseInstrInfo *TII;

  /// \return an unused Slot from the mapping.
  std::optional<MCSlotKind> getUnusedLoadSlot() const {
    const SmallVector<MCSlotKind, 2> LoadSlots =
        TII->getFormatInterface()->getLoadSlotKinds();

    for (const auto &Slot : LoadSlots) {

      // check if Slot is already used in SlotMemBankBitsMap
      auto FoundSlot = SlotToBanks.find(Slot);
      if (FoundSlot != SlotToBanks.end())
        continue;

      LLVM_DEBUG(dbgs() << "    Found Unused Slot " << Slot << "\n");
      return Slot;
    }
    return {};
  }

  /// Cycle through load Slots and \return an already used Slot
  /// FIXME: use a heuristic that takes utilization into account, instead of
  /// blindly cycling through the Slots.
  std::optional<MCSlotKind> getLeastRecentlyUsedSlot() {
    const auto AvailableSlots = TII->getFormatInterface()->getLoadSlotKinds();

    if (ReassignIndex >= AvailableSlots.size())
      ReassignIndex = 0;

    return AvailableSlots[ReassignIndex++];
  }
};

/// \return a map between Slots and the MemoryBanks that occurs within \p MBB .
SlotMapping getAssignedSlots(const MachineBasicBlock &MBB,
                             const AIEBaseInstrInfo *TII,
                             const AIEHazardRecognizer &HR) {
  SlotMapping SlotToBanks(TII);

  LLVM_DEBUG(dbgs() << "Collecting any already materialized Slot to MemoryBank "
                       "assignments\n");
  for (const auto &MI : MBB) {
    if (!MI.mayLoad() || TII->isMultiSlotPseudo(MI))
      continue;

    const auto Slot = TII->getSlotKind(MI.getOpcode());
    const MemoryBankBits MemBankBits = HR.getMemoryBanks(&MI);
    LLVM_DEBUG(dbgs() << "Slot: " << Slot << " MemoryBank: " << MemBankBits
                      << " on " << MI);

    SlotToBanks.update(Slot, MemBankBits);
  }
  return SlotToBanks;
}

/// \return whether a valid assignment of Slots to MemoryBankBits is found.
/// Multi-Slot pseudo load instructions in \p MBB get a Slot assigned, according
/// to the MemoyBankBits that is attached to the MachineInstr. Existing mappings
/// in \p SlotToBanks are used and updated.
bool assignSlots(SlotMapping &SlotToBanks, MachineBasicBlock &MBB,
                 const AIEBaseInstrInfo *TII, const AIEHazardRecognizer &HR) {
  LLVM_DEBUG(dbgs() << "Assigning Slots\n");
  // Visit every load so each missing-memory-bank gets its own remark.
  bool AllAssignable = true;
  for (auto &MI : MBB) {
    if (!MI.mayLoad() || !TII->isMultiSlotPseudo(MI))
      continue;

    if (!SlotToBanks.isSlotAssignable(MI, HR))
      AllAssignable = false;
  }
  if (!AllAssignable)
    return false;

  const bool SingleSlotAssignment = SlotToBanks.hasSingleMapping();
  if (SingleSlotAssignment)
    LLVM_DEBUG(
        dbgs()
        << "WARNING: Only a single Slot was assigned to all multi-slot Load "
           " Instructions.\n");

  if (!SlotToBanks.hasUniqueSlotForBank())
    return false;

  if (SkipSingleSlotAssignment)
    return !SingleSlotAssignment;

  return true;
}

namespace {

/// Apply round-robin slot assignment to instructions selected by
/// isRoundRobinSlotCandidate, ensuring consecutive candidates land on
/// different slots.
///
/// \return true if at least two candidates were assigned to different slots.
bool applyRoundRobinSlotAssignment(MachineBasicBlock &MBB,
                                   const AIEBaseInstrInfo *TII) {
  const AIEBaseMCFormats *Formats = TII->getFormatInterface();
  SmallVector<MachineInstr *, 4> Candidates;
  for (auto &MI : MBB) {
    if (TII->isRoundRobinSlotCandidate(MI))
      Candidates.push_back(&MI);
  }

  if (Candidates.size() < 2)
    return false;

  LLVM_DEBUG(dbgs() << "Applying round-robin slot assignment to "
                    << Candidates.size() << " candidates\n");

  MCSlotKind LastSlot;
  for (auto *MI : Candidates) {
    const auto *Alts = Formats->getAlternateInstsOpcode(MI->getOpcode());
    assert(Alts && Alts->size() >= 2 &&
           "Round-robin candidates must have at least two alternatives");
    unsigned ChosenOpc = Alts->front();
    for (const unsigned AltOpc : *Alts) {
      const MCSlotKind AltSlot = Formats->getSlotKind(AltOpc);
      const bool IsDifferentSlot = AltSlot != LastSlot;
      if (IsDifferentSlot) {
        ChosenOpc = AltOpc;
        break;
      }
    }
    LastSlot = Formats->getSlotKind(ChosenOpc);
    LLVM_DEBUG(dbgs() << "Materializing: " << *MI);
    MI->setDesc(TII->get(ChosenOpc));
    LLVM_DEBUG(dbgs() << "          to: " << *MI);
  }

  return true;
}

void materializeMSP(MachineInstr *MSP, SlotStatistics &Statistics,
                    const SlotCounts &NeighborFixed,
                    const AIEBaseInstrInfo *TII) {
  const SlotCounts Counts = Statistics.MSPSlotCounts[MSP];
  const AIEBaseMCFormats *Formats = TII->getFormatInterface();
  const auto *Alternatives = Formats->getAlternateInstsOpcode(MSP->getOpcode());
  assert(Alternatives && !Alternatives->empty());

  auto SlotOf = [Formats](unsigned Opc) {
    return static_cast<int>(Formats->getSlotKind(Opc));
  };

  // Select the slot with lower pressure. Fixed and NeighborFixed are summed
  // for a combined comparison. Free is the final tiebreaker.
  auto Better = [&](int A, int B) -> bool {
    const int CombinedA = Statistics.Fixed.at(A) + NeighborFixed.at(A);
    const int CombinedB = Statistics.Fixed.at(B) + NeighborFixed.at(B);
    if (CombinedA != CombinedB)
      return CombinedA < CombinedB;
    return Statistics.Free.at(A) < Statistics.Free.at(B);
  };

  // Iterate only over the primary slots of the MSP's alternatives.
  // With conflict-set-based statistics, secondary (conflict) indices
  // would also appear non-zero in Counts, but they are not valid
  // materialization targets.
  unsigned BestOpcode = Alternatives->front();
  int BestSlot = SlotOf(BestOpcode);
  for (const unsigned AltOpcode : *Alternatives) {
    const int S = SlotOf(AltOpcode);
    if (Better(S, BestSlot)) {
      BestSlot = S;
      BestOpcode = AltOpcode;
    }
  }

  LLVM_DEBUG(dbgs() << "Materializing " << *MSP);

  MSP->setDesc(TII->get(BestOpcode));

  // Update statistics: add conflict-set counts for the materialized opcode,
  // and remove the MSP's Free contribution.
  Statistics.Fixed += SlotStatistics::Unit * getConflictCounts(BestOpcode, TII);
  Statistics.Free -= Counts;

  LLVM_DEBUG(dbgs() << "           to " << *MSP);
  LLVM_DEBUG(dbgs() << "New Fixed:\n" << Statistics.Fixed << "\n");
}

/// A slot has reached its fair share when:
///
///   Fixed(S) * NumAlternatives >= InitialTotal(S)
///
/// where InitialTotal = Fixed + Free (precomputed before materialization).
/// This checks whether the current Fixed count, scaled by the number of
/// alternatives, meets or exceeds the initial total for that slot -- i.e.,
/// whether this slot has received its proportional share of instructions.
///
/// \return whether any alternative slot of the current MSP has reached its
/// fair-share threshold. The threshold is computed from the initial (pre-
/// materialization) statistics to provide a stable target, while the
/// comparison uses the current Fixed counts which grow as MSPs are assigned.
bool hasAlternativeReachedThreshold(const SlotStatistics &Statistics,
                                    const SlotCounts &InitialTotal,
                                    ArrayRef<unsigned> Alternatives,
                                    const AIEBaseMCFormats *Formats) {
  const int N = Alternatives.size();
  for (const unsigned AltOpc : Alternatives) {
    const int S = static_cast<int>(Formats->getSlotKind(AltOpc));
    const int ScaledFixed = Statistics.Fixed.at(S) * N;
    const bool HasLoopBodyPressure = InitialTotal.at(S) > 0;
    const bool ReachedFairShare = ScaledFixed >= InitialTotal.at(S);
    if (HasLoopBodyPressure && ReachedFairShare)
      return true;
  }
  return false;
}

/// Remove neighbor bias for slots that have reached their fair-share threshold.
///
/// When any alternative slot of the current MSP reaches its threshold, bias is
/// removed for ALL alternative slots of that MSP. This prevents a deadlock
/// where the biased slot never accumulates Fixed because instructions are
/// pushed away from it: once one slot has its fair share, the remaining MSPs
/// should balance based on loop body pressure alone.
void removeReachedNeighborBias(const SlotStatistics &Statistics,
                               SlotCounts &NeighborFixed,
                               const SlotCounts &InitialTotal,
                               ArrayRef<unsigned> Alternatives,
                               const AIEBaseMCFormats *Formats) {
  // Remove bias for slots with no loop body pressure.
  for (int S = 0; S < NeighborFixed.size(); ++S) {
    const bool HasBias = NeighborFixed.at(S) != 0;
    const bool HasLoopBodyPressure = InitialTotal.at(S) != 0;
    if (!HasBias || HasLoopBodyPressure)
      continue;
    LLVM_DEBUG(dbgs() << "Removing neighbor bias for slot " << S
                      << " (no loop body pressure)"
                      << " [Fixed=" << Statistics.Fixed.at(S) << "]\n");
    NeighborFixed[S] = 0;
  }

  if (!hasAlternativeReachedThreshold(Statistics, InitialTotal, Alternatives,
                                      Formats)) {
    return;
  }

  // Remove bias for ALL alternative slots so remaining MSPs can balance.
  const int N = Alternatives.size();
  for (const unsigned AltOpc : Alternatives) {
    const int S = static_cast<int>(Formats->getSlotKind(AltOpc));
    const bool HasBias = NeighborFixed.at(S) != 0;
    if (!HasBias)
      continue;
    LLVM_DEBUG(dbgs() << "Removing neighbor bias for slot " << S
                      << " (alternative reached fair share)"
                      << " [Fixed=" << Statistics.Fixed.at(S)
                      << " InitialTotal=" << InitialTotal.at(S)
                      << " NumAlternatives=" << N << "]\n");
    NeighborFixed[S] = 0;
  }
}

void materializeToMinimizeSlotTotals(MachineBasicBlock &MBB,
                                     const AIEBaseInstrInfo *TII) {
  SlotStatistics Statistics = computeSlotStatistics(MBB, TII);
  LLVM_DEBUG(dbgs() << "Materializing multi-slot pseudos for " << MBB.getName()
                    << "\n");

  SlotCounts NeighborFixed;
  // Precompute the initial total slot pressure (Fixed + Free) before
  // materialization. This provides a stable baseline for the fair-share
  // threshold that does not shift as MSPs are materialized.
  const SlotCounts InitialTotal = Statistics.Fixed + Statistics.Free;
  if (const auto *Preheader =
          AIELoopUtils::getDedicatedFallThroughPreheader(MBB)) {
    const SlotStatistics PreheaderStats =
        computeSlotStatistics(const_cast<MachineBasicBlock &>(*Preheader), TII);
    NeighborFixed = PreheaderStats.Fixed;
    LLVM_DEBUG(dbgs() << "Neighbor Fixed:\n  " << NeighborFixed << "\n");
  }

  const AIEBaseMCFormats *Formats = TII->getFormatInterface();
  // Sort the list by increasing alternative count
  llvm::sort(Statistics.MSPs, [Formats](MachineInstr *A, MachineInstr *B) {
    const auto *AltA = Formats->getAlternateInstsOpcode(A->getOpcode());
    const auto *AltB = Formats->getAlternateInstsOpcode(B->getOpcode());
    return AltA->size() < AltB->size();
  });
  LLVM_DEBUG(dbgs() << "Statistics:\n");
  LLVM_DEBUG(Statistics.dump());
  LLVM_DEBUG(dbgs() << "----\n");

  for (auto *MSP : Statistics.MSPs) {
    const auto *Alternatives =
        Formats->getAlternateInstsOpcode(MSP->getOpcode());
    materializeMSP(MSP, Statistics, NeighborFixed, TII);
    if (Alternatives)
      removeReachedNeighborBias(Statistics, NeighborFixed, InitialTotal,
                                *Alternatives, Formats);
  }
}

} // namespace

/// Materialise \p MI into its slot assigned by \p SlotToBanks .
void materializeInstr(MachineInstr &MI, const SlotMapping &SlotToBanks,
                      const AIEBaseInstrInfo *TII,
                      const AIEHazardRecognizer &HR) {
  auto MemBankBits = HR.getMemoryBanks(&MI);
  assert(MemBankBits && "No MemoryBanks attached to MachineInstr.");

  const auto Slot = SlotToBanks.getAssignedSlot(MemBankBits);
  assert(Slot && "Could not find Slot for MemoryBank!");

  auto OpCode = TII->getSlotOpcode(*Slot, MI);
  assert(OpCode && "Failed to retrieve a valid Opcode");

  MI.setDesc(TII->get(*OpCode));
  LLVM_DEBUG(dbgs() << "Assigned " << *Slot << " to " << MI);
}

/// Materialize multi-slot pseudo instructions in \p MBB according to
/// overlapping MemoryBankBits between a MachineInstr and the Slot mapping in
/// \p SlotToBanks .
void materializeSlots(const SlotMapping &SlotToBanks, MachineBasicBlock &MBB,
                      const AIEBaseInstrInfo *TII,
                      const AIEHazardRecognizer &HR) {
  LLVM_DEBUG(dbgs() << "\nAssigning Slots to MachineInstr\n");

  for (auto &MI : MBB) {
    if (!MI.mayLoad() || !TII->isMultiSlotPseudo(MI))
      continue;

    materializeInstr(MI, SlotToBanks, TII, HR);
  }
}

void staticallyMaterializeMultiSlotInstructions(MachineBasicBlock &MBB,
                                                const AIEHazardRecognizer &HR,
                                                bool MaterializePipeline) {
  LLVM_DEBUG(dbgs() << "Statically Assigning multi slot pseudos for "
                    << MBB.getName() << "\n");

  const AIEBaseInstrInfo *TII = static_cast<const AIEBaseInstrInfo *>(
      MBB.getParent()->getSubtarget().getInstrInfo());

  auto SlotToBanks = getAssignedSlots(MBB, TII, HR);

  if (assignSlots(SlotToBanks, MBB, TII, HR)) {
    materializeSlots(SlotToBanks, MBB, TII, HR);
  } else {
    LLVM_DEBUG(
        dbgs()
        << "Could not find slot assignments, skipping bank materialization\n");
    // Apply round-robin slot assignment to eligible pseudos. Remaining
    // multi-slot pseudos are left for the pipeliner or MaterializePipeline.
    applyRoundRobinSlotAssignment(MBB, TII);
  }

  if (MaterializePipeline) {
    materializeToMinimizeSlotTotals(MBB, TII);
  }
}
} // namespace llvm::AIE
//
