//===----AIE2PSTargetTransformInfo.h - AIE2PS specific TTI --*----- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file uses the target's specific information to
// provide more precise answers to certain TTI queries, while letting the
// target independent and default TTI implementations handle the rest.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE2PS_AIE2PSTARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_AIE_AIE2PS_AIE2PSTARGETTRANSFORMINFO_H

#include "AIE2PSTargetMachine.h"
#include "AIEBaseTargetTransformInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"

namespace llvm {
class Loop;
class ScalarEvolution;
class AIE2PSTTIImpl : public AIEBaseTTIImpl<AIE2PSTTIImpl> {
  typedef AIEBaseTTIImpl<AIE2PSTTIImpl> BaseT;
  typedef TargetTransformInfo TTI;
  friend BaseT;
  AIETTICommon Common;

public:
  explicit AIE2PSTTIImpl(const AIE2PSTargetMachine *TM, const Function &F)
      : BaseT(TM, F.getParent()->getDataLayout(),
              (const AIESubtarget *)TM->getSubtargetImpl(F)) {}

  bool isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                AssumptionCache &AC, TargetLibraryInfo *LibInfo,
                                HardwareLoopInfo &HWLoopInfo);
  bool isProfitableOuterLSR(const Loop &L) const;

  InstructionCost getMemoryOpCost(
      unsigned Opcode, Type *Src, Align Alignment, unsigned AddressSpace,
      TTI::TargetCostKind CostKind,
      TTI::OperandValueInfo OpInfo = {TTI::OK_AnyValue, TTI::OP_None},
      const Instruction *I = nullptr);
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIE2PS_AIE2PSTARGETTRANSFORMINFO_H
