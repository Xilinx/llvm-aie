//===---AIE2PTargetTransformInfo.cpp - AIEngine V2P specific TTI ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE2PTargetTransformInfo.h"
#include "Utils/AIEIRUtils.h"
#include "llvm/IR/IntrinsicsAIE2P.h"
#include "llvm/Transforms/InstCombine/InstCombiner.h"

using namespace llvm;

#define DEBUG_TYPE "aie2ptti"

bool AIE2PTTICommon::isVectorExtractIntrinsicID(Intrinsic::ID ID) const {
  return ID == Intrinsic::aie2p_inv || ID == Intrinsic::aie2p_invsqrt ||
         ID == Intrinsic::aie2p_put_ms;
}

bool AIE2PTTICommon::isGetSSIntrinsicID(Intrinsic::ID ID) const {
  return ID == Intrinsic::aie2p_get_ss;
}

void AIE2PTTIImpl::getUnrollingPreferences(Loop *L, ScalarEvolution &SE,
                                           TTI::UnrollingPreferences &UP,
                                           OptimizationRemarkEmitter *ORE) const {
  BaseT::getUnrollingPreferences(L, SE, UP, ORE);
  Common.adjustUnrollingPreferences(L, SE, UP, ORE);
  Common.applyLoopIdiomUnrolling(L, UP);
}

bool AIE2PTTIImpl::isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                            AssumptionCache &AC,
                                            TargetLibraryInfo *LibInfo,
                                            HardwareLoopInfo &HWLoopInfo) const {
  return Common.isHardwareLoopProfitable(L, SE, AC, LibInfo, HWLoopInfo);
}

bool AIE2PTTIImpl::isProfitableOuterLSR(const Loop &L) const {
  return Common.isProfitableOuterLSR(L);
}

InstructionCost AIE2PTTIImpl::getMemoryOpCost(unsigned Opcode, Type *Src,
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

std::optional<Instruction *>
AIE2PTTIImpl::instCombineIntrinsic(InstCombiner &IC, IntrinsicInst &II) const {
  Intrinsic::ID IID = II.getIntrinsicID();
  switch (IID) {
  default:
    break;
  case Intrinsic::aie2p_vsel16:
    if (AIEIRUtils::isUpperPartOfResultDiscarded(II))
      return AIEIRUtils::instCombineDemandedBits(IC, II, 16, 2);
    break;
  case Intrinsic::aie2p_vsel32:
    return AIEIRUtils::instCombineDemandedBits(IC, II, 16, 2);
  }
  return std::nullopt;
}
