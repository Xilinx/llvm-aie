//===- AIE2PSOuterLoopPointerOptimizerConfig.cpp - AIE2PS OLPO policy ----===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE2PSOuterLoopPointerOptimizerConfig.h"
#include "AIEBaseInstrInfo.h"

using namespace llvm;

bool AIE2PSOLPOTargetConfig::isLegalPointerAddImmediate(int64_t Offset) const {
  // c10s_step64: a multiple of 64 in [-512, 448].
  return checkSignedImmediateRange<4, 64>(APInt(64, Offset, /*isSigned=*/true));
}
