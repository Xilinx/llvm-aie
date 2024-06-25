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
#include "AIEBaseAliasAnalysis.h"
#include "AIEDumpArtifacts.h"
#include "AIEFinalizeBundle.h"
#include "AIEMachineBlockPlacement.h"
#include "AIEMachineFunctionInfo.h"
#include "AIEMachineScheduler.h"
#include "AIETargetObjectFile.h"
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
using namespace llvm;

static cl::opt<bool>
    EnableCustomAliasAnalysis("aie-enable-alias-analysis",
                              cl::desc("Enable AIE alias analysis pass"),
                              cl::init(true), cl::Hidden);

// Option to run internalize pass.
static cl::opt<bool> InternalizeSymbols(
    "aie-internalize-symbols",
    cl::desc("Enable elimination of non-kernel functions and unused globals"),
    cl::init(false), cl::Hidden);
// Option to skip the functions we don't want to internalize.
static cl::list<std::string>
    FunctionSkipList("aie-internalize-skip-functions",
                     cl::desc("List of function names to skip internalization"),
                     cl::Hidden, cl::list_init<std::string>({"main"}),
                     cl::CommaSeparated);

extern bool AIEDumpArtifacts;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeAIETarget() {
  RegisterTargetMachine<AIETargetMachine> X(getTheAIETarget());
  RegisterTargetMachine<AIE2TargetMachine> Y(getTheAIE2Target());
  //  auto PR = PassRegistry::getPassRegistry();
  //  initializeAIEExpandPseudoPass(*PR);
  auto *PR = PassRegistry::getPassRegistry();
  initializeGlobalISel(*PR);
  initializeAIEClusterBaseAddressPass(*PR);
  initializeAIE2PreLegalizerCombinerPass(*PR);
  initializeAIE2PostLegalizerGenericCombinerPass(*PR);
  initializeAIE2PostLegalizerCustomCombinerPass(*PR);
  initializeAIEPostSelectOptimizePass(*PR);
  initializeAIEPseudoBranchExpansionPass(*PR);
  initializeAIESubRegConstrainerPass(*PR);
  initializeAIESuperRegRewriterPass(*PR);
  initializeAIEFinalizeBundlePass(*PR);
  initializeAIEMachineAlignmentPass(*PR);
  initializeAIEMachineBlockPlacementPass(*PR);
  initializeAIEBaseHardwareLoopsPass(*PR);
  initializeAIEBaseAAWrapperPassPass(*PR);
  initializeAIEBaseExternalAAWrapperPass(*PR);
  initializeAIESplitInstrBuilderPass(*PR);
  initializeAIESplitInstrReplacerPass(*PR);
}

static StringRef computeDataLayout(const Triple &TT) {
  return "e-m:e-p:20:32-i1:8:32-i8:8:32-i16:16:32-i32:32:32-f32:32:32-i64:32-"
         "f64:32-a:0:32-n32";
}

static Reloc::Model getEffectiveRelocModel(const Triple &TT,
                                           std::optional<Reloc::Model> RM) {
  if (!RM.has_value())
    return Reloc::Static;
  // AIE does not support PIC code.  If PIC code is asked for then just ignore
  // it. The main user of this is compiling the builtin library, which asks for
  // PIC.
  if (*RM == Reloc::PIC_)
    return Reloc::Static;
  return *RM;
}

