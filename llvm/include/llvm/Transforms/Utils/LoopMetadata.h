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

  unsigned MinIterCount;

  /// Branch Compare Data
  ICmpInst *LoopCmpInstr;
  bool IsLoopIncrementing;
  int64_t LoopStepSize;

  /// Loop Boundaries
  /// lower loop boundary
  Value *LowerBoundary;
  /// upper loop boundary
  Value *UpperBoundary;

  /// extract loop iteration counts and create an assumption
  bool assignLoopMetadata(Loop &L);

  void addAssumeToLoopHeader();

   /// get lower and upper boundaries of the loop
  void getBoundaries(const SCEV *S);

  /// calculate the minimum difference between lower and upper boundary. The
  /// minimum iteration counts are provided by MinIterCount, which is
  /// extracted from the loop metadata.
  Value *calcMinIterValue(const SCEV *S, int MinIterCount,
                          LLVMContext *Context);

  /// return true, if it can be determined, if the loop increments or
  /// decrements
  bool canExtractIncrement(const SCEV *S);

  /// get the SCEV of the loop that describes how the loop IV is modified
  const SCEV *getSCEV();
  /// recursive low level function that can extracts
  const SCEV *getAddRecSCEV(Value *Op);

public:
  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U);

  static bool isRequired() { return true; }
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_UTILS_LOOPMETADATA_H
