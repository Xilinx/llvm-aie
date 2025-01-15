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
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 7);
  if (dstRegRaw >= std::size(mLdaSclDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mLdaSclDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_lda_q_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, 2);
  if (dstRegRaw >= std::size(mQQsaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQQsaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmhb_lda_2D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (dstRegRaw >= std::size(eRDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = eRDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDms_lda_3D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 7);
  if (dstRegRaw >= std::size(mLdaSclDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mLdaSclDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_lda_q_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, 2);
  if (dstRegRaw >= std::size(mQQsaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQQsaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmhb_lda_3D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (dstRegRaw >= std::size(eRDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = eRDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeTm_lda_2D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (dstRegRaw >= std::size(eRDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = eRDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeTm_lda_3D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (dstRegRaw >= std::size(eRDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = eRDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLda_ptr_inc_2D(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLda_ptr_inc_3D(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLdb_or1_2D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeLdb_or1_3D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeSt_ptr_inc_2D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeSt_ptr_inc_3D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDms_sts_2D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 4, 7);
  if (srcRegRaw >= std::size(mSclStDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mSclStDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_sts_q_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 9, 2);
  if (srcRegRaw >= std::size(mQQssDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mQQssDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmhb_sts_2D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (srcRegRaw >= std::size(eRDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = eRDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDms_sts_3D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 4, 7);
  if (srcRegRaw >= std::size(mSclStDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mSclStDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_sts_q_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 9, 2);
  if (srcRegRaw >= std::size(mQQssDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mQQssDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmhb_sts_3D(MCInst &MI, InsnType &Insn,
                                      uint64_t Address,
                                      const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (srcRegRaw >= std::size(eRDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = eRDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeTm_sts_2D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (srcRegRaw >= std::size(eRDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = eRDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeTm_sts_3D(MCInst &MI, InsnType &Insn,
                                    uint64_t Address,
                                    const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (srcRegRaw >= std::size(eRDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = eRDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_lda_w_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (dstRegRaw >= std::size(mWaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mWaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_bf_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned opRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (opRegRaw >= std::size(mBMmDecoderTable))
    return MCDisassembler::Fail;
  Register opReg = mBMmDecoderTable[opRegRaw];
  assert(opReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(opReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_bf_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned opRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (opRegRaw >= std::size(mCMmDecoderTable))
    return MCDisassembler::Fail;
  Register opReg = mCMmDecoderTable[opRegRaw];
  assert(opReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(opReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_w2b_2D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (dstRegRaw >= std::size(mBMmDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mBMmDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned suRegRaw = fieldFromInstruction(Insn, 4, 2);
  if (suRegRaw >= std::size(eSDecoderTable))
    return MCDisassembler::Fail;
  Register suReg = eSDecoderTable[suRegRaw];
  assert(suReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(suReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_x2c_2D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mCMmDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mCMmDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned suRegRaw = fieldFromInstruction(Insn, 5, 2);
  if (suRegRaw >= std::size(eSDecoderTable))
    return MCDisassembler::Fail;
  Register suReg = eSDecoderTable[suRegRaw];
  assert(suReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(suReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_w2c_2D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mCMmDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mCMmDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned suRegRaw = fieldFromInstruction(Insn, 5, 2);
  if (suRegRaw >= std::size(eSDecoderTable))
    return MCDisassembler::Fail;
  Register suReg = eSDecoderTable[suRegRaw];
  assert(suReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(suReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_x2d_2D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, 3);
  if (dstRegRaw >= std::size(eDMDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = eDMDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned suRegRaw = fieldFromInstruction(Insn, 6, 2);
  if (suRegRaw >= std::size(eSDecoderTable))
    return MCDisassembler::Fail;
  Register suReg = eSDecoderTable[suRegRaw];
  assert(suReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(suReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_w_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (dstRegRaw >= std::size(mWaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mWaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_bm_2D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (dstRegRaw >= std::size(mBMmDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mBMmDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_fifohl_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, 3);
  if (dstRegRaw >= std::size(mFifoHLRegDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mFifoHLRegDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_x_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mXaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mXaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_lda_w_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (dstRegRaw >= std::size(mWaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mWaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_bf_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned opRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (opRegRaw >= std::size(mBMmDecoderTable))
    return MCDisassembler::Fail;
  Register opReg = mBMmDecoderTable[opRegRaw];
  assert(opReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(opReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_bf_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned opRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (opRegRaw >= std::size(mCMmDecoderTable))
    return MCDisassembler::Fail;
  Register opReg = mCMmDecoderTable[opRegRaw];
  assert(opReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(opReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_w2b_3D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (dstRegRaw >= std::size(mBMmDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mBMmDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned suRegRaw = fieldFromInstruction(Insn, 4, 2);
  if (suRegRaw >= std::size(eSDecoderTable))
    return MCDisassembler::Fail;
  Register suReg = eSDecoderTable[suRegRaw];
  assert(suReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(suReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_x2c_3D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mCMmDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mCMmDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned suRegRaw = fieldFromInstruction(Insn, 5, 2);
  if (suRegRaw >= std::size(eSDecoderTable))
    return MCDisassembler::Fail;
  Register suReg = eSDecoderTable[suRegRaw];
  assert(suReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(suReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_ups_w2c_3D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mCMmDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mCMmDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned suRegRaw = fieldFromInstruction(Insn, 5, 2);
  if (suRegRaw >= std::size(eSDecoderTable))
    return MCDisassembler::Fail;
  Register suReg = eSDecoderTable[suRegRaw];
  assert(suReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(suReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_ups_x2d_3D(MCInst &MI, InsnType &Insn,
                                             uint64_t Address,
                                             const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, 3);
  if (dstRegRaw >= std::size(eDMDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = eDMDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned suRegRaw = fieldFromInstruction(Insn, 6, 2);
  if (suRegRaw >= std::size(eSDecoderTable))
    return MCDisassembler::Fail;
  Register suReg = eSDecoderTable[suRegRaw];
  assert(suReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(suReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_lda_w_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (dstRegRaw >= std::size(mWaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mWaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_bm_3D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (dstRegRaw >= std::size(mBMmDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mBMmDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_fifohl_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 8, 3);
  if (dstRegRaw >= std::size(mFifoHLRegDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mFifoHLRegDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_lda_x_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mXaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mXaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDA_FILL_512(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_x__fifo_2d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mXaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mXaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_x__fifo_3d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mXaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mXaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_512_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mXaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mXaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eMDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eMDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_512_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mXaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mXaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_ex_ebs16__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mEXaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_ex_ebs16__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mEXaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_544_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mEXaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eMDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eMDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_544_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mEXaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_ex_ebs8__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mEXaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_ex_ebs8__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mEXaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_576_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mEXaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eMDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eMDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_576_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (dstRegRaw >= std::size(mEXaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_qx__fifo_2d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, 2);
  if (dstRegRaw >= std::size(mQXsaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQXsaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_qx__fifo_3d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, 2);
  if (dstRegRaw >= std::size(mQXsaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQXsaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_640_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, 2);
  if (dstRegRaw >= std::size(mQXsaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQXsaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eMDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eMDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_640_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, 2);
  if (dstRegRaw >= std::size(mQXsaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQXsaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_qex_ebs16__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, 2);
  if (dstRegRaw >= std::size(mQEXsaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQEXsaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_lda_fifo_qex_ebs16__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, 2);
  if (dstRegRaw >= std::size(mQEXsaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQEXsaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_704_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, 2);
  if (dstRegRaw >= std::size(mQEXsaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQEXsaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eMDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eMDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDA_POP_704_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 9, 2);
  if (dstRegRaw >= std::size(mQEXsaDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQEXsaDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 17, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_ldb_2D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 3, 5);
  if (dstRegRaw >= std::size(mWbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mWbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_ldb_unpack_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_ldb_unpack_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 5, 3);
  if (dstRegRaw >= std::size(eYDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = eYDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_ldb_2D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 3, 5);
  if (dstRegRaw >= std::size(mWbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mWbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_ldb_x_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_ldb_3D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 3, 5);
  if (dstRegRaw >= std::size(mWbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mWbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_ldb_unpack_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_ldb_unpack_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 5, 3);
  if (dstRegRaw >= std::size(eYDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = eYDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_ldb_3D(MCInst &MI, InsnType &Insn,
                                     uint64_t Address,
                                     const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 3, 5);
  if (dstRegRaw >= std::size(mWbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mWbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_ldb_x_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDB_FILLX_512(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
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

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDB_FILL_512(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeVLDB_POPX_512(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
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

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_x__fifo_2d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_x__fifo_3d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                  const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_512_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eMDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eMDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_512_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_ex_ebs16__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mEXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_ex_ebs16__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                         uint64_t Address,
                                         const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mEXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_544_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mEXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eMDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eMDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_544_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mEXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_ex_ebs8__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mEXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_ex_ebs8__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mEXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_576_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mEXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eMDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eMDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_576_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 4, 4);
  if (dstRegRaw >= std::size(mEXbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mEXbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_qx__fifo_2d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 2);
  if (dstRegRaw >= std::size(mQXsbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQXsbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_qx__fifo_3d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                                   const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 2);
  if (dstRegRaw >= std::size(mQXsbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQXsbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_640_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 2);
  if (dstRegRaw >= std::size(mQXsbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQXsbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eMDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eMDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_640_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 2);
  if (dstRegRaw >= std::size(mQXsbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQXsbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_qex_ebs16__fifo_2d_pop(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 2);
  if (dstRegRaw >= std::size(mQEXsbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQEXsbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeDmx_ldb_fifo_qex_ebs16__fifo_3d_pop(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 2);
  if (dstRegRaw >= std::size(mQEXsbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQEXsbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_704_fifo_1d_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                               const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 2);
  if (dstRegRaw >= std::size(mQEXsbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQEXsbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 11, 3);
  if (modRegRaw >= std::size(eMDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eMDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus
DecodeVLDB_POP_704_normal_pop(MCInst &MI, InsnType &Insn, uint64_t Address,
                              const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned dstRegRaw = fieldFromInstruction(Insn, 6, 2);
  if (dstRegRaw >= std::size(mQEXsbDecoderTable))
    return MCDisassembler::Fail;
  Register dstReg = mQEXsbDecoderTable[dstRegRaw];
  assert(dstReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dstReg));

  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptr_outRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePSDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned fifo_reg_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_reg_outRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_reg_outReg = eLdFifoRegDecoderTable[fifo_reg_outRegRaw];
  assert(fifo_reg_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_reg_outReg));

  unsigned pos_outRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (pos_outRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register pos_outReg = eRF2DecoderTable[pos_outRegRaw];
  assert(pos_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(pos_outReg));

  // --------------------- Ins ------------------------
  unsigned ptrRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (ptrRegRaw >= std::size(ePSDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePSDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned fifo_regRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (fifo_regRegRaw >= std::size(eLdFifoRegDecoderTable))
    return MCDisassembler::Fail;
  Register fifo_regReg = eLdFifoRegDecoderTable[fifo_regRegRaw];
  assert(fifo_regReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(fifo_regReg));

  unsigned posRegRaw = fieldFromInstruction(Insn, 14, 1);
  if (posRegRaw >= std::size(eRF2DecoderTable))
    return MCDisassembler::Fail;
  Register posReg = eRF2DecoderTable[posRegRaw];
  assert(posReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(posReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_sts_w_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (srcRegRaw >= std::size(mWsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mWsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_srs_bf_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (srcRegRaw >= std::size(mBMsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_srs_bf_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (srcRegRaw >= std::size(mCMsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mCMsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_pack_2D(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (srcRegRaw >= std::size(mXsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mXsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_pack_2D(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, 3);
  if (srcRegRaw >= std::size(eYDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = eYDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDm_sts_srs_cm_2D(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (srcRegRaw >= std::size(mCMsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mCMsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned suRegRaw = fieldFromInstruction(Insn, 4, 2);
  if (suRegRaw >= std::size(eSDecoderTable))
    return MCDisassembler::Fail;
  Register suReg = eSDecoderTable[suRegRaw];
  assert(suReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(suReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_srs_bm_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (srcRegRaw >= std::size(mBMsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned suRegRaw = fieldFromInstruction(Insn, 4, 2);
  if (suRegRaw >= std::size(eSDecoderTable))
    return MCDisassembler::Fail;
  Register suReg = eSDecoderTable[suRegRaw];
  assert(suReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(suReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_srs_dm_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, 3);
  if (srcRegRaw >= std::size(eDMDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = eDMDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned suRegRaw = fieldFromInstruction(Insn, 4, 2);
  if (suRegRaw >= std::size(eSDecoderTable))
    return MCDisassembler::Fail;
  Register suReg = eSDecoderTable[suRegRaw];
  assert(suReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(suReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_w_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (srcRegRaw >= std::size(mWsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mWsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_bm_2D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (srcRegRaw >= std::size(mBMsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_fifohl_2D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, 3);
  if (srcRegRaw >= std::size(mFifoHLRegDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mFifoHLRegDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_x_2D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dcReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (srcRegRaw >= std::size(mXsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mXsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmv_sts_w_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (srcRegRaw >= std::size(mWsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mWsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_srs_bf_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (srcRegRaw >= std::size(mBMsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_srs_bf_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (srcRegRaw >= std::size(mCMsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mCMsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_pack_3D(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (srcRegRaw >= std::size(mXsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mXsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_pack_3D(MCInst &MI, InsnType &Insn,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, 3);
  if (srcRegRaw >= std::size(eYDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = eYDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDm_sts_srs_cm_3D(MCInst &MI, InsnType &Insn,
                                           uint64_t Address,
                                           const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (srcRegRaw >= std::size(mCMsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mCMsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned suRegRaw = fieldFromInstruction(Insn, 4, 2);
  if (suRegRaw >= std::size(eSDecoderTable))
    return MCDisassembler::Fail;
  Register suReg = eSDecoderTable[suRegRaw];
  assert(suReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(suReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_srs_bm_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (srcRegRaw >= std::size(mBMsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned suRegRaw = fieldFromInstruction(Insn, 4, 2);
  if (suRegRaw >= std::size(eSDecoderTable))
    return MCDisassembler::Fail;
  Register suReg = eSDecoderTable[suRegRaw];
  assert(suReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(suReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_srs_dm_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, 3);
  if (srcRegRaw >= std::size(eDMDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = eDMDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned suRegRaw = fieldFromInstruction(Insn, 4, 2);
  if (suRegRaw >= std::size(eSDecoderTable))
    return MCDisassembler::Fail;
  Register suReg = eSDecoderTable[suRegRaw];
  assert(suReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(suReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmw_sts_w_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (srcRegRaw >= std::size(mWsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mWsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_bm_3D(MCInst &MI, InsnType &Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 6, 5);
  if (srcRegRaw >= std::size(mBMsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mBMsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_fifohl_3D(MCInst &MI, InsnType &Insn,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 8, 3);
  if (srcRegRaw >= std::size(mFifoHLRegDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mFifoHLRegDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}

template <typename InsnType>
static DecodeStatus DecodeDmx_sts_x_3D(MCInst &MI, InsnType &Insn,
                                       uint64_t Address,
                                       const MCDisassembler *Decoder) {
  // ------------------- Outs ------------------------
  unsigned ptr_outRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptr_outRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptr_outReg = ePDecoderTable[ptr_outRegRaw];
  assert(ptr_outReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptr_outReg));

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dchReg));

  // --------------------- Ins ------------------------
  unsigned srcRegRaw = fieldFromInstruction(Insn, 7, 4);
  if (srcRegRaw >= std::size(mXsDecoderTable))
    return MCDisassembler::Fail;
  Register srcReg = mXsDecoderTable[srcRegRaw];
  assert(srcReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(srcReg));

  unsigned ptrRegRaw = fieldFromInstruction(Insn, 17, 3);
  if (ptrRegRaw >= std::size(ePDecoderTable))
    return MCDisassembler::Fail;
  Register ptrReg = ePDecoderTable[ptrRegRaw];
  assert(ptrReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(ptrReg));

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
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

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
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

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
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

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
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

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
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

  unsigned dcRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (dcRegRaw >= std::size(eDCDecoderTable))
    return MCDisassembler::Fail;
  Register dcReg = eDCDecoderTable[dcRegRaw];
  assert(dcReg && "Register not found.");
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

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 3);
  if (modRegRaw >= std::size(eDDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
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

  unsigned dclRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dclRegRaw >= std::size(eDCLDecoderTable))
    return MCDisassembler::Fail;
  Register dclReg = eDCLDecoderTable[dclRegRaw];
  assert(dclReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(dclReg));

  unsigned dchRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (dchRegRaw >= std::size(eDCHDecoderTable))
    return MCDisassembler::Fail;
  Register dchReg = eDCHDecoderTable[dchRegRaw];
  assert(dchReg && "Register not found.");
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

  unsigned modRegRaw = fieldFromInstruction(Insn, 14, 2);
  if (modRegRaw >= std::size(eDSDecoderTable))
    return MCDisassembler::Fail;
  Register modReg = eDSDecoderTable[modRegRaw];
  assert(modReg && "Register not found.");
  MI.addOperand(MCOperand::createReg(modReg));

  return MCDisassembler::Success;
}