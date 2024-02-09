//===- AIE.cpp ------------------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "ABIInfoImpl.h"
#include "TargetInfo.h"

using namespace clang;
using namespace clang::CodeGen;

//===----------------------------------------------------------------------===//
// AIE ABI Implementation
//===----------------------------------------------------------------------===//

namespace {
class AIEABIInfo : public DefaultABIInfo {
public:
  AIEABIInfo(CodeGen::CodeGenTypes &CGT) : DefaultABIInfo(CGT) {}

  void computeInfo(CGFunctionInfo &FI) const override {
    if (!getCXXABI().classifyReturnType(FI))
      FI.getReturnInfo() = classifyReturnType(FI.getReturnType());
    for (auto &I : FI.arguments())
      I.info = classifyArgumentType(I.type);
  }

  ABIArgInfo classifyArgumentType(QualType RetTy) const;
  ABIArgInfo classifyReturnType(QualType RetTy) const;
};
} // end anonymous namespace

ABIArgInfo AIEABIInfo::classifyReturnType(QualType RetTy) const {
  if (RetTy->isVoidType())
    return ABIArgInfo::getIgnore();

  // Aggregates can be returned in registers if they don't exceed 16 bytes.
  // In particular: this also means that a struct of 16 chars gets returned
  // directly. An exception to this are sparse types which should be returned
  // in registers.
  const unsigned MaxAggregateReturnSize = 128;
  if (isAggregateTypeForABI(RetTy) &&
      (getContext().getTypeSize(RetTy) > MaxAggregateReturnSize &&
       !RetTy->getAsRecordDecl()->hasAttr<AIE2ReturnInRegistersAttr>()))
    return getNaturalAlignIndirect(RetTy);

  // Treat an enum type as its underlying type.
  if (const EnumType *EnumTy = RetTy->getAs<EnumType>())
    RetTy = EnumTy->getDecl()->getIntegerType();

  if (const auto *EIT = RetTy->getAs<BitIntType>())
    if (EIT->getNumBits() > MaxAggregateReturnSize)
      return getNaturalAlignIndirect(RetTy);

  return (isPromotableIntegerTypeForABI(RetTy) ? ABIArgInfo::getExtend(RetTy)
                                               : ABIArgInfo::getDirect());
}

ABIArgInfo AIEABIInfo::classifyArgumentType(QualType Ty) const {
  // Check with the C++ ABI first.
  const RecordType *RT = Ty->getAs<RecordType>();
  if (RT) {
    CGCXXABI::RecordArgABI RAA = getRecordArgABI(RT, getCXXABI());
    if (RAA == CGCXXABI::RAA_Indirect) {
      return getNaturalAlignIndirect(Ty, /*ByVal=*/false);
    }
    if (RAA == CGCXXABI::RAA_DirectInMemory) {
      return getNaturalAlignIndirect(Ty, /*ByVal=*/true);
    }
    // The stack alignment is set to 32 bytes for sparse types.
    return ABIArgInfo::getDirect(
        CGT.ConvertType(Ty), /*Offset=*/0, /*Padding=*/nullptr,
        /*CanBeFlattened=*/false,
        /*Align=*/Ty->getAsRecordDecl()->hasAttr<AIE2IsSparseAttr>() ? 32 : 0);
  }

  // Treat an enum type as its underlying type.
  if (const auto *EnumTy = Ty->getAs<EnumType>())
    Ty = EnumTy->getDecl()->getIntegerType();

  if (getContext().isPromotableIntegerType(Ty)) {
    return ABIArgInfo::getExtend(Ty);
  }

  // Setting CanBeFlattened=false is needed to keep compound types as is instead
  // of splitting then in their different members.
  llvm::Type *LTy = CGT.ConvertType(Ty);
  return ABIArgInfo::getDirect(
      LTy, /*Offset=*/0, /*Padding=*/nullptr, /*CanBeFlattened=*/false);
}

namespace {
class AIETargetCodeGenInfo : public TargetCodeGenInfo {
public:
  AIETargetCodeGenInfo(CodeGen::CodeGenTypes &CGT)
    : TargetCodeGenInfo(std::make_unique<AIEABIInfo>(CGT)) {}
};
}

std::unique_ptr<TargetCodeGenInfo>
CodeGen::createAIETargetCodeGenInfo(CodeGenModule &CGM) {
  return std::make_unique<AIETargetCodeGenInfo>(CGM.getTypes());
}
