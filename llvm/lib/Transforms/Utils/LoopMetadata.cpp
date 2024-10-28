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
bool isRotatable(const Loop *L);
bool hasSCEVOperands(ScalarEvolution *SE, Value *Op);
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
  LLVM_DEBUG(dbgs() << "Preheader:");
  if (L.getLoopPreheader())
    LLVM_DEBUG(dbgs() << L.getLoopPreheader()->getName());
  LLVM_DEBUG(dbgs() << "\nHeader:");
  if (L.getHeader())
    LLVM_DEBUG(dbgs() << L.getHeader()->getName());
  LLVM_DEBUG(dbgs() << "\n");

  if (MinIterCount.has_value() && MinIterCount.value() > 0) {
    this->MinIterCount = MinIterCount.value();

    if (!isRotatable(this->L)) {
      LLVM_DEBUG(dbgs() << "Processing Loop Metadata: "
                        << L.getHeader()->getParent()->getName() << " "
                        << L.getName() << " (" << MinIterCount.value()
                        << ")\nAborting Metadata due to not rotatable!\n");
      return false;
    }

    LLVM_DEBUG(L.getHeader()->getParent()->dump(););

    addAssumeToLoopHeader(MinIterCount.value(), Context);
    return true;
  }

  return false;
}

/// check basic loop rotation conditions
bool isRotatable(const Loop *L) {
  BranchInst *BI = dyn_cast<BranchInst>(L->getHeader()->getTerminator());
  return L->isLoopExiting(L->getHeader()) && (BI && BI->isConditional());
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
  LoopStepSize = SCEVConst->getValue()->getSExtValue();
  IsLoopIncrementing = LoopStepSize > 0;
  return true;
}

Value *LoopMetadata::calcMinIterValue(const SCEV *S, int MinIterCount,
                                      LLVMContext *Context) {
  assert(MinIterCount > 0);

  // extract loop counter increment/decrement
  const SCEVAddRecExpr *AR = dyn_cast<SCEVAddRecExpr>(S);
  if (!AR) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning: S i not a SCEVAddRecExpr ";
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

  llvm::ConstantInt *ConstIncValue = llvm::ConstantInt::get(
      llvm::Type::getInt32Ty(*Context), MinIterValue, true);
  return static_cast<llvm::Value *>(ConstIncValue);
}

bool hasSCEVOperands(ScalarEvolution *SE, Value *Op) {
  return SE->isSCEVable(Op->getType()) && SE->getExistingSCEV(Op);
}

bool LoopMetadata::validateBounds() {
  if (!LowerBoundary || !UpperBoundary) {
    LowerBoundary = nullptr;
    UpperBoundary = nullptr;
    return false;
  }

  LLVM_DEBUG(dbgs() << "MinValue = "; LowerBoundary->dump());
  LLVM_DEBUG(dbgs() << "MaxValue = "; UpperBoundary->dump());

  if (isa<Instruction>(UpperBoundary)) {
    BasicBlock *MaxBB = dyn_cast<Instruction>(UpperBoundary)->getParent();
    if (MaxBB && !DT->dominates(MaxBB, L->getHeader())) {
      LLVM_DEBUG(
          dbgs() << "LoopMetadata-Warning: MaxBoundry is not in the same "
                    "BB as the Header ("
                 << L->getHeader()->getName() << ")\nMaxBoundry =";
          UpperBoundary->dump(); if (MaxBB) {
            dbgs() << "MaxBoundry BB = " << MaxBB->getName() << "\n";
          });

      LowerBoundary = nullptr;
      UpperBoundary = nullptr;
      return false;
    }
  }

  if (isa<Constant>(UpperBoundary)) {
    LLVM_DEBUG(dbgs() << "Iteration Variable (Max value) is an integer and "
                         "therefore no assumption "
                         "has to be added!");
    LowerBoundary = nullptr;
    UpperBoundary = nullptr;
    return false;
  }

  if (!isa<Constant>(LowerBoundary)) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning:: Annotation with non-constant "
                         "Minimum Values "
                         "is currently not supported! Found ";
               LowerBoundary->getType()->dump());
    LowerBoundary = nullptr;
    UpperBoundary = nullptr;
    return false;
  }
  return true;
}

