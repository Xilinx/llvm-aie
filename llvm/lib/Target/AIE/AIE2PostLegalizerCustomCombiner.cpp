//=== lib/CodeGen/GlobalISel/AIE2PostLegalizerCustomCombiner.cpp ----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass does combining of machine instructions at the generic MI level,
// after the legalizer, postlegalizer generic combiner and address clustering.
//
//===----------------------------------------------------------------------===//

#include "AIE2TargetMachine.h"
#include "AIECombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/CSEInfo.h"
#include "llvm/CodeGen/GlobalISel/Combiner.h"
#include "llvm/CodeGen/GlobalISel/CombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/CombinerInfo.h"
#include "llvm/CodeGen/GlobalISel/GIMatchTableExecutorImpl.h"
#include "llvm/CodeGen/GlobalISel/GISelKnownBits.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/InitializePasses.h"

#define DEBUG_TYPE "aie2-postlegalizer-custom-combiner"

#define GET_GICOMBINER_DEPS
#include "AIE2GenPostLegalizerGICustomCombiner.inc"
#undef GET_GICOMBINER_DEPS

using namespace llvm;

static const char AIE2_POSTLEGALIZER_CUSTOM_COMBINER[] =
    "AIE2 Post Legalizer Custom Combiner";

namespace {

#define GET_GICOMBINER_TYPES
#include "AIE2GenPostLegalizerGICustomCombiner.inc"
#undef GET_GICOMBINER_TYPES

class AIE2PostLegalizerCustomCombinerImpl : public Combiner {
protected:
  // TODO: Make CombinerHelper methods const.
  mutable CombinerHelper Helper;
  const AIE2PostLegalizerCustomCombinerImplRuleConfig &RuleConfig;
  const AIE2Subtarget &STI;

public:
  AIE2PostLegalizerCustomCombinerImpl(
      MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
      GISelKnownBits &KB, GISelCSEInfo *CSEInfo,
      const AIE2PostLegalizerCustomCombinerImplRuleConfig &RuleConfig,
      const AIE2Subtarget &STI, MachineDominatorTree *MDT,
      const LegalizerInfo *LI);

  static const char *getName() { return "AIE2PostLegalizerCustomCombiner"; }

  bool tryCombineAll(MachineInstr &I) const override;

private:
#define GET_GICOMBINER_CLASS_MEMBERS
#include "AIE2GenPostLegalizerGICustomCombiner.inc"
#undef GET_GICOMBINER_CLASS_MEMBERS
};

#define GET_GICOMBINER_IMPL
#include "AIE2GenPostLegalizerGICustomCombiner.inc"
#undef GET_GICOMBINER_IMPL

AIE2PostLegalizerCustomCombinerImpl::AIE2PostLegalizerCustomCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelKnownBits &KB, GISelCSEInfo *CSEInfo,
    const AIE2PostLegalizerCustomCombinerImplRuleConfig &RuleConfig,
    const AIE2Subtarget &STI,
    MachineDominatorTree *MDT,
    const LegalizerInfo *LI)
    : Combiner(MF, CInfo, TPC, &KB, CSEInfo),
      Helper(Observer, B, /*IsPostLegalize*/ false, &KB, MDT, LI),
      RuleConfig(RuleConfig), STI(STI),
#define GET_GICOMBINER_CONSTRUCTOR_INITS
#include "AIE2GenPostLegalizerGICustomCombiner.inc"
#undef GET_GICOMBINER_CONSTRUCTOR_INITS
{
}

class AIE2PostLegalizerCustomCombiner : public MachineFunctionPass {
public:
  static char ID;
  AIE2PostLegalizerCustomCombiner();

  StringRef getPassName() const override {
    return AIE2_POSTLEGALIZER_CUSTOM_COMBINER;
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<TargetPassConfig>();
    AU.setPreservesCFG();
    getSelectionDAGFallbackAnalysisUsage(AU);
    AU.addRequired<GISelKnownBitsAnalysis>();
    AU.addPreserved<GISelKnownBitsAnalysis>();
    AU.addRequired<MachineDominatorTree>();
    AU.addPreserved<MachineDominatorTree>();
    AU.addRequired<GISelCSEAnalysisWrapperPass>();
    AU.addPreserved<GISelCSEAnalysisWrapperPass>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

private:
  AIE2PostLegalizerCustomCombinerImplRuleConfig RuleConfig;
};
} // end anonymous namespace

AIE2PostLegalizerCustomCombiner::AIE2PostLegalizerCustomCombiner()
    : MachineFunctionPass(ID) {
  initializeAIE2PostLegalizerCustomCombinerPass(
      *PassRegistry::getPassRegistry());
}

bool AIE2PostLegalizerCustomCombiner::runOnMachineFunction(
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

  const AIE2Subtarget &ST = MF.getSubtarget<AIE2Subtarget>();
  const auto *LI = ST.getLegalizerInfo();

  GISelKnownBits *KB = &getAnalysis<GISelKnownBitsAnalysis>().get(MF);
  MachineDominatorTree *MDT = &getAnalysis<MachineDominatorTree>();

  CombinerInfo CInfo(/*AllowIllegalOps*/ true, /*ShouldLegalizeIllegal*/ false,
                     /*LegalizerInfo*/ nullptr, EnableOpt, F.hasOptSize(),
                     F.hasMinSize());
  AIE2PostLegalizerCustomCombinerImpl Impl(MF, CInfo, TPC, *KB, CSEInfo,
                                        RuleConfig, ST, MDT, LI);
  return Impl.combineMachineInstrs();
}

char AIE2PostLegalizerCustomCombiner::ID = 0;
INITIALIZE_PASS_BEGIN(AIE2PostLegalizerCustomCombiner, DEBUG_TYPE,
                      AIE2_POSTLEGALIZER_CUSTOM_COMBINER, false, false)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_DEPENDENCY(GISelKnownBitsAnalysis)
INITIALIZE_PASS_DEPENDENCY(GISelCSEAnalysisWrapperPass)
INITIALIZE_PASS_END(AIE2PostLegalizerCustomCombiner, DEBUG_TYPE,
                    AIE2_POSTLEGALIZER_CUSTOM_COMBINER, false, false)

namespace llvm {
FunctionPass *createAIE2PostLegalizerCustomCombiner() {
  return new AIE2PostLegalizerCustomCombiner();
}
} // end namespace llvm
