//===-- AIEMaxLatencyFinder.cpp - AIE interblock latency helpers ----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//===----------------------------------------------------------------------===//
//
// This file implements the interblock latency utilities
//
//===----------------------------------------------------------------------===//

#include "AIEMaxLatencyFinder.h"

#undef DEBUG_TYPE
#define DEBUG_TYPE "machine-scheduler"

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

bool MaxLatencyFinder::overlap(MachineOperand &SrcOp,
                               MachineOperand &DstOp) const {
  Register SrcReg = SrcOp.getReg();
  Register DstReg = DstOp.getReg();
  for (MCRegAliasIterator Ali(SrcReg, TRI, true); Ali.isValid(); ++Ali) {
    if (*Ali == DstReg.asMCReg()) {
      return true;
    }
  }
  return false;
}

// Check whether Dst depends on Src
bool MaxLatencyFinder::depends(MachineInstr &Src, MachineInstr &Dst) const {
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
      if (overlap(SrcOp, DstOp)) {
        return true;
      }
    }
  }

  return false;
}

// Find the first dependence on SrcMI in Bundles or Prune,
// whichever is less
int MaxLatencyFinder::findEarliestRef(
    MachineInstr &SrcMI, const std::vector<llvm::AIE::MachineBundle> &Bundles,
    int Prune) {
  int Cycle = 0;
  for (const auto &Bundle : Bundles) {
    if (Cycle >= Prune) {
      return Cycle;
    }
    for (MachineInstr *DstMI : Bundle.getInstrs()) {
      if (depends(SrcMI, *DstMI)) {
        LLVM_DEBUG(dbgs() << *DstMI << " depends in cycle=" << Cycle << "\n");
        return Cycle;
      }
    }
    Cycle++;
  }
  return Cycle;
}

MaxLatencyFinder::MaxLatencyFinder(AIEScheduleDAGMI *DAG)
    : Scheduler(DAG->getSchedImpl()),
      TII(static_cast<const AIEBaseInstrInfo *>(DAG->TII)),
      Itineraries(DAG->getSchedModel()->getInstrItineraries()),
      TRI(DAG->MF.getSubtarget().getRegisterInfo()),
      CurBB(Scheduler->getCurMBB()),
      InterBlock(InterBlockLatency && CurBB &&
                 isBottomRegion(DAG->ExitSU.getInstr()) &&
                 Scheduler->successorsAreScheduled(CurBB)) {}

unsigned MaxLatencyFinder::operator()(MachineInstr &MI) {
  // If we don't use interblock information, include the 'StageLatency'
  // in maxLatency. This influences the height parameters, telling the
  // scheduler to prefer deep-pipeline instructions over shorter ones.
  const bool IncludeStages = !InterBlock;
  int Latency = maxLatency(&MI, *TII, *Itineraries, IncludeStages);
  if (!InterBlock) {
    return Latency;
  }
  LLVM_DEBUG(dbgs() << MI << "Earliest for " << MI);
  // Track the earliest use in any successor block, given the cycles in
  // which these uses are scheduled
  int Earliest = Latency;
  for (MachineBasicBlock *SuccBB : CurBB->successors()) {
    const AIEPostRASchedStrategy::Region &TopBundles =
        Scheduler->getBlockState(SuccBB).getTop();
    Earliest = std::min(Earliest, findEarliestRef(MI, TopBundles, Earliest));
  }
  LLVM_DEBUG(dbgs() << "   Earliest=" << Earliest << "\n");
  return std::max(Latency - Earliest, 1);
}

} // namespace llvm::AIE
