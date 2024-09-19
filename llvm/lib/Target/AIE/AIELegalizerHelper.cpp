//===- AIELegalizerHelper.cpp --------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements AIE specific legalization functions
//===----------------------------------------------------------------------===//

#include "AIELegalizerHelper.h"
#include "AIEBaseISelLowering.h"
#include "AIEBaseInstrInfo.h"
#include "AIEBaseSubtarget.h"
#include "AIEMachineFunctionInfo.h"
#include "llvm/CodeGen/GlobalISel/GenericMachineInstrs.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/RuntimeLibcalls.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/IR/IntrinsicsAIE2.h"

namespace llvm {

AIELegalizerHelper::AIELegalizerHelper(const AIEBaseSubtarget &ST) : ST(ST) {}

const AIEBaseInstrInfo *AIELegalizerHelper::getInstrInfo() {
  return ST.getInstrInfo();
}

bool AIELegalizerHelper::pack32BitVector(LegalizerHelper &Helper,
                                         MachineInstr &MI,
                                         Register SourceReg) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();

  const LLT SourceRegTy = MRI.getType(SourceReg);
  const Register DstReg = MI.getOperand(0).getReg();
  assert(SourceRegTy.getSizeInBits() == 32 &&
         "cannot pack vectors larger or smaller than 32-bit");

  const LLT S32 = LLT::scalar(32);
  unsigned Offset = 0;
  Register DstCastReg = MRI.createGenericVirtualRegister(S32);

  // Skip the destination operand since that is where we are writing to.
  MachineOperand *Operand = MI.operands_begin() + 1,
                 *OperandEnd = MI.operands_end();
  MIRBuilder.buildConstant(DstCastReg, 0);

  const LLT RegTy = MRI.getType(DstReg);
  while (Operand != OperandEnd) {
    Register DestinationOperand = Operand->getReg();
    const LLT DstOpTy = MRI.getType(DestinationOperand);

    if (DstOpTy.getSizeInBits() != 32) {
      const Register TmpReg32 = MRI.createGenericVirtualRegister(S32);
      MIRBuilder.buildZExt({TmpReg32}, {DestinationOperand});
      DestinationOperand = TmpReg32;
    }

    // Avoid a useless shift for the first element, since it doesn't get
    // optimized out in O0.
    const Register AccumulatorReg = MRI.createGenericVirtualRegister(S32);
    if (Offset != 0) {
      const MachineInstrBuilder ShiftConstant =
          MIRBuilder.buildConstant(S32, Offset);
      const MachineInstrBuilder Masked =
          MIRBuilder.buildShl(S32, DestinationOperand, ShiftConstant);
      MIRBuilder.buildOr(AccumulatorReg, DstCastReg, Masked);
    } else {
      MIRBuilder.buildOr(AccumulatorReg, DstCastReg, DestinationOperand);
    }

    DstCastReg = AccumulatorReg;
    Offset += RegTy.getScalarSizeInBits();
    ++Operand;
  }

  MIRBuilder.buildBitcast(DstReg, DstCastReg);
  MI.eraseFromParent();
  return true;
}

bool AIELegalizerHelper::unpack32BitVector(LegalizerHelper &Helper,
                                           MachineInstr &MI,
                                           Register SourceReg) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();

  const LLT SourceRegTy = MRI.getType(SourceReg);
  assert(SourceRegTy.getSizeInBits() == 32 &&
         "cannot unpack vectors larger or smaller than 32-bit");

  const LLT S32 = LLT::scalar(32);
  unsigned Offset = 0;
  Register DstCastReg = MRI.createGenericVirtualRegister(S32);

  MachineOperand *Operand = MI.operands_begin(),
                 *OperandEnd = MI.operands_end() - 1;
  const LLT RegTy = MRI.getType(Operand->getReg());
  MIRBuilder.buildBitcast(DstCastReg, SourceReg);
  while (Operand != OperandEnd) {
    Register DestinationOperand = Operand->getReg();
    // Avoid a useless shift for the first element, since it doesn't get
    // optimized out in O0.
    if (Offset != 0) {
      const MachineInstrBuilder ShiftConstant =
          MIRBuilder.buildConstant(S32, Offset);
      const MachineInstrBuilder Masked =
          MIRBuilder.buildLShr(S32, DstCastReg, ShiftConstant);
      MIRBuilder.buildTrunc(DestinationOperand, Masked);

    } else {
      MIRBuilder.buildTrunc(DestinationOperand, DstCastReg);
    }

    Offset += RegTy.getScalarSizeInBits();
    ++Operand;
  }

  MI.eraseFromParent();
  return true;
}

/// @brief Get the AIE intrinsic corresponding to the VSHIFT.
static unsigned getVShiftIntrID(const AIEBaseSubtarget &ST) {
  if (ST.isAIE2())
    return Intrinsic::aie2_vshift_I512_I512;

  llvm_unreachable("Called with unknown target triple!");
}

