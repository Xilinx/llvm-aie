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
#include "AIEMachineAlignment.h"
#include "AIEMachineBlockPlacement.h"
#include "AIEMachineFunctionInfo.h"
#include "AIETargetObjectFile.h"
#include "llvm/ADT/STLExtras.h"
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
#include "llvm/Support/FormattedStream.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Scalar.h"

using namespace llvm;

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

static cl::opt<bool>
    EnableWAWRegRewrite("aie-wawreg-rewrite",
                        cl::desc("Enable the WAW Register Renaming in loops"),
                        cl::init(true), cl::Hidden);
static cl::opt<bool>
    EnableReservedRegsLICM("aie-reserved-regs-licm", cl::Hidden, cl::init(true),
                           cl::desc("Enable LICM for some reserved registers"));
static cl::opt<bool>
    AllocateMRegsFirst("aie-mod-ra-first", cl::Hidden, cl::init(false),
                       cl::desc("Allocate M registers first in staged RA."));
static cl::opt<bool> EnablePreMISchedCoalescer(
    "aie-premisched-coalescer", cl::Hidden, cl::init(true),
    cl::desc("Run the coalescer again after the pre-RA scheduler"));

static cl::opt<unsigned> StackAddrSpace(
    "aie-stack-addrspace", cl::init(0),
    cl::desc("Specify the addrspace where the stack is allocated "
             "(5: Bank A, 6: Bank B, 7: Bank C, 8: Bank D)"));

static cl::opt<bool> EnableAddressChaining("aie-address-chaining", cl::Hidden,
                                           cl::init(true),
                                           cl::desc("Enable ptradd chaining."));

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
class AIE2PassConfig final : public AIEBasePassConfig {
public:
  AIE2PassConfig(LLVMTargetMachine &TM, PassManagerBase &PM)
      : AIEBasePassConfig(TM, PM) {
    if (!EnableSubregRenaming)
      disablePass(&RenameIndependentSubregsID);
  }

  AIE2TargetMachine &getAIETargetMachine() const {
    return getTM<AIE2TargetMachine>();
  }

  bool addPreISel() override;
  void addPreEmitPass() override;
  bool addInstSelector() override;
  bool addGlobalInstructionSelect() override;
  void addPreRegAlloc() override;
  bool addRegAssignAndRewriteOptimized() override;
  void addPostRewrite() override;
  void addMachineLateOptimization() override;
  void addPreSched2() override;
  void addBlockPlacement() override;
  void addPreLegalizeMachineIR() override;
  void addPreRegBankSelect() override;
};

TargetPassConfig *AIE2TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new AIE2PassConfig(*this, PM);
}

bool AIE2PassConfig::addPreISel() {
  if (TM->getOptLevel() != CodeGenOptLevel::None)
    addPass(createHardwareLoopsLegacyPass());
  return false;
}

void AIE2PassConfig::addPreEmitPass() {}

void AIE2PassConfig::addPreLegalizeMachineIR() {
  addPass(createAIEAddressSpaceFlattening());
  if (getOptLevel() != CodeGenOptLevel::None)
    addPass(createAIE2PreLegalizerCombiner());
  addPass(createAIEEliminateDuplicatePHI());
}

void AIE2PassConfig::addPreRegBankSelect() {
  if (getOptLevel() != CodeGenOptLevel::None) {
    addPass(createAIE2PostLegalizerGenericCombiner());
    if (EnableAddressChaining)
      addPass(createAIEClusterBaseAddress());
    addPass(createAIE2PostLegalizerCustomCombiner());
  }
}

bool AIE2PassConfig::addGlobalInstructionSelect() {
  addPass(new InstructionSelect(getOptLevel()));
  if (getOptLevel() != CodeGenOptLevel::None) {
    addPass(
        createDeadMachineInstructionElim(/*KeepLifetimeInstructions=*/true));
    addPass(createAIEPostSelectOptimize());
    addPass(
        createDeadMachineInstructionElim(/*KeepLifetimeInstructions=*/true));
    if (EnableReservedRegsLICM) {
      /// Try and hoist assignments to reserved registers out of loops.
      insertPass(&EarlyMachineLICMID, &ReservedRegsLICMID);
    }
  }
  return false;
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

static bool onlyAllocate3DRegisters(const TargetRegisterInfo &TRI,
                                    const TargetRegisterClass &RC) {
  return AIE2::eDSRegClass.hasSubClassEq(&RC);
}
static bool onlyAllocate3D2DRegisters(const TargetRegisterInfo &TRI,
                                      const TargetRegisterClass &RC) {
  return AIE2::eDSRegClass.hasSubClassEq(&RC) ||
         AIE2::eDRegClass.hasSubClassEq(&RC);
}
static bool onlyAllocateMRegisters(const TargetRegisterInfo &TRI,
                                   const TargetRegisterClass &RC) {
  return AIE2::eMRegClass.hasSubClassEq(&RC);
}

bool AIE2PassConfig::addRegAssignAndRewriteOptimized() {

  // Pre-RA scheduling might have exposed simplifiable copies.
  if (EnablePreMISchedCoalescer)
    addPass(&RegisterCoalescerID);

  if (!EnableStagedRA && !EnableSuperRegSplitting)
    return TargetPassConfig::addRegAssignAndRewriteOptimized();

  // Rewrite instructions which use large tuple regs into _split variants
  // to better expose sub-registers and facilitate RA.
  if (EnableSuperRegSplitting)
    addPass(createAIESplitInstrBuilder());

  addPass(createAIERegClassConstrainer());

  if (AllocateMRegsFirst)
    addPass(createGreedyRegisterAllocator(onlyAllocateMRegisters));
  if (EnableStagedRA) {
    addPass(createGreedyRegisterAllocator(onlyAllocate3DRegisters));
    addPass(createAIESuperRegRewriter());
    addPass(createGreedyRegisterAllocator(onlyAllocate3D2DRegisters));
    addPass(createAIESuperRegRewriter());
  }
  addPass(createGreedyRegisterAllocator());
  if (EnableWAWRegRewrite)
    addPass(createAIEWawRegRewriter());
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
  addPass(createAIEMachineAlignment());
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

unsigned
AIE2TargetMachine::getAddressSpaceForPseudoSourceKind(unsigned Kind) const {
  switch (Kind) {
  case PseudoSourceValue::Stack:
  case PseudoSourceValue::FixedStack:
    return StackAddrSpace;
  case AIETargetPSV::AIETileMem:
    return static_cast<unsigned>(AIE2::AddressSpaces::TM);
  default:
    return static_cast<unsigned>(AIE2::AddressSpaces::none);
  }
}
