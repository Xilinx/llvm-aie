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

///  Check if the minimum value fits into the given type
bool fitstype(unsigned MinValue, const Type *T);

/// extract the SCEVAddExpr that is in the operands of the Instr
const SCEVAddExpr *getAddExpr(const Instruction *Instr, ScalarEvolution *SE);

/// get SCEVTruncExpr from the SCEV S
const SCEVTruncateExpr *getSCEVTruncate(const SCEV *S);

/// extract the step size by which the loop IV changes.
int getSCEVStepSize(const SCEV *S);

/// based on the SCEVTruncateExpr extract the Loop Start Value
Value *getSCEVStart(const SCEVTruncateExpr *S, const BasicBlock *LoopPreHeader,
                    const ScalarEvolution *SE);

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

Value *LoopMetadata::getTruncatedLoopInvariant() const {
  for (Value *Op : LoopCmpInstr->operands()) {
    if (SE->isSCEVable(Op->getType()) &&
        SCEVCastExpr::classof(SE->getSCEV(Op))) {
      continue;
    }
    return Op;
  }
  return nullptr;
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
    if (IsTruncatedSCEV) {
      UpperBoundary = getTruncatedLoopInvariant();
    } else {
      UpperBoundary = getLoopInvariant(Op0, Op1, SE);
    }

  } else {

    if (SCEVConst)
      UpperBoundary = SCEVConst->getValue();

    if (const SCEVUnknown *S = dyn_cast<SCEVUnknown>(AR->getStart()))
      UpperBoundary = S->getValue();

    if (IsTruncatedSCEV) {
      LowerBoundary = getTruncatedLoopInvariant();
    } else {
      LowerBoundary = getLoopInvariant(Op0, Op1, SE);
    }
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

  return getTruncatedSCEV();
}

///  Check if the minimum value fits into the given type
bool fitstype(unsigned MinValue, const Type *T) {
  return (uint64_t)MinValue <
         *APInt::getSignedMaxValue(T->getIntegerBitWidth()).getRawData();
}

const SCEVAddExpr *getAddExpr(const Instruction *Instr, ScalarEvolution *SE) {
  for (unsigned I = 0; I < Instr->getNumOperands(); I++) {
    PHINode *PN = dyn_cast<PHINode>(Instr->getOperand(I));

    // find loop variant Operand, which coincides with SCEV
    if (PN) {
      for (Value *Op : PN->operands())
        if (const SCEVAddExpr *AddExpr = dyn_cast<SCEVAddExpr>(SE->getSCEV(Op)))
          return AddExpr;
    } else {
      if (const SCEVAddExpr *AddExpr =
              dyn_cast<SCEVAddExpr>(SE->getSCEV(Instr->getOperand(I))))
        return AddExpr;
    }
  }
  return nullptr;
}

const SCEVTruncateExpr *getSCEVTruncate(const SCEV *S) {
  const SCEVZeroExtendExpr *Zext = dyn_cast<SCEVZeroExtendExpr>(S);
  if (Zext && SCEVTruncateExpr::classof(Zext->getOperand(0))) {
    return dyn_cast<SCEVTruncateExpr>(Zext->getOperand(0));
  }
  return nullptr;
}

int getSCEVStepSize(const SCEV *S) {
  assert(isa<SCEVConstant>(S));
  return dyn_cast<SCEVConstant>(S)->getValue()->getSExtValue();
}

Value *getSCEVStart(const SCEVTruncateExpr *S, const BasicBlock *LoopPreHeader,
                    ScalarEvolution *SE) {

  Value *StartVal = dyn_cast<SCEVUnknown>(S->getOperand())->getValue();

  if (!StartVal)
    return nullptr;

  PHINode *PN = dyn_cast<PHINode>(dyn_cast<Instruction>(StartVal));
  if (!PN)
    return nullptr;

  // get operand that is defined outside of the loop and that has not a scalar
  // evolution
  for (uint Index = 0; Index < PN->getNumOperands(); Index++) {
    Value *Op = PN->getOperand(Index);
    // Operand must strongly dominate
    if (!SE->isSCEVable(Op->getType()))
      continue;

    const SCEV *OpSCEV = SE->getSCEV(Op);
    if (PN->getIncomingBlock(Index)->getName() == LoopPreHeader->getName() &&
        (SCEVConstant::classof(OpSCEV) || SCEVUnknown::classof(OpSCEV))) {
      return PN->getOperand(Index);
    }
  }

  return nullptr;
}

const SCEV *LoopMetadata::extractSCEVFromTruncation(Instruction *I) {
  if (!I || !SE->isSCEVable(I->getType()))
    return nullptr;

  const SCEV *S = SE->getSCEV(I);
  LLVM_DEBUG(dbgs() << "SCEV "; S->dump());
  LLVM_DEBUG(dbgs() << S->getSCEVType() << "\n");

  const SCEVAddExpr *AddExpr = getAddExpr(I, SE);
  if (!AddExpr)
    return nullptr;

  const SCEVTruncateExpr *TruncExpr = getSCEVTruncate(S);
  if (!TruncExpr)
    return nullptr;

  const SCEV *AddExprOp = AddExpr->getOperand(0);
  if (!isa<SCEVConstant>(AddExprOp))
    return nullptr;
  const int StepSize = getSCEVStepSize(AddExprOp);

  Value *Start = getSCEVStart(TruncExpr, L->getLoopPreheader(), SE);
  if (!Start)
    return nullptr;

  if (!fitstype(MinIterCount * std::abs(StepSize), TruncExpr->getType()))
    return nullptr;

  const SCEV *Step =
      SE->getConstant(TruncExpr->getOperand()->getType(), StepSize, true);

  const SCEV *SCEVStart = SE->getSCEV(Start);
  const SCEV *AddRecExpr = SE->getAddRecExpr(
      SCEVStart, Step, L, llvm::SCEVAddExpr::NoWrapFlags::FlagAnyWrap);

  // min boundry already found, so assign it early
  LowerBoundary = Start;
  LLVM_DEBUG(dbgs() << "Found SCEV "; AddRecExpr->dump();
             dbgs() << "and Lower bound "; LowerBoundary->dump());

  return AddRecExpr;
}

const SCEV *LoopMetadata::getTruncatedSCEV() {
  for (Value *Op : LoopCmpInstr->operands()) {
    if (const SCEV *TruncSCEV =
            extractSCEVFromTruncation(dyn_cast<Instruction>(Op))) {
      IsTruncatedSCEV = true;
      return TruncSCEV;
    }
  }
  return nullptr;
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
  IsTruncatedSCEV = false;

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
