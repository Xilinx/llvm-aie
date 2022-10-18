//===-- AIEBaseMCCodeEmitter.cpp - Convert AIE code to machine code -------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIEBaseMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseMCCodeEmitter.h"
#include "MCTargetDesc/AIEMCExpr.h"
#include "MCTargetDesc/AIEMCInstrInfo.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"
#include "Utils/AIEBaseInfo.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/ScopedPrinter.h"
#include "llvm/Support/raw_ostream.h"
#include <memory>

using namespace llvm;

#define DEBUG_TYPE "mccodeemitter"

STATISTIC(MCNumEmitted, "Number of MC instructions emitted");
STATISTIC(MCNumFixups, "Number of MC fixups created");

void AIEBaseMCCodeEmitter::encodeInstruction(const MCInst &MI, SmallVectorImpl<char> &CB,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  const MCInstrDesc &Desc = MCII.get(MI.getOpcode());

  // Get byte count of instruction.
  unsigned Size = Desc.getSize();
  assert(Size <= 16);
  assert(Size % 2 == 0);

  // Get all encoding bits
  APInt Binary, Scratch;
  getBinaryCodeForInstr(MI, Fixups, Binary, Scratch, STI);

  // Emit them in chunks of 16 bits.
  const int ChunkSize = 16;
  int BitOffset = 0;
  for (size_t Bytes = 0; Bytes < Size; Bytes += 2) {
    uint16_t Chunk = Binary.extractBitsAsZExtValue(ChunkSize, BitOffset);
    support::endian::write(CB, Chunk, endianness::little);
    BitOffset += ChunkSize;
  }
  MCNumEmitted++;
}

/// Ensure that we are dealing with a valid MCExpr (Target or SymbolRef)
static bool isValidExpr(const MCExpr *Expr) {
  MCExpr::ExprKind Kind = Expr->getKind();

  switch ((unsigned)Kind) {
  case MCExpr::Target: {
    const AIEMCExpr *RVExpr = cast<AIEMCExpr>(Expr);
    return (RVExpr->getKind() == AIEMCExpr::VK_AIE_GLOBAL ||
            RVExpr->getKind() == AIEMCExpr::VK_AIE_CALL);
  }
  case MCExpr::SymbolRef:
    return true;
  }
  return false;
}

void AIEBaseMCCodeEmitter::getMachineOpValue(const MCInst &MI,
                                             const MCOperand &MO, APInt &op,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  if (MO.isReg())
    op = Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
  else if (MO.isImm())
    op = static_cast<unsigned>(MO.getImm());
  else if (MO.isExpr()) {
    const MCExpr *Expr = MO.getExpr();
    assert(isValidExpr(Expr));

    unsigned OpIdx = AIEMCInstrInfo::getMCOperandIndex(MI, MO);
    const MCFormatDesc &FormatDesc =
        FormatInterface.getFormatDesc(MI.getOpcode());

    // Retrieve the MCFormatField(s) of the current MCOperand
    ArrayRef<const MCFormatField *> RelocatableFields =
        FormatDesc.getFieldsCoveredByOpIdx(OpIdx);

    // Conversion between the 2 fields data structure
    const SmallVector<FixupField> FixupFields =
        MCFixupKinds->convertFieldsLocsInFixupFields(RelocatableFields);

    // Find the fixup matching the format size of the instruction and the set
    // of relocatable fields
    unsigned FormatSize = MCII.get(MI.getOpcode()).getSize();
    MCFixupKind FixupKind =
        MCFixupKinds->findFixupfromFixupFields(MI, FormatSize, FixupFields);

    Fixups.push_back(MCFixup::create(0, Expr, FixupKind, MI.getLoc()));
    ++MCNumFixups;

    // These bits will be relocated lately, the intermediate immediate encoding
    // is thus filled with zeroes
    op = 0;
  } else if (MO.isInst()) {
    // A MCOperand representing a nested instruction of a VLIW encoding.
    // This is only the case for a sub-instruction.
    APInt Binary, Scratch;

    // Introduce intermediate references to make the code more explicit
    const MCInst &CompositeInst = MI;
    const MCInst &SubInst = *MO.getInst();

    // Intermediate container used to store the emitted (base) Fixups
    SmallVector<MCFixup> BaseFixups;

    // Retrieve the encoding and the fixups of the standalone instruction and
    // put them respectively into the out parameters Binary and BaseFixups
    getBinaryCodeForInstr(SubInst, BaseFixups, Binary, Scratch, STI);

    // If indeed some Fixups have been emitted for the standalone instruction
    if (!BaseFixups.empty()) {
      // Make the translation "local fixup" (Standalone instruction scope) to
      // the "global fixup" (Composite instruction scope)
      SmallVector<MCFixup> TranslatedFixups =
          translateFixupsInComposite(CompositeInst, SubInst, BaseFixups);
      // Push the translatedFixups into the out parameter
      Fixups.append(TranslatedFixups);
    }

    // Let's check if we have an instr16.
    // Get the size of the sub-instruction in bytes.
    unsigned SubInstSize = MCII.get(SubInst.getOpcode()).getSize();
    LLVM_DEBUG(if (SubInstSize == 2) {
      llvm::dbgs() << "Warning: instr16 encountered\n";
    });

    // Retrieve the slot number of the sub-instr to emit at this call and put
    // it in the out parameter SlotIdx
    unsigned SlotIdx = AIEMCInstrInfo::getMCOperandIndex(CompositeInst, MO);

    const AIEPacketFormat &CompositeFormat =
        FormatInterface.getPacketFormat(CompositeInst.getOpcode());
    const AIEInstFormat &SubInstFormat =
        FormatInterface.getSubInstFormat(SubInst.getOpcode());

    // Retrieve the slot kind information of the current composite format
    const MCSlotKind Kind = CompositeFormat.getSlot(SlotIdx);

    // Is it equal to the empty/unknown Kind?
    if (Kind == MCSlotKind()) {
      SmallString<40> S;
      op.toString(S, 16, true);
      llvm::dbgs() << MO.getInst()->getOpcode() << " op=" << S << "\n";
      report_fatal_error("Instruction with an unknown slot!");
    }

    const MCSlotInfo *SlotInfo = FormatInterface.getSlotInfo(Kind);
    assert(SlotInfo);

    // Get the offset of the slot in the Standalone instruction
    unsigned StartPos = SubInstFormat.getSlotOffsetLoBit();

    // Extract the encoding of the slot in the Standalone instruction (to
    // prepare its emission as operand in the top-level Composite instruction)
    // from the APInt and put it into the out parameter.
    op = Binary.extractBits(SlotInfo->getSize(), StartPos);
  } else
    llvm_unreachable("Unhandled operand!");
}

