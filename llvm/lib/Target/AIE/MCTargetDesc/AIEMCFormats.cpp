//===- AIEMCFormats.cpp -----------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
#include "AIEMCFormats.h"
#include "../AIESlotStructure.h"
#include "AIE.h"

#undef DEBUG_TYPE
#define DEBUG_TYPE "mcformats"

namespace llvm {

#define GET_FORMATS_PACKETS_TABLE
#define GET_FORMATS_SLOTS_DEFS
#define GET_FORMATS_SLOTINFOS_MAPPING
#define GET_OPCODE_FORMATS_INDEX_FUNC
#define GET_ALTERNATE_INST_OPCODE_FUNC
#define GET_SLOTSTRUCTURE_NUMREALSLOTS
#define GET_SLOTSTRUCTURE_COMPOSITIONS
#define GET_SLOTSTRUCTURE_MSP_OPCODE_TO_CLASS
#define GET_SLOTSTRUCTURE_MSP_MATERIALIZATION
#include "AIEGenFormats.inc"

namespace AIE {
#define GET_FORMATS_INFO
#define GET_FORMATS_FORMATS_DEFS
#include "AIEGenFormats.inc"
} // end namespace AIE

/// AIE1-specific SlotStructure implementation
class AIESlotStructureImpl final : public AIESlotStructure {
public:
  unsigned getNumRealSlots() const override { return NumRealSlots; }

  SlotBits getMSPComposition(unsigned ClassIdx) const override {
    if (ClassIdx >= TotalSlotClasses) {
      return 0;
    }
    return SlotCompositions[ClassIdx];
  }
};

// Static instance
static const AIESlotStructureImpl SlotStructureInstance;

/***************** AIEInstFormat *******************/

bool AIEInstFormat::hasSingleSlot() const {
  return (SlotsMap.size() == 1 && !HasMultipleSlotOptions);
}

unsigned AIEInstFormat::getLittleEndianSlotOffset() const {
  assert(!HasMultipleSlotOptions);
  MCSlotKind Kind = getSlot();
  auto BEOffsets = SlotsMap.at(Kind).FormatField->getOffsets();
  // Offsets are stored as big-endian indexes.
  // We need to make a transformation:
  return getFormatSize() - BEOffsets.RightOffset - 1;
}

unsigned AIEInstFormat::getBigEndianSlotOffset() const {
  MCSlotKind Kind = getSlot();
  return getSlotOffsetsHiBit(Kind).LeftOffset;
}

const MCSlotKind AIEInstFormat::getSlot() const {
  // NOTE: for a regular instruction (not packet), the map contains only 1
  // value.
  assert(hasSingleSlot());
  return SlotsMap.begin()->SlotKind;
}

/***************** AIEMCFormats *******************/

const MCFormatDesc *AIEMCFormats::getMCFormats() const { return AIE::Formats; }

const PacketFormats &AIEMCFormats::getPacketFormats() const { return Formats; }

ArrayRef<bool> AIEMCFormats::getIsFormatAvailable() const {
  return FormatAvailable;
}

const AIESlotStructure &AIEMCFormats::getSlotStructure() const {
  return SlotStructureInstance;
}

MultiSlotClass AIEMCFormats::getMultiSlotClass(unsigned Opcode) const {
  return getMSPClassIndexForOpcode(Opcode);
}

unsigned AIEMCFormats::getMaterializedOpcode(unsigned Opcode,
                                             unsigned SlotIdx) const {
  return getMaterializedOpcodeImpl(Opcode, SlotIdx);
}

} // end namespace llvm
