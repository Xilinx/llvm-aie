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
}

llvm::SmallVector<Builtin::InfosShard> AIETargetInfo::getTargetBuiltins() const {
  return {{&BuiltinStrings, BuiltinInfos}};
}
