//===--- AIESlotUtils.h     -----------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
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

uint64_t getSlotSet(unsigned Opcode, const AIEBaseInstrInfo *TII);
SlotCounts getSlotCounts(unsigned Opcode, const AIEBaseInstrInfo *TII);

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIESLOTUTILS_H
