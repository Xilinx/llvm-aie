//===-- LoopIterCountAssumptions.h - Add loop assumptions -------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass converts Loop Iteration Count Metadata to Assumptions which can be
// picked up by Loop Rotate to remove Loop Guards.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_UTILS_LOOPITERCOUNTASSUMPTIONS_H
#define LLVM_TRANSFORMS_UTILS_LOOPITERCOUNTASSUMPTIONS_H
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"

namespace llvm {

class Loop;
/// Converts Loop Iteration Count Metadata to Assumptions.
class LoopIterCountAssumptions
    : public PassInfoMixin<LoopIterCountAssumptions> {

public:
  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U);
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_UTILS_LOOPITERCOUNTASSUMPTIONS_H