AIEBaseTargetMachine::AIEBaseTargetMachine(const Target &T, const Triple &TT,
                                           StringRef CPU, StringRef FS,
                                           const TargetOptions &Options,
                                           std::optional<Reloc::Model> RM,
                                           std::optional<CodeModel::Model> CM,
                                           CodeGenOptLevel OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(TT), TT, CPU, FS, Options,
                        getEffectiveRelocModel(TT, RM),
                        getEffectiveCodeModel(CM, CodeModel::Small), OL),
      TLOF(std::make_unique<AIEELFTargetObjectFile>()) {
  initAsmInfo();
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

TargetPassConfig *AIETargetMachine::createPassConfig(PassManagerBase &PM) {
  return new AIEPassConfig(*this, PM);
}

void AIEPassConfig::addIRPasses() {
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

void AIEPassConfig::addMachineLateOptimization() {
  TargetPassConfig::addMachineLateOptimization();
  // Run MachineCopyPropagation again, but take into account
  // architecture-specific mov operations using isMoveReg (see isCopyInstrImpl
  // hook)
  addPass(createMachineCopyPropagationPass(true));
}

TargetTransformInfo
AIETargetMachine::getTargetTransformInfo(const Function &F) const {
  return TargetTransformInfo(AIETTIImpl(this, F));
}

bool AIEPassConfig::addInstSelector() {
  addPass(createAIEISelDag(getAIETargetMachine()));
  return false;
}

bool AIEPassConfig::addIRTranslator() {
  addPass(new IRTranslator(getOptLevel()));
  return false;
}

bool AIEPassConfig::addLegalizeMachineIR() {
  addPass(new Legalizer());
  return false;
}

bool AIEPassConfig::addRegBankSelect() {
  addPass(new RegBankSelect());
  return false;
}

bool AIEPassConfig::addGlobalInstructionSelect() {
  addPass(new InstructionSelect(getOptLevel()));
  return false;
}

void AIEPassConfig::addPreEmitPass() {
  addPass(createAIEDelaySlotFillerPass(getAIETargetMachine()));
  // As it is now, this just sets block alignment, which is a necessary
  // step
  addPass(createAIEMachineBlockPlacement());
}

void AIEPassConfig::addPreSched2() {
  // PostRAScheduler is required to insert NoOps for correctness.
  // We always run it, independently of the Opt level.
  addPass(&PostRASchedulerID);
  // After scheduling, create the bundles from the BundleWithPred flags
  addPass(&FinalizeMachineBundlesID);
}

void AIEPassConfig::addPreEmitPass2() {}

void AIEPassConfig::addPreRegAlloc() {}

ScheduleDAGInstrs *
AIEPassConfig::createPostMachineScheduler(MachineSchedContext *C) const {
  ScheduleDAGMI *DAG =
      new AIEScheduleDAGMI(C, std::make_unique<AIEPostRASchedStrategy>(C),
                           /* RemoveKillFlags=*/true);
  for (auto &Mutation :
       AIEBaseSubtarget::getPostRAMutationsImpl(TM->getTargetTriple()))
    DAG->addMutation(std::move(Mutation));
  return DAG;
}

ScheduleDAGInstrs *
AIEPassConfig::createMachineScheduler(MachineSchedContext *C) const {
  ScheduleDAGMILive *DAG =
      new AIEScheduleDAGMILive(C, std::make_unique<AIEPreRASchedStrategy>(C));
  DAG->addMutation(createCopyConstrainDAGMutation(DAG->TII, DAG->TRI));

  for (auto &Mutation :
       AIEBaseSubtarget::getPreRAMutationsImpl(TM->getTargetTriple()))
    DAG->addMutation(std::move(Mutation));
  return DAG;
}

yaml::MachineFunctionInfo *
AIEBaseTargetMachine::createDefaultFuncInfoYAML() const {
  return new yaml::AIEMachineFunctionInfo();
}

yaml::MachineFunctionInfo *
AIEBaseTargetMachine::convertFuncInfoToYAML(const MachineFunction &MF) const {
  const auto *MFI = MF.getInfo<AIEMachineFunctionInfo>();
  return new yaml::AIEMachineFunctionInfo(*MFI);
}

bool AIEBaseTargetMachine::parseMachineFunctionInfo(
    const yaml::MachineFunctionInfo &MFI, PerFunctionMIParsingState &PFS,
    SMDiagnostic &Error, SMRange &SourceRange) const {
  const auto &YamlMFI =
      reinterpret_cast<const yaml::AIEMachineFunctionInfo &>(MFI);
  MachineFunction &MF = PFS.MF;
  MF.getInfo<AIEMachineFunctionInfo>()->initializeBaseYamlFields(YamlMFI);
  return false;
}

MachineFunctionInfo *AIEBaseTargetMachine::createMachineFunctionInfo(
    BumpPtrAllocator &Allocator, const Function &F,
    const TargetSubtargetInfo *STI) const {
  return new AIEMachineFunctionInfo(F, STI, *this);
}

std::unique_ptr<CSEConfigBase> AIEPassConfig::getCSEConfig() const {
  // We don't want CSE to run at -O0, as it introduces constrained register
  // operands (r27) that RegAllocFast is not able to resolve.
  if (TM->getOptLevel() == CodeGenOptLevel::None)
    return std::make_unique<CSEConfigBase>();
  return getStandardCSEConfigForOpt(TM->getOptLevel());
}

void AIEBaseTargetMachine::registerDefaultAliasAnalyses(AAManager &AAM) {
  if (EnableCustomAliasAnalysis)
    AAM.registerFunctionAnalysis<AIEBaseAA>();
}

/// Predicate for Internalize pass.
/// Preserve functions for names passed via FunctionSkipList. These are usually
/// entry points which shouldn't be internalized to let linker know of them
/// (default entry point is 'main'). Preserve function declarations, as if we
/// internalize a declaration, it would not be accessible outside of its
/// translation unit, which could lead to linker errors if the function is
/// used/defined elsewhere.
static bool mustPreserveGV(const GlobalValue &GV) {
  if (const Function *F = dyn_cast<Function>(&GV)) {
    bool Skip = llvm::any_of(FunctionSkipList, [&](const std::string &Name) {
      return F->getName().equals(Name);
    });
    return F->isDeclaration() || Skip;
  }

  GV.removeDeadConstantUsers();
  return !GV.use_empty();
}

void AIEBaseTargetMachine::registerPassBuilderCallbacks(
    PassBuilder &PB, bool PopulateClassToPassNames) {
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

  if (InternalizeSymbols) {
    PB.registerPipelineEarlySimplificationEPCallback(
        [](ModulePassManager &PM, OptimizationLevel) {
          if (InternalizeSymbols) {
            PM.addPass(InternalizePass(mustPreserveGV));
            PM.addPass(GlobalDCEPass());
          }
        });
  }
}
