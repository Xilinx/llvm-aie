//===- AIEMachineAlignment.cpp ----------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEMachineAlignment.h"
#include "AIE.h"
#include "AIEBundle.h"
#include "Utils/AIELoopUtils.h"
#include "Utils/AIEMachineBasicBlockUtils.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/Debug.h"

using namespace llvm;
using namespace AIEMachineBasicBlockUtils;

#define DEBUG_TYPE "aie-machine-alignment"
static cl::opt<bool> SkipMachineAlignment(
    "skip-machine-alignment", cl::init(false), cl::Hidden,
    cl::desc("Use this option to skip the Machine Alignment Pass."));

namespace {

const AIEBaseInstrInfo *getTII(const MachineFunction &MF) {
  auto &Subtarget = MF.getSubtarget();
  return static_cast<const AIEBaseInstrInfo *>(Subtarget.getInstrInfo());
}

/// Find Regions which are Alignment Candidate e.g. Region ending with Return
/// Address, End of BB, ZOL distance, etc.
std::vector<AIE::AlignmentRegion> findRegions(MachineBasicBlock &MBB) {
  const AIEBaseInstrInfo *TII = getTII(*MBB.getParent());
  std::vector<AIE::AlignmentRegion> Regions;
  std::vector<MachineBasicBlock::iterator> AlignCandidate =
      TII->getAlignmentBoundaries(MBB);

  MachineBasicBlock::iterator RegionBegin = MBB.begin();
  for (auto MII : AlignCandidate) {
    MachineBasicBlock::iterator RegionEnd = MII;
    Regions.emplace_back(llvm::make_range(RegionBegin, RegionEnd));
    RegionBegin = RegionEnd;
  }
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
  const AIEBaseInstrInfo *TII = getTII(*MBB.getParent());

  // Most nop bundles contain the 16-bit nop, which can not be represented
  // in other formats. Therefore, we remove all nops both from the bundle and
  // from the basic block, reconstituting a fresh nop bundle from the format's
  // native nops.
  if (Bundle.isNOPBundle()) {
    Bundle.clearMBBBundle();
    Bundle.clear();
  }
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

const VLIWFormat *findClosestBundleFormat(AIE::MachineBundle &Bundle,
                                          const int OrigBundleSize,
                                          int StartBundleSize,
                                          const int EndBundleSize,
                                          const int Step) {
  const VLIWFormat *Format = nullptr;
  for (; !Format && StartBundleSize > OrigBundleSize &&
         StartBundleSize <= EndBundleSize;
       StartBundleSize += Step) {
    Format = getElongatedFormat(Bundle, StartBundleSize);
  }
  return Format;
}

/// \return Missing Bytes to reach alignment. Elongate Bundles between
/// \p PadRegion to reach alignment. Some bundles can only grow
/// by more than the 2 bytes, and we don't want to elongate
/// these if they would overshoot the requested padding and others could supply
/// an exact fit. \p MinimalPadding allows the Bundles to only add
/// \p PadBytes to reach alignment. If set to false, It will add more than
/// strictly necessary to achieve alignment.
unsigned tryRegionAlignment(MultiBlockRegion &PadRegion, unsigned PadBytes,
                            bool MinimalPadding) {

  const AIEBaseInstrInfo &TII = PadRegion.getTII();
  const unsigned MaxBundleSize = TII.getMachineBlockAlignmentBytes();
  bool OvershotPadding = false;
  for (AIE::AlignmentRegion &Region : PadRegion.getRegions()) {
    for (auto MI = Region.begin(); MI != Region.end() && PadBytes != 0; ++MI) {
      if (!MI->isBundle())
        continue;

      AIE::MachineBundle Bundle = TII.getAIEMachineBundle(MI);
      const VLIWFormat *Format = Bundle.getFormatOrNull();
      assert(Format);
      const unsigned BundleSize = Format->getSize();
      const unsigned NumFreeBytesInBundle = MaxBundleSize - BundleSize;

      if (NumFreeBytesInBundle == 0)
        continue;

      const unsigned AddBytesToBundle =
          std::min(NumFreeBytesInBundle, PadBytes);

      // Elongate Bundle with MinimalPadding, i.e. try to minimize newly added
      // NOPs.
      const unsigned IdealBundleSize = BundleSize + AddBytesToBundle;
      Format = getElongatedFormat(Bundle, /*Size=*/IdealBundleSize);
      if (!Format) {
        if (MinimalPadding) {
          // Find largest possible Bundle without increasing the MBB
          // unnecessarily.
          Format = findClosestBundleFormat(
              Bundle, /*OrigBundleSize=*/BundleSize,
              /*StartBundleSize=*/IdealBundleSize,
              /*EndBundleSize=*/BundleSize, /*Step=*/-2);

        } else {
          const unsigned BytesToAlignment = PadBytes + BundleSize;
          // Find smallest bundle that overshoots the strictly needed padding.
          Format = findClosestBundleFormat(
              Bundle, /*OrigBundleSize=*/BundleSize,
              /*StartBundleSize=*/BytesToAlignment,
              /*EndBundleSize=*/MaxBundleSize, /*Step=*/2);
          OvershotPadding = true;
        }
      }
      if (Format) {
        const unsigned BytesAdded = (Format->getSize() - BundleSize);
        assert(BytesAdded > 0);
        elongateBundle(Bundle, *Format, &*MI, std::next(MI));
        PadBytes = (PadBytes - BytesAdded) % MaxBundleSize;
        if (OvershotPadding)
          return PadBytes;
      }
    }
  }

  return PadBytes;
}

void padRegionForAlignment(MultiBlockRegion &PadRegion,
                           const unsigned AlignOffset) {
  const AIEBaseInstrInfo &TII = PadRegion.getTII();

  const unsigned MachineBlockAlignment = TII.getMachineBlockAlignmentBytes();
  const unsigned ModuloAlignOffset = AlignOffset % MachineBlockAlignment;
  unsigned PadBytes =
      (MachineBlockAlignment - ModuloAlignOffset) % MachineBlockAlignment;
  if (PadBytes == 0) {
    LLVM_DEBUG(dbgs() << "No Alignment needed for Region\n");
    return;
  }
  LLVM_DEBUG(dbgs() << "  Need to pad by " << PadBytes << " bytes\n");

  // Try to reach alignment with minimal nop insertions first. If we cannot
  // reach the alignment, insert too many nops and try to align again.
  bool UseMinimalPadding = true;
  while (PadBytes != 0) {
    PadBytes = tryRegionAlignment(PadRegion, PadBytes,
                                  /*MinimalPadding=*/UseMinimalPadding);
    // Allow too much padding, i.e. we may add more bytes than
    // the needed PadBytes.
    UseMinimalPadding = !UseMinimalPadding;
  }
}

bool canMergeFirstRegion(MachineBasicBlock &MBB) {
  const bool IsJumpTarget = !isBlockOnlyReachableByFallthrough(&MBB);
  if (IsJumpTarget)
    return false;

  assert(MBB.pred_size() == 1);
  MachineBasicBlock *PrevMBB = getPrevNonEmptyMBB(&MBB);
  assert(PrevMBB);
  const AIEBaseInstrInfo *TII = getTII(*MBB.getParent());
  assert(TII);
  // MBB cannot be merged with a ZOL body since the fallthrough of a ZOL body
  // must be aligned.
  return !TII->isZOLBody(*PrevMBB);
}

void verifyAlignment(MachineFunction &MF) {
  const AIEBaseInstrInfo &TII = *getTII(MF);
  LLVM_DEBUG(dbgs() << "Alignment Summary: \n");
  unsigned Alignment = 0;
  for (MachineBasicBlock &MBB : MF) {

    if (!canMergeFirstRegion(MBB)) {
      LLVM_DEBUG(dbgs() << "MBB " << MBB.getNumber() << " needs Alignment: "
                        << Alignment % TII.getMachineBlockAlignmentBytes()
                        << "\n");
      assert(Alignment % TII.getMachineBlockAlignmentBytes() == 0 &&
             "Jump Candidate Alignment is wrong");
    }

    LLVM_DEBUG(dbgs() << "MBB " << MBB.getNumber() << " Alignment: "
                      << Alignment % TII.getMachineBlockAlignmentBytes()
                      << "\n");
    const unsigned MBBSize = TII.getMBBSizeInBytes(MBB);
    LLVM_DEBUG(dbgs() << "MBB Size: " << MBBSize << "\n");
    Alignment += MBBSize;
  }
  LLVM_DEBUG(dbgs() << "Final Alignment: "
                    << Alignment % TII.getMachineBlockAlignmentBytes()
                    << "\nFinal Size: " << Alignment << "\n");
  assert(Alignment % TII.getMachineBlockAlignmentBytes() == 0);
}

/// For each ZOL (zero-overhead loop) body, compute and print the distance in
/// bytes from the PC of the last setup instruction (in the preheader) to the
/// PC of the loop end instruction. Also print the distance from every jump
/// (in blocks that have the loop as successor, excluding the loop body) to the
/// loop end.
void verifyDistanceToLoopEnd(MachineFunction &MF) {
  const AIEBaseInstrInfo &TII = *getTII(MF);
  auto ZOLSupport = TII.getZOLSupport();
  if (!ZOLSupport)
    return;
  if (!ZOLSupport->LoopSetupRequiresByteDistance &&
      ZOLSupport->JumpToLoopEndDistanceInBytes == 0)
    return;

  // Compute start offset (in bytes from function start) of each MBB in layout
  // order.
  DenseMap<const MachineBasicBlock *, unsigned> MBBStartOffset;
  unsigned Offset = 0;
  for (MachineBasicBlock &MBB : MF) {
    MBBStartOffset[&MBB] = Offset;
    Offset += TII.getMBBSizeInBytes(MBB);
  }

  for (MachineBasicBlock &MBB : MF) {
    if (!TII.isZOLBody(MBB))
      continue;
    MachineBasicBlock *Preheader =
        AIELoopUtils::getDedicatedFallThroughPreheader(MBB);
    if (!Preheader)
      continue;

    auto LastInstr = MBB.getLastNonDebugInstr();
    unsigned BytesToLoopEndStart = 0;
    for (auto MI = MBB.begin(); MI != LastInstr; ++MI)
      BytesToLoopEndStart += TII.getAIEMachineBundleSize(MI);
    unsigned LoopEndPC = MBBStartOffset[&MBB] + BytesToLoopEndStart;

    if (ZOLSupport->LoopSetupRequiresByteDistance) {
      unsigned BytesToLastSetupStart = 0;
      MachineBasicBlock::const_iterator LastSetupBundle = Preheader->end();
      for (auto MI = Preheader->begin(), E = Preheader->end(); MI != E; ++MI) {
        if (MI->isBundle() && TII.isZOLSetupBundle(MI) &&
            TII.isLastZOLSetupBundleInMBB(MI)) {
          LastSetupBundle = MI;
          break;
        }
        BytesToLastSetupStart += TII.getAIEMachineBundleSize(MI);
      }
      if (LastSetupBundle != Preheader->end()) {
        unsigned LastSetupPC =
            MBBStartOffset[Preheader] + BytesToLastSetupStart;
        unsigned SetupToLoopEndDist = LoopEndPC - LastSetupPC;
        LLVM_DEBUG(dbgs() << "setup-to-loopend distance (MBB "
                          << MBB.getNumber() << "): " << SetupToLoopEndDist
                          << " bytes\n");
        unsigned MinSetupDist =
            TII.getMachineBlockAlignmentBytes() * ZOLSupport->LoopSetupDistance;
        assert(SetupToLoopEndDist >= MinSetupDist &&
               "Setup to Loop End distance is below minimum required");
      }
    }

    if (ZOLSupport->JumpToLoopEndDistanceInBytes > 0) {
      // Find the last jump (closest to loop end) elsewhere in the function that
      // has the preheader or ZOL body as successor (exclude the ZOL body
      // itself).
      for (MachineBasicBlock &OtherMBB : MF) {
        if (&OtherMBB == &MBB)
          continue;
        bool HasLoopAsSuccessor = false;
        for (MachineBasicBlock *Succ : OtherMBB.successors()) {
          if (Succ == Preheader || Succ == &MBB) {
            HasLoopAsSuccessor = true;
            break;
          }
        }
        if (!HasLoopAsSuccessor)
          continue;
        unsigned BytesToClosestJumpStart = 0;
        bool FoundJump = false;
        unsigned CurrentOffset = 0;
        for (auto MI = OtherMBB.begin(), E = OtherMBB.end(); MI != E; ++MI) {
          if (MI->isBundle() && TII.isJumpBundle(MI)) {
            unsigned JumpPC = MBBStartOffset[&OtherMBB] + CurrentOffset;
            if (JumpPC < LoopEndPC) {
              FoundJump = true;
              BytesToClosestJumpStart = CurrentOffset;
            }
          }
          CurrentOffset += TII.getAIEMachineBundleSize(MI);
        }
        if (FoundJump) {
          unsigned JumpPC = MBBStartOffset[&OtherMBB] + BytesToClosestJumpStart;
          unsigned JumpToLoopEndDist = LoopEndPC - JumpPC;
          LLVM_DEBUG(dbgs() << "jump-to-loopend distance (ZOL body MBB "
                            << MBB.getNumber() << ", jump in MBB "
                            << OtherMBB.getNumber()
                            << "): " << JumpToLoopEndDist << " bytes\n");
          assert(JumpToLoopEndDist >=
                     ZOLSupport->JumpToLoopEndDistanceInBytes &&
                 "Jump to Loop End distance is below minimum required");
        }
      }
    }
  }
}

void padRegions(std::vector<MultiBlockRegion> &AllRegions) {
  for (auto [Idx, Region] : enumerate(AllRegions)) {
    LLVM_DEBUG(dbgs() << "Padding Region " << Idx << " to align the next"
                      << "\n");
    padRegionForAlignment(Region, Region.getRegionSize());
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

  const AIEBaseInstrInfo &TII = *getTII(MF);
  std::vector<MultiBlockRegion> CollectedRegions;
  LLVM_DEBUG(dbgs() << MF.getName() << "\n");
  for (MachineBasicBlock &MBB : MF) {
    if (MBB.empty())
      continue; // no need to align empty MBBs.

    LLVM_DEBUG(dbgs() << "bb." << MBB.getNumber() << "\n"; MBB.dump());

    if (TII.isZOLBody(MBB)) {
      // If the Preheader of the ZOL is not aligned properly when we call
      // findRegions , the LoopBody gets too many Regions which enforce too many
      // Alignments and thus unnecessarily increase program memory size.
      LLVM_DEBUG(dbgs() << "Ensure ZOL Preheader is padded.\n");
      padRegions(CollectedRegions);
      CollectedRegions.clear();
    }

    std::vector<AIE::AlignmentRegion> Regions = findRegions(MBB);
    assert(!Regions.empty());

    LLVM_DEBUG(dbgs() << "Region " << CollectedRegions.size() << "\n");

    // Check if the first Region can be appended to the previous
    // MultiRegionBlock
    const bool MergeFirstRegion = canMergeFirstRegion(MBB);
    if (MergeFirstRegion) {
      CollectedRegions.back().append(Regions[0]);
    } else {
      CollectedRegions.emplace_back(Regions[0], TII);
    }

    // Always align the following Regions
    for (AIE::AlignmentRegion &RegionMBB : drop_begin(Regions, 1)) {
      LLVM_DEBUG(dbgs() << "Region " << CollectedRegions.size() << "\n");
      CollectedRegions.emplace_back(RegionMBB, TII);
    }
  }
  // Align the last unaligned Regions of the MF.
  padRegions(CollectedRegions);

  verifyAlignment(MF);

  verifyDistanceToLoopEnd(MF);

  return true;
}

MultiBlockRegion::MultiBlockRegion(const AIE::AlignmentRegion Region,
                                   const AIEBaseInstrInfo &TII)
    : Regions{Region}, TII(TII) {}

void MultiBlockRegion::append(const AIE::AlignmentRegion &Region) {
  LLVM_DEBUG(dbgs() << "Aligning with previous Region\n");
  Regions.emplace_back(Region);
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
