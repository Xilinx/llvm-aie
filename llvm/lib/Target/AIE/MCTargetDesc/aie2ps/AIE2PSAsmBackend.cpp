//===-- AIE2PSAsmBackend.cpp - AIE2ps Assembler Backend -------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

#include "AIE2PSAsmBackend.h"
#include "MCTargetDesc/AIEMCFixupKinds.h"
#include "llvm/MC/AIERelocationPatch.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCObjectStreamer.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/ErrorHandling.h"

#define DEBUG_TYPE "aie2ps-asm-backend"

using namespace llvm;
using namespace llvm::support::endian;

// Define empty map for instruction fixup flags (required by the .inc file)
const static std::map<unsigned, FixupFlag> AIE2PSInstrFixupFlags = {};

// Include AIE2PS fixup field information from generated file
#define AIE2PS_CUSTOM_FIXUP_KINDS
#define GET_MCFIXUPKINDS_IMPLEM
#include "FixupInfo/AIE2PSFixupInfo.inc"

/// Try to apply a PC-relative fixup with split 11-bit encoding.
/// Returns true if the fixup was handled, false if it's not a PC-relative
/// fixup.
bool AIE2PSAsmBackend::tryApplyPCRelativeFixup(const MCFragment &Fragment,
                                               MCFixupKind Kind, uint8_t *Data,
                                               uint64_t Value,
                                               const MCFixup &Fixup) const {
  // Only PC-relative (ZOL) fixups are handled here. Non-ZOL fixups (e.g.
  // regular branch fixups like jl) have isPCRel=false and must be rejected
  // before the field lookup, because AIE2PSFixupFieldsInfos contains ALL
  // fixups and a non-ZOL fixup with a single field would trip the
  // Fields.size()==2 assertion below.
  if (!Fixup.isPCRel())
    return false;

  const auto FieldsIt = AIE2PSFixupFieldsInfos.find(Kind);
  const auto FormatSizeIt = AIE2PSFixupFormatSize.find(Kind);
  if (FieldsIt == AIE2PSFixupFieldsInfos.end() ||
      FormatSizeIt == AIE2PSFixupFormatSize.end())
    return false;

  const unsigned InstrSize = FormatSizeIt->second;
  assert(Fixup.getOffset() <= Fragment.getSize() &&
         InstrSize <= Fragment.getSize() - Fixup.getOffset() &&
         "Invalid PC-relative fixup offset!");

  const SmallVector<FixupField> &Fields = FieldsIt->second;
  assert(Fields.size() == 2 && "PC-rel ZOL fixups must have 2 fields");

  const unsigned HighSize = Fields[0].Size;
  const unsigned LowSize = Fields[1].Size;
  const unsigned TotalBits = HighSize + LowSize;

  // Verify the value fits without overflow
  // Note that we are explicitly mentioning ZOL here because they are
  // the only consumers of this code, so emitting generic error is not
  // a good diagnose.
  if (!isUIntN(TotalBits, Value))
    getContext().reportError(Fixup.getLoc(), "PC-rel ZOL fixup value " +
                                                 Twine(Value) +
                                                 " overflows 11-bit field");

  AIE::patchNBytes(InstrSize, Data, Value, /*Hi=*/HighSize + LowSize - 1,
                   /*Lo=*/LowSize, /*Pos=*/Fields[0].Offset);
  AIE::patchNBytes(InstrSize, Data, Value, /*Hi=*/LowSize - 1, /*Lo=*/0,
                   /*Pos=*/Fields[1].Offset);
  return true;
}

void AIE2PSAsmBackend::applyFixup(const MCFragment &Fragment,
                                  const MCFixup &Fixup, const MCValue &Target,
                                  uint8_t *Data, uint64_t Value,
                                  bool IsResolved) {
  unsigned FixupNum = Fixup.getKind() - FirstTargetFixupKind;
  LLVM_DEBUG(dbgs() << "AIE2PS applyFixup: fixup_" << FixupNum << " Value="
                    << Value << " IsPCRel=" << Fixup.isPCRel() << "\n");
  // Try to handle PC-relative fixups
  if (tryApplyPCRelativeFixup(Fragment, Fixup.getKind(), Data, Value, Fixup))
    return;

  // Delegate non-ZOL fixups to parent class
  AIEBaseAsmBackend::applyFixup(Fragment, Fixup, Target, Data, Value,
                                IsResolved);
}

void AIE2PSAsmBackend::relaxInstruction(MCInst &Inst,
                                        const MCSubtargetInfo &STI) const {
  llvm_unreachable("relaxInstruction call not expected in AIE2PS");
}

bool AIE2PSAsmBackend::writeNopData(raw_ostream &OS, uint64_t Count,
                                    const MCSubtargetInfo *STI) const {

  // We fill with maximal packets of nops.
  // These correspond to the NOPnn instructions

  // First check instruction granularity
  const unsigned MinNopLen = 2;
  if ((Count % MinNopLen) != 0)
    return false;
  // First shave off cycles of sixteen bytes
  while (Count >= 16) {
    OS.write("\x15\x3e\x00\x00\x00\x00\x00\x00\x40\x2d\x00\x58\x00\x68\x0c\x00",
             16);
    Count -= 16;
  }

  // Then do the tail
  switch (Count) {
  case 2:
    OS.write("\x00\0", 2);
    break;
  case 4:
    OS.write("\x30\x00\x00\x20", 4);
    break;
  case 6:
    OS.write("\x06\x00\x00\x00\x00\x00", 6);
    break;
  case 8:
    OS.write("\x0a\x00\x00\x58\x00\x68\x0c\x00", 8);
    break;
  case 10:
    OS.write("\xb8\x00\x00\x00\x00\x00\x00\x68\x0c\x00", 10);
    break;
  case 12:
    OS.write("\xb4\x00\x00\x00\x00\x00\x00\x58\x00\x68\x0c\x00", 12);
    break;
  case 14:
    OS.write("\x7c\xa8\x05\x00\x00\x00\x00\x00\x00\x58\x00\x68\x0c\x00 ", 14);
    break;
  default:
    assert(Count == 0);
  }

  return true;
}
