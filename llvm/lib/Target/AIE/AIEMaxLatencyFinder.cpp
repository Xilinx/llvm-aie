//===-- AIEMaxLatencyFinder.cpp - AIE interblock latency helpers ----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the interblock latency utilities
//
//===----------------------------------------------------------------------===//

#include "AIEMaxLatencyFinder.h"

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

/// Check whether SrcOp and DstOp might refer to the same value
static bool overlap(const MachineOperand &SrcOp, const MachineOperand &DstOp,
                    const TargetRegisterInfo *TRI) {
  Register SrcReg = SrcOp.getReg();
  Register DstReg = DstOp.getReg();
  for (MCRegAliasIterator Ali(SrcReg, TRI, true); Ali.isValid(); ++Ali) {
    if (*Ali == DstReg.asMCReg()) {
      return true;
    }
  }
  return false;
}

/// Check whether Dst depends on Src
static bool depends(const MachineInstr &Src, const MachineInstr &Dst,
                    const TargetRegisterInfo *TRI) {
  // We don't try anything clever in terms of alias analysis
  // The memory latency is accounted for by maxLatency() and any
  // possible dependence will be corrected for by its scheduled cycle.
  // (RAW || WAW) ||
  // (WAR)
  if ((Src.mayStore() && (Dst.mayLoad() || Dst.mayStore())) ||
      (Src.mayLoad() && Dst.mayStore())) {
    return true;
  }

  // We detect any common register input/output between Dst and Src
  for (auto &SrcOp : Src.operands()) {
    if (!SrcOp.isReg()) {
      continue;
    }
    for (auto &DstOp : Dst.operands()) {
      if (!DstOp.isReg()) {
        continue;
      }
      // Exclude the RAR case
      if (SrcOp.isUse() && DstOp.isUse()) {
        continue;
      }
      if (overlap(SrcOp, DstOp, TRI)) {
        return true;
      }
    }
  }

  return false;
}

InstrAndCycle findEarliestRef(const MachineInstr &SrcMI,
                              ArrayRef<MachineBundle> Bundles, int Prune) {
  const TargetRegisterInfo *TRI =
      SrcMI.getMF()->getSubtarget().getRegisterInfo();

  int Cycle = 0;
  for (const auto &Bundle : Bundles) {
    if (Cycle >= Prune) {
      LLVM_DEBUG(dbgs() << " prune at " << Cycle << "\n");
      return {/*MI=*/nullptr, Cycle};
    }
    for (MachineInstr *DstMI : Bundle.getInstrs()) {
      LLVM_DEBUG(dbgs() << "  " << *DstMI);
      if (depends(SrcMI, *DstMI, TRI)) {
        LLVM_DEBUG(dbgs() << "    depends in cycle=" << Cycle << "\n");
        return {DstMI, Cycle};
      }
    }
    Cycle++;
  }
  return {/*MI=*/nullptr, Cycle};
}

MaxLatencyFinder::MaxLatencyFinder(
    const AIEPostRASchedStrategy *const Scheduler,
    const AIEBaseInstrInfo *const TII,
    const InstrItineraryData *const Itineraries,
    const MCRegisterInfo *const TRI, MachineBasicBlock *const CurBB)
    : Scheduler(Scheduler), TII(TII), Itineraries(Itineraries), TRI(TRI),
      CurBB(CurBB), InterBlock(true) {}

// This is called from different contexts, so we need some case analysis
// If we have a basic block, we are in a regular MachineScheduler invocation,
// and we will be able to retrieve its strategy,
// Otherwise we are an abstract region; Scheduler will be nullptr, which
// will not be derefenced.
MaxLatencyFinder::MaxLatencyFinder(ScheduleDAGInstrs *DAG)
    : Scheduler(DAG->getBB()
                    ? static_cast<AIEScheduleDAGMI *>(DAG)->getSchedImpl()
                    : nullptr),
      TII(static_cast<const AIEBaseInstrInfo *>(DAG->TII)),
      Itineraries(DAG->getSchedModel()->getInstrItineraries()),
      TRI(DAG->MF.getSubtarget().getRegisterInfo()), CurBB(DAG->getBB()),
      InterBlock(InterBlockLatency && CurBB &&
                 isBottomRegion(DAG->ExitSU.getInstr()) &&
                 Scheduler->successorsAreScheduled(CurBB)) {}

unsigned MaxLatencyFinder::operator()(MachineInstr &MI) {
  LLVM_DEBUG(dbgs() << MI << "\n");
  // If we don't use interblock information, include the 'StageLatency'
  // in maxLatency. This influences the height parameters, telling the
  // scheduler to prefer deep-pipeline instructions over shorter ones.
  int Latency = maxLatency(&MI, *TII, *Itineraries, !InterBlock);
  LLVM_DEBUG(dbgs() << "MaxLatency=" << Latency << "\n");
  if (!CurBB) {
    // This indicates we are called from an abstract block, e.g.
    // InterBlockEdges. We don't know successors here.
    return Latency;
  }

  // We have two distinct modes here. For interblock, we have perfect
  // information, and we try to reduce the latency by detailed analysis of
  // the successor schedules.
  // For non interblock, we may be given an optimistic cap on the latency
  // which will gradually increase during convergence of iteratively
  // scheduling a loop.
  const AIE::InterBlockScheduling &IB = Scheduler->getInterBlock();
  if (!InterBlock) {
    if (auto Cap = IB.getLatencyCap(MI)) {
      LLVM_DEBUG(dbgs() << "Capped at " << *Cap << "\n");
      Latency = std::min(Latency, *Cap);
    }
    return Latency;
  }
  LLVM_DEBUG(dbgs() << "Earliest for: " << MI);
  // Track the earliest use in any successor block, given the cycles in
  // which these uses are scheduled
  int Earliest = Latency;
  for (MachineBasicBlock *SuccBB : CurBB->successors()) {
    auto &SBS = IB.getBlockState(SuccBB);
    assert(SBS.isScheduled());
    if (SBS.getRegions().empty()) {
      // Blocks can be empty. getTop() will fail, and Earliest=0 is
      // a conservative value
      Earliest = 0;
      continue;
    }
    const std::vector<AIE::MachineBundle> &TopBundles = SBS.getTop().Bundles;
    Earliest = findEarliestRef(MI, TopBundles, Earliest).Cycle;
  }

  LLVM_DEBUG(dbgs() << "   Earliest=" << Earliest << "\n");
  Latency = std::max(Latency - Earliest, 1);
  LLVM_DEBUG(dbgs() << "EffectiveLatency=" << Latency << "\n");
  return Latency;
}

} // namespace llvm::AIE
