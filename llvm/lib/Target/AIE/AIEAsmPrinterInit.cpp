//===-- AIEAsmPrinterInit.cpp - AIE AsmPrinter Registration ---------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file registers AsmPrinters for all AIE targets.
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/Compiler.h"

namespace llvm {
// Forward declarations of registration functions defined in target-specific
// files
void RegisterAIE1AsmPrinter();
void RegisterAIE2AsmPrinter();
void RegisterAIE2PAsmPrinter();
void RegisterAIE2PSAsmPrinter();
} // namespace llvm

// Force static initialization for all AIE targets.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeAIEAsmPrinter() {
  // Each target provides its own registration function
  llvm::RegisterAIE1AsmPrinter();
  llvm::RegisterAIE2AsmPrinter();
  llvm::RegisterAIE2PAsmPrinter();
  llvm::RegisterAIE2PSAsmPrinter();
}
