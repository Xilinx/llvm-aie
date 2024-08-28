//===-- AIEBaseTargetMachine.h - Common AIE Target Machine ------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains common TargetMachine and TargetPassConfig code
// between AIE versions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_BASETARGETMACHINE_H
#define LLVM_LIB_TARGET_AIE_BASETARGETMACHINE_H

#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"
#include <optional>

namespace llvm {

class AIEBaseTargetMachine : public LLVMTargetMachine {
protected:
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  bool EnableCustomAliasAnalysis = true;

public:
  AIEBaseTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                       StringRef FS, const TargetOptions &Options,
                       std::optional<Reloc::Model> RM,
                       std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                       bool JIT);

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
  yaml::MachineFunctionInfo *createDefaultFuncInfoYAML() const override;
  yaml::MachineFunctionInfo *
  convertFuncInfoToYAML(const MachineFunction &MF) const override;
  bool parseMachineFunctionInfo(const yaml::MachineFunctionInfo &,
                                PerFunctionMIParsingState &PFS,
                                SMDiagnostic &Error,
                                SMRange &SourceRange) const override;

  MachineFunctionInfo *
  createMachineFunctionInfo(BumpPtrAllocator &Allocator, const Function &F,
                            const TargetSubtargetInfo *STI) const override;

  void registerDefaultAliasAnalyses(AAManager &) override;
  void registerPassBuilderCallbacks(PassBuilder &PB,
                                    bool PopulateClassToPassNames) override;
  bool isNoopAddrSpaceCast(unsigned SrcAS, unsigned DestAS) const override;
};

class AIEBasePassConfig : public TargetPassConfig {
protected:
  bool EnableCustomAliasAnalysis = true;

public:
  AIEBasePassConfig(LLVMTargetMachine &TM, PassManagerBase &PM);

  bool addIRTranslator() override;
  bool addLegalizeMachineIR() override;
  bool addRegBankSelect() override;
  bool addGlobalInstructionSelect() override;
  void addIRPasses() override;
  void addMachineLateOptimization() override;
  bool addInstSelector() override;
  void addPreEmitPass() override;
  void addPreEmitPass2() override;
  void addPreRegAlloc() override;
  void addPreSched2() override;

  ScheduleDAGInstrs *
  createPostMachineScheduler(MachineSchedContext *C) const override;

  ScheduleDAGInstrs *
  createMachineScheduler(MachineSchedContext *C) const override;

  std::unique_ptr<CSEConfigBase> getCSEConfig() const override;
};

} // namespace llvm

#endif
