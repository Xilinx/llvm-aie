//===-- AIE2InstPrinter.cpp - Convert AIEngine V2 MCInst to asm syntax-----===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This class prints an AIEngine V2 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "AIE2InstPrinter.h"
#include "AIEInstPrinter.h"
#include "Utils/AIEBaseInfo.h"
#include "aie2/AIE2InstrInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

#define DEBUG_TYPE "aie2-asm-printer"

// Include the auto-generated portion of the assembly writer.
#define PRINT_ALIAS_INSTR
#include "AIE2GenAsmWriter.inc"

void AIE2InstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                StringRef Annot, const MCSubtargetInfo &STI,
                                raw_ostream &O) {
  AIECommonInstPrinter::printInstr(MI, Address, Annot, STI, O);
}

void AIE2InstPrinter::printRegName(raw_ostream &O, MCRegister RegNo) {
  O << getRegisterName(RegNo);
}

template <int offset>
void AIE2InstPrinter::printImmOffset(const MCInst *MI, unsigned OpNo,
                                     const MCSubtargetInfo &STI, raw_ostream &O,
                                     const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &MO = MI->getOperand(OpNo);
  AIECommonInstPrinter::printImmOffset<offset>(MO, STI, O);
}

void AIE2InstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                   const MCSubtargetInfo &STI, raw_ostream &O,
                                   const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &MO = MI->getOperand(OpNo);
  AIECommonInstPrinter::printOperand(MO, STI, O);
}
