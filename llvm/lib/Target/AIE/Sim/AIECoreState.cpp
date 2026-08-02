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

AIERegisterFile::AIERegisterFile(const MCRegisterInfo &MRI) : MRI(MRI) {
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

  // Registers built out of others are left without storage, so that reaching
  // one faults instead of silently reading an independent copy that aliasing
  // would never keep in step.
  for (unsigned Reg = 1; Reg < NumRegs; ++Reg)
    if (!MRI.subregs(Reg).empty())
      Widths[Reg] = 0;

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

bool AIERegisterFile::read(MCRegister Reg, APInt &Out, uint64_t Cycle) const {
  if (!getWidth(Reg) || Poisoned[Reg.id()])
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
  if (!W)
    return;
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
