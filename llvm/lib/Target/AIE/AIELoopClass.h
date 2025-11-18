//===--- AIELoopClass.h ----------------------------------------------r===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines an abstract loop classification. Intended for single-block
// loops, it gets the loop block and returns a vector of scores for each
// registered loop. The first loop with the best score will be considered the
// loop class for the loop under test.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIELOOPCLASS_H
#define LLVM_LIB_TARGET_AIE_AIELOOPCLASS_H

#include "AIESlotStatistics.h"
#include <vector>

namespace llvm {
struct AIEBaseInstrInfo;
class MachineBasicBlock;
} // namespace llvm

namespace llvm::AIE {

// Returns the score vector for MBB. Each element of the returned vector
// represents the match error for a standard kernel. Hence, the lowest number
// represents the best match.
std::vector<int> getLoopClassScores(const SlotStatistics &Stats);
std::vector<int> getLoopClassScores(MachineBasicBlock &MBB,
                                    const AIEBaseInstrInfo *TII);

// Return the index of the best matching kernel.
int classifyLoop(const SlotStatistics &Stats);
int classifyLoop(MachineBasicBlock &MBB, const AIEBaseInstrInfo *TII);

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIELOOPCLASS_H
