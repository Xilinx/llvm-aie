//===-- AIETargetMachine.cpp - Define TargetMachine for AIE -----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Implements the info about AIE target spec.
//
//===----------------------------------------------------------------------===//

#include "AIETargetMachine.h"
#include "AIE.h"
#include "AIE2TargetMachine.h"
#include "AIEDumpArtifacts.h"
#include "AIEFinalizeBundle.h"
#include "AIEMachineBlockPlacement.h"
#include "AIETargetTransformInfo.h"
#include "TargetInfo/AIETargetInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/CSEConfigBase.h"
#include "llvm/CodeGen/GlobalISel/CSEInfo.h"
#include "llvm/CodeGen/GlobalISel/IRTranslator.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelect.h"
#include "llvm/CodeGen/GlobalISel/Legalizer.h"
#include "llvm/CodeGen/GlobalISel/RegBankSelect.h"
#include "llvm/CodeGen/MIRParser/MIParser.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/InitializePasses.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/PassRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/IPO/GlobalDCE.h"
#include "llvm/Transforms/IPO/Internalize.h"
#include "llvm/Transforms/Scalar.h"

using namespace llvm;

extern bool AIEDumpArtifacts;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeAIETarget() {
  RegisterTargetMachine<AIETargetMachine> X(getTheAIETarget());
  RegisterTargetMachine<AIE2TargetMachine> Y(getTheAIE2Target());
  //  auto PR = PassRegistry::getPassRegistry();
  //  initializeAIEExpandPseudoPass(*PR);
  auto *PR = PassRegistry::getPassRegistry();
  initializeGlobalISel(*PR);
  initializeAIEAddressSpaceFlatteningPass(*PR);
  initializeAIEEliminateDuplicatePHIPass(*PR);
  initializeAIEClusterBaseAddressPass(*PR);
  initializeAIE2PreLegalizerCombinerPass(*PR);
  initializeAIE2PostLegalizerGenericCombinerPass(*PR);
  initializeAIE2PostLegalizerCustomCombinerPass(*PR);
  initializeAIEPostSelectOptimizePass(*PR);
  initializeAIEPseudoBranchExpansionPass(*PR);
  initializeAIESubRegConstrainerPass(*PR);
  initializeAIESuperRegRewriterPass(*PR);
  initializeAIEWawRegRewriterPass(*PR);
  initializeAIEFinalizeBundlePass(*PR);
  initializeAIEMachineAlignmentPass(*PR);
  initializeAIEMachineBlockPlacementPass(*PR);
  initializeAIEBaseHardwareLoopsPass(*PR);
  initializeAIEBaseAAWrapperPassPass(*PR);
  initializeAIEBaseExternalAAWrapperPass(*PR);
  initializeAIESplitInstrBuilderPass(*PR);
  initializeAIESplitInstrReplacerPass(*PR);
  initializeReservedRegsLICMPass(*PR);
}

AIETargetMachine::AIETargetMachine(const Target &T, const Triple &TT,
                                   StringRef CPU, StringRef FS,
                                   const TargetOptions &Options,
                                   std::optional<Reloc::Model> RM,
                                   std::optional<CodeModel::Model> CM,
                                   CodeGenOptLevel OL, bool JIT)
    : AIEBaseTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, false),
      // FIXME: Note that the 'aie' CPU is hardcoded below.  This should not
      // be necessary, but for some reason the CPU passed in is empty.
      Subtarget(TT, StringRef("aie"), StringRef("aie"), FS,
                Options.MCOptions.getABIName(), *this) {}

// AIE1 Pass Setup
class AIEPassConfig final : public AIEBasePassConfig {
public:
  AIEPassConfig(LLVMTargetMachine &TM, PassManagerBase &PM)
      : AIEBasePassConfig(TM, PM) {}

  AIETargetMachine &getAIETargetMachine() const {
    return getTM<AIETargetMachine>();
  }

  bool addInstSelector() override;
  void addPreEmitPass() override;
};

TargetPassConfig *AIETargetMachine::createPassConfig(PassManagerBase &PM) {
  return new AIEPassConfig(*this, PM);
}

TargetTransformInfo
AIETargetMachine::getTargetTransformInfo(const Function &F) const {
  return TargetTransformInfo(AIETTIImpl(this, F));
}

bool AIEPassConfig::addInstSelector() {
  addPass(createAIEISelDag(getAIETargetMachine()));
  return false;
}

void AIEPassConfig::addPreEmitPass() {
  addPass(createAIEDelaySlotFillerPass(getAIETargetMachine()));
  // As it is now, this just sets block alignment, which is a necessary
  // step
  addPass(createAIEMachineBlockPlacement());
}
