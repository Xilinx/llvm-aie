//===- AIE2MCCodeEmitterRegOperandDef.h -------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
void AIE2MCCodeEmitter::getmAluCgOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);
  if (Reg == AIE2::LC) {
    Result = 0b000001;
  } else if (AIE2MCRegisterClasses[AIE2::eRRegClassID].contains(Reg)) {
    Result = (Encoding << 1);
  } else {
    llvm_unreachable("Register not in AluCg");
  }
  Op = Result;
}

void AIE2MCCodeEmitter::getmLdaCgOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);
  if (Reg == AIE2::LC) {
    Result = 0b0010101;
  } else if (AIE2MCRegisterClasses[AIE2::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1101;
  } else if (AIE2MCRegisterClasses[AIE2::eRRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2MCRegisterClasses[AIE2::eMRegClassID].contains(Reg)) {
    Result = ((Encoding | 0b00000) << 2) | 0b10;
  } else if (AIE2MCRegisterClasses[AIE2::eDNRegClassID].contains(Reg)) {
    Result = ((Encoding | 0b01000) << 2) | 0b10;
  } else if (AIE2MCRegisterClasses[AIE2::eDJRegClassID].contains(Reg)) {
    Result = ((Encoding | 0b10000) << 2) | 0b10;
  } else if (AIE2MCRegisterClasses[AIE2::eDCRegClassID].contains(Reg)) {
    Result = ((Encoding | 0b11000) << 2) | 0b10;
  } else {
    llvm_unreachable("Register not in LdaCg");
  }
  Op = Result;
}

void AIE2MCCodeEmitter::getmLdaSclOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);
  if (Reg == AIE2::lr) {
    Result = 0b0000101;
  } else if (AIE2MCRegisterClasses[AIE2::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1101;
  } else if (AIE2MCRegisterClasses[AIE2::eRRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2MCRegisterClasses[AIE2::eMRegClassID].contains(Reg)) {
    Result = ((Encoding | 0b00000) << 2) | 0b10;
  } else if (AIE2MCRegisterClasses[AIE2::eDNRegClassID].contains(Reg)) {
    Result = ((Encoding | 0b01000) << 2) | 0b10;
  } else if (AIE2MCRegisterClasses[AIE2::eDJRegClassID].contains(Reg)) {
    Result = ((Encoding | 0b10000) << 2) | 0b10;
  } else if (AIE2MCRegisterClasses[AIE2::eDCRegClassID].contains(Reg)) {
    Result = ((Encoding | 0b11000) << 2) | 0b10;
  } else {
    llvm_unreachable("Register not in LdaScl");
  }
  Op = Result;
}

void AIE2MCCodeEmitter::getmSclMSOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  // Happens to be the same.
  getmLdaSclOpValue(MI, OpNo, Op, Fixups, STI);
}

void AIE2MCCodeEmitter::getmSclStOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  // Happens to be the same.
  getmLdaSclOpValue(MI, OpNo, Op, Fixups, STI);
}

