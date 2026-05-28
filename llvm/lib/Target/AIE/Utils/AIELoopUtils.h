//===-- AIELoopUtils.h ----------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains helper functions to handle loops and its metadata.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_AIE_UTILS_AIELOOPUTILS_H
#define LLVM_LIB_TARGET_AIE_UTILS_AIELOOPUTILS_H

#include "llvm/Analysis/LoopInfo.h"
#include "llvm/CodeGen/MachineLoopInfo.h"

namespace llvm {
struct AIEBaseInstrInfo;
class MachineBasicBlock;
} // namespace llvm

namespace llvm::AIELoopUtils {

/// Get the LoopID from a single block loop or nullptr
const MDNode *getLoopID(const MachineBasicBlock &LoopBlock);

/// LoopBlock should hold the backedge. Since we generally call it for
/// single block loops, that's automatically true.
std::optional<int64_t> getMinTripCount(const MachineBasicBlock &LoopBlock);

bool hasIIPragma(const MachineBasicBlock &LoopBlock);

/// Loop-hint key the outer-loop pipeliner stamps on a loop it pipelined, and
/// that isOuterLoopPipelined() tests for. Single source for both sides.
constexpr StringLiteral OuterLoopPipelinedKey{
    "llvm.loop.hint.aie_outerloop_pipeliner_success"};

bool isOuterLoopPipelined(const MachineBasicBlock &LoopLatch);

/// Returns true if MBB is the steady-state epilog block produced by the
/// outer-loop pipeliner.
bool isOuterLoopEpilog(const MachineBasicBlock &MBB);

/// Returns true if this is a loop latch that has a pipeliner disable pragma,
/// none otherwise.
std::optional<bool> getPipelinerDisabled(const MachineBasicBlock &LoopBlock);

/// Check that the single block loop represented by LoopBlock has a fallthrough
/// preheader. Return the preheader if true, nullptr otherwise
MachineBasicBlock *
getDedicatedFallThroughPreheader(const MachineBasicBlock &LoopBlock);

// get all the Machine Basic Blocks (MBBs) that contain a Single Block Loop,
// which is defined by having 2 Successors, where one of the succesors, is the
// MBB itself.
SmallVector<const MachineBasicBlock *, 4>
getSingleBlockLoopMBBs(const MachineFunction &MF);

/// Non-const overload: returns mutable single-MBB loop blocks.
SmallVector<MachineBasicBlock *, 4> getSingleBlockLoopMBBs(MachineFunction &MF);

/// Check if this block is a single block loop.
bool isSingleMBBLoop(const MachineBasicBlock *MBB);

/// Considering that MBB has a single predecessor that is a loop
/// and also layout predecessor, return it.
/// Note: this function should be used only with single MBB loops.
MachineBasicBlock *getLoopPredecessor(const MachineBasicBlock &EpilogueMBB);

/// Extract the string key (operand 0) from a metadata entry node.
/// Returns nullopt if operand 0 is missing or not an MDString.
std::optional<StringRef> getMetadataKey(const MDNode &MD);

/// Extract an integer value (operand 1) from a metadata entry node.
/// Returns nullopt if operand 1 is missing or not a ConstantInt.
std::optional<int64_t> getMetadataIntValue(const MDNode &MD);

/// Extract a string value (operand 1) from a metadata entry node.
/// Returns nullopt if operand 1 is missing or not an MDString.
std::optional<StringRef> getMetadataStringValue(const MDNode &MD);

/// Return all valid metadata entry nodes from a loop metadata node.
/// Handles null LoopID (returns empty), validates the LoopID structure,
/// and filters out non-MDNode operands.
SmallVector<const MDNode *, 4> getLoopMetadataEntries(const MDNode *LoopID);

std::optional<const MDNode *> getLoopMetadata(const MDNode *LoopID,
                                              const StringRef Name);

/// Extract a boolean value (i64 0/1) from the named loop metadata entry.
std::optional<bool> getLoopHintBool(const MDNode *LoopID, StringRef Key);
std::optional<bool> getLoopHintBool(const MachineBasicBlock &MBB,
                                    StringRef Key);

/// Extract an integer value (i64) from the named loop metadata entry.
std::optional<int64_t> getLoopHintInt(const MDNode *LoopID, StringRef Key);
std::optional<int64_t> getLoopHintInt(const MachineBasicBlock &MBB,
                                      StringRef Key);

/// Extract a string value (MDString) from the named loop metadata entry.
std::optional<StringRef> getLoopHintString(const MDNode *LoopID, StringRef Key);
std::optional<StringRef> getLoopHintString(const MachineBasicBlock &MBB,
                                           StringRef Key);

// Returns true if the loop has an unroll(full) pragma.
bool hasUnrollFullPragma(const MDNode *LoopID);

// Returns true if the loop has an unroll(enable) pragma. This metadata is used
// for both "#pragma unroll" and "#pragma clang loop unroll(enable)" directives.
bool hasUnrollEnablePragma(const MDNode *LoopID);

// Returns true if the loop has an unroll x pragma.
bool hasUnrollCountPragma(const MDNode *LoopID);

bool hasUnrollPragma(const Loop *L);

/// For a single-MBB loop, return its unique prologue (non-loop predecessor)
/// and epilogue (non-loop successor). Either may be nullptr if the loop has
/// no non-loop predecessor or successor (e.g. entry-block loops).
/// Asserts uniqueness if a prologue or epilogue does exist.
std::pair<MachineBasicBlock *, MachineBasicBlock *>
findPrologueEpilogue(const MachineBasicBlock &LoopBB);

/// For a ZOL loop block, check whether it was software-pipelined.
/// Scans non-loop predecessors for the lowered LoopStart whose trip-count
/// adjustment encodes the stage count: Adj == -(NS - 1) for an NS-stage
/// pipelined loop. Adj == 0 means the loop was not software-pipelined.
/// \return The number of stages (NS) if pipelined, std::nullopt otherwise.
std::optional<unsigned> getSWPStageCount(const MachineBasicBlock &LoopBB,
                                         const AIEBaseInstrInfo &TII);

} // namespace llvm::AIELoopUtils

#endif
