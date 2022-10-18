//===-- AIE2AsmBackend.h - AIE2 Assembler Backend -----------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2ASMBACKEND_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2ASMBACKEND_H

#include "MCTargetDesc/AIE2MCFixupKinds.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "MCTargetDesc/AIEBaseAsmBackend.h"
#include "llvm/MC/MCSubtargetInfo.h"

namespace llvm {
class raw_ostream;

class AIE2AsmBackend : public AIEBaseAsmBackend {
public:
  AIE2AsmBackend(const MCSubtargetInfo &STI, uint8_t OSABI,
                 const MCTargetOptions &Options)
      : AIEBaseAsmBackend(STI, OSABI, Options) {}
  ~AIE2AsmBackend() {}

  unsigned getNumFixupKinds() const override {
    return AIE2::NumTargetFixupKinds;
  }

  unsigned maxRelaxIncrement(const MCInst &Inst,
                             const MCSubtargetInfo &STI) const override;

  void relaxInstruction(MCInst &Inst,
                        const MCSubtargetInfo &STI) const override;

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override;

  bool isCall(unsigned Opcode) const override;

  bool isDelaySlotInstr(unsigned Opcode) const override;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2ASMBACKEND_H
