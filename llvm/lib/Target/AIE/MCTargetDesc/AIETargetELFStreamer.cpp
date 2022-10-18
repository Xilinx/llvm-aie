//===-- AIEELFStreamer.cpp - AIE ELF Target Streamer Methods ----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file provides AIE specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "AIETargetELFStreamer.h"
#include "AIEMCTargetDesc.h"
#include "Utils/AIEBaseInfo.h"
#include "llvm/TargetParser/Triple.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"

using namespace llvm;

// This part is for ELF object output.
AIETargetELFStreamer::AIETargetELFStreamer(MCStreamer &S,
                                           const MCSubtargetInfo &STI)
    : AIETargetStreamer(S), STI(STI) {
  MCAssembler &MCA = getStreamer().getAssembler();

  unsigned EFlags = 0;
  switch (STI.getTargetTriple().getArch()) {
  case Triple::aie:
    EFlags = ELF::EF_AIE_AIE1;
    break;
  case Triple::aie2:
    EFlags = ELF::EF_AIE_AIE2;
    break;
  default:
    llvm_unreachable("unknown aie triple");
  }
  MCA.setELFHeaderEFlags(EFlags);

  // We get RelaxAll from the driver at -O0. It assumes that fully relaxing
  // all instructions doesn't change correctness, while not requiring other
  // optimization passes. However, fully relaxing our nops breaks return address
  // alignment and causes huge code expansion.
  MCA.setRelaxAll(false);
}

MCELFStreamer &AIETargetELFStreamer::getStreamer() {
  return static_cast<MCELFStreamer &>(Streamer);
}

void AIETargetELFStreamer::emitDirectiveOptionPush() {}
void AIETargetELFStreamer::emitDirectiveOptionPop() {}
void AIETargetELFStreamer::emitDirectiveOptionRVC() {}
void AIETargetELFStreamer::emitDirectiveOptionNoRVC() {}
void AIETargetELFStreamer::emitDirectiveOptionRelax() {}
void AIETargetELFStreamer::emitDirectiveOptionNoRelax() {}

void AIETargetELFStreamer::finish() {
  // This is the last thing we do when producing an ELF file. We prevent
  // the linker from padding the code section with zero bytes by adding a code
  // alignment fragment, which will fill out with proper nop instructions.
  MCELFStreamer &Streamer = getStreamer();
  auto &Context = Streamer.getContext();
  auto *Text = Context.getELFSection(".text", ELF::SHT_PROGBITS, 0);
  Streamer.switchSection(Text);
  Streamer.emitCodeAlignment(Align(16), &STI);
}
