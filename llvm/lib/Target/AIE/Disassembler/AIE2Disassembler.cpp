//===-- AIE2Disassembler.cpp - Disassembler for AIE --------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIE2Disassembler class.
//
//===----------------------------------------------------------------------===//

#include "AIE2RegisterInfo.h"
#include "AIEBaseDisassembler.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "llvm/MC/MCDecoderOps.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Format.h"

using namespace llvm;

#define DEBUG_TYPE "aie2-disassembler"

using namespace AIE2;

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {
class AIE2Disassembler : public MCDisassembler {

public:
  AIE2Disassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
      : MCDisassembler(STI, Ctx) {}

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &CStream) const override;
};
} // end anonymous namespace

MCDisassembler *createAIE2Disassembler(const Target &T,
                                       const MCSubtargetInfo &STI,
                                       MCContext &Ctx) {
  return new AIE2Disassembler(STI, Ctx);
}

static const unsigned eRDecoderTable[] = {
    r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,  r8,  r9,  r10,
    r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21,
    r22, r23, r24, r25, r26, r27, r28, r29, r30, r31};

static const unsigned eRS4DecoderTable[] = {r16, r17, r18, r19};

static const unsigned eRS8DecoderTable[] = {r16, r17, r18, r19,
                                            r20, r21, r22, r23};

static const unsigned eLDecoderTable[] = {l0, l1, l2, l3, l4, l5, l6, l7};

static const unsigned ePDecoderTable[] = {
    p0, p1, p2, p3, p4, p5, p6, p7,
};
static const unsigned eMDecoderTable[] = {
    m0, m1, m2, m3, m4, m5, m6, m7,
};
static const unsigned eDJDecoderTable[] = {
    dj0, dj1, dj2, dj3, dj4, dj5, dj6, dj7,
};
static const unsigned eDDecoderTable[] = {
    d0, d1, d2, d3, d4, d5, d6, d7,
};
static const unsigned eDSDecoderTable[] = {d0_3d, d1_3d, d2_3d, d3_3d};

static const unsigned mCRmDecoderTable[] = {
    crVaddSign, crF2FMask, crF2IMask, crFPMask, crMCDEn,   crPackSign,
    crRnd,      crSCDEn,   crSRSSign, crSat,    crUPSSign, crUnpackSign};

static const unsigned mLdaSclDecoderTable[] = {
    r0,  0, m0,  0, r1,  lr, m1,  0, r2,  0,  m2,  0, r3,  p0, m3,  0,
    r4,  0, m4,  0, r5,  0,  m5,  0, r6,  0,  m6,  0, r7,  p1, m7,  0,
    r8,  0, dn0, 0, r9,  0,  dn1, 0, r10, 0,  dn2, 0, r11, p2, dn3, 0,
    r12, 0, dn4, 0, r13, 0,  dn5, 0, r14, 0,  dn6, 0, r15, p3, dn7, 0,
    r16, 0, dj0, 0, r17, 0,  dj1, 0, r18, 0,  dj2, 0, r19, p4, dj3, 0,
    r20, 0, dj4, 0, r21, 0,  dj5, 0, r22, p5, dj6, 0, r23, p5, dj7, 0,
    r24, 0, dc0, 0, r25, 0,  dc1, 0, r26, 0,  dc2, 0, r27, p6, dc3, 0,
    r28, 0, dc4, 0, r29, 0,  dc5, 0, r30, 0,  dc6, 0, r31, p7, dc7, 0};

static const unsigned mLdaCgDecoderTable[] = {
    r0,  0, m0,  0, r1,  0,  m1,  0, r2,  0,  m2,  0, r3,  p0, m3,  0,
    r4,  0, m4,  0, r5,  LC, m5,  0, r6,  0,  m6,  0, r7,  p1, m7,  0,
    r8,  0, dn0, 0, r9,  0,  dn1, 0, r10, 0,  dn2, 0, r11, p2, dn3, 0,
    r12, 0, dn4, 0, r13, 0,  dn5, 0, r14, 0,  dn6, 0, r15, p3, dn7, 0,
    r16, 0, dj0, 0, r17, 0,  dj1, 0, r18, 0,  dj2, 0, r19, p4, dj3, 0,
    r20, 0, dj4, 0, r21, 0,  dj5, 0, r22, p5, dj6, 0, r23, p5, dj7, 0,
    r24, 0, dc0, 0, r25, 0,  dc1, 0, r26, 0,  dc2, 0, r27, p6, dc3, 0,
    r28, 0, dc4, 0, r29, 0,  dc5, 0, r30, 0,  dc6, 0, r31, p7, dc7, 0};

static const unsigned mMvSclDstDecoderTable[] = {
    r0,  crVaddSign, m0,  p0,          r1,  srCarry,
    m1,  LS,         r2,  crF2FMask,   m2,  s0,
    r3,  srCompr_uf, m3,  0,           r4,  crF2IMask,
    m4,  p1,         r5,  srF2FFlags,  m5,  DP,
    r6,  crFPMask,   m6,  0,           r7,  srF2IFlags,
    m7,  0,          r8,  crMCDEn,     dn0, p2,
    r9,  srFPFlags,  dn1, lr,          r10, crPackSign,
    dn2, s1,         r11, srMS0,       dn3, 0,
    r12, crRnd,      dn4, p3,          r13, srSRS_of,
    dn5, CORE_ID,    r14, crSCDEn,     dn6, 0,
    r15, srSS0,      dn7, 0,           r16, crSRSSign,
    dj0, p4,         r17, srSparse_of, dj1, LE,
    r18, crSat,      dj2, s2,          r19, srUPS_of,
    dj3, 0,          r20, crUPSSign,   dj4, p5,
    r21, 0,          dj5, LC,          r22, crUnpackSign,
    dj6, 0,          r23, 0,           dj7, 0,
    r24, 0,          dc0, p6,          r25, 0,
    dc1, SP,         r26, 0,           dc2, s3,
    r27, 0,          dc3, 0,           r28, 0,
    dc4, p7,         r29, 0,           dc5, 0,
    r30, 0,          dc6, 0,           r31, 0,
    dc7, 0};

