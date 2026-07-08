//===- AIEIRUtils.h - AIE IR Utility Functions ------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_UTILS_AIEIRUTILS_H
#define LLVM_LIB_TARGET_AIE_UTILS_AIEIRUTILS_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Intrinsics.h"
#include <optional>

namespace llvm {
class Instruction;
class IntrinsicInst;
class Type;
class InstCombiner;
class Triple;
class Loop;
} // namespace llvm

namespace llvm::AIEIRUtils {

/// True if I establishes a hardware-loop trip count
/// (@llvm.set.loop.iterations or @llvm.start.loop.iterations).
bool isHardwareLoopSetup(const Instruction *I);

/// True if I is a call to @llvm.loop.decrement, the intrinsic that controls a
/// hardware-loop latch branch.
bool isHardwareLoopDecrement(const Instruction *I);

/// Helper function to recursively check if a user (and all its users if it's a
/// bitcast) access lanes higher than HighestLane.
bool checkIfUsersDontAccessLanesHigherThan(Instruction *User, Type *CurrentType,
                                           Instruction *Source,
                                           int HighestLane);

/// Check if all users of the intrinsic instruction discard the upper half of
/// the result vector. This is determined by verifying that users only extract
/// the lower half lanes through shuffle operations with a sequential mask.
bool isUpperPartOfResultDiscarded(IntrinsicInst &II);

/// Simplify demanded bits for an intrinsic instruction.
/// Uses InstCombiner to simplify the specified operand to only the low NumBits.
/// Operand defaults to 0 if not specified.
std::optional<Instruction *> instCombineDemandedBits(InstCombiner &IC,
                                                     IntrinsicInst &II,
                                                     unsigned NumBits,
                                                     unsigned Operand = 0);

/// Return the subtarget-specific loop-version-threshold intrinsic for \p TT, or
/// Intrinsic::not_intrinsic if the subtarget does not support loop versioning.
Intrinsic::ID getLoopVersionThresholdIntrinsic(const Triple &TT);

/// Rebuild \p L's loop id, dropping every metadata entry whose string key is in
/// \p KeysToDrop. A no-op if the loop has no loop id or nothing is requested.
void dropLoopMetadata(Loop &L, ArrayRef<StringRef> KeysToDrop);

} // namespace llvm::AIEIRUtils

#endif // LLVM_LIB_TARGET_AIE_UTILS_AIEIRUTILS_H
