//===---AIEBaseTargetTransformInfo.h - AIEngine generic TTI -*- C++  ----*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
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
#include "llvm/IR/Intrinsics.h"

namespace llvm {

// Forward declarations
class Loop;
class PHINode;

// Standalone helper function for congruent IV analysis
bool shouldMergeCongruentIVsImpl(const PHINode *IV1, const PHINode *IV2);

/// This is just a bunch of shared methods that can be easily reused.
/// It is foreseen that some of them may be overridden in derived classes used
/// by the actual TTIImpl classes
class AIETTICommon {
public:
  virtual ~AIETTICommon() = default;
  virtual bool isLoweredToCall(const Function *F);
  virtual bool isAllowedInZOL(llvm::Instruction &Instr);

  virtual bool isVectorExtractIntrinsicID(Intrinsic::ID ID) const {
    return false;
  }
  virtual bool isGetSSIntrinsicID(Intrinsic::ID ID) const { return false; }

  bool isVectorExtractIntrinsicSequence(const Instruction &I) const;
  bool isGetSSExtractInsertVectorSequence(const Instruction &I) const;
  bool isScalarizedVectorOpIdiomLoop(const Loop *L) const;
  static bool isScalarLoop(const Loop *L);

  void adjustUnrollingPreferences(Loop *L, ScalarEvolution &SE,
                                  TTI::UnrollingPreferences &UP,
                                  OptimizationRemarkEmitter *ORE);
  void applyLoopIdiomUnrolling(Loop *L, TTI::UnrollingPreferences &UP) const;
  bool isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                AssumptionCache &AC, TargetLibraryInfo *LibInfo,
                                HardwareLoopInfo &HWLoopInfo);
  bool isProfitableOuterLSR(const Loop &L) const;

  InstructionCost getMemoryOpCost(unsigned Opcode, Type *Src, Align Alignment,
                                  unsigned AddressSpace,
                                  const DataLayout &DL) const;
};

template <typename T> class AIEBaseTTIImpl : public BasicTTIImplBase<T> {
private:
  using BaseT = BasicTTIImplBase<T>;
  using TTI = TargetTransformInfo;
  friend BaseT;
  const AIESubtarget *ST;
  const AIEBaseTargetLowering *TLI;

  const AIESubtarget *getST() const { return ST; }
  const AIEBaseTargetLowering *getTLI() const { return TLI; }
  /// Helper function to access this as a T.
  T *thisT() { return static_cast<T *>(this); }

protected:
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

  // We define a store vector factor of  4 for 8-bit and 2 for 16-bit. This
  // allows combining 2 16-bit stores or 4 8-bit stores into a single 32-bit
  // vector store. This is deemed beneficial because of the LMS nature of
  // part-word stores which require more cycles to complete and need to stay
  // clear of other memory instructions.
  unsigned getStoreVectorFactor(unsigned VF, unsigned StoreSizeInBits,
                                unsigned ChainSizeInBytes,
                                VectorType *VecTy) const {
    // The cases of interest are 8 and 16-bit only.
    return (StoreSizeInBits == 8) ? 4 : (StoreSizeInBits == 16) ? 2 : 1;
  }

  // This load vector factor of 1 basically prevents load vectorization which is
  // deemed too expensive. The reason being that we have to shift out each
  // element after the block load and perform a sign extension for each, whereas
  // the scalarized approach results in only 4 part-word loads with implicit
  // sign extension.
  unsigned getLoadVectorFactor(unsigned VF, unsigned LoadSizeInBits,
                               unsigned ChainSizeInBytes,
                               VectorType *VecTy) const {
    // Block load vectorization, it is costly to extract elements from vectors.
    return 1;
  }

  bool isLegalToVectorizeStoreChain(unsigned ChainSizeInBytes, Align Alignment,
                                    unsigned AddrSpace) const {
    // Start from 4 byte sequences, to reach word stores. Alignment is
    // considered by default by the pass.
    // Default return of allowsMisalignedMemoryAccesses is false.
    return ChainSizeInBytes >= 4;
  }

  /// Congruent IVs are induction variables that have the same SCEV expression,
  /// meaning they start at the same value and increment by the same amount each
  /// iteration. Normally, LLVM merges congruent IVs to reduce register pressure
  /// and eliminate redundant computations.
  /// For AIE targets, determine if congruent IVs should be merged.
  /// AIE prefers to keep separate pointer IVs when one is used for loads
  /// and the other for stores, as this enables better instruction scheduling
  /// and utilization of independent address generation units.
  bool shouldMergeCongruentIVs(const PHINode *IV1, const PHINode *IV2) const {
    return shouldMergeCongruentIVsImpl(IV1, IV2);
  }

  /// For the `TCK_SizeAndLatency` cost kind report all shufflevectors as
  /// free. This is the cost kind queried by
  /// `SimplifyCFG::computeSpeculationCost` (the function that decides
  /// whether to fold a `br + 2 BBs + phi` diamond back into a `select`)
  /// via `BasicTTIImplBase::computeSpeculationCost` →
  /// `TTI::getInstructionCost(I, ..., TCK_SizeAndLatency)`. On AIE every
  /// shufflevector that survives to ISel maps to a register-class view
  /// (`wl0`/`wh0`/etc.) or a single-cycle `vshift`/`vsel`, so reporting
  /// the default per-element scalarised cost there is wildly conservative
  /// and blows the `4 * TCC_Basic` speculation budget for
  /// `c ? wide_a : wide_b` ternaries — leaving a real branch in the
  /// final asm. The throughput / latency / recip-throughput cost kinds
  /// fall through to the base implementation so the loop vectoriser and
  /// other passes still see the realistic per-element cost.
  InstructionCost getShuffleCost(TTI::ShuffleKind Kind, VectorType *Tp,
                                 ArrayRef<int> Mask,
                                 TTI::TargetCostKind CostKind, int Index,
                                 VectorType *SubTp,
                                 ArrayRef<const Value *> Args = {},
                                 const Instruction *CxtI = nullptr) {
    if (CostKind == TTI::TCK_SizeAndLatency)
      return TTI::TCC_Free;
    return BaseT::getShuffleCost(Kind, Tp, Mask, CostKind, Index, SubTp, Args,
                                 CxtI);
  }
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEBASETARGETTRANSFORMINFO_H
