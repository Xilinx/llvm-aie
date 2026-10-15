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

/// Controls whether slot-total minimization materializes pseudo instructions
/// or only reports statistics.
enum class MaterializationMode {
  // Compute and report slot statistics only; do not materialize any pseudo
  // instructions.
  None,
  // Report slot statistics and materialize MSPs to minimize slot totals,
  // guided by the bias instructions.
  Full,
};

/// Called for every region to compute and report slot statistics under the
/// "slot-statistics" debug type. In \p Full mode also materializes
/// multi-slot pseudo instructions to minimize slot pressure imbalance.
/// \p Instrs holds all instructions in the region (free and fixed).
/// \p BiasInstrs supplies pre-materialized instructions from a neighbouring
/// block (e.g. a loop preheader) whose fixed slot counts bias slot selection;
/// pass an empty span for unbiased / non-pipeline-candidate regions.
void materializeToMinimizeSlotTotals(StringRef RegionName,
                                     ArrayRef<MachineInstr *> Instrs,
                                     ArrayRef<MachineInstr *> BiasInstrs,
                                     MaterializationMode Mode,
                                     const AIEBaseInstrInfo *TII);

/// PostPipeliner-specific pre-heuristics: bank-based slot assignment and
/// round-robin fallback for multi-slot pseudo instructions. Call this only for
/// PostPipeliner candidates, before \c materializeToMinimizeSlotTotals.
///
/// FIXME: Currently only multi-slot memory load pseudos are handled.
void materializeMultiSlotInstructions(MachineBasicBlock &MBB,
                                      const AIEHazardRecognizer &HR);

} // namespace llvm::AIE
