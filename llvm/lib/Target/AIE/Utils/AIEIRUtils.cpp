//===- AIEIRUtils.cpp - AIE IR Utility Functions ----------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEIRUtils.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Analysis/VectorUtils.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/KnownBits.h"
#include "llvm/Transforms/InstCombine/InstCombiner.h"
#include "llvm/Transforms/Utils/Local.h"

namespace llvm::AIEIRUtils {

static Intrinsic::ID getIntrinsicID(const Instruction *I) {
  const auto *Call = dyn_cast<CallInst>(I);
  if (!Call)
    return Intrinsic::not_intrinsic;
  const auto *Fn = Call->getCalledFunction();
  return Fn ? Fn->getIntrinsicID() : Intrinsic::not_intrinsic;
}

bool isHardwareLoopSetup(const Instruction *I) {
  Intrinsic::ID IID = getIntrinsicID(I);
  return IID == Intrinsic::set_loop_iterations ||
         IID == Intrinsic::start_loop_iterations;
}

bool isHardwareLoopDecrement(const Instruction *I) {
  return getIntrinsicID(I) == Intrinsic::loop_decrement;
}

std::optional<Instruction *> instCombineDemandedBits(InstCombiner &IC,
                                                     IntrinsicInst &II,
                                                     unsigned NumBits,
                                                     unsigned Operand) {
  KnownBits ScalarKnown(32);
  if (IC.SimplifyDemandedBits(&II, Operand, APInt::getLowBitsSet(32, NumBits),
                              ScalarKnown))
    return &II;

  return std::nullopt;
}

/// Helper function to recursively check if a user (and all its users if it's a
/// bitcast) access lanes higher than HighestLane.
bool checkIfUsersDontAccessLanesHigherThan(Instruction *User, Type *CurrentType,
                                           Instruction *Source,
                                           int HighestLane) {
  // If it's a bitcast, recursively check all its users
  if (BitCastInst *BitCast = dyn_cast<BitCastInst>(User)) {
    Type *BitCastType = BitCast->getType();
    for (Use &U : BitCast->uses()) {
      Instruction *BitCastUser = cast<Instruction>(U.getUser());
      if (!checkIfUsersDontAccessLanesHigherThan(BitCastUser, BitCastType,
                                                 BitCast, HighestLane))
        return false;
    }
    return true;
  }

  // If it's a shuffle, check if it extracts only the lower half
  if (const ShuffleVectorInst *Shuffle = dyn_cast<ShuffleVectorInst>(User)) {
    // Verify the shuffle uses our source as operand 0
    if (Shuffle->getOperand(0) != Source)
      return false;

    // Check if the shuffle mask extracts only from the lower half
    return llvm::none_of(Shuffle->getShuffleMask(),
                         [&](int Lane) { return Lane > HighestLane; });
  }

  // Any other instruction type means we can't guarantee upper part is discarded
  return false;
}

// Check if all users of the intrinsic instruction discard the upper half of
// the result vector. This is determined by verifying that users only extract
// the lower half lanes through shuffle operations with a sequential mask.
bool isUpperPartOfResultDiscarded(IntrinsicInst &II) {
  // If there are no uses, upper part is not discarded
  if (II.use_empty())
    return false;

  Type *Type = II.getType();
  auto *OutVecTy = cast<FixedVectorType>(Type);
  const unsigned HalfElemCount = OutVecTy->getNumElements() / 2;

  // Check all users of the intrinsic instruction
  for (Use &U : II.uses()) {
    Instruction *User = cast<Instruction>(U.getUser());
    if (!checkIfUsersDontAccessLanesHigherThan(User, Type, &II,
                                               HalfElemCount - 1))
      return false;
  }

  return true;
}

} // namespace llvm::AIEIRUtils
