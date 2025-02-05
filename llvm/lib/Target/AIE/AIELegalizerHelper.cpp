//===- AIELegalizerHelper.cpp --------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2025 Advanced Micro Devices, Inc. or its affiliates
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
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/GlobalISel/GenericMachineInstrs.h"
#include "llvm/CodeGen/GlobalISel/LegalizerHelper.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RuntimeLibcalls.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/IR/IntrinsicsAIE2.h"
#include "llvm/IR/IntrinsicsAIE2P.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

AIELegalizerHelper::AIELegalizerHelper(const AIEBaseSubtarget &ST) : ST(ST) {}

const AIEBaseInstrInfo *AIELegalizerHelper::getInstrInfo() {
  return ST.getInstrInfo();
}

/// Append the result registers of G_UNMERGE_VALUES \p MI to \p Regs.
static void getUnmergeResults(SmallVectorImpl<Register> &Regs,
                              const MachineInstr &MI) {
  assert(MI.getOpcode() == TargetOpcode::G_UNMERGE_VALUES);

  const int StartIdx = Regs.size();
  const int NumResults = MI.getNumOperands() - 1;
  Regs.resize(Regs.size() + NumResults);
  for (int I = 0; I != NumResults; ++I)
    Regs[StartIdx + I] = MI.getOperand(I).getReg();
}

