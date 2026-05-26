//=== lib/CodeGen/GlobalISel/AIECombinerHelper.h --------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIECOMBINERHELPER_H
#define LLVM_LIB_TARGET_AIE_AIECOMBINERHELPER_H

#include "AIEPtrModOptimizer.h"
#include "llvm/CodeGen/GlobalISel/CombinerHelper.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/MachineInstr.h"
namespace llvm {

struct AIEBaseInstrInfo;

struct ShuffleMaskValidity {
  bool IsValid;
  // Holds mask indices that don't satisfy the mask constraints
  SmallVector<unsigned, 4> MaskExceptions;
};

struct FrequentIndexResult {
  unsigned FrequentIdx;
  unsigned NonMatchingCount;
};

struct AIEConcatUnmergeCombineMatchData {
  // Concat Instruction's MBB where the G_CONCAT of ConcatSub[0] through
  // ConcatSub[n-1] should be created.
  MachineBasicBlock *NewConcatMBB = nullptr;
  // Concat Source Registers.
  SmallVector<Register, 4> ConcatSubVecs;

  // PHI component of the backedge
  // Register for the PHI Source operand 2 sitting in the backedge path.
  std::optional<Register> UnmergeSourceReg;
};

/// The mask is represented by a sawtooth function F with Period, Height and
/// Amplitude, i.e., F(idx + Period) = F(idx) = Height + idx * Amplitude, where
/// idx >= 0.
/// Example: mask = (4, 6, 8, 4, 6, 8) <=> Height=4, Amplitude=2, Period=3
class MaskMatch {
public:
  MaskMatch(unsigned MaskHeight, unsigned MaskPeriod = 0, int MaskAmplitude = 1)
      : Period{MaskPeriod}, Height{MaskHeight}, Amplitude{MaskAmplitude} {}

  bool isValidMask(ArrayRef<int> Mask) const;
  ShuffleMaskValidity getShuffleMaskValidity(ArrayRef<int> Mask) const;

  unsigned getHeight() const { return Height; }

  static bool isMaskWithAllUndefs(ArrayRef<int> Mask);
  static std::optional<unsigned> getHeight(ArrayRef<int> Mask, unsigned Period);
  static std::optional<int> getUniqueIndex(ArrayRef<int> Mask);
  static bool isMaskWithinRangeOrUndef(ArrayRef<int> Mask, int MinValue,
                                       int MaxValue);
  static std::optional<FrequentIndexResult>
  getFrequentIndexResult(ArrayRef<int> Mask, unsigned MinFrequency);

