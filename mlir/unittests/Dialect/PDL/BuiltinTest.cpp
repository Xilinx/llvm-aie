//===- BuiltinTest.cpp - PDL Builtin Tests --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "mlir/Dialect/PDL/IR/Builtins.h"
#include "gmock/gmock.h"
#include <cstdint>
#include <gtest/gtest.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <mlir/IR/Attributes.h>
#include <mlir/IR/BuiltinAttributes.h>
#include <mlir/IR/BuiltinTypes.h>
#include <mlir/IR/PatternMatch.h>
#include <mlir/IR/Region.h>

using namespace mlir;
using namespace mlir::pdl;

namespace {

class TestPatternRewriter : public PatternRewriter {
public:
  TestPatternRewriter(MLIRContext *ctx) : PatternRewriter(ctx) {}
};

class TestPDLResultList : public PDLResultList {
public:
  TestPDLResultList(unsigned maxNumResults) : PDLResultList(maxNumResults) {}
  /// Return the list of PDL results.
  MutableArrayRef<PDLValue> getResults() { return results; }
};

class BuiltinTest : public ::testing::Test {
public:
  MLIRContext ctx;
  TestPatternRewriter rewriter{&ctx};
};

TEST_F(BuiltinTest, addEntryToDictionaryAttr) {
  TestPDLResultList results(1);

  auto dictAttr = rewriter.getDictionaryAttr({});
  EXPECT_TRUE(succeeded(builtin::addEntryToDictionaryAttr(
      rewriter, results,
      {dictAttr, rewriter.getStringAttr("testAttr"),
       rewriter.getI16IntegerAttr(0)})));
  ASSERT_TRUE(results.getResults().size() == 1);
  mlir::Attribute updated = results.getResults().front().cast<Attribute>();
  EXPECT_TRUE(cast<DictionaryAttr>(updated).contains("testAttr"));

  results = TestPDLResultList(1);
  EXPECT_TRUE(succeeded(builtin::addEntryToDictionaryAttr(
      rewriter, results,
      {updated, rewriter.getStringAttr("testAttr2"),
       rewriter.getI16IntegerAttr(0)})));
  ASSERT_TRUE(results.getResults().size() == 1);
  mlir::Attribute second = results.getResults().front().cast<Attribute>();

  EXPECT_TRUE(cast<DictionaryAttr>(second).contains("testAttr"));
  EXPECT_TRUE(cast<DictionaryAttr>(second).contains("testAttr2"));
}

TEST_F(BuiltinTest, addElemToArrayAttr) {
  TestPDLResultList results(1);

  auto dict = rewriter.getDictionaryAttr(
      rewriter.getNamedAttr("key", rewriter.getStringAttr("value")));
  rewriter.getArrayAttr({});

  auto arrAttr = rewriter.getArrayAttr({});
  EXPECT_TRUE(succeeded(
      builtin::addElemToArrayAttr(rewriter, results, {arrAttr, dict})));
  mlir::Attribute updatedArrAttr =
      results.getResults().front().cast<Attribute>();

  auto dictInsideArrAttr =
      cast<DictionaryAttr>(*cast<ArrayAttr>(updatedArrAttr).begin());
  EXPECT_EQ(dictInsideArrAttr, dict);
}

TEST_F(BuiltinTest, mul) {
  auto twoI8 = rewriter.getI8IntegerAttr(2);
  auto twoI16 = rewriter.getI16IntegerAttr(2);

  auto largestI8 = rewriter.getI8IntegerAttr(-1);

  // signless integer: overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(twoI8.getType().isSignlessInteger());

    EXPECT_TRUE(builtin::mul(rewriter, results, {twoI8, largestI8}).failed());
  }

  // signless integer: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::mul(rewriter, results, {twoI8, twoI8}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getSExtValue(),
        4);
  }

  IntegerType UInt8 = rewriter.getIntegerType(8, false);
  auto twoUInt8 = rewriter.getIntegerAttr(UInt8, APInt(8, 2, false));
  auto largestUInt8 = rewriter.getIntegerAttr(UInt8, APInt(8, 255, false));

  // unsigned integer: overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(
        builtin::mul(rewriter, results, {twoUInt8, largestUInt8}).failed());
  }

  IntegerType SInt8 = rewriter.getIntegerType(8, true);
  auto twoSInt8 = rewriter.getIntegerAttr(SInt8, APInt(8, 2, true));
  auto largestSInt8 = rewriter.getIntegerAttr(SInt8, APInt(8, 127, true));

  // signed integer: overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(
        builtin::mul(rewriter, results, {twoSInt8, largestSInt8}).failed());
  }

  // integer types mismatch
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::mul(rewriter, results, {twoI8, twoI16}).failed());
  }

  auto twoF16 = rewriter.getF16FloatAttr(2.0);
  auto maxValF16 = rewriter.getF16FloatAttr(
      llvm::APFloat::getLargest(llvm::APFloat::IEEEhalf()).convertToFloat());

  // float: overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::mul(rewriter, results, {twoF16, maxValF16}).failed());
  }

  // float: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::mul(rewriter, results, {twoF16, twoF16}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<FloatAttr>(result.cast<Attribute>()).getValue().convertToFloat(),
        4.0);
  }
}

