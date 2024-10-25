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

// check basic loop rotation conditions
bool isRotatable(const Loop *L) {
  BranchInst *BI = dyn_cast<BranchInst>(L->getHeader()->getTerminator());
  return L->isLoopExiting(L->getHeader()) && (BI && BI->isConditional());
}

bool LoopMetadata::calcIncrement(const SCEV *S) {
  if (const SCEVAddRecExpr *AR = cast<SCEVAddRecExpr>(S)) {
    IsLoopIncrementing =
        cast<SCEVConstant>(*AR->getOperand(1)).getValue()->getSExtValue() > 0;
    return true;
  }

  return false;
}

Value *LoopMetadata::getMinIterValue(const SCEV *S, int MinIterCount,
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

  int IncValue = cast<SCEVConstant>(ConstExpr)->getValue()->getSExtValue();

  int MinIterValue = std::abs(IncValue * MinIterCount);

  // to emulate builtin_assume, subtract MinIterValue by 1 if loop is
  // incrementing
  if (IsLoopIncrementing)
    MinIterValue--;

  // If the loop does not start at 0, add the loop start to the Minimum
  // Iteration Value
  assert(isa<Constant>(MinBoundary));
  int LoopStart =
      dyn_cast<Constant>(MinBoundary)->getUniqueInteger().getSExtValue();
  MinIterValue += LoopStart;

  llvm::ConstantInt *ConstIncValue = llvm::ConstantInt::get(
      llvm::Type::getInt32Ty(*Context), MinIterValue, true);
  return static_cast<llvm::Value *>(ConstIncValue);
}

Value *findLoopInvariantValue(Value *V, const Loop *L, DominatorTree *DT) {
  Instruction *Op = dyn_cast<Instruction>(V);
  if (!Op)
    return V;

  // if Op0 is from a previous block, this is the loop invariant part
  if (DT->dominates(Op->getParent(), L->getHeader()))
    return V;

  return nullptr;
}

Value *LoopMetadata::getLoopInvariantValue(Value *V) const {
  PHINode *MaxPHI = dyn_cast_or_null<PHINode>(V);
  if (!MaxPHI)
    return V;

  for (Value *Op : MaxPHI->operands()) {
    if (Value *LoopInvariant = findLoopInvariantValue(Op, L, DT))
      return LoopInvariant;
  }

  return nullptr;
}

bool hasSCEVOperands(ScalarEvolution *SE, Value *Op) {
  return SE->isSCEVable(Op->getType()) && SE->getExistingSCEV(Op);
}

bool LoopMetadata::assignBoundsInEqualComparison(Value *Op0, Value *Op1) {
  if (hasSCEVOperands(SE, Op0) && hasSCEVOperands(SE, Op1)) {
    // since both operands have SCEV, we cannot derive any information about
    // the (fixed) maximum bound of the loop
    MinBoundary = nullptr;
    MinBoundary = nullptr;
    LLVM_DEBUG(
        dbgs()
        << "LoopMetadata-Warning: Both Condition Operands have a SCEV, "
           "however "
           "the maximum value is expected to not have a SCEV since this pass "
           "assumes that it is loop invariant.\nWill not add loop "
           "Metadata.\n");
    return false;
  }

  Value *LoopVariant = nullptr;
  Value *LoopInVariant = nullptr;
  if (hasSCEVOperands(SE, Op0)) {
    LoopVariant = getLoopVariantInEqualityComparison(Op0);
    LoopInVariant = getLoopInvariantValue(Op1);
  } else {
    LoopVariant = getLoopVariantInEqualityComparison(Op1);
    LoopInVariant = getLoopInvariantValue(Op0);
  }

  if (IsLoopIncrementing) {
    MinBoundary = LoopVariant;
    MaxBoundary = LoopInVariant;
  } else {
    MinBoundary = LoopInVariant;
    MaxBoundary = LoopVariant;
  }
  return true;
}

Value *LoopMetadata::getLoopVariantInEqualityComparison(Value *Op) const {
  assert(isa<Instruction>(Op));
  // Assumption: IV is incremented or decremented by a fixed amount
  Instruction *Instr = dyn_cast<Instruction>(Op);
  if (Instr->getNumOperands() != 2) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning: Instruction not supported!";
               Instr->dump());
    return nullptr;
  }

  Value *Op0 = Instr->getOperand(0);
  PHINode *P0 = dyn_cast<PHINode>(Op0);
  Value *Op1 = Instr->getOperand(1);
  PHINode *P1 = dyn_cast<PHINode>(Op1);
  if (P0 && P1) {
    // Assumption: the Loop IV will increment or decrement by a fixed value.
    // if this however cannot be determined, prefer to not extract any
    // information
    LLVM_DEBUG(
        dbgs()
            << "LoopMetadata-Warning: Both Operands are Phi nodes. This pass "
               "assumes only one Phi node! Abort Metadata annotation.";
        Instr->getOperand(0)->dump(); Instr->getOperand(1)->dump());
    return nullptr;
  }

  if (P0)
    return getLoopInvariantValue(P0);
  if (P1)
    return getLoopInvariantValue(P1);

  LLVM_DEBUG(
      dbgs() << "LoopMetadata-Warning: No Operand is a Phi nodes. This pass "
                "assumes only one Phi node! Abort Metadata annotation.";
      Instr->getOperand(0)->dump(); Instr->getOperand(1)->dump());
  return nullptr;
}

bool LoopMetadata::validateBounds() {
  if (!MinBoundary || !MaxBoundary) {
    MinBoundary = nullptr;
    MaxBoundary = nullptr;
    return false;
  }

  LLVM_DEBUG(dbgs() << "MinValue = "; MinBoundary->dump());
  LLVM_DEBUG(dbgs() << "MaxValue = "; MaxBoundary->dump());

  if (isa<Instruction>(MaxBoundary)) {
    BasicBlock *MaxBB = dyn_cast<Instruction>(MaxBoundary)->getParent();
    if (MaxBB && !DT->dominates(MaxBB, L->getHeader())) {
      LLVM_DEBUG(
          dbgs() << "LoopMetadata-Warning: MaxBoundry is not in the same "
                    "BB as the Header ("
                 << L->getHeader()->getName() << ")\nMaxBoundry =";
          MaxBoundary->dump(););
      if (MaxBB)
        LLVM_DEBUG(dbgs() << "MaxBoundry BB = " << MaxBB->getName() << "\n";);
      MinBoundary = nullptr;
      MaxBoundary = nullptr;
      return false;
    }
  }

  if (isa<Constant>(MaxBoundary)) {
    LLVM_DEBUG(dbgs() << "Iteration Variable (Max value) is an integer and "
                         "therefore no assumption "
                         "has to be added!");
    MinBoundary = nullptr;
    MaxBoundary = nullptr;
    return false;
  }

  if (!isa<Constant>(MinBoundary)) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning:: Annotation with non-constant "
                         "Minimum Values "
                         "is currently not supported! Found ";
               MinBoundary->getType()->dump());
    MinBoundary = nullptr;
    MaxBoundary = nullptr;
    return false;
  }
  return true;
}

void LoopMetadata::getBoundaries() {

  BranchInst *BI = dyn_cast<BranchInst>(L->getExitingBlock()->getTerminator());
  if (!BI)
    return;

  ICmpInst *ICmp = dyn_cast<ICmpInst>(BI->getCondition());
  if (!ICmp)
    return;
  LLVM_DEBUG(dbgs() << "Branch Instruction Found: "; ICmp->dump();
             dbgs() << "\n");

  CmpInst::Predicate Pred = ICmp->getPredicate();
  Value *Op0 = ICmp->getOperand(0);
  Value *Op1 = ICmp->getOperand(1);

  if (Pred == CmpInst::Predicate::ICMP_EQ) {
    assignBoundsInEqualComparison(Op0, Op1);
    validateBounds();
    return;
  }

  if (ICmpInst::isLT(Pred) || ICmpInst::isLE(Pred)) {
    if (!MinBoundary)
      MinBoundary = getLoopInvariantValue(Op0);
    MaxBoundary = getLoopInvariantValue(Op1);
  } else {
    if (!MinBoundary)
      MinBoundary = getLoopInvariantValue(Op1);
    MaxBoundary = getLoopInvariantValue(Op0);
  }

  validateBounds();
}

