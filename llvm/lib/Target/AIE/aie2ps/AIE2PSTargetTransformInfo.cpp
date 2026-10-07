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
#include "Utils/AIEIRUtils.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/IntrinsicsAIE2PS.h"
#include "llvm/Transforms/InstCombine/InstCombiner.h"

using namespace llvm;

#define DEBUG_TYPE "aie2pstti"

bool AIE2PSTTICommon::isVectorExtractIntrinsicID(Intrinsic::ID ID) const {
  return ID == Intrinsic::aie2ps_inv || ID == Intrinsic::aie2ps_invsqrt ||
         ID == Intrinsic::aie2ps_put_ms;
}

bool AIE2PSTTICommon::isGetSSIntrinsicID(Intrinsic::ID ID) const {
  return ID == Intrinsic::aie2ps_get_ss;
}

bool AIE2PSTTICommon::isAllowedInZOL(Instruction &I) const {
  // The base class rejects these because they lower to libcalls on other
  // targets; on AIE2PS they lower inline, so they are safe inside a ZOL (a ZOL
  // must not contain a call):
  //   - scalar f32 add/sub -> the vector float adder; neg -> a sign-mask xor.
  //     f32 mul and compares still lower to __mulsf3 / __*sf2 and remain
  //     rejected by the base class.
  //   - scalar div by a constant (negated, for sdiv) power of two -> shifts,
  //     but only through the pre-legalizer combiner's intdiv_combines; the
  //     legalizer turns any division left over into a libcall. That combiner
  //     and HardwareLoops both run only above -O0.
  switch (I.getOpcode()) {
  case Instruction::FAdd:
  case Instruction::FSub:
  case Instruction::FNeg:
    if (I.getType()->isFloatTy())
      return true;
    break;
  case Instruction::SDiv:
  case Instruction::UDiv: {
    const auto *Divisor = dyn_cast<ConstantInt>(I.getOperand(1));
    if (!Divisor || I.getType()->isVectorTy() ||
        I.getType()->getScalarSizeInBits() > 32)
      break;
    const APInt &D = Divisor->getValue();
    if (D.isPowerOf2() ||
        (I.getOpcode() == Instruction::SDiv && D.isNegatedPowerOf2()))
      return true;
    break;
  }
  default:
    break;
  }
  return AIETTICommon::isAllowedInZOL(I);
}

void AIE2PSTTIImpl::getUnrollingPreferences(Loop *L, ScalarEvolution &SE,
                                            TTI::UnrollingPreferences &UP,
                                            OptimizationRemarkEmitter *ORE) const {
  BaseT::getUnrollingPreferences(L, SE, UP, ORE);
  Common.adjustUnrollingPreferences(L, SE, UP, ORE);
  Common.applyLoopIdiomUnrolling(L, UP);
}

bool AIE2PSTTIImpl::isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                             AssumptionCache &AC,
                                             TargetLibraryInfo *LibInfo,
                                             HardwareLoopInfo &HWLoopInfo) const {
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
AIE2PSTTIImpl::instCombineIntrinsic(InstCombiner &IC, IntrinsicInst &II) const {
  Intrinsic::ID IID = II.getIntrinsicID();
  switch (IID) {
  default:
    break;
  case Intrinsic::aie2ps_vsel16:
    if (AIEIRUtils::isUpperPartOfResultDiscarded(II))
      return AIEIRUtils::instCombineDemandedBits(IC, II, 16, 2);
    break;
  case Intrinsic::aie2ps_vsel32:
    return AIEIRUtils::instCombineDemandedBits(IC, II, 16, 2);
  }
  return std::nullopt;
}
