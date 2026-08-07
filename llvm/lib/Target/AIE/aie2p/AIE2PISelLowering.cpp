//===-- AIE2PISelLowering.cpp - AIE2p IR Lowering Interface ----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the AIE2p-specific interfaces used to lower IR to MIR.
//
//===----------------------------------------------------------------------===//

#include "AIE2PISelLowering.h"
#include "AIEBaseSubtarget.h"
#include "MCTargetDesc/aie2p/AIE2PMCTargetDesc.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/IR/IntrinsicsAIE2P.h"
#include "llvm/IR/RuntimeLibcalls.h"

using namespace llvm;

#define DEBUG_TYPE "aie-lower"

extern cl::opt<bool> VecCCLibcalls;

AIE2PTargetLowering::AIE2PTargetLowering(const TargetMachine &TM,
                                         const AIEBaseSubtarget &STI)
    : AIEBaseTargetLowering(TM, STI) {

  if (VecCCLibcalls) {
    setLibcallImplCallingConv(getLibcallImpl(RTLIB::SINTTOFP_I32_F32),
                          CallingConv::AIE_PreserveAll_Vec);
    setLibcallImplCallingConv(getLibcallImpl(RTLIB::SINTTOFP_I32_F64),
                          CallingConv::AIE_PreserveAll_Vec);
    setLibcallImplCallingConv(getLibcallImpl(RTLIB::SINTTOFP_I64_F32),
                          CallingConv::AIE_PreserveAll_Vec);
    setLibcallImplCallingConv(getLibcallImpl(RTLIB::SINTTOFP_I64_F64),
                          CallingConv::AIE_PreserveAll_Vec);
    setLibcallImplCallingConv(getLibcallImpl(RTLIB::SDIV_I32), CallingConv::AIE_PreserveAll_Vec);
    setLibcallImplCallingConv(getLibcallImpl(RTLIB::SDIV_I64), CallingConv::AIE_PreserveAll_Vec);
    setLibcallImplCallingConv(getLibcallImpl(RTLIB::UDIV_I32), CallingConv::AIE_PreserveAll_Vec);
    setLibcallImplCallingConv(getLibcallImpl(RTLIB::UDIV_I64), CallingConv::AIE_PreserveAll_Vec);
    setLibcallImplCallingConv(getLibcallImpl(RTLIB::SREM_I32), CallingConv::AIE_PreserveAll_Vec);
    setLibcallImplCallingConv(getLibcallImpl(RTLIB::SREM_I64), CallingConv::AIE_PreserveAll_Vec);
    setLibcallImplCallingConv(getLibcallImpl(RTLIB::UREM_I32), CallingConv::AIE_PreserveAll_Vec);
    setLibcallImplCallingConv(getLibcallImpl(RTLIB::UREM_I64), CallingConv::AIE_PreserveAll_Vec);
  }

  const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();

  // We already define in .td which types are legal for each register class.
  // Let's re-use the information.
  for (unsigned i = 0; i != MVT::VALUETYPE_SIZE; ++i) {
    MVT Ty = MVT::SimpleValueType(i);
    // As a base rule, a type will be legal if there is a register class which
    // can natively hold it. Note that the class selected below does not matter
    // for a GlobalISel flow, since the selection is RegisterBank-based.
    const auto *RCIt =
        find_if(TRI->regclasses(), [Ty, TRI](const TargetRegisterClass *RC) {
          return TRI->isTypeLegalForClass(*RC, Ty);
        });
    // Add 128-bit RegClass as unavailable regclass for the 128-bit value type
    // as this RegClass is not supported natively
    if (RCIt != TRI->regclass_end() && !Ty.is128BitVector()) {
      addRegisterClass(Ty, *RCIt);
    }
  }
  computeRegisterProperties(STI.getRegisterInfo());
  setStackPointerRegisterToSaveRestore(AIE2P::sp);
}

namespace {

// Returns true if type name matches with a bfp16 type name
bool isTyNameBfp16(StringRef TyName) {
  if (TyName.ends_with("struct.v64bfp16ebs16"))
    return true;
  if (TyName.ends_with("struct.v64bfp16ebs8"))
    return true;
  if (TyName.ends_with("struct.v128bfp16ebs16"))
    return true;
  if (TyName.ends_with("struct.v128bfp16ebs8"))
    return true;

  return false;
}
} // namespace
bool AIE2PTargetLowering::functionArgumentNeedsConsecutiveRegisters(
    Type *Ty, CallingConv::ID CallConv, bool isVarArg,
    const DataLayout &DL) const {
  StructType *STy = dyn_cast<StructType>(Ty);
  if (!STy || STy->isLiteral())
    return false;
  if (isTyNameBfp16(STy->getName())) {
    return true;
  }
  return false;
}

bool AIE2PTargetLowering::getTgtMemIntrinsic(IntrinsicInfo &Info,
                                             const CallInst &I,
                                             MachineFunction &MF,
                                             unsigned Intrinsic) const {
  switch (Intrinsic) {
  case Intrinsic::aie2p_fifo_ld_fill:
  case Intrinsic::aie2p_fifo_ld_fillx:
  case Intrinsic::aie2p_fifo_ld_popx:
  case Intrinsic::aie2p_fifo_ld_pop_unaligned:
  case Intrinsic::aie2p_fifo_ld_pop_1d_unaligned:
  case Intrinsic::aie2p_fifo_ld_pop_2d_unaligned:
  case Intrinsic::aie2p_fifo_ld_pop_3d_unaligned:
  case Intrinsic::aie2p_fifo_ld_pop_544_1d_bfp16:
  case Intrinsic::aie2p_fifo_ld_pop_544_2d_bfp16:
  case Intrinsic::aie2p_fifo_ld_pop_544_3d_bfp16:
  case Intrinsic::aie2p_fifo_ld_pop_544_bfp16:
  case Intrinsic::aie2p_fifo_ld_pop_576_1d_bfp16:
  case Intrinsic::aie2p_fifo_ld_pop_576_2d_bfp16:
  case Intrinsic::aie2p_fifo_ld_pop_576_3d_bfp16:
  case Intrinsic::aie2p_fifo_ld_pop_576_bfp16:
    // The HW does a 512-bit load from somewhere between addr-63 and addr+128
    // depending on the FIFO availability and the input alignment.
    // A conservative access range would be [addr-64, addr+192)
    // To be safe, we prefer "unknown-size".
    Info.memVT = MVT::Other;
    Info.ptrVal = I.getArgOperand(0);
    Info.align = Align(1); // Can we somehow recover the original alignment?
    Info.flags = MachineMemOperand::MOLoad;
    return true;

  case Intrinsic::aie2p_fifo_st_push_512_bfp16:
  case Intrinsic::aie2p_fifo_st_push_576_bfp16:
  case Intrinsic::aie2p_fifo_st_push_544_bfp16:
  case Intrinsic::aie2p_fifo_st_flush:
  case Intrinsic::aie2p_fifo_st_flush_1d:
  case Intrinsic::aie2p_fifo_st_flush_2d:
  case Intrinsic::aie2p_fifo_st_flush_3d:
  case Intrinsic::aie2p_fifo_st_flush_conv:
  case Intrinsic::aie2p_fifo_st_flush_1d_conv:
  case Intrinsic::aie2p_fifo_st_flush_2d_conv:
  case Intrinsic::aie2p_fifo_st_flush_3d_conv:
    // The HW does a 512-bit store at somewhere between addr and addr-128
    // depending on the FIFO availability.
    // A conservative access range would be [addr-128, addr+64)
    // To be safe, we prefer "unknown-size".
    Info.memVT = MVT::Other;
    Info.ptrVal = I.getArgOperand(0);
    Info.align = Align(4); // Can we somehow recover the original alignment?
    Info.flags = MachineMemOperand::MOStore;
    return true;

  // Pointer-coupled locks are not memory accesses; the ptr arg only tags which
  // buffer the lock orders against (see mayLockOrderWithMemOp). MONone would
  // match that semantics, but IRTranslator builds the MMO via
  // MachineMemOperand(..., Flags, LLT), which asserts isLoad() || isStore().
  // MOLoad here is GISel plumbing only — ACQ/REL still have no mayLoad, and
  // hasAnnotativeMemOperands() tells MachineVerifier the MMO is annotative.
  case Intrinsic::aie2p_acquire_ptr:
  case Intrinsic::aie2p_acquire_cond_ptr:
  case Intrinsic::aie2p_release_ptr:
  case Intrinsic::aie2p_release_cond_ptr:
    Info.ptrVal = I.getArgOperand(0);
    Info.memVT = MVT::Other;
    Info.flags = MachineMemOperand::MOLoad;
    return true;
  }
  return false;
}

MVT AIE2PTargetLowering::getRegisterTypeForCallingConvAssignment(
    LLVMContext &Context, CallingConv::ID CC, EVT VT) const {
  // 128-bit vectors are passed in 256-bit W registers
  if (VT.isSimple() && VT.is128BitVector())
    return getRegisterType(VT.getSimpleVT());

  return AIEBaseTargetLowering::getRegisterTypeForCallingConvAssignment(Context,
                                                                        CC, VT);
}

MVT AIE2PTargetLowering::getRegisterTypeForCallingConv(LLVMContext &Context,
                                                       CallingConv::ID CC,
                                                       EVT VT) const {
  // 128-bits registers aren't considered legal because AIE2P only has vector
  // ops for 256+ bits vectors. However, for ABI compatibility reasons, we still
  // want to keep those vectors as is, because they are passed differently
  // compared to 256-bits vectors.
  if (VT.isSimple() && VT.is128BitVector())
    return VT.getSimpleVT();

  return AIEBaseTargetLowering::getRegisterTypeForCallingConv(Context, CC, VT);
}

TargetLoweringBase::LegalizeTypeAction
AIE2PTargetLowering::getPreferredVectorAction(MVT VT) const {
  if (VT.is128BitVector())
    return TypeWidenVector;

  return TargetLoweringBase::getPreferredVectorAction(VT);
}
