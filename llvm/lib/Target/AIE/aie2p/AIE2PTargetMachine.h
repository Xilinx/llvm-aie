//==-----AIE2PTargetMachine.h -Define TargetMachine for AIE2p ----*- C++-*-==//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the AIE2p specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE2PTARGETMACHINE_H
#define LLVM_LIB_TARGET_AIE_AIE2PTARGETMACHINE_H

#include "AIE2PSubtarget.h"
#include "MCTargetDesc/aie2p/AIE2PMCTargetDesc.h"
#include "aie2/AIE2TargetMachine.h"
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"
#include <optional>

namespace llvm {

class AIE2PTargetMachine : public AIEBaseTargetMachine {
  AIE2PSubtarget Subtarget;
  virtual void anchor();

public:
  AIE2PTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     std::optional<Reloc::Model> RM,
                     std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                     bool JIT);
  const AIE2PSubtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }
  TargetTransformInfo getTargetTransformInfo(const Function &F) const override;
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  /// PostRAScheduling is scheduled as part of PreSched2 passes.
  bool targetSchedulesPostRAScheduling() const override { return true; }
};

// AIE2P Pass Setup
class AIE2PPassConfig : public AIE2PassConfig {
public:
  AIE2PPassConfig(TargetMachine &TM, PassManagerBase &PM)
      : AIE2PassConfig(TM, PM) {}
  void addPreRegBankSelect() override;
  void addPreLegalizeMachineIR() override;
  bool addRegAssignAndRewriteOptimized() override;
};

} // namespace llvm

#endif
