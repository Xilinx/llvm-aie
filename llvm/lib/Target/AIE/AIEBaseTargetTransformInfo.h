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
  virtual bool isLoweredToCall(const Function *F) const;
  virtual bool isAllowedInZOL(llvm::Instruction &Instr) const;

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
                                  OptimizationRemarkEmitter *ORE) const;
  void applyLoopIdiomUnrolling(Loop *L, TTI::UnrollingPreferences &UP) const;
  bool isHardwareLoopProfitable(Loop *L, ScalarEvolution &SE,
                                AssumptionCache &AC, TargetLibraryInfo *LibInfo,
                                HardwareLoopInfo &HWLoopInfo) const;
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

  static bool isIndexSizedInteger(Type *Ty, const DataLayout &DL) {
    return Ty->isIntegerTy() &&
           Ty->getIntegerBitWidth() == DL.getIndexSizeInBits(0);
  }

protected:
  explicit AIEBaseTTIImpl(const TargetMachine *TM, const DataLayout &DL,
                          const AIESubtarget *Subtarget)
      : BaseT(TM, DL), ST(Subtarget), TLI(Subtarget->getTargetLowering()) {}
  virtual ~AIEBaseTTIImpl() = default;

public:
  InstructionCost getIntImmCost(const APInt &Imm, Type *Ty,
                    TTI::TargetCostKind CostKind) const override {
    // TODO Handle Target Specific constant cost
    //  Larger constants require an add.
    return TTI::TCC_Basic;
  }
  InstructionCost getMaskedMemoryOpCost(
      unsigned Opcode, Type *Src, Align Alignment, unsigned AddressSpace,
      TTI::TargetCostKind CostKind = TTI::TCK_RecipThroughput) const override {
    // Default cost is 32.  We can do better than that, but what is the real
    // cost?
    return TTI::TCC_Basic;
  }

  // AIE vector arithmetic operates on 512-bit registers. The default
  // getRegisterBitWidth returns 32 (scalar width), which causes the loop
  // vectorizer to choose VF=4 for i8, creating <4 x i8> vectors. The
  // GlobalISel Legalizer only has rules for 512-bit vector arithmetic
  // (V64S8, V32S16, V16S32), so sub-512-bit vector ops are illegal.
  // Return 512 for vector register kinds to match the legal types.
  //
  // See {AIE1,AIE2,AIE2P,AIE2PS}LegalizerInfo.cpp for the authoritative
  // legal-types list (search for `legalFor({V16S32, V32S16, V64S8})` on the
  // G_ADD/G_SUB/G_XOR rules; AIE1 has no legal vector arithmetic). If a
  // future target legalizes additional widths, the three TTI overrides
  // below must be revisited.
  TypeSize getRegisterBitWidth(TargetTransformInfo::RegisterKind K) const override {
    switch (K) {
    case TargetTransformInfo::RGK_Scalar:
      return TypeSize::getFixed(32);
    case TargetTransformInfo::RGK_FixedWidthVector:
      return TypeSize::getFixed(512);
    case TargetTransformInfo::RGK_ScalableVector:
      // Use a scalable TypeSize to represent "no scalable vector registers".
      return TypeSize::getScalable(0);
    }
    llvm_unreachable("Unsupported register kind");
  }

  // Minimum vector width for the SLP vectorizer and VectorCombine.
  // Must match the Legalizer's supported vector arithmetic widths (512-bit).
  unsigned getMinVectorRegisterBitWidth() const override { return 512; }

  // AIE vector arithmetic only supports 512-bit operations (V64S8, V32S16,
  // V16S32). Sub-512-bit vector ops like <8 x i8> are illegal in the
  // GlobalISel Legalizer and would crash during code generation. Return
  // Invalid cost to prevent vectorizers (especially SLP) from creating them.
  //
  // The < 512 width filter is conservative on the upper end: a 512-bit
  // vector that is not in the legal-arithmetic list (e.g. V8S64) would also
  // be illegal, but no current vectorizer pass produces such types from AIE
  // workloads in practice. Querying LegalizerInfo directly from TTI would be
  // the precise solution but is a cross-layer violation -- TTI operates on
  // LLVM IR types during mid-level optimization, while LegalizerInfo uses
  // LLT machine types and is only initialized when GlobalISel is active.
  // No upstream LLVM target queries LegalizerInfo from TTI.
  InstructionCost getArithmeticInstrCost(
      unsigned Opcode, Type *Ty, TTI::TargetCostKind CostKind,
      TTI::OperandValueInfo Opd1Info = {TTI::OK_AnyValue, TTI::OP_None},
      TTI::OperandValueInfo Opd2Info = {TTI::OK_AnyValue, TTI::OP_None},
      ArrayRef<const Value *> Args = {},
      const Instruction *CxtI = nullptr) const override {
    if (auto *VTy = dyn_cast<FixedVectorType>(Ty)) {
      if (VTy->getPrimitiveSizeInBits() < 512)
        return InstructionCost::getInvalid();
    }
    return BaseT::getArithmeticInstrCost(Opcode, Ty, CostKind, Opd1Info,
                                         Opd2Info, Args, CxtI);
  }

  // We define a store vector factor of  4 for 8-bit and 2 for 16-bit. This
  // allows combining 2 16-bit stores or 4 8-bit stores into a single 32-bit
  // vector store. This is deemed beneficial because of the LMS nature of
  // part-word stores which require more cycles to complete and need to stay
  // clear of other memory instructions.
  unsigned getStoreVectorFactor(unsigned VF, unsigned StoreSizeInBits,
                                unsigned ChainSizeInBytes,
                                VectorType *VecTy) const override {
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
                               VectorType *VecTy) const override {
    // Block load vectorization, it is costly to extract elements from vectors.
    return 1;
  }

  bool isLegalToVectorizeStoreChain(unsigned ChainSizeInBytes, Align Alignment,
                                    unsigned AddrSpace) const override {
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
  bool shouldMergeCongruentIVs(const PHINode *IV1,
                               const PHINode *IV2) const override {
    return shouldMergeCongruentIVsImpl(IV1, IV2);
  }

  /// Any index-sized recurrence is valid, not just GEP indices: that width is
  /// the AIE address register width, for which getIndexSizeInBits() is only a
  /// proxy. Narrowing this to pointer-arithmetic users would reject the
  /// non-GEP i20 IV in llvm/test/CodeGen/AIE/opt/lsr-i20-mixed-use.ll.
  bool isValidIVUserType(Type *Ty) const override {
    return BaseT::isValidIVUserType(Ty) ||
           isIndexSizedInteger(Ty, BaseT::getDataLayout());
  }
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEBASETARGETTRANSFORMINFO_H
