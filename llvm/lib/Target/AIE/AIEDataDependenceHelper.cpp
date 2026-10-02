//===- AIEDataDependenceHelper.cpp - Inter-block DAG --------------------- ===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEDataDependenceHelper.h"
#include "AIEBaseSubtarget.h"
#include "llvm/CodeGen/ScheduleDAGInstrs.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>

namespace llvm::AIE {

// This option facilitates experimenting and unit tests
static cl::opt<bool>
    MemDeps("aie-dep-helper-memdeps", cl::Hidden, cl::init(true),
            cl::desc("Allow memory dependences in DataDependenceHelper "));

DataDependenceHelper::DataDependenceHelper(const MachineSchedContext &Context,
                                           bool AddMutators,
                                           bool ExactLatencies)
    : ScheduleDAGInstrs(*Context.MF, Context.MLI), Context(Context) {
  if (!AddMutators)
    return;

  auto &Subtarget = Context.MF->getSubtarget();
  auto TT = Subtarget.getTargetTriple();
  for (auto &M :
       AIEBaseSubtarget::getDDGMutationsImpl(TT, ExactLatencies, Context.AA)) {
    Mutations.emplace_back(std::move(M));
  }
}

void DataDependenceHelper::buildEdges() {
  ScheduleDAGInstrs::buildEdges(Context.AA);
  for (auto &M : Mutations) {
    M->apply(this);
  }
}

bool DataDependenceHelper::mayAlias(SUnit *SUa, SUnit *SUb, bool TBAA) {
  if (!MemDeps) {
    return false;
  }
  return ScheduleDAGInstrs::mayAlias(SUa, SUb, TBAA);
}

int DataDependenceHelper::maxDepth() const {
  int MaxDepth = 0;
  for (auto &SU : SUnits) {
    MaxDepth = std::max(MaxDepth, int(SU.getDepth()));
  }
  return MaxDepth;
}

void DataDependenceHelper::dumpDot(raw_ostream &OS,
                                   bool IncludeBoundaries) const {
  OS << "digraph {\n";
  for (auto &SU : SUnits) {
    OS << format("N%d\n", SU.NodeNum);
    for (auto &Dep : SU.Succs) {
      if (!IncludeBoundaries && Dep.getSUnit()->isBoundaryNode()) {
        continue;
      }
      OS << format("N%d -> N%d [label=\"%d\"]\n", SU.NodeNum,
                   Dep.getSUnit()->NodeNum, Dep.getSignedLatency());
    }
  }
  OS << "}\n";
}

void InterBlockEdges::addNode(MachineInstr *MI) {
  const auto Index = initSUnit(*MI);
  if (!Index) {
    return;
  }

  IndexMap &TheMap = Boundary ? SuccMap : PredMap;
  TheMap.emplace(MI, *Index);
}

void InterBlockEdges::markBoundary() {
  assert(!Boundary.has_value());
  Boundary = SUnits.size();
}

bool InterBlockEdges::mayAlias(SUnit *SUa, SUnit *SUb, bool TBAA) {
  if (SafeToIgnoreMemDeps && Boundary) {
    // Suppress memory edges that cross the pre/post boundary.
    const bool AIsPost = SUa->NodeNum >= *Boundary;
    const bool BIsPost = SUb->NodeNum >= *Boundary;
    if (AIsPost != BIsPost)
      return false;
  }
  return DataDependenceHelper::mayAlias(SUa, SUb, TBAA);
}

const SUnit *InterBlockEdges::getPreBoundaryNode(MachineInstr *MI) const {
  const auto Found = PredMap.find(MI);
  if (Found == PredMap.end()) {
    return nullptr;
  }
  return &SUnits.at(Found->second);
}

const SUnit *InterBlockEdges::getPostBoundaryNode(MachineInstr *MI) const {
  const auto Found = SuccMap.find(MI);
  if (Found == SuccMap.end()) {
    return nullptr;
  }
  return &SUnits.at(Found->second);
}

bool InterBlockEdges::isPreBoundaryNode(const SUnit *SU) const {
  return Boundary ? SU->NodeNum < *Boundary : true;
}

bool InterBlockEdges::isPostBoundaryNode(const SUnit *SU) const {
  return Boundary ? SU->NodeNum >= *Boundary : false;
}

void InterBlockEdges::clearPostDepths() {
  PostDepths.clear();
  PostRegionMaxDepth = 0;
}

void InterBlockEdges::clearPreDepths() {
  PreDepths.clear();
  PreRegionMaxDepth = 0;
}

void InterBlockEdges::clearPreHeights() {
  PreHeights.clear();
  PreRegionMaxHeight = 0;
}

void InterBlockEdges::clearPostHeights() {
  PostHeights.clear();
  PostRegionMaxHeight = 0;
}

void InterBlockEdges::clear() {
  clearDAG();
  Boundary = {};
  clearPostDepths();
  clearPreDepths();
  clearPreHeights();
  clearPostHeights();
}

void InterBlockEdges::recordValue(const IndexMap &IMap,
                                  std::map<unsigned, int> &Values, int &MaxVal,
                                  MachineInstr *MI, int Val) {
  // Even if we can't find an index, our caller has found evidence that we
  // have code at this cycle. As a common example, consider bundles with
  // BottomFixed instructions that are not part of the first iteration.
  MaxVal = std::max(MaxVal, Val);
  const auto Found = IMap.find(MI);
  if (Found == IMap.end()) {
    // When inserting scheduled bundles, we may be interleaved with
    // instructions from Fixed regions. It's easiest to ignore them here.
    return;
  }
  Values[Found->second] = Val;
}

static int getValueOr(const std::map<unsigned, int> &Values, const SUnit *SU,
                      int Default) {
  const auto It = Values.find(SU->NodeNum);
  return It != Values.end() ? It->second : Default;
}

static bool hasValue(const std::map<unsigned, int> &Values, const SUnit *SU) {
  return Values.count(SU->NodeNum);
}

// Record a cycle that is not represented by a MachineInstr, for instance an
// empty bundle.
void InterBlockEdges::recordPostDepth(int Depth) {
  PostRegionMaxDepth = std::max(PostRegionMaxDepth, Depth);
}

void InterBlockEdges::recordPostDepth(MachineInstr *MI, int Depth) {
  recordValue(SuccMap, PostDepths, PostRegionMaxDepth, MI, Depth);
}

int InterBlockEdges::getPostDepthOr(const SUnit *SU, int Default) const {
  return getValueOr(PostDepths, SU, Default);
}

bool InterBlockEdges::hasPostDepth(const SUnit *SU) const {
  return hasValue(PostDepths, SU);
}

void InterBlockEdges::recordPreDepth(int Depth) {
  PreRegionMaxDepth = std::max(PreRegionMaxDepth, Depth);
}

void InterBlockEdges::recordPreDepth(MachineInstr *MI, int Depth) {
  recordValue(PredMap, PreDepths, PreRegionMaxDepth, MI, Depth);
}

int InterBlockEdges::getPreDepthOr(const SUnit *SU, int Default) const {
  return getValueOr(PreDepths, SU, Default);
}

bool InterBlockEdges::hasPreDepth(const SUnit *SU) const {
  return hasValue(PreDepths, SU);
}

void InterBlockEdges::recordPreHeight(int Height) {
  PreRegionMaxHeight = std::max(PreRegionMaxHeight, Height);
}

void InterBlockEdges::recordPreHeight(MachineInstr *MI, int Height) {
  recordValue(PredMap, PreHeights, PreRegionMaxHeight, MI, Height);
}

int InterBlockEdges::getPreHeightOr(const SUnit *SU, int Default) const {
  return getValueOr(PreHeights, SU, Default);
}

bool InterBlockEdges::hasPreHeight(const SUnit *SU) const {
  return hasValue(PreHeights, SU);
}

void InterBlockEdges::recordPostHeight(int Height) {
  PostRegionMaxHeight = std::max(PostRegionMaxHeight, Height);
}

void InterBlockEdges::recordPostHeight(MachineInstr *MI, int Height) {
  recordValue(SuccMap, PostHeights, PostRegionMaxHeight, MI, Height);
}

int InterBlockEdges::getPostHeightOr(const SUnit *SU, int Default) const {
  return getValueOr(PostHeights, SU, Default);
}

bool InterBlockEdges::hasPostHeight(const SUnit *SU) const {
  return hasValue(PostHeights, SU);
}

} // end namespace llvm::AIE
