//===-- AIEMachineBasicBlockUtils.h ------------------------------*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains utility function declarations for MachineBasicBlocks in
// the AIE target
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_AIE_UTILS_AIEMACHINEBASICBLOCKUTILS_H
#define LLVM_LIB_TARGET_AIE_UTILS_AIEMACHINEBASICBLOCKUTILS_H

#include "llvm/CodeGen/MachineBasicBlock.h"

namespace llvm::AIEMachineBasicBlockUtils {
/// isBlockOnlyReachableByFallthough - Return true if the basic block has
/// exactly one predecessor and the control transfer mechanism between
/// the predecessor and this block is a fall-through.
/// Note: code copied from AmsPrinter::isBlockOnlyReachableByFallthrough
bool isBlockOnlyReachableByFallthrough(const MachineBasicBlock *MBB);

} // namespace llvm::AIEMachineBasicBlockUtils

#endif
