//===- AIECoreState.cpp - Architectural state of an AIE core --------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIECoreState.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace llvm::AIESim;

AIERegisterFile::AIERegisterFile(const MCRegisterInfo &MRI,
                                 ArrayRef<AIESubRegRange> SubRegRanges)
    : MRI(MRI), Ranges(SubRegRanges) {
  const unsigned NumRegs = MRI.getNumRegs();
  Widths.assign(NumRegs, 0);

  // A register's width is the narrowest register class holding it. The classes
  // are the only place the MC layer records a size, and a register that is
  // 20 bits wide is reachable through 32-bit classes as well: sp is in both
  // AIE2P20BitRegisterClass and the 32-bit mMvSclDst.
  for (unsigned I = 0, E = MRI.getNumRegClasses(); I != E; ++I) {
    const MCRegisterClass &RC = MRI.getRegClass(I);
    for (MCPhysReg Reg : RC) {
      unsigned &W = Widths[Reg];
      if (W == 0 || RC.getSizeInBits() < W)
        W = RC.getSizeInBits();
    }
  }

  // Registers built out of others get no storage of their own: an independent
  // copy would never stay in step with the parts. They are composed on demand
  // from the parts instead, which needs the class width kept here.
  ClassWidths = Widths;
  for (unsigned Reg = 1; Reg < NumRegs; ++Reg)
    if (!MRI.subregs(Reg).empty())
      Widths[Reg] = 0;

  // The table must describe the same layout as the MC layer it is paired with.
  // Checked rather than trusted: every index that names a sub-register with
  // storage must agree on that register's width. A transcription slip or a
  // .td change shows up here instead of as a wrong value later.
  if (!Ranges.empty())
    for (unsigned Reg = 1; Reg < NumRegs; ++Reg)
      for (unsigned Idx = 1; Idx < MRI.getNumSubRegIndices(); ++Idx) {
        MCRegister Sub = MRI.getSubReg(Reg, Idx);
        if (!Sub || Idx >= Ranges.size())
          continue;
        assert((!Widths[Sub.id()] || Ranges[Idx].sizeBits == Widths[Sub.id()]) &&
               "sub-register range table disagrees with the MC layer");
        // Transitivity, which is the check that actually pins the LAYOUT: if
        // R contains Sub at offset A and Sub contains Leaf at offset B, then R
        // must contain Leaf at A + B. Sizes alone cannot catch a high/low pair
        // transcribed the wrong way round -- both halves are the same size --
        // and this can, because the sum only works out for one assignment.
        for (unsigned J = 1; J < MRI.getNumSubRegIndices(); ++J) {
          MCRegister Leaf = MRI.getSubReg(Sub, J);
          if (!Leaf || J >= Ranges.size())
            continue;
          unsigned K = MRI.getSubRegIndex(Reg, Leaf);
          if (!K || K >= Ranges.size())
            continue;
          if (Ranges[Idx].offsetBits == kNoContiguousRange ||
              Ranges[J].offsetBits == kNoContiguousRange ||
              Ranges[K].offsetBits == kNoContiguousRange)
            continue;
          assert(Ranges[K].offsetBits ==
                     Ranges[Idx].offsetBits + Ranges[J].offsetBits &&
                 "sub-register offsets are not transitively consistent");
        }
      }

  // Every register starts with one entry visible from cycle 0, so a read
  // before any write sees the reset value rather than an empty history.
  Storage.resize(NumRegs);
  for (unsigned Reg = 0; Reg != NumRegs; ++Reg)
    Storage[Reg].push_back({0, APInt(std::max(Widths[Reg], 1u), 0)});
  Poisoned.assign(NumRegs, false);
}

unsigned AIERegisterFile::getWidth(MCRegister Reg) const {
  return Reg.id() < Widths.size() ? Widths[Reg.id()] : 0;
}

bool AIERegisterFile::readComposed(MCRegister Reg, APInt &Out,
                                  uint64_t Cycle) const {
  const unsigned W = Reg.id() < ClassWidths.size() ? ClassWidths[Reg.id()] : 0;
  if (Ranges.empty() || !W)
    return false;
  // Only sub-registers that HAVE storage are read: they are the leaves, and
  // leaves tile the parent. Intermediate levels (y0 lists x0 as well as x0's
  // own halves) would otherwise be counted twice.
  APInt Acc(W, 0);
  unsigned Covered = 0;
  for (unsigned Idx = 1; Idx < MRI.getNumSubRegIndices(); ++Idx) {
    MCRegister Sub = MRI.getSubReg(Reg, Idx);
    if (!Sub || !Widths[Sub.id()] || Idx >= Ranges.size())
      continue;
    const AIESubRegRange &R = Ranges[Idx];
    if (R.offsetBits == kNoContiguousRange ||
        R.offsetBits + R.sizeBits > W)
      return false; // Not one contiguous run, or not inside the parent.
    APInt V;
    if (!read(Sub, V, Cycle))
      return false;
    Acc.insertBits(V.zextOrTrunc(R.sizeBits), R.offsetBits);
    Covered += R.sizeBits;
  }
  // Refuse a partial composition: a value assembled out of some of the parts
  // is worse than none, because it looks like a number.
  if (Covered != W)
    return false;
  Out = Acc;
  return true;
}

