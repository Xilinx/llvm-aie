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
#include "llvm/IR/PatternMatch.h"
#include "llvm/Support/KnownBits.h"
#include "llvm/Transforms/InstCombine/InstCombiner.h"
#include "llvm/Transforms/Utils/Local.h"

namespace llvm::AIEIRUtils {

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

std::optional<Instruction *> instCombineVExtractBroadcast(InstCombiner &IC,
                                                          IntrinsicInst &II,
                                                          unsigned LaneBits) {
  // The fold only applies when the lane index is a compile-time constant.
  // For runtime indices we leave the opaque intrinsic so the backend can
  // select a single VEXTBCST instruction.
  auto *IdxC = dyn_cast<ConstantInt>(II.getArgOperand(1));
  if (!IdxC)
    return std::nullopt;

  Value *Vec = II.getArgOperand(0);
  auto *VecTy = cast<FixedVectorType>(Vec->getType());
  const unsigned ElemBits =
      VecTy->getElementType()->getPrimitiveSizeInBits().getFixedValue();

  // The lane width must be a whole multiple of the element width, and the
  // 512-bit register must split into a whole number of LaneBits-wide lanes.
  // Bail out instead of asserting so the optimisation is robust against
  // unexpected intrinsic signatures introduced later.
  if (ElemBits == 0 || LaneBits % ElemBits != 0)
    return std::nullopt;
  const unsigned LaneElems = LaneBits / ElemBits;
  const unsigned NumElems = VecTy->getNumElements();
  if (LaneElems == 0 || NumElems % LaneElems != 0)
    return std::nullopt;
  const unsigned NumLanes = NumElems / LaneElems;

  // VEXTBCST encodes the index modulo the number of lanes (the upper bits of
  // the 6-bit immediate field are documented as zero-extended).
  const unsigned LaneIdx = IdxC->getZExtValue() & (NumLanes - 1);
  const unsigned Base = LaneIdx * LaneElems;

  // Build a mask that broadcasts elements [Base, Base+LaneElems) across all
  // NumLanes quadrants of the 512-bit result.
  SmallVector<int, 64> Mask;
  Mask.reserve(NumElems);
  for (unsigned Lane = 0; Lane < NumLanes; ++Lane)
    for (unsigned E = 0; E < LaneElems; ++E)
      Mask.push_back(Base + E);

  return new ShuffleVectorInst(Vec, Mask);
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
