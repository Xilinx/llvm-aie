//===---- AIE2PMCCodeEmitter.cpp - Convert AIE2p code to machine code ----===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIE2PMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseMCCodeEmitter.h"
#include "AIEMCFormats.h"
#include "MCTargetDesc/aie2p/AIE2PMCFixupKinds.h"
#include "MCTargetDesc/aie2p/AIE2PMCTargetDesc.h"

using namespace llvm;

namespace {

namespace {
const AIE2PMCFormats AIE2PFormats;
} // namespace
class AIE2PMCCodeEmitter : public AIEBaseMCCodeEmitter {
  AIE2PMCCodeEmitter(const AIE2PMCCodeEmitter &) = delete;
  void operator=(const AIE2PMCCodeEmitter &) = delete;

public:
  AIE2PMCCodeEmitter(MCContext &Ctx, MCInstrInfo const &MCII)
      : AIEBaseMCCodeEmitter(Ctx, MCII, createAIE2PMCFixupKinds(),
                             AIE2PFormats) {}

  ~AIE2PMCCodeEmitter() override {}
  void getBinaryCodeForInstr(const MCInst &MI, SmallVectorImpl<MCFixup> &Fixups,
                             APInt &Inst, APInt &Scratch,
                             const MCSubtargetInfo &STI) const override;
#include "AIE2PMCCodeEmitterGen.h"
};

#include "AIE2PMCCodeEmitterGen.inc"

} // end anonymous namespace

MCCodeEmitter *llvm::createAIE2PMCCodeEmitter(const MCInstrInfo &MCII,
                                              MCContext &Ctx) {
  return new AIE2PMCCodeEmitter(Ctx, MCII);
}

#include "AIE2PGenMCCodeEmitter.inc"
