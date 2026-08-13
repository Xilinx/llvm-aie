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
  bool isAccumulatorType(const VectorType *VT) const {
    const BuiltinType *BT = VT->getElementType()->getAs<BuiltinType>();
    llvm::Triple Triple = getTarget().getTriple();
    if ((Triple.getArch() == llvm::Triple::aie2p ||
         Triple.getArch() == llvm::Triple::aie2ps) &&
        BT &&
        (BT->getKind() == BuiltinType::ACC32 ||
         BT->getKind() == BuiltinType::ACCFLOAT ||
         BT->getKind() == BuiltinType::ACC64))
      return true;
    return false;
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
    return getNaturalAlignIndirect(RetTy, getDataLayout().getAllocaAddrSpace());

  // Treat an enum type as its underlying type.
  if (const EnumType *EnumTy = RetTy->getAs<EnumType>())
    RetTy = EnumTy->getOriginalDecl()->getIntegerType();

  if (const auto *EIT = RetTy->getAs<BitIntType>())
    if (EIT->getNumBits() > MaxAggregateReturnSize)
      return getNaturalAlignIndirect(RetTy, getDataLayout().getAllocaAddrSpace());

  auto ArgInfo = ABIArgInfo::getDirect();
  const auto *VT = RetTy->getAs<VectorType>();
  if (VT) {
    // When the Inreg attribute is set, VectorType is treated
    // as an Accumulator type.
    if (isAccumulatorType(VT))
      ArgInfo.setInReg(true);
  }
  return (isPromotableIntegerTypeForABI(RetTy) ? ABIArgInfo::getExtend(RetTy)
                                               : ArgInfo);
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
    Ty = EnumTy->getOriginalDecl()->getIntegerType();

  if (getContext().isPromotableIntegerType(Ty)) {
    return ABIArgInfo::getExtend(Ty);
  }

  // Setting CanBeFlattened=false is needed to keep compound types as is instead
  // of splitting then in their different members.
  llvm::Type *LTy = CGT.ConvertType(Ty);
  ABIArgInfo ArgInfo = ABIArgInfo::getDirect(
      LTy, /*Offset=*/0, /*Padding=*/nullptr, /*CanBeFlattened=*/false);
  const auto *VT = Ty->getAs<VectorType>();
  if (VT) {
    // When the Inreg attribute is set, VectorType is treated
    // as an Accumulator type.
    if (isAccumulatorType(VT))
      ArgInfo.setInReg(true);
  }
  return ArgInfo;
}

namespace {
class AIETargetCodeGenInfo : public TargetCodeGenInfo {
public:
  AIETargetCodeGenInfo(CodeGen::CodeGenTypes &CGT)
      : TargetCodeGenInfo(std::make_unique<AIEABIInfo>(CGT)) {}
};
} // namespace

std::unique_ptr<TargetCodeGenInfo>
CodeGen::createAIETargetCodeGenInfo(CodeGenModule &CGM) {
  return std::make_unique<AIETargetCodeGenInfo>(CGM.getTypes());
}
