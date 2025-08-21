//===- TestAffineExpressionBounds.cpp - Test affine expression bounds --=====//
//----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "mlir/Analysis/AffineExprBounds.h"

#include "mlir/Dialect/DLTI/DLTI.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/BuiltinTypeInterfaces.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/FormatVariadic.h"

#include "TestDialect.h"

using namespace mlir;

namespace {

struct TestAffineExpressionBounds
    : public PassWrapper<TestAffineExpressionBounds,
                         OperationPass<func::FuncOp>> {
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(TestAffineExpressionBounds)

  StringRef getArgument() const final { return "test-affine-expr-bounds"; }
  StringRef getDescription() const final {
    return "Test simplify affine expression simplication";
  }

  FailureOr<SmallVector<std::optional<APInt>>>
  getBound(Operation *op, StringRef boundType, bool *resultSigned,
           uint64_t *resultWidth, bool optional = false) {
    SmallVector<std::optional<APInt>> result;

    bool isSigned = false;
    uint64_t width = 0;

    auto dict = op->getAttrDictionary();
    if (!dict) {
      return op->emitError("No dictionary found");
    }

    auto bounds = dict.getNamed(boundType);
    if (!bounds) {
      if (!optional) {
        return op->emitError(llvm::formatv("No {} attribute found", boundType));
      }
      return failure();
    }

    auto boundsValue = cast<ArrayAttr>(bounds->getValue());

    for (auto v : boundsValue) {
      if (auto value = dyn_cast<IntegerAttr>(v)) {
        if (width == 0) {
          isSigned = (value.getType().isSignedInteger() ||
                      value.getType().isSignlessInteger());
          width = value.getType().getIntOrFloatBitWidth();
        } else if (isSigned != (value.getType().isSignedInteger() ||
                                value.getType().isSignlessInteger())) {
          return op->emitError("Mixed signedness in bounds");
        } else if (width != value.getType().getIntOrFloatBitWidth()) {
          return op->emitError("Mixed width in bounds");
        }
        result.push_back(value.getValue());
      } else if (auto value = dyn_cast<StringAttr>(v)) {
        if (value.getValue() == "?") {
          result.push_back(std::nullopt);
        } else {
          return op->emitError("Unknown string value found");
        }
      } else {
        return op->emitError("Non-integer or string value found in bounds");
      }
    }

    *resultSigned = isSigned;
    *resultWidth = width;

    return result;
  }

  FailureOr<AffineExpr> getAffineExpr(Operation *op) {
    auto dict = op->getAttrDictionary();
    if (!dict) {
      return op->emitError("No dictionary found");
    }
    auto affineMap = dict.getNamed("affine_map");
    if (!affineMap) {
      return op->emitError("No affine_map attribute found");
    }
    auto mapAttr = dyn_cast<AffineMapAttr>(affineMap->getValue());
    if (!mapAttr) {
      return op->emitError("Invalid affine_map attribute found");
    }

    auto map = mapAttr.getAffineMap();
    if (map.getNumResults() != 1) {
      return op->emitError("Invalid number of affine_map results");
    }

    return map.getResult(0);
  }

  void runOnOperation() override {
    func::FuncOp func = getOperation();
    IRRewriter rewriter(func.getContext());

    func.walk([&](Operation *op) {
      if (op->getDialect() !=
          op->getContext()->getLoadedDialect<test::TestDialect>()) {
        return;
      }

      auto expr = getAffineExpr(op);
      bool ubSigned, lbSigned;
      uint64_t ubWidth, lbWidth;
      auto ubs = getBound(op, "ubs", &ubSigned, &ubWidth);
      auto lbs = getBound(op, "lbs", &lbSigned, &lbWidth);

      if (failed(expr) || failed(ubs) || failed(lbs)) {
        return;
      }

      if (ubs->size() != lbs->size()) {
        op->emitError("Mismatched number of bounds");
        return;
      }
      if (ubWidth != lbWidth &&
          !((ubWidth == 0 && lbWidth > 0) || (ubWidth > 0 && lbWidth == 0))) {
        op->emitError("Mismatched width in bounds");
        return;
      }
      bool signCheck =
          !(ubWidth == 0 && lbWidth > 0) && !(ubWidth > 0 && lbWidth == 0);
      if (signCheck && (ubSigned != lbSigned)) {
        op->emitError("Mixed signedness in bounds");
        return;
      }

      uint64_t width = (ubWidth == 0) ? lbWidth : ubWidth;

      AffineExprBoundsVisitor visitor(*lbs, *ubs, lbSigned, width,
                                      &getContext());
      auto exprLB = visitor.getLowerBound(*expr);
      auto exprUB = visitor.getUpperBound(*expr);

      if (!exprLB || !exprUB) {
        op->emitError("Failed to compute bounds");
        return;
      }

      auto namedAttrList = mlir::NamedAttrList{rewriter.getDictionaryAttr(
          {rewriter.getNamedAttr(
               "expr_lb",
               IntegerAttr::get(
                   IntegerType::get(
                       &getContext(), width,
                       (lbSigned) ? IntegerType::SignednessSemantics::Signless
                                  : IntegerType::SignednessSemantics::Unsigned),
                   *exprLB)),
           rewriter.getNamedAttr(
               "expr_ub",
               IntegerAttr::get(
                   IntegerType::get(
                       &getContext(), width,
                       (ubSigned) ? IntegerType::SignednessSemantics::Signless
                                  : IntegerType::SignednessSemantics::Unsigned),
                   *exprUB))})};
      op->setAttrs(namedAttrList);
    });
  }
};
} // namespace

namespace mlir {
namespace test {
void registerTestAffineExpressionBounds() {
  PassRegistration<TestAffineExpressionBounds>();
}
} // namespace test
} // namespace mlir
