//===- AIEMCFormats.cpp -----------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
#include "AIEMCFormats.h"
#include "AIE.h"

#undef DEBUG_TYPE
#define DEBUG_TYPE "mcformats"

namespace llvm {

#define GET_FORMATS_PACKETS_TABLE
#define GET_FORMATS_CLASS_DEF
#define GET_FORMATS_SLOTS_DEFS
#define GET_FORMATS_SLOTINFOS_MAPPING
#define GET_OPCODE_FORMATS_INDEX_FUNC
#define GET_ALTERNATE_INST_OPCODE_FUNC
#include "AIEGenFormats.inc"

namespace AIE {
#define GET_FORMATS_INFO
#define GET_FORMATS_FORMATS_DEFS
#include "AIEGenFormats.inc"
} // end namespace AIE

/***************** AIEInstFormat *******************/

bool AIEInstFormat::hasSingleSlot() const {
  return (SlotsMap.size() == 1 && !HasMultipleSlotOptions);
}

unsigned AIEInstFormat::getSlotOffsetLoBit() const {
  MCSlotKind Kind = getSlot();
  return getSlotOffsetsLoBit(Kind).RightOffset;
}

unsigned AIEInstFormat::getSlotOffsetHiBit() const {
  MCSlotKind Kind = getSlot();
  return getSlotOffsetsHiBit(Kind).LeftOffset;
}

const MCSlotKind AIEInstFormat::getSlot() const {
  // NOTE: for a regular instruction (not packet), the map contains only 1
  // value.
  // NOTE: so... Overkill to use a map in this case?
  assert(hasSingleSlot());
  return SlotsMap.begin()->first;
}


/***************** AIEMCFormats *******************/

const MCFormatDesc *AIEMCFormats::getMCFormats() const { return AIE::Formats; }

const PacketFormats &AIEMCFormats::getPacketFormats() const { return Formats; }

} // end namespace llvm