SmallVector<MCFixup> AIEBaseMCCodeEmitter::translateFixupsInComposite(
    const MCInst &CompositeInst, const MCInst &SubInst,
    SmallVector<MCFixup> &BaseFixups) const {

  // Initialize our intermediate container
  SmallVector<MCFixup> TranslatedFixups;
  TranslatedFixups.reserve(BaseFixups.size());

  const AIEPacketFormat &CompositeFormat =
      FormatInterface.getPacketFormat(CompositeInst.getOpcode());
  const AIEInstFormat &SubInstFormat =
      FormatInterface.getSubInstFormat(SubInst.getOpcode());

  // Retrieve the slot of the sub-Instruction
  const MCSlotKind Slot = SubInstFormat.getSlot();

  // Get the current slot offset in the standalone instruction
  unsigned SlotOffsetInSubInstruction = SubInstFormat.getSlotOffsetHiBit();
  // Get the expected slot offset in the Composite Format
  unsigned SlotOffsetInCompositeInstr =
      CompositeFormat.getSlotOffsetHiBit(Slot);

  for (const MCFixup &Fixup : BaseFixups) {
    SmallVector<FixupField> TranslatedFields;
    // Retrieve the list of relocatable field(s) of our current fixup
    const SmallVector<FixupField> &RelocFields =
        MCFixupKinds->getFixupFields(Fixup.getKind());

    // For each FixupField, perform the translation:
    // Standalone Format -> Composite Format
    for (const FixupField &RelocField : RelocFields) {
      TranslatedFields.push_back({RelocField.Offset -
                                      SlotOffsetInSubInstruction +
                                      SlotOffsetInCompositeInstr,
                                  RelocField.Size});
    }
    unsigned FormatSize = MCII.get(CompositeInst.getOpcode()).getSize();
    // Retrieve the "main" translated Fixup based on the translated relocatable
    // fields
    MCFixupKind TranslatedFixup = MCFixupKinds->findFixupfromFixupFields(
        SubInst, FormatSize, TranslatedFields);
    // Create a new MCFixup and push it into the saving container
    TranslatedFixups.push_back(MCFixup::create(
        0, Fixup.getValue(), MCFixupKind(TranslatedFixup), Fixup.getLoc()));
  }
  BaseFixups.clear();
  return TranslatedFixups;
}