  unsigned getMaskValue(unsigned Idx) const {
    unsigned BaseIdx = Period == 0 ? Idx : Idx % Period;
    return Height + BaseIdx * Amplitude;
  }

protected:
  unsigned Period = 0;
  unsigned Height = 0;
  /// Negative amplitude can be used for reverse mask patterns.
  int Amplitude = 1;
};

struct AIESingleDiffLaneBuildVectorMatchData {
  /// Destination register of G_BUILD_VECTOR
  Register DstVecReg;
  /// The repeated register
  Register SplatReg;
  /// Register for the differing element
  Register DifferingReg;
  /// Lane index of the single differing element
  unsigned DifferingIndex;
};

/// Match data for combine_alternating_build_vector.
/// Describes a G_BUILD_VECTOR whose operands form a repeating sub-pattern of
/// length Period. The Period elements are packed into a single scalar of
/// MergedBits width and then broadcast into the full vector.
struct AIEAlternatingBuildVectorMatchData {
  /// Destination register of the G_BUILD_VECTOR.
  Register DstVecReg;
  /// The repeating period element registers (length = Period).
  SmallVector<Register, 4> PeriodElts;
  /// Width in bits of the merged scalar (= ElemBits * Period).
  unsigned MergedBits;
};

/// \return whether Concat-Unmerge-PHI combine pattern based on  \p ConcatI is
/// found.
bool matchConcatUnmergePhis(MachineInstr &ConcatI, MachineRegisterInfo &MRI,
                            CombinerHelper &Helper,
                            AIEConcatUnmergeCombineMatchData &MatchInfo);
/// apply Concat-Unmerge-PHI Combiner
void applyConcatUnmergePhis(MachineInstr &ConcatI, MachineRegisterInfo &MRI,
                            MachineIRBuilder &B,
                            AIEConcatUnmergeCombineMatchData &MatchInfo,
                            GISelChangeObserver &Observer);

bool matchGlobalPtrModOptimizer(MachineInstr &MemI, MachineRegisterInfo &MRI,
                                CombinerHelper &Helper,
                                const TargetInstrInfo &TII,
                                AIE::FoundCombiners *GlobalCombinerPtr);

/// Look for any PtrAdd instruction that use the same base as \a MI that can be
/// combined with it and stores it in \a GlobalCombinerPtr
/// \return true if an instruction is found
bool matchLdStInc(MachineInstr &MI, MachineRegisterInfo &MRI,
                  CombinerHelper &Helper, const TargetInstrInfo &TII,
                  AIE::FoundCombiners *GlobalCombinerPtr);
/// Combines \a MI and the instruction stored in \a GlobalCombinerPtr
void applyLdStInc(MachineInstr &MI, MachineRegisterInfo &MRI,
                  CombinerHelper &Helper, MachineIRBuilder &B,
                  GISelChangeObserver &Observer,
                  AIE::FoundCombiners *GlobalCombinerPtr);
/// Look for  with G_IMPLICIT_DEF source operands
/// \return true if such an instruction is found
bool matchAddVecEltUndef(MachineInstr &MI, MachineRegisterInfo &MRI,
                         const TargetInstrInfo &TII);
/// Combine G_AIE_ADD_VECTOR_ELT_HI with COPY
void applyAddVecEltUndef(MachineInstr &MI, MachineRegisterInfo &MRI,
                         MachineIRBuilder &B, GISelChangeObserver &Observer);
/// combine G_GLOBAL_VALUE with G_CONSTANT and store in \a MatchData
/// \return true if it is possible to combine
void applyGlobalValOffset(MachineInstr &MI, MachineRegisterInfo &MRI,
                          MachineIRBuilder &B, GISelChangeObserver &Observer,
                          uint64_t &MatchInfo);
bool matchGlobalValOffset(MachineInstr &MI, MachineRegisterInfo &MRI,
                          uint64_t &MatchInfo);
/// Combine G_SHUFFLE_VECTOR(G_BUILD_VECTOR (VAL, UNDEF, ...), mask<0,0,...>)
/// idiom into G_AIE_BROADCAST
bool matchBroadcastElement(MachineInstr &MI, MachineRegisterInfo &MRI,
                           std::pair<Register, Register> &MatchInfo);
bool matchShuffleToBroadcast(MachineInstr &MI, MachineRegisterInfo &MRI,
                             const AIEBaseInstrInfo &TII, BuildFnTy &MatchInfo);
/// Combine G_SHUFFLE_VECTOR(G_BUILD_VECTOR (VAL, UNDEF, ...), mask<0,0,...>)
/// idiom into G_AIE_VSEL
bool matchShuffleToVSel(MachineInstr &MI, MachineRegisterInfo &MRI,
                        const AIEBaseInstrInfo &TII, BuildFnTy &MatchInfo);
/// Combine a shuffle vector with a mask that extracts the only element from
/// the first source vector and broadcasts it.
bool matchShuffleToExtractBroadcast(MachineInstr &MI, MachineRegisterInfo &MRI,
                                    const AIEBaseInstrInfo &TII,
                                    BuildFnTy &MatchInfo);
/// \return true if \a MemI can be moved just before \a Dest in order to allow
/// post-increment combining
bool canDelayMemOp(MachineInstr &MemI, MachineInstr &Dest,
                   const MachineRegisterInfo &MRI);
/// \return true if \a Dest can be moved just after \a MemI in order to allow
/// combining.
///
/// \param SideEffectsAreChecked When false (the default), asserts that \a Dest
/// is not a G_INTRINSIC_W_SIDE_EFFECTS and does not load/store. This is the
/// safe default for most combining scenarios.
///
/// WARNING: Setting \p SideEffectsAreChecked to true bypasses critical safety
/// checks. Only use this in specialized combining flows (e.g., VLDA_UPS
/// combining in instruction selection) where you have verified that moving
/// \a Dest is safe despite its side effects. In almost all cases, this
/// parameter should remain false.
bool canAdvanceOp(MachineInstr &MemI, MachineInstr &Dest,
                  const MachineRegisterInfo &MRI,
                  bool SideEffectsAreChecked = false);
/// Find the def instruction for \p Reg, folding away any trivial copies and
/// bitcasts. May return nullptr if \p Reg is not a generic virtual register.
MachineInstr *getDefIgnoringCopiesAndBitcasts(Register Reg,
                                              const MachineRegisterInfo &MRI);

bool matchExtractVecEltAndExt(MachineInstr &MI, MachineRegisterInfo &MRI,
                              std::pair<MachineInstr *, bool> &MatchInfo);
void applyExtractVecEltAndExt(MachineInstr &MI, MachineRegisterInfo &MRI,
                              MachineIRBuilder &B,
                              std::pair<MachineInstr *, bool> &MatchInfo,
                              GISelChangeObserver &Observer);

bool matchSplatVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                      std::pair<Register, Register> &MatchInfo);
bool applySplatVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                      MachineIRBuilder &B,
                      std::pair<Register, Register> &MatchInfo,
                      GISelChangeObserver &Observer);

bool matchSingleDiffLaneBuildVector(
    MachineInstr &MI, MachineRegisterInfo &MRI,
    AIESingleDiffLaneBuildVectorMatchData &MatchInfo);
bool applySingleDiffLaneBuildVector(
    MachineInstr &MI, MachineRegisterInfo &MRI, MachineIRBuilder &B,
    AIESingleDiffLaneBuildVectorMatchData &MatchInfo,
    GISelChangeObserver &Observer);

bool matchSymmetricBuildVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                               GISelChangeObserver &Observer,
                               BuildFnTy &MatchInfo);

bool matchUnpadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                      const AIEBaseInstrInfo &TII);
void applyUnpadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                      MachineIRBuilder &B, GISelChangeObserver &Observer);

bool matchPadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                    const AIEBaseInstrInfo &TII, Register &MatchedInputVector);
bool matchConcatPadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                          const AIEBaseInstrInfo &TII,
                          Register &MatchedInputVector);
void applyPadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                    MachineIRBuilder &B, Register MatchedInputVector,
                    GISelChangeObserver &Observer);
bool tryToCombineVectorShiftsByZero(MachineInstr &MI, MachineRegisterInfo &MRI,
                                    GISelChangeObserver &Observer);

bool matchExtractConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                        const AIEBaseInstrInfo &TII, Register &MatchInfo);
void applyExtractConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                        MachineIRBuilder &B, Register &MatchInfo,
                        GISelChangeObserver &Observer);

bool matchUnmergeConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                        const AIEBaseInstrInfo &TII,
                        std::pair<MachineInstr *, unsigned> &MatchInfo);
void applyUnmergeConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                        MachineIRBuilder &B,
                        std::pair<MachineInstr *, unsigned> &MatchInfo,
                        GISelChangeObserver &Observer);

bool matchCSEVectorOp(MachineInstr &MI, MachineRegisterInfo &MRI,
                      CombinerHelper &Helper, Register &MatchInfo);

bool matchUpdToConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                      const AIEBaseInstrInfo &TII,
                      std::map<unsigned, Register> &IndexRegMap);
void applyUpdToConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                      MachineIRBuilder &B,
                      std::map<unsigned, Register> &IndexRegMap,
                      GISelChangeObserver &Observer);

