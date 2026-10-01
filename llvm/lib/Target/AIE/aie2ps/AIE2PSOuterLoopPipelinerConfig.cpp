//===- AIE2PSOuterLoopPipelinerConfig.cpp - AIE2PS OLP policy ------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE2PSOuterLoopPipelinerConfig.h"
#include "llvm/IR/IntrinsicInst.h"

using namespace llvm;

bool AIE2PSOLPTargetConfig::isLeanStage0LoadIntrinsic(
    const Instruction &I) const {
  const auto *II = dyn_cast<IntrinsicInst>(&I);
  if (!II || !II->getCalledFunction())
    return false;
  // A FIFO load returns the loaded vector together with the updated pointer
  // and multidimensional state, so the whole intrinsic has to move to stage 0;
  // there is no separate address computation to leave behind in stage 1.
  return II->getCalledFunction()->getName().starts_with("llvm.aie2ps.fifo.ld");
}
