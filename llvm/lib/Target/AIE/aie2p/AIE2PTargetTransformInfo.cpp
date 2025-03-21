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
#include "llvm/IR/IntrinsicsAIE2P.h"

using namespace llvm;

static cl::opt<bool>
    UnrollOnlyLoopsWithPragma("aie2p-unroll-only-pragma-loops",
                              cl::desc("Only unroll loops guarded by pragmas"),
                              cl::init(true), cl::Hidden);

static cl::opt<unsigned> LoopIdiomUnrollingThreshold(
    "aie2p-loop-idiom-unrolling-threshold",
    cl::desc("Unrolling threshold for unrolling vector idiom loops"),
    cl::init(350), cl::Hidden);

#define DEBUG_TYPE "aie2ptti"

// Extract element vector -> intrinsic pattern.
// This covers specific sequences where we apply INV, INVSQRT or PUT_MS
// to the extracted element.
static bool isVectorExtractIntrinsicSequence(const Instruction &I) {

  if (auto *MaybeIntrinsicCall = dyn_cast<CallBase>(&I)) {
    Intrinsic::ID ID = MaybeIntrinsicCall->getIntrinsicID();
    if (ID == Intrinsic::aie2p_inv || ID == Intrinsic::aie2p_invsqrt ||
        ID == Intrinsic::aie2p_put_ms) {
      if (isa<ExtractElementInst>(MaybeIntrinsicCall->getArgOperand(0)))
        return true;
    }
  }
  return false;
}

// get_ss -> Extract -> Insert vector element pattern.
// This covers specific sequences where we insert elements that were
// extracted from a GET_SS intrinsic call.
static bool isGetSSExtractInsertVectorSequence(const Instruction &I) {

  if (auto *MaybeInsert = dyn_cast<InsertElementInst>(&I)) {
    if (auto *MaybeExtract =
            dyn_cast<ExtractValueInst>(MaybeInsert->getOperand(1))) {
      if (auto *MaybeIntrinsicCall =
              dyn_cast<CallBase>(MaybeExtract->getAggregateOperand())) {
        Intrinsic::ID ID = MaybeIntrinsicCall->getIntrinsicID();
        if (ID == Intrinsic::aie2p_get_ss)
          return true;
      }
    }
  }
  return false;
}

static bool isScalarizedVectorOpIdiomLoop(const Loop *L) {

  if (L->getNumBlocks() != 1)
    return false;

  bool IsVectorLoopIdiom = false;
  const BasicBlock *LoopBlock = L->getHeader();

  for (auto &I : *LoopBlock) {
    // Memory operations are not allowed.
    if (isa<LoadInst>(I) || isa<StoreInst>(I))
      return false;
    // This is a weak detection approach for some vector loop
    // idioms that we want to unroll to be able to further apply
    // combiners.
    // TODO: implement an exact detection by looking into IV + insert/extract.
    IsVectorLoopIdiom |= (isVectorExtractIntrinsicSequence(I) ||
                          isGetSSExtractInsertVectorSequence(I));
  }

  return IsVectorLoopIdiom;
}

void AIE2PTTIImpl::getUnrollingPreferences(Loop *L, ScalarEvolution &SE,
                                           TTI::UnrollingPreferences &UP,
                                           OptimizationRemarkEmitter *ORE) {

  BaseT::getUnrollingPreferences(L, SE, UP, ORE);
  Common.adjustUnrollingPreferences(L, SE, UP, ORE);

  if (isScalarizedVectorOpIdiomLoop(L)) {
    UP.Threshold = LoopIdiomUnrollingThreshold;
  } else if (UnrollOnlyLoopsWithPragma && !AIELoopUtils::hasUnrollPragma(L)) {
    // If we don't have unroll pragma, block unroll by zeroing the
    // Threshold.
    UP.Threshold = 0;
    // We also disable partial because it can bypass the Threshold logic.
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
