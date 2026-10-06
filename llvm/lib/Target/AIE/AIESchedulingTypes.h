//===- AIESchedulingTypes.h - Scheduling Types and Enums -------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines common types and enumerations used by the AIE scheduling
// infrastructure, including block classification, pipeliner modes, scheduling
// stages, and scoreboard trust levels.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIESCHEDULINGTYPES_H
#define LLVM_LIB_TARGET_AIE_AIESCHEDULINGTYPES_H

namespace llvm::AIE {

/// BlockType determines scheduling priority, direction and safety margin
/// handling.
enum class BlockType { Regular, Loop, Epilogue };

/// PostPipelinerMode determines whether the postpipeliner operates on physical
/// registers or virtualizes them for better scheduling opportunities.
enum class PostPipelinerMode {
  /// No pipelining mode selected.
  None,
  /// Physical register mode - no virtualization, registers stay physical.
  Physical,
  /// Virtual register mode - registers are virtualized for scheduling freedom.
  Virtual,
  /// Reserved virtual mode - registers are virtualized but some are reserved.
  ReservedVirtual
};

/// Helper function to get the name of a PostPipelinerMode as a string.
const char *getPostPipelinerModeName(PostPipelinerMode Mode);

/// SchedulingStage represents the states in the state machine that drives
/// scheduling.
enum class SchedulingStage {

  /// We are scheduling, which includes iterating during loop-aware scheduling.
  Scheduling,

  /// This is a fatal error state, when we didn't converge in loop-aware
  /// scheduling. It may not be observable.
  SchedulingNotConverged,

  /// We have found a schedule. This is the final state for regular blocks.
  /// SWP candidates proceed from here into Pipelining with II=1.
  SchedulingDone,

  /// We are busy pipelining the loop. Each round will try a larger II.
  Pipelining,

  /// We found a SWP schedule. This is a final state.
  PipeliningDone,

  /// We tried pipelining, but didn't find a SWP schedule. This is a final
  /// state equivalent to SchedulingDone, except that it doesn't proceed to
  /// Pipelining anymore.
  PipeliningFailed
};

/// ScoreboardTrust represents how accurate our successor information is.
enum class ScoreboardTrust {
  /// The bundles represent the true start of the blocks.
  Absolute,
  /// The bundles are accurate, but may shift at most one cycle
  /// due to alignment of a successor block.
  AccountForAlign,
  /// We don't have bundles for all successors.
  Conservative
};

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIESCHEDULINGTYPES_H
