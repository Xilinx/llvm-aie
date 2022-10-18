//===-- AIETargetObjectFile.cpp - AIE Object Info -----------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIETargetObjectFile.h"
#include "AIETargetMachine.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionELF.h"

using namespace llvm;

static cl::opt<bool>
    MatchSymbolAlignment("aie-sections-matching-alignment", cl::init(true),
                         cl::desc("[AIE] Create multiple .bss and .data "
                                  "sections to match object alignment"));

void AIEELFTargetObjectFile::Initialize(MCContext &Ctx,
                                          const TargetMachine &TM) {
  TargetLoweringObjectFileELF::Initialize(Ctx, TM);
  InitializeELF(TM.Options.UseInitArray);

  // Legacy linker requires unique names for every symbol
  Ctx.setUseNamesOnTempLabels(true);
}

static MCSection *getDataSectionWithAlign(const GlobalObject &GO,
                                          MCContext &Ctx) {
  auto Align = std::to_string(GO.getAlignment());
  return Ctx.getELFSection(".data.align." + Align, ELF::SHT_PROGBITS,
                           ELF::SHF_WRITE | ELF::SHF_ALLOC);
}

static MCSection *getBssSectionWithAlign(const GlobalObject &GO,
                                         MCContext &Ctx) {
  auto Align = std::to_string(GO.getAlignment());
  return Ctx.getELFSection(".bss.align." + Align, ELF::SHT_NOBITS,
                           ELF::SHF_ALLOC | ELF::SHF_WRITE);
}

MCSection *AIEELFTargetObjectFile::SelectSectionForGlobal(
    const GlobalObject *GO, SectionKind Kind, const TargetMachine &TM) const {
  // To ensure compatibility with the other linkers, we need multiple .bss and
  // .data sections. Linkers can infer individual symbol alignment from the
  // alignment of their parent section, and can complain if the linker script
  // requests a symbol address that does not meet the section alignment.
  if (MatchSymbolAlignment && Kind.isGlobalWriteableData() &&
      !TM.getDataSections()) {
    assert(GO);
    if (Kind.isBSS())
      return getBssSectionWithAlign(*GO, getContext());
    if (Kind.isData())
      return getDataSectionWithAlign(*GO, getContext());
  }
  return TargetLoweringObjectFileELF::SelectSectionForGlobal(GO, Kind, TM);
}