static Register emitPadUndefVector(MachineRegisterInfo &MRI,
                                   MachineIRBuilder &MIRBuilder,
                                   const LLT WideTy, Register SrcReg) {

  const LLT OrigTy = MRI.getType(SrcReg);
  assert(WideTy.getSizeInBits() % OrigTy.getSizeInBits() == 0 &&
         "Expected to pad to a multiple of the src type");
  const unsigned NumPadElts =
      WideTy.getSizeInBits() / OrigTy.getSizeInBits() - 1;

  const Register UndefReg = MRI.createGenericVirtualRegister(OrigTy);
  MIRBuilder.buildUndef(UndefReg);

  SmallVector<Register, 4> Regs;
  Regs.push_back(SrcReg);
  for (unsigned I = 0; I < NumPadElts; I++)
    Regs.push_back(UndefReg);
  const Register NewSrcReg = MRI.createGenericVirtualRegister(WideTy);
  MIRBuilder.buildMergeLikeInstr(NewSrcReg, Regs);
  return NewSrcReg;
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
  if (ST.isAIE2P())
    return Intrinsic::aie2p_vshift_I512_I512;
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
  if (!ST.isAIE2P()) {
    assert(EltSize != 64 && "Not expected 64-bit elmt type vector!");
  }

  assert((EltSize == 8 || EltSize == 16 || EltSize == 32 || EltSize == 64) &&
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
  MachineOperand *OperandEnd = std::prev(MI.operands_end());
  for (auto &Operand : drop_begin(MI.operands(), 1)) {
    Register EltReg = Operand.getReg();
    LLT EltRegTy = MRI.getType(EltReg);
    Register Dst = MRI.createGenericVirtualRegister(VecTy);

    if (DstVecSize == 512 && &Operand == OperandEnd) {
      Dst = DstReg;
    }

    // vpush takes 32/64-bit operands so we sign extend the input variable. This
    // is required here since we don't have 8 or 16-bit registers.
    if (DstVecEltTy.getSizeInBits() < 32 && EltRegTy.getSizeInBits() != 32) {
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

// Legalize G_UNMERGE_VALUES of 256-bit vector into 2 x 128-bit vector. For an
// example,
//     %1:_(<4 x s32>), %2:_(<4 x s32>) = G_UNMERGE_VALUES %0:_(<8 x s32>)
//  to
//     %1:_(<4 x s32>) = G_AIE_UNPAD_VECTOR %0(<8 x s32>)
//     %3:_(s32) = G_CONSTANT i32 16
//     %4:_(<16 x s32>) = G_CONCAT_VECTORS %0(<8 x s32>), %0(<8 x s32>)
//     %5:_(<16 x s32>) = G_IMPLICIT_DEF
//     %6:_(<16 x s32>) = G_AIE_VSHIFT_RIGHT %4, %5, %3(s32)
//     %2:_(<4 x s32>) = G_AIE_UNPAD_VECTOR %6(<16 x s32>)
bool AIELegalizerHelper::legalizeG_UNMERGE_VALUES_128bit(
    LegalizerHelper &Helper, MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();

  assert(MI.getNumOperands() == 3 &&
         "Expected G_UNMERGE_VALUES of 256-bit vector to 2 x 128-bit vector");
  const AIEBaseInstrInfo *II = ST.getInstrInfo();
  const unsigned UnpadOpc = II->getGenericUnpadVectorOpcode();
  const Register SrcReg = MI.getOperand(MI.getNumOperands() - 1).getReg();
  const Register DstRegLow = MI.getOperand(0).getReg();
  const Register DstRegHigh = MI.getOperand(1).getReg();
  const LLT SrcTy = MRI.getType(SrcReg);

  // Extracting lower 128-bit is easy: just discard (unpad) the high bits
  MIRBuilder.buildInstr(UnpadOpc, {DstRegLow}, {SrcReg});

  // To extract the higher 128-bit, we need to shift them to the lower position,
  // then unpad again.
  // We need to shift the upper 128-bit content by 16-byte (128-bit)
  auto ShiftAmt = MIRBuilder.buildConstant(LLT::scalar(32), 16);

  // VSHIFT operates on 512-bit inputs. We need to pad the 256-bit source
  // operand to 512-bit
  const Register ImplicitDef256 = MIRBuilder.buildUndef(SrcTy).getReg(0);

  const LLT Vec512 = SrcTy.multiplyElements(2);

  // Create the first 512-bit vector input
  auto ConcatValue =
      MIRBuilder.buildConcatVectors({Vec512}, {SrcReg, ImplicitDef256});

  // The second input will be ignored. Just create a dummy input
  auto ImplicitDef512 = MIRBuilder.buildUndef(Vec512);

  // Now create the VSHIFT
  const unsigned VShiftOpc = II->getGenericVShiftOpcode();
  auto VShift = MIRBuilder.buildInstr(VShiftOpc, {Vec512},
                                      {ConcatValue, ImplicitDef512, ShiftAmt});

  // Finally, unpad the 512-bit result to 128-bit
  MIRBuilder.buildInstr(UnpadOpc, {DstRegHigh}, {VShift});

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

  if (ST.isAIE2P() && FirstTy.isVector() && FirstTy.getSizeInBits() == 128 &&
      LastTy.getSizeInBits() == 256)
    return legalizeG_UNMERGE_VALUES_128bit(Helper, MI);

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

static bool
legalizeG_EXTRACT_VECTOR_ELT_TO_UNMERGE_VALUES(LegalizerHelper &Helper,
                                               MachineInstr &MI) {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();
  const LLT DstTy = MRI.getType(MI.getOperand(0).getReg());
  auto Unmerge = MIRBuilder.buildUnmerge(DstTy, MI.getOperand(1));
  SmallVector<Register, 4> VectorParts;
  const LLT SrcTy = MRI.getType(MI.getOperand(1).getReg());
  const unsigned NumParts = SrcTy.getNumElements();
  const LLT S32 = LLT::scalar(32);

  Register PrevRes = MIRBuilder.buildUndef(SrcTy.getElementType()).getReg(0);
  for (int I = NumParts - 1; I >= 0; I--) {
    auto Cst = MIRBuilder.buildConstant(S32, I);
    auto Cmp = MIRBuilder.buildICmp(CmpInst::ICMP_EQ, LLT::scalar(1), Cst,
                                    MI.getOperand(2));
    Register NewDst;
    if (I == 0)
      NewDst = MI.getOperand(0).getReg();
    else
      NewDst = MRI.createGenericVirtualRegister(DstTy);
    auto Sel = MIRBuilder.buildSelect(NewDst, Cmp, Unmerge.getReg(I), PrevRes);
    PrevRes = Sel.getReg(0);
  }

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

  if (SrcVecTy.getElementType().getSizeInBits() >= 256) {
    return legalizeG_EXTRACT_VECTOR_ELT_TO_UNMERGE_VALUES(Helper, MI);
  }

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
  case 1024:
  case 2048: {
    const LLT S8 = LLT::scalar(8);
    const LLT S16 = LLT::scalar(16);
    const LLT S64 = LLT::scalar(64);
    if (!ST.isAIE2P()) {
      assert(SrcVecSize != 2048 && "Not expected 2048 vector type!");
      assert(SrcVecEltTy != S64 && "Not expected 64-bit elmt type vector!");
    }
    const AIEBaseInstrInfo *II = ST.getInstrInfo();
    assert((SrcVecEltTy == S8 || SrcVecEltTy == S16 || SrcVecEltTy == S64 ||
            SrcVecEltTy == S32) &&
           "Unexpected vector element type for extract vector elt!");

    MachineInstr *ExtractInstr = nullptr;
    if (SrcVecEltTy == S8 || SrcVecEltTy == S16) {
      const Register ExtEltDstReg = MRI.createGenericVirtualRegister(S32);
      const Register ExtDstReg = MRI.createGenericVirtualRegister(S32);
      ExtractInstr = MIRBuilder.buildInstr(
          II->getGenericExtractVectorEltOpcode(/*SignExt*/ true),
          {ExtEltDstReg}, {SrcVecReg, IdxReg});
      MIRBuilder.buildAssertInstr(TargetOpcode::G_ASSERT_SEXT, ExtDstReg,
                                  ExtEltDstReg, SrcVecEltTy.getSizeInBits());
      MIRBuilder.buildTrunc(DstReg, ExtDstReg);
    } else {
      ExtractInstr = MIRBuilder.buildInstr(
          II->getGenericExtractVectorEltOpcode(/*SignExt*/ true), {DstReg},
          {SrcVecReg, IdxReg});
    }
    MI.eraseFromParent();

    const unsigned LegalVectorSize = II->getBasicVectorBitSize();
    // If this instruction is already legal, we are done
    if (SrcVecSize == LegalVectorSize)
      return true;

    // For any illegal vector type, re-use the existing legalization strategy

    // Set a valid insertion point after erasing the original instruction.
    MIRBuilder.setInstr(*ExtractInstr);
    return legalizeG_AIE_EXTRACT_VECTOR_ELT(Helper, *ExtractInstr,
                                            LegalVectorSize);
  }
  default:
    llvm_unreachable("Unexpected vector size for extract vector elt!");
  }
  MI.eraseFromParent();
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

static unsigned getAIE2FCmpIntrID(CmpInst::Predicate Predicate,
                                  bool &SwapOperands, bool &PromoteToFP32) {
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

static unsigned getAIE2PFCmpIntrID(CmpInst::Predicate Predicate,
                                   bool &SwapOperands, bool &PromoteToFP32) {
  switch (Predicate) {
  default:
    PromoteToFP32 = true;
    return 0;
  case CmpInst::FCMP_OGE:
  case CmpInst::FCMP_OEQ:
    return Intrinsic::aie2p_vgebf16;
  case CmpInst::FCMP_OLT:
  case CmpInst::FCMP_ONE:
    return Intrinsic::aie2p_vltbf16;
  case CmpInst::FCMP_OGT:
    SwapOperands = true;
    return Intrinsic::aie2p_vltbf16;
  case CmpInst::FCMP_OLE:
    SwapOperands = true;
    return Intrinsic::aie2p_vgebf16;
  }
}

/// @brief Get the AIE intrinsic corresponding to the fcmp predicate.
static unsigned getFCmpIntrID(const AIEBaseSubtarget &ST,
                              CmpInst::Predicate Predicate, bool &SwapOperands,
                              bool &PromoteToFP32) {
  if (ST.isAIE2())
    return getAIE2FCmpIntrID(Predicate, SwapOperands, PromoteToFP32);
  if (ST.isAIE2P())
    return getAIE2PFCmpIntrID(Predicate, SwapOperands, PromoteToFP32);

  llvm_unreachable("Called with unknown target triple!");
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
  const unsigned FCmpIntrID =
      getFCmpIntrID(ST, FPred, SwapOperands, PromoteToFP32);
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

static unsigned getFpTrunc32ToBF16IntrID(const AIEBaseSubtarget &ST) {
  if (ST.isAIE2())
    return Intrinsic::aie2_v16accfloat_to_v16bf16;
  if (ST.isAIE2P())
    return Intrinsic::aie2p_v16accfloat_to_v16bf16;

  llvm_unreachable("Called with unknown target triple!");
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

  Register Acc512Reg;
  if (ST.isAIE2()) {
    // Accumulator registers in AIE2 must have an element type of s64
    Acc512Reg = MRI.createGenericVirtualRegister(ACC512);
    MIRBuilder.buildBitcast(Acc512Reg, Vec512Reg);
  } else {
    // For all other architectures the virtual vector register can directly be
    // used as the accumulator register
    Acc512Reg = Vec512Reg;
  }

  Register Vec256Reg = MRI.createGenericVirtualRegister(V16S16);
  MIRBuilder
      .buildIntrinsic(getFpTrunc32ToBF16IntrID(ST), Vec256Reg, true, false)
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

bool AIELegalizerHelper::legalizeG_FADD_G_FSUB(LegalizerHelper &Helper,
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

  Register FPRes;
  if (ST.isAIE2()) {
    FPRes = MIRBuilder.buildBitcast(ACC512, FPOp).getReg(0);
  } else {
    FPRes = FPOp;
  }

  Register Conv =
      MIRBuilder
          .buildIntrinsic(getFpTrunc32ToBF16IntrID(ST), {V16BF16}, true, false)
          .addUse(FPRes)
          .getReg(0);

  auto Pad512 =
      emitPadUndefVector(MRI, MIRBuilder, V16BF16.multiplyElements(2), Conv);

  const Register ExtEltDstReg = MRI.createGenericVirtualRegister(S32);
  const Register ExtDstReg = MRI.createGenericVirtualRegister(S32);
  const unsigned ExtractEltOpc =
      ST.getInstrInfo()->getGenericExtractVectorEltOpcode(/*SignExt*/ true);
  MIRBuilder.buildInstr(ExtractEltOpc, {ExtEltDstReg}, {Pad512, IdxReg});
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

// Legalize < MaxBitSize-bit G_SELECT
// Expand the source vectors to MaxBitSize-bits by padding it with undefs.
bool AIELegalizerHelper::legalizeG_SELECT(LegalizerHelper &Helper,
                                          MachineInstr &MI,
                                          const unsigned MaxBitSize) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();

  const Register DstReg = MI.getOperand(0).getReg();
  const LLT DstTy = MRI.getType(DstReg);
  const unsigned DstVecSize = DstTy.getSizeInBits();

  assert(DstTy.isVector() && DstVecSize < MaxBitSize &&
         "Expected to legalize < MaxBitSize-bit vector G_SELECT");
  assert(!(MaxBitSize % DstVecSize) &&
         "Vector size should be a factor of MaxBitSize");

  const Register SrcReg0 = MI.getOperand(1).getReg(); // Scalar
  const Register SrcReg1 = MI.getOperand(2).getReg();
  const Register SrcReg2 = MI.getOperand(3).getReg();

  const LLT NewVecTy =
      LLT::fixed_vector(MaxBitSize / DstTy.getElementType().getSizeInBits(),
                        DstTy.getElementType());

  const LLT WideTy = DstTy.multiplyElements(MaxBitSize / DstVecSize);
  const Register NewSrcReg1 =
      emitPadUndefVector(MRI, MIRBuilder, WideTy, SrcReg1);
  const Register NewSrcReg2 =
      emitPadUndefVector(MRI, MIRBuilder, WideTy, SrcReg2);

  const Register NewDstReg = MRI.createGenericVirtualRegister(NewVecTy);
  MIRBuilder.buildInstr(MI.getOpcode(), {NewDstReg},
                        {SrcReg0, NewSrcReg1, NewSrcReg2}, MI.getFlags());

  const unsigned NumPadElts = (MaxBitSize / DstVecSize) - 1;
  SmallVector<Register, 4> Regs;
  Regs.push_back(DstReg);
  for (unsigned I = 0; I < NumPadElts; ++I)
    Regs.push_back(MRI.createGenericVirtualRegister(DstTy));
  MIRBuilder.buildUnmerge(Regs, NewDstReg);

  MI.eraseFromParent();
  return true;
}

/// Legalize the incoming \p MI G_CONCAT_VECTORS to half the number of inputs,
/// but at least 2 inputs.
bool AIELegalizerHelper::legalizeG_CONCAT_VECTORS(LegalizerHelper &Helper,
                                                  MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;

  const auto [DstReg, DstTy, SrcReg, SrcTy] = MI.getFirst2RegLLTs();
  assert(DstTy.isVector() && SrcTy.isVector() && "Expected vector types");
  assert(SrcTy.getSizeInBits() >= 128 &&
         "Vectors < 128-bit should be lowered to insert vector elt");

  // Prevent infinite looping in the Legalizer. The base case should be legal
  // and we should not reach this.
  assert(DstTy.getSizeInBits() > 2 * SrcTy.getSizeInBits());

  const LLT StepTy = SrcTy.multiplyElements(2);

  // Concatenate pairs of source vector operands.
  SmallVector<Register, 4> ConcatSteps;
  for (size_t I = 1; I < MI.getNumOperands(); I += 2) {
    const Register ConcatStep =
        MIRBuilder
            .buildConcatVectors({StepTy}, {MI.getOperand(I).getReg(),
                                           MI.getOperand(I + 1).getReg()})
            .getReg(0);
    ConcatSteps.push_back(ConcatStep);
  }

  // Concatenate the resulting artifacts.
  MIRBuilder.buildConcatVectors(DstReg, ConcatSteps);
  MI.eraseFromParent();
  return true;
}

bool AIELegalizerHelper::legalizeG_BITCAST(LegalizerHelper &Helper,
                                           MachineInstr &MI) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();

  const Register DstReg = MI.getOperand(0).getReg();
  const Register SrcReg = MI.getOperand(1).getReg();

  const LLT DstTy = MRI.getType(DstReg);
  const LLT SrcTy = MRI.getType(SrcReg);
  assert(DstTy.getSizeInBits() == 16 && SrcTy.getSizeInBits() == 16 &&
         "Expected to legalize 16-bit G_BITCAST");
  const LLT VEC32 = LLT::fixed_vector(2, 16);
  if (DstTy.isVector()) {
    const Register TmpReg32A =
        MRI.createGenericVirtualRegister(LLT::scalar(32));
    MIRBuilder.buildAnyExt({TmpReg32A}, {SrcReg});
    const Register TmpReg32B =
        MRI.createGenericVirtualRegister(LLT::scalar(32));
    MIRBuilder.buildShl(TmpReg32B, TmpReg32A,
                        MIRBuilder.buildConstant(LLT::scalar(32), 8));
    const Register TmpReg32C =
        MRI.createGenericVirtualRegister(LLT::scalar(32));
    MIRBuilder.buildAnd(TmpReg32C, TmpReg32B,
                        MIRBuilder.buildConstant(LLT::scalar(32), 0xFF0000));
    const Register TmpReg32D =
        MRI.createGenericVirtualRegister(LLT::scalar(32));
    MIRBuilder.buildAnd(TmpReg32D, TmpReg32A,
                        MIRBuilder.buildConstant(LLT::scalar(32), 0xFF));
    const Register TmpReg32E =
        MRI.createGenericVirtualRegister(LLT::scalar(32));
    MIRBuilder.buildOr(TmpReg32E, TmpReg32D, TmpReg32C);

    const Register TmpReg2x16 = MRI.createGenericVirtualRegister(VEC32);
    MIRBuilder.buildBitcast({TmpReg2x16}, {TmpReg32E});
    MIRBuilder.buildTrunc(DstReg, TmpReg2x16);

  } else {
    const Register TmpReg2x16 = MRI.createGenericVirtualRegister(VEC32);
    MIRBuilder.buildAnyExt({TmpReg2x16}, {SrcReg});
    const Register TmpReg32 = MRI.createGenericVirtualRegister(LLT::scalar(32));
    MIRBuilder.buildBitcast({TmpReg32}, {TmpReg2x16});
    MIRBuilder.buildTrunc(DstReg, TmpReg32);
  }

  MI.eraseFromParent();
  return true;
}

bool AIELegalizerHelper::legalizeBinOp(LegalizerHelper &Helper,
                                       MachineInstr &MI) const {
  assert(MI.getOpcode() == TargetOpcode::G_ADD ||
         MI.getOpcode() == TargetOpcode::G_SUB ||
         MI.getOpcode() == TargetOpcode::G_XOR);

  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();

  const Register DstReg = MI.getOperand(0).getReg();
  const LLT DstTy = MRI.getType(DstReg);
  const auto VectorSize = DstTy.getSizeInBits();
  assert(DstTy.isVector() && VectorSize < 512 &&
         "Expected vector size less than 512-bits");
  assert(!(512 % VectorSize) && "Vector size should be a multiple of 512");

  const Register Src1Reg = MI.getOperand(1).getReg();
  const Register Src2Reg = MI.getOperand(2).getReg();
  assert(DstTy == MRI.getType(Src1Reg));

  auto NewVecTy = LLT::fixed_vector(
      512 / DstTy.getElementType().getSizeInBits(), DstTy.getElementType());

  Register NewDstReg = MRI.createGenericVirtualRegister(NewVecTy);
  Register NewSrc1Reg = emitPadUndefVector(MRI, MIRBuilder, NewVecTy, Src1Reg);
  Register NewSrc2Reg = emitPadUndefVector(MRI, MIRBuilder, NewVecTy, Src2Reg);

  unsigned NumberOfPadElts = (512 / VectorSize) - 1;
  MIRBuilder.buildInstr(MI.getOpcode(), {NewDstReg}, {NewSrc1Reg, NewSrc2Reg},
                        MI.getFlags());

  SmallVector<Register, 8> Regs;
  Regs.push_back(DstReg);
  for (unsigned i = 0; i < NumberOfPadElts; ++i)
    Regs.push_back(MRI.createGenericVirtualRegister(DstTy));
  MIRBuilder.buildUnmerge(Regs, NewDstReg);

  MI.eraseFromParent();
  return true;
}

/// Clamps the source vector size of the incoming \p MI to the legal base vector
/// size \p LegalVectorSize.
static bool narrowAIEVectorExtractEltSrcVecToLegalSize(
    MachineRegisterInfo &MRI, MachineIRBuilder &MIRBuilder,
    LegalizerHelper &Helper, MachineInstr &MI, const unsigned LegalVectorSize) {
  auto [DstReg, SrcVec] = MI.getFirst2Regs();
  Register Idx = MI.getOperand(MI.getNumOperands() - 1).getReg();

  const LLT VecTy = MRI.getType(SrcVec);
  assert((VecTy.getSizeInBits() % LegalVectorSize == 0) &&
         "Expected source vector to be multiple of LegalVectorSize");
  const LLT NarrowVecTy = VecTy.divide(VecTy.getSizeInBits() / LegalVectorSize);

  // If the index is a constant, we can really break this down as you would
  // expect, and index into the target size pieces.
  auto MaybeCst = getIConstantVRegValWithLookThrough(Idx, MRI);
  if (MaybeCst) {
    const int64_t IdxVal = MaybeCst->Value.getSExtValue();
    // Avoid out of bounds indexing the pieces.
    if (IdxVal >= VecTy.getNumElements() || IdxVal < 0) {
      MIRBuilder.buildUndef(DstReg);
      return true;
    }

    SmallVector<Register, 4> VecParts;
    const auto Unmerge = MIRBuilder.buildUnmerge(NarrowVecTy, SrcVec);
    getUnmergeResults(VecParts, *Unmerge);

    const unsigned NewNumElts = NarrowVecTy.getNumElements();
    const int64_t PartIdx = IdxVal / NewNumElts;
    const LLT IdxTy = MRI.getType(Idx);
    const auto NewIdx = MIRBuilder.buildConstant(IdxTy, IdxVal % NewNumElts);
    MIRBuilder.buildInstr(MI.getOpcode(), {DstReg},
                          {VecParts[PartIdx], NewIdx});
    return true;
  }

  // If the index is not a constant, we need to find the sub-vector to extract
  // from at runtime.
  SmallVector<Register, 4> VecParts;
  const auto Unmerge = MIRBuilder.buildUnmerge(NarrowVecTy, SrcVec);
  getUnmergeResults(VecParts, *Unmerge);

  assert(VecParts.size() >= 2 &&
         "Expected split into at least two sub-vectors");

  const unsigned NewNumElts = NarrowVecTy.getNumElements();
  const LLT IdxTy = MRI.getType(Idx);
  Register NewSrcVec = VecParts[VecParts.size() - 1];
  Register AdjImm =
      MIRBuilder.buildConstant(IdxTy, (VecParts.size() - 1) * NewNumElts)
          .getReg(0);

  for (size_t I = VecParts.size() - 1; I > 0; I--) {
    const auto HighIdx = MIRBuilder.buildConstant(IdxTy, I * NewNumElts);
    // Check if the Idx falls into the low or high part of the split vector.
    const auto Cmp =
        MIRBuilder.buildICmp(CmpInst::ICMP_SLT, LLT::scalar(1), Idx, HighIdx);
    // Select between the low and high part of the split vector.
    NewSrcVec =
        MIRBuilder.buildSelect(NarrowVecTy, Cmp, VecParts[I - 1], NewSrcVec)
            .getReg(0);

    // We need to adjust the Idx to select from either low or high sub-vector
    const auto AdjLow = MIRBuilder.buildConstant(IdxTy, (I - 1) * NewNumElts);
    AdjImm = MIRBuilder.buildSelect(IdxTy, Cmp, AdjLow, AdjImm).getReg(0);
  }

  const auto NewIdx = MIRBuilder.buildSub(IdxTy, Idx, AdjImm);
  MIRBuilder.buildInstr(MI.getOpcode(), {DstReg}, {NewSrcVec, NewIdx});
  return true;
}

bool AIELegalizerHelper::legalizeG_AIE_EXTRACT_VECTOR_ELT(
    LegalizerHelper &Helper, MachineInstr &MI,
    const unsigned LegalVectorSize) const {
  MachineIRBuilder &MIRBuilder = Helper.MIRBuilder;
  MachineRegisterInfo &MRI = *MIRBuilder.getMRI();

  const Register DstReg = MI.getOperand(0).getReg();
  const Register SrcVecReg = MI.getOperand(1).getReg();
  const LLT SrcVecTy = MRI.getType(SrcVecReg);
  const Register IdxReg = MI.getOperand(2).getReg();
  const unsigned SrcVecSize = SrcVecTy.getSizeInBits();

  // Already legal
  if (SrcVecSize == LegalVectorSize)
    return true;

  if (SrcVecSize < LegalVectorSize) {
    assert((LegalVectorSize % SrcVecSize == 0) &&
           "Expected LegalVectorSize to be a multiple of source vector size");
    const unsigned MultiplyFactor = LegalVectorSize / SrcVecSize;
    const LLT LegalVecTy = SrcVecTy.multiplyElements(MultiplyFactor);
    const Register NewSrc =
        emitPadUndefVector(MRI, MIRBuilder, LegalVecTy, SrcVecReg);

    MIRBuilder.buildInstr(MI.getOpcode(), {DstReg}, {NewSrc, IdxReg});
  } else if (SrcVecSize > LegalVectorSize && isPowerOf2_32(SrcVecSize)) {
    if (!narrowAIEVectorExtractEltSrcVecToLegalSize(MRI, MIRBuilder, Helper, MI,
                                                    LegalVectorSize))
      return false;
  } else {
    llvm_unreachable(
        "Illegal vector size for G_AIE_[ZS]EXT_EXTRACT_VECTOR_ELT");
  }

  MI.eraseFromParent();
  return true;
}

} // namespace llvm
