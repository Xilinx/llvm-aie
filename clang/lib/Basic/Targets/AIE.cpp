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

void AIETargetInfo::getTargetDefines(const LangOptions &Opts,
                                     MacroBuilder &Builder) const {
  Builder.defineMacro("__aie__");
  Builder.defineMacro("__AIENGINE__");
  Builder.defineMacro("__AIECC__");
  Builder.defineMacro("__PEANO__");
  Builder.defineMacro("__ELF__");
}

static constexpr Builtin::Info BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS)                                               \
  {#ID, TYPE, ATTRS, nullptr, HeaderDesc::NO_HEADER, ALL_LANGUAGES},
// #define LANGBUILTIN(ID, TYPE, ATTRS, LANG)                                     \
//   {#ID, TYPE, ATTRS, nullptr, LANG, nullptr},
// #define LIBBUILTIN(ID, TYPE, ATTRS, HEADER)                                    \
//   {#ID, TYPE, ATTRS, HEADER, ALL_LANGUAGES, nullptr},
// #define TARGET_HEADER_BUILTIN(ID, TYPE, ATTRS, HEADER, LANGS, FEATURE)         \
//   {#ID, TYPE, ATTRS, HEADER, LANGS, FEATURE},
#include "clang/Basic/BuiltinsAIE.def"
};

ArrayRef<Builtin::Info> AIETargetInfo::getTargetBuiltins() const {
  return llvm::ArrayRef(BuiltinInfo,
                        clang::AIE::LastTSBuiltin - Builtin::FirstTSBuiltin);
}
