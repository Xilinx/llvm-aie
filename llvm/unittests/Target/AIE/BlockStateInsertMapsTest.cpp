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
  BS.TopInsert.Bundles = {makeBundle({A}), makeBundle({}), makeBundle({B, C})};

  BS.TopInsert.rebuildCycleMapFromStart();

  EXPECT_EQ(BS.TopInsert.CycleMap.lookup(A), 0);
  EXPECT_EQ(BS.TopInsert.CycleMap.lookup(B), 2);
  EXPECT_EQ(BS.TopInsert.CycleMap.lookup(C), 2);
  EXPECT_EQ(BS.TopInsert.CycleMap.size(), 3u);
}

TEST_F(BlockStateInsertMapsTest, BottomInsertCycleMapDepths) {
  auto *A = appendPlainInstr();
  auto *B = appendPlainInstr();
  auto *C = appendPlainInstr();
  BlockState BS(MBB);
  BS.BottomInsert.Bundles = {makeBundle({A}), makeBundle({}),
                             makeBundle({B, C})};

  BS.BottomInsert.rebuildCycleMapFromEnd();

  EXPECT_EQ(BS.BottomInsert.CycleMap.lookup(A), -3);
  EXPECT_EQ(BS.BottomInsert.CycleMap.lookup(B), -1);
  EXPECT_EQ(BS.BottomInsert.CycleMap.lookup(C), -1);
  EXPECT_EQ(BS.BottomInsert.CycleMap.size(), 3u);
}

TEST_F(BlockStateInsertMapsTest, TopInsertCycleMapIgnoresCloneListOrder) {
  auto *A = appendPlainInstr();
  auto *B = appendPlainInstr();
  BlockState BS(MBB);
  // Bundle order is B then A; Clones lists A then B. Depths follow Bundles.
  BS.TopInsert.Bundles = {makeBundle({B}), makeBundle({A})};
  BS.TopInsert.Clones = {A, B};

  BS.TopInsert.rebuildCycleMapFromStart();

  EXPECT_EQ(BS.TopInsert.Clones.size(), 2u);
  EXPECT_EQ(BS.TopInsert.CycleMap.lookup(B), 0);
  EXPECT_EQ(BS.TopInsert.CycleMap.lookup(A), 1);
}

TEST_F(BlockStateInsertMapsTest, BottomInsertCycleMapIgnoresCloneListOrder) {
  auto *A = appendPlainInstr();
  auto *B = appendPlainInstr();
  BlockState BS(MBB);
  // Bundle order is B then A; Clones lists A then B. Depths follow Bundles.
  BS.BottomInsert.Bundles = {makeBundle({B}), makeBundle({A})};
  BS.BottomInsert.Clones = {A, B};

  BS.BottomInsert.rebuildCycleMapFromEnd();

  EXPECT_EQ(BS.BottomInsert.Clones.size(), 2u);
  // Depths before the end: last bundle is -1, first is -2.
  EXPECT_EQ(BS.BottomInsert.CycleMap.lookup(B), -2);
  EXPECT_EQ(BS.BottomInsert.CycleMap.lookup(A), -1);
}

} // namespace
