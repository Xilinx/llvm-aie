//===----AIE2PTargetTransformInfo.h - AIE2p specific TTI -*----- C++ //-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file uses the target's specific information to
// provide more precise answers to certain TTI queries, while letting the
// target independent and default TTI implementations handle the rest.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE2PTARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_AIE_AIE2PTARGETTRANSFORMINFO_H

#include "AIE2PTargetMachine.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"

namespace llvm {
class AIE2PTTIImpl : public BasicTTIImplBase<AIE2PTTIImpl> {
  typedef BasicTTIImplBase<AIE2PTTIImpl> BaseT;
  typedef TargetTransformInfo TTI;
  friend BaseT;

  const AIE2PSubtarget *ST;
  const AIE2PTargetLowering *TLI;

  const AIE2PSubtarget *getST() const { return ST; }
  const AIE2PTargetLowering *getTLI() const { return TLI; }

public:
  explicit AIE2PTTIImpl(const AIE2PTargetMachine *TM, const Function &F)
      : BaseT(TM, F.getParent()->getDataLayout()), ST(TM->getSubtargetImpl(F)),
        TLI(ST->getTargetLowering()) {}

  int getIntImmCost(const APInt &Imm, Type *Ty, TTI::TargetCostKind CostKind) {
    // TODO Handle Target Specific constant cost
    //  Larger constants require an add.
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

#endif // LLVM_LIB_TARGET_AIE_AIE2PTARGETTRANSFORMINFO_H
