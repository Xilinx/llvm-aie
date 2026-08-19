//=- llvm/CodeGen/ResourceScoreboard.h - Schedule Support -*- C++ -*-=//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the ResourceScoreboard class, which
// encapsulates hazard-avoidance heuristics for scheduling, based on the
// scheduling itineraries specified for the target.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CODEGEN_RESOURCESCOREBOARD_H
#define LLVM_CODEGEN_RESOURCESCOREBOARD_H

#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>
#include <climits>
#include <vector>

namespace llvm {

template <typename RC> class ResourceScoreboard {
  /// Scoreboard to track function unit usage. Scoreboard[0] is a
  /// representation of the resources in use in the current cycle
  /// Scoreboard[1] for the next cycle, etc. The Scoreboard is used
  /// as a circular buffer with the current cycle indicated by Head.
  //
  /// Scoreboard always counts cycles in forward execution order. If used by a
  /// bottom-up scheduler, then the scoreboard cycles are the inverse of the
  /// scheduler's cycles.
  std::vector<RC> Cycles;

  /// The maximum number of cycles monitored by the Scoreboard.
  /// For the sliding-window mode, it is rounded up to a power of two.
  int Size = 0;

  /// When non-zero, the scoreboard operates in modulo mode: operator[]
  /// maps any cycle to ((Cycle + Period) % Period). There is no sliding
  /// window; calling advance() or recede() is a contract violation.
  /// Valid cycle indices are in [-Period, INT_MAX - Period].
  int Period = 0;

  /// The scoreboard extends from Head - Lowest to Head + Highest
  /// When querying for conflicts, we have more liberty, with out-of-range
  /// cycles interpreted as being empty.
  int LowestCycle = 0;
  int HighestCycle = 0;

  /// Index into the Scoreboard that represents the current cycle.
  int Head = 0;

public:
  int getSize() const { return Size; }

  /// Index operators.
  /// Sliding-window mode: Size is a power of two and masking wraps.
  /// Modulo mode: Cycle must be in [-Period, INT_MAX - Period] so that
  /// adding Period yields a non-negative value before the final reduction.
  const RC &operator[](int Cycle) const {
    if (Period > 0)
      return Cycles[(Cycle + Period) % Period];
    return Cycles[(Head + Cycle) & (Size - 1)];
  }
  RC &operator[](int Cycle) {
    if (Period > 0)
      return Cycles[(Cycle + Period) % Period];
    return Cycles[(Head + Cycle) & (Size - 1)];
  }

  void clear() {
    assert(Size);
    Cycles.clear();
    Cycles.resize(Size);
    Head = 0;
  }

  // Sets up the scoreboard to be able to maintain a range of cycles
  // between LowestCycle and HighestCycle relative to Head
  // When Head changes position, empty cycles appear at the cycles that come
  // into range. The cycles that go out of range are lost.
  // Direct indexing out of [Lwb, Upb] is invalid. In operations an index
  // outside of this range represents an empty cycle.
  void config(int Lwb, int Upb) {
    LowestCycle = Lwb;
    HighestCycle = Upb;
    const int Req = Upb - Lwb + 1;
    // Implementation relies on masking for wrap-around, so round up
    // to a power of two.
    int Pow2 = 1;
    while (Pow2 < Req) {
      Pow2 += Pow2;
    }
    Size = Pow2;
    clear();
  }

  bool isInRange(int Index) const {
    // In modulo mode the valid range is [-Period, INT_MAX - Period], which
    // ensures that (Index + Period) neither underflows nor overflows before
    // the final modulo reduction.
    if (Period > 0)
      return Index >= -Period && Index <= INT_MAX - Period;
    return Index >= LowestCycle && Index <= HighestCycle;
  }

  // Configure a modulo scoreboard of size P.
  // operator[] maps any cycle to (cycle % P), so pipeline residuals that
  // cross the stage boundary wrap around automatically.
  // Calling advance() or recede() on a modulo scoreboard is a contract
  // violation and will trigger llvm_unreachable.
  void configModulo(int P) {
    assert(P > 0 && "Modulo period must be positive.");
    Period = P;
    Size = P;
    LowestCycle = 0;
    HighestCycle = P - 1;
    Head = 0;
    Cycles.assign(P, RC{});
  }

  void advance() {
    if (Period > 0)
      llvm_unreachable("advance() is not valid in modulo mode.");
    (*this)[LowestCycle].clearResources();
    Head = (Head + 1) & (Size - 1);
  }

  void recede() {
    if (Period > 0)
      llvm_unreachable("recede() is not valid in modulo mode.");
    (*this)[HighestCycle].clearResources();
    Head = (Head - 1) & (Size - 1);
  }

  /// Check whether this and Other have a conflict.
  /// \param DeltaCycles displacement in cycles of Other relative to this.
  bool conflict(const ResourceScoreboard &Other, int DeltaCycles) const {
    // All cycles outside of either scoreboard are considered empty,
    // so cannot cause conflicts
    // We check every cycle in the overlapping region.
    int Cycle = LowestCycle + DeltaCycles;
    int OtherCycle = Other.LowestCycle;
    while (Cycle < LowestCycle) {
      Cycle++;
      OtherCycle++;
    }
    while (Cycle <= HighestCycle && OtherCycle <= Other.HighestCycle) {
      if ((*this)[Cycle].conflict(Other[OtherCycle])) {
        return true;
      }
      Cycle++;
      OtherCycle++;
    }
    return false;
  }

  int firstOccupied() const {
    int First = LowestCycle;
    while (First < 0 && (*this)[First].isEmpty())
      First++;
    return First;
  }

  int lastOccupied() const {
    int Last = HighestCycle;
    while ((Last > 0) && (*this)[Last].isEmpty())
      Last--;

    return Last;
  }

  // Print the scoreboard.
  void dump() const {
    int First = firstOccupied();
    int Last = lastOccupied();
    RC Previous;
    int Repeats = 0;
    for (int C = First; C <= Last; C++) {
      const RC &Cycle = (*this)[C];
      if (C == 0) {
        dbgs() << ">";
      } else if (C > First && Cycle == Previous) {
        Repeats++;
        continue;
      }
      if (Repeats) {
        dbgs() << "+ " << Repeats << " more\n";
        Repeats = 0;
      }
      dbgs() << "\t";
      Cycle.dump();
      dbgs() << "\n";
      Previous = Cycle;
    }
    if (Repeats) {
      dbgs() << "+ " << Repeats << " more\n";
    }
  }

  // Print the full scoreboard .
  void dumpFull() const {
    int First = firstOccupied();
    int Last = lastOccupied();
    for (int C = First; C <= Last; C++) {
      const RC &Cycle = (*this)[C];
      if (C == 0) {
        dbgs() << ">";
      }
      dbgs() << "\t";
      Cycle.dump();
      dbgs() << "\n";
    }
  }

  // This is a relic from the original users of the scoreboard, mainly
  // hazardrecognizer variants. In that original use, D was determined by
  // the pipeline depth of the itineraries. We have doubled the size and
  // put the origin in the middle, so that we could insert in cycles < 0.
  void reset(int D) { config(-D, D - 1); }
};

} // end namespace llvm

#endif // LLVM_CODEGEN_RESOURCESCOREBOARD_H
