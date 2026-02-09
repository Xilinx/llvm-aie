//=== lib/CodeGen/GlobalISel/AIE2PSPreLegalizerCombiner.cpp -------------===//
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
// before the legalizer.
//
//===--------------------------------------------------------------------===//
#include "AIE2PSTargetMachine.h"
#include "AIECombinerHelper.h"
#include "AIELegalizerHelper.h"
#include "MCTargetDesc/aie2ps/AIE2PSMCTargetDesc.h"
#include "llvm/CodeGen/GlobalISel/CSEInfo.h"
#include "llvm/CodeGen/GlobalISel/Combiner.h"
#include "llvm/CodeGen/GlobalISel/CombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/CombinerInfo.h"
#include "llvm/CodeGen/GlobalISel/GIMatchTableExecutorImpl.h"
#include "llvm/CodeGen/GlobalISel/GISelKnownBits.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/IR/IntrinsicsAIE2PS.h"
#include "llvm/InitializePasses.h"

#define DEBUG_TYPE "aie2ps-prelegalizer-combiner"

#define GET_GICOMBINER_DEPS
#include "AIE2PSGenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_DEPS

using namespace llvm;

extern cl::opt<bool> InlineMemCalls;
extern cl::opt<bool> CombineVecShiftByZero;

namespace {

#define GET_GICOMBINER_TYPES
#include "AIE2PSGenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_TYPES

class AIE2PSPreLegalizerCombinerImpl : public Combiner {
protected:
  // TODO: Make CombinerHelper methods const.
  mutable CombinerHelper Helper;
  const AIE2PSPreLegalizerCombinerImplRuleConfig &RuleConfig;
  const AIE2PSSubtarget &STI;

public:
  AIE2PSPreLegalizerCombinerImpl(
      MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
      GISelKnownBits &KB, GISelCSEInfo *CSEInfo,
      const AIE2PSPreLegalizerCombinerImplRuleConfig &RuleConfig,
      const AIE2PSSubtarget &STI, MachineDominatorTree *MDT,
      const LegalizerInfo *LI);

  static const char *getName() { return "AIE2PSPreLegalizerCombiner"; }

  bool tryCombineAll(MachineInstr &I) const override;

  bool tryCombineAllImpl(MachineInstr &I) const;

  bool tryToCombineIntrinsic(MachineInstr &MI) const;

private:
#define GET_GICOMBINER_CLASS_MEMBERS
#include "AIE2PSGenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_CLASS_MEMBERS
};

#define GET_GICOMBINER_IMPL
#include "AIE2PSGenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_IMPL

AIE2PSPreLegalizerCombinerImpl::AIE2PSPreLegalizerCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelKnownBits &KB, GISelCSEInfo *CSEInfo,
    const AIE2PSPreLegalizerCombinerImplRuleConfig &RuleConfig,
    const AIE2PSSubtarget &STI, MachineDominatorTree *MDT,
    const LegalizerInfo *LI)
    : Combiner(MF, CInfo, TPC, &KB, CSEInfo),
      Helper(Observer, B, /*IsPreLegalize*/ true, &KB, MDT, LI),
      RuleConfig(RuleConfig), STI(STI),
#define GET_GICOMBINER_CONSTRUCTOR_INITS
#include "AIE2PSGenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_CONSTRUCTOR_INITS
{
}

bool AIE2PSPreLegalizerCombinerImpl::tryToCombineIntrinsic(
    MachineInstr &MI) const {

  switch (cast<GIntrinsic>(MI).getIntrinsicID()) {
  case Intrinsic::aie2ps_vshift_I512_I512: {
    return CombineVecShiftByZero &&
           llvm::tryToCombineVectorShiftsByZero(MI, MRI, Observer);
  }
  default:
    break;
  }
  return false;
}

bool AIE2PSPreLegalizerCombinerImpl::tryCombineAll(MachineInstr &MI) const {
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
  case AIE2PS::G_AIE_ZEXT_EXTRACT_VECTOR_ELT:
  case AIE2PS::G_AIE_SEXT_EXTRACT_VECTOR_ELT: {
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

class AIE2PSPreLegalizerCombiner : public MachineFunctionPass {
public:
  static char ID;
  AIE2PSPreLegalizerCombiner();

  StringRef getPassName() const override {
    return "AIE2PSPreLegalizerCombiner";
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
    MachineFunctionPass::getAnalysisUsage(AU);
  }

private:
  AIE2PSPreLegalizerCombinerImplRuleConfig RuleConfig;
};
} // end anonymous namespace

AIE2PSPreLegalizerCombiner::AIE2PSPreLegalizerCombiner()
    : MachineFunctionPass(ID) {
  initializeAIE2PSPreLegalizerCombinerPass(*PassRegistry::getPassRegistry());
  if (!RuleConfig.parseCommandLineOption())
    report_fatal_error("Invalid rule identifier");
}

bool AIE2PSPreLegalizerCombiner::runOnMachineFunction(MachineFunction &MF) {
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

  CombinerInfo CInfo(/*AllowIllegalOps*/ true, /*ShouldLegalizeIllegal*/ false,
                     /*LegalizerInfo*/ nullptr, EnableOpt, F.hasOptSize(),
                     F.hasMinSize());
  AIE2PSPreLegalizerCombinerImpl Impl(MF, CInfo, TPC, *KB, CSEInfo, RuleConfig,
                                      ST, MDT, LI);
  return Impl.combineMachineInstrs();
}

char AIE2PSPreLegalizerCombiner::ID = 0;
INITIALIZE_PASS_BEGIN(AIE2PSPreLegalizerCombiner, DEBUG_TYPE,
                      "Combine AIE2PS machine instrs before legalization",
                      false, false)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_DEPENDENCY(GISelKnownBitsAnalysis)
INITIALIZE_PASS_DEPENDENCY(GISelCSEAnalysisWrapperPass)
INITIALIZE_PASS_END(AIE2PSPreLegalizerCombiner, DEBUG_TYPE,
                    "Combine AIE2PS machine instrs before legalization", false,
                    false)

namespace llvm {
FunctionPass *createAIE2PSPreLegalizerCombiner() {
  return new AIE2PSPreLegalizerCombiner();
}
} // end namespace llvm
