//===- TosaCanonicalizations.cpp - Canonicalization patterns & folders ----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// \file
// TOSA canonicalization patterns and folders.
//
//===----------------------------------------------------------------------===//

#include "mlir/Dialect/Quant/IR/Quant.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/Dialect/Tosa/IR/TosaOps.h"
#include "mlir/Dialect/Tosa/Utils/ConversionUtils.h"
#include "mlir/Dialect/Tosa/Utils/QuantUtils.h"
#include "mlir/Dialect/Tosa/Utils/ShapeUtils.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/BuiltinTypeInterfaces.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/Matchers.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Transforms/FoldUtils.h"
#include "mlir/Transforms/InliningUtils.h"
#include "mlir/Transforms/RegionUtils.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/TypeSwitch.h"

#include <functional>

using namespace mlir;
using namespace mlir::tosa;

//===----------------------------------------------------------------------===//
// Operator Canonicalizers.
//===----------------------------------------------------------------------===//

struct ConcatOptimization : public OpRewritePattern<tosa::ConcatOp> {
  using OpRewritePattern<tosa::ConcatOp>::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::ConcatOp op,
                                PatternRewriter &rewriter) const override {
    if (op.getInput1().size() != 1)
      return failure();
    if (op.getInput1().front().getType() != op.getType()) {
      rewriter
          .replaceOpWithNewOp<tensor::CastOp>(op, op.getType(),
                                              op.getInput1().front())
          .getResult();
      return success();
    }

    rewriter.replaceOp(op, op.getInput1().front());
    return success();
  }
};

struct SelfConcatToTile : public OpRewritePattern<tosa::ConcatOp> {
  using OpRewritePattern<tosa::ConcatOp>::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::ConcatOp concatOp,
                                PatternRewriter &rewriter) const override {
    if (llvm::all_equal(concatOp->getUsers())) {
      const auto concatUser = llvm::dyn_cast<tosa::ConcatOp>(
          concatOp->getUses().begin()->getOwner());
      if (concatUser) {
        // Try folding the concat into its consumer before rewriting it to a
        // tile.
        SmallVector<Value> replacementValues;
        auto foldResult = rewriter.tryFold(concatUser, replacementValues);
        if (foldResult.succeeded()) {
          if (!replacementValues.empty()) {
            rewriter.replaceOp(concatUser, replacementValues);
          }
          return success();
        }
      }
    }

    if (!llvm::all_equal(concatOp->getOperands())) {
      return rewriter.notifyMatchFailure(
          concatOp, "Requires all operands to be the same");
    }
    const auto concatType = dyn_cast<ShapedType>(concatOp.getType());
    if (!concatType || !concatType.hasRank()) {
      return rewriter.notifyMatchFailure(concatOp,
                                         "Requires concat to be ranked");
    }
    SmallVector<int64_t> multiplies(concatType.getRank(), 1);
    multiplies[concatOp.getAxis()] = concatOp->getNumOperands();
    auto constantShapeValue =
        getTosaConstShape(rewriter, concatOp->getLoc(), multiplies);
    auto tileOp = rewriter.createOrFold<tosa::TileOp>(
        concatOp->getLoc(), concatOp.getType(), concatOp->getOperand(0),
        constantShapeValue);
    rewriter.replaceOp(concatOp, {tileOp});
    return success();
  }
};

void ConcatOp::getCanonicalizationPatterns(RewritePatternSet &results,
                                           MLIRContext *context) {
  results.add<ConcatOptimization>(context);
  results.add<SelfConcatToTile>(context);
}

struct FuseChainedTile : public OpRewritePattern<tosa::TileOp> {
  using OpRewritePattern<tosa::TileOp>::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::TileOp op,
                                PatternRewriter &rewriter) const override {
    SmallVector<int64_t> multiplies;
    if (failed(op.getConstantMultiples(multiplies))) {
      return rewriter.notifyMatchFailure(op, "Requires const multiplies");
    }
    auto inputTile = op.getInput1().getDefiningOp<TileOp>();
    if (!inputTile) {
      return rewriter.notifyMatchFailure(op, "Input is not a TileOp");
    }
    if (!inputTile->hasOneUse()) {
      return rewriter.notifyMatchFailure(op,
                                         "Input tile should only have one use");
    }
    SmallVector<int64_t> inputTileMultiples;
    if (failed(inputTile.getConstantMultiples(inputTileMultiples))) {
      return rewriter.notifyMatchFailure(
          op, "Requires const multiplies on input tile");
      ;
    }

    for (auto [idx, multiplier] : llvm::enumerate(inputTileMultiples)) {
      multiplies[idx] *= multiplier;
    }
    auto constantShapeValue = getTosaConstShape(
        rewriter,
        rewriter.getFusedLoc(
            {op.getMultiples().getLoc(), inputTile.getMultiples().getLoc()}),
        multiplies);

    rewriter.modifyOpInPlace(op, [&]() {
      op.setOperand(0, inputTile->getOperand(0));
      op.setOperand(1, constantShapeValue);
      op.getOperation()->setLoc(
          FusedLoc::get(getContext(), {inputTile->getLoc(), op.getLoc()}));
    });

    return success();
  }
};

void TileOp::getCanonicalizationPatterns(RewritePatternSet &results,
                                         MLIRContext *context) {
  results.add<FuseChainedTile>(context);
}

struct SqrtReciprocalOptimization : public OpRewritePattern<tosa::PowOp> {
  using OpRewritePattern<tosa::PowOp>::OpRewritePattern;
  // Pattern that matches a Sqrt + Reciprocal to replace them by a rsqrt.
  // Sqrt is represented in tosa by a Pow so we check for Pow + reciprocal.
  LogicalResult matchAndRewrite(tosa::PowOp op,
                                PatternRewriter &rewriter) const override {
    // Check that the PowOp has a single user
    if (!op->hasOneUse())
      return rewriter.notifyMatchFailure(op,
                                         "pow operator has more than one user");

    Operation *user = *op->user_begin();
    // Check that this user is a reciprocal
    if (!isa<tosa::ReciprocalOp>(user))
      return rewriter.notifyMatchFailure(op,
                                         "expected a pow + reciprocal pattern");

    // Check that the Pow op is an Sqrt - its second input should be the scale,
    // 0.5 for Sqrt.
    Operation *powScale = op.getInput2().getDefiningOp();
    if (!powScale || !isa<tosa::ConstOp>(powScale))
      return rewriter.notifyMatchFailure(
          op, "expected the pow to have a constant scale input");

    auto scale =
        cast<DenseElementsAttr>(cast<tosa::ConstOp>(powScale).getValue());
    if (!scale.isSplat())
      return rewriter.notifyMatchFailure(
          op, "expected the pow scale to be a splat tensor");

    float scaleValue = scale.getSplatValue<llvm::APFloat>().convertToFloat();
    if (scaleValue != 0.5)
      return rewriter.notifyMatchFailure(
          op, "expected the pow to have a scale of 0.5 to be a sqrt");

    auto inputType = cast<ShapedType>(op.getOperand(0).getType());
    auto outputType = cast<ShapedType>(op.getType());
    // If the operator needs tiling, fail to match
    // An improvement for the future would be to generate a tile operator here
    // instead
    if (inputType != outputType)
      return rewriter.notifyMatchFailure(
          op, "input type and output type are different, tiling is not "
              "supported for this canonicalization");

    auto rsqrtOp = rewriter.create<tosa::RsqrtOp>(
        rewriter.getFusedLoc({op.getLoc(), user->getLoc()}), outputType,
        op.getInput1());
    rewriter.replaceOp(user, rsqrtOp);

    return success();
  }
};

void PowOp::getCanonicalizationPatterns(RewritePatternSet &results,
                                        MLIRContext *context) {
  results.add<SqrtReciprocalOptimization>(context);
}

struct SelectLogicalNotOptimization : public OpRewritePattern<tosa::SelectOp> {
  using OpRewritePattern::OpRewritePattern;
  LogicalResult matchAndRewrite(tosa::SelectOp op,
                                PatternRewriter &rewriter) const override {
    auto notOp = op.getPred().getDefiningOp<tosa::LogicalNotOp>();
    if (!notOp)
      return failure();
    rewriter.modifyOpInPlace(op, [&]() {
      op.getOperation()->setOperands(
          {notOp.getInput1(), op.getOnFalse(), op.getOnTrue()});
    });
    return success();
  }
};

// This canonicalizes the following patterns:
// %0 = tosa.greater_equal(input, x)
// %1 = tosa.select(%0, input, x)
// to tosa.clamp{min = x, max = max}(input)
// and
// %0 = tosa.greater_equal(input, x)
// %1 = tosa.select(%0, x, input)
// to tosa.clamp{min = min, max = x}(input)
// The first pattern occurs in decompositions of LeakyReLU/PReLU with an alpha
// of zero
struct SelectToClampOptimization : public OpRewritePattern<tosa::SelectOp> {
  using OpRewritePattern::OpRewritePattern;
  LogicalResult matchAndRewrite(tosa::SelectOp op,
                                PatternRewriter &rewriter) const override {

    auto geq = op.getPred().getDefiningOp<tosa::GreaterEqualOp>();
    if (!geq) {
      return rewriter.notifyMatchFailure(op,
                                         "Predicate is not a GreaterEqualOp");
    }

    DenseElementsAttr geqIn2Attr;
    if (!matchPattern(geq.getInput2(), m_Constant(&geqIn2Attr))) {
      return rewriter.notifyMatchFailure(
          op, "RHS of predicate GreaterEqualOp is not a constant");
    }

    auto isCompatibleSplat = [](DenseElementsAttr a,
                                DenseElementsAttr b) -> bool {
      if (!a.isSplat() || !b.isSplat()) {
        return false;
      }

      auto aAsIntegerType = dyn_cast<IntegerType>(a.getElementType());
      auto bAsIntegerType = dyn_cast<IntegerType>(b.getElementType());
      if (aAsIntegerType && bAsIntegerType) {
        if (aAsIntegerType.getSignedness() != bAsIntegerType.getSignedness()) {
          return false;
        }

        auto aAsAPInt = a.getSplatValue<APInt>();
        auto bAsAPInt = b.getSplatValue<APInt>();

        const size_t aBitWidth = aAsAPInt.getBitWidth();
        const size_t bBitWidth = bAsAPInt.getBitWidth();

        if (aBitWidth >= bBitWidth) {
          return aAsAPInt == (bAsIntegerType.isUnsigned()
                                  ? bAsAPInt.zext(aBitWidth)
                                  : bAsAPInt.sext(aBitWidth));
        }
        return (aAsIntegerType.isUnsigned()
                    ? aAsAPInt.zext(bBitWidth)
                    : aAsAPInt.sext(bBitWidth)) == bAsAPInt;
      }

      auto aAsFloatType = dyn_cast<FloatType>(a.getElementType());
      auto bAsFloatType = dyn_cast<FloatType>(b.getElementType());
      if (!aAsFloatType || aAsFloatType != bAsFloatType) {
        return false;
      }

      return a.getSplatValue<APFloat>() == b.getSplatValue<APFloat>();
    };

    auto onFalse = op.getOnFalse();
    auto onTrue = op.getOnTrue();
    DenseElementsAttr onFalseAttr;
    DenseElementsAttr onTrueAttr;

    // Case one:
    // %0 = tosa.greater_equal(input, cmp)
    // %1 = tosa.select(%0, input, cmp)
    // to tosa.clamp{min = cmp, max = max}(input)
    // Predicate: geq.input2 == select.onFalse AND geq.input1 == select.onTrue
    const bool isCaseOne =
        matchPattern(onFalse, m_Constant(&onFalseAttr)) &&
        isCompatibleSplat(onFalseAttr, geqIn2Attr) &&
        onTrue.getDefiningOp() == geq.getInput1().getDefiningOp();

    // Case two:
    // %0 = tosa.greater_equal(input, cmp)
    // %1 = tosa.select(%0, cmp, input)
    // to tosa.clamp{min = input, max = cmp}(input)
    // Predicate: geq.input2 == select.onTrue AND geq.input1 == select.onFalse
    const bool isCaseTwo =
        !isCaseOne && matchPattern(onTrue, m_Constant(&onTrueAttr)) &&
        isCompatibleSplat(onTrueAttr, geqIn2Attr) &&
        onFalse.getDefiningOp() == geq.getInput1().getDefiningOp();

    if (!isCaseOne && !isCaseTwo) {
      return rewriter.notifyMatchFailure(
          op, "select does not match GEQ + select -> clamp pattern");
    }

    const auto inputElementType = geqIn2Attr.getElementType();
    int64_t clampIntMin = std::numeric_limits<int64_t>::min();
    int64_t clampIntMax = std::numeric_limits<int64_t>::max();
    FloatAttr clampFloatMin;
    FloatAttr clampFloatMax;
    if (auto integerType = dyn_cast<IntegerType>(inputElementType)) {
      int64_t splatValue;
      if (integerType.isUnsigned()) {
        if (integerType.getWidth() >= 63) {
          return rewriter.notifyMatchFailure(
              op, "Can not represent all values of input type as int64");
        }
        splatValue = geqIn2Attr.getSplatValue<APInt>().getZExtValue();
      } else {
        splatValue = geqIn2Attr.getSplatValue<APInt>().getSExtValue();
      }
      clampFloatMin =
          rewriter.getF32FloatAttr(-std::numeric_limits<float>::infinity());
      clampFloatMax =
          rewriter.getF32FloatAttr(std::numeric_limits<float>::infinity());
      if (isCaseOne) {
        clampIntMin = splatValue;
      } else {
        clampIntMax = splatValue;
      }
    } else if (isa<FloatType>(inputElementType)) {
      auto splatValue = geqIn2Attr.getSplatValue<APFloat>();
      if (isCaseOne) {
        clampFloatMin = rewriter.getFloatAttr(inputElementType, splatValue);
        clampFloatMax = rewriter.getFloatAttr(
            inputElementType,
            APFloat::getInf(splatValue.getSemantics(), false));
      } else {
        clampFloatMin = rewriter.getFloatAttr(
            inputElementType, APFloat::getInf(splatValue.getSemantics(), true));
        clampFloatMax = rewriter.getFloatAttr(inputElementType, splatValue);
      }
    }

    Value input = geq.getInput1();

    // In case they do not have same bit width, insert a cast to still be able
    // to do this canonicalization
    const size_t geqBitWidth =
        geq.getInput1().getType().getElementTypeBitWidth();
    const size_t selectBitWidth = op.getType().getElementTypeBitWidth();
    if (geqBitWidth != selectBitWidth) {
      input = rewriter.create<tosa::CastOp>(
          op->getLoc(),
          geq.getInput1().getType().clone(op.getType().getElementType()),
          input);
    }

    rewriter.replaceOpWithNewOp<tosa::ClampOp>(
        op, op.getType(), input, rewriter.getI64IntegerAttr(clampIntMin),
        rewriter.getI64IntegerAttr(clampIntMax), clampFloatMin, clampFloatMax);

    return success();
  }
};

void tosa::SelectOp::getCanonicalizationPatterns(RewritePatternSet &results,
                                                 MLIRContext *context) {
  results.add<SelectLogicalNotOptimization>(context);
  results.add<SelectToClampOptimization>(context);
}

struct ConsolidateTransposeOptimization
    : public OpRewritePattern<tosa::TransposeOp> {
  using OpRewritePattern::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::TransposeOp transposeOp,
                                PatternRewriter &rewriter) const override {
    // Input is also TransposeOp - transpose(transpose(A)).
    auto innerTranspose =
        transposeOp.getInput1().getDefiningOp<tosa::TransposeOp>();
    if (!innerTranspose)
      return rewriter.notifyMatchFailure(transposeOp,
                                         "input must be transpose operation");

    SmallVector<int32_t> transposePerms, innerTransposePerms;
    if (transposeOp.getConstantPerms(transposePerms).failed())
      return rewriter.notifyMatchFailure(transposeOp,
                                         "transpose perms must be constant");
    if (innerTranspose.getConstantPerms(innerTransposePerms).failed())
      return rewriter.notifyMatchFailure(
          transposeOp, "inner transpose perms must be constant");
    if (transposePerms.size() != innerTransposePerms.size())
      return rewriter.notifyMatchFailure(
          transposeOp,
          "transpose and inner transpose perms sizes must be equal");
    if (transposePerms.empty())
      return rewriter.notifyMatchFailure(
          transposeOp, "transpose perms sizes must be positive");

    // Consolidate transposes into one transpose.
    SmallVector<int32_t> perms(transposePerms.size());
    for (int i = 0, s = transposePerms.size(); i < s; ++i)
      perms[i] = innerTransposePerms[transposePerms[i]];

    auto permsTy =
        RankedTensorType::get(transposePerms.size(), rewriter.getI32Type());
    auto permsAttr = DenseIntElementsAttr::get(permsTy, perms);
    Value permsValue = rewriter.create<tosa::ConstOp>(transposeOp.getLoc(),
                                                      permsTy, permsAttr);

    rewriter.replaceOpWithNewOp<tosa::TransposeOp>(
        transposeOp, transposeOp.getResult().getType(),
        innerTranspose.getInput1(), permsValue);

    return success();
  }
};

