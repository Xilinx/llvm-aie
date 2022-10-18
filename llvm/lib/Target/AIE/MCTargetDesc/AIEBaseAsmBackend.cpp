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

bool AIEBaseAsmBackend::hasDelaySlotInstr(const MCInst &Inst, bool &IsCall,
                                          const Triple &TT) const {

  unsigned Opcode = Inst.getOpcode();

  // If it is a plain instruction having delay slot.
  if (isDelaySlotInstr(Opcode)) {
    IsCall = isCall(Opcode);
    return true;
  }

  // AIE1 does not support bundling delay slot instructions.
  if (TT.isAIE1())
    return false;

  // If it is a bundle having a delay slot instr.
  for (const MCOperand &Op : Inst) {
    if (!Op.isInst())
      continue;
    const unsigned SubOpcode = Op.getInst()->getOpcode();
    if (isDelaySlotInstr(SubOpcode)) {
      IsCall = isCall(SubOpcode);
      return true;
    }
  }

  return false;
}

void AIEBaseAsmBackend::emitInstructionEnd(MCObjectStreamer &OS,
                                           const MCInst &Inst) {
  bool IsCall = false;
  const Triple &TT = STI.getTargetTriple();
  if (hasDelaySlotInstr(Inst, IsCall, TT)) {
    if (DelaySlot > 0)
      llvm_unreachable("Cannot have branch in branch delay slot!\n");

    // AIE branches have 5 delay slots.
    DelaySlot = 5;
    NeedsAlign = IsCall;
    return;
  }

  // Count down and insert an alignment directive after the last delay slot.
  // The goal is to force the instruction after the last delay slot (i.e. the
  // target of a RET instruction) to be 16-byte aligned.
  // This is required by the architecture.
  // However, we can't simply insert NOOPs since that would change the
  // scheduling of the branch.
  if (DelaySlot > 0) {
    if (NeedsAlign && DelaySlot == 1)
      OS.insert(new MCAlignByPaddingFragment(16));
    DelaySlot--;
  }
}

// Actually 'canBeRelaxed'. Returning true here will insert it in a relaxable
// fragment, which will be picked up by the return address alignment padding
bool AIEBaseAsmBackend::mayNeedRelaxation(const MCInst &Inst,
                                          const MCSubtargetInfo &STI) const {
  return maxRelaxIncrement(Inst, STI) > 0;
}

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

void AIEBaseAsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                                   const MCValue &Target,
                                   MutableArrayRef<char> Data, uint64_t Value,
                                   bool IsResolved,
                                   const MCSubtargetInfo *STI) const {
  if (!Value)
    return; // Doesn't change encoding.

  MCContext &Ctx = Asm.getContext();

  // Apply any target-specific value adjustments.
  Value = adjustFixupValue(Fixup, Value, Ctx);

  MCFixupKindInfo Info = getFixupKindInfo(Fixup.getKind());

  // Shift the value into position.
  Value <<= Info.TargetOffset;

  unsigned Offset = Fixup.getOffset();
  unsigned NumBytes = alignTo(Info.TargetSize + Info.TargetOffset, 8) / 8;

  assert(Offset + NumBytes <= Data.size() && "Invalid fixup offset!");

  // For each byte of the fragment that the fixup touches, mask in the
  // bits from the fixup value.
  for (unsigned i = 0; i != NumBytes; ++i) {
    Data[Offset + i] |= uint8_t((Value >> (i * 8)) & 0xff);
  }
}

std::unique_ptr<MCObjectTargetWriter>
AIEBaseAsmBackend::createObjectTargetWriter() const {
  return createAIEELFObjectWriter(OSABI, STI.getTargetTriple());
}
