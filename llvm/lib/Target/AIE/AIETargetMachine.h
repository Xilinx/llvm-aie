//===-- AIETargetMachine.h - Define TargetMachine for AIE ---*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the AIE specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIETARGETMACHINE_H
#define LLVM_LIB_TARGET_AIE_AIETARGETMACHINE_H

#include "AIEBaseTargetMachine.h"
#include "AIESubtarget.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"

namespace llvm {

class AIETargetMachine : public AIEBaseTargetMachine {
  AIESubtarget Subtarget;

public:
  AIETargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                   StringRef FS, const TargetOptions &Options,
                   std::optional<Reloc::Model> RM,
                   std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                   bool JIT);

  const AIESubtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetTransformInfo getTargetTransformInfo(const Function &F) const override;

  /// PostRAScheduling is scheduled as part of PreSched2 passes.
  bool targetSchedulesPostRAScheduling() const override { return true; }
};

} // namespace llvm

#endif
