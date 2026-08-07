//===---AIE2TargetTransformInfo.cpp - AIEngine V2 specific TTI -----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE2TargetTransformInfo.h"
#include "Utils/AIEIRUtils.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/IR/IntrinsicsAIE2.h"
#include "llvm/Transforms/InstCombine/InstCombiner.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"

using namespace llvm;

#define DEBUG_TYPE "aie2tti"

bool AIE2TTICommon::isAllowedInZOL(Instruction &I) const {
  // FMul is legalized to a VMUL for bfloat16 in other targets
  if (I.getOpcode() == Instruction::FMul) {
    return false;
  }
  return AIETTICommon::isAllowedInZOL(I);
}

std::optional<Instruction *>
AIE2TTIImpl::instCombineIntrinsic(InstCombiner &IC, IntrinsicInst &II) const {
  Intrinsic::ID IID = II.getIntrinsicID();
  switch (IID) {
  default:
    break;
  case Intrinsic::aie2_vbroadcast8_I512:
    return AIEIRUtils::instCombineDemandedBits(IC, II, 8);
  case Intrinsic::aie2_vbroadcast16_I512:
    return AIEIRUtils::instCombineDemandedBits(IC, II, 16);
  }
  return std::nullopt;
}

void AIE2TTIImpl::getUnrollingPreferences(
    Loop *L, ScalarEvolution &SE, TTI::UnrollingPreferences &UP,
    OptimizationRemarkEmitter *ORE) const {
  UP.Partial = UP.Runtime = true;
  BaseT::getUnrollingPreferences(L, SE, UP, ORE);
  Common.adjustUnrollingPreferences(L, SE, UP, ORE);
}

bool AIE2TTIImpl::isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                           AssumptionCache &AC,
                                           TargetLibraryInfo *LibInfo,
                                           HardwareLoopInfo &HWLoopInfo) const {
  return Common.isHardwareLoopProfitable(L, SE, AC, LibInfo, HWLoopInfo);
}

bool AIE2TTIImpl::isProfitableOuterLSR(const Loop &L) const {
  return Common.isProfitableOuterLSR(L);
}

InstructionCost AIE2TTIImpl::getMemoryOpCost(unsigned Opcode, Type *Src,
                                             Align Alignment,
                                             unsigned AddressSpace,
                                             TTI::TargetCostKind CostKind,
                                             TTI::OperandValueInfo OpInfo,
                                             const Instruction *I) const {
  // Try AIE-specific cost model first
  InstructionCost Cost =
      Common.getMemoryOpCost(Opcode, Src, Alignment, AddressSpace, DL);

  // If AIE-specific cost model doesn't handle it, use base implementation
  if (!Cost.isValid())
    return BaseT::getMemoryOpCost(Opcode, Src, Alignment, AddressSpace,
                                  CostKind, OpInfo, I);

  return Cost;
}
