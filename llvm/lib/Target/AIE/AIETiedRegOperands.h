//===-- AIETiedRegOperands.h - Information about tied operands --*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the AIE specific datatypes for tying operands.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIETIEDREGOPERANDS_H
#define LLVM_LIB_TARGET_AIE_AIETIEDREGOPERANDS_H

#include "llvm/ADT/SmallVector.h"

namespace llvm {

/// Information on one particular sub-reg to be split from a super-reg.
struct SubRegSplit {
  SubRegSplit(unsigned SubReg, bool IsUndef = false)
      : SubReg(SubReg), IsUndef(IsUndef) {}

  /// Subreg index to be extracted into a new reg operand.
  unsigned SubReg;

  /// Whether the new reg operand needs an 'undef' attribute.
  bool IsUndef = false;
};

struct OperandSubRegMapping {
  /// Index of the operand in a MachineInstr
  unsigned OpIdx;
  /// The sub-register of a tied register that this operand uses
  unsigned SubRegIdx;

  /// Information on how to split the operand into multiple operands.
  /// For super-regs, this is the list of sub-regs to split the operand into
  /// when rewriting the instruction into SplitOpcode.
  /// For for AIE2's 2D registers, this would be:
  ///   {MOD, DIM_SIZE, DIM_STRIDE, DIM_COUNT}
  SmallVector<SubRegSplit, 8> SubRegsSplit = {};
};

/// Information about one set of tied source and destination operands
/// with regards to the sub-registers they use.
struct TiedRegOperands {
  SmallVector<OperandSubRegMapping, 4> DstOps;
  OperandSubRegMapping SrcOp;

  /// Find the OperandSubRegMapping for the operand at index \p OpIdx, if any.
  const OperandSubRegMapping *findOperandInfo(unsigned OpIdx) const;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIETIEDREGOPERANDS_H
