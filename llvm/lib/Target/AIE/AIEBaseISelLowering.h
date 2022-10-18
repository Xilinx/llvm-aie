//===-- AIEBaseISelLowering.h - AIE IR Lowering Interface -------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the common interfaces that all AIE versions rely on to
// lower IR.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEBASEISELLOWERING_H
#define LLVM_LIB_TARGET_AIE_AIEBASEISELLOWERING_H

#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {
class AIEBaseSubtarget;

class AIEBaseTargetLowering : public TargetLowering {
public:
  explicit AIEBaseTargetLowering(const TargetMachine &TM,
                                 const AIEBaseSubtarget &STI);
  // Set the preferred type for memset intrinsics.
  EVT getOptimalMemOpType(const MemOp &Op,
                          const AttributeList &FuncAttributes) const override {
    // Vector registers are hard to initilaize.  For the time being, try
    // to keep memory intrinsics (e.g. memset) using scalar types.
    return MVT::i32;
  }

  MVT getVectorIdxTy(const DataLayout &DL) const override;

  /// Returns if it's reasonable to merge stores to MemVT size.
  bool canMergeStoresTo(unsigned AS, EVT MemVT,
                        const MachineFunction &MF) const override {
    // Don't create vector types if we don't have to.
    if (MemVT.isVector()) {
      return false;
    }
    return TargetLowering::canMergeStoresTo(AS, MemVT, MF);
  }

  CCAssignFn *CCAssignFnForCall(bool IsVarArg = false) const;
  CCAssignFn *CCAssignFnForReturn() const;

  /// Make sure the first vararg stack slot is 32-byte aligned.
  static void alignFirstVASlot(CCState &CCInfo);

  /// Returns the size of a stack slot in bytes.
  static unsigned getStackSlotSize() { return 4; }

  /// Returns the size of a stack slot in bits.
  static unsigned getStackSlotSizeInBits() { return 8 * getStackSlotSize(); }

  /// Returns the alignment for arguments on the stack.
  static Align getStackArgumentAlignment() { return Align(4); }

protected:
  bool isEligibleForTailCallOptimization(
      CCState &CCInfo, CallLoweringInfo &CLI, MachineFunction &MF,
      const SmallVector<CCValAssign, 16> &ArgLocs) const;

  const AIEBaseSubtarget &Subtarget;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEBASEISELLOWERING_H
