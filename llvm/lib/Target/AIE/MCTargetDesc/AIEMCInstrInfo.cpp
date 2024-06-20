//===- AIEMCInstrInfo.cpp - Utility functions on AIE MCInsts ------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/AIEMCInstrInfo.h"
#include "AIEMCFormats.h"
#include "MCTargetDesc/AIEMCExpr.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCInstrItineraries.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include <cassert>
#include <cstdint>
#include <limits>

using namespace llvm;

namespace {
const AIEMCFormats AIE1Formats;
const AIEBaseMCFormats *FormatInterface = &AIE1Formats;
} // namespace

MCSlotKind AIEMCInstrInfo::getSlotKind(const MCInst &MCI) {
  // First, we check that the instruction has a format defined.
  // NOTE: in the MC Layer, the instruction should be automatically supported
  // as we're going to ask for its encoding. Anyway, we check it.
  if (!FormatInterface->isSupportedInstruction(MCI.getOpcode()))
    return MCSlotKind();
  // Then we retrieve the format.
  const auto &SIF = FormatInterface->getSubInstFormat(MCI.getOpcode());
  return SIF.getSlot();
}

unsigned AIEMCInstrInfo::getMCOperandIndex(MCInst const &MI,
                                           MCOperand const &MO) {
  unsigned OpIdx = 0;
  for (MCOperand const &MCO : MI) {
    if (&MCO == &MO)
      return OpIdx;
    OpIdx++;
  }
  llvm_unreachable("MO is not an MCOperand of MI");
}