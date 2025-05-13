//===-- OutlineGEP.cpp - Outline GEP ----------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass creates virtual registers for GEP that were previously inlined
// to memory instructions and call instructions.
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Scalar/OutlineGEP.h"

using namespace llvm;

#define DEBUG_TYPE "outline-gep"

namespace {

/// \return Operand Index of the Pointer in \p MemInstr , given that it is a
/// load or store instruction
std::optional<unsigned> getMemoryPointerOperandIdx(const Instruction &MemInstr);

/// \return GEP of \p Operand if inlined
GEPOperator *getInlinedGEPOperator(Value *Operand);

/// Reverse the inlined GEP inline in \p CallInstr
void revertCallInlinedGEPs(CallInst &CallInstr);

/// Reverse the inlined GEP in \p I , if I is a Memory Instruction
void revertMemoryInstrInlinedGEPs(Instruction &I);

/// Insert a new GEP Instruction based on \p GEP at Operand Index \p OpIdx
/// before the Position \p InsertionPoint
void insertNewGEPInst(const unsigned OpIdx, Instruction &InsertionPoint,
                      GEPOperator &GEP);

void reverseGEPInline(BasicBlock &BB) {
  for (auto &I : BB) {
    if (auto *CallInstr = dyn_cast<CallInst>(&I)) {
      revertCallInlinedGEPs(*CallInstr);
    } else {
      revertMemoryInstrInlinedGEPs(I);
    }
  }
}

void revertCallInlinedGEPs(CallInst &CallInstr) {
  for (unsigned Idx = 0; Idx < CallInstr.getNumOperands(); Idx++) {
    auto *OP = CallInstr.getOperand(Idx);
    GEPOperator *GEP = getInlinedGEPOperator(OP);
    if (!GEP)
      continue;
    LLVM_DEBUG(dbgs() << "Found Call Instr " << CallInstr);

    insertNewGEPInst(Idx, CallInstr, *GEP);
  }
}

void revertMemoryInstrInlinedGEPs(Instruction &I) {
  const auto PointerOperandIdx = getMemoryPointerOperandIdx(I);
  if (!PointerOperandIdx)
    return;

  GEPOperator *GEP = getInlinedGEPOperator(I.getOperand(*PointerOperandIdx));
  if (!GEP)
    return;
  LLVM_DEBUG(dbgs() << "Found " << I);

  insertNewGEPInst(*PointerOperandIdx, I, *GEP);
}

void insertNewGEPInst(const unsigned OpIdx, Instruction &InsertionPoint,
                      GEPOperator &GEP) {
  const std::vector<Value *> Indices(GEP.idx_begin(), GEP.idx_end());

  GetElementPtrInst *NewGEPInstr = GetElementPtrInst::Create(
      GEP.getSourceElementType(), GEP.getPointerOperand(), Indices,
      "reverse.arrayidx", &InsertionPoint);
  NewGEPInstr->setIsInBounds(GEP.isInBounds());

  LLVM_DEBUG(dbgs() << "Created New " << *NewGEPInstr);

  InsertionPoint.setOperand(OpIdx, NewGEPInstr);
}

GEPOperator *getInlinedGEPOperator(Value *Operand) {
  assert(Operand);
  if (isa<GetElementPtrInst>(Operand))
    // not inlined
    return nullptr;

  return dyn_cast<GEPOperator>(Operand);
}

std::optional<unsigned>
getMemoryPointerOperandIdx(const Instruction &MemInstr) {
  if (const auto *Load = dyn_cast<LoadInst>(&MemInstr))
    return Load->getPointerOperandIndex();

  if (const auto *Store = dyn_cast<StoreInst>(&MemInstr))
    return Store->getPointerOperandIndex();

  return {};
}

}; // namespace

PreservedAnalyses OutlineGEPPass::run(Function &F,
                                      FunctionAnalysisManager &AM) {
  for (auto &BB : F) {
    reverseGEPInline(BB);
  }
  return PreservedAnalyses::all();
}
