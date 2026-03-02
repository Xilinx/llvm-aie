//===-AIE2PSGenDecoderMethods.inc -----------------------------*- tablegen
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

template <typename InsnType>
static DecodeStatus
DecodeLDA_2D_dml_lda2_scalar_EE(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEEaDecoderTable));
  Register dstReg = mEEaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeLDA_2D_dml_lda2_scalar_EG(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEGaDecoderTable));
  Register dstReg = mEGaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeLDA_2D_dml_lda2_scalar_GG(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mGGaDecoderTable));
  Register dstReg = mGGaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeLDA_2D_dml_lda_scalar_F(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFaDecoderTable));
  Register dstReg = mFaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeLDA_2D_dml_lda_scalar_L(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eLDecoderTable));
  Register dstReg = eLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_2D_dms_lda_g(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mGaDecoderTable));
  Register dstReg = mGaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_2D_dms_lda_scalar(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 7;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 5, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mLdaSclDecoderTable));
  Register dstReg = mLdaSclDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_2D_dmv_lda_eg2(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEG2aDecoderTable));
  Register dstReg = mEG2aDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_2D_dmv_lda_f(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFFaDecoderTable));
  Register dstReg = mFFaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_2D_dmv_lda_feg(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEGaDecoderTable));
  Register dstReg = mFEGaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_2D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                 const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mRaDecoderTable));
  Register dstReg = mRaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeLDA_3D_dml_lda2_scalar_EE(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEEaDecoderTable));
  Register dstReg = mEEaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeLDA_3D_dml_lda2_scalar_EG(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEGaDecoderTable));
  Register dstReg = mEGaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeLDA_3D_dml_lda2_scalar_GG(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mGGaDecoderTable));
  Register dstReg = mGGaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeLDA_3D_dml_lda_scalar_F(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFaDecoderTable));
  Register dstReg = mFaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeLDA_3D_dml_lda_scalar_L(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eLDecoderTable));
  Register dstReg = eLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_3D_dms_lda_g(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mGaDecoderTable));
  Register dstReg = mGaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_3D_dms_lda_scalar(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 7;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 5, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mLdaSclDecoderTable));
  Register dstReg = mLdaSclDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_3D_dmv_lda_eg2(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEG2aDecoderTable));
  Register dstReg = mEG2aDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_3D_dmv_lda_f(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFFaDecoderTable));
  Register dstReg = mFFaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_3D_dmv_lda_feg(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEGaDecoderTable));
  Register dstReg = mFEGaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_3D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                 const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mRaDecoderTable));
  Register dstReg = mRaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_TM_2D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mRaDecoderTable));
  Register dstReg = mRaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLDA_TM_3D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mRaDecoderTable));
  Register dstReg = mRaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodePADD_2D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodePADD_3D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodePADDB_2D(MCInst &MI, InsnType &Insn, uint64_t Address,
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
static DecodeStatus DecodePADDB_3D(MCInst &MI, InsnType &Insn, uint64_t Address,
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
static DecodeStatus DecodeST_2D_dml_sts_scalar(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 7;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 5, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mlStsDecoderTable));
  Register srcReg = mlStsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_2D_dms_sts_g(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mGsDecoderTable));
  Register srcReg = mGsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_2D_dms_sts_scalar(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 7;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 5, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mSclStDecoderTable));
  Register srcReg = mSclStDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_2D_dmv_sts_f(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mFFsDecoderTable));
  Register srcReg = mFFsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_2D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mRsDecoderTable));
  Register srcReg = mRsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_3D_dml_sts_scalar(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 7;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 5, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mlStsDecoderTable));
  Register srcReg = mlStsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_3D_dms_sts_g(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mGsDecoderTable));
  Register srcReg = mGsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_3D_dms_sts_scalar(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 7;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 5, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mSclStDecoderTable));
  Register srcReg = mSclStDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_3D_dmv_sts_f(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mFFsDecoderTable));
  Register srcReg = mFFsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_3D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mRsDecoderTable));
  Register srcReg = mRsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_TM_2D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mRsDecoderTable));
  Register srcReg = mRsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeST_TM_3D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mRsDecoderTable));
  Register srcReg = mRsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_vmul_bf_core_X_QY(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc2Reg = eDMDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_vmul_bf_core_X_X(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc2Reg = eDMDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_vmul_bf_core_Y_R(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc2Reg = eDMDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_vmul_bf_core_Y_Y(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc2Reg = eDMDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc2Reg = eCMLDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc2Reg = eCMLDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVADDMAC_f_vaddmac_bf_half_vmul_bf_core_Y_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc2Reg = eCMLDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVADDMAC_f_vaddmac_bf_half_vmul_bf_half_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc2Reg = eCMLDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_Y_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_half_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_Y_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_half_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bfp13(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc2Reg = eDMDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mFEXvDecoderTable));
  Register s1Reg = mFEXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eFEYDecoderTable));
  Register s2Reg = eFEYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bfp13_scd_add_scd_incr(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mFEXvDecoderTable));
  Register s1Reg = mFEXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eFEYDecoderTable));
  Register s2Reg = eFEYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVADDMAC_f_vaddmac_bfp13_scd_add_scd_noincr(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mFEXvDecoderTable));
  Register s1Reg = mFEXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eFEYDecoderTable));
  Register s2Reg = eFEYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bfp16(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc2Reg = eDMDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1lRegRawNumBits = 4;
  unsigned s1lRegRaw = fieldFromInstruction(Insn, 15, s1lRegRawNumBits);
  static_assert((1 << s1lRegRawNumBits) == std::size(mEWlv_bisDecoderTable));
  Register s1lReg = mEWlv_bisDecoderTable[s1lRegRaw];
  if (!s1lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1lReg));

  const unsigned s2lRegRawNumBits = 2;
  unsigned s2lRegRaw = fieldFromInstruction(Insn, 13, s2lRegRawNumBits);
  static_assert((1 << s2lRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2lReg = eEYsDecoderTable[s2lRegRaw];
  if (!s2lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2lReg));

  const unsigned s2hRegRawNumBits = 2;
  unsigned s2hRegRaw = fieldFromInstruction(Insn, 11, s2hRegRawNumBits);
  static_assert((1 << s2hRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2hReg = eEYsDecoderTable[s2hRegRaw];
  if (!s2hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2hReg));

  const unsigned s1hRegRawNumBits = 4;
  unsigned s1hRegRaw = fieldFromInstruction(Insn, 15, s1hRegRawNumBits);
  static_assert((1 << s1hRegRawNumBits) == std::size(mEWhvDecoderTable));
  Register s1hReg = mEWhvDecoderTable[s1hRegRaw];
  if (!s1hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1hReg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMAC_f_vaddmac_bfp16_scd_add_scd_incr(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1lRegRawNumBits = 4;
  unsigned s1lRegRaw = fieldFromInstruction(Insn, 15, s1lRegRawNumBits);
  static_assert((1 << s1lRegRawNumBits) == std::size(mEWlv_bisDecoderTable));
  Register s1lReg = mEWlv_bisDecoderTable[s1lRegRaw];
  if (!s1lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1lReg));

  const unsigned s2lRegRawNumBits = 2;
  unsigned s2lRegRaw = fieldFromInstruction(Insn, 13, s2lRegRawNumBits);
  static_assert((1 << s2lRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2lReg = eEYsDecoderTable[s2lRegRaw];
  if (!s2lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2lReg));

  const unsigned s2hRegRawNumBits = 2;
  unsigned s2hRegRaw = fieldFromInstruction(Insn, 11, s2hRegRawNumBits);
  static_assert((1 << s2hRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2hReg = eEYsDecoderTable[s2hRegRaw];
  if (!s2hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2hReg));

  const unsigned s1hRegRawNumBits = 4;
  unsigned s1hRegRaw = fieldFromInstruction(Insn, 15, s1hRegRawNumBits);
  static_assert((1 << s1hRegRawNumBits) == std::size(mEWhvDecoderTable));
  Register s1hReg = mEWhvDecoderTable[s1hRegRaw];
  if (!s1hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1hReg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVADDMAC_f_vaddmac_bfp16_scd_add_scd_noincr(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1lRegRawNumBits = 4;
  unsigned s1lRegRaw = fieldFromInstruction(Insn, 15, s1lRegRawNumBits);
  static_assert((1 << s1lRegRawNumBits) == std::size(mEWlv_bisDecoderTable));
  Register s1lReg = mEWlv_bisDecoderTable[s1lRegRaw];
  if (!s1lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1lReg));

  const unsigned s2lRegRawNumBits = 2;
  unsigned s2lRegRaw = fieldFromInstruction(Insn, 13, s2lRegRawNumBits);
  static_assert((1 << s2lRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2lReg = eEYsDecoderTable[s2lRegRaw];
  if (!s2lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2lReg));

  const unsigned s2hRegRawNumBits = 2;
  unsigned s2hRegRaw = fieldFromInstruction(Insn, 11, s2hRegRawNumBits);
  static_assert((1 << s2hRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2hReg = eEYsDecoderTable[s2hRegRaw];
  if (!s2hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2hReg));

  const unsigned s1hRegRawNumBits = 4;
  unsigned s1hRegRaw = fieldFromInstruction(Insn, 15, s1hRegRawNumBits);
  static_assert((1 << s1hRegRawNumBits) == std::size(mEWhvDecoderTable));
  Register s1hReg = mEWhvDecoderTable[s1hRegRaw];
  if (!s1hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1hReg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_vmul_bf_core_X_QY(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc2Reg = eDMDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_vmul_bf_core_X_X(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc2Reg = eDMDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_vmul_bf_core_Y_R(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc2Reg = eDMDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_vmul_bf_core_Y_Y(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc2Reg = eDMDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc2Reg = eCMLDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc2Reg = eCMLDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVADDMSC_f_vaddmac_bf_half_vmul_bf_core_Y_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc2Reg = eCMLDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVADDMSC_f_vaddmac_bf_half_vmul_bf_half_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc2Reg = eCMLDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_Y_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_half_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_R(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_Y(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_Y_QY(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_half_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bfp13(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc2Reg = eDMDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mFEXvDecoderTable));
  Register s1Reg = mFEXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eFEYDecoderTable));
  Register s2Reg = eFEYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bfp13_scd_add_scd_incr(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mFEXvDecoderTable));
  Register s1Reg = mFEXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eFEYDecoderTable));
  Register s2Reg = eFEYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVADDMSC_f_vaddmac_bfp13_scd_add_scd_noincr(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mFEXvDecoderTable));
  Register s1Reg = mFEXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eFEYDecoderTable));
  Register s2Reg = eFEYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bfp16(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 19, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned acc2RegRawNumBits = 3;
  unsigned acc2RegRaw = fieldFromInstruction(Insn, 4, acc2RegRawNumBits);
  static_assert((1 << acc2RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc2Reg = eDMDecoderTable[acc2RegRaw];
  if (!acc2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1lRegRawNumBits = 4;
  unsigned s1lRegRaw = fieldFromInstruction(Insn, 15, s1lRegRawNumBits);
  static_assert((1 << s1lRegRawNumBits) == std::size(mEWlv_bisDecoderTable));
  Register s1lReg = mEWlv_bisDecoderTable[s1lRegRaw];
  if (!s1lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1lReg));

  const unsigned s2lRegRawNumBits = 2;
  unsigned s2lRegRaw = fieldFromInstruction(Insn, 13, s2lRegRawNumBits);
  static_assert((1 << s2lRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2lReg = eEYsDecoderTable[s2lRegRaw];
  if (!s2lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2lReg));

  const unsigned s2hRegRawNumBits = 2;
  unsigned s2hRegRaw = fieldFromInstruction(Insn, 11, s2hRegRawNumBits);
  static_assert((1 << s2hRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2hReg = eEYsDecoderTable[s2hRegRaw];
  if (!s2hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2hReg));

  const unsigned s1hRegRawNumBits = 4;
  unsigned s1hRegRaw = fieldFromInstruction(Insn, 15, s1hRegRawNumBits);
  static_assert((1 << s1hRegRawNumBits) == std::size(mEWhvDecoderTable));
  Register s1hReg = mEWhvDecoderTable[s1hRegRaw];
  if (!s1hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1hReg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVADDMSC_f_vaddmac_bfp16_scd_add_scd_incr(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const TargetRegisterClass &acc2_outRC = AIE2PS::mR31_scdRegClass;
  if (acc2_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2_outReg = acc2_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2_outReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1lRegRawNumBits = 4;
  unsigned s1lRegRaw = fieldFromInstruction(Insn, 15, s1lRegRawNumBits);
  static_assert((1 << s1lRegRawNumBits) == std::size(mEWlv_bisDecoderTable));
  Register s1lReg = mEWlv_bisDecoderTable[s1lRegRaw];
  if (!s1lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1lReg));

  const unsigned s2lRegRawNumBits = 2;
  unsigned s2lRegRaw = fieldFromInstruction(Insn, 13, s2lRegRawNumBits);
  static_assert((1 << s2lRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2lReg = eEYsDecoderTable[s2lRegRaw];
  if (!s2lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2lReg));

  const unsigned s2hRegRawNumBits = 2;
  unsigned s2hRegRaw = fieldFromInstruction(Insn, 11, s2hRegRawNumBits);
  static_assert((1 << s2hRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2hReg = eEYsDecoderTable[s2hRegRaw];
  if (!s2hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2hReg));

  const unsigned s1hRegRawNumBits = 4;
  unsigned s1hRegRaw = fieldFromInstruction(Insn, 15, s1hRegRawNumBits);
  static_assert((1 << s1hRegRawNumBits) == std::size(mEWhvDecoderTable));
  Register s1hReg = mEWhvDecoderTable[s1hRegRaw];
  if (!s1hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1hReg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVADDMSC_f_vaddmac_bfp16_scd_add_scd_noincr(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const TargetRegisterClass &acc2RC = AIE2PS::mR31_scdRegClass;
  if (acc2RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned acc2Reg = acc2RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(acc2Reg));

  const unsigned s1lRegRawNumBits = 4;
  unsigned s1lRegRaw = fieldFromInstruction(Insn, 15, s1lRegRawNumBits);
  static_assert((1 << s1lRegRawNumBits) == std::size(mEWlv_bisDecoderTable));
  Register s1lReg = mEWlv_bisDecoderTable[s1lRegRaw];
  if (!s1lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1lReg));

  const unsigned s2lRegRawNumBits = 2;
  unsigned s2lRegRaw = fieldFromInstruction(Insn, 13, s2lRegRawNumBits);
  static_assert((1 << s2lRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2lReg = eEYsDecoderTable[s2lRegRaw];
  if (!s2lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2lReg));

  const unsigned s2hRegRawNumBits = 2;
  unsigned s2hRegRaw = fieldFromInstruction(Insn, 11, s2hRegRawNumBits);
  static_assert((1 << s2hRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2hReg = eEYsDecoderTable[s2hRegRaw];
  if (!s2hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2hReg));

  const unsigned s1hRegRawNumBits = 4;
  unsigned s1hRegRaw = fieldFromInstruction(Insn, 15, s1hRegRawNumBits);
  static_assert((1 << s1hRegRawNumBits) == std::size(mEWhvDecoderTable));
  Register s1hReg = mEWhvDecoderTable[s1hRegRaw];
  if (!s1hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1hReg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_2D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mWaDecoderTable));
  Register dstReg = mWaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_2D_CONV_fp32_bf16_dmw_lda_ups_bf16(MCInst &MI, InsnType &Insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mBMaDecoderTable));
  Register dstReg = mBMaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_2D_CONV_fp32(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mCMaDecoderTable));
  Register dstReg = mCMaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsS(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mBMmDecoderTable));
  Register dstReg = mBMmDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned baseRegRawNumBits = 2;
  unsigned baseRegRaw = fieldFromInstruction(Insn, 5, baseRegRawNumBits);
  static_assert((1 << baseRegRawNumBits) == std::size(eSDecoderTable));
  Register baseReg = eSDecoderTable[baseRegRaw];
  if (!baseReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(baseReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_2D_UPS(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mCMmDecoderTable));
  Register dstReg = mCMmDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned baseRegRawNumBits = 2;
  unsigned baseRegRaw = fieldFromInstruction(Insn, 5, baseRegRawNumBits);
  static_assert((1 << baseRegRawNumBits) == std::size(eSDecoderTable));
  Register baseReg = eSDecoderTable[baseRegRaw];
  if (!baseReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(baseReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsS(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned baseRegRawNumBits = 2;
  unsigned baseRegRaw = fieldFromInstruction(Insn, 5, baseRegRawNumBits);
  static_assert((1 << baseRegRawNumBits) == std::size(eSDecoderTable));
  Register baseReg = eSDecoderTable[baseRegRaw];
  if (!baseReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(baseReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_2D_dmw_lda_feg2(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEG2aDecoderTable));
  Register dstReg = mFEG2aDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_2D_dmx_lda_bm(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mBM_ldDecoderTable));
  Register dstReg = mBM_ldDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_2D_dmx_lda_x(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXaDecoderTable));
  Register dstReg = mXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_3D(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mWaDecoderTable));
  Register dstReg = mWaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_3D_CONV_fp32_bf16_dmw_lda_ups_bf16(MCInst &MI, InsnType &Insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mBMaDecoderTable));
  Register dstReg = mBMaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_3D_CONV_fp32(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mCMaDecoderTable));
  Register dstReg = mCMaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsS(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mBMmDecoderTable));
  Register dstReg = mBMmDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned baseRegRawNumBits = 2;
  unsigned baseRegRaw = fieldFromInstruction(Insn, 5, baseRegRawNumBits);
  static_assert((1 << baseRegRawNumBits) == std::size(eSDecoderTable));
  Register baseReg = eSDecoderTable[baseRegRaw];
  if (!baseReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(baseReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_3D_UPS(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mCMmDecoderTable));
  Register dstReg = mCMmDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned baseRegRawNumBits = 2;
  unsigned baseRegRaw = fieldFromInstruction(Insn, 5, baseRegRawNumBits);
  static_assert((1 << baseRegRawNumBits) == std::size(eSDecoderTable));
  Register baseReg = eSDecoderTable[baseRegRaw];
  if (!baseReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(baseReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsS(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned baseRegRawNumBits = 2;
  unsigned baseRegRaw = fieldFromInstruction(Insn, 5, baseRegRawNumBits);
  static_assert((1 << baseRegRawNumBits) == std::size(eSDecoderTable));
  Register baseReg = eSDecoderTable[baseRegRaw];
  if (!baseReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(baseReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_3D_dmw_lda_feg2(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEG2aDecoderTable));
  Register dstReg = mFEG2aDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_3D_dmx_lda_bm(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 5;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mBM_ldDecoderTable));
  Register dstReg = mBM_ldDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_3D_dmx_lda_x(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXaDecoderTable));
  Register dstReg = mXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_FILL(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_POP_2D_1(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXaDecoderTable));
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_POP_2D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEXaDecoderTable));
  Register dstReg = mFEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_2D_dmx_lda_fifo_x(MCInst &MI, InsnType &Insn, uint64_t Address,
                                 const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXaDecoderTable));
  Register dstReg = mXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_POP_3D_1(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXaDecoderTable));
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_POP_3D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEXaDecoderTable));
  Register dstReg = mFEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_3D_dmx_lda_fifo_x(MCInst &MI, InsnType &Insn, uint64_t Address,
                                 const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXaDecoderTable));
  Register dstReg = mXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_POP_dmx_lda_fifo_ex_bfp16_fifo_1d_pop(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXaDecoderTable));
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eMDecoderTable));
  Register modReg = eMDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_dmx_lda_fifo_ex_bfp16_normal_pop(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXaDecoderTable));
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_POP_dmx_lda_fifo_fex_bfp13_fifo_1d_pop(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEXaDecoderTable));
  Register dstReg = mFEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eMDecoderTable));
  Register modReg = eMDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_POP_dmx_lda_fifo_fex_bfp13_normal_pop(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEXaDecoderTable));
  Register dstReg = mFEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_dmx_lda_fifo_x_fifo_1d_pop(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXaDecoderTable));
  Register dstReg = mXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eMDecoderTable));
  Register modReg = eMDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_dmx_lda_fifo_x_normal_pop(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mXaDecoderTable));
  Register dstReg = mXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_mx4_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEXaDecoderTable));
  Register dstReg = mFEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eMDecoderTable));
  Register modReg = eMDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_mx4_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEXaDecoderTable));
  Register dstReg = mFEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_s_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXaDecoderTable));
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eMDecoderTable));
  Register modReg = eMDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_POP_s_normal_pop(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mEXaDecoderTable));
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  const unsigned ptr_outRegRawNumBits = 1;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned fifo_reg_outRegRawNumBits = 1;
  unsigned fifo_reg_outRegRaw =
      fieldFromInstruction(Insn, 18, fifo_reg_outRegRawNumBits);
  static_assert((1 << fifo_reg_outRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  if (!fifo_reg_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const unsigned pos_outRegRawNumBits = 1;
  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 18, pos_outRegRawNumBits);
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register pos_outReg = mRF2aDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const unsigned ptrRegRawNumBits = 1;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePSDecoderTable));
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned fifo_regRegRawNumBits = 1;
  unsigned fifo_regRegRaw =
      fieldFromInstruction(Insn, 18, fifo_regRegRawNumBits);
  static_assert((1 << fifo_regRegRawNumBits) ==
                std::size(eLdFifoRegDecoderTable));
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  if (!fifo_regReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const unsigned posRegRawNumBits = 1;
  unsigned posRegRaw = fieldFromInstruction(Insn, 18, posRegRawNumBits);
  static_assert((1 << posRegRawNumBits) == std::size(mRF2aDecoderTable));
  Register posReg = mRF2aDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDB_2D(MCInst &MI, InsnType &Insn, uint64_t Address,
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
static DecodeStatus
DecodeVLDB_2D_UNPACK_dmw_ldb_unpack_unpackS(MCInst &MI, InsnType &Insn,
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
static DecodeStatus
DecodeVLDB_2D_UNPACK_dmx_ldb_unpack_unpackS(MCInst &MI, InsnType &Insn,
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
static DecodeStatus DecodeVLDB_2D_dmx_ldb_x(MCInst &MI, InsnType &Insn,
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
static DecodeStatus DecodeVLDB_3D(MCInst &MI, InsnType &Insn, uint64_t Address,
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
static DecodeStatus
DecodeVLDB_3D_UNPACK_dmw_ldb_unpack_unpackS(MCInst &MI, InsnType &Insn,
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
static DecodeStatus
DecodeVLDB_3D_UNPACK_dmx_ldb_unpack_unpackS(MCInst &MI, InsnType &Insn,
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
static DecodeStatus DecodeVLDB_3D_dmx_ldb_x(MCInst &MI, InsnType &Insn,
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
static DecodeStatus DecodeVLDB_FILLX(MCInst &MI, InsnType &Insn,
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const TargetRegisterClass &conf_e1RC = AIE2PS::mR30_fifo_step_e1RegClass;
  if (conf_e1RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned conf_e1Reg = conf_e1RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(conf_e1Reg));

  const TargetRegisterClass &conf_e7RC = AIE2PS::mR30_fifo_step_e7RegClass;
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDB_FILL(MCInst &MI, InsnType &Insn,
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDB_POPX(MCInst &MI, InsnType &Insn,
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
  if (!pos_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  const TargetRegisterClass &conf_e1RC = AIE2PS::mR30_fifo_step_e1RegClass;
  if (conf_e1RC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned conf_e1Reg = conf_e1RC.getRegister(0);
  MI.addOperand(MCOperand::createReg(conf_e1Reg));

  const TargetRegisterClass &conf_e7RC = AIE2PS::mR30_fifo_step_e7RegClass;
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDB_POP_2D(MCInst &MI, InsnType &Insn,
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
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
static DecodeStatus DecodeVLDB_POP_2D_1(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEXbDecoderTable));
  Register dstReg = mFEXbDecoderTable[dstRegRaw];
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
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
DecodeVLDB_POP_2D_dmx_ldb_fifo_x(MCInst &MI, InsnType &Insn, uint64_t Address,
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
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
static DecodeStatus DecodeVLDB_POP_3D(MCInst &MI, InsnType &Insn,
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
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
static DecodeStatus DecodeVLDB_POP_3D_1(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEXbDecoderTable));
  Register dstReg = mFEXbDecoderTable[dstRegRaw];
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
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
DecodeVLDB_POP_3D_dmx_ldb_fifo_x(MCInst &MI, InsnType &Insn, uint64_t Address,
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
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
static DecodeStatus DecodeVLDB_POP_dmx_ldb_fifo_ex_bfp16_fifo_1d_pop(
    MCInst &MI, InsnType &Insn, uint64_t Address,
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
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
DecodeVLDB_POP_dmx_ldb_fifo_ex_bfp16_normal_pop(MCInst &MI, InsnType &Insn,
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDB_POP_dmx_ldb_fifo_fex_bfp13_fifo_1d_pop(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEXbDecoderTable));
  Register dstReg = mFEXbDecoderTable[dstRegRaw];
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
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
static DecodeStatus DecodeVLDB_POP_dmx_ldb_fifo_fex_bfp13_normal_pop(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEXbDecoderTable));
  Register dstReg = mFEXbDecoderTable[dstRegRaw];
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_dmx_ldb_fifo_x_fifo_1d_pop(MCInst &MI, InsnType &Insn,
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
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
DecodeVLDB_POP_dmx_ldb_fifo_x_normal_pop(MCInst &MI, InsnType &Insn,
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_mx4_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEXbDecoderTable));
  Register dstReg = mFEXbDecoderTable[dstRegRaw];
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
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
DecodeVLDB_POP_mx4_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 4;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(mFEXbDecoderTable));
  Register dstReg = mFEXbDecoderTable[dstRegRaw];
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_s_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
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
static DecodeStatus DecodeVLDB_POP_s_normal_pop(MCInst &MI, InsnType &Insn,
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
  static_assert((1 << pos_outRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register pos_outReg = mRF2bDecoderTable[pos_outRegRaw];
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
  static_assert((1 << posRegRawNumBits) == std::size(mRF2bDecoderTable));
  Register posReg = mRF2bDecoderTable[posRegRaw];
  if (!posReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_vmul_bf_core_X_QY(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_vmul_bf_core_X_X(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_vmul_bf_core_Y_R(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_vmul_bf_core_Y_Y(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_half_vmul_bf_core_X_R(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_half_vmul_bf_core_X_Y(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_half_vmul_bf_core_Y_QY(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMAC_f_vmac_bf_half_vmul_bf_half_core_X_X(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVMAC_f_vmac_bfp13(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mFEXvDecoderTable));
  Register s1Reg = mFEXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eFEYDecoderTable));
  Register s2Reg = eFEYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVMAC_f_vmac_bfp16(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1lRegRawNumBits = 4;
  unsigned s1lRegRaw = fieldFromInstruction(Insn, 15, s1lRegRawNumBits);
  static_assert((1 << s1lRegRawNumBits) == std::size(mEWlv_bisDecoderTable));
  Register s1lReg = mEWlv_bisDecoderTable[s1lRegRaw];
  if (!s1lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1lReg));

  const unsigned s2lRegRawNumBits = 2;
  unsigned s2lRegRaw = fieldFromInstruction(Insn, 13, s2lRegRawNumBits);
  static_assert((1 << s2lRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2lReg = eEYsDecoderTable[s2lRegRaw];
  if (!s2lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2lReg));

  const unsigned s2hRegRawNumBits = 2;
  unsigned s2hRegRaw = fieldFromInstruction(Insn, 11, s2hRegRawNumBits);
  static_assert((1 << s2hRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2hReg = eEYsDecoderTable[s2hRegRaw];
  if (!s2hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2hReg));

  const unsigned s1hRegRawNumBits = 4;
  unsigned s1hRegRaw = fieldFromInstruction(Insn, 15, s1hRegRawNumBits);
  static_assert((1 << s1hRegRawNumBits) == std::size(mEWhvDecoderTable));
  Register s1hReg = mEWhvDecoderTable[s1hRegRaw];
  if (!s1hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1hReg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_vmul_bf_core_X_QY(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_vmul_bf_core_X_X(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_vmul_bf_core_Y_R(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_vmul_bf_core_Y_Y(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_half_vmul_bf_core_X_R(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_half_vmul_bf_core_X_Y(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_half_vmul_bf_core_Y_QY(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMSC_f_vmac_bf_half_vmul_bf_half_core_X_X(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eCMLDecoderTable));
  Register acc1Reg = eCMLDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVMSC_f_vmac_bfp13(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mFEXvDecoderTable));
  Register s1Reg = mFEXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eFEYDecoderTable));
  Register s2Reg = eFEYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVMSC_f_vmac_bfp16(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned acc1RegRawNumBits = 3;
  unsigned acc1RegRaw = fieldFromInstruction(Insn, 4, acc1RegRawNumBits);
  static_assert((1 << acc1RegRawNumBits) == std::size(eDMDecoderTable));
  Register acc1Reg = eDMDecoderTable[acc1RegRaw];
  if (!acc1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(acc1Reg));

  const unsigned s1lRegRawNumBits = 4;
  unsigned s1lRegRaw = fieldFromInstruction(Insn, 15, s1lRegRawNumBits);
  static_assert((1 << s1lRegRawNumBits) == std::size(mEWlv_bisDecoderTable));
  Register s1lReg = mEWlv_bisDecoderTable[s1lRegRaw];
  if (!s1lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1lReg));

  const unsigned s2lRegRawNumBits = 2;
  unsigned s2lRegRaw = fieldFromInstruction(Insn, 13, s2lRegRawNumBits);
  static_assert((1 << s2lRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2lReg = eEYsDecoderTable[s2lRegRaw];
  if (!s2lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2lReg));

  const unsigned s2hRegRawNumBits = 2;
  unsigned s2hRegRaw = fieldFromInstruction(Insn, 11, s2hRegRawNumBits);
  static_assert((1 << s2hRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2hReg = eEYsDecoderTable[s2hRegRaw];
  if (!s2hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2hReg));

  const unsigned s1hRegRawNumBits = 4;
  unsigned s1hRegRaw = fieldFromInstruction(Insn, 15, s1hRegRawNumBits);
  static_assert((1 << s1hRegRawNumBits) == std::size(mEWhvDecoderTable));
  Register s1hReg = mEWhvDecoderTable[s1hRegRaw];
  if (!s1hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1hReg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_vmul_bf_core_X_QY(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_vmul_bf_core_X_X(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_vmul_bf_core_Y_R(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_vmul_bf_core_Y_Y(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_half_vmul_bf_core_X_R(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_half_vmul_bf_core_X_Y(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_half_vmul_bf_core_Y_QY(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVMUL_f_vmul_bf_half_vmul_bf_half_core_X_X(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVMUL_f_vmul_bfp13(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mFEXvDecoderTable));
  Register s1Reg = mFEXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eFEYDecoderTable));
  Register s2Reg = eFEYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVMUL_f_vmul_bfp16(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1lRegRawNumBits = 4;
  unsigned s1lRegRaw = fieldFromInstruction(Insn, 15, s1lRegRawNumBits);
  static_assert((1 << s1lRegRawNumBits) == std::size(mEWlv_bisDecoderTable));
  Register s1lReg = mEWlv_bisDecoderTable[s1lRegRaw];
  if (!s1lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1lReg));

  const unsigned s2lRegRawNumBits = 2;
  unsigned s2lRegRaw = fieldFromInstruction(Insn, 13, s2lRegRawNumBits);
  static_assert((1 << s2lRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2lReg = eEYsDecoderTable[s2lRegRaw];
  if (!s2lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2lReg));

  const unsigned s2hRegRawNumBits = 2;
  unsigned s2hRegRaw = fieldFromInstruction(Insn, 11, s2hRegRawNumBits);
  static_assert((1 << s2hRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2hReg = eEYsDecoderTable[s2hRegRaw];
  if (!s2hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2hReg));

  const unsigned s1hRegRawNumBits = 4;
  unsigned s1hRegRaw = fieldFromInstruction(Insn, 15, s1hRegRawNumBits);
  static_assert((1 << s1hRegRawNumBits) == std::size(mEWhvDecoderTable));
  Register s1hReg = mEWhvDecoderTable[s1hRegRaw];
  if (!s1hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1hReg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVNEGMUL_f_vmul_bf_vmul_bf_core_X_QY(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVNEGMUL_f_vmul_bf_vmul_bf_core_X_X(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVNEGMUL_f_vmul_bf_vmul_bf_core_Y_R(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVNEGMUL_f_vmul_bf_vmul_bf_core_Y_Y(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVNEGMUL_f_vmul_bf_half_vmul_bf_core_X_R(MCInst &MI, InsnType &Insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mRV16sDecoderTable));
  Register s2Reg = mRV16sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVNEGMUL_f_vmul_bf_half_vmul_bf_core_X_Y(MCInst &MI, InsnType &Insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eYDecoderTable));
  Register s2Reg = eYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVNEGMUL_f_vmul_bf_half_vmul_bf_core_Y_QY(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 3;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 16, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(eYDecoderTable));
  Register s1Reg = eYDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eEY_sDecoderTable));
  Register s2Reg = eEY_sDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVNEGMUL_f_vmul_bf_half_vmul_bf_half_core_X_X(
    MCInst &MI, InsnType &Insn, uint64_t Address,
    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eCMLDecoderTable));
  Register dstReg = eCMLDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mXvDecoderTable));
  Register s1Reg = mXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 4;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 11, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(mXwDecoderTable));
  Register s2Reg = mXwDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVNEGMUL_f_vmul_bfp13(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1RegRawNumBits = 4;
  unsigned s1RegRaw = fieldFromInstruction(Insn, 15, s1RegRawNumBits);
  static_assert((1 << s1RegRawNumBits) == std::size(mFEXvDecoderTable));
  Register s1Reg = mFEXvDecoderTable[s1RegRaw];
  if (!s1Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1Reg));

  const unsigned s2RegRawNumBits = 3;
  unsigned s2RegRaw = fieldFromInstruction(Insn, 12, s2RegRawNumBits);
  static_assert((1 << s2RegRawNumBits) == std::size(eFEYDecoderTable));
  Register s2Reg = eFEYDecoderTable[s2RegRaw];
  if (!s2Reg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2Reg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVNEGMUL_f_vmul_bfp16(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned dstRegRawNumBits = 3;
  unsigned dstRegRaw = fieldFromInstruction(Insn, 19, dstRegRawNumBits);
  static_assert((1 << dstRegRawNumBits) == std::size(eDMDecoderTable));
  Register dstReg = eDMDecoderTable[dstRegRaw];
  if (!dstReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dstReg));

  // --------------------- Ins ------------------------
  const unsigned s1lRegRawNumBits = 4;
  unsigned s1lRegRaw = fieldFromInstruction(Insn, 15, s1lRegRawNumBits);
  static_assert((1 << s1lRegRawNumBits) == std::size(mEWlv_bisDecoderTable));
  Register s1lReg = mEWlv_bisDecoderTable[s1lRegRaw];
  if (!s1lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1lReg));

  const unsigned s2lRegRawNumBits = 2;
  unsigned s2lRegRaw = fieldFromInstruction(Insn, 13, s2lRegRawNumBits);
  static_assert((1 << s2lRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2lReg = eEYsDecoderTable[s2lRegRaw];
  if (!s2lReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2lReg));

  const unsigned s2hRegRawNumBits = 2;
  unsigned s2hRegRaw = fieldFromInstruction(Insn, 11, s2hRegRawNumBits);
  static_assert((1 << s2hRegRawNumBits) == std::size(eEYsDecoderTable));
  Register s2hReg = eEYsDecoderTable[s2hRegRaw];
  if (!s2hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s2hReg));

  const unsigned s1hRegRawNumBits = 4;
  unsigned s1hRegRaw = fieldFromInstruction(Insn, 15, s1hRegRawNumBits);
  static_assert((1 << s1hRegRawNumBits) == std::size(mEWhvDecoderTable));
  Register s1hReg = mEWhvDecoderTable[s1hRegRaw];
  if (!s1hReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(s1hReg));

  const unsigned accRegRawNumBits = 3;
  unsigned accRegRaw = fieldFromInstruction(Insn, 22, accRegRawNumBits);
  static_assert((1 << accRegRawNumBits) == std::size(mRv8DecoderTable));
  Register accReg = mRv8DecoderTable[accRegRaw];
  if (!accReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(accReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_2D_128_dmv_sts_eg(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mEG2sDecoderTable));
  Register srcReg = mEG2sDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_2D_128_dmv_sts_feg(MCInst &MI, InsnType &Insn, uint64_t Address,
                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mFEGsDecoderTable));
  Register srcReg = mFEGsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_2D_128_dmv_sts_w(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mWs_128bDecoderTable));
  Register srcReg = mWs_128bDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_2D_CONV_bf16_fp32_dmw_sts_srs_bf16(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mBMsDecoderTable));
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_2D_CONV(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mCMsDecoderTable));
  Register srcReg = mCMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_2D_CONV_1(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mCMsDecoderTable));
  Register srcReg = mCMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned iRegRawNumBits = 2;
  unsigned iRegRaw = fieldFromInstruction(Insn, 6, iRegRawNumBits);
  static_assert((1 << iRegRawNumBits) == std::size(eSDecoderTable));
  Register iReg = eSDecoderTable[iRegRaw];
  if (!iReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(iReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_2D_PACK_dmw_sts_pack_packS(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mXsDecoderTable));
  Register srcReg = mXsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_2D_PACK_dmx_sts_pack_packS(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 3;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 9, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(eYDecoderTable));
  Register srcReg = eYDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_2D_SRS(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mCMsDecoderTable));
  Register srcReg = mCMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned baseRegRawNumBits = 2;
  unsigned baseRegRaw = fieldFromInstruction(Insn, 5, baseRegRawNumBits);
  static_assert((1 << baseRegRawNumBits) == std::size(eSDecoderTable));
  Register baseReg = eSDecoderTable[baseRegRaw];
  if (!baseReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(baseReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_2D_SRS_2x_dmw_sts_srs_bm_srsS(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mBMsDecoderTable));
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned baseRegRawNumBits = 2;
  unsigned baseRegRaw = fieldFromInstruction(Insn, 5, baseRegRawNumBits);
  static_assert((1 << baseRegRawNumBits) == std::size(eSDecoderTable));
  Register baseReg = eSDecoderTable[baseRegRaw];
  if (!baseReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(baseReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_2D_SRS_4x_dmx_sts_srs_dm_srsS(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 3;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 9, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(eDMDecoderTable));
  Register srcReg = eDMDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned baseRegRawNumBits = 2;
  unsigned baseRegRaw = fieldFromInstruction(Insn, 5, baseRegRawNumBits);
  static_assert((1 << baseRegRawNumBits) == std::size(eSDecoderTable));
  Register baseReg = eSDecoderTable[baseRegRaw];
  if (!baseReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(baseReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_2D_dmw_sts_feg2(MCInst &MI, InsnType &Insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mFEG2sDecoderTable));
  Register srcReg = mFEG2sDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_2D_dmw_sts_w(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mWsDecoderTable));
  Register srcReg = mWsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_2D_dmx_sts_bm(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mBMs_v16w32DecoderTable));
  Register srcReg = mBMs_v16w32DecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_2D_dmx_sts_x(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mXs_splitDecoderTable));
  Register srcReg = mXs_splitDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_3D_128_dmv_sts_eg(MCInst &MI, InsnType &Insn,
                                                uint64_t Address,
                                                const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mEG2sDecoderTable));
  Register srcReg = mEG2sDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_3D_128_dmv_sts_feg(MCInst &MI, InsnType &Insn, uint64_t Address,
                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mFEGsDecoderTable));
  Register srcReg = mFEGsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_3D_128_dmv_sts_w(MCInst &MI, InsnType &Insn,
                                               uint64_t Address,
                                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mWs_128bDecoderTable));
  Register srcReg = mWs_128bDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_3D_CONV_bf16_fp32_dmw_sts_srs_bf16(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mBMsDecoderTable));
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_3D_CONV(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mCMsDecoderTable));
  Register srcReg = mCMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_3D_CONV_1(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mCMsDecoderTable));
  Register srcReg = mCMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned iRegRawNumBits = 2;
  unsigned iRegRaw = fieldFromInstruction(Insn, 6, iRegRawNumBits);
  static_assert((1 << iRegRawNumBits) == std::size(eSDecoderTable));
  Register iReg = eSDecoderTable[iRegRaw];
  if (!iReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(iReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_3D_PACK_dmw_sts_pack_packS(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mXsDecoderTable));
  Register srcReg = mXsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_3D_PACK_dmx_sts_pack_packS(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 3;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 9, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(eYDecoderTable));
  Register srcReg = eYDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_3D_SRS(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mCMsDecoderTable));
  Register srcReg = mCMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned baseRegRawNumBits = 2;
  unsigned baseRegRaw = fieldFromInstruction(Insn, 5, baseRegRawNumBits);
  static_assert((1 << baseRegRawNumBits) == std::size(eSDecoderTable));
  Register baseReg = eSDecoderTable[baseRegRaw];
  if (!baseReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(baseReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_3D_SRS_2x_dmw_sts_srs_bm_srsS(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mBMsDecoderTable));
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned baseRegRawNumBits = 2;
  unsigned baseRegRaw = fieldFromInstruction(Insn, 5, baseRegRawNumBits);
  static_assert((1 << baseRegRawNumBits) == std::size(eSDecoderTable));
  Register baseReg = eSDecoderTable[baseRegRaw];
  if (!baseReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(baseReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVST_3D_SRS_4x_dmx_sts_srs_dm_srsS(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 3;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 9, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(eDMDecoderTable));
  Register srcReg = eDMDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned baseRegRawNumBits = 2;
  unsigned baseRegRaw = fieldFromInstruction(Insn, 5, baseRegRawNumBits);
  static_assert((1 << baseRegRawNumBits) == std::size(eSDecoderTable));
  Register baseReg = eSDecoderTable[baseRegRaw];
  if (!baseReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(baseReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_3D_dmw_sts_feg2(MCInst &MI, InsnType &Insn,
                                              uint64_t Address,
                                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mFEG2sDecoderTable));
  Register srcReg = mFEG2sDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_3D_dmw_sts_w(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mWsDecoderTable));
  Register srcReg = mWsDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_3D_dmx_sts_bm(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 5;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mBMs_v16w32DecoderTable));
  Register srcReg = mBMs_v16w32DecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_3D_dmx_sts_x(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const unsigned ptr_outRegRawNumBits = 3;
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 18, ptr_outRegRawNumBits);
  static_assert((1 << ptr_outRegRawNumBits) == std::size(ePDecoderTable));
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  if (!ptr_outReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const unsigned srcRegRawNumBits = 4;
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, srcRegRawNumBits);
  static_assert((1 << srcRegRawNumBits) == std::size(mXs_splitDecoderTable));
  Register srcReg = mXs_splitDecoderTable[srcRegRaw];
  if (!srcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(srcReg));

  const unsigned ptrRegRawNumBits = 3;
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 18, ptrRegRawNumBits);
  static_assert((1 << ptrRegRawNumBits) == std::size(ePDecoderTable));
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  if (!ptrReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(ptrReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_FLUSH_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const TargetRegisterClass &fifo_reg_outRC = AIE2PS::mStFifoRegClass;
  if (fifo_reg_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned fifo_reg_outReg = fifo_reg_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const TargetRegisterClass &ptr_outRC = AIE2PS::mPfsRegClass;
  if (ptr_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned ptr_outReg = ptr_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const TargetRegisterClass &avail_outRC = AIE2PS::mR26_fifo_stRegClass;
  if (avail_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned avail_outReg = avail_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(avail_outReg));

  const unsigned dcRegRawNumBits = 3;
  unsigned dcRegRaw = fieldFromInstruction(Insn, 15, dcRegRawNumBits);
  static_assert((1 << dcRegRawNumBits) == std::size(eDCDecoderTable));
  Register dcReg = eDCDecoderTable[dcRegRaw];
  if (!dcReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  const TargetRegisterClass &fifo_regRC = AIE2PS::mStFifoRegClass;
  if (fifo_regRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned fifo_regReg = fifo_regRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const TargetRegisterClass &ptrRC = AIE2PS::mPfsRegClass;
  if (ptrRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned ptrReg = ptrRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(ptrReg));

  const TargetRegisterClass &availRC = AIE2PS::mR26_fifo_stRegClass;
  if (availRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned availReg = availRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(availReg));

  const unsigned modRegRawNumBits = 3;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDDecoderTable));
  Register modReg = eDDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVST_FLUSH_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  const TargetRegisterClass &fifo_reg_outRC = AIE2PS::mStFifoRegClass;
  if (fifo_reg_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned fifo_reg_outReg = fifo_reg_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  const TargetRegisterClass &ptr_outRC = AIE2PS::mPfsRegClass;
  if (ptr_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned ptr_outReg = ptr_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  const TargetRegisterClass &avail_outRC = AIE2PS::mR26_fifo_stRegClass;
  if (avail_outRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned avail_outReg = avail_outRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(avail_outReg));

  const unsigned dclRegRawNumBits = 2;
  unsigned dclRegRaw = fieldFromInstruction(Insn, 15, dclRegRawNumBits);
  static_assert((1 << dclRegRawNumBits) == std::size(eDCLDecoderTable));
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  if (!dclReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dclReg));

  const unsigned dchRegRawNumBits = 2;
  unsigned dchRegRaw = fieldFromInstruction(Insn, 15, dchRegRawNumBits);
  static_assert((1 << dchRegRawNumBits) == std::size(eDCHDecoderTable));
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  if (!dchReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  const TargetRegisterClass &fifo_regRC = AIE2PS::mStFifoRegClass;
  if (fifo_regRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned fifo_regReg = fifo_regRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  const TargetRegisterClass &ptrRC = AIE2PS::mPfsRegClass;
  if (ptrRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned ptrReg = ptrRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(ptrReg));

  const TargetRegisterClass &availRC = AIE2PS::mR26_fifo_stRegClass;
  if (availRC.getNumRegs() != 1)
    return MCDisassembler::Fail;
  unsigned availReg = availRC.getRegister(0);
  MI.addOperand(MCOperand::createReg(availReg));

  const unsigned modRegRawNumBits = 2;
  unsigned modRegRaw = fieldFromInstruction(Insn, 15, modRegRawNumBits);
  static_assert((1 << modRegRawNumBits) == std::size(eDSDecoderTable));
  Register modReg = eDSDecoderTable[modRegRaw];
  if (!modReg)
    return MCDisassembler::Fail;
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}
