//===- AIE2OuterLoopPointerOptimizerConfig.cpp - AIE2 OLPO policy --------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE2OuterLoopPointerOptimizerConfig.h"
#include "AIEBaseInstrInfo.h"

using namespace llvm;

bool AIE2OLPOTargetConfig::isLegalPointerAddImmediate(int64_t Offset) const {
  // imm10x4: a multiple of 4 in [-2048, 2044].
  return checkSignedImmediateRange<10, 4>(APInt(64, Offset, /*isSigned=*/true));
}