bool matchLoadStoreSplit(GLoadStore &MI, MachineRegisterInfo &MRI,
                         const AIEBaseInstrInfo &TII, unsigned &MaxMemSize);
void applyLoadStoreSplit(GLoadStore &MI, MachineRegisterInfo &MRI,
                         MachineIRBuilder &B, const unsigned MaxMemSize,
                         GISelChangeObserver &Observer);

bool matchOffsetLoadStorePtrAdd(MachineInstr &MI, MachineRegisterInfo &MRI,
                                const AIEBaseInstrInfo &TII,
                                std::pair<Register, int64_t> &RegOffset);

void applyOffsetLoadStorePtrAdd(MachineInstr &MI, MachineRegisterInfo &MRI,
                                MachineIRBuilder &B,
                                const std::pair<Register, int64_t> &RegOffset,
                                GISelChangeObserver &Observer);

bool matchOffsetLoadStoreSharePtrAdd(MachineInstr &MI, MachineRegisterInfo &MRI,
                                     CombinerHelper &Helper,
                                     const AIEBaseInstrInfo &TII,
                                     Register &PtrAddReg);

void applyOffsetLoadStoreSharePtrAdd(MachineInstr &MI, MachineRegisterInfo &MRI,
                                     MachineIRBuilder &B, Register &PtrAddReg,
                                     GISelChangeObserver &Observer);

bool matchShuffleToExtractSubvec(MachineInstr &MI, MachineRegisterInfo &MRI,
                                 const AIEBaseInstrInfo &TII,
                                 BuildFnTy &MatchInfo);

bool matchShuffleToConcatExtractedSubvectors(MachineInstr &MI,
                                             MachineRegisterInfo &MRI,
                                             const AIEBaseInstrInfo &TII,
                                             BuildFnTy &MatchInfo);

bool matchShuffleToCopy(MachineInstr &MI, MachineRegisterInfo &MRI,
                        BuildFnTy &MatchInfo);
bool matchShuffleBcstToCopy(MachineInstr &MI, MachineRegisterInfo &MRI,
                            const TargetInstrInfo &TII, BuildFnTy &MatchInfo);

bool matchMostlySequentialShuffleWithInsertions(MachineInstr &MI,
                                                MachineRegisterInfo &MRI,
                                                BuildFnTy &MatchInfo);

bool matchPairedExtracts(MachineInstr &MI, MachineRegisterInfo &MRI,
                         CombinerHelper &Helper, const TargetInstrInfo &TII,
                         GISelChangeObserver &Observer, BuildFnTy &MatchInfo);

bool matchShuffleToExtractInsertEltToBroadcast(MachineInstr &MI,
                                               MachineRegisterInfo &MRI,
                                               BuildFnTy &MatchInfo);

bool matchBroadcastToShl(MachineInstr &MI, MachineRegisterInfo &MRI,
                         const AIEBaseInstrInfo &TII, BuildFnTy &MatchInfo);

bool matchNarrowPhi(MachineInstr &Phi, MachineRegisterInfo &MRI,
                    CombinerHelper &Helper, GISelChangeObserver &Observer,
                    BuildFnTy &MatchInfo);

bool matchNarrowTruncConstant(MachineInstr &MI, MachineRegisterInfo &MRI,
                              GISelChangeObserver &Observer,
                              BuildFnTy &MatchInfo);

bool matchNarrowZext(MachineInstr &MI, MachineRegisterInfo &MRI,
                     GISelChangeObserver &Observer, BuildFnTy &MatchInfo);

bool matchWidenFMul(MachineInstr &MI, MachineRegisterInfo &MRI,
                    GISelChangeObserver &Observer, BuildFnTy &MatchInfo);

bool matchCombineExtAndTrunc(MachineInstr &MI, MachineRegisterInfo &MRI,
                             BuildFnTy &MatchInfo);

bool matchConstLoad(MachineInstr &MI, MachineRegisterInfo &MRI,
                    GISelChangeObserver &Observer, BuildFnTy &MatchInfo);

