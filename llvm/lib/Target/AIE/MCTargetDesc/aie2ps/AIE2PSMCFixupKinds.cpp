//===-- AIE2PSMCFixupKinds.cpp - AIE2ps Specific Fixup Entries --*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

#include "AIE2PSMCFixupKinds.h"
#include "AIEMCTargetDesc.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <memory>
#include <vector>

using namespace llvm;

const static std::map<unsigned, FixupFlag> AIE2PSInstrFixupFlags = {};

// Need to be placed after InstrFixupFlags definition
// Define before include to use custom factory function below
#define AIE2PS_CUSTOM_FIXUP_KINDS
#define GET_MCFIXUPKINDS_IMPLEM
#include "FixupInfo/AIE2PSFixupInfo.inc"

// Override for AIE2PS to handle PC-relative fixup selection
namespace llvm {
class AIE2PSMCFixupKinds : public AIEMCFixupKinds {
public:
  using AIEMCFixupKinds::AIEMCFixupKinds;

  bool isPCRelFixup(MCFixupKind Kind) const override {
    if (!isTargetFixup(Kind))
      return false;
    const unsigned FixupNum = Kind - AIE2PS::fixup_aie2ps_1 + 1;
    // ZOL (Zero-Overhead Loop) fixups: 139-172
    return (FixupNum >= 139 && FixupNum <= 172);
  }
};

// Provide the factory function that returns our custom subclass
std::unique_ptr<AIEMCFixupKinds> createAIE2PSMCFixupKinds() {
  return std::make_unique<AIE2PSMCFixupKinds>(
      AIE2PSFixupFieldsInfos, AIE2PSFixupFieldsMapper, AIE2PSFixupFormatSize,
      AIE2PSFixupFlagMap, AIE2PSInstrFixupFlags);
}
} // namespace llvm
