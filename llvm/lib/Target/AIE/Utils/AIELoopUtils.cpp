//===- AIELoopUtils.cpp ---------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
#include "AIELoopUtils.h"
#include "llvm/IR/Constants.h"

#define DEBUG_TYPE "aielooputils"

namespace llvm::AIELoopUtils {

std::optional<int64_t> getMinTripCount(const MDNode *LoopID) {

  if (LoopID == nullptr)
    return std::nullopt;

  assert(LoopID->getNumOperands() > 0 && "requires at least one operand");
  assert(LoopID->getOperand(0) == LoopID && "invalid loop");

  int64_t MinTripCount = 0;
  for (unsigned I = 1, E = LoopID->getNumOperands(); I < E; ++I) {
    const MDNode *MD = dyn_cast<MDNode>(LoopID->getOperand(I));

    if (MD == nullptr)
      continue;

    if (const MDString *S = dyn_cast<MDString>(MD->getOperand(0))) {
      if (S->getString() == "llvm.loop.itercount.range") {
        assert((MD->getNumOperands() >= 2 && MD->getNumOperands() <= 3) &&
               "Iteration count hint should have one or two numeric operands.");
        MinTripCount =
            mdconst::extract<ConstantInt>(MD->getOperand(1))->getSExtValue();
        assert(MinTripCount >= 0 && "Range lwb should not be negative.");
        LLVM_DEBUG(dbgs() << "AIELoopUtils: MinTripCount from pragma =  "
                          << MinTripCount << "\n");
        return MinTripCount;
      }
    }
  }
  return std::nullopt;
}

} // namespace llvm::AIELoopUtils
