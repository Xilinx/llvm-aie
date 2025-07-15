//===---AIEBaseTargetTransformInfo.h - AIEngine generic TTI -*- C++  ----*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file act as base for AIE's specific information to provide answers to
// certain TTI queries and let child classes to provide even more precise
// answers while letting the target independent and default TTI implementations
// handle the rest.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEBASETARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_AIE_AIEBASETARGETTRANSFORMINFO_H

#include "aie1/AIE1Subtarget.h" // For AIEBaseSubTarget
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"

namespace llvm {
/// This is just a bunch of shared methods that can be easily reused.
/// It is foreseen that some of them may be overridden in derived classes used
/// by the actual TTIImpl classes
class AIETTICommon {
public:
  virtual ~AIETTICommon() = default;
  virtual bool isLoweredToCall(const Function *F);
  virtual bool isAllowedInZOL(llvm::Instruction &Instr);
  void adjustUnrollingPreferences(Loop *L, ScalarEvolution &SE,
                                  TTI::UnrollingPreferences &UP,
                                  OptimizationRemarkEmitter *ORE);
  bool isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                AssumptionCache &AC, TargetLibraryInfo *LibInfo,
                                HardwareLoopInfo &HWLoopInfo);
  bool isProfitableOuterLSR(const Loop &L) const;
};

template <typename T> class AIEBaseTTIImpl : public BasicTTIImplBase<T> {
private:
  using BaseT = BasicTTIImplBase<T>;
  using TTI = TargetTransformInfo;
  friend BaseT;
  const AIESubtarget *ST;
  const AIEBaseTargetLowering *TLI;

  const AIEBaseTargetLowering *getTLI() const { return TLI; }
  /// Helper function to access this as a T.
  T *thisT() { return static_cast<T *>(this); }

protected:
  const AIESubtarget *getST() const { return ST; }
  explicit AIEBaseTTIImpl(const TargetMachine *TM, const DataLayout &DL,
                          const AIESubtarget *Subtarget)
      : BaseT(TM, DL), ST(Subtarget), TLI(Subtarget->getTargetLowering()) {}
  virtual ~AIEBaseTTIImpl() = default;

public:
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
  void adjustUnrollingPreferences(Loop *L, ScalarEvolution &SE,
                                  TTI::UnrollingPreferences &UP,
                                  OptimizationRemarkEmitter *ORE);
  bool isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                AssumptionCache &AC, TargetLibraryInfo *LibInfo,
                                HardwareLoopInfo &HWLoopInfo);
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEBASETARGETTRANSFORMINFO_H
