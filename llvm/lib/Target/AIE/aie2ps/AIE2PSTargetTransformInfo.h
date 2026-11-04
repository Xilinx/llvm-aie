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
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/Instructions.h"

namespace llvm {
class Loop;
class ScalarEvolution;

class AIE2PSTTICommon : public AIETTICommon {
public:
  bool isVectorExtractIntrinsicID(Intrinsic::ID ID) const override;
  bool isGetSSIntrinsicID(Intrinsic::ID ID) const override;
};

class AIE2PSTTIImpl : public AIEBaseTTIImpl<AIE2PSTTIImpl> {
  typedef AIEBaseTTIImpl<AIE2PSTTIImpl> BaseT;
  typedef TargetTransformInfo TTI;
  friend BaseT;
  AIE2PSTTICommon Common;

public:
  explicit AIE2PSTTIImpl(const AIE2PSTargetMachine *TM, const Function &F)
      : BaseT(TM, F.getParent()->getDataLayout(),
              (const AIESubtarget *)TM->getSubtargetImpl(F)) {}

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

  std::optional<Instruction *> instCombineIntrinsic(InstCombiner &IC,
                                                    IntrinsicInst &II) const override;

  // AIE2PS has a 64-bit scalar LDA instruction (LDA_dml_lda_scalar_L)
  // that loads into eL registers. This allows merging two consecutive
  // i32 loads into a single 64-bit load. The base class only allows
  // pairs for 8-bit loads; here we extend that to 32-bit loads as well.
  unsigned getLoadVectorFactor(unsigned VF, unsigned LoadSizeInBits,
                               unsigned ChainSizeInBytes,
                               VectorType *VecTy) const override {
    if (LoadSizeInBits == 8)
      return 2;
    if (LoadSizeInBits == 32)
      return 2; // 2 x i32 -> i64 via LDA_dml_lda_scalar_L
    return 1;
  }

  // Do not vectorize i32 loads where all users ultimately truncate to
  // i20, even through PHI chains. The prelegalizer combiner
  // (matchNarrowTruncLoad) can narrow these to s20 loads that go
  // directly to pointer/modifier registers, which is more profitable
  // than a 64-bit vector load followed by extraction.
  //
  // We walk through PHI nodes with a worklist to handle cases like:
  //   %val = load i32, ptr %p
  //   %phi = phi i32 [ %val, %bb1 ], [ %other, %bb2 ]
  //   %ptr = trunc i32 %phi to i20
  bool isLegalToVectorizeLoad(LoadInst *LI) const override {
    const Type *Ty = LI->getType();
    if (!Ty->isIntegerTy(32) || LI->use_empty())
      return true;

    // Walk users through PHI chains. If every leaf (non-PHI) user is
    // a trunc to i20, block vectorization.
    SmallVector<const User *, 8> Worklist(LI->users());
    SmallPtrSet<const User *, 8> Visited;
    while (!Worklist.empty()) {
      const User *U = Worklist.pop_back_val();
      if (!Visited.insert(U).second)
        continue;
      if (auto *TI = dyn_cast<TruncInst>(U)) {
        if (TI->getType()->isIntegerTy(20))
          continue;  // This leaf is a trunc to i20 -- keep checking.
        return true; // Trunc to something other than i20 -- allow.
      }
      if (auto *PN = dyn_cast<PHINode>(U)) {
        // Traverse through the PHI to its users.
        for (const User *PU : PN->users())
          Worklist.push_back(PU);
        continue;
      }
      // Any other user (store, add, etc.) -- allow vectorization.
      return true;
    }
    // All leaf users are trunc to i20; block vectorization so the
    // prelegalizer can narrow to s20 loads.
    return false;
  }
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIE2PS_AIE2PSTARGETTRANSFORMINFO_H