TEST_F(BuiltinTest, div) {
  auto sixI8 = rewriter.getI8IntegerAttr(6);
  auto twoI8 = rewriter.getI8IntegerAttr(2);
  auto zeroI8 = rewriter.getI8IntegerAttr(0);

  // signless integer: division by zero
  {
    TestPDLResultList results(1);
    EXPECT_DEATH(
        static_cast<void>(builtin::div(rewriter, results, {twoI8, zeroI8})),
        "Divide by zero?");
  }

  // signless integer: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::div(rewriter, results, {sixI8, twoI8}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getSExtValue(),
        3);
  }

  IntegerType UInt8 = rewriter.getIntegerType(8, false);
  auto oneUInt8 = rewriter.getIntegerAttr(UInt8, APInt(8, 1, false));
  auto largestUInt8 = rewriter.getIntegerAttr(UInt8, APInt(8, 255, false));
  auto zeroUInt8 = rewriter.getIntegerAttr(UInt8, APInt(8, 0, false));

  // unsigned integer: division by zero
  {
    TestPDLResultList results(1);
    EXPECT_DEATH(static_cast<void>(
                     builtin::div(rewriter, results, {oneUInt8, zeroUInt8})),
                 "Divide by zero?");
  }

  // unsigned integer: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(
        builtin::div(rewriter, results, {largestUInt8, oneUInt8}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getZExtValue(),
        (uint64_t)255);
  }

  IntegerType SInt8 = rewriter.getIntegerType(8, true);
  auto smallestSInt8 = rewriter.getIntegerAttr(SInt8, APInt(8, -128, true));
  auto minusOneSInt8 = rewriter.getIntegerAttr(SInt8, APInt(8, -1, true));
  auto twoSInt8 = rewriter.getIntegerAttr(SInt8, APInt(8, 2, true));

  // signed integer: overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::div(rewriter, results, {smallestSInt8, minusOneSInt8})
                    .failed());
  }

  // signed integer: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(
        builtin::div(rewriter, results, {twoSInt8, minusOneSInt8}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getSExtValue(),
        -2);
  }

  // signed integer: division by zero
  auto zeroSInt8 = rewriter.getIntegerAttr(SInt8, APInt(8, 0, true));
  {
    TestPDLResultList results(1);
    EXPECT_DEATH(static_cast<void>(
                     builtin::div(rewriter, results, {twoSInt8, zeroSInt8})),
                 "Divide by zero?");
  }

  auto BF16Type = rewriter.getBF16Type();
  auto oneBF16 = rewriter.getFloatAttr(BF16Type, 1.0);
  auto nineBF16 = rewriter.getFloatAttr(BF16Type, 9.0);

  // float: inexact result
  // return success(), but warning is emitted.
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(
        builtin::div(rewriter, results, {oneBF16, nineBF16}).succeeded());
  }

  auto twoF16 = rewriter.getF16FloatAttr(2.0);
  auto zeroF16 = rewriter.getF16FloatAttr(0.0);
  auto negzeroF16 = rewriter.getF16FloatAttr(-0.0);

  // float: division by zero
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::div(rewriter, results, {twoF16, zeroF16}).failed());
  }

  // float: division by negative zero
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::div(rewriter, results, {twoF16, negzeroF16}).failed());
  }

  // float: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::div(rewriter, results, {twoF16, twoF16}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<FloatAttr>(result.cast<Attribute>()).getValue().convertToFloat(),
        1.0);
  }

  // type mismatch
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::div(rewriter, results, {twoF16, twoI8}).failed());
  }
}

