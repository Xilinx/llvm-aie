//===---AIE2TargetTransformInfo.cpp - AIEngine V2 specific TTI -----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE2TargetTransformInfo.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/IR/IntrinsicsAIE2.h"
#include "llvm/Transforms/InstCombine/InstCombiner.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"

using namespace llvm;

#define DEBUG_TYPE "aie2tti"

static std::optional<Instruction *>
instCombineDemandedBits(InstCombiner &IC, IntrinsicInst &II, unsigned NumBits) {
  KnownBits ScalarKnown(32);
  if (IC.SimplifyDemandedBits(&II, 0, APInt::getLowBitsSet(32, NumBits),
                              ScalarKnown, 0)) {
    return &II;
  }

  return std::nullopt;
}

std::optional<Instruction *>
AIE2TTIImpl::instCombineIntrinsic(InstCombiner &IC, IntrinsicInst &II) const {
  Intrinsic::ID IID = II.getIntrinsicID();
  switch (IID) {
  default:
    break;
  case Intrinsic::aie2_vbroadcast8_I512:
    return instCombineDemandedBits(IC, II, 8);
  case Intrinsic::aie2_vbroadcast16_I512:
    return instCombineDemandedBits(IC, II, 16);
  }
  return std::nullopt;
}

void AIE2TTIImpl::getUnrollingPreferences(Loop *L, ScalarEvolution &SE,
                                          TTI::UnrollingPreferences &UP,
                                          OptimizationRemarkEmitter *ORE) {
  UP.Partial = UP.Runtime = true;
  BaseT::getUnrollingPreferences(L, SE, UP, ORE);
  Common.adjustUnrollingPreferences(L, SE, UP, ORE);
}

bool AIE2TTIImpl::isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                           AssumptionCache &AC,
                                           TargetLibraryInfo *LibInfo,
                                           HardwareLoopInfo &HWLoopInfo) {
  return Common.isHardwareLoopProfitable(L, SE, AC, LibInfo, HWLoopInfo);
}

bool AIE2TTIImpl::isProfitableOuterLSR(const Loop &L) const {
  return Common.isProfitableOuterLSR(L);
}

bool AIE2TTIImpl::addrspacesMayAlias(unsigned AS0, unsigned AS1) const {
  if (AS0 == AS1)
    return true;

  // Tile Memory and Data Memory are disjoint, since we allways annotate Tile
  // Memory access even if another address space is not annotated we can assume
  // that they are disjoint.
  const unsigned TileMemoryAS = static_cast<unsigned>(AIE2::AddressSpaces::TM);
  if (AS0 == TileMemoryAS || AS1 == TileMemoryAS)
    return false;

  const AIEBaseAddrSpaceInfo &ASI = getST()->getAddrSpaceInfo();
  return ASI.addrspacesMayAlias(AS0, AS1);
}
