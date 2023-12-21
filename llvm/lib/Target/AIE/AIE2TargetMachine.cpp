//===-- AIE2TargetMachine.cpp - Define TargetMachine for AIEngine V2 ------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Implements the info about AIEngine V2 target spec.
//
//===----------------------------------------------------------------------===//

#include "AIE2TargetMachine.h"
#include "AIE.h"
#include "AIE2.h"
#include "AIE2TargetTransformInfo.h"
#include "AIEBaseAliasAnalysis.h"
#include "AIEDumpArtifacts.h"
#include "AIEFinalizeBundle.h"
#include "AIEFormatSelector.h"
#include "AIEMachineBlockPlacement.h"
#include "AIETargetObjectFile.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/CodeGenPassBuilder.h"
#include "llvm/CodeGen/GlobalISel/IRTranslator.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelect.h"
#include "llvm/CodeGen/GlobalISel/Legalizer.h"
#include "llvm/CodeGen/GlobalISel/RegBankSelect.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/InitializePasses.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/PassRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Scalar.h"

using namespace llvm;

static cl::opt<bool>
    EnableCustomAliasAnalysis("aie-enable-alias-analysis",
                              cl::desc("Enable AIE alias analysis pass"),
                              cl::init(true), cl::Hidden);

static cl::opt<bool>
    EnableStagedRA("aie-staged-ra", cl::Hidden, cl::init(true),
                   cl::desc("Enable multi-stage register allocation"));
static cl::opt<bool>
    EnableSubregRenaming("aie-subreg-renaming", cl::Hidden, cl::init(false),
                         cl::desc("Enable RenameIndependentSubregs pass"));
static cl::opt<bool>
    EnableSuperRegSplitting("aie-split-superregs", cl::Hidden, cl::init(true),
                            cl::desc("Enable splitting super-regs into their "
                                     "smaller components to facilitate RA"));

extern bool AIEDumpArtifacts;

void AIE2TargetMachine::anchor() {}

AIE2TargetMachine::AIE2TargetMachine(const Target &T, const Triple &TT,
                                     StringRef CPU, StringRef FS,
                                     const TargetOptions &Options,
                                     std::optional<Reloc::Model> RM,
                                     std::optional<CodeModel::Model> CM,
                                     CodeGenOptLevel OL, bool JIT)
    : AIEBaseTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, false),
      Subtarget(TT, StringRef("aie2"), StringRef("aie2"), FS,
                Options.MCOptions.getABIName(), *this) {
  setGlobalISel(true);
  setFastISel(false);
  setGlobalISelAbort(GlobalISelAbortMode::Enable);
}

// AIE2 Pass Setup
class AIE2PassConfig final : public AIEPassConfig {
public:
  AIE2PassConfig(LLVMTargetMachine &TM, PassManagerBase &PM)
      : AIEPassConfig(TM, PM) {
    if (!EnableSubregRenaming)
      disablePass(&RenameIndependentSubregsID);
  }
  bool addPreISel() override;
  void addPreEmitPass() override;
  bool addInstSelector() override;
  void addPreRegAlloc() override;
  bool addRegAssignAndRewriteOptimized() override;
  void addPostRewrite() override;
  void addMachineLateOptimization() override;
  void addPreSched2() override;
  void addBlockPlacement() override;
  void addPreLegalizeMachineIR() override;
  void addPreRegBankSelect() override;
  void addIRPasses() override;
};

TargetPassConfig *AIE2TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new AIE2PassConfig(*this, PM);
}

void AIE2TargetMachine::registerDefaultAliasAnalyses(AAManager &AAM) {
  if (EnableCustomAliasAnalysis)
    AAM.registerFunctionAnalysis<AIEBaseAA>();
}

bool AIE2PassConfig::addPreISel() {
  if (TM->getOptLevel() != CodeGenOptLevel::None)
    addPass(createHardwareLoopsLegacyPass());
  return false;
}

void AIE2PassConfig::addPreEmitPass() {}

void AIE2PassConfig::addIRPasses() {
  // Always expand atomic operations, we don't deal with atomicrmw or cmpxchg
  // ourselves.
  addPass(createAtomicExpandPass());

  if (TM->getOptLevel() > CodeGenOptLevel::None) {

    if (EnableCustomAliasAnalysis) {
      addPass(createAIEBaseAAWrapperPass());
      addPass(
          createExternalAAWrapperPass([](Pass &P, Function &, AAResults &AAR) {
            if (auto *WrapperPass =
                    P.getAnalysisIfAvailable<AIEBaseAAWrapperPass>())
              AAR.addAAResult(WrapperPass->getResult());
          }));
    }
  }
  TargetPassConfig::addIRPasses();
}

