//===- AIE2MCFormats.cpp ----------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE2MCTargetDesc.h"
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
#include "AIE2GenFormats.inc"
namespace AIE2 {
#define GET_FORMATS_INFO
#define GET_FORMATS_FORMATS_DEFS
#include "AIE2GenFormats.inc"
} // namespace AIE2

/***************** AIEMCFormats *******************/

const MCFormatDesc *AIE2MCFormats::getMCFormats() const {
  return AIE2::Formats;
}

const PacketFormats &AIE2MCFormats::getPacketFormats() const { return Formats; }

} // end namespace llvm
