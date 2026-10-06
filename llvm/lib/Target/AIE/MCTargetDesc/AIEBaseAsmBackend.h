//===-- AIEBaseAsmBackend.h - AIE Assembler Backend -----------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEBASEASMBACKEND_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEBASEASMBACKEND_H

#include "MCTargetDesc/AIEMCFixupKinds.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"
#include "Utils/AIEBaseInfo.h"
#include "llvm/ADT/bit.h"
#include "llvm/MC/MCAsmBackend.h"

namespace llvm {
class MCAssembler;
class MCSubTargetInfo;
class MCObjectTargetWriter;
class raw_ostream;

class AIEBaseAsmBackend : public MCAsmBackend {
protected:
  const MCSubtargetInfo &STI;
  uint8_t OSABI;
  const MCTargetOptions &TargetOptions;
  AIEABI::ABI TargetABI = AIEABI::ABI_VITIS;

public:
  AIEBaseAsmBackend(const MCSubtargetInfo &STI, uint8_t OSABI,
                    const MCTargetOptions &Options)
      : MCAsmBackend(endianness::little), STI(STI), OSABI(OSABI),
        TargetOptions(Options) {
    (void)STI;
  }
  virtual ~AIEBaseAsmBackend() override {}

  void applyFixup(const MCFragment &, const MCFixup &Fixup,
                  const MCValue &Target, uint8_t *Data,
                  uint64_t Value, bool IsResolved) override;

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override;

  bool fixupNeedsRelaxation(const MCFixup &Fixup,
                            uint64_t Value) const override {
    // All fixups are symbolic references that don't change by relaxation,
    // which only adds nop slots.
    return false;
  }

  MCFixupKindInfo getFixupKindInfo(MCFixupKind Kind) const override {
    // If the kind is a base LLVM Fixup
    if (!AIEMCFixupKinds::isTargetFixup(Kind))
      return MCAsmBackend::getFixupKindInfo(Kind);

    // Otherwise the Kind is target specific. In AIE, MCFixupKindInfo is not
    // sufficient to fully describe a fixup (which targets possibly multiple
    // fields). Thus, this method must not be used to retrieve the fixup's
    // semantic. Instead, we rely on the FixupField structure to describe each
    // independent patch.

    // getFixupKindInfo() is called by the MCAssembler when evaluating the
    // fixups, even though they will not be relaxed. This is why we can't put
    // an unreachable here and we always return a Dummy value with a "0" flag.
    static MCFixupKindInfo Dummy = {"", 0, 0, 0};
    return Dummy;
  }

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override = 0;

  const MCTargetOptions &getTargetOptions() const { return TargetOptions; }
  AIEABI::ABI getTargetABI() const { return TargetABI; }
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEBASEASMBACKEND_H
