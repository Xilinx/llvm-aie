//===- SinkInputOpsThroughConcat.cpp
//-------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
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

struct ReshapeOperandInfo {
  tosa::ReshapeOp reshape;
  ShapedType inputType;
  SmallVector<size_t> candidateAxes;
};

FailureOr<ReshapeOperandInfo> getReshapeOperandInfo(Value operand,
                                                    uint32_t concatAxisAfter,
                                                    tosa::ConcatOp concatOp,
                                                    PatternRewriter &rewriter) {
  if (!operand.hasOneUse())
    return rewriter.notifyMatchFailure(
        concatOp, "Operands must just connect to this concat.");

  auto reshape = operand.getDefiningOp<tosa::ReshapeOp>();
  if (!reshape)
    return rewriter.notifyMatchFailure(concatOp,
                                       "Operand is not a tosa.reshape");

  const auto inputType = dyn_cast<ShapedType>(reshape.getInput1().getType());
  const auto outputType = dyn_cast<ShapedType>(reshape.getType());
  if (!inputType || !inputType.hasStaticShape() || !outputType ||
      !outputType.hasStaticShape())
    return rewriter.notifyMatchFailure(
        concatOp, "Dynamic shapes for reshapes are not supported.");

  if (inputType.getRank() == 0)
    return rewriter.notifyMatchFailure(
        concatOp, "Tensors of rank 0 cannot have an independent concat axis.");

  int64_t prefixProductAfterReshape = 1;
  for (size_t i = 0; i < concatAxisAfter; ++i)
    prefixProductAfterReshape *= outputType.getDimSize(i);

  int64_t prefixProductBeforeReshape = 1;
  SmallVector<size_t> candidateAxes;
  for (size_t i = 0; i < static_cast<size_t>(inputType.getRank()); ++i) {
    if (prefixProductBeforeReshape == prefixProductAfterReshape)
      candidateAxes.push_back(i);
    prefixProductBeforeReshape *= inputType.getDimSize(i);
  }

  if (candidateAxes.empty())
    return rewriter.notifyMatchFailure(
        concatOp, "Sinking reshape not possible. No compatible dimension for "
                  "concat axis found.");

  ReshapeOperandInfo info;
  info.reshape = reshape;
  info.inputType = inputType;
  info.candidateAxes = std::move(candidateAxes);
  return info;
}

bool areConcatInputsCompatible(ArrayRef<ShapedType> inputTypes,
                               size_t concatAxis) {
  if (inputTypes.empty())
    return false;

  const ShapedType referenceType = inputTypes.front();
  const int64_t rank = referenceType.getRank();
  for (ShapedType inputType : inputTypes) {
    if (inputType.getRank() != rank)
      return false;

    for (int64_t dim = 0; dim < rank; ++dim) {
      if (static_cast<size_t>(dim) == concatAxis)
        continue;
      if (inputType.getDimSize(dim) != referenceType.getDimSize(dim))
        return false;
    }
  }

  return true;
}

struct SinkReshapeOp : public OpRewritePattern<tosa::ConcatOp> {
  using OpRewritePattern<tosa::ConcatOp>::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::ConcatOp concatOp,
                                PatternRewriter &rewriter) const override {
    // High-level algorithm:
    //   Before rewrite:
    //     reshape(x0), reshape(x1), ... -> concat(axis = A)
    //   After rewrite:
    //     concat(x0, x1, ... , axis = B) -> reshape
    //
    // Each reshape operand may admit several possible pre-reshape concat axes
    // because the reshape can fold or unfold groups of dimensions around the
    // post-reshape concat axis A. For every operand we compute the candidate
    // axes B whose prefix product before B matches the prefix product before A
    // in the reshaped tensor. This aligns the concat boundary in the same
    // linearized position, independent of whether the reshapes inserted or
    // removed size-1 dimensions or spread the concatenated chunk across
    // multiple input dimensions.
    //
    // We then intersect those candidate sets across all operands and choose the
    // first common axis that also makes the raw reshape inputs valid operands
    // of a concat, i.e. they have the same rank, element type, and identical
    // dimensions on all non-concat axes. If such an axis exists, we build the
    // concat on the reshape inputs and restore the original result shape with a
    // single reshape after the concat.
    const auto concatResultType = dyn_cast<ShapedType>(concatOp.getType());
    if (!concatResultType || !concatResultType.hasStaticShape())
      return rewriter.notifyMatchFailure(
          concatOp, "Concat result must be statically shaped.");

    const uint32_t concatAxisAfterReshape = concatOp.getAxis();
    SmallVector<ShapedType> inputTypes;
    SmallVector<Value> concatOperands;
    SmallVector<size_t> commonAxes;
    SmallVector<Location> fusedLocs{concatOp.getLoc()};
    [[maybe_unused]] Type inputElementType;

    for (Value operand : concatOp.getOperands()) {
      auto infoOrError = getReshapeOperandInfo(operand, concatAxisAfterReshape,
                                               concatOp, rewriter);
      if (failed(infoOrError))
        return failure();

      ReshapeOperandInfo &info = *infoOrError;
      if (!inputElementType)
        inputElementType = info.inputType.getElementType();
      assert(info.inputType.getElementType() == inputElementType &&
             "All reshape inputs must have the same element type.");

      if (inputTypes.empty()) {
        commonAxes = info.candidateAxes;
      } else {
        llvm::erase_if(commonAxes, [&](size_t axis) {
          return !llvm::is_contained(info.candidateAxes, axis);
        });
      }

      inputTypes.push_back(info.inputType);
      concatOperands.push_back(info.reshape.getInput1());
      fusedLocs.push_back(info.reshape.getLoc());
    }

    if (commonAxes.empty())
      return rewriter.notifyMatchFailure(
          concatOp, "Sinking reshape not possible. No common compatible "
                    "dimension for concat axis found.");

    std::optional<size_t> concatAxisBeforeReshape;
    for (size_t candidateAxis : commonAxes) {
      if (areConcatInputsCompatible(inputTypes, candidateAxis)) {
        concatAxisBeforeReshape = candidateAxis;
        break;
      }
    }

    if (!concatAxisBeforeReshape)
      return rewriter.notifyMatchFailure(
          concatOp, "Sinking reshape not possible. Reshape inputs are not "
                    "concat-compatible on a common pre-reshape axis.");

    Location fusedLoc = rewriter.getFusedLoc(fusedLocs);
    auto concatNew = rewriter.create<tosa::ConcatOp>(fusedLoc, concatOperands,
                                                     *concatAxisBeforeReshape);
    auto reshapeNew = rewriter.create<tosa::ReshapeOp>(
        fusedLoc, concatResultType, concatNew,
        rewriter.getDenseI64ArrayAttr(concatResultType.getShape()));
    rewriter.replaceOp(concatOp, reshapeNew.getResult());
    return success();
  }
};

