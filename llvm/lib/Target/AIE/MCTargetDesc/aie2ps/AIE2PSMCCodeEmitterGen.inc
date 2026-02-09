//===-AIE2PSMCCodeEmitterDefinition.h -----------------------------*- tablegen
//-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//* Automatically generated file, do not edit! *

void AIE2PSMCCodeEmitter::getmFEXvOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFEXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mFEXv");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmCMmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eCMLRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eCMHRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mCMm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmCRxOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (Reg == AIE2PS::crSat) {
    Result = 0b011110;
  } else if (Reg == AIE2PS::crRnd) {
    Result = 0b011000;
  } else if (Reg == AIE2PS::crFPMask) {
    Result = 0b010000;
  } else if (Reg == AIE2PS::crF2IMask) {
    Result = 0b000110;
  } else if (Reg == AIE2PS::crF2FMask) {
    Result = 0b000100;
  } else if (Reg == AIE2PS::crF2BMask) {
    Result = 0b000010;
  } else if (Reg == AIE2PS::crSRSMode) {
    Result = 0b011100;
  } else if (Reg == AIE2PS::crUPSMode) {
    Result = 0b100000;
  } else if (Reg == AIE2PS::crUnpackSize) {
    Result = 0b100010;
  } else if (Reg == AIE2PS::crPackSize) {
    Result = 0b010110;
  } else if (Reg == AIE2PS::crBF8conf) {
    Result = 0b000000;
  } else if (Reg == AIE2PS::crFP8conf) {
    Result = 0b001000;
  } else if (Reg == AIE2PS::crFPConvSat) {
    Result = 0b001110;
  } else if (Reg == AIE2PS::crSCDEn) {
    Result = 0b011010;
  } else if (Reg == AIE2PS::crMCDEn) {
    Result = 0b010100;
  } else if (Reg == AIE2PS::crFPNlfMask) {
    Result = 0b010010;
  } else if (Reg == AIE2PS::crFPCnvFx2FlMask) {
    Result = 0b001100;
  } else if (Reg == AIE2PS::crFPCnvFl2FxMask) {
    Result = 0b001010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eSRSSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 5) | 0b00101;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eUPSSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 5) | 0b01001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::ePackSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 5) | 0b00001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eUnpackSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 5) | 0b01101;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eVaddSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 5) | 0b10001;
  } else {
    llvm_unreachable("Register not in mCRx");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmAguSrcOpValue(const MCInst &MI, unsigned OpNo,
                                            APInt &Op,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1101;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0101;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eReRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b100;
  } else {
    llvm_unreachable("Register not in mAguSrc");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEGmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEGle_sRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEGlo_sRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEGhe_sRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEGho_sRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mEGm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmBMaOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eBMLLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMLHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMHLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMHHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mBMa");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEG2aOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEG2e_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEG2o_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEG2a");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEhmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEheRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEhoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEhm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmXwOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mXw");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmRv8OpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReV8RegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoV8RegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mRv8");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmShflBMDstOpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eBMSLLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMSLHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMSHLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMSHHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mShflBMDst");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFEXaOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFEXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mFEXa");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFEG2sOpValue(const MCInst &MI, unsigned OpNo,
                                           APInt &Op,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFEG2eRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEG2oRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mFEG2s");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFaOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mFa");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmGhmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eGheRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eGhoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mGhm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmMvSclDstOpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b101;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0110;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eSRegClassID].contains(Reg)) {
    Result = (Encoding << 5) | 0b01110;
  } else if (Reg == AIE2PS::lr) {
    Result = 0b0111110;
  } else if (Reg == AIE2PS::sp) {
    Result = 0b1011110;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eSRSSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b001010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eUPSSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b010010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::ePackSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b000010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eUnpackSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b011010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eVaddSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b100010;
  } else if (Reg == AIE2PS::lc) {
    Result = 0b0011110;
  } else {
    llvm_unreachable("Register not in mMvSclDst");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmRV16sOpValue(const MCInst &MI, unsigned OpNo,
                                           APInt &Op,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReV16RegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoV16RegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mRV16s");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmRsOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mRs");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmWbOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eWleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mWb");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmMvBMXSrcOpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eBMLLRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMLHRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMHLRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMHHRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b110;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b101;
  } else if (Reg == AIE2PS::sfh) {
    Result = 0b110011;
  } else if (Reg == AIE2PS::sfl) {
    Result = 0b011011;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eLdFifoHRegRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 5) | 0b00011;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eLdFifoLRegRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 5) | 0b01011;
  } else if (Reg == AIE2PS::lfe) {
    Result = 0b010011;
  } else {
    llvm_unreachable("Register not in mMvBMXSrc");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEGaOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEGle_sRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEGlo_sRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEGhe_sRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEGho_sRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mEGa");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFFaOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFFeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFFoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mFFa");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmXsOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mXs");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEXsOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEXe_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEXo_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEXs");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmCMaOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eCMLRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eCMHRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mCMa");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEWmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEWleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEWloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEWheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mEWm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFEG2aOpValue(const MCInst &MI, unsigned OpNo,
                                           APInt &Op,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFEG2eRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEG2oRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mFEG2a");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmElmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEleRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEloRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mElm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmRF2aOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  if (Reg == AIE2PS::r24) {
    Result = 0b0;
  } else if (Reg == AIE2PS::r25) {
    Result = 0b1;
  } else {
    llvm_unreachable("Register not in mRF2a");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFEWmOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFEWleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEWloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEWheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mFEWm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEWhvOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEWheRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEWhv");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFEXnOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFEXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mFEXn");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmMvSclSrcOpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b101;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0110;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eSRegClassID].contains(Reg)) {
    Result = (Encoding << 5) | 0b01110;
  } else if (Reg == AIE2PS::lr) {
    Result = 0b1111110;
  } else if (Reg == AIE2PS::sp) {
    Result = 0b0100011;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eSRSSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b001010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eUPSSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b010010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::ePackSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b000010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eUnpackSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b011010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eVaddSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b100010;
  } else if (Reg == AIE2PS::lc) {
    Result = 0b0111110;
  } else if (Reg == AIE2PS::le) {
    Result = 0b1011110;
  } else if (Reg == AIE2PS::ls) {
    Result = 0b0000011;
  } else if (Reg == AIE2PS::CORE_ID) {
    Result = 0b0011110;
  } else {
    llvm_unreachable("Register not in mMvSclSrc");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmSclStOpValue(const MCInst &MI, unsigned OpNo,
                                           APInt &Op,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b110;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0011;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEleRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEloRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEheRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0101;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEhoRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1101;
  } else if (Reg == AIE2PS::lr) {
    Result = 0b0000111;
  } else {
    llvm_unreachable("Register not in mSclSt");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFmOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mFm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmRyOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mRy");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEG2sOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEG2e_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEG2o_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEG2s");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEXaOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEXe_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEXo_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEXa");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmRzOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mRz");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFFsOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFFeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFFoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mFFs");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFEWaOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFEWleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEWloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEWheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mFEWa");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFEGaOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFEGleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEGloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEGheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEGhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mFEGa");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEXbOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEXe_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEXo_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEXb");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEEaOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEEeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEEoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEEa");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmWaOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eWleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mWa");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmXmOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mXm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmBM_ldOpValue(const MCInst &MI, unsigned OpNo,
                                           APInt &Op,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eBMLLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMLHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMHLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMHHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mBM_ld");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFFmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFFeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFFoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mFFm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmRaOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mRa");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmRxOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mRx");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFEXmOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFEXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mFEXm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFEXbOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFEXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mFEXb");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmLdaSclOpValue(const MCInst &MI, unsigned OpNo,
                                            APInt &Op,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b110;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0011;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEleRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEloRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEheRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0101;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEhoRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1101;
  } else if (Reg == AIE2PS::lr) {
    Result = 0b0000111;
  } else {
    llvm_unreachable("Register not in mLdaScl");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmWsOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eWleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mWs");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmAguDstOpValue(const MCInst &MI, unsigned OpNo,
                                            APInt &Op,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b011;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b000;
  } else {
    llvm_unreachable("Register not in mAguDst");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEXmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEXe_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEXo_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEXm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmGGmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eGGeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eGGoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mGGm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFEXsOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFEXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mFEXs");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmBMmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eBMLLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMLHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMHLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMHHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mBMm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmGlmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eGleRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eGloRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mGlm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmMvBMXDstOpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eBMLLRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMLHRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMHLRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMHHRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b110;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b101;
  } else if (Reg == AIE2PS::sfh) {
    Result = 0b110011;
  } else if (Reg == AIE2PS::sfl) {
    Result = 0b011011;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eLdFifoHRegRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 5) | 0b00011;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eLdFifoLRegRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 5) | 0b01011;
  } else if (Reg == AIE2PS::lfe) {
    Result = 0b010011;
  } else {
    llvm_unreachable("Register not in mMvBMXDst");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmShflXDstOpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mShflXDst");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEXnOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEXe_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEXo_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEXn");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmRy8OpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReS8RegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoS8RegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mRy8");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmXnOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mXn");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmMvSclDstCgOpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b101;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0110;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eSRegClassID].contains(Reg)) {
    Result = (Encoding << 5) | 0b01110;
  } else if (Reg == AIE2PS::lr) {
    Result = 0b0111110;
  } else if (Reg == AIE2PS::sp) {
    Result = 0b1011110;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eSRSSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b001010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eUPSSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b010010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::ePackSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b000010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eUnpackSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b011010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eVaddSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b100010;
  } else if (Reg == AIE2PS::lc) {
    Result = 0b0011110;
  } else {
    llvm_unreachable("Register not in mMvSclDstCg");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmXvOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mXv");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmSclMSOpValue(const MCInst &MI, unsigned OpNo,
                                           APInt &Op,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0110;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1110;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1101;
  } else {
    llvm_unreachable("Register not in mSclMS");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEWlv_bisOpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEWleRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEWloRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEWlv_bis");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmAluCgOpValue(const MCInst &MI, unsigned OpNo,
                                           APInt &Op,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (Reg == AIE2PS::lc) {
    Result = 0b000001;
  } else {
    llvm_unreachable("Register not in mAluCg");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEXwOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEXe_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEXo_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEXw");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmCMsOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eCMLRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eCMHRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mCMs");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmWs_128bOpValue(const MCInst &MI, unsigned OpNo,
                                             APInt &Op,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eWleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mWs_128b");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFEGsOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFEGleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEGloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEGheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEGhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mFEGs");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmRS16mOpValue(const MCInst &MI, unsigned OpNo,
                                           APInt &Op,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReS16RegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoS16RegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mRS16m");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmGGaOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eGGeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eGGoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mGGa");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmGaOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eGleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eGloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eGheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eGhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mGa");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmXaOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mXa");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmRF2bOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  if (Reg == AIE2PS::r24) {
    Result = 0b0;
  } else if (Reg == AIE2PS::r25) {
    Result = 0b1;
  } else {
    llvm_unreachable("Register not in mRF2b");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmWmOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eWleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mWm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFEWsOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFEWleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEWloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEWheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFEWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mFEWs");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmXs_splitOpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mXs_split");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmlStsOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eFleRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFloRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1001;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFheRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0101;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eFhoRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1101;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEEeRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEEoRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eGGeRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0110;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eGGoRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1110;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEGle_sRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEGlo_sRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEGhe_sRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEGho_sRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eLRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b011;
  } else {
    llvm_unreachable("Register not in mlSts");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmSCmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  if (Reg == AIE2PS::crSat) {
    Result = 0b011110;
  } else if (Reg == AIE2PS::crRnd) {
    Result = 0b011000;
  } else if (Reg == AIE2PS::crFPMask) {
    Result = 0b010000;
  } else if (Reg == AIE2PS::crF2IMask) {
    Result = 0b000110;
  } else if (Reg == AIE2PS::crF2FMask) {
    Result = 0b000100;
  } else if (Reg == AIE2PS::crF2BMask) {
    Result = 0b000010;
  } else if (Reg == AIE2PS::crSRSMode) {
    Result = 0b011100;
  } else if (Reg == AIE2PS::crUPSMode) {
    Result = 0b100000;
  } else if (Reg == AIE2PS::crUnpackSize) {
    Result = 0b100010;
  } else if (Reg == AIE2PS::crPackSize) {
    Result = 0b010110;
  } else if (Reg == AIE2PS::crBF8conf) {
    Result = 0b000000;
  } else if (Reg == AIE2PS::crFP8conf) {
    Result = 0b001000;
  } else if (Reg == AIE2PS::crFPConvSat) {
    Result = 0b001110;
  } else if (Reg == AIE2PS::crSCDEn) {
    Result = 0b011010;
  } else if (Reg == AIE2PS::crMCDEn) {
    Result = 0b010100;
  } else if (Reg == AIE2PS::crFPNlfMask) {
    Result = 0b010010;
  } else if (Reg == AIE2PS::crFPCnvFx2FlMask) {
    Result = 0b001100;
  } else if (Reg == AIE2PS::crFPCnvFl2FxMask) {
    Result = 0b001010;
  } else if (Reg == AIE2PS::srCarry) {
    Result = 0b000001;
  } else if (Reg == AIE2PS::srSS0) {
    Result = 0b110001;
  } else if (Reg == AIE2PS::srMS0) {
    Result = 0b101001;
  } else if (Reg == AIE2PS::srSRS_of) {
    Result = 0b101101;
  } else if (Reg == AIE2PS::srUPS_of) {
    Result = 0b111001;
  } else if (Reg == AIE2PS::srFifo_of) {
    Result = 0b100001;
  } else if (Reg == AIE2PS::srFifo_uf) {
    Result = 0b100101;
  } else if (Reg == AIE2PS::srSparse_of) {
    Result = 0b110101;
  } else if (Reg == AIE2PS::srFPFlags) {
    Result = 0b011001;
  } else if (Reg == AIE2PS::srF2IFlags) {
    Result = 0b001101;
  } else if (Reg == AIE2PS::srF2FFlags) {
    Result = 0b001001;
  } else if (Reg == AIE2PS::srF2BFlags) {
    Result = 0b000101;
  } else if (Reg == AIE2PS::srFPNlf) {
    Result = 0b011101;
  } else if (Reg == AIE2PS::srFPCnvFx2Fl) {
    Result = 0b010101;
  } else if (Reg == AIE2PS::srFPCnvFl2Fx) {
    Result = 0b010001;
  } else {
    llvm_unreachable("Register not in mSCm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmBMs_v16w32OpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eBMLLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMLHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMHLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMHHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mBMs_v16w32");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmGsOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eGleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eGloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eGheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eGhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mGs");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmLdaCgOpValue(const MCInst &MI, unsigned OpNo,
                                           APInt &Op,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b000;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b100;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0110;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1110;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0010;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1101;
  } else if (Reg == AIE2PS::lc) {
    Result = 0b0010101;
  } else {
    llvm_unreachable("Register not in mLdaCg");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEEmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEEeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEEoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEEm");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmFl2FxSrc_WOpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eWleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mFl2FxSrc_W");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmXbOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mXb");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmEG2mOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eEG2e_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eEG2o_sRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEG2m");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmBMsOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eBMLLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMLHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMHLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eBMHHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mBMs");
  }
  Op = Result;
}

void AIE2PSMCCodeEmitter::getmRmOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PSMCRegisterClasses[AIE2PS::eReRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PSMCRegisterClasses[AIE2PS::eRoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mRm");
  }
  Op = Result;
}
