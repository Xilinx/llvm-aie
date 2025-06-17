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

class GenericCombiner {

public:
  Combiner CombinerData;
};

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIEGLOBALCOMBINER_H
