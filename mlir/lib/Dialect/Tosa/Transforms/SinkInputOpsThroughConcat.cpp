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
#include "mlir/IR/BuiltinTypes.h"
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

#include <numeric>
#include <optional>

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

// Returns the product of all dimensions in `shape` (1 if empty).
// Example: shapeProduct([2, 3, 4]) == 24; shapeProduct([]) == 1.
int64_t shapeProduct(ArrayRef<int64_t> shape) {
  return std::accumulate(shape.begin(), shape.end(), static_cast<int64_t>(1),
                         std::multiplies<int64_t>());
}

// Decomposition of one reshape input around its pre-reshape concat axis `B`:
//   - `prefix`:     input dims before `B`;
//   - `innerGroup`: dims after `B` that collapse with `B` into the concat axis;
//   - `suffix`:     dims after that group.
// The concat group is the contiguous run of input axes starting at `B` whose
// product equals the post-reshape concat dimension.
//
// Example: input tensor<2x3x4x5x6> reshaped to tensor<2x60x6>, concat on the
// output axis of size 60. `B` is the input axis of size 3, and the run
// 3*4*5 == 60 collapses into the concat axis, so:
//   prefix = [2], innerGroup = [4, 5], suffix = [6].
struct OperandAxisLayout {
  SmallVector<int64_t> prefix;
  SmallVector<int64_t> innerGroup;
  SmallVector<int64_t> suffix;

  // Decomposes one reshape input around the concat axis, or returns nullopt if
  // no such decomposition exists. `boundary` is the product of the output dims
  // before the concat axis (shared by all operands, as those dims are
  // concat-invariant); `target` is this operand's post-reshape concat-axis
  // size. `B` is the first non-unit input axis whose prefix product equals
  // `boundary`, so unit dims at the boundary are never chosen as the concat
  // axis.
  //
  // Example: inputType tensor<2x3x4x5x6>, boundary = 2, target = 60. The prefix
  // product reaches 2 at input axis 1 (size 3, non-unit), so B = 1. Growing the
  // group from there gives 3*4*5 == 60 == target, yielding
  // prefix = [2], innerGroup = [4, 5], suffix = [6].
  static std::optional<OperandAxisLayout>
  compute(ShapedType inputType, int64_t boundary, int64_t target) {
    ArrayRef<int64_t> inShape = inputType.getShape();

    int64_t prefixProduct = 1;
    size_t axisB = inShape.size();
    for (size_t i = 0; i < inShape.size(); ++i) {
      if (prefixProduct == boundary && inShape[i] != 1) {
        axisB = i;
        break;
      }
      prefixProduct *= inShape[i];
    }
    if (axisB == inShape.size())
      return std::nullopt;

    // Grow the concat group from `axisB` until its product reaches `target`.
    int64_t groupProduct = 1;
    size_t groupEnd = axisB;
    for (; groupEnd < inShape.size(); ++groupEnd) {
      groupProduct *= inShape[groupEnd];
      if (groupProduct >= target)
        break;
    }
    if (groupProduct != target)
      return std::nullopt;
    ++groupEnd;

    OperandAxisLayout layout;
    layout.prefix.assign(inShape.begin(), inShape.begin() + axisB);
    layout.innerGroup.assign(inShape.begin() + axisB + 1,
                             inShape.begin() + groupEnd);
    layout.suffix.assign(inShape.begin() + groupEnd, inShape.end());
    return layout;
  }
};

// Plan to sink reshapes whose inputs decompose the concat axis differently (or
// differ only in unit-dim placement). Each minority operand gets one adapter
// reshape to a shared layout, after which a single concat plus one trailing
// reshape is enough. `adaptedShapes[k]` is the input shape of operand `k` after
// adaptation; `needsAdapter[k]` is set iff that operand requires the reshape.
//
// Example: three reshapes feeding a concat on output axis 1:
//   op0, op1: tensor<2x3x4x5>  -> tensor<2x12x5>   (split 12 as 3x4)
//   op2:      tensor<2x12x2x5> -> tensor<2x24x5>   (split 24 as 12x2)
// The majority split (3x4, innerGroup [4]) is the reference, so only op2 needs
// an adapter to tensor<2x6x4x5> (re-splitting 24 as 6x4), giving:
//   concatAxis    = 1
//   adaptedShapes = {2x3x4x5, 2x3x4x5, 2x6x4x5}
//   needsAdapter  = {false, false, true}
struct ReshapeSinkAdapterPlan {
  size_t concatAxis;
  SmallVector<SmallVector<int64_t>> adaptedShapes;
  SmallVector<bool> needsAdapter;

