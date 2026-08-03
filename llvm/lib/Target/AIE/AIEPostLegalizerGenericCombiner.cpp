//===-- AIEPostLegalizerGenericCombiner.cpp ====================//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Unified aie post legalizer generic combiner pass for all AIE architectures.
// Dispatches to the architecture-specific CombinerImpl at runtime.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseSubtarget.h"
#include "AIECombiners.h"
#include "llvm/CodeGen/GlobalISel/CSEInfo.h"
#include "llvm/CodeGen/GlobalISel/Combiner.h"
#include "llvm/CodeGen/GlobalISel/CombinerInfo.h"
#include "llvm/CodeGen/GlobalISel/GISelValueTracking.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/InitializePasses.h"

#define DEBUG_TYPE "aie-postlegalizer-generic-combiner"

using namespace llvm;

// Factory functions defined in arch-specific files.
std::unique_ptr<Combiner> createAIE2PostLegalizerGenericCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelValueTracking &VT, GISelCSEInfo *CSEInfo, const AIEBaseSubtarget &STI,
    MachineDominatorTree *MDT, const LegalizerInfo *LI);
std::unique_ptr<Combiner> createAIE2PPostLegalizerGenericCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelValueTracking &VT, GISelCSEInfo *CSEInfo, const AIEBaseSubtarget &STI,
    MachineDominatorTree *MDT, const LegalizerInfo *LI);
std::unique_ptr<Combiner> createAIE2PSPostLegalizerGenericCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelValueTracking &VT, GISelCSEInfo *CSEInfo, const AIEBaseSubtarget &STI,
    MachineDominatorTree *MDT, const LegalizerInfo *LI);

namespace {
class AIEPostLegalizerGenericCombiner : public MachineFunctionPass {
public:
  static char ID;

  AIEPostLegalizerGenericCombiner() : MachineFunctionPass(ID) {
    initializeAIEPostLegalizerGenericCombinerPass(
        *PassRegistry::getPassRegistry());
  }

  StringRef getPassName() const override {
    return "AIE Post Legalizer Generic Combiner";
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<TargetPassConfig>();
    AU.setPreservesCFG();
    getSelectionDAGFallbackAnalysisUsage(AU);
    AU.addRequired<GISelValueTrackingAnalysisLegacy>();
    AU.addPreserved<GISelValueTrackingAnalysisLegacy>();
    AU.addRequired<MachineDominatorTreeWrapperPass>();
    AU.addPreserved<MachineDominatorTreeWrapperPass>();
    AU.addRequired<GISelCSEAnalysisWrapperPass>();
    AU.addPreserved<GISelCSEAnalysisWrapperPass>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }
};
} // end anonymous namespace

bool AIEPostLegalizerGenericCombiner::runOnMachineFunction(
    MachineFunction &MF) {
  if (MF.getProperties().hasProperty(
          MachineFunctionProperties::Property::FailedISel))
    return false;

  auto *TPC = &getAnalysis<TargetPassConfig>();
  auto &Wrapper = getAnalysis<GISelCSEAnalysisWrapperPass>().getCSEWrapper();
  auto *CSEInfo = &Wrapper.get(TPC->getCSEConfig());
  const Function &F = MF.getFunction();
  const bool EnableOpt =
      MF.getTarget().getOptLevel() != CodeGenOptLevel::None && !skipFunction(F);
  const auto &ST = MF.getSubtarget<AIEBaseSubtarget>();
  const auto *LI = ST.getLegalizerInfo();
  auto *VT = &getAnalysis<GISelValueTrackingAnalysisLegacy>().get(MF);
  auto *MDT = &getAnalysis<MachineDominatorTreeWrapperPass>().getDomTree();

  CombinerInfo CInfo(/*AllowIllegalOps*/ true, /*ShouldLegalizeIllegal*/ false,
                     /*LegalizerInfo*/ nullptr, EnableOpt, F.hasOptSize(),
                     F.hasMinSize());

  const Triple &TT = ST.getTargetTriple();
  std::unique_ptr<Combiner> Impl;
  if (TT.isAIE2P())
    Impl = createAIE2PPostLegalizerGenericCombinerImpl(MF, CInfo, TPC, *VT,
                                                       CSEInfo, ST, MDT, LI);
  else if (TT.isAIE2PS())
    Impl = createAIE2PSPostLegalizerGenericCombinerImpl(MF, CInfo, TPC, *VT,
                                                        CSEInfo, ST, MDT, LI);
  else
    Impl = createAIE2PostLegalizerGenericCombinerImpl(MF, CInfo, TPC, *VT,
                                                      CSEInfo, ST, MDT, LI);

  return Impl->combineMachineInstrs();
}

char AIEPostLegalizerGenericCombiner::ID = 0;
INITIALIZE_PASS_BEGIN(AIEPostLegalizerGenericCombiner, DEBUG_TYPE,
                      "AIE Post Legalizer Generic Combiner", false, false)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_DEPENDENCY(GISelValueTrackingAnalysisLegacy)
INITIALIZE_PASS_DEPENDENCY(GISelCSEAnalysisWrapperPass)
INITIALIZE_PASS_END(AIEPostLegalizerGenericCombiner, DEBUG_TYPE,
                    "AIE Post Legalizer Generic Combiner", false, false)

namespace llvm {
FunctionPass *createAIEPostLegalizerGenericCombiner() {
  return new AIEPostLegalizerGenericCombiner();
}
} // end namespace llvm
