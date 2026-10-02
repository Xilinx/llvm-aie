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
#include "AIEMaxLatencyFinder.h"
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

  EXPECT_FALSE(DAG.hasPostDepth(PostSU));
  EXPECT_EQ(DAG.getPostDepthOr(PostSU, 42), 42);
  EXPECT_EQ(DAG.getPostRegionMaxDepth(), 0);

  DAG.recordPostDepth(Post, 3);
  EXPECT_TRUE(DAG.hasPostDepth(PostSU));
  EXPECT_EQ(DAG.getPostDepthOr(PostSU, 42), 3);
  EXPECT_EQ(DAG.getPostRegionMaxDepth(), 3);

  DAG.recordPostDepth(7);
  EXPECT_EQ(DAG.getPostDepthOr(PostSU, 42), 3);
  EXPECT_EQ(DAG.getPostRegionMaxDepth(), 7);

  DAG.recordPostDepth(Unknown, 9);
  EXPECT_EQ(DAG.getPostBoundaryNode(Unknown), nullptr);
  EXPECT_EQ(DAG.getPostRegionMaxDepth(), 9);

  DAG.clearPostDepths();
  EXPECT_FALSE(DAG.hasPostDepth(PostSU));
  EXPECT_EQ(DAG.getPostDepthOr(PostSU, 42), 42);
  EXPECT_EQ(DAG.getPostRegionMaxDepth(), 0);
}

TEST_F(InterBlockEdgesTest, PreDepthRecording) {
  auto *Pre = appendPlainInstr();
  auto *Unknown = appendPlainInstr();
  InterBlockEdges DAG = makeDAG();

  DAG.addNode(Pre);
  DAG.markBoundary();
  const SUnit *PreSU = DAG.getPreBoundaryNode(Pre);
  ASSERT_NE(PreSU, nullptr);

  EXPECT_FALSE(DAG.hasPreDepth(PreSU));
  EXPECT_EQ(DAG.getPreDepthOr(PreSU, 99), 99);
  EXPECT_EQ(DAG.getPreRegionMaxDepth(), 0);

  DAG.recordPreDepth(Pre, -3);
  EXPECT_TRUE(DAG.hasPreDepth(PreSU));
  EXPECT_EQ(DAG.getPreDepthOr(PreSU, 99), -3);
  // Region max starts at 0, so a negative record does not lower it.
  EXPECT_EQ(DAG.getPreRegionMaxDepth(), 0);

  DAG.recordPreDepth(1);
  EXPECT_EQ(DAG.getPreDepthOr(PreSU, 99), -3);
  EXPECT_EQ(DAG.getPreRegionMaxDepth(), 1);

  DAG.recordPreDepth(Unknown, 5);
  EXPECT_EQ(DAG.getPreBoundaryNode(Unknown), nullptr);
  EXPECT_EQ(DAG.getPreRegionMaxDepth(), 5);

  DAG.clearPreDepths();
  EXPECT_FALSE(DAG.hasPreDepth(PreSU));
  EXPECT_EQ(DAG.getPreDepthOr(PreSU, 99), 99);
  EXPECT_EQ(DAG.getPreRegionMaxDepth(), 0);
}

TEST_F(InterBlockEdgesTest, PreHeightRecording) {
  auto *Pre = appendPlainInstr();
  auto *Unknown = appendPlainInstr();
  InterBlockEdges DAG = makeDAG();

  DAG.addNode(Pre);
  DAG.markBoundary();
  const SUnit *PreSU = DAG.getPreBoundaryNode(Pre);
  ASSERT_NE(PreSU, nullptr);

  EXPECT_FALSE(DAG.hasPreHeight(PreSU));
  EXPECT_EQ(DAG.getPreHeightOr(PreSU, 99), 99);
  EXPECT_EQ(DAG.getPreRegionMaxHeight(), 0);

  DAG.recordPreHeight(Pre, 4);
  EXPECT_TRUE(DAG.hasPreHeight(PreSU));
  EXPECT_EQ(DAG.getPreHeightOr(PreSU, 99), 4);
  EXPECT_EQ(DAG.getPreRegionMaxHeight(), 4);

  DAG.recordPreHeight(8);
  EXPECT_EQ(DAG.getPreHeightOr(PreSU, 99), 4);
  EXPECT_EQ(DAG.getPreRegionMaxHeight(), 8);

  DAG.recordPreHeight(Unknown, 5);
  EXPECT_EQ(DAG.getPreBoundaryNode(Unknown), nullptr);
  EXPECT_EQ(DAG.getPreRegionMaxHeight(), 8);

  DAG.clearPreHeights();
  EXPECT_FALSE(DAG.hasPreHeight(PreSU));
  EXPECT_EQ(DAG.getPreHeightOr(PreSU, 99), 99);
  EXPECT_EQ(DAG.getPreRegionMaxHeight(), 0);
}

