//===--- AIE.h - Declare AIE target feature support -------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
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

static const char *const DataLayoutStringAIE2P =
    "e-m:e-p:20:32-i1:8:32-i8:8:32-i16:16:32-"
    "i32:32:32-f32:32:32-i64:32-f64:32-a:0:32-n32";

static const char *const DataLayoutStringAIE2PS =
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
  static bool isAIE2P(const llvm::Triple &TT) {
    return TT.getArch() == llvm::Triple::aie2p;
  }
  static bool isAIE2PS(const llvm::Triple &TT) {
    return TT.getArch() == llvm::Triple::aie2ps;
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

    // On AIE2p and AIE2ps, vector types have a maximum alignment of 64 bytes,
    // whereas on AIE1 and AIE2, the maximum alignment is 32 bytes.
    if (isAIE2P(getTriple()) || isAIE2PS(getTriple()))
      MaxVectorAlign = 512;
    else
      MaxVectorAlign = 256;

    std::string DataLayout;

    if (isAIE1(getTriple()))
      DataLayout = DataLayoutStringAIE;
    else if (isAIE2(getTriple()))
      DataLayout = DataLayoutStringAIE2;
    else if (isAIE2P(getTriple()))
      DataLayout = DataLayoutStringAIE2P;
    else if (isAIE2PS(getTriple()))
      DataLayout = DataLayoutStringAIE2PS;
    resetDataLayout(DataLayout);

    if (hasBFloat16Type()) {
      BFloat16Width = BFloat16Align = 16;
      BFloat16Format = &llvm::APFloat::BFloat();
    }
    if (hasFloat16Type()) {
      HalfWidth = HalfAlign = 16;
      HalfFormat = &llvm::APFloat::IEEEhalf();
    }
  }

  bool isAddressSpaceSupersetOf(LangAS A, LangAS B) const override {
    return A == LangAS::Default && isTargetAddressSpace(B);
  }

  void adjust(DiagnosticsEngine &Diags, LangOptions &Opts) override {
    TargetInfo::adjust(Diags, Opts);
    // Enable native half type operations when we have legal half type support
    if (hasLegalHalfType())
      Opts.NativeHalfType = true;
  }

  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override;

  llvm::SmallVector<Builtin::InfosShard> getTargetBuiltins() const override;

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
  bool hasBFloat16Type() const override {
    return isAIE2(getTriple()) || isAIE2P(getTriple()) || isAIE2PS(getTriple());
  }
  bool hasFloat16Type() const override { return isAIE2PS(getTriple()); }
  bool hasLegalHalfType() const override { return hasFloat16Type(); }
  bool hasInt128Type() const override { return isAIE2(getTriple()); }
  bool isCLZForZeroUndef() const override {
    if (isAIE2(getTriple()) || isAIE2P(getTriple()) || isAIE2PS(getTriple()))
      return false;
    return true;
  }
  /// We treat the builtin __bf16 type as a vendor extension type.
  /// Mangle the type name using its user visible name, "bfloat16".
  const char *getBFloat16Mangling() const override { return "8bfloat16"; }

  bool treatBFloat16AsVendorType() const override { return true; }

  /// We treat the builtin _Float16 type as a vendor extension type.
  /// Mangle the type name using its user visible name, "float16".
  const char *getFloat16Mangling() const override { return "7float16"; }

  bool treatFloat16AsVendorType() const override { return true; }
};

} // namespace targets
} // namespace clang

#endif // LLVM_CLANG_LIB_BASIC_TARGETS_AIE_H
