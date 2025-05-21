//===--- AIEGlobalCombiner.h - Global Combiner Helper Interface -----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This is a generic global (Machine Basic Block-level) Combiner that searches
// through multiple combiners to provide a set of combiners that maximize the
// gain of the individual combiners.
//
// The interface is AIEGlobalCombiner, which consists of a setup-function to
// setup the Machine Basic Block, a generateCombiners-function that generates
// all possible combiners and the findBeneficialCombiners function that searches
// for a profitable set of combiners.
//
// A Combiner (CombineResult) consists of a Root, which is a MachineInstruction
// that is replaced by a new Instruction of the Combiner. Additionally, multiple
// Combiners can be clustered together by the Registers that are common between
// Combiners. Combiners with common clustered Registers may conflict with each
// other, meaning that they cannot be applied together.
//
// The search for a set of beneficial combiners is divided into two parts, a
// greedy heuristic and the search. The search is inspired by A*. The goal of
// the search is to maximize the gain of the Combiners.
//

#ifndef LLVM_LIB_TARGET_AIE_AIEGLOBALCOMBINER_H
#define LLVM_LIB_TARGET_AIE_AIEGLOBALCOMBINER_H

#include "AIEBaseInstrInfo.h"
#include "AIEInterBlockScheduling.h"

namespace llvm::AIE {

class CombinerGain {
public:
  // Hierarchical gain vector. Larger Indices are less important in the Gain
  // calculation
  std::array<int, 3> GainVector;

  CombinerGain() : CombinerGain({}) {}
  CombinerGain(std::initializer_list<int> InitialGain);
  CombinerGain(const CombinerGain &Other) = default;

  virtual ~CombinerGain() = default;

  /// reset the GainVector
  void reset();

  bool operator>(const CombinerGain &Rhs) const;
  bool operator<=(const CombinerGain &Rhs) const;
  bool operator<(const CombinerGain &Rhs) const;
  bool operator==(const CombinerGain &Rhs) const;
  bool operator!=(const CombinerGain &Rhs) const;

  CombinerGain &operator+=(const CombinerGain &Rhs);
  CombinerGain operator+(const CombinerGain &Rhs) const;
  CombinerGain &operator-=(const CombinerGain &Rhs);
  CombinerGain operator-(const CombinerGain &Rhs) const;
};

raw_ostream &operator<<(raw_ostream &OS, const CombinerGain &Val);

class Combiner {
public:
  /// Root of the Combiner
  MachineInstr *CombineRoot = nullptr;
  /// Keep track of the MachineInstrs used by the Combiner
  /// If null insert instructions at the end of
  /// the BB, otherwise insert just before this
  /// Instruction
  MachineInstr *InsertionPoint = nullptr;
  std::vector<MachineInstr *> CombineInstrs;
  /// Instructions to be moved up just before InsertionPoint
  std::vector<MachineInstr *> MoveUpInstrsToInsertionPoint;
  /// Instructions to move down to InsertionPoint
  std::vector<MachineInstr *> DelayInstrToInsertionPoint;
  /// Instruction to move below InsertionPoint
  std::vector<MachineInstr *> DelayInstrPastInsertionPoint;
  /// Opcode of the combined instruction
  unsigned CombinedInstrOpcode = -1;
  /// Subset of CombineInstructions to be removed
  BitVector RemoveInstrs;
  StringRef Name;

  Combiner() = default;
  Combiner(std::vector<MachineInstr *> CombineInstrs,
           unsigned CombinedInstrOpcode, MachineInstr *InsertionPoint,
           MachineInstr *CombineRoot,
           std::vector<MachineInstr *> MoveUpInstrsToInsertionPoint,
           BitVector RemoveInstrs, StringRef Name);

  /// Dump all the relevant information of the Combiner.
  void dumpFull() const;
  void dumpFull(unsigned *GlobalID, CombinerGain *Gain) const;
};

raw_ostream &operator<<(raw_ostream &OS, const Combiner &Val);

/// Abstract Combiner Class. Every Combiner should derive from this and
/// implement the necessary methods.
class GenericCombiner {

