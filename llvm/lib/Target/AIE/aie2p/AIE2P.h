//===-- AIE2P.h -------- Top-level interface for AIE2P ---------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// AIE2P back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE2P_H
#define LLVM_LIB_TARGET_AIE_AIE2P_H

namespace llvm {
class AIE2PRegisterBankInfo;
class AIE2PSubtarget;
class AIE2PTargetMachine;
class InstructionSelector;
class PassRegistry;
class FunctionPass;

InstructionSelector *createAIE2PInstructionSelector(const AIE2PTargetMachine &,
                                                    AIE2PSubtarget &,
                                                    AIE2PRegisterBankInfo &);
FunctionPass *createAIE2PPreLegalizerCombiner();
void initializeAIE2PPreLegalizerCombinerPass(PassRegistry &);
FunctionPass *createAIE2PPostLegalizerGenericCombiner();
void initializeAIE2PPostLegalizerGenericCombinerPass(PassRegistry &);
FunctionPass *createAIE2PPostLegalizerCustomCombiner();
void initializeAIE2PPostLegalizerCustomCombinerPass(PassRegistry &);
} // namespace llvm

#endif