  // Builds an adapter plan, or returns nullopt when sinking is impossible or
  // would not reduce the reshape count. Non-concat regions are matched by
  // element count (so differing decompositions and unit-dim placements still
  // qualify), and minority operands are reshaped to the majority's real layout.
  //
  // Example: four reshapes feeding a concat on output axis 1 (boundary = 4):
  //   arg0: tensor<4x4x4x5> -> tensor<4x16x5>   (innerGroup [4])
  //   arg1: tensor<4x2x8x5> -> tensor<4x16x5>   (innerGroup [8])
  //   arg2: tensor<4x8x2x5> -> tensor<4x16x5>   (innerGroup [2])
  //   arg3: tensor<4x8x4x5> -> tensor<4x32x5>   (innerGroup [4])
  // The innerGroup [4] layout shared by arg0 and arg3 is the majority, so arg1
  // and arg2 are adapted to tensor<4x4x4x5>, yielding concatAxis = 1 and
  // needsAdapter = {false, true, true, false}. The 4 reshapes then collapse to
  // 2 adapters + 1 concat + 1 trailing reshape.
  static std::optional<ReshapeSinkAdapterPlan>
  compute(ArrayRef<tosa::ReshapeOp> reshapes, ArrayRef<ShapedType> inputTypes,
          size_t concatAxisAfter) {
    const size_t numOperands = inputTypes.size();
    if (numOperands == 0)
      return std::nullopt;

    // Dims before the concat axis are concat-invariant, so their product
    // (`boundary`) is shared by all operands.
    tosa::ReshapeOp firstReshape = reshapes.front();
    auto firstOutput = cast<ShapedType>(firstReshape.getType());
    const int64_t boundary =
        shapeProduct(firstOutput.getShape().take_front(concatAxisAfter));

    SmallVector<OperandAxisLayout> layouts;
    layouts.reserve(numOperands);
    for (size_t k = 0; k < numOperands; ++k) {
      tosa::ReshapeOp reshape = reshapes[k];
      const int64_t target =
          cast<ShapedType>(reshape.getType()).getDimSize(concatAxisAfter);
      std::optional<OperandAxisLayout> layout =
          OperandAxisLayout::compute(inputTypes[k], boundary, target);
      if (!layout)
        return std::nullopt;
      layouts.push_back(std::move(*layout));
    }

    // The concat is valid only if all operands agree on the element counts of
    // the regions surrounding the concat axis. Their decompositions may differ:
    // each minority operand is rewritten to the reference layout by an adapter
    // reshape, which relinearizes its prefix/suffix regardless of how those
    // dims were originally factored (e.g. a 40x2 suffix and an 80x1 suffix
    // describe the same data and adapt to one another). Unit-dim placement
    // differences are covered too. The products are guaranteed equal whenever
    // the surrounding output dims match, but this stays defensive.
    const int64_t refPrefixProduct = shapeProduct(layouts.front().prefix);
    const int64_t refSuffixProduct = shapeProduct(layouts.front().suffix);
    for (const OperandAxisLayout &layout : layouts)
      if (shapeProduct(layout.prefix) != refPrefixProduct ||
          shapeProduct(layout.suffix) != refSuffixProduct)
        return std::nullopt;

    // Pick the real layout shared by the most operands so the fewest adapters
    // are needed; ties resolve to the first occurrence.
    auto sameLayout = [](const OperandAxisLayout &a,
                         const OperandAxisLayout &b) {
      return a.prefix == b.prefix && a.innerGroup == b.innerGroup &&
             a.suffix == b.suffix;
    };
    const OperandAxisLayout *reference = &layouts.front();
    size_t bestCount = 0;
    for (const OperandAxisLayout &candidate : layouts) {
      const size_t count = llvm::count_if(layouts, [&](const auto &layout) {
        return sameLayout(layout, candidate);
      });
      if (count > bestCount) {
        bestCount = count;
        reference = &candidate;
      }
    }

    const int64_t referenceInnerProduct = shapeProduct(reference->innerGroup);
    if (referenceInnerProduct == 0)
      return std::nullopt;

    // Reshape every minority operand to the reference layout, re-splitting its
    // concat dim as <concatDim / innerProduct, innerGroup...>.
    SmallVector<SmallVector<int64_t>> adaptedShapes(numOperands);
    SmallVector<bool> needsAdapter(numOperands, false);
    size_t numAdapters = 0;
    for (size_t k = 0; k < numOperands; ++k) {
      if (sameLayout(layouts[k], *reference)) {
        adaptedShapes[k] = llvm::to_vector(inputTypes[k].getShape());
        continue;
      }

      tosa::ReshapeOp reshape = reshapes[k];
      const int64_t concatDim =
          cast<ShapedType>(reshape.getType()).getDimSize(concatAxisAfter);
      if (concatDim % referenceInnerProduct != 0)
        return std::nullopt;

      SmallVector<int64_t> &shape = adaptedShapes[k];
      shape = reference->prefix;
      shape.push_back(concatDim / referenceInnerProduct);
      llvm::append_range(shape, reference->innerGroup);
      llvm::append_range(shape, reference->suffix);
      needsAdapter[k] = true;
      ++numAdapters;
    }

    // Cost gate: the rewrite emits `numAdapters` reshapes plus one trailing
    // reshape, so only fire when that is strictly fewer than the originals.
    if (numAdapters + 1 >= numOperands)
      return std::nullopt;

    return ReshapeSinkAdapterPlan{reference->prefix.size(),
                                  std::move(adaptedShapes),
                                  std::move(needsAdapter)};
  }
};