// Determines the case when tosa.transpose is a tosa.reshape operation.
struct TransposeIsReshape : public OpRewritePattern<tosa::TransposeOp> {
  using OpRewritePattern::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::TransposeOp op,
                                PatternRewriter &rewriter) const override {
    DenseIntElementsAttr permAttr;
    if (!matchPattern(op.getPerms(), m_Constant(&permAttr)))
      return rewriter.notifyMatchFailure(op, "Non-constant permutation");

    if (op.getInput1().getDefiningOp<tosa::TransposeOp>())
      return rewriter.notifyMatchFailure(
          op, "Src is from transpose, can compose transposes");

    Value result = op.getResult();
    for (Operation *subop : result.getUsers()) {
      if (dyn_cast_or_null<tosa::TransposeOp>(subop))
        return rewriter.notifyMatchFailure(
            op, "Dest is used by transpose, can compose transposes");
    }

    auto input = op.getInput1();
    auto inputTy = llvm::cast<ShapedType>(input.getType());
    if (!inputTy.hasRank())
      return rewriter.notifyMatchFailure(op, "Unranked input.");

    int64_t numDynDims = 0;
    for (int i = 0; i < inputTy.getRank(); ++i)
      if (inputTy.isDynamicDim(i))
        numDynDims++;

    if (numDynDims > 1)
      return rewriter.notifyMatchFailure(op, "Has more than one dynamic dim.");

    SmallVector<int64_t> permValues = llvm::to_vector<6>(
        llvm::map_range(permAttr.getValues<APInt>(),
                        [](const APInt &val) { return val.getSExtValue(); }));

    SmallVector<int64_t> nonZeroPerms;
    nonZeroPerms.reserve(permValues.size());
    for (auto idx : permValues) {
      auto sz = inputTy.getDimSize(idx);
      if (sz != 1)
        nonZeroPerms.push_back(idx);
    }

    for (int i = 1, s = nonZeroPerms.size(); i < s; ++i)
      if (nonZeroPerms[i - 1] > nonZeroPerms[i])
        return rewriter.notifyMatchFailure(op,
                                           "Transpose changes memory layout.");

    SmallVector<int64_t> newShape;
    newShape.reserve(inputTy.getRank());
    for (int i = 0, s = inputTy.getRank(); i < s; ++i)
      newShape.push_back(inputTy.getDimSize(permValues[i]));

    rewriter.replaceOpWithNewOp<tosa::ReshapeOp>(
        op, op.getType(), op.getInput1(),
        rewriter.getDenseI64ArrayAttr(newShape));
    return success();
  }
};

void TransposeOp::getCanonicalizationPatterns(RewritePatternSet &results,
                                              MLIRContext *context) {
  results.add<ConsolidateTransposeOptimization, TransposeIsReshape>(context);
}

struct MaterializePadValue : public OpRewritePattern<tosa::PadOp> {
  using OpRewritePattern::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::PadOp op,
                                PatternRewriter &rewriter) const override {
    if (op.getPadConst())
      return failure();

    auto input = op.getInput1();
    auto padding = op.getPadding();

    ShapedType inputTy = llvm::cast<ShapedType>(input.getType());
    Type elementTy = inputTy.getElementType();

    Attribute constantAttr;
    if (llvm::isa<FloatType>(elementTy)) {
      constantAttr = rewriter.getFloatAttr(elementTy, 0.0);
    } else if (llvm::isa<IntegerType>(elementTy) && !op.getQuantizationInfo()) {
      constantAttr = rewriter.getIntegerAttr(elementTy, 0);
    } else if (llvm::isa<IntegerType>(elementTy) && op.getQuantizationInfo()) {
      auto value = op.getQuantizationInfo()->getInputZp();
      constantAttr = rewriter.getIntegerAttr(elementTy, value);
    }

    if (!constantAttr) {
      return rewriter.notifyMatchFailure(
          op,
          "tosa.pad to linalg lowering encountered an unknown element type");
    }

    auto denseAttr = DenseElementsAttr::get(
        RankedTensorType::get({}, elementTy), constantAttr);
    auto constantVal = rewriter.create<tosa::ConstOp>(
        op.getLoc(), denseAttr.getType(), denseAttr);

    rewriter.replaceOpWithNewOp<tosa::PadOp>(
        op, op.getType(), ValueRange{input, padding, constantVal},
        op->getAttrs());
    return success();
  }
};

void PadOp::getCanonicalizationPatterns(RewritePatternSet &results,
                                        MLIRContext *context) {
  results.add<MaterializePadValue>(context);
}

struct MaxPool2dIsNoOp : public OpRewritePattern<tosa::MaxPool2dOp> {
  using OpRewritePattern::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::MaxPool2dOp op,
                                PatternRewriter &rewriter) const override {
    Value input = op.getInput();
    Value output = op.getOutput();
    ShapedType inputType = llvm::cast<ShapedType>(input.getType());
    ShapedType outputType = llvm::cast<ShapedType>(output.getType());

    if (!inputType.hasStaticShape() || !outputType.hasStaticShape()) {
      return failure();
    }

    // If the output and input shapes are 1x1, then this is a no op.
    ArrayRef<int64_t> outputShape = outputType.getShape();
    if (outputShape[1] != 1 || outputShape[2] != 1) {
      return failure();
    }

    ArrayRef<int64_t> inputShape = inputType.getShape();
    if (inputShape[1] != 1 || inputShape[2] != 1) {
      return failure();
    }

    rewriter.replaceOp(op, input);
    return success();
  }
};

void MaxPool2dOp::getCanonicalizationPatterns(RewritePatternSet &results,
                                              MLIRContext *context) {
  results.add<MaxPool2dIsNoOp>(context);
}

struct ClampIsNoOp : public OpRewritePattern<tosa::ClampOp> {
  using OpRewritePattern::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::ClampOp op,
                                PatternRewriter &rewriter) const override {
    Value input = op.getInput();
    auto inputType = llvm::dyn_cast<RankedTensorType>(op.getInput().getType());
    auto inputElementType = inputType.getElementType();

    if (!inputType.hasStaticShape()) {
      return failure();
    }

    if (isa<FloatType>(inputElementType)) {
      // Unlike integer types, floating point types can represent infinity.
      auto minClamp = op.getMinFp();
      auto maxClamp = op.getMaxFp();
      bool isMin = minClamp.isInfinity() && minClamp.isNegative();
      bool isMax = maxClamp.isInfinity() && !maxClamp.isNegative();

      if (isMin && isMax) {
        rewriter.replaceOp(op, input);
        return success();
      }
      return failure();
    }

    if (inputElementType.isUnsignedInteger()) {
      int64_t minClamp = op.getMinInt();
      int64_t maxClamp = op.getMaxInt();

      int64_t intMin =
          APInt::getMinValue(inputElementType.getIntOrFloatBitWidth())
              .getZExtValue();
      int64_t intMax =
          APInt::getMaxValue(inputElementType.getIntOrFloatBitWidth())
              .getZExtValue();

      if (minClamp <= intMin && maxClamp >= intMax) {
        rewriter.replaceOp(op, input);
        return success();
      }
      return failure();
    }

    if (llvm::isa<IntegerType>(inputElementType)) {
      int64_t minClamp = op.getMinInt();
      int64_t maxClamp = op.getMaxInt();

      int64_t intMin =
          APInt::getSignedMinValue(inputElementType.getIntOrFloatBitWidth())
              .getSExtValue();
      int64_t intMax =
          APInt::getSignedMaxValue(inputElementType.getIntOrFloatBitWidth())
              .getSExtValue();

      if (minClamp <= intMin && maxClamp >= intMax) {
        rewriter.replaceOp(op, input);
        return success();
      }
      return failure();
    }

    return failure();
  }
};

