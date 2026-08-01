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

  Storage.reserve(NumRegs);
  for (unsigned Reg = 0; Reg != NumRegs; ++Reg)
    Storage.emplace_back(std::max(Widths[Reg], 1u), 0);
  Poisoned.assign(NumRegs, false);
}

unsigned AIERegisterFile::getWidth(MCRegister Reg) const {
  return Reg.id() < Widths.size() ? Widths[Reg.id()] : 0;
}

bool AIERegisterFile::read(MCRegister Reg, APInt &Out) const {
  if (!getWidth(Reg) || Poisoned[Reg.id()])
    return false;
  Out = Storage[Reg.id()];
  return true;
}

bool AIERegisterFile::write(MCRegister Reg, const APInt &Value) {
  const unsigned W = getWidth(Reg);
  if (!W)
    return false;
  Storage[Reg.id()] = Value.zextOrTrunc(W);
  Poisoned[Reg.id()] = false;
  return true;
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
    if (!Widths[Reg] || Storage[Reg].isZero())
      continue;
    SmallString<64> Hex;
    Storage[Reg].toString(Hex, 16, /*Signed=*/false);
    OS << MRI.getName(Reg) << " = 0x" << Hex << '\n';
  }
}
