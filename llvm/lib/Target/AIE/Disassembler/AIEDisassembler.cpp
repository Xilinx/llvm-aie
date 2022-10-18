//===-- AIEDisassembler.cpp - Disassembler for AIE --------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIEDisassembler class.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseDisassembler.h"
#include "AIERegisterInfo.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDecoderOps.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Endian.h"

using namespace llvm;

#define DEBUG_TYPE "aie-disassembler"

using namespace AIE;

namespace {
class AIEDisassembler : public MCDisassembler {

public:
  AIEDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
      : MCDisassembler(STI, Ctx) {}

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &CStream) const override;
};
} // end anonymous namespace

MCDisassembler *createAIE1Disassembler(const Target &T,
                                       const MCSubtargetInfo &STI,
                                       MCContext &Ctx) {
  return new AIEDisassembler(STI, Ctx);
}

static const unsigned GPRDecoderTable[] = {
  r0,  r1,  r2,  r3,
  r4,  r5,  r6,  r7,
  r8,  r9,  r10, r11,
  r12, r13, r14, r15
};

static const unsigned PTRDecoderTable[] = {
  p0,  p1,  p2,  p3,
  p4,  p5,  p6,  p7,
};

static const unsigned MODDecoderTable[] = {
  m0,  m1,  m2,  m3,
  m4,  m5,  m6,  m7,
};

static const unsigned SPRDecoderTable[] = {
  0,  SP,  LC,  LE,  LS,  lr,   0,    0,
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
  s0,  s1,  s2,  s3,  s4,  s5,  s6,  s7,
  mc0, mc1, mc0, mc1,md0, md1, md0, md1,
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
};

static const unsigned mSDecoderTable[] = {
  s0,  s1,  s2,  s3,  s4,  s5,  s6,  s7,
};

static const unsigned mCBDecoderTable[] = {
  cb0,  cb1,  cb2,  cb3,
  cb4,  cb5,  cb6,  cb7,
};
static const unsigned mCSDecoderTable[] = {
  cs0,  cs1,  cs2,  cs3,
  cs4,  cs5,  cs6,  cs7,
};
static const unsigned mCLDecoderTable[] = {
  cl0,  cl1,  cl2,  cl3,
  cl4,  cl5,  cl6,  cl7,
};
static const unsigned mCHDecoderTable[] = {
  ch0,  ch1,  ch2,  ch3,
  ch4,  ch5,  ch6,  ch7,
};
static const unsigned mCDecoderTable[] = {
  c0,  c1,  c2,  c3,
  c4,  c5,  c6,  c7,
};

static const unsigned GPRPTRMODDecoderTable[] = {
  // 0x0????
  m0,  m1,  m2,  m3,  m4,  m5,  m6,  m7,
  p0,  p1,  p2,  p3,  p4,  p5,  p6,  p7,
  // 0x1????
  r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,
  r8,  r9,  r10, r11, r12, r13, r14, r15

};

// mMvScl Encoding
// 000	mCHm_CLH
// 001	mCLm_CLH
// 01x	mAm
// 010  mCB
// 011  mCS
// 100	mMm
// 101	mPm
// 11	mRm

static const unsigned mMvSclDecoderTable[] = {
  // 0x00????
  ch0, ch1, ch2, ch3, ch4, ch5, ch6, ch7,
  cl0, cl1, cl2, cl3, cl4, cl5, cl6, cl7,
  // 0x01????
  cb0, cb1, cb2, cb3, cb4, cb5, cb6, cb7,
  cs0, cs1, cs2, cs3, cs4, cs5, cs6, cs7,
  // 0x10????
  m0,  m1,  m2,  m3,  m4,  m5,  m6,  m7,
  p0,  p1,  p2,  p3,  p4,  p5,  p6,  p7,
  // 0x11????
  r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,
  r8,  r9,  r10, r11, r12, r13, r14, r15

};

// mAluCg12 encoding
// 0001xx	mLCcg
// 001	mCS   NOTE: different from mvSCL encoding
// 100	mMf
// 11	mRa
static const unsigned mAluCg12DecoderTable[] = {
  // 0x00????
  0, 0, 0, 0, LC, LC, LC, LC,
  cs0, cs1, cs2, cs3, cs4, cs5, cs6, cs7,
  // 0x01????
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
  // 0x10????
  m0,  m1,  m2,  m3,  m4,  m5,  m6,  m7,
  0,   0,   0,   0,   0,   0,   0,   0,
  // 0x11????
  r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,
  r8,  r9,  r10, r11, r12, r13, r14, r15

};

