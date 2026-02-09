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
#include "llvm/MC/MCObjectStreamer.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

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