bool AIELegalizerHelper::legalizeG_BUILD_VECTOR(LegalizerHelper &Helper,
                                                MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();

  const Register DstReg = MI.getOperand(0).getReg();
  LLT DstVecTy = MRI.getType(DstReg);
  assert(DstVecTy.isVector());
  unsigned DstVecSize = DstVecTy.getSizeInBits();
  const LLT DstVecEltTy = DstVecTy.getElementType();
  const unsigned EltSize = DstVecEltTy.getScalarSizeInBits();
  assert((EltSize == 8 || EltSize == 16 || EltSize == 32) &&
         "non-existent integer size");
  assert(DstVecSize == 32 || (DstVecSize > 64 && DstVecSize <= 1024 &&
                              "non-native vectors are not supported"));
  assert(DstVecSize < 1024 && "vadd takes a 512-bit argument");

  // If our vector is 32-bit we can store it as packed integer vector
  if (DstVecSize == 32)
    return pack32BitVector(Helper, MI, DstReg);

  // We are using an undef since we are building over multiple instructions
  const TypeSize VecEltTySize = DstVecEltTy.getSizeInBits();
  const LLT VecTy = LLT::fixed_vector(512 / VecEltTySize, DstVecEltTy);
  Register Src = MRI.createGenericVirtualRegister(VecTy);
  MIRBuilder.buildUndef(Src);

  const AIEBaseInstrInfo *II = ST.getInstrInfo();
  MachineOperand *OperandBegin = MI.operands_begin() + 1;
  for (auto &Operand : drop_begin(MI.operands(), 1)) {
    Register EltReg = Operand.getReg();
    LLT EltRegTy = MRI.getType(EltReg);
    Register Dst = MRI.createGenericVirtualRegister(VecTy);

    if (DstVecSize == 512 && &Operand == OperandBegin) {
      Dst = DstReg;
    }

    // vpush takes 32-bit operands so we sign extend the input variable. This is
    // required here since we don't have 16 or 32-bit registers.
    if (DstVecEltTy.getSizeInBits() != 32 && EltRegTy.getSizeInBits() != 32) {
      const Register EltReg32 =
          MRI.createGenericVirtualRegister(LLT::scalar(32));
      MIRBuilder.buildAnyExt({EltReg32}, {EltReg});
      EltReg = EltReg32;
    }

    MIRBuilder.buildInstr(II->getGenericAddVectorEltOpcode(), {Dst},
                          {Src, EltReg});
    Src = Dst;
  }

  // For >512, the G_CONCAT_VECTOR is used instead which is added by the
  // automatic rules.
  // TODO: replace this with G_EXTRACT_SUBVECTOR when it lands into our tree.
  //    https://github.com/llvm/llvm-project/pull/84538
  if (DstVecSize == 256) {
    const Register UnusedSubReg = MRI.createGenericVirtualRegister(DstVecTy);
    // As elements are added from the high bits, the 256-bit result is placed in
    // the upper half of the 512-bit vector.
    MIRBuilder.buildUnmerge({UnusedSubReg, DstReg}, Src);
  } else if (DstVecSize == 128) {
    const LLT V16S32 = LLT::fixed_vector(16, 32);
    Register Vec512Reg = MRI.createGenericVirtualRegister(V16S32);

    Register Zero = MIRBuilder.buildConstant(LLT::scalar(32), 0).getReg(0);
    Register ShiftConstant =
        MIRBuilder.buildConstant(LLT::scalar(32), 48).getReg(0);

    Register NewSrc1 =
        EltSize == 32 ? Src : MIRBuilder.buildBitcast(V16S32, Src).getReg(0);

    // Shift the result to the lower 128 bits of a 512-bit vector, as the
    // result is currently stored in the upper 128 bits of the vector.
    MIRBuilder.buildIntrinsic(getVShiftIntrID(ST), Vec512Reg, false, false)
        .addUse(NewSrc1)
        .addUse(NewSrc1)
        .addUse(Zero)
        .addUse(ShiftConstant);
    Register NewSrc2 =
        EltSize == 32 ? Vec512Reg
                      : MIRBuilder.buildBitcast(VecTy, Vec512Reg).getReg(0);
    MIRBuilder.buildInstr(II->getGenericUnpadVectorOpcode(), {DstReg},
                          {NewSrc2});
  }

  MI.eraseFromParent();
  return true;
}

bool AIELegalizerHelper::legalizeG_UNMERGE_VALUES(LegalizerHelper &Helper,
                                                  MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();
  const AIEBaseInstrInfo *II = ST.getInstrInfo();

  const Register FirstReg = MI.getOperand(0).getReg();
  const Register LastReg = MI.getOperand(MI.getNumOperands() - 1).getReg();
  const LLT FirstTy = MRI.getType(FirstReg);
  const LLT LastTy = MRI.getType(LastReg);
  assert(LastTy.isVector() &&
         (FirstTy.getScalarSizeInBits() * (MI.getNumOperands() - 1)) ==
             LastTy.getSizeInBits() &&
         "This operation is only supported for vectors");

  if (LastTy.getSizeInBits() == 32)
    return unpack32BitVector(Helper, MI, LastReg);

  // Pad vectors of 128-bit vectors to 256-bit
  Register TargetReg = LastReg;
  if (LastTy.getSizeInBits() == 128) {
    const LLT NewRegTy =
        LLT::fixed_vector(LastTy.getNumElements() * 2, LastTy.getScalarType());
    const Register NewReg = MRI.createGenericVirtualRegister(NewRegTy);
    MIRBuilder.buildInstr(II->getGenericPadVectorOpcode(), {NewReg}, {LastReg});
    TargetReg = NewReg;
  }

  const unsigned NumOperands = MI.getNumOperands() - 1;
  for (unsigned Index = 0; Index < NumOperands; ++Index) {
    const Register Current = MI.getOperand(Index).getReg();
    const LLT CurrentTy = MRI.getType(Current);
    assert(CurrentTy.isScalar() &&
           "this operation is only supported for scalar types");

    // We build the constant ourselves since the default behaviour
    // of the builtin is to create 64-bit constants.
    const MachineInstrBuilder CurrentIndex =
        MIRBuilder.buildConstant(LLT::scalar(32), Index);
    MIRBuilder.buildExtractVectorElement(Current, TargetReg, CurrentIndex);
  }

  MI.eraseFromParent();
  return true;
}

bool AIELegalizerHelper::legalizeG_SEXT_INREG(LegalizerHelper &Helper,
                                              MachineInstr &MI) const {

  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();

  const Register DestReg = MI.getOperand(0).getReg();
  const LLT DestRegTy = MRI.getType(DestReg);
  const LLT S32 = LLT::scalar(32);

  const int64_t Imm = MI.getOperand(2).getImm();
  if ((Imm != 8 && Imm != 16) || DestRegTy != S32)
    Helper.lowerSextInreg(MI);

  return true;
}

