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
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicsAIE2.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include <optional>

using namespace llvm;

static cl::opt<unsigned> BaseObjectSearchLimit(
    "aie-alias-analysis-search-limit",
    cl::desc("Search depth limit for base objects in AIE alias analysis"),
    cl::init(100), cl::Hidden);

static cl::opt<bool> DisambiguateAccessSameOriginPointers(
    "aie-alias-analysis-disambiguation",
    cl::desc("Disambiguate pointers derived from the same origin"),
    cl::init(true), cl::Hidden);

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

/// This gives all indexes for read only operands ie.
/// excluding counters.
static SmallVector<unsigned, 5> getAddIntrinsicOps(Intrinsic::ID ID) {
  switch (ID) {
  case Intrinsic::aie2_add_2d:
    return {1, 2, 3 /*,count*/};
  case Intrinsic::aie2_add_3d:
    return {1, 2, 3, 4, /*,count1*/ 6,
            /*,count2*/};
  default:
    llvm_unreachable("Unknown intrinsic");
  }

  return {};
}

/// This gives all indexes counter operands.
static SmallVector<unsigned, 5> getAddIntrinsicCounterOps(Intrinsic::ID ID) {
  switch (ID) {
  case Intrinsic::aie2_add_2d:
    return {4};
  case Intrinsic::aie2_add_3d:
    return {5, 7};
  default:
    llvm_unreachable("Unknown intrinsic");
  }

  return {};
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

static const Value *skipCast(const Value *V) {
  if (const Instruction *Instr = dyn_cast<const Instruction>(V)) {
    if (Instr->isCast())
      return Instr->getOperand(0);
  }
  return V;
}

struct IntrinsicArgsInfo {
  SmallVector<const Value *, 5> ConstantArgs;
  SmallVector<const Value *, 5> CounterArgs;
};

struct IntrinsicChainInfo {
  // Calls up to a specific point.
  unsigned IntrinsicCalls = 0;
  // Total calls across the loop.
  unsigned TotalIntrinsicCalls = 0;
  // All parameters.
  IntrinsicArgsInfo IntrinsicParameters;

  // Same number of call until the point of interest.
  bool hasSameNumberOfCalls(const IntrinsicChainInfo &Other) const {
    return IntrinsicCalls == Other.IntrinsicCalls;
  }

  // Same number of calls accross the loop.
  bool hasSameTotalNumberOfCalls(const IntrinsicChainInfo &Other) const {
    return TotalIntrinsicCalls == Other.TotalIntrinsicCalls;
  }

  // Check for the same constant parameters.
  bool hasSameParameters(const IntrinsicChainInfo &Other) const {
    return IntrinsicParameters.ConstantArgs ==
           Other.IntrinsicParameters.ConstantArgs;
  }

  // Check for the same initial values for the counters.
  bool hasSameStartingCounters(const IntrinsicChainInfo &Other) const {

    auto GetExternalIndex = [](const PHINode *Phi) {
      return Phi->getIncomingBlock(0) == Phi->getParent() ? 1 : 0;
    };

    for (const Value *CountA : IntrinsicParameters.CounterArgs) {
      // Some truncation can be expected.
      CountA = skipCast(CountA);
      if (auto *PhiA = dyn_cast<PHINode>(CountA)) {
        const Value *ExtPhiValA =
            PhiA->getIncomingValue(GetExternalIndex(PhiA));
        for (const Value *CountB : Other.IntrinsicParameters.CounterArgs) {
          CountB = skipCast(CountB);
          if (auto *PhiB = dyn_cast<PHINode>(CountB)) {
            const Value *ExtPhiValB =
                PhiB->getIncomingValue(GetExternalIndex(PhiB));
            if (ExtPhiValA != ExtPhiValB)
              return false;
          } else {
            return false; // CountB is not PHI.
          }
        }
      } else {
        return false; // CountA is not PHI.
      }
    }
    return true;
  }

  bool mayOverlap(const IntrinsicChainInfo &Other) const {
    // Different number of updates in the chain.
    // * but maybe with consistent updates (same steps)
    if (!hasSameTotalNumberOfCalls(Other))
      return true;

    // Different parameters.
    if (!hasSameParameters(Other))
      return true;

    // Every counter must start equally.
    if (!hasSameStartingCounters(Other))
      return true;

    // Check for different steps of calculation.
    if (hasSameNumberOfCalls(Other))
      return true;

    return false;
  }

  void bumpIteration(const unsigned VirtIteration) {
    IntrinsicCalls += VirtIteration * TotalIntrinsicCalls;
  }
};

static IntrinsicArgsInfo collectIntrinsicOperands(const Value *V) {
  const auto *Extract = dyn_cast<ExtractValueInst>(V);
  assert(Extract && "Not an Extract");
  const auto *IntrinsicCall =
      dyn_cast<CallBase>(Extract->getAggregateOperand());
  assert(IntrinsicCall && "Not an Intrinsic");
  Intrinsic::ID ID = IntrinsicCall->getIntrinsicID();
  IntrinsicArgsInfo Args;
  // Necessary operands to disambiguate.
  // Constants.
  for (unsigned Op : getAddIntrinsicOps(ID)) {
    Args.ConstantArgs.push_back(IntrinsicCall->getArgOperand(Op));
  }
  // Counters.
  for (unsigned Op : getAddIntrinsicCounterOps(ID)) {
    Args.CounterArgs.push_back(IntrinsicCall->getArgOperand(Op));
  }
  return Args;
}

// We can reach a pointer definition starting from a counter.
static bool
isAddressingIntrinsicOutput(ArrayRef<const Value *> AdressingOutputs,
                            const Value *Intrinsic) {

  for (const Value *Counter : AdressingOutputs) {
    if (Intrinsic != lookThroughAIEAddressingModes(Counter))
      return false;
  }

  return true;
}

static bool allInSameBB(SmallVector<const Value *> Values) {

  for (unsigned I = 0; I < Values.size() - 1; I++) {
    const Instruction *InstrA = dyn_cast<Instruction>(Values[I]);
    const Instruction *InstrB = dyn_cast<Instruction>(Values[I + 1]);

    if (!InstrA || !InstrB)
      return false;

    if (InstrA->getParent() != InstrB->getParent())
      return false;
  }
  return true;
}

/// This function returns some tracking information related to the upward path
/// that starts from a Value *TargetPointer to the Phi node that firstly defines
/// a pointer value to the same ambiguous memory. We start the search from the
/// last update of the pointer.
std::optional<IntrinsicChainInfo>
trackIntrinsicChain(const Value *LastUpdate, const Value *TargetPointer) {
  IntrinsicChainInfo ChainInfo;
  // We count backwards.
  std::optional<unsigned> LastCalls = std::nullopt;
  // Let's expose the real pointer.
  TargetPointer = skipCast(TargetPointer);
  for (unsigned Count = 0; Count < BaseObjectSearchLimit; ++Count) {
    if (!allInSameBB({LastUpdate, TargetPointer}))
      return std::nullopt;

    LastUpdate = skipCast(LastUpdate);

    // This information is crucial to calculate the number
    // of pointer updates before TargetPointer.
    if (LastUpdate == TargetPointer)
      LastCalls = ChainInfo.TotalIntrinsicCalls;

    if (!LastUpdate->getType()->isPointerTy())
      return std::nullopt; // not a pointer.

    if (auto *GEP = dyn_cast<GEPOperator>(LastUpdate)) {
      if (!GEP->hasAllZeroIndices())
        return std::nullopt; // No pointer arithmetic is allowed.
      // The result pointer and the first operand have
      // the same value
      LastUpdate = GEP->getPointerOperand();
    } else if (auto *PHI = dyn_cast<PHINode>(LastUpdate)) {
      // Reached final destination.
      if (LastCalls) {
        ChainInfo.IntrinsicCalls = ChainInfo.TotalIntrinsicCalls - *LastCalls;
        return ChainInfo;
      }
      return std::nullopt; // The object was not found.

    } else {
      const Value *AIEObject = lookThroughAIEAddressingModes(LastUpdate);

      // lookThroughAIEAddressingModes cannot decode LastUpdate.
      if (LastUpdate == AIEObject) {
        return std::nullopt;
      }

      IntrinsicArgsInfo NewArgs = collectIntrinsicOperands(LastUpdate);

      // Initialize counters.
      if (!ChainInfo.IntrinsicParameters.CounterArgs.empty() &&
          !isAddressingIntrinsicOutput(
              ArrayRef(ChainInfo.IntrinsicParameters.CounterArgs), AIEObject)) {
        // if we already have counters, we need to reach the same
        // AIEObject from the counters. Otherwise we can have some undesired
        // counter update in between.
        return std::nullopt; // not reachable.
      }

      if (!ChainInfo.IntrinsicParameters.ConstantArgs.empty() &&
          ChainInfo.IntrinsicParameters.ConstantArgs != NewArgs.ConstantArgs) {
        // Constant iterator information mismatch between sequence of
        // intrinsic calls. We stop here.
        return std::nullopt;
      }

      // Update params for the next round.
      ChainInfo.IntrinsicParameters.CounterArgs = NewArgs.CounterArgs;
      ChainInfo.IntrinsicParameters.ConstantArgs = NewArgs.ConstantArgs;

      LastUpdate = AIEObject;
      ChainInfo.TotalIntrinsicCalls++;
    }
  }
  return std::nullopt;
}

static AliasResult aliasAIEIntrinsic(const Value *ValueA, const Value *ValueB,
                                     const Value *BaseA, const Value *BaseB,
                                     const unsigned VirtUnrollLevelA,
                                     const unsigned VirtUnrollLevelB) {

  const PHINode *PhiA = dyn_cast<PHINode>(BaseA);
  const PHINode *PhiB = dyn_cast<PHINode>(BaseB);

  // Both bases must be PHI nodes.
  if (!PhiA || !PhiB)
    return AliasResult::MayAlias;

  // Skip non-pointer values (cast from pointers?).
  if (!PhiA->getType()->isPointerTy() || !PhiB->getType()->isPointerTy())
    return AliasResult::MayAlias;

  // Our target is loop-recurrent PHI nodes.
  if (PhiA->getNumIncomingValues() != 2 || PhiB->getNumIncomingValues() != 2)
    return AliasResult::MayAlias;

  // Both PHI nodes must be in the same MBB.
  if (PhiA->getParent() != PhiB->getParent())
    return AliasResult::MayAlias;

  auto IsPHIRecurrent = [](const PHINode *Phi) {
    return Phi->getIncomingBlock(0) == Phi->getParent() ||
           Phi->getIncomingBlock(1) == Phi->getParent();
  };

  // Both PHI nodes are recurrent.
  if (!IsPHIRecurrent(PhiA) || !IsPHIRecurrent(PhiA)) {
    return AliasResult::MayAlias;
  }

  auto GetExternalIndex = [](const PHINode *Phi) {
    return Phi->getIncomingBlock(0) == Phi->getParent() ? 1 : 0;
  };

  auto GetInternalIndex = [](const PHINode *Phi) {
    return Phi->getIncomingBlock(0) == Phi->getParent() ? 0 : 1;
  };

  const Value *ExtPhiValA = PhiA->getIncomingValue(GetExternalIndex(PhiA));
  const Value *ExtPhiValB = PhiB->getIncomingValue(GetExternalIndex(PhiB));

  // Both Phis must have the same value as an external incoming operand,
  // so we guarantee that we have two new pointers rooted in the same
  // original pointer.
  //
  // NOTE: We don't care if PhiA and PhiB are the same or not, so we can also
  // disambiguate different steps starting from from the same pointer (Phi).
  // We only care if the external value is the same.
  // For example, we can have the following case:
  // a =  phi ptr [ %an, %this_mbb ], [ %a_ext, %pred_mbb ]
  // x1 = load (a)
  // a1 = add_2d(a, /*fixed parameters + chained counters*/)
  // x2 = load (a1)
  // an = add_2d(a1, /*fixed parameters + chained counters*/)
  // Where we can prove that those two loads are not aliasing, even though
  // they are sharing the same phi node as an underlying object.
  if (ExtPhiValA != ExtPhiValB)
    return AliasResult::MayAlias;

  // Most of the remaining code aims to prove that the two pointers
  // are updated with same steps.

  const Value *IntPhiValA = PhiA->getIncomingValue(GetInternalIndex(PhiA));
  const Value *IntPhiValB = PhiB->getIncomingValue(GetInternalIndex(PhiB));

  // Assumption: same basic block.
  if (!allInSameBB({ValueA, ValueB, BaseA, BaseB, IntPhiValA, IntPhiValB}))
    return AliasResult::MayAlias;

  // Now we know that the pointers are updated consistently across
  // loop iterations, let's look to the values.
  auto ATracking = trackIntrinsicChain(IntPhiValA, ValueA);
  if (!ATracking) // Non uniform address update accross the loop.
    return AliasResult::MayAlias;

  auto BTracking = trackIntrinsicChain(IntPhiValB, ValueB);
  if (!BTracking) // Non uniform address update accross the loop.
    return AliasResult::MayAlias;

  // If we are in PostRA SWP, we need to increase the count
  // by the number of unrolls x complete number of pointer
  // updates for every completed iteration. In this way,
  // we will have the notion of distance between different
  // iterations and associated instructions.
  ATracking->bumpIteration(VirtUnrollLevelA);
  BTracking->bumpIteration(VirtUnrollLevelB);

  // If there is no overlap, NoAlias
  if (ATracking->mayOverlap(*BTracking))
    return AliasResult::MayAlias;

  return AliasResult::NoAlias;
}

AliasResult AIEBaseAAResult::alias(const MemoryLocation &LocA,
                                   const MemoryLocation &LocB,
                                   AAQueryInfo &AAQI, const Instruction *) {

  const Value *BaseA = getUnderlyingObjectAIE(LocA.Ptr);
  const Value *BaseB = getUnderlyingObjectAIE(LocB.Ptr);

  if (DisambiguateAccessSameOriginPointers &&
      aliasAIEIntrinsic(LocA.Ptr, LocB.Ptr, BaseA, BaseB, 0, 0
                        /*No virtually unrolled*/) == AliasResult::NoAlias)
    return AliasResult::NoAlias;

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

AliasResult AIE::aliasAcrossVirtualUnrolls(const MachineInstr *MIA,
                                           const MachineInstr *MIB,
                                           unsigned UnrollLevelMIA,
                                           unsigned UnrollLevelMIB) {

  auto GetValueBasePair = [](const MachineMemOperand *MMO)
      -> std::optional<std::pair<const Value *, const Value *>> {
    const Value *Val = MMO->getValue();
    // We know nothing about the MMO.
    if (!Val)
      return std::nullopt;

    const Value *Base = getUnderlyingObjectAIE(Val);
    if (!Base)
      return std::nullopt;

    return std::make_pair(Val, Base);
  };

  // Check each pair of memory operands from both instructions.
  for (auto *MMOA : MIA->memoperands()) {

    auto ValueBasePairA = GetValueBasePair(MMOA);
    if (!ValueBasePairA)
      return AliasResult::MayAlias;

    for (auto *MMOB : MIB->memoperands()) {

      auto ValueBasePairB = GetValueBasePair(MMOB);
      if (!ValueBasePairB)
        return AliasResult::MayAlias;

      if (aliasAIEIntrinsic(ValueBasePairA->first, ValueBasePairB->first,
                            ValueBasePairA->second, ValueBasePairB->second,
                            UnrollLevelMIA,
                            UnrollLevelMIB) == AliasResult::MayAlias) {
        return AliasResult::MayAlias;
      }
    }
  }
  return AliasResult::NoAlias;
}