void AIE2MCCodeEmitter::getmMvSclSrcOpValue(const MCInst &MI, unsigned OpNo,
                                            APInt &Op,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);
  switch (Reg) {
  case AIE2::LC:
  case AIE2::SP:
  case AIE2::lr:
  case AIE2::LS:
  case AIE2::LE:
  case AIE2::DP:
  case AIE2::CORE_ID:
    Result = Encoding;
    Op = Result;
    return;
  }

  if (AIE2MCRegisterClasses[AIE2::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0011;
  } else if (AIE2MCRegisterClasses[AIE2::eRRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2MCRegisterClasses[AIE2::eMRegClassID].contains(Reg)) {
    Result = ((Encoding | 0b0000) << 2) | 0b10;
  } else if (AIE2MCRegisterClasses[AIE2::eDNRegClassID].contains(Reg)) {
    Result = ((Encoding | 0b01000) << 2) | 0b10;
  } else if (AIE2MCRegisterClasses[AIE2::eDJRegClassID].contains(Reg)) {
    Result = ((Encoding | 0b10000) << 2) | 0b10;
  } else if (AIE2MCRegisterClasses[AIE2::eDCRegClassID].contains(Reg)) {
    Result = ((Encoding | 0b11000) << 2) | 0b10;
  } else if (AIE2MCRegisterClasses[AIE2::eSRegClassID].contains(Reg)) {
    Result = (Encoding << 5) | 0b01011;
  } else if (AIE2MCRegisterClasses[AIE2::mCRmRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b001;
  } else if (AIE2MCRegisterClasses[AIE2::mSRmRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b101;
  } else {
    llvm_unreachable("Register not in MvSclSrc");
  }
  Op = Result;
}

void AIE2MCCodeEmitter::getmMvSclDstOpValue(const MCInst &MI, unsigned OpNo,
                                            APInt &Op,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  // Happens to be the same.
  getmMvSclSrcOpValue(MI, OpNo, Op, Fixups, STI);
}

void AIE2MCCodeEmitter::getmMvSclDstCgOpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  // Happens to be the same.
  getmMvSclSrcOpValue(MI, OpNo, Op, Fixups, STI);
}

void AIE2MCCodeEmitter::getmMvAMWQDstOpValue(const MCInst &MI, unsigned OpNo,
                                             APInt &Op,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);
  if (AIE2MCRegisterClasses[AIE2::mAMmRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else if (AIE2MCRegisterClasses[AIE2::eWLERegClassID].contains(Reg)) {
    Result = (0b00 << 5) | ((Encoding >> 2) << 2) | 0b00;
  } else if (AIE2MCRegisterClasses[AIE2::eWLORegClassID].contains(Reg)) {
    Result = (0b01 << 5) | ((Encoding >> 2) << 2) | 0b00;
  } else if (AIE2MCRegisterClasses[AIE2::eWHERegClassID].contains(Reg)) {
    Result = (0b10 << 5) | ((Encoding >> 2) << 2) | 0b00;
  } else if (AIE2MCRegisterClasses[AIE2::eWHORegClassID].contains(Reg)) {
    Result = (0b11 << 5) | ((Encoding >> 2) << 2) | 0b00;
  } else if (AIE2MCRegisterClasses[AIE2::mQQmRegClassID].contains(Reg)) {
    Result = (Encoding << 5) | 0b00010;
  } else {
    llvm_unreachable("Register not in mMvAMWQDst");
  }
  Op = Result;
}

void AIE2MCCodeEmitter::getmMvAMWQSrcOpValue(const MCInst &MI, unsigned OpNo,
                                             APInt &Op,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);
  if (AIE2MCRegisterClasses[AIE2::mAMmRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b001;
  } else if (AIE2MCRegisterClasses[AIE2::eWLERegClassID].contains(Reg)) {
    Result = (0b110000 << 3) | (Encoding >> 2);
  } else if (AIE2MCRegisterClasses[AIE2::eWLORegClassID].contains(Reg)) {
    Result = (0b110001 << 3) | (Encoding >> 2);
  } else if (AIE2MCRegisterClasses[AIE2::eWHERegClassID].contains(Reg)) {
    Result = (0b110010 << 3) | (Encoding >> 2);
  } else if (AIE2MCRegisterClasses[AIE2::eWHORegClassID].contains(Reg)) {
    Result = (0b110011 << 3) | (Encoding >> 2);
  } else if (AIE2MCRegisterClasses[AIE2::mQQmRegClassID].contains(Reg)) {
    Result = (0b111 << 6) | (Encoding << 4) | 0b0000;
  } else {
    llvm_unreachable("Register not in mMvAMWQSrc");
  }
  Op = Result;
}

void AIE2MCCodeEmitter::getmMvBMXSrcOpValue(const MCInst &MI, unsigned OpNo,
                                            APInt &Op,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);
  if (AIE2MCRegisterClasses[AIE2::mBMmRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0000;
  } else if (AIE2MCRegisterClasses[AIE2::mXmRegClassID].contains(Reg)) {
    Result = (0b11000 << 4) | Encoding;
  } else {
    llvm_unreachable("Register not in mMvBMXSrc");
  }
  Op = Result;
}

void AIE2MCCodeEmitter::getmMvBMXDstOpValue(const MCInst &MI, unsigned OpNo,
                                            APInt &Op,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);
  if (AIE2MCRegisterClasses[AIE2::mBMmRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else if (AIE2MCRegisterClasses[AIE2::mXmRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else {
    llvm_unreachable("Register not in mMvBMXDst");
  }
  Op = Result;
}

void AIE2MCCodeEmitter::geteRS4OpValue(const MCInst &MI, unsigned OpNo,
                                       APInt &Op,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);
  if (AIE2MCRegisterClasses[AIE2::eRS4RegClassID].contains(Reg)) {
    Op = Encoding - 16;
  } else {
    llvm_unreachable("Register not in eRS4");
  }
}

void AIE2MCCodeEmitter::getmShflDstOpValue(const MCInst &MI, unsigned OpNo,
                                           APInt &Op,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);
  if (AIE2MCRegisterClasses[AIE2::mBMSmRegClassID].contains(Reg)) {
    Result = ((((Encoding & 0b1) << 3) | (Encoding >> 1)) << 1) | 0b1;
    /*Note: The above encoding is needed since mBMSm is not defined properly
     * The most significant bit is supposed to decide if it is Lower or Upper
     * set, But bml/bmh by default forces least significant */
  } else if (AIE2MCRegisterClasses[AIE2::mXmRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else {
    llvm_unreachable("Register not in mShflDst");
  }
  Op = Result;
}

void AIE2MCCodeEmitter::getmWm_1OpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);
  if (AIE2MCRegisterClasses[AIE2::mWmRegClassID].contains(Reg)) {
    Result = (((((Encoding & 0b1) << 1) | ((Encoding >> 1) & 0b1)) << 3) |
              (Encoding >> 2));
    /*Note: The above encoding is needed since mWm is not defined properly
     * there are two possible encoding we force to 0 by default*/
  } else {
    llvm_unreachable("Register not in mWm_1");
  }
  Op = Result;
}

void AIE2MCCodeEmitter::getmQXHLbOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);
  if (AIE2MCRegisterClasses[AIE2::mQXHbRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2MCRegisterClasses[AIE2::mQXLbRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mQXHLb");
  }
  Op = Result;
}
