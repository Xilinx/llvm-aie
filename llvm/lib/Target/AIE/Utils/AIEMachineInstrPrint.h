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

#include "llvm/CodeGen/MachineInstr.h"
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

/// Helper class to print just the opcode of a MachineInstr.
///
/// Usage:
///   dbgs() << OpcodeOnly(MI);
///
class OpcodeOnly {
  const MachineInstr &MI;

public:
  explicit OpcodeOnly(MachineInstr &MI) : MI(MI) {}

  void print(raw_ostream &OS) const {
    MI.print(OS, /*IsStandalone=*/true, /*SkipOpers=*/true,
             /*SkipDebugLoc=*/true, /*AddNewLine=*/false);
  }
};

inline raw_ostream &operator<<(raw_ostream &OS, const OpcodeOnly &ND) {
  ND.print(OS);
  return OS;
}

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_UTILS_AIEMACHINEINSTRPRINT_H
