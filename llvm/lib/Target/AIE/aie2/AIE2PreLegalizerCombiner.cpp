//=== lib/CodeGen/GlobalISel/AIE2PreLegalizerCombiner.cpp --------------===//
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
// before the legalizer.
//
//===----------------------------------------------------------------------===//

#include "AIE2InstrInfo.h"
#include "AIECombinerBase.h"
#include "AIECombinerHelper.h"
#include "AIECombiners.h"
#include "AIELegalizerHelper.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "llvm/CodeGen/GlobalISel/CSEInfo.h"
#include "llvm/CodeGen/GlobalISel/Combiner.h"
#include "llvm/CodeGen/GlobalISel/CombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/CombinerInfo.h"
#include "llvm/CodeGen/GlobalISel/GIMatchTableExecutorImpl.h"
#include "llvm/CodeGen/GlobalISel/GISelValueTracking.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/IR/IntrinsicsAIE2.h"
#include "llvm/InitializePasses.h"

#define GET_GICOMBINER_DEPS
#include "AIE2GenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_DEPS

using namespace llvm;

extern cl::opt<bool> InlineMemCalls;
extern cl::opt<bool> CombineVecShiftByZero;

static cl::opt<bool> Combine256To512SetExtract(
    "combine-256-to-512-set-extract", cl::init(false), cl::Hidden,
    cl::desc("Combine vectors shift by zero into copies."));
