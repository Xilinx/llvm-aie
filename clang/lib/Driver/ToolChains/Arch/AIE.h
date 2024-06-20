//===--- AIE.h - AIE-specific Tool Helpers ------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_AIE_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_AIE_H

#include "clang/Driver/Driver.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Option/Option.h"
#include <string>
#include <vector>

namespace clang {
namespace driver {
namespace tools {
namespace aie {
void getAIETargetFeatures(const Driver &D, const llvm::opt::ArgList &Args,
                            std::vector<llvm::StringRef> &Features);
StringRef getAIEABI(const llvm::opt::ArgList &Args,
                      const llvm::Triple &Triple);
} // end namespace aie
} // namespace tools
} // end namespace driver
} // end namespace clang

#endif // LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_ARCH_AIE_H
