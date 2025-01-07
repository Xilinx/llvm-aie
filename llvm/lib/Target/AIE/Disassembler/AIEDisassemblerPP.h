//===--  AIEDisassemblerPP.h - Disassembler Preprocessor Macros for AIE --===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//
//
// This file defines common PP for AIEx
//
//===---------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_DISASSEMBLER_AIEPP_H
#define LLVM_LIB_TARGET_AIE_DISASSEMBLER_AIEPP_H

#define TABLEBASEDDECODER2(ClassName, TableClassName)                          \
  static DecodeStatus Decode##ClassName##RegisterClass(                        \
      MCInst &Inst, uint64_t RegNo, uint64_t Address,                          \
      const MCDisassembler *Decoder) {                                         \
    if (RegNo >= std::size(TableClassName##DecoderTable))                      \
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

#define SLOTDECODERDecl(ClassName)                                             \
  template <typename InsnType>                                                 \
  static DecodeStatus decode##ClassName##Slot(MCInst &MI, InsnType &Insn,      \
                                              int64_t Address,                 \
                                              const MCDisassembler *Decoder)

#endif // LLVM_LIB_TARGET_AIE_DISASSEMBLER_AIEPP_H
