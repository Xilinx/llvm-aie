//===-- AIE2AsmBackend.cpp - AIE2 Assembler Backend ---------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE2AsmBackend.h"
#include "llvm/MC/MCObjectStreamer.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

bool AIE2AsmBackend::isCall(unsigned Opcode) const {
  switch (Opcode) {
  default:
    break;
  case AIE2::JL:
  case AIE2::JL_IND:
    return true;
  }
  return false;
}

bool AIE2AsmBackend::isDelaySlotInstr(unsigned Opcode) const {
  switch (Opcode) {
  default:
    break;
  case AIE2::JL:
  case AIE2::JL_IND:
  case AIE2::J_jump_imm:
  case AIE2::J_jump_ind:
  case AIE2::JNZ:
  case AIE2::JZ:
  case AIE2::RET:
  case AIE2::JNZD:
    return true;
  }
  return false;
}

void AIE2AsmBackend::relaxInstruction(MCInst &Inst,
                                      const MCSubtargetInfo &STI) const {

  switch (Inst.getOpcode()) {
  default:
    break;
  case AIE2::NOP:
    Inst.setOpcode(AIE2::NOP32);
    return;
  case AIE2::NOP32:
    Inst.setOpcode(AIE2::NOP48);
    return;
  case AIE2::NOP48:
    Inst.setOpcode(AIE2::NOP64);
    return;
  case AIE2::NOP64:
    Inst.setOpcode(AIE2::NOP80);
    return;
  case AIE2::NOP80:
    Inst.setOpcode(AIE2::NOP96);
    return;
  case AIE2::NOP96:
    Inst.setOpcode(AIE2::NOP112);
    return;
  case AIE2::NOP112:
    Inst.setOpcode(AIE2::NOP128);
    return;
  }
  llvm_unreachable("Opcode not expected!");
}

unsigned AIE2AsmBackend::maxRelaxIncrement(const MCInst &Inst,
                                           const MCSubtargetInfo &STI) const {

  switch (Inst.getOpcode()) {
  default:
    break;
  case AIE2::NOP:
  case AIE2::NOP32:
  case AIE2::NOP48:
  case AIE2::NOP64:
  case AIE2::NOP80:
  case AIE2::NOP96:
  case AIE2::NOP112:
    return 2;
  }

  return 0;
}

bool AIE2AsmBackend::writeNopData(raw_ostream &OS, uint64_t Count,
                                  const MCSubtargetInfo *STI) const {
  // TODO: There is common logic with AIE1. This should be abstracted away as
  // much as possible. Same goes for relaxation.

  // We fill with maximal packets of nops.
  // These correspond to the NOPnn instructions

  // First check instruction granularity
  const unsigned MinNopLen = 2;
  if ((Count % MinNopLen) != 0)
    return false;

  // First shave off cycles of sixteen bytes
  while (Count >= 16) {
    OS.write("\xc0\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
             16);
    Count -= 16;
  }

  // Then do the tail
  switch (Count) {
  case 2:
    OS.write("\x01\0", 2);
    break;
  case 4:
    OS.write("\x19\x00\x00\x10", 4);
    break;
  case 6:
    OS.write("\x15\x00\x00\x00\x00\x00", 6);
    break;
  case 8:
    OS.write("\x03\x80\x03\x00\x00\x00\x00\x00", 8);
    break;
  case 10:
    OS.write("\xbb\x8e\x03\x00\x00\x00\x00\x00\x00\x00", 10);
    break;
  case 12:
    OS.write("\x37\x88\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00", 12);
    break;
  case 14:
    OS.write("\x2f\x78\x00\x00\x38\x00\x40\x00\x00\x00\x00\x00\x00\x00", 14);
    break;
  default:
    assert(Count == 0);
  }

  return true;
}
