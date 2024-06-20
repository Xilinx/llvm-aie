//===--- AIE.h - Declare AIE target feature support -------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares AIE TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_AIE_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_AIE_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/TargetParser/Triple.h"
#include "llvm/Support/Compiler.h"

namespace clang {
namespace targets {

static const char *const DataLayoutStringAIE =
    "e-m:e-p:20:32-i1:8:32-i8:8:32-i16:16:32-"
    "i32:32:32-f32:32:32-i64:32-f64:32-a:0:32-n32";

static const char *const DataLayoutStringAIE2 =
    "e-m:e-p:20:32-i1:8:32-i8:8:32-i16:16:32-"
    "i32:32:32-f32:32:32-i64:32-f64:32-a:0:32-n32";

class LLVM_LIBRARY_VISIBILITY AIETargetInfo : public TargetInfo {

  // TODO aie1 is currently named as aie, perhaps we should change the
  // triple name to aie1?
  static bool isAIE1(const llvm::Triple &TT) {
    return TT.getArch() == llvm::Triple::aie;
  }
  static bool isAIE2(const llvm::Triple &TT) {
    return TT.getArch() == llvm::Triple::aie2;
  }

public:
  AIETargetInfo(const llvm::Triple &Triple, const TargetOptions &)
      : TargetInfo(Triple) {
    NoAsmVariants = true;
    LongLongAlign = 32;
    SuitableAlign = 32;
    DoubleAlign = LongDoubleAlign = 32;
    SizeType = UnsignedInt;
    PtrDiffType = SignedInt;
    IntPtrType = SignedInt;
    UseZeroLengthBitfieldAlignment = true;

    // Vector types on AIE have maximum 32-byte alignment
    MaxVectorAlign = 256;

    resetDataLayout(isAIE2(getTriple()) ? DataLayoutStringAIE2
                                        : DataLayoutStringAIE);
    if (hasBFloat16Type()) {
      BFloat16Width = BFloat16Align = 16;
      BFloat16Format = &llvm::APFloat::BFloat();
    }
  }

  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override;

  ArrayRef<Builtin::Info> getTargetBuiltins() const override;

  BuiltinVaListKind getBuiltinVaListKind() const override {
    return TargetInfo::VoidPtrBuiltinVaList;
  }

  std::string_view getClobbers() const override { return ""; }

  // FIXME: Make sure this matches with RegisterInfo in LLVM.
  ArrayRef<const char *> getGCCRegNames() const override {
    static const char *const GCCRegNames[] = {
        "r0", "r1",  "r2",  "r3",  "r4",  "r5",  "r6",  "r7", "r8",
        "r9", "r10", "r11", "r12", "r13", "r14", "r15", "sp", "lr"};
    return llvm::ArrayRef(GCCRegNames);
  }

  ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override {
    return std::nullopt;
  }

  bool validateAsmConstraint(const char *&Name,
                             TargetInfo::ConstraintInfo &Info) const override {
    return false;
  }
  bool hasBitIntType() const override { return true; }
  bool hasBFloat16Type() const override { return isAIE2(getTriple()); }
  bool hasInt128Type() const override { return isAIE2(getTriple()); }
  bool isCLZForZeroUndef() const override {
    if (isAIE2(getTriple()))
      return false;
    return true;
  }
  /// Return the mangled code of bfloat. this is needed, else it asserts
  // in TargetInfo.h
  const char *getBFloat16Mangling() const override { return "u6__bf16"; };
};

} // namespace targets
} // namespace clang

#endif // LLVM_CLANG_LIB_BASIC_TARGETS_AIE_H
