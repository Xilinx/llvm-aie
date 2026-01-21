//===- QuantOps.cpp - Quantization Type and Ops Implementation --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// Modifications (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its
// affiliates
//
//===----------------------------------------------------------------------===//

#include "QuantDialectBytecode.h"
#include "TypeDetail.h"

#include "mlir/Dialect/Quant/IR/Quant.h"
#include "mlir/Dialect/Quant/IR/QuantTypes.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/IR/TypeUtilities.h"
#include "mlir/Transforms/InliningUtils.h"

#include "mlir/Dialect/Quant/IR/QuantOpsDialect.cpp.inc"

namespace mlir {
namespace quant {

namespace {

// Verify the integrity of per-axis quantization information, if present.
//
// - quantizedType
//   Any quantized type. Any quantized type with no per-axis quantization is
//   ignored.
//
// - containerType
//   Original input or result type of the operation using the provided quantized
//   type. Used to ensure that the quantized type appears within a tensor and
//   that the tensor is compatible with per-axis quantization information.
//
LogicalResult verifyPerAxisQuantization(Operation *op,
                                        QuantizedType quantizedType,
                                        Type containerType) {
  auto quantizedPerAxisType =
      dyn_cast<UniformQuantizedPerAxisType>(quantizedType);
  if (!quantizedPerAxisType)
    return success();

  auto shapedType = dyn_cast<ShapedType>(containerType);
  if (!shapedType)
    return op->emitError("scalar types may not use per-axis quantization");

  if (!shapedType.hasRank())
    return success();

  int64_t quantizedDimension = quantizedPerAxisType.getQuantizedDimension();
  if (quantizedDimension >= shapedType.getRank())
    return op->emitError("quantized dimension must be less than tensor rank");

  int64_t quantizedDimensionSize = shapedType.getDimSize(quantizedDimension);
  if (quantizedDimensionSize != ShapedType::kDynamic &&
      quantizedDimensionSize !=
          (int64_t)quantizedPerAxisType.getScales().size())
    return op->emitError(
        "quantized dimension size does not match number of scales");

  return success();
}

// Verify the integrity of block float quantization information, if present.
//
// - quantizedType
//   Any quantized type. Any quantized type with no block float quantization is
//   ignored.
//
// - containerType
//   Original input or result type of the operation using the provided quantized
//   type. Used to ensure that the quantized type appears within a tensor and
//   that the tensor is compatible with block float quantization information.
//
LogicalResult verifyBlockFloatQuantization(Operation *op,
                                           QuantizedType quantizedType,
                                           Type containerType) {
  auto blockModeType = dyn_cast<BlockFloatQuantizedType>(quantizedType);
  if (!blockModeType)
    return success();

  auto shapedType = dyn_cast<ShapedType>(containerType);
  if (!shapedType)
    return op->emitError("scalar types may not use block float quantization");
  if (!shapedType.hasRank())
    return success();
  // We could also check that the tensor is a multiple of the block size, but
  // that requires that all padding is visible in MLIR
  if (blockModeType.getAxis() >= shapedType.getRank())
    return op->emitError("block axis must be less than tensor rank");
  return success();
}

// Common verification logic for 'quant.dcast' and 'quant.qcast' ops.
//
// - quantizedType
//   Quantized type used in the input ('quant.dcast') or result ('quant.qcast'),
//   whether as a primitive type or in a tensor.
//
// - floatType
//   Float type used in the input ('quant.qcast') or result ('quant.dcast'),
//   whether as a primitive type or in a tensor.
//
// - containerType
//   Type of original input or result.
//
LogicalResult verifyQuantizationOp(Operation *op, QuantizedType quantizedType,
                                   FloatType floatType, Type containerType) {
  if (!isa<BlockFloatQuantizedType>(quantizedType) &&
      quantizedType.getExpressedType() != floatType)
    return op->emitError(
        "expressed type in quantized type expected to match float type");

  if (failed(verifyPerAxisQuantization(op, quantizedType, containerType)))
    return failure();

  if (failed(verifyBlockFloatQuantization(op, quantizedType, containerType)))
    return failure();

  return success();
}

struct QuantInlinerInterface : public DialectInlinerInterface {
  using DialectInlinerInterface::DialectInlinerInterface;
  /// All quant dialect ops can be inlined.
  bool isLegalToInline(Operation *, Region *, bool, IRMapping &) const final {
    return true;
  }
};

} // namespace

//===----------------------------------------------------------------------===//
// Dialect
//===----------------------------------------------------------------------===//

void QuantDialect::initialize() {
  addTypes<AnyQuantizedType, BlockFloatQuantizedType, CalibratedQuantizedType,
           UniformQuantizedType, UniformQuantizedPerAxisType>();
  addOperations<
#define GET_OP_LIST
#include "mlir/Dialect/Quant/IR/QuantOps.cpp.inc"
      >();
  detail::addBytecodeInterface(this);
  addInterfaces<QuantInlinerInterface>();
}

//===----------------------------------------------------------------------===//
// DequantizeCastOp
//===----------------------------------------------------------------------===//

LogicalResult DequantizeCastOp::verify() {
  return verifyQuantizationOp(*this, getQuantizedType(), getFloatType(),
                              getInput().getType());
}

OpFoldResult DequantizeCastOp::fold(FoldAdaptor adaptor) {
  // Matches x -> quant.qcast -> quant.dcast -> y, replacing the quant.dcast op
  // with the value of x. Values x and y are guaranteed to be of the same type
  // in this pattern.
  auto srcQcastOp = getInput().getDefiningOp<QuantizeCastOp>();
  if (!srcQcastOp)
    return {};
  assert(srcQcastOp.getInput().getType() == getType());
  return srcQcastOp.getInput();
}

FloatType DequantizeCastOp::getFloatType() {
  return cast<FloatType>(getElementTypeOrSelf(getResult().getType()));
}

QuantizedType DequantizeCastOp::getQuantizedType() {
  return cast<QuantizedType>(getElementTypeOrSelf(getInput().getType()));
}

//===----------------------------------------------------------------------===//
// QuantizeCastOp
//===----------------------------------------------------------------------===//

LogicalResult QuantizeCastOp::verify() {
  return verifyQuantizationOp(*this, getQuantizedType(), getFloatType(),
                              getInput().getType());
}

OpFoldResult QuantizeCastOp::fold(FoldAdaptor adaptor) {
  // Matches x -> quant.dcast -> quant.qcast -> y, replacing the quant.qcast op
  // with the value of x if the casts invert each other. Contrary to the folding
  // pattern in quant.dcast (i.e., x -> quant.qcast -> quant.dcast -> y), values
  // x and y are not guaranteed to be of the same type here, as they may use
  // different quantization parameters.
  auto srcDcastOp = getInput().getDefiningOp<DequantizeCastOp>();
  if (!srcDcastOp || srcDcastOp.getInput().getType() != getType())
    return {};
  return srcDcastOp.getInput();
}

FloatType QuantizeCastOp::getFloatType() {
  return cast<FloatType>(getElementTypeOrSelf(getInput().getType()));
}

QuantizedType QuantizeCastOp::getQuantizedType() {
  return cast<QuantizedType>(getElementTypeOrSelf(getResult().getType()));
}

//===----------------------------------------------------------------------===//
// StorageCastOp
//===----------------------------------------------------------------------===//

LogicalResult StorageCastOp::verify() {
  auto quantizedType = getQuantizedType();
  if (isa<BlockFloatQuantizedType>(quantizedType))
    return getOperation()->emitError(
        "storage cast not supported for block float quantized types");
  auto integerType = getIntegerType();
  if (quantizedType.getStorageType() != integerType)
    return emitError(
        "storage type in quantized type expected to match integer type");

  // Verify integrity of per-axis quantization information, if available. While
  // the quantization type may appear in the input or the result, their tensor
  // shapes are guaranteed to be identical at this point.
  return verifyPerAxisQuantization(*this, quantizedType, getInput().getType());
}

OpFoldResult StorageCastOp::fold(FoldAdaptor adaptor) {
  // Matches x -> quant.scast -> quant.scast -> y, replacing the second
  // quant.scast with the value of x if the casts invert each other.
  auto srcScastOp = getInput().getDefiningOp<StorageCastOp>();
  if (!srcScastOp || srcScastOp.getInput().getType() != getType())
    return {};
  return srcScastOp.getInput();
}

IntegerType StorageCastOp::getIntegerType() {
  auto inputScalarType = getElementTypeOrSelf(getInput().getType());
  if (auto integerType = dyn_cast<IntegerType>(inputScalarType))
    return integerType;

  auto resultScalarType = getElementTypeOrSelf(getResult().getType());
  return cast<IntegerType>(resultScalarType);
}

QuantizedType StorageCastOp::getQuantizedType() {
  auto inputScalarType = getElementTypeOrSelf(getInput().getType());
  if (auto quantizedType = dyn_cast<QuantizedType>(inputScalarType))
    return quantizedType;

  auto resultScalarType = getElementTypeOrSelf(getResult().getType());
  return cast<QuantizedType>(resultScalarType);
}

} // namespace quant
} // namespace mlir

#define GET_OP_CLASSES
#include "mlir/Dialect/Quant/IR/QuantOps.cpp.inc"
