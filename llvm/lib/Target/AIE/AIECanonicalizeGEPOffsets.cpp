//===-- AIECanonicalizeGEPOffsets.cpp - Canonicalize GEP offsets -*- C++ -*-//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass splits mixed GEP indices of memory operations: an index built
// from terms and a folded-in constant (e.g. the `ptr + (iv + 64)` shapes
// produced by the inliner when a loop body is unrolled over
// `iv + K1, iv + K2, ...`) becomes
//
//   gep(gep(base, {terms}), {constant})
//
// so that every GEP link is one pointer increment step again. Such hidden
// offsets defeat the GlobalISel post-increment and offset load/store
// combiners, which only see direct pointer chains, and materialize as
// per-operation index registers (modifier register setup plus scalar adds),
// costing setup cycles and program memory without changing the steady-state
// schedule. Chains of pure-term and pure-const GEP links need no work: they
// are already a one-step-at-a-time pointer walk.
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/ValueHandle.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Utils/Local.h"
#include <map>
#include <tuple>
using namespace llvm;

#define DEBUG_TYPE "aie-canonicalize-gep-offsets"

/// Accumulate the non-constant terms and the constant offset of \p V. Only
/// arithmetic that survives regrouping under wrap-around semantics is split:
/// plain adds, subtractions of constants and `or disjoint`; anything else is
/// an opaque non-constant term. \return false if the expression is too deep,
/// has too many terms, or mixes index widths.
static bool splitIndexValue(Value *V, SmallVectorImpl<Value *> &Terms,
                            APInt &ConstOffset, unsigned Depth) {
  if (Depth > 16 || Terms.size() > 32)
    return false;

  Type *Ty = V->getType();
  if (!Ty->isIntegerTy() ||
      Ty->getIntegerBitWidth() != ConstOffset.getBitWidth() ||
      Ty->getIntegerBitWidth() > 64)
    return false;

  if (auto *CI = dyn_cast<ConstantInt>(V)) {
    ConstOffset += CI->getValue();
    return true;
  }

  if (auto *BO = dyn_cast<BinaryOperator>(V)) {
    const bool IsAssocCommArith =
        BO->getOpcode() == Instruction::Add ||
        (BO->getOpcode() == Instruction::Or &&
         cast<PossiblyDisjointInst>(BO)->isDisjoint());
    if (IsAssocCommArith)
      return splitIndexValue(BO->getOperand(0), Terms, ConstOffset,
                             Depth + 1) &&
             splitIndexValue(BO->getOperand(1), Terms, ConstOffset, Depth + 1);
    if (BO->getOpcode() == Instruction::Sub) {
      if (auto *CI = dyn_cast<ConstantInt>(BO->getOperand(1))) {
        const APInt ConstBefore = ConstOffset;
        const unsigned TermsBefore = Terms.size();
        if (splitIndexValue(BO->getOperand(0), Terms, ConstOffset, Depth + 1)) {
          ConstOffset -= CI->getValue();
          return true;
        }
        ConstOffset = ConstBefore;
        Terms.truncate(TermsBefore);
      }
    }
  }

  Terms.push_back(V);
  return true;
}

/// \return the operand index of the pointer operand of \p I, if \p I is a
/// simple load or store.
static std::optional<unsigned> getMemPointerOperandIdx(const Instruction &I) {
  if (const auto *Load = dyn_cast<LoadInst>(&I))
    if (Load->isSimple())
      return Load->getPointerOperandIndex();
  if (const auto *Store = dyn_cast<StoreInst>(&I))
    if (Store->isSimple())
      return Store->getPointerOperandIndex();
  return std::nullopt;
}

namespace {
class AIECanonicalizeGEPOffsets : public FunctionPass {
public:
  AIECanonicalizeGEPOffsets() : FunctionPass(ID) {}
  void getAnalysisUsage(AnalysisUsage &AU) const override;
  bool runOnFunction(Function &Fn) override;
  bool canonicalizeMemOpPointer(
      Instruction &MemI,
      std::map<std::tuple<Value *, Value *, int64_t>, Value *> &ChainCache,
      SmallVectorImpl<WeakTrackingVH> &MaybeDead);
  static char ID;
};
} // end anonymous namespace

char AIECanonicalizeGEPOffsets::ID = 0;
char &llvm::AIECanonicalizeGEPOffsetsID = AIECanonicalizeGEPOffsets::ID;
INITIALIZE_PASS(AIECanonicalizeGEPOffsets, DEBUG_TYPE,
                /*name=*/"AIE canonicalize GEP offsets",
                /*isCFGOnly=*/false, /*is_analysis=*/false)

FunctionPass *llvm::createAIECanonicalizeGEPOffsetsPass() {
  return new AIECanonicalizeGEPOffsets();
}

void AIECanonicalizeGEPOffsets::getAnalysisUsage(AnalysisUsage &AU) const {}

