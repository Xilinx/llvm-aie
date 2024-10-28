//===-- LoopMetadata.cpp - Convert Loop Metadata to assumes --*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/LoopMetadata.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/LoopUtils.h"

#define DEBUG_TYPE "loop-metadata"

using namespace llvm;

/// extract the step size by which the loop IV changes.
int getSCEVStepSize(const SCEV *S);

PreservedAnalyses LoopMetadata::run(Loop &L, LoopAnalysisManager &AM,
                                    LoopStandardAnalysisResults &AR,
                                    LPMUpdater &U) {
  SE = &AR.SE;
  AC = &AR.AC;
  DT = &AR.DT;

  assignLoopMetadata(L);

  return PreservedAnalyses::all();
}

bool LoopMetadata::assignLoopMetadata(Loop &L) {
  this->L = &L;
  Context = &L.getHeader()->getParent()->getContext();

  std::optional<int> MinIterCount = getMinTripCount(&L);

  // dump loop summary
  LLVM_DEBUG(if (L.getLoopPreheader()) {
    dbgs() << "Preheader:" << L.getLoopPreheader()->getName() << "\n";
  });
  LLVM_DEBUG(dbgs() << "Header:" << L.getHeader()->getName() << "\n");

  if (MinIterCount.has_value() && MinIterCount.value() > 0) {
    this->MinIterCount = MinIterCount.value();

    LLVM_DEBUG(L.getHeader()->getParent()->dump(););
    addAssumeToLoopHeader();
    return true;
  }

  return false;
}
bool LoopMetadata::canExtractIncrement(const SCEV *S) {
  const SCEVAddRecExpr *AR = dyn_cast<SCEVAddRecExpr>(S);
  if (!AR)
    return false;

  const SCEVConstant *SCEVConst =
      dyn_cast<SCEVConstant>(AR->getStepRecurrence(*SE));
  if (!SCEVConst) {
    return false;
  }
  LoopStepSize = getSCEVStepSize(SCEVConst);
  IsLoopIncrementing = LoopStepSize > 0;
  return true;
}

Value *LoopMetadata::calcMinIterValue(const SCEV *S, int MinIterCount,
                                      LLVMContext *Context) {
  assert(MinIterCount > 0);

  // extract loop counter increment/decrement
  const SCEVAddRecExpr *AR = dyn_cast<SCEVAddRecExpr>(S);
  if (!AR) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning: SCEV S i not a SCEVAddRecExpr ";
               S->dump());
    return nullptr;
  }
  const SCEV *ConstExpr = AR->getOperand(1);
  if (!AR->isAffine() || !isa<SCEVConstant>(ConstExpr)) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning: Unknown SCEVAddRecExpr ";
               AR->dump());
    return nullptr;
  }

  int MinIterValue = std::abs(LoopStepSize * MinIterCount);
  // calculate the minimum iteration value, since SGE is used, subtract 1
  MinIterValue--;

  // If the loop does not start at 0, add the loop start to the Minimum
  // Iteration Value
  assert(isa<Constant>(LowerBoundary));
  int LoopStart =
      dyn_cast<Constant>(LowerBoundary)->getUniqueInteger().getSExtValue();
  MinIterValue += LoopStart;

  return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*Context), MinIterValue,
                                true);
}

Value *getLoopInvariant(Value *Op0, Value *Op1, ScalarEvolution *SE) {
  if ((SE->isSCEVable(Op0->getType()) &&
       (SCEVAddExpr::classof(SE->getSCEV(Op0)) ||
        SCEVAddRecExpr::classof(SE->getSCEV(Op0)))) ||
      isa<PHINode>(Op0))
    return Op1;

  return Op0;
}

void LoopMetadata::getBoundaries(const SCEV *S) {
  assert(SCEVAddRecExpr::classof(S));

  Value *Op0 = LoopCmpInstr->getOperand(0);
  Value *Op1 = LoopCmpInstr->getOperand(1);

  const SCEVAddRecExpr *AR = dyn_cast<SCEVAddRecExpr>(S);
  const SCEVConstant *SCEVConst = dyn_cast<SCEVConstant>(AR->getStart());

  if (IsLoopIncrementing) {
    if (!SCEVConst) {
      LowerBoundary = nullptr;
      UpperBoundary = nullptr;
      return;
    }
    LowerBoundary = SCEVConst->getValue();
    UpperBoundary = getLoopInvariant(Op0, Op1, SE);

  } else {

    if (SCEVConst)
      UpperBoundary = SCEVConst->getValue();

    if (const SCEVUnknown *S = dyn_cast<SCEVUnknown>(AR->getStart()))
      UpperBoundary = S->getValue();

    LowerBoundary = getLoopInvariant(Op0, Op1, SE);
  }

}

