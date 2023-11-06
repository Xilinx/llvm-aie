//===-- AIEBaseSubtarget.cpp - AIE Base Subtarget Information -------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIE base subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "AIEBaseSubtarget.h"
#include "AIEMachineScheduler.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/CodeGen/ScheduleDAGInstrs.h"
#include "llvm/CodeGen/ScheduleDAGMutation.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

static cl::opt<bool> EnableStrongCopyEdges(
    "aie-strong-copy-edges", cl::Hidden, cl::init(true),
    cl::desc("Enforces edges between COPY sources and other users of those "
             "sources to limit live range overlaps"));

// These are debugging/testing options.

// aie-latency-margin defines the latency that will be given to ExitSU edges.
// If it is not set explicitly, it will be derived from the worst case latency
// of the instruction at the Src of the ExitSU edge.
static cl::opt<unsigned>
    UserLatencyMargin("aie-latency-margin", cl::Hidden, cl::init(0),
                      cl::desc("Define the latency on ExitSU edges"));

// Resetting aie-interblock-latency will use worst case instruction latencies
// on ExitSU edges. This reverts to the 'classical' safety margin for
// interblock scheduling
static cl::opt<bool>
    InterBlockLatency("aie-interblock-latency", cl::Hidden, cl::init(true),
                      cl::desc("Use interblock latencies on ExitSU edges"));

#define DEBUG_TYPE "aie-subtarget"

// Perform target-specific adjustments to the latency of a schedule
// dependency.
// If a pair of operands is associated with the schedule dependency, DefOpIdx
// and UseOpIdx are the indices of the operands in Def and Use, respectively.
// Otherwise, either may be -1.
//
// This is the shared implementation between all AIE targets.

namespace {

SDep &getInverseEdge(const SUnit &SrcSU, const SDep &E, bool Backward) {
  SDep ReversedDep = E;
  ReversedDep.setSUnit(const_cast<SUnit *>(&SrcSU));
  SmallVector<SDep, 4> &InverseEdges =
      Backward ? E.getSUnit()->Preds : E.getSUnit()->Succs;
  for (SDep &PredEdge : InverseEdges) {
    if (PredEdge == ReversedDep)
      return PredEdge;
  }
  llvm_unreachable("No corresponding edge for the other direction.");
}
SDep &getForwardEdge(const SUnit &SrcSU, const SDep &E) {
  return getInverseEdge(SrcSU, E, /*Backward=*/false);
}

} // namespace

// Reminder: this is called for ALL dependencies carried by physical registers,
// but only for DATA dependencies on virtual registers...
void AIEBaseSubtarget::adjustSchedDependency(
    const InstrItineraryData &Itineraries, SUnit *Def, int DefIdx, SUnit *Use,
    int UseIdx, SDep &Dep) const {

  // Ignore artificial and SDNode-based (SelectionDAG) dependencies.
  // Those are not needed for correctness.
  if (Dep.isArtificial() || !Def->isInstr() || !Use->isInstr()) {
    return;
  }

  // Note: Def is a misnomer, it is the source of the edge, but it isn't
  // necessarily a Def in case of WAW or WAR dependencies. Similar for Use.
  assert(DefIdx >= 0 && UseIdx >= 0);
  MachineInstr &SrcMI = *Def->getInstr();
  auto SrcOpIdx = unsigned(DefIdx);
  MachineInstr &DstMI = *Use->getInstr();
  auto DstOpIdx = unsigned(UseIdx);

  // We cannot use itineraries for implicit operands that were added
  // "dynamically", i.e. those that are not part of the static MCInstrDesc.
  auto NumStaticOps = [](const MCInstrDesc &D) -> unsigned {
    return D.NumOperands + D.NumImplicitDefs + D.NumImplicitUses;
  };
  if (SrcOpIdx >= NumStaticOps(SrcMI.getDesc()) ||
      DstOpIdx >= NumStaticOps(DstMI.getDesc())) {
    LLVM_DEBUG(llvm::dbgs()
               << "Warning!: No latency for dynamic op #" << SrcOpIdx << " in "
               << SrcMI << "Or op #" << DstOpIdx << " in " << DstMI << "\n");
    return;
  }

  const AIEBaseInstrInfo *TII = getInstrInfo();
  if (std::optional<int> Lat = TII->getSignedOperandLatency(
          &Itineraries, SrcMI, DefIdx, DstMI, UseIdx, Dep.getKind())) {
    Dep.setSignedLatency(*Lat);
  }
}

