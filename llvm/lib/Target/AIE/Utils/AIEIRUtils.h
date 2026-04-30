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

#include <optional>

namespace llvm {
class Instruction;
class IntrinsicInst;
class Type;
class InstCombiner;
} // namespace llvm

namespace llvm::AIEIRUtils {

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

/// Fold a `vextract_broadcast{8,16,32,64,128}_I512` (or _bf512) call with a
/// constant lane index into an equivalent `shufflevector` that broadcasts the
/// chosen lane (LaneBits wide) across all lanes of the 512-bit result. Once
/// the opaque intrinsic is replaced, downstream low-N shuffles collapse to a
/// single constant-mask shuffle, restoring the codegen quality of constant-idx
/// callers (e.g. `extract_v*(_, 0)` from the `GET_SS_4`/`GET_SS_8` macros and
/// any templated `extract<>(constant)` chain).
///
/// The intrinsic is assumed to take (vec512, i32 idx) and return the same
/// vector type. The shape is uniform across AIE2 / AIE2P / AIE2PS, so the
/// caller passes the lane width in bits and this helper does the rest.
std::optional<Instruction *> instCombineVExtractBroadcast(InstCombiner &IC,
                                                          IntrinsicInst &II,
                                                          unsigned LaneBits);

} // namespace llvm::AIEIRUtils

#endif // LLVM_LIB_TARGET_AIE_UTILS_AIEIRUTILS_H
