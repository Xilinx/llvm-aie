//===-- AIECommonInstPrinter.cpp - Convert AIEngine MCInst to asm syntax--==//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This class prints an AIEngine MCInst to a .s file.
//
//===----------------------------------------------------------------------===//
#include "AIECommonInstPrinter.h"
#include "Utils/AIEBaseInfo.h"
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

static bool isComposite(const MCInst *MI) {
  for (auto &i : *MI) {
    if (i.isInst()) {
      return true;
    }
  }
  return false;
  // This might be better, but I'm not sure how the flags get set.
  // llvm::dbgs() << "Flags = " << MI->getFlags() << "\n";
  // return (MI->getFlags() >> 13) & 0x1;
}

void AIECommonInstPrinter::printInstr(const MCInst *MI, uint64_t Address,
                                      StringRef Annot,
                                      const MCSubtargetInfo &STI,
                                      raw_ostream &O) {
  if (isComposite(MI)) {
    for (auto i = MI->begin(); i != MI->end(); i++) {
      if (i->isInst()) {
        const MCInst *SI = i->getInst();
        printInstruction(SI, Address, STI, O);
        printAnnotation(O, Annot);
      } else {
        O << "?";
      }
      if (i != MI->end() - 1)
        O << ";\t";
    }
    return;
  }
  if (NoAliases || !printAliasInstr(MI, Address, STI, O))
    printInstruction(MI, Address, STI, O);
  printAnnotation(O, Annot);
}

void AIECommonInstPrinter::printOperand(const MCOperand &MO,
                                        const MCSubtargetInfo &STI,
                                        raw_ostream &O) {
  // Print Registers normally.
  if (MO.isReg()) {
    printRegName(O, MO.getReg());
    return;
  }

  const Triple &TT = STI.getTargetTriple();

  if (MO.isImm()) {
    // Print Immediates with a preceding hash sign.
    if (TT.isAIE1() || TT.isAIE2())
      O << "#";

    O << formatImm(MO.getImm());
    return;
  }

  assert(MO.isExpr() && "Unknown operand kind in printOperand");
  if (MO.getExpr()->getKind() == MCExpr::SymbolRef)
    // Print Immediates with a preceding hash sign.
    if (TT.isAIE1() || TT.isAIE2())
      O << "#";
  MO.getExpr()->print(O, &MAI);
}
