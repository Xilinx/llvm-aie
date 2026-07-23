//=== lib/CodeGen/GlobalISel/AIE2PPostLegalizerCustomCombiner.cpp--------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===--------------------------------------------------------------------===//
//
// This pass does combining of machine instructions at the generic MI level,
// after the legalizer, postlegalizer generic combiner and address clustering.
//
//===--------------------------------------------------------------------===//

#include "AIECombinerBase.h"
#include "AIECombinerHelper.h"
#include "AIECombiners.h"
#include "llvm/CodeGen/GlobalISel/CSEInfo.h"
#include "llvm/CodeGen/GlobalISel/Combiner.h"
#include "llvm/CodeGen/GlobalISel/CombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/CombinerInfo.h"
#include "llvm/CodeGen/GlobalISel/GIMatchTableExecutorImpl.h"
#include "llvm/CodeGen/GlobalISel/GISelValueTracking.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/InitializePasses.h"

#define DEBUG_TYPE "aie2p-postlegalizer-custom-combiner"

#define GET_GICOMBINER_DEPS
#include "AIE2PGenPostLegalizerGICustomCombiner.inc"
#undef GET_GICOMBINER_DEPS

using namespace llvm;

extern cl::opt<bool> EnableGlobalPtrModOptimizer;

namespace {

#define GET_GICOMBINER_TYPES
#include "AIE2PGenPostLegalizerGICustomCombiner.inc"
#undef GET_GICOMBINER_TYPES

class AIE2PPostLegalizerCustomCombinerImpl
    : public AIECombinerBase<AIE2PPostLegalizerCustomCombinerImplRuleConfig> {
protected:
  AIE::FoundCombiners EmptyGlobalCombiner;
  AIE::FoundCombiners *GlobalCombiners = nullptr;

public:
  AIE2PPostLegalizerCustomCombinerImpl(
      MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
      GISelValueTracking &VT, GISelCSEInfo *CSEInfo,
      AIE::FoundCombiners *GlobalCombiner,
      const AIE2PPostLegalizerCustomCombinerImplRuleConfig &RuleConfig,
      const AIEBaseSubtarget &STI, MachineDominatorTree *MDT,
      const LegalizerInfo *LI);

  static const char *getName() { return "AIE2PPostLegalizerCustomCombiner"; }

  bool tryCombineAll(MachineInstr &I) const override;

private:
#define GET_GICOMBINER_CLASS_MEMBERS
#include "AIE2PGenPostLegalizerGICustomCombiner.inc"
#undef GET_GICOMBINER_CLASS_MEMBERS
};

#define GET_GICOMBINER_IMPL
#include "AIE2PGenPostLegalizerGICustomCombiner.inc"
#undef GET_GICOMBINER_IMPL

AIE2PPostLegalizerCustomCombinerImpl::AIE2PPostLegalizerCustomCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelValueTracking &VT, GISelCSEInfo *CSEInfo,
    AIE::FoundCombiners *GlobalCombiner,
    const AIE2PPostLegalizerCustomCombinerImplRuleConfig &RuleConfig,
    const AIEBaseSubtarget &STI, MachineDominatorTree *MDT,
    const LegalizerInfo *LI)
    : AIECombinerBase(MF, CInfo, TPC, VT, CSEInfo, RuleConfig, STI, MDT, LI,
                      /*IsPreLegalize=*/false),
      GlobalCombiners(GlobalCombiner),
#define GET_GICOMBINER_CONSTRUCTOR_INITS
#include "AIE2PGenPostLegalizerGICustomCombiner.inc"
#undef GET_GICOMBINER_CONSTRUCTOR_INITS
{
  if (!GlobalCombiner)
    GlobalCombiners = &EmptyGlobalCombiner;
}
} // end anonymous namespace

std::unique_ptr<Combiner> createAIE2PPostLegalizerCustomCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelValueTracking &VT, GISelCSEInfo *CSEInfo,
    AIE::FoundCombiners *GlobalCombiners, const AIEBaseSubtarget &STI,
    MachineDominatorTree *MDT, const LegalizerInfo *LI) {
  static AIE2PPostLegalizerCustomCombinerImplRuleConfig RuleConfig;
  static bool Parsed = [] {
    if (!RuleConfig.parseCommandLineOption())
      report_fatal_error("Invalid rule identifier");
    return true;
  }();
  (void)Parsed;
  return std::make_unique<AIE2PPostLegalizerCustomCombinerImpl>(
      MF, CInfo, TPC, VT, CSEInfo, GlobalCombiners, RuleConfig, STI, MDT, LI);
}
