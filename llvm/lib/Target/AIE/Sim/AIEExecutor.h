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
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCInst.h"
#include <string>

namespace llvm {
class InstrItineraryData;
class MCDisassembler;
class MCInstrInfo;
class MCRegisterInfo;

namespace AIESim {

/// Structural hazards, taken from the itinerary stage lists.
///
/// LLVM's two reservation kinds carry the rule: a Required use of a unit
/// conflicts with any other use of it, while two Reserved uses do not conflict
/// with each other. AIE2P's part-word-store hazard is written in exactly that
/// asymmetry -- the sub-word stores take PART_WORD_STORE as Required for 7
/// cycles and the other memory forms take it as Reserved for 1, so one
/// sub-word store blocks them all while no two ordinary accesses block each
/// other.
///
/// InstrStage::getUnits() is a unit INDEX here, not a bitmask. AIE predefines
/// FUNCUNIT_REPRESENTATION(x) as (x) (AIEMCTargetDesc.cpp, and StaticBitSet's
/// int constructor on the CodeGen side), overriding the shift the subtarget
/// emitter would otherwise supply -- AIE2P has 73 units, which no 64-bit mask
/// holds. Reading the field as a mask yields conflicts that look plausible.
class ResourceScoreboard {
public:
  /// \p Depth must be at least the longest stage span in the itineraries, so a
  /// live reservation can never be overwritten by one a full ring later.
  explicit ResourceScoreboard(unsigned Depth) : Ring(Depth ? Depth : 1) {}

  /// How many cycles a bundle offered at \p At must wait for its stages to
  /// fit. Zero when it fits now; never more than the ring depth, past which
  /// every reservation has expired.
  unsigned delay(const InstrItineraryData &Itin, ArrayRef<unsigned> SchedClasses,
                 uint64_t At) const;

  /// Record \p SchedClasses as issuing at \p At. Call only after delay()
  /// returned zero for the same arguments.
  void reserve(const InstrItineraryData &Itin, ArrayRef<unsigned> SchedClasses,
               uint64_t At);

  /// The longest stage span across every class an instruction names, which is
  /// the depth a scoreboard over \p Itin needs.
  static unsigned depthFor(const InstrItineraryData &Itin,
                           const MCInstrInfo &MII);

private:
  /// A cycle's occupancy, as unit indices. `At` makes the ring self-clearing:
  /// a slot whose stamp does not match the cycle being asked about holds an
  /// expired reservation and reads as empty.
  struct Slot {
    uint64_t At = ~0ULL;
    SmallVector<uint16_t, 8> Required;
    SmallVector<uint16_t, 8> Reserved;
  };

  /// Null when nothing is recorded for \p C.
  const Slot *peek(uint64_t C) const;
  Slot &open(uint64_t C);

  SmallVector<Slot, 16> Ring;
};

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

  /// Perform every store whose source has been sampled by now, and drop it.
  /// \p Final drains the rest regardless, for the end of the program.
  ///
  /// \returns false when a store's source could not be read, which means the
  /// schedule sampled it inside its producer's latency window. The store did
  /// not happen, so the caller must fault rather than continue past memory
  /// that silently kept its old contents.
  bool drainStores(bool Final = false);

  /// \returns nullptr and sets FaultMsg when \p Addr does not decode.
  const Bundle *decode(uint64_t Addr);
  StepResult executeSlot(const MCInst &MI, SlotEffects &Eff);
  void commit(const SlotEffects &Eff);
  /// Advance PC past \p BundleSize, honouring a retiring branch and the
  /// hardware loop.
  void advancePC(uint64_t BundleAddr, uint64_t BundleSize);

  /// The scheduling classes of every slot in \p Bundle, in issue order.
  void schedClassesOf(const MCInst &Bundle,
                      SmallVectorImpl<unsigned> &Out) const;

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
  /// Stores waiting for their source operand's cycle. Short: a store is
  /// outstanding only for its own use-cycle worth of bundles.
  SmallVector<SlotEffects::MemWrite, 4> PendingStores;
  /// Null when the build has no itineraries, in which case every bundle
  /// issues the cycle it is offered.
  const InstrItineraryData *Itin = nullptr;
  ResourceScoreboard Hazards;
  SmallVector<unsigned, 4> SchedClasses;
};

} // namespace AIESim
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_SIM_AIEEXECUTOR_H
