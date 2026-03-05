//===-- AIEAsmPrinter.cpp - AIE LLVM assembly writer ------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the AIE assembly language.
//
//===----------------------------------------------------------------------===//

#include "AIE1AsmPrinter.h"
#include "AIE.h"
#include "AIE1TargetMachine.h"
#include "AIEBaseAsmPrinter.h"
#include "InstPrinter/AIEInstPrinter.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {
class AIEAsmPrinter : public AIEBaseAsmPrinter {
public:
  explicit AIEAsmPrinter(TargetMachine &TM,
                         std::unique_ptr<MCStreamer> Streamer)
      : AIEBaseAsmPrinter(TM, std::move(Streamer)) {}

  StringRef getPassName() const override { return "AIE Assembly Printer"; }

  bool lowerPseudoInstExpansion(const MachineInstr *MI, MCInst &Inst) override;

  // Wrapper needed for tblgenned pseudo lowering.
  bool lowerOperand(const MachineOperand &MO, MCOperand &MCOp) const {
    return LowerAIEMachineOperandToMCOperand(MO, MCOp, *this);
  }
};
} // namespace

// Simple pseudo-instructions have their lowering (with expansion to real
// instructions) auto-generated.
#include "AIEGenMCPseudoLowering.inc"

// Registration function called from AIEAsmPrinterInit.cpp
void llvm::RegisterAIE1AsmPrinter() {
  RegisterAsmPrinter<AIEAsmPrinter> X(getTheAIETarget());
}
