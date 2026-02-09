//===-- AIE2SISelLowering.h - AIE2ps IR Lowering Interface -------*- C++ -*-==//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the AIE2ps-specific interfaces used to lower IR to MIR.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE2PSISELLOWERING_H
#define LLVM_LIB_TARGET_AIE_AIE2PSISELLOWERING_H

#include "AIEBaseISelLowering.h"

namespace llvm {

class AIE2PSTargetLowering : public AIEBaseTargetLowering {
public:
  explicit AIE2PSTargetLowering(const TargetMachine &TM,
                                const AIEBaseSubtarget &STI);

  MVT getRegisterTypeForCallingConvAssignment(LLVMContext &Context,
                                              CallingConv::ID CC,
                                              EVT VT) const override;
  MVT getRegisterTypeForCallingConv(LLVMContext &Context, CallingConv::ID CC,
                                    EVT VT) const override;
  TargetLoweringBase::LegalizeTypeAction
  getPreferredVectorAction(MVT VT) const override;

  bool functionArgumentNeedsConsecutiveRegisters(
      Type *Ty, CallingConv::ID CallConv, bool isVarArg,
      const DataLayout &DL) const override;

  bool getTgtMemIntrinsic(IntrinsicInfo &Info, const CallInst &I,
                          MachineFunction &MF,
                          unsigned Intrinsic) const override;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIE2PSISELLOWERING_H