  /// \return whether Candidate is Movable in the direction \p MoveDown to
  /// \p InsertionPointDepth . The Instructions are collected in \p ToMove .
  bool isMovable(SUnit &Candidate, const unsigned InsertionPointDepth,
                 const AIE::DataDependenceHelper &DAG, const bool MoveDown,
                 std::set<SUnit *> &ToMove);

  /// \return whether Combine Instructions can be reordered and the Combiner is
  /// valid
  bool canReorderCombineInstrs(const AIE::DataDependenceHelper &DAG,
                               const MachineRegisterInfo &MRI,
                               const AIEBaseInstrInfo &TII);

protected:
  /// Collect all Immediate Values that have to be stored in Registers, because
  /// the bitencoding does not allow it to be encoded in the Instruction
  std::vector<APInt> ImmInRegs;

  /// Unique ID for the Combiner. It is used to keep track of Conflicts.
  unsigned GlobalID = -1;
  /// Keeps track of Combiners that have conflicts with this Combiner. A
  /// Combiner is encoded through its GlobalID.
  BitVector ConflictCombiners;

  /// \return SUnits that have to be moved up or down before the InsertionPoint
  /// : <Move Up, Move Down>. \pre InsertionPoint has been set
  virtual std::optional<std::pair<std::vector<SUnit *>, std::vector<SUnit *>>>
  getInstructionsToMove(const AIE::DataDependenceHelper &DAG);

  /// Set the OpCode for the Combiner. \return whether the Opcode can be set
  virtual bool tryToSetCombinedOpCode() = 0;

  /// Set the InsertionPoint of the Combiner, i.e. the position, where the
  /// CombinedOpcode Instruction should be inserted
  virtual void setInsertionPoint() = 0;

  /// Set the Gain for the Combiner
  virtual void adjustGain(const MachineDominatorTree &MDT) = 0;

  /// \return Immediate that don't fit into the immediate Bit Encoding and have
  /// to be stored in a Register
  virtual std::vector<APInt> getAllImmInRegs() const { return ImmInRegs; };

  /// \return Overlap Penalty with \p Combiner . Note: Only subtract the
  /// Penalty.
  virtual CombinerGain
  getOverlapPenalty(const GenericCombiner *Combiner) const = 0;

  /// \return Gain of an Immediate that can be reused from \p UsedImmediates
  virtual CombinerGain
  getImmediateReuseGain(const std::vector<APInt> &UsedImmediates) const = 0;

  /// \return If Combiner can handle gain and conflict calculation if
  /// \p Candidate is moved in the direction \p MoveDown
  virtual bool canMove(const SUnit *Candidate, const bool MoveDown) const = 0;

public:
  /// Internal data structure of the Combiner
  Combiner CombinerData;
  /// CombineRoot Node Number in the DAG
  unsigned CombineRootNodeNum = -1;
  /// Node Number in DAG of InsertionPoint
  unsigned InsertionPointNodeNum = -1;
  /// Keep track of the Node Numbers of the CombineInstrs in the DAG
  std::vector<unsigned> CombineInstrNodeNum;

  GenericCombiner() = default;
  GenericCombiner(const GenericCombiner &Other) = default;
  GenericCombiner(StringRef Name) { CombinerData.Name = Name; }

  virtual ~GenericCombiner() = default;

  /// \return A clone of this Combiner.
  /// Note: This is used on the template (empty) Combiner.
  virtual std::unique_ptr<GenericCombiner> clone() const = 0;

  /// \return All Combiners that can be found with the same \p CombineRoot .
  /// Note: This is used on the template (empty) Combiner.
  std::vector<std::unique_ptr<GenericCombiner>>
  applyCombiner(SUnit &CombineRoot, const MachineDominatorTree &MDT,
                const AIE::DataDependenceHelper &DAG,
                const MachineRegisterInfo &MRI,
                const AIEBaseInstrInfo &TII) const;

  /// \return OpCode of the to-be-inserted Instruction
  unsigned getCombinedOpCode() const;

  /// \return MachineInstrs that can be used to form Combiners, based on
  /// \p OriginInstr . Each combination of MachineInstr with \p OriginInstr
  /// could be an individual Combiner
  virtual std::vector<MachineInstr *>
  getCombineCandidates(MachineInstr *OriginInstr,
                       const AIE::DataDependenceHelper &DAG) const = 0;

