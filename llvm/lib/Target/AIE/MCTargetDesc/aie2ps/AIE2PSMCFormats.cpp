//===- AIE2PSMCFormats.cpp --------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE2PSMCTargetDesc.h"
#include "AIEMCFormats.h"

#undef DEBUG_TYPE
#define DEBUG_TYPE "mcformats"

namespace llvm {

#define GET_FORMATS_PACKETS_TABLE
#define GET_FORMATS_CLASS_DEF
#define GET_FORMATS_SLOTS_DEFS
#define GET_FORMATS_SLOTINFOS_MAPPING
#define GET_OPCODE_FORMATS_INDEX_FUNC
#define GET_ALTERNATE_INST_OPCODE_FUNC
#include "AIE2PSGenFormats.inc"
namespace AIE2PS {
#define GET_FORMATS_INFO
#define GET_FORMATS_FORMATS_DEFS
#include "AIE2PSGenFormats.inc"
} // namespace AIE2PS

/***************** AIEMCFormats *******************/

const MCFormatDesc *AIE2PSMCFormats::getMCFormats() const {
  return AIE2PS::Formats;
}

const PacketFormats &AIE2PSMCFormats::getPacketFormats() const {
  return Formats;
}

ArrayRef<bool> AIE2PSMCFormats::getIsFormatAvailable() const {
  return FormatAvailable;
}

SmallVector<MCSlotKind, 2> AIE2PSMCFormats::getLoadSlotKinds() const {
  return {MCSlotKind::AIE2PS_SLOT_LDB, MCSlotKind::AIE2PS_SLOT_LDA};
}

} // end namespace llvm
