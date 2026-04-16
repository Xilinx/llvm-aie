//===- AIELoopClass.cpp - Kernel Classifications --------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIELoopClass.h"
#include "AIEBaseInstrInfo.h"
#include "AIESlotStatistics.h"
#include "llvm/CodeGen/MachineBasicBlock.h"

#define DEBUG_TYPE "aie-loop-class"

namespace llvm::AIE {

class KernelFeatures {
public:
  int Index;
  SlotStatistics Slots;
};

// Note: Index is added to provide a stable numbering to be used by external
// consumers. In that way we we can strip useless entries without the need to
// renumber.
// NOTE: Since we return the best match, we should not strip too far back to
// maintain diversity among the population
// TODO: provide symbolic names for the ones we really use.
static const KernelFeatures Kernels[] = {
    {0, {{0, 0, 0, 0, 0, 0, 120}, {}}},
    {1, {{0, 0, 0, 0, 0, 0, 120, 120}, {0, 120, 120}}},
    {2, {{0, 0, 0, 0, 0, 0, 60}, {}}},
    {3, {{0, 0, 0, 0, 0, 0, 60}, {0, 30, 30}}},
    {4, {{0, 0, 0, 0, 120, 0, 0, 240}, {0, 180, 180}}},
    {5, {{0, 0, 0, 0, 120, 0, 120}, {0, 30, 30}}},
    {6, {{0, 0, 0, 0, 120, 0, 120}, {0, 60, 60}}},
    {7, {{0, 0, 0, 0, 120, 0, 60}, {0, 30, 30}}},
    {8, {{0, 0, 0, 0, 240, 0, 0, 240}, {0, 210, 210}}},
    {9, {{0, 0, 0, 0, 240, 0, 60}, {0, 60, 60}}},
    {10, {{0, 0, 0, 0, 60}, {}}},
    {11, {{0, 0, 0, 0, 600, 0, 0, 480}, {0, 60, 60}}},
    {12, {{0, 0, 0, 0, 60, 0, 60, 60}, {0, 30, 30}}},
    {13, {{0, 120, 0, 0, 0, 0, 120}, {}}},
    {14, {{0, 120, 480, 0, 1020, 0, 420, 240}, {0, 30, 30, 120, 120}}},
    {15, {{0, 180, 0, 0, 0, 0, 0, 240}, {}}},
    {16, {{0, 180, 0, 0, 240, 0, 240, 360}, {0, 130, 130, 0, 0, 0, 40}}},
    {17, {{0, 180, 0, 0, 840, 0, 1020, 780}, {0, 30, 30, 60, 60}}},
    {18, {{0, 360, 0, 0, 0, 0, 360, 480}, {0, 360, 360}}},
    {19, {{0, 60, 0, 0, 0, 0, 0, 60}, {}}},
    {20, {{0, 60, 0, 0, 0, 0, 180}, {}}},
    {21, {{0, 60, 0, 0, 0, 0, 60, 60}, {}}},
    {22, {{0, 60, 0, 0, 0, 0, 60, 60}, {0, 60, 60}}},
    {23, {{0, 60, 0, 0, 60, 0, 120, 60}, {0, 60, 60}}},
    {24, {{120, 0, 0, 0, 120, 0, 120}, {0, 80, 80, 0, 0, 0, 20}}},
    {25, {{120, 120, 0, 0, 0, 0, 120, 60}, {}}},
    {26, {{120, 60, 0, 0, 300, 0, 60, 300}, {}}},
    {27, {{120, 60, 0, 0, 660}, {}}},
    {28, {{3840, 120, 0, 0, 3840, 0, 360, 120}, {0, 60, 60}}},
    {29, {{420, 60, 0, 0, 1260, 0, 360, 1740}, {0, 90, 90}}},
    {30, {{60, 0, 0, 0, 0, 0, 480}, {}}},
    {31, {{60, 0, 0, 0, 120, 0, 60}, {}}},
    {32, {{60, 0, 0, 0, 180, 0, 120}, {}}},
    {33, {{60, 300, 0, 0, 1200, 0, 960, 720}, {0, 30, 30, 60, 60}}},
    {34, {{660, 180, 0, 1560, 0, 0, 2520}, {45, 45, 0, 45, 45}}},
    {35, {{660, 180, 0, 1620, 0, 0, 2520}, {30, 30, 0, 30, 30}}},
    {36, {{660, 180, 0, 1620, 0, 0, 2520}, {45, 45, 0, 45, 45}}},
    {37, {{720, 480, 0, 0, 0, 0, 240}, {0, 80, 80, 0, 0, 0, 80}}},
    {38, {{900, 300, 0, 2280, 0, 0, 2400}, {90, 90, 0, 90, 90}}},
    {39, {{900, 300, 0, 2340, 0, 0, 2400}, {75, 75, 0, 75, 75}}},
    {40, {{900, 300, 0, 2400, 0, 0, 2400}, {45, 45, 0, 45, 45}}},
    {41, {{900, 300, 0, 2400, 0, 0, 2400}, {60, 60, 0, 60, 60}}},
    {42, {{900, 300, 0, 2400, 0, 0, 3360}, {45, 45, 0, 45, 45}}},
    {43, {{900, 300, 0, 2460, 0, 0, 3360}, {30, 30, 0, 30, 30}}},
    {44, {{900, 300, 0, 2520, 0, 0, 3360}, {30, 30, 0, 30, 30}}},
    {45, {{900, 300, 0, 2520, 0, 0, 3360}, {45, 45, 0, 45, 45}}},
    {46, {{0, 0, 0, 0, 2160, 0, 120, 1080}, {0, 420, 420}}},
    {47, {{0, 0, 0, 0, 360, 0, 240, 360}, {0, 60, 60}}},
    // These are pre-regalloc
    {1000, {{0, 0, 0, 0, 480, 0, 480, 480}, {0, 120, 120}}},
    {1001, {{0, 0, 0, 0, 360, 0, 480, 720}, {0, 60, 60}}},

};

std::vector<int> getLoopClassScores(const SlotStatistics &Stats) {
  LLVM_DEBUG(dbgs() << "SlotStatistics="; Stats.dumpShort());
  std::vector<int> Scores;
  for (const KernelFeatures &K : Kernels) {
    Scores.push_back(Stats.distance(K.Slots));
  }
  return Scores;
}

std::vector<int> getLoopClassScores(MachineBasicBlock &MBB,
                                    const AIEBaseInstrInfo *TII) {
  SlotStatistics Stats = computeSlotStatistics(MBB, TII);
  return getLoopClassScores(Stats);
}

int classifyLoop(const SlotStatistics &Stats) {
  std::vector<int> Scores = getLoopClassScores(Stats);
  int Best = 0;
  for (unsigned I = 1; I < Scores.size(); I++) {
    if (Scores[I] < Scores[Best]) {
      Best = I;
    }
  }
  int LoopClass = Kernels[Best].Index;
  LLVM_DEBUG(dbgs() << "LoopClass=" << Best << " Score=" << LoopClass << "\n");
  return LoopClass;
}

int classifyLoop(MachineBasicBlock &MBB, const AIEBaseInstrInfo *TII) {
  SlotStatistics Stats = computeSlotStatistics(MBB, TII);
  return classifyLoop(Stats);
}

} // namespace llvm::AIE
