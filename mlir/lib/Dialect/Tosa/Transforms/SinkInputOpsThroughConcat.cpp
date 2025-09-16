//===- SinkInputOpsThroughConcat.cpp
//-------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// ----------
// Motivation:
// ----------

// Sometimes, models contain the same operation multiple times with an immediate
// concatenation of the results afterwards. In some cases, it's possible to sink
// these operations through the concat to enable better optimizations
// afterwards.

// -------------------
// High-Level Overview:
// -------------------
//
// Replacing            with
// - Op -\              -\
// - OP ---> Concat ==> ---> Concat -> Op
// - Op -/              -/
//

// -----------
// Overall design:
// -----------
//
// The pass uses an allowlist of operations that are known to be sinkable.
// Additionally, it consists of mainly three parts/pattern matchers:
// 1. A generic matcher (SinkGenericOp): It doesn't do anything except
//    outputting statistics.
// 2. A more specific matcher that can be specialized (SinkSpecificOp): It's
//    implemented as template and accepts extra operation specific checks but
//    always perfoms the same transformation.
// 3. Operation specific matcher with an operation specific transformation
//    (currently just for reshape).
//===----------------------------------------------------------------------===//
#include "mlir/Dialect/Tosa/Transforms/Passes.h"

#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Tosa/IR/TosaOps.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/LogicalResult.h"
#include "llvm/Support/raw_ostream.h"

#include <variant>
namespace mlir {
namespace tosa {
#define GEN_PASS_DEF_SINKINPUTOPSTHROUGHCONCAT
#include "mlir/Dialect/Tosa/Transforms/Passes.h.inc"
} // namespace tosa
} // namespace mlir

using namespace mlir;
using namespace mlir::tosa;

//===----------------------------------------------------------------------===//
// TOSA Sink input Ops through Concat Pass
//===----------------------------------------------------------------------===//

namespace {
struct SinkGenericOp : public OpRewritePattern<tosa::ConcatOp> {
  SinkGenericOp(MLIRContext *context, PatternBenefit benefit,
                llvm::StringMap<unsigned> &operationFrequency,
                llvm::raw_ostream &os)
      : OpRewritePattern<tosa::ConcatOp>::OpRewritePattern(context, benefit),
        operationFrequency(operationFrequency), os(os) {}

  LogicalResult matchAndRewrite(tosa::ConcatOp concatOp,
                                PatternRewriter &rewriter) const override {

    Operation *sample = nullptr;
    for (auto val : concatOp->getOperands()) {
      if (!val.hasOneUse())
        return rewriter.notifyMatchFailure(
            concatOp, "Operands must just connect to this concat.");

      auto *genericOp = val.getDefiningOp();
      if (!genericOp)
        return rewriter.notifyMatchFailure(
            concatOp, "Requires all operands to be operators");

      if (!sample)
        sample = genericOp;

      if (sample->getName().getIdentifier() !=
          genericOp->getName().getIdentifier())
        return rewriter.notifyMatchFailure(
            concatOp, "Requires all operands to be the same");
    }
    auto opName = sample->getName().getStringRef();
    auto amount = ++operationFrequency[opName];
    if (amount == 1 || amount % 50 == 0)
      os << "SinkInputOpsThroughConcat: Operation " << opName
         << " -> Matched amount: " << amount << "\n";
    return rewriter.notifyMatchFailure(concatOp,
                                       "Only for statistics printing");
  }

private:
  llvm::StringMap<unsigned> &operationFrequency;
  llvm::raw_ostream &os;
};

template <class OpT>
struct SinkSpecificOp : public OpRewritePattern<tosa::ConcatOp> {
  using OpRewritePattern<tosa::ConcatOp>::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::ConcatOp concatOp,
                                PatternRewriter &rewriter) const override {
    auto sampleOrError = doGenericChecks(concatOp, rewriter);
    if (failed(sampleOrError))
      return sampleOrError;
    OpT sample = *sampleOrError;

    const auto extraChecks = doExtraChecks(sample, concatOp, rewriter);
    if (!extraChecks.succeeded())
      return extraChecks;

    // rewriting, first collect all inputs and rearrange them for the concats
    // to visualize the process
    //
    // A -.
    // B -- (pOp1) --.
    // C -*          |- (concatOp)
    // D -.          |
    // E -- (pOp2) --*
    // F -*
    //
    // The for loops traverse it in this order: ((A, B, C), (D, E, F))
    // It is assigned in reverse order in the vectors:
    // [[A, D], [B, E], [C, F]]
    SmallVector<SmallVector<Value>> concatOperands(sample->getNumOperands());
    for (auto val : concatOp.getOperands()) {
      auto *producerOp = val.getDefiningOp();
      assert(producerOp != nullptr &&
             "Previous check about null already happened.");
      for (unsigned j = 0; j < producerOp->getNumOperands(); ++j) {
        concatOperands[j].emplace_back(producerOp->getOperand(j));
      }
    }

    // then create the concats and replacement op
    SmallVector<Value> concatOps;
    for (auto ops : concatOperands) {
      auto concatOpReplacement = rewriter.create<tosa::ConcatOp>(
          concatOp.getLoc(), ops, concatOp.getAxis());
      concatOps.emplace_back(concatOpReplacement);
    }
    rewriter.replaceOpWithNewOp<OpT>(concatOp, concatOp->getResultTypes(),
                                     concatOps, sample->getAttrs());
    return success();
  }

protected:
  llvm::FailureOr<OpT> doGenericChecks(tosa::ConcatOp concatOp,
                                       PatternRewriter &rewriter) const {
    OpT sample = nullptr;

    for (auto val : concatOp->getOperands()) {
      if (!val.hasOneUse())
        return rewriter.notifyMatchFailure(
            concatOp, "Operands must just connect to this concat.");

      auto op = val.getDefiningOp<OpT>();
      if (!op)
        return rewriter.notifyMatchFailure(
            concatOp, Twine("Operand is not a ") + OpT::getOperationName());

      if (!sample)
        sample = op;

      if (!llvm::equal(op->getOperandTypes(), sample->getOperandTypes()))
        return rewriter.notifyMatchFailure(
            concatOp, "Requires all operand types to be the same");

      if (llvm::any_of(OpT::getAttributeNames(), [&](const auto &name) {
            return sample->getAttr(name) != op->getAttr(name);
          }))
        return rewriter.notifyMatchFailure(
            concatOp, "Requires all operand attributes to be the same");
    }

    if (sample->getNumOperands() == 0) {
      return rewriter.notifyMatchFailure(
          concatOp, "Requires all operands to have one or more inputs");
    }

    return sample;
  }

  virtual LogicalResult doExtraChecks(OpT, tosa::ConcatOp,
                                      PatternRewriter &) const {
    return success();
  }
};

template <class OpT>
struct SinkElementwiseBroadcastableOp : public SinkSpecificOp<OpT> {
  using SinkSpecificOp<OpT>::SinkSpecificOp;

protected:
  // check that the broadcast happens on another axis
  LogicalResult doExtraChecks(OpT op, tosa::ConcatOp concat,
                              PatternRewriter &rewriter) const override {
    SmallVector<ArrayRef<int64_t>> shapes;
    for (auto ty : op->getOperandTypes()) {
      // lifetime bound to underlying ShapedType object
      auto tenType = dyn_cast<ShapedType>(ty);
      if (tenType && tenType.hasStaticShape()) {
        shapes.emplace_back(tenType.getShape());
      } else {
        return rewriter.notifyMatchFailure(
            concat, "Check for broadcast on an unshaped or not static type.");
      }
    }
    assert(shapes.size() == op->getNumOperands() &&
           "Something went wrong with the above loop.");

    // check that the ranks on the axis that the concat uses is not one for all
    // ops or one for all ops
    const auto axis = concat.getAxis();
    const size_t oneDimensions =
        llvm::count_if(shapes, [&](const auto &s) { return s[axis] == 1; });
    if (oneDimensions != shapes.size() && oneDimensions != 0) {
      return rewriter.notifyMatchFailure(
          concat, "Operand broadcasts on same axis then concat.");
    }
    return success();
  }
};

template <class OpT>
struct SinkReduceOp : public SinkSpecificOp<OpT> {
  using SinkSpecificOp<OpT>::SinkSpecificOp;

protected:
  // check that the concat happens on another axis than this one
  LogicalResult doExtraChecks(OpT op, tosa::ConcatOp concat,
                              PatternRewriter &rewriter) const override {
    if (op.getAxis() == concat.getAxis())
      return rewriter.notifyMatchFailure(
          concat, "Operator must not be on the same axis than concat.");
    return success();
  }
};

struct SinkMatmulOp : public SinkSpecificOp<tosa::MatMulOp> {
  using SinkSpecificOp<tosa::MatMulOp>::SinkSpecificOp;

protected:
  // check that the concat happens on another axis than the one used by matmul
  LogicalResult doExtraChecks(tosa::MatMulOp op, tosa::ConcatOp concat,
                              PatternRewriter &rewriter) const override {
    if (concat.getAxis() != 0)
      return rewriter.notifyMatchFailure(
          concat, "Matmul concat about different axis not yet supported");
    return success();
  }
};

struct SinkReshapeOp : public SinkSpecificOp<tosa::ReshapeOp> {
  using SinkSpecificOp<tosa::ReshapeOp>::SinkSpecificOp;

