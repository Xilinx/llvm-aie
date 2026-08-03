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
  //
  // Only SIZES are cross-checked here. Offsets cannot be: a sub-register
  // index's offset is declared once for the level it belongs to and reused at
  // others, so `getSubRegIndex(Reg, Leaf)` does not compose transitively.
  // computePlacements descends the hierarchy instead, and the tiling it
  // insists on is the real layout check.
  if (!Ranges.empty())
    for (unsigned Reg = 1; Reg < NumRegs; ++Reg)
      for (unsigned Idx = 1; Idx < MRI.getNumSubRegIndices(); ++Idx) {
        MCRegister Sub = MRI.getSubReg(Reg, Idx);
        if (!Sub || Idx >= Ranges.size())
          continue;
        assert((!Widths[Sub.id()] || Ranges[Idx].sizeBits == Widths[Sub.id()]) &&
               "sub-register range table disagrees with the MC layer");
      }

  // Where every composed register's leaves sit, worked out once.
  Composition.resize(NumRegs);
  if (!Ranges.empty())
    for (unsigned Reg = 1; Reg < NumRegs; ++Reg) {
      if (Widths[Reg] || !ClassWidths[Reg])
        continue;
      if (!computePlacements(Reg, ClassWidths[Reg], Composition[Reg]))
        Composition[Reg].clear();
    }

  // Every register starts with one entry visible from cycle 0, so a read
  // before any write sees the reset value rather than an empty history.
  Storage.resize(NumRegs);
  for (unsigned Reg = 0; Reg != NumRegs; ++Reg)
    // The reset value forwards from nothing, so it carries no bypass class.
    Storage[Reg].push_back({0, APInt(std::max(Widths[Reg], 1u), 0), /*fwd=*/0});
  Poisoned.assign(NumRegs, false);
}

unsigned AIERegisterFile::getWidth(MCRegister Reg) const {
  return Reg.id() < Widths.size() ? Widths[Reg.id()] : 0;
}

unsigned AIERegisterFile::getClassWidth(MCRegister Reg) const {
  return Reg.id() < ClassWidths.size() ? ClassWidths[Reg.id()] : 0;
}

bool AIERegisterFile::computePlacements(
    MCRegister Reg, unsigned Width, SmallVectorImpl<Placement> &Out) const {
  // Immediate children only: those not reachable through another child. The
  // MC layer flattens the hierarchy, so ex0 lists eh0 alongside e0, and taking
  // the flattened list at face value is what puts eh0 at its within-e0 offset.
  for (MCPhysReg Sub : MRI.subregs(Reg)) {
    bool Indirect = false;
    for (MCPhysReg Other : MRI.subregs(Reg))
      if (Other != Sub && MRI.isSubRegister(Other, Sub)) {
        Indirect = true;
        break;
      }
    if (Indirect)
      continue;

    unsigned Idx = MRI.getSubRegIndex(Reg, Sub);
    if (!Idx || Idx >= Ranges.size())
      return false;
    const AIESubRegRange &R = Ranges[Idx];
    if (R.offsetBits == kNoContiguousRange ||
        uint64_t(R.offsetBits) + R.sizeBits > Width)
      return false;

    if (Widths[Sub]) {
      Out.push_back({MCRegister(Sub), R.offsetBits, R.sizeBits});
      continue;
    }
    // A composed child: place its own leaves, shifted by where it sits here.
    SmallVector<Placement, 4> Inner;
    if (!computePlacements(MCRegister(Sub), R.sizeBits, Inner))
      return false;
    for (Placement &P : Inner)
      Out.push_back({P.Leaf, P.offsetBits + R.offsetBits, P.sizeBits});
  }

  // An exact tiling of [0, Width), which is what makes composing sound. Sizes
  // summing to Width is not enough -- the bfp16 registers pass that with
  // overlapping ranges.
  //
  // KNOWN GAP, and a refusal rather than a wrong answer: the 12 ex and 6 ey
  // registers carry TWO decompositions -- ex0 is [x0, e0] as declared, and the
  // MC layer also lists the inferred [ewl0, ewh0] regrouping -- so their
  // immediate children overlap and no tiling comes out. Picking the declared
  // partition needs a rule this cannot see, and nothing executes a bfp16
  // vector register yet, so it stays unbuilt until a test needs one.
  llvm::sort(Out, [](const Placement &A, const Placement &B) {
    return A.offsetBits < B.offsetBits;
  });
  unsigned At = 0;
  for (const Placement &P : Out) {
    if (P.offsetBits != At)
      return false;
    At += P.sizeBits;
  }
  return At == Width;
}

