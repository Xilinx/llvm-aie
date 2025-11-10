//===-- AIEBaseDisassembler.h - Disassembler for AIE --------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines common utils for AIEx disassemblers
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_DISASSEMBLER_AIEBASEDISASSEMBLER_H
#define LLVM_LIB_TARGET_AIE_DISASSEMBLER_AIEBASEDISASSEMBLER_H

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
using DecodeStatus = MCDisassembler::DecodeStatus;

namespace {
template <unsigned N>
static DecodeStatus decodeUImmOperand(MCInst &Inst, uint64_t Imm,
                                      int64_t Address,
                                      const MCDisassembler *Decoder) {
  assert(isUInt<N>(Imm) && "Invalid immediate");
  // FIXME?  addImplySP(Inst, Address, Decoder);
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

template <unsigned N>
static DecodeStatus decodeUImmNonZeroOperand(MCInst &Inst, uint64_t Imm,
                                             int64_t Address,
                                             const MCDisassembler *Decoder) {
  if (Imm == 0)
    return MCDisassembler::Fail;
  return decodeUImmOperand<N>(Inst, Imm, Address, Decoder);
}

template <unsigned N>
static DecodeStatus decodeSImmOperand(MCInst &Inst, uint64_t Imm,
                                      int64_t Address,
                                      const MCDisassembler *Decoder) {
  assert(isUInt<N>(Imm) && "Invalid immediate");
  // FIXME?  addImplySP(Inst, Address, Decoder);
  // Sign-extend the number in the bottom N bits of Imm
  Inst.addOperand(MCOperand::createImm(SignExtend64<N>(Imm)));
  return MCDisassembler::Success;
}

template <unsigned N>
static DecodeStatus decodeSImmNonZeroOperand(MCInst &Inst, uint64_t Imm,
                                             int64_t Address,
                                             const MCDisassembler *Decoder) {
  if (Imm == 0)
    return MCDisassembler::Fail;
  return decodeSImmOperand<N>(Inst, Imm, Address, Decoder);
}

template <unsigned N>
static DecodeStatus decodeSImmOperandX1(MCInst &Inst, uint64_t Imm,
                                        int64_t Address,
                                        const MCDisassembler *Decoder) {
  assert(isUInt<N>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(SignExtend64<N + 1>(Imm)));
  return MCDisassembler::Success;
}

template <unsigned N, unsigned step, bool isNegative, bool isUnsigned>
static DecodeStatus decodeSImmOperandXStep(MCInst &Inst, uint64_t Imm,
                                           int64_t Address,
                                           const MCDisassembler *Decoder) {
  assert(isUInt<N>(Imm) && "Invalid immediate");
  const unsigned FixedZeroBits = CTLog2<step>();
  if (isNegative)
    // Decode and sign extend a negative number encoded as fixed 1
    // <encoded_length bits> fixedZeroBits
    Inst.addOperand(MCOperand::createImm(
        SignExtend64((Imm << FixedZeroBits) | 1 << (N + FixedZeroBits),
                     N + 1 + FixedZeroBits)));
  else if (isUnsigned)
    // Decode an unsigned N+fixedZeroBits bit number encoded in N bits,
    // with the LSBs as zero.
    Inst.addOperand(MCOperand::createImm(Imm << FixedZeroBits));
  else
    // Decode and sign extend an N+fixedZeroBits bit number encoded in N bits,
    // with the LSBs as zero.
    Inst.addOperand(MCOperand::createImm(
        SignExtend64(Imm << FixedZeroBits, N + FixedZeroBits)));
  return MCDisassembler::Success;
}
} // namespace

#endif // LLVM_LIB_TARGET_AIE_DISASSEMBLER_AIEBASEDISASSEMBLER_H
