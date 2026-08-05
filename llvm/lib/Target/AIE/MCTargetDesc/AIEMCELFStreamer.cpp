//===- AIEMCELFStreamer.cpp - AIE subclass of MCELFStreamer ---------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEMCELFStreamer.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCObjectWriter.h"

using namespace llvm;

void AIEMCELFStreamer::visitInstOperands(const MCInst &Inst) {
  for (unsigned I = Inst.getNumOperands(); I--;) {
    const MCOperand &Op = Inst.getOperand(I);
    if (Op.isExpr())
      visitUsedExpr(*Op.getExpr());
    else if (Op.isInst())
      visitInstOperands(*Op.getInst());
  }
}

void AIEMCELFStreamer::emitInstruction(const MCInst &Inst,
                                       const MCSubtargetInfo &STI) {
  // Before emitting, so the symbols exist by the time relocations are recorded.
  // Recursing rather than calling emitInstruction per slot, which would emit
  // every sub-instruction again.
  visitInstOperands(Inst);
  MCObjectStreamer::emitInstruction(Inst, STI);
}

MCStreamer *llvm::createAIEELFStreamer(const Triple &TT, MCContext &Context,
                                       std::unique_ptr<MCAsmBackend> &&MAB,
                                       std::unique_ptr<MCObjectWriter> &&OW,
                                       std::unique_ptr<MCCodeEmitter> &&CE) {
  return new AIEMCELFStreamer(Context, std::move(MAB), std::move(OW),
                              std::move(CE));
}
