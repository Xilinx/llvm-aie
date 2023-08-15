//=- llvm/CodeGen/ResourceScoreboard.h - Schedule Support -*- C++ -*-=//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
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
#include "llvm/Support/raw_ostream.h"
#include <cassert>
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

  /// The maximum number of cycles monitored by the Scoreboard. This
  /// value is determined based on the target itineraries to ensure
  /// that all hazards can be tracked.
  /// For efficiency, it is rounded up to a power of two.
  int Size = 0;

  /// The scoreboard extends from Head - Depth to Head + Depth - 1
  /// Depth is half of Size. Emitting an instruction is allowed in the
  /// range [-Depth, 0], which guarantees that all of its resources are
  /// recorded.
  /// When querying for conflicts, we have more liberty, with out-of-range
  /// cycles interpreted as being empty.
  int Depth = 0;

  /// Index into the Scoreboard that represents the current cycle.
  int Head = 0;

public:
  int getDepth() const { return Depth; }
  int getSize() const { return Size; }

  /// Index operators. Size is checked to be a power of two. Masking it
  /// will properly wrap around in the allocated space.
  const RC &operator[](int Cycle) const {
    return Cycles[(Head + Cycle) & (Size - 1)];
  }
  RC &operator[](int Cycle) { return Cycles[(Head + Cycle) & (Size - 1)]; }

  void reset(int D = 1) {
    if (Cycles.empty()) {
      Depth = D;
      Size = 2 * Depth;
    }
    // Check for a power of two
    assert((Size & (Size - 1)) == 0);
    Cycles.clear();
    Cycles.resize(Size);
    Head = 0;
  }
  bool isValidDelta(int DeltaCycles) {
    return DeltaCycles >= -Depth && DeltaCycles <= 0;
  }

  void advance() { Head = (Head + 1) & (Size - 1); }
  void recede() { Head = (Head - 1) & (Size - 1); }

  /// Check whether this and Other have a conflict.
  /// \param DeltaCycles displacement in cycles of Other relative to this.
  bool conflict(const ResourceScoreboard &Other, int DeltaCycles) const {
    // All cycles outside of either scoreboard are considered empty,
    // so cannot cause conflicts
    // We check every cycle in the overlapping region.
    int Cycle = -Depth + DeltaCycles;
    int OtherCycle = -Other.Depth;
    while (Cycle < -Depth) {
      Cycle++;
      OtherCycle++;
    }
    while (Cycle < Depth && OtherCycle < Other.Depth) {
      if ((*this)[Cycle].conflict(Other[OtherCycle])) {
        return true;
      }
      Cycle++;
      OtherCycle++;
    }
    return false;
  }

#if !defined(NDEBUG) || defined(LLVM_ENABLE_DUMP)
  // Print the scoreboard.
  void dump() const {
    int First = -Depth;
    while (First < 0 && (*this)[First].empty())
      First++;
    int Last = Depth - 1;
    while ((Last > 0) && (*this)[Last].empty())
      Last--;

    for (int C = First; C <= Last; C++) {
      if (C == 0) {
        dbgs() << ">";
      }
      dbgs() << "\t";
      (*this)[C].dump();
      dbgs() << "\n";
    }
  }
#endif
};

} // end namespace llvm

#endif // LLVM_CODEGEN_RESOURCESCOREBOARD_H
