//===-- AIE2PSDisassembler.cpp - Disassembler for AIE2p -------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIE2PSDisassembler class.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseDisassembler.h"
#include "AIEDisassemblerPP.h"
#include "MCTargetDesc/aie2ps/AIE2PSMCTargetDesc.h"
#include "aie2ps/AIE2PSRegisterInfo.h"
#include "llvm/MC/MCDecoderOps.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Format.h"

using namespace llvm;

#define DEBUG_TYPE "aie2ps-disassembler"

using namespace AIE2PS;

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {
class AIE2PSDisassembler : public MCDisassembler {

public:
  AIE2PSDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
      : MCDisassembler(STI, Ctx) {}

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &CStream) const override;
};
} // end anonymous namespace

MCDisassembler *createAIE2PSDisassembler(const Target &T,
                                         const MCSubtargetInfo &STI,
                                         MCContext &Ctx) {
  return new AIE2PSDisassembler(STI, Ctx);
}

// Include generated operand decoder tables and methods.
#include "AIE2PSDisassembler.inc"

namespace {
SLOTDECODERDecl(Lda);
SLOTDECODERDecl(Ldb);
SLOTDECODERDecl(Alu);
SLOTDECODERDecl(St);
SLOTDECODERDecl(Mv);
SLOTDECODERDecl(Vec);
SLOTDECODERDecl(Lng);
SLOTDECODERDecl(Nop);
} // namespace

#include "AIE2PSGenDecoderMethods.h"

#include "AIE2PSGenDisassemblerTables.inc"

#include "AIE2PSGenDecoderMethods.inc"

namespace {
/// Slot decoders
template <typename InsnType>
DecodeStatus decodeAIE2PSSlot(const uint8_t *DecoderTable, MCInst &Inst,
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

template <typename InsnType>
DecodeStatus decodeNopSlot(MCInst &Inst, InsnType &Imm, int64_t Address,
                           const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Nop Instruction\n");
  return decodeAIE2PSSlot(DecoderTableNop16, Inst, Imm, Address, Decoder);
}

template <typename InsnType>
DecodeStatus decodeLdaSlot(MCInst &Inst, InsnType &Imm, int64_t Address,
                           const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Lda Instruction\n");
  return decodeAIE2PSSlot(DecoderTableLda32, Inst, Imm, Address, Decoder);
}

template <typename InsnType>
DecodeStatus decodeLdbSlot(MCInst &Inst, InsnType &Imm, int64_t Address,
                           const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Ldb Instruction\n");
  return decodeAIE2PSSlot(DecoderTableLdb32, Inst, Imm, Address, Decoder);
}

template <typename InsnType>
DecodeStatus decodeAluSlot(MCInst &Inst, InsnType &Imm, int64_t Address,
                           const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Alu Instruction\n");
  return decodeAIE2PSSlot(DecoderTableAlu32, Inst, Imm, Address, Decoder);
}

template <typename InsnType>
DecodeStatus decodeMvSlot(MCInst &Inst, InsnType &Imm, int64_t Address,
                          const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Mv Instruction\n");
  return decodeAIE2PSSlot(DecoderTableMv32, Inst, Imm, Address, Decoder);
}

template <typename InsnType>
DecodeStatus decodeStSlot(MCInst &Inst, InsnType &Imm, int64_t Address,
                          const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode St Instruction\n");
  return decodeAIE2PSSlot(DecoderTableSt32, Inst, Imm, Address, Decoder);
}

template <typename InsnType>
DecodeStatus decodeVecSlot(MCInst &Inst, InsnType &Imm, int64_t Address,
                           const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Vec Instruction\n");
  return decodeAIE2PSSlot(DecoderTableVec32, Inst, Imm, Address, Decoder);
}

template <typename InsnType>
DecodeStatus decodeLngSlot(MCInst &Inst, InsnType &Imm, int64_t Address,
                           const MCDisassembler *Decoder) {
  LLVM_DEBUG(dbgs() << "Decode Lng Instruction\n");
  return decodeAIE2PSSlot(DecoderTableLng48, Inst, Imm, Address, Decoder);
}

/// Interpret the code bits to determine the format size.
int formatSize(ArrayRef<uint8_t> Bytes) {
  switch (Bytes[0] & 0b11111) {
  default:
    break;
  case 0b00001:
  case 0b00011:
  case 0b00101:
  case 0b00111:
  case 0b01001:
  case 0b01011:
  case 0b01101:
  case 0b01111:
  case 0b10001:
  case 0b10011:
  case 0b10101:
  case 0b10111:
  case 0b11001:
  case 0b11011:
  case 0b11101:
  case 0b11111:
    return 16;
  case 0b00000:
    return 2;
  case 0b00010:
  case 0b01010:
  case 0b10010:
  case 0b11010:
    return 8;
  case 0b00110:
  case 0b01110:
  case 0b10110:
  case 0b11110:
    return 6;
  case 0b00100:
  case 0b10100:
    return 12;
  case 0b10000:
    return 4;
  case 0b01000:
  case 0b11000:
    return 10;
  case 0b01100:
  case 0b11100:
    return 14;
  }
  dbgs() << format("0x%08x", Bytes[0]) << "\n";
  report_fatal_error("Unknown Instruction in Disassembler");
  return -1;
}

} // namespace

DecodeStatus AIE2PSDisassembler::getInstruction(MCInst &MI, uint64_t &Size,
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
