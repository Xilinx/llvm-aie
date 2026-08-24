//===-- AIEMachineBundleUtils.h - MachineBundle utilities -------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains utility functions for working with MachineBundle
// in the AIE backend.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_AIE_UTILS_AIEMACHINEBUNDLEUTILS_H
#define LLVM_LIB_TARGET_AIE_UTILS_AIEMACHINEBUNDLEUTILS_H

#include "AIEBaseInstrInfo.h"
#include "AIEBundle.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm/ADT/ArrayRef.h"
#include <functional>
#include <vector>

namespace llvm {
class MachineBasicBlock;
class MachineFunction;
} // namespace llvm

namespace llvm::AIEMachineBundleUtils {

/// Merge trailing bundles from DstBundles with leading bundles from SrcBundles.
/// Creates new cloned bundles - does not modify any MBB.
///
/// The merge overlaps the last NumBundlesToMerge bundles of DstBundles with
/// the first NumBundlesToMerge bundles of SrcBundles. Instructions from both
/// are cloned into the merged bundles.
///
/// Example with NumBundlesToMerge=2:
///   DstBundles: [D0, D1, D2, D3]  (epilogue)
///   SrcBundles: [S0, S1, S2]      (prologue)
///   Result:     [D0, D1, D2+S0, D3+S1]
///
/// \param MF MachineFunction to clone instructions into
/// \param DstBundles Destination bundles (e.g., epilogue bundles)
/// \param SrcBundles Source bundles to merge from (e.g., prologue bundles)
/// \param NumBundlesToMerge How many leading SrcBundles overlap trailing
///        DstBundles. Must be <= min(DstBundles.size(), SrcBundles.size()).
/// \param FormatInterface Bundle format interface for creating new bundles
/// \param NopOpc Opcode for NOPs to skip when cloning from DstBundles
/// \returns New vector of merged bundles with cloned instructions
std::vector<AIE::MachineBundle>
mergeBundles(MachineFunction &MF, ArrayRef<AIE::MachineBundle> DstBundles,
             ArrayRef<AIE::MachineBundle> SrcBundles,
             unsigned NumBundlesToMerge,
             const AIEBaseMCFormats *FormatInterface, unsigned NopOpc);

/// Replace all non-terminator instructions in MBB with instructions from
/// Bundles. This erases existing non-terminators, inserts the bundle
/// instructions, and applies MIR bundling via
/// AIEHazardRecognizer::applyBundles.
///
/// For empty bundles, a NOP is inserted to maintain cycle count.
///
/// \param MBB The MachineBasicBlock to modify
/// \param Bundles The bundles to emit into the MBB
/// \param TII Target instruction info for NOP insertion
void replaceMBBWithBundles(MachineBasicBlock &MBB,
                           const std::vector<AIE::MachineBundle> &Bundles,
                           const AIEBaseInstrInfo &TII);

/// Merge source bundles into destination MBB.
/// This is a convenience wrapper that combines mergeBundles +
/// replaceMBBWithBundles.
///
/// \param DstMBB The MBB to modify (its non-terminator instructions are
///        replaced)
/// \param DstBundles The original bundles for DstMBB (e.g., epilogue bundles)
/// \param SrcBundles The bundles to merge from (e.g., prologue bundles)
/// \param NumBundlesToMerge How many leading SrcBundles merge into trailing
///        DstBundles
/// \param TII Target instruction info
/// \returns The merged bundles (same size as DstBundles) with cloned
///          instructions. The caller can use this to update BlockState.
std::vector<AIE::MachineBundle>
mergeBundlesIntoMBB(MachineBasicBlock &DstMBB,
                    ArrayRef<AIE::MachineBundle> DstBundles,
                    ArrayRef<AIE::MachineBundle> SrcBundles,
                    unsigned NumBundlesToMerge, const AIEBaseInstrInfo &TII);

/// Type for instruction filter - returns true if instruction should stop
/// matching.
using InstrStopFilter = std::function<bool(const MachineInstr &)>;

/// Count how many leading bundles match between two bundle vectors.
/// Bundles match if they contain the same number of instructions with the
/// same opcodes in the same order.
///
/// The optional StopFilter allows early termination: if StopFilter returns
/// true for any instruction in a bundle being examined, that bundle is
/// considered non-matching and counting stops immediately.
///
/// Example with BundlesA = [A0, A1, A2] and BundlesB = [B0, B1, B2, B3]:
///   - If A0 matches B0 and A1 matches B1 but A2 != B2, returns 2
///   - If StopFilter(some instr in A1) returns true, returns 1 (A0 matched,
///     but A1 stopped early)
///
/// \param BundlesA First bundle vector
/// \param BundlesB Second bundle vector
/// \param StopFilter Optional filter function - if it returns true for any
///        instruction in a bundle, that bundle is considered non-matching
///        and counting stops. Pass nullptr to disable filtering.
/// \returns Number of consecutive matching leading bundles
unsigned countMatchingLeadingBundles(ArrayRef<AIE::MachineBundle> BundlesA,
                                     ArrayRef<AIE::MachineBundle> BundlesB,
                                     InstrStopFilter StopFilter = nullptr);

/// Remove the first NumBundles from the MBB, keeping only the remaining ones.
/// This clones the remaining bundles, erases all non-terminator instructions
/// from the MBB, and re-emits the remaining bundles with proper formatting.
///
/// Example with Bundles = [B0, B1, B2, B3] and NumBundles = 2:
///   - Result: MBB contains only [B2, B3] (cloned)
///   - Returns new vector with [B2, B3]
///
/// \param MBB The MachineBasicBlock to modify
/// \param Bundles The original bundles in the MBB
/// \param NumBundles Number of leading bundles to remove
/// \param TII Target instruction info for NOP insertion
/// \returns Vector of remaining bundles (cloned instructions)
std::vector<AIE::MachineBundle>
removeLeadingBundles(MachineBasicBlock &MBB,
                     ArrayRef<AIE::MachineBundle> Bundles, unsigned NumBundles,
                     const AIEBaseInstrInfo &TII);

/// Transfer the first NumBundles from SrcMBB to the end of DstMBB.
/// This clones the leading bundles and appends them to DstMBB before its
/// terminator, then removes those bundles from SrcMBB.
///
/// Example:
///   SrcBundles = [S0, S1, S2, S3], NumBundles = 2
///   DstMBB gets [S0, S1] appended before terminator
///   SrcMBB is modified to contain only [S2, S3]
///
/// \param DstMBB Destination MBB - bundles are appended before terminator
/// \param SrcMBB Source MBB - leading bundles are removed
/// \param SrcBundles The original bundles in SrcMBB
/// \param NumBundles Number of leading bundles to transfer
/// \param TII Target instruction info
/// \returns Pair of (transferred bundles for DstMBB, remaining bundles for
///          SrcMBB). The caller can use these to update BlockStates.
std::pair<std::vector<AIE::MachineBundle>, std::vector<AIE::MachineBundle>>
transferLeadingBundles(MachineBasicBlock &DstMBB, MachineBasicBlock &SrcMBB,
                       ArrayRef<AIE::MachineBundle> SrcBundles,
                       unsigned NumBundles, const AIEBaseInstrInfo &TII);

} // namespace llvm::AIEMachineBundleUtils

#endif // LLVM_LIB_TARGET_AIE_UTILS_AIEMACHINEBUNDLEUTILS_H
