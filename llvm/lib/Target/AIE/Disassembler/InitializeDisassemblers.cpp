//===- InitializeDisassemblers.cpp ------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
#include "Utils/AIEBaseInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

MCDisassembler *createAIE1Disassembler(const Target &T,
                                       const MCSubtargetInfo &STI,
                                       MCContext &Ctx);

MCDisassembler *createAIE2Disassembler(const Target &T,
                                       const MCSubtargetInfo &STI,
                                       MCContext &Ctx);

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeAIEDisassembler() {
  // Register the disassembler for each target.
  TargetRegistry::RegisterMCDisassembler(getTheAIETarget(),
                                         createAIE1Disassembler);
  TargetRegistry::RegisterMCDisassembler(getTheAIE2Target(),
                                         createAIE2Disassembler);
}