// mSclSt Encoding
// 001xxx	mlr
// 010	mCHm_CLH
// 011	mCLm_CLH
// 100	mMm
// 101	mPm
// 11	mRm

static const unsigned mSclStDecoderTable[] = {
  // 0x00????
  0,   0,   0,   0,   0,   0,   0,   0,
  lr,  lr,  lr,  lr,  lr, lr,  lr,  lr,
  // 0x01????
  ch0, ch1, ch2, ch3, ch4, ch5, ch6, ch7,
  cl0, cl1, cl2, cl3, cl4, cl5, cl6, cl7,
  // 0x10????
  m0,  m1,  m2,  m3,  m4,  m5,  m6,  m7,
  p0,  p1,  p2,  p3,  p4,  p5,  p6,  p7,
  // 0x11????
  r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,
  r8,  r9,  r10, r11, r12, r13, r14, r15

};

// mLdaScl Encoding
// 000	mCB
// 001	mCS
// 010	mCHm_CLH
// 011	mCLm_CLH
// 100	mMm
// 101	mPm
// 11	  mRa

static const unsigned mLdaSclDecoderTable[] = {
  // 0x00????
  cb0, cb1, cb2, cb3, cb4, cb5, cb6, cb7,
  cs0, cs1, cs2, cs3, cs4, cs5, cs6, cs7,
  // 0x01????
  ch0, ch1, ch2, ch3, ch4, ch5, ch6, ch7,
  cl0, cl1, cl2, cl3, cl4, cl5, cl6, cl7,
  // 0x10????
  m0,  m1,  m2,  m3,  m4,  m5,  m6,  m7,
  p0,  p1,  p2,  p3,  p4,  p5,  p6,  p7,
  // 0x11????
  r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,
  r8,  r9,  r10, r11, r12, r13, r14, r15

};

// mLdbScl Encoding (Really!)
// 0	mMm	10
// 0	mPm	11
// 0100	mLC	0x
// 0101	mLE	0x
// 0110	mLR	0x
// 0111	mLS	0x
// 1	mRb	1
// 1	mCLm_CLH	10
// 1	mCHm_CLH	00
static const unsigned mLdbSclDecoderTable[] = {
  // 0x00????
  0,   0,   m0, p0,   0,   0,   m1, p1,
  0,   0,   m2, p2,   0,   0,   m3, p3,
  // 0x010???
  LC,  LC,  m4, p4,   LE,  LE,  m5, p5,
  // 0x011???
  lr,  lr,  m6, p6,   LS,  LS,  m7, p7,
  // 0x10????
  ch0, r0,  cl0,r1,   ch1, r2,  cl1,r3,
  ch2, r4,  cl2,r5,   ch3, r6,  cl3,r7,
  ch4, r8,  cl4,r9,   ch5, r10, cl5,r11,
  ch6, r12, cl5,r13,  ch7, r14, cl6,r15,
};

// mMv0Cg20 Encoding
// 000	mCHm_CLH
// 001	mCLm_CLH
// 010	mCB
// 011x00	mLS
// 011x01	mSP
// 011x11	mLE
// 101	mPm
// 11	mRm
static const unsigned mMv0Cg20DecoderTable[] = {
  // 0x00????
  ch0, ch1, ch2, ch3, ch4, ch5, ch6, ch7,
  cl0, cl1, cl2, cl3, cl4, cl5, cl6, cl7,
  // 0x01????
  cb0, cb1, cb2, cb3, cb4, cb5, cb6, cb7,
  LS,  SP,  0,   LE,  LS,  SP,  0,   LE,
  // 0x10????
  0,   0,   0,   0,   0,   0,   0,   0,
  p0,  p1,  p2,  p3,  p4,  p5,  p6,  p7,
  // 0x11????
  r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,
  r8,  r9,  r10, r11, r12, r13, r14, r15

};

//mRCm Encoding
// 01	mRm
// 001	mCLm_CLH
// 000	mCHm_CLH
static const unsigned mRCmDecoderTable[] = {
  // 0x00????
  ch0, ch1, ch2, ch3, ch4, ch5, ch6, ch7,
  cl0, cl1, cl2, cl3, cl4, cl5, cl6, cl7,
  // 0x01????
  r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,
  r8,  r9,  r10, r11, r12, r13, r14, r15,
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
};

// mMCMD Encoding (part of mBitGetSet
// 010x0	mMC0
// 010x1	mMC1
// 011x0	mMD0
// 011x1	mMD1
static const unsigned mMCMDDecoderTable[] = {
  // 0x0????
  0,   0,   0,   0,   0,   0,   0,   0,
  mc0, mc1, mc0, mc1, md0, md1, md0, md1,
  // 0x10???
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0,
};