bool matchBitcastUnmerge(MachineInstr &Phi, MachineRegisterInfo &MRI,
                         const AIEBaseInstrInfo &TII,
                         GISelChangeObserver &Observer, BuildFnTy &MatchInfo);

bool matchPhiBitcast(MachineInstr &Phi, MachineRegisterInfo &MRI,
                     CombinerHelper &Helper, const AIEBaseInstrInfo &TII,
                     GISelChangeObserver &Observer, BuildFnTy &MatchInfo);

bool matchPhiOfUndef(MachineInstr &MI, MachineRegisterInfo &MRI,
                     GISelChangeObserver &Observer, BuildFnTy &MatchInfo);

bool matchAlignMemset(MachineInstr &MI, MachineRegisterInfo &MRI,
                      const AIEBaseInstrInfo &TII,
                      GISelChangeObserver &Observer, BuildFnTy &MatchInfo);

bool matchPeelMemset(MachineInstr &MI, MachineRegisterInfo &MRI,
                     const AIEBaseInstrInfo &TII, GISelChangeObserver &Observer,
                     BuildFnTy &MatchInfo);

bool matchSequentialStores(GStore &MI, MachineRegisterInfo &MRI,
                           GISelChangeObserver &Observer, BuildFnTy &MatchInfo);

bool matchSplitConcatStore(MachineInstr &MI, MachineRegisterInfo &MRI,
                           const AIEBaseInstrInfo &TII, BuildFnTy &MatchInfo);

bool matchNarrowTruncLoad(MachineInstr &Phi, MachineRegisterInfo &MRI,
                          CombinerHelper &Helper, GISelChangeObserver &Observer,
                          BuildFnTy &MatchInfo);

bool matchLoadInttoptrFold(MachineInstr &MI, MachineRegisterInfo &MRI,
                           CombinerHelper &Helper,
                           GISelChangeObserver &Observer, BuildFnTy &MatchInfo);

bool matchExtractVecEltAssertBcst(MachineInstr &MI, MachineRegisterInfo &MRI,
                                  const AIEBaseInstrInfo &TII,
                                  GISelChangeObserver &Observer,
                                  BuildFnTy &MatchInfo);

bool matchExtractBroadcastToScalar(MachineInstr &MI, MachineRegisterInfo &MRI,
                                   const AIEBaseInstrInfo &TII,
                                   BuildFnTy &MatchInfo);

/// Check if a scalar register contains an MSB-only constant
/// (0x80 for s8, 0x8000 for s16, 0x80000000 for s32)
/// The element type is determined from the broadcast vector destination
/// register
bool matchMsbScalar(Register ScalarReg, Register BroadcastReg,
                    MachineRegisterInfo &MRI);

bool matchInsertExtractVectorEltToCopy(MachineInstr &MI,
                                       MachineRegisterInfo &MRI,
                                       const AIEBaseInstrInfo &TII,
                                       BuildFnTy &MatchInfo);

bool matchBroadcastExtractToCopy(MachineInstr &MI, MachineRegisterInfo &MRI,
                                 const AIEBaseInstrInfo &TII,
                                 BuildFnTy &MatchInfo);

bool matchVSelToUnmergeConcatOrCopy(MachineInstr &MI, MachineRegisterInfo &MRI,
                                    const AIEBaseInstrInfo &TII,
                                    BuildFnTy &MatchInfo);

bool matchCopyOfImplicitDef(MachineInstr &MI, MachineRegisterInfo &MRI);
void applyCopyOfImplicitDef(MachineInstr &MI, MachineRegisterInfo &MRI,
                            MachineIRBuilder &B, GISelChangeObserver &Observer);

/// Match a G_BUILD_VECTOR whose operands form a repeating sub-pattern of
/// length Period (e.g., [a, b, a, b, ...] for Period=2). The Period elements
/// are packed into a merged scalar and then broadcast via
/// G_AIE_BROADCAST_VECTOR to avoid full scalarization.
/// Only matches vectors >= 128 bits. Skips pure splats (Period=1).
bool matchAlternatingBuildVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                                 const AIEBaseInstrInfo &TII,
                                 AIEAlternatingBuildVectorMatchData &MatchInfo);
void applyAlternatingBuildVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                                 MachineIRBuilder &B,
                                 AIEAlternatingBuildVectorMatchData &MatchInfo,
                                 GISelChangeObserver &Observer);

/// Match and split a 512-bit SRS intrinsic that feeds stores through BITCAST
/// and UNMERGE. This enables later SRS+STORE fusion in instruction selection.
/// Pattern matched:
///   %srs:_(<64 x s8>) = G_INTRINSIC_W_SIDE_EFFECTS
///       intrinsic(@llvm.aie2ps.I512.v64.acc32.srs), %acc(<64 x s32>), %shift,
///       %sign
///   %bitcast:_(<16 x s32>) = G_BITCAST %srs
///   %lo:_(<8 x s32>), %hi:_(<8 x s32>) = G_UNMERGE_VALUES %bitcast
///   G_STORE %lo, %ptr1
///   G_STORE %hi, %ptr2
/// Transforms to:
///   %acc_lo:_(<32 x s32>), %acc_hi:_(<32 x s32>) = G_UNMERGE_VALUES %acc
///   %srs_lo:_(<32 x s8>) = G_INTRINSIC_W_SIDE_EFFECTS
///       intrinsic(@llvm.aie2ps.I256.v32.acc32.srs), %acc_lo, %shift, %sign
///   %srs_hi:_(<32 x s8>) = G_INTRINSIC_W_SIDE_EFFECTS
///       intrinsic(@llvm.aie2ps.I256.v32.acc32.srs), %acc_hi, %shift, %sign
///   %lo:_(<8 x s32>) = G_BITCAST %srs_lo
///   %hi:_(<8 x s32>) = G_BITCAST %srs_hi
///   G_STORE %lo, %ptr1
///   G_STORE %hi, %ptr2
bool matchSplitIntrinsicForStore(MachineInstr &MI, MachineRegisterInfo &MRI,
                                 const AIEBaseInstrInfo &TII,
                                 BuildFnTy &MatchInfo);

bool matchVShiftChainToCopy(MachineInstr &MI, MachineRegisterInfo &MRI,
                            const AIEBaseInstrInfo &TII, BuildFnTy &MatchInfo);

bool matchPadUnpadToCopy(MachineInstr &MI, MachineRegisterInfo &MRI,
                         const AIEBaseInstrInfo &TII, BuildFnTy &MatchInfo);

bool matchUnpadPadToCopy(MachineInstr &MI, MachineRegisterInfo &MRI,
                         const AIEBaseInstrInfo &TII, BuildFnTy &MatchInfo);

bool matchUnpadUnmerge(MachineInstr &MI, MachineRegisterInfo &MRI,
                       const AIEBaseInstrInfo &TII, CombinerHelper &Helper,
                       GISelChangeObserver &Observer, BuildFnTy &MatchInfo);

bool matchPadUnpadFusion(MachineInstr &MI, MachineRegisterInfo &MRI,
                         const AIEBaseInstrInfo &TII, BuildFnTy &MatchInfo);

bool matchPadPadFusion(MachineInstr &MI, MachineRegisterInfo &MRI,
                       const AIEBaseInstrInfo &TII, BuildFnTy &MatchInfo);

bool matchUnpadUnpadFusion(MachineInstr &MI, MachineRegisterInfo &MRI,
                           const AIEBaseInstrInfo &TII, BuildFnTy &MatchInfo);

bool matchConcatUnpadFusion(MachineInstr &MI, MachineRegisterInfo &MRI,
                            const AIEBaseInstrInfo &TII, BuildFnTy &MatchInfo);

bool matchFlattenNestedConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                              const AIEBaseInstrInfo &TII,
                              BuildFnTy &MatchInfo);

