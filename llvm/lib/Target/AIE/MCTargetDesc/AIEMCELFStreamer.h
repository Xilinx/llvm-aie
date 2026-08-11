//===- AIEMCELFStreamer.h - AIE subclass of MCELFStreamer -------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCELFSTREAMER_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCELFSTREAMER_H

#include "llvm/MC/MCELFStreamer.h"

namespace llvm {

/// Registers the symbols an AIE bundle references from its slots.
///
/// A bundle is one MCInst whose operands are its slot instructions, so a symbol
/// referenced from a slot sits one level below MCStreamer::emitInstruction's
/// operand scan. Hexagon subclasses the ELF streamer for the same reason.
class AIEMCELFStreamer : public MCELFStreamer {
public:
  AIEMCELFStreamer(MCContext &Context, std::unique_ptr<MCAsmBackend> TAB,
                   std::unique_ptr<MCObjectWriter> OW,
                   std::unique_ptr<MCCodeEmitter> Emitter)
      : MCELFStreamer(Context, std::move(TAB), std::move(OW),
                      std::move(Emitter)) {}

  void emitInstruction(const MCInst &Inst, const MCSubtargetInfo &STI) override;

private:
  /// Visit every expression in \p Inst, descending into slot sub-instructions.
  void visitInstOperands(const MCInst &Inst);
};

MCStreamer *createAIEELFStreamer(const Triple &TT, MCContext &Context,
                                 std::unique_ptr<MCAsmBackend> &&MAB,
                                 std::unique_ptr<MCObjectWriter> &&OW,
                                 std::unique_ptr<MCCodeEmitter> &&CE);

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCELFSTREAMER_H