// Attempts the following transformation:
//
// For integers a, b, a', and b' such that [a, b] ∩ [a', b'] ≠ ∅ and input
// tensor X the following identity holds:
//
// CLAMP(CLAMP(X, a, b), a', b') = CLAMP(X, max(a, a'),  min(b, b'))
//
// subject to the following valid NaN propagation semantics:
// --------------------------------------------
// | OUTER CLAMP | INNER CLAMP  | RESULT MODE |
// |-------------|--------------|-------------|
// | PROPAGATE   | PROPAGATE    | PROPAGATE   |
// | PROPAGATE   | IGNORE       | IGNORE      |
// | IGNORE      | PROPAGATE    | INVALID     |
// | IGNORE      | IGNORE       | IGNORE      |
// |------------------------------------------|

struct ClampClampOptimization : public OpRewritePattern<tosa::ClampOp> {
  using OpRewritePattern<tosa::ClampOp>::OpRewritePattern;

  // Helper structure to describe the range of a clamp operation.
  template <typename T>
  struct ClampRange {
    ClampRange(const T &start, const T &end) : start(start), end(end) {}
    T start;
    T end;

    // Helper function to determine if two Clamp ranges intersect.
    bool intersects(const ClampRange<T> &otherRange) {
      return start < otherRange.end && otherRange.start < end;
    }
  };

  LogicalResult matchAndRewrite(tosa::ClampOp op,
                                PatternRewriter &rewriter) const override {
    // Check the input to the CLAMP op is itself a CLAMP.
    auto clampOp =
        dyn_cast_if_present<tosa::ClampOp>(op.getInput().getDefiningOp());
    if (!clampOp)
      return failure();

    // Check we have a valid NaN propagation combination.
    const auto opNanMode = op.getNanMode();
    const auto clampNanMode = clampOp.getNanMode();
    if (opNanMode == "IGNORE" && clampNanMode == "PROPAGATE")
      return failure();

    // Check we have intersecting ranges.
    const auto opMinInt = op.getMinInt();
    const auto opMaxInt = op.getMaxInt();
    const auto clampOpMinInt = clampOp.getMinInt();
    const auto clampOpMaxInt = clampOp.getMaxInt();
    ClampRange<std::int64_t> opRangeIntRange(opMinInt, opMaxInt);
    ClampRange<std::int64_t> clampRangeIntRange(clampOpMinInt, clampOpMaxInt);
    if (!opRangeIntRange.intersects(clampRangeIntRange))
      return failure();

    const auto opMinFloat = op.getMinFp();
    const auto opMaxFloat = op.getMaxFp();
    const auto clampOpMinFloat = clampOp.getMinFp();
    const auto clampOpMaxFloat = clampOp.getMaxFp();
    ClampRange<APFloat> opRangeFloatRange(opMinFloat, opMaxFloat);
    ClampRange<APFloat> clampRangeFloatRange(clampOpMinFloat, clampOpMaxFloat);
    if (!opRangeFloatRange.intersects(clampRangeFloatRange))
      return failure();

    // Run the transformation.
    const auto minFp = std::max(opMinFloat, clampOpMinFloat).convertToFloat();
    const auto maxFp = std::min(opMaxFloat, clampOpMaxFloat).convertToFloat();
    const auto minInt = std::max(opMinInt, clampOpMinInt);
    const auto maxInt = std::min(opMaxInt, clampOpMaxInt);
    rewriter.replaceOpWithNewOp<tosa::ClampOp>(
        op, {op->getLoc(), clampOp->getLoc()}, op.getType(), clampOp.getInput(),
        rewriter.getI64IntegerAttr(minInt), rewriter.getI64IntegerAttr(maxInt),
        rewriter.getF32FloatAttr(minFp), rewriter.getF32FloatAttr(maxFp),
        rewriter.getStringAttr((opNanMode != clampNanMode) ? "IGNORE"
                                                           : opNanMode));
    return success();
  }
};

void ClampOp::getCanonicalizationPatterns(RewritePatternSet &results,
                                          MLIRContext *context) {
  results.add<ClampIsNoOp>(context);
  results.add<ClampClampOptimization>(context);
}

struct ConcatSliceOptimization : public OpRewritePattern<tosa::SliceOp> {
  using OpRewritePattern<tosa::SliceOp>::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::SliceOp sliceOp,
                                PatternRewriter &rewriter) const override {
    Value sliceInput = sliceOp.getInput1();
    auto concatOp = sliceInput.getDefiningOp<tosa::ConcatOp>();
    if (!concatOp)
      return rewriter.notifyMatchFailure(
          sliceOp, "slice input must be concat operation");

    OperandRange inputs = concatOp.getInput1();
    auto concatType = dyn_cast<RankedTensorType>(concatOp.getType());
    if (!concatType || !concatType.hasStaticShape())
      return rewriter.notifyMatchFailure(
          sliceOp, "slice input must be a static ranked tensor");
    int32_t axis = concatOp.getAxis();

    llvm::SmallVector<int64_t> sliceStart(sliceOp.getStart());
    llvm::ArrayRef<int64_t> sliceSize = sliceOp.getSize();
    llvm::SmallVector<Value> requiredConcatInputs;
    int64_t processedOriginalConcatInputSize = 0;
    int64_t droppedConcatInputSize = 0;
    for (auto input : inputs) {
      const auto inputType = dyn_cast<RankedTensorType>(input.getType());
      if (!inputType || !inputType.hasStaticShape())
        return rewriter.notifyMatchFailure(
            sliceOp, "concat input must be a static ranked tensor");
      if (processedOriginalConcatInputSize <
              (sliceStart[axis] + sliceSize[axis]) &&
          (processedOriginalConcatInputSize + inputType.getDimSize(axis)) >
              sliceStart[axis]) {
        if (requiredConcatInputs.empty()) {
          droppedConcatInputSize = processedOriginalConcatInputSize;
        }
        requiredConcatInputs.push_back(input);
      }
      processedOriginalConcatInputSize += inputType.getDimSize(axis);
    }
    if (requiredConcatInputs.size() == concatOp->getNumOperands()) {
      return rewriter.notifyMatchFailure(
          sliceOp, "Could not reduce number of inputs to preceding concat");
    }
    if (requiredConcatInputs.size() != 1 && !concatOp->hasOneUse()) {
      return rewriter.notifyMatchFailure(
          sliceOp,
          "Preceding concat must have a single use"); // Do not introduce new
                                                      // concats
    }
    if (requiredConcatInputs.empty()) {
      return rewriter.notifyMatchFailure(
          sliceOp, "degenerate slice with zero sized dim in output");
    }
    sliceStart[axis] -= droppedConcatInputSize;
    auto newConcat = rewriter.create<tosa::ConcatOp>(
        concatOp->getLoc(), requiredConcatInputs, axis);
    auto newSlice = rewriter.create<tosa::SliceOp>(
        sliceOp->getLoc(), sliceOp.getType(), newConcat,
        rewriter.getDenseI64ArrayAttr(sliceStart),
        rewriter.getDenseI64ArrayAttr(sliceSize));
    rewriter.replaceOp(sliceOp, newSlice);
    return success();
  }
};