bool AIELegalizerHelper::legalizeG_VASTART(LegalizerHelper &Helper,
                                           MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineFunction &MF = MIRBuilder.getMF();
  auto *FuncInfo = MF.getInfo<AIEMachineFunctionInfo>();
  Register ListPtr = MI.getOperand(0).getReg();
  LLT PtrTy = MIRBuilder.getMRI()->getType(ListPtr);
  const Align PtrAlign = AIEBaseTargetLowering::getStackArgumentAlignment();

  Register VAList =
      MIRBuilder.buildFrameIndex(PtrTy, FuncInfo->getVarArgsFrameIndex())
          .getReg(0);
  MIRBuilder.buildStore(VAList, ListPtr,
                        *MF.getMachineMemOperand(MachinePointerInfo(),
                                                 MachineMemOperand::MOStore,
                                                 PtrTy, PtrAlign));

  MI.eraseFromParent();
  return true;
}

bool AIELegalizerHelper::legalizeG_VAARG(LegalizerHelper &Helper,
                                         MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineFunction &MF = MIRBuilder.getMF();
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();

  Align Alignment(MI.getOperand(2).getImm());
  const Align PtrAlign = AIEBaseTargetLowering::getStackArgumentAlignment();
  assert((Alignment <= PtrAlign) &&
         "Vaarg alignment is greater than the one of stack slots.");

  // Retrieve the vararg list pointer.
  Register ListPtr = MI.getOperand(1).getReg();
  LLT PtrTy = MRI.getType(ListPtr);
  auto VAList = MIRBuilder.buildLoad(
      PtrTy, ListPtr,
      *MF.getMachineMemOperand(MachinePointerInfo(), MachineMemOperand::MOLoad,
                               PtrTy, PtrAlign));

  // Compute the size of the current vararg slot. That is smallest multiple of
  // PtrAlign which can fit the vararg type.
  Register Dst = MI.getOperand(0).getReg();
  LLT ValTy = MRI.getType(Dst);
  unsigned ArgSize = alignTo(ValTy.getSizeInBytes(), PtrAlign);

  // Compute the address of the current VAARG by subtracting its size
  // from the previous VAARG address.
  LLT IntTy = LLT::scalar(32);
  auto Offset = MIRBuilder.buildConstant(IntTy, -ArgSize);
  auto NewVAList = MIRBuilder.buildPtrAdd(PtrTy, VAList.getReg(0), Offset);

  // Actually load the vararg and feed it into Dst
  MIRBuilder.buildLoad(
      Dst, NewVAList,
      *MF.getMachineMemOperand(MachinePointerInfo(), MachineMemOperand::MOLoad,
                               ValTy, std::max(Alignment, PtrAlign)));

  // Then store the new vararg list pointer so it can be used for next G_VARARG.
  MIRBuilder.buildStore(NewVAList, ListPtr,
                        *MF.getMachineMemOperand(MachinePointerInfo(),
                                                 MachineMemOperand::MOStore,
                                                 PtrTy, PtrAlign));

  MI.eraseFromParent();
  return true;
}

bool AIELegalizerHelper::legalizeMemCalls(
    LegalizerHelper &Helper, MachineInstr &MI,
    LostDebugLocObserver &LocObserver) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  LLVMContext &Ctx = MIRBuilder.getMF().getFunction().getContext();
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();
  Register ResultReg = MRI.createGenericVirtualRegister(LLT::pointer(0, 20));

  RTLIB::Libcall LibEntry = RTLIB::UNKNOWN_LIBCALL;
  PointerType *VoidPtrTy = PointerType::get(Ctx, 0);
  IntegerType *IntTy = Type::getInt32Ty(Ctx);
  LLT S32 = LLT::scalar(32);

  SmallVector<CallLowering::ArgInfo, 3> Args;
  Args.emplace_back(MI.getOperand(0).getReg(), VoidPtrTy, 0);
  MachineInstrBuilder ZextSize = MIRBuilder.buildZExt(S32, MI.getOperand(2));

  switch (MI.getOpcode()) {
  case TargetOpcode::G_MEMSET: {
    LibEntry = RTLIB::MEMSET;
    Register CharReg = MI.getOperand(1).getReg();
    LLT CharLLT = MRI.getType(CharReg);
    if (CharLLT != S32) {
      MachineInstrBuilder ZextChar =
          MIRBuilder.buildZExt(S32, MI.getOperand(1));
      CharReg = ZextChar->getOperand(0).getReg();
    }
    Args.emplace_back(CharReg, IntTy, 0);
    Args.emplace_back(ZextSize->getOperand(0).getReg(), IntTy, 0);
    break;
  }
  case TargetOpcode::G_MEMCPY:
    LibEntry = RTLIB::MEMCPY;
    Args.emplace_back(MI.getOperand(1).getReg(), VoidPtrTy, 0);
    Args.emplace_back(ZextSize->getOperand(0).getReg(), IntTy, 0);
    break;
  case TargetOpcode::G_MEMMOVE:
    LibEntry = RTLIB::MEMMOVE;
    Args.emplace_back(MI.getOperand(1).getReg(), VoidPtrTy, 0);
    Args.emplace_back(ZextSize->getOperand(0).getReg(), IntTy, 0);
    break;
  default:
    return false;
  }

  auto Status = createLibcall(MIRBuilder, LibEntry, {ResultReg, VoidPtrTy, 0},
                              Args, LocObserver);
  if (Status != LegalizerHelper::Legalized) {
    return false;
  }

  MI.eraseFromParent();
  return true;
}

bool AIELegalizerHelper::legalizeG_BRJT(LegalizerHelper &Helper,
                                        MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineFunction &MF = MIRBuilder.getMF();
  LLT S32 = LLT::scalar(32);
  LLT P0 = LLT::pointer(0, 20);
  unsigned EntrySize = MF.getJumpTableInfo()->getEntrySize(MF.getDataLayout());

  auto CopyIndexTo32 = MIRBuilder.buildZExt(S32, MI.getOperand(2));
  auto ConstantShift = MIRBuilder.buildConstant(S32, Log2_32(EntrySize));
  auto LShift = MIRBuilder.buildShl(S32, CopyIndexTo32->getOperand(0),
                                    ConstantShift->getOperand(0));
  auto PtrAdd =
      MIRBuilder.buildPtrAdd(P0, MI.getOperand(0), LShift->getOperand(0));
  auto *MMO = MF.getMachineMemOperand(
      MachinePointerInfo(), MachineMemOperand::MOLoad, P0,
      Align(MF.getJumpTableInfo()->getEntryAlignment(MF.getDataLayout())));
  auto LoadAddress = MIRBuilder.buildLoad(P0, PtrAdd->getOperand(0), *MMO);
  MIRBuilder.buildBrIndirect(LoadAddress->getOperand(0).getReg());

  MI.eraseFromParent();
  return true;
}

bool AIELegalizerHelper::legalizeG_FCONSTANT(LegalizerHelper &Helper,
                                             MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  LLVMContext &Ctx = MIRBuilder.getMF().getFunction().getContext();

  // Convert to integer constants, while preserving the binary representation.
  auto AsInteger = MI.getOperand(1).getFPImm()->getValueAPF().bitcastToAPInt();
  MIRBuilder.buildConstant(MI.getOperand(0), *ConstantInt::get(Ctx, AsInteger));

  MI.eraseFromParent();
  return true;
}

bool AIELegalizerHelper::legalizeG_JUMP_TABLE(LegalizerHelper &Helper,
                                              MachineInstr &MI) const {
  Helper.Observer.changingInstr(MI);
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MI.setDesc(MIRBuilder.getTII().get(TargetOpcode::G_GLOBAL_VALUE));
  Helper.Observer.changedInstr(MI);
  return true;
}

bool AIELegalizerHelper::legalizeG_DYN_STACKALLOC(LegalizerHelper &Helper,
                                                  MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  const LLT P0 = LLT::pointer(0, 20);
  Register Dst = MI.getOperand(0).getReg();
  auto Size = MI.getOperand(1);
  Register SPReg =
      Helper.getTargetLowering().getStackPointerRegisterToSaveRestore();

  auto SPTmp = MIRBuilder.buildCopy(P0, SPReg);
  MIRBuilder.buildCopy(Dst, SPTmp);
  SPTmp = MIRBuilder.buildPtrAdd(P0, SPTmp, Size);
  MIRBuilder.buildCopy(SPReg, SPTmp);

  MI.removeFromParent();
  return true;
}

//%2:_(s8) = G_EXTRACT_VECTOR_ELT %0, %1
//==>
//%3:_(s32) = G_AIE_SEXT_EXTRACT_VECTOR_ELT %0, %1
//%4:_(s32) = G_ASSERT_SEXT %3, Elt_Size
//%2:_(s8) = G_TRUNC %4
bool AIELegalizerHelper::legalizeG_EXTRACT_VECTOR_ELT(LegalizerHelper &Helper,
                                                      MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();
  const Register DstReg = MI.getOperand(0).getReg();
  const Register SrcVecReg = MI.getOperand(1).getReg();
  const Register IdxReg = MI.getOperand(2).getReg();
  const LLT SrcVecTy = MRI.getType(SrcVecReg);
  const unsigned SrcVecSize = SrcVecTy.getSizeInBits();
  const LLT SrcVecEltTy = SrcVecTy.getElementType();
  assert(SrcVecEltTy == MRI.getType(DstReg));

  const LLT S32 = LLT::scalar(32);
  switch (SrcVecSize) {
  case 64: {
    assert(SrcVecTy == LLT::fixed_vector(2, 32) && "Unexpected 64bit vector!");
    const Register Reg0 = MRI.createGenericVirtualRegister(S32);
    const Register Reg1 = MRI.createGenericVirtualRegister(S32);
    MIRBuilder.buildUnmerge({Reg0, Reg1}, SrcVecReg);

    auto IdxVal = getIConstantVRegValWithLookThrough(IdxReg, MRI);
    if (!IdxVal)
      MIRBuilder.buildSelect(DstReg, IdxReg, Reg1, Reg0);
    else {
      const unsigned LaneIdx = IdxVal->Value.getZExtValue();
      if (LaneIdx)
        MIRBuilder.buildCopy(DstReg, Reg1);
      else
        MIRBuilder.buildCopy(DstReg, Reg0);
    }
    break;
  }
  case 256:
  case 512:
  case 1024: {
    const LLT S8 = LLT::scalar(8);
    const LLT S16 = LLT::scalar(16);
    const AIEBaseInstrInfo *II = ST.getInstrInfo();
    bool IsS32 = SrcVecEltTy == S32;
    assert((SrcVecEltTy == S8 || SrcVecEltTy == S16 || IsS32) &&
           "Unexpected vector element type for extract vector elt!");
    if (!IsS32) {
      const Register ExtEltDstReg = MRI.createGenericVirtualRegister(S32);
      const Register ExtDstReg = MRI.createGenericVirtualRegister(S32);
      MIRBuilder.buildInstr(
          II->getGenericExtractVectorEltOpcode(/*SignExt*/ true),
          {ExtEltDstReg}, {SrcVecReg, IdxReg});
      MIRBuilder.buildAssertInstr(TargetOpcode::G_ASSERT_SEXT, ExtDstReg,
                                  ExtEltDstReg, SrcVecEltTy.getSizeInBits());
      MIRBuilder.buildTrunc(DstReg, ExtDstReg);
    } else {
      MIRBuilder.buildInstr(
          II->getGenericExtractVectorEltOpcode(/*SignExt*/ true), {DstReg},
          {SrcVecReg, IdxReg});
    }
    break;
  }
  default:
    llvm_unreachable("Unexpected vector size for extract vector elt!");
  }
  MI.removeFromParent();
  return true;
}