/// Match redundant exact widen/narrow conversion pairs and eliminate them.
/// Controlled by -aie-combine-redundant-widen-narrow-conversions flag.
bool matchRedundantWidenNarrowConversion(MachineInstr &MI,
                                         MachineRegisterInfo &MRI,
                                         const AIEBaseInstrInfo &TII,
                                         BuildFnTy &MatchInfo);

/// Match a pattern of chained G_PTR_ADD operations where both offsets
/// come from non-constant sources, like G_TRUNC/G_ZEXTLOAD of s32 values. The
/// pattern:
///   %195:modregbank(s20) = G_TRUNC %1021(s32)
///   %201:modregbank(s20) = G_TRUNC %164(s32)
///   %337:ptrregbank(p0) = G_PTR_ADD %335, %195(s20)
///   %339:ptrregbank(p0) = G_PTR_ADD %337, %201(s20)
/// Can be optimized to:
///   %combined:_(s32) = G_ADD %1021(s32), %164(s32)
///   %offset:modregbank(s20) = G_TRUNC %combined(s32)
///   %339:ptrregbank(p0) = G_PTR_ADD %335, %offset(s20)
/// Requires one s32 source to dominate the other for safe insertion.
bool matchChainedPtrAddWithNonConstOffsets(MachineInstr &MI,
                                           MachineRegisterInfo &MRI,
                                           CombinerHelper &Helper,
                                           BuildFnTy &MatchInfo);

/// Match a pattern of G_AIE_POSTINC_LOAD/STORE followed by G_PTR_ADD where both
/// offsets come from G_TRUNC of s32 values. The pattern:
///   %offset1:_(s20) = G_TRUNC %src1(s32)
///   %offset2:_(s20) = G_TRUNC %src2(s32)
///   %data:_(<32 x s16>), %ptr1:_(p0) = G_AIE_POSTINC_LOAD %base, %offset1(s20)
///   %ptr2:_(p0) = G_PTR_ADD %ptr1, %offset2(s20)
/// Can be optimized to:
///   %combined:_(s32) = G_ADD %src1(s32), %src2(s32)
///   %offset:_(s20) = G_TRUNC %combined(s32)
///   %data:_(<32 x s16>), %ptr2:_(p0) = G_AIE_POSTINC_LOAD %base, %offset(s20)
/// Requires the POSTINC's pointer output to have only one use (the
/// PTR_ADD) and one s32 source to dominate the other for safe insertion.
bool matchPostIncLoadStorePtrAddWithTrunc(MachineInstr &MI,
                                          MachineRegisterInfo &MRI,
                                          CombinerHelper &Helper,
                                          const AIEBaseInstrInfo &TII,
                                          GISelChangeObserver &Observer,
                                          BuildFnTy &MatchInfo);

/// Match data for combine_lag_ptr_add: the post-increment pointer register
/// produced by a dominating POSTINC instruction and the step constant S it
/// adds to the pre-increment pointer.
struct LagPtrAddMatchInfo {
  Register PostReg; ///< %post_ptr produced by the dominating POSTINC.
  int64_t Step;     ///< Step constant S added by the POSTINC (post = pre + S).
};

/// Eliminate lag-register copies by rewriting G_PTR_ADD uses of the
/// pre-increment pointer register in terms of the POSTINC's post-pointer def.
/// Rewrites  %dst = G_PTR_ADD %pre, C
/// to        %dst = COPY %post           (when C == S)
/// or        %dst = G_PTR_ADD %post, (C-S)  (otherwise)
bool matchLagPtrAdd(MachineInstr &MI, MachineRegisterInfo &MRI,
                    const AIEBaseInstrInfo &TII, CombinerHelper &Helper,
                    LagPtrAddMatchInfo &MatchInfo);
void applyLagPtrAdd(MachineInstr &MI, MachineRegisterInfo &MRI,
                    MachineIRBuilder &B, GISelChangeObserver &Observer,
                    LagPtrAddMatchInfo &MatchInfo);

} // namespace llvm

#endif
