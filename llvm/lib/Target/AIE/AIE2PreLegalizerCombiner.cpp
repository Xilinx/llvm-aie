//=== lib/CodeGen/GlobalISel/AIE2PreLegalizerCombiner.cpp --------------===//
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
// before the legalizer.
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
#include "llvm/IR/IntrinsicsAIE2.h"
#include "llvm/InitializePasses.h"

#define DEBUG_TYPE "aie2-prelegalizer-combiner"

#define GET_GICOMBINER_DEPS
#include "AIE2GenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_DEPS

using namespace llvm;

static cl::opt<bool>
    InlineMemCalls("aie-inline-mem-calls", cl::init(true), cl::Hidden,
                   cl::desc("Inline mem calls when profitable."));

static cl::opt<bool> CombineVecShiftByZero(
    "aie-combine-vec-shift-by-zero", cl::init(true), cl::Hidden,
    cl::desc("Combine vectors shift by zero into copies."));

static cl::opt<bool> Combine256To512SetExtract(
    "combine-256-to-512-set-extract", cl::init(false), cl::Hidden,
    cl::desc("Combine vectors shift by zero into copies."));
namespace {

#define GET_GICOMBINER_TYPES
#include "AIE2GenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_TYPES

class AIE2PreLegalizerCombinerImpl : public Combiner {
protected:
  // TODO: Make CombinerHelper methods const.
  mutable CombinerHelper Helper;
  const AIE2PreLegalizerCombinerImplRuleConfig &RuleConfig;
  const AIE2Subtarget &STI;
  std::map<unsigned, Register>
  getVectorInsertIndices(MachineInstr *CurMI, unsigned SclSrcBits,
                         MachineRegisterInfo &MRI) const;

public:
  AIE2PreLegalizerCombinerImpl(
      MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
      GISelKnownBits &KB, GISelCSEInfo *CSEInfo,
      const AIE2PreLegalizerCombinerImplRuleConfig &RuleConfig,
      const AIE2Subtarget &STI, MachineDominatorTree *MDT,
      const LegalizerInfo *LI);

  static const char *getName() { return "AIE2PreLegalizerCombiner"; }

  bool tryCombineAll(MachineInstr &I) const override;

  bool tryCombineAllImpl(MachineInstr &I) const;

  bool tryToCombineVectorShiftsByZero(MachineInstr &MI) const;

  bool tryToCombineSetExtract(MachineInstr &MI) const;

  bool tryToCombineVectorInserts(MachineInstr &MI, unsigned SclSrcBits) const;

  bool tryToCombineIntrinsic(MachineInstr &MI) const;

private:
#define GET_GICOMBINER_CLASS_MEMBERS
#include "AIE2GenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_CLASS_MEMBERS
};

#define GET_GICOMBINER_IMPL
#include "AIE2GenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_IMPL

AIE2PreLegalizerCombinerImpl::AIE2PreLegalizerCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelKnownBits &KB, GISelCSEInfo *CSEInfo,
    const AIE2PreLegalizerCombinerImplRuleConfig &RuleConfig,
    const AIE2Subtarget &STI,
    MachineDominatorTree *MDT,
    const LegalizerInfo *LI)
    : Combiner(MF, CInfo, TPC, &KB, CSEInfo),
      Helper(Observer, B, /*IsPreLegalize*/ false, &KB, MDT, LI),
      RuleConfig(RuleConfig), STI(STI),
#define GET_GICOMBINER_CONSTRUCTOR_INITS
#include "AIE2GenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_CONSTRUCTOR_INITS
{
}

/// \returns true if it is possible to combine the below sequence of MIRs
/// into a COPY.
/// From : %1:_(<64 x s8>) = G_INTRINSIC intrinsic(@llvm.aie2.v64int8/v32int16)
///        %2:_(<16 x s32>) = G_BITCAST %1:_(<64 x s8>)
///        %3:_(s32) = G_CONSTANT i32 0
///        %4:_(<16 x s32>) = G_INTRINSIC
///        intrinsic(@llvm.aie2.vshift.I512.I512), %X:_(<16 x s32>), %2:_(<16 x
///        s32>), %3:_(s32), %3:_(s32)
/// To :   4%:_(<16 x s32>) = COPY %X
/// Or even:
/// From : %1:_(<64 x s8>) = G_INTRINSIC intrinsic(@llvm.aie2.v16int32)
///        %2:_(s32) = G_CONSTANT i32 0
///        %3:_(<16 x s32>) = G_INTRINSIC
///        intrinsic(@llvm.aie2.vshift.I512.I512), %X:_(<16 x s32>), %1:_(<16 x
///        s32>), %2:_(s32), %2:_(s32)
/// To :   3%:_(<16 x s32>) = COPY %X
bool AIE2PreLegalizerCombinerImpl::tryToCombineVectorShiftsByZero(
    MachineInstr &MI) const {

  const Register DstReg = MI.getOperand(0).getReg();
  const Register SrcReg = MI.getOperand(2).getReg();
  const Register ThirdSrcReg = MI.getOperand(4).getReg();
  const Register ShiftAmtSrcReg = MI.getOperand(5).getReg();

  auto IsConstantZeroReg = [&](const Register Reg) {
    auto Cst = getIConstantVRegValWithLookThrough(Reg, MRI);
    return Cst && Cst->Value.isZero();
  };

  if (!IsConstantZeroReg(ThirdSrcReg) || !IsConstantZeroReg(ShiftAmtSrcReg))
    return false;

  MachineIRBuilder MIRBuilder(MI);
  MIRBuilder.buildCopy(DstReg, SrcReg);
  MI.eraseFromParent();

  return true;
}

