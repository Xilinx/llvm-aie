//===- AnnotateInputTypes.cpp - Type attribute annotation for func ops ----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements a pass that creates type attributes for func parameters,
// that mirror the actual type. This is useful when the func op input types
// might change.
//
//===----------------------------------------------------------------------===//

#include "mlir/Dialect/Func/Transforms/Passes.h"

#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/Pass/Pass.h"

using namespace mlir;

namespace mlir::func {
#define GEN_PASS_DEF_ANNOTATEFUNCTIONTYPE
#include "mlir/Dialect/Func/Transforms/Passes.h.inc"
} // namespace mlir::func

namespace {
struct AnnotateFunctionTypePass
    : public mlir::func::impl::AnnotateFunctionTypeBase<
          AnnotateFunctionTypePass> {

  void runOnOperation() override {
    func::FuncOp func = getOperation();
    auto inputs = func.getArgumentTypes();
    auto results = func.getResultTypes();

    for (const auto [argNum, type] : llvm::enumerate(inputs)) {
      func.setArgAttr(argNum, "func.orig_type", TypeAttr::get(type));
    }

    for (const auto [resultNum, type] : llvm::enumerate(results)) {
      func.setResultAttr(resultNum, "func.orig_type", TypeAttr::get(type));
    }
  }
};
} // namespace
