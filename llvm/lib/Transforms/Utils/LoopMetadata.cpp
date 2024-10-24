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
    LLVM_DEBUG(dbgs() << "Dumping Full Function:"
                      << L.getHeader()->getParent()->getName() << "\n";
               L.getHeader()->getParent()->dump(););
    return true;
  }

  return false;
}

// check basic loop rotation conditions
bool isRotatable(const Loop *L) {
  BranchInst *BI = dyn_cast<BranchInst>(L->getHeader()->getTerminator());
  return L->isLoopExiting(L->getHeader()) && (BI && BI->isConditional());
}

void LoopMetadata::calcIncrement(const SCEV *S) {
  IsLoopIncrementing = false;
  switch (S->getSCEVType()) {
  case scAddRecExpr: {
    const SCEVAddRecExpr *AR = cast<SCEVAddRecExpr>(S);
    IsLoopIncrementing =
        cast<SCEVConstant>(*AR->getOperand(1)).getValue()->getSExtValue() > 0;
    break;
  }

  default:
    assert(false && "Could not extract Iteration Variable from ");
  }
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
  if (IsLoopIncrementing)
    MaxValue--;

  // If the loop does not start at 0, add the loop start to the Minimum
  // Iteration Value
  assert(isa<Constant>(MinBoundry));
  int LoopStart =
      dyn_cast<Constant>(MinBoundry)->getUniqueInteger().getSExtValue();
  MaxValue += LoopStart;

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

bool hasSCEVOperands(ScalarEvolution *SE, Value *Op) {
  return SE->isSCEVable(Op->getType()) && SE->getExistingSCEV(Op);
}

bool LoopMetadata::assignBoundsInEqualComparison(Value *Op0, Value *Op1) {
  if (hasSCEVOperands(SE, Op0) && hasSCEVOperands(SE, Op1)) {
    // since both operands have SCEV, we cannot derive any information about the
    // (fixed) maximum bound of the loop
    MinBoundry = nullptr;
    MinBoundry = nullptr;
    LLVM_DEBUG(
        dbgs()
        << "LoopMetadata-Warning: Both Condition Operands have a SCEV, however "
           "the maximum value is expected to not have a SCEV since this pass "
           "assumes that it is loop invariant.\nWill not add loop Metadata.\n");
    return false;
  }
  Value *LoopVariant = nullptr;
  Value *LoopInVariant = nullptr;
  if (hasSCEVOperands(SE, Op0)) {
    LoopVariant = getLoopVariantInEqualityComparison(Op0);
    LoopInVariant = getValue(Op1);
  } else {
    LoopVariant = getLoopVariantInEqualityComparison(Op1);
    LoopInVariant = getValue(Op0);
  }

  if (IsLoopIncrementing) {
    MinBoundry = LoopVariant;
    MaxBoundry = LoopInVariant;
  } else {
    MinBoundry = LoopInVariant;
    MaxBoundry = LoopVariant;
  }
  return true;
}

Value *LoopMetadata::getLoopVariantInEqualityComparison(Value *Op) const {
  // Assumption: IV is incremented or decremented by a fixed
  Instruction *Instr = dyn_cast<Instruction>(Op);
  if (Instr->getNumOperands() != 2) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning: Instruction not supported!";
               Instr->dump());
    return nullptr;
  }

  PHINode *P0 = dyn_cast<PHINode>(Instr->getOperand(0));
  PHINode *P1 = dyn_cast<PHINode>(Instr->getOperand(1));
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
    return getValue(Instr->getOperand(0));
  if (P1)
    return getValue(Instr->getOperand(1));

  LLVM_DEBUG(
      dbgs() << "LoopMetadata-Warning: No Operand is a Phi nodes. This pass "
                "assumes only one Phi node! Abort Metadata annotation.";
      Instr->getOperand(0)->dump(); Instr->getOperand(1)->dump());
  return nullptr;
}

