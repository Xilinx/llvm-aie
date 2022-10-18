//===-- AIE2.h - Top-level interface for AIEngine V2 ------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// AIE2 back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE2_H
#define LLVM_LIB_TARGET_AIE_AIE2_H

#include "Utils/AIEBaseInfo.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class AIE2RegisterBankInfo;
class AIEBaseSubtarget;
class AIE2Subtarget;
class AIEBaseTargetMachine;
class AIE2TargetMachine;
class AsmPrinter;
class FunctionPass;
class InstructionSelector;
class MCInst;
class MCOperand;
class MachineInstr;
class MachineOperand;
class PassRegistry;

FunctionPass *createAIE2ISelDag(TargetMachine &TM);
FunctionPass *createAIE2PreLegalizerCombiner();
FunctionPass *createAIE2PostLegalizerCustomCombiner();
FunctionPass *createAIE2PostLegalizerGenericCombiner();
InstructionSelector *createAIE2InstructionSelector(const AIE2TargetMachine &,
                                                   AIE2Subtarget &,
                                                   AIE2RegisterBankInfo &);

void initializeAIE2PreLegalizerCombinerPass(PassRegistry &);
void initializeAIE2PostLegalizerCustomCombinerPass(PassRegistry &);
void initializeAIE2PostLegalizerGenericCombinerPass(PassRegistry &);
} // namespace llvm

#endif