  LogicalResult matchAndRewrite(tosa::ConcatOp concatOp,
                                PatternRewriter &rewriter) const override {
    auto reshapeOrError = doGenericChecks(concatOp, rewriter);
    if (failed(reshapeOrError))
      return reshapeOrError;
    auto reshape = *reshapeOrError;

    const auto tenType = reshape.getInput1().getType();
    if (!tenType || !tenType.hasStaticShape())
      return rewriter.notifyMatchFailure(
          concatOp, "Dynamic shapes for reshapes are not supported.");
    const ArrayRef<int64_t> shapeBeforeReshape = tenType.getShape();
    const ArrayRef<int64_t> shapeAfterReshape = reshape.getNewShape();
    if (shapeBeforeReshape.size() == 0)
      return rewriter.notifyMatchFailure(
          concatOp,
          "Tensors of rank 0 cannot have an independent concat axis.");

    // Approach: Before rewrite, we have a reshape followed by a concat.
    // This concat concatenates on the concatAxisAfterReshape. For switching, we
    // need to calculate a new shape and a new concat axis
    // (concatAxisBeforeReshape). For that, we check the product of the reshape
    // dimensions before the concatAxisAfterReshape and match that with the
    // product of the dimensions of the shapeBeforeReshape.
    //
    // Example:
    // 6x1x6 --(reshape)--> 2x3x1x2x3 --(concat)--> 2x3x2x2x3
    // concatAxisAfterReshape is 2, after rewrite it would be 1:
    // 6x1x6 --(concat)--> 6x2x6 --(reshape)--> 2x3x2x2x3
    //
    // The axis is not always unique:
    // 1x1x6 --(reshape)--> 1x1x1x6 --(concat)--> 2x1x1x6
    // concatAxisAfterReshape is 0, after rewrite it can be 0 or 1:
    // 1x1x6 --(concat)--> 2x1x6 --(reshape)--> 2x1x1x6
    // 1x1x6 --(concat)--> 1x2x6 --(reshape)--> 2x1x1x6
    //
    // We also need to take the concat dimension into account:
    // 1x4x1 --(reshape)--> 1x4 --(concat)--> 1x8
    // concatAxisAfterReshape is 1, after rewrite it would be 1 as well:
    // 1x4x1 --(concat)--> 1x8x1 --(reshape)--> 1x8
    const uint32_t concatAxisAfterReshape = concatOp.getAxis();
    int64_t prefixProductAfterReshape = 1;
    // also count the concat dimension itself
    for (size_t i = 0; i <= concatAxisAfterReshape; ++i) {
      prefixProductAfterReshape *= shapeAfterReshape[i];
    }

    int64_t prefixProductBeforeReshape = 1;
    std::optional<size_t> concatAxisBeforeReshape = std::nullopt;
    long sizeOfConcatDim = shapeAfterReshape[concatAxisAfterReshape];
    for (size_t i = 0; i < shapeBeforeReshape.size(); ++i) {
      prefixProductBeforeReshape *= shapeBeforeReshape[i];
      if (prefixProductBeforeReshape == prefixProductAfterReshape &&
          shapeBeforeReshape[i] == sizeOfConcatDim) {
        concatAxisBeforeReshape = i;
        break;
      }
    }

    if (!concatAxisBeforeReshape)
      return rewriter.notifyMatchFailure(
          concatOp, "Sinking reshape not possible. No compatible dimension for "
                    "concat axis found.");

    SmallVector<Value> concatOperands;
    for (auto val : concatOp.getOperands()) {
      auto *producerOp = val.getDefiningOp();
      assert(producerOp != nullptr &&
             "Previous check about null already happened.");
      for (auto val : producerOp->getOperands()) {
        concatOperands.emplace_back(val);
      }
    }
    auto concatNew = rewriter.create<tosa::ConcatOp>(
        concatOp.getLoc(), concatOperands, *concatAxisBeforeReshape);
    // calculate new shape for reshape by combining the shape of the concat with
    // the remaining prefix of the old reshape
    Type concatType = concatNew.getType();
    auto concatShapeT = cast<ShapedType>(concatType);
    assert(concatShapeT.hasStaticShape() &&
           "op and thus concat must be static");
    auto concatShape = concatShapeT.getShape();

    SmallVector<int64_t> reshapeShape(shapeAfterReshape);
    reshapeShape[concatAxisAfterReshape] =
        concatShape[*concatAxisBeforeReshape];

    auto reshapeNew = rewriter.create<tosa::ReshapeOp>(reshape.getLoc(),
                                                       concatNew, reshapeShape);
    rewriter.replaceOp(concatOp, reshapeNew);
    return success();
  }
};

