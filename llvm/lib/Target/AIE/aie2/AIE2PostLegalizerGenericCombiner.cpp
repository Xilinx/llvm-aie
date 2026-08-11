//=== lib/CodeGen/GlobalISel/AIE2PostLegalizerGenericCombiner.cpp ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass does combining of machine instructions at the generic MI level,
// after the legalizer and before address clustering.
//
//===----------------------------------------------------------------------===//

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

#define DEBUG_TYPE "aie2-postlegalizer-generic-combiner"

#define GET_GICOMBINER_DEPS
#include "AIE2GenPostLegalizerGIGenericCombiner.inc"
#undef GET_GICOMBINER_DEPS

using namespace llvm;

namespace {

#define GET_GICOMBINER_TYPES
#include "AIE2GenPostLegalizerGIGenericCombiner.inc"
#undef GET_GICOMBINER_TYPES

class AIE2PostLegalizerGenericCombinerImpl
    : public AIECombinerBase<AIE2PostLegalizerGenericCombinerImplRuleConfig> {
protected:
public:
  AIE2PostLegalizerGenericCombinerImpl(
      MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
      GISelValueTracking &VT, GISelCSEInfo *CSEInfo,
      const AIE2PostLegalizerGenericCombinerImplRuleConfig &RuleConfig,
      const AIEBaseSubtarget &STI, MachineDominatorTree *MDT,
      const LegalizerInfo *LI);

  static const char *getName() { return "AIE2PostLegalizerGenericCombiner"; }

  bool tryCombineAll(MachineInstr &I) const override;

private:
#define GET_GICOMBINER_CLASS_MEMBERS
#include "AIE2GenPostLegalizerGIGenericCombiner.inc"
#undef GET_GICOMBINER_CLASS_MEMBERS
};

#define GET_GICOMBINER_IMPL
#include "AIE2GenPostLegalizerGIGenericCombiner.inc"
#undef GET_GICOMBINER_IMPL

AIE2PostLegalizerGenericCombinerImpl::AIE2PostLegalizerGenericCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelValueTracking &VT, GISelCSEInfo *CSEInfo,
    const AIE2PostLegalizerGenericCombinerImplRuleConfig &RuleConfig,
    const AIEBaseSubtarget &STI, MachineDominatorTree *MDT,
    const LegalizerInfo *LI)
    : AIECombinerBase(MF, CInfo, TPC, VT, CSEInfo, RuleConfig, STI, MDT, LI,
                      /*IsPreLegalize=*/false),
#define GET_GICOMBINER_CONSTRUCTOR_INITS
#include "AIE2GenPostLegalizerGIGenericCombiner.inc"
#undef GET_GICOMBINER_CONSTRUCTOR_INITS
{
}
} // end anonymous namespace

std::unique_ptr<Combiner> createAIE2PostLegalizerGenericCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelValueTracking &VT, GISelCSEInfo *CSEInfo, const AIEBaseSubtarget &STI,
    MachineDominatorTree *MDT, const LegalizerInfo *LI) {
  static AIE2PostLegalizerGenericCombinerImplRuleConfig RuleConfig;
  static bool Parsed = [] {
    if (!RuleConfig.parseCommandLineOption())
      report_fatal_error("Invalid rule identifier");
    return true;
  }();
  (void)Parsed;
  return std::make_unique<AIE2PostLegalizerGenericCombinerImpl>(
      MF, CInfo, TPC, VT, CSEInfo, RuleConfig, STI, MDT, LI);
}
