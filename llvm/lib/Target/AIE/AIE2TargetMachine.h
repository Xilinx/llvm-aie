//===--AIE2TargetMachine.h -Define TargetMachine for AIEngine V2 -*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
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

} // namespace llvm

#endif
