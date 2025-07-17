//===- AIEMachineAlignment.cpp ----------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEMachineAlignment.h"
#include "AIE.h"
#include "AIEBundle.h"
#include "Utils/AIEMachineBasicBlockUtils.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineInstrBundle.h"

#include <iostream>
using namespace llvm;

#define DEBUG_TYPE "aie-machine-alignment"
static cl::opt<bool> SkipMachineAlignment(
    "skip-machine-alignment", cl::init(false), cl::Hidden,
    cl::desc("Use this option to skip the Machine Alignment Pass."));

namespace {

const AIEBaseInstrInfo *getTII(const MachineFunction &MF) {
  auto &Subtarget = MF.getSubtarget();
  return static_cast<const AIEBaseInstrInfo *>(Subtarget.getInstrInfo());
}

/// Find Regions for Alignment Candidate e.g. Region ending with Return Address,
/// End of BB, ZOL distance, etc. \p EnforceAsserts runs sanity checks in
/// asserts
std::vector<llvm::iterator_range<MachineBasicBlock::iterator>>
findRegions(MachineBasicBlock &MBB, const bool EnforceAsserts) {
  const AIEBaseInstrInfo *TII = getTII(*MBB.getParent());
  MachineBasicBlock::iterator RegionBegin = MBB.begin();
  std::vector<llvm::iterator_range<MachineBasicBlock::iterator>> Regions;
  std::vector<MachineBasicBlock::iterator> AlgnCandidate =
      TII->getAlignmentBoundaries(MBB, /*EnforceAsserts=*/EnforceAsserts);
  for (auto MII : AlgnCandidate) {
    MachineBasicBlock::iterator RegionEnd = MII;
    Regions.emplace_back(llvm::make_range(RegionBegin, RegionEnd));
    RegionBegin = RegionEnd;
  }
  Regions.emplace_back(llvm::make_range(RegionBegin, MBB.end()));
  return Regions;
}

const VLIWFormat *getElongatedFormat(AIE::MachineBundle &Bundle,
                                     unsigned Size) {
  if (Bundle.isNOPBundle()) {
    AIE::MachineBundle EmptyBundle(Bundle.FormatInterface);
    return EmptyBundle.getFormatOrNull(Size);
  }
  return Bundle.getFormatOrNull(Size);
}

void elongateBundle(AIE::MachineBundle &Bundle,
                    const VLIWFormat &ElongatedFormat, MachineInstr *BundleRoot,
                    MachineBasicBlock::iterator InsertPoint) {

  if (Bundle.empty())
    return;
  const VLIWFormat *CurrentFormat = Bundle.getFormatOrNull();
  assert(CurrentFormat);
  if (CurrentFormat->getSize() == ElongatedFormat.getSize())
    return;
  MachineBasicBlock &MBB = *Bundle.getInstrs()[0]->getParent();
  const auto *TII = getTII(*MBB.getParent());

  // Clear the NOPBundle
  if (Bundle.isNOPBundle())
    Bundle.clearBundle();
  // Run over the slots of the format and either insert the occupying
  // instruction or a nop. Reapply bundling.
  for (MCSlotKind Slot : ElongatedFormat.getSlots()) {
    const MCSlotInfo *SlotInfo = TII->getSlotInfo(Slot);
    assert(SlotInfo);

    llvm::MachineInstr *Instr = Bundle.at(Slot);
    if (Instr) {
      Instr->removeFromBundle();
      MBB.insert(InsertPoint, Instr);
    } else {
      DebugLoc DL;
      Instr = BuildMI(MBB, InsertPoint, DL, TII->get(SlotInfo->getNOPOpcode()));
    }
    if (!BundleRoot)
      BundleRoot = Instr;
    else if (Instr)
      Instr->bundleWithPred();
  }
}

const VLIWFormat *findClosestBundle(AIE::MachineBundle &Bundle,
                                    const int OrigBundleSize,
                                    int StartBundleSize,
                                    const int EndBundleSize,
                                    const int Increment) {
  const VLIWFormat *Format = nullptr;
  for (; !Format && StartBundleSize > OrigBundleSize &&
         StartBundleSize <= EndBundleSize;
       StartBundleSize += Increment) {
    Format = getElongatedFormat(Bundle, StartBundleSize);
  }
  if (StartBundleSize == OrigBundleSize) {
    // Could not find a larger Bundle than the original Bundle Size
    return nullptr;
  }

  return Format;
}

/// \return Missing Bytes to reach alignment. Elongate Bundles between
/// \p PadRegion to reach alignment. Some bundles can only grow
/// by four bytes, and we don't want to elongate these if they would overshoot
/// the requested padding and others could supply an exact fit. \p OverShooting
/// allows the Bundles to extend larger than \p PadBytes to reach alignment.
unsigned tryRegionAlignment(MultiBlockRegion &PadRegion, unsigned PadBytes,
                            bool OverShooting) {

  const AIEBaseInstrInfo &TII = PadRegion.getTII();
  const unsigned MaxBundleSize = TII.getMachineBlockAlignmentBytes();
  for (auto Region : PadRegion.getRegions()) {
    for (auto MI = Region.begin(); MI != Region.end() && PadBytes != 0; ++MI) {
      if (!MI->isBundle())
        continue;

      AIE::MachineBundle Bundle = TII.getAIEMachineBundle(MI);
      const VLIWFormat *Format = Bundle.getFormatOrNull();
      assert(Format);
      const unsigned BundleSize = Format->getSize();
      const unsigned NumFreeBytesInBundle = MaxBundleSize - Format->getSize();

      if (NumFreeBytesInBundle == 0)
        continue;

      const unsigned AddBytesToBundle =
          std::min(NumFreeBytesInBundle, PadBytes);

      // Elongate Bundle with no overshooting.
      const unsigned IdealBundleSize = BundleSize + AddBytesToBundle;
      Format = getElongatedFormat(Bundle, /*Size=*/IdealBundleSize);
      if (!Format) {
        if (!OverShooting) {
          // Find largest possible Bundle without overshooting.
          Format = findClosestBundle(Bundle, /*OrigBundleSize=*/BundleSize,
                                     /*StartBundleSize=*/IdealBundleSize,
                                     /*EndBundleSize=*/BundleSize, -2);

        } else {
          const unsigned BytesToAlignment = PadBytes + BundleSize;
          // Find smallest bundle that overshoots the padding.
          Format = findClosestBundle(Bundle, /*OrigBundleSize=*/BundleSize,
                                     /*StartBundleSize=*/BytesToAlignment,
                                     /*EndBundleSize=*/MaxBundleSize, 2);
        }
      }
      if (Format) {
        assert(Format);
        const unsigned BytesAdded = (Format->getSize() - BundleSize);
        assert(BytesAdded > 0);
        elongateBundle(Bundle, *Format, &*MI, std::next(MI));
        PadBytes = (PadBytes - BytesAdded) % MaxBundleSize;
      }
    }
  }

  return PadBytes;
}

void padRegion(MultiBlockRegion &PadRegion, unsigned AlignOffset) {
  const auto &TII = PadRegion.getTII();

  const unsigned MachineBlockAlignment = TII.getMachineBlockAlignmentBytes();
  unsigned PadBytes =
      (MachineBlockAlignment - AlignOffset) % MachineBlockAlignment;
  if (PadBytes == 0) {
    LLVM_DEBUG(dbgs() << "No Alignment needed for Region\n");
    return;
  }
  LLVM_DEBUG(dbgs() << "  Need to pad by " << PadBytes << " bytes\n");

  while (PadBytes != 0) {

    // Try to reach alignment with minimal nop insertion (no overshooting to
    // reach alignment).
    PadBytes = tryRegionAlignment(PadRegion, PadBytes,
                                  /*OverShooting=*/false);

    if (PadBytes != 0) {
      // Allow overshooting during padding, i.e. we may add more bytes than the
      // needed PadBytes.
      PadBytes = tryRegionAlignment(PadRegion, PadBytes,
                                    /*OverShooting=*/true);
    }
  }
}

bool isJumpTarget(const MachineBasicBlock &MBB) {
  if (MBB.pred_size() > 1 || MBB.hasAddressTaken())
    return true;

  if (MBB.pred_empty())
    return false;

  assert(MBB.pred_size() == 1);
  const MachineBasicBlock *PredMBB = *MBB.pred_begin();
  assert(PredMBB);
  const bool LayoutSuccessor = PredMBB->getNumber() + 1 == MBB.getNumber();
  if (!LayoutSuccessor)
    return true;

  if (PredMBB->empty())
    return isJumpTarget(*PredMBB);
  return false;
}

unsigned getMBBSize(MachineBasicBlock &MBB, const bool EnforceAsserts) {
  const AIEBaseInstrInfo *TII = getTII(*MBB.getParent());
  unsigned Size = 0;
  for (auto &MI : MBB) {
    Size += TII->getAIEMachineBundleSize(&MI);
  }
  return Size;
}

void verifyAlignment(MachineFunction &MF) {
  const AIEBaseInstrInfo &TII = *getTII(MF);
  LLVM_DEBUG(dbgs() << "Alignment Summary: \n");
  unsigned Alignment = 0;
  for (auto &MBB : MF) {

    if (isJumpTarget(MBB) || getTII(*MBB.getParent())->isZOLBody(MBB)) {
      LLVM_DEBUG(dbgs() << "MBB " << MBB.getNumber() << " needs Alignment: "
                        << Alignment % TII.getMachineBlockAlignmentBytes()
                        << "\n");
      assert(Alignment % TII.getMachineBlockAlignmentBytes() == 0 &&
             "Jump Candidate Alignment is wrong");
    }

    // verify Alignment of Regions.
    findRegions(MBB, /*EnforceAsserts=*/true);

    LLVM_DEBUG(dbgs() << "MBB " << MBB.getNumber() << " Alignment: "
                      << Alignment % TII.getMachineBlockAlignmentBytes()
                      << "\n");
    Alignment += getMBBSize(MBB, true);
  }
  LLVM_DEBUG(dbgs() << "Final Alignment: "
                    << Alignment % TII.getMachineBlockAlignmentBytes()
                    << "\nFinal Size: " << Alignment << "\n");
}

MachineBasicBlock *getPrevNonEmptyMBB(MachineBasicBlock *MBB) {
  if (!MBB)
    return nullptr;

  while ((MBB = MBB->getPrevNode())) {
    if (!MBB->empty()) {
      return MBB;
    }
  }
  return nullptr;
}

bool canMergeFirstRegion(MachineBasicBlock &MBB) {
  if (isJumpTarget(MBB))
    return false;

  MachineBasicBlock *PrevMBB = getPrevNonEmptyMBB(&MBB);
  if (!PrevMBB)
    return false;

  if (getTII(*MBB.getParent())->isZOLBody(*PrevMBB))
    return false;

  if (PrevMBB->pred_empty())
    return true; // can always merge with 1. MBB of MF

  return AIEMachineBasicBlockUtils::isBlockOnlyReachableByFallthrough(PrevMBB);
}

void padRegions(std::vector<MultiBlockRegion> &AllRegions) {
  for (unsigned Idx = 0; Idx < AllRegions.size(); Idx++) {
    MultiBlockRegion &Region = AllRegions[Idx];
    LLVM_DEBUG(dbgs() << "Aligning Region " << Idx << "\n");
    padRegion(Region, Region.getRegionSize());
  }
}

} // namespace

