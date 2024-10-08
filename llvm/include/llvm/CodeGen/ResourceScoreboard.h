//=- llvm/CodeGen/ResourceScoreboard.h - Schedule Support -*- C++ -*-=//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
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

  void clear() {
    assert(Size);
    Cycles.clear();
    Cycles.resize(Size);
    Head = 0;
  }

  void reset(int D) {
    // Implementation relies on masking to wrap-around, so round up
    // to a power of two.
    int Pow2 = 1;
    while (Pow2 < D) {
      Pow2 += Pow2;
    }
    Depth = Pow2;
    Size = 2 * Depth;
    clear();
  }
  bool isValidDelta(int DeltaCycles) const {
    return DeltaCycles >= -Depth && DeltaCycles <= 0;
  }

  void advance() {
    (*this)[-Depth].clearResources();
    Head = (Head + 1) & (Size - 1);
  }
  void recede() {
    (*this)[Depth - 1].clearResources();
    Head = (Head - 1) & (Size - 1);
  }

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

  int firstOccupied() const {
    int First = -Depth;
    while (First < 0 && (*this)[First].isEmpty())
      First++;
    return First;
  }

  int lastOccupied() const {
    int Last = Depth - 1;
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
};

} // end namespace llvm

#endif // LLVM_CODEGEN_RESOURCESCOREBOARD_H
