//===- BlockStateInsertMapsTest.cpp ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEInterBlockScheduling.h"
#include "AIETestTarget.h"
#include "ScheduleDAGMITestUtils.h"

using namespace llvm;
using namespace llvm::AIE;

namespace {

class BlockStateInsertMapsTest : public ScheduleDAGMITest {
public:
  BlockStateInsertMapsTest()
      : ScheduleDAGMITest(AIE::createAIETestTargetMachine()) {}
};

MachineBundle makeBundle(std::initializer_list<MachineInstr *> Instrs) {
  return MachineBundle(Instrs, /*FormatInterface=*/nullptr);
}

TEST_F(BlockStateInsertMapsTest, TopInsertCycleMapFromBundles) {
  auto *A = appendPlainInstr();
  auto *B = appendPlainInstr();
  auto *C = appendPlainInstr();
  BlockState BS(MBB);
  BS.TopInsert = {makeBundle({A}), makeBundle({}), makeBundle({B, C})};

  BS.rebuildTopInsertCycleMap();

  EXPECT_EQ(BS.TopInsertCycleMap.lookup(A), 0);
  EXPECT_EQ(BS.TopInsertCycleMap.lookup(B), 2);
  EXPECT_EQ(BS.TopInsertCycleMap.lookup(C), 2);
  EXPECT_EQ(BS.TopInsertCycleMap.size(), 3u);
}

TEST_F(BlockStateInsertMapsTest, BottomInsertCycleMapHeights) {
  auto *A = appendPlainInstr();
  auto *B = appendPlainInstr();
  auto *C = appendPlainInstr();
  BlockState BS(MBB);
  BS.BottomInsert = {makeBundle({A}), makeBundle({}), makeBundle({B, C})};

  BS.rebuildBottomInsertCycleMap();

  EXPECT_EQ(BS.BottomInsertCycleMap.lookup(A), 2);
  EXPECT_EQ(BS.BottomInsertCycleMap.lookup(B), 0);
  EXPECT_EQ(BS.BottomInsertCycleMap.lookup(C), 0);
  EXPECT_EQ(BS.BottomInsertCycleMap.size(), 3u);
}

} // namespace