bool AIELegalizerHelper::legalizeG_INSERT_VECTOR_ELT(LegalizerHelper &Helper,
                                                     MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();
  const Register DstVecReg = MI.getOperand(0).getReg();
  const Register SrcVecReg = MI.getOperand(1).getReg();
  const Register ValReg = MI.getOperand(2).getReg();
  const Register IdxReg = MI.getOperand(3).getReg();
  const LLT DstVecTy = MRI.getType(DstVecReg);
  const unsigned DstVecSize = DstVecTy.getSizeInBits();
  const LLT S32 = LLT::scalar(32);

  switch (DstVecSize) {
  case 64: {
    if (DstVecTy != LLT::fixed_vector(2, 32)) {
      llvm_unreachable("Unexpected 64-bit vector type!");
    }
    std::array<Register, 2> Regs = {MRI.createGenericVirtualRegister(S32),
                                    MRI.createGenericVirtualRegister(S32)};
    MIRBuilder.buildUnmerge(Regs, SrcVecReg);
    auto IdxVal = getIConstantVRegValWithLookThrough(IdxReg, MRI);
    if (IdxVal) {
      unsigned Idx = IdxVal->Value.getZExtValue();
      assert(Idx < Regs.size() && "Expected idx to be 0 or 1.");
      Regs[Idx] = ValReg;
      MIRBuilder.buildBuildVector(DstVecReg, Regs);
    } else {
      std::array<Register, 2> TmpSelDsts = {
          MRI.createGenericVirtualRegister(S32),
          MRI.createGenericVirtualRegister(S32)};
      MIRBuilder.buildSelect(TmpSelDsts[0], IdxReg, Regs[0], ValReg);
      MIRBuilder.buildSelect(TmpSelDsts[1], IdxReg, ValReg, Regs[1]);
      MIRBuilder.buildBuildVector(DstVecReg, TmpSelDsts);
    }
    break;
  }
  case 256:
  case 512:
  case 1024: {
    const LLT ValTy = MRI.getType(ValReg);
    const AIEBaseInstrInfo *II = ST.getInstrInfo();
    if (ValTy == LLT::scalar(64)) {
      llvm_unreachable("Unexpected scalar value type for insert vec elt!");
    }
    Register NewValReg;
    if (ValTy == LLT::scalar(8) || ValTy == LLT::scalar(16)) {
      NewValReg = MRI.createGenericVirtualRegister(S32);
      MIRBuilder.buildAnyExt(NewValReg, ValReg);
    } else {
      NewValReg = ValReg;
    }
    MIRBuilder.buildInstr(II->getGenericInsertVectorEltOpcode(), {DstVecReg},
                          {SrcVecReg, NewValReg, IdxReg});
    break;
  }
  default:
    llvm_unreachable("Unexpected vector size for insert vector elt!");
  }
  MI.removeFromParent();
  return true;
}

static RTLIB::Libcall getFCmpLibCall(CmpInst::Predicate Predicate,
                                     CmpInst::Predicate &IPredicate) {
  switch (Predicate) {
  default:
    llvm_unreachable("Unsupported FCmp predicate");
  case CmpInst::FCMP_OEQ:
    IPredicate = CmpInst::ICMP_EQ;
    return RTLIB::OEQ_F32;
  case CmpInst::FCMP_OGE:
    IPredicate = CmpInst::ICMP_SGE;
    return RTLIB::OGE_F32;
  case CmpInst::FCMP_OGT:
    IPredicate = CmpInst::ICMP_SGT;
    return RTLIB::OGT_F32;
  case CmpInst::FCMP_OLE:
    IPredicate = CmpInst::ICMP_SLE;
    return RTLIB::OLE_F32;
  case CmpInst::FCMP_OLT:
    IPredicate = CmpInst::ICMP_SLT;
    return RTLIB::OLT_F32;
  case CmpInst::FCMP_ORD:
    IPredicate = CmpInst::ICMP_EQ;
    return RTLIB::UO_F32;
  /* Unordered comparisons are built from
   * the complement of the ordered ones */
  case CmpInst::FCMP_UGE:
    IPredicate = CmpInst::ICMP_SGE;
    return RTLIB::OLT_F32;
  case CmpInst::FCMP_UGT:
    IPredicate = CmpInst::ICMP_SGT;
    return RTLIB::OLE_F32;
  case CmpInst::FCMP_ULE:
    IPredicate = CmpInst::ICMP_SLE;
    return RTLIB::OGT_F32;
  case CmpInst::FCMP_ULT:
    IPredicate = CmpInst::ICMP_SLT;
    return RTLIB::OGE_F32;
  case CmpInst::FCMP_UNE:
    IPredicate = CmpInst::ICMP_NE;
    return RTLIB::UNE_F32;
  case CmpInst::FCMP_UNO:
    IPredicate = CmpInst::ICMP_NE;
    return RTLIB::UO_F32;
  }
}

bool AIELegalizerHelper::legalizeG_FCMP_FP32(
    LegalizerHelper &Helper, MachineInstr &MI,
    const CmpInst::Predicate FPredicate,
    LostDebugLocObserver &LocObserver) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();
  assert(MRI.getType(MI.getOperand(2).getReg()) == LLT::scalar(32) &&
         MRI.getType(MI.getOperand(3).getReg()) == LLT::scalar(32) &&
         "Expected single precision floating point operands!");

  LLVMContext &Ctx = MIRBuilder.getMF().getFunction().getContext();
  auto DstReg = MI.getOperand(0).getReg();

  RTLIB::Libcall Libcall = RTLIB::UNKNOWN_LIBCALL;
  CmpInst::Predicate IPredicate = CmpInst::BAD_ICMP_PREDICATE;
  SmallVector<std::pair<RTLIB::Libcall, CmpInst::Predicate>, 2> Libcalls;
  switch (FPredicate) {
  /* Compose ordered and unequal operands as follows:
   * a != b ==> a > b || a < b */
  case CmpInst::FCMP_ONE:
    Libcalls.push_back(std::make_pair(RTLIB::OGT_F32, CmpInst::ICMP_SGT));
    Libcalls.push_back(std::make_pair(RTLIB::OLT_F32, CmpInst::ICMP_SLT));
    break;
  /* Compose unordered or equal operands as follows:
   * (unord(a, b) or a == b) ==> a == b || a != b */
  case CmpInst::FCMP_UEQ:
    Libcalls.push_back(std::make_pair(RTLIB::OEQ_F32, CmpInst::ICMP_EQ));
    Libcalls.push_back(std::make_pair(RTLIB::UO_F32, CmpInst::ICMP_NE));
    break;
  default:
    Libcall = getFCmpLibCall(FPredicate, IPredicate);
    Libcalls.push_back(std::make_pair(Libcall, IPredicate));
    break;
  }
  auto *ArgTy = Type::getFloatTy(Ctx);
  auto *RetTy = Type::getInt32Ty(Ctx);

  SmallVector<Register, 2> Results;
  for (auto &[LibEntry, Predicate] : Libcalls) {
    auto LibcallResult = MRI.createGenericVirtualRegister(LLT::scalar(32));
    auto Status = createLibcall(MIRBuilder, LibEntry, {LibcallResult, RetTy, 0},
                                {{MI.getOperand(2).getReg(), ArgTy, 0},
                                 {MI.getOperand(3).getReg(), ArgTy, 0}},
                                LocObserver);

    if (Status != LegalizerHelper::Legalized)
      return false;

    auto NewDstReg =
        Libcalls.size() == 1
            ? DstReg
            : MRI.createGenericVirtualRegister(MRI.getType(DstReg));

    CmpInst::Predicate IDestPred = Predicate;
    // Compare against 0. Example, a ole b is transformed to ole(a, b) <= 0
    assert(CmpInst::isIntPredicate(IDestPred) && "Expected Int Predicate");
    auto Zero = MIRBuilder.buildConstant(LLT::scalar(32), 0);
    MIRBuilder.buildICmp(IDestPred, NewDstReg, LibcallResult, Zero);
    Results.push_back(NewDstReg);
  }
  // OR the results when we have two libcalls
  if (Results.size() != 1) {
    assert(Results.size() == 2 && "Unexpected Number of Results");
    MIRBuilder.buildOr(DstReg, Results[0], Results[1]);
  }
  MI.eraseFromParent();
  return true;
}

/// @brief Get the AIE2 intrinsic corresponding to the fcmp predicate.
unsigned getFCmpIntrID(CmpInst::Predicate Predicate, bool &SwapOperands,
                       bool &PromoteToFP32) {
  switch (Predicate) {
  default:
    PromoteToFP32 = true;
    return 0;
  case CmpInst::FCMP_OGE:
  case CmpInst::FCMP_OEQ:
    return Intrinsic::aie2_vgebf16;
  case CmpInst::FCMP_OLT:
  case CmpInst::FCMP_ONE:
    return Intrinsic::aie2_vltbf16;
  case CmpInst::FCMP_OGT:
    SwapOperands = true;
    return Intrinsic::aie2_vltbf16;
  case CmpInst::FCMP_OLE:
    SwapOperands = true;
    return Intrinsic::aie2_vgebf16;
  }
}

