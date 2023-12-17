//===--- AIE.h - AIE ToolChain Implementations --------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_AIE_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_AIE_H

#include "Gnu.h"
#include "clang/Driver/Options.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {
namespace aie {

class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
  Linker(const ToolChain &TC) : Tool("aie::Linker", "ld.lld", TC) {}
  bool isLinkJob() const override { return true; }
  bool hasIntegratedCPP() const override { return false; }
  void ConstructJob(Compilation &C, const JobAction &JA,
                    const InputInfo &Output, const InputInfoList &Inputs,
                    const llvm::opt::ArgList &Args,
                    const char *LinkingOutput) const override;
};

} // end namespace aie
} // end namespace tools

namespace toolchains {

class LLVM_LIBRARY_VISIBILITY AIEToolChain : public Generic_ELF {
public:
  AIEToolChain(const Driver &D, const llvm::Triple &Triple,
               const llvm::opt::ArgList &Args);

  // No support for finding a C++ standard library yet.
  void addLibCxxIncludePaths(
      const llvm::opt::ArgList &DriverArgs,
      llvm::opt::ArgStringList &CC1Args) const override {}
  void addLibStdCxxIncludePaths(
      const llvm::opt::ArgList &DriverArgs,
      llvm::opt::ArgStringList &CC1Args) const override {}

  void AddClangSystemIncludeArgs(const llvm::opt::ArgList &DriverArgs,
                                 llvm::opt::ArgStringList &CC1Args) const override;
  void addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
                             llvm::opt::ArgStringList &CC1Args,
                             Action::OffloadKind DeviceOffloadKind) const override;
  unsigned getMaxDwarfVersion() const override;

  bool IsIntegratedAssemblerDefault() const override { return true; }
  Tool *buildLinker() const override {
      return new tools::aie::Linker(*this);
  }

  /// GetDefaultRuntimeLibType - Get the default runtime library variant to use.
  RuntimeLibType GetDefaultRuntimeLibType() const override {
    return ToolChain::RLT_CompilerRT;
  }
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif // LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_AIE_H
