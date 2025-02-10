//===--- AIEMultiSlotInstrMaterializer.h -Multi Slot Instr materializer----===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// \file assigns an issue-slot to multi slot pseudo instructions within a single
// block loop to help loop pipelining.
//
//===----------------------------------------------------------------------===//
#include "AIEBaseInstrInfo.h"

namespace llvm {
class AIEHazardRecognizer;
}

namespace llvm::AIE {

/// Statically assign and materialize Slots to multi-slot pseudo MachineInstr in
/// \p MBB .
/// FIXME: Currently we are only handling multi-slot memory load pseudos.
void staticallyMaterializeMultiSlotInstructions(MachineBasicBlock &MBB,
                                                const AIEHazardRecognizer &HR);

} // namespace llvm::AIE
