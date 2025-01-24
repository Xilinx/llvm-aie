//===------ AIE2PTargetMachine.cpp - Define TargetMachine for AIE2p ------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Implements the info about AIE2p target spec.
//
//===----------------------------------------------------------------------===//

#include "AIE2PTargetMachine.h"
#include "AIE2PTargetTransformInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

using namespace llvm;
extern cl::opt<bool> EnableStagedRA;
extern cl::opt<bool> EnableSuperRegSplitting;
extern cl::opt<bool> AllocateMRegsFirst;
extern cl::opt<bool> EnablePreMISchedCoalescer;
extern cl::opt<bool> EnableAddressChaining;

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

// AIE2P Pass Setup
class AIE2PPassConfig final : public AIE2PassConfig {
public:
  AIE2PPassConfig(LLVMTargetMachine &TM, PassManagerBase &PM)
      : AIE2PassConfig(TM, PM) {}
  void addPreRegBankSelect() override;
  void addPreLegalizeMachineIR() override;
  bool addRegAssignAndRewriteOptimized() override;
};

void AIE2PPassConfig::addPreLegalizeMachineIR() {
  addPass(createAIEAddressSpaceFlattening());
  if (getOptLevel() != CodeGenOptLevel::None)
    addPass(createAIE2PPreLegalizerCombiner());
  addPass(createAIEEliminateDuplicatePHI());
}

void AIE2PPassConfig::addPreRegBankSelect() {
  if (getOptLevel() != CodeGenOptLevel::None) {
    addPass(createAIE2PPostLegalizerGenericCombiner());
    if (EnableAddressChaining)
      addPass(createAIEClusterBaseAddress());
    addPass(createAIE2PPostLegalizerCustomCombiner());
  }
}

static bool onlyAllocate3DRegisters(const TargetRegisterInfo &TRI,
                                    const TargetRegisterClass &RC) {
  return AIE2P::eDSRegClass.hasSubClassEq(&RC);
}
static bool onlyAllocate3D2DRegisters(const TargetRegisterInfo &TRI,
                                      const TargetRegisterClass &RC) {
  return AIE2P::eDSRegClass.hasSubClassEq(&RC) ||
         AIE2P::eDRegClass.hasSubClassEq(&RC);
}
static bool onlyAllocateMRegisters(const TargetRegisterInfo &TRI,
                                   const TargetRegisterClass &RC) {
  return AIE2P::eMRegClass.hasSubClassEq(&RC);
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
  }
  addPass(createGreedyRegisterAllocator());
  addPass(createVirtRegRewriter());

  return true;
}

TargetPassConfig *AIE2PTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new AIE2PPassConfig(*this, PM);
}

TargetTransformInfo
AIE2PTargetMachine::getTargetTransformInfo(const Function &F) const {
  return TargetTransformInfo(AIE2PTTIImpl(this, F));
}
