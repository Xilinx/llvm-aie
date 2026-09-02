//===------ AIE2PSTargetMachine.cpp - Define TargetMachine for AIE2ps -----===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Implements the info about AIE2ps target spec.
//
//===----------------------------------------------------------------------===//

#include "AIE2PSTargetMachine.h"
#include "AIE2PSTargetTransformInfo.h"
#include "AIECombiners.h"
#include "AIESuperRegUtils.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

using namespace llvm;
extern cl::opt<bool> EnableStagedRA;
extern cl::opt<bool> EnableSuperRegSplitting;
extern cl::opt<bool> AllocateMRegsFirst;
extern cl::opt<bool> EnablePreMISchedCoalescer;
extern cl::opt<bool> EnableFineGrainedStagedRA;
extern cl::opt<bool> EnableWAWRegRewrite;
extern cl::opt<bool> EnableAddressChaining;
extern cl::opt<bool> EnableGlobalPtrModOptimizer;

void AIE2PSTargetMachine::anchor() {}

AIE2PSTargetMachine::AIE2PSTargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         std::optional<Reloc::Model> RM,
                                         std::optional<CodeModel::Model> CM,
                                         CodeGenOptLevel OL, bool JIT)
    : AIEBaseTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, false),
      Subtarget(TT, StringRef("aie2ps"), StringRef("aie2ps"), FS,
                Options.MCOptions.getABIName(), *this) {
  setGlobalISel(true);
  setFastISel(false);
  setGlobalISelAbort(GlobalISelAbortMode::Enable);
}

// AIE2PS Pass Setup
class AIE2PSPassConfig final : public AIE2PPassConfig {
public:
  AIE2PSPassConfig(TargetMachine &TM, PassManagerBase &PM)
      : AIE2PPassConfig(TM, PM) {}
  void addPreRegBankSelect() override;
  void addPreLegalizeMachineIR() override;
  void addISelPrepare() override;
  bool addRegAssignAndRewriteOptimized() override;
};

void AIE2PSPassConfig::addPreRegBankSelect() {
  if (getOptLevel() != CodeGenOptLevel::None) {
    addPass(createAIEPostLegalizerGenericCombiner());
    if (EnableAddressChaining)
      addPass(createAIEClusterBaseAddress());
    if (EnableGlobalPtrModOptimizer)
      addPass(createAIEPtrModOptimizer());
    addPass(createAIEPostLegalizerCustomCombiner());
    addPass(createAIEPostLegalizerFinalCombiner());
  }
}

void AIE2PSPassConfig::addPreLegalizeMachineIR() {
  addPass(createAIEAddressSpaceFlattening());
  if (getOptLevel() != CodeGenOptLevel::None)
    addPass(createAIEPreLegalizerCombiner());
  addPass(createAIEEliminateDuplicatePHI());
}

void AIE2PSPassConfig::addISelPrepare() {
  AIE2PassConfig::addISelPrepare();
  addPass(createAIE2PSConvertFP16ScalarOperationPass());
}

static bool onlyAllocate3DRegisters(const TargetRegisterInfo &TRI,
                                    const MachineRegisterInfo &MRI,
                                    const Register &R) {

  const TargetRegisterClass *RegClass = MRI.getRegClass(R);
  if (!AIE2PS::eDSRegClass.hasSubClassEq(RegClass))
    return false;
  return EnableFineGrainedStagedRA
             ? AIESuperRegUtils::isRegUsedBy2DOr3DInstruction(MRI, R)
             : true;
}

static bool onlyAllocate3D2DRegisters(const TargetRegisterInfo &TRI,
                                      const MachineRegisterInfo &MRI,
                                      const Register &R) {
  const TargetRegisterClass *RegClass = MRI.getRegClass(R);
  if (!AIE2PS::eDSRegClass.hasSubClassEq(RegClass) &&
      !AIE2PS::eDRegClass.hasSubClassEq(RegClass))
    return false;
  return EnableFineGrainedStagedRA
             ? AIESuperRegUtils::isRegUsedBy2DOr3DInstruction(MRI, R)
             : true;
}

static bool onlyAllocateMRegisters(const TargetRegisterInfo &TRI,
                                   const MachineRegisterInfo &MRI,
                                   const Register &R) {
  return AIE2PS::eMRegClass.hasSubClassEq(MRI.getRegClass(R));
}

bool AIE2PSPassConfig::addRegAssignAndRewriteOptimized() {

  // Pre-RA scheduling might have exposed simplifiable copies.
  if (EnablePreMISchedCoalescer)
    addPass(&RegisterCoalescerID);

  if (!EnableStagedRA && !EnableSuperRegSplitting)
    return TargetPassConfig::addRegAssignAndRewriteOptimized();

  // Rewrite instructions which use large tuple regs into _split variants
  // to better expose sub-registers and facilitate RA.
  if (EnableSuperRegSplitting)
    addPass(createAIESplitInstrBuilder());

  if (AllocateMRegsFirst)
    addPass(createGreedyRegisterAllocator(onlyAllocateMRegisters));
  if (EnableStagedRA) {
    addPass(createGreedyRegisterAllocator(onlyAllocate3DRegisters));
    addPass(createAIESuperRegRewriter());
    addPass(createGreedyRegisterAllocator(onlyAllocate3D2DRegisters));
    addPass(createAIESuperRegRewriter());
    if (EnableFineGrainedStagedRA)
      addPass(createAIEUnallocatedSuperRegRewriter());
  }
  addPass(createGreedyRegisterAllocator());
  addRegRewritePasses();

  return true;
}

TargetPassConfig *AIE2PSTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new AIE2PSPassConfig(*this, PM);
}

TargetTransformInfo
AIE2PSTargetMachine::getTargetTransformInfo(const Function &F) const {
  return TargetTransformInfo(std::make_unique<AIE2PSTTIImpl>(this, F));
}