TEST_F(BuiltinTest, mod) {
  auto eightI8 = rewriter.getI8IntegerAttr(8);
  auto threeI8 = rewriter.getI8IntegerAttr(3);
  auto zeroI8 = rewriter.getI8IntegerAttr(0);

  // signless integer: remainder by zero
  {
    TestPDLResultList results(1);
    EXPECT_DEATH(
        static_cast<void>(builtin::mod(rewriter, results, {eightI8, zeroI8})),
        "Remainder by zero?");
  }

  // signless integer: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(
        builtin::mod(rewriter, results, {eightI8, threeI8}).succeeded());
    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getSExtValue(),
        2);
  }

  IntegerType UInt8 = rewriter.getIntegerType(8, false);
  auto oneUInt8 = rewriter.getIntegerAttr(UInt8, APInt(8, 1, false));
  auto largestUInt8 = rewriter.getIntegerAttr(UInt8, APInt(8, 255, false));
  auto zeroUInt8 = rewriter.getIntegerAttr(UInt8, APInt(8, 0, false));

  // unsigned integer: remainder by zero
  {
    TestPDLResultList results(1);
    EXPECT_DEATH(static_cast<void>(
                     builtin::mod(rewriter, results, {oneUInt8, zeroUInt8})),
                 "Remainder by zero?");
  }

  // unsigned integer: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(
        builtin::mod(rewriter, results, {largestUInt8, oneUInt8}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getZExtValue(),
        (uint64_t)0);
  }

  IntegerType SInt8 = rewriter.getIntegerType(8, true);
  auto minusTenSInt8 = rewriter.getIntegerAttr(SInt8, APInt(8, -10, true));
  auto threeSInt8 = rewriter.getIntegerAttr(SInt8, APInt(8, 3, true));
  auto zeroSInt8 = rewriter.getIntegerAttr(SInt8, APInt(8, 0, true));

  // signed integer: remainder by zero
  {
    TestPDLResultList results(1);
    EXPECT_DEATH(static_cast<void>(
                     builtin::mod(rewriter, results, {threeSInt8, zeroSInt8})),
                 "Remainder by zero?");
  }

  // signed integer: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::mod(rewriter, results, {minusTenSInt8, threeSInt8})
                    .succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getSExtValue(),
        -1);
  }

  auto twoF16 = rewriter.getF16FloatAttr(2.0);
  auto zeroF16 = rewriter.getF16FloatAttr(0.0);
  auto negzeroF16 = rewriter.getF16FloatAttr(-0.0);

  // float: remainder by zero
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::mod(rewriter, results, {twoF16, zeroF16}).failed());
  }

  // float: remainder by negative zero
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::mod(rewriter, results, {twoF16, negzeroF16}).failed());
  }

  // float: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::mod(rewriter, results, {twoF16, twoF16}).succeeded());
    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<FloatAttr>(result.cast<Attribute>()).getValue().convertToFloat(),
        0.0);
  }

  // type mismatch
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::mod(rewriter, results, {twoF16, eightI8}).failed());
  }
}

TEST_F(BuiltinTest, add) {
  auto oneI16 = rewriter.getI16IntegerAttr(1);
  auto oneI32 = rewriter.getI32IntegerAttr(1);
  auto oneI8 = rewriter.getI8IntegerAttr(1);
  auto largestI8 = rewriter.getI8IntegerAttr(-1);

  // signless integer: overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(oneI8.getType().isSignlessInteger());
    EXPECT_TRUE(builtin::add(rewriter, results, {oneI8, largestI8}).failed());
  }

  // signless integer: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::add(rewriter, results, {oneI16, oneI16}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getSExtValue(),
        2);
  }

  IntegerType UInt8 = rewriter.getIntegerType(8, false);
  auto oneUInt8 = rewriter.getIntegerAttr(UInt8, APInt(8, 1, false));
  auto largestUInt8 = rewriter.getIntegerAttr(UInt8, APInt(8, 255, false));

  // unsigned integer: overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(
        builtin::add(rewriter, results, {oneUInt8, largestUInt8}).failed());
  }

  IntegerType SInt8 = rewriter.getIntegerType(8, true);
  auto oneSInt8 = rewriter.getIntegerAttr(SInt8, APInt(8, 1, true));
  auto largestSInt8 = rewriter.getIntegerAttr(SInt8, APInt(8, 127, true));

  // signed integer: overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(
        builtin::add(rewriter, results, {oneSInt8, largestSInt8}).failed());
  }

  // integer types mismatch
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::add(rewriter, results, {oneI16, oneI32}).failed());
  }

  auto oneF32 = rewriter.getF32FloatAttr(1.0);
  auto zeroF32 = rewriter.getF32FloatAttr(0.0);
  auto negzeroF32 = rewriter.getF32FloatAttr(-0.0);
  auto zeroF64 = rewriter.getF64FloatAttr(0.0);
  auto overflowF16 = rewriter.getF16FloatAttr(32768);

  // float: overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(
        builtin::add(rewriter, results, {overflowF16, overflowF16}).failed());
  }

  // float: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::add(rewriter, results, {oneF32, oneF32}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<FloatAttr>(result.cast<Attribute>()).getValue().convertToFloat(),
        2.0);
  }

  // float: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(
        builtin::add(rewriter, results, {zeroF32, negzeroF32}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<FloatAttr>(result.cast<Attribute>()).getValue().convertToFloat(),
        0.0);
  }

  // float types mismatch
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::add(rewriter, results, {zeroF32, zeroF64}).failed());
  }
}