static const unsigned VEC128DecoderTable[] = {
  vl0,  vl1,  vl2,  vl3,  vl4,  vl5,  vl6,  vl7,
  vh0,  vh1,  vh2,  vh3,  vh4,  vh5,  vh6,  vh7,
};

static const unsigned VEC256DecoderTable[] = {
  wr0,  wr1,  wr2,  wr3,  wc0,  wc1,  wd0,  wd1,
};

static const unsigned VEC512DecoderTable[] = {
  xa,  xb,  xc,  xd,
};

static const unsigned VEC1024DecoderTable[] = {
  ya,  yd,  xc,  xd,
};

static const unsigned VECFWCDecoderTable[] = {
  wc0,  wc1,
};
static const unsigned mWBDvDecoderTable[] = {
  wr2, wr3, wd0, wd1
};
static const unsigned mXCDvDecoderTable[] = {
  xc, xd
};
static const unsigned mVnDecoderTable[] = {
  vl0,   vh0,
  vl1,   vh1,
  vl2,   vh2,
  vl3,   vh3,
  vl4,   vh4,
  vl5,   vh5,
  vl6,   vh6,
  vl7,   vh7,
};

static const unsigned ACC384DecoderTable[] = {
  aml0,  aml1,  aml2,  aml3,
  amh0,  amh1,  amh2,  amh3,
};

static const unsigned ACC768DecoderTable[] = {
  bm0,  bm1,  bm2,  bm3,
};

static const unsigned AMLDecoderTable[] = {
  aml0,  aml1,  aml2,  aml3,
};

static const unsigned AMHDecoderTable[] = {
  amh0,  amh1,  amh2,  amh3,
};

