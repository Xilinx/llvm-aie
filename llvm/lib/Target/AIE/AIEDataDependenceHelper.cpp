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

bool InterBlockEdges::isPostBoundaryNode(SUnit *SU) const {
  return Boundary ? SU->NodeNum >= *Boundary : false;
}

void InterBlockEdges::clearPostDepths() {
  PostDepths.clear();
  PostRegionMaxDepth = 0;
}

void InterBlockEdges::clear() {
  clearDAG();
  Boundary = {};
  clearPostDepths();
}

// Record code at a particular depth that is not represented
// by a MachineInstr, for instance an empty bundle.
void InterBlockEdges::recordPostDepth(int Depth) {
  PostRegionMaxDepth = std::max(PostRegionMaxDepth, Depth);
}

void InterBlockEdges::recordPostDepth(MachineInstr *MI, int Depth) {
  // Even if we can't find an index, our caller has found evidence
  // that we have code at this depth. As a common example,
  // consider bundles with BottomFixed instructions that are not
  // part of the first iteration.
  recordPostDepth(Depth);

  const auto Found = SuccMap.find(MI);
  if (Found == SuccMap.end()) {
    // When inserting scheduled bundles, we may be interleaved with
    // instructions from Fixed regions. It's easiest to ignore them
    // here.
    return;
  }
  PostDepths[Found->second] = Depth;
}

int InterBlockEdges::getPostDepthOr(const SUnit *SU, int Default) const {
  const auto It = PostDepths.find(SU->NodeNum);
  return It != PostDepths.end() ? It->second : Default;
}

} // end namespace llvm::AIE
