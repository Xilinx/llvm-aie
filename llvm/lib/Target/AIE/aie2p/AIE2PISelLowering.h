//===-- AIE2ISelLowering.h - AIE2p IR Lowering Interface --------*- C++ -*-==//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the AIE2p-specific interfaces used to lower IR to MIR.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE2PISELLOWERING_H
#define LLVM_LIB_TARGET_AIE_AIE2PISELLOWERING_H

#include "AIEBaseISelLowering.h"

namespace llvm {

class AIE2PTargetLowering : public AIEBaseTargetLowering {
public:
  explicit AIE2PTargetLowering(const TargetMachine &TM,
                               const AIEBaseSubtarget &STI);
  bool functionArgumentNeedsConsecutiveRegisters(
      Type *Ty, CallingConv::ID CallConv, bool isVarArg,
      const DataLayout &DL) const override;
  // Returning true will prevent targetlowering from implementing
  // CTLZ as CTLZ_ZERO_UNDEF. Effectively it says: We have an instruction
  // that properly implements CTLZ.
  // I wonder why this isn't the default.
  bool isCheapToSpeculateCtlz(Type *) const override { return true; }
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIE2PISELLOWERING_H
