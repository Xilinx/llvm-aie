//==----AIE2PSTargetMachine.h -Define TargetMachine for AIE2ps ----*- C++-*-==//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the AIE2ps specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE2PSTARGETMACHINE_H
#define LLVM_LIB_TARGET_AIE_AIE2PSTARGETMACHINE_H

#include "AIE2PSSubtarget.h"
#include "AIEBaseTargetMachine.h"
#include "MCTargetDesc/aie2ps/AIE2PSMCTargetDesc.h"
#include "aie2p/AIE2PTargetMachine.h"
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"
#include <optional>

namespace llvm {

class AIE2PSTargetMachine : public AIEBaseTargetMachine {
  AIE2PSSubtarget Subtarget;
  virtual void anchor();

public:
  AIE2PSTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                      StringRef FS, const TargetOptions &Options,
                      std::optional<Reloc::Model> RM,
                      std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                      bool JIT);
  const AIE2PSSubtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }
  TargetTransformInfo getTargetTransformInfo(const Function &F) const override;
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  /// PostRAScheduling is scheduled as part of PreSched2 passes.
  bool targetSchedulesPostRAScheduling() const override { return true; }
};

} // namespace llvm

#endif
