//===- AIELiveRangeUtils.h - Live Range Utilities -------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains utilities for analyzing and scheduling live ranges.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIELIVERANGEUTILS_H
#define LLVM_LIB_TARGET_AIE_AIELIVERANGEUTILS_H

namespace llvm {

class AIEHazardRecognizer;
class AIEScheduleInterpreter;
class RegLiveRange;
class ScheduleDAG;

namespace AIE {

/// Result of live range scheduling analysis.
class LiveRangeScheduleResult {
  unsigned MinimalLength;

public:
  LiveRangeScheduleResult(unsigned MinimalLength)
      : MinimalLength(MinimalLength) {}

  /// Get the minimal live length for the range.
  unsigned getMinimalLiveLength() const { return MinimalLength; }
};

/// Compute the minimal live length for a single live range.
///
/// Schedules the instructions in the live range (defs and uses) greedily
/// using the AIEScheduleInterpreter for latency information and
/// AIEHazardRecognizer for structural resource checking. Returns the
/// minimal event-space coverage from first def to last use.
///
/// \param LR The live range to schedule
/// \param DAG The schedule DAG providing dependency information
/// \param HR The hazard recognizer for resource checking
/// \param Interp The schedule interpreter providing latency/event mapping
/// \return Result containing the minimal live length
LiveRangeScheduleResult
computeMinimalSchedule(const RegLiveRange &LR, const ScheduleDAG &DAG,
                       const AIEHazardRecognizer &HR,
                       const AIEScheduleInterpreter &Interp);

} // end namespace AIE
} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIELIVERANGEUTILS_H
