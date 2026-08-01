//===- AIECoreState.h - Architectural state of an AIE core ------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_SIM_AIECORESTATE_H
#define LLVM_LIB_TARGET_AIE_SIM_AIECORESTATE_H

#include "llvm/ADT/APInt.h"
#include "llvm/MC/MCRegister.h"
#include <optional>
#include <vector>

namespace llvm {
class MCRegisterInfo;
class raw_ostream;

namespace AIESim {

/// AIE branches become visible five bundles later
/// (AIEBaseInstrInfo.cpp, NumDelaySlots).
constexpr unsigned NumDelaySlots = 5;

/// The register file, keyed on physical registers.
///
/// Most AIE physical registers carry several register-class aliases that exist
/// only to constrain encoding: mRa, mRm, mRx and friends are all eR. Keying on
/// the register number rather than the operand class keeps one copy of the
/// state.
///
/// Storage lives on registers that have no sub-registers. A register built out
/// of others (l0 = r1:r0, and the vector and accumulator hierarchy) has no
/// storage here, because composing it needs sub-register bit offsets that the
/// MC layer does not carry; accessing one is a fault rather than a guess.
class AIERegisterFile {
public:
  explicit AIERegisterFile(const MCRegisterInfo &MRI);

  /// Width in bits, or 0 when the register has no storage in this model.
  unsigned getWidth(MCRegister Reg) const;

  /// \returns false when \p Reg has no storage, or holds a value this model
  /// declined to compute.
  bool read(MCRegister Reg, APInt &Out) const;
  bool write(MCRegister Reg, const APInt &Value);

  /// Record that hardware would have changed \p Reg in a way the model does not
  /// compute, so that a later read faults instead of returning a stale value.
  /// ADD defines srCarry, and a model that skips the carry-out but still
  /// answers "mov r0, srCarry" is wrong rather than incomplete.
  void poison(MCRegister Reg);
  bool isPoisoned(MCRegister Reg) const;

  void print(raw_ostream &OS) const;

private:
  const MCRegisterInfo &MRI;
  std::vector<unsigned> Widths;
  std::vector<APInt> Storage;
  std::vector<bool> Poisoned;
};

/// The whole architectural state of one core.
class AIECoreState {
public:
  AIECoreState(const MCRegisterInfo &MRI, uint64_t EntryPoint)
      : Regs(MRI), PC(EntryPoint) {}

  AIERegisterFile Regs;
  uint64_t PC;
  uint64_t RetiredBundles = 0;

  /// A taken control transfer, held until its delay slots have retired.
  struct PendingBranch {
    uint64_t Target;
    unsigned BundlesLeft;
  };
  std::optional<PendingBranch> Branch;
};

} // namespace AIESim
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_SIM_AIECORESTATE_H
