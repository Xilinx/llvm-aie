//===--- AIEGlobalCombiner.cpp - Global Combiner Helper Interface ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Implements the generic algorithmic parts of the global combiner search.
//
//===----------------------------------------------------------------------===//

#include "AIEGlobalCombiner.h"

namespace llvm::AIE {

// -------------------------- CombinerGain -----------------------------------//

CombinerGain::CombinerGain(std::initializer_list<int> InitialGain) {
  assert(InitialGain.size() <= GainVector.size());
  reset();

  std::copy(InitialGain.begin(), InitialGain.end(), GainVector.begin());
}

void CombinerGain::reset() {
  std::fill(GainVector.begin(), GainVector.end(), 0);
}

bool CombinerGain::operator>(const CombinerGain &Rhs) const {
  for (unsigned Idx = 0; Idx < GainVector.size(); Idx++) {
    if (GainVector[Idx] == Rhs.GainVector[Idx])
      continue;

    return GainVector[Idx] > Rhs.GainVector[Idx];
  }
  return false;
}

bool CombinerGain::operator<(const CombinerGain &Rhs) const {
  assert(Rhs.GainVector.size() == GainVector.size());
  return GainVector < Rhs.GainVector;
}

bool CombinerGain::operator==(const CombinerGain &Rhs) const {
  assert(Rhs.GainVector.size() == GainVector.size());
  return GainVector == Rhs.GainVector;
}

bool CombinerGain::operator!=(const CombinerGain &Rhs) const {
  assert(Rhs.GainVector.size() == GainVector.size());
  return !(*this == Rhs);
}

CombinerGain &CombinerGain::operator+=(const CombinerGain &Rhs) {
  for (unsigned Idx = 0; Idx < GainVector.size(); Idx++) {
    GainVector[Idx] += Rhs.GainVector[Idx];
  }
  return *this;
}

CombinerGain CombinerGain::operator+(const CombinerGain &Rhs) const {
  CombinerGain Result(*this);
  return Result += Rhs;
}

CombinerGain &CombinerGain::operator-=(const CombinerGain &Rhs) {
  for (unsigned Idx = 0; Idx < GainVector.size(); Idx++) {
    GainVector[Idx] -= Rhs.GainVector[Idx];
  }
  return *this;
}

CombinerGain CombinerGain::operator-(const CombinerGain &Rhs) const {
  CombinerGain Result(*this);
  return Result -= Rhs;
}

// --------------------------- Combiner --------------------------------------//

Combiner::Combiner(std::vector<MachineInstr *> CombineInstrs,
                   unsigned CombinedInstrOpcode, MachineInstr *InsertionPoint,
                   MachineInstr *CombineRoot,
                   std::vector<MachineInstr *> MoveUpInstrsToCombineRoot,
                   BitVector RemoveInstrs, StringRef Name)
    : CombineRoot(CombineRoot), InsertionPoint(InsertionPoint),
      CombineInstrs(CombineInstrs),
      MoveUpInstrsToInsertionPoint(MoveUpInstrsToCombineRoot),
      CombinedInstrOpcode(CombinedInstrOpcode), RemoveInstrs(RemoveInstrs),
      Name(Name) {}

void Combiner::dumpFull() const { dumpFull(nullptr, nullptr); }

void Combiner::dumpFull(unsigned *GlobalID, CombinerGain *Gain) const {
  if (!GlobalID || !Gain)
    dbgs() << "{Combiner " << Name << "\n";
  else
    dbgs() << "{Combiner " << Name << " [" << *GlobalID << "]; Gain = " << *Gain
           << "\n";

  for (auto *MI : CombineInstrs) {
    dbgs() << "  " << *MI;
  }
  dbgs() << "    Insertion Point: ";
  dbgs() << "              " << *InsertionPoint;

  if (!MoveUpInstrsToInsertionPoint.empty()) {
    dbgs() << "    to Move Up: \n";
    for (const auto *MI : MoveUpInstrsToInsertionPoint)
      dbgs() << "              " << *MI;
  }
  if (!DelayInstrToInsertionPoint.empty()) {
    dbgs() << "    Move to Insertion: \n";
    for (const auto *MI : DelayInstrToInsertionPoint)
      dbgs() << "                  " << *MI;
  }
  if (!DelayInstrPastInsertionPoint.empty()) {
    dbgs() << "    Delay past Insertion: \n";
    for (const auto *MI : DelayInstrPastInsertionPoint)
      dbgs() << "                          " << *MI;
  }
  dbgs() << "}\n";
}

// ---------------------------------------------------------------------------//

raw_ostream &operator<<(raw_ostream &OS, const CombinerGain &Val) {
  OS << "[";
  for (auto &Gain : Val.GainVector) {
    OS << Gain << ",";
  }
  OS << "]";
  return OS;
}

raw_ostream &operator<<(raw_ostream &OS, const Combiner &Val) {
  OS << Val.Name << " (";
  for (auto *MI : Val.CombineInstrs) {
    OS << MI->getOperand(0) << ",";
  }
  OS << ") ";
  return OS;
}

}; // namespace llvm::AIE
