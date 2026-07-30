//===-- AIECombinerBase.h - Shared base for AIE combiner impls --*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIECOMBINERBASE_H
#define LLVM_LIB_TARGET_AIE_AIECOMBINERBASE_H

#include "AIEBaseSubtarget.h"
#include "llvm/CodeGen/GlobalISel/Combiner.h"
#include "llvm/CodeGen/GlobalISel/CombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/CombinerInfo.h"

namespace llvm {

template <typename RuleConfigT> class AIECombinerBase : public Combiner {
protected:
  mutable CombinerHelper Helper;
  const RuleConfigT &RuleConfig;
  const AIEBaseSubtarget &STI;

  AIECombinerBase(MachineFunction &MF, CombinerInfo &CInfo,
                  const TargetPassConfig *TPC, GISelValueTracking &VT,
                  GISelCSEInfo *CSEInfo, const RuleConfigT &RC,
                  const AIEBaseSubtarget &STI, MachineDominatorTree *MDT,
                  const LegalizerInfo *LI, bool IsPreLegalize)
      : Combiner(MF, CInfo, TPC, &VT, CSEInfo),
        Helper(Observer, B, IsPreLegalize, &VT, MDT, LI), RuleConfig(RC),
        STI(STI) {}
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIECOMBINERBASE_H
