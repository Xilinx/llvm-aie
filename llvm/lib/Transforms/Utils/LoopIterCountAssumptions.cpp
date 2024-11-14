//===-- LoopIterCountAssumptions.cpp - add Loop assumptions -----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass converts Loop Iteration Count Metadata to Assumptions which can be
// picked up by Loop Rotate to remove Loop Guards.
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/LoopIterCountAssumptions.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/MemorySSA.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/Casting.h"
#include "llvm/Transforms/Scalar/LICM.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"

#define DEBUG_TYPE "loop-iter-count-assumptions"

using namespace llvm;

namespace {

std::string getFunctionAndBlockNames(const BasicBlock &BB) {
  return BB.getParent()->getName().str() + " " + BB.getName().str();
}

/// Return the Branch Compare Instruction of CurrentLoop if the Loop is well
/// formed and this pass can process the Predicate
ICmpInst *getLoopCmpInst(const Loop &CurrentLoop) {

  if (CurrentLoop.isRotatedForm()) {
    LLVM_DEBUG(dbgs() << "Loop already in rotated form. Will not add Loop "
                         "Iteration Count assumptions.\n");
    return nullptr;
  }

  /// Check that the loop has a single Exiting Block. If the CurrentLoop
  /// has multiple Exiting Blocks, ExitBB will be a nullptr
  auto *ExitBB = CurrentLoop.getExitingBlock();
  if (!ExitBB)
    return nullptr;

  BranchInst *BI = dyn_cast<BranchInst>(ExitBB->getTerminator());
  if (!BI)
    return nullptr;

  ICmpInst *LoopCmpInstr = dyn_cast<ICmpInst>(BI->getCondition());
  if (!LoopCmpInstr)
    return nullptr;

  LLVM_DEBUG(dbgs() << "Condition Found: " << *LoopCmpInstr << "\n");
  return LoopCmpInstr;
}

bool hasVariableStepSize(Value &Op, ScalarEvolution &SE) {
  const SCEVAddRecExpr *AddRec =
      dyn_cast_or_null<SCEVAddRecExpr>(SE.getSCEV(&Op));
  if (!AddRec)
    return false;

  const SCEV *StepSize = AddRec->getStepRecurrence(SE);
  return !isa<SCEVConstant>(StepSize);
}

/// Return the AddRecExpr evaluated at Iteration \p IterCount if an
/// AddRecExpr can be extracted, otherwise return loop invariant Value of \p Op
Value *expandValueAtIteration(Value *Op, ScalarEvolution &SE,
                              SCEVExpander &Expander,
                              Instruction *InsertionPoint, Loop *CurrentLoop,
                              int64_t IterCount) {
  const SCEVAddRecExpr *AddRec =
      dyn_cast_or_null<SCEVAddRecExpr>(SE.getSCEV(Op));
  if (AddRec) {
    const SCEV *IterSCEV =
        AddRec->evaluateAtIteration(SE.getConstant(APInt(32, IterCount)), SE);

    // Copy Overflow Flags to SCEV
    SCEV::NoWrapFlags NWF = AddRec->getNoWrapFlags(
        SCEV::NoWrapFlags(/*Mask=*/SCEV::FlagNUW | SCEV::FlagNSW));

    // IterSCEV can either be an AddExpr or simplify to a MulExpr
    // (in the case of zero offset and a variable stepsize), therefore assign
    // Overflow Flags to every CommutativeExpr that will be generated from the
    // AddRecExpr evaluation
    // If IterSCEV can be evaluated to a constant, no need to add a Flag
    auto *CE = dyn_cast<SCEVCommutativeExpr>(const_cast<SCEV *>(IterSCEV));
    if (CE && NWF) {
      CE->setNoWrapFlags(NWF);
      IterSCEV = dyn_cast<const SCEV>(CE);
    }

    if (!Expander.isSafeToExpand(IterSCEV)) {
      LLVM_DEBUG(dbgs() << "LoopIterCountAssumptions-Warning: Cannot Expand "
                           "Iteration Scalar Evolution"
                        << *IterSCEV << "\n");
      return nullptr;
    }
    return Expander.expandCodeFor(IterSCEV, Op->getType(), InsertionPoint);
  }

  LLVM_DEBUG(dbgs() << "Could not extract AddRecExpr, will try to get loop "
                       "invariant Value of "
                    << *Op << "\n");

  if (CurrentLoop->isLoopInvariant(Op))
    return Op;

  LLVM_DEBUG(dbgs() << "Operand is loop variant " << *Op << "\n");
  return nullptr;
}

/// Try to create an assumption into the Loop PreHeader, that at iteration
/// \p IterCount the condition is true
void tryInsertIterationAssumption(ICmpInst &LoopCmpInstr, Loop &CurrentLoop,
                                  int64_t IterCount, ScalarEvolution &SE,
                                  AssumptionCache &AC) {

  if (!CurrentLoop.getLoopPreheader()) {
    LLVM_DEBUG(dbgs() << "LoopIterCountAssumptions-Warning: Loop has no "
                         "preheader, will not insert Assumption!\n");
    return;
  }

  Instruction *InsertionPoint = CurrentLoop.getLoopPreheader()->getTerminator();
  LLVM_DEBUG(dbgs() << "Inserting Assumption with IterCount " << IterCount
                    << " before: " << *InsertionPoint << "\n");

  // LoopRotate uses SimplifyQuery to determine, if a Branch is conditional or
  // not. SimplifyQuery can only take an Assumption into account, if it is
  // before the to-be-evaluated Compare Instruction. Here they are inserted into
  // the Preheader, so that the assumption is only valid once and not on every
  // entry of the Loop Header.
  IRBuilder<> Builder(dyn_cast<Instruction>(InsertionPoint));

  SCEVExpander Expander(
      SE, CurrentLoop.getLoopPreheader()->getModule()->getDataLayout(),
      "expanded");

  Value *LHS = expandValueAtIteration(LoopCmpInstr.getOperand(0), SE, Expander,
                                      InsertionPoint, &CurrentLoop, IterCount);
  if (!LHS)
    return;
  LLVM_DEBUG(dbgs() << "LHS = " << *LHS << "\n");

  Value *RHS = expandValueAtIteration(LoopCmpInstr.getOperand(1), SE, Expander,
                                      InsertionPoint, &CurrentLoop, IterCount);

  if (!RHS)
    return;
  LLVM_DEBUG(dbgs() << "RHS = " << *RHS << "\n");

  // If the false-branch-target is to the Loop Body, inverse the
  // predicate, since the Loop Condition is inversed to remain in the Loop
  CmpInst::Predicate Pred = LoopCmpInstr.getPredicate();
  if (!CurrentLoop.contains(
          dyn_cast<BranchInst>(CurrentLoop.getExitingBlock()->getTerminator())
              ->getSuccessor(0)))
    Pred = LoopCmpInstr.getInversePredicate();

  Value *Cmp = Builder.CreateICmp(Pred, LHS, RHS);

  // Insert Assumption
  CallInst *Assumption = Builder.CreateAssumption(Cmp);
  AC.registerAssumption(dyn_cast<AssumeInst>(Assumption));
  LLVM_DEBUG(dbgs() << "With Comparator             :" << *Cmp << "\n"
                    << "Assume                      :" << *Assumption << "\n");
}

/// Determine if the \param CurrentLoop is not rotated yet and Loop Iteration
/// Count Metadata is greater than 0. \return Minimum Iteration Count of the
/// Loop
std::optional<int64_t> getValidMinIterCount(Loop &CurrentLoop) {
  BasicBlock *LoopHeader = CurrentLoop.getHeader();

  // Dump loop summary
  LLVM_DEBUG(if (CurrentLoop.getLoopPreheader()) {
    dbgs() << "Preheader:" << CurrentLoop.getLoopPreheader()->getName() << "\n";
  } dbgs() << "LoopIterCountAssumption-Info: Function = "
           << getFunctionAndBlockNames(*LoopHeader) << "\n");

  std::optional<int64_t> RawMinIterationCount = getMinTripCount(&CurrentLoop);
  if (!RawMinIterationCount) {
    LLVM_DEBUG(dbgs() << "LoopIterCountAssumptions: Loop Iteration "
                         "Count not provided for "
                      << getFunctionAndBlockNames(*LoopHeader) << "\n");
    return std::nullopt;
  }

  const int64_t MinIterCount = *RawMinIterationCount;
  if (MinIterCount <= 0) {
    LLVM_DEBUG(dbgs() << "LoopIterCountAssumptions-Warning: Loop Iteration "
                         "Count is smaller or equal to zero for "
                      << getFunctionAndBlockNames(*LoopHeader) << "\n");
    return std::nullopt;
  }

  LLVM_DEBUG(dbgs() << "Processing Loop Iteration Count Metadata: "
                    << getFunctionAndBlockNames(*LoopHeader) << " ("
                    << MinIterCount << ")\n");
  return MinIterCount;
}

void tryInsertIterationAssumptions(ICmpInst &LoopCmpInstr, Loop &CurrentLoop,
                                   int64_t IterCount,
                                   LoopStandardAnalysisResults &AR) {
  const bool ContainsEqualPredicate =
      LoopCmpInstr.getPredicate() == CmpInst::ICMP_EQ ||
      LoopCmpInstr.getPredicate() == CmpInst::ICMP_NE;

  // Guarantee that the Loop will execute at least once, to handle variable
  // StepSizes and EQ/NE predicates
  if (hasVariableStepSize(*LoopCmpInstr.getOperand(0), AR.SE) ||
      hasVariableStepSize(*LoopCmpInstr.getOperand(1), AR.SE) ||
      ContainsEqualPredicate)
    tryInsertIterationAssumption(LoopCmpInstr, CurrentLoop, 0, AR.SE, AR.AC);

  // Insert Assumption evaluated at IterCount - 1 to prohibit Loop Unrolling
  // from inserting a Loop Guard
  tryInsertIterationAssumption(LoopCmpInstr, CurrentLoop, IterCount - 1, AR.SE,
                               AR.AC);
}

} // namespace

PreservedAnalyses LoopIterCountAssumptions::run(Loop &CurrentLoop,
                                                LoopAnalysisManager &AM,
                                                LoopStandardAnalysisResults &AR,
                                                LPMUpdater &U) {
  std::optional<int64_t> MinIterCount = getValidMinIterCount(CurrentLoop);
  if (!MinIterCount)
    return PreservedAnalyses::all();

  ICmpInst *LoopCmpInstr = getLoopCmpInst(CurrentLoop);
  if (!LoopCmpInstr)
    return PreservedAnalyses::all();

  tryInsertIterationAssumptions(*LoopCmpInstr, CurrentLoop, *MinIterCount, AR);
  return PreservedAnalyses::all();
}
