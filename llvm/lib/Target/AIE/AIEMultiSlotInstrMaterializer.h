//===--- AIEMultiSlotInstrMaterializer.h -Multi Slot Instr materializer----===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// \file assigns an issue-slot to multi slot pseudo instructions within a single
// block loop to help loop pipelining.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"

namespace llvm {
class AIEHazardRecognizer;
struct AIEBaseInstrInfo;
class MachineBasicBlock;
class MachineInstr;
} // namespace llvm

namespace llvm::AIE {

/// Compute and report slot statistics of \p Instrs under the
/// "slot-statistics" debug type. This is free if that debug type is disabled.
/// \p Instrs holds all instructions in the region (free and fixed).
void reportSlotStatistics(StringRef RegionName, ArrayRef<MachineInstr *> Instrs,
                          const AIEBaseInstrInfo *TII);

/// Materialize multi-slot pseudo instructions in \p Instrs to minimize slot
/// pressure imbalance, and report the slot statistics.
/// \p BiasInstrs supplies pre-materialized instructions from a neighbouring
/// block (e.g. a loop preheader) whose fixed slot counts bias slot selection;
/// pass an empty span for unbiased regions.
void materializeToMinimizeSlotTotals(StringRef RegionName,
                                     ArrayRef<MachineInstr *> Instrs,
                                     ArrayRef<MachineInstr *> BiasInstrs,
                                     const AIEBaseInstrInfo *TII);

/// PostPipeliner-specific pre-heuristic: memory bank based slot assignment of
/// multi-slot load pseudos, with round-robin as fallback. Call this only for
/// PostPipeliner candidates, before \c materializeToMinimizeSlotTotals.
///
/// FIXME: Currently only multi-slot memory load pseudos are handled.
void materializeLoadsByMemoryBank(MachineBasicBlock &MBB,
                                  const AIEHazardRecognizer &HR);

} // namespace llvm::AIE
