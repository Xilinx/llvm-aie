//===- UBToEmitC.cpp - UB to EmitC dialect conversion ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "mlir/Conversion/UBToEmitC/UBToEmitC.h"

#include "mlir/Dialect/EmitC/IR/EmitC.h"
#include "mlir/Dialect/EmitC/Transforms/TypeConversions.h"
#include "mlir/Dialect/UB/IR/UBOps.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/TypeUtilities.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"

namespace mlir {
#define GEN_PASS_DEF_CONVERTUBTOEMITC
#include "mlir/Conversion/Passes.h.inc"
} // namespace mlir

using namespace mlir;

namespace {
struct PoisonOpLowering : public OpConversionPattern<ub::PoisonOp> {
  bool noInitialization;

public:
  PoisonOpLowering(const TypeConverter &converter, MLIRContext *context,
                   bool noInitialization)
      : OpConversionPattern<ub::PoisonOp>(converter, context),
        noInitialization(noInitialization) {}

  LogicalResult
  matchAndRewrite(ub::PoisonOp op, OpAdaptor adaptor,
                  ConversionPatternRewriter &rewriter) const override {
    const TypeConverter *converter = getTypeConverter();
    Type convertedType = converter->convertType(op.getType());

    if (!convertedType)
      return rewriter.notifyMatchFailure(op.getLoc(), "type conversion failed");

    if (!(emitc::isIntegerIndexOrOpaqueType(convertedType) ||
          emitc::isSupportedFloatType(convertedType))) {
      return rewriter.notifyMatchFailure(
          op.getLoc(), "only scalar poison values can be lowered");
    }

    Attribute value;
    if (noInitialization) {
      value = emitc::OpaqueAttr::get(op->getContext(), "");
      auto var = rewriter.create<emitc::VariableOp>(
          op.getLoc(), emitc::LValueType::get(convertedType), value);
      rewriter.replaceOpWithNewOp<emitc::LoadOp>(op, convertedType, var);
      return success();
    }

    // Any constant will be fine to lower a poison op
    if (emitc::isIntegerIndexOrOpaqueType(convertedType)) {
      if (auto opaqueType = dyn_cast<emitc::OpaqueType>(convertedType)) {
        // Use brace-initialization for opaque types; there is no universally
        // valid constant we can use for opaque types otherwise. Generated EmitC
        // will be C++.
        value = emitc::OpaqueAttr::get(op->getContext(), "{}");
      } else {
        value = IntegerAttr::get((emitc::isPointerWideType(convertedType))
                                     ? IndexType::get(op.getContext())
                                     : convertedType,
                                 42);
      }
    } else if (emitc::isSupportedFloatType(convertedType)) {
      value = FloatAttr::get(convertedType, 42.0f);
    }
    rewriter.replaceOpWithNewOp<emitc::ConstantOp>(op, convertedType, value);

    return success();
  }
};
} // namespace

void ub::populateUBToEmitCConversionPatterns(TypeConverter &converter,
                                             RewritePatternSet &patterns,
                                             bool noInitialization) {
  MLIRContext *ctx = patterns.getContext();
  patterns.add<PoisonOpLowering>(converter, ctx, noInitialization);
}

struct ConvertUBToEmitC : public impl::ConvertUBToEmitCBase<ConvertUBToEmitC> {
  using Base::Base;

  void runOnOperation() override {
    RewritePatternSet patterns(&getContext());
    TypeConverter converter;
    converter.addConversion([](Type t) { return t; });
    populateEmitCSizeTTypeConversions(converter);

    ConversionTarget target(getContext());
    target.addLegalDialect<emitc::EmitCDialect>();
    target.addIllegalDialect<ub::UBDialect>();

    mlir::ub::populateUBToEmitCConversionPatterns(converter, patterns,
                                                  noInitialization);

    if (failed(applyPartialConversion(getOperation(), target,
                                      std::move(patterns))))
      signalPassFailure();
  }
};
