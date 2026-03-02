//=== lib/CodeGen/GlobalISel/AIE2PSPostLegalizerCustomCombiner.cpp------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===--------------------------------------------------------------------===//
//
// This pass does combining of machine instructions at the generic MI level,
// after the legalizer, postlegalizer generic combiner and address clustering.
//
//===--------------------------------------------------------------------===//

#include "AIE2PSTargetMachine.h"
#include "AIECombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/CSEInfo.h"
#include "llvm/CodeGen/GlobalISel/Combiner.h"
#include "llvm/CodeGen/GlobalISel/CombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/CombinerInfo.h"
#include "llvm/CodeGen/GlobalISel/GIMatchTableExecutorImpl.h"
#include "llvm/CodeGen/GlobalISel/GISelKnownBits.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/InitializePasses.h"

#define DEBUG_TYPE "aie2ps-postlegalizer-custom-combiner"

#define GET_GICOMBINER_DEPS
#include "AIE2PSGenPostLegalizerGICustomCombiner.inc"
#undef GET_GICOMBINER_DEPS

using namespace llvm;

extern cl::opt<bool> EnableGlobalPtrModOptimizer;

static const char AIE2PS_POSTLEGALIZER_CUSTOM_COMBINER[] =
    "AIE2PS Post Legalizer Custom Combiner";

namespace {

#define GET_GICOMBINER_TYPES
#include "AIE2PSGenPostLegalizerGICustomCombiner.inc"
#undef GET_GICOMBINER_TYPES

class AIE2PSPostLegalizerCustomCombinerImpl : public Combiner {
protected:
  // TODO: Make CombinerHelper methods const.
  mutable CombinerHelper Helper;
  AIE::FoundCombiners EmptyGlobalCombiner;
  AIE::FoundCombiners *GlobalCombiners = nullptr;
  const AIE2PSPostLegalizerCustomCombinerImplRuleConfig &RuleConfig;
  const AIE2PSSubtarget &STI;

public:
  AIE2PSPostLegalizerCustomCombinerImpl(
      MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
      GISelKnownBits &KB, GISelCSEInfo *CSEInfo,
      AIE::FoundCombiners *GlobalCombiner,
      const AIE2PSPostLegalizerCustomCombinerImplRuleConfig &RuleConfig,
      const AIE2PSSubtarget &STI, MachineDominatorTree *MDT,
      const LegalizerInfo *LI);

  static const char *getName() { return "AIE2PSPostLegalizerCustomCombiner"; }

  bool tryCombineAll(MachineInstr &I) const override;

private:
#define GET_GICOMBINER_CLASS_MEMBERS
#include "AIE2PSGenPostLegalizerGICustomCombiner.inc"
#undef GET_GICOMBINER_CLASS_MEMBERS
};

#define GET_GICOMBINER_IMPL
#include "AIE2PSGenPostLegalizerGICustomCombiner.inc"
#undef GET_GICOMBINER_IMPL

AIE2PSPostLegalizerCustomCombinerImpl::AIE2PSPostLegalizerCustomCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelKnownBits &KB, GISelCSEInfo *CSEInfo,
    AIE::FoundCombiners *GlobalCombiner,
    const AIE2PSPostLegalizerCustomCombinerImplRuleConfig &RuleConfig,
    const AIE2PSSubtarget &STI, MachineDominatorTree *MDT,
    const LegalizerInfo *LI)
    : Combiner(MF, CInfo, TPC, &KB, CSEInfo),
      Helper(Observer, B, /*IsPostLegalize*/ false, &KB, MDT, LI),
      GlobalCombiners(GlobalCombiner), RuleConfig(RuleConfig), STI(STI),
#define GET_GICOMBINER_CONSTRUCTOR_INITS
#include "AIE2PSGenPostLegalizerGICustomCombiner.inc"
#undef GET_GICOMBINER_CONSTRUCTOR_INITS
{
  if (!GlobalCombiner)
    GlobalCombiners = &EmptyGlobalCombiner;
}

class AIE2PSPostLegalizerCustomCombiner : public MachineFunctionPass {
public:
  static char ID;
  AIE2PSPostLegalizerCustomCombiner();

