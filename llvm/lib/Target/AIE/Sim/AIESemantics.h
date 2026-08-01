//===- AIESemantics.h - Behaviour of one subtarget's ISA --------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_SIM_AIESEMANTICS_H
#define LLVM_LIB_TARGET_AIE_SIM_AIESEMANTICS_H

#include "AIECoreState.h"
#include "AIEHostInterface.h"
#include "llvm/ADT/SmallVector.h"
#include <array>
#include <memory>
#include <optional>
#include <string>

namespace llvm {
class MCInst;
class MCInstrInfo;
class MCRegisterInfo;
class MCSubtargetInfo;

namespace AIESim {

enum class StepResult {
  Retired,
  /// A port was not ready. Nothing was committed and the bundle is re-issued.
  Stalled,
  /// The core reached its end instruction.
  Done,
  Fault,
};

/// What a slot wants to change, computed against the state at the top of the
/// bundle and committed only once every slot has succeeded.
///
/// Slots in one bundle issue together, so a slot must not observe another
/// slot's writes, and a stalled slot must leave no trace of the slots that
/// already ran beside it.
struct SlotEffects {
  struct MemWrite {
    uint64_t Addr;
    unsigned NumBytes;
    APInt Value;
  };

  SmallVector<std::pair<MCRegister, APInt>, 4> RegWrites;
  SmallVector<MemWrite, 2> MemWrites;
  SmallVector<MCRegister, 2> RegPoisons;
  /// Target of a taken control transfer, entering the delay-slot pipeline.
  std::optional<uint64_t> Branch;
  /// jl: the link register gets the return address once the delay slots
  /// retire, not here -- see AIEExecutor::advancePC. The value is not yet
  /// known at issue time (it depends on the byte size of bundles that have
  /// not been fetched), so it is not a RegWrites entry; this flag tells the
  /// executor to compute and commit it when the branch resolves instead of
  /// poisoning the link register the way an ordinary unwritten Def would.
  bool Link = false;
};

/// The behaviour of one subtarget's instructions.
class AIESemantics {
public:
  virtual ~AIESemantics();

  /// Compute \p Eff from \p MI against \p State, which is the state at the top
  /// of the bundle. On StepResult::Fault, \p FaultMsg says which instruction
  /// and why.
  virtual StepResult execute(const MCInst &MI, const AIECoreState &State,
                             AIEHostInterface &Host, SlotEffects &Eff,
                             std::string &FaultMsg) = 0;

  /// The instruction that ends a core's program, if the subtarget has one.
  virtual bool isEndOfProgram(const MCInst &MI) const = 0;

  /// The zero-overhead loop registers: count, start, end.
  virtual std::array<MCRegister, 3> getLoopRegisters() const = 0;

  /// jl/ret's link register, written with the return address by the
  /// executor (see SlotEffects::Link) rather than by semantics directly.
  virtual MCRegister getLinkRegister() const = 0;
};

/// \returns nullptr when \p STI names a subtarget with no semantics yet.
std::unique_ptr<AIESemantics> createSemantics(const MCSubtargetInfo &STI,
                                              const MCInstrInfo &MII,
                                              const MCRegisterInfo &MRI);

} // namespace AIESim
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_SIM_AIESEMANTICS_H
