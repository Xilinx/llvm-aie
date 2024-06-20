//===--- AIE.cpp - AIE Helpers for Tools --------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "clang/Basic/CharInfo.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "clang/Driver/Options.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/TargetParser.h"
#include "llvm/Support/raw_ostream.h"
#include "ToolChains/CommonArgs.h"

using namespace clang::driver;
using namespace clang::driver::tools;
using namespace clang;
using namespace llvm::opt;


void aie::getAIETargetFeatures(const Driver &D, const ArgList &Args,
                                   std::vector<StringRef> &Features) {
  if (const Arg *A = Args.getLastArg(options::OPT_march_EQ)) {
    StringRef MArch = A->getValue();

    // RISC-V ISA strings must be lowercase.
    if (std::any_of(std::begin(MArch), std::end(MArch),
                    [](char c) { return isupper(c); })) {

      D.Diag(diag::err_drv_invalid_aie_arch_name) << MArch
        << "string must be lowercase";
      return;
    }


  // Now add any that the user explicitly requested on the command line,
  // which may override the defaults.
  handleTargetFeaturesGroup(Args, Features, options::OPT_m_aie_Features_Group);
}

StringRef aie::getAIEABI(const ArgList &Args, const llvm::Triple &Triple) {
  if (Arg *A = Args.getLastArg(options::OPT_mabi_EQ))
    return A->getValue();

  return "ilp32";
}
