//===- AIEMachineAlignment.cpp -----------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "AIE2.h"
#include "AIE2InstrInfo.h"
#include "AIE2Subtarget.h"
#include "AIE2TargetMachine.h"
#include "AIEBundle.h"
#include "AIESubtarget.h"

#include "AIEBundle.h"
#include "AIEMachineAlignment.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineInstrBundle.h"

#include <iostream>
using namespace llvm;

#define DEBUG_TYPE "aie-machine-alignment"
static cl::opt<bool> SkipMachineAlignment(
    "skip-machine-alignment", cl::init(false), cl::Hidden,
    cl::desc("Use this option to skip the Machine Alignment Pass."));

namespace {

/// Get TargetInstrInfo from a MachineBasicBlock::iterator
const AIEBaseInstrInfo *getTII(MachineBasicBlock::iterator MII) {
  const MachineBasicBlock *MBB = MII->getParent();
  auto *MF = MBB->getParent();
  auto &Subtarget = MF->getSubtarget();
  return static_cast<const AIEBaseInstrInfo *>(Subtarget.getInstrInfo());
}

const AIE::MachineBundle getAIEMachineBundle(MachineBasicBlock::iterator MII) {
  const AIEBaseInstrInfo *TII = getTII(MII);
  AIE::MachineBundle Bundle(TII->getFormatInterface());
  // Iterate over the instructions in the bundle.
  MachineBasicBlock::const_instr_iterator I = ++MII->getIterator();
  MachineBasicBlock::instr_iterator E = MII->getParent()->instr_end();
  while (I != E && I->isInsideBundle()) {
    MachineInstr *MI = const_cast<MachineInstr *>(&(*I));
    Bundle.add(MI);
    I++;
  }
  return Bundle;
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
  MachineBasicBlock &MBB = *Bundle.getInstrs()[0]->getParent();
  auto *TII = static_cast<const AIEBaseInstrInfo *>(
      MBB.getParent()->getSubtarget().getInstrInfo());

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

unsigned applyRegionAlignment(MachineBasicBlock::iterator MI,
                              MachineBasicBlock::iterator EndMI,
                              unsigned PadBytes, bool AllowCrossingPadBytes) {
  unsigned Size = 0;
  unsigned Stretchability = 0;
  unsigned f = 0;
  while (MI != EndMI) {
    if (MI->isBundle()) {
      AIE::MachineBundle Bundle = getAIEMachineBundle(MI);
      const VLIWFormat *Format = Bundle.getFormatOrNull();
      assert(Format);
      Size = Format->getSize();
      Stretchability = 16 - Format->getSize();
      if (Stretchability == 0) {
        ++MI;
        continue;
      }
      if (Stretchability >= PadBytes) {
        Format = getElongatedFormat(Bundle, PadBytes + Size);
        if (Format) {
          elongateBundle(Bundle, *Format, &*MI, std::next(MI));
          return 0;
        } else {
          // Try to expand to a ValidFormat of Size
          // < (PadBytes + Size) if !AllowCrossingPadBytes or > (PadBytes +
          // Size) if AllowCrossingPadBytes
          for (f = 2;; f = f + 2) {
            unsigned FormatSize = AllowCrossingPadBytes ? PadBytes + Size + f
                                                        : PadBytes + Size - f;
            Format = getElongatedFormat(Bundle, FormatSize);
            if (Format)
              break;
          }
          if (Format->getSize() > Size) {
            elongateBundle(Bundle, *Format, &*MI, std::next(MI));
            Size = Format->getSize();
            PadBytes = AllowCrossingPadBytes ? 16 - f : f;
          }
        }
      } else {
        Format = getElongatedFormat(Bundle, Stretchability + Size);
        assert(Format);
        elongateBundle(Bundle, *Format, &*MI, std::next(MI));
        Size = Format->getSize();
        PadBytes = PadBytes - Stretchability;
        ++MI;
        continue;
      }
    }
    ++MI;
  }
  return PadBytes;
}

unsigned
getRegionSize(llvm::iterator_range<MachineBasicBlock::iterator> Region) {
  unsigned Size = 0;
  LLVM_DEBUG(dbgs() << "---Region Begin---\n");
  for (auto it = Region.begin(), end = Region.end(); it != end; ++it) {
    if (it->isBundle()) {
      AIE::MachineBundle Bundle = getAIEMachineBundle(it);
      const VLIWFormat *Format = Bundle.getFormatOrNull();
      assert(Format);
      Size += Format->getSize();
      LLVM_DEBUG(dbgs() << Format->Name << "\n");
    }
  }
  LLVM_DEBUG(dbgs() << "---Region End---\n");
  LLVM_DEBUG(dbgs() << "Region Size"
                    << " " << Size << "\n");
  return Size;
}

} // namespace

void AIEMachineAlignment::applyBundlesAlignment(
    const std::vector<llvm::iterator_range<MachineBasicBlock::iterator>>
        &Regions) {
  for (auto Region : Regions) {
    unsigned Size = 0;
    unsigned PadBytes = 0;
    Size = getRegionSize(Region);
    if ((Size % 16) == 0)
      continue;
    PadBytes = 16 - (Size % 16);
    while (PadBytes) {
      PadBytes = applyRegionAlignment(Region.begin(), Region.end(), PadBytes,
                                      /*AllowCrossingPadBytes*/ false);
      if (PadBytes) {
        PadBytes = applyRegionAlignment(Region.begin(), Region.end(), PadBytes,
                                        /*AllowCrossingPadBytes*/ true);
      }
    }
  }
}

// Find Regions for Alignment Candidate e.g. Region ending with Return Address,
// End of BB, etc.
static std::vector<llvm::iterator_range<MachineBasicBlock::iterator>>
findRegions(MachineBasicBlock &MBB) {
  auto *TII = static_cast<const AIEBaseInstrInfo *>(
      MBB.getParent()->getSubtarget().getInstrInfo());
  MachineBasicBlock::iterator RegionBegin = MBB.begin();
  std::vector<llvm::iterator_range<MachineBasicBlock::iterator>> Regions;
  std::vector<MachineBasicBlock::iterator> AlgnCandidate =
      TII->getAlignmentBoundaries(MBB);
  for (auto MII : AlgnCandidate) {
    MachineBasicBlock::iterator RegionEnd = MII;
    Regions.emplace_back(llvm::make_range(RegionBegin, RegionEnd));
    RegionBegin = RegionEnd;
  }
  Regions.emplace_back(llvm::make_range(RegionBegin, MBB.end()));
  return Regions;
}

bool AIEMachineAlignment::runOnMachineFunction(MachineFunction &MF) {
  if (SkipMachineAlignment)
    return false;
  // This discards all of the MachineBasicBlock numbers and recomputes them.
  // This guarantees that the MBB numbers are sequential, dense, and match the
  // ordering of the blocks within the function.
  MF.RenumberBlocks();

  for (auto &MBB : MF) {
    std::vector<llvm::iterator_range<MachineBasicBlock::iterator>> Regions =
        findRegions(MBB);
    applyBundlesAlignment(Regions);
    // Clean up BB local Regions
    Regions.clear();
  }
  return true;
}

INITIALIZE_PASS_BEGIN(AIEMachineAlignment, DEBUG_TYPE, "AIE Machine Alignment",
                      false, false)
INITIALIZE_PASS_END(AIEMachineAlignment, DEBUG_TYPE, "AIE Machine Alignment",
                    false, false)

char AIEMachineAlignment::ID = 0;
llvm::FunctionPass *llvm::createAIEMachineAlignment() {
  return new AIEMachineAlignment();
}