  ///  Set up the Combiner with \p CombineInstrs .
  /// \return whether the setup was successful.
  /// This method is used if the Combiner is cloned from an empty (default)
  /// Combiner.
  virtual bool setupCombiner(std::vector<MachineInstr *> CombineInstrs,
                             const AIE::DataDependenceHelper *DAG) = 0;

  /// \return Name of the Combiner.
  StringRef getName() const { return CombinerData.Name; };

  /// Dump all the relevant information of the Combiner.
  void dumpFull() const;

  /// \return whether \p MI can be a CombineRoot of the Combiner.
  virtual bool isCombineRootCandidate(const MachineInstr *MI) const = 0;

  /// \return whether \p is the CombineRoot
  bool isCombineRoot(const MachineInstr *MI) const;

  /// \return the Gain that applying the combiner would incur. The gain is
  /// maximized.
  virtual const CombinerGain &getGain() const = 0;

  /// \return Gain when \p AlreadyUsedCombiners are already selected. A
  /// position in \p  AlreadyUsedCombiners corresponds to the position in
  /// \p CombinerSubSet
  CombinerGain
  getOverlapGain(const BitVector &AlreadyUsedCombiners,
                 const std::vector<GenericCombiner *> &CombinerSubSet) const;

  /// \return the Registers by which different Combiners can be clustered
  virtual const std::vector<Register> getClusterRegs() const = 0;

  /// \return whether Combiner contains \p MI in the CombineInstrs
  bool contains(MachineInstr &MI) const;

  /// \return whether this CombineResult conflicts with \p Combiner , i.e.
  /// both cannot be applied together
  bool hasConflict(const GenericCombiner &Combiner) const;

  /// \return whether this Combiner has a Conflict with \p ConflictVector
  bool hasConflict(const BitVector &ConflictVector) const;

  /// \return BitVector containing all the conflicting Combiners with this
  /// Combiner. Each Combiner position in the Bitvector is determined by
  /// GlobalID.
  const BitVector &getConflicts() const { return ConflictCombiners; }

  /// Statically set Conflicts in ConflictBits for each Combiner in
  /// \p AllCombiners that conflicts with this combiner.
  void
  setConflicts(std::vector<std::unique_ptr<GenericCombiner>> &AllCombiners);

  /// \return GlobalID of this Combiner, representing the total number
  /// of Combiners created previously.
  unsigned getGlobalID() const;

  /// Set unique Identifier for this Combiner to \p GlobalID
  void setGlobalID(unsigned GlobalID);
};

raw_ostream &operator<<(raw_ostream &OS, const GenericCombiner &Val);

/// Helper Struct to help searching for a good Combinerset
class CombinerSolution {
  /// Next to considered Combiner in the list of all Combiners
  unsigned Index = 0;
  /// Gain of the Solution
  CombinerGain Gain;
  /// Potential maximum future gain for this Solution
  CombinerGain MaxFutureGain;
  /// Applied Combiners to get the Solution
  BitVector Combiners;
  /// Conflicting Combiners with the Solution
  BitVector ConflictCombiners;

public:
  CombinerSolution() = default;
  CombinerSolution(const CombinerSolution &Other) = default;
  /// Only Initialize the ConflictCombiners
  CombinerSolution(const unsigned NumCombiners);

  /// \p Combiner is at position \p Idx in the \p CombinerSubSet
  CombinerSolution(const CombinerSolution &Other,
                   const GenericCombiner *Combiner,
                   const CombinerGain &MaxFutureGain, const unsigned Idx,
                   const std::vector<GenericCombiner *> &CombinerSubSet);

  unsigned getIndex() const { return Index; }

  void setIndex(unsigned Index) { this->Index = Index; }

  const CombinerGain &getGain() const { return Gain; }

  void setMaxFutureGain(const CombinerGain &Gain) { MaxFutureGain = Gain; }

  const CombinerGain &getMaxFutureGain() const { return MaxFutureGain; }

  bool hasConflict(const GenericCombiner *Combiner) const;

  /// Add \p Combiner to Solution. \p Combiner is at position \p Idx in the
  /// \p CombinerSubSet
  void add(const GenericCombiner *Combiner, const unsigned Idx,
           const std::vector<GenericCombiner *> &CombinerSubSet);

  /// Remove Combiner at Position \p Idx
  void remove(const int Idx);