TEST_F(BuiltinTest, sub) {
  auto oneI16 = rewriter.getI16IntegerAttr(1);
  auto oneI32 = rewriter.getI32IntegerAttr(1);
  auto oneI8 = rewriter.getI8IntegerAttr(1);
  auto largestI8 = rewriter.getI8IntegerAttr(-1);

  // signless integer: overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(oneI8.getType().isSignlessInteger());
    EXPECT_TRUE(builtin::sub(rewriter, results, {oneI8, largestI8}).failed());
  }

  // signless integer: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::sub(rewriter, results, {oneI16, oneI16}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getSExtValue(),
        0);
  }

  IntegerType UInt8 = rewriter.getIntegerType(8, false);
  auto oneUInt8 = rewriter.getIntegerAttr(UInt8, APInt(8, 1, false));
  auto largestUInt8 = rewriter.getIntegerAttr(UInt8, APInt(8, 255, false));

  // unsigned integer: overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(
        builtin::sub(rewriter, results, {oneUInt8, largestUInt8}).failed());
  }

  IntegerType SInt8 = rewriter.getIntegerType(8, true);
  auto minusOneSInt8 = rewriter.getIntegerAttr(SInt8, APInt(8, -1, true));
  auto largestSInt8 = rewriter.getIntegerAttr(SInt8, APInt(8, 127, true));

  // signed integer: overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::sub(rewriter, results, {largestSInt8, minusOneSInt8})
                    .failed());
  }

  // integer types mismatch
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::sub(rewriter, results, {oneI16, oneI32}).failed());
  }

  auto oneF16 = rewriter.getF16FloatAttr(100.0);
  auto oneF32 = rewriter.getF32FloatAttr(1.0);
  auto zeroF32 = rewriter.getF32FloatAttr(0.0);
  auto negzeroF32 = rewriter.getF32FloatAttr(-0.0);
  auto zeroF64 = rewriter.getF64FloatAttr(0.0);
  auto minValF16 = rewriter.getF16FloatAttr(-65504);

  // float: overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::sub(rewriter, results, {oneF16, minValF16}).failed());
  }

  // float: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::sub(rewriter, results, {oneF32, oneF32}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<FloatAttr>(result.cast<Attribute>()).getValue().convertToFloat(),
        0.0);
  }

  // float: correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(
        builtin::sub(rewriter, results, {zeroF32, negzeroF32}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<FloatAttr>(result.cast<Attribute>()).getValue().convertToFloat(),
        0.0);
  }

  // float types mismatch
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::sub(rewriter, results, {zeroF32, zeroF64}).failed());
  }
}

TEST_F(BuiltinTest, log2) {
  auto twoI32 = rewriter.getI32IntegerAttr(2);

  // check correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::log2(rewriter, results, {twoI32}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getSExtValue(),
        1);
  }

  auto zeroI32 = rewriter.getI32IntegerAttr(0);
  {
    TestPDLResultList results(1);
    EXPECT_FALSE(builtin::log2(rewriter, results, {zeroI32}).succeeded());
  }

  auto minusOneI32 = rewriter.getI32IntegerAttr(-1);
  {
    TestPDLResultList results(1);
    EXPECT_FALSE(builtin::log2(rewriter, results, {minusOneI32}).succeeded());
  }

  auto fourF16 = rewriter.getF16FloatAttr(4.0);

  // check correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::log2(rewriter, results, {fourF16}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<FloatAttr>(result.cast<Attribute>()).getValue().convertToFloat(),
        2.0);
  }

  auto threeF16 = rewriter.getF16FloatAttr(3.0);

  // check correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::log2(rewriter, results, {threeF16}).succeeded());

    PDLValue result = results.getResults()[0];
    float resultVal =
        cast<FloatAttr>(result.cast<Attribute>()).getValue().convertToFloat();
    EXPECT_TRUE(resultVal > 1.58 && resultVal < 1.59);
  }
}