TEST_F(InterBlockEdgesTest, PostHeightRecording) {
  auto *Post = appendPlainInstr();
  auto *Unknown = appendPlainInstr();
  InterBlockEdges DAG = makeDAG();

  DAG.markBoundary();
  DAG.addNode(Post);
  const SUnit *PostSU = DAG.getPostBoundaryNode(Post);
  ASSERT_NE(PostSU, nullptr);

  EXPECT_FALSE(DAG.hasPostHeight(PostSU));
  EXPECT_EQ(DAG.getPostHeightOr(PostSU, 99), 99);
  EXPECT_EQ(DAG.getPostRegionMaxHeight(), 0);

  DAG.recordPostHeight(Post, 2);
  EXPECT_TRUE(DAG.hasPostHeight(PostSU));
  EXPECT_EQ(DAG.getPostHeightOr(PostSU, 99), 2);
  EXPECT_EQ(DAG.getPostRegionMaxHeight(), 2);

  DAG.recordPostHeight(6);
  EXPECT_EQ(DAG.getPostHeightOr(PostSU, 99), 2);
  EXPECT_EQ(DAG.getPostRegionMaxHeight(), 6);

  DAG.recordPostHeight(Unknown, -1);
  EXPECT_EQ(DAG.getPostBoundaryNode(Unknown), nullptr);
  EXPECT_EQ(DAG.getPostRegionMaxHeight(), 6);

  DAG.clearPostHeights();
  EXPECT_FALSE(DAG.hasPostHeight(PostSU));
  EXPECT_EQ(DAG.getPostHeightOr(PostSU, 99), 99);
  EXPECT_EQ(DAG.getPostRegionMaxHeight(), 0);
}

TEST_F(InterBlockEdgesTest, ClearResetsDAGAndMaps) {
  auto *Pre = appendPlainInstr();
  auto *Post = appendPlainInstr();
  InterBlockEdges DAG = makeDAG();

  DAG.addNode(Pre);
  DAG.markBoundary();
  DAG.addNode(Post);
  DAG.recordPostDepth(Post, 4);
  DAG.recordPreDepth(Pre, -2);
  DAG.recordPreHeight(Pre, 3);
  DAG.recordPostHeight(Post, 5);

  DAG.clear();

  EXPECT_EQ(DAG.begin(), DAG.end());
  EXPECT_EQ(DAG.getPostRegionMaxDepth(), 0);
  EXPECT_EQ(DAG.getPreRegionMaxDepth(), 0);
  EXPECT_EQ(DAG.getPreRegionMaxHeight(), 0);
  EXPECT_EQ(DAG.getPostRegionMaxHeight(), 0);
}

TEST_F(InterBlockEdgesTest, ComputeMinEntryDepthFromKnownDepths) {
  auto *LoopD = appendPlainInstr();
  auto *Top0 = appendPlainInstr();
  auto *Top2 = appendPlainInstr();
  auto *OtherFree = appendPlainInstr();
  auto *Free = appendPlainInstr();
  InterBlockEdges DAG = makeDAG();

  DAG.addNode(LoopD);
  DAG.recordPreDepth(LoopD, 3);
  DAG.markBoundary();
  DAG.addNode(Top0);
  DAG.addNode(Top2);
  DAG.addNode(OtherFree);
  DAG.addNode(Free);
  DAG.recordPostDepth(Top0, 0);
  DAG.recordPostDepth(Top2, 2);

  SUnit *LoopDSU = const_cast<SUnit *>(DAG.getPreBoundaryNode(LoopD));
  SUnit *Top0SU = const_cast<SUnit *>(DAG.getPostBoundaryNode(Top0));
  SUnit *Top2SU = const_cast<SUnit *>(DAG.getPostBoundaryNode(Top2));
  SUnit *OtherFreeSU = const_cast<SUnit *>(DAG.getPostBoundaryNode(OtherFree));
  SUnit *FreeSU = const_cast<SUnit *>(DAG.getPostBoundaryNode(Free));
  ASSERT_NE(LoopDSU, nullptr);
  ASSERT_NE(Top0SU, nullptr);
  ASSERT_NE(Top2SU, nullptr);
  ASSERT_NE(OtherFreeSU, nullptr);
  ASSERT_NE(FreeSU, nullptr);

  // No preds: unconstrained, free to issue alongside the fixed region.
  EXPECT_EQ(computeMinEntryDepth(*FreeSU, DAG), 0);

  // A pred without a recorded depth carries no position information.
  SDep FromFree(OtherFreeSU, SDep::Artificial);
  FromFree.setLatency(10);
  FreeSU->addPred(FromFree, /*Required=*/true);
  EXPECT_EQ(computeMinEntryDepth(*FreeSU, DAG), 0);

  SDep FromTop0(Top0SU, SDep::Artificial);
  FromTop0.setLatency(1);
  FreeSU->addPred(FromTop0, /*Required=*/true);
  EXPECT_EQ(computeMinEntryDepth(*FreeSU, DAG), 1);

  SDep FromTop2(Top2SU, SDep::Artificial);
  FromTop2.setLatency(2);
  FreeSU->addPred(FromTop2, /*Required=*/true);
  EXPECT_EQ(computeMinEntryDepth(*FreeSU, DAG), 4);

  SDep FromLoop(LoopDSU, SDep::Artificial);
  FromLoop.setLatency(2);
  FreeSU->addPred(FromLoop, /*Required=*/true);
  EXPECT_EQ(computeMinEntryDepth(*FreeSU, DAG), 5);
}

} // namespace
