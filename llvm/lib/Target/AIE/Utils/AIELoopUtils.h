//===-- AIELoopUtils.h ----------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains helper functions to handle loops and its metadata.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_AIE_UTILS_AIELOOPUTILS_H
#define LLVM_LIB_TARGET_AIE_UTILS_AIELOOPUTILS_H

#include "llvm/Analysis/LoopInfo.h"
#include "llvm/CodeGen/MachineLoopInfo.h"

namespace llvm {
class MachineBasicBlock;
} // namespace llvm

namespace llvm::AIELoopUtils {

/// Get the LoopID from a single block loop or nullptr
const MDNode *getLoopID(const MachineBasicBlock &LoopBlock);

/// LoopBlock should hold the backedge. Since we generally call it for
/// single block loops, that's automatically true.
std::optional<int64_t> getMinTripCount(const MachineBasicBlock &LoopBlock);

/// Returns true if this is a loop latch that has a pipeliner disable pragma,
/// none otherwise.
std::optional<bool> getPipelinerDisabled(const MachineBasicBlock &LoopBlock);

/// Check that the single block loop represented by LoopBlock has a fallthrough
/// preheader. Return the preheader if true, nullptr otherwise
MachineBasicBlock *
getDedicatedFallThroughPreheader(const MachineBasicBlock &LoopBlock);

// get all the Machine Basic Blocks (MBBs) that contain a Single Block Loop,
// which is defined by having 2 Successors, where one of the succesors, is the
// MBB itself.
SmallVector<const MachineBasicBlock *, 4>
getSingleBlockLoopMBBs(const MachineFunction &MF);

/// Check if this block is a single block loop.
bool isSingleMBBLoop(const MachineBasicBlock *MBB);

} // namespace llvm::AIELoopUtils

#endif
