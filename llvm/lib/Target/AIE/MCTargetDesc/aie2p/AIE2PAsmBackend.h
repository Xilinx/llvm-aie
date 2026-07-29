//===-- AIE2PAsmBackend.h - AIE2P Assembler Backend ----------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2PASMBACKEND_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2PASMBACKEND_H

#include "MCTargetDesc/AIEBaseAsmBackend.h"
#include "MCTargetDesc/aie2p/AIE2PMCFixupKinds.h"
#include "MCTargetDesc/aie2p/AIE2PMCTargetDesc.h"
#include "llvm/MC/MCSubtargetInfo.h"

namespace llvm {
class raw_ostream;

class AIE2PAsmBackend : public AIEBaseAsmBackend {
public:
  AIE2PAsmBackend(const MCSubtargetInfo &STI, uint8_t OSABI,
                  const MCTargetOptions &Options)
      : AIEBaseAsmBackend(STI, OSABI, Options) {}
  ~AIE2PAsmBackend() {}

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2PASMBACKEND_H