namespace {
// Compute the latency of this instruction. We take the maximum of the
// stages and the operand latencies.
int maxLatency(const MachineInstr *MI, const AIEBaseInstrInfo &InstrInfo,
               const InstrItineraryData &Itineraries) {
  int Latency = 0;
  unsigned SrcClass = MI->getDesc().getSchedClass();
  for (unsigned I = 0;; I++) {
    std::optional<unsigned> OpLat = Itineraries.getOperandCycle(SrcClass, I);
    if (OpLat) {
      Latency = std::max(Latency, int(*OpLat));
    } else {
      // Beyond last operand
      break;
    }
  }
  Latency =
      std::max(Latency, int(InstrInfo.getInstrLatency(&Itineraries, *MI)));
  return Latency;
}

// Set latency and declare height dirty if it changes
// return whether anything changed
bool updatePredLatency(SDep &Dep, const SUnit &SuccSU, int Latency) {
  if (Latency == Dep.getSignedLatency()) {
    return false;
  }

  // Change the dependence in both directions.
  getForwardEdge(SuccSU, Dep).setSignedLatency(Latency);
  Dep.setSignedLatency(Latency);
  Dep.getSUnit()->setHeightDirty();
  return true;
}

// Set the latency of ordering edges between memory operations and locks.
// The initial graph will have ordering edges induced by hasSideEffects of the
// locks
class LockDelays : public ScheduleDAGMutation {
  void apply(ScheduleDAGInstrs *DAG) override {
    // FIXME: There was mention of a delay of locks. I can't find that in the
    // ISA, but it sounds reasonable that it can't be taken before the semaphore
    // hardware has been accessed, which starts in E2.
    const int LockDelay = 2;
    const auto *InstrInfo = static_cast<const AIEBaseInstrInfo *>(DAG->TII);
    const auto *Itineraries = DAG->getSchedModel()->getInstrItineraries();
    for (auto &SU : DAG->SUnits) {
      if (!SU.isInstr()) {
        continue;
      }
      MachineInstr *Lock = SU.getInstr();
      if (!InstrInfo->isLock(Lock->getOpcode())) {
        continue;
      }
      bool UpdateDepth = false;
      for (auto &PredEdge : SU.Preds) {
        if (PredEdge.getKind() != SDep::Order) {
          continue;
        }
        MachineInstr *LdSt = PredEdge.getSUnit()->getInstr();
        if (!LdSt->mayLoad() && !LdSt->mayStore()) {
          // Note that scalar streams are kept away from locks using
          // a reserved FuncUnit instead.  See AIE2Schedule.td
          continue;
        }
        int Delay = maxLatency(LdSt, *InstrInfo, *Itineraries) - LockDelay;
        UpdateDepth |= updatePredLatency(PredEdge, SU, std::max(Delay, 0));
      }
      if (UpdateDepth) {
        SU.setDepthDirty();
      }
    }
  };
};

#undef DEBUG_TYPE
#define DEBUG_TYPE "machine-scheduler"

class MaxLatencyFinder {
  const AIEPostRASchedStrategy *const Scheduler;
  const AIEBaseInstrInfo *const TII;
  const InstrItineraryData *const Itineraries;
  const MCRegisterInfo *const TRI;
  MachineBasicBlock *const CurBB;
  const bool InterBlock;

  // Check whether this region connects to the successor blocks
  //
  bool isBottomRegion(MachineInstr *ExitMI) {
    if (!ExitMI) {
      // ExitMI is always the leading instruction of the next region.
      // If it is missing we are falling through to the next block.
      return true;
    }
    MachineBasicBlock::instr_iterator It(ExitMI);
    return std::next(It) == CurBB->end();
  }

