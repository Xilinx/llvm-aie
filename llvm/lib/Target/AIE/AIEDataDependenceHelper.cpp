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

void InterBlockEdges::NodeValues::clear() {
  Values.clear();
  RegionMax = 0;
}

void InterBlockEdges::NodeValues::record(MachineInstr *MI, int Val) {
  RegionMax = std::max(RegionMax, Val);
  const auto Found = Nodes.find(MI);
  if (Found == Nodes.end())
    return;
  const unsigned Idx = Found->second;
  if (Values.size() <= Idx)
    Values.resize(Idx + 1);
  Values[Idx] = Val;
}

std::optional<int>
InterBlockEdges::NodeValues::getValue(const SUnit *SU) const {
  if (SU->NodeNum >= Values.size())
    return std::nullopt;
  return Values[SU->NodeNum];
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

void InterBlockEdges::markFixedInstrBoundary() {
  assert(!FixedInstrBoundary && "Fixed/free boundary already set");
  assert(!Boundary &&
         "Fixed/free boundary must be marked before the CFG boundary");
  FixedInstrBoundary = SUnits.size();
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

bool InterBlockEdges::isFixedPreBoundaryNode(const SUnit *SU) const {
  return FixedInstrBoundary && SU->NodeNum >= *FixedInstrBoundary &&
         isPreBoundaryNode(SU);
}

bool InterBlockEdges::isPostBoundaryNode(const SUnit *SU) const {
  return Boundary ? SU->NodeNum >= *Boundary : false;
}

void InterBlockEdges::clear() {
  clearDAG();
  Boundary = {};
  // updatePerSuccEdges rebuilds this object in place. addNode uses emplace, so
  // a stale index would keep pointing at whatever instruction now occupies it.
  PredMap.clear();
  SuccMap.clear();
  PostDepths.clear();
  PreDepths.clear();
  FixedInstrBoundary.reset();
}

std::map<unsigned, int> InterBlockEdges::computePreHeights() const {
  std::map<unsigned, int> PreHeights;
  if (!Boundary)
    return PreHeights;

  auto HeightOf = [&PreHeights](unsigned NodeNum) {
    const auto It = PreHeights.find(NodeNum);
    return It != PreHeights.end() ? It->second : 0;
  };

  // NodeNum is topological, so a reverse walk settles every successor before
  // the node that depends on it.
  for (const SUnit &SU : reverse(SUnits)) {
    if (SU.NodeNum >= *Boundary)
      continue;
    int Height = 0;
    for (const SDep &Dep : SU.Succs) {
      // Cut the chain at the boundary. ExitSU is covered as well, its NodeNum
      // being larger than any real node's.
      const unsigned SuccNum = Dep.getSUnit()->NodeNum;
      if (SuccNum >= *Boundary)
        continue;
      Height = std::max(Height, Dep.getSignedLatency() + HeightOf(SuccNum));
    }
    if (Height > 0)
      PreHeights[SU.NodeNum] = Height;
  }
  return PreHeights;
}

} // end namespace llvm::AIE
