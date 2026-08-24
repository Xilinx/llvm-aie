//===-- AIEMachineBundleUtils.cpp - MachineBundle utilities ---------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEMachineBundleUtils.h"
#include "AIEBaseInstrInfo.h"
#include "AIEBundle.h"
#include "AIEHazardRecognizer.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "aie-bundle-utils"

using namespace llvm;

namespace {

//===----------------------------------------------------------------------===//
// Internal Helper Functions
//===----------------------------------------------------------------------===//

/// Clone a range of bundles [Start, End) from the source bundles.
/// \param MF The MachineFunction used for cloning instructions
/// \param Bundles The source bundles to clone from
/// \param Start Start index (inclusive)
/// \param End End index (exclusive)
/// \param FormatInterface Format interface for creating new bundles
/// \returns A vector of cloned bundles
std::vector<AIE::MachineBundle>
cloneBundleRange(MachineFunction &MF, ArrayRef<AIE::MachineBundle> Bundles,
                 unsigned Start, unsigned End,
                 const AIEBaseMCFormats *FormatInterface) {
  assert(Start <= End && End <= Bundles.size() && "Invalid range");

  std::vector<AIE::MachineBundle> ClonedBundles;
  ClonedBundles.reserve(End - Start);

  for (unsigned I = Start; I < End; ++I) {
    AIE::MachineBundle NewBundle(FormatInterface);
    for (MachineInstr *MI : Bundles[I].getInstrs()) {
      MachineInstr *Clone = MF.CloneMachineInstr(MI);
      NewBundle.add(Clone);
    }
    ClonedBundles.push_back(std::move(NewBundle));
  }

  return ClonedBundles;
}

/// Remove all non-terminator instructions from the given MBB.
void clearNonTerminatorInstrs(MachineBasicBlock &MBB) {
  for (auto I = MBB.begin(); I != MBB.getFirstTerminator();) {
    MachineInstr &MI = *I++;
    MI.eraseFromParent();
  }
}

/// Insert bundles into MBB before the first terminator and apply bundling.
/// \param MBB The target basic block
/// \param Bundles The bundles to emit
/// \param TII The target instruction info
void emitBundlesToMBB(MachineBasicBlock &MBB,
                      const std::vector<AIE::MachineBundle> &Bundles,
                      const AIEBaseInstrInfo &TII) {
  MachineBasicBlock::iterator InsertPt = MBB.getFirstTerminator();

  for (const auto &Bundle : Bundles) {
    if (Bundle.empty()) {
      TII.insertNoop(MBB, InsertPt);
    } else {
      for (MachineInstr *MI : Bundle.getInstrs()) {
        MBB.insert(InsertPt, MI);
      }
    }
  }

  if (!Bundles.empty())
    AIEHazardRecognizer::applyBundles(Bundles, &MBB);
}

/// Check if a bundle contains any instruction matching the stop filter.
/// \returns true if stop filter is triggered, false otherwise
bool containsStopFilterMatch(
    const AIE::MachineBundle &Bundle,
    llvm::AIEMachineBundleUtils::InstrStopFilter Filter, unsigned BundleIdx,
    const char *BundleName) {
  if (!Filter)
    return false;

  for (const MachineInstr *MI : Bundle.getInstrs()) {
    if (Filter(*MI)) {
      LLVM_DEBUG(dbgs() << "countMatchingLeadingBundles: StopFilter "
                        << "triggered on " << BundleName << "[" << BundleIdx
                        << "]\n");
      return true;
    }
  }
  return false;
}

/// Check if two bundles have matching opcodes.
/// \returns true if all opcodes match, false otherwise
bool bundlesHaveMatchingOpcodes(const AIE::MachineBundle &BundleA,
                                const AIE::MachineBundle &BundleB,
                                unsigned BundleIdx) {
  if (BundleA.size() != BundleB.size()) {
    LLVM_DEBUG(dbgs() << "countMatchingLeadingBundles: size mismatch at "
                      << BundleIdx << " (" << BundleA.size() << " vs "
                      << BundleB.size() << ")\n");
    return false;
  }

  auto ItA = BundleA.getInstrs().begin();
  auto ItB = BundleB.getInstrs().begin();

  for (; ItA != BundleA.getInstrs().end(); ++ItA, ++ItB) {
    if ((*ItA)->getOpcode() != (*ItB)->getOpcode()) {
      LLVM_DEBUG(dbgs() << "countMatchingLeadingBundles: opcode mismatch at "
                        << "bundle " << BundleIdx << "\n");
      return false;
    }
  }

  return true;
}

} // anonymous namespace