namespace {

#define GET_GICOMBINER_TYPES
#include "AIE2GenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_TYPES

class AIE2PreLegalizerCombinerImpl
    : public AIECombinerBase<AIE2PreLegalizerCombinerImplRuleConfig> {
protected:
  std::map<unsigned, Register>
  getVectorInsertIndices(MachineInstr *CurMI, unsigned SclSrcBits,
                         MachineRegisterInfo &MRI) const;
  bool isTruncExtToS20Sequence(Register DstReg, bool SignVal,
                               unsigned SrcEltSize) const;

public:
  AIE2PreLegalizerCombinerImpl(
      MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
      GISelValueTracking &VT, GISelCSEInfo *CSEInfo,
      const AIE2PreLegalizerCombinerImplRuleConfig &RuleConfig,
      const AIEBaseSubtarget &STI, MachineDominatorTree *MDT,
      const LegalizerInfo *LI);

  static const char *getName() { return "AIE2PreLegalizerCombiner"; }

  bool tryCombineAll(MachineInstr &I) const override;

  bool tryCombineAllImpl(MachineInstr &I) const;

  bool tryToCombineSetExtract(MachineInstr &MI) const;

  bool tryToCombineVectorInserts(MachineInstr &MI, unsigned SclSrcBits) const;

  bool tryToCombineExtBcst(MachineInstr &MI) const;

  bool tryToCombineVExtractElt(MachineInstr &MI) const;

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
    GISelValueTracking &VT, GISelCSEInfo *CSEInfo,
    const AIE2PreLegalizerCombinerImplRuleConfig &RuleConfig,
    const AIEBaseSubtarget &STI, MachineDominatorTree *MDT,
    const LegalizerInfo *LI)
    : AIECombinerBase(MF, CInfo, TPC, VT, CSEInfo, RuleConfig, STI, MDT, LI,
                      /*IsPreLegalize=*/true),
#define GET_GICOMBINER_CONSTRUCTOR_INITS
#include "AIE2GenPreLegalizerGICombiner.inc"
#undef GET_GICOMBINER_CONSTRUCTOR_INITS
{
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

unsigned getVInsertScalarSize(unsigned IntrinsicID) {
  switch (IntrinsicID) {
  case Intrinsic::aie2_vinsert8_I512:
    return 8;
  case Intrinsic::aie2_vinsert16_I512:
    return 16;
  case Intrinsic::aie2_vinsert32_I512:
    return 32;
  default:
    return 0;
  }
}

// Returns a map with InsertIndices and registers holding the insert values.
std::map<unsigned, Register>
AIE2PreLegalizerCombinerImpl::getVectorInsertIndices(
    MachineInstr *CurMI, unsigned SclSrcBits, MachineRegisterInfo &MRI) const {
  std::map<unsigned, Register> RegMap;
  auto IsVInsert = [](const MachineInstr *MI, unsigned SclSrcBits) {
    return isa<GIntrinsic>(MI) &&
           getVInsertScalarSize(cast<GIntrinsic>(*MI).getIntrinsicID()) ==
               SclSrcBits;
  };
  auto IsSet = [](const MachineInstr *MI) {
    return isa<GIntrinsic>(MI) && (cast<GIntrinsic>(*MI).getIntrinsicID() ==
                                       Intrinsic::aie2_set_I512_I128 ||
                                   cast<GIntrinsic>(*MI).getIntrinsicID() ==
                                       Intrinsic::aie2_set_I512_I256);
  };

  while (IsVInsert(CurMI, SclSrcBits)) {
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
  // Avoid bitcast if types match, use copy instead
  if (MRI.getType(DstRegPad) == MRI.getType(DstReg))
    MIRBuilder.buildCopy(DstReg, DstRegPad);
  else
    MIRBuilder.buildBitcast(DstReg, DstRegPad);

  MI.eraseFromParent();
  return true;
}

// Combines vextract and vbroadcast into vextract_broadcast
bool AIE2PreLegalizerCombinerImpl::tryToCombineExtBcst(MachineInstr &MI) const {
  // Returns the combined intrinsicID for matching broadcast and extract ops
  auto getExtBcstIntrinsicID = [](unsigned BcastID,
                                  unsigned ExtID) -> std::optional<unsigned> {
    switch (BcastID) {
    case Intrinsic::aie2_vbroadcast8_I512:
      if (ExtID == Intrinsic::aie2_vextract_elem8_I512)
        return Intrinsic::aie2_vextract_broadcast8_I512;
      break;
    case Intrinsic::aie2_vbroadcast16_I512:
      if (ExtID == Intrinsic::aie2_vextract_elem16_I512)
        return Intrinsic::aie2_vextract_broadcast16_I512;
      break;
    case Intrinsic::aie2_vbroadcast32_I512:
      if (ExtID == Intrinsic::aie2_vextract_elem32_I512)
        return Intrinsic::aie2_vextract_broadcast32_I512;
      break;
    }
    return std::nullopt;
  };
  assert(isa<GIntrinsic>(MI) && "this combine only supports instrinsics");
  const Register DstReg = MI.getOperand(0).getReg();
  MachineInstr *ExtMI = getDefIgnoringCopies(MI.getOperand(2).getReg(), MRI);
  if (!isa<GIntrinsic>(*ExtMI))
    return false;
  // Checks for single use of extracted element
  if (!MRI.hasOneNonDBGUse(ExtMI->getOperand(0).getReg()))
    return false;

  const unsigned BcstID = cast<GIntrinsic>(MI).getIntrinsicID();
  const unsigned ExtID = cast<GIntrinsic>(*ExtMI).getIntrinsicID();
  const std::optional<unsigned> ExtBcstIntrinsicID =
      getExtBcstIntrinsicID(BcstID, ExtID);
  if (!ExtBcstIntrinsicID)
    return false;

  const Register SrcReg = ExtMI->getOperand(2).getReg();
  const Register IdxReg = ExtMI->getOperand(3).getReg();
  MachineIRBuilder MIRBuilder(MI);
  MIRBuilder.buildIntrinsic(*ExtBcstIntrinsicID, DstReg, false, false)
      .addUse(SrcReg)
      .addUse(IdxReg);
  MI.eraseFromParent();

  return true;
}

/// Determines if it is safe to combine vextract by checking the uses of DstReg,
/// specifically for a pattern involving TRUNC followed by EXT.
bool AIE2PreLegalizerCombinerImpl::isTruncExtToS20Sequence(
    Register DstReg, bool SignVal, unsigned SrcEltSize) const {
  // Returns the single non-debug use of a register with a specific opcode
  // and destination size.
  auto GetOneUseWithOpcode =
      [&](const Register Reg, const unsigned OpcodeToCheck,
          const unsigned DstSize) -> std::optional<MachineInstr *> {
    if (MRI.hasOneNonDBGUser(Reg)) {
      MachineInstr &Use = *MRI.use_nodbg_instructions(Reg).begin();
      if (Use.getOpcode() == OpcodeToCheck) {
        const LLT DstRegTy = MRI.getType(Use.getOperand(0).getReg());
        if (DstRegTy.getSizeInBits() == DstSize)
          return &Use;
      }
    }
    return std::nullopt;
  };
  auto Trunc = GetOneUseWithOpcode(DstReg, TargetOpcode::G_TRUNC, SrcEltSize);
  if (!Trunc)
    return false;

  const MachineInstr *TruncMI = *Trunc;
  const unsigned ExtOpcode =
      SignVal ? TargetOpcode::G_SEXT : TargetOpcode::G_ZEXT;
  const Register UseDstReg = TruncMI->getOperand(0).getReg();
  return GetOneUseWithOpcode(UseDstReg, ExtOpcode, 20).has_value();
}

/// \returns true if it is possible to combine the below sequence of MIRs
/// From : %3:_(s32) = G_INTRINSIC
///         intrinsic(@llvm.aie2.vextract.elem[8/16].I512), %2(<32 x s16>),
///         %0(s32), %1(s32)
///        %4:_(s16) = G_TRUNC %3(s32)
///        %5:_(s20) = G_SEXT %4(s16)
/// To :   %9:_(s20) = G_AIE_SEXT_EXTRACT_VECTOR_ELT %2(<32 x s16>), %0(s32)
///        %10:_(s20) = G_ASSERT_[S/Z]EXT %9, 16
///        %4:_(s16) = G_TRUNC %10(s20)
///        %5:_(s20) = G_[S/Z]EXT %4(s16)
/// This combine enables S20Narrowing for vextract
bool AIE2PreLegalizerCombinerImpl::tryToCombineVExtractElt(
    MachineInstr &MI) const {
  const Register DstReg = MI.getOperand(0).getReg();
  // In this case of G_INTRINSIC operand 1 is target intrinsic
  const Register SrcReg = MI.getOperand(2).getReg();
  const Register IdxReg = MI.getOperand(3).getReg();
  const Register SignReg = MI.getOperand(4).getReg();

  const auto SignVal = getIConstantVRegSExtVal(SignReg, MRI);
  if (!SignVal)
    return false;

  const LLT SrcVecTy = MRI.getType(SrcReg);
  const unsigned SrcEltSize = SrcVecTy.getScalarSizeInBits();
  // Checks for the required pattern in uses of DstReg
  if (!isTruncExtToS20Sequence(DstReg, SignVal.value(), SrcEltSize))
    return false;

  auto *TII = static_cast<const AIE2InstrInfo *>(STI.getInstrInfo());
  const unsigned Opcode =
      TII->getGenericExtractVectorEltOpcode(SignVal.value());
  const unsigned AssertExtOpcode = SignVal.value()
                                       ? TargetOpcode::G_ASSERT_SEXT
                                       : TargetOpcode::G_ASSERT_ZEXT;
  const unsigned ExtOpcode =
      SignVal.value() ? TargetOpcode::G_SEXT : TargetOpcode::G_ZEXT;
  const LLT S20 = LLT::scalar(20);
  Register DstReg20Bit = MRI.createGenericVirtualRegister(S20);
  Register ExtReg20Bit = MRI.createGenericVirtualRegister(S20);
  B.setInstrAndDebugLoc(MI);

  B.buildInstr(Opcode, {DstReg20Bit}, {SrcReg, IdxReg});
  B.buildAssertInstr(AssertExtOpcode, ExtReg20Bit, DstReg20Bit, SrcEltSize);
  B.buildInstr(ExtOpcode, {DstReg}, {ExtReg20Bit});
  MI.eraseFromParent();
  return true;
}

bool AIE2PreLegalizerCombinerImpl::tryToCombineIntrinsic(
    MachineInstr &MI) const {
  const unsigned IntrinsicID = cast<GIntrinsic>(MI).getIntrinsicID();
  switch (IntrinsicID) {
  case Intrinsic::aie2_vshift_I512_I512: {
    return CombineVecShiftByZero &&
           llvm::tryToCombineVectorShiftsByZero(MI, MRI, Observer);
  }
  case Intrinsic::aie2_set_I512_I128: {
    return tryToCombineSetExtract(MI);
  }
  case Intrinsic::aie2_set_I512_I256: {
    return Combine256To512SetExtract && tryToCombineSetExtract(MI);
  }
  case Intrinsic::aie2_vinsert8_I512:
  case Intrinsic::aie2_vinsert16_I512:
  case Intrinsic::aie2_vinsert32_I512: {
    return tryToCombineVectorInserts(MI, getVInsertScalarSize(IntrinsicID));
  }
  case Intrinsic::aie2_vbroadcast8_I512:
  case Intrinsic::aie2_vbroadcast16_I512:
  case Intrinsic::aie2_vbroadcast32_I512:
  case Intrinsic::aie2_vbroadcast64_I512: {
    return tryToCombineExtBcst(MI);
  }
  case Intrinsic::aie2_vextract_elem8_I512:
  case Intrinsic::aie2_vextract_elem16_I512: {
    return tryToCombineVExtractElt(MI);
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
  case AIE2::G_AIE_ZEXT_EXTRACT_VECTOR_ELT:
  case AIE2::G_AIE_SEXT_EXTRACT_VECTOR_ELT: {
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

std::unique_ptr<Combiner> createAIE2PreLegalizerCombinerImpl(
    MachineFunction &MF, CombinerInfo &CInfo, const TargetPassConfig *TPC,
    GISelValueTracking &VT, GISelCSEInfo *CSEInfo, const AIEBaseSubtarget &STI,
    MachineDominatorTree *MDT, const LegalizerInfo *LI) {
  static AIE2PreLegalizerCombinerImplRuleConfig RuleConfig;
  static bool Parsed = [] {
    if (!RuleConfig.parseCommandLineOption())
      report_fatal_error("Invalid rule identifier");
    return true;
  }();
  (void)Parsed;
  return std::make_unique<AIE2PreLegalizerCombinerImpl>(
      MF, CInfo, TPC, VT, CSEInfo, RuleConfig, STI, MDT, LI);
}
