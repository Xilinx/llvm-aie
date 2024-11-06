//===---AIE2TargetTransformInfo.h - AIEngine V2 specific TTI -*- C++ //-*-===//
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

#ifndef LLVM_LIB_TARGET_AIE_AIE2TARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_AIE_AIE2TARGETTRANSFORMINFO_H

#include "AIE2TargetMachine.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"

namespace llvm {
class AIE2TTIImpl : public BasicTTIImplBase<AIE2TTIImpl> {
  typedef BasicTTIImplBase<AIE2TTIImpl> BaseT;
  typedef TargetTransformInfo TTI;
  friend BaseT;

  const AIE2Subtarget *ST;
  const AIE2TargetLowering *TLI;

  const AIE2Subtarget *getST() const { return ST; }
  const AIE2TargetLowering *getTLI() const { return TLI; }

public:
  explicit AIE2TTIImpl(const AIE2TargetMachine *TM, const Function &F)
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
                               OptimizationRemarkEmitter *ORE);

  bool isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                AssumptionCache &AC, TargetLibraryInfo *LibInfo,
                                HardwareLoopInfo &HWLoopInfo);

  bool isProfitableOuterLSR(const Loop &L) const;
  std::optional<Instruction *> instCombineIntrinsic(InstCombiner &IC,
                                                    IntrinsicInst &II) const;

  // By returning false here, we will prevent the following type
  // of code from reaching the backend when GEPs are used as incoming values:
  // for.body:     ; preds = %for.body, %for.preheader
  //   %phi0 = phi ptr [ %in0, %for.body ], [ %out0, %for.preheader ]
  //   %phi1 = phi ptr [ %phi0, %for.body ], [ %out1, %for.preheader ]
  // This type of code can lead to additional pointer arithmetics and
  // and pointer moves (especially due to the pre-pipeliner).
  bool isProfitableFoldGEPIntoPHI() const { return false; }
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIE2TARGETTRANSFORMINFO_H
