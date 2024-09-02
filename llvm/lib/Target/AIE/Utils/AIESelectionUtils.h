//===-- AIESelectionUtils.h -----------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains helper functions related to instruction selection.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_AIE_UTILS_AIESELECTIONUTILS_H
#define LLVM_LIB_TARGET_AIE_UTILS_AIESELECTIONUTILS_H

#define DEBUG_TYPE "aie-isel"

namespace llvm::AIESelectionUtils { // namespace llvm::AIESelectionUtils

template <unsigned NumEncodingBits, unsigned Step>
bool checkImmediateRange(std::optional<APInt> Immediate) {
  unsigned MaxPow2 = NumEncodingBits + llvm::Log2_64(Step);
  if (Immediate && isIntN(MaxPow2, Immediate->getSExtValue()) &&
      Immediate->getSExtValue() % Step == 0) {
    LLVM_DEBUG(dbgs() << "Immediate " << Immediate << " is valid for MaxPow2 "
                      << MaxPow2 << " and Step " << Step << ".\n");
    return true;
  }
  return false;
}

template <unsigned NumEncodingBits, unsigned Step, unsigned SplitOffset>
bool checkImmediateRangeSplitting(std::optional<APInt> Immediate) {
  return Immediate && checkImmediateRange<NumEncodingBits, Step>(Immediate) &&
         checkImmediateRange<NumEncodingBits, Step>(*Immediate + SplitOffset);
}

static unsigned getLoadStoreSize(const MachineInstr &MI) {
  // We are guaranteed to have MMOs during Instruction Selection.
  // We need them to select the correct instruction when they depend on the
  // size in memory and not on the register size. E.g.: part word stores.
  return (*MI.memoperands_begin())->getSizeInBits().getValue();
}

} // namespace llvm::AIESelectionUtils

#undef DEBUG_TYPE

#endif