bool AIE2PreLegalizerCombinerImpl::tryToCombineSetExtract(
    MachineInstr &MI) const {
  const Register DstReg = MI.getOperand(0).getReg();
  MachineInstr *ExtOp = getDefIgnoringCopies(MI.getOperand(2).getReg(), MRI);

  if (!isa<GIntrinsic>(MI) || !isa<GIntrinsic>(*ExtOp))
    return false;
  switch (cast<GIntrinsic>(MI).getIntrinsicID()) {
  case Intrinsic::aie2_set_I512_I128: {
    if (cast<GIntrinsic>(*ExtOp).getIntrinsicID() !=
        Intrinsic::aie2_extract_I128_I512)
      return false;
    break;
  }
  case Intrinsic::aie2_set_I512_I256: {
    if (cast<GIntrinsic>(*ExtOp).getIntrinsicID() !=
        Intrinsic::aie2_ext_I256_I512)
      return false;
    const Register SetOpIdxReg = MI.getOperand(3).getReg();
    const Register ExtOpIdxReg = ExtOp->getOperand(3).getReg();
    auto SetOpCst = getIConstantVRegValWithLookThrough(SetOpIdxReg, MRI);
    auto ExtOpCst = getIConstantVRegValWithLookThrough(ExtOpIdxReg, MRI);
    if (SetOpIdxReg != ExtOpIdxReg &&
        (!SetOpCst || !ExtOpCst ||
         SetOpCst->Value.getZExtValue() != ExtOpCst->Value.getZExtValue()))
      return false;
    break;
  }
  default:
    return false;
  }

  MachineIRBuilder MIRBuilder(MI);
  MIRBuilder.buildCopy(DstReg, ExtOp->getOperand(2).getReg());
  MI.eraseFromParent();

  return true;
}

// Returns a map with InsertIndices and registers holding the insert values.
std::map<unsigned, Register>
AIE2PreLegalizerCombinerImpl::getVectorInsertIndices(
    MachineInstr *CurMI, unsigned SclSrcBits, MachineRegisterInfo &MRI) const {
  std::map<unsigned, Register> RegMap;
  auto Is8BitVInsert = [](const MachineInstr *MI) {
    return isa<GIntrinsic>(MI) && cast<GIntrinsic>(*MI).getIntrinsicID() ==
                                      Intrinsic::aie2_vinsert8_I512;
  };
  auto IsSet = [](const MachineInstr *MI) {
    return isa<GIntrinsic>(MI) && (cast<GIntrinsic>(*MI).getIntrinsicID() ==
                                       Intrinsic::aie2_set_I512_I128 ||
                                   cast<GIntrinsic>(*MI).getIntrinsicID() ==
                                       Intrinsic::aie2_set_I512_I256);
  };

  while (Is8BitVInsert(CurMI)) {
    // In this case of G_INTRINSIC operand 1 is target intrinsic
    const Register SrcReg = CurMI->getOperand(2).getReg();
    const Register IdxReg = CurMI->getOperand(3).getReg();
    const Register SclSrcReg = CurMI->getOperand(4).getReg();

    // Collecting registers and their indices
    auto Cst = getIConstantVRegValWithLookThrough(IdxReg, MRI);
    if (!Cst ||
        !RegMap.try_emplace(Cst->Value.getZExtValue(), SclSrcReg).second)
      return {};
    CurMI = getDefIgnoringCopies(SrcReg, MRI);

    // Combining Set and Extract to fetch next VInsert
    if (IsSet(CurMI) && tryToCombineSetExtract(*CurMI))
      CurMI = getDefIgnoringCopies(SrcReg, MRI);
  }

  // For 128/256-bit vectors, not all lanes are explicitly defined. If the
  // source MI is identified as a Set intrinsic that sets the required lanes,
  // the transformation can proceed safely.
  if (!IsSet(CurMI))
    return {};
  unsigned DstRegBits =
      MRI.getType(CurMI->getOperand(2).getReg()).getSizeInBits();
  // Check for the right amount of lanes matching the size of input vector of
  // Set instrinsic.
  if (DstRegBits != RegMap.size() * SclSrcBits)
    return {};
  return RegMap;
}

