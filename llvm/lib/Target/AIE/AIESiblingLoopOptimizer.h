//===- AIESiblingLoopOptimizer.h - Sibling loop optimization ----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Optimization for outer-loop pipelined sibling loops. Merges matching
// leading prologue bundles into the epilogue and optionally overlaps
// transferred bundles with the preheader's trailing cycles.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIESIBLINGLOOPOPTIMIZER_H
#define LLVM_LIB_TARGET_AIE_AIESIBLINGLOOPOPTIMIZER_H

namespace llvm {

class AIEAlternateDescriptors;
struct AIEBaseInstrInfo;
class AIEHazardRecognizer;
struct MachineSchedContext;

} // namespace llvm

namespace llvm::AIE {

class BlockState;
class InterBlockScheduling;

/// Optimizes sibling loops created by the outer-loop pipeliner (OLP).
///
/// For each outer-loop pipelined structure, this optimizer:
///  1. Identifies matching leading bundles between the steady-state and
///     last-iteration prologues.
///  2. Validates scheduling slack (latency) and resource mergeability.
///  3. Merges the matching bundles into the epilogue.
///  4. Transfers them to the outer preheader, optionally overlapping
///     with existing preheader content when resources and latency permit.
///
/// The optimizer accesses inter-block state through a reference to
/// InterBlockScheduling without owning any of it.
class SiblingLoopOptimizer {
  InterBlockScheduling &IBS;
  const AIEHazardRecognizer &HR;
  const AIEBaseInstrInfo &TII;
  const AIEAlternateDescriptors &SelectedAltDescs;
  const MachineSchedContext &Context;

  /// Compute how many leading bundles can be merged from prologues into
  /// epilogue. Returns 0 if merging is not possible.
  unsigned getNumberOfMergeableBundles(const BlockState &EpilogueBS,
                                       const BlockState &SteadyTopBS,
                                       const BlockState &LastIterTopBS);

  /// Compute how many transferred prologue bundles can be merged into the
  /// preheader's trailing bundles (best-effort, resource + latency safe).
  unsigned getPreheaderMergeCount(const BlockState &EntryBS,
                                  const BlockState &SteadyTopBS,
                                  unsigned NumTransferred);

  /// Merge N leading bundles from prologues into epilogue and transfer to
  /// entry.
  void mergeBundles(BlockState &EntryBS, BlockState &SteadyTopBS,
                    BlockState &EpilogueBS, BlockState &LastIterTopBS,
                    unsigned NumBundles);

  /// Try to merge matching prologue bundles into the epilogue and transfer
  /// them to the entry block.
  void tryMergePrologues(BlockState &EntryBS, BlockState &SteadyTopBS,
                         BlockState &EpilogueBS, BlockState &LastIterTopBS);

public:
  SiblingLoopOptimizer(InterBlockScheduling &IBS, const AIEHazardRecognizer &HR,
                       const AIEBaseInstrInfo &TII,
                       const AIEAlternateDescriptors &SelectedAltDescs,
                       const MachineSchedContext &Context);

  /// Optimize all sibling loops found via OuterLoopContext in the function.
  void run();
};

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIESIBLINGLOOPOPTIMIZER_H