const SCEV *LoopMetadata::getSCEV() {
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

  return getTruncInductionSCEV();
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
  if (const SCEVZeroExtendExpr *Zext = dyn_cast<SCEVZeroExtendExpr>(S)) {
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

  if (StartVal) {
    PHINode *PN = dyn_cast<PHINode>(dyn_cast<Instruction>(StartVal));
    if (PN) {
      for (uint Op = 0; Op < PN->getNumOperands(); Op++) {
        LLVM_DEBUG(dbgs() << PN->getIncomingBlock(Op)->getName() << "\n");
        if (PN->getIncomingBlock(Op)->getName() == LoopPreHeader->getName()) {
          Value *V = PN->getOperand(Op);
          if (isa<Constant>(V)) {
            return V;
          }
        }
      }
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
  MinBoundary = Start;
  LLVM_DEBUG(dbgs() << "Found SCEV "; AddRecExpr->dump();
             dbgs() << "and MinValue "; MinBoundary->dump());
  return AddRecExpr;
}

const SCEV *LoopMetadata::getTruncInductionSCEV() {
  if (const SCEV *TruncSCEV = extractSCEVFromTruncation(LoopBound0))
    return TruncSCEV;

  return extractSCEVFromTruncation(LoopBound1);
}

void LoopMetadata::addAssumeToLoopHeader(uint64_t MinIterCount,
                                         LLVMContext *Context) {
  LLVM_DEBUG(dbgs() << "Processing Loop Metadata: "
                    << L->getHeader()->getParent()->getName() << " "
                    << L->getName() << " (" << MinIterCount << ")\n");

  BranchInst *BI = dyn_cast<BranchInst>(L->getExitingBlock()->getTerminator());
  if (!BI)
    return;

  ICmpInst *LoopCmpInstr = dyn_cast<ICmpInst>(BI->getCondition());
  if (!LoopCmpInstr)
    return;

  LLVM_DEBUG(dbgs() << "Branch Instruction Found: "; LoopCmpInstr->dump();
             dbgs() << "\n");
  LoopBound0 = dyn_cast<Instruction>(LoopCmpInstr->getOperand(0));
  LoopBound1 = dyn_cast<Instruction>(LoopCmpInstr->getOperand(1));
  LLVM_DEBUG(dbgs() << "Compare Instructions \nOperand0"; LoopBound0->dump());
  LLVM_DEBUG(if (LoopBound1) {
    dbgs() << " Operand1";
    LoopBound1->dump();
  });
  MinBoundary = nullptr;
  MaxBoundary = nullptr;

  // get Scalar Evolution of the loop counter
  const SCEV *S = getSCEV();
  if (!S) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning: Could not extract "
                         "SCEVAddRecExpr! Will not process Metadata\n");
    return;
  }

  if (!calcIncrement(S)) {
    LLVM_DEBUG(
        dbgs() << "LoopMetadata-Warning: Could not calculate "
                  "Increment/Decrement of Loop Counter. Will not process "
                  "Metadata\n");
    return;
  }

  getBoundaries();
  if (!MaxBoundary) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning: Could not find Iteration "
                         "Variable. Will not process Metadata\n");
    return;
  }

  Value *MinIterValue = getMinIterValue(S, MinIterCount, Context);
  if (!MinIterValue) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning: Could not extract Minimum "
                         "Iteration Value\n");
    return;
  }

  LLVM_DEBUG(dbgs() << "Min Iteration Value  : "; MinIterValue->dump());
  LLVM_DEBUG(dbgs() << "Max Value            : "; MaxBoundary->dump());

  IRBuilder<> Builder(L->getHeader()->getTerminator());

  Value *Cmp = nullptr;
  // ensure equalize types in the comparison
  if (MaxBoundary->getType() != MinIterValue->getType()) {
    if (MinIterValue->getType()->getScalarSizeInBits() <
        MaxBoundary->getType()->getScalarSizeInBits()) {
      MinIterValue = Builder.CreateSExt(MinIterValue, MaxBoundary->getType());
    } else {
      MaxBoundary = Builder.CreateSExt(MaxBoundary, MinIterValue->getType());
    }
  }
  Cmp = Builder.CreateICmpSGT(MaxBoundary, MinIterValue);

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
