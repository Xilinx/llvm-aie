//===--- AIESloStatistics.h ----------------------------------------------r===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
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

namespace llvm {
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

  // Add an instruction to the statistics
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

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIESLOTSTATISTICS_H
