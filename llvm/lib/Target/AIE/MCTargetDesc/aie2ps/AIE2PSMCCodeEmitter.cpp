//===---- AIE2PSMCCodeEmitter.cpp - Convert AIE2ps code to machine code ---===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIE2PSMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseMCCodeEmitter.h"
#include "AIEMCFormats.h"
#include "MCTargetDesc/aie2ps/AIE2PSMCFixupKinds.h"
#include "MCTargetDesc/aie2ps/AIE2PSMCTargetDesc.h"

using namespace llvm;

namespace {

namespace {
const AIE2PSMCFormats AIE2PSFormats;
} // namespace
class AIE2PSMCCodeEmitter : public AIEBaseMCCodeEmitter {
  AIE2PSMCCodeEmitter(const AIE2PSMCCodeEmitter &) = delete;
  void operator=(const AIE2PSMCCodeEmitter &) = delete;

public:
  AIE2PSMCCodeEmitter(MCContext &Ctx, MCInstrInfo const &MCII)
      : AIEBaseMCCodeEmitter(Ctx, MCII, createAIE2PSMCFixupKinds(),
                             AIE2PSFormats) {}

  ~AIE2PSMCCodeEmitter() override {}
  void getBinaryCodeForInstr(const MCInst &MI, SmallVectorImpl<MCFixup> &Fixups,
                             APInt &Inst, APInt &Scratch,
                             const MCSubtargetInfo &STI) const override;
#include "AIE2PSMCCodeEmitterGen.h"
};

#include "AIE2PSMCCodeEmitterGen.inc"

} // end anonymous namespace

MCCodeEmitter *llvm::createAIE2PSMCCodeEmitter(const MCInstrInfo &MCII,
                                               MCContext &Ctx) {
  return new AIE2PSMCCodeEmitter(Ctx, MCII);
}

#include "AIE2PSGenMCCodeEmitter.inc"