///  This patterns adjust the multipliers of a tile followed by a slice to only
///  tile as much data as it is required by the slice
struct TileSliceOptimization : public OpRewritePattern<tosa::SliceOp> {
  using OpRewritePattern<tosa::SliceOp>::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::SliceOp sliceOp,
                                PatternRewriter &rewriter) const override {
    Value sliceInput = sliceOp.getInput1();
    auto tileOp = sliceInput.getDefiningOp<tosa::TileOp>();
    if (!tileOp)
      return rewriter.notifyMatchFailure(sliceOp,
                                         "slice input must be tile operation");
    if (!tileOp->hasOneUse())
      return rewriter.notifyMatchFailure(
          sliceOp, "preceding tile must have a single use"); // Do not insert
                                                             // additional tiles

    const auto tileOpInputType =
        dyn_cast<RankedTensorType>(tileOp->getOperand(0).getType());
    if (!tileOpInputType || !tileOpInputType.hasStaticShape())
      return rewriter.notifyMatchFailure(
          sliceOp, "input to preceding tile op must be a static ranked tensor");
    llvm::SmallVector<int64_t> requiredMultipliers;
    llvm::SmallVector<int64_t> newTileStarts;
    requiredMultipliers.reserve(tileOpInputType.getRank());
    newTileStarts.reserve(tileOpInputType.getRank());
    SmallVector<int64_t> tileMultiplies;
    const LogicalResult tileHasConstantMultiplies =
        tileOp.getConstantMultiples(tileMultiplies);
    for (auto [axis, sliceStart, sliceSize] :
         llvm::enumerate(sliceOp.getStart(), sliceOp.getSize())) {
      if (sliceSize <= 0) {
        return rewriter.notifyMatchFailure(
            sliceOp, "degenerate slice with zero sized dim");
      }
      const int64_t tileInputDimSize = tileOpInputType.getDimSize(axis);
      const int64_t sliceOffsetInNewFirstTile = sliceStart % tileInputDimSize;
      const int64_t sliceSizeInFirstTile =
          std::min(tileInputDimSize - sliceOffsetInNewFirstTile, sliceSize);
      assert(sliceSizeInFirstTile > 0);
      const int64_t requiredMultiplierWithoutFirstTile =
          llvm::divideCeil(sliceSize - sliceSizeInFirstTile, tileInputDimSize);
      const int64_t requiredMultiplier =
          requiredMultiplierWithoutFirstTile + (sliceSizeInFirstTile != 0);
      assert(failed(tileHasConstantMultiplies) ||
             requiredMultiplier <= tileMultiplies[axis]);
      requiredMultipliers.push_back(requiredMultiplier);
      newTileStarts.push_back(sliceOffsetInNewFirstTile);
    }

    if (succeeded(tileHasConstantMultiplies) &&
        requiredMultipliers == tileMultiplies) {
      return rewriter.notifyMatchFailure(
          sliceOp, "could not reduce multipliers in preceding tile");
    }

    llvm::SmallVector<int64_t> newTileShape(tileOpInputType.getShape());
    for (auto [newShape, multiplier] :
         llvm::zip_equal(newTileShape, requiredMultipliers)) {
      newShape *= multiplier;
    }
    auto constantShapeValue = getTosaConstShape(
        rewriter, tileOp.getMultiples().getLoc(), requiredMultipliers);
    auto newTile = rewriter.create<tosa::TileOp>(
        tileOp->getLoc(), tileOpInputType.clone(newTileShape),
        tileOp->getOperand(0), constantShapeValue);
    auto newSlice = rewriter.create<tosa::SliceOp>(
        sliceOp->getLoc(), sliceOp.getType(), newTile,
        rewriter.getDenseI64ArrayAttr(newTileStarts), sliceOp.getSizeAttr());
    rewriter.replaceOp(sliceOp, newSlice);
    return success();
  }
};

void SliceOp::getCanonicalizationPatterns(RewritePatternSet &results,
                                          MLIRContext *context) {
  results.add<ConcatSliceOptimization>(context);
  results.add<TileSliceOptimization>(context);
}

struct MinToClampOptimization : public OpRewritePattern<tosa::MinimumOp> {
  using OpRewritePattern<tosa::MinimumOp>::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::MinimumOp op,
                                PatternRewriter &rewriter) const override {

    DenseElementsAttr constant;
    if (!matchPattern(op.getInput2(), m_Constant(&constant)) ||
        !constant.isSplat())
      return failure();

    Value input = op.getInput1();
    auto elementTy = llvm::cast<ShapedType>(input.getType()).getElementType();

    int64_t minInt = std::numeric_limits<int32_t>::min();
    float minFp = std::numeric_limits<float>::lowest();

    int64_t maxInt;
    float maxFp;
    if (isa<FloatType>(elementTy)) {
      auto constMin = constant.getSplatValue<llvm::APFloat>();
      maxFp = constMin.convertToFloat();
      maxInt = constMin.convertToFloat();
    } else {
      auto constMin = constant.getSplatValue<llvm::APInt>();
      maxFp = constMin.getSExtValue();
      maxInt = constMin.getSExtValue();
    }

    rewriter.replaceOpWithNewOp<tosa::ClampOp>(
        op, op.getType(), input, rewriter.getI64IntegerAttr(minInt),
        rewriter.getI64IntegerAttr(maxInt), rewriter.getF32FloatAttr(minFp),
        rewriter.getF32FloatAttr(maxFp));

    return success();
  }
};

void MinimumOp::getCanonicalizationPatterns(RewritePatternSet &results,
                                            MLIRContext *context) {
  results.add<MinToClampOptimization>(context);
}

struct MaxToClampOptimization : public OpRewritePattern<tosa::MaximumOp> {
  using OpRewritePattern<tosa::MaximumOp>::OpRewritePattern;

  LogicalResult matchAndRewrite(tosa::MaximumOp op,
                                PatternRewriter &rewriter) const override {

    DenseElementsAttr constant;
    if (!matchPattern(op.getInput2(), m_Constant(&constant)) ||
        !constant.isSplat())
      return failure();

    Value input = op.getInput1();
    auto elementTy = llvm::cast<ShapedType>(input.getType()).getElementType();

    int64_t maxInt = std::numeric_limits<int64_t>::max();
    float maxFp = std::numeric_limits<float>::max();

    int64_t minInt;
    float minFp;
    if (isa<FloatType>(elementTy)) {
      auto constMax = constant.getSplatValue<llvm::APFloat>();
      minFp = constMax.convertToFloat();
      minInt = constMax.convertToFloat();
    } else {
      auto constMax = constant.getSplatValue<llvm::APInt>();
      minFp = constMax.getSExtValue();
      minInt = constMax.getSExtValue();
    }

    rewriter.replaceOpWithNewOp<tosa::ClampOp>(
        op, op.getType(), input, rewriter.getI64IntegerAttr(minInt),
        rewriter.getI64IntegerAttr(maxInt), rewriter.getF32FloatAttr(minFp),
        rewriter.getF32FloatAttr(maxFp));

    return success();
  }
};

void MaximumOp::getCanonicalizationPatterns(RewritePatternSet &results,
                                            MLIRContext *context) {
  results.add<MaxToClampOptimization>(context);
}

//===----------------------------------------------------------------------===//
// Operator Folders.
//===----------------------------------------------------------------------===//

template <typename IntFolder, typename FloatFolder>
DenseElementsAttr binaryFolder(DenseElementsAttr lhs, DenseElementsAttr rhs,
                               RankedTensorType returnTy) {
  if (rhs && lhs && rhs.isSplat() && lhs.isSplat()) {
    auto lETy = llvm::cast<ShapedType>(lhs.getType()).getElementType();
    auto rETy = llvm::cast<ShapedType>(rhs.getType()).getElementType();
    if (lETy != rETy)
      return {};

    if (llvm::isa<IntegerType>(lETy)) {
      APInt l = lhs.getSplatValue<APInt>();
      APInt r = rhs.getSplatValue<APInt>();
      auto result = IntFolder()(l, r);
      return DenseElementsAttr::get(returnTy, result);
    }

    if (llvm::isa<FloatType>(lETy)) {
      APFloat l = lhs.getSplatValue<APFloat>();
      APFloat r = rhs.getSplatValue<APFloat>();
      auto result = FloatFolder()(l, r);
      return DenseElementsAttr::get(returnTy, result);
    }
  }

  return {};
}

static bool isSplatZero(Type elemType, DenseElementsAttr val) {
  if (llvm::isa<FloatType>(elemType))
    return val && val.isSplat() && val.getSplatValue<APFloat>().isZero();
  if (llvm::isa<IntegerType>(elemType))
    return val && val.isSplat() && val.getSplatValue<APInt>().isZero();
  return false;
}

static bool isSplatOne(Type elemType, DenseElementsAttr val, int64_t shift) {
  if (llvm::isa<FloatType>(elemType))
    return val && val.isSplat() &&
           val.getSplatValue<APFloat>().isExactlyValue(1.0);
  if (llvm::isa<IntegerType>(elemType)) {
    const int64_t shifted = 1LL << shift;
    return val && val.isSplat() &&
           val.getSplatValue<APInt>().getSExtValue() == shifted;
  }
  return false;
}

OpFoldResult AddOp::fold(FoldAdaptor adaptor) {
  auto lhsTy = llvm::dyn_cast<RankedTensorType>(getInput1().getType());
  auto rhsTy = llvm::dyn_cast<RankedTensorType>(getInput2().getType());
  auto resultTy = llvm::dyn_cast<RankedTensorType>(getType());
  if (!lhsTy || !rhsTy || !resultTy)
    return {};

  // Cannot create an ElementsAttr from non-int/float/index types
  if (!lhsTy.getElementType().isIntOrIndexOrFloat() ||
      !rhsTy.getElementType().isIntOrIndexOrFloat())
    return {};

  auto resultETy = resultTy.getElementType();
  auto lhsAttr =
      llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput1());
  auto rhsAttr =
      llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput2());

  if (lhsTy == resultTy && isSplatZero(resultETy, rhsAttr))
    return getInput1();
  if (rhsTy == resultTy && isSplatZero(resultETy, lhsAttr))
    return getInput2();

  if (!lhsAttr || !rhsAttr)
    return {};

  return binaryFolder<std::plus<APInt>, std::plus<APFloat>>(lhsAttr, rhsAttr,
                                                            resultTy);
}

