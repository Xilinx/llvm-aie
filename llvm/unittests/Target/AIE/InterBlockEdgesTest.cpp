//===- InterBlockEdgesTest.cpp --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEDataDependenceHelper.h"
#include "AIETestTarget.h"
#include "ScheduleDAGMITestUtils.h"

using namespace llvm;
using namespace llvm::AIE;

namespace {

class InterBlockEdgesTest : public ScheduleDAGMITest {
public:
  InterBlockEdgesTest()
      : ScheduleDAGMITest(AIE::createAIETestTargetMachine()) {}

protected:
  void SetUp() override { SchedCtx.MF = MF.get(); }

  InterBlockEdges makeDAG() { return InterBlockEdges(SchedCtx); }
};

TEST_F(InterBlockEdgesTest, PreVsPostClassification) {
  auto *Pre = appendPlainInstr();
  auto *Post = appendPlainInstr();
  InterBlockEdges DAG = makeDAG();

  DAG.addNode(Pre);
  DAG.markBoundary();
  DAG.addNode(Post);

  const SUnit *PreSU = DAG.getPreBoundaryNode(Pre);
  const SUnit *PostSU = DAG.getPostBoundaryNode(Post);
  ASSERT_NE(PreSU, nullptr);
  ASSERT_NE(PostSU, nullptr);
  EXPECT_TRUE(DAG.isPreBoundaryNode(PreSU));
  EXPECT_FALSE(DAG.isPostBoundaryNode(PreSU));
  EXPECT_EQ(DAG.getPreBoundaryNode(Post), nullptr);
  EXPECT_TRUE(DAG.isPostBoundaryNode(PostSU));
  EXPECT_FALSE(DAG.isPreBoundaryNode(PostSU));
}

TEST_F(InterBlockEdgesTest, SameMIOnBothSides) {
  auto *MI = appendPlainInstr();
  InterBlockEdges DAG = makeDAG();

  DAG.addNode(MI);
  DAG.markBoundary();
  DAG.addNode(MI);

  const SUnit *PreSU = DAG.getPreBoundaryNode(MI);
  const SUnit *PostSU = DAG.getPostBoundaryNode(MI);
  ASSERT_NE(PreSU, nullptr);
  ASSERT_NE(PostSU, nullptr);
  EXPECT_NE(PreSU, PostSU);
  EXPECT_FALSE(DAG.isPostBoundaryNode(PreSU));
  EXPECT_TRUE(DAG.isPostBoundaryNode(PostSU));
}

TEST_F(InterBlockEdgesTest, PostDepthRecording) {
  auto *Post = appendPlainInstr();
  auto *Unknown = appendPlainInstr();
  InterBlockEdges DAG = makeDAG();

  DAG.markBoundary();
  DAG.addNode(Post);
  const SUnit *PostSU = DAG.getPostBoundaryNode(Post);
  ASSERT_NE(PostSU, nullptr);

  EXPECT_FALSE(DAG.getPostDepths().getValue(PostSU).has_value());
  EXPECT_EQ(DAG.getPostDepths().getValueOr(PostSU, 42), 42);
  EXPECT_EQ(DAG.getPostDepths().getRegionMax(), 0);

  DAG.getPostDepths().record(Post, 3);
  EXPECT_TRUE(DAG.getPostDepths().getValue(PostSU).has_value());
  EXPECT_EQ(DAG.getPostDepths().getValueOr(PostSU, 42), 3);
  EXPECT_EQ(DAG.getPostDepths().getRegionMax(), 3);

  DAG.getPostDepths().record(7);
  EXPECT_EQ(DAG.getPostDepths().getValueOr(PostSU, 42), 3);
  EXPECT_EQ(DAG.getPostDepths().getRegionMax(), 7);

  DAG.getPostDepths().record(Unknown, 9);
  EXPECT_EQ(DAG.getPostBoundaryNode(Unknown), nullptr);
  EXPECT_EQ(DAG.getPostDepths().getRegionMax(), 9);

  DAG.getPostDepths().clear();
  EXPECT_FALSE(DAG.getPostDepths().getValue(PostSU).has_value());
  EXPECT_EQ(DAG.getPostDepths().getValueOr(PostSU, 42), 42);
  EXPECT_EQ(DAG.getPostDepths().getRegionMax(), 0);
}

TEST_F(InterBlockEdgesTest, PreDepthRecording) {
  auto *Pre = appendPlainInstr();
  auto *Unknown = appendPlainInstr();
  InterBlockEdges DAG = makeDAG();

  DAG.addNode(Pre);
  DAG.markBoundary();
  const SUnit *PreSU = DAG.getPreBoundaryNode(Pre);
  ASSERT_NE(PreSU, nullptr);

  EXPECT_FALSE(DAG.getPreDepths().getValue(PreSU).has_value());
  EXPECT_EQ(DAG.getPreDepths().getValueOr(PreSU, 99), 99);
  EXPECT_EQ(DAG.getPreDepths().getRegionMax(), 0);

  DAG.getPreDepths().record(Pre, -3);
  EXPECT_TRUE(DAG.getPreDepths().getValue(PreSU).has_value());
  EXPECT_EQ(DAG.getPreDepths().getValueOr(PreSU, 99), -3);
  // Region max starts at 0, so a negative record does not lower it.
  EXPECT_EQ(DAG.getPreDepths().getRegionMax(), 0);

  DAG.getPreDepths().record(1);
  EXPECT_EQ(DAG.getPreDepths().getValueOr(PreSU, 99), -3);
  EXPECT_EQ(DAG.getPreDepths().getRegionMax(), 1);

  DAG.getPreDepths().record(Unknown, 5);
  EXPECT_EQ(DAG.getPreBoundaryNode(Unknown), nullptr);
  EXPECT_EQ(DAG.getPreDepths().getRegionMax(), 5);

  DAG.getPreDepths().clear();
  EXPECT_FALSE(DAG.getPreDepths().getValue(PreSU).has_value());
  EXPECT_EQ(DAG.getPreDepths().getValueOr(PreSU, 99), 99);
  EXPECT_EQ(DAG.getPreDepths().getRegionMax(), 0);
}

TEST_F(InterBlockEdgesTest, ClearResetsDAGAndMaps) {
  auto *Pre = appendPlainInstr();
  auto *Post = appendPlainInstr();
  InterBlockEdges DAG = makeDAG();

  DAG.addNode(Pre);
  DAG.markBoundary();
  DAG.addNode(Post);
  DAG.getPostDepths().record(Post, 4);
  DAG.getPreDepths().record(Pre, -2);

  DAG.clear();

  EXPECT_EQ(DAG.begin(), DAG.end());
  EXPECT_EQ(DAG.getPostDepths().getRegionMax(), 0);
  EXPECT_EQ(DAG.getPreDepths().getRegionMax(), 0);

  // A rebuild can place the same instruction at a new index. The old map entry
  // must not win.
  auto *First = appendPlainInstr();
  auto *Second = appendPlainInstr();
  DAG.addNode(First);
  DAG.addNode(Second);
  DAG.markBoundary();
  DAG.addNode(Post);
  EXPECT_EQ(DAG.getPreBoundaryNode(Pre), nullptr);
  const SUnit *Rebuilt = DAG.getPostBoundaryNode(Post);
  ASSERT_TRUE(Rebuilt);
  EXPECT_EQ(Rebuilt->getInstr(), Post);
}

} // namespace
