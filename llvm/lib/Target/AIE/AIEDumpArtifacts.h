//===- AIEDumpArtifacts.h - Passes to dump out IR constructs ---*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
/// \file
///
/// This file contains an interface for creating passes to dump out IR or
/// MIR in various granularities.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_DUMP_ARTIFACTS_H
#define LLVM_LIB_TARGET_AIE_DUMP_ARTIFACTS_H

#include <string>

namespace llvm {
class raw_ostream;
class StringRef;
class MachineFunctionPass;
class ModulePass;
class Pass;
class PassRegistry;

/// Create and return a pass that dumps the module to local storage.
ModulePass *createDumpModulePass(const std::string &Suffix = "",
                                 bool ShouldPreserveUseListOrder = false);

/// Create and return a pass that dumps MIR to local storage.
MachineFunctionPass *
createMachineFunctionDumperPass(const std::string &Suffix = "");

void initializeAIEDumpModulePassWrapperPass(PassRegistry &);
void initializeAIEMachineFunctionDumperPass(PassRegistry &);

} // namespace llvm

#endif
