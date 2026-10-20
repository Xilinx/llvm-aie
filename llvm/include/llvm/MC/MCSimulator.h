//===- llvm/MC/MCSimulator.h - Target instruction simulator -----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares MCSimulator, which a target registers through
// TargetRegistry so that a driver can execute its machine code without
// including target headers.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_MC_MCSIMULATOR_H
#define LLVM_MC_MCSIMULATOR_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Compiler.h"
#include <cstdint>
#include <utility>

namespace llvm {

class raw_ostream;

/// Executes machine code for one core against memory the simulator owns.
class LLVM_ABI MCSimulator {
public:
  enum class Status {
    Done,
    Fault,
    /// Waiting on a resource that nothing will provide.
    Stalled,
    StepLimit,
  };

  virtual ~MCSimulator() = default;

  /// Back [Addr, Addr + Bytes.size()) with a copy of \p Bytes.
  virtual void mapMemory(uint64_t Addr, ArrayRef<uint8_t> Bytes) = 0;
  /// Back [Addr, Addr + Size), reading as zero where nothing was written.
  virtual void mapZeroedMemory(uint64_t Addr, uint64_t Size) = 0;
  /// \returns empty when the range is not backed.
  virtual ArrayRef<uint8_t> readMemory(uint64_t Addr, uint64_t Size) const = 0;

  /// Execute from \p Entry, issuing at most \p MaxSteps instructions or
  /// bundles.
  virtual Status run(uint64_t Entry, uint64_t MaxSteps) = 0;

  virtual uint64_t getPC() const = 0;
  virtual StringRef getFaultMessage() const = 0;

  /// Target-defined counters, such as retired bundles and cycles.
  virtual void printStatistics(raw_ostream &OS) const = 0;
  virtual void printRegisters(raw_ostream &OS) const = 0;

  /// Each opcode the run reached, paired with whether it has semantics.
  virtual void
  getCoverage(SmallVectorImpl<std::pair<unsigned, bool>> &Out) const = 0;
};

} // namespace llvm

#endif // LLVM_MC_MCSIMULATOR_H
