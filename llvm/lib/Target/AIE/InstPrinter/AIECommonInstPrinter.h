//==--AIECommonInstPrinter.h- Convert AIEngine MCInst to asm syntax-*-C++-*--//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This class prints a AIEngine MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_INSTPRINTER_AIECOMMONINSTPRINTER_H
#define LLVM_LIB_TARGET_AIE_INSTPRINTER_AIECOMMONINSTPRINTER_H

#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/CommandLine.h"

extern llvm::cl::opt<bool> NoAliases;
namespace llvm {
class MCOperand;
class AIECommonInstPrinter : public MCInstPrinter {
public:
  AIECommonInstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII,
                       const MCRegisterInfo &MRI)
      : MCInstPrinter(MAI, MII, MRI) {}
  void printInstr(const MCInst *MI, uint64_t Address, StringRef Annot,
                  const MCSubtargetInfo &STI, raw_ostream &O);
  void printOperand(const MCOperand &MO, const MCSubtargetInfo &STI,
                    raw_ostream &O);
  template <int offset>
  void printImmOffset(const MCOperand &MO, const MCSubtargetInfo &STI,
                      raw_ostream &O) {

    const Triple &TT = STI.getTargetTriple();

    if (MO.isImm()) {
      int64_t Imm =
          MO.getImm() + offset; // adjust offset of .hi accumulator registers
      // Print Immediates with a precending hash sign
      if (TT.isAIE1() || TT.isAIE2())
        O << "#";
      O << Imm;
      return;
    }

    assert(MO.isExpr() && "Unknown operand kind in printOperand");
    MO.getExpr()->print(O, &MAI);
  }
  virtual void printInstruction(const MCInst *MI, uint64_t Address,
                                const MCSubtargetInfo &STI, raw_ostream &O) = 0;
  virtual bool printAliasInstr(const MCInst *MI, uint64_t Address,
                               const MCSubtargetInfo &STI, raw_ostream &O) = 0;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_INSTPRINTER_AIECOMMONINSTPRINTER_H