bool AIERegisterFile::readComposed(MCRegister Reg, APInt &Out, uint64_t Cycle,
                                   unsigned Fwd) const {
  const unsigned W = getClassWidth(Reg);
  if (!W || Reg.id() >= Composition.size() || Composition[Reg.id()].empty())
    return false;
  APInt Acc(W, 0);
  for (const Placement &P : Composition[Reg.id()]) {
    APInt V;
    if (!read(P.Leaf, V, Cycle, Fwd))
      return false;
    Acc.insertBits(V.zextOrTrunc(P.sizeBits), P.offsetBits);
  }
  Out = Acc;
  return true;
}

bool AIERegisterFile::writeComposed(MCRegister Reg, const APInt &Value,
                                    uint64_t VisibleAt, unsigned Fwd) {
  const unsigned W = getClassWidth(Reg);
  if (!W || Reg.id() >= Composition.size() || Composition[Reg.id()].empty())
    return false;
  const APInt V = Value.zextOrTrunc(W);
  for (const Placement &P : Composition[Reg.id()])
    write(P.Leaf, V.extractBits(P.sizeBits, P.offsetBits), VisibleAt, Fwd);
  return true;
}

bool AIERegisterFile::read(MCRegister Reg, APInt &Out, uint64_t Cycle,
                           unsigned Fwd) const {
  if (!getWidth(Reg))
    return readComposed(Reg, Out, Cycle, Fwd);
  if (Poisoned[Reg.id()])
    return false;
  // Newest entry that has landed STRICTLY before Cycle: a write becoming
  // visible at N is not seen by a read at N. Measured, not reasoned -- the
  // discriminator is `or r17, r7, r2` landing exactly on a load's cycle and
  // needing the value from before it.
  //
  // ... unless the reading operand shares a pipeline-forwarding class with the
  // one that produced the value, in which case it comes off the bypass and a
  // write landing ON this cycle IS seen. Worth exactly one cycle, matching
  // AIEBaseInstrInfo::getNumBypassedCycles, which is what the schedule assumed.
  const SmallVectorImpl<Timed> &H = Storage[Reg.id()];
  for (auto It = H.rbegin(), E = H.rend(); It != E; ++It) {
    const bool Bypassed = Fwd && It->fwd == Fwd;
    if (It->visibleAt < Cycle || (Bypassed && It->visibleAt <= Cycle)) {
      Out = It->value;
      return true;
    }
  }
  // Every entry is still in flight, which means the reset value was already
  // superseded and this read is inside a producer's latency window with
  // nothing older to see. Refusing beats inventing one.
  return false;
}

bool AIERegisterFile::write(MCRegister Reg, const APInt &Value,
                            uint64_t VisibleAt, unsigned Fwd) {
  const unsigned W = getWidth(Reg);
  if (!W)
    return writeComposed(Reg, Value, VisibleAt, Fwd);
  SmallVectorImpl<Timed> &H = Storage[Reg.id()];
  // Keep newest-last by visibleAt. Two writes can land out of issue order when
  // their latencies differ, and program order is what decides the winner, so a
  // later-issued write replaces any entry that would land at or after it.
  while (!H.empty() && H.back().visibleAt > VisibleAt)
    H.pop_back();
  H.push_back({VisibleAt, Value.zextOrTrunc(W), Fwd});
  Poisoned[Reg.id()] = false;
  return true;
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
