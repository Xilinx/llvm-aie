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
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"

using namespace llvm;

#define DEBUG_TYPE "aie2ptti"

extern cl::opt<bool> EnableAIEHardwareLoops;
extern cl::opt<bool> AllowAIEZOL;
extern cl::opt<int> MinIterCountHLReject;
extern cl::opt<bool> ForceHLGeneration;

extern cl::opt<bool> EnableAutoUnroll;
extern cl::opt<unsigned> MaxUnrollCount;
extern cl::opt<int> MaxUnrollLoads;
extern cl::opt<unsigned> MaxUnrollCost;
extern cl::opt<unsigned> PreferSwpOverUnroll;

void AIE2PTTIImpl::getUnrollingPreferences(Loop *L, ScalarEvolution &SE,
                                           TTI::UnrollingPreferences &UP,
                                           OptimizationRemarkEmitter *ORE) {
  UP.Partial = UP.Runtime = true;
  BaseT::getUnrollingPreferences(L, SE, UP, ORE);
  UP.Partial &= UP.Runtime &= EnableAutoUnroll;
  UP.MaxCount = MaxUnrollCount;
  UP.FullUnrollMaxCount = 32;
  UP.Threshold = MaxUnrollCost;
  UP.AllowExpensiveTripCount = true;

  if (L->getNumBlocks() == 1) {
    BasicBlock *LoopBlock = L->getHeader();
    if (MaxUnrollLoads >= 0) {
      int NumLoads = count_if(*LoopBlock, [](const Instruction &I) {
        return I.mayReadFromMemory();
      });
      if (NumLoads)
        UP.MaxCount =
            std::min(UP.MaxCount, unsigned(MaxUnrollLoads / NumLoads));
    }
    auto MinIterCount = getMinTripCount(L->getLoopID());
    if (MinIterCount && *MinIterCount >= PreferSwpOverUnroll) {
      UP.Partial = false;
      UP.Runtime = false;
    }
  }
}

bool AIE2PTTIImpl::isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
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

  if (!ForceHLGeneration) {
    std::optional<int64_t> MinTripCount = getMinTripCount(L, &SE);
    if (MinTripCount) {
      // Reject HL for this case.
      if (*MinTripCount <= MinIterCountHLReject) {
        return false;
      }
    } else {
      // We have metadata, but not iteration information.
      return false;
    }
  }

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
  // We always allow nested hardware loops, but only the innermost loop
  // can use actual zero overhead loop instructions
  HWLoopInfo.IsNestingLegal = true;
  if (L->isInnermost() && AllowAIEZOL) {
    LLVM_DEBUG(dbgs() << "AIE Loops: Loop is ZOL candidate\n");
    HWLoopInfo.CounterInReg = false;
  } else {
    LLVM_DEBUG(dbgs() << "AIE Loops: Loop is JNZD candidate\n");
    HWLoopInfo.CounterInReg = true;
  }
  return true;
}