  StringRef getPassName() const override {
    return AIE2PS_POSTLEGALIZER_CUSTOM_COMBINER;
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<TargetPassConfig>();
    AU.setPreservesCFG();
    getSelectionDAGFallbackAnalysisUsage(AU);
    AU.addRequired<GISelKnownBitsAnalysis>();
    AU.addPreserved<GISelKnownBitsAnalysis>();
    AU.addRequired<MachineDominatorTreeWrapperPass>();
    AU.addPreserved<MachineDominatorTreeWrapperPass>();
    AU.addRequired<GISelCSEAnalysisWrapperPass>();
    AU.addPreserved<GISelCSEAnalysisWrapperPass>();
    if (EnableGlobalPtrModOptimizer) {
      AU.addRequired<AIEPtrModOptimizer>();
    }
    MachineFunctionPass::getAnalysisUsage(AU);
  }

private:
  AIE2PSPostLegalizerCustomCombinerImplRuleConfig RuleConfig;
};
} // end anonymous namespace

AIE2PSPostLegalizerCustomCombiner::AIE2PSPostLegalizerCustomCombiner()
    : MachineFunctionPass(ID) {
  initializeAIE2PSPostLegalizerCustomCombinerPass(
      *PassRegistry::getPassRegistry());
  if (!RuleConfig.parseCommandLineOption())
    report_fatal_error("Invalid rule identifier");
}

bool AIE2PSPostLegalizerCustomCombiner::runOnMachineFunction(
    MachineFunction &MF) {
  if (MF.getProperties().hasProperty(
          MachineFunctionProperties::Property::FailedISel))
    return false;
  auto *TPC = &getAnalysis<TargetPassConfig>();

  // Enable CSE.
  GISelCSEAnalysisWrapper &Wrapper =
      getAnalysis<GISelCSEAnalysisWrapperPass>().getCSEWrapper();
  auto *CSEInfo = &Wrapper.get(TPC->getCSEConfig());

  const Function &F = MF.getFunction();
  bool EnableOpt =
      MF.getTarget().getOptLevel() != CodeGenOptLevel::None && !skipFunction(F);

  const AIE2PSSubtarget &ST = MF.getSubtarget<AIE2PSSubtarget>();
  const auto *LI = ST.getLegalizerInfo();

  GISelKnownBits *KB = &getAnalysis<GISelKnownBitsAnalysis>().get(MF);
  MachineDominatorTree *MDT =
      &getAnalysis<MachineDominatorTreeWrapperPass>().getDomTree();

  AIE::FoundCombiners *AIEGlobalPtrIncResults = nullptr;
  if (auto *PtrModOptPass = getAnalysisIfAvailable<AIEPtrModOptimizer>())
    AIEGlobalPtrIncResults = PtrModOptPass->getGlobalPtrCombiners();

  CombinerInfo CInfo(/*AllowIllegalOps*/ true, /*ShouldLegalizeIllegal*/ false,
                     /*LegalizerInfo*/ nullptr, EnableOpt, F.hasOptSize(),
                     F.hasMinSize());
  AIE2PSPostLegalizerCustomCombinerImpl Impl(MF, CInfo, TPC, *KB, CSEInfo,
                                             AIEGlobalPtrIncResults, RuleConfig,
                                             ST, MDT, LI);
  return Impl.combineMachineInstrs();
}

char AIE2PSPostLegalizerCustomCombiner::ID = 0;
INITIALIZE_PASS_BEGIN(AIE2PSPostLegalizerCustomCombiner, DEBUG_TYPE,
                      AIE2PS_POSTLEGALIZER_CUSTOM_COMBINER, false, false)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_DEPENDENCY(GISelKnownBitsAnalysis)
INITIALIZE_PASS_DEPENDENCY(GISelCSEAnalysisWrapperPass)
INITIALIZE_PASS_END(AIE2PSPostLegalizerCustomCombiner, DEBUG_TYPE,
                    AIE2PS_POSTLEGALIZER_CUSTOM_COMBINER, false, false)

namespace llvm {
FunctionPass *createAIE2PSPostLegalizerCustomCombiner() {
  return new AIE2PSPostLegalizerCustomCombiner();
}
} // end namespace llvm
