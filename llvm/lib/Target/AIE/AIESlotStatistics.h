//===--- AIESlotStatistics.h ---------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines a class representing the slot statistics of instructions,
// including multi-slot pseudos.
// Additionally, it defines some utilities
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIESLOTSTATISTICS_H
#define LLVM_LIB_TARGET_AIE_AIESLOTSTATISTICS_H

#include "AIESlotCounts.h"
#include "llvm/ADT/SmallVector.h"

namespace llvm {
class AIEBaseMCFormats;
struct AIEBaseInstrInfo;
class MachineBasicBlock;
class MachineInstr;
} // namespace llvm

namespace llvm::AIE {

class SlotStatistics {
public:
  SlotStatistics() = default;
  SlotStatistics(ArrayRef<int> Fixed, ArrayRef<int> Free);

  // Q: Why do hours have 60 seconds?
  // A: Because it has 1, 2, 3, 4, 5 and 6 as divisors.
  static const int Unit = 60;

  // The total of the fixed slots
  SlotCounts Fixed;

  // The total of the unmaterialized slots
  SlotCounts Free;

  // Set of MultiSlotPseudos in priority order. Note that they are first
  // collected, then sorted.
  std::vector<MachineInstr *> MSPs;

  // The slot counts of each MSP
  std::unordered_map<MachineInstr *, SlotCounts> MSPSlotCounts;

  // Add an instruction to the statistics. Slot demand is counted in units of
  // 1/Unit of an instruction. A non-multi-slot instruction adds Unit to every
  // slot of its conflict set (Fixed). A multi-slot pseudo adds Unit in total,
  // spread uniformly over its alternatives, each adding its share to every
  // slot of its conflict set (Free).
  void addInstruction(MachineInstr &MI, const AIEBaseInstrInfo *TII);

  // The number of represented slots
  int size() const { return std::max(Fixed.size(), Free.size()); }

  // Compute the distance, the sum of absolute diffs of all components of
  // Free and Fixed
  int distance(const SlotStatistics &Other) const;

  // Computes the minimum initiation interval based on the slot occupation.
  // Multislot pseudos add their alternatives as uniform probabilities.
  int getMinII() const;

  // print the value on dbgs()
  void dump() const;

  // print the value in a one-liner
  void dumpShort() const;
};

SlotStatistics computeSlotStatistics(MachineBasicBlock &MBB,
                                     const AIEBaseInstrInfo *TII);

// Compute slot statistics for an explicit list of instructions.
SlotStatistics computeSlotStatistics(ArrayRef<MachineInstr *> Instrs,
                                     const AIEBaseInstrInfo *TII);

/// Tracks the slot demand of the not yet scheduled free instructions of a
/// region, and uses it to order the alternatives of multi-slot pseudos. The
/// demand is region-wide, which allows carrying the pressure of one region
/// into the scheduling of a neighbouring one. A local demand (ready list)
/// would be a more precise signal for the next cycles, but is harder to
/// combine with that. Using the local demand first and the region-wide demand
/// only as a tie-breaker is a possible refinement.
class SlotDemandTracker {
  SlotStatistics Stats;
  SlotCounts Remaining;
  bool Active = false;

public:
  /// Stop tracking. Alternatives keep their TableGen order.
  void reset() {
    Stats = SlotStatistics();
    Remaining = SlotCounts();
    Active = false;
  }

  /// Start tracking the demand of \p FreeInstrs.
  void init(ArrayRef<MachineInstr *> FreeInstrs, const AIEBaseInstrInfo *TII);

  bool isActive() const { return Active; }
  const SlotCounts &getRemainingDemand() const { return Remaining; }

  /// Remove the demand of \p MI. This must only be called for the free
  /// instructions that init() was given, never for fixed ones.
  void removeDemand(MachineInstr &MI, const AIEBaseInstrInfo *TII);

  /// Reorder \p Alternatives, the alternatives of a multi-slot pseudo in
  /// TableGen order, so that the one with the least remaining demand comes
  /// first if it beats the default by a margin. No-op if not tracking.
  void orderAlternatives(SmallVectorImpl<unsigned> &Alternatives,
                         const AIEBaseMCFormats *Formats) const;
};

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIESLOTSTATISTICS_H