/// Legalize FCMP operations.
/// For single precision floating pt., we use libcalls.
/// For bfloat16, we insert the bf16 elements into a 512bit vector (due to lack
/// of instructions that can directly do floating pt. comparisons), use AIE2
/// intrinsics to compare the vectors and return the S32 where each bit is a
/// comparison o/p for each S16 element in the vector.
/// Ordered predicates mentioned in \ref getFCmpIntrID are lowered to AIE2
/// intrinsics, otherwise, they are promoted to fp32 and supported using
/// libcalls.
bool AIELegalizerHelper::legalizeG_FCMP(
    LegalizerHelper &Helper, MachineInstr &MI,
    LostDebugLocObserver &LocObserver) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();
  auto &CmpMI = cast<GFCmp>(MI);
  const CmpInst::Predicate FPred = CmpMI.getCond();

  const Register DstReg = CmpMI.getReg(0);
  const Register LHS = CmpMI.getLHSReg();
  const Register RHS = CmpMI.getRHSReg();
  assert(MRI.getType(LHS) == MRI.getType(RHS) &&
         "Mismatched operands for G_FCMP");

  assert(CmpInst::isFPPredicate(FPred) && "Unsupported FCmp predicate");

  if (FPred == CmpInst::FCMP_TRUE || FPred == CmpInst::FCMP_FALSE) {
    MIRBuilder.buildConstant(DstReg, FPred == CmpInst::FCMP_TRUE ? 1 : 0);
    MI.eraseFromParent();
    return true;
  }

  const unsigned LHSSize = MRI.getType(LHS).getSizeInBits();
  if (LHSSize == 32)
    return legalizeG_FCMP_FP32(Helper, MI, FPred, LocObserver);

  assert(LHSSize == 16 && "Expected bf16 operands for FCmp");

  const LLT S32 = LLT::scalar(32);

  bool SwapOperands = false, PromoteToFP32 = false;
  const unsigned FCmpIntrID = getFCmpIntrID(FPred, SwapOperands, PromoteToFP32);
  if (PromoteToFP32) {
    const Register FPExtDst1 = MRI.createGenericVirtualRegister(S32);
    const Register FPExtDst2 = MRI.createGenericVirtualRegister(S32);
    MIRBuilder.buildFPExt(FPExtDst1, LHS);
    MIRBuilder.buildFPExt(FPExtDst2, RHS);
    MIRBuilder.buildFCmp(FPred, DstReg, FPExtDst1, FPExtDst2);
    MI.eraseFromParent();
    return true;
  }

  const LLT V32S16 = LLT::fixed_vector(32, 16);
  const Register VecUndef = MRI.createGenericVirtualRegister(V32S16);
  MIRBuilder.buildUndef(VecUndef);
  const Register IdxReg = MRI.createGenericVirtualRegister(S32);
  MIRBuilder.buildConstant(IdxReg, 0);

  auto CreateAndInsert = [&](const Register &SrcReg) {
    Register Vec512Reg = MRI.createGenericVirtualRegister(V32S16);
    MIRBuilder.buildInstr(TargetOpcode::G_INSERT_VECTOR_ELT, {Vec512Reg},
                          {VecUndef, SrcReg, IdxReg});
    return Vec512Reg;
  };

  Register Vec512LHS = CreateAndInsert(LHS);
  Register Vec512RHS = CreateAndInsert(RHS);

  const bool IsFCmpEq = FPred == CmpInst::FCMP_OEQ;
  const bool IsFCmpNEq = FPred == CmpInst::FCMP_ONE;

  if (SwapOperands) {
    std::swap(Vec512LHS, Vec512RHS);
  }

  Register TmpDstReg = DstReg;
  if (IsFCmpNEq || IsFCmpEq) {
    TmpDstReg = MRI.createGenericVirtualRegister(S32);
  }

  MIRBuilder.buildIntrinsic(FCmpIntrID, TmpDstReg, false, false)
      .addUse(Vec512LHS)
      .addUse(Vec512RHS);

  // a != b : a < b || a > b
  // a == b : a >= b && b >= a
  if (IsFCmpNEq || IsFCmpEq) {
    const Register TmpDstReg2 = MRI.createGenericVirtualRegister(S32);
    MIRBuilder.buildIntrinsic(FCmpIntrID, TmpDstReg2, false, false)
        .addUse(Vec512RHS)
        .addUse(Vec512LHS);
    if (IsFCmpEq) {
      MIRBuilder.buildAnd(DstReg, TmpDstReg, TmpDstReg2);
    } else {
      MIRBuilder.buildOr(DstReg, TmpDstReg, TmpDstReg2);
    }
  }

  MI.eraseFromParent();
  return true;
}

bool AIELegalizerHelper::legalizeG_FPTRUNC(LegalizerHelper &Helper,
                                           MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();

  Register DstReg = MI.getOperand(0).getReg();
  Register SrcReg = MI.getOperand(1).getReg();
  LLT DstTy = MRI.getType(DstReg);
  LLT SrcTy = MRI.getType(SrcReg);

  // We only handle single precision to bfloat16 conversion
  if (DstTy != LLT::scalar(16) || SrcTy != LLT::scalar(32))
    return false;

  LLT ACC512 = LLT::fixed_vector(8, 64);
  LLT V16S32 = LLT::fixed_vector(16, 32);
  LLT V16S16 = LLT::fixed_vector(16, 16);
  Register Vec512Reg = MRI.createGenericVirtualRegister(V16S32);
  Register Vec512Undef = MRI.createGenericVirtualRegister(V16S32);
  Register IdxReg = MRI.createGenericVirtualRegister(LLT::scalar(32));
  MIRBuilder.buildUndef(Vec512Undef);
  MIRBuilder.buildConstant(IdxReg, 0);
  MIRBuilder.buildInstr(TargetOpcode::G_INSERT_VECTOR_ELT, {Vec512Reg},
                        {Vec512Undef, SrcReg, IdxReg});

  Register Acc512Reg = MRI.createGenericVirtualRegister(ACC512);
  MIRBuilder.buildBitcast(Acc512Reg, Vec512Reg);

  Register Vec256Reg = MRI.createGenericVirtualRegister(V16S16);
  MIRBuilder
      .buildIntrinsic(Intrinsic::aie2_v16accfloat_to_v16bf16, Vec256Reg, true,
                      false)
      .addUse(Acc512Reg);

  MIRBuilder.buildInstr(TargetOpcode::G_EXTRACT_VECTOR_ELT, {DstReg},
                        {Vec256Reg, IdxReg});
  MI.eraseFromParent();
  return true;
}

bool AIELegalizerHelper::legalizeG_FPEXT(LegalizerHelper &Helper,
                                         MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();

  LLT S32 = LLT::scalar(32);

  Register DstReg = MI.getOperand(0).getReg();
  Register SrcReg = MI.getOperand(1).getReg();
  LLT DstTy = MRI.getType(DstReg);
  LLT SrcTy = MRI.getType(SrcReg);

  // We only handle bfloat16 to single precision conversion
  if (DstTy != LLT::scalar(32) || SrcTy != LLT::scalar(16))
    return false;

  Register AnyExt = MIRBuilder.buildAnyExt(S32, SrcReg).getReg(0);
  Register Cst = MIRBuilder.buildConstant(S32, 16).getReg(0);
  MIRBuilder.buildShl(DstReg, AnyExt, Cst);

  MI.eraseFromParent();
  return true;
}

