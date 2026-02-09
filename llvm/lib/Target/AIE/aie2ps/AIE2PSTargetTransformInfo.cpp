//===---AIE2PSTargetTransformInfo.cpp - AIEngine 2PS specific TTI ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE2PSTargetTransformInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"

using namespace llvm;

#define DEBUG_TYPE "aie2pstti"

bool AIE2PSTTIImpl::isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                             AssumptionCache &AC,
                                             TargetLibraryInfo *LibInfo,
                                             HardwareLoopInfo &HWLoopInfo) {
  return Common.isHardwareLoopProfitable(L, SE, AC, LibInfo, HWLoopInfo);
}

bool AIE2PSTTIImpl::isProfitableOuterLSR(const Loop &L) const {
  return Common.isProfitableOuterLSR(L);
}

InstructionCost AIE2PSTTIImpl::getMemoryOpCost(unsigned Opcode, Type *Src,
                                               Align Alignment,
                                               unsigned AddressSpace,
                                               TTI::TargetCostKind CostKind,
                                               TTI::OperandValueInfo OpInfo,
                                               const Instruction *I) {
  // Try AIE-specific cost model first
  InstructionCost Cost =
      Common.getMemoryOpCost(Opcode, Src, Alignment, AddressSpace, DL);

  // If AIE-specific cost model doesn't handle it, use base implementation
  if (!Cost.isValid())
    return BaseT::getMemoryOpCost(Opcode, Src, Alignment, AddressSpace,
                                  CostKind, OpInfo, I);

  return Cost;
}
