//===-- AIEPostLegalizerCustomCombiner.cpp =====================//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Unified aie post legalizer custom combiner pass for all AIE architectures.
// Dispatches to the architecture-specific CombinerImpl at runtime.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseSubtarget.h"
#include "AIECombiners.h"
#include "AIEPtrModOptimizer.h"
#include "llvm/CodeGen/GlobalISel/CSEInfo.h"
#include "llvm/CodeGen/GlobalISel/Combiner.h"
#include "llvm/CodeGen/GlobalISel/CombinerInfo.h"
#include "llvm/CodeGen/GlobalISel/GISelValueTracking.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/InitializePasses.h"

#define DEBUG_TYPE "aie-postlegalizer-custom-combiner"

using namespace llvm;

extern cl::opt<bool> EnableGlobalPtrModOptimizer;

// Factory functions defined in arch-specific files.
std::unique_ptr<Combiner> createAIE2PostLegalizerCustomCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelValueTracking &VT, GISelCSEInfo *CSEInfo,
    AIE::FoundCombiners *GlobalCombiners, const AIEBaseSubtarget &STI,
    MachineDominatorTree *MDT, const LegalizerInfo *LI);
std::unique_ptr<Combiner> createAIE2PPostLegalizerCustomCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelValueTracking &VT, GISelCSEInfo *CSEInfo,
    AIE::FoundCombiners *GlobalCombiners, const AIEBaseSubtarget &STI,
    MachineDominatorTree *MDT, const LegalizerInfo *LI);
std::unique_ptr<Combiner> createAIE2PSPostLegalizerCustomCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelValueTracking &VT, GISelCSEInfo *CSEInfo,
    AIE::FoundCombiners *GlobalCombiners, const AIEBaseSubtarget &STI,
    MachineDominatorTree *MDT, const LegalizerInfo *LI);

namespace {
class AIEPostLegalizerCustomCombiner : public MachineFunctionPass {
public:
  static char ID;

  AIEPostLegalizerCustomCombiner() : MachineFunctionPass(ID) {
    initializeAIEPostLegalizerCustomCombinerPass(
        *PassRegistry::getPassRegistry());
  }

  StringRef getPassName() const override {
    return "AIE Post Legalizer Custom Combiner";
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<TargetPassConfig>();
    AU.setPreservesCFG();
    getSelectionDAGFallbackAnalysisUsage(AU);
    AU.addRequired<GISelValueTrackingAnalysis>();
    AU.addPreserved<GISelValueTrackingAnalysis>();
    AU.addRequired<MachineDominatorTreeWrapperPass>();
    AU.addPreserved<MachineDominatorTreeWrapperPass>();
    AU.addRequired<GISelCSEAnalysisWrapperPass>();
    AU.addPreserved<GISelCSEAnalysisWrapperPass>();
    if (EnableGlobalPtrModOptimizer)
      AU.addRequired<AIEPtrModOptimizer>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }
};
} // end anonymous namespace

bool AIEPostLegalizerCustomCombiner::runOnMachineFunction(MachineFunction &MF) {
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
  auto *VT = &getAnalysis<GISelValueTrackingAnalysis>().get(MF);
  auto *MDT = &getAnalysis<MachineDominatorTreeWrapperPass>().getDomTree();

  AIE::FoundCombiners *AIEGlobalPtrIncResults = nullptr;
  if (auto *PtrModOptPass = getAnalysisIfAvailable<AIEPtrModOptimizer>())
    AIEGlobalPtrIncResults = PtrModOptPass->getGlobalPtrCombiners();

  CombinerInfo CInfo(/*AllowIllegalOps*/ true, /*ShouldLegalizeIllegal*/ false,
                     /*LegalizerInfo*/ nullptr, EnableOpt, F.hasOptSize(),
                     F.hasMinSize());

  const Triple &TT = ST.getTargetTriple();
  std::unique_ptr<Combiner> Impl;
  if (TT.isAIE2P())
    Impl = createAIE2PPostLegalizerCustomCombinerImpl(
        MF, CInfo, TPC, *VT, CSEInfo, AIEGlobalPtrIncResults, ST, MDT, LI);
  else if (TT.isAIE2PS())
    Impl = createAIE2PSPostLegalizerCustomCombinerImpl(
        MF, CInfo, TPC, *VT, CSEInfo, AIEGlobalPtrIncResults, ST, MDT, LI);
  else
    Impl = createAIE2PostLegalizerCustomCombinerImpl(
        MF, CInfo, TPC, *VT, CSEInfo, AIEGlobalPtrIncResults, ST, MDT, LI);

  const bool Changed = Impl->combineMachineInstrs();
  if (AIEGlobalPtrIncResults)
    AIEGlobalPtrIncResults->finalizeDeferredDeletes(MF);
  return Changed;
}

char AIEPostLegalizerCustomCombiner::ID = 0;
INITIALIZE_PASS_BEGIN(AIEPostLegalizerCustomCombiner, DEBUG_TYPE,
                      "AIE Post Legalizer Custom Combiner", false, false)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_DEPENDENCY(GISelValueTrackingAnalysis)
INITIALIZE_PASS_DEPENDENCY(GISelCSEAnalysisWrapperPass)
INITIALIZE_PASS_END(AIEPostLegalizerCustomCombiner, DEBUG_TYPE,
                    "AIE Post Legalizer Custom Combiner", false, false)

namespace llvm {
FunctionPass *createAIEPostLegalizerCustomCombiner() {
  return new AIEPostLegalizerCustomCombiner();
}
} // end namespace llvm
