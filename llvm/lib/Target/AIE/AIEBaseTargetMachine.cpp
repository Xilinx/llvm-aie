//===-- AIEBaseTargetMachine.cpp - AIE Target Machine -----------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains common implementation  of TargetMachine and
// TargetPassConfig code between AIE versions.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseTargetMachine.h"
#include "AIE.h"
#include "AIE2TargetMachine.h"
#include "AIEBaseAliasAnalysis.h"
#include "AIECombiners.h"
#include "AIEMachineFunctionInfo.h"
#include "AIEMachineScheduler.h"
#include "AIETargetObjectFile.h"
#include "TargetInfo/AIETargetInfo.h"
#include "aie1/AIE1TargetMachine.h"
#include "aie2p/AIE2PTargetMachine.h"
#include "aie2ps/AIE2PSTargetMachine.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/CSEConfigBase.h"
#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"
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
#include "llvm/Pass.h"
#include "llvm/PassRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/IPO/GlobalDCE.h"
#include "llvm/Transforms/IPO/Internalize.h"
#include "llvm/Transforms/IPO/SCCP.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Vectorize/LoadStoreVectorizer.h"

using namespace llvm;

extern cl::opt<unsigned> JumpInstCost;
extern cl::opt<unsigned> MisfetchCost;
extern cl::opt<bool> ForcePreciseRotationCost;

static cl::opt<bool>
    EnableCustomAliasAnalysisOpt("aie-enable-alias-analysis",
                                 cl::desc("Enable AIE alias analysis pass"),
                                 cl::init(true), cl::Hidden);

static cl::opt<bool>
    EnableTailMergingOpt("aie-enable-tail-merge",
                         cl::desc("Enable tail merging for AIE."),
                         cl::init(false), cl::Hidden);

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

cl::opt<bool> EnableAddressChaining("aie-address-chaining", cl::Hidden,
                                    cl::init(true),
                                    cl::desc("Enable ptradd chaining."));

// Option to run IPSCCP once more.
cl::opt<bool> EnableIPSCCP("aie-enable-ipsccp", cl::Hidden, cl::init(true),
                           cl::desc("Enable another run of IPSCCP."));

cl::opt<bool> EnableGlobalPtrModOptimizer(
    "aie-global-ptr-mod-opt", cl::Hidden, cl::init(true),
    cl::desc("Enable global pointer modifier optimization."));

cl::opt<bool>
    EnableStagedRA("aie-staged-ra", cl::Hidden, cl::init(true),
                   cl::desc("Enable multi-stage register allocation"));

cl::opt<bool> EnableFineGrainedStagedRA(
    "aie-staged-ra-fine-grained-alloc", cl::Hidden, cl::init(true),
    cl::desc("Enable multi-stage register allocation with fine-grained "
             "selection of live intervals"));

cl::opt<bool>
    EnableWAWRegRewrite("aie-wawreg-rewrite",
                        cl::desc("Enable the WAW Register Renaming in loops"),
                        cl::init(true), cl::Hidden);

cl::opt<bool>
    EnableSuperRegSplitting("aie-split-superregs", cl::Hidden, cl::init(true),
                            cl::desc("Enable splitting super-regs into their "
                                     "smaller components to facilitate RA"));
cl::opt<bool>
    AllocateMRegsFirst("aie-mod-ra-first", cl::Hidden, cl::init(false),
                       cl::desc("Allocate M registers first in staged RA."));
cl::opt<bool> EnablePreMISchedCoalescer(
    "aie-premisched-coalescer", cl::Hidden, cl::init(true),
    cl::desc("Run the coalescer again after the pre-RA scheduler"));

cl::opt<bool> SimplifyCRSRRegs(
    "aie-simplify-crsr-edges", cl::Hidden, cl::init(true),
    cl::desc("Allow simplifying redundant CR and SR reg assignments"));

cl::opt<bool>
    EnableAIEIfConversion("aie-enable-if-conversion",
                          cl::desc("Enable If Conversion optimization"),
                          cl::init(true), cl::Hidden);

cl::opt<bool>
    VectorizePartWordStores("aie-enable-part-store-vect",
                            cl::desc("Enable Part-word store vectorization"),
                            cl::init(true), cl::Hidden);

static StringRef computeDataLayout(const Triple &TT) {
  return "e-m:e-p:20:32-i1:8:32-i8:8:32-i16:16:32-i32:32:32-f32:32:32-i64:32-"
         "f64:32-a:0:32-n32";
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeAIETarget() {
  RegisterTargetMachine<AIETargetMachine> X(getTheAIETarget());
  RegisterTargetMachine<AIE2TargetMachine> Y(getTheAIE2Target());
  RegisterTargetMachine<AIE2PTargetMachine> A(getTheAIE2PTarget());
  RegisterTargetMachine<AIE2PSTargetMachine> B(getTheAIE2PSTarget());
  //  auto PR = PassRegistry::getPassRegistry();
  //  initializeAIEExpandPseudoPass(*PR);
  auto *PR = PassRegistry::getPassRegistry();
  initializeGlobalISel(*PR);
  initializeAIEAddressSpaceFlatteningPass(*PR);
  initializeAIEEliminateDuplicatePHIPass(*PR);
  initializeAIEClusterBaseAddressPass(*PR);
  initializeAIEPtrModOptimizerPass(*PR);
  initializeAIEPreLegalizerCombinerPass(*PR);
  initializeAIEPostLegalizerGenericCombinerPass(*PR);
  initializeAIEPostLegalizerCustomCombinerPass(*PR);
  initializeAIEPostLegalizerFinalCombinerPass(*PR);
  initializeAIEPreISelCombinerPass(*PR);
  initializeAIEPostSelectOptimizePass(*PR);
  initializeAIEPseudoBranchExpansionPass(*PR);
  initializeAIESubRegConstrainerPass(*PR);
  initializeAIESuperRegRewriterPass(*PR);
  initializeAIEUnallocatedSuperRegRewriterPass(*PR);
  initializeAIEWawRegRewriterPass(*PR);
  initializeAIEOutlineMemoryGEPPass(*PR);
  initializeAIEFinalizeBundlePass(*PR);
  initializeAIEMachineAlignmentPass(*PR);
  initializeAIE1MachineBlockPlacementPass(*PR);
  initializeAIEBaseHardwareLoopsPass(*PR);
  initializeAIEBaseAAWrapperPassPass(*PR);
  initializeAIEBaseExternalAAWrapperPass(*PR);
  initializeAIESplitInstrBuilderPass(*PR);
  initializeAIESplitInstrReplacerPass(*PR);
  initializeAIERegClassConstrainerPass(*PR);
  initializeReservedRegsLICMPass(*PR);
  initializeAIEOuterLoopPipelinerPass(*PR);
  initializeAIESpillSlotOptimizationPass(*PR);
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
    : CodeGenTargetMachineImpl(T, computeDataLayout(TT), TT, CPU, FS, Options,
                               getEffectiveRelocModel(TT, RM),
                               getEffectiveCodeModel(CM, CodeModel::Small), OL),
      TLOF(std::make_unique<AIEELFTargetObjectFile>()) {
  initAsmInfo();
  EnableCustomAliasAnalysis = EnableCustomAliasAnalysisOpt;

  setMBBPlacementOpts();
}

void AIEBaseTargetMachine::setMBBPlacementOpts() {
  // 5 Branch Delay Slots
  JumpInstCost = 5;
  // No cache, so no Misfetch costs.
  MisfetchCost = 0;
  // Attempt to rotate loops based on profile data to reduce branch costs.
  ForcePreciseRotationCost = true;
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
  return new (Allocator.Allocate<AIEMachineFunctionInfo>())
      AIEMachineFunctionInfo(F, STI, *this);
}

bool AIEBaseTargetMachine::isNoopAddrSpaceCast(unsigned SrcAS,
                                               unsigned DestAS) const {
  // AIE address space is used for bank annotation only.
  // aie-addrspace-flattening pass retyped pointer with a AS to default AS.
  return true;
}

void AIEBaseTargetMachine::registerDefaultAliasAnalyses(AAManager &AAM) {
  if (EnableCustomAliasAnalysis)
    AAM.registerFunctionAnalysis<AIEBaseAA>();
}

/// Predicate for Internalize pass.
/// Preserve functions that can be an entry point or that have uses within the
/// Module.
static bool mustPreserveGV(const GlobalValue &GV) {
  if (const Function *F = dyn_cast<Function>(&GV)) {
    bool Skip = llvm::any_of(FunctionSkipList, [&](const std::string &Name) {
      return F->getName() == Name;
    });
    return F->isDeclaration() || Skip;
  }

  GV.removeDeadConstantUsers();
  return !GV.use_empty();
}

void AIEBaseTargetMachine::registerPassBuilderCallbacks(PassBuilder &PB) {
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
        [](ModulePassManager &PM, OptimizationLevel, ThinOrFullLTOPhase) {
          if (InternalizeSymbols) {
            PM.addPass(InternalizePass(mustPreserveGV));
            PM.addPass(GlobalDCEPass());
          }
        });
  }

  if (EnableIPSCCP) {
    PB.registerOptimizerEarlyEPCallback(
        [](ModulePassManager &PM, OptimizationLevel, ThinOrFullLTOPhase) {
          PM.addPass(IPSCCPPass(IPSCCPOptions(/*AllowFuncSpec=*/false)));
        });
  }
}

AIEBasePassConfig::AIEBasePassConfig(TargetMachine &TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {
  EnableTailMerge = EnableTailMergingOpt;
  EnableCustomAliasAnalysis = EnableCustomAliasAnalysisOpt;
}

void AIEBasePassConfig::addIRPasses() {
  // Always expand atomic operations, we don't deal with atomicrmw or cmpxchg
  // ourselves.
  addPass(createAtomicExpandLegacyPass());

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
  if (TM->getOptLevel() > CodeGenOptLevel::None)
    addPass(createInferAddressSpacesPass());
  TargetPassConfig::addIRPasses();
  if (TM->getOptLevel() > CodeGenOptLevel::None) {
    if (VectorizePartWordStores && !TM->getTargetTriple().isAIE1())
      addPass(createLoadStoreVectorizerPass());
  }
}

void AIEBasePassConfig::addMachineLateOptimization() {
  TargetPassConfig::addMachineLateOptimization();
  // Run MachineCopyPropagation again, but take into account
  // architecture-specific mov operations using isMoveReg (see isCopyInstrImpl
  // hook)
  addPass(createMachineCopyPropagationPass(true));
}

bool AIEBasePassConfig::addIRTranslator() {
  addPass(new IRTranslator(getOptLevel()));
  return false;
}

bool AIEBasePassConfig::addLegalizeMachineIR() {
  addPass(new Legalizer());
  return false;
}

bool AIEBasePassConfig::addRegBankSelect() {
  addPass(new RegBankSelect());
  return false;
}

bool AIEBasePassConfig::addGlobalInstructionSelect() {
  addPass(new InstructionSelect(getOptLevel()));
  return false;
}

bool AIEBasePassConfig::addInstSelector() { return false; }

void AIEBasePassConfig::addPreEmitPass() {}

void AIEBasePassConfig::addPreEmitPass2() {}

void AIEBasePassConfig::addPreRegAlloc() {}

void AIEBasePassConfig::addPreSched2() {
  // PostRAScheduler is required to insert NoOps for correctness.
  // We always run it, independently of the Opt level.
  addPass(&PostRASchedulerID);
  // After scheduling, create the bundles from the BundleWithPred flags
  addPass(&FinalizeMachineBundlesID);
}

ScheduleDAGInstrs *
AIEBaseTargetMachine::createPostMachineScheduler(MachineSchedContext *C) const {
  ScheduleDAGMI *DAG =
      new AIEScheduleDAGMI(C, std::make_unique<AIEPostRASchedStrategy>(C),
                           /* RemoveKillFlags=*/true);
  for (auto &Mutation :
       AIEBaseSubtarget::getPostRAMutationsImpl(getTargetTriple(), C->AA))
    DAG->addMutation(std::move(Mutation));
  return DAG;
}

ScheduleDAGInstrs *
AIEBaseTargetMachine::createMachineScheduler(MachineSchedContext *C) const {
  ScheduleDAGMILive *DAG =
      new AIEScheduleDAGMILive(C, std::make_unique<AIEPreRASchedStrategy>(C));
  DAG->addMutation(createCopyConstrainDAGMutation(DAG->TII, DAG->TRI));

  for (auto &Mutation :
       AIEBaseSubtarget::getPreRAMutationsImpl(getTargetTriple()))
    DAG->addMutation(std::move(Mutation));
  return DAG;
}

std::unique_ptr<CSEConfigBase> AIEBasePassConfig::getCSEConfig() const {
  // We don't want CSE to run at -O0, as it introduces constrained register
  // operands (r27) that RegAllocFast is not able to resolve.
  if (TM->getOptLevel() == CodeGenOptLevel::None)
    return std::make_unique<CSEConfigBase>();
  return getStandardCSEConfigForOpt(TM->getOptLevel());
}
