//===---AIEBaseTargetTransformInfo.cpp - AIEngine specific TTI ------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEBaseTargetTransformInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/IR/Instruction.h"

using namespace llvm;

#define DEBUG_TYPE "aietti"

static cl::opt<bool>
    EnableAIEHardwareLoops("enable-aie-hardware-loops",
                           cl::desc("Enable hardware loops on AIE"),
                           cl::init(true), cl::Hidden);

static cl::opt<bool> EnableZOLForLoopsWithoutMinIterCount(
    "enable-aie-zol-without-minitercount",
    cl::desc("Enable zol for loops without minitercount"), cl::init(true),
    cl::Hidden);

static cl::opt<bool>
    AllowAIEZOL("enable-aie-zero-overhead-loops",
                cl::desc("Enable true zero overhead hardware loops on AIE"),
                cl::init(true), cl::Hidden);

static cl::opt<int> MinIterCountHLReject(
    "aie-hardware-loops-minitercount", cl::Hidden, cl::init(1),
    cl::desc("Minimum trip count threshold for HL rejection"));

static cl::opt<bool>
    ForceHLGeneration("aie-force-hl-gen", cl::Hidden, cl::init(false),
                      cl::desc("Force HL generation ignoring metadata info."));

static cl::opt<bool>
    ConsiderLSROuterLoops("aie-lsr-consider-outer", cl::Hidden, cl::init(false),
                          cl::desc("Whether to consider outer loops for LSR"));

static cl::opt<bool>
    EnablePartialUnroll("aie-unroll-partial", cl::Hidden, cl::init(true),
                        cl::desc("Whether to partially unroll loops"));
static cl::opt<bool> EnableRuntimeUnroll(
    "aie-unroll-runtime", cl::Hidden, cl::init(false),
    cl::desc("Whether to unroll loops with runtime checks"));
static cl::opt<unsigned>
    MaxUnrollCount("aie-unroll-max-count", cl::Hidden, cl::init(4),
                   cl::desc("Maximum partial unroll count for loops"));
static cl::opt<int>
    MaxUnrollLoads("aie-unroll-max-loads", cl::Hidden, cl::init(-1),
                   cl::desc("Maximum partial unroll count for loops"));
static cl::opt<unsigned>
    MaxUnrollCost("aie-unroll-max-cost", cl::Hidden, cl::init(200),
                  cl::desc("Maximum partial unroll cost for loops"));
static cl::opt<unsigned> PreferSwpOverUnroll(
    "aie-prefer-swp-over-unroll", cl::Hidden, cl::init(9),
    cl::desc("Aim for pipelining if MinIterCount is at least this value."));

bool AIETTICommon::isLoweredToCall(const Function *F) {
  return !F->isIntrinsic();
}

bool AIETTICommon::isAllowedInZOL(Instruction &I) {
  Type *Ty = I.getType();
  if (Ty->getScalarType()->isDoubleTy()) {
    return false;
  }
  switch (I.getOpcode()) {
  case Instruction::FAdd:
  case Instruction::FSub:
  case Instruction::FMul:
  case Instruction::FNeg:
    if (!Ty->getScalarType()->isBFloatTy()) {
      return false;
    }
    break;
  case Instruction::FCmp: {
    Type *CmpTy = I.getOperand(0)->getType();
    if (!CmpTy->getScalarType()->isBFloatTy()) {
      return false;
    }
    break;
  }
  case Instruction::FPExt: {
    // Extend from bfloat16 to float32
    Type *FromTy = I.getOperand(0)->getType();
    if (!FromTy->getScalarType()->isBFloatTy() ||
        !Ty->getScalarType()->isFloatTy()) {
      return false;
    }
    break;
  }
  case Instruction::FPTrunc: {
    // Trunc from float32 to bfloat16
    Type *FromTy = I.getOperand(0)->getType();
    if (!FromTy->getScalarType()->isFloatTy() ||
        !Ty->getScalarType()->isBFloatTy()) {
      return false;
    }
    break;
  }

  case Instruction::FDiv:
  case Instruction::FPToSI:
  case Instruction::FPToUI:
    return false;
  }
  return true;
}

void AIETTICommon::adjustUnrollingPreferences(Loop *L, ScalarEvolution &SE,
                                              TTI::UnrollingPreferences &UP,
                                              OptimizationRemarkEmitter *ORE) {
  UP.Partial = EnablePartialUnroll;
  UP.Runtime = EnableRuntimeUnroll;
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

bool AIETTICommon::isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
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
    } else if (!EnableZOLForLoopsWithoutMinIterCount) {
      return false;
    }
  }

  // Scan the loop: loops with calls - make it unprofitable
  for (BasicBlock *BB : L->blocks()) {
    for (Instruction &I : *BB) {
      if (isa<CallInst>(I) || isa<InvokeInst>(I)) {
        if (const Function *F = cast<CallBase>(I).getCalledFunction()) {
          if (!isLoweredToCall(F)) {
            continue;
          }
        }
        return false;
      }
      if (!isAllowedInZOL(I)) {
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

bool AIETTICommon::isProfitableOuterLSR(const Loop &L) const {
  // Down-counting loops are essentially always profitable for AIE.
  // They typically need a single GPR for counting down, while "up-counting"
  // loop need one for the IV, and one for the upper bound.
  return ConsiderLSROuterLoops.getNumOccurrences() > 0 ? ConsiderLSROuterLoops
                                                       : true;
}
