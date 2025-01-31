//=== lib/CodeGen/GlobalISel/AIE2PPreLegalizerCombiner.cpp --------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===--------------------------------------------------------------------===//
//
// This pass does combining of machine instructions at the generic MI level,
// before the legalizer.
//
//===--------------------------------------------------------------------===//

#include "AIE2PTargetMachine.h"
#include "AIECombinerHelper.h"
#include "AIELegalizerHelper.h"
#include "MCTargetDesc/aie2p/AIE2PMCTargetDesc.h"
#include "llvm/CodeGen/GlobalISel/CSEInfo.h"
#include "llvm/CodeGen/GlobalISel/Combiner.h"
#include "llvm/CodeGen/GlobalISel/CombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/CombinerInfo.h"
#include "llvm/CodeGen/GlobalISel/GIMatchTableExecutorImpl.h"
#include "llvm/CodeGen/GlobalISel/GISelKnownBits.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/IR/IntrinsicsAIE2P.h"
#include "llvm/InitializePasses.h"

#define DEBUG_TYPE "aie2p-prelegalizer-combiner"

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

class AIE2PPreLegalizerCombinerImpl : public Combiner {
protected:
  // TODO: Make CombinerHelper methods const.
  mutable CombinerHelper Helper;
  const AIE2PPreLegalizerCombinerImplRuleConfig &RuleConfig;
  const AIE2PSubtarget &STI;

public:
  AIE2PPreLegalizerCombinerImpl(
      MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
      GISelKnownBits &KB, GISelCSEInfo *CSEInfo,
      const AIE2PPreLegalizerCombinerImplRuleConfig &RuleConfig,
      const AIE2PSubtarget &STI, MachineDominatorTree *MDT,
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
    GISelKnownBits &KB, GISelCSEInfo *CSEInfo,
    const AIE2PPreLegalizerCombinerImplRuleConfig &RuleConfig,
    const AIE2PSubtarget &STI, MachineDominatorTree *MDT,
    const LegalizerInfo *LI)
    : Combiner(MF, CInfo, TPC, &KB, CSEInfo),
      Helper(Observer, B, /*IsPreLegalize*/ false, &KB, MDT, LI),
      RuleConfig(RuleConfig), STI(STI),
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
           llvm::tryToCombineVectorShiftsByZero(MI, MRI);
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
    // We support concat for vector sizes greater than 128 bits.
    if (SrcType.getSizeInBits() >= 128) {
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

class AIE2PPreLegalizerCombiner : public MachineFunctionPass {
public:
  static char ID;

  AIE2PPreLegalizerCombiner();

  StringRef getPassName() const override { return "AIE2PPreLegalizerCombiner"; }

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
  AIE2PPreLegalizerCombinerImplRuleConfig RuleConfig;
};
} // end anonymous namespace

AIE2PPreLegalizerCombiner::AIE2PPreLegalizerCombiner()
    : MachineFunctionPass(ID) {
  initializeAIE2PPreLegalizerCombinerPass(*PassRegistry::getPassRegistry());
}

bool AIE2PPreLegalizerCombiner::runOnMachineFunction(MachineFunction &MF) {
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

  const AIE2PSubtarget &ST = MF.getSubtarget<AIE2PSubtarget>();
  const auto *LI = ST.getLegalizerInfo();

  GISelKnownBits *KB = &getAnalysis<GISelKnownBitsAnalysis>().get(MF);
  MachineDominatorTree *MDT = &getAnalysis<MachineDominatorTree>();

  CombinerInfo CInfo(/*AllowIllegalOps*/ true, /*ShouldLegalizeIllegal*/ false,
                     /*LegalizerInfo*/ nullptr, EnableOpt, F.hasOptSize(),
                     F.hasMinSize());
  AIE2PPreLegalizerCombinerImpl Impl(MF, CInfo, TPC, *KB, CSEInfo, RuleConfig,
                                     ST, MDT, LI);
  return Impl.combineMachineInstrs();
}

char AIE2PPreLegalizerCombiner::ID = 0;
INITIALIZE_PASS_BEGIN(AIE2PPreLegalizerCombiner, DEBUG_TYPE,
                      "Combine AIE2P machine instrs before legalization", false,
                      false)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_DEPENDENCY(GISelKnownBitsAnalysis)
INITIALIZE_PASS_DEPENDENCY(GISelCSEAnalysisWrapperPass)
INITIALIZE_PASS_END(AIE2PPreLegalizerCombiner, DEBUG_TYPE,
                    "Combine AIE2P machine instrs before legalization", false,
                    false)

namespace llvm {
FunctionPass *createAIE2PPreLegalizerCombiner() {
  return new AIE2PPreLegalizerCombiner();
}
} // end namespace llvm
