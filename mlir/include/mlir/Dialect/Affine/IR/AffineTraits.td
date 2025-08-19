//===- AffineTraits.td - Affine dialect traits -------------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Defines traits brought in by the MLIR Affine dialect.
//
//===----------------------------------------------------------------------===//
#ifndef AFFINE_TRAITS
#define AFFINE_TRAITS

include "mlir/IR/OpBase.td"

// Trait to declare that an op result is an affine dimension identifier.
// Prevents the result from being seen as a symbol into AffineMaps
// and IntegerSets.
// This is a deviation from upstream to consider linalg.index as
// a dimension rather than a symbol. See this PR:
// https://github.com/Xilinx/llvm-project/pull/537
def AffineDim : NativeOpTrait<"AffineDim">;

#endif // AFFINE_TRAITS