  void recalculateGain(const std::vector<GenericCombiner *> &AllCombiners);

  const BitVector &getCombinersBitVector() const;

  const BitVector &getConflicts() const { return ConflictCombiners; }

  bool operator<(const CombinerSolution &Other) const;

  bool operator==(const CombinerSolution &Other) const;
};

raw_ostream &operator<<(raw_ostream &OS, const CombinerSolution &Val);

/// Helper Class that contains all the Combiners that have overlapping clustered
/// Registers.
class CombineCandidates {

  std::vector<GenericCombiner *> Combiners;

  /// \return An initial greedy Solution
  CombinerSolution getGreedySolution() const;

  /// \return Maximum potential gain starting with the Solution \p Current and
  /// searching through the Combiner starting at \p Index
  CombinerGain getMaxPotentialGain(const CombinerSolution &Current,
                                   const unsigned Index) const;

public:
  /// Add \p Combiner
  void append(GenericCombiner *Combiner);

  /// \return Combiners from \p OwnedCombineCandidates that maximize the gain
  /// when applied
  std::vector<const GenericCombiner *>
  searchCombinerSet(const std::vector<std::unique_ptr<GenericCombiner>>
                        &OwnedCombineCandidates);

  /// Filtering out all Conflicts with \p UsedCombiners
  void filterOut(const std::vector<const GenericCombiner *> &UsedCombiners);

  /// Clear internal data
  void clear();
};

class AIEGlobalCombiner {
  const MachineRegisterInfo *MRI = nullptr;
  const AIEBaseInstrInfo *TII = nullptr;
  const MachineDominatorTree *MDT = nullptr;
  /// List of all template Combiners
  const std::vector<const GenericCombiner *> CombinerOptions;
  /// Map between Base Register and Registers that derive from the BaseRegister
  std::map<Register, std::vector<Register>> BaseRegisterMap;

  /// DAG of the MBB
  AIE::DataDependenceHelper &DAG;
  /// Clustered overlapping Combiner Candidates
  std::map<Register, CombineCandidates> ClusteredCombiners;
  /// Owner of the CombineCandidates
  std::vector<std::unique_ptr<GenericCombiner>> OwnedCombineCandidates;

  /// Generate all Combiner possible
  void generateCombiners(MachineBasicBlock &MBB);

  /// Search for the best combiner set that maximizes the gain from the
  /// Combiners
  std::vector<const GenericCombiner *> findBeneficialCombiners();

  /// Reverse Address Chaining effect to find out the base Pointer of this
  /// Combiner
  /// \return Register of the Base Pointer
  Register getClusterBaseRegister(GenericCombiner &Combiner);

  /// Set instructions to move up or down to the InsertionPoint for each
  /// Combiner in \p Combiners
  void setMovableInstrs(std::vector<GenericCombiner *> &Combiners);

  /// Selected Combiners that help maximize the Combiner gain
  std::vector<const GenericCombiner *> FixedCombiners;
  /// Index to current CombineCandidate to process
  int CombineIdx = 0;

  /// clear combiner related temporary data
  void clear();

  /// Calculate all possible Combiners that are possible with \p CombineRoot and
  /// \p Combiner
  void calculateCombineCandidates(SUnit &CombineRoot,
                                  const GenericCombiner *Combiner);

  void calculateCombinerConflicts();

  /// \return CombineCandidates sorted by highest potential gain
  std::vector<CombineCandidates> getCombineCandidates(
      std::map<Register, CombineCandidates> ClusteredCombiners);

public:
  AIEGlobalCombiner(const std::vector<const GenericCombiner *> &Combiners,
                    const MachineDominatorTree &MDT,
                    AIE::DataDependenceHelper &DAG,
                    const MachineRegisterInfo *MRI, const AIEBaseInstrInfo *TII)
      : MRI(MRI), TII(TII), MDT(&MDT), CombinerOptions(Combiners), DAG(DAG) {}

  std::vector<const GenericCombiner *> getCombiners(MachineBasicBlock &MBB);
};

namespace GlobalCombiner {
/// Init the \p DAG for the \p MBB
void initDAG(AIE::DataDependenceHelper &DAG, MachineBasicBlock &MBB);
} // namespace GlobalCombiner

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIEGLOBALCOMBINER_H
