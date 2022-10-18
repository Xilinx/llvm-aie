//===-- AIEMCCodeEmitter.cpp - Convert AIE code to machine code -------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIEMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseMCCodeEmitter.h"
#include "AIEInstrInfo.h"
#include "MCTargetDesc/AIE1MCFixupKinds.h"
#include "MCTargetDesc/AIEMCExpr.h"
#include "MCTargetDesc/AIEMCInstrInfo.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"
#include "Utils/AIEBaseInfo.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/ScopedPrinter.h"
#include "llvm/Support/raw_ostream.h"
#include <memory>

using namespace llvm;

#define DEBUG_TYPE "mccodeemitter"

namespace {
const AIEMCFormats AIE1Formats;
} // namespace

namespace {
class AIEMCCodeEmitter : public AIEBaseMCCodeEmitter {
  AIEMCCodeEmitter(const AIEMCCodeEmitter &) = delete;
  void operator=(const AIEMCCodeEmitter &) = delete;

public:
  AIEMCCodeEmitter(MCContext &ctx, MCInstrInfo const &MCII)
      : AIEBaseMCCodeEmitter(ctx, MCII, createAIE1MCFixupKinds(), AIE1Formats) {
  }

  void getBinaryCodeForInstr(const MCInst &MI, SmallVectorImpl<MCFixup> &Fixups,
                             APInt &Inst, APInt &Scratch,
                             const MCSubtargetInfo &STI) const override;

  ~AIEMCCodeEmitter() override {}
#define GET_IMMEDIATE_ARGS const MCInst &MI, unsigned OpNo, APInt &op, SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI

  // Return special binary encoding for various register classes.
  void getmMvSclOpValue(GET_IMMEDIATE_ARGS) const;
  void getmSclStOpValue(GET_IMMEDIATE_ARGS) const;
  void getmLdaSclOpValue(GET_IMMEDIATE_ARGS) const;
  void getmLdbSclOpValue(GET_IMMEDIATE_ARGS) const;
  void getmAluCg12OpValue(GET_IMMEDIATE_ARGS) const;
  void getmMv0Cg20OpValue(GET_IMMEDIATE_ARGS) const;
  void getmRCmOpValue(GET_IMMEDIATE_ARGS) const;
  void getmWBDvOpValue(GET_IMMEDIATE_ARGS) const;
  void getmXCDvOpValue(GET_IMMEDIATE_ARGS) const;
  void getmVnOpValue(GET_IMMEDIATE_ARGS) const;
};
} // end anonymous namespace

MCCodeEmitter *llvm::createAIEMCCodeEmitter(const MCInstrInfo &MCII,
                                              MCContext &Ctx) {
  return new AIEMCCodeEmitter(Ctx, MCII);
}

void
AIEMCCodeEmitter::getmMvSclOpValue(const MCInst &MI, unsigned OpNo, APInt &op,
                                    SmallVectorImpl<MCFixup> &Fixups,
                                    const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (!MO.isReg()) {
    llvm_unreachable("Unhandled expression!");
  }

  unsigned Res = MO.getReg();
  unsigned encoding = Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
  if(AIEMCRegisterClasses[AIE::GPRPTRMODRegClassID].contains(Res)) {
    // Encoding 0b1?????  This is completely specified
    // by the HWEncoding field in AIERegisterInfo.td
    op = encoding;
   } else if(AIEMCRegisterClasses[AIE::mCSRegClassID].contains(Res)) {
    // Encoding 0b011xxx
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op =  0x18;
  } else if(AIEMCRegisterClasses[AIE::mCBRegClassID].contains(Res)) {
    // Encoding 0b010xxx
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op =  0x10;
 } else if(AIEMCRegisterClasses[AIE::mCHRegClassID].contains(Res)) {
    // Encoding 0b000???
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op =  encoding;
  } else if(AIEMCRegisterClasses[AIE::mCLRegClassID].contains(Res)) {
    // Encoding 0b001???
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op =  encoding | 0x8;
  } else {
    llvm_unreachable("Unhandled register class!");
  }
}
void
AIEMCCodeEmitter::getmSclStOpValue(const MCInst &MI, unsigned OpNo, APInt &op,
                                    SmallVectorImpl<MCFixup> &Fixups,
                                    const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (!MO.isReg()) {
    llvm_unreachable("Unhandled expression!");
  }

  unsigned Res = MO.getReg();
  unsigned encoding = Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
  if(AIEMCRegisterClasses[AIE::GPRPTRMODRegClassID].contains(Res)) {
    //AIE::GPRPTRMODRegClass.contains(Res)) {
    // Encoding 0b1?????  This is completely specified
    // by the HWEncoding field in AIERegisterInfo.td
    op =  encoding;
  } else if(AIEMCRegisterClasses[AIE::mCHRegClassID].contains(Res)) {
    //AIE::mCHRegClass.contains(Res)) {
    // Encoding 0b010???
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op =  encoding | 0x10;
  } else if(AIEMCRegisterClasses[AIE::mCLRegClassID].contains(Res)) {
    //} else if(AIE::mCLRegClass.contains(Res)) {
    // Encoding 0b011???
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op =  encoding | 0x18;
  } else if(AIEMCRegisterClasses[AIE::LRRegClassID].contains(Res)) {
    //} else if(AIE::LRRegClass.contains(Res)) {
    // Encoding 0b001xxx
    // Info in AIEReigsterInfo.td is unuseful for this case.
    op =  0x08;
  } else {
    llvm_unreachable("Unhandled register class!");
  }
}

void
AIEMCCodeEmitter::getmLdaSclOpValue(const MCInst &MI, unsigned OpNo, APInt &op,
                                    SmallVectorImpl<MCFixup> &Fixups,
                                    const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (!MO.isReg()) {
    llvm_unreachable("Unhandled expression!");
  }

  unsigned Res = MO.getReg();
  unsigned encoding = Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
  if(AIEMCRegisterClasses[AIE::GPRPTRMODRegClassID].contains(Res)) {
    //AIE::GPRPTRMODRegClass.contains(Res)) {
    // Encoding 0b1?_????  This is completely specified
    // by the HWEncoding field in AIERegisterInfo.td
    op =  encoding;
  } else if(AIEMCRegisterClasses[AIE::mCHRegClassID].contains(Res)) {
    //AIE::mCHRegClass.contains(Res)) {
    // Encoding 0b01_0???
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op =  encoding | 0x10;
  } else if(AIEMCRegisterClasses[AIE::mCLRegClassID].contains(Res)) {
    //} else if(AIE::mCLRegClass.contains(Res)) {
    // Encoding 0b01_1???
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op =  encoding | 0x18;
  } else if(AIEMCRegisterClasses[AIE::mCSRegClassID].contains(Res)) {
    //} else if(AIE::LRRegClass.contains(Res)) {
    // Encoding 0b00_1xxx
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op = encoding | 0x08;
  } else if(AIEMCRegisterClasses[AIE::mCBRegClassID].contains(Res)) {
    //} else if(AIE::LRRegClass.contains(Res)) {
    // Encoding 0b00_0xxx
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op = encoding | 0x00;
  } else {
    llvm_unreachable("Unhandled register class!");
  }
}

void
AIEMCCodeEmitter::getmLdbSclOpValue(const MCInst &MI, unsigned OpNo, APInt &op,
                                    SmallVectorImpl<MCFixup> &Fixups,
                                    const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (!MO.isReg()) {
    llvm_unreachable("Unhandled expression!");
  }

  unsigned Res = MO.getReg();
  unsigned encoding = Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
  if(AIEMCRegisterClasses[AIE::GPRRegClassID].contains(Res)) {
    // Encoding 0b1?_???1  4 bits from from AIERegisterInfo.td
    op =  ((encoding & 0xF) << 1) | 0x21;
  } else if(AIEMCRegisterClasses[AIE::PTRRegClassID].contains(Res)) {
    // Encoding 0b0?_??11  3 bits specified in AIERegisterInfo.td
    op =  ((encoding & 0x7) << 2) | 0x03;
  } else if(AIEMCRegisterClasses[AIE::MODRegClassID].contains(Res)) {
    // Encoding 0b0?_??10  3 bits specified in AIERegisterInfo.td
    op =  ((encoding & 0x7) << 2) | 0x02;
  } else if(AIEMCRegisterClasses[AIE::mCHRegClassID].contains(Res)) {
    // Encoding 0b1?_??10  3 bits specified in AIERegisterInfo.td
    op =  ((encoding & 0x7) << 2) | 0x22;
  } else if(AIEMCRegisterClasses[AIE::mCLRegClassID].contains(Res)) {
    // Encoding 0b1?_??00  3 bits specified in AIERegisterInfo.td
    op =  ((encoding & 0x7) << 2) | 0x20;
  } else if(AIE::lr == Res) {
    // Encoding 0b01_100x
    // Info in AIEReigsterInfo.td is unuseful for this case.
    op =  0x18;
  } else if(AIE::LS == Res) {
    // Encoding 0b01_110x
    // Info in AIEReigsterInfo.td is unuseful for this case.
    op =  0x1C;
  } else if(AIE::LE == Res) {
    // Encoding 0b01_010x
    // Info in AIEReigsterInfo.td is unuseful for this case.
    op =  0x14;
  } else if(AIE::LC == Res) {
    // Encoding 0b01_000x
    // Info in AIEReigsterInfo.td is unuseful for this case.
    op =  0x10;
  } else {
    llvm_unreachable("Unhandled register class!");
  }
}

void
AIEMCCodeEmitter::getmAluCg12OpValue(const MCInst &MI, unsigned OpNo, APInt &op,
                                    SmallVectorImpl<MCFixup> &Fixups,
                                    const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (!MO.isReg()) {
    llvm_unreachable("Unhandled expression!");
  }

  unsigned Res = MO.getReg();
  unsigned encoding = Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
  if(AIEMCRegisterClasses[AIE::GPRRegClassID].contains(Res) ||
     AIEMCRegisterClasses[AIE::MODRegClassID].contains(Res)) {
    // Encoding 0b1?_????  This is completely specified
    // by the HWEncoding field in AIERegisterInfo.td
    op =  encoding;
  } else if(AIEMCRegisterClasses[AIE::mCSRegClassID].contains(Res)) {
    // Encoding 0b00_1???
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op =  encoding | 0x08;
  } else if(AIE::LC == Res) {
    // Encoding 0b00_01xx
    // Info in AIEReigsterInfo.td is unuseful for this case.
    op =  0x04;
  } else {
    llvm_unreachable("Unhandled register class!");
  }
}


void
AIEMCCodeEmitter::getmMv0Cg20OpValue(const MCInst &MI, unsigned OpNo, APInt &op,
                                    SmallVectorImpl<MCFixup> &Fixups,
                                    const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (!MO.isReg()) {
    llvm_unreachable("Unhandled expression!");
  }

  unsigned Res = MO.getReg();
  unsigned encoding = Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
  if(AIEMCRegisterClasses[AIE::GPRRegClassID].contains(Res) ||
     AIEMCRegisterClasses[AIE::PTRRegClassID].contains(Res)) {
    // Encoding 0b1?_????  This is completely specified
    // by the HWEncoding field in AIERegisterInfo.td
    op =  encoding;
  } else if(AIEMCRegisterClasses[AIE::mCHRegClassID].contains(Res)) {
    // Encoding 0b00_0???
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op =  encoding | 0x00;
  } else if(AIEMCRegisterClasses[AIE::mCLRegClassID].contains(Res)) {
    // Encoding 0b00_1???
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op =  encoding | 0x08;
  } else if(AIEMCRegisterClasses[AIE::mCBRegClassID].contains(Res)) {
    // Encoding 0b01_0???
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op =  encoding | 0x10;
  } else if(AIE::LS == Res) {
    // Encoding 0b01_1x00
    // Info in AIEReigsterInfo.td is unuseful for this case.
    op =  0x18;
  } else if(AIE::LE == Res) {
    // Encoding 0b01_1x11
    // Info in AIEReigsterInfo.td is unuseful for this case.
    op =  0x1B;
  } else if(AIE::SP == Res) {
    // Encoding 0b01_1x01
    // Info in AIEReigsterInfo.td is unuseful for this case.
    op =  0x19;
  } else {
    llvm_unreachable("Unhandled register class!");
  }
}

