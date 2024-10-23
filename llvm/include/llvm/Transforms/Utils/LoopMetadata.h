//===-- LoopMetadata.h - Convert Loop Metadata to assumptions --*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_UTILS_LOOPMETADATA_H
#define LLVM_TRANSFORMS_UTILS_LOOPMETADATA_H
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"

namespace llvm {

class Loop;
/// Converts Loop Metadata to assumptions.
class LoopMetadata : public PassInfoMixin<LoopMetadata> {
private:
  LLVMContext *Context;
  ScalarEvolution *SE;
  AssumptionCache *AC;
  DominatorTree *DT;
  const Loop *L;
  Instruction *LoopBound0;
  Instruction *LoopBound1;
  unsigned MinIterCount;

  Value *MinValue;
  Value *MaxBoundry;

  bool extractMetaData(Loop &L);

  void addAssumeToLoopHeader(uint64_t MinIterCount, LLVMContext *Context);

  void getBoundries();
  Value *getMinIterValue(const SCEV *S, int MinIterCount, LLVMContext *Context);

  Value *getValue(Value *V) const;
  Value *getMinBoundInEqualityComparison(Value *Op) const;

  const SCEV *getTruncInductionSCEV();
  const SCEV *getSCEV();

public:
  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U);

  static bool isRequired() { return true; }
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_UTILS_LOOPMETADATA_H