const SCEV *LoopMetadata::getAddRecSCEV(Value *Op) {
  if (!SE->isSCEVable(Op->getType()))
    return nullptr;

  const SCEV *S = SE->getSCEV(Op);
  if (SCEVAddRecExpr::classof(S))
    return S;

  return nullptr;
}

const SCEV *LoopMetadata::getSCEV() {
  for (Value *Op : LoopCmpInstr->operands()) {
    if (const SCEV *S = getAddRecSCEV(Op))
      return S;
  }

  return nullptr;
}


int getSCEVStepSize(const SCEV *S) {
  assert(isa<SCEVConstant>(S));
  return dyn_cast<SCEVConstant>(S)->getValue()->getSExtValue();
}

/// match the types of the loop bound and the minimum iteration value
/// Insert signed Extension Instruction if needed
std::pair<llvm::Value *, llvm::Value *>
promoteMismatchedType(Value *Value1, Value *Value2, IRBuilder<> &Builder) {
  Type *Type1 = Value1->getType();
  Type *Type2 = Value2->getType();

  if (Type1 == Type2)
    return std::make_pair(Value1, Value2);

  if (Type2->getScalarSizeInBits() < Type1->getScalarSizeInBits()) {
    Value2 = Builder.CreateSExt(Value2, Type1);
    LLVM_DEBUG(dbgs() << "Type Matching for "; Value2->dump());
  } else {
    Value1 = Builder.CreateSExt(Value1, Type2);
    LLVM_DEBUG(dbgs() << "Type Matching for  "; Value1->dump());
  }
  return std::make_pair(Value1, Value2);
}

void LoopMetadata::addAssumeToLoopHeader() {
  // reset Loop specific information;
  LowerBoundary = nullptr;
  UpperBoundary = nullptr;

  LLVM_DEBUG(dbgs() << "Processing Loop Metadata: "
                    << L->getHeader()->getParent()->getName() << " "
                    << L->getName() << " (" << MinIterCount << ")\n");

  BranchInst *BI = dyn_cast<BranchInst>(L->getExitingBlock()->getTerminator());
  if (!BI)
    return;

  LoopCmpInstr = dyn_cast<ICmpInst>(BI->getCondition());
  if (!LoopCmpInstr)
    return;

  LLVM_DEBUG(dbgs() << "Branch Instruction Found: "; LoopCmpInstr->dump();
             dbgs() << "\n");

  // get Scalar Evolution of the induction variable
  const SCEV *S = getSCEV();
  if (!S) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning: Could not extract "
                         "SCEVAddRecExpr! Will not process Metadata\n");
    return;
  }

  if (!canExtractIncrement(S)) {
    LLVM_DEBUG(
        dbgs() << "LoopMetadata-Warning: Could not calculate "
                  "Increment/Decrement of Loop Counter. Will not process "
                  "Metadata\n");
    return;
  }

  getBoundaries(S);
  if (!UpperBoundary) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning: Could not find Iteration "
                         "Variable. Will not process Metadata\n");
    return;
  }

  Value *MinIterValue = calcMinIterValue(S, MinIterCount, Context);
  if (!MinIterValue) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning: Could not extract Minimum "
                         "Iteration Value\n");
    return;
  }

  LLVM_DEBUG(dbgs() << "Loop Metadata        : " << MinIterCount << "\n");
  LLVM_DEBUG(dbgs() << "Min Iteration Value  : "; MinIterValue->dump());
  LLVM_DEBUG(dbgs() << "Upper Loop boundry   : "; UpperBoundary->dump());

  IRBuilder<> Builder(L->getHeader()->getTerminator());

  std::pair<llvm::Value *, llvm::Value *> CompareOps =
      promoteMismatchedType(UpperBoundary, MinIterValue, Builder);
  Value *Cmp = Builder.CreateICmpSGT(CompareOps.first, CompareOps.second);

  // Insert assert
  Function *AssumeFn =
      Intrinsic::getDeclaration(L->getHeader()->getModule(), Intrinsic::assume);
  CallInst *Call = Builder.CreateCall(AssumeFn, Cmp);
  Call->setTailCall(true);
  AC->registerAssumption(dyn_cast<AssumeInst>(Call));

  // first is minIterValue
  LLVM_DEBUG(dbgs() << "Inserting Condition :"; CompareOps.second->dump();
             dbgs() << "With Comparator   :"; Cmp->dump();
             dbgs() << "Assume            :"; Call->dump());
}
