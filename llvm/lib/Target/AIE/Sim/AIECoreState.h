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
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
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
/// Where one sub-register index sits inside its parent. Target data, passed in
/// rather than known here, so this file stays generic.
struct AIESubRegRange {
  uint32_t offsetBits;
  uint32_t sizeBits;
};
/// An index that does not name one contiguous run of bits.
inline constexpr uint32_t kNoContiguousRange = 65535;

class AIERegisterFile {
public:
  /// \p SubRegRanges is indexed by sub-register index number and may be empty,
  /// in which case a composed register stays unreadable rather than being
  /// composed from a layout nobody supplied. Not defaulted: empty is a real
  /// choice with a silent consequence -- every vector register unreadable and
  /// every vector write dropped -- so a caller states it rather than inherits
  /// it. Both callers had inherited it, which is how composition shipped with
  /// no way to reach it.
  AIERegisterFile(const MCRegisterInfo &MRI,
                  ArrayRef<AIESubRegRange> SubRegRanges);

  /// Width in bits, or 0 when the register has no storage in this model.
  unsigned getWidth(MCRegister Reg) const;

  /// Width in bits from the register classes, which is defined for a composed
  /// register too -- that is exactly where it cannot be read off storage.
  unsigned getClassWidth(MCRegister Reg) const;

  /// \returns false when \p Reg has no storage, or holds a value this model
  /// declined to compute.
  ///
  /// Reads the value visible at \p Cycle. AIE has no interlocks, so a register
  /// legitimately holds different values to instructions issued a few bundles
  /// apart: a producer's result appears at its own def cycle, and a consumer
  /// samples at its own use cycle. Measured on silicon -- an aievec kernel
  /// stores a multiply's result through a store that ISSUED four bundles
  /// earlier, because the store samples late.
  ///
  /// A write landing AT the read's cycle is not yet visible. The itineraries
  /// do not answer this -- the backend excludes the same-cycle case from its
  /// own worst-case arithmetic -- so it was measured on a device-verified
  /// object instead: strict gives 1024 of 1024 lanes, `<=` gives 4 of 8.
  bool read(MCRegister Reg, APInt &Out, uint64_t Cycle) const;

  /// \returns false when \p Reg is composed and its parts do not tile it, in
  /// which case nothing was written. A composed write is all-or-nothing: half
  /// a vector is a number that looks like a result.
  bool write(MCRegister Reg, const APInt &Value, uint64_t VisibleAt);

  /// Drop history that no read can reach any more. \p Horizon is the earliest
  /// cycle a future read could name; entries fully superseded before it are
  /// removed, so a long run does not accumulate one entry per write.
  void forgetBefore(uint64_t Horizon);

  /// Record that hardware would have changed \p Reg in a way the model does not
  /// compute, so that a later read faults instead of returning a stale value.
  /// ADD defines srCarry, and a model that skips the carry-out but still
  /// answers "mov r0, srCarry" is wrong rather than incomplete.
  void poison(MCRegister Reg);
  bool isPoisoned(MCRegister Reg) const;

  void print(raw_ostream &OS) const;

private:
  /// One value and the cycle it becomes readable. Kept newest-last.
  struct Timed {
    uint64_t visibleAt;
    APInt value;
  };

  /// One leaf of a composed register and where it sits inside it.
  struct Placement {
    MCRegister Leaf;
    uint32_t offsetBits;
    uint32_t sizeBits;
  };

  /// Locate the leaves of \p Reg by descending one level at a time.
  ///
  /// A sub-register index's offset is only meaningful at the level it was
  /// declared for, so it cannot be used to place a leaf inside an arbitrary
  /// ancestor. AIE2P declares `sub_hi_exp : SubRegIndex<32, 32>` for the
  /// 64-bit exponent registers and reuses it further up: ex0 is
  /// [x0 @0, e0 @512] and e0 is [el0 @0, eh0 @32], so eh0 sits at 544 in ex0
  /// while `getSubRegIndex(ex0, eh0)` still answers sub_hi_exp, offset 32.
  /// Descending accumulates 512 + 32 and gets it right.
  ///
  /// \returns false unless the leaves tile [0, width) exactly. Overlaps are
  /// the reason this is checked rather than counted: the 18 bfp16 registers
  /// have leaves whose SIZES sum to the parent's width while their ranges
  /// overlap, so a total alone accepts a layout that is wrong.
  bool computePlacements(MCRegister Reg, unsigned Width,
                         SmallVectorImpl<Placement> &Out) const;

  /// Compose \p Reg out of the leaves that tile it.
  bool readComposed(MCRegister Reg, APInt &Out, uint64_t Cycle) const;
  /// Split \p Value across them. \returns false if they do not tile \p Reg.
  bool writeComposed(MCRegister Reg, const APInt &Value, uint64_t VisibleAt);

  const MCRegisterInfo &MRI;
  ArrayRef<AIESubRegRange> Ranges;
  std::vector<unsigned> Widths;
  /// Per composed register, the leaves tiling it, or empty when they do not --
  /// in which case it stays unreadable rather than being assembled wrongly.
  /// Computed once: it depends only on the target description.
  std::vector<SmallVector<Placement, 4>> Composition;
  /// Width from the register classes, kept even where Widths is zeroed because
  /// the register is composed -- that is exactly where composition needs it.
  std::vector<unsigned> ClassWidths;
  /// Per register, the values in flight and the last one that landed. Short by
  /// construction: only writes still inside their latency window can add to it.
  std::vector<SmallVector<Timed, 2>> Storage;
  std::vector<bool> Poisoned;
};

/// The whole architectural state of one core.
class AIECoreState {
public:
  AIECoreState(const MCRegisterInfo &MRI, uint64_t EntryPoint,
               ArrayRef<AIESubRegRange> SubRegRanges)
      : Regs(MRI, SubRegRanges), PC(EntryPoint) {}

  AIERegisterFile Regs;
  uint64_t PC;
  uint64_t RetiredBundles = 0;

  /// The core's clock, which every operand cycle and store sample is
  /// denominated in. It runs ahead of RetiredBundles by StallCycles: a bundle
  /// issues one per cycle unless a structural hazard holds it, and AIE has no
  /// interlocks, so a held bundle costs time without retiring anything.
  uint64_t Cycle = 0;
  /// Cycles lost to structural hazards, which is per-op occupancy the bundle
  /// count cannot show.
  uint64_t StallCycles = 0;

  /// A taken control transfer, held until its delay slots have retired.
  struct PendingBranch {
    uint64_t Target;
    unsigned BundlesLeft;
    /// jl: write the link register with the fallthrough address (PC as of
    /// the last delay-slot bundle, i.e. before Target overwrites it) when
    /// this resolves.
    bool Link = false;
  };
  std::optional<PendingBranch> Branch;
};

} // namespace AIESim
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_SIM_AIECORESTATE_H
