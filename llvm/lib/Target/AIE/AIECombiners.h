//===-- AIECombiners.h - Unified AIE combiner declarations ------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIECOMBINERS_H
#define LLVM_LIB_TARGET_AIE_AIECOMBINERS_H

namespace llvm {

class FunctionPass;
class PassRegistry;

FunctionPass *createAIEPreLegalizerCombiner();
FunctionPass *createAIEPostLegalizerGenericCombiner();
FunctionPass *createAIEPostLegalizerCustomCombiner();
FunctionPass *createAIEPostLegalizerFinalCombiner();

void initializeAIEPreLegalizerCombinerPass(PassRegistry &);
void initializeAIEPostLegalizerGenericCombinerPass(PassRegistry &);
void initializeAIEPostLegalizerCustomCombinerPass(PassRegistry &);
void initializeAIEPostLegalizerFinalCombinerPass(PassRegistry &);

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIECOMBINERS_H
