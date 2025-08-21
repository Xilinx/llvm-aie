//===---------- FunctionOpAssembly.h - Parser for `emitc.func` op ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef MLIR_INCLUDE_MLIR_DIALECT_EMITC_IR_FUNCTIONOPASSEMBLY_H
#define MLIR_INCLUDE_MLIR_DIALECT_EMITC_IR_FUNCTIONOPASSEMBLY_H

#include "mlir/IR/OperationSupport.h"
#include "mlir/Interfaces/FunctionImplementation.h"
#include "mlir/Support/LogicalResult.h"

#include "mlir/IR/Builders.h"

namespace mlir::emitc {

class FuncOp;

ParseResult
parseFunctionSignature(OpAsmParser &parser, bool allowVariadic,
                       SmallVectorImpl<OpAsmParser::Argument> &arguments,
                       bool &isVariadic, SmallVectorImpl<Type> &resultTypes,
                       SmallVectorImpl<DictionaryAttr> &resultAttrs);

ParseResult
parseFunctionOp(OpAsmParser &parser, OperationState &result, bool allowVariadic,
                StringAttr typeAttrName,
                function_interface_impl::FuncTypeBuilder funcTypeBuilder,
                StringAttr argAttrsName, StringAttr resAttrsName);

void printFunctionSignature(OpAsmPrinter &p, FuncOp op, ArrayRef<Type> argTypes,
                            bool isVariadic, ArrayRef<Type> resultTypes);

void printFunctionOp(OpAsmPrinter &p, FuncOp op, bool isVariadic,
                     StringRef typeAttrName, StringAttr argAttrsName,
                     StringAttr resAttrsName);

} // namespace mlir::emitc

#endif // MLIR_INCLUDE_MLIR_DIALECT_EMITC_IR_FUNCTIONOPASSEMBLY_H