OpFoldResult ArgMaxOp::fold(FoldAdaptor adaptor) {
  auto inputTy = llvm::dyn_cast<RankedTensorType>(getInput().getType());
  auto outputTy = llvm::dyn_cast<RankedTensorType>(getType());
  if (!inputTy || !outputTy || !inputTy.hasStaticShape() ||
      !outputTy.hasStaticShape())
    return {};

  if (inputTy.getDimSize(getAxis()) == 1)
    return DenseElementsAttr::get(outputTy, 0);

  return {};
}

OpFoldResult IntDivOp::fold(FoldAdaptor adaptor) {
  auto lhsTy = llvm::dyn_cast<RankedTensorType>(getInput1().getType());
  auto rhsTy = llvm::dyn_cast<RankedTensorType>(getInput2().getType());
  auto resultTy = llvm::dyn_cast<RankedTensorType>(getType());
  if (!lhsTy || !rhsTy || !resultTy)
    return {};
  if (lhsTy != rhsTy)
    return {};

  // IntDivOp inputs must be integer type, no need to check for quantized type
  auto resultETy = resultTy.getElementType();
  auto lhsAttr =
      llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput1());
  auto rhsAttr =
      llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput2());
  if (lhsAttr && lhsAttr.isSplat()) {
    if (llvm::isa<IntegerType>(resultETy) &&
        lhsAttr.getSplatValue<APInt>().isZero())
      return lhsAttr;
  }

  if (rhsAttr && rhsAttr.isSplat()) {
    if (llvm::isa<IntegerType>(resultETy) &&
        rhsAttr.getSplatValue<APInt>().isOne())
      return getInput1();
  }

  if (rhsAttr && lhsAttr && rhsAttr.isSplat() && lhsAttr.isSplat()) {
    if (llvm::isa<IntegerType>(resultETy)) {
      APInt l = lhsAttr.getSplatValue<APInt>();
      APInt r = rhsAttr.getSplatValue<APInt>();
      APInt result = l.sdiv(r);
      return DenseElementsAttr::get(resultTy, result);
    }
  }

  return {};
}

namespace {
DenseElementsAttr mulBinaryFolder(DenseElementsAttr lhs, DenseElementsAttr rhs,
                                  RankedTensorType ty, int32_t shift) {
  if (rhs && lhs && rhs.isSplat() && lhs.isSplat()) {
    if (llvm::isa<IntegerType>(ty.getElementType())) {
      APInt l = lhs.getSplatValue<APInt>();
      APInt r = rhs.getSplatValue<APInt>();

      if (shift == 0) {
        return DenseElementsAttr::get(ty, l * r);
      }

      auto bitwidth = ty.getElementType().getIntOrFloatBitWidth();
      l = l.sext(bitwidth * 2);
      r = r.sext(bitwidth * 2);
      auto result = l * r;
      result.lshrInPlace(shift);
      result = result.trunc(bitwidth);
      return DenseElementsAttr::get(ty, result);
    }

    if (llvm::isa<FloatType>(ty.getElementType())) {
      APFloat l = lhs.getSplatValue<APFloat>();
      APFloat r = rhs.getSplatValue<APFloat>();
      APFloat result = l * r;
      return DenseElementsAttr::get(ty, result);
    }
  }

  return {};
}
} // namespace

OpFoldResult MulOp::fold(FoldAdaptor adaptor) {
  auto lhs = getInput1();
  auto rhs = getInput2();
  auto lhsTy = llvm::dyn_cast<RankedTensorType>(lhs.getType());
  auto rhsTy = llvm::dyn_cast<RankedTensorType>(rhs.getType());
  auto resultTy = llvm::dyn_cast<RankedTensorType>(getType());
  if (!lhsTy || !rhsTy || !resultTy || !resultTy.hasStaticShape())
    return {};

  auto resultETy = resultTy.getElementType();
  auto lhsAttr =
      llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput1());
  auto rhsAttr =
      llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput2());

  const int64_t shift = llvm::isa<IntegerType>(resultETy) ? getShift() : 0;

  if (rhsTy == resultTy) {
    if (isSplatZero(resultETy, lhsAttr))
      return lhsAttr.resizeSplat(resultTy);
    if (isSplatOne(resultETy, lhsAttr, shift))
      return rhs;
  }
  if (lhsTy == resultTy) {
    if (isSplatZero(resultETy, rhsAttr))
      return rhsAttr.resizeSplat(resultTy);
    if (isSplatOne(resultETy, rhsAttr, shift))
      return lhs;
  }

  return mulBinaryFolder(lhsAttr, rhsAttr, resultTy, getShift());
}

OpFoldResult SubOp::fold(FoldAdaptor adaptor) {
  auto lhsTy = llvm::dyn_cast<RankedTensorType>(getInput1().getType());
  auto rhsTy = llvm::dyn_cast<RankedTensorType>(getInput2().getType());
  auto resultTy = llvm::dyn_cast<RankedTensorType>(getType());
  if (!lhsTy || !rhsTy || !resultTy)
    return {};

  // Cannot create an ElementsAttr from non-int/float/index types
  if (!lhsTy.getElementType().isIntOrIndexOrFloat() ||
      !rhsTy.getElementType().isIntOrIndexOrFloat())
    return {};

  auto resultETy = resultTy.getElementType();
  auto lhsAttr =
      llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput1());
  auto rhsAttr =
      llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput2());

  if (lhsTy == resultTy && isSplatZero(resultETy, rhsAttr))
    return getInput1();

  if (!lhsAttr || !rhsAttr)
    return {};

  if (lhsTy != rhsTy)
    return {};

  return binaryFolder<std::minus<APInt>, std::minus<APFloat>>(lhsAttr, rhsAttr,
                                                              resultTy);
}

namespace {
template <typename Cmp>
struct ComparisonFold {
  ComparisonFold() = default;
  APInt operator()(const APInt &l, const APInt &r) {
    return APInt(1, Cmp()(l, r));
  }

  APInt operator()(const APFloat &l, const APFloat &r) {
    return APInt(1, Cmp()(l, r));
  }
};

struct APIntFoldGreater {
  APIntFoldGreater() = default;
  APInt operator()(const APInt &l, const APInt &r) {
    return APInt(1, l.sgt(r));
  }
};

struct APIntFoldGreaterEqual {
  APIntFoldGreaterEqual() = default;
  APInt operator()(const APInt &l, const APInt &r) {
    return APInt(1, l.sge(r));
  }
};
} // namespace

OpFoldResult GreaterOp::fold(FoldAdaptor adaptor) {
  auto resultTy = llvm::dyn_cast<RankedTensorType>(getType());
  auto lhsAttr =
      llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput1());
  auto rhsAttr =
      llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput2());

  if (!lhsAttr || !rhsAttr)
    return {};

  return binaryFolder<APIntFoldGreater, ComparisonFold<std::greater<APFloat>>>(
      lhsAttr, rhsAttr, resultTy);
}

OpFoldResult GreaterEqualOp::fold(FoldAdaptor adaptor) {
  auto resultTy = llvm::dyn_cast<RankedTensorType>(getType());
  auto lhsAttr =
      llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput1());
  auto rhsAttr =
      llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput2());

  if (!lhsAttr || !rhsAttr)
    return {};

  return binaryFolder<APIntFoldGreaterEqual,
                      ComparisonFold<std::greater_equal<APFloat>>>(
      lhsAttr, rhsAttr, resultTy);
}

