//===--- AIEMultiSlotInstrMaterializer.cpp - -Multi Slot Instr materializer===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// \file assigns an issue slot to multi-slot pseudo instructions within a single
// block loop to help loop pipelining.
//
//===----------------------------------------------------------------------===//

#include "AIEMultiSlotInstrMaterializer.h"
#include "AIEHazardRecognizer.h"

using namespace llvm;

#define DEBUG_TYPE "aie-multi-slot-pseudo"

static cl::opt<bool> SkipSingleSlotAssignment(
    "aie-skip-single-slot-assignment", cl::Hidden, cl::init(true),
    cl::desc("Skip preassigning if all multi-slot instr are assigned to the "
             "same Slot."));
namespace llvm::AIE {

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
  bool assignSlot(const MachineInstr &MI, const AIEHazardRecognizer &HR) {
    auto MemBankBits = HR.getMemoryBanks(&MI);
    LLVM_DEBUG(dbgs() << "Memory Bank: " << MemBankBits << " " << MI);
    if (!MemBankBits) {
      LLVM_DEBUG(dbgs() << "Warning: No MemoryBanks assigned to " << MI);
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
  unsigned ReassignIndex = 0;
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
bool assignSlots(SlotMapping &SlotToBanks, const MachineBasicBlock &MBB,
                 const AIEBaseInstrInfo *TII, const AIEHazardRecognizer &HR) {
  LLVM_DEBUG(dbgs() << "Assigning Slots\n");
  for (const auto &MI : MBB) {
    if (!MI.mayLoad() || !TII->isMultiSlotPseudo(MI))
      continue;

    if (!SlotToBanks.assignSlot(MI, HR)) {
      return false;
    }
  }

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
                                                const AIEHazardRecognizer &HR) {
  LLVM_DEBUG(dbgs() << "Statically Assigning multi slot pseudos for "
                    << MBB.getName() << "\n");

  const AIEBaseInstrInfo *TII = static_cast<const AIEBaseInstrInfo *>(
      MBB.getParent()->getSubtarget().getInstrInfo());

  auto SlotToBanks = getAssignedSlots(MBB, TII, HR);

  if (!assignSlots(SlotToBanks, MBB, TII, HR)) {
    LLVM_DEBUG(
        dbgs()
        << "Could not find Slot Assignments, Skipping materialization\n");
    return;
  }

  materializeSlots(SlotToBanks, MBB, TII, HR);
}
} // namespace llvm::AIE
//