bool AIECanonicalizeGEPOffsets::canonicalizeMemOpPointer(
    Instruction &MemI,
    std::map<std::tuple<Value *, Value *, int64_t>, Value *> &ChainCache,
    SmallVectorImpl<WeakTrackingVH> &MaybeDead) {
  const auto PointerOperandIdx = getMemPointerOperandIdx(MemI);
  if (!PointerOperandIdx)
    return false;

  // Collect the GEP chain feeding the pointer operand, innermost first. Only
  // single-index byte GEPs can be regrouped freely.
  SmallVector<GetElementPtrInst *, 4> Chain;
  Value *Cur = MemI.getOperand(*PointerOperandIdx);
  while (auto *GEP = dyn_cast<GetElementPtrInst>(Cur)) {
    if (GEP->getNumIndices() != 1 ||
        !GEP->getSourceElementType()->isIntegerTy(8))
      break;
    Chain.push_back(GEP);
    Cur = GEP->getPointerOperand();
  }
  if (Chain.empty())
    return false;

  // Chains need rewriting when an index mixes terms with a constant folded
  // into one expression. A chain of pure-term and pure-const links that all
  // directly feed memory operations is a one-step-at-a-time pointer walk and
  // must be left alone. A pure-const inner link whose result feeds only
  // further address computation is pure address math and is merged too, so
  // that gep(gep(out, C), iv) shares the iv-walking base with siblings.
  SmallVector<Value *, 8> Terms;
  APInt ConstOffset(
      cast<IntegerType>(Chain.front()->getOperand(1)->getType())->getBitWidth(),
      0);
  bool NeedsRewrite = false;
  for (unsigned I = 0; I < Chain.size(); I++) {
    GetElementPtrInst *GEP = Chain[I];
    const unsigned TermsBefore = Terms.size();
    const APInt ConstBefore = ConstOffset;
    if (!splitIndexValue(GEP->getOperand(1), Terms, ConstOffset, 0))
      return false;
    if (Terms.size() != TermsBefore && ConstOffset != ConstBefore) {
      NeedsRewrite = true;
      continue;
    }
    // Pure-const inner link used only for address computation?
    if (I > 0 && ConstOffset != ConstBefore &&
        none_of(GEP->users(),
                [](User *U) { return isa<StoreInst, LoadInst>(U); }))
      NeedsRewrite = true;
  }
  if (!NeedsRewrite)
    return false;

  // Rebuild: shared base, non-constant indices innermost-first, then the
  // merged constant offset. The cache shares prefixes between sibling memory
  // operations and reuses already-canonical GEPs.
  Value *NewPtr = Cur;
  auto GetOrCreateGEP = [&](Value *Ptr, Value *IdxV, int64_t COff) -> Value * {
    auto Key = std::make_tuple(Ptr, IdxV, COff);
    auto It = ChainCache.find(Key);
    if (It != ChainCache.end())
      return It->second;
    Value *Idx = IdxV;
    if (!Idx)
      Idx = ConstantInt::get(Chain.back()->getOperand(1)->getType(), COff);
    // No wrap flags: regrouped intermediate sums may temporarily leave the
    // object the original inbounds chain stayed within.
    auto *NewGEP =
        GetElementPtrInst::Create(Type::getInt8Ty(MemI.getContext()), Ptr, Idx,
                                  "canon.gepidx", MemI.getIterator());
    ChainCache[Key] = NewGEP;
    return NewGEP;
  };

  for (Value *Term : Terms)
    NewPtr = GetOrCreateGEP(NewPtr, Term, 0);
  if (ConstOffset != 0)
    NewPtr = GetOrCreateGEP(NewPtr, nullptr, ConstOffset.getSExtValue());

  if (NewPtr == MemI.getOperand(*PointerOperandIdx))
    return false;
  LLVM_DEBUG(dbgs() << "Canonicalized address of " << MemI << "\n  via "
                    << *NewPtr << "\n");
  // The now-orphaned index arithmetic is deleted after the function walk,
  // once no cache entry can reference it anymore.
  for (GetElementPtrInst *GEP : Chain)
    MaybeDead.push_back(GEP);
  MemI.setOperand(*PointerOperandIdx, NewPtr);
  return true;
}

bool AIECanonicalizeGEPOffsets::runOnFunction(Function &Fn) {
  bool Changed = false;
  SmallVector<WeakTrackingVH, 8> MaybeDead;
  for (auto &BB : Fn) {
    std::map<std::tuple<Value *, Value *, int64_t>, Value *> ChainCache;
    // Seed the cache with the GEPs seen so far in the block so that an
    // address that is already in canonical form is reused as-is instead of
    // being duplicated.
    for (auto &I : BB) {
      if (auto *GEP = dyn_cast<GetElementPtrInst>(&I)) {
        if (GEP->getNumIndices() == 1 &&
            GEP->getSourceElementType()->isIntegerTy(8)) {
          Value *Ptr = GEP->getPointerOperand();
          Value *Idx = GEP->getOperand(1);
          std::tuple<Value *, Value *, int64_t> Key;
          if (auto *CI = dyn_cast<ConstantInt>(Idx)) {
            if (CI->getBitWidth() > 64)
              continue;
            Key = {Ptr, nullptr, CI->getSExtValue()};
          } else if (Idx->getType()->isIntegerTy()) {
            Key = {Ptr, Idx, 0};
          } else {
            continue;
          }
          // Keep the first (dominating) GEP for a given key.
          ChainCache.emplace(Key, GEP);
        }
        continue;
      }
      Changed |= canonicalizeMemOpPointer(I, ChainCache, MaybeDead);
    }
  }
  Changed |= RecursivelyDeleteTriviallyDeadInstructionsPermissive(MaybeDead);
  return Changed;
}
