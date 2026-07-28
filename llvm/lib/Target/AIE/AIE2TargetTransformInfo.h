//===---AIE2TargetTransformInfo.h - AIEngine V2 specific TTI -*- C++ //-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2025 Advanced Micro Devices, Inc. or its affiliates
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
#include "AIEBaseTargetTransformInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"

namespace llvm {

class AIE2TTICommon : public AIETTICommon {
private:
  bool isAllowedInZOL(llvm::Instruction &Instr) const override;
};

class AIE2TTIImpl : public AIEBaseTTIImpl<AIE2TTIImpl> {
  typedef AIEBaseTTIImpl<AIE2TTIImpl> BaseT;
  typedef TargetTransformInfo TTI;
  friend BaseT;
  AIE2TTICommon Common;

public:
  explicit AIE2TTIImpl(const AIE2TargetMachine *TM, const Function &F)
      : BaseT(TM, F.getParent()->getDataLayout(),
              (const AIESubtarget *)TM->getSubtargetImpl(F)) {}

  std::optional<Instruction *> instCombineIntrinsic(InstCombiner &IC,
                                                    IntrinsicInst &II) const override;

  // By returning false here, we will prevent the following type
  // of code from reaching the backend when GEPs are used as incoming values:
  // for.body:     ; preds = %for.body, %for.preheader
  //   %phi0 = phi ptr [ %in0, %for.body ], [ %out0, %for.preheader ]
  //   %phi1 = phi ptr [ %phi0, %for.body ], [ %out1, %for.preheader ]
  // This type of code can lead to additional pointer arithmetics and
  // and pointer moves (especially due to the pre-pipeliner).
  bool isProfitableFoldGEPIntoPHI() const override { return false; }
  void getUnrollingPreferences(Loop *L, ScalarEvolution &SE,
                               TTI::UnrollingPreferences &UP,
                               OptimizationRemarkEmitter *ORE) const override;
  bool isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                AssumptionCache &AC, TargetLibraryInfo *LibInfo,
                                HardwareLoopInfo &HWLoopInfo) const override;
  bool isProfitableOuterLSR(const Loop &L) const override;

  InstructionCost getMemoryOpCost(
      unsigned Opcode, Type *Src, Align Alignment, unsigned AddressSpace,
      TTI::TargetCostKind CostKind,
      TTI::OperandValueInfo OpInfo = {TTI::OK_AnyValue, TTI::OP_None},
      const Instruction *I = nullptr) const override;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIE2TARGETTRANSFORMINFO_H
