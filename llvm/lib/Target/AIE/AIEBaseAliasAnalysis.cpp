//===- AIEBaseAliasAnalysis -----------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
/// \file
/// This is the AIE alias analysis pass. It looks through AIE's complex
/// addressing mode intrinsics to find the underlying base object, then
/// calls back into regular alias analysis to infer NoAlias relations.
//===----------------------------------------------------------------------===//

#include "AIEBaseAliasAnalysis.h"
#include "AIE.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/MemoryLocation.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicsAIE2.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

static cl::opt<unsigned> BaseObjectSearchLimit(
    "aie-alias-analysis-search-limit",
    cl::desc("Search depth limit for base objects in AIE alias analysis"),
    cl::init(100), cl::Hidden);

#define DEBUG_TYPE "aie-aa"

AnalysisKey AIEBaseAA::Key;

// Register this pass...
char AIEBaseAAWrapperPass::ID = 0;
char AIEBaseExternalAAWrapper::ID = 0;

INITIALIZE_PASS(AIEBaseAAWrapperPass, "aie-aa",
                "AIE complex addressing modes based Alias Analysis", false,
                true)

INITIALIZE_PASS(AIEBaseExternalAAWrapper, "aie-aa-wrapper",
                "AIE complex addressing modes based Alias Analysis Wrapper",
                false, true)

ImmutablePass *llvm::createAIEBaseAAWrapperPass() {
  return new AIEBaseAAWrapperPass();
}

ImmutablePass *llvm::createAIEBaseExternalAAWrapperPass() {
  return new AIEBaseExternalAAWrapper();
}

AIEBaseAAWrapperPass::AIEBaseAAWrapperPass() : ImmutablePass(ID) {
  initializeAIEBaseAAWrapperPassPass(*PassRegistry::getPassRegistry());
}

void AIEBaseAAWrapperPass::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

static bool isAIEPtrAddIntrinsic(Intrinsic::ID ID, unsigned &InPtrIdx) {
  switch (ID) {
  case Intrinsic::aie2_add_2d:
  case Intrinsic::aie2_add_3d:
    InPtrIdx = 0;
    return true;
  default:
    break;
  }

  return false;
}

static const Value *lookThroughAIEAddressingModes(const Value *V) {
  if (const auto *Extract = dyn_cast<ExtractValueInst>(V)) {
    if (const auto *MaybeIntrinsicCall =
            dyn_cast<CallBase>(Extract->getAggregateOperand())) {
      Intrinsic::ID ID = MaybeIntrinsicCall->getIntrinsicID();
      unsigned PtrIdx;
      if (isAIEPtrAddIntrinsic(ID, PtrIdx)) {
        return MaybeIntrinsicCall->getArgOperand(PtrIdx);
      }
    }
  }
  return V;
}

static const Value *getUnderlyingObjectAIE(const Value *V) {

  // The search below looks through AIE specific intrinsics, and then continues
  // its traversal using the generic getUnderlyingObject. Importantly this
  // applies a much higher search depth limit, as we want to look through more
  // pointer arithmetic than BasicAliasAnalysis would do.
  unsigned SearchLimit = BaseObjectSearchLimit;
  for (unsigned Count = 0; Count < SearchLimit; ++Count) {
    // First check if we are looking at an AIE addressing intrinsic
    // If so, retrieve the base pointer and continue the search
    V = lookThroughAIEAddressingModes(V);

    // Now see whether the generic traversal finds another base
    const Value *BaseV = getUnderlyingObject(V, SearchLimit - Count);

    // If yes, set it as our new base and continue the search
    if (BaseV != V) {
      V = BaseV;
      continue;
    }
    // Search didn't reveal any new object, bail out
    break;
  }
  return V;
}

AliasResult AIEBaseAAResult::alias(const MemoryLocation &LocA,
                                   const MemoryLocation &LocB,
                                   AAQueryInfo &AAQI, const Instruction *) {

  const Value *BaseA = getUnderlyingObjectAIE(LocA.Ptr);
  const Value *BaseB = getUnderlyingObjectAIE(LocB.Ptr);

  // Our look-through didn't reveal a different object, pass on to the next
  // alias analysis
  if ((LocA.Ptr == BaseA) && (LocB.Ptr == BaseB))
    return AAResultBase::alias(LocA, LocB, AAQI, nullptr);

  MemoryLocation NewLocA = MemoryLocation::getBeforeOrAfter(BaseA, LocA.AATags);
  MemoryLocation NewLocB = MemoryLocation::getBeforeOrAfter(BaseB, LocB.AATags);

  AliasResult Res = AAQI.AAR.alias(NewLocA, NewLocB, AAQI);
  if (Res == AliasResult::NoAlias)
    return AliasResult::NoAlias;

  // Since our alias anlysis is purely based on base objects, we can only infer
  // NoAlias (based on different base object). In particular, we cannot infer
  // MustAlias, and need to be conservative by returning MayAlias
  return AliasResult::MayAlias;
}
