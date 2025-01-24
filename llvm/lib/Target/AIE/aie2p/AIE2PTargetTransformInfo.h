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
#include "AIEBaseTargetTransformInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"

namespace llvm {
class AIE2PTTIImpl : public AIEBaseTTIImpl<AIE2PTTIImpl> {
  typedef AIEBaseTTIImpl<AIE2PTTIImpl> BaseT;
  typedef TargetTransformInfo TTI;
  friend BaseT;

public:
  explicit AIE2PTTIImpl(const AIE2PTargetMachine *TM, const Function &F)
      : BaseT(TM, F.getParent()->getDataLayout(),
              (const AIESubtarget *)TM->getSubtargetImpl(F)) {}

  void getUnrollingPreferences(Loop *L, ScalarEvolution &SE,
                               TTI::UnrollingPreferences &UP,
                               OptimizationRemarkEmitter *ORE);
  bool isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                AssumptionCache &AC, TargetLibraryInfo *LibInfo,
                                HardwareLoopInfo &HWLoopInfo);
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIE2PTARGETTRANSFORMINFO_H
