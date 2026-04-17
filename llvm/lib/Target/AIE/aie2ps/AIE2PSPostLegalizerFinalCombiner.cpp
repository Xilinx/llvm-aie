//===-------AIE2PSPostLegalizerFinalCombiner.cpp---------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass does combining of machine instructions at the generic MI level,
// after the legalizer and custom combiner. This is the final combining pass
// to avoid conflicts with other combiners.
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
#include "llvm/CodeGen/GlobalISel/GISelKnownBits.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/InitializePasses.h"

#define DEBUG_TYPE "aie2ps-postlegalizer-final-combiner"

#define GET_GICOMBINER_DEPS
#include "AIE2PSGenPostLegalizerGIFinalCombiner.inc"
#undef GET_GICOMBINER_DEPS

using namespace llvm;

namespace {

#define GET_GICOMBINER_TYPES
#include "AIE2PSGenPostLegalizerGIFinalCombiner.inc"
#undef GET_GICOMBINER_TYPES

class AIE2PSPostLegalizerFinalCombinerImpl
    : public AIECombinerBase<AIE2PSPostLegalizerFinalCombinerImplRuleConfig> {
public:
  AIE2PSPostLegalizerFinalCombinerImpl(
      MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
      GISelKnownBits &KB, GISelCSEInfo *CSEInfo,
      const AIE2PSPostLegalizerFinalCombinerImplRuleConfig &RuleConfig,
      const AIEBaseSubtarget &STI, MachineDominatorTree *MDT,
      const LegalizerInfo *LI);

  static const char *getName() { return "AIE2PSPostLegalizerFinalCombiner"; }

  bool tryCombineAll(MachineInstr &I) const override;

private:
#define GET_GICOMBINER_CLASS_MEMBERS
#include "AIE2PSGenPostLegalizerGIFinalCombiner.inc"
#undef GET_GICOMBINER_CLASS_MEMBERS
};

#define GET_GICOMBINER_IMPL
#include "AIE2PSGenPostLegalizerGIFinalCombiner.inc"
#undef GET_GICOMBINER_IMPL

AIE2PSPostLegalizerFinalCombinerImpl::AIE2PSPostLegalizerFinalCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelKnownBits &KB, GISelCSEInfo *CSEInfo,
    const AIE2PSPostLegalizerFinalCombinerImplRuleConfig &RuleConfig,
    const AIEBaseSubtarget &STI, MachineDominatorTree *MDT,
    const LegalizerInfo *LI)
    : AIECombinerBase(MF, CInfo, TPC, KB, CSEInfo, RuleConfig, STI, MDT, LI,
                      /*IsPreLegalize=*/false),
#define GET_GICOMBINER_CONSTRUCTOR_INITS
#include "AIE2PSGenPostLegalizerGIFinalCombiner.inc"
#undef GET_GICOMBINER_CONSTRUCTOR_INITS
{
}
} // end anonymous namespace

std::unique_ptr<Combiner> createAIE2PSPostLegalizerFinalCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelKnownBits &KB, GISelCSEInfo *CSEInfo, const AIEBaseSubtarget &STI,
    MachineDominatorTree *MDT, const LegalizerInfo *LI) {
  static AIE2PSPostLegalizerFinalCombinerImplRuleConfig RuleConfig;
  static bool Parsed = [] {
    if (!RuleConfig.parseCommandLineOption())
      report_fatal_error("Invalid rule identifier");
    return true;
  }();
  (void)Parsed;
  return std::make_unique<AIE2PSPostLegalizerFinalCombinerImpl>(
      MF, CInfo, TPC, KB, CSEInfo, RuleConfig, STI, MDT, LI);
}
