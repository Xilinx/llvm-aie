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
#include "llvm/ADT/StringRef.h"
#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace llvm {
class InstrItineraryData;
class Triple;
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
  /// A store, whose SOURCE is sampled later than the store issues.
  ///
  /// The address is computed at issue (a store reads its pointer and index at
  /// cycle 1) but the data is read at the source operand's own cycle, which is
  /// 7 on the narrow forms. That is not a detail: measured on silicon, an
  /// aievec kernel's store commits the result of a multiply that issues FOUR
  /// bundles after the store does. So the register is named here and read when
  /// the pipeline reaches SampleAt, rather than resolved now.
  struct MemWrite {
    uint64_t Addr;
    unsigned NumBytes;
    MCRegister SrcReg;
    uint64_t SampleAt;
    /// Forwarding class of the store's SOURCE operand, matched against the
    /// producer's the same way a register read is.
    unsigned Fwd;
    /// What to write, given the source register's value at SampleAt. Empty for
    /// an ordinary store, which writes those bits unchanged.
    ///
    /// The fused stores need this because they do arithmetic on the way out:
    /// vst.srs shifts, rounds, saturates and narrows an accumulator. Doing it
    /// at issue instead would defeat the deferral above -- the source may be
    /// written by an instruction issued AFTER the store, which is the measured
    /// behaviour this whole struct exists to model. A callable rather than a
    /// description of the arithmetic, so that subtarget semantics stay out of
    /// the executor.
    ///
    /// Returns nullopt when the value sampled is one the model will not
    /// convert, which the executor turns into a fault. The integer narrowing
    /// is total and never declines; the bf16 one does, for a NaN whose payload
    /// lives only in the bits it would discard -- that truncates to an
    /// infinity and the hardware's behaviour is not established. Checking at
    /// issue instead is not open to us: the value is not known until SampleAt.
    std::function<std::optional<APInt>(const APInt &)> Narrow;
  };

  /// A register write and the cycle it becomes readable. The cycle is the
  /// issuing instruction's own def cycle, so a consumer scheduled inside the
  /// window sees the OLD value -- which is what an exposed pipeline does and
  /// what silicon was measured doing.
  ///
  /// \p Fwd is the itinerary's pipeline-forwarding class for the operand that
  /// produced this write, 0 for none. A consumer whose own use operand carries
  /// the SAME class reads the result off the bypass instead of the register
  /// file, one cycle before it lands. Carried per write because it is a
  /// property of the (def operand, use operand) pair, not of the register.
  struct RegWrite {
    MCRegister Reg;
    APInt Value;
    uint64_t VisibleAt;
    unsigned Fwd = 0;
  };
  SmallVector<RegWrite, 4> RegWrites;
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

  /// Where each sub-register index sits inside its parent, for this subtarget.
  /// Carried on the semantics object rather than looked up by whoever builds a
  /// register file: it is per-subtarget data, the semantics object is the
  /// per-subtarget object, and a caller that forgets it gets no diagnostic --
  /// only vector registers that silently refuse to hold a value.
  virtual ArrayRef<AIESubRegRange> getSubRegRanges() const = 0;

  /// The subtarget's itineraries, or null when this build has none. Semantics
  /// read the operand cycles themselves; the executor needs the stage lists,
  /// which are where the structural hazards live.
  virtual const InstrItineraryData *getItineraries() const { return nullptr; }
};

/// \returns nullptr when \p STI names a subtarget with no semantics yet.
std::unique_ptr<AIESemantics> createSemantics(const MCSubtargetInfo &STI,
                                              const MCInstrInfo &MII,
                                              const MCRegisterInfo &MRI);

/// Where each sub-register index sits inside its parent, for \p TT, or empty
/// when this build has no table for it -- in which case composed registers stay
/// unreadable rather than being composed from a guessed layout.
ArrayRef<AIESubRegRange> subRegRangesForTriple(const Triple &TT);

/// The processor name to build MCSubtargetInfo with, for \p TT.
///
/// Not optional and not cosmetic: with an empty CPU the subtarget carries no
/// scheduling model, so every operand latency reads as "none" and a machine
/// with no interlocks cannot be executed correctly. AIE names each processor
/// exactly after its architecture, so the triple already holds the answer.
StringRef cpuForTriple(const Triple &TT);

} // namespace AIESim
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_SIM_AIESEMANTICS_H
