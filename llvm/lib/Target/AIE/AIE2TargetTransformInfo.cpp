//===---AIE2TargetTransformInfo.cpp - AIEngine V2 specific TTI -----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE2TargetTransformInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"

using namespace llvm;

#define DEBUG_TYPE "aie2tti"

static cl::opt<bool>
    EnableAIEHardwareLoops("enable-aie-hardware-loops",
                           cl::desc("Enable hardware loops on AIE"),
                           cl::init(false), cl::Hidden);

static cl::opt<bool>
    AllowAIEZOL("enable-aie-zero-overhead-loops",
                cl::desc("Enable true zero overhead hardware loops on AIE"),
                cl::init(false), cl::Hidden);

static cl::opt<bool>
    ConsiderLSROuterLoops("aie-lsr-consider-outer", cl::Hidden, cl::init(false),
                          cl::desc("Whether to consider outer loops for LSR"));

bool AIE2TTIImpl::isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                           AssumptionCache &AC,
                                           TargetLibraryInfo *LibInfo,
                                           HardwareLoopInfo &HWLoopInfo) {

  if (!EnableAIEHardwareLoops) {
    LLVM_DEBUG(dbgs() << "AIE Loops: Disabled\n");
    return false;
  }

  if (!SE.hasLoopInvariantBackedgeTakenCount(L)) {
    LLVM_DEBUG(dbgs() << "AIE Loops: No static backedge taken count\n");
    return false;
  }

  const SCEV *BackedgeTakenCount = SE.getBackedgeTakenCount(L);
  assert(!isa<SCEVCouldNotCompute>(BackedgeTakenCount));
  const SCEV *TripCountSCEV = SE.getAddExpr(
      BackedgeTakenCount, SE.getOne(BackedgeTakenCount->getType()));

  // We need to store the trip count in GPR/LC, a 32-bit register.
  if (SE.getUnsignedRangeMax(TripCountSCEV).getBitWidth() > 32) {
    LLVM_DEBUG(dbgs() << "AIE Loops: Trip count does not fit into 32bits\n");
    return false;
  }

  // For now, we'll handle only single BB loops for AIE
  // zero-overhead loop.
  if (L->getNumBlocks() > 1)
    return false;

  // Scan the loop: loops with calls - make it unprofitable
  for (BasicBlock *BB : L->blocks()) {
    for (Instruction &I : *BB) {
      if (isa<CallInst>(I) || isa<InvokeInst>(I)) {
        if (const Function *F = cast<CallBase>(I).getCalledFunction()) {
          if (!isLoweredToCall(F))
            continue;
        }
        return false;
      }
    }
  }
  // We don't want to use ZOL in these cases
  LLVMContext &C = L->getHeader()->getContext();
  HWLoopInfo.CountType = Type::getInt32Ty(C);
  HWLoopInfo.LoopDecrement = ConstantInt::get(HWLoopInfo.CountType, 1);
  // We always allow nested hardware loops, but only the inntermost loop
  // can use actual zero overhead loop instructions
  HWLoopInfo.IsNestingLegal = true;
  if (L->isInnermost() && AllowAIEZOL) {
    LLVM_DEBUG(dbgs() << "AIE Loops: Loop is ZOL candiate\n");
    HWLoopInfo.CounterInReg = false;
  } else {
    LLVM_DEBUG(dbgs() << "AIE Loops: Loop is JNZD candiate\n");
    HWLoopInfo.CounterInReg = true;
  }
  return true;
}

bool AIE2TTIImpl::isProfitableOuterLSR(const Loop &L) const {
  // Down-counting loops are essentially always profitable for AIE.
  // They typically need a single GPR for counting down, while "up-counting"
  // loop need one for the IV, and one for the upper bound.
  return ConsiderLSROuterLoops.getNumOccurrences() > 0 ? ConsiderLSROuterLoops
                                                       : true;
}
