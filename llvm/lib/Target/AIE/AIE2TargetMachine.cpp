//===-- AIE2TargetMachine.cpp - Define TargetMachine for AIEngine V2 ------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Implements the info about AIEngine V2 target spec.
//
//===----------------------------------------------------------------------===//

#include "AIE2TargetMachine.h"
#include "AIE2TargetTransformInfo.h"
#include "AIECombiners.h"
#include "AIEDumpArtifacts.h"
#include "AIEMachineFunctionInfo.h"
#include "llvm/CodeGen/GlobalISel/IRTranslator.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelect.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

cl::opt<bool>
    EnableSubregRenaming("aie-subreg-renaming", cl::Hidden, cl::init(false),
                         cl::desc("Enable RenameIndependentSubregs pass"));

static cl::opt<bool>
    EnableReservedRegsLICM("aie-reserved-regs-licm", cl::Hidden, cl::init(true),
                           cl::desc("Enable LICM for some reserved registers"));

static cl::opt<unsigned> StackAddrSpace(
    "aie-stack-addrspace", cl::init(0),
    cl::desc("Specify the addrspace where the stack is allocated "
             "(5: Bank A, 6: Bank B, 7: Bank C, 8: Bank D)"));

static cl::opt<bool> EnableOutlineMemoryGEP(
    "enable-outline-memory-gep", cl::Hidden, cl::init(true),
    cl::desc("Enable Outlining GEPs in Memory Instructions."));

static cl::opt<bool> EnableStackMinimization(
    "aie-stack-minimize", cl::Hidden, cl::init(true),
    cl::desc("Enable spill decomposition and stack slot minimization"));

extern cl::opt<bool> EnableAddressChaining;
extern cl::opt<bool> EnableGlobalPtrModOptimizer;
extern cl::opt<bool> EnableStagedRA;
extern cl::opt<bool> EnableSuperRegSplitting;
extern cl::opt<bool> AllocateMRegsFirst;
extern cl::opt<bool> EnablePreMISchedCoalescer;
extern cl::opt<bool> EnableWAWRegRewrite;
extern cl::opt<bool> EnableAIEIfConversion;

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

TargetPassConfig *AIE2TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new AIE2PassConfig(*this, PM);
}

bool AIE2PassConfig::addPreISel() {
  if (TM->getOptLevel() != CodeGenOptLevel::None) {
    addPass(createHardwareLoopsLegacyPass());
    addPass(createAIEOuterLoopPipelinerPass());
  }
  return false;
}

void AIE2PassConfig::addPreEmitPass() {}

void AIE2PassConfig::addPreLegalizeMachineIR() {
  addPass(createAIEAddressSpaceFlattening());
  if (getOptLevel() != CodeGenOptLevel::None)
    addPass(createAIEPreLegalizerCombiner());
  addPass(createAIEEliminateDuplicatePHI());
}

void AIE2PassConfig::addPreRegBankSelect() {
  if (getOptLevel() != CodeGenOptLevel::None) {
    addPass(createAIEPostLegalizerGenericCombiner());
    if (EnableAddressChaining)
      addPass(createAIEClusterBaseAddress());
    if (EnableGlobalPtrModOptimizer)
      addPass(createAIEPtrModOptimizer());
    addPass(createAIEPostLegalizerCustomCombiner());
  }
}

void AIE2PassConfig::addPreGlobalInstructionSelect() {
  addPass(createAIEPreISelCombiner());
}

bool AIE2PassConfig::addGlobalInstructionSelect() {
  addPass(new InstructionSelect(getOptLevel()));
  if (getOptLevel() != CodeGenOptLevel::None) {
    addPass(
        createDeadMachineInstructionElim(/*KeepLifetimeInstructions=*/true));
    addPass(createAIEPostSelectOptimize());
    addPass(
        createDeadMachineInstructionElim(/*KeepLifetimeInstructions=*/true));
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

void AIE2PassConfig::addISelPrepare() {
  if (EnableOutlineMemoryGEP)
    addPass(createAIEOutlineMemoryGEP());
  TargetPassConfig::addISelPrepare();
}

bool AIE2PassConfig::addILPOpts() {
  if (EnableAIEIfConversion)
    addPass(&EarlyIfConverterLegacyID);
  return true;
}

static bool onlyAllocate3DRegisters(const TargetRegisterInfo &TRI,
                                    const MachineRegisterInfo &MRI,
                                    const Register &R) {
  return AIE2::eDSRegClass.hasSubClassEq(MRI.getRegClass(R));
}
static bool onlyAllocate3D2DRegisters(const TargetRegisterInfo &TRI,
                                      const MachineRegisterInfo &MRI,
                                      const Register &R) {
  return AIE2::eDSRegClass.hasSubClassEq(MRI.getRegClass(R)) ||
         AIE2::eDRegClass.hasSubClassEq(MRI.getRegClass(R));
}
static bool onlyAllocateMRegisters(const TargetRegisterInfo &TRI,
                                   const MachineRegisterInfo &MRI,
                                   const Register &R) {
  return AIE2::eMRegClass.hasSubClassEq(MRI.getRegClass(R));
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
  if (EnableWAWRegRewrite) {
    addPass(createAIEWawRegRewriter());
    addPass(createGreedyRegisterAllocator());
  }
  addPass(createVirtRegRewriter());

  return true;
}

void AIE2PassConfig::addPostRewrite() {
  if (EnableStackMinimization) {
    // Decompose composite spills into subreg spills when only some subregs
    // are live, and minimize stack slot sizes based on actual usage patterns.
    addPass(createAIESpillSlotOptimization());
  }

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

  if (TM->getOptLevel() != CodeGenOptLevel::None)
    addPass(createAIEBaseHardwareLoopsPass());

  if (EnableReservedRegsLICM)
    addPass(createReservedRegsLICMPass());

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