OpFoldResult EqualOp::fold(FoldAdaptor adaptor) {
  auto resultTy = llvm::dyn_cast<RankedTensorType>(getType());
  auto lhsAttr =
      llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput1());
  auto rhsAttr =
      llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput2());
  Value lhs = getInput1();
  Value rhs = getInput2();
  auto lhsTy = llvm::cast<ShapedType>(lhs.getType());

  // If we are comparing an integer value to itself it is always true. We can
  // not do this with float due to float values.
  if (llvm::isa<IntegerType>(lhsTy.getElementType()) && resultTy &&
      resultTy.hasStaticShape() && lhs == rhs) {
    return DenseElementsAttr::get(resultTy, true);
  }

  if (!lhsAttr || !rhsAttr)
    return {};

  return binaryFolder<ComparisonFold<std::equal_to<APInt>>,
                      ComparisonFold<std::equal_to<APFloat>>>(lhsAttr, rhsAttr,
                                                              resultTy);
}

OpFoldResult CastOp::fold(FoldAdaptor adaptor) {
  if (getInput().getType() == getType())
    return getInput();

  // cast-to-iN(cast-to-iM(x)) -> cast-to-iN(x) when N <= M
  if (auto cast = getInput().getDefiningOp<CastOp>()) {
    auto intermediateElTy =
        cast.getType().getElementType().dyn_cast<IntegerType>();
    auto finalElTy = getType().getElementType().dyn_cast<IntegerType>();
    if (intermediateElTy && finalElTy &&
        intermediateElTy.getSignedness() == finalElTy.getSignedness() &&
        intermediateElTy.getWidth() >= finalElTy.getWidth()) {
      getInputMutable().assign(cast.getInput());
      return getResult();
    }
  }

  // Fold cast from bf16 -> f32 -> bf16 into no-op.
  if (auto cast = getInput().getDefiningOp<CastOp>()) {
    auto sourceElTy = cast.getInput().getType().getElementType();
    auto intermediateElTy = cast.getType().getElementType();
    auto finalElTy = getType().getElementType();
    if (isa<BFloat16Type>(sourceElTy) && isa<Float32Type>(intermediateElTy) &&
        isa<BFloat16Type>(finalElTy)) {
      getInputMutable().assign(cast.getInput());
      return getResult();
    }
  }

  auto operand = llvm::dyn_cast_if_present<ElementsAttr>(adaptor.getInput());
  if (!operand)
    return {};

  auto inTy = llvm::cast<ShapedType>(getInput().getType());
  auto outTy = llvm::cast<ShapedType>(getType());
  auto inETy = inTy.getElementType();
  auto outETy = outTy.getElementType();

  if (operand.isSplat()) {
    if (llvm::isa<FloatType>(inETy) && llvm::isa<FloatType>(outETy)) {
      bool overflow;
      auto splatVal = operand.getSplatValue<APFloat>();
      auto &semantics = llvm::cast<FloatType>(outETy).getFloatSemantics();
      splatVal.convert(semantics, llvm::RoundingMode::NearestTiesToEven,
                       &overflow);
      return SplatElementsAttr::get(outTy, splatVal);
    }

    if (llvm::isa<IntegerType>(inETy) && llvm::isa<FloatType>(outETy)) {
      auto unsign = llvm::cast<IntegerType>(inETy).isUnsignedInteger();
      APFloat splatVal(llvm::cast<FloatType>(outETy).getFloatSemantics());
      splatVal.convertFromAPInt(operand.getSplatValue<APInt>(), !unsign,
                                llvm::RoundingMode::NearestTiesToEven);
      return SplatElementsAttr::get(outTy, splatVal);
    }

    if (llvm::isa<FloatType>(inETy) && llvm::isa<IntegerType>(outETy)) {
      auto unsign = llvm::cast<IntegerType>(outETy).isUnsignedInteger();
      auto intVal = APSInt(
          llvm::cast<IntegerType>(outETy).getIntOrFloatBitWidth(), unsign);
      auto floatVal = operand.getSplatValue<APFloat>();
      bool exact;
      floatVal.convertToInteger(intVal, llvm::RoundingMode::NearestTiesToEven,
                                &exact);
      return SplatElementsAttr::get(outTy, intVal);
    }

    if (llvm::isa<IntegerType>(inETy) && llvm::isa<IntegerType>(outETy)) {
      auto unsignIn = llvm::cast<IntegerType>(inETy).isUnsignedInteger();
      bool trunc =
          inETy.getIntOrFloatBitWidth() > outETy.getIntOrFloatBitWidth();
      auto intVal = operand.getSplatValue<APInt>();
      auto bitwidth = outETy.getIntOrFloatBitWidth();

      if (trunc) {
        intVal = intVal.trunc(bitwidth);
      } else if (unsignIn) {
        intVal = intVal.zext(bitwidth);
      } else {
        intVal = intVal.sext(bitwidth);
      }

      return SplatElementsAttr::get(outTy, intVal);
    }
  }

  return {};
}

OpFoldResult ConstOp::fold(FoldAdaptor adaptor) { return getValueAttr(); }

OpFoldResult ConstShapeOp::fold(FoldAdaptor adaptor) { return getValueAttr(); }

#define REDUCE_FOLDER(OP)                                                      \
  OpFoldResult OP::fold(FoldAdaptor adaptor) {                                 \
    ShapedType inputTy = llvm::cast<ShapedType>(getInput().getType());         \
    if (!inputTy.hasRank())                                                    \
      return {};                                                               \
    if (inputTy != getType())                                                  \
      return {};                                                               \
    if (inputTy.getRank() == 0 || inputTy.getDimSize(getAxis()) == 1)          \
      return getInput();                                                       \
    return {};                                                                 \
  }

REDUCE_FOLDER(ReduceAllOp)
REDUCE_FOLDER(ReduceAnyOp)
REDUCE_FOLDER(ReduceMaxOp)
REDUCE_FOLDER(ReduceMinOp)
REDUCE_FOLDER(ReduceProdOp)
REDUCE_FOLDER(ReduceSumOp)
#undef REDUCE_FOLDER

OpFoldResult ReshapeOp::fold(FoldAdaptor adaptor) {
  auto inputTy = llvm::dyn_cast<RankedTensorType>(getInput1().getType());
  auto outputTy = llvm::dyn_cast<RankedTensorType>(getType());

  if (!inputTy || !outputTy)
    return {};

  // Fold when the input and output types are the same. This is only safe when
  // there is at most 1 dynamic dimension. For 2 or more dynamic dimensions,
  // there may still be a productive reshape.
  if (inputTy == outputTy && inputTy.getNumDynamicDims() < 2)
    return getInput1();

  // reshape(reshape(x)) -> reshape(x)
  if (auto reshapeOp = llvm::dyn_cast_if_present<tosa::ReshapeOp>(
          getInput1().getDefiningOp())) {
    getInput1Mutable().assign(reshapeOp.getInput1());

    // Fuse locations so that first ReshapeOp location isn't lost.
    getResult().getDefiningOp()->setLoc(
        mlir::FusedLoc::get(getContext(), {reshapeOp->getLoc(), getLoc()}));
    return getResult();
  }

  // Cannot create an ElementsAttr from non-int/float/index types
  if (!inputTy.getElementType().isIntOrIndexOrFloat())
    return {};

  // reshape(const(x)) -> const(reshape-attr(x))
  if (auto operand =
          llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput1())) {
    // Constants must have static shape.
    if (!outputTy.hasStaticShape())
      return {};

    // Okay to duplicate splat constants.
    if (operand.isSplat())
      return SplatElementsAttr::get(outputTy,
                                    operand.getSplatValue<Attribute>());

    // Don't duplicate other constants.
    if (!getInput1().hasOneUse())
      return {};

    return operand.reshape(
        llvm::cast<ShapedType>(operand.getType()).clone(getNewShape()));
  }

  return {};
}

OpFoldResult PadOp::fold(FoldAdaptor adaptor) {
  // If the pad is all zeros we can fold this operation away.
  if (adaptor.getPadding() && getInput1().getType() == getType()) {
    auto densePad = llvm::dyn_cast<DenseElementsAttr>(adaptor.getPadding());
    if (densePad && densePad.isSplat() &&
        densePad.getSplatValue<APInt>().isZero()) {
      return getInput1();
    }
  }

  return {};
}

// Fold away cases where a tosa.resize operation returns a copy
// of the input image.
OpFoldResult ResizeOp::fold(FoldAdaptor adaptor) {
  ArrayRef<int64_t> offset = getOffset();
  ArrayRef<int64_t> border = getBorder();
  ArrayRef<int64_t> scale = getScale();

  // Check unit scaling.
  if (scale[0] != scale[1] || scale[2] != scale[3]) {
    return {};
  }

  // There should be no offset.
  if (offset[0] != 0 || offset[1] != 0) {
    return {};
  }

  // There should be no border.
  if (border[0] != 0 || border[1] != 0) {
    return {};
  }

  auto input = getInput();
  auto inputTy = llvm::cast<RankedTensorType>(input.getType());
  auto resultTy = llvm::cast<RankedTensorType>(getType());
  if (inputTy != resultTy)
    return {};

  return input;
}