struct SinkInputOpsThroughConcat
    : public tosa::impl::SinkInputOpsThroughConcatBase<
          SinkInputOpsThroughConcat> {

  SinkInputOpsThroughConcat() = default;
  SinkInputOpsThroughConcat(llvm::raw_ostream &os) : os(os) {};

  void runOnOperation() override {
    auto func = getOperation();
    RewritePatternSet patterns(func.getContext());
    MLIRContext *ctx = func.getContext();

    populateSinkInputOpsThroughConcatPattern(patterns, ctx);

    if (applyPatternsGreedily(func, std::move(patterns)).failed())
      signalPassFailure();
  }

private:
  void populateSinkInputOpsThroughConcatPattern(RewritePatternSet &patterns,
                                                MLIRContext *ctx) {
    // elementwise
    patterns
        .add<SinkSpecificOp<tosa::AbsOp>, SinkSpecificOp<tosa::BitwiseNotOp>,
             SinkSpecificOp<tosa::CeilOp>, SinkSpecificOp<tosa::ClampOp>,
             SinkSpecificOp<tosa::ClzOp>, SinkSpecificOp<tosa::CosOp>,
             SinkSpecificOp<tosa::ErfOp>, SinkSpecificOp<tosa::ExpOp>,
             SinkSpecificOp<tosa::FloorOp>, SinkSpecificOp<tosa::LogicalNotOp>,
             SinkSpecificOp<tosa::NegateOp>, SinkSpecificOp<tosa::ReciprocalOp>,
             SinkSpecificOp<tosa::RsqrtOp>, SinkSpecificOp<tosa::SigmoidOp>,
             SinkSpecificOp<tosa::SinOp>, SinkSpecificOp<tosa::TanhOp>>(
            ctx, /*benefit=*/2);
    patterns.add<SinkElementwiseBroadcastableOp<tosa::AddOp>,
                 SinkElementwiseBroadcastableOp<tosa::ArithmeticRightShiftOp>,
                 SinkElementwiseBroadcastableOp<tosa::BitwiseAndOp>,
                 SinkElementwiseBroadcastableOp<tosa::BitwiseOrOp>,
                 SinkElementwiseBroadcastableOp<tosa::BitwiseXorOp>,
                 SinkElementwiseBroadcastableOp<tosa::EqualOp>,
                 SinkElementwiseBroadcastableOp<tosa::GreaterOp>,
                 SinkElementwiseBroadcastableOp<tosa::GreaterEqualOp>,
                 SinkElementwiseBroadcastableOp<tosa::IntDivOp>,
                 SinkElementwiseBroadcastableOp<tosa::LogOp>,
                 SinkElementwiseBroadcastableOp<tosa::LogicalAndOp>,
                 SinkElementwiseBroadcastableOp<tosa::LogicalLeftShiftOp>,
                 SinkElementwiseBroadcastableOp<tosa::LogicalOrOp>,
                 SinkElementwiseBroadcastableOp<tosa::LogicalRightShiftOp>,
                 SinkElementwiseBroadcastableOp<tosa::LogicalXorOp>,
                 SinkElementwiseBroadcastableOp<tosa::MaximumOp>,
                 SinkElementwiseBroadcastableOp<tosa::MinimumOp>,
                 SinkElementwiseBroadcastableOp<tosa::PowOp>,
                 SinkElementwiseBroadcastableOp<tosa::SelectOp>,
                 SinkElementwiseBroadcastableOp<tosa::SubOp>>(ctx,
                                                              /*benefit=*/2);
    // reduce
    patterns
        .add<SinkReduceOp<tosa::ReduceAllOp>, SinkReduceOp<tosa::ReduceAnyOp>,
             SinkReduceOp<tosa::ReduceMaxOp>, SinkReduceOp<tosa::ReduceMinOp>,
             SinkReduceOp<tosa::ReduceProdOp>, SinkReduceOp<tosa::ReduceSumOp>>(
            ctx, /*benefit=*/2);
    // others
    patterns.add<SinkMatmulOp>(ctx, /*benefit=*/2);
    patterns.add<SinkReshapeOp>(ctx, /*benefit=*/2);
    patterns.add<SinkGenericOp>(ctx, /*benefit=*/1, operationFrequency, os);
  }

  llvm::StringMap<unsigned> operationFrequency;
  llvm::raw_ostream &os = llvm::errs();
};
} // namespace

std::unique_ptr<Pass>
mlir::tosa::createSinkInputOpsThroughConcatPass(llvm::raw_ostream &os) {
  return std::make_unique<SinkInputOpsThroughConcat>(os);
}

std::unique_ptr<Pass> mlir::tosa::createSinkInputOpsThroughConcatPass() {
  return std::make_unique<SinkInputOpsThroughConcat>();
}