  bool overlap(MachineOperand &SrcOp, MachineOperand &DstOp) const {
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
  bool depends(MachineInstr &Src, MachineInstr &Dst) const {
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
  int findEarliestRef(MachineInstr &SrcMI,
                      const std::vector<llvm::AIE::MachineBundle> &Bundles,
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

public:
  MaxLatencyFinder(AIEScheduleDAGMI *DAG)
      : Scheduler(DAG->getSchedImpl()),
        TII(static_cast<const AIEBaseInstrInfo *>(DAG->TII)),
        Itineraries(DAG->getSchedModel()->getInstrItineraries()),
        TRI(DAG->MF.getSubtarget().getRegisterInfo()),
        CurBB(Scheduler->getCurMBB()),
        InterBlock(InterBlockLatency && CurBB &&
                   isBottomRegion(DAG->ExitSU.getInstr()) &&
                   Scheduler->successorsAreScheduled(CurBB)) {}

  unsigned operator()(MachineInstr &MI) {
    int Latency = maxLatency(&MI, *TII, *Itineraries);
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
};

class RegionEndEdges : public ScheduleDAGMutation {
  void removeExitSUPreds(ScheduleDAGInstrs *DAG) {
    SUnit &ExitSU = DAG->ExitSU;
    while (!ExitSU.Preds.empty()) {
      ExitSU.removePred(ExitSU.Preds.back());
    }
  }
  void apply(ScheduleDAGInstrs *DAG) override {
    MaxLatencyFinder MaxLatency(static_cast<AIEScheduleDAGMI *>(DAG));

    // Default edges to ExitSU are conservative, and can't be shrunk.
    // We really should know what we're doing here, so just remove and
    // recompute all of them.
    removeExitSUPreds(DAG);

    const auto *TII = static_cast<const AIEBaseInstrInfo *>(DAG->TII);
    bool UserSetLatencyMargin = UserLatencyMargin.getNumOccurrences() > 0;
    for (MachineInstr &MI : *DAG) {
      SUnit *SU = DAG->getSUnit(&MI);
      if (!SU)
        continue;
      SDep ExitDep(SU, SDep::Artificial);

      unsigned DelaySlots = TII->getNumDelaySlots(MI);
      unsigned EdgeLatency = !DelaySlots && UserSetLatencyMargin
                                 ? UserLatencyMargin
                                 : MaxLatency(MI);
      // Extend the edge latency if MI requires delay slots. This makes sure
      // there are at least getNumDelaySlots() cycles between MI and ExitSU.
      if (DelaySlots) {
        assert(EdgeLatency < DelaySlots);
        EdgeLatency = DelaySlots + 1;
      }

      ExitDep.setLatency(EdgeLatency);
      DAG->ExitSU.addPred(ExitDep, /*Required=*/true);
    }

    // Note: the DAG does not use bi-directional edges, there are two distinct
    // edges for connecting a predecessor-successor pair.
    // The backward edge gets (Latency - 1) because we want instructions
    // to be able to issue in the same cycle as ExitSU (cycle #0 in bottom-up
    // scheduling).
    for (SDep &PredEdge : DAG->ExitSU.Preds) {
      if (!PredEdge.isArtificial())
        continue;
      unsigned BackwardLatency =
          PredEdge.getLatency() ? PredEdge.getLatency() - 1 : 0;
      PredEdge.setLatency(BackwardLatency);
    }
    DAG->ExitSU.setDepthDirty();
  };
};

/// Pre-RA MachineScheduler will add "weak" edges to try and limit the number of
/// COPY instructions that actually materialize into MOVs. Here we turn those
/// into "strong" edges to help with register pressure.
///
/// E.g. in
/// I0: %2:ep_as_32bit = COPY %0
/// I1: %2:ep_as_32bit = PADD_imm_pseudo %2, 64
/// I2: $wl0 = VLDA_dmw_lda_w_ag_idx_imm %0, 0
/// I3: $wh0 = VLDA_dmw_lda_w_ag_idx_imm %0, 32
///
/// We enforce the I2->I0 and I3->I0 edges, esssentially forcing the PADD to
/// come after the two users of the COPY source %0 and reducing register
/// pressure on pointers.
class EnforceCopyEdges : public ScheduleDAGMutation {
  void apply(ScheduleDAGInstrs *DAG) override {
    for (MachineInstr &MI : *DAG) {
      SUnit *SU = DAG->getSUnit(&MI);
      if (!SU || !MI.isCopy())
        continue;
      for (SDep &PredEdge : SU->Preds) {
        if (!PredEdge.isWeak())
          continue;
        // Note: We are now forcing the whole dependence tree of MI to come
        // after PredEdge.getSUnit(), this might increase the overall latency.
        // See indirect-copy-dep-incr-latency.mir
        if (DAG->canAddEdge(SU, PredEdge.getSUnit())) {
          SDep StrongPred(PredEdge.getSUnit(), SDep::Artificial);
          SU->addPred(StrongPred);
        }
      }
    }
  }
};

/// Fix memory dependencies. Based on their type, LLVM gives them a latency of
/// 0 or 1 cycle by default. This isn't always correct for AIE, so one needs to
/// fix the latencies to preserve the ordering.
/// E.g. in AIE2: VST.SRS stores in E7, while VLDA reads in E5.
class MemoryEdges : public ScheduleDAGMutation {
  void apply(ScheduleDAGInstrs *DAG) override {
    const auto *TII = static_cast<const AIEBaseInstrInfo *>(DAG->TII);

    // Run over all instructions that may load or store, and correct the
    // latencies for all their memory dependencies.
    for (SUnit &SU : DAG->SUnits) {
      MachineInstr &MI = *SU.getInstr();
      if (!MI.mayLoadOrStore()) {
        continue;
      }

      bool UpdateDepth = false;
      for (auto &PredEdge : SU.Preds) {
        MachineInstr &SrcMI = *PredEdge.getSUnit()->getInstr();

        // Ignore non-memory dependencies. Locks or other instructions with side
        // effects aren't handled with MemInstrItinData itineraries.
        if (!PredEdge.isNormalMemoryOrBarrier() || !SrcMI.mayLoadOrStore()) {
          continue;
        }

        // Ignore Load-Load (RAR) dependencies.
        // TODO: Those should probably be removed altogether.
        if (!SrcMI.mayStore() && !MI.mayStore()) {
          continue;
        }

        // Get the correct latency from the Sched model.
        std::optional<int> MemLat = TII->getMemoryLatency(
            SrcMI.getDesc().getSchedClass(), MI.getDesc().getSchedClass());
        if (!MemLat.has_value()) {
          LLVM_DEBUG(llvm::dbgs()
                     << "Error: no memory latency info for dependency\n  from: "
                     << SrcMI << "    to: " << MI);
          report_fatal_error("Missing memory latency info.");
        }
        UpdateDepth |= updatePredLatency(PredEdge, SU, *MemLat);
      }
      if (UpdateDepth) {
        SU.setDepthDirty();
      }
    }
  };
};

} // namespace

std::vector<std::unique_ptr<ScheduleDAGMutation>>
AIEBaseSubtarget::getPostRAMutationsImpl(const Triple &TT) {
  std::vector<std::unique_ptr<ScheduleDAGMutation>> Mutations;
  Mutations.emplace_back(std::make_unique<LockDelays>());
  if (!TT.isAIE1()) {
    Mutations.emplace_back(std::make_unique<RegionEndEdges>());
    Mutations.emplace_back(std::make_unique<MemoryEdges>());
  }
  return Mutations;
}

std::vector<std::unique_ptr<ScheduleDAGMutation>>
AIEBaseSubtarget::getPreRAMutationsImpl(const Triple &TT) {
  std::vector<std::unique_ptr<ScheduleDAGMutation>> Mutations;
  if (EnableStrongCopyEdges)
    Mutations.emplace_back(std::make_unique<EnforceCopyEdges>());
  return Mutations;
}
