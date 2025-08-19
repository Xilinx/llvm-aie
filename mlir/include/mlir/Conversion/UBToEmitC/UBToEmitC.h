//===- UBToEmitC.h - UB to EmitC dialect conversion -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MLIR_CONVERSION_UBTOEMITC_UBTOEMITC_H
#define MLIR_CONVERSION_UBTOEMITC_UBTOEMITC_H

#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"

namespace mlir {
#define GEN_PASS_DECL_CONVERTUBTOEMITC
#include "mlir/Conversion/Passes.h.inc"

namespace ub {
void populateUBToEmitCConversionPatterns(TypeConverter &converter,
                                         RewritePatternSet &patterns,
                                         bool noInitialization);
} // namespace ub
} // namespace mlir

#endif // MLIR_CONVERSION_UBTOEMITC_UBTOEMITC_H
