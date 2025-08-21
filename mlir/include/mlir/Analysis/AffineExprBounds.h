//===- AffineExprBounds.h - Compute bounds of affine expressions *- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This header file defines an analysis of affine expressions to compute their
// ranges (lower/upper bounds) in a given context.
//
//===----------------------------------------------------------------------===//
#ifndef MLIR_ANALYSIS_AFFINEEXPRBOUNDS_H
#define MLIR_ANALYSIS_AFFINEEXPRBOUNDS_H

#include "mlir/IR/AffineExprVisitor.h"
#include "mlir/IR/Attributes.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/Interfaces/InferIntRangeInterface.h"

#include "mlir/IR/AffineExpr.h"
#include "mlir/IR/AffineMap.h"
#include "mlir/Support/LogicalResult.h"

using namespace mlir;

/// This visitor computes the bounds of affine expressions, using as context the
/// bounds of the dimensions of the expression.
///
/// Example:
/// Given bounds 0 <= d0 <= 99 and 0 <= d1 <= 199, we can compute the bounds
/// of the following expression:
/// lb(2 * d0 + 3 * d1) = 0
/// ub(2 * d0 + 3 * d1) = 795
///
///  * The bounds given in the context are inclusive, and the bounds returned
///  are also inclusive.
///  * If bounds are not available for a dimension, std::nullopt can be used
///  instead. The bounds of an expression that involves it will be std::nullopt.
///  * Limitations:
///    - Parametric expressions (using symbols) are not supported.
///    - Unsigned FloorDiv is currently not supported.
class AffineExprBoundsVisitor
    : public AffineExprVisitor<AffineExprBoundsVisitor, LogicalResult> {
public:
  /// Initialize the context (bounds) with APInt. All bounds must have the same
  /// signedness and bit width.
  AffineExprBoundsVisitor(ArrayRef<std::optional<APInt>> constLowerBounds,
                          ArrayRef<std::optional<APInt>> constUpperBounds,
                          bool boundsSigned, uint64_t bitWidth,
                          MLIRContext *context);

  /// Initialize the context (bounds) with 64-bit signed integers. This allows
  /// to directly map index-type values such as Linalg op bounds, which are
  /// represented as int64_t.
  AffineExprBoundsVisitor(ArrayRef<std::optional<int64_t>> constLowerBounds,
                          ArrayRef<std::optional<int64_t>> constUpperBounds,
                          MLIRContext *context);

  /// Get the upper bound of \p expr using the context bounds.
  std::optional<APInt> getUpperBound(AffineExpr expr);
  std::optional<int64_t> getIndexUpperBound(AffineExpr expr);

  /// Get the lower bound of \p expr using the context bounds.
  std::optional<APInt> getLowerBound(AffineExpr expr);
  std::optional<int64_t> getIndexLowerBound(AffineExpr expr);

  // These methods are directly called by the AffineExprVisitor base class.
  LogicalResult visitMulExpr(AffineBinaryOpExpr expr);
  LogicalResult visitAddExpr(AffineBinaryOpExpr expr);
  LogicalResult visitDimExpr(AffineDimExpr expr);
  LogicalResult visitSymbolExpr(AffineSymbolExpr expr);
  LogicalResult visitConstantExpr(AffineConstantExpr expr);
  LogicalResult visitCeilDivExpr(AffineBinaryOpExpr expr);
  LogicalResult visitFloorDivExpr(AffineBinaryOpExpr expr);
  LogicalResult visitModExpr(AffineBinaryOpExpr expr);

private:
  bool boundsSigned;
  uint64_t bitWidth;
  void inferBinOpRange(
      AffineBinaryOpExpr expr,
      const std::function<ConstantIntRanges(ArrayRef<ConstantIntRanges>)>
          &opInference);

  /// Bounds that have been computed for subexpressions are memoized and reused.
  llvm::DenseMap<AffineExpr, APInt> lb;
  llvm::DenseMap<AffineExpr, APInt> ub;
};

#endif // MLIR_ANALYSIS_AFFINEEXPRBOUNDS_H
