//===- AffineExprBounds.h - Compute bounds of affine expressions *- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements an analysis of affine expressions to compute their
// ranges (lower/upper bounds) in a given context.
//
//===----------------------------------------------------------------------===//
#include "mlir/Analysis/AffineExprBounds.h"

#include "mlir/IR/AffineExpr.h"
#include "mlir/IR/AffineExprVisitor.h"
#include "mlir/IR/AffineMap.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/Interfaces/InferIntRangeInterface.h"
#include "mlir/Interfaces/Utils/InferIntRangeCommon.h"
#include "llvm/ADT/APInt.h"

#include <cstdint>

using namespace mlir;

AffineExprBoundsVisitor::AffineExprBoundsVisitor(
    ArrayRef<std::optional<APInt>> constLowerBounds,
    ArrayRef<std::optional<APInt>> constUpperBounds, bool boundsSigned,
    uint64_t bitWidth, MLIRContext *context)
    : boundsSigned(boundsSigned), bitWidth(bitWidth) {
  assert(constLowerBounds.size() == constUpperBounds.size());
  for (unsigned i = 0; i < constLowerBounds.size(); i++) {
    if (constLowerBounds[i].has_value()) {
      lb[getAffineDimExpr(i, context)] = constLowerBounds[i].value();
    }
    if (constUpperBounds[i].has_value()) {
      ub[getAffineDimExpr(i, context)] = constUpperBounds[i].value();
    }
  }
}

AffineExprBoundsVisitor::AffineExprBoundsVisitor(
    ArrayRef<std::optional<int64_t>> constLowerBounds,
    ArrayRef<std::optional<int64_t>> constUpperBounds, MLIRContext *context)
    : boundsSigned(true), bitWidth(64) {
  assert(constLowerBounds.size() == constUpperBounds.size());
  // Convert int64_ts to APInts.
  for (unsigned i = 0; i < constLowerBounds.size(); i++) {
    if (constLowerBounds[i].has_value()) {
      lb[getAffineDimExpr(i, context)] =
          APInt(64, constLowerBounds[i].value(), /*isSigned=*/true);
    }
    if (constUpperBounds[i].has_value()) {
      ub[getAffineDimExpr(i, context)] =
          APInt(64, constUpperBounds[i].value(), /*isSigned=*/true);
    }
  }
}

std::optional<APInt> AffineExprBoundsVisitor::getUpperBound(AffineExpr expr) {
  // Use memoized bound if available.
  auto i = ub.find(expr);
  if (i != ub.end()) {
    return i->second;
  }
  // Compute the bound otherwise.
  if (failed(walkPostOrder(expr))) {
    return std::nullopt;
  }
  return ub[expr];
}

std::optional<APInt> AffineExprBoundsVisitor::getLowerBound(AffineExpr expr) {
  // Use memoized bound if available.
  auto i = lb.find(expr);
  if (i != lb.end()) {
    return i->second;
  }
  // Compute the bound otherwise.
  if (failed(walkPostOrder(expr))) {
    return std::nullopt;
  }
  return lb[expr];
}

std::optional<int64_t>
AffineExprBoundsVisitor::getIndexUpperBound(AffineExpr expr) {
  std::optional<APInt> apIntResult = getUpperBound(expr);
  if (!apIntResult)
    return std::nullopt;

  return apIntResult->getSExtValue();
}

std::optional<int64_t>
AffineExprBoundsVisitor::getIndexLowerBound(AffineExpr expr) {
  std::optional<APInt> apIntResult = getLowerBound(expr);
  if (!apIntResult)
    return std::nullopt;

  return apIntResult->getSExtValue();
}

ConstantIntRanges getRange(APInt lb, APInt ub, bool boundsSigned) {
  return ConstantIntRanges::range(lb, ub, boundsSigned);
}

/// Wrapper around the intrange::infer* functions that infers the range of
/// binary operations on two ranges.
void AffineExprBoundsVisitor::inferBinOpRange(
    AffineBinaryOpExpr expr,
    const std::function<ConstantIntRanges(ArrayRef<ConstantIntRanges>)>
        &opInference) {
  ConstantIntRanges lhsRange =
      getRange(lb[expr.getLHS()], ub[expr.getLHS()], boundsSigned);
  ConstantIntRanges rhsRange =
      getRange(lb[expr.getRHS()], ub[expr.getRHS()], boundsSigned);
  ConstantIntRanges result = opInference({lhsRange, rhsRange});

  lb[expr] = (boundsSigned) ? result.smin() : result.umin();
  ub[expr] = (boundsSigned) ? result.smax() : result.umax();
}

// Visitor method overrides.
LogicalResult AffineExprBoundsVisitor::visitMulExpr(AffineBinaryOpExpr expr) {
  inferBinOpRange(expr, [](ArrayRef<ConstantIntRanges> ranges) {
    return intrange::inferMul(ranges);
  });
  return success();
}
LogicalResult AffineExprBoundsVisitor::visitAddExpr(AffineBinaryOpExpr expr) {
  inferBinOpRange(expr, [](ArrayRef<ConstantIntRanges> ranges) {
    return intrange::inferAdd(ranges);
  });
  return success();
}
LogicalResult
AffineExprBoundsVisitor::visitCeilDivExpr(AffineBinaryOpExpr expr) {
  inferBinOpRange(
      expr, [boundsSigned = boundsSigned](ArrayRef<ConstantIntRanges> ranges) {
        if (boundsSigned) {
          return intrange::inferCeilDivS(ranges);
        }
        return intrange::inferCeilDivU(ranges);
      });
  return success();
}
LogicalResult
AffineExprBoundsVisitor::visitFloorDivExpr(AffineBinaryOpExpr expr) {
  // There is no inferFloorDivU in the intrange library. We only offer
  // computation of bounds for signed floordiv operations.
  if (boundsSigned) {
    inferBinOpRange(expr, [](ArrayRef<ConstantIntRanges> ranges) {
      return intrange::inferFloorDivS(ranges);
    });
    return success();
  }
  return failure();
}
LogicalResult AffineExprBoundsVisitor::visitModExpr(AffineBinaryOpExpr expr) {
  // Only support integers >= 1 as RHS.
  auto rhsConst = dyn_cast<AffineConstantExpr>(expr.getRHS());
  if (!rhsConst || rhsConst.getValue() < 1)
    return failure();

  inferBinOpRange(expr, [boundsSigned =
                             boundsSigned](ArrayRef<ConstantIntRanges> ranges) {
    // Mod must return a value between 0 and N-1.
    // Computing (N + (expr mod N)) mod N is guaranteed to yield a result in
    // this range.
    if (boundsSigned) {
      auto rhs = ranges[1];
      auto lhs = ranges[0];
      return intrange::inferRemS(
          {intrange::inferAdd({intrange::inferRemS({lhs, rhs}), rhs}), rhs});
    }
    return intrange::inferRemU(ranges);
  });
  return success();
}
LogicalResult AffineExprBoundsVisitor::visitDimExpr(AffineDimExpr expr) {
  if (lb.find(expr) == lb.end() || ub.find(expr) == ub.end()) {
    return failure();
  }
  return success();
}
LogicalResult AffineExprBoundsVisitor::visitSymbolExpr(AffineSymbolExpr expr) {
  return failure();
}
LogicalResult
AffineExprBoundsVisitor::visitConstantExpr(AffineConstantExpr expr) {
  APInt apIntVal =
      APInt(bitWidth, static_cast<uint64_t>(expr.getValue()), boundsSigned);
  lb[expr] = apIntVal;
  ub[expr] = apIntVal;
  return success();
}
