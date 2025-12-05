//===- AIE2PMCFormats.cpp --------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "../../AIESlotStructure.h"
#include "AIE2PMCTargetDesc.h"
#include "AIEMCFormats.h"

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
#include "AIE2PGenFormats.inc"

namespace AIE2P {
#define GET_FORMATS_FORMATS_DEFS
#include "AIE2PGenFormats.inc"
} // namespace AIE2P

/// AIE2P-specific SlotStructure implementation
class AIE2PSlotStructureImpl final : public AIESlotStructure {
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
static const AIE2PSlotStructureImpl SlotStructureInstance;

/***************** AIE2PMCFormats *******************/

const MCFormatDesc *AIE2PMCFormats::getMCFormats() const {
  return AIE2P::Formats;
}

const PacketFormats &AIE2PMCFormats::getPacketFormats() const {
  return Formats;
}

ArrayRef<bool> AIE2PMCFormats::getIsFormatAvailable() const {
  return FormatAvailable;
}

const AIESlotStructure &AIE2PMCFormats::getSlotStructure() const {
  return SlotStructureInstance;
}

SmallVector<MCSlotKind, 2> AIE2PMCFormats::getLoadSlotKinds() const {
  return {MCSlotKind::AIE2P_SLOT_LDB, MCSlotKind::AIE2P_SLOT_LDA};
}

MultiSlotClass AIE2PMCFormats::getMultiSlotClass(unsigned Opcode) const {
  return getMSPClassIndexForOpcode(Opcode);
}

unsigned AIE2PMCFormats::getMaterializedOpcode(unsigned Opcode,
                                               unsigned SlotIdx) const {
  return getMaterializedOpcodeImpl(Opcode, SlotIdx);
}

} // end namespace llvm
