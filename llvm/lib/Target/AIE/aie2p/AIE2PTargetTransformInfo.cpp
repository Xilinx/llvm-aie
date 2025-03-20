//===---AIE2PTargetTransformInfo.cpp - AIEngine V2P specific TTI ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE2PTargetTransformInfo.h"
#include "Utils/AIELoopUtils.h"

using namespace llvm;

static cl::opt<bool>
    UnrollOnlyLoopsWithPragma("aie2p-unroll-only-pragma-loops",
                              cl::desc("Only unroll loops guarded by pragmas"),
                              cl::init(true), cl::Hidden);

#define DEBUG_TYPE "aie2ptti"

void AIE2PTTIImpl::getUnrollingPreferences(Loop *L, ScalarEvolution &SE,
                                           TTI::UnrollingPreferences &UP,
                                           OptimizationRemarkEmitter *ORE) {

  BaseT::getUnrollingPreferences(L, SE, UP, ORE);
  Common.adjustUnrollingPreferences(L, SE, UP, ORE);

  // If we don't have unroll pragma, block unroll by zeroing the
  // Threshold.
  // We also disable partial because it can bypass the Threshold logic.
  if (UnrollOnlyLoopsWithPragma && !AIELoopUtils::hasUnrollPragma(L)) {
    UP.Threshold = 0;
    UP.Partial = false;
  }
}

bool AIE2PTTIImpl::isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                            AssumptionCache &AC,
                                            TargetLibraryInfo *LibInfo,
                                            HardwareLoopInfo &HWLoopInfo) {
  return Common.isHardwareLoopProfitable(L, SE, AC, LibInfo, HWLoopInfo);
}

bool AIE2PTTIImpl::isProfitableOuterLSR(const Loop &L) const {
  return Common.isProfitableOuterLSR(L);
}