bool AIEMachineAlignment::runOnMachineFunction(MachineFunction &MF) {
  if (SkipMachineAlignment)
    return false;
  // This discards all of the MachineBasicBlock numbers and recomputes them.
  // This guarantees that the MBB numbers are sequential, dense, and match the
  // ordering of the blocks within the function.
  MF.RenumberBlocks();

  auto AllRegions = getAllRegions(MF);

  padRegions(AllRegions);

  verifyAlignment(MF);

  return true;
}

std::vector<MultiBlockRegion>
AIEMachineAlignment::getAllRegions(MachineFunction &MF) const {

  const AIEBaseInstrInfo &TII = *getTII(MF);
  std::vector<MultiBlockRegion> AllRegions;
  BitVector AlignRegion{MF.size()};
  LLVM_DEBUG(dbgs() << MF.getName() << "\n");
  unsigned RegionIdx = 0;
  for (auto &MBB : MF) {
    if (MBB.empty())
      continue; // no need to align empty MBBs.

    LLVM_DEBUG(dbgs() << "bb." << MBB.getNumber() << "\n");

    // Do not enforce asserts, because Region boundaries have not yet been
    // aligned.
    auto Regions = findRegions(MBB, /*EnforceAsserts=*/false);
    assert(!Regions.empty());

    LLVM_DEBUG(dbgs() << "Region " << RegionIdx++ << "\n");
    // Check if the first Region can be appended to the previous
    // MultiRegionBlock
    const bool MergeFirstRegion = canMergeFirstRegion(MBB);
    if (MergeFirstRegion) {
      AllRegions.back().append(Regions[0]);
    } else {
      AllRegions.push_back({Regions[0], TII});
    }

    // Always align the all the following Regions
    for (auto &RegionMBB : drop_begin(Regions, 1)) {
      LLVM_DEBUG(dbgs() << "Region " << RegionIdx++ << "\n");
      AllRegions.emplace_back(RegionMBB, TII);
    }
  }
  return AllRegions;
}

MultiBlockRegion::MultiBlockRegion(const Region Region,
                                   const AIEBaseInstrInfo &TII)
    : Regions{Region}, TII(TII) {}

void MultiBlockRegion::append(const Region &Region) {
  LLVM_DEBUG(dbgs() << "Aligning with previous Region\n");
  Regions.push_back(Region);
}

const AIEBaseInstrInfo &MultiBlockRegion::getTII() const { return TII; }

unsigned MultiBlockRegion::getRegionSize() const {
  unsigned Size = 0;
  for (auto &Region : Regions) {
    Size += TII.getRegionSizeInBytes(Region);
  }
  return Size;
}

INITIALIZE_PASS_BEGIN(AIEMachineAlignment, DEBUG_TYPE, "AIE Machine Alignment",
                      false, false)
INITIALIZE_PASS_END(AIEMachineAlignment, DEBUG_TYPE, "AIE Machine Alignment",
                    false, false)

char AIEMachineAlignment::ID = 0;
llvm::FunctionPass *llvm::createAIEMachineAlignment() {
  return new AIEMachineAlignment();
}