//===----------------------------------------------------------------------===//
// Public API Implementation
//===----------------------------------------------------------------------===//

namespace llvm::AIEMachineBundleUtils {

std::vector<AIE::MachineBundle>
mergeBundles(MachineFunction &MF, ArrayRef<AIE::MachineBundle> DstBundles,
             ArrayRef<AIE::MachineBundle> SrcBundles,
             unsigned NumBundlesToMerge,
             const AIEBaseMCFormats *FormatInterface, unsigned NopOpc) {
  const int DstSize = static_cast<int>(DstBundles.size());
  const int SrcSize = static_cast<int>(SrcBundles.size());
  const int MergeOffset = DstSize - static_cast<int>(NumBundlesToMerge);

  LLVM_DEBUG(dbgs() << "mergeBundles: merging " << NumBundlesToMerge
                    << " bundles (DstSize=" << DstSize
                    << ", SrcSize=" << SrcSize << ")\n");

  std::vector<AIE::MachineBundle> MergedBundles;
  MergedBundles.reserve(DstSize);

  for (int BundleIdx = 0; BundleIdx < DstSize; ++BundleIdx) {
    AIE::MachineBundle NewBundle(FormatInterface);

    // Clone destination instructions (skip NOPs)
    for (MachineInstr *MI : DstBundles[BundleIdx].getInstrs()) {
      if (MI->getOpcode() != NopOpc) {
        MachineInstr *Clone = MF.CloneMachineInstr(MI);
        NewBundle.add(Clone);
      }
    }

    // Clone source instructions for trailing bundles
    const int SrcIdx = BundleIdx - MergeOffset;
    if (SrcIdx >= 0 && SrcIdx < static_cast<int>(NumBundlesToMerge) &&
        SrcIdx < SrcSize) {
      for (MachineInstr *MI : SrcBundles[SrcIdx].getInstrs()) {
        MachineInstr *Clone = MF.CloneMachineInstr(MI);
        NewBundle.add(Clone);
        LLVM_DEBUG(dbgs() << "  Added source clone to bundle " << BundleIdx
                          << ": " << *Clone);
      }
    }

    MergedBundles.push_back(std::move(NewBundle));
  }

  return MergedBundles;
}

void replaceMBBWithBundles(MachineBasicBlock &MBB,
                           const std::vector<AIE::MachineBundle> &Bundles,
                           const AIEBaseInstrInfo &TII) {
  LLVM_DEBUG(dbgs() << "replaceMBBWithBundles: BB#" << MBB.getNumber()
                    << " with " << Bundles.size() << " bundles\n");

  clearNonTerminatorInstrs(MBB);
  emitBundlesToMBB(MBB, Bundles, TII);
}

std::vector<AIE::MachineBundle>
mergeBundlesIntoMBB(MachineBasicBlock &DstMBB,
                    ArrayRef<AIE::MachineBundle> DstBundles,
                    ArrayRef<AIE::MachineBundle> SrcBundles,
                    unsigned NumBundlesToMerge, const AIEBaseInstrInfo &TII) {
  if (NumBundlesToMerge == 0)
    return std::vector<AIE::MachineBundle>(DstBundles.begin(),
                                           DstBundles.end());

  MachineFunction &MF = *DstMBB.getParent();
  const AIEBaseMCFormats *FormatInterface = TII.getFormatInterface();
  const unsigned NopOpc = FormatInterface->getSlotInfo(0)->getNOPOpcode();

  auto MergedBundles = mergeBundles(MF, DstBundles, SrcBundles,
                                    NumBundlesToMerge, FormatInterface, NopOpc);
  replaceMBBWithBundles(DstMBB, MergedBundles, TII);

  return MergedBundles;
}

unsigned countMatchingLeadingBundles(ArrayRef<AIE::MachineBundle> BundlesA,
                                     ArrayRef<AIE::MachineBundle> BundlesB,
                                     InstrStopFilter StopFilter) {
  const unsigned MinSize = std::min(BundlesA.size(), BundlesB.size());
  unsigned MatchCount = 0;

  for (unsigned I = 0; I < MinSize; ++I) {
    const AIE::MachineBundle &BundleA = BundlesA[I];
    const AIE::MachineBundle &BundleB = BundlesB[I];

    // Check stop filter on both bundles
    if (containsStopFilterMatch(BundleA, StopFilter, I, "BundleA") ||
        containsStopFilterMatch(BundleB, StopFilter, I, "BundleB"))
      return MatchCount;

    // Check if bundles match
    if (!bundlesHaveMatchingOpcodes(BundleA, BundleB, I))
      return MatchCount;

    ++MatchCount;
  }

  LLVM_DEBUG(dbgs() << "countMatchingLeadingBundles: matched " << MatchCount
                    << " leading bundles\n");
  return MatchCount;
}

std::vector<AIE::MachineBundle>
removeLeadingBundles(MachineBasicBlock &MBB,
                     ArrayRef<AIE::MachineBundle> Bundles, unsigned NumBundles,
                     const AIEBaseInstrInfo &TII) {
  assert(NumBundles <= Bundles.size() && "NumBundles exceeds bundle count");
  if (NumBundles == 0)
    return std::vector<AIE::MachineBundle>(Bundles.begin(), Bundles.end());

  MachineFunction &MF = *MBB.getParent();
  const AIEBaseMCFormats *FormatInterface = TII.getFormatInterface();
  const unsigned BundleSize = Bundles.size();

  LLVM_DEBUG(dbgs() << "removeLeadingBundles: removing " << NumBundles
                    << " bundles of " << BundleSize << " from BB#"
                    << MBB.getNumber() << "\n");

  // Clone the remaining bundles (after NumBundles)
  auto RemainingBundles =
      cloneBundleRange(MF, Bundles, NumBundles, BundleSize, FormatInterface);

  // Replace MBB content with remaining bundles
  clearNonTerminatorInstrs(MBB);
  emitBundlesToMBB(MBB, RemainingBundles, TII);

  return RemainingBundles;
}

std::pair<std::vector<AIE::MachineBundle>, std::vector<AIE::MachineBundle>>
transferLeadingBundles(MachineBasicBlock &DstMBB, MachineBasicBlock &SrcMBB,
                       ArrayRef<AIE::MachineBundle> SrcBundles,
                       unsigned NumBundles, const AIEBaseInstrInfo &TII) {
  assert(NumBundles <= SrcBundles.size() && "NumBundles exceeds bundle count");
  if (NumBundles == 0)
    return {
        {},
        std::vector<AIE::MachineBundle>(SrcBundles.begin(), SrcBundles.end())};

  MachineFunction &MF = *DstMBB.getParent();
  const AIEBaseMCFormats *FormatInterface = TII.getFormatInterface();
  const unsigned SrcSize = SrcBundles.size();

  LLVM_DEBUG(dbgs() << "transferLeadingBundles: transferring " << NumBundles
                    << " bundles of " << SrcSize << " from BB#"
                    << SrcMBB.getNumber() << " to BB#" << DstMBB.getNumber()
                    << "\n");

  // Clone bundles to transfer and remaining bundles
  auto TransferredBundles =
      cloneBundleRange(MF, SrcBundles, 0, NumBundles, FormatInterface);
  auto RemainingBundles =
      cloneBundleRange(MF, SrcBundles, NumBundles, SrcSize, FormatInterface);

  // Emit transferred bundles to destination
  emitBundlesToMBB(DstMBB, TransferredBundles, TII);

  // Replace source content with remaining bundles
  clearNonTerminatorInstrs(SrcMBB);
  emitBundlesToMBB(SrcMBB, RemainingBundles, TII);

  return {std::move(TransferredBundles), std::move(RemainingBundles)};
}

} // namespace llvm::AIEMachineBundleUtils
