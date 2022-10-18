//===-- AIE2MCCodeEmitter.cpp - Convert AIEngine V2 code to machine code --===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIE2MCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseMCCodeEmitter.h"
#include "AIEMCFormats.h"
#include "MCTargetDesc/AIE2MCFixupKinds.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "MCTargetDesc/AIEMCExpr.h"
#include "MCTargetDesc/AIEMCInstrInfo.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

namespace {
const AIE2MCFormats AIE2Formats;
} // namespace
class AIE2MCCodeEmitter : public AIEBaseMCCodeEmitter {
  AIE2MCCodeEmitter(const AIE2MCCodeEmitter &) = delete;
  void operator=(const AIE2MCCodeEmitter &) = delete;

public:
  AIE2MCCodeEmitter(MCContext &Ctx, MCInstrInfo const &MCII)
      : AIEBaseMCCodeEmitter(Ctx, MCII, createAIE2MCFixupKinds(), AIE2Formats) {
  }

  ~AIE2MCCodeEmitter() override {}
  void getBinaryCodeForInstr(const MCInst &MI, SmallVectorImpl<MCFixup> &Fixups,
                             APInt &Inst, APInt &Scratch,
                             const MCSubtargetInfo &STI) const override;
#include "AIE2MCCodeEmitterDeclaration.h"
};

#include "AIE2MCCodeEmitterRegOperandDef.h"

} // end anonymous namespace

MCCodeEmitter *llvm::createAIE2MCCodeEmitter(const MCInstrInfo &MCII,
                                             MCContext &Ctx) {
  return new AIE2MCCodeEmitter(Ctx, MCII);
}

#include "AIE2GenMCCodeEmitter.inc"