void AIE2PassConfig::addPreLegalizeMachineIR() {
  if (getOptLevel() != CodeGenOptLevel::None)
    addPass(createAIE2PreLegalizerCombiner());
}

void AIE2PassConfig::addPreRegBankSelect() {
  if (getOptLevel() != CodeGenOptLevel::None) {
    addPass(createAIE2PostLegalizerGenericCombiner());
    addPass(createAIEClusterBaseAddress());
    addPass(createAIE2PostLegalizerCustomCombiner());
  }
}

void AIE2PassConfig::addPreRegAlloc() {
  if (getOptLevel() >= CodeGenOptLevel::Default) {
    // Perform software pipelining
    addPass(&MachinePipelinerID);
    // Remove unused debris afer SWP
    addPass(&DeadMachineInstructionElimID);
  }
  insertPass(&PHIEliminationID, &AIESubRegConstrainerID);
  if (AIEDumpArtifacts) {
    addPass(createDumpModulePass(/*Suffix=*/"before-ra"));
    addPass(createMachineFunctionDumperPass(/*Suffix=*/"before-ra"));
  }
}

bool AIE2PassConfig::addRegAssignAndRewriteOptimized() {
  if (!EnableStagedRA && !EnableSuperRegSplitting)
    return TargetPassConfig::addRegAssignAndRewriteOptimized();

  // Rewrite instructions which use large tuple regs into _split variants
  // to better expose sub-registers and facilitate RA.
  if (EnableSuperRegSplitting)
    addPass(createAIESplitInstrBuilder());

  // TODO: Actually do staged RA here
  addPass(createGreedyRegisterAllocator());
  addPass(createVirtRegRewriter());

  return true;
}

void AIE2PassConfig::addPostRewrite() {
  if (getOptLevel() != CodeGenOptLevel::None && EnableSuperRegSplitting) {
    // Rewrite _split instructions which were used to facilitate RA.
    // Now we want the real "target" instructions with encoding and scheduling
    // information.
    addPass(createAIESplitInstrReplacer());
  }
}

void AIE2PassConfig::addMachineLateOptimization() {
  TargetPassConfig::addMachineLateOptimization();
  // Run MachineCopyPropagation again, but take into account
  // architecture-specific mov operations using isMoveReg (see isCopyInstrImpl
  // hook)
  addPass(createMachineCopyPropagationPass(true));
}

void AIE2PassConfig::addPreSched2() {
  // Remove dead code after PostRA Pseudo Instruction Expansion Pass.
  addPass(&DeadMachineInstructionElimID);
  if (getOptLevel() != CodeGenOptLevel::None)
    addPass(&MachineBlockPlacementID);
  // As it is now, this just sets block alignment, which is a necessary
  // step
  addPass(createAIEMachineBlockPlacement());

  if (TM->getOptLevel() != CodeGenOptLevel::None)
    addPass(createAIEBaseHardwareLoopsPass());

  addPass(createAIEPseudoBranchExpansion());
  if (AIEDumpArtifacts)
    addPass(createMachineFunctionDumperPass(/*Suffix=*/"before-post-ra-sched"));
  // PostRAScheduler is required to insert NoOps for correctness.
  // We always run it, independently of the Opt level.
  addPass(&PostMachineSchedulerID);
  // After scheduling, create the bundles from the BundleWithPred flags
  addPass(createAIEFinalizeBundle());
  if (AIEDumpArtifacts)
    addPass(createMachineFunctionDumperPass(/*Suffix=*/"after-post-ra-sched"));
}

void AIE2PassConfig::addBlockPlacement() {
  // Block placement is done pre-scheduling.
  return;
}

TargetTransformInfo
AIE2TargetMachine::getTargetTransformInfo(const Function &F) const {
  return TargetTransformInfo(AIE2TTIImpl(this, F));
}

bool AIE2PassConfig::addInstSelector() {
  if (AIEDumpArtifacts)
    addPass(createMachineFunctionDumperPass(/*Suffix=*/"before-isel"));
  addPass(createAIE2ISelDag(getAIETargetMachine()));
  if (AIEDumpArtifacts)
    addPass(createMachineFunctionDumperPass(/*Suffix=*/"after-isel"));
  return false;
}

void AIE2TargetMachine::registerPassBuilderCallbacks(PassBuilder &PB) {
  if (EnableCustomAliasAnalysis) {
    PB.registerAnalysisRegistrationCallback([](FunctionAnalysisManager &FAM) {
      FAM.registerPass([&] { return AIEBaseAA(); });
    });
    PB.registerParseAACallback([](StringRef AAName, AAManager &AAM) {
      if (AAName == "aie-aa") {
        AAM.registerFunctionAnalysis<AIEBaseAA>();
        return true;
      }
      return false;
    });
  }
}
