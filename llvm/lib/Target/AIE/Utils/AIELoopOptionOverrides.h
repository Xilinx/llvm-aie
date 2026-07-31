//===-- AIELoopOptionOverrides.h ------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Per-loop option overrides via !llvm.loop metadata.
//
// Any AIE pass can use LoopOptionOverrides to make its cl::opt options
// overridable per-loop. The override map is built once per loop from
// metadata entries of the form:
//
//   !{!"llvm.loop.hint.<cl-opt-argstr>", i64 <value>}
//   !{!"llvm.loop.hint.<cl-opt-argstr>", !"<string-value>"}
//
// The cl::opt globals are never mutated.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_AIE_UTILS_AIELOOPOPTIONOVERRIDES_H
#define LLVM_LIB_TARGET_AIE_UTILS_AIELOOPOPTIONOVERRIDES_H

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include <cstdint>
#include <string>
#include <type_traits>

namespace llvm {
class MDNode;
class MachineBasicBlock;
} // namespace llvm

namespace llvm::AIE {

/// Reads all llvm.loop.hint.* metadata entries from a loop and provides
/// typed queries keyed by cl::opt ArgStr. The cl::opt globals are never
/// mutated; this is a read-only overlay.
///
/// Usage:
///   LoopOptionOverrides Overrides(MBB);
///   bool Aggressive = Overrides.get(AggressiveReAlloc);
///   RewriteMode Mode = Overrides.get(RegRewriteMode);
class LoopOptionOverrides {
  /// Integer-valued overrides (covers bool, int, unsigned, and enum types).
  StringMap<int64_t> IntVals;

  /// String-valued overrides.
  StringMap<std::string> StrVals;

public:
  /// Prefix joining a cl::opt ArgStr to its llvm.loop.hint metadata key; the
  /// single source for both reading overrides and writing hint keys.
  static constexpr StringLiteral Prefix{"llvm.loop.hint."};

  LoopOptionOverrides() = default;

  /// Build overrides from the given loop metadata node. If LoopID is
  /// nullptr, the result is empty (equivalent to default construction).
  explicit LoopOptionOverrides(const MDNode *LoopID);

  /// Convenience: build from a MachineBasicBlock by extracting its LoopID.
  explicit LoopOptionOverrides(const MachineBasicBlock &MBB);

  /// Return the effective value for a cl::opt.
  ///
  /// Precedence (highest to lowest):
  ///   1. Explicit command-line setting (user passed --flag=value)
  ///   2. Per-loop metadata override (llvm.loop.hint.<argstr>)
  ///   3. cl::opt default value
  ///
  /// Works for bool, integral, enum, and std::string types.
  template <typename T> T get(const cl::opt<T> &Opt) const {
    // Explicit command-line setting takes precedence over loop metadata.
    const bool IsUserSpecified = Opt.getNumOccurrences() > 0;
    if (IsUserSpecified)
      return Opt;

    if constexpr (std::is_same_v<T, std::string>) {
      auto It = StrVals.find(Opt.ArgStr);
      if (It != StrVals.end())
        return It->second;
    } else {
      auto It = IntVals.find(Opt.ArgStr);
      if (It != IntVals.end())
        return static_cast<T>(It->second);
    }
    return Opt;
  }

  /// Return whether an option has an explicit command-line or loop-metadata
  /// override, regardless of the override's value.
  template <typename T> bool hasOverride(const cl::opt<T> &Opt) const {
    if (Opt.getNumOccurrences() > 0)
      return true;

    if constexpr (std::is_same_v<T, std::string>)
      return StrVals.contains(Opt.ArgStr);
    else
      return IntVals.contains(Opt.ArgStr);
  }

  bool hasOverrides() const { return !IntVals.empty() || !StrVals.empty(); }
};

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_UTILS_AIELOOPOPTIONOVERRIDES_H