/// FIXME: call getLoopInvariantInTruncExpr something like this.
///  get loop invariant constant (that is either high or low), but a boundry
Value *LoopMetadata::getUpperTruncatedBound() const {
  for (Value *Op : LoopCmpInstr->operands()) {
    if (SE->isSCEVable(Op->getType()) &&
        SCEVCastExpr::classof(SE->getSCEV(Op))) {
      continue;
    }

    return Op;
  }
  return nullptr;
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
      UpperBoundary = getUpperTruncatedBound();
    } else {
      if ((SE->isSCEVable(Op0->getType()) &&
           (SCEVAddExpr::classof(SE->getSCEV(Op0)) ||
            SCEVAddRecExpr::classof(SE->getSCEV(Op0)))) ||
          isa<PHINode>(Op0))
        UpperBoundary = Op1;
      else
        UpperBoundary = Op0;
    }

  } else {

    if (SCEVConst)
      UpperBoundary = SCEVConst->getValue();

    if (const SCEVUnknown *S = dyn_cast<SCEVUnknown>(AR->getStart()))
      UpperBoundary = S->getValue();

    if (IsTruncatedSCEV) {
      UpperBoundary = getUpperTruncatedBound();
    } else {
      if ((SE->isSCEVable(Op0->getType()) &&
           (SCEVAddExpr::classof(SE->getSCEV(Op0)) ||
            SCEVAddRecExpr::classof(SE->getSCEV(Op0)))) ||
          isa<PHINode>(Op0))
        LowerBoundary = Op1;
      else
        LowerBoundary = Op0;
    }
  }

  validateBounds();
}

const SCEV *LoopMetadata::getSCEV() {
  for (Value *Op : LoopCmpInstr->operands()) {
    if (SE->isSCEVable(Op->getType())) {
      const SCEV *S = SE->getSCEV(Op);
      if (S && S->getSCEVType() == SCEVTypes::scAddRecExpr)
        return S;
    }
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
  return std::abs(dyn_cast<SCEVConstant>(S)->getValue()->getSExtValue());
}

Value *getSCEVStart(const SCEVTruncateExpr *S, const BasicBlock *LoopPreHeader,
                    const ScalarEvolution *SE) {

  Value *StartVal = dyn_cast<SCEVUnknown>(S->getOperand())->getValue();

  if (!StartVal)
    return nullptr;

  PHINode *PN = dyn_cast<PHINode>(dyn_cast<Instruction>(StartVal));
  if (!PN)
    return nullptr;

  for (uint Op = 0; Op < PN->getNumOperands(); Op++) {
    if (PN->getIncomingBlock(Op)->getName() == LoopPreHeader->getName() &&
        isa<Constant>(PN->getOperand(Op))) {
      return PN->getOperand(Op);
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

  if (!fitstype(MinIterCount * StepSize, TruncExpr->getType()))
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

void LoopMetadata::matchCompareTypes(Value *MinIterValue,
                                     IRBuilder<> &Builder) {
  // ensure equal types in the comparison
  if (UpperBoundary->getType() != MinIterValue->getType()) {
    if (MinIterValue->getType()->getScalarSizeInBits() <
        UpperBoundary->getType()->getScalarSizeInBits()) {
      MinIterValue = Builder.CreateSExt(MinIterValue, UpperBoundary->getType());
      LLVM_DEBUG(dbgs() << "Type Matching for Minimum Iteration Value "
                        << MinIterValue);
    } else {
      UpperBoundary =
          Builder.CreateSExt(UpperBoundary, MinIterValue->getType());
      LLVM_DEBUG(dbgs() << "Type Matching for Upper loop Boundary "
                        << UpperBoundary);
    }
  }
}

void LoopMetadata::addAssumeToLoopHeader(uint64_t MinIterCount,
                                         LLVMContext *Context) {
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

  // reset Loop specific information;
  LowerBoundary = nullptr;
  UpperBoundary = nullptr;
  IsTruncatedSCEV = false;

  // get Scalar Evolution of the loop counter
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

  Value *Cmp = nullptr;
  matchCompareTypes(MinIterValue, Builder);
  Cmp = Builder.CreateICmpSGT(UpperBoundary, MinIterValue);

  // Insert the `llvm.assume` Call
  Function *AssumeFn =
      Intrinsic::getDeclaration(L->getHeader()->getModule(), Intrinsic::assume);
  CallInst *Call = Builder.CreateCall(AssumeFn, Cmp);
  Call->setTailCall(true);
  AC->registerAssumption(dyn_cast<AssumeInst>(Call));

  LLVM_DEBUG(dbgs() << "Inserting Condition :"; MinIterValue->dump();
             dbgs() << "With Comparator   :"; Cmp->dump();
             dbgs() << "Assume            :"; Call->dump());
}
