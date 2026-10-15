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

  static void addDep(SUnit &Pred, SUnit &Succ, int Latency) {
    SDep Dep(&Pred, SDep::Artificial);
    Dep.setLatency(Latency);
    Succ.addPred(Dep, /*Required=*/true);
  }

  /// Latency adds into the successor's depth and the predecessor's height.
  /// This holds for every edge, on either side of the boundary and across it.
  static void expectDep(const SUnit &Pred, const SUnit &Succ,
                        unsigned Latency) {
    EXPECT_GE(Succ.getDepth(), Pred.getDepth() + Latency);
    EXPECT_GE(Pred.getHeight(), Succ.getHeight() + Latency);
  }
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

TEST_F(InterBlockEdgesTest, ComputeMinEntryLatencyFromKnownDepths) {
  auto *LoopD = appendPlainInstr();
  auto *Top0 = appendPlainInstr();
  auto *Top2 = appendPlainInstr();
  auto *OtherFree = appendPlainInstr();
  auto *Free = appendPlainInstr();
  InterBlockEdges DAG = makeDAG();

  DAG.addNode(LoopD);
  DAG.getPreDepths().record(LoopD, 3);
  DAG.markBoundary();
  DAG.addNode(Top0);
  DAG.addNode(Top2);
  DAG.addNode(OtherFree);
  DAG.addNode(Free);
  DAG.getPostDepths().record(Top0, 0);
  DAG.getPostDepths().record(Top2, 2);

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
  EXPECT_EQ(computeMinEntryLatency(*FreeSU, DAG), 0);

  // A pred without a recorded depth carries no position information.
  addDep(*OtherFreeSU, *FreeSU, 10);
  EXPECT_EQ(computeMinEntryLatency(*FreeSU, DAG), 0);

  addDep(*Top0SU, *FreeSU, 1);
  EXPECT_EQ(computeMinEntryLatency(*FreeSU, DAG), 1);

  addDep(*Top2SU, *FreeSU, 2);
  EXPECT_EQ(computeMinEntryLatency(*FreeSU, DAG), 4);

  addDep(*LoopDSU, *FreeSU, 2);
  EXPECT_EQ(computeMinEntryLatency(*FreeSU, DAG), 5);

  expectDep(*OtherFreeSU, *FreeSU, 10);
  expectDep(*Top0SU, *FreeSU, 1);
  expectDep(*Top2SU, *FreeSU, 2);
  expectDep(*LoopDSU, *FreeSU, 2);
}

TEST_F(InterBlockEdgesTest, ComputeMinEntryLatencyFromEpilogueLoopPreBoundary) {
  // Loop bundles on the loop-to-latch edge are recorded at Depth = I - L;
  // the last bundle is -1 (one cycle before epilogue cycle 0).
  auto *LoopEarlier = appendPlainInstr();
  auto *LoopLast = appendPlainInstr();
  auto *Free = appendPlainInstr();
  InterBlockEdges DAG = makeDAG();

  DAG.addNode(LoopEarlier);
  DAG.getPreDepths().record(LoopEarlier, -2);
  DAG.addNode(LoopLast);
  DAG.getPreDepths().record(LoopLast, -1);
  DAG.markBoundary();
  DAG.addNode(Free);

  SUnit *EarlierSU = const_cast<SUnit *>(DAG.getPreBoundaryNode(LoopEarlier));
  SUnit *LastSU = const_cast<SUnit *>(DAG.getPreBoundaryNode(LoopLast));
  SUnit *FreeSU = const_cast<SUnit *>(DAG.getPostBoundaryNode(Free));
  ASSERT_NE(EarlierSU, nullptr);
  ASSERT_NE(LastSU, nullptr);
  ASSERT_NE(FreeSU, nullptr);

  // Latency 1 from depth -1 does not push past EntrySU cycle 0.
  addDep(*LastSU, *FreeSU, 1);
  EXPECT_EQ(computeMinEntryLatency(*FreeSU, DAG), 0);

  addDep(*LastSU, *FreeSU, 4);
  EXPECT_EQ(computeMinEntryLatency(*FreeSU, DAG), 3);

  addDep(*EarlierSU, *FreeSU, 6);
  EXPECT_EQ(computeMinEntryLatency(*FreeSU, DAG), 4);

  expectDep(*LastSU, *FreeSU, 4);
  expectDep(*EarlierSU, *FreeSU, 6);
}

} // namespace