struct SinkReshapeOp : public OpRewritePattern<tosa::ConcatOp> {
  using OpRewritePattern<tosa::ConcatOp>::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::ConcatOp concatOp,
                                PatternRewriter &rewriter) const override {
    // Sinks per-operand reshapes below the concat:
    //   concat(reshape(x0), reshape(x1), ...) -> reshape(concat(x0', x1', ...))
    // The pre-reshape concat axis is the input axis mapping to the same
    // linearized position as the post-reshape concat axis. When operands split
    // that axis differently (or place unit dims differently), minority operands
    // are first adapted to a shared layout. For example:
    //   6x1x6 -reshape-> 2x3x1x2x3 -concat(axis=2)-> 2x3x2x2x3
    // sinks to:
    //   6x1x6 -concat(axis=1)-> 6x2x6 -reshape-> 2x3x2x2x3
    const auto concatResultType = dyn_cast<ShapedType>(concatOp.getType());
    if (!concatResultType || !concatResultType.hasStaticShape())
      return rewriter.notifyMatchFailure(
          concatOp, "Concat result must be statically shaped.");

    const uint32_t concatAxisAfterReshape = concatOp.getAxis();
    SmallVector<ShapedType> inputTypes;
    SmallVector<tosa::ReshapeOp> reshapes;
    SmallVector<size_t> commonAxes;
    SmallVector<Location> fusedLocs{concatOp.getLoc()};

    for (Value operand : concatOp.getOperands()) {
      FailureOr<ReshapeOperandInfo> info = getReshapeOperandInfo(
          operand, concatAxisAfterReshape, concatOp, rewriter);
      if (failed(info))
        return failure();

      if (inputTypes.empty())
        commonAxes = info->candidateAxes;
      else
        llvm::erase_if(commonAxes, [&](size_t axis) {
          return !llvm::is_contained(info->candidateAxes, axis);
        });

      inputTypes.push_back(info->inputType);
      reshapes.push_back(info->reshape);
      fusedLocs.push_back(info->reshape.getLoc());
    }

    const Location fusedLoc = rewriter.getFusedLoc(fusedLocs);
    SmallVector<Value> operands;
    size_t concatAxisBeforeReshape = 0;

    // Fast path: a common pre-reshape axis whose raw inputs are directly
    // concat-compatible needs no adapter. An empty `commonAxes` (e.g. unit dims
    // shifted the axis indices apart) is not fatal; the adapter plan can still
    // normalize the operands.
    for (size_t candidateAxis : commonAxes) {
      if (areConcatInputsCompatible(inputTypes, candidateAxis)) {
        concatAxisBeforeReshape = candidateAxis;
        for (tosa::ReshapeOp reshape : reshapes)
          operands.push_back(reshape.getInput1());
        break;
      }
    }

    // Slow path: adapt minority operands to a shared decomposition, inserting
    // an adapter reshape on each.
    if (operands.empty()) {
      std::optional<ReshapeSinkAdapterPlan> plan =
          ReshapeSinkAdapterPlan::compute(reshapes, inputTypes,
                                          concatAxisAfterReshape);
      if (!plan)
        return rewriter.notifyMatchFailure(
            concatOp, "Reshape inputs are not concat-compatible on a common "
                      "pre-reshape axis, even after adapting decompositions.");

      concatAxisBeforeReshape = plan->concatAxis;
      for (size_t k = 0; k < reshapes.size(); ++k) {
        Value input = reshapes[k].getInput1();
        if (plan->needsAdapter[k])
          input = rewriter.create<tosa::ReshapeOp>(
              fusedLoc,
              RankedTensorType::get(plan->adaptedShapes[k],
                                    inputTypes[k].getElementType()),
              input, rewriter.getDenseI64ArrayAttr(plan->adaptedShapes[k]));
        operands.push_back(input);
      }
    }

    auto concatNew = rewriter.create<tosa::ConcatOp>(fusedLoc, operands,
                                                     concatAxisBeforeReshape);
    rewriter.replaceOpWithNewOp<tosa::ReshapeOp>(
        concatOp, concatResultType, concatNew,
        rewriter.getDenseI64ArrayAttr(concatResultType.getShape()));
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
