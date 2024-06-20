//===-- AIE1AsmBackend.h - AIE1 Assembler Backend -----------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE1ASMBACKEND_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE1ASMBACKEND_H

#include "MCTargetDesc/AIE1MCFixupKinds.h"
#include "MCTargetDesc/AIEBaseAsmBackend.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"
#include "Utils/AIEBaseInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"

namespace llvm {
class raw_ostream;

class AIE1AsmBackend : public AIEBaseAsmBackend {
public:
  AIE1AsmBackend(const MCSubtargetInfo &STI, uint8_t OSABI,
                 const MCTargetOptions &Options)
      : AIEBaseAsmBackend(STI, OSABI, Options) {}
  ~AIE1AsmBackend() override {}

  unsigned getNumFixupKinds() const override {
    return AIE::NumTargetFixupKinds;
  }

  void relaxInstruction(MCInst &Inst,
                        const MCSubtargetInfo &STI) const override;

  unsigned maxRelaxIncrement(const MCInst &Inst,
                             const MCSubtargetInfo &STI) const override;

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override;

  bool isCall(unsigned Opcode) const override;

  bool isDelaySlotInstr(unsigned Opcode) const override;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE1ASMBACKEND_H
