//===-- AIE1AsmBackend.cpp - AIE1 Assembler Backend ---------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE1AsmBackend.h"
#include "llvm/MC/MCObjectStreamer.h"

using namespace llvm;

bool AIE1AsmBackend::isCall(unsigned Opcode) const {
  switch (Opcode) {
  default:
    break;
  case AIE::JAL:
    return true;
  }
  return false;
}

bool AIE1AsmBackend::isDelaySlotInstr(unsigned Opcode) const {
  switch (Opcode) {
  default:
    break;
  case AIE::JAL:
  case AIE::J:
  case AIE::BNEZ:
    return true;
  }
  return false;
}

void AIE1AsmBackend::relaxInstruction(MCInst &Inst,
                                      const MCSubtargetInfo &STI) const {
  switch (Inst.getOpcode()) {
  default:
    llvm_unreachable("Opcode not expected!");
  case AIE::JAL:
    Inst.setOpcode(AIE::JAL64);
    return;
  case AIE::NOP:
    Inst.setOpcode(AIE::NOP32);
    return;
  case AIE::NOP32:
    Inst.setOpcode(AIE::NOP64);
    return;
  case AIE::NOP64:
    Inst.setOpcode(AIE::NOP96);
    return;
  case AIE::NOP96:
    Inst.setOpcode(AIE::NOP128);
    return;
  }
}

unsigned AIE1AsmBackend::maxRelaxIncrement(const MCInst &Inst,
                                           const MCSubtargetInfo &STI) const {
  switch (Inst.getOpcode()) {
  default:
    break;
  case AIE::NOP:
    return 2;
  case AIE::NOP32:
  case AIE::NOP64:
  case AIE::NOP96:
    return 4;
  }
  return 0;
}

bool AIE1AsmBackend::writeNopData(raw_ostream &OS, uint64_t Count,
                                  const MCSubtargetInfo *STI) const {
  unsigned MinNopLen = 2;

  if ((Count % MinNopLen) != 0)
    return false;

  uint64_t Nop32Count = 0; // Count / 4;
  // Generate 32-bit encoding for NOP
  for (uint64_t i = Nop32Count; i != 0; --i)
    OS.write("\x0b\0\0\0", 4);

  uint64_t Nop16Count = (Count - Nop32Count * 4) / 2;
  // Generate 16-bit encoding for NOP
  for (uint64_t i = Nop16Count; i != 0; --i)
    OS.write("\x01\0", 2);

  return true;
}