/// Look for VINSERT sequence that can be rewritten as G_BUILD_VECTOR_TRUNC
bool AIE2PreLegalizerCombinerImpl::tryToCombineVectorInserts(
    MachineInstr &MI, unsigned SclSrcBits) const {
  std::map<unsigned, Register> RegMap;
  MachineInstr *CurMI = &MI;
  const Register DstReg = MI.getOperand(0).getReg();
  unsigned DstRegBits = MRI.getType(DstReg).getSizeInBits();

  auto InsertIndices = getVectorInsertIndices(CurMI, SclSrcBits, MRI);
  unsigned DstRegLen = InsertIndices.size();
  if (DstRegLen == 0)
    return false;

  MachineIRBuilder MIRBuilder(MI);
  SmallVector<Register, 16> Regs;
  // Collect registers in order for G_BUILD_VECTOR_TRUNC
  for (unsigned I = 0; I < DstRegLen; I++) {
    auto It = InsertIndices.find(I);
    if (It == InsertIndices.end())
      return false;
    Regs.push_back(It->second);
  }
  Register DstRegTrunc = MRI.createGenericVirtualRegister(
      LLT::fixed_vector(DstRegLen, SclSrcBits));
  Register DstRegPad = MRI.createGenericVirtualRegister(
      LLT::fixed_vector(DstRegBits / SclSrcBits, SclSrcBits));

  MIRBuilder.buildBuildVectorTrunc(DstRegTrunc, Regs);
  MIRBuilder.buildInstr(AIE2::G_AIE_PAD_VECTOR_UNDEF, {DstRegPad},
                        {DstRegTrunc});
  MIRBuilder.buildBitcast(DstReg, DstRegPad);

  MI.eraseFromParent();
  return true;
}

bool AIE2PreLegalizerCombinerImpl::tryToCombineIntrinsic(
    MachineInstr &MI) const {

  switch (cast<GIntrinsic>(MI).getIntrinsicID()) {
  case Intrinsic::aie2_vshift_I512_I512: {
    return CombineVecShiftByZero && tryToCombineVectorShiftsByZero(MI);
  }
  case Intrinsic::aie2_set_I512_I128: {
    return tryToCombineSetExtract(MI);
  }
  case Intrinsic::aie2_set_I512_I256: {
    return Combine256To512SetExtract && tryToCombineSetExtract(MI);
  }
  case Intrinsic::aie2_vinsert8_I512: {
    return tryToCombineVectorInserts(MI, 8);
  }
  default:
    break;
  }
  return false;
}

bool AIE2PreLegalizerCombinerImpl::tryCombineAll(MachineInstr &MI) const {
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
  default:
    break;
  }

  return false;
}

class AIE2PreLegalizerCombiner : public MachineFunctionPass {
public:
  static char ID;

  AIE2PreLegalizerCombiner();

  StringRef getPassName() const override { return "AIE2PreLegalizerCombiner"; }

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
  AIE2PreLegalizerCombinerImplRuleConfig RuleConfig;
};
} // end anonymous namespace

AIE2PreLegalizerCombiner::AIE2PreLegalizerCombiner()
    : MachineFunctionPass(ID) {
  initializeAIE2PreLegalizerCombinerPass(*PassRegistry::getPassRegistry());
}

bool AIE2PreLegalizerCombiner::runOnMachineFunction(MachineFunction &MF) {
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
  AIE2PreLegalizerCombinerImpl Impl(MF, CInfo, TPC, *KB, CSEInfo,
                                        RuleConfig, ST, MDT, LI);
  return Impl.combineMachineInstrs();
}

char AIE2PreLegalizerCombiner::ID = 0;
INITIALIZE_PASS_BEGIN(AIE2PreLegalizerCombiner, DEBUG_TYPE,
                      "Combine AIE2 machine instrs before legalization", false,
                      false)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_DEPENDENCY(GISelKnownBitsAnalysis)
INITIALIZE_PASS_DEPENDENCY(GISelCSEAnalysisWrapperPass)
INITIALIZE_PASS_END(AIE2PreLegalizerCombiner, DEBUG_TYPE,
                    "Combine AIE2 machine instrs before legalization", false,
                    false)

namespace llvm {
FunctionPass *createAIE2PreLegalizerCombiner() {
  return new AIE2PreLegalizerCombiner();
}
} // end namespace llvm
