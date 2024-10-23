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

PreservedAnalyses LoopMetadata::run(Loop &L, LoopAnalysisManager &AM,
                                    LoopStandardAnalysisResults &AR,
                                    LPMUpdater &U) {
  SE = &AR.SE;
  AC = &AR.AC;
  DT = &AR.DT;
  extractMetaData(L);
  return PreservedAnalyses::all();
}

bool LoopMetadata::extractMetaData(Loop &L) {
  this->L = &L;
  Context = &L.getHeader()->getParent()->getContext();

  std::optional<int> MinIterCount = getMinIterCounts(&L);

  // dump loop summary
  LLVM_DEBUG(dbgs() << "Preheader:");
  if (L.getLoopPreheader())
    LLVM_DEBUG(dbgs() << L.getLoopPreheader()->getName());
  LLVM_DEBUG(dbgs() << "\nHeader:");
  if (L.getHeader())
    LLVM_DEBUG(dbgs() << L.getHeader()->getName());
  LLVM_DEBUG(dbgs() << "\n");

  if (MinIterCount.has_value() && MinIterCount.value() > 0) {

    LLVM_DEBUG(L.getHeader()->getParent()->dump(););

    addAssumeToLoopHeader(MinIterCount.value(), Context);
    LLVM_DEBUG(dbgs() << "Dumping Full Function:"
                      << L.getHeader()->getParent()->getName() << "\n";
               L.getHeader()->getParent()->dump(););
    return true;
  }

  return false;
}

bool isIncrement(const SCEV *S) {
  switch (S->getSCEVType()) {
  case scAddRecExpr: {
    const SCEVAddRecExpr *AR = cast<SCEVAddRecExpr>(S);
    assert(AR->getNumOperands() == 2 &&
           "Unknown Handling of more than 2 Operands");
    return cast<SCEVConstant>(*AR->getOperand(1)).getValue()->getSExtValue() >
           0;
    break;
  }

  default:
    assert(false && "Could not extract Iteration Variable from ");
  }
  return false;
}

Value *LoopMetadata::getMinIterValue(const SCEV *S, int MinIterCount,
                                     LLVMContext *Context) {
  assert(MinIterCount > 0);

  // extract loop counter increment/decrement
  int IncValue = 0;
  switch (S->getSCEVType()) {
  case scAddRecExpr: {
    const SCEVAddRecExpr *AR = cast<SCEVAddRecExpr>(S);
    assert(AR->getNumOperands() == 2 &&
           "Unknown Handling of more than 2 Operands");

    if (!isa<SCEVConstant>(*AR->getOperand(1))) {
      // FIXME: variable increments are not supported, since iteration direction
      // is unkown! return cast<SCEVUnknown>(*AR->getOperand(1)).getValue();

      LLVM_DEBUG(dbgs() << "could not extract Increment of SCEV "; S->dump());
      return nullptr;
    }

    IncValue =
        cast<SCEVConstant>(*AR->getOperand(1)).getValue()->getSExtValue();
    break;
  }

  default:
    assert(false && "Could not extract Iteration Variable from ");
  }

  int MaxValue = std::abs(IncValue * MinIterCount);
  // if loop condition contains equality and less or more
  // FIXME: only decrement if the condition is lt (on EQ dont do this)
  if (isIncrement(S))
    MaxValue--;

  llvm::ConstantInt *ConstIncValue =
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(*Context), MaxValue, true);
  return static_cast<llvm::Value *>(ConstIncValue);
}

Value *LoopMetadata::getValue(Value *V) const {
  if (PHINode *MaxPHI = dyn_cast_or_null<PHINode>(V)) {
    bool HasSecondOp = MaxPHI->getNumOperands() > 1;
    // Is a function argument, no need to check domination relationship
    if (!isa<Instruction>(MaxPHI->getOperand(0))) {
      return MaxPHI->getOperand(0);
    }
    // Is a function argument, no need to check domination relationship
    if (HasSecondOp && !isa<Instruction>(MaxPHI->getOperand(1))) {
      return MaxPHI->getOperand(1);
    }
    // Check if the Op0 is from a previous block, then this is the correct value
    if (DT->dominates(dyn_cast<Instruction>(MaxPHI->getOperand(0))->getParent(),
                      L->getHeader())) {
      return MaxPHI->getOperand(0);
    }
    if (HasSecondOp &&
        DT->dominates(dyn_cast<Instruction>(MaxPHI->getOperand(1))->getParent(),
                      L->getHeader())) {
      return MaxPHI->getOperand(1);
    }
    return nullptr;
  }
  return V;
}

Value *LoopMetadata::getMinBoundInEqualityComparison(Value *Op) const {
  // FIXME: think of a better way of extracting min value
  //  1 instruction has to be constant?
  // 2 num of ops && 1 must be constant ?
  Instruction *Instr0 = dyn_cast<Instruction>(Op);
  for (unsigned I = 0; I < Instr0->getNumOperands(); I++) {
    Value *ParentOp = Instr0->getOperand(I);
    if (!isa<Constant>(ParentOp)) {
      return getValue(ParentOp);
      break;
    }
  }
  return nullptr;
}

void LoopMetadata::getBoundries() {

  BranchInst *BI = dyn_cast<BranchInst>(L->getExitingBlock()->getTerminator());
  if (!BI || !BI->isConditional())
    assert(false && "Exiting block does not have a conditional branch.\n");

  Value *Condition = BI->getCondition();
  ICmpInst *ICmp = dyn_cast<ICmpInst>(Condition);
  LLVM_DEBUG(dbgs() << "Branch Instruction Found: "; ICmp->dump();
             dbgs() << "\n");

  CmpInst::Predicate Pred = ICmp->getPredicate();
  Value *Op0 = ICmp->getOperand(0);
  Value *Op1 = ICmp->getOperand(1);
  if (ICmpInst::isLT(Pred) || ICmpInst::isLE(Pred)) {
    MinValue = getValue(Op0);
    MaxBoundry = getValue(Op1);
  } else if (Pred == CmpInst::Predicate::ICMP_EQ) {
    // assume the upper loop bound does not change, so that the max bound has no
    // SCEV.
    if (SE->isSCEVable(Op0->getType()) && SE->getExistingSCEV(Op0) &&
        SE->isSCEVable(Op1->getType()) && SE->getExistingSCEV(Op1)) {
      MinValue = nullptr;
      MaxBoundry = nullptr;
      return;
    }
    if (SE->isSCEVable(Op0->getType()) && SE->getExistingSCEV(Op0)) {
      if (!MinValue)
        MinValue = getMinBoundInEqualityComparison(Op0);

      MaxBoundry = getValue(Op1);
    } else {
      if (!MinValue)
        MinValue = getMinBoundInEqualityComparison(Op1);

      MaxBoundry = getValue(Op0);
    }

  } else {
    MinValue = getValue(Op1);
    MaxBoundry = getValue(Op0);
  }
  LLVM_DEBUG(dbgs() << "MinValue = "; MinValue->dump());
  LLVM_DEBUG(dbgs() << "MaxValue = "; MaxBoundry->dump());

  if (isa<Constant>(MaxBoundry)) {
    LLVM_DEBUG(dbgs() << "Iteration Variable (Max value) is an integer and "
                         "therefore no assumption "
                         "has to be added!");
    MinValue = nullptr;
    MaxBoundry = nullptr;
    return;
  }

  // if minimum value is not zero, assumptions will not help rotation, therefore
  // do nothing
  //
  if (!isa<Constant>(MinValue)) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning:: Annotation with non-constant "
                         "Minimum Values "
                         "is currently not supported! Found ";
               MinValue->getType()->dump());
    MinValue = nullptr;
    MaxBoundry = nullptr;
    return;
  }
}

const SCEV *LoopMetadata::getSCEV() const {
  if (SE->isSCEVable(LoopBound0->getType())) {
    const SCEV *S = SE->getSCEV(LoopBound0);
    if (S && S->getSCEVType() == SCEVTypes::scAddRecExpr)
      return S;
  }
  if (LoopBound1 && SE->isSCEVable(LoopBound1->getType())) {
    const SCEV *S = SE->getSCEV(LoopBound1);
    if (S && S->getSCEVType() == SCEVTypes::scAddRecExpr)
      return S;
  }

  return nullptr;
}

void LoopMetadata::addAssumeToLoopHeader(uint64_t MinIterCount,
                                         LLVMContext *Context) {
  LLVM_DEBUG(dbgs() << "Processing Loop Metadata: "
                    << L->getHeader()->getParent()->getName() << " "
                    << L->getName() << " (" << MinIterCount << ")\n");

  ICmpInst *CombInstr = dyn_cast<ICmpInst>(
      dyn_cast<BranchInst>(L->getExitingBlock()->getTerminator())
          ->getCondition());
  LoopBound0 = dyn_cast<Instruction>(CombInstr->getOperand(0));
  LoopBound1 = dyn_cast<Instruction>(CombInstr->getOperand(1));
  LLVM_DEBUG(dbgs() << "Compare Instructions \nOperand0"; LoopBound0->dump());
  LLVM_DEBUG(if (LoopBound1) {
    dbgs() << " Operand1";
    LoopBound1->dump();
  });

  // get Scalar Evolution of the loop counter
  const SCEV *S = getSCEV();
  if (!S) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning: Could not extract "
                         "SCEVAddRecExpr! Will not process Metadata\n");
    return;
  }

  Value *MinIterValue = getMinIterValue(S, MinIterCount, Context);
  if (!MinIterValue) {
    LLVM_DEBUG(
        dbgs()
        << "LoopMetadata-Warning: Could not extract Minimum Iteration Value\n");
    return;
  }

  getBoundries();
  if (!MaxBoundry) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning: Could not find Iteration "
                         "Variable. Will not process Metadata\n");
    return;
  }

  LLVM_DEBUG(dbgs() << "Min Iteration Value  : "; MinIterValue->dump());
  LLVM_DEBUG(dbgs() << "Max Value            : "; MaxBoundry->dump());

  IRBuilder<> Builder(L->getHeader()->getTerminator());

  Value *Cmp = nullptr;
  Cmp = Builder.CreateICmpSGT(MaxBoundry, MinIterValue);

  LLVM_DEBUG(dbgs() << "Inserting Condition:"; MinIterValue->dump();
             dbgs() << "With Comparator:"; Cmp->dump());

  // Insert the `llvm.assume` Call
  Function *AssumeFn =
      Intrinsic::getDeclaration(L->getHeader()->getModule(), Intrinsic::assume);
  CallInst *Call = Builder.CreateCall(AssumeFn, Cmp);
  Call->setTailCall(true);
  AC->registerAssumption(dyn_cast<AssumeInst>(Call));
}
