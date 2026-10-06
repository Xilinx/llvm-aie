//===------ AIE2PTargetMachine.cpp - Define TargetMachine for AIE2p ------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Implements the info about AIE2p target spec.
//
//===----------------------------------------------------------------------===//

#include "AIE2PTargetMachine.h"
#include "AIE2PTargetTransformInfo.h"
#include "AIECombiners.h"
#include "AIESuperRegUtils.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

using namespace llvm;
extern cl::opt<bool> EnableStagedRA;
extern cl::opt<bool> EnableSuperRegSplitting;
extern cl::opt<bool> AllocateMRegsFirst;
extern cl::opt<bool> EnablePreMISchedCoalescer;
extern cl::opt<bool> EnableAddressChaining;
extern cl::opt<bool> EnableGlobalPtrModOptimizer;
extern cl::opt<bool> EnableWAWRegRewrite;
extern cl::opt<bool> EnableAIEIfConversion;
extern cl::opt<bool> EnableFineGrainedStagedRA;

void AIE2PTargetMachine::anchor() {}

AIE2PTargetMachine::AIE2PTargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       std::optional<Reloc::Model> RM,
                                       std::optional<CodeModel::Model> CM,
                                       CodeGenOptLevel OL, bool JIT)
    : AIEBaseTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, false),
      Subtarget(TT, StringRef("aie2p"), StringRef("aie2p"), FS,
                Options.MCOptions.getABIName(), *this) {
  setGlobalISel(true);
  setFastISel(false);
  setGlobalISelAbort(GlobalISelAbortMode::Enable);
}

void AIE2PPassConfig::addPreLegalizeMachineIR() {
  addPass(createAIEAddressSpaceFlattening());
  if (getOptLevel() != CodeGenOptLevel::None)
    addPass(createAIEPreLegalizerCombiner());
  addPass(createAIEEliminateDuplicatePHI());
}

void AIE2PPassConfig::addPreRegBankSelect() {
  if (getOptLevel() != CodeGenOptLevel::None) {
    addPass(createAIEPostLegalizerGenericCombiner());
    if (EnableAddressChaining)
      addPass(createAIEClusterBaseAddress());
    if (EnableGlobalPtrModOptimizer)
      addPass(createAIEPtrModOptimizer());
    addPass(createAIEPostLegalizerCustomCombiner());
  }
}

static bool onlyAllocate3DRegisters(const TargetRegisterInfo &TRI,
                                    const MachineRegisterInfo &MRI,
                                    const Register &R) {

  const TargetRegisterClass *RegClass = MRI.getRegClass(R);
  if (!AIE2P::eDSRegClass.hasSubClassEq(RegClass))
    return false;
  return EnableFineGrainedStagedRA
             ? AIESuperRegUtils::isRegUsedBy2DOr3DInstruction(MRI, R)
             : true;
}

static bool onlyAllocate3D2DRegisters(const TargetRegisterInfo &TRI,
                                      const MachineRegisterInfo &MRI,
                                      const Register &R) {
  const TargetRegisterClass *RegClass = MRI.getRegClass(R);
  if (!AIE2P::eDSRegClass.hasSubClassEq(RegClass) &&
      !AIE2P::eDRegClass.hasSubClassEq(RegClass))
    return false;
  return EnableFineGrainedStagedRA
             ? AIESuperRegUtils::isRegUsedBy2DOr3DInstruction(MRI, R)
             : true;
}

static bool onlyAllocateMRegisters(const TargetRegisterInfo &TRI,
                                   const MachineRegisterInfo &MRI,
                                   const Register &R) {
  return AIE2P::eMRegClass.hasSubClassEq(MRI.getRegClass(R));
}

bool AIE2PPassConfig::addRegAssignAndRewriteOptimized() {

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

TargetPassConfig *AIE2PTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new AIE2PPassConfig(*this, PM);
}

TargetTransformInfo
AIE2PTargetMachine::getTargetTransformInfo(const Function &F) const {
  return TargetTransformInfo(std::make_unique<AIE2PTTIImpl>(this, F));
}
