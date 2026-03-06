//===-- AIE2PS.h -------- Top-level interface for AIE2ps ---------*- C++
//-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// AIE2ps back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE2PS_H
#define LLVM_LIB_TARGET_AIE_AIE2PS_H

namespace llvm {
class AIE2PSRegisterBankInfo;
class AIE2PSSubtarget;
class AIE2PSTargetMachine;
class InstructionSelector;
class PassRegistry;
class FunctionPass;
InstructionSelector *
createAIE2PSInstructionSelector(const AIE2PSTargetMachine &, AIE2PSSubtarget &,
                                AIE2PSRegisterBankInfo &);

FunctionPass *createAIE2PSConvertFP16ScalarOperationPass();
void initializeAIE2PSConvertFP16ScalarOperationPassPass(PassRegistry &);
} // namespace llvm

#endif
