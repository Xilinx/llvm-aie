//=== lib/CodeGen/GlobalISel/AIE2PPreLegalizerCombiner.cpp --------------===//
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
// before the legalizer.
//
//===--------------------------------------------------------------------===//

#include "AIECombinerBase.h"
#include "AIECombinerHelper.h"
#include "AIECombiners.h"
#include "AIELegalizerHelper.h"
#include "MCTargetDesc/aie2p/AIE2PMCTargetDesc.h"
#include "llvm/CodeGen/GlobalISel/CSEInfo.h"
#include "llvm/CodeGen/GlobalISel/Combiner.h"
#include "llvm/CodeGen/GlobalISel/CombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/CombinerInfo.h"
#include "llvm/CodeGen/GlobalISel/GIMatchTableExecutorImpl.h"
#include "llvm/CodeGen/GlobalISel/GISelValueTracking.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/IR/IntrinsicsAIE2P.h"
#include "llvm/InitializePasses.h"

#define GET_GICOMBINER_DEPS
#include "AIE2PGenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_DEPS

using namespace llvm;

extern cl::opt<bool> InlineMemCalls;
extern cl::opt<bool> CombineVecShiftByZero;

namespace {

#define GET_GICOMBINER_TYPES
#include "AIE2PGenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_TYPES

class AIE2PPreLegalizerCombinerImpl
    : public AIECombinerBase<AIE2PPreLegalizerCombinerImplRuleConfig> {
protected:
public:
  AIE2PPreLegalizerCombinerImpl(
      MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
      GISelValueTracking &VT, GISelCSEInfo *CSEInfo,
      const AIE2PPreLegalizerCombinerImplRuleConfig &RuleConfig,
      const AIEBaseSubtarget &STI, MachineDominatorTree *MDT,
      const LegalizerInfo *LI);

  static const char *getName() { return "AIE2PPreLegalizerCombiner"; }

  bool tryCombineAll(MachineInstr &I) const override;

  bool tryCombineAllImpl(MachineInstr &I) const;

  bool tryToCombineIntrinsic(MachineInstr &MI) const;

private:
#define GET_GICOMBINER_CLASS_MEMBERS
#include "AIE2PGenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_CLASS_MEMBERS
};

#define GET_GICOMBINER_IMPL
#include "AIE2PGenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_IMPL

AIE2PPreLegalizerCombinerImpl::AIE2PPreLegalizerCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelValueTracking &VT, GISelCSEInfo *CSEInfo,
    const AIE2PPreLegalizerCombinerImplRuleConfig &RuleConfig,
    const AIEBaseSubtarget &STI, MachineDominatorTree *MDT,
    const LegalizerInfo *LI)
    : AIECombinerBase(MF, CInfo, TPC, VT, CSEInfo, RuleConfig, STI, MDT, LI,
                      /*IsPreLegalize=*/true),
#define GET_GICOMBINER_CONSTRUCTOR_INITS
#include "AIE2PGenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_CONSTRUCTOR_INITS
{
}

bool AIE2PPreLegalizerCombinerImpl::tryToCombineIntrinsic(
    MachineInstr &MI) const {

  switch (cast<GIntrinsic>(MI).getIntrinsicID()) {
  case Intrinsic::aie2p_vshift_I512_I512: {
    return CombineVecShiftByZero &&
           llvm::tryToCombineVectorShiftsByZero(MI, MRI, Observer);
  }
  default:
    break;
  }
  return false;
}

bool AIE2PPreLegalizerCombinerImpl::tryCombineAll(MachineInstr &MI) const {
  if (tryCombineAllImpl(MI))
    return true;

  unsigned Opc = MI.getOpcode();
  switch (Opc) {

  case TargetOpcode::G_MEMCPY:
  case TargetOpcode::G_MEMMOVE:
  case TargetOpcode::G_MEMSET: {
    if (InlineMemCalls &&
        Helper.tryCombineMemCpyFamily(MI, 0 /*Use fed by TargetLowering*/))
      return true;
    break;
  }
  case TargetOpcode::G_INTRINSIC: {
    return tryToCombineIntrinsic(MI);
  }
  case TargetOpcode::G_SHUFFLE_VECTOR: {
    Register Src = MI.getOperand(1).getReg();
    LLT SrcType = MRI.getType(Src);
    // We support concat for vector sizes greater than 128 bits as well as those
    // of 32 bits.
    const unsigned SrcSize = SrcType.getSizeInBits();
    if (SrcSize >= 128 || SrcSize == 32) {
      return Helper.tryCombineShuffleVector(MI);
    }
    break;
  }
  case AIE2P::G_AIE_ZEXT_EXTRACT_VECTOR_ELT:
  case AIE2P::G_AIE_SEXT_EXTRACT_VECTOR_ELT: {
    const LLT SrcVecTy = MRI.getType(MI.getOperand(1).getReg());
    const unsigned BasicVecSize = STI.getInstrInfo()->getBasicVectorBitSize();
    if (SrcVecTy.getSizeInBits() != BasicVecSize) {
      LegalizerHelper Helper(*MI.getMF(), Observer, B);
      AIELegalizerHelper AIEHelper(STI);
      return AIEHelper.legalizeG_AIE_EXTRACT_VECTOR_ELT(Helper, MI,
                                                        BasicVecSize);
    }
    break;
  }
  default:
    break;
  }

  return false;
}
} // end anonymous namespace

std::unique_ptr<Combiner> createAIE2PPreLegalizerCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelValueTracking &VT, GISelCSEInfo *CSEInfo, const AIEBaseSubtarget &STI,
    MachineDominatorTree *MDT, const LegalizerInfo *LI) {
  static AIE2PPreLegalizerCombinerImplRuleConfig RuleConfig;
  static bool Parsed = [] {
    if (!RuleConfig.parseCommandLineOption())
      report_fatal_error("Invalid rule identifier");
    return true;
  }();
  (void)Parsed;
  return std::make_unique<AIE2PPreLegalizerCombinerImpl>(
      MF, CInfo, TPC, VT, CSEInfo, RuleConfig, STI, MDT, LI);
}
