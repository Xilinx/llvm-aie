//===- AIEMCInstrInfo.h - Utility functions on AIE MCInsts ------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Utility functions for AIE specific MCInst queries
// Deeply inspired by the Hexagon MCInstrInfo set of files
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCINSTRINFO_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCINSTRINFO_H

#include "AIE.h"
#include "AIEMCFormats.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/Support/MathExtras.h"
#include <cstddef>
#include <cstdint>

namespace llvm {
class MCInstrInfo;
class MCSubtargetInfo;

namespace AIEMCInstrInfo {

/// Return the Slot Kind of the MCInst
MCSlotKind getSlotKind(MCInst const &MCI);

/// Get the current index of the MCOperand MO in the MCInst MI as an out
/// parameter and returns true if it succeeds (meaning that MO is indeed
/// an MCOperand of MI), otherwise returns false
unsigned getMCOperandIndex(const MCInst &MI, const MCOperand &MO);

} // end namespace AIEMCInstrInfo
} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCINSTRINFO_H
