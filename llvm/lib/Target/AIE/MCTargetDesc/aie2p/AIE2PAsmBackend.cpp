//===-- AIE2PAsmBackend.cpp - AIE2p Assembler Backend --------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

#include "AIE2PAsmBackend.h"
#include "llvm/MC/MCObjectStreamer.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

bool AIE2PAsmBackend::isCall(unsigned Opcode) const {
  switch (Opcode) {
  // TODO Add Call Opcode e.g. JL
  default:
    break;
  }
  return false;
}

bool AIE2PAsmBackend::isDelaySlotInstr(unsigned Opcode) const {
  switch (Opcode) {
  default:
    break;
  // TODO Add other opcode e.g. JL, JZ, etc.
  case AIE2P::RET:
    return true;
  }
  return false;
}

void AIE2PAsmBackend::relaxInstruction(MCInst &Inst,
                                       const MCSubtargetInfo &STI) const {
  llvm_unreachable("relaxInstruction call not expected in AIE2P");
}

unsigned AIE2PAsmBackend::maxRelaxIncrement(const MCInst &Inst,
                                            const MCSubtargetInfo &STI) const {
  return 0;
}

bool AIE2PAsmBackend::writeNopData(raw_ostream &OS, uint64_t Count,
                                   const MCSubtargetInfo *STI) const {

  // We fill with maximal packets of nops.
  // These correspond to the NOPnn instructions

  // First check instruction granularity
  const unsigned MinNopLen = 2;
  if ((Count % MinNopLen) != 0)
    return false;
  // First shave off cycles of sixteen bytes
  while (Count >= 16) {
    OS.write("\xe1\x00\x00\x00\x00\x00\x00\x00\x00\x5b\x01\x20\x00\xf0\x2c\x00",
             16);
    Count -= 16;
  }

  // Then do the tail
  switch (Count) {
  case 2:
    OS.write("\x00\0", 2);
    break;
  case 4:
    OS.write("\x18\x00\x00\x10", 4);
    break;
  case 6:
    OS.write("\x04\x00\x00\x00\x00\x00", 6);
    break;
  case 8:
    OS.write("\x22\x1c\x00\x00\x04\xf0\x2c\x00", 8);
    break;
  case 10:
    OS.write("\xba\x72\xa5\x01\x00\x5b\x01\xf0\x2c\x00", 10);
    break;
  case 12:
    OS.write("\x26\x1c\x00\x00\xe2\x4a\x03\x20\x00\xf0\x2c\x00", 12);
    break;
  case 14:
    OS.write("\x2e\x1c\x00\x00\x57\x1a\x40\x00\x00\xb6\x02\xf0\x2c\x00", 14);
    break;
  default:
    assert(Count == 0);
  }

  return true;
}
