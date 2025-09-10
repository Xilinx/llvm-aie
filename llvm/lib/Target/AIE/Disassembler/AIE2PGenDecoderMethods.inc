//===-AIE2PGenDecoderMethods.inc -----------------------------*- tablegen
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

template <typename InsnType>
static DecodeStatus DecodeDms_lda_2D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 7;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mLdaSclDecoderTable));
  Register dstReg = mLdaSclDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_lda_q_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQQsaDecoderTable));
  Register dstReg = mQQsaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmhb_lda_2D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eRDecoderTable));
  Register dstReg = eRDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDms_lda_3D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 7;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mLdaSclDecoderTable));
  Register dstReg = mLdaSclDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_lda_q_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQQsaDecoderTable));
  Register dstReg = mQQsaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmhb_lda_3D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eRDecoderTable));
  Register dstReg = eRDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeTm_lda_2D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eRDecoderTable));
  Register dstReg = eRDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeTm_lda_3D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eRDecoderTable));
  Register dstReg = eRDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLda_ptr_inc_2D(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLda_ptr_inc_3D(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLdb_or1_2D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLdb_or1_3D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeSt_ptr_inc_2D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeSt_ptr_inc_3D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDms_sts_2D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 7;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 4, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mSclStDecoderTable));
  Register srcReg = mSclStDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_sts_q_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 2;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 9, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mQQssDecoderTable));
  Register srcReg = mQQssDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmhb_sts_2D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(eRDecoderTable));
  Register srcReg = eRDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDms_sts_3D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 7;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 4, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mSclStDecoderTable));
  Register srcReg = mSclStDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_sts_q_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 2;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 9, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mQQssDecoderTable));
  Register srcReg = mQQssDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmhb_sts_3D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(eRDecoderTable));
  Register srcReg = eRDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeTm_sts_2D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(eRDecoderTable));
  Register srcReg = eRDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeTm_sts_3D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(eRDecoderTable));
  Register srcReg = eRDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_lda_w_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mWaDecoderTable));
  Register dstReg = mWaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_bf_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned opRegRawNumBits = 5;
  unsigned opRegRaw = fieldFromInstruction(Insn, 6, opRegRawNumBits);
  static_assert((1 << opRegRawNumBits) == std::size(mBMmDecoderTable));
  Register opReg = mBMmDecoderTable[opRegRaw];
  if (!opReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(opReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_bf_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned opRegRawNumBits = 4;
  unsigned opRegRaw = fieldFromInstruction(Insn, 7, opRegRawNumBits);
  static_assert((1 << opRegRawNumBits) == std::size(mCMmDecoderTable));
  Register opReg = mCMmDecoderTable[opRegRaw];
  if (!opReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(opReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_w2b_2D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mBMmDecoderTable));
  Register dstReg = mBMmDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned suRegRawNumBits = 2;
  unsigned suRegRaw = fieldFromInstruction(Insn, 4, suRegRawNumBits);
  static_assert((1 << suRegRawNumBits) == std::size(eSDecoderTable));
  Register suReg = eSDecoderTable[suRegRaw];
  if (!suReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(suReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_x2c_2D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mCMmDecoderTable));
  Register dstReg = mCMmDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned suRegRawNumBits = 2;
  unsigned suRegRaw = fieldFromInstruction(Insn, 5, suRegRawNumBits);
  static_assert((1 << suRegRawNumBits) == std::size(eSDecoderTable));
  Register suReg = eSDecoderTable[suRegRaw];
  if (!suReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(suReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_w2c_2D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mCMmDecoderTable));
  Register dstReg = mCMmDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned suRegRawNumBits = 2;
  unsigned suRegRaw = fieldFromInstruction(Insn, 5, suRegRawNumBits);
  static_assert((1 << suRegRawNumBits) == std::size(eSDecoderTable));
  Register suReg = eSDecoderTable[suRegRaw];
  if (!suReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(suReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_x2d_2D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned suRegRawNumBits = 2;
  unsigned suRegRaw = fieldFromInstruction(Insn, 6, suRegRawNumBits);
  static_assert((1 << suRegRawNumBits) == std::size(eSDecoderTable));
  Register suReg = eSDecoderTable[suRegRaw];
  if (!suReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(suReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_w_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mWaDecoderTable));
  Register dstReg = mWaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_bm_2D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mBMmDecoderTable));
  Register dstReg = mBMmDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_fifohl_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFifoHLRegDecoderTable));
  Register dstReg = mFifoHLRegDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_x_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXaDecoderTable));
  Register dstReg = mXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_lda_w_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mWaDecoderTable));
  Register dstReg = mWaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_bf_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned opRegRawNumBits = 5;
  unsigned opRegRaw = fieldFromInstruction(Insn, 6, opRegRawNumBits);
  static_assert((1 << opRegRawNumBits) == std::size(mBMmDecoderTable));
  Register opReg = mBMmDecoderTable[opRegRaw];
  if (!opReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(opReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_bf_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned opRegRawNumBits = 4;
  unsigned opRegRaw = fieldFromInstruction(Insn, 7, opRegRawNumBits);
  static_assert((1 << opRegRawNumBits) == std::size(mCMmDecoderTable));
  Register opReg = mCMmDecoderTable[opRegRaw];
  if (!opReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(opReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_w2b_3D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mBMmDecoderTable));
  Register dstReg = mBMmDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned suRegRawNumBits = 2;
  unsigned suRegRaw = fieldFromInstruction(Insn, 4, suRegRawNumBits);
  static_assert((1 << suRegRawNumBits) == std::size(eSDecoderTable));
  Register suReg = eSDecoderTable[suRegRaw];
  if (!suReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(suReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_x2c_3D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mCMmDecoderTable));
  Register dstReg = mCMmDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned suRegRawNumBits = 2;
  unsigned suRegRaw = fieldFromInstruction(Insn, 5, suRegRawNumBits);
  static_assert((1 << suRegRawNumBits) == std::size(eSDecoderTable));
  Register suReg = eSDecoderTable[suRegRaw];
  if (!suReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(suReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_w2c_3D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mCMmDecoderTable));
  Register dstReg = mCMmDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned suRegRawNumBits = 2;
  unsigned suRegRaw = fieldFromInstruction(Insn, 5, suRegRawNumBits);
  static_assert((1 << suRegRawNumBits) == std::size(eSDecoderTable));
  Register suReg = eSDecoderTable[suRegRaw];
  if (!suReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(suReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_x2d_3D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned suRegRawNumBits = 2;
  unsigned suRegRaw = fieldFromInstruction(Insn, 6, suRegRawNumBits);
  static_assert((1 << suRegRawNumBits) == std::size(eSDecoderTable));
  Register suReg = eSDecoderTable[suRegRaw];
  if (!suReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(suReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_w_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mWaDecoderTable));
  Register dstReg = mWaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_bm_3D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mBMmDecoderTable));
  Register dstReg = mBMmDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_fifohl_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFifoHLRegDecoderTable));
  Register dstReg = mFifoHLRegDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_x_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXaDecoderTable));
  Register dstReg = mXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_FILL_512(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_x__fifo_2d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXaDecoderTable));
  Register dstReg = mXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_x__fifo_3d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXaDecoderTable));
  Register dstReg = mXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_512_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXaDecoderTable));
  Register dstReg = mXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eMDecoderTable));
  Register modReg = eMDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_512_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXaDecoderTable));
  Register dstReg = mXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_ex_ebs16__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXaDecoderTable));
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_ex_ebs16__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXaDecoderTable));
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_544_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXaDecoderTable));
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eMDecoderTable));
  Register modReg = eMDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_544_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXaDecoderTable));
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_ex_ebs8__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXaDecoderTable));
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_ex_ebs8__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXaDecoderTable));
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_576_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXaDecoderTable));
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eMDecoderTable));
  Register modReg = eMDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_576_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXaDecoderTable));
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_qx__fifo_2d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQXsaDecoderTable));
  Register dstReg = mQXsaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_qx__fifo_3d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQXsaDecoderTable));
  Register dstReg = mQXsaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_640_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQXsaDecoderTable));
  Register dstReg = mQXsaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eMDecoderTable));
  Register modReg = eMDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_640_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQXsaDecoderTable));
  Register dstReg = mQXsaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_qex_ebs16__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQEXsaDecoderTable));
  Register dstReg = mQEXsaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_qex_ebs16__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQEXsaDecoderTable));
  Register dstReg = mQEXsaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_704_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQEXsaDecoderTable));
  Register dstReg = mQEXsaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eMDecoderTable));
  Register modReg = eMDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_704_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQEXsaDecoderTable));
  Register dstReg = mQEXsaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 17, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 17, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 17, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_ldb_2D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 3, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mWbDecoderTable));
  Register dstReg = mWbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_ldb_unpack_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXbDecoderTable));
  Register dstReg = mXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_ldb_unpack_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 5, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eYDecoderTable));
  Register dstReg = eYDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_ldb_2D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 3, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mWbDecoderTable));
  Register dstReg = mWbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_ldb_x_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXbDecoderTable));
  Register dstReg = mXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_ldb_3D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 3, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mWbDecoderTable));
  Register dstReg = mWbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_ldb_unpack_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXbDecoderTable));
  Register dstReg = mXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_ldb_unpack_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 5, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eYDecoderTable));
  Register dstReg = eYDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_ldb_3D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 3, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mWbDecoderTable));
  Register dstReg = mWbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_ldb_x_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXbDecoderTable));
  Register dstReg = mXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDB_FILLX_512(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const TargetRegisterClass &conf_e1RC = AIE2P::mR30_fifo_step_e1RegClass;
  if (conf_e1RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned conf_e1Reg = conf_e1RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(conf_e1Reg));

  const TargetRegisterClass &conf_e7RC = AIE2P::mR30_fifo_step_e7RegClass;
  if (conf_e7RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned conf_e7Reg = conf_e7RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(conf_e7Reg));

  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDB_FILL_512(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDB_POPX_512(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXbDecoderTable));
  Register dstReg = mXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const TargetRegisterClass &conf_e1RC = AIE2P::mR30_fifo_step_e1RegClass;
  if (conf_e1RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned conf_e1Reg = conf_e1RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(conf_e1Reg));

  const TargetRegisterClass &conf_e7RC = AIE2P::mR30_fifo_step_e7RegClass;
  if (conf_e7RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned conf_e7Reg = conf_e7RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(conf_e7Reg));

  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_x__fifo_2d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXbDecoderTable));
  Register dstReg = mXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_x__fifo_3d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXbDecoderTable));
  Register dstReg = mXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_512_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXbDecoderTable));
  Register dstReg = mXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eMDecoderTable));
  Register modReg = eMDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_512_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXbDecoderTable));
  Register dstReg = mXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_ex_ebs16__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXbDecoderTable));
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_ex_ebs16__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXbDecoderTable));
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_544_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXbDecoderTable));
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eMDecoderTable));
  Register modReg = eMDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_544_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXbDecoderTable));
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_ex_ebs8__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXbDecoderTable));
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_ex_ebs8__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXbDecoderTable));
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_576_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXbDecoderTable));
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eMDecoderTable));
  Register modReg = eMDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_576_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXbDecoderTable));
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_qx__fifo_2d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQXsbDecoderTable));
  Register dstReg = mQXsbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_qx__fifo_3d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQXsbDecoderTable));
  Register dstReg = mQXsbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_640_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQXsbDecoderTable));
  Register dstReg = mQXsbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eMDecoderTable));
  Register modReg = eMDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_640_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQXsbDecoderTable));
  Register dstReg = mQXsbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_qex_ebs16__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQEXsbDecoderTable));
  Register dstReg = mQEXsbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_qex_ebs16__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQEXsbDecoderTable));
  Register dstReg = mQEXsbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_704_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQEXsbDecoderTable));
  Register dstReg = mQEXsbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 11, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eMDecoderTable));
  Register modReg = eMDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_704_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 2;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mQEXsbDecoderTable));
  Register dstReg = mQEXsbDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 14, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(eRF2DecoderTable));
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 14, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 14, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(eRF2DecoderTable));
  Register posReg = eRF2DecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_sts_w_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mWsDecoderTable));
  Register srcReg = mWsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_srs_bf_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mBMsDecoderTable));
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_srs_bf_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mCMsDecoderTable));
  Register srcReg = mCMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_pack_2D(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mXsDecoderTable));
  Register srcReg = mXsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_pack_2D(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 3;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(eYDecoderTable));
  Register srcReg = eYDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDm_sts_srs_cm_2D(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mCMsDecoderTable));
  Register srcReg = mCMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned suRegRawNumBits = 2;
  unsigned suRegRaw = fieldFromInstruction(Insn, 4, suRegRawNumBits);
  static_assert((1 << suRegRawNumBits) == std::size(eSDecoderTable));
  Register suReg = eSDecoderTable[suRegRaw];
  if (!suReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(suReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_srs_bm_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mBMsDecoderTable));
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned suRegRawNumBits = 2;
  unsigned suRegRaw = fieldFromInstruction(Insn, 4, suRegRawNumBits);
  static_assert((1 << suRegRawNumBits) == std::size(eSDecoderTable));
  Register suReg = eSDecoderTable[suRegRaw];
  if (!suReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(suReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_srs_dm_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 3;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(eDMDecoderTable));
  Register srcReg = eDMDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned suRegRawNumBits = 2;
  unsigned suRegRaw = fieldFromInstruction(Insn, 4, suRegRawNumBits);
  static_assert((1 << suRegRawNumBits) == std::size(eSDecoderTable));
  Register suReg = eSDecoderTable[suRegRaw];
  if (!suReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(suReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_w_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mWsDecoderTable));
  Register srcReg = mWsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_bm_2D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mBMsDecoderTable));
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_fifohl_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 3;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mFifoHLRegDecoderTable));
  Register srcReg = mFifoHLRegDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_x_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mXsDecoderTable));
  Register srcReg = mXsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_sts_w_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mWsDecoderTable));
  Register srcReg = mWsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_srs_bf_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mBMsDecoderTable));
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_srs_bf_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mCMsDecoderTable));
  Register srcReg = mCMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_pack_3D(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mXsDecoderTable));
  Register srcReg = mXsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_pack_3D(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 3;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(eYDecoderTable));
  Register srcReg = eYDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDm_sts_srs_cm_3D(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mCMsDecoderTable));
  Register srcReg = mCMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned suRegRawNumBits = 2;
  unsigned suRegRaw = fieldFromInstruction(Insn, 4, suRegRawNumBits);
  static_assert((1 << suRegRawNumBits) == std::size(eSDecoderTable));
  Register suReg = eSDecoderTable[suRegRaw];
  if (!suReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(suReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_srs_bm_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mBMsDecoderTable));
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned suRegRawNumBits = 2;
  unsigned suRegRaw = fieldFromInstruction(Insn, 4, suRegRawNumBits);
  static_assert((1 << suRegRawNumBits) == std::size(eSDecoderTable));
  Register suReg = eSDecoderTable[suRegRaw];
  if (!suReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(suReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_srs_dm_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 3;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(eDMDecoderTable));
  Register srcReg = eDMDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned suRegRawNumBits = 2;
  unsigned suRegRaw = fieldFromInstruction(Insn, 4, suRegRawNumBits);
  static_assert((1 << suRegRawNumBits) == std::size(eSDecoderTable));
  Register suReg = eSDecoderTable[suRegRaw];
  if (!suReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(suReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_w_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mWsDecoderTable));
  Register srcReg = mWsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_bm_3D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mBMsDecoderTable));
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_fifohl_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 3;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mFifoHLRegDecoderTable));
  Register srcReg = mFifoHLRegDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_x_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mXsDecoderTable));
  Register srcReg = mXsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_sts_fifo_bare_x__fifo_2d_flush(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const TargetRegisterClass &fifo_reg_outRC = AIE2P::mStFifoRegClass;
  if (fifo_reg_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned fifo_reg_outReg = fifo_reg_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const TargetRegisterClass &ptr_outRC = AIE2P::mPfsRegClass;
  if (ptr_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned ptr_outReg = ptr_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const TargetRegisterClass &avail_outRC = AIE2P::mR26_fifo_stRegClass;
  if (avail_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned avail_outReg = avail_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(avail_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const TargetRegisterClass &fifo_regRC = AIE2P::mStFifoRegClass;
  if (fifo_regRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned fifo_regReg = fifo_regRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const TargetRegisterClass &ptrRC = AIE2P::mPfsRegClass;
  if (ptrRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned ptrReg = ptrRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(ptrReg));

  const TargetRegisterClass &availRC = AIE2P::mR26_fifo_stRegClass;
  if (availRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned availReg = availRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(availReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_sts_fifo_bare_x__fifo_3d_flush(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const TargetRegisterClass &fifo_reg_outRC = AIE2P::mStFifoRegClass;
  if (fifo_reg_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned fifo_reg_outReg = fifo_reg_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const TargetRegisterClass &ptr_outRC = AIE2P::mPfsRegClass;
  if (ptr_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned ptr_outReg = ptr_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const TargetRegisterClass &avail_outRC = AIE2P::mR26_fifo_stRegClass;
  if (avail_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned avail_outReg = avail_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(avail_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const TargetRegisterClass &fifo_regRC = AIE2P::mStFifoRegClass;
  if (fifo_regRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned fifo_regReg = fifo_regRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const TargetRegisterClass &ptrRC = AIE2P::mPfsRegClass;
  if (ptrRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned ptrReg = ptrRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(ptrReg));

  const TargetRegisterClass &availRC = AIE2P::mR26_fifo_stRegClass;
  if (availRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned availReg = availRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(availReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_sts_fifo_conv_x__fifo_2d_flush(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const TargetRegisterClass &fifo_reg_outRC = AIE2P::mStFifoRegClass;
  if (fifo_reg_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned fifo_reg_outReg = fifo_reg_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const TargetRegisterClass &ptr_outRC = AIE2P::mPfsRegClass;
  if (ptr_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned ptr_outReg = ptr_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const TargetRegisterClass &avail_outRC = AIE2P::mR26_fifo_stRegClass;
  if (avail_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned avail_outReg = avail_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(avail_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const TargetRegisterClass &fifo_regRC = AIE2P::mStFifoRegClass;
  if (fifo_regRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned fifo_regReg = fifo_regRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const TargetRegisterClass &ptrRC = AIE2P::mPfsRegClass;
  if (ptrRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned ptrReg = ptrRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(ptrReg));

  const TargetRegisterClass &availRC = AIE2P::mR26_fifo_stRegClass;
  if (availRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned availReg = availRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(availReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_sts_fifo_conv_x__fifo_3d_flush(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const TargetRegisterClass &fifo_reg_outRC = AIE2P::mStFifoRegClass;
  if (fifo_reg_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned fifo_reg_outReg = fifo_reg_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const TargetRegisterClass &ptr_outRC = AIE2P::mPfsRegClass;
  if (ptr_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned ptr_outReg = ptr_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const TargetRegisterClass &avail_outRC = AIE2P::mR26_fifo_stRegClass;
  if (avail_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned avail_outReg = avail_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(avail_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const TargetRegisterClass &fifo_regRC = AIE2P::mStFifoRegClass;
  if (fifo_regRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned fifo_regReg = fifo_regRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const TargetRegisterClass &ptrRC = AIE2P::mPfsRegClass;
  if (ptrRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned ptrReg = ptrRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(ptrReg));

  const TargetRegisterClass &availRC = AIE2P::mR26_fifo_stRegClass;
  if (availRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned availReg = availRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(availReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 14, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}