// Legalized by masking sign bit of both double and float
bool AIELegalizerHelper::legalizeG_FABS(LegalizerHelper &Helper,
                                        MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();

  Register DstReg = MI.getOperand(0).getReg();
  Register SrcReg = MI.getOperand(1).getReg();
  LLT SrcTy = MRI.getType(SrcReg);
  if (SrcTy == LLT::scalar(64)) {
    Register SrcLSB = MRI.createGenericVirtualRegister(LLT::scalar(32));
    Register SrcMSB = MRI.createGenericVirtualRegister(LLT::scalar(32));
    Register AndDst = MRI.createGenericVirtualRegister(LLT::scalar(32));

    MIRBuilder.buildInstr(TargetOpcode::G_UNMERGE_VALUES, {SrcLSB, SrcMSB},
                          {SrcReg});
    auto Ones = MIRBuilder.buildConstant(LLT::scalar(32), 0x7fffffff);
    MIRBuilder.buildAnd(AndDst, SrcMSB, Ones);
    MIRBuilder.buildInstr(TargetOpcode::G_MERGE_VALUES, {DstReg},
                          {SrcLSB, AndDst});
  } else if (SrcTy == LLT::scalar(32)) {
    auto Ones = MIRBuilder.buildConstant(LLT::scalar(32), 0x7fffffff);
    MIRBuilder.buildAnd(DstReg, SrcReg, Ones);
  } else if (SrcTy == LLT::scalar(16)) {
    const LLT S32 = LLT::scalar(32);
    auto AnyExt = MIRBuilder.buildAnyExt(S32, SrcReg);
    auto Ones = MIRBuilder.buildConstant(S32, 0x7fff);
    auto And = MIRBuilder.buildAnd(S32, AnyExt, Ones);
    MIRBuilder.buildTrunc(DstReg, And);
  }

  MI.eraseFromParent();
  return true;
}

bool AIELegalizerHelper::legalizeG_FADDSUB(LegalizerHelper &Helper,
                                           MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();

  Register DstReg = MI.getOperand(0).getReg();
  Register SrcReg1 = MI.getOperand(1).getReg();
  Register SrcReg2 = MI.getOperand(2).getReg();

  assert(MRI.getType(DstReg) == LLT::scalar(16) &&
         "Expected bfloat16 type in custom legalization.");

  const LLT S32 = LLT::scalar(32);
  const LLT V16BF16 = LLT::fixed_vector(16, 16);
  const LLT V16FP32 = LLT::fixed_vector(16, 32);
  const LLT ACC512 = LLT::fixed_vector(8, 64);

  Register NewSrcReg1 = MIRBuilder.buildFPExt(S32, SrcReg1).getReg(0);
  Register NewSrcReg2 = MIRBuilder.buildFPExt(S32, SrcReg2).getReg(0);
  Register IdxReg = MIRBuilder.buildConstant(S32, 0).getReg(0);
  Register Src1Vec = MIRBuilder.buildUndef(V16FP32).getReg(0);
  Register Src2Vec = MIRBuilder.buildUndef(V16FP32).getReg(0);

  const unsigned InsertEltOpc =
      ST.getInstrInfo()->getGenericInsertVectorEltOpcode();
  Register NewSrc1 =
      MIRBuilder
          .buildInstr(InsertEltOpc, {V16FP32}, {Src1Vec, NewSrcReg1, IdxReg})
          .getReg(0);
  Register NewSrc2 =
      MIRBuilder
          .buildInstr(InsertEltOpc, {V16FP32}, {Src2Vec, NewSrcReg2, IdxReg})
          .getReg(0);

  Register FPOp;
  if (MI.getOpcode() == TargetOpcode::G_FADD)
    FPOp = MIRBuilder.buildFAdd(V16FP32, NewSrc1, NewSrc2).getReg(0);
  else
    FPOp = MIRBuilder.buildFSub(V16FP32, NewSrc1, NewSrc2).getReg(0);

  Register FPRes = MIRBuilder.buildBitcast(ACC512, FPOp).getReg(0);
  Register Conv = MIRBuilder
                      .buildIntrinsic(Intrinsic::aie2_v16accfloat_to_v16bf16,
                                      {V16BF16}, true, false)
                      .addUse(FPRes)
                      .getReg(0);

  const Register ExtEltDstReg = MRI.createGenericVirtualRegister(S32);
  const Register ExtDstReg = MRI.createGenericVirtualRegister(S32);
  const unsigned ExtractEltOpc =
      ST.getInstrInfo()->getGenericExtractVectorEltOpcode(/*SignExt*/ true);
  MIRBuilder.buildInstr(ExtractEltOpc, {ExtEltDstReg}, {Conv, IdxReg});
  MIRBuilder.buildAssertInstr(TargetOpcode::G_ASSERT_SEXT, ExtDstReg,
                              ExtEltDstReg, 16);
  MIRBuilder.buildTrunc(DstReg, ExtDstReg);

  MI.eraseFromParent();
  return true;
}

bool AIELegalizerHelper::legalizeLoopDecrement(LegalizerHelper &Helper,
                                               MachineInstr &MI) const {
  assert(MI.getOpcode() == TargetOpcode::G_INTRINSIC_W_SIDE_EFFECTS);

  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  // Insert after our instruction
  MIRBuilder.setInsertPt(*MI.getParent(), ++MI.getIterator());

  Register OrigDst = MI.getOperand(0).getReg();
  Register NewDst =
      MIRBuilder.getMRI()->createGenericVirtualRegister(LLT::scalar(32));
  // NOTE: we don't inform the observer about this change as we do not want to
  // revisit this instruction
  MI.getOperand(0).setReg(NewDst);
  Register ZExtValueReg =
      MIRBuilder.buildAssertZExt(LLT::scalar(32), NewDst, 1).getReg(0);
  MIRBuilder.buildTrunc(OrigDst, ZExtValueReg);
  return true;
}

} // namespace llvm
