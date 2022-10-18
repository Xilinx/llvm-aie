//===- AIEBaseMCFormats.cpp -------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
#include "AIEMCFormats.h"

#undef DEBUG_TYPE
#define DEBUG_TYPE "mcformats"

namespace llvm {

const MCSlotKind AIEPacketFormat::getSlot(unsigned Idx) const {
  // NOTE: we can't directly retrieve the slot by querying the SlotsMap as the
  // map doesn't keep the VLIW ordering (using the integer ordering of the enum
  // kind).
  // NOTE: probably a bit of overhead here
  return (*OpFormatMapper.at(Idx).begin())->getSlotKind();
}

unsigned AIEPacketFormat::getNumberOfSlot() const {
  // NOTE: property here SlotsMap.size() == OperandMap.size()
  return SlotsMap.size();
}

unsigned AIEPacketFormat::getSlotOffsetLoBit(unsigned Idx) const {
  MCSlotKind Kind = getSlot(Idx);
  return getSlotOffsetsLoBit(Kind).RightOffset;
}

unsigned AIEPacketFormat::getSlotOffsetHiBit(unsigned Idx) const {
  MCSlotKind Kind = getSlot(Idx);
  return getSlotOffsetsHiBit(Kind).LeftOffset;
}

unsigned AIEPacketFormat::getSlotOffsetLoBit(const MCSlotKind Kind) const {
  return getSlotOffsetsLoBit(Kind).RightOffset;
}

unsigned AIEPacketFormat::getSlotOffsetHiBit(const MCSlotKind Kind) const {
  return getSlotOffsetsHiBit(Kind).LeftOffset;
}

const AIEPacketFormat &
AIEBaseMCFormats::getPacketFormat(unsigned int Opcode) const {
  return *cast<AIEPacketFormat>(&getFormatDesc(Opcode));
}

const AIEInstFormat &
AIEBaseMCFormats::getSubInstFormat(unsigned int Opcode) const {
  return *cast<AIEInstFormat>(&getFormatDesc(Opcode));
}

const MCFormatDesc &AIEBaseMCFormats::getFormatDesc(unsigned int Opcode) const {
  if (auto const TableIdx = getFormatDescIndex(Opcode)) {
    unsigned int TableIdxVal = TableIdx.value();
    const MCFormatDesc *Formats = getMCFormats();
    assert(Formats[TableIdxVal].getOpcode() == Opcode);
    return Formats[TableIdxVal];
  } else {
    // Trigger an unreachable if the data isn't available
    LLVM_DEBUG(dbgs() << "Unsupported instruction: " << Opcode << "\n"
                      << "please verify that it isn't Pseudo/CodeGenOnly\n");
    llvm_unreachable("[InstrFormats] Unsupported instruction");
  }
}

bool AIEBaseMCFormats::isSupportedInstruction(unsigned int Opcode) const {
  // getFormatDescIndex(...) defined in AIE2GenFormats.inc
  return getFormatDescIndex(Opcode) ? true : false;
}

bool AIEBaseMCFormats::hasMultipleSlotOptions(unsigned Opcode) const {
  const std::vector<unsigned int> *AlternateInsts =
      getAlternateInstsOpcode(Opcode);
  if (AlternateInsts)
    return (AlternateInsts->size() > 1);
  else
    return false;
}

std::optional<const unsigned>
AIEBaseMCFormats::getMaterializableOpcodeForSlot(unsigned int Opcode,
                                                 MCSlotKind TargetSlot) const {
  const std::vector<unsigned int> *AlternateInsts =
      getAlternateInstsOpcode(Opcode);
  if (AlternateInsts) {
    // Find the Opcode from possible AlternateInstsOpcode set which matches
    // the slot.
    for (const auto AltInstOpcode : *AlternateInsts) {
      if (getSlotKind(AltInstOpcode) == TargetSlot) {
        return AltInstOpcode;
      }
    }
  }

  return std::nullopt;
}

const MCSlotKind AIEBaseMCFormats::getSlotKind(unsigned int Opcode) const {
  // First, we check that the instruction has a format defined.
  // Some KILLs instructions are still in the pipeline for example...
  if (!isSupportedInstruction(Opcode))
    // (Slot initialized to default/unknown)
    return MCSlotKind();
  // Then we retrieve the format.
  const AIEInstFormat &SIF = getSubInstFormat(Opcode);
  return SIF.getSlot();
}

const std::vector<MCSlotKind>
AIEBaseMCFormats::getSlotAlternatives(unsigned int Opcode) const {
  // First, we check that the instruction has a format defined.
  // Some KILLs instructions are still in the pipeline for example...
  if (!isSupportedInstruction(Opcode)) {
    // (No known valid slot for given Opcode)
    return {};
  }

  std::vector<MCSlotKind> Slots;
  const std::vector<unsigned int> *AlternateInsts =
      getAlternateInstsOpcode(Opcode);
  if (AlternateInsts) {
    for (const auto AltInstOpcode : *AlternateInsts) {
      const MCSlotKind Slot = getSlotKind(AltInstOpcode);
      if (Slot != MCSlotKind())
        Slots.push_back(Slot);
      else {
        LLVM_DEBUG(
            dbgs() << "Unsupported instruction: " << Opcode << "\n"
                   << "please verify that it isn't Pseudo/GenericOpCode\n");
        llvm_unreachable("Unknown Slot for Multi-Slot Pseudo");
      }
    }
  } else {
    const MCSlotKind Slot = getSlotKind(Opcode);
    if (Slot != MCSlotKind())
      Slots.push_back(Slot);
    else {
      LLVM_DEBUG(
          dbgs()
          << "Unsupported instruction: " << Opcode << "\n"
          << "please verify that it isn't Multi-Slot/Pseudo/GenericOpCode\n");
      llvm_unreachable("Unknown Slot could be Pseudo/Generic Opcode");
    }
  }
  return Slots;
}

} // end namespace llvm
