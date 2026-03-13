//===- AIEMachineInstrPrint.h - MachineInstr Print Helpers ------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains helper classes for printing MachineInstr objects with
// various formatting options.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_UTILS_AIEMACHINEINSTRPRINT_H
#define LLVM_LIB_TARGET_AIE_UTILS_AIEMACHINEINSTRPRINT_H

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm::AIE {

/// Helper class to print a MachineInstr without debug location information.
///
/// Usage:
///   dbgs() << NoDebug(MI);
///
class NoDebug {
  const MachineInstr &MI;

public:
  explicit NoDebug(const MachineInstr &MI) : MI(MI) {}

  void print(raw_ostream &OS) const {
    MI.print(OS, /*IsStandalone=*/true, /*SkipOpers=*/false,
             /*SkipDebugLoc=*/true, /*AddNewLine=*/false);
  }
};

inline raw_ostream &operator<<(raw_ostream &OS, const NoDebug &ND) {
  ND.print(OS);
  return OS;
}

/// Helper class to print just the opcode name of a MachineInstr.
///
/// Usage:
///   dbgs() << OpcodeOnly(MI);
///   dbgs() << OpcodeOnly(MI, 10);  // Clip to 10 characters
///
class OpcodeOnly {
  const MachineInstr &MI;
  unsigned MaxLength;

public:
  explicit OpcodeOnly(MachineInstr &MI, unsigned MaxLength = 0)
      : MI(MI), MaxLength(MaxLength) {}

  void print(raw_ostream &OS) const {
    const auto *TII = MI.getMF()->getSubtarget().getInstrInfo();
    StringRef Name = TII->getName(MI.getOpcode());
    if (MaxLength > 0 && Name.size() > MaxLength) {
      OS << Name.substr(0, MaxLength);
    } else {
      OS << Name;
    }
  }
};

inline raw_ostream &operator<<(raw_ostream &OS, const OpcodeOnly &ND) {
  ND.print(OS);
  return OS;
}

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_UTILS_AIEMACHINEINSTRPRINT_H