bool LoopMetadata::checkBoundries() {
  if (!MinBoundry || !MaxBoundry) {
    MinBoundry = nullptr;
    MaxBoundry = nullptr;
    return false;
  }

  LLVM_DEBUG(dbgs() << "MinValue = "; MinBoundry->dump());
  LLVM_DEBUG(dbgs() << "MaxValue = "; MaxBoundry->dump());

  if (isa<Instruction>(MaxBoundry)) {
    BasicBlock *MaxBB = dyn_cast<Instruction>(MaxBoundry)->getParent();
    if (MaxBB && !DT->dominates(MaxBB, L->getHeader())) {
      LLVM_DEBUG(
          dbgs() << "LoopMetadata-Warning: MaxBoundry is not in the same "
                    "BB as the Header ("
                 << L->getHeader()->getName() << ")\nMaxBoundry =";
          MaxBoundry->dump(););
      if (MaxBB)
        LLVM_DEBUG(dbgs() << "MaxBoundry BB = " << MaxBB->getName() << "\n";);
      MinBoundry = nullptr;
      MaxBoundry = nullptr;
      return false;
    }
  }

  if (isa<Constant>(MaxBoundry)) {
    LLVM_DEBUG(dbgs() << "Iteration Variable (Max value) is an integer and "
                         "therefore no assumption "
                         "has to be added!");
    MinBoundry = nullptr;
    MaxBoundry = nullptr;
    return false;
  }

  if (!isa<Constant>(MinBoundry)) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning:: Annotation with non-constant "
                         "Minimum Values "
                         "is currently not supported! Found ";
               MinBoundry->getType()->dump());
    MinBoundry = nullptr;
    MaxBoundry = nullptr;
    return false;
  }
  return true;
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

  if (Pred == CmpInst::Predicate::ICMP_EQ) {
    assignBoundsInEqualComparison(Op0, Op1);
    checkBoundries();
    return;
  }

  if (ICmpInst::isLT(Pred) || ICmpInst::isLE(Pred)) {
    if (!MinBoundry)
      MinBoundry = getValue(Op0);
    MaxBoundry = getValue(Op1);
  } else {
    if (!MinBoundry)
      MinBoundry = getValue(Op1);
    MaxBoundry = getValue(Op0);
  }

  checkBoundries();
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

bool fitstype(unsigned MinValue, const Type *T) {
  return (uint64_t)MinValue <
         *APInt::getSignedMaxValue(T->getIntegerBitWidth()).getRawData();
}

const SCEVAddExpr *getAddExpr(const Instruction *Instr, ScalarEvolution *SE) {
  for (unsigned I = 0; I < Instr->getNumOperands(); I++) {
    Instr->getOperand(I)->dump();
    PHINode *PN = dyn_cast<PHINode>(Instr->getOperand(I));

    const SCEVAddExpr *AddExpr = nullptr;
    if (PN) {
      // find loop variant Operand, which coincides with SCEV
      for (Value *Op : PN->operands())
        AddExpr = dyn_cast<SCEVAddExpr>(SE->getSCEV(Op));
      if (AddExpr)
        return AddExpr;
    } else {
      AddExpr = dyn_cast<SCEVAddExpr>(SE->getSCEV(Instr->getOperand(I)));
      if (AddExpr)
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

  const SCEV *SIntern = SE->getSCEV(I);
  LLVM_DEBUG(dbgs() << "SCEV "; SIntern->dump());
  LLVM_DEBUG(dbgs() << SIntern->getSCEVType() << "\n");

  const SCEVAddExpr *AddExpr = getAddExpr(I, SE);
  if (!AddExpr)
    return nullptr;

  const SCEVTruncateExpr *TruncExpr = getSCEVTruncate(SIntern);
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
  const SCEV *S = SE->getAddRecExpr(
      SCEVStart, Step, L, llvm::SCEVAddExpr::NoWrapFlags::FlagAnyWrap);
  // min boundry already found, so assign it early
  MinBoundry = Start;
  LLVM_DEBUG(dbgs() << "Found SCEV "; S->dump(); dbgs() << "and MinValue ";
             MinBoundry->dump());
  return S;
}

const SCEV *LoopMetadata::getTruncInductionSCEV() {
  const SCEV *TruncSCEV = extractSCEVFromTruncation(LoopBound0);
  if (TruncSCEV)
    return TruncSCEV;

  return extractSCEVFromTruncation(LoopBound1);
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
  MinBoundry = nullptr;
  MaxBoundry = nullptr;

  // get Scalar Evolution of the loop counter
  const SCEV *S = getSCEV();
  if (!S) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning: Could not extract "
                         "SCEVAddRecExpr! Will not process Metadata\n");
    return;
  }

  calcIncrement(S);

  getBoundries();
  if (!MaxBoundry) {
    LLVM_DEBUG(dbgs() << "LoopMetadata-Warning: Could not find Iteration "
                         "Variable. Will not process Metadata\n");
    return;
  }

  Value *MinIterValue = getMinIterValue(S, MinIterCount, Context);
  if (!MinIterValue) {
    LLVM_DEBUG(
        dbgs()
        << "LoopMetadata-Warning: Could not extract Minimum Iteration Value\n");
    return;
  }

  LLVM_DEBUG(dbgs() << "Min Iteration Value  : "; MinIterValue->dump());
  LLVM_DEBUG(dbgs() << "Max Value            : "; MaxBoundry->dump());

  IRBuilder<> Builder(L->getHeader()->getTerminator());

  Value *Cmp = nullptr;
  // ensure equalize types in the comparison
  if (MaxBoundry->getType() != MinIterValue->getType()) {
    if (MinIterValue->getType()->getScalarSizeInBits() <
        MaxBoundry->getType()->getScalarSizeInBits()) {
      MinIterValue = Builder.CreateSExt(MinIterValue, MaxBoundry->getType());
    } else {
      MaxBoundry = Builder.CreateSExt(MaxBoundry, MinIterValue->getType());
    }
  }
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
