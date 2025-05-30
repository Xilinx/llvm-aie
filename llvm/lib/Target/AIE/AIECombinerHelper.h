//=== lib/CodeGen/GlobalISel/AIECombinerHelper.h --------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
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

void foundPattern(MachineInstr &MemI);

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
                         MachineIRBuilder &B);
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
/// combining
bool canAdvanceOp(MachineInstr &MemI, MachineInstr &Dest,
                  const MachineRegisterInfo &MRI);
/// Find the def instruction for \p Reg, folding away any trivial copies and
/// bitcasts. May return nullptr if \p Reg is not a generic virtual register.
MachineInstr *getDefIgnoringCopiesAndBitcasts(Register Reg,
                                              const MachineRegisterInfo &MRI);

bool matchExtractVecEltAndExt(MachineInstr &MI, MachineRegisterInfo &MRI,
                              std::pair<MachineInstr *, bool> &MatchInfo);
void applyExtractVecEltAndExt(MachineInstr &MI, MachineRegisterInfo &MRI,
                              MachineIRBuilder &B,
                              std::pair<MachineInstr *, bool> &MatchInfo);

bool matchSplatVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                      std::pair<Register, Register> &MatchInfo);
bool applySplatVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                      MachineIRBuilder &B,
                      std::pair<Register, Register> &MatchInfo);

bool matchSingleDiffLaneBuildVector(
    MachineInstr &MI, MachineRegisterInfo &MRI,
    AIESingleDiffLaneBuildVectorMatchData &MatchInfo);
bool applySingleDiffLaneBuildVector(
    MachineInstr &MI, MachineRegisterInfo &MRI, MachineIRBuilder &B,
    AIESingleDiffLaneBuildVectorMatchData &MatchInfo);

bool matchSymmetricBuildVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                               GISelChangeObserver &Observer,
                               BuildFnTy &MatchInfo);

bool matchUnpadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                      const AIEBaseInstrInfo &TII);
void applyUnpadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                      MachineIRBuilder &B);

bool matchPadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                    const AIEBaseInstrInfo &TII, Register &MatchedInputVector);
bool matchConcatPadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                          const AIEBaseInstrInfo &TII,
                          Register &MatchedInputVector);
void applyPadVector(MachineInstr &MI, MachineRegisterInfo &MRI,
                    MachineIRBuilder &B, Register MatchedInputVector);
bool tryToCombineVectorShiftsByZero(MachineInstr &MI, MachineRegisterInfo &MRI);

bool matchExtractConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                        const AIEBaseInstrInfo &TII, Register &MatchInfo);
void applyExtractConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                        MachineIRBuilder &B, Register &MatchInfo);

bool matchUnmergeConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                        const AIEBaseInstrInfo &TII,
                        std::pair<MachineInstr *, unsigned> &MatchInfo);
void applyUnmergeConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                        MachineIRBuilder &B,
                        std::pair<MachineInstr *, unsigned> &MatchInfo);

bool matchUpdToConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                      const AIEBaseInstrInfo &TII,
                      std::map<unsigned, Register> &IndexRegMap);
void applyUpdToConcat(MachineInstr &MI, MachineRegisterInfo &MRI,
                      MachineIRBuilder &B,
                      std::map<unsigned, Register> &IndexRegMap);

bool matchLoadStoreSplit(GLoadStore &MI, MachineRegisterInfo &MRI,
                         const AIEBaseInstrInfo &TII, unsigned &MaxMemSize);
void applyLoadStoreSplit(GLoadStore &MI, MachineRegisterInfo &MRI,
                         MachineIRBuilder &B, const unsigned MaxMemSize);

bool matchOffsetLoadStorePtrAdd(MachineInstr &MI, MachineRegisterInfo &MRI,
                                const AIEBaseInstrInfo &TII,
                                std::pair<Register, int64_t> &RegOffset);

void applyOffsetLoadStorePtrAdd(MachineInstr &MI, MachineRegisterInfo &MRI,
                                MachineIRBuilder &B,
                                const std::pair<Register, int64_t> &RegOffset);

bool matchOffsetLoadStoreSharePtrAdd(MachineInstr &MI, MachineRegisterInfo &MRI,
                                     CombinerHelper &Helper,
                                     const AIEBaseInstrInfo &TII,
                                     Register &PtrAddReg);

void applyOffsetLoadStoreSharePtrAdd(MachineInstr &MI, MachineRegisterInfo &MRI,
                                     MachineIRBuilder &B, Register &PtrAddReg);

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

bool matchShuffleToExtractInsertElt(MachineInstr &MI, MachineRegisterInfo &MRI,
                                    BuildFnTy &MatchInfo);

bool matchPairedExtracts(MachineInstr &MI, MachineRegisterInfo &MRI,
                         CombinerHelper &Helper, const TargetInstrInfo &TII,
                         BuildFnTy &MatchInfo);

bool matchShuffleToExtractInsertEltToBroadcast(MachineInstr &MI,
                                               MachineRegisterInfo &MRI,
                                               BuildFnTy &MatchInfo);

bool matchBroadcastToShl(MachineInstr &MI, MachineRegisterInfo &MRI,
                         const AIEBaseInstrInfo &TII, BuildFnTy &MatchInfo);

bool matchNarrowPhi(MachineInstr &Phi, MachineRegisterInfo &MRI,
                    GISelChangeObserver &Observer, BuildFnTy &MatchInfo);

bool matchNarrowTrunc(MachineInstr &MI, MachineRegisterInfo &MRI,
                      GISelChangeObserver &Observer, BuildFnTy &MatchInfo);

bool matchNarrowZext(MachineInstr &MI, MachineRegisterInfo &MRI,
                     GISelChangeObserver &Observer, BuildFnTy &MatchInfo);

} // namespace llvm

#endif
