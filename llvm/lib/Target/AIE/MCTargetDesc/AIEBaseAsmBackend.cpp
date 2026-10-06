//===-- AIEBaseAsmBackend.cpp - AIE Assembler Backend ---------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEBaseAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCObjectStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"

using namespace llvm;

static uint64_t adjustFixupValue(const MCFixup &Fixup, uint64_t Value,
                                 MCContext &Ctx) {
  unsigned Kind = Fixup.getKind();
  switch (Kind) {
  default:
    llvm_unreachable("Unknown fixup kind!");
  case FK_Data_1:
  case FK_Data_2:
  case FK_Data_4:
  case FK_Data_8:
    return Value;
  }
}

void AIEBaseAsmBackend::applyFixup(const MCFragment &F, const MCFixup &Fixup,
                                   const MCValue &Target,
                                   uint8_t *Data, uint64_t Value,
                                   bool IsResolved) {
  maybeAddReloc(F, Fixup, Target, Value, IsResolved);

  if (!Value)
    return; // Doesn't change encoding.

  MCContext &Ctx = getContext();

  // Apply any target-specific value adjustments.
  Value = adjustFixupValue(Fixup, Value, Ctx);

  MCFixupKindInfo Info = getFixupKindInfo(Fixup.getKind());

  // Shift the value into position.
  Value <<= Info.TargetOffset;

  unsigned NumBytes = alignTo(Info.TargetSize + Info.TargetOffset, 8) / 8;

  // For each byte of the fragment that the fixup touches, mask in the
  // bits from the fixup value.
  for (unsigned i = 0; i != NumBytes; ++i) {
    Data[i] |= uint8_t((Value >> (i * 8)) & 0xff);
  }
}

std::unique_ptr<MCObjectTargetWriter>
AIEBaseAsmBackend::createObjectTargetWriter() const {
  return createAIEELFObjectWriter(OSABI, STI.getTargetTriple());
}
