//===-- AIE2PISelLowering.cpp - AIE2p IR Lowering Interface ----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the AIE2p-specific interfaces used to lower IR to MIR.
//
//===----------------------------------------------------------------------===//

#include "AIE2PISelLowering.h"
#include "AIESubtarget.h"
#include "MCTargetDesc/aie2p/AIE2PMCTargetDesc.h"

using namespace llvm;

#define DEBUG_TYPE "aie-lower"

AIE2PTargetLowering::AIE2PTargetLowering(const TargetMachine &TM,
                                         const AIEBaseSubtarget &STI)
    : AIEBaseTargetLowering(TM, STI) {
  const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();

  // We already define in .td which types are legal for each register class.
  // Let's re-use the information.
  for (unsigned i = 0; i != MVT::VALUETYPE_SIZE; ++i) {
    MVT Ty = MVT::SimpleValueType(i);
    // As a base rule, a type will be legal if there is a register class which
    // can natively hold it. Note that the class selected below does not matter
    // for a GlobalISel flow, since the selection is RegisterBank-based.
    const auto *RCIt =
        find_if(TRI->regclasses(), [Ty, TRI](const TargetRegisterClass *RC) {
          return TRI->isTypeLegalForClass(*RC, Ty);
        });
    // Add 128-bit RegClass as unavailable regclass for the 128-bit value type
    // as this RegClass is not supported natively
    if (RCIt != TRI->regclass_end() && !Ty.is128BitVector()) {
      addRegisterClass(Ty, *RCIt);
    }
  }
  computeRegisterProperties(STI.getRegisterInfo());
  setStackPointerRegisterToSaveRestore(AIE2P::sp);
}

namespace {

// Returns true if type name matches with a bfp16 type name
bool isTyNameBfp16(StringRef TyName) {
  if (TyName.ends_with("struct.v64bfp16ebs16"))
    return true;
  else if (TyName.ends_with("struct.v64bfp16ebs8"))
    return true;
  else if (TyName.ends_with("struct.v128bfp16ebs16"))
    return true;
  else if (TyName.ends_with("struct.v128bfp16ebs8"))
    return true;
  else
    return false;
}
} // namespace
bool AIE2PTargetLowering::functionArgumentNeedsConsecutiveRegisters(
    Type *Ty, CallingConv::ID CallConv, bool isVarArg,
    const DataLayout &DL) const {
  StructType *STy = dyn_cast<StructType>(Ty);
  if (!STy || STy->isLiteral())
    return false;
  if (isTyNameBfp16(STy->getName())) {
    return true;
  }
  return false;
}