void
AIEMCCodeEmitter::getmRCmOpValue(const MCInst &MI, unsigned OpNo, APInt &op,
                                    SmallVectorImpl<MCFixup> &Fixups,
                                    const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (!MO.isReg()) {
    llvm_unreachable("Unhandled expression!");
  }

  unsigned Res = MO.getReg();
  unsigned encoding = Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
  if(AIEMCRegisterClasses[AIE::GPRRegClassID].contains(Res)) {
    // Encoding 0b01_????  Four bits come from
    // the HWEncoding field in AIERegisterInfo.td
    // Fill in the high order bits.
    op =  (encoding & 0xF) | 0x10;
  } else if(AIEMCRegisterClasses[AIE::mCHRegClassID].contains(Res)) {
    // Encoding 0b00_0???
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op =  encoding | 0x00;
  } else if(AIEMCRegisterClasses[AIE::mCLRegClassID].contains(Res)) {
    // Encoding 0b00_1???
    // Lower three bits specified in AIERegisterInfo.td
    // Fill in high order bits correctly
    op =  encoding | 0x08;
  } else {
    llvm_unreachable("Unhandled register class!");
  }
}

void
AIEMCCodeEmitter::getmWBDvOpValue(const MCInst &MI, unsigned OpNo, APInt &op,
                                  SmallVectorImpl<MCFixup> &Fixups,
                                  const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (!MO.isReg()) {
    llvm_unreachable("Unhandled expression!");
  }

  unsigned Res = MO.getReg();
  unsigned encoding = Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
  if(AIEMCRegisterClasses[AIE::VECFWBRegClassID].contains(Res)) {
    // Encoding 0b0?
    op =  (encoding & 0x1);
  } else if(AIEMCRegisterClasses[AIE::VECFWDRegClassID].contains(Res)) {
    // Encoding 0b1?
    op =  (encoding & 0x1) | 0x2;
  } else {
    llvm_unreachable("Unhandled register class!");
  }
}

void
AIEMCCodeEmitter::getmXCDvOpValue(const MCInst &MI, unsigned OpNo, APInt &op,
                                  SmallVectorImpl<MCFixup> &Fixups,
                                  const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (!MO.isReg()) {
    llvm_unreachable("Unhandled expression!");
  }

  unsigned Res = MO.getReg();
  unsigned encoding = Ctx.getRegisterInfo()->getEncodingValue(Res);
  op =  (encoding & 0x1);
}

void
AIEMCCodeEmitter::getmVnOpValue(const MCInst &MI, unsigned OpNo, APInt &op,
                                SmallVectorImpl<MCFixup> &Fixups,
                                const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (!MO.isReg()) {
    llvm_unreachable("Unhandled expression!");
  }

  unsigned Res = MO.getReg();
  unsigned encoding = Ctx.getRegisterInfo()->getEncodingValue(Res);
  // The bits are swapped around in this encoding:
  // vl0 -> 0000
  // vl7 -> 1110
  // vh0 -> 0001
  // vh7 -> 1111
  op =  ((encoding & 0x8) >> 3) | encoding << 1;
}

#include "AIEGenMCCodeEmitter.inc"