#define TABLEBASEDDECODER(ClassName) \
DecodeStatus Decode##ClassName##RegisterClass(MCInst &Inst, uint64_t RegNo, \
                                           uint64_t Address, \
                                           const MCDisassembler *Decoder) { \
  if (RegNo >= std::size(ClassName##DecoderTable)) \
    return MCDisassembler::Fail; \
  unsigned Reg = ClassName##DecoderTable[RegNo]; \
  assert(Reg && "Register not found!"); \
  Inst.addOperand(MCOperand::createReg(Reg)); \
  return MCDisassembler::Success; \
}

TABLEBASEDDECODER(GPR)
TABLEBASEDDECODER(PTR)
TABLEBASEDDECODER(MOD)
TABLEBASEDDECODER(SPR)
TABLEBASEDDECODER(mS)
TABLEBASEDDECODER(mCB)
TABLEBASEDDECODER(mCS)
TABLEBASEDDECODER(mCL)
TABLEBASEDDECODER(mCH)
TABLEBASEDDECODER(mC)
TABLEBASEDDECODER(GPRPTRMOD)
TABLEBASEDDECODER(mMvScl)
TABLEBASEDDECODER(mSclSt)
TABLEBASEDDECODER(mLdaScl)
TABLEBASEDDECODER(mLdbScl)
TABLEBASEDDECODER(mMv0Cg20)
TABLEBASEDDECODER(mRCm)
TABLEBASEDDECODER(mMCMD)
TABLEBASEDDECODER(mAluCg12)
TABLEBASEDDECODER(VEC128)
TABLEBASEDDECODER(VEC256)
TABLEBASEDDECODER(VEC512)
TABLEBASEDDECODER(VEC1024)
TABLEBASEDDECODER(VECFWC)
TABLEBASEDDECODER(mWBDv)
TABLEBASEDDECODER(mXCDv)
TABLEBASEDDECODER(mVn)
TABLEBASEDDECODER(ACC384)
TABLEBASEDDECODER(ACC768)
TABLEBASEDDECODER(AML)
TABLEBASEDDECODER(AMH)

#define FIXEDREGDECODER(ClassName)                                             \
  template <typename InsnType>                                                 \
  static DecodeStatus Decode##ClassName##RegisterClass(                        \
      MCInst &Inst, InsnType Insn, uint64_t Address,                           \
      const MCDisassembler *Decoder) {                                         \
    return Decoder->decodeSingletonRegClass(Inst, ClassName##RegClass);        \
  }

FIXEDREGDECODER(GPR0)
FIXEDREGDECODER(GPR1)
FIXEDREGDECODER(LR)
FIXEDREGDECODER(MC0)
FIXEDREGDECODER(MD0)
FIXEDREGDECODER(S2)
FIXEDREGDECODER(S3)
FIXEDREGDECODER(S7)

DecodeStatus DecodeGPR0_7RegisterClass(MCInst &Inst, uint64_t RegNo,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  if (RegNo >= 8)
      return MCDisassembler::Fail;
  return DecodeGPRRegisterClass(Inst, RegNo, Address, Decoder);
}

// NOTE: The usage of templates are needed as InsnType could be APInt or
// uint64_t, even with the "APInt interface" as some temporaires are still
// relying on the integral type.

#define SLOTDECODERDecl(ClassName)                                             \
  template <typename InsnType>                                                 \
  static DecodeStatus decode##ClassName##Instruction(                          \
      MCInst &MI, InsnType &Insn, int64_t Address,                             \
      const MCDisassembler *Decoder)

SLOTDECODERDecl(Lng);
SLOTDECODERDecl(Alu);
SLOTDECODERDecl(St);
SLOTDECODERDecl(Lda);
SLOTDECODERDecl(Ldb);
SLOTDECODERDecl(Mv0);
SLOTDECODERDecl(Mv1);
SLOTDECODERDecl(Mv1ssb);
SLOTDECODERDecl(VecMed);
SLOTDECODERDecl(VecAll);
SLOTDECODERDecl(VecShft);
SLOTDECODERDecl(VecStrm);
SLOTDECODERDecl(VecShrt);

#include "AIEGenDisassemblerTables.inc"

template <typename InsnType>
DecodeStatus decodeLngInstruction(MCInst &Inst, InsnType &Imm, int64_t Address,
                                  const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Long Instruction\n");
  const AIEDisassembler *disassembler = (const AIEDisassembler *)Decoder;
  MCInst *MCI = disassembler->getContext().createMCInst();
  Imm <<= 4;
  Imm |= 0xB;
  DecodeStatus Result = decodeInstruction(DecoderTable32, *MCI, Imm, Address, Decoder, disassembler->getSubtargetInfo());
  LLVM_DEBUG(dbgs() << "Result = " << Result << "\n");
  if (Result != MCDisassembler::Success) {
    MCI->clear();
    MCI->setOpcode(UNKNOWN_lng);
  }
  Inst.addOperand(MCOperand::createInst(MCI));
  return MCDisassembler::Success;
}

template <typename InsnType>
DecodeStatus decodeVecShftInstruction(MCInst &Inst, InsnType &SlotEncoding,
                                      int64_t Address,
                                      const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode VecShft Instruction\n");
  const AIEDisassembler *disassembler = (const AIEDisassembler *)Decoder;
  MCInst *MCI = disassembler->getContext().createMCInst();

  // Slot encoding in the canonical format:
  // -------------------------------------------------
  // |                instr32                |  011  |
  // -------------------------------------------------
  // |           vec_med              | xx10 |   -   |
  // ---------------------------------|---------------
  // | 000 |        ved_dpd           |      --      |
  // ---------------------------------|---------------
  // |  -  | 0....0 | v_shft | 0....0 |      --      |
  // -------------------------------------------------

  SlotEncoding <<= 17;
  SlotEncoding |= (0b0010 << 3) | 0b011;

  DecodeStatus Result =
      decodeInstruction(DecoderTable32, *MCI, SlotEncoding, Address, Decoder,
                        disassembler->getSubtargetInfo());
  LLVM_DEBUG(dbgs() << "Result = " << Result << "\n");
  if (Result != MCDisassembler::Success) {
    MCI->clear();
    MCI->setOpcode(UNKNOWN_vec_dpd);
  }
  Inst.addOperand(MCOperand::createInst(MCI));
  return MCDisassembler::Success;
}

template <typename InsnType>
DecodeStatus decodeVecStrmInstruction(MCInst &Inst, InsnType &SlotEncoding,
                                      int64_t Address,
                                      const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode VecStrm Instruction\n");
  const AIEDisassembler *disassembler = (const AIEDisassembler *)Decoder;
  MCInst *MCI = disassembler->getContext().createMCInst();

  // Slot encoding in the canonical format:
  // -------------------------------------------------
  // |                instr32                |  011  |
  // -------------------------------------------------
  // |           vec_med              | xx10 |   -   |
  // ---------------------------------|---------------
  // | 000 |        ved_dpd           |      --      |
  // ---------------------------------|---------------
  // |  -  | v_strm | 0....0 | 0....0 |      --      |
  // -------------------------------------------------

  SlotEncoding <<= 23;
  SlotEncoding |= (0b0010 << 3) | 0b011;

  DecodeStatus Result =
      decodeInstruction(DecoderTable32, *MCI, SlotEncoding, Address, Decoder,
                        disassembler->getSubtargetInfo());
  LLVM_DEBUG(dbgs() << "Result = " << Result << "\n");
  if (Result != MCDisassembler::Success) {
    MCI->clear();
    MCI->setOpcode(UNKNOWN_vec_dpd);
  }
  Inst.addOperand(MCOperand::createInst(MCI));
  return MCDisassembler::Success;
}

template <typename InsnType>
DecodeStatus decodeVecShrtInstruction(MCInst &Inst, InsnType &SlotEncoding,
                                      int64_t Address,
                                      const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode VecShrt Instruction\n");
  const AIEDisassembler *disassembler = (const AIEDisassembler *)Decoder;
  MCInst *MCI = disassembler->getContext().createMCInst();

  // Slot encoding in the canonical format:
  // -------------------------------------------------
  // |                instr32                |  011  |
  // -------------------------------------------------
  // |           vec_med              | xx10 |   -   |
  // ---------------------------------|---------------
  // | 000 |        ved_dpd           |      --      |
  // ---------------------------------|---------------
  // |  -  | 0....0 | 0....0 | v_shrt |      --      |
  // -------------------------------------------------

  SlotEncoding <<= 7;
  SlotEncoding |= (0b0010 << 3) | 0b011;

  DecodeStatus Result =
      decodeInstruction(DecoderTable32, *MCI, SlotEncoding, Address, Decoder,
                        disassembler->getSubtargetInfo());
  LLVM_DEBUG(dbgs() << "Result = " << Result << "\n");
  if (Result != MCDisassembler::Success) {
    MCI->clear();
    MCI->setOpcode(UNKNOWN_vec_dpd);
  }
  Inst.addOperand(MCOperand::createInst(MCI));
  return MCDisassembler::Success;
}

template <typename InsnType>
DecodeStatus decodeAluInstruction(MCInst &Inst, InsnType &Imm, int64_t Address,
                                  const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Alu Instruction\n");
  const AIEDisassembler *disassembler = (const AIEDisassembler *)Decoder;
  MCInst *MCI = disassembler->getContext().createMCInst();
  Imm <<= 12;
  Imm |= 0x3;
  DecodeStatus Result = decodeInstruction(DecoderTable32, *MCI, Imm, Address, Decoder, disassembler->getSubtargetInfo());
  LLVM_DEBUG(dbgs() << "Result = " << Result << "\n");
  if (Result != MCDisassembler::Success) {
    MCI->clear();
    MCI->setOpcode(UNKNOWN_alu_format);
  }
  Inst.addOperand(MCOperand::createInst(MCI));
  return MCDisassembler::Success;
}

template <typename InsnType>
DecodeStatus decodeStInstruction(MCInst &Inst, InsnType &Imm, int64_t Address,
                                 const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode St Instruction\n");
  const AIEDisassembler *disassembler = (const AIEDisassembler *)Decoder;
  MCInst *MCI = disassembler->getContext().createMCInst();
  Imm <<= 11;
  Imm |= 0x80000003;
  DecodeStatus Result = decodeInstruction(DecoderTable32, *MCI, Imm, Address, Decoder, disassembler->getSubtargetInfo());
  LLVM_DEBUG(dbgs() << "Result = " << Result << "\n");
  if (Result != MCDisassembler::Success) {
    MCI->clear();
    MCI->setOpcode(UNKNOWN_st);
  }
  Inst.addOperand(MCOperand::createInst(MCI));
  return MCDisassembler::Success;
}

// Slot encoding in the canonical format:
// ---------------------------------------------------------
// |                  instr32                      |  011  |
// ---------------------------------------------------------
// |          i32_shrt                 | xxxxxx00  |   -   |
// ---------------------------------------------------------
// | 0 |          i32_lda           01 |         -         |
// ---------------------------------------------------------

template <typename InsnType>
DecodeStatus decodeLdaInstruction(MCInst &Inst, InsnType &Imm, int64_t Address,
                                  const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Lda Instruction: " << Imm << "\n");
  const AIEDisassembler *disassembler = (const AIEDisassembler *)Decoder;
  MCInst *MCI = disassembler->getContext().createMCInst();
  Imm <<= 13;
  Imm |= 0x0803;
  LLVM_DEBUG(dbgs() << "Decode Lda Instruction: " << Imm << "\n");
  DecodeStatus Result = decodeInstruction(DecoderTable32, *MCI, Imm, Address, Decoder, disassembler->getSubtargetInfo());
  LLVM_DEBUG(dbgs() << "Result = " << Result << "\n");
  if (Result != MCDisassembler::Success) {
    MCI->clear();
    MCI->setOpcode(UNKNOWN_i32_lda);
  }
  Inst.addOperand(MCOperand::createInst(MCI));
  return MCDisassembler::Success;
}

// Slot encoding in the canonical format:
// ---------------------------------------------------------
// |                  instr32                      |  011  |
// ---------------------------------------------------------
// |          i32_shrt                 | xxxxxx00  |   -   |
// ---------------------------------------------------------
// | 0 |          mv0_ldb         | 11 |         -         |
// ---------------------------------------------------------
// | - |         ldb            1 |         -              |
// ---------------------------------------------------------

template <typename InsnType>
DecodeStatus decodeLdbInstruction(MCInst &Inst, InsnType &Imm, int64_t Address,
                                  const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Ldb Instruction:" << Imm << "\n");
  const AIEDisassembler *disassembler = (const AIEDisassembler *)Decoder;
  MCInst *MCI = disassembler->getContext().createMCInst();
  Imm <<= 14;
  Imm |= 0x3803;
  DecodeStatus Result = decodeInstruction(DecoderTable32, *MCI, Imm, Address, Decoder, disassembler->getSubtargetInfo());
  LLVM_DEBUG(dbgs() << "Result = " << Result << "\n");
  if (Result != MCDisassembler::Success) {
    MCI->clear();
    MCI->setOpcode(UNKNOWN_ldb);
  }
  Inst.addOperand(MCOperand::createInst(MCI));
  return MCDisassembler::Success;
}

// Slot encoding in the canonical format:
// ---------------------------------------------------------
// |                  instr32                      |  011  |
// ---------------------------------------------------------
// |          i32_shrt                 | xxxxxx00  |   -   |
// ---------------------------------------------------------
// | 0 |          mv0_ldb         | 11 |         -         |
// ---------------------------------------------------------
// | - |         ldb           00 |         -              |
// ---------------------------------------------------------

template <typename InsnType>
DecodeStatus decodeMv0Instruction(MCInst &Inst, InsnType &Imm, int64_t Address,
                                  const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Mv0 Instruction:" << Imm << "\n");
  const AIEDisassembler *disassembler = (const AIEDisassembler *)Decoder;
  MCInst *MCI = disassembler->getContext().createMCInst();
  Imm <<= 15;
  Imm |= 0x1803;
  DecodeStatus Result = decodeInstruction(DecoderTable32, *MCI, Imm, Address, Decoder, disassembler->getSubtargetInfo());
  LLVM_DEBUG(dbgs() << "Result = " << Result << "\n");
  if (Result != MCDisassembler::Success) {
    MCI->clear();
    MCI->setOpcode(UNKNOWN_mv0_ldb);
  }
  Inst.addOperand(MCOperand::createInst(MCI));
  return MCDisassembler::Success;
}

// Slot encoding in the canonical format:
// ---------------------------------------------------------
// |                  instr32                      |  011  |
// ---------------------------------------------------------
// |          i32_shrt                 | xxxxxx00  |   -   |
// ---------------------------------------------------------
// | 0 |          mv1        | xxx1011 |         -         |
// ---------------------------------------------------------
// | - |  mv1_ssb  | x00000  |              -              |
// ---------------------------------------------------------

template <typename InsnType>
DecodeStatus decodeMv1Instruction(MCInst &Inst, InsnType &Imm, int64_t Address,
                                  const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Mv1 Instruction:" << Imm << "\n");
  const AIEDisassembler *disassembler = (const AIEDisassembler *)Decoder;
  MCInst *MCI = disassembler->getContext().createMCInst();
  Imm <<= 18;
  Imm |= (0b1011 << 11) | 0b11;
  DecodeStatus Result = decodeInstruction(DecoderTable32, *MCI, Imm, Address, Decoder, disassembler->getSubtargetInfo());
  LLVM_DEBUG(dbgs() << "Result = " << Result << "\n");
  if (Result != MCDisassembler::Success) {
    MCI->clear();
    MCI->setOpcode(UNKNOWN_mv0_ldb);
  }
  Inst.addOperand(MCOperand::createInst(MCI));
  return MCDisassembler::Success;
}

template <typename InsnType>
DecodeStatus decodeMv1ssbInstruction(MCInst &Inst, InsnType &SlotEncoding,
                                     int64_t Address,
                                     const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Mv1ssb Instruction:" << SlotEncoding << "\n");
  const AIEDisassembler *disassembler = (const AIEDisassembler *)Decoder;
  MCInst *MCI = disassembler->getContext().createMCInst();

  // NOTE: MV1SSB has a size of 7-bit and the canonical format used to represent
  // it is the instr32 format. It is a sub-slot / subset of the MV1 slot.

  // Slot encoding in the canonical format:
  // ---------------------------------------------------------
  // |                  instr32                      |  011  |
  // ---------------------------------------------------------
  // |          i32_shrt                 | xxxxxx00  |   -   |
  // ---------------------------------------------------------
  // | 0 |          mv1        | xxx1011 |         -         |
  // ---------------------------------------------------------
  // | - |  mv1_ssb  | x00000  |              -              |
  // ---------------------------------------------------------

  SlotEncoding <<= 24;
  SlotEncoding |= (0b1011 << 11) | 0b11;

  DecodeStatus Result =
      decodeInstruction(DecoderTable32, *MCI, SlotEncoding, Address, Decoder,
                        disassembler->getSubtargetInfo());
  LLVM_DEBUG(dbgs() << "Result = " << Result << "\n");
  if (Result != MCDisassembler::Success) {
    MCI->clear();
    MCI->setOpcode(UNKNOWN_mv0_ldb);
  }
  Inst.addOperand(MCOperand::createInst(MCI));
  return MCDisassembler::Success;
}

template <typename InsnType>
DecodeStatus decodeVecMedInstruction(MCInst &Inst, InsnType &SlotEncoding,
                                     int64_t Address,
                                     const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode VecMed Instruction:" << SlotEncoding << "\n");
  const AIEDisassembler *disassembler = (const AIEDisassembler *)Decoder;
  MCInst *MCI = disassembler->getContext().createMCInst();

  // NOTE: VECM has a size of 25-bit and the canonical format used to represent
  // it is the instr32 format (and this is why we use the DecoderTable32).

  // Slot encoding in the canonical format:
  // ----------------------------
  // |      instr32     |  011  |
  // ----------------------------
  // |  vec_med  | xx10 |   -   |
  // ----------------------------

  SlotEncoding <<= 7;
  SlotEncoding |= (0b0010 << 3) | 0b011;

  DecodeStatus Result =
      decodeInstruction(DecoderTable32, *MCI, SlotEncoding, Address, Decoder,
                        disassembler->getSubtargetInfo());
  LLVM_DEBUG(dbgs() << "Result = " << Result << "\n");
  if (Result != MCDisassembler::Success) {
    MCI->clear();
    MCI->setOpcode(UNKNOWN_vec_med);
  }
  Inst.addOperand(MCOperand::createInst(MCI));
  return MCDisassembler::Success;
}

template <typename InsnType>
DecodeStatus decodeVecAllInstruction(MCInst &Inst, InsnType &SlotEncoding,
                                     int64_t Address,
                                     const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode VecAll Instruction:" << SlotEncoding << "\n");
  const AIEDisassembler *disassembler = (const AIEDisassembler *)Decoder;
  MCInst *MCI = disassembler->getContext().createMCInst();

  // NOTE: VECA has a size of 37-bit and the canonical format used to represent
  // it is the instr64 format (and this is why we use the DecoderTable64) as it
  // obviously doesn't fit in an instr32 format.

  // Slot encoding in the canonical format:
  // -----------------------------------------
  // |           instr64              | 0111 |
  // -----------------------------------------
  // |           i64_vec         | 00 |  --  |
  // -----------------------------------------
  // |   i32_shrt   |   vec_all  |    --     |
  // -----------------------------------------
  // with i32_shrt = 0 (see AIEInstrFormats.td)

  SlotEncoding <<= 6;
  SlotEncoding |= 0b111;

  // NOTE: the Slot Encoding has been zero extended, hence the upper bits are
  // set to 0.
  // Note that we don't directly use the regular 64-bit table here, because that
  // table parses composite instructions, which would end up calling this
  // function again. Instead we skip directly to decoding the veca_slot only.
  DecodeStatus Result =
      decodeInstruction(DecoderTableveca_slot64, *MCI, SlotEncoding, Address,
                        Decoder, disassembler->getSubtargetInfo());
  LLVM_DEBUG(dbgs() << "Result = " << Result << "\n");
  if (Result != MCDisassembler::Success) {
    MCI->clear();
    MCI->setOpcode(UNKNOWN_vec_all);
  }
  Inst.addOperand(MCOperand::createInst(MCI));
  return MCDisassembler::Success;
}

DecodeStatus AIEDisassembler::getInstruction(MCInst &MI, uint64_t &Size,
                                               ArrayRef<uint8_t> Bytes,
                                               uint64_t Address,
                                               raw_ostream &CS) const {
  DecodeStatus Result;

  if ((Bytes[0] & 0x3) == 0x1) {
      // It's a 16 bit instruction if it starts with 0b01
      Size = 2;
  } else if ((Bytes[0] & 0x7) == 0x3) {
      // It's a 32 bit instruction if it starts with 0b011
      Size = 4;
  } else if ((Bytes[0] & 0xF) == 0x7) {
      // It's a 64 bit instruction if it starts with 0b0111
      Size = 8;
  } else if ((Bytes[0] & 0xF) == 0xF) {
      // It's a 96 bit instruction if it starts with 0b1111
      Size = 12;
  } else if ((Bytes[0] & 0x1) == 0x0) {
      // It's a 128 bit instruction if it starts with 0b0
      Size = 16;
  } else {
      dbgs() << format("0x%08x", Bytes[0]) << "\n";
      report_fatal_error("Unknown Instruction in Disassembler");
  }
  if (Bytes.size() < Size) {
      LLVM_DEBUG(dbgs() << "Disassembler failed with buffer overrun: "
                 << Bytes.size() << "<" << Size << "\n");
      Size = 0;
      return MCDisassembler::Fail;
  }

  // Calling the auto-generated decoder function.
  if (Size == 2) {
    APInt Insn(16, support::endian::read16le(Bytes.data()));
    LLVM_DEBUG(dbgs() << "Trying AIE16 table :\n");
    Result = decodeInstruction(DecoderTable16, MI, Insn, Address, this, STI);
    if (Result == MCDisassembler::Fail) {
      LLVM_DEBUG(dbgs() << "Trying Fallback16 table :\n");
      Result = decodeInstruction(DecoderTableFallback16, MI, Insn, Address,
                                 this, STI);
    }
  } else if (Size == 4) {
    APInt Insn(32, support::endian::read32le(Bytes.data()));
    LLVM_DEBUG(dbgs() << "Trying AIE32 table :\n");
    Result = decodeInstruction(DecoderTable32, MI, Insn, Address, this, STI);
    if (Result == MCDisassembler::Fail) {
      LLVM_DEBUG(dbgs() << "Trying Fallback32 table :\n");
      Result = decodeInstruction(DecoderTableFallback32, MI, Insn, Address,
                                 this, STI);
    }
  } else if (Size == 8) {
    APInt Insn(64, support::endian::read64le(Bytes.data()));
    LLVM_DEBUG(dbgs() << "Trying AIE64 table :\n");
    Result = decodeInstruction(DecoderTable64, MI, Insn, Address, this, STI);
    if (Result == MCDisassembler::Fail) {
      LLVM_DEBUG(dbgs() << "Trying Fallback64 table :\n");
      Result = decodeInstruction(DecoderTableFallback64, MI, Insn, Address,
                                 this, STI);
    }
  } else if (Size == 12) {
    // Note that there are some nasty bits in the instruction decode
    // tables generated by tablegen if decoding more than 64 bits at
    // once.  See: https://reviews.llvm.org/D52100
    APInt Insn(96, support::endian::read32le(Bytes.data() + 8));
    Insn = Insn.shl(64);
    Insn |= support::endian::read64le(Bytes.data());
    LLVM_DEBUG(dbgs() << "Trying AIE96 table :\n");
    Result = decodeInstruction(DecoderTable96, MI, Insn, Address, this, STI);
    if (Result == MCDisassembler::Fail) {
      LLVM_DEBUG(dbgs() << "Trying Fallback96 table :\n");
      Result = decodeInstruction(DecoderTableFallback96, MI, Insn, Address,
                                 this, STI);
    }
  } else if (Size == 16) {
    APInt Insn(128, support::endian::read64le(Bytes.data() + 8));
    Insn = Insn.shl(64);
    Insn |= support::endian::read64le(Bytes.data());
    LLVM_DEBUG(dbgs() << "Trying AIE128 table :\n");
    Result = decodeInstruction(DecoderTable128, MI, Insn, Address, this, STI);
    if (Result == MCDisassembler::Fail) {
      LLVM_DEBUG(dbgs() << "Trying Fallback128 table :\n");
      Result = decodeInstruction(DecoderTableFallback128, MI, Insn, Address,
                                 this, STI);
    }
  } else {
    assert(false);
  }
  return Result;
}
