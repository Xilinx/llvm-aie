//===------ AIETargetTransformInfo.h - AIE specific TTI ---------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file uses the target's specific information to
// provide more precise answers to certain TTI queries, while letting the
// target independent and default TTI implementations handle the rest.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIETARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_AIE_AIETARGETTRANSFORMINFO_H

#include "AIETargetMachine.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"

namespace llvm {
class AIETTIImpl : public BasicTTIImplBase<AIETTIImpl> {
  typedef BasicTTIImplBase<AIETTIImpl> BaseT;
  typedef TargetTransformInfo TTI;
  friend BaseT;

  const AIESubtarget *ST;
  const AIEBaseTargetLowering *TLI;

  const AIESubtarget *getST() const { return ST; }
  const AIEBaseTargetLowering *getTLI() const { return TLI; }

public:
  explicit AIETTIImpl(const AIETargetMachine *TM, const Function &F)
      : BaseT(TM, F.getParent()->getDataLayout()), ST(TM->getSubtargetImpl(F)),
        TLI(ST->getTargetLowering()) {}

  int getIntImmCost(const APInt &Imm, Type *Ty, TTI::TargetCostKind CostKind) {
    // Constants that can be materialized with one slot.
    if (Imm.getBitWidth() <= 64 && isInt<20>(Imm.getSExtValue()))
      return TTI::TCC_Free;

    // Larger constants require an add.
    return TTI::TCC_Basic;
  }
  InstructionCost getMaskedMemoryOpCost(
      unsigned Opcode, Type *Src, Align Alignment, unsigned AddressSpace,
      TTI::TargetCostKind CostKind = TTI::TCK_RecipThroughput) const {
    // Default cost is 32.  We can do better than that, but what is the real
    // cost?
    return TTI::TCC_Basic;
  }
  void getUnrollingPreferences(Loop *L, ScalarEvolution &SE,
                               TTI::UnrollingPreferences &UP,
                               OptimizationRemarkEmitter *ORE) {
    UP.Partial = UP.Runtime = true;
    UP.AllowExpensiveTripCount = true;
    UP.Threshold = 200;
    BaseT::getUnrollingPreferences(L, SE, UP, ORE);
  }
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIETARGETTRANSFORMINFO_H
