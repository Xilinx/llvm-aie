//===- AIEDataDependenceHelper.cpp - Inter-block DAG --------------------- ===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2025 Advanced Micro Devices, Inc. or its affiliates
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
                                           bool AddMutators)
    : ScheduleDAGInstrs(*Context.MF, Context.MLI), Context(Context) {
  if (!AddMutators)
    return;

  auto &Subtarget = Context.MF->getSubtarget();
  auto TT = Subtarget.getTargetTriple();
  for (auto &M : AIEBaseSubtarget::getInterBlockMutationsImpl(TT)) {
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

} // end namespace llvm::AIE
