//===- AIEExecutor.h - Bundle-at-a-time execution of AIE code ---*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_SIM_AIEEXECUTOR_H
#define LLVM_LIB_TARGET_AIE_SIM_AIEEXECUTOR_H

#include "AIECoreState.h"
#include "AIESemantics.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCInst.h"
#include <string>

namespace llvm {
class MCDisassembler;
class MCInstrInfo;
class MCRegisterInfo;

namespace AIESim {

/// Executes machine code one bundle at a time.
///
/// Bundles arrive from MCDisassembler as one composite MCInst carrying a nested
/// MCInst per issue slot, so slot boundaries are never re-derived here.
class AIEExecutor {
public:
  AIEExecutor(const MCDisassembler &DisAsm, const MCInstrInfo &MII,
              const MCRegisterInfo &MRI, AIESemantics &Sem,
              AIEHostInterface &Host, uint64_t EntryPoint);

  StepResult step();

  AIECoreState &getState() { return State; }
  const AIECoreState &getState() const { return State; }
  StringRef getFaultMessage() const { return FaultMsg; }

  /// Opcodes this run reached, and those among them with no semantics. Coverage
  /// is a number rather than a feeling, which matters while the model is
  /// incomplete.
  const DenseSet<unsigned> &getExecutedOpcodes() const { return Executed; }
  const DenseSet<unsigned> &getUnmodelledOpcodes() const { return Unmodelled; }

private:
  struct Bundle {
    MCInst Inst;
    uint64_t Size;
  };

  /// \returns nullptr and sets FaultMsg when \p Addr does not decode.
  const Bundle *decode(uint64_t Addr);
  StepResult executeSlot(const MCInst &MI, SlotEffects &Eff);
  void commit(const SlotEffects &Eff);
  /// Advance PC past \p BundleSize, honouring a retiring branch and the
  /// hardware loop.
  void advancePC(uint64_t BundleAddr, uint64_t BundleSize);

  const MCDisassembler &DisAsm;
  const MCInstrInfo &MII;
  const MCRegisterInfo &MRI;
  AIESemantics &Sem;
  AIEHostInterface &Host;
  AIECoreState State;

  MCRegister LCReg, LSReg, LEReg, LRReg;
  /// Each address decodes once. The slot sub-instructions live in the
  /// disassembler's MCContext, which never reclaims them, so re-decoding a loop
  /// body costs memory proportional to bundles executed rather than to program
  /// size. Assumes program memory does not change while the core runs.
  DenseMap<uint64_t, Bundle> Decoded;
  std::string FaultMsg;
  DenseSet<unsigned> Executed;
  DenseSet<unsigned> Unmodelled;
};

} // namespace AIESim
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_SIM_AIEEXECUTOR_H
