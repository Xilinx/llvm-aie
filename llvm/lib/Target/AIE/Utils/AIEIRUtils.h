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
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Intrinsics.h"
#include <optional>

namespace llvm {
class BasicBlock;
class GetElementPtrInst;
class Instruction;
class IntrinsicInst;
class Type;
class InstCombiner;
class Triple;
class Loop;
class Value;
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

//===----------------------------------------------------------------------===//
// GEP / Pointer Utilities
// Shared between AIEOuterLoopPointerOptimizer and AIEInnerLoopPointerOptimizer.
//===----------------------------------------------------------------------===//

/// True if GEP has exactly one index (simple GEP).
bool isSimpleGEP(const GetElementPtrInst *GEP);

/// True if GEP has exactly one index and the source element type is i8.
bool isSimpleI8GEP(const GetElementPtrInst *GEP);

/// True if GEP is a valid chain-link candidate:
///   - i8-based with a single positive constant index.
/// Sets \p OutOffset to the signed byte offset on success.
bool isChainLinkCandidate(GetElementPtrInst *GEP, int64_t &OutOffset);

/// Collect loads/stores that directly use \p V, or that use an addrspacecast
/// of \p V.  Covers both bare-pointer and cast-pointer memory patterns.
SmallVector<Instruction *, 4> collectMemUsers(Value *V);

/// Return the instruction from \p Insns that appears textually last inside
/// \p BB.  Returns nullptr if none of the instructions belong to \p BB.
Instruction *findLastInBlock(ArrayRef<Instruction *> Insns, BasicBlock *BB);

} // namespace llvm::AIEIRUtils

#endif // LLVM_LIB_TARGET_AIE_UTILS_AIEIRUTILS_H