bool AIERegisterFile::writeComposed(MCRegister Reg, const APInt &Value,
                                    uint64_t VisibleAt) {
  const unsigned W = Reg.id() < ClassWidths.size() ? ClassWidths[Reg.id()] : 0;
  if (Ranges.empty() || !W)
    return false;
  unsigned Covered = 0;
  for (unsigned Idx = 1; Idx < MRI.getNumSubRegIndices(); ++Idx) {
    MCRegister Sub = MRI.getSubReg(Reg, Idx);
    if (!Sub || !Widths[Sub.id()] || Idx >= Ranges.size())
      continue;
    const AIESubRegRange &R = Ranges[Idx];
    if (R.offsetBits == kNoContiguousRange || R.offsetBits + R.sizeBits > W)
      return false;
    Covered += R.sizeBits;
  }
  if (Covered != W)
    return false;
  const APInt V = Value.zextOrTrunc(W);
  for (unsigned Idx = 1; Idx < MRI.getNumSubRegIndices(); ++Idx) {
    MCRegister Sub = MRI.getSubReg(Reg, Idx);
    if (!Sub || !Widths[Sub.id()] || Idx >= Ranges.size())
      continue;
    const AIESubRegRange &R = Ranges[Idx];
    write(Sub, V.extractBits(R.sizeBits, R.offsetBits), VisibleAt);
  }
  return true;
}

bool AIERegisterFile::read(MCRegister Reg, APInt &Out, uint64_t Cycle) const {
  if (!getWidth(Reg))
    return readComposed(Reg, Out, Cycle);
  if (Poisoned[Reg.id()])
    return false;
  // Newest entry that has landed by Cycle. `<=` because a schedule where the
  // consumer's use cycle equals the producer's def cycle is legal.
  const SmallVectorImpl<Timed> &H = Storage[Reg.id()];
  for (auto It = H.rbegin(), E = H.rend(); It != E; ++It)
    if (It->visibleAt < Cycle) {
      Out = It->value;
      return true;
    }
  // Every entry is still in flight, which means the reset value was already
  // superseded and this read is inside a producer's latency window with
  // nothing older to see. Refusing beats inventing one.
  return false;
}

void AIERegisterFile::write(MCRegister Reg, const APInt &Value,
                            uint64_t VisibleAt) {
  const unsigned W = getWidth(Reg);
  if (!W) {
    writeComposed(Reg, Value, VisibleAt);
    return;
  }
  SmallVectorImpl<Timed> &H = Storage[Reg.id()];
  // Keep newest-last by visibleAt. Two writes can land out of issue order when
  // their latencies differ, and program order is what decides the winner, so a
  // later-issued write replaces any entry that would land at or after it.
  while (!H.empty() && H.back().visibleAt > VisibleAt)
    H.pop_back();
  H.push_back({VisibleAt, Value.zextOrTrunc(W)});
  Poisoned[Reg.id()] = false;
}

void AIERegisterFile::forgetBefore(uint64_t Horizon) {
  for (SmallVectorImpl<Timed> &H : Storage) {
    // Keep the newest entry that is already visible at Horizon: a later read
    // still resolves to it. Everything strictly older is unreachable.
    size_t KeepFrom = 0;
    for (size_t I = 0; I != H.size(); ++I)
      if (H[I].visibleAt <= Horizon)
        KeepFrom = I;
    if (KeepFrom)
      H.erase(H.begin(), H.begin() + KeepFrom);
  }
}

void AIERegisterFile::poison(MCRegister Reg) {
  if (Reg.id() < Poisoned.size())
    Poisoned[Reg.id()] = true;
}

bool AIERegisterFile::isPoisoned(MCRegister Reg) const {
  return Reg.id() < Poisoned.size() && Poisoned[Reg.id()];
}

void AIERegisterFile::print(raw_ostream &OS) const {
  for (unsigned Reg = 1, E = Widths.size(); Reg != E; ++Reg) {
    if (!Widths[Reg] || Storage[Reg].back().value.isZero())
      continue;
    SmallString<64> Hex;
    Storage[Reg].back().value.toString(Hex, 16, /*Signed=*/false);
    OS << MRI.getName(Reg) << " = 0x" << Hex << '\n';
  }
}
