//===-AIE2PMCCodeEmitterGen.inc -----------------------------*- tablegen
//-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//* Automatically generated file, do not edit! *

void AIE2PMCCodeEmitter::getmWaOpValue(const MCInst &MI, unsigned OpNo,
                                       APInt &Op,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eWheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PMCRegisterClasses[AIE2P::eWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PMCRegisterClasses[AIE2P::eWleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PMCRegisterClasses[AIE2P::eWloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mWa");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmXmOpValue(const MCInst &MI, unsigned OpNo,
                                       APInt &Op,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mXm");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmCMmOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eCMLRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eCMHRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mCMm");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmQEXsmOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eQEXseRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eQEXsoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mQEXsm");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmEsOpValue(const MCInst &MI, unsigned OpNo,
                                       APInt &Op,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eEheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mEs");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmAguSrcOpValue(const MCInst &MI, unsigned OpNo,
                                           APInt &Op,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0010;
  } else if (AIE2PMCRegisterClasses[AIE2P::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1101;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1001;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0101;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0001;
  } else if (AIE2PMCRegisterClasses[AIE2P::eRRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else {
    llvm_unreachable("Register not in mAguSrc");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmQXsbOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eQXseRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eQXsoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mQXsb");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmEhmOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eEheRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEhoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEhm");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmLdaSclOpValue(const MCInst &MI, unsigned OpNo,
                                           APInt &Op,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eRRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0100;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1000;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1100;
  } else if (AIE2PMCRegisterClasses[AIE2P::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0000;
  } else if (AIE2PMCRegisterClasses[AIE2P::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0011;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEheRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0001;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEhoRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1001;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEleRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0101;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEloRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1101;
  } else if (Reg == AIE2P::lr) {
    Result = 0b0000111;
  } else {
    llvm_unreachable("Register not in mLdaScl");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmQQsmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eQQseRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eQQsoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mQQsm");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmWsOpValue(const MCInst &MI, unsigned OpNo,
                                       APInt &Op,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eWheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PMCRegisterClasses[AIE2P::eWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PMCRegisterClasses[AIE2P::eWleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PMCRegisterClasses[AIE2P::eWloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mWs");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmXwOpValue(const MCInst &MI, unsigned OpNo,
                                       APInt &Op,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mXw");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmAguDstOpValue(const MCInst &MI, unsigned OpNo,
                                           APInt &Op,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b100;
  } else if (AIE2PMCRegisterClasses[AIE2P::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b011;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b010;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b001;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b000;
  } else {
    llvm_unreachable("Register not in mAguDst");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmEXmOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eEXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEXm");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmQXsaOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eQXseRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eQXsoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mQXsa");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmDmOpValue(const MCInst &MI, unsigned OpNo,
                                       APInt &Op,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else if (AIE2PMCRegisterClasses[AIE2P::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else {
    llvm_unreachable("Register not in mDm");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmQQssOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eQQseRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eQQsoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mQQss");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmQXsmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eQXseRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eQXsoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mQXsm");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmShflBMDstOpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eBMSLLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMSLHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMSHLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMSHHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mShflBMDst");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmBMmOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eBMLLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMLHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMHLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMHHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mBMm");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmMvBMXDstOpValue(const MCInst &MI, unsigned OpNo,
                                             APInt &Op,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eBMLLRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b000;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMLHRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b010;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMHLRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b100;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMHHRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b110;
  } else if (AIE2PMCRegisterClasses[AIE2P::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b001;
  } else if (AIE2PMCRegisterClasses[AIE2P::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b101;
  } else if (Reg == AIE2P::sfh) {
    Result = 0b110011;
  } else if (Reg == AIE2P::sfl) {
    Result = 0b011011;
  } else if (AIE2PMCRegisterClasses[AIE2P::eLdFifoHRegRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 5) | 0b00011;
  } else if (AIE2PMCRegisterClasses[AIE2P::eLdFifoLRegRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 5) | 0b01011;
  } else if (Reg == AIE2P::lfe) {
    Result = 0b010011;
  } else {
    llvm_unreachable("Register not in mMvBMXDst");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmShflXDstOpValue(const MCInst &MI, unsigned OpNo,
                                             APInt &Op,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mShflXDst");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmMvSclDstOpValue(const MCInst &MI, unsigned OpNo,
                                             APInt &Op,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eRRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0100;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1000;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1100;
  } else if (AIE2PMCRegisterClasses[AIE2P::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0000;
  } else if (AIE2PMCRegisterClasses[AIE2P::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0110;
  } else if (AIE2PMCRegisterClasses[AIE2P::eSRegClassID].contains(Reg)) {
    Result = (Encoding << 5) | 0b01110;
  } else if (Reg == AIE2P::le) {
    Result = 0b0110111;
  } else if (Reg == AIE2P::ls) {
    Result = 0b0001111;
  } else if (Reg == AIE2P::lr) {
    Result = 0b1110111;
  } else if (Reg == AIE2P::sp) {
    Result = 0b1001111;
  } else if (Reg == AIE2P::srCarry) {
    Result = 0b0000010;
  } else if (Reg == AIE2P::srSS0) {
    Result = 0b1100010;
  } else if (Reg == AIE2P::srMS0) {
    Result = 0b1010010;
  } else if (Reg == AIE2P::srSRS_of) {
    Result = 0b1011010;
  } else if (Reg == AIE2P::srUPS_of) {
    Result = 0b1110010;
  } else if (Reg == AIE2P::srFifo_of) {
    Result = 0b1000010;
  } else if (Reg == AIE2P::srFifo_uf) {
    Result = 0b1001010;
  } else if (Reg == AIE2P::srSparse_of) {
    Result = 0b1101010;
  } else if (Reg == AIE2P::srFPFlags) {
    Result = 0b0110010;
  } else if (Reg == AIE2P::srF2IFlags) {
    Result = 0b0011010;
  } else if (Reg == AIE2P::srF2FFlags) {
    Result = 0b0010010;
  } else if (Reg == AIE2P::srF2BFlags) {
    Result = 0b0001010;
  } else if (Reg == AIE2P::srFPNlf) {
    Result = 0b0111010;
  } else if (Reg == AIE2P::srFPCnvFx2Fl) {
    Result = 0b0101010;
  } else if (Reg == AIE2P::srFPCnvFl2Fx) {
    Result = 0b0100010;
  } else if (Reg == AIE2P::crSat) {
    Result = 0b0100111;
  } else if (Reg == AIE2P::crRnd) {
    Result = 0b1111011;
  } else if (Reg == AIE2P::crFPMask) {
    Result = 0b1101011;
  } else if (Reg == AIE2P::crF2IMask) {
    Result = 0b0001011;
  } else if (Reg == AIE2P::crF2FMask) {
    Result = 0b1110011;
  } else if (Reg == AIE2P::crF2BMask) {
    Result = 0b0110011;
  } else if (Reg == AIE2P::crSRSMode) {
    Result = 0b1000111;
  } else if (Reg == AIE2P::crUPSMode) {
    Result = 0b1100111;
  } else if (Reg == AIE2P::crUnpackSize) {
    Result = 0b0010111;
  } else if (Reg == AIE2P::crPackSize) {
    Result = 0b0111011;
  } else if (AIE2PMCRegisterClasses[AIE2P::eSRSSignRegClassID].contains(Reg)) {
    Result = (Encoding << 6) | 0b111110;
  } else if (AIE2PMCRegisterClasses[AIE2P::eUPSSignRegClassID].contains(Reg)) {
    Result = (Encoding << 6) | 0b000011;
  } else if (AIE2PMCRegisterClasses[AIE2P::ePackSignRegClassID].contains(Reg)) {
    Result = (Encoding << 6) | 0b011110;
  } else if (AIE2PMCRegisterClasses[AIE2P::eUnpackSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b100011;
  } else if (AIE2PMCRegisterClasses[AIE2P::eVaddSignRegClassID].contains(Reg)) {
    Result = (Encoding << 6) | 0b010011;
  } else if (Reg == AIE2P::crSCDEn) {
    Result = 0b0000111;
  } else if (Reg == AIE2P::crMCDEn) {
    Result = 0b1011011;
  } else if (Reg == AIE2P::crFPNlfMask) {
    Result = 0b0011011;
  } else if (Reg == AIE2P::crFPCnvFx2FlMask) {
    Result = 0b0101011;
  } else if (Reg == AIE2P::crFPCnvFl2FxMask) {
    Result = 0b1001011;
  } else if (Reg == AIE2P::lc) {
    Result = 0b1010111;
  } else {
    llvm_unreachable("Register not in mMvSclDst");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmMcdXSrcOpValue(const MCInst &MI, unsigned OpNo,
                                            APInt &Op,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mMcdXSrc");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmEXnOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eEXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEXn");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmMvSclDstCgOpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eRRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0100;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1000;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1100;
  } else if (AIE2PMCRegisterClasses[AIE2P::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0000;
  } else if (AIE2PMCRegisterClasses[AIE2P::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0110;
  } else if (AIE2PMCRegisterClasses[AIE2P::eSRegClassID].contains(Reg)) {
    Result = (Encoding << 5) | 0b01110;
  } else if (Reg == AIE2P::le) {
    Result = 0b0110111;
  } else if (Reg == AIE2P::ls) {
    Result = 0b0001111;
  } else if (Reg == AIE2P::lr) {
    Result = 0b1110111;
  } else if (Reg == AIE2P::sp) {
    Result = 0b1001111;
  } else if (Reg == AIE2P::srCarry) {
    Result = 0b0000010;
  } else if (Reg == AIE2P::srSS0) {
    Result = 0b1100010;
  } else if (Reg == AIE2P::srMS0) {
    Result = 0b1010010;
  } else if (Reg == AIE2P::srSRS_of) {
    Result = 0b1011010;
  } else if (Reg == AIE2P::srUPS_of) {
    Result = 0b1110010;
  } else if (Reg == AIE2P::srFifo_of) {
    Result = 0b1000010;
  } else if (Reg == AIE2P::srFifo_uf) {
    Result = 0b1001010;
  } else if (Reg == AIE2P::srSparse_of) {
    Result = 0b1101010;
  } else if (Reg == AIE2P::srFPFlags) {
    Result = 0b0110010;
  } else if (Reg == AIE2P::srF2IFlags) {
    Result = 0b0011010;
  } else if (Reg == AIE2P::srF2FFlags) {
    Result = 0b0010010;
  } else if (Reg == AIE2P::srF2BFlags) {
    Result = 0b0001010;
  } else if (Reg == AIE2P::srFPNlf) {
    Result = 0b0111010;
  } else if (Reg == AIE2P::srFPCnvFx2Fl) {
    Result = 0b0101010;
  } else if (Reg == AIE2P::srFPCnvFl2Fx) {
    Result = 0b0100010;
  } else if (Reg == AIE2P::crSat) {
    Result = 0b0100111;
  } else if (Reg == AIE2P::crRnd) {
    Result = 0b1111011;
  } else if (Reg == AIE2P::crFPMask) {
    Result = 0b1101011;
  } else if (Reg == AIE2P::crF2IMask) {
    Result = 0b0001011;
  } else if (Reg == AIE2P::crF2FMask) {
    Result = 0b1110011;
  } else if (Reg == AIE2P::crF2BMask) {
    Result = 0b0110011;
  } else if (Reg == AIE2P::crSRSMode) {
    Result = 0b1000111;
  } else if (Reg == AIE2P::crUPSMode) {
    Result = 0b1100111;
  } else if (Reg == AIE2P::crUnpackSize) {
    Result = 0b0010111;
  } else if (Reg == AIE2P::crPackSize) {
    Result = 0b0111011;
  } else if (AIE2PMCRegisterClasses[AIE2P::eSRSSignRegClassID].contains(Reg)) {
    Result = (Encoding << 6) | 0b111110;
  } else if (AIE2PMCRegisterClasses[AIE2P::eUPSSignRegClassID].contains(Reg)) {
    Result = (Encoding << 6) | 0b000011;
  } else if (AIE2PMCRegisterClasses[AIE2P::ePackSignRegClassID].contains(Reg)) {
    Result = (Encoding << 6) | 0b011110;
  } else if (AIE2PMCRegisterClasses[AIE2P::eUnpackSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b100011;
  } else if (AIE2PMCRegisterClasses[AIE2P::eVaddSignRegClassID].contains(Reg)) {
    Result = (Encoding << 6) | 0b010011;
  } else if (Reg == AIE2P::crSCDEn) {
    Result = 0b0000111;
  } else if (Reg == AIE2P::crMCDEn) {
    Result = 0b1011011;
  } else if (Reg == AIE2P::crFPNlfMask) {
    Result = 0b0011011;
  } else if (Reg == AIE2P::crFPCnvFx2FlMask) {
    Result = 0b0101011;
  } else if (Reg == AIE2P::crFPCnvFl2FxMask) {
    Result = 0b1001011;
  } else if (Reg == AIE2P::lc) {
    Result = 0b1010111;
  } else {
    llvm_unreachable("Register not in mMvSclDstCg");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmSclMSOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eRRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0110;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1010;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1110;
  } else if (AIE2PMCRegisterClasses[AIE2P::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0010;
  } else if (AIE2PMCRegisterClasses[AIE2P::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1101;
  } else {
    llvm_unreachable("Register not in mSclMS");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmXnOpValue(const MCInst &MI, unsigned OpNo,
                                       APInt &Op,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mXn");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmBMSmOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eBMSLLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMSLHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMSHLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMSHHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mBMSm");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmXvOpValue(const MCInst &MI, unsigned OpNo,
                                       APInt &Op,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mXv");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmWbOpValue(const MCInst &MI, unsigned OpNo,
                                       APInt &Op,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eWheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PMCRegisterClasses[AIE2P::eWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PMCRegisterClasses[AIE2P::eWleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PMCRegisterClasses[AIE2P::eWloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mWb");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmMvBMXSrcOpValue(const MCInst &MI, unsigned OpNo,
                                             APInt &Op,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eBMLLRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b000;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMLHRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b010;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMHLRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b100;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMHHRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b110;
  } else if (AIE2PMCRegisterClasses[AIE2P::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b001;
  } else if (AIE2PMCRegisterClasses[AIE2P::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 3) | 0b101;
  } else if (Reg == AIE2P::sfh) {
    Result = 0b110011;
  } else if (Reg == AIE2P::sfl) {
    Result = 0b011011;
  } else if (AIE2PMCRegisterClasses[AIE2P::eLdFifoHRegRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 5) | 0b00011;
  } else if (AIE2PMCRegisterClasses[AIE2P::eLdFifoLRegRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 5) | 0b01011;
  } else if (Reg == AIE2P::lfe) {
    Result = 0b010011;
  } else {
    llvm_unreachable("Register not in mMvBMXSrc");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmAluCgOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eRRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (Reg == AIE2P::lc) {
    Result = 0b000001;
  } else {
    llvm_unreachable("Register not in mAluCg");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmEXwOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eEXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEXw");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmXsOpValue(const MCInst &MI, unsigned OpNo,
                                       APInt &Op,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mXs");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmEXsOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eEXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEXs");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmCMsOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eCMLRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eCMHRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mCMs");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmFifoHLRegOpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (Reg == AIE2P::sfh) {
    Result = 0b110;
  } else if (Reg == AIE2P::sfl) {
    Result = 0b011;
  } else if (AIE2PMCRegisterClasses[AIE2P::eLdFifoHRegRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PMCRegisterClasses[AIE2P::eLdFifoLRegRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (Reg == AIE2P::lfe) {
    Result = 0b010;
  } else {
    llvm_unreachable("Register not in mFifoHLReg");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmSRmOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  if (Reg == AIE2P::srCarry) {
    Result = 0b0000;
  } else if (Reg == AIE2P::srSS0) {
    Result = 0b1100;
  } else if (Reg == AIE2P::srMS0) {
    Result = 0b1010;
  } else if (Reg == AIE2P::srSRS_of) {
    Result = 0b1011;
  } else if (Reg == AIE2P::srUPS_of) {
    Result = 0b1110;
  } else if (Reg == AIE2P::srFifo_of) {
    Result = 0b1000;
  } else if (Reg == AIE2P::srFifo_uf) {
    Result = 0b1001;
  } else if (Reg == AIE2P::srSparse_of) {
    Result = 0b1101;
  } else if (Reg == AIE2P::srFPFlags) {
    Result = 0b0110;
  } else if (Reg == AIE2P::srF2IFlags) {
    Result = 0b0011;
  } else if (Reg == AIE2P::srF2FFlags) {
    Result = 0b0010;
  } else if (Reg == AIE2P::srF2BFlags) {
    Result = 0b0001;
  } else if (Reg == AIE2P::srFPNlf) {
    Result = 0b0111;
  } else if (Reg == AIE2P::srFPCnvFx2Fl) {
    Result = 0b0101;
  } else if (Reg == AIE2P::srFPCnvFl2Fx) {
    Result = 0b0100;
  } else {
    llvm_unreachable("Register not in mSRm");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmElmOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eEleRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEloRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mElm");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmQEXswOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eQEXseRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eQEXsoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mQEXsw");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmQQsaOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eQQseRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eQQsoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mQQsa");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmEXvOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eEXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEXv");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmXaOpValue(const MCInst &MI, unsigned OpNo,
                                       APInt &Op,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mXa");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmWmOpValue(const MCInst &MI, unsigned OpNo,
                                       APInt &Op,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eWheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PMCRegisterClasses[AIE2P::eWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PMCRegisterClasses[AIE2P::eWleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PMCRegisterClasses[AIE2P::eWloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mWm");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmMvSclSrcOpValue(const MCInst &MI, unsigned OpNo,
                                             APInt &Op,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eRRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0100;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1000;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1100;
  } else if (AIE2PMCRegisterClasses[AIE2P::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0000;
  } else if (AIE2PMCRegisterClasses[AIE2P::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0110;
  } else if (AIE2PMCRegisterClasses[AIE2P::eSRegClassID].contains(Reg)) {
    Result = (Encoding << 5) | 0b01110;
  } else if (Reg == AIE2P::le) {
    Result = 0b1110111;
  } else if (Reg == AIE2P::ls) {
    Result = 0b1001111;
  } else if (Reg == AIE2P::lr) {
    Result = 0b0001111;
  } else if (Reg == AIE2P::sp) {
    Result = 0b0101111;
  } else if (Reg == AIE2P::srCarry) {
    Result = 0b0000010;
  } else if (Reg == AIE2P::srSS0) {
    Result = 0b1100010;
  } else if (Reg == AIE2P::srMS0) {
    Result = 0b1010010;
  } else if (Reg == AIE2P::srSRS_of) {
    Result = 0b1011010;
  } else if (Reg == AIE2P::srUPS_of) {
    Result = 0b1110010;
  } else if (Reg == AIE2P::srFifo_of) {
    Result = 0b1000010;
  } else if (Reg == AIE2P::srFifo_uf) {
    Result = 0b1001010;
  } else if (Reg == AIE2P::srSparse_of) {
    Result = 0b1101010;
  } else if (Reg == AIE2P::srFPFlags) {
    Result = 0b0110010;
  } else if (Reg == AIE2P::srF2IFlags) {
    Result = 0b0011010;
  } else if (Reg == AIE2P::srF2FFlags) {
    Result = 0b0010010;
  } else if (Reg == AIE2P::srF2BFlags) {
    Result = 0b0001010;
  } else if (Reg == AIE2P::srFPNlf) {
    Result = 0b0111010;
  } else if (Reg == AIE2P::srFPCnvFx2Fl) {
    Result = 0b0101010;
  } else if (Reg == AIE2P::srFPCnvFl2Fx) {
    Result = 0b0100010;
  } else if (Reg == AIE2P::crSat) {
    Result = 0b0100111;
  } else if (Reg == AIE2P::crRnd) {
    Result = 0b1111011;
  } else if (Reg == AIE2P::crFPMask) {
    Result = 0b1101011;
  } else if (Reg == AIE2P::crF2IMask) {
    Result = 0b0001011;
  } else if (Reg == AIE2P::crF2FMask) {
    Result = 0b1110011;
  } else if (Reg == AIE2P::crF2BMask) {
    Result = 0b0110011;
  } else if (Reg == AIE2P::crSRSMode) {
    Result = 0b1000111;
  } else if (Reg == AIE2P::crUPSMode) {
    Result = 0b1100111;
  } else if (Reg == AIE2P::crUnpackSize) {
    Result = 0b0010111;
  } else if (Reg == AIE2P::crPackSize) {
    Result = 0b0111011;
  } else if (AIE2PMCRegisterClasses[AIE2P::eSRSSignRegClassID].contains(Reg)) {
    Result = (Encoding << 6) | 0b111110;
  } else if (AIE2PMCRegisterClasses[AIE2P::eUPSSignRegClassID].contains(Reg)) {
    Result = (Encoding << 6) | 0b000011;
  } else if (AIE2PMCRegisterClasses[AIE2P::ePackSignRegClassID].contains(Reg)) {
    Result = (Encoding << 6) | 0b011110;
  } else if (AIE2PMCRegisterClasses[AIE2P::eUnpackSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 6) | 0b100011;
  } else if (AIE2PMCRegisterClasses[AIE2P::eVaddSignRegClassID].contains(Reg)) {
    Result = (Encoding << 6) | 0b010011;
  } else if (Reg == AIE2P::crSCDEn) {
    Result = 0b0000111;
  } else if (Reg == AIE2P::crMCDEn) {
    Result = 0b1011011;
  } else if (Reg == AIE2P::crFPNlfMask) {
    Result = 0b0011011;
  } else if (Reg == AIE2P::crFPCnvFx2FlMask) {
    Result = 0b0101011;
  } else if (Reg == AIE2P::crFPCnvFl2FxMask) {
    Result = 0b1001011;
  } else if (Reg == AIE2P::lc) {
    Result = 0b0110111;
  } else if (Reg == AIE2P::CORE_ID) {
    Result = 0b1010111;
  } else {
    llvm_unreachable("Register not in mMvSclSrc");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmQEXsaOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eQEXseRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eQEXsoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mQEXsa");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmCRmOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (Reg == AIE2P::crSat) {
    Result = 0b01110;
  } else if (Reg == AIE2P::crRnd) {
    Result = 0b01010;
  } else if (Reg == AIE2P::crFPMask) {
    Result = 0b00110;
  } else if (Reg == AIE2P::crF2IMask) {
    Result = 0b00100;
  } else if (Reg == AIE2P::crF2FMask) {
    Result = 0b00010;
  } else if (Reg == AIE2P::crF2BMask) {
    Result = 0b00000;
  } else if (Reg == AIE2P::crSRSMode) {
    Result = 0b01100;
  } else if (Reg == AIE2P::crUPSMode) {
    Result = 0b10000;
  } else if (Reg == AIE2P::crUnpackSize) {
    Result = 0b10010;
  } else if (Reg == AIE2P::crPackSize) {
    Result = 0b01000;
  } else if (AIE2PMCRegisterClasses[AIE2P::eSRSSignRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0011;
  } else if (AIE2PMCRegisterClasses[AIE2P::eUPSSignRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0101;
  } else if (AIE2PMCRegisterClasses[AIE2P::ePackSignRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0001;
  } else if (AIE2PMCRegisterClasses[AIE2P::eUnpackSignRegClassID].contains(
                 Reg)) {
    Result = (Encoding << 4) | 0b0111;
  } else if (AIE2PMCRegisterClasses[AIE2P::eVaddSignRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1001;
  } else if (Reg == AIE2P::crSCDEn) {
    Result = 0b11011;
  } else if (Reg == AIE2P::crMCDEn) {
    Result = 0b01011;
  } else {
    llvm_unreachable("Register not in mCRm");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmSclStOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eRRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0100;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1000;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1100;
  } else if (AIE2PMCRegisterClasses[AIE2P::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0000;
  } else if (AIE2PMCRegisterClasses[AIE2P::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0011;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEheRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0001;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEhoRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1001;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEleRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0101;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEloRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1101;
  } else if (Reg == AIE2P::lr) {
    Result = 0b0000111;
  } else {
    llvm_unreachable("Register not in mSclSt");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmMcdBMSrcOpValue(const MCInst &MI, unsigned OpNo,
                                             APInt &Op,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eBMLLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMLHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMHLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMHHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mMcdBMSrc");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmQXswOpValue(const MCInst &MI, unsigned OpNo,
                                         APInt &Op,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eQXseRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eQXsoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mQXsw");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmEXaOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eEXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEXa");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmQEXsbOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eQEXseRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eQEXsoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mQEXsb");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmLdaCgOpValue(const MCInst &MI, unsigned OpNo,
                                          APInt &Op,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eRRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDNRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0110;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDJRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1010;
  } else if (AIE2PMCRegisterClasses[AIE2P::eDCRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1110;
  } else if (AIE2PMCRegisterClasses[AIE2P::eMRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b0010;
  } else if (AIE2PMCRegisterClasses[AIE2P::ePRegClassID].contains(Reg)) {
    Result = (Encoding << 4) | 0b1101;
  } else if (Reg == AIE2P::lc) {
    Result = 0b0010101;
  } else {
    llvm_unreachable("Register not in mLdaCg");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmFl2FxSrc_WOpValue(
    const MCInst &MI, unsigned OpNo, APInt &Op,
    SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eWheRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PMCRegisterClasses[AIE2P::eWhoRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PMCRegisterClasses[AIE2P::eWleRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PMCRegisterClasses[AIE2P::eWloRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mFl2FxSrc_W");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmXbOpValue(const MCInst &MI, unsigned OpNo,
                                       APInt &Op,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mXb");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmBMsOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eBMLLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b00;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMLHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b01;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMHLRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b10;
  } else if (AIE2PMCRegisterClasses[AIE2P::eBMHHRegClassID].contains(Reg)) {
    Result = (Encoding << 2) | 0b11;
  } else {
    llvm_unreachable("Register not in mBMs");
  }
  Op = Result;
}

void AIE2PMCCodeEmitter::getmEXbOpValue(const MCInst &MI, unsigned OpNo,
                                        APInt &Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  MCOperand RegOp = MI.getOperand(OpNo);
  unsigned Reg = RegOp.getReg();
  uint64_t Result = 0;
  uint64_t Encoding = Ctx.getRegisterInfo()->getEncodingValue(Reg);

  if (AIE2PMCRegisterClasses[AIE2P::eEXeRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b0;
  } else if (AIE2PMCRegisterClasses[AIE2P::eEXoRegClassID].contains(Reg)) {
    Result = (Encoding << 1) | 0b1;
  } else {
    llvm_unreachable("Register not in mEXb");
  }
  Op = Result;
}
