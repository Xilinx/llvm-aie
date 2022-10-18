//===-- AIE2AsmPrinter.h - AIEngine V2 LLVM assembly writer --------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the AIEngine V2 assembly language.
//
//===----------------------------------------------------------------------===//
//
#ifndef LLVM_LIB_TARGET_AIE2_AIE2ASMPRINTER_H
#define LLVM_LIB_TARGET_AIE2_AIE2ASMPRINTER_H

#include "AIEBaseAsmPrinter.h"

namespace llvm {

class AIE2AsmPrinter : public AIEBaseAsmPrinter {
public:
  explicit AIE2AsmPrinter(TargetMachine &TM,
                          std::unique_ptr<MCStreamer> Streamer)
      : AIEBaseAsmPrinter(TM, std::move(Streamer)) {}

  StringRef getPassName() const override { return "AIE2 Assembly Printer"; }

  bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                       const char *ExtraCode, raw_ostream &OS) override;
  bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNo,
                             const char *ExtraCode, raw_ostream &OS) override;

  bool emitPseudoExpansionLowering(MCStreamer &OutStreamer,
                                   const MachineInstr *MI) override;

  // Wrapper needed for tblgenned pseudo lowering.
  bool lowerOperand(const MachineOperand &MO, MCOperand &MCOp) const;
};

AsmPrinter *createAIE2AsmPrinterPass(TargetMachine &TM,
                                     std::unique_ptr<MCStreamer> &&Streamer);
} // namespace llvm

#endif // #define LLVM_LIB_TARGET_AIE2_AIE2ASMPRINTER_H
