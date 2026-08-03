//===-- AIEELFObjectWriter.cpp - AIE ELF Writer -----------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/AIE1MCFixupKinds.h"
#include "MCTargetDesc/AIE2MCFixupKinds.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"
#include "MCTargetDesc/aie2p/AIE2PMCFixupKinds.h"
#include "MCTargetDesc/aie2ps/AIE2PSMCFixupKinds.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/TargetParser/Triple.h"

using namespace llvm;

namespace {
class AIEELFObjectWriter : public MCELFObjectTargetWriter {
  Triple TargetTriple;

public:
  AIEELFObjectWriter(uint8_t OSABI, Triple Triple);

  ~AIEELFObjectWriter() override;

  // Return true if the given relocation must be with a symbol rather than
  // section plus offset.
  bool needsRelocateWithSymbol(const MCValue &Val,
                               unsigned Type) const override {
    // TODO: this is very conservative,
    return true;
  }

protected:
  unsigned getRelocType(const MCFixup &Fixup, const MCValue &Target,
                        bool IsPCRel) const override;
};
}

// The Target tools generate ELF files with machine type EM_AIE.
AIEELFObjectWriter::AIEELFObjectWriter(uint8_t OSABI, Triple Triple)
    : MCELFObjectTargetWriter(/*Is64Bit=*/false, OSABI, ELF::EM_AIE,
                              /*HasRelocationAddend*/ true),
      TargetTriple(Triple) {}

AIEELFObjectWriter::~AIEELFObjectWriter() {}

static unsigned getRelocTypeAIE1(const MCFixup &Fixup) {
  MCFixupKind Kind = Fixup.getKind();

  // One-to-one mapping between the fixups and the relocations as the
  // enumerations are dense (fixup_aie_i maps to R_AIE_i for all i).
  if (AIEMCFixupKinds::isTargetFixup(Kind))
    return Kind - AIE::fixup_aie_0 + ELF::R_AIE_0;

  // Conversely, the other fixups (targeting data memory words) must be handled
  // manually below:
  switch (unsigned(Kind)) {
  default:
    dbgs() << "Fixup Kind: " << Kind << "\n";
    llvm_unreachable("invalid fixup kind!");
  case llvm::FK_Data_4:
    return ELF::R_AIE_72;
  }
}

static unsigned getRelocTypeAIE2(const MCFixup &Fixup) {
  MCFixupKind Kind = Fixup.getKind();

  // One-to-one mapping between the fixups and the relocations as the
  // enumerations are dense (fixup_aie2_i maps to R_AIE_i for all i).
  if (AIEMCFixupKinds::isTargetFixup(Kind))
    return Kind - AIE2::fixup_aie2_0 + ELF::R_AIE_0;

  // Conversely, the other fixups (targeting data memory words) must be handled
  // manually below:
  switch (unsigned(Kind)) {
  default:
    dbgs() << "Fixup Kind: " << Kind << "\n";
    llvm_unreachable("invalid fixup kind!");
  case llvm::FK_Data_4:
    return ELF::R_AIE_50;
  }
}

static unsigned getRelocTypeAIE2P(const MCFixup &Fixup) {
  MCFixupKind Kind = Fixup.getKind();

  // One-to-one mapping between the fixups and the relocations as the
  // enumerations are dense (fixup_aie2p_i maps to R_AIE_i for all i).
  if (AIEMCFixupKinds::isTargetFixup(Kind))
    return Kind - AIE2P::fixup_aie2p_0 + ELF::R_AIE_0;

  // Conversely, the other fixups (targeting data memory words) must be handled
  // manually below:
  switch (unsigned(Kind)) {
  default:
    dbgs() << "Fixup Kind: " << Kind << "\n";
    llvm_unreachable("invalid fixup kind!");
  case llvm::FK_Data_4:
    return ELF::R_AIE_62;
  }
}

static unsigned getRelocTypeAIE2PS(const MCFixup &Fixup) {
  MCFixupKind Kind = Fixup.getKind();

  // One-to-one mapping between the fixups and the relocations as the
  // enumerations are dense (fixup_aie2ps_i maps to R_AIE_i for all i).
  if (AIEMCFixupKinds::isTargetFixup(Kind))
    return Kind - AIE2PS::fixup_aie2ps_0 + ELF::R_AIE_0;

  // Conversely, the other fixups (targeting data memory words) must be handled
  // manually below:
  switch (unsigned(Kind)) {
  default:
    dbgs() << "Fixup Kind: " << Kind << "\n";
    llvm_unreachable("invalid fixup kind!");
  case llvm::FK_Data_4:
    return ELF::R_AIE_135;
  }
}

unsigned AIEELFObjectWriter::getRelocType(const MCFixup &Fixup,
                                          const MCValue &Target,
                                          bool IsPCRel) const {
  // NOTE: Their is no pc-relative call in AIE1/2 so the value of IsPCRel isn't
  // relevant.
  if (TargetTriple.getArch() == Triple::aie)
    return getRelocTypeAIE1(Fixup);
  if (TargetTriple.getArch() == Triple::aie2)
    return getRelocTypeAIE2(Fixup);
  if (TargetTriple.getArch() == Triple::aie2p)
    return getRelocTypeAIE2P(Fixup);
  if (TargetTriple.getArch() == Triple::aie2ps)
    return getRelocTypeAIE2PS(Fixup);
  llvm_unreachable("Unsupported Relocations for other AIE version");
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createAIEELFObjectWriter(uint8_t OSABI, Triple TargetTriple) {
  return std::make_unique<AIEELFObjectWriter>(OSABI, TargetTriple);
}
