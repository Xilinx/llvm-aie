//===-- AIEPreISelCombiner.cpp -------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass runs combining after register bank selection and before
// instruction selection. It folds patterns introduced by register bank
// selection that would otherwise prevent ISel pattern matching.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseSubtarget.h"
#include "AIEBaseTargetMachine.h"
#include "AIECombinerBase.h"
#include "AIECombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/CSEInfo.h"
#include "llvm/CodeGen/GlobalISel/Combiner.h"
#include "llvm/CodeGen/GlobalISel/CombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/CombinerInfo.h"
#include "llvm/CodeGen/GlobalISel/GIMatchTableExecutorImpl.h"
#include "llvm/CodeGen/GlobalISel/GISelValueTracking.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/InitializePasses.h"

#define DEBUG_TYPE "aie-preisel-combiner"

#define GET_GICOMBINER_DEPS
#include "AIEGenPreISelGICombiner.inc"
#undef GET_GICOMBINER_DEPS

using namespace llvm;

namespace {

#define GET_GICOMBINER_TYPES
#include "AIEGenPreISelGICombiner.inc"
#undef GET_GICOMBINER_TYPES

class AIEPreISelCombinerImpl
    : public AIECombinerBase<AIEPreISelCombinerImplRuleConfig> {
public:
  AIEPreISelCombinerImpl(MachineFunction &MF, CombinerInfo &CInfo,
                         const TargetPassConfig *TPC, GISelValueTracking &VT,
                         GISelCSEInfo *CSEInfo,
                         const AIEPreISelCombinerImplRuleConfig &RuleConfig,
                         const AIEBaseSubtarget &STI, MachineDominatorTree *MDT,
                         const LegalizerInfo *LI);

  static const char *getName() { return "AIEPreISelCombiner"; }

  bool tryCombineAll(MachineInstr &I) const override;

private:
#define GET_GICOMBINER_CLASS_MEMBERS
#include "AIEGenPreISelGICombiner.inc"
#undef GET_GICOMBINER_CLASS_MEMBERS
};

#define GET_GICOMBINER_IMPL
#include "AIEGenPreISelGICombiner.inc"
#undef GET_GICOMBINER_IMPL

AIEPreISelCombinerImpl::AIEPreISelCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelValueTracking &VT, GISelCSEInfo *CSEInfo,
    const AIEPreISelCombinerImplRuleConfig &RuleConfig,
    const AIEBaseSubtarget &STI, MachineDominatorTree *MDT,
    const LegalizerInfo *LI)
    : AIECombinerBase(MF, CInfo, TPC, VT, CSEInfo, RuleConfig, STI, MDT, LI,
                      /*IsPreLegalize*/ false),
#define GET_GICOMBINER_CONSTRUCTOR_INITS
#include "AIEGenPreISelGICombiner.inc"
#undef GET_GICOMBINER_CONSTRUCTOR_INITS
{
}

class AIEPreISelCombiner : public MachineFunctionPass {
public:
  static char ID;
  AIEPreISelCombiner();

  StringRef getPassName() const override { return "AIE Pre-ISel Combiner"; }

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

private:
  AIEPreISelCombinerImplRuleConfig RuleConfig;
};

} // end anonymous namespace

AIEPreISelCombiner::AIEPreISelCombiner() : MachineFunctionPass(ID) {
  initializeAIEPreISelCombinerPass(*PassRegistry::getPassRegistry());
  if (!RuleConfig.parseCommandLineOption())
    report_fatal_error("Invalid rule identifier");
}

bool AIEPreISelCombiner::runOnMachineFunction(MachineFunction &MF) {
  if (MF.getProperties().hasProperty(
          MachineFunctionProperties::Property::FailedISel))
    return false;
  auto *TPC = &getAnalysis<TargetPassConfig>();

  GISelCSEAnalysisWrapper &Wrapper =
      getAnalysis<GISelCSEAnalysisWrapperPass>().getCSEWrapper();
  auto *CSEInfo = &Wrapper.get(TPC->getCSEConfig());

  const Function &F = MF.getFunction();
  const bool EnableOpt =
      MF.getTarget().getOptLevel() != CodeGenOptLevel::None && !skipFunction(F);

  const auto &ST = MF.getSubtarget<AIEBaseSubtarget>();
  const auto *LI = ST.getLegalizerInfo();

  GISelValueTracking *VT =
      &getAnalysis<GISelValueTrackingAnalysisLegacy>().get(MF);
  MachineDominatorTree *MDT =
      &getAnalysis<MachineDominatorTreeWrapperPass>().getDomTree();

  CombinerInfo CInfo(/*AllowIllegalOps*/ true, /*ShouldLegalizeIllegal*/ false,
                     /*LegalizerInfo*/ nullptr, EnableOpt, F.hasOptSize(),
                     F.hasMinSize());
  AIEPreISelCombinerImpl Impl(MF, CInfo, TPC, *VT, CSEInfo, RuleConfig, ST, MDT,
                              LI);
  return Impl.combineMachineInstrs();
}

char AIEPreISelCombiner::ID = 0;
INITIALIZE_PASS_BEGIN(AIEPreISelCombiner, DEBUG_TYPE, "AIE Pre-ISel Combiner",
                      false, false)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_DEPENDENCY(GISelValueTrackingAnalysisLegacy)
INITIALIZE_PASS_DEPENDENCY(GISelCSEAnalysisWrapperPass)
INITIALIZE_PASS_END(AIEPreISelCombiner, DEBUG_TYPE, "AIE Pre-ISel Combiner",
                    false, false)

namespace llvm {
FunctionPass *createAIEPreISelCombiner() { return new AIEPreISelCombiner(); }
} // end namespace llvm
