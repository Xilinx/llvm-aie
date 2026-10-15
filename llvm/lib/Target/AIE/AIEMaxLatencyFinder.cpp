//===-- AIEMaxLatencyFinder.cpp - AIE interblock latency helpers ----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the interblock latency utilities
//
//===----------------------------------------------------------------------===//

#include "AIEMaxLatencyFinder.h"
#include "Utils/AIEMachineInstrPrint.h"

#undef DEBUG_TYPE
#define DEBUG_TYPE "sched-blocks"

namespace llvm::AIE {

// Resetting aie-interblock-latency will use worst case instruction latencies
// on ExitSU edges. This reverts to the 'classical' safety margin for
// interblock scheduling
static cl::opt<bool>
    InterBlockLatency("aie-interblock-latency", cl::Hidden, cl::init(true),
                      cl::desc("Use interblock latencies on ExitSU edges"));

// Compute the latency of this instruction. We take the maximum of the
// operand and the memory latency. Include the stage latency if requested.
int maxLatency(const MachineInstr *MI, const AIEBaseInstrInfo &InstrInfo,
               const InstrItineraryData &Itineraries, bool IncludeStages) {

  // If we have a Bundle, query maxLatency for each bundled instruction.
  if (MI->isBundle()) {
    int BundleLatency = 0;
    for (const MachineInstr &BundledMI : const_bundled_instrs(*MI)) {
      BundleLatency = std::max(
          maxLatency(&BundledMI, InstrInfo, Itineraries, IncludeStages),
          BundleLatency);
    }
    return BundleLatency;
  }

  int Latency = 0;
  unsigned SrcClass = MI->getDesc().getSchedClass();
  for (unsigned I = 0;; I++) {
    std::optional<unsigned> OpLat = Itineraries.getOperandCycle(SrcClass, I);
    if (!OpLat) {
      // Beyond last operand
      break;
    }
    Latency = std::max(Latency, int(*OpLat));
  }
  Latency = std::max(Latency, InstrInfo.getConservativeMemoryLatency(SrcClass));
  if (IncludeStages) {
    int StageLatency = InstrInfo.getInstrLatency(&Itineraries, *MI);
    Latency = std::max(Latency, StageLatency);
  }

  return Latency;
}

// Check whether this region connects to the successor blocks
//
bool MaxLatencyFinder::isBottomRegion(MachineInstr *ExitMI) {
  if (!ExitMI) {
    // ExitMI represents an instruction after this region. If it is
    // missing we are falling through to the next block, and hence we
    // are the bottom region.
    return true;
  }
  MachineBasicBlock::instr_iterator It(ExitMI);
  return std::next(It) == CurBB->end();
}

// This is called from different contexts, so we need some case analysis
// If we have a basic block, we are in a regular MachineScheduler invocation,
// and we will be able to retrieve its strategy,
// Otherwise we are an abstract region; Scheduler will be nullptr, which
// will not be dereferenced.
MaxLatencyFinder::MaxLatencyFinder(ScheduleDAGInstrs *DAG)
    : Scheduler(DAG->getBB()
                    ? static_cast<AIEScheduleDAGMI *>(DAG)->getSchedImpl()
                    : nullptr),
      TII(static_cast<const AIEBaseInstrInfo *>(DAG->TII)),
      Itineraries(DAG->getSchedModel()->getInstrItineraries()),
      TRI(DAG->MF.getSubtarget().getRegisterInfo()), CurBB(DAG->getBB()),
      IsBottomRegion(isBottomRegion(DAG->ExitSU.getInstr())),
      SuccessorsAreScheduled(IsBottomRegion && CurBB &&
                             Scheduler->successorsAreScheduled(CurBB)) {
  HasUnknownSuccessors = CurBB && CurBB->succ_empty();
  // PerSuccEdges graph was built by InterBlockScheduling::buildPerSuccEdges.
  // Update post-depths now that the scheduler may have produced bundles.
  if (CurBB && Scheduler && IsBottomRegion)
    Scheduler->getInterBlock().recordPostDepths(CurBB);
}

int MaxLatencyFinder::computeEffectiveLatency(MachineInstr &MI) {
  int EffectiveLatency = 0;
  int SuccNo = 0;
  const auto &PerSuccEdges =
      Scheduler->getInterBlock().getBlockState(CurBB).getPerSuccEdges();
  for (auto &SEPtr : PerSuccEdges) {
    InterBlockEdges &SE = *SEPtr;
    LLVM_DEBUG(
        dbgs() << "Processing InterBlockEdge: Pred="
               << (SE.getPred() ? SE.getPred()->getNumber() : -1) << " Succ="
               << (SE.getSucc() ? SE.getSucc()->getNumber() : -1) << "\n");
    LLVM_DEBUG(dbgs() << format("Successor %d PostRegionMaxDepth=%d\n", SuccNo,
                                SE.getPostDepths().getRegionMax()));
    const SUnit *Pred = SE.getPreBoundaryNode(&MI);
    if (!Pred) {
      LLVM_DEBUG(
          dbgs() << "   No pre-boundary node for this successor, skip\n");
      continue;
    }
    LLVM_DEBUG(dbgs() << "   Pre-boundary " << Pred->NodeNum << " has "
                      << Pred->Succs.size() << " successor edge(s)\n");

    for (const SDep &Dep : Pred->Succs) {
      SUnit *Succ = Dep.getSUnit();
      int Depth;
      if (SE.isPreBoundaryNode(Succ)) {
        // A pre-boundary node with a depth is an SWP prologue clone, fixed
        // before the end of this block. Other pre-boundary nodes are free
        // instructions of this region, ordered by the region DAG.
        std::optional<int> PreDepth = SE.getPreDepths().getValue(Succ);
        if (!PreDepth) {
          LLVM_DEBUG(dbgs() << "   SU" << Succ->NodeNum
                            << " is a free pre-boundary node, skip\n");
          continue;
        }
        Depth = *PreDepth;
      } else {
        // For ExitSU the depth is the full length of the successor block's
        // top region (all its cycles have elapsed before reaching ExitSU).
        // For a regular instruction node the depth is its scheduled cycle
        // within the block.
        Depth = Succ->isBoundaryNode() ? SE.getPostDepths().getRegionMax() + 1
                                       : SE.getPostDepths().getValueOr(Succ, 0);
      }
      const int EdgeLat = Dep.getSignedLatency();
      const int Remaining = EdgeLat - Depth;
      LLVM_DEBUG(
          dbgs() << "   " << (Succ->isBoundaryNode() ? "ExitSU" : "SU")
                 << (Succ->isBoundaryNode() ? ""
                                            : std::to_string(Succ->NodeNum))
                 << ": latency=" << EdgeLat << ", depth=" << Depth
                 << ", remaining=" << Remaining
                 << ", updating EffectiveLatency " << EffectiveLatency << " -> "
                 << std::max(EffectiveLatency, Remaining) << "\n");
      EffectiveLatency = std::max(EffectiveLatency, Remaining);
    }
    SuccNo++;
  }
  return EffectiveLatency;
}

unsigned MaxLatencyFinder::operator()(MachineInstr &MI) {
  LLVM_DEBUG(dbgs() << NoDebug(MI) << "\n");
  // If we don't use interblock information, include the 'StageLatency'
  // in maxLatency. This influences the height parameters, telling the
  // scheduler to prefer deep-pipeline instructions over shorter ones.
  int Latency = maxLatency(&MI, *TII, *Itineraries, !SuccessorsAreScheduled);
  LLVM_DEBUG(dbgs() << "MaxLatency=" << Latency << "\n");
  if (!CurBB) {
    // This indicates we are called from an abstract block, e.g.
    // InterBlockEdges. We don't know successors here.
    return Latency;
  }

  if (HasUnknownSuccessors) {
    LLVM_DEBUG(dbgs() << "Unkown successors MaxLatency=" << Latency << "\n");
    return Latency;
  }

  const AIE::InterBlockScheduling &IB = Scheduler->getInterBlock();
  // Loop-aware convergence: gradually tighten the ExitSU latency from zero
  // toward the real value.
  if (IB.getBlockState(CurBB).Kind == BlockType::Loop && IsBottomRegion) {
    if (auto Cap = IB.getLatencyCap(MI)) {
      LLVM_DEBUG(dbgs() << "Capped at " << *Cap << "\n");
      return std::min(Latency, *Cap);
    }
  }

  if (IsBottomRegion) {
    // A free MI may feed an SWP prologue instruction at the bottom of this
    // block. There is no DAG edge between them, so this ExitSU latency is
    // what keeps MI early enough. It can exceed MI's worst-case latency, so
    // keep it even when interblock latency is disabled.
    int EffectiveLatency = computeEffectiveLatency(MI);
    Latency = InterBlockLatency ? std::max(0, EffectiveLatency)
                                : std::max(Latency, EffectiveLatency);
    LLVM_DEBUG(dbgs() << "   EffectiveLatency=" << EffectiveLatency << "\n");
  }

  return Latency;
}

int computeMinEntryLatency(const SUnit &EdgeSU, const InterBlockEdges &Edges) {
  int MaxDepth = 0;
  for (const SDep &PredEdge : EdgeSU.Preds) {
    const SUnit *PredSU = PredEdge.getSUnit();
    const InterBlockEdges::NodeValues &Depths = Edges.isPreBoundaryNode(PredSU)
                                                    ? Edges.getPreDepths()
                                                    : Edges.getPostDepths();
    const std::optional<int> PredDepth = Depths.getValue(PredSU);
    MaxDepth = std::max(
        MaxDepth, PredDepth ? *PredDepth + PredEdge.getSignedLatency() : 0);
  }
  return MaxDepth;
}

} // namespace llvm::AIE