static const unsigned mWaDecoderTable[] = {
    wl0, wh0, wl1, wh1, wl2, wh2, wl3, wh3, wl4,  wh4,  wl5,  wh5,
    wl6, wh6, wl7, wh7, wl8, wh8, wl9, wh9, wl10, wh10, wl11, wh11,
};

static const unsigned mWm_1DecoderTable[] = {
    wl0, wl2, wl4, wl6, wl8, wl10, 0, 0, wl1, wl3, wl5, wl7, wl9, wl11, 0, 0,
    wh0, wh2, wh4, wh6, wh8, wh10, 0, 0, wh1, wh3, wh5, wh7, wh9, wh11, 0, 0};

static const unsigned mXvDecoderTable[] = {x0, x1, x2, x3, x4,  x5,
                                           x6, x7, x8, x9, x10, x11};

static const unsigned mAMmDecoderTable[] = {
    amll0, amlh0, amhl0, amhh0, amll1, amlh1, amhl1, amhh1, amll2,
    amlh2, amhl2, amhh2, amll3, amlh3, amhl3, amhh3, amll4, amlh4,
    amhl4, amhh4, amll5, amlh5, amhl5, amhh5, amll6, amlh6, amhl6,
    amhh6, amll7, amlh7, amhl7, amhh7, amll8, amlh8, amhl8, amhh8,
};

static const unsigned mBMmDecoderTable[] = {
    bml0, bmh0, bml1, bmh1, bml2, bmh2, bml3, bmh3, bml4,
    bmh4, bml5, bmh5, bml6, bmh6, bml7, bmh7, bml8, bmh8,
};

static const unsigned mBMSmDecoderTable[] = {
    bml0, bml1, bml2, bml3, bml4, bml5, bml6, bml7,
    bmh0, bmh1, bmh2, bmh3, bmh4, bmh5, bmh6, bmh7,
};

static const unsigned eCMDecoderTable[] = {cm0, cm1, cm2, cm3, cm4,
                                           cm5, cm6, cm7, cm8};

static const unsigned mQQmDecoderTable[] = {q0, q1, q2, q3};

static const unsigned mSsDecoderTable[] = {s0, s1, s2, s3};

static const unsigned mQQXwDecoderTable[] = {qx0, qx1, qx2, qx3};

static const unsigned eYsDecoderTable[] = {y2, y3, y4, y5};

static const unsigned mQXHLbDecoderTable[] = {
    qwh0, qwl0, qwh1, qwl1, qwh2, qwl2, qwh3, qwl3,
};

#define TABLEBASEDDECODER2(ClassName, TableClassName)                          \
  static DecodeStatus Decode##ClassName##RegisterClass(                        \
      MCInst &Inst, uint64_t RegNo, uint64_t Address,                          \
      const MCDisassembler *Decoder) {                                         \
    if (RegNo >= std::size(TableClassName##DecoderTable))                           \
      return MCDisassembler::Fail;                                             \
    unsigned Reg = TableClassName##DecoderTable[RegNo];                        \
    LLVM_DEBUG(dbgs() << "RegEnc=" << RegNo << " Reg=" << Reg << "\n");        \
    assert(Reg && "Register not found!");                                      \
    Inst.addOperand(MCOperand::createReg(Reg));                                \
    return MCDisassembler::Success;                                            \
  }

#define TABLEBASEDDECODER(ClassName) TABLEBASEDDECODER2(ClassName, ClassName)

#define FIXEDREGDECODER(ClassName)                                             \
  template <typename InsnType>                                                 \
  static DecodeStatus Decode##ClassName##RegisterClass(                        \
      MCInst &Inst, InsnType Insn, uint64_t Address,                           \
      const MCDisassembler *Decoder) {                                         \
    return Decoder->decodeSingletonRegClass(Inst, ClassName##RegClass);        \
  }

TABLEBASEDDECODER(eR)
TABLEBASEDDECODER(eRS4)
TABLEBASEDDECODER(eRS8)
TABLEBASEDDECODER(eL)
TABLEBASEDDECODER(eP)
TABLEBASEDDECODER(eM)
TABLEBASEDDECODER(eDJ)
TABLEBASEDDECODER(mLdaScl)
TABLEBASEDDECODER2(mSclSt, mLdaScl)
TABLEBASEDDECODER2(mSclMS, mLdaScl)
TABLEBASEDDECODER2(mMvSclDstCg, mMvSclDst)
TABLEBASEDDECODER(mLdaCg)
TABLEBASEDDECODER(mMvSclDst)
TABLEBASEDDECODER2(mMvSclSrc, mMvSclDst)
TABLEBASEDDECODER(mWa)
TABLEBASEDDECODER2(mWs, mWa)
TABLEBASEDDECODER2(mWb, mWa)
TABLEBASEDDECODER2(mWm, mWa)
TABLEBASEDDECODER(mWm_1)
TABLEBASEDDECODER(mXv)
TABLEBASEDDECODER2(mXw, mXv)
TABLEBASEDDECODER2(mXm, mXv)
TABLEBASEDDECODER2(mXn, mXv)
TABLEBASEDDECODER2(mXa, mXv)
TABLEBASEDDECODER2(mXs, mXv)
TABLEBASEDDECODER(mAMm)
TABLEBASEDDECODER2(mAMs, mAMm)
TABLEBASEDDECODER(mBMm)
TABLEBASEDDECODER2(mBMs, mBMm)
TABLEBASEDDECODER2(mBMa, mBMm)
TABLEBASEDDECODER(mBMSm)
TABLEBASEDDECODER(eCM)
TABLEBASEDDECODER2(mCMm, eCM)
TABLEBASEDDECODER2(mCMs, eCM)
TABLEBASEDDECODER2(mCMa, eCM)
TABLEBASEDDECODER(mCRm)
TABLEBASEDDECODER(mQXHLb)
TABLEBASEDDECODER(mQQm)
TABLEBASEDDECODER2(mQQa, mQQm)
TABLEBASEDDECODER(mQQXw)
TABLEBASEDDECODER(eYs)
TABLEBASEDDECODER(mSs)
TABLEBASEDDECODER2(mSm, mSs)
FIXEDREGDECODER(eR26)
FIXEDREGDECODER(eR27)
FIXEDREGDECODER(eR28)
FIXEDREGDECODER(eR29)
FIXEDREGDECODER(eR31)

static DecodeStatus DecodemAluCgRegisterClass(MCInst &Inst, uint64_t RegNo,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder) {
  if ((RegNo & 0b1) == 0b0 &&
      DecodeeRRegisterClass(Inst, RegNo >> 1, Address, Decoder))
    return DecodeStatus::Success;
  else {
    Inst.addOperand(MCOperand::createReg(LC));
    return DecodeStatus::Success;
  }
  return MCDisassembler::Fail;
}

static DecodeStatus
DecodemMvAMWQSrcRegisterClass(MCInst &Inst, uint64_t RegNo, uint64_t Address,
                              const MCDisassembler *Decoder) {
  if ((RegNo & 0b101) == 0b001 &&
      DecodemAMmRegisterClass(Inst, RegNo >> 3, Address, Decoder))
    return DecodeStatus::Success;
  if ((RegNo >> 6) == 0b110 &&
      DecodemWm_1RegisterClass(Inst, RegNo & 0b11111, Address, Decoder))
    return DecodeStatus::Success;
  if ((RegNo >> 6) == 0b111 &&
      DecodemQQmRegisterClass(Inst, (RegNo >> 4) & 0b11, Address, Decoder))
    return DecodeStatus::Success;
  return MCDisassembler::Fail;
}

static DecodeStatus
DecodemMvAMWQDstRegisterClass(MCInst &Inst, uint64_t RegNo, uint64_t Address,
                              const MCDisassembler *Decoder) {
  if ((RegNo & 0b1) == 0b1 &&
      DecodemAMmRegisterClass(Inst, RegNo >> 1, Address, Decoder))
    return DecodeStatus::Success;
  if ((RegNo & 0b11) == 0b00 &&
      DecodemWm_1RegisterClass(Inst, RegNo >> 2, Address, Decoder))
    return DecodeStatus::Success;
  if ((RegNo & 0b11) == 0b10 &&
      DecodemQQmRegisterClass(Inst, RegNo >> 5, Address, Decoder))
    return DecodeStatus::Success;
  return MCDisassembler::Fail;
}

static DecodeStatus
DecodemMvBMXSrcRegisterClass(MCInst &Inst, uint64_t RegNo, uint64_t Address,
                             const MCDisassembler *Decoder) {
  if (DecodemBMmRegisterClass(Inst, RegNo >> 4, Address, Decoder) ==
      DecodeStatus::Success)
    return DecodeStatus::Success;
  if ((RegNo >> 7) == 0b11 &&
      DecodemXmRegisterClass(Inst, RegNo & 0b1111, Address, Decoder) ==
          DecodeStatus::Success)
    return DecodeStatus::Success;
  return MCDisassembler::Fail;
}

static DecodeStatus
DecodemMvBMXDstRegisterClass(MCInst &Inst, uint64_t RegNo, uint64_t Address,
                             const MCDisassembler *Decoder) {
  if ((RegNo & 0b1) == 0b1 &&
      DecodemBMmRegisterClass(Inst, RegNo >> 1, Address, Decoder) ==
          DecodeStatus::Success)
    return DecodeStatus::Success;
  if ((RegNo & 0b1) == 0b0 &&
      DecodemXmRegisterClass(Inst, RegNo >> 2, Address, Decoder) ==
          DecodeStatus::Success)
    return DecodeStatus::Success;
  return MCDisassembler::Fail;
}

static DecodeStatus DecodemShflDstRegisterClass(MCInst &Inst, uint64_t RegNo,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder) {
  if ((RegNo & 0b1) == 0b1) {
    RegNo = RegNo >> 1;
    if (DecodemBMSmRegisterClass(Inst, RegNo, Address, Decoder) ==
        DecodeStatus::Success) {
      return DecodeStatus::Success;
    }
  }
  if ((RegNo & 0b1) == 0b0 &&
      DecodemXmRegisterClass(Inst, RegNo >> 1, Address, Decoder) ==
          DecodeStatus::Success)
    return DecodeStatus::Success;
  return MCDisassembler::Fail;
}

#define SLOTDECODERDecl(ClassName)                                             \
  template <typename InsnType>                                                 \
  static DecodeStatus decode##ClassName##Instruction(                          \
      MCInst &MI, InsnType &Insn, int64_t Address,                             \
      const MCDisassembler *Decoder)

SLOTDECODERDecl(Lda);
SLOTDECODERDecl(Ldb);
SLOTDECODERDecl(Alu);
SLOTDECODERDecl(St);
SLOTDECODERDecl(Mv);
SLOTDECODERDecl(Vec);
SLOTDECODERDecl(Lng);
SLOTDECODERDecl(Nop);

template <typename InsnType>
static DecodeStatus DecodeLDA_2D_Instruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_2D_HBInstruction(MCInst &MI, InsnType &insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_3D_Instruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_3D_HBInstruction(MCInst &MI, InsnType &insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_2D_Instruction(MCInst &MI, InsnType &insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_2D_HBInstruction(MCInst &MI, InsnType &insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_3D_HBInstruction(MCInst &MI, InsnType &insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_3D_Instruction(MCInst &MI, InsnType &insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodePADD_2DInstruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodePADD_3DInstruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_W_2DInstruction(MCInst &MI, InsnType &insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_3D_WInstruction(MCInst &MI, InsnType &insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_PACK_2DInstruction(MCInst &MI, InsnType &insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_PACK_3DInstruction(MCInst &MI, InsnType &insn,
                                                 uint64_t Address,
                                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_Q_2DInstruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeST_Q_3DInstruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_AM_2DInstruction(MCInst &MI, InsnType &insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVST_3D_AMInstruction(MCInst &MI, InsnType &insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_2D_WInstruction(MCInst &MI, InsnType &insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_3D_WInstruction(MCInst &MI, InsnType &insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_2D_AMInstruction(MCInst &MI, InsnType &insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDA_3D_AMInstruction(MCInst &MI, InsnType &insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_2DInstruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeVLDB_3DInstruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder);

template <typename InsnType>
static DecodeStatus
DecodeVLDB_UNPACK_2DInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDB_UNPACK_3DInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder);

template <typename InsnType>
static DecodeStatus
DecodeVST_2D_SRS_CMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVST_3D_SRS_CMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVST_2D_SRS_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVST_3D_SRS_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                               const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVST_2D_CONV_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVST_3D_CONV_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_2D_UPS_CMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_3D_UPS_CMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_2D_UPS_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_3D_UPS_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_2D_CONV_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus
DecodeVLDA_3D_CONV_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                 const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_Q_2DInstruction(MCInst &MI, InsnType &insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder);
template <typename InsnType>
static DecodeStatus DecodeLDA_Q_3DInstruction(MCInst &MI, InsnType &insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder);

#include "AIE2GenDisassemblerTables.inc"

template <typename InsnType>
DecodeStatus decodeSlot(const uint8_t *DecoderTable, MCInst &Inst,
                        InsnType &Imm, int64_t Address,
                        const MCDisassembler *Decoder) {
  MCInst *MCI = Decoder->getContext().createMCInst();
  DecodeStatus Result = decodeInstruction(DecoderTable, *MCI, Imm, Address,
                                          Decoder, Decoder->getSubtargetInfo());
  if (Result != MCDisassembler::Success) {
    MCI->clear();
  }
  Inst.addOperand(MCOperand::createInst(MCI));
  return MCDisassembler::Success;
}

// Slots:
// ------------------------------------------
// |            instr16              | 0001 |
// ------------------------------------------

// ------------------------------------------
// |xxxxxxxxxxxxxxxx                 | 0001 |
// ------------------------------------------
template <typename InsnType>
DecodeStatus decodeNopInstruction(MCInst &Inst, InsnType &Imm, int64_t Address,
                                  const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Nop Instruction\n");
  Imm <<= 4;
  Imm |= 0b0001;
  return decodeSlot(DecoderTable16, Inst, Imm, Address, Decoder);
}

// Slots:
// -----------------------------------------
// |            instr32             | 1001 |
// -----------------------------------------
// | x0000 |        lda        | x1 |      |
// -----------------------------------------
// | xx111 |     ldb      | xxxxxx1 |      |
// -----------------------------------------
// | x0010 |        alu       | xx1 |      |
// -----------------------------------------
// | x0011 |         mv         | 1 |      |
// -----------------------------------------
// | x0001 |         st        | x1 |      |
// -----------------------------------------
// |           vec             | x0 |      |
// -----------------------------------------

// -----------------------------------------
// | x0000 |        lda        | x1 | 1001 |
// -----------------------------------------
template <typename InsnType>
DecodeStatus decodeLdaInstruction(MCInst &Inst, InsnType &Imm, int64_t Address,
                                  const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Lda Instruction\n");
  Imm <<= 6;
  Imm |= (0b00000 << 27) | 0b010000;
  return decodeSlot(DecoderTable32, Inst, Imm, Address, Decoder);
}

// -----------------------------------------
// | xx111 |     ldb      | xxxxxx1 | 1001 |
// -----------------------------------------
template <typename InsnType>
DecodeStatus decodeLdbInstruction(MCInst &Inst, InsnType &Imm, int64_t Address,
                                  const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Ldb Instruction\n");
  Imm <<= 11;
  Imm |= (0b00111 << 27) | 0b00000010000;
  return decodeSlot(DecoderTable32, Inst, Imm, Address, Decoder);
}

// -----------------------------------------
// | x0010 |        alu       | xx1 | 1001 |
// -----------------------------------------
template <typename InsnType>
DecodeStatus decodeAluInstruction(MCInst &Inst, InsnType &Imm, int64_t Address,
                                  const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Alu Instruction\n");
  Imm <<= 7;
  Imm |= (0b00010 << 27) | 0b0010000;
  return decodeSlot(DecoderTable32, Inst, Imm, Address, Decoder);
}

// -----------------------------------------
// | x0011 |         mv         | 1 | 1001 |
// -----------------------------------------
template <typename InsnType>
DecodeStatus decodeMvInstruction(MCInst &Inst, InsnType &Imm, int64_t Address,
                                 const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Mv Instruction\n");
  Imm <<= 5;
  Imm |= (0b00011 << 27) | 0b10000;
  return decodeSlot(DecoderTable32, Inst, Imm, Address, Decoder);
}

// -----------------------------------------
// | x0001 |         st        | x1 | 1001 |
// -----------------------------------------
template <typename InsnType>
DecodeStatus decodeStInstruction(MCInst &Inst, InsnType &Imm, int64_t Address,
                                 const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode St Instruction\n");
  Imm <<= 6;
  Imm |= (0b00001 << 27) | 0b10000;
  return decodeSlot(DecoderTable32, Inst, Imm, Address, Decoder);
}

// -----------------------------------------
// |           vec             | x0 | 1001 |
// -----------------------------------------
template <typename InsnType>
DecodeStatus decodeVecInstruction(MCInst &Inst, InsnType &Imm, int64_t Address,
                                  const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Vec Instruction\n");
  Imm <<= 6;
  Imm |= 0b000000;
  return decodeSlot(DecoderTable32, Inst, Imm, Address, Decoder);
}

// Slots:
// -----------------------------------------
// |            instr48              | 101 |
// -----------------------------------------
// |              lng          | 010 |     |
// -----------------------------------------
template <typename InsnType>
DecodeStatus decodeLngInstruction(MCInst &Inst, InsnType &Imm, int64_t Address,
                                  const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Lng Instruction\n");
  Imm <<= 6;
  Imm |= 0b010101;
  return decodeSlot(DecoderTable48, Inst, Imm, Address, Decoder);
}

namespace {
class BuildNDim {
  MCInst &MI;
  Register PtrReg;
  Register ModReg;
  int NDim;

public:
  BuildNDim(MCInst &MI, Register PtrReg, Register ModReg, int NDim)
      : MI(MI), PtrReg(PtrReg), ModReg(ModReg), NDim(NDim) {}

  BuildNDim &addReg(Register Reg) {
    MI.addOperand(MCOperand::createReg(Reg));
    return *this;
  }
  BuildNDim &addInputs() {
    addReg(PtrReg);
    addReg(ModReg);
    return *this;
  }
  BuildNDim &addOutputs(const MCDisassembler *Decoder) {
    const MCRegisterInfo &MRI = *Decoder->getContext().getRegisterInfo();
    addReg(PtrReg);
    if (NDim == 2) {
      addReg(MRI.getSubReg(ModReg, AIE2::sub_dim_count));
    } else {
      assert(NDim == 3);
      addReg(MRI.getSubReg(ModReg, AIE2::sub_dim_count));
      addReg(MRI.getSubReg(ModReg, AIE2::sub_hi_dim_then_sub_dim_count));
    }
    return *this;
  }
};

BuildNDim build2D(MCInst &MI, unsigned PtrRegRaw, unsigned ModRegRaw) {
  assert(PtrRegRaw < std::size(ePDecoderTable));
  assert(ModRegRaw < std::size(eDDecoderTable));
  Register PtrReg = ePDecoderTable[PtrRegRaw];
  Register ModReg = eDDecoderTable[ModRegRaw];
  assert(PtrReg && ModReg);

  return {MI, PtrReg, ModReg, 2};
}

BuildNDim build3D(MCInst &MI, unsigned PtrRegRaw, unsigned ModRegRaw) {
  assert(PtrRegRaw < std::size(ePDecoderTable));
  assert(ModRegRaw < std::size(eDSDecoderTable));
  Register PtrReg = ePDecoderTable[PtrRegRaw];
  Register ModReg = eDSDecoderTable[ModRegRaw];
  assert(PtrReg && ModReg);

  return {MI, PtrReg, ModReg, 3};
}
} // namespace

template <typename InsnType>
static DecodeStatus DecodeST_2D_Instruction(MCInst &MI, InsnType &insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  unsigned SrcRegRaw = fieldFromInstruction(insn, 7, 7);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);

  if (SrcRegRaw >= std::size(mLdaSclDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = mLdaSclDecoderTable[SrcRegRaw];
  assert(SrcReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addReg(SrcReg)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_2D_HBInstruction(MCInst &MI, InsnType &insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder) {
  unsigned SrcRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (SrcRegRaw >= std::size(eRDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = eRDecoderTable[SrcRegRaw];

  assert(SrcReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addReg(SrcReg)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_3D_Instruction(MCInst &MI, InsnType &insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  unsigned SrcRegRaw = fieldFromInstruction(insn, 7, 7);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);

  if (SrcRegRaw >= std::size(mLdaSclDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = mLdaSclDecoderTable[SrcRegRaw];
  assert(SrcReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addReg(SrcReg)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_3D_HBInstruction(MCInst &MI, InsnType &insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder) {
  unsigned SrcRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (SrcRegRaw >= std::size(eRDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = eRDecoderTable[SrcRegRaw];

  assert(SrcReg && "Register not found.");
  build3D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addReg(SrcReg)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_2D_Instruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 7, 7);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (DstRegRaw >= std::size(mLdaSclDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = mLdaSclDecoderTable[DstRegRaw];
  assert(DstReg && "Register not found.");
  build2D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_2D_HBInstruction(MCInst &MI, InsnType &insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (DstRegRaw >= std::size(eRDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = eRDecoderTable[DstRegRaw];
  assert(DstReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_3D_Instruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  unsigned DstRegRaw = fieldFromInstruction(insn, 7, 7);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (DstRegRaw >= std::size(mLdaSclDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = mLdaSclDecoderTable[DstRegRaw];
  assert(DstReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_3D_HBInstruction(MCInst &MI, InsnType &insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  unsigned DstRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (DstRegRaw >= std::size(eRDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = eRDecoderTable[DstRegRaw];

  assert(DstReg && "Register not found.");
  build3D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodePADD_2DInstruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {

  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);

  build2D(MI, PtrRegRaw, ModRegRaw).addOutputs(Decoder).addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodePADD_3DInstruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {

  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);

  build3D(MI, PtrRegRaw, ModRegRaw).addOutputs(Decoder).addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_W_2DInstruction(MCInst &MI, InsnType &insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder) {

  unsigned SrcRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (SrcRegRaw >= std::size(mWaDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = mWaDecoderTable[SrcRegRaw]; // mWs == mWa
  assert(SrcReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addReg(SrcReg)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_3D_WInstruction(MCInst &MI, InsnType &insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder) {

  unsigned SrcRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (SrcRegRaw >= std::size(mWaDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = mWaDecoderTable[SrcRegRaw]; // mWs == mWa
  assert(SrcReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addReg(SrcReg)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_PACK_2DInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                             const MCDisassembler *Decoder) {

  unsigned SrcRegRaw = fieldFromInstruction(insn, 10, 4);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);

  if (SrcRegRaw >= std::size(mXvDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = mXvDecoderTable[SrcRegRaw];
  assert(SrcReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addInputs()
      .addReg(SrcReg);

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_PACK_3DInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                             const MCDisassembler *Decoder) {

  unsigned SrcRegRaw = fieldFromInstruction(insn, 10, 4);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (SrcRegRaw >= std::size(mXvDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = mXvDecoderTable[SrcRegRaw];
  assert(SrcReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addInputs()
      .addReg(SrcReg);

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_Q_2DInstruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {

  unsigned SrcRegRaw = fieldFromInstruction(insn, 12, 2);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (SrcRegRaw >= std::size(mQQmDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = mQQmDecoderTable[SrcRegRaw];
  assert(SrcReg && "Register not found.");
  build2D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addReg(SrcReg)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_Q_3DInstruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {

  unsigned SrcRegRaw = fieldFromInstruction(insn, 12, 2);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (SrcRegRaw >= std::size(mQQmDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = mQQmDecoderTable[SrcRegRaw];
  assert(SrcReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addReg(SrcReg)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_Q_2DInstruction(MCInst &MI, InsnType &insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 12, 2);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (DstRegRaw >= std::size(mQQmDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = mQQmDecoderTable[DstRegRaw];
  assert(DstReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_Q_3DInstruction(MCInst &MI, InsnType &insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 12, 2);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (DstRegRaw >= std::size(mQQmDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = mQQmDecoderTable[DstRegRaw];
  assert(DstReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_AM_2DInstruction(MCInst &MI, InsnType &insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {

  unsigned SrcRegRaw = fieldFromInstruction(insn, 8, 6);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (SrcRegRaw >= std::size(mAMmDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = mAMmDecoderTable[SrcRegRaw];
  assert(SrcReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addReg(SrcReg)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_3D_AMInstruction(MCInst &MI, InsnType &insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {

  unsigned SrcRegRaw = fieldFromInstruction(insn, 8, 6);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (SrcRegRaw >= std::size(mAMmDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = mAMmDecoderTable[SrcRegRaw];
  assert(SrcReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addReg(SrcReg)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_2D_WInstruction(MCInst &MI, InsnType &insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (DstRegRaw >= std::size(mWaDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = mWaDecoderTable[DstRegRaw];
  assert(DstReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_3D_WInstruction(MCInst &MI, InsnType &insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (DstRegRaw >= std::size(mWaDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = mWaDecoderTable[DstRegRaw];
  assert(DstReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_2D_AMInstruction(MCInst &MI, InsnType &insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 8, 6);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (DstRegRaw >= std::size(mAMmDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = mAMmDecoderTable[DstRegRaw];
  assert(DstReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_3D_AMInstruction(MCInst &MI, InsnType &insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 8, 6);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (DstRegRaw >= std::size(mAMmDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = mAMmDecoderTable[DstRegRaw];
  assert(DstReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDB_2DInstruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 13, 5);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (DstRegRaw >= std::size(mWaDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = mWaDecoderTable[DstRegRaw];
  assert(DstReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDB_3DInstruction(MCInst &MI, InsnType &insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 13, 5);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (DstRegRaw >= std::size(mWaDecoderTable)) // mWa == mWb
    return MCDisassembler::Fail;

  Register DstReg = mWaDecoderTable[DstRegRaw];
  assert(DstReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}
template <typename InsnType>
static DecodeStatus
DecodeVLDB_UNPACK_3DInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder) {
  unsigned DstRegRaw = fieldFromInstruction(insn, 14, 4);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (DstRegRaw >= std::size(mXvDecoderTable)) // mXv == mXs
    return MCDisassembler::Fail;

  Register DstReg = mXvDecoderTable[DstRegRaw];
  assert(DstReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_UNPACK_2DInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder) {
  unsigned DstRegRaw = fieldFromInstruction(insn, 14, 4);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (DstRegRaw >= std::size(mXvDecoderTable)) // mXv == mXs
    return MCDisassembler::Fail;

  Register DstReg = mXvDecoderTable[DstRegRaw];
  assert(DstReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_2D_SRS_CMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                               const MCDisassembler *Decoder) {

  unsigned SrcRegRaw = fieldFromInstruction(insn, 10, 4);
  unsigned ShftRegRaw = fieldFromInstruction(insn, 16, 2);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (SrcRegRaw >= std::size(eCMDecoderTable) ||
      ShftRegRaw >= std::size(mSsDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = eCMDecoderTable[SrcRegRaw];
  Register ShftReg = mSsDecoderTable[ShftRegRaw];
  assert(SrcReg && ShftReg && "Register not found.");
  build2D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addInputs()
      .addReg(SrcReg)
      .addReg(ShftReg);

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_3D_SRS_CMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                               const MCDisassembler *Decoder) {

  unsigned SrcRegRaw = fieldFromInstruction(insn, 10, 4);
  unsigned ShftRegRaw = fieldFromInstruction(insn, 16, 2);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (SrcRegRaw >= std::size(eCMDecoderTable) ||
      ShftRegRaw >= std::size(mSsDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = eCMDecoderTable[SrcRegRaw];
  Register ShftReg = mSsDecoderTable[ShftRegRaw];
  assert(SrcReg && ShftReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addInputs()
      .addReg(SrcReg)
      .addReg(ShftReg);

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_2D_SRS_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                               const MCDisassembler *Decoder) {

  unsigned SrcRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned ShftRegRaw = fieldFromInstruction(insn, 16, 2);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (SrcRegRaw >= std::size(mBMmDecoderTable) ||
      ShftRegRaw >= std::size(mSsDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = mBMmDecoderTable[SrcRegRaw];
  Register ShftReg = mSsDecoderTable[ShftRegRaw];

  assert(SrcReg && ShftReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addInputs()
      .addReg(SrcReg)
      .addReg(ShftReg);

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_3D_SRS_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                               const MCDisassembler *Decoder) {

  unsigned SrcRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned ShftRegRaw = fieldFromInstruction(insn, 16, 2);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (SrcRegRaw >= std::size(mBMmDecoderTable) ||
      ShftRegRaw >= std::size(mSsDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = mBMmDecoderTable[SrcRegRaw];
  Register ShftReg = mSsDecoderTable[ShftRegRaw];

  assert(SrcReg && ShftReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addInputs()
      .addReg(SrcReg)
      .addReg(ShftReg);

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_2D_CONV_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder) {

  unsigned SrcRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (SrcRegRaw >= std::size(mBMmDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = mBMmDecoderTable[SrcRegRaw];
  assert(SrcReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addInputs()
      .addReg(SrcReg);

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_3D_CONV_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder) {

  unsigned SrcRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (SrcRegRaw >= std::size(mBMmDecoderTable))
    return MCDisassembler::Fail;

  Register SrcReg = mBMmDecoderTable[SrcRegRaw];
  assert(SrcReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addOutputs(Decoder)
      .addInputs()
      .addReg(SrcReg);

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_2D_UPS_CMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 10, 4);
  unsigned ShftRegRaw = fieldFromInstruction(insn, 16, 2);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (DstRegRaw >= std::size(eCMDecoderTable) ||
      ShftRegRaw >= std::size(mSsDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = eCMDecoderTable[DstRegRaw];
  Register ShftReg = mSsDecoderTable[ShftRegRaw];
  assert(DstReg && ShftReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addReg(ShftReg)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_3D_UPS_CMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 10, 4);
  unsigned ShftRegRaw = fieldFromInstruction(insn, 16, 2);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (DstRegRaw >= std::size(eCMDecoderTable) ||
      ShftRegRaw >= std::size(mSsDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = eCMDecoderTable[DstRegRaw];
  Register ShftReg = mSsDecoderTable[ShftRegRaw];
  assert(DstReg && ShftReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addReg(ShftReg)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_2D_UPS_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned ShftRegRaw = fieldFromInstruction(insn, 16, 2);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (DstRegRaw >= std::size(mBMmDecoderTable) ||
      ShftRegRaw >= std::size(mSsDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = mBMmDecoderTable[DstRegRaw];
  Register ShftReg = mSsDecoderTable[ShftRegRaw];
  assert(DstReg && ShftReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addReg(ShftReg)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_3D_UPS_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned ShftRegRaw = fieldFromInstruction(insn, 16, 2);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (DstRegRaw >= std::size(mBMmDecoderTable) ||
      ShftRegRaw >= std::size(mSsDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = mBMmDecoderTable[DstRegRaw];
  Register ShftReg = mSsDecoderTable[ShftRegRaw];
  assert(DstReg && ShftReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addReg(ShftReg)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_2D_CONV_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                 const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 3);
  if (DstRegRaw >= std::size(mBMmDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = mBMmDecoderTable[DstRegRaw];
  assert(DstReg && "Register not found.");

  build2D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_3D_CONV_BMInstruction(MCInst &MI, InsnType &insn, uint64_t Address,
                                 const MCDisassembler *Decoder) {

  unsigned DstRegRaw = fieldFromInstruction(insn, 9, 5);
  unsigned PtrRegRaw = fieldFromInstruction(insn, 24, 3);
  unsigned ModRegRaw = fieldFromInstruction(insn, 21, 2);
  if (DstRegRaw >= std::size(mBMmDecoderTable))
    return MCDisassembler::Fail;

  Register DstReg = mBMmDecoderTable[DstRegRaw];
  assert(DstReg && "Register not found.");

  build3D(MI, PtrRegRaw, ModRegRaw)
      .addReg(DstReg)
      .addOutputs(Decoder)
      .addInputs();

  return MCDisassembler::Success;
}
namespace {

/// Interpret the code bits to determine the format size.
int formatSize(ArrayRef<uint8_t> Bytes) {
  switch (Bytes[0] & 0b1111) {
  default:
    break;
  case 0b0000:
  case 0b0010:
  case 0b0100:
  case 0b0110:
  case 0b1000:
  case 0b1010:
  case 0b1100:
  case 0b1110:
    return 16;
  case 0b0001:
    return 2;
  case 0b0011:
    return 8;
  case 0b0101:
    return 6;
  case 0b0111:
    return 12;
  case 0b1001:
    return 4;
  case 0b1011:
    return 10;
  case 0b1101:
    return 6;
  case 0b1111:
    return 14;
  }
  dbgs() << format("0x%08x", Bytes[0]) << "\n";
  report_fatal_error("Unknown Instruction in Disassembler");
  return -1;
}

} // namespace

DecodeStatus AIE2Disassembler::getInstruction(MCInst &MI, uint64_t &Size,
                                              ArrayRef<uint8_t> Bytes,
                                              uint64_t Address,
                                              raw_ostream &CS) const {
  Size = formatSize(Bytes);
  if (Bytes.size() < Size) {
    LLVM_DEBUG(dbgs() << "Disassembler failed with buffer overrun: "
                      << Bytes.size() << "<" << Size << "\n");
    Size = 0;
    return MCDisassembler::Fail;
  }

  // Calling the auto-generated decoder function.
  // For formats bigger than 64 bits we use APint.
  // Note that there are some nasty bits in the instruction decode
  // tables generated by tablegen if decoding more than 64 bits at
  // once. See: https://reviews.llvm.org/D52100
  // TLDR: The format size can be more than 64 bits, but any
  // field extracted from it should fit in a 64 bit type.
  // Since all stand-alone instructions fit in 48 bits, this is not a
  // problem.
  switch (Size) {
  case 2: {
    uint16_t Insn = support::endian::read16le(Bytes.data());
    LLVM_DEBUG(dbgs() << "Trying AIE16 table :\n");
    return decodeInstruction(DecoderTableFormats16, MI, Insn, Address, this,
                             STI);
  }
  case 4: {
    uint32_t Insn = support::endian::read32le(Bytes.data());
    LLVM_DEBUG(dbgs() << "Trying AIE32 table :\n";);
    return decodeInstruction(DecoderTableFormats32, MI, Insn, Address, this,
                             STI);
  }
  case 6: {
    uint64_t Low = support::endian::read32le(Bytes.data());
    uint64_t High = support::endian::read16le(Bytes.data() + 4);
    uint64_t Insn = (High << 32) | Low;
    LLVM_DEBUG(dbgs() << "Trying AIE48 table :\n";);
    return decodeInstruction(DecoderTableFormats48, MI, Insn, Address, this,
                             STI);
  }
  case 8: {
    uint64_t Insn = support::endian::read64le(Bytes.data());
    LLVM_DEBUG(dbgs() << "Trying AIE64 table :\n";);
    return decodeInstruction(DecoderTableFormats64, MI, Insn, Address, this,
                             STI);
  }
  case 10: {
    APInt Insn(96, support::endian::read16le(Bytes.data() + 8));
    Insn <<= 64;
    Insn |= support::endian::read64le(Bytes.data());
    LLVM_DEBUG(dbgs() << "Trying AIE80 table :\n");
    return decodeInstruction(DecoderTableFormats80, MI, Insn, Address, this,
                             STI);
  }
  case 12: {
    APInt Insn(96, support::endian::read32le(Bytes.data() + 8));
    Insn <<= 64;
    Insn |= support::endian::read64le(Bytes.data());
    LLVM_DEBUG(dbgs() << "Trying AIE96 table :\n");
    return decodeInstruction(DecoderTableFormats96, MI, Insn, Address, this,
                             STI);
  }
  case 14: {
    APInt Insn(112, support::endian::read16le(Bytes.data() + 12));
    Insn <<= 32;
    Insn |= support::endian::read32le(Bytes.data() + 8);
    Insn <<= 64;
    Insn |= support::endian::read64le(Bytes.data());
    LLVM_DEBUG(dbgs() << "Trying AIE112 table :\n");
    return decodeInstruction(DecoderTableFormats112, MI, Insn, Address, this,
                             STI);
  }
  case 16: {
    APInt Insn(128, support::endian::read64le(Bytes.data() + 8));
    Insn <<= 64;
    Insn |= support::endian::read64le(Bytes.data());
    LLVM_DEBUG(dbgs() << "Trying AIE128 table :\n");
    return decodeInstruction(DecoderTableFormats128, MI, Insn, Address, this,
                             STI);
  }
  default:
    assert(false);
  }
  return MCDisassembler::Fail;
}
