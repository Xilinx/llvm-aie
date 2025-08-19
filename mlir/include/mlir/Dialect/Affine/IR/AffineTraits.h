//===- AffineTraits.h - MLIR Affine Traits --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines traits brought in by the Affine dialect.
//
//===----------------------------------------------------------------------===//
#ifndef AFFINE_TRAITS_H
#define AFFINE_TRAITS_H

#include "mlir/IR/OpDefinition.h"

namespace mlir::OpTrait {

template <typename ConcreteType>
class AffineDim : public TraitBase<ConcreteType, AffineDim> {
public:
  static LogicalResult verifyTrait(Operation *op) { return success(); }
};

} // namespace mlir::OpTrait

#endif // AFFINE_TRAITS_H
