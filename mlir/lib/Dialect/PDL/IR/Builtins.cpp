#include <cassert>
#include <cstdint>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/APSInt.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/TypeSwitch.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/ErrorHandling.h>
#include <mlir/Dialect/PDL/IR/Builtins.h>
#include <mlir/IR/Attributes.h>
#include <mlir/IR/BuiltinAttributes.h>
#include <mlir/IR/Location.h>
#include <mlir/IR/PatternMatch.h>
#include <mlir/Support/LogicalResult.h>

using namespace mlir;

namespace mlir::pdl {
namespace builtin {

LogicalResult addEntryToDictionaryAttr(PatternRewriter &rewriter,
                                       PDLResultList &results,
                                       ArrayRef<PDLValue> args) {
  auto dictAttr = cast<DictionaryAttr>(args[0].cast<Attribute>());
  auto name = cast<StringAttr>(args[1].cast<Attribute>());
  auto attrEntry = args[2].cast<Attribute>();

  std::vector<NamedAttribute> values = dictAttr.getValue().vec();

  // Remove entry if it exists in the dictionary.
  llvm::erase_if(values, [&](NamedAttribute &namedAttr) {
    return namedAttr.getName() == name.getValue();
  });

  values.push_back(rewriter.getNamedAttr(name, attrEntry));
  results.push_back(rewriter.getDictionaryAttr(values));
  return success();
}

LogicalResult addElemToArrayAttr(PatternRewriter &rewriter,
                                 PDLResultList &results,
                                 ArrayRef<PDLValue> args) {

  assert(args.size() == 2 &&
         "Expected two arguments, one ArrayAttr and one Attr");
  auto arrayAttr = cast<ArrayAttr>(args[0].cast<Attribute>());
  auto attrElement = args[1].cast<Attribute>();
  llvm::SmallVector<Attribute> values(arrayAttr.getValue());
  values.push_back(attrElement);

  results.push_back(rewriter.getArrayAttr(values));
  return success();
}

template <UnaryOpKind T>
LogicalResult static unaryOp(PatternRewriter &rewriter, PDLResultList &results,
                             ArrayRef<PDLValue> args) {
  assert(args.size() == 1 && "Expected one operand for unary operation");
  auto operandAttr = args[0].cast<Attribute>();

  if (auto operandIntAttr = dyn_cast_or_null<IntegerAttr>(operandAttr)) {
    auto integerType = cast<IntegerType>(operandIntAttr.getType());
    auto bitWidth = integerType.getIntOrFloatBitWidth();

    if constexpr (T == UnaryOpKind::exp2) {
      uint64_t resultVal =
          integerType.isUnsigned() || integerType.isSignless()
              ? std::pow(2, operandIntAttr.getValue().getZExtValue())
              : std::pow(2, operandIntAttr.getValue().getSExtValue());

      APInt resultInt(bitWidth, resultVal, integerType.isSigned(),
                      /*implicitTrunc*/ true);

      bool isOverflow = integerType.isSigned()
                            ? resultInt.slt(operandIntAttr.getValue())
                            : resultInt.ult(operandIntAttr.getValue());

      if (isOverflow)
        return failure();

      results.push_back(rewriter.getIntegerAttr(integerType, resultInt));
    } else if constexpr (T == UnaryOpKind::log2) {
      auto getIntegerAsAttr = [&](const APSInt &value) -> Attribute {
        // Only calculate values for exact log2.
        int32_t log2Value = value.exactLogBase2();
        if (log2Value < 0)
          return Attribute();
        return rewriter.getIntegerAttr(
            integerType,
            APSInt(APInt(bitWidth, log2Value), integerType.isUnsigned()));
      };
      // for log2 we treat signless integer as signed
      Attribute log2IntegerAttr;
      if (integerType.isSignless())
        log2IntegerAttr =
            getIntegerAsAttr(APSInt(operandIntAttr.getValue(), false));
      else
        log2IntegerAttr = getIntegerAsAttr(operandIntAttr.getAPSInt());
      // Return failure if log2 value isn't exact.
      if (!log2IntegerAttr)
        return failure();
      results.push_back(log2IntegerAttr);
    } else if constexpr (T == UnaryOpKind::abs) {
      if (integerType.isSigned()) {
        // check overflow
        if (operandIntAttr.getAPSInt() ==
            APSInt::getMinValue(integerType.getIntOrFloatBitWidth(), false))
          return failure();

        results.push_back(rewriter.getIntegerAttr(
            integerType, operandIntAttr.getValue().abs()));
        return success();
      }
      if (integerType.isSignless()) {
        // Overflow should not be checked.
        // Otherwise the purpose of signless integer is meaningless.
        results.push_back(rewriter.getIntegerAttr(
            integerType, operandIntAttr.getValue().abs()));
        return success();
      }
      // If unsigned, do nothing
      results.push_back(operandIntAttr);
      return success();
    } else {
      llvm::llvm_unreachable_internal(
          "encountered an unsupported unary operator");
    }
    return success();
  }

  if (auto operandFloatAttr = dyn_cast_or_null<FloatAttr>(operandAttr)) {
    if constexpr (T == UnaryOpKind::exp2) {
      auto type = operandFloatAttr.getType();

      return TypeSwitch<Type, LogicalResult>(type)
          .template Case<Float64Type>([&results, &rewriter,
                                       &operandFloatAttr](auto floatType) {
            APFloat resultAPFloat(
                std::exp2(operandFloatAttr.getValue().convertToDouble()));

            // check overflow
            if (!resultAPFloat.isNormal())
              return failure();

            results.push_back(rewriter.getFloatAttr(floatType, resultAPFloat));
            return success();
          })
          .template Case<Float32Type, Float16Type, BFloat16Type>(
              [&results, &rewriter, &operandFloatAttr](auto floatType) {
                APFloat resultAPFloat(
                    std::exp2(operandFloatAttr.getValue().convertToFloat()));

                // check overflow and underflow
                // If overflow happens, resultAPFloat is inf
                // If underflow happens, resultAPFloat is 0
                if (!resultAPFloat.isNormal())
                  return failure();

                results.push_back(
                    rewriter.getFloatAttr(floatType, resultAPFloat));
                return success();
              })
          .Default([](Type /*type*/) { return failure(); });
    } else if constexpr (T == UnaryOpKind::log2) {
      results.push_back(rewriter.getFloatAttr(
          operandFloatAttr.getType(),
          std::log2(operandFloatAttr.getValueAsDouble())));
    } else if constexpr (T == UnaryOpKind::abs) {
      auto resultVal = operandFloatAttr.getValue();
      resultVal.clearSign();
      results.push_back(
          rewriter.getFloatAttr(operandFloatAttr.getType(), resultVal));
    } else {
      llvm::llvm_unreachable_internal(
          "encountered an unsupported unary operator");
    }
    return success();
  }
  return failure();
}

template <BinaryOpKind T>
LogicalResult static binaryOp(PatternRewriter &rewriter, PDLResultList &results,
                              llvm::ArrayRef<PDLValue> args) {
  assert(args.size() == 2 && "Expected two operands for binary operation");
  auto lhsAttr = args[0].cast<Attribute>();
  auto rhsAttr = args[1].cast<Attribute>();

  if (auto lhsIntAttr = dyn_cast_or_null<IntegerAttr>(lhsAttr)) {
    auto rhsIntAttr = dyn_cast_or_null<IntegerAttr>(rhsAttr);
    if (!rhsIntAttr || lhsIntAttr.getType() != rhsIntAttr.getType())
      return failure();

    auto integerType = lhsIntAttr.getType();
    APInt resultAPInt;
    bool isOverflow = false;
    if constexpr (T == BinaryOpKind::add) {
      if (integerType.isSignlessInteger() || integerType.isUnsignedInteger()) {
        resultAPInt =
            lhsIntAttr.getValue().uadd_ov(rhsIntAttr.getValue(), isOverflow);
      } else {
        resultAPInt =
            lhsIntAttr.getValue().sadd_ov(rhsIntAttr.getValue(), isOverflow);
      }
    } else if constexpr (T == BinaryOpKind::sub) {
      if (integerType.isSignlessInteger() || integerType.isUnsignedInteger()) {
        resultAPInt =
            lhsIntAttr.getValue().usub_ov(rhsIntAttr.getValue(), isOverflow);
      } else {
        resultAPInt =
            lhsIntAttr.getValue().ssub_ov(rhsIntAttr.getValue(), isOverflow);
      }
    } else if constexpr (T == BinaryOpKind::mul) {
      if (integerType.isSignlessInteger() || integerType.isUnsignedInteger()) {
        resultAPInt =
            lhsIntAttr.getValue().umul_ov(rhsIntAttr.getValue(), isOverflow);
      } else {
        resultAPInt =
            lhsIntAttr.getValue().smul_ov(rhsIntAttr.getValue(), isOverflow);
      }
    } else if constexpr (T == BinaryOpKind::div) {
      if (integerType.isSignlessInteger() || integerType.isUnsignedInteger()) {
        resultAPInt = lhsIntAttr.getValue().udiv(rhsIntAttr.getValue());
      } else {
        resultAPInt =
            lhsIntAttr.getValue().sdiv_ov(rhsIntAttr.getValue(), isOverflow);
      }
    } else if constexpr (T == BinaryOpKind::mod) {
      if (integerType.isSignlessInteger() || integerType.isUnsignedInteger()) {
        resultAPInt = lhsIntAttr.getValue().urem(rhsIntAttr.getValue());
      } else {
        resultAPInt = lhsIntAttr.getValue().srem(rhsIntAttr.getValue());
      }
    } else {
      llvm::llvm_unreachable_internal(
          "encounter an unsupported binary operator.");
    }

    if (isOverflow)
      return failure();

    results.push_back(rewriter.getIntegerAttr(integerType, resultAPInt));
    return success();
  }

  if (auto lhsFloatAttr = dyn_cast_or_null<FloatAttr>(lhsAttr)) {
    auto rhsFloatAttr = dyn_cast_or_null<FloatAttr>(rhsAttr);
    if (!rhsFloatAttr || lhsFloatAttr.getType() != rhsFloatAttr.getType())
      return failure();

    APFloat lhsVal = lhsFloatAttr.getValue();
    APFloat rhsVal = rhsFloatAttr.getValue();
    APFloat resultVal(lhsVal);
    auto floatType = lhsFloatAttr.getType();

    APFloat::opStatus operationStatus;
    if constexpr (T == BinaryOpKind::add) {
      operationStatus =
          resultVal.add(rhsVal, llvm::APFloatBase::rmNearestTiesToEven);
    } else if constexpr (T == BinaryOpKind::sub) {
      operationStatus =
          resultVal.subtract(rhsVal, llvm::APFloatBase::rmNearestTiesToEven);
    } else if constexpr (T == BinaryOpKind::mul) {
      operationStatus =
          resultVal.multiply(rhsVal, llvm::APFloatBase::rmNearestTiesToEven);
    } else if constexpr (T == BinaryOpKind::div) {
      operationStatus =
          resultVal.divide(rhsVal, llvm::APFloatBase::rmNearestTiesToEven);
    } else if constexpr (T == BinaryOpKind::mod) {
      operationStatus = resultVal.mod(rhsVal);
    } else {
      llvm::llvm_unreachable_internal(
          "encounter an unsupported binary operator.");
    }

    if (operationStatus != APFloat::opOK &&
        operationStatus != APFloat::opInexact) {
      return failure();
    }

    results.push_back(rewriter.getFloatAttr(floatType, resultVal));
    return success();
  }
  return failure();
}

LogicalResult add(mlir::PatternRewriter &rewriter, mlir::PDLResultList &results,
                  llvm::ArrayRef<mlir::PDLValue> args) {
  return binaryOp<BinaryOpKind::add>(rewriter, results, args);
}

LogicalResult sub(mlir::PatternRewriter &rewriter, mlir::PDLResultList &results,
                  llvm::ArrayRef<mlir::PDLValue> args) {
  return binaryOp<BinaryOpKind::sub>(rewriter, results, args);
}

LogicalResult mul(PatternRewriter &rewriter, PDLResultList &results,
                  llvm::ArrayRef<PDLValue> args) {
  return binaryOp<BinaryOpKind::mul>(rewriter, results, args);
}

LogicalResult div(PatternRewriter &rewriter, PDLResultList &results,
                  llvm::ArrayRef<PDLValue> args) {
  return binaryOp<BinaryOpKind::div>(rewriter, results, args);
}

LogicalResult mod(PatternRewriter &rewriter, PDLResultList &results,
                  ArrayRef<PDLValue> args) {
  return binaryOp<BinaryOpKind::mod>(rewriter, results, args);
}

LogicalResult exp2(PatternRewriter &rewriter, PDLResultList &results,
                   llvm::ArrayRef<PDLValue> args) {
  return unaryOp<UnaryOpKind::exp2>(rewriter, results, args);
}

LogicalResult log2(PatternRewriter &rewriter, PDLResultList &results,
                   llvm::ArrayRef<PDLValue> args) {
  return unaryOp<UnaryOpKind::log2>(rewriter, results, args);
}
LogicalResult abs(PatternRewriter &rewriter, PDLResultList &results,
                  llvm::ArrayRef<PDLValue> args) {
  return unaryOp<UnaryOpKind::abs>(rewriter, results, args);
}

LogicalResult equals(mlir::PatternRewriter &, mlir::Attribute lhs,
                     mlir::Attribute rhs) {
  // Attribute::operator== considers 0.0f and -0.0f to be not equal.
  // We use APFloat to get the natural comparison result.
  if (auto lhsAttr = dyn_cast_or_null<FloatAttr>(lhs)) {
    auto rhsAttr = dyn_cast_or_null<FloatAttr>(rhs);
    if (!rhsAttr || lhsAttr.getType() != rhsAttr.getType())
      return failure();

    APFloat lhsVal = lhsAttr.getValue();
    APFloat rhsVal = rhsAttr.getValue();
    return success(lhsVal.compare(rhsVal) == llvm::APFloatBase::cmpEqual);
  }
  return success(lhs == rhs);
}
} // namespace builtin

void registerBuiltins(PDLPatternModule &pdlPattern) {
  using namespace builtin;
  // See Parser::defineBuiltins()
  pdlPattern.registerRewriteFunction(
      "__builtin_addEntryToDictionaryAttr_rewrite", addEntryToDictionaryAttr);
  pdlPattern.registerConstraintFunction(
      "__builtin_addEntryToDictionaryAttr_constraint",
      addEntryToDictionaryAttr);

  pdlPattern.registerRewriteFunction("__builtin_addElemToArrayAttrRewriter",
                                     addElemToArrayAttr);
  pdlPattern.registerConstraintFunction(
      "__builtin_addElemToArrayAttrConstraint", addElemToArrayAttr);

  pdlPattern.registerRewriteFunction("__builtin_mulRewrite", mul);
  pdlPattern.registerRewriteFunction("__builtin_divRewrite", div);
  pdlPattern.registerRewriteFunction("__builtin_modRewrite", mod);
  pdlPattern.registerRewriteFunction("__builtin_addRewrite", add);
  pdlPattern.registerRewriteFunction("__builtin_subRewrite", sub);
  pdlPattern.registerRewriteFunction("__builtin_log2Rewrite", log2);
  pdlPattern.registerRewriteFunction("__builtin_exp2Rewrite", exp2);
  pdlPattern.registerRewriteFunction("__builtin_absRewrite", abs);
  pdlPattern.registerConstraintFunction("__builtin_mulConstraint", mul);
  pdlPattern.registerConstraintFunction("__builtin_divConstraint", div);
  pdlPattern.registerConstraintFunction("__builtin_modConstraint", mod);
  pdlPattern.registerConstraintFunction("__builtin_addConstraint", add);
  pdlPattern.registerConstraintFunction("__builtin_subConstraint", sub);
  pdlPattern.registerConstraintFunction("__builtin_log2Constraint", log2);
  pdlPattern.registerConstraintFunction("__builtin_exp2Constraint", exp2);
  pdlPattern.registerConstraintFunction("__builtin_absConstraint", abs);
  pdlPattern.registerConstraintFunction("__builtin_equals", equals);
}
} // namespace mlir::pdl
