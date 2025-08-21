//===- EliminateLibm.cpp - Replace Libm by Math.h Inclusion -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements a pass that replaces C++ Libm standard library
// prototypes (e.g., inserted by the MathToLibm pass) by an inclusion of the
// <cmath> header.
//
//===----------------------------------------------------------------------===//

#include "mlir/Dialect/EmitC/IR/EmitC.h"
#include "mlir/Dialect/EmitC/Transforms/Passes.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Diagnostics.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/IR/Visitors.h"
#include "mlir/Interfaces/FunctionInterfaces.h"
#include "mlir/Transforms/DialectConversion.h"
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"
#include <optional>

namespace mlir {
#define GEN_PASS_DEF_ELIMINATELIBM
#include "mlir/Dialect/EmitC/Transforms/Passes.h.inc"
} // namespace mlir

using namespace mlir;

namespace {

/// Replace all Libm calls (where callee has `libm` attribute + no definition)
/// by opaque calls
struct OpacifyLibmCall : public OpRewritePattern<emitc::CallOp> {
  using OpRewritePattern<emitc::CallOp>::OpRewritePattern;
  LogicalResult matchAndRewrite(emitc::CallOp callOp,
                                PatternRewriter &rewriter) const override {

    auto *st = SymbolTable::getNearestSymbolTable(callOp);
    auto *calleeSym = SymbolTable::lookupSymbolIn(st, callOp.getCallee());
    assert(isa<FunctionOpInterface>(calleeSym));
    FunctionOpInterface callee = dyn_cast<FunctionOpInterface>(calleeSym);

    if (!(callee->hasAttr("libm") && callee.isDeclaration()))
      return failure();

    auto opaqueCall = rewriter.create<emitc::CallOpaqueOp>(
        callOp->getLoc(), callOp.getResultTypes(), callOp.getCallee(),
        callOp.getArgOperands());

    rewriter.replaceOp(callOp, opaqueCall);
    return success();
  }
};

struct EliminateLibmPass : public impl::EliminateLibmBase<EliminateLibmPass> {
  void runOnOperation() override {
    ModuleOp module = getOperation();
    MLIRContext *context = module->getContext();

    // Find the first math.h inclusion
    SmallVector<emitc::FuncOp> libmPrototypes;
    module.walk([&libmPrototypes](emitc::FuncOp funcOp) {
      if (funcOp->hasAttr("libm") && funcOp.isDeclaration())
        libmPrototypes.push_back(funcOp);
    });

    if (libmPrototypes.empty())
      return;

    // Use a rewrite pattern to turn calls into opaque
    RewritePatternSet opacifyLibmCallPatterns(context);
    opacifyLibmCallPatterns.add<OpacifyLibmCall>(
        opacifyLibmCallPatterns.getContext());

    if (failed(applyPatternsAndFoldGreedily(
            module, std::move(opacifyLibmCallPatterns))))
      return signalPassFailure();

    // Check that none of the prototypes have users
    for (auto &proto : libmPrototypes) {
      auto uses = proto.getSymbolUses(module);
      if (!uses || (!uses->empty())) {
        return signalPassFailure();
      }
    }

    OpBuilder builder(module.getRegion());

    // This will only work if the target language is C++.
    builder.create<emitc::IncludeOp>(builder.getUnknownLoc(), "cmath",
                                     /*"is_standard_include=*/true);

    for (auto &proto : libmPrototypes) {
      proto->erase();
    }
  }
};

} // namespace
