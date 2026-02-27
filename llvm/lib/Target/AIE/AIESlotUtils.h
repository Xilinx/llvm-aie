//===--- AIESlotUtils.h     -----------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares some utilities related to slots.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIESLOTUTILS_H
#define LLVM_LIB_TARGET_AIE_AIESLOTUTILS_H

#include "AIESlotStatistics.h"

namespace llvm {
struct AIEBaseInstrInfo;
} // namespace llvm

namespace llvm::AIE {

/// \return per-slot instruction counts based on the primary slot occupancy
/// of \p Opcode. Each instruction contributes exactly one count to its own
/// slot index. Use this for resource-constrained lower bounds (e.g. ResMII)
/// where only actual slot occupancy matters.
SlotCounts getSlotCounts(unsigned Opcode, const AIEBaseInstrInfo *TII);

/// \return per-slot instruction counts based on the conflict set of
/// \p Opcode. An instruction contributes a count to every slot it conflicts
/// with (e.g. Lng contributes to Alu, Lng, and Mv). Use this for slot
/// pressure heuristics where cross-slot interference must be considered.
SlotCounts getConflictCounts(unsigned Opcode, const AIEBaseInstrInfo *TII);

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIESLOTUTILS_H
