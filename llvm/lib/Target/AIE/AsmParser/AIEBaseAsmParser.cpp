//===-- AIEBaseAsmParser.cpp - AIE assembly parser registration -----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Force static initialization of all AIE sub-target asm parsers.
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

void initializeAIE1AsmParser();
void initializeAIE2AsmParser();
void initializeAIE2PAsmParser();
void initializeAIE2PSAsmParser();

// Force static initialization of all AIE sub-target asm parsers.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeAIEAsmParser() {
  initializeAIE1AsmParser();
  initializeAIE2AsmParser();
  initializeAIE2PAsmParser();
  initializeAIE2PSAsmParser();
}
