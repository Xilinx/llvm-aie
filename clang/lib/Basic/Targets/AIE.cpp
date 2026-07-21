//===--- AIE.cpp - Implement AIE target feature support -------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements AIE TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "clang/Basic/Builtins.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"
#include "llvm/ADT/StringSwitch.h"

using namespace clang;
using namespace clang::targets;

static constexpr int NumBuiltins =
    clang::AIE::LastTSBuiltin - Builtin::FirstTSBuiltin;

static constexpr llvm::StringTable BuiltinStrings =
    CLANG_BUILTIN_STR_TABLE_START
#define BUILTIN CLANG_BUILTIN_STR_TABLE
#include "clang/Basic/BuiltinsAIE.def"
    ;

static constexpr auto BuiltinInfos = Builtin::MakeInfos<NumBuiltins>({
#define BUILTIN CLANG_BUILTIN_ENTRY
#include "clang/Basic/BuiltinsAIE.def"
});

void AIETargetInfo::getTargetDefines(const LangOptions &Opts,
                                     MacroBuilder &Builder) const {
  Builder.defineMacro("__aie__");
  Builder.defineMacro("__AIENGINE__");
  Builder.defineMacro("__AIECC__");
  Builder.defineMacro("__PEANO__");
  Builder.defineMacro("__ELF__");

  // Capability macros derived from the resolved SubtargetFeatures. A kernel
  // selects a code path by capability (for example #if __AIE_HAS_BFP16__)
  // instead of hard-coding the generation with an arch macro.
  if (HasBFP16)
    Builder.defineMacro("__AIE_HAS_BFP16__");
  if (HasAcc2048)
    Builder.defineMacro("__AIE_HAS_ACC2048__");
}

bool AIETargetInfo::initFeatureMap(
    llvm::StringMap<bool> &Features, DiagnosticsEngine &Diags, StringRef CPU,
    const std::vector<std::string> &FeaturesVec) const {
  // Per-generation default capability set. aie2p carries native bfp16 and the
  // 2048-bit accumulator. Capability is a set, not a level, so this is keyed
  // off the concrete target rather than assumed to grow monotonically across
  // generations. User -target-feature flags are applied on top by the base.
  if (isAIE2P(getTriple())) {
    Features["bfp16"] = true;
    Features["acc2048"] = true;
  }
  return TargetInfo::initFeatureMap(Features, Diags, CPU, FeaturesVec);
}

bool AIETargetInfo::handleTargetFeatures(std::vector<std::string> &Features,
                                         DiagnosticsEngine &Diags) {
  for (const auto &Feature : Features) {
    if (Feature == "+bfp16")
      HasBFP16 = true;
    else if (Feature == "-bfp16")
      HasBFP16 = false;
    else if (Feature == "+acc2048")
      HasAcc2048 = true;
    else if (Feature == "-acc2048")
      HasAcc2048 = false;
  }
  return true;
}

bool AIETargetInfo::hasFeature(StringRef Feature) const {
  return llvm::StringSwitch<bool>(Feature)
      .Case("bfp16", HasBFP16)
      .Case("acc2048", HasAcc2048)
      .Default(false);
}

llvm::SmallVector<Builtin::InfosShard>
AIETargetInfo::getTargetBuiltins() const {
  return {{&BuiltinStrings, BuiltinInfos}};
}
