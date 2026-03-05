//===-- AIE2AsmPrinter.cpp - AIEngine V2 LLVM assembly writer ------------===//
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
// of machine-dependent LLVM code to the AIEngine V2 assembly language.
//
//===----------------------------------------------------------------------===//

#include "AIE2AsmPrinter.h"
#include "AIE2TargetMachine.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "aie-asm-printer"

// Simple pseudo-instructions have their lowering (with expansion to real
// instructions) auto-generated.
#include "AIE2GenMCPseudoLowering.inc"

bool AIE2AsmPrinter::lowerOperand(const MachineOperand &MO,
                                  MCOperand &MCOp) const {
  return LowerAIEMachineOperandToMCOperand(MO, MCOp, *this);
}

AsmPrinter *
llvm::createAIE2AsmPrinterPass(TargetMachine &TM,
                               std::unique_ptr<MCStreamer> &&Streamer) {
  return new AIE2AsmPrinter(TM, std::move(Streamer));
}

// Registration functions called from AIEAsmPrinterInit.cpp
void llvm::RegisterAIE2AsmPrinter() {
  RegisterAsmPrinter<AIE2AsmPrinter> Y(getTheAIE2Target());
}

void llvm::RegisterAIE2PAsmPrinter() {
  RegisterAsmPrinter<AIE2AsmPrinter> A(getTheAIE2PTarget());
}

void llvm::RegisterAIE2PSAsmPrinter() {
  RegisterAsmPrinter<AIE2AsmPrinter> B(getTheAIE2PSTarget());
}