struct SinkInputOpsThroughConcat
    : public tosa::impl::SinkInputOpsThroughConcatBase<
          SinkInputOpsThroughConcat> {

  SinkInputOpsThroughConcat() = default;
  explicit SinkInputOpsThroughConcat(
      const SinkInputOpsThroughConcatOptions &options, llvm::raw_ostream &os)
      : tosa::impl::SinkInputOpsThroughConcatBase<SinkInputOpsThroughConcat>(
            options),
        os(os) {}

  explicit SinkInputOpsThroughConcat(
      const SinkInputOpsThroughConcatOptions &options)
      : tosa::impl::SinkInputOpsThroughConcatBase<SinkInputOpsThroughConcat>(
            options) {}

  void runOnOperation() override {
    auto moduleOp = getOperation();
    RewritePatternSet patterns(moduleOp.getContext());
    MLIRContext *ctx = moduleOp.getContext();

    populateSinkInputOpsThroughConcatPattern(patterns, ctx);

    if (applyPatternsGreedily(moduleOp, std::move(patterns)).failed())
      signalPassFailure();
  }

private:
  void populateSinkInputOpsThroughConcatPattern(RewritePatternSet &patterns,
                                                MLIRContext *ctx) {
    // elementwise
    if (enableElementwises) {
      patterns.add<
          SinkSpecificOp<tosa::AbsOp>, SinkSpecificOp<tosa::BitwiseNotOp>,
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
    }
    // reduce
    if (enableReductions) {
      patterns
          .add<SinkReduceOp<tosa::ReduceAllOp>, SinkReduceOp<tosa::ReduceAnyOp>,
               SinkReduceOp<tosa::ReduceMaxOp>, SinkReduceOp<tosa::ReduceMinOp>,
               SinkReduceOp<tosa::ReduceProdOp>,
               SinkReduceOp<tosa::ReduceSumOp>>(ctx, /*benefit=*/2);
    }
    // others
    if (enableMatmul) {
      patterns.add<SinkMatmulOp>(ctx, /*benefit=*/2);
    }
    if (enableReshape) {
      populateSinkInputOpsThroughConcatReshapePatterns(patterns,
                                                       /*benefit=*/2);
    }
    if (matchUntransformedOperations) {
      patterns.add<SinkGenericOp>(ctx, /*benefit=*/1, operationFrequency, os);
    }
  }

  llvm::StringMap<unsigned> operationFrequency;
  llvm::raw_ostream &os = llvm::errs();
};
} // namespace

void mlir::tosa::populateSinkInputOpsThroughConcatReshapePatterns(
    RewritePatternSet &patterns, PatternBenefit benefit) {
  patterns.add<SinkReshapeOp>(patterns.getContext(), benefit);
}

std::unique_ptr<Pass> mlir::tosa::createSinkInputOpsThroughConcatPass(
    const SinkInputOpsThroughConcatOptions &options, llvm::raw_ostream &os) {
  return std::make_unique<SinkInputOpsThroughConcat>(options, os);
}