OpFoldResult ReverseOp::fold(FoldAdaptor adaptor) {
  auto operand = getInput1();
  auto operandTy = llvm::cast<ShapedType>(operand.getType());
  auto axis = getAxis();
  auto operandAttr =
      llvm::dyn_cast_if_present<SplatElementsAttr>(adaptor.getInput1());
  if (operandAttr)
    return operandAttr;

  // If the dim-length is 1, tosa.reverse is a no-op.
  if (operandTy.hasRank() &&
      (operandTy.getRank() == 0 || operandTy.getDimSize(axis) == 1))
    return operand;

  return {};
}

OpFoldResult SliceOp::fold(FoldAdaptor adaptor) {
  const auto tryFoldWithPrecedingSlice = [this](FoldAdaptor adaptor) {
    auto precedingSliceOp = getInput1().getDefiningOp<SliceOp>();
    if (!precedingSliceOp)
      return failure();
    const auto precedingSliceStart = precedingSliceOp.getStart();
    const auto thisSliceStart = getStart();
    SmallVector<int64_t> newSliceStart;
    newSliceStart.reserve(precedingSliceStart.size());
    for (auto [startPreceding, startThis] :
         llvm::zip_equal(precedingSliceStart, thisSliceStart)) {
      newSliceStart.push_back(startPreceding + startThis);
    }
    setOperand(precedingSliceOp->getOperand(0));
    setStart(newSliceStart);
    getOperation()->setLoc(
        FusedLoc::get(getContext(), {precedingSliceOp->getLoc(), getLoc()}));
    return success();
  };

  // First try folding the preceding slice, this also works if the shapes are
  // dynamic
  if (succeeded(tryFoldWithPrecedingSlice(adaptor)))
    return getResult();

  auto inputTy = llvm::dyn_cast<RankedTensorType>(getInput1().getType());
  auto outputTy = llvm::dyn_cast<RankedTensorType>(getType());

  if (!inputTy || !outputTy)
    return {};

  if (inputTy == outputTy && inputTy.hasStaticShape())
    return getInput1();

  if (!adaptor.getInput1())
    return {};

  // Cannot create an ElementsAttr from non-int/float/index types
  if (!inputTy.getElementType().isIntOrIndexOrFloat() ||
      !outputTy.getElementType().isIntOrIndexOrFloat())
    return {};

  auto operand = llvm::cast<ElementsAttr>(adaptor.getInput1());
  if (operand.isSplat() && outputTy.hasStaticShape()) {
    return SplatElementsAttr::get(outputTy, operand.getSplatValue<Attribute>());
  }

  if (inputTy.hasStaticShape() && outputTy.hasStaticShape() &&
      outputTy.getNumElements() == 1) {
    llvm::SmallVector<uint64_t> indices(getStart());
    auto value = operand.getValues<Attribute>()[indices];
    return SplatElementsAttr::get(outputTy, value);
  }

  return {};
}

OpFoldResult tosa::SelectOp::fold(FoldAdaptor adaptor) {
  if (getOnTrue() == getOnFalse())
    return getOnTrue();

  auto predicate =
      llvm::dyn_cast_if_present<DenseIntElementsAttr>(adaptor.getPred());
  if (!predicate)
    return {};

  if (!predicate.isSplat())
    return {};
  return predicate.getSplatValue<APInt>().getBoolValue() ? getOnTrue()
                                                         : getOnFalse();
}

OpFoldResult TileOp::fold(FoldAdaptor adaptor) {
  SmallVector<int64_t> multiples;
  if (getInput1().getType() != getType() ||
      failed(getConstantMultiples(multiples)) ||
      !llvm::all_of(multiples, [](int64_t v) { return v == 1; })) {
    return {};
  }
  return getInput1();
}

OpFoldResult TransposeOp::fold(FoldAdaptor adaptor) {
  auto resultTy = llvm::cast<ShapedType>(getType());

  // Transposing splat values just means reshaping.
  if (auto input =
          llvm::dyn_cast_if_present<DenseElementsAttr>(adaptor.getInput1())) {
    if (input.isSplat() && resultTy.hasStaticShape() &&
        input.getType().getElementType() == resultTy.getElementType())
      return input.reshape(resultTy);
  }

  // Transpose is not the identity transpose.
  SmallVector<int32_t> perms;
  if (getConstantPerms(perms).failed())
    return {};

  if (!llvm::equal(llvm::seq<int32_t>(0, perms.size()), perms))
    return {};

  return getInput1();
}

OpFoldResult tosa::LogOp::fold(FoldAdaptor adaptor) {
  auto input = getInput1();
  // Element-wise log(exp(x)) = x
  if (auto op = input.getDefiningOp<tosa::ExpOp>()) {
    return op.getInput1();
  }

  return {};
}

OpFoldResult tosa::ExpOp::fold(FoldAdaptor adaptor) {
  auto input = getInput1();
  // Element-wise exp(log(x)) = x
  if (auto op = input.getDefiningOp<tosa::LogOp>()) {
    return op.getInput1();
  }

  return {};
}

OpFoldResult tosa::NegateOp::fold(FoldAdaptor adaptor) {
  auto input = getInput1();
  // Element-wise negate(negate(x)) = x
  if (auto op = input.getDefiningOp<tosa::NegateOp>()) {
    return op.getInput1();
  }

  return {};
}

OpFoldResult tosa::AbsOp::fold(FoldAdaptor adaptor) {
  auto input = getInput1();
  // Element-wise abs(abs(x)) = abs(x)
  if (auto op = input.getDefiningOp<tosa::AbsOp>()) {
    return input;
  }

  return {};
}

OpFoldResult ConcatOp::fold(FoldAdaptor adaptor) {
  /// Remove operands that have zero elements.
  bool changed = false;
  for (size_t i = 0; i < getInput1().size();) {
    auto input = cast<RankedTensorType>(getInput1()[i].getType());
    // Ensure that we have at least one operand left.
    if (input.getDimSize(getAxis()) == 0 && getInput1().size() > 1) {
      getInput1Mutable().erase(i);
      changed = true;
    } else {
      ++i;
    }
  }
  if (changed)
    return getResult();

  // Fold consecutive concats on the same axis into a single op.
  // Keep track of the operands so we are able to construct a new concat
  // later. Conservatively assume that we double the number of operands when
  // folding
  SmallVector<Value, 8> concatOperands;
  concatOperands.reserve(2 * getNumOperands());

  // Find all operands that are foldable concats
  bool foundFoldableConcat = false;
  for (Value operand : getOperands()) {
    concatOperands.emplace_back(operand);

    auto producer = dyn_cast_or_null<ConcatOp>(operand.getDefiningOp());
    if (!producer)
      continue;

    // Not foldable if axes are not the same
    if (getAxis() != producer.getAxis())
      continue;

    // If there are multiple uses of this operand concat and they are different
    // operations, this means that operand concat will have to happen, so do not
    // add its operands to us to avoid repeating data concatenation
    const bool allConcatUsersAreThisConcat = llvm::all_of(
        producer->getUsers(), [&](Operation *user) { return *this == user; });
    if (!allConcatUsersAreThisConcat)
      continue;

    // Replace the original operand with all incoming operands
    foundFoldableConcat = true;
    concatOperands.pop_back();
    llvm::append_range(concatOperands, producer->getOperands());
  }

  if (!foundFoldableConcat)
    return {};

  getOperation()->setOperands(concatOperands);
  return getResult();
}

OpFoldResult tosa::ReciprocalOp::fold(FoldAdaptor adaptor) {
  auto input = adaptor.getInput1();

  auto inputAttr = llvm::dyn_cast_if_present<DenseElementsAttr>(input);
  // Fold splat inputs only.
  if (!inputAttr || !inputAttr.isSplat())
    return {};

  auto shapeType = llvm::cast<ShapedType>(getType());
  if (auto floatType = llvm::dyn_cast<FloatType>(inputAttr.getElementType())) {
    auto floatVal = inputAttr.getSplatValue<APFloat>();
    return DenseElementsAttr::get(shapeType,
                                  ReciprocalOp::calcOneElement(floatVal));
  }

  return {};
}