TEST_F(BuiltinTest, exp2) {
  auto oneInt8 = rewriter.getI8IntegerAttr(1);

  // Check correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::exp2(rewriter, results, {oneInt8}).succeeded());
    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getSExtValue(),
        2);
  }

  auto eightInt8 = rewriter.getI8IntegerAttr(8);
  // Check overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::exp2(rewriter, results, {eightInt8}).failed());
  }

  IntegerType SInt8 = rewriter.getIntegerType(8, true);
  auto oneSInt8 = rewriter.getIntegerAttr(SInt8, APInt(8, 1, true));
  auto sevenSInt8 = rewriter.getIntegerAttr(SInt8, APInt(8, 7, true));

  // Check correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::exp2(rewriter, results, {oneSInt8}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getSExtValue(),
        2);
  }

  // Check signed integer overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::exp2(rewriter, results, {sevenSInt8}).failed());
  }

  IntegerType UInt8 = rewriter.getIntegerType(8, false);
  auto oneUInt8 = rewriter.getIntegerAttr(UInt8, APInt(8, 1, false));
  auto eightUInt8 = rewriter.getIntegerAttr(UInt8, APInt(8, 8, false));

  // Check unsigned integer correctness
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::exp2(rewriter, results, {oneUInt8}).succeeded());

    PDLValue result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getZExtValue(),
        (uint64_t)2);
  }

  // unsigned integer: overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::exp2(rewriter, results, {eightUInt8}).failed());
  }

  auto hundredFortyF32 = rewriter.getF32FloatAttr(140.0);

  // Float: overflow
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::exp2(rewriter, results, {hundredFortyF32}).failed());
  }

  // Float: underflow
  auto minusHundredFiftyF32 = rewriter.getF32FloatAttr(-150.0);
  {
    TestPDLResultList results(1);
    EXPECT_TRUE(
        builtin::exp2(rewriter, results, {minusHundredFiftyF32}).failed());
  }
}

TEST_F(BuiltinTest, abs) {
  // signed integer overflow
  {
    auto SI8Type = rewriter.getIntegerType(8, true);
    auto value = rewriter.getIntegerAttr(SI8Type, -128);
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::abs(rewriter, results, {value}).failed());
  }

  // signed integer correctness
  {
    auto value = rewriter.getSI32IntegerAttr(-1);
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::abs(rewriter, results, {value}).succeeded());
    auto result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getSExtValue(),
        1);
  }

  // unsigned integer
  {
    auto value = rewriter.getUI32IntegerAttr(1);
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::abs(rewriter, results, {value}).succeeded());
    auto result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getZExtValue(),
        (uint64_t)1);
  }

  // signless integer
  {
    auto value = rewriter.getI8IntegerAttr(-7);
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::abs(rewriter, results, {value}).succeeded());
    auto result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getSExtValue(),
        7);
  }

  // signless integer: edge case -128
  // Overflow should not be checked
  // otherwise the purpose of signless integer is meaningless
  {
    auto value = rewriter.getI8IntegerAttr(-128);
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::abs(rewriter, results, {value}).succeeded());
    auto result = results.getResults()[0];
    EXPECT_EQ(
        cast<IntegerAttr>(result.cast<Attribute>()).getValue().getSExtValue(),
        -128);
  }

  // float
  {
    auto value = rewriter.getF32FloatAttr(-1.0);
    TestPDLResultList results(1);
    EXPECT_TRUE(builtin::abs(rewriter, results, {value}).succeeded());
    auto result = results.getResults()[0];
    EXPECT_EQ(
        cast<FloatAttr>(result.cast<Attribute>()).getValue().convertToFloat(),
        1.0);
  }
}

TEST_F(BuiltinTest, equals) {
  auto onei16 = rewriter.getI16IntegerAttr(1);
  auto onei32 = rewriter.getI32IntegerAttr(1);
  auto zeroi32 = rewriter.getI32IntegerAttr(0);

  EXPECT_TRUE(builtin::equals(rewriter, onei16, onei16).succeeded());
  EXPECT_TRUE(builtin::equals(rewriter, onei16, onei32).failed());
  EXPECT_TRUE(builtin::equals(rewriter, zeroi32, onei32).failed());

  auto onef32 = rewriter.getF32FloatAttr(1.0);
  auto zerof32 = rewriter.getF32FloatAttr(0.0);
  auto negzerof32 = rewriter.getF32FloatAttr(-0.0);
  auto zerof64 = rewriter.getF64FloatAttr(0.0);

  EXPECT_TRUE(builtin::equals(rewriter, onef32, onef32).succeeded());
  EXPECT_TRUE(builtin::equals(rewriter, onef32, zerof32).failed());
  EXPECT_TRUE(builtin::equals(rewriter, negzerof32, zerof32).succeeded());
  EXPECT_TRUE(builtin::equals(rewriter, zerof32, zerof64).failed());
}
} // namespace
