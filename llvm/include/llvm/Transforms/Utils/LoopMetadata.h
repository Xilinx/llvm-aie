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

  /// Branch Compare Data
  ICmpInst *LoopCmpInstr;

  unsigned MinIterCount;
  bool IsLoopIncrementing;
  int64_t LoopStepSize;
  bool IsTruncatedSCEV;

  /// lower loop boundary
  Value *LowerBoundary;
  /// upper loop boundary
  Value *UpperBoundary;

  /// extract loop iteration counts and create an assumption
  bool assignLoopMetadata(Loop &L);

  void addAssumeToLoopHeader(uint64_t MinIterCount, LLVMContext *Context);

  Value *getUpperTruncatedBound() const;
  /// get lower and upper boundaries of the loop
  void getBoundaries(const SCEV *S);
  /// validate that the boundries are correctly extracted and that this pass can
  /// process the boundries
  bool validateBounds();

  /// calculate the minimum difference between lower and upper boundary. The
  /// minimum iteration counts are provided by MinIterCount, which is extracted
  /// from the loop metadata.
  Value *calcMinIterValue(const SCEV *S, int MinIterCount,
                          LLVMContext *Context);

  /// return true, if it can be determined, if the loop increments or
  /// decrements
  bool canExtractIncrement(const SCEV *S);

  /// If the IV is modified through a trunctation, generate a SCEVAddRecExpr
  /// that can be processed by this pass
  const SCEV *getTruncatedSCEV();

  /// Check if the Instruction is part of a truncated SCEVAddExpr that can be
  /// converted into a SCEVAddRecExpr. During the conversion the truncation is
  /// removed.
  const SCEV *extractSCEVFromTruncation(Instruction *I);

  /// get the SCEV of the loop that describes how the loop IV is modified
  const SCEV *getSCEV();

  /// match types for the compare instruction
  void matchCompareTypes(Value *MinIterValue, IRBuilder<> &Builder);

public:
  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U);

  static bool isRequired() { return true; }
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_UTILS_LOOPMETADATA_H
