//===--AIE2TargetMachine.h -Define TargetMachine for AIEngine V2 -*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the AIEngine V2 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE2TARGETMACHINE_H
#define LLVM_LIB_TARGET_AIE_AIE2TARGETMACHINE_H

#include "AIE2Subtarget.h"
#include "AIEBaseTargetMachine.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"

extern llvm::cl::opt<bool> EnableSubregRenaming;
namespace llvm {

class AIE2TargetMachine : public AIEBaseTargetMachine {
  AIE2Subtarget Subtarget;
  virtual void anchor();

public:
  AIE2TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                    StringRef FS, const TargetOptions &Options,
                    std::optional<Reloc::Model> RM,
                    std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                    bool JIT);
  const AIE2Subtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }
  TargetTransformInfo getTargetTransformInfo(const Function &F) const override;
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  /// PostRAScheduling is scheduled as part of PreSched2 passes.
  bool targetSchedulesPostRAScheduling() const override { return true; }
  unsigned getAddressSpaceForPseudoSourceKind(unsigned Kind) const override;
};

// AIE2 Pass Setup
class AIE2PassConfig : public AIEBasePassConfig {
public:
  AIE2PassConfig(TargetMachine &TM, PassManagerBase &PM)
      : AIEBasePassConfig(TM, PM) {
    if (!EnableSubregRenaming)
      disablePass(&RenameIndependentSubregsID);
  }

  AIE2TargetMachine &getAIETargetMachine() const {
    return getTM<AIE2TargetMachine>();
  }

  bool addPreISel() override;
  void addPreEmitPass() override;
  bool addGlobalInstructionSelect() override;
  void addPreRegAlloc() override;
  bool addRegAssignAndRewriteOptimized() override;
  void addPostRewrite() override;
  void addMachineLateOptimization() override;
  void addPreSched2() override;
  void addBlockPlacement() override;
  void addPreLegalizeMachineIR() override;
  void addPreRegBankSelect() override;
  void addPreGlobalInstructionSelect() override;
  void addISelPrepare() override;
  bool addILPOpts() override;

protected:
  void addRegRewritePasses();
};

} // namespace llvm

#endif
