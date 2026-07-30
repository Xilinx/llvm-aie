//===-- AIE2PSAsmBackend.h - AIE2ps Assembler Backend
//----------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2PSASMBACKEND_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2PSASMBACKEND_H

#include "MCTargetDesc/AIEBaseAsmBackend.h"
#include "MCTargetDesc/aie2ps/AIE2PSMCFixupKinds.h"
#include "MCTargetDesc/aie2ps/AIE2PSMCTargetDesc.h"
#include "llvm/MC/MCSubtargetInfo.h"

namespace llvm {
class raw_ostream;

class AIE2PSAsmBackend : public AIEBaseAsmBackend {
public:
  AIE2PSAsmBackend(const MCSubtargetInfo &STI, uint8_t OSABI,
                   const MCTargetOptions &Options)
      : AIEBaseAsmBackend(STI, OSABI, Options) {}
  ~AIE2PSAsmBackend() {}

  void relaxInstruction(MCInst &Inst,
                        const MCSubtargetInfo &STI) const override;

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2PSASMBACKEND_H
