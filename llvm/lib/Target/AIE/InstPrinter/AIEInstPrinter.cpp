//===-- AIEInstPrinter.cpp - Convert AIE MCInst to asm syntax ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This class prints an AIE MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "AIEInstPrinter.h"
#include "AIEInstrInfo.h"
// #include "MCTargetDesc/AIEMCExpr.h"
#include "Utils/AIEBaseInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

// Include the auto-generated portion of the assembly writer.
#define PRINT_ALIAS_INSTR
#include "AIEGenAsmWriter.inc"

cl::opt<bool>
    NoAliases("aie-no-aliases",
              cl::desc("Disable the emission of assembler pseudo instructions"),
              cl::init(false), cl::Hidden);

static bool isComposite(const MCInst *MI) {
  for(auto &i : *MI) {
    if(i.isInst()) {
      return true;
    }
  }
  return false;
  // This might be better, but I'm not sure how the flags get set.
  // llvm::dbgs() << "Flags = " << MI->getFlags() << "\n";
  // return (MI->getFlags() >> 13) & 0x1;
}

void AIEInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                               StringRef Annot, const MCSubtargetInfo &STI,
                               raw_ostream &O) {
  if(isComposite(MI)) {
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

void AIEInstPrinter::printRegName(raw_ostream &O, MCRegister RegNo) const {
  O << getRegisterName(RegNo);
}

void AIEInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                    const MCSubtargetInfo &STI, raw_ostream &O,
                                    const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &MO = MI->getOperand(OpNo);

  // Print Registers normally.
  if (MO.isReg()) {
    printRegName(O, MO.getReg());
    return;
  }

  // Print Immediates with a precending hash sign.
  if (MO.isImm()) {
    O << "#" << MO.getImm();
    return;
  }

  assert(MO.isExpr() && "Unknown operand kind in printOperand");
  MO.getExpr()->print(O, &MAI);
}

template<int offset>
void AIEInstPrinter::printImmOffset(const MCInst *MI, unsigned OpNo,
                                    const MCSubtargetInfo &STI, raw_ostream &O,
                                    const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &MO = MI->getOperand(OpNo);

  // Print Immediates with a precending hash sign.
  if (MO.isImm()) {
    int64_t Imm = MO.getImm() + offset; //adjust offset of .hi accumulator registers
    O << "#" << Imm;
    return;
  }

  assert(MO.isExpr() && "Unknown operand kind in printOperand");
  MO.getExpr()->print(O, &MAI);
}
