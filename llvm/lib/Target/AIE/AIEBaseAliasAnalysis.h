//===- AIEBaseAliasAnalysis -------------------------------------*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
/// \file
/// This is the AIE alias analysis pass.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEBASEALIASANALYSIS_H
#define LLVM_LIB_TARGET_AIE_AIEBASEALIASANALYSIS_H

#include "llvm/Analysis/AliasAnalysis.h"

namespace llvm {

class DataLayout;
class MemoryLocation;

class AIEBaseAAResult : public AAResultBase {
  const DataLayout &DL;

public:
  explicit AIEBaseAAResult(const DataLayout &DL) : DL(DL) {}
  AIEBaseAAResult(AIEBaseAAResult &&Arg)
      : AAResultBase(std::move(Arg)), DL(Arg.DL) {}

  /// Handle invalidation events from the new pass manager.
  ///
  /// By definition, this result is stateless and so remains valid.
  bool invalidate(Function &, const PreservedAnalyses &,
                  FunctionAnalysisManager::Invalidator &Inv) {
    return false;
  }

  AliasResult alias(const MemoryLocation &LocA, const MemoryLocation &LocB,
                    AAQueryInfo &AAQI, const Instruction *CtxI);
};

/// Analysis pass providing a never-invalidated alias analysis result.
class AIEBaseAA : public AnalysisInfoMixin<AIEBaseAA> {
  friend AnalysisInfoMixin<AIEBaseAA>;

  static AnalysisKey Key;

public:
  using Result = AIEBaseAAResult;

  AIEBaseAAResult run(Function &F, AnalysisManager<Function> &AM) {
    return AIEBaseAAResult(F.getParent()->getDataLayout());
  }
};

/// Legacy wrapper pass to provide the AIEBaseAAResult object.
class AIEBaseAAWrapperPass : public ImmutablePass {
  std::unique_ptr<AIEBaseAAResult> Result;

public:
  static char ID;

  AIEBaseAAWrapperPass();

  AIEBaseAAResult &getResult() { return *Result; }
  const AIEBaseAAResult &getResult() const { return *Result; }

  bool doInitialization(Module &M) override {
    Result.reset(new AIEBaseAAResult(M.getDataLayout()));
    return false;
  }

  bool doFinalization(Module &M) override {
    Result.reset();
    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override;
};

// Wrapper around ExternalAAWrapperPass so that the default constructor gets the
// callback.
class AIEBaseExternalAAWrapper : public ExternalAAWrapperPass {
public:
  static char ID;

  AIEBaseExternalAAWrapper()
      : ExternalAAWrapperPass([](Pass &P, Function &, AAResults &AAR) {
          if (auto *WrapperPass =
                  P.getAnalysisIfAvailable<AIEBaseAAWrapperPass>())
            AAR.addAAResult(WrapperPass->getResult());
        }) {}
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEBASEALIASANALYSIS_H
