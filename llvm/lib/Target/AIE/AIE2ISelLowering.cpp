//===-- AIE2ISelLowering.cpp - AIE IR Lowering Interface --------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the AIEv2-specific interfaces used to lower IR to MIR.
//
//===----------------------------------------------------------------------===//

#include "AIE2ISelLowering.h"
#include "AIESubtarget.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/CodeGen/ValueTypes.h"

using namespace llvm;

#define DEBUG_TYPE "aie-lower"

AIE2TargetLowering::AIE2TargetLowering(const TargetMachine &TM,
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
    // Add 128-bit RegClass as unavailable regclass for the 128-bit vector type
    // as this RegClass is not supported natively. Make it also unavailable for
    // the 128-bit integer type to prevent 64-bit promotion to 128-bit as these
    // should be passed and returned in 32-bit register pairs.
    if (RCIt != TRI->regclass_end() && !Ty.is128BitVector() &&
        !(Ty.isScalarInteger() && Ty.getScalarSizeInBits() == 128)) {
      addRegisterClass(Ty, *RCIt);
    }
  }
  computeRegisterProperties(STI.getRegisterInfo());

  setStackPointerRegisterToSaveRestore(AIE2::SP);
}
MVT AIE2TargetLowering::getRegisterTypeForCallingConvAssignment(
    LLVMContext &Context, CallingConv::ID CC, EVT VT) const {
  // 128-bit vectors are passed in 256-bit W registers
  if (VT.isSimple() && VT.is128BitVector())
    return getRegisterType(VT.getSimpleVT());
  // 128-bit scalars are passed in mask Q registers.
  if (VT.isScalarInteger() && VT.getScalarSizeInBits() == 128)
    return VT.getSimpleVT();
  return AIEBaseTargetLowering::getRegisterTypeForCallingConvAssignment(Context,
                                                                        CC, VT);
}

MVT AIE2TargetLowering::getRegisterTypeForCallingConv(LLVMContext &Context,
                                                      CallingConv::ID CC,
                                                      EVT VT) const {
  // 128-bits registers aren't considered legal because AIE2 only has vector ops
  // for 256+ bits vectors. However, for ABI compatibility reasons, we still
  // want to keep those vectors as is, because they are passed differently
  // compared to 256-bits vectors.
  if (VT.isSimple() && VT.is128BitVector())
    return VT.getSimpleVT();
  // 128-bit integers aren't considered legal to prevent dafault legalization of
  // 64-bit integers into 128-bit. However, for ABI compatibility reasons, we
  // still want to keep those integers as is, because they are passed in mask
  // registers.
  if (VT.isScalarInteger() && VT.getScalarSizeInBits() == 128)
    return VT.getSimpleVT();

  return TargetLowering::getRegisterTypeForCallingConv(Context, CC, VT);
}

unsigned AIE2TargetLowering::getNumRegistersForCallingConv(LLVMContext &Context,
                                                           CallingConv::ID CC,
                                                           EVT VT) const {
  // See getRegisterTypeForCallingConv(): i128 is passed in a single register
  if (VT.isScalarInteger() && VT.getScalarSizeInBits() == 128)
    return 1;
  return TargetLowering::getNumRegistersForCallingConv(Context, CC, VT);
}

TargetLoweringBase::LegalizeTypeAction
AIE2TargetLowering::getPreferredVectorAction(MVT VT) const {
  return TypeWidenVector;
}
