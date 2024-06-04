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
#include "AIEBaseRegisterInfo.h"
#include "AIEMachineScheduler.h"
#include "AIEMaxLatencyFinder.h"
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
static cl::opt<bool> EnablePreMISchedPropagateIncomingLatencies(
    "aie-premisched-propagate-incoming-latencies", cl::Hidden, cl::init(false),
    cl::desc(
        "Move input latency of copy-like instructions to their successors"));
static cl::opt<unsigned> RegPressureInstrPreMISchedThreshold(
    "aie-premisched-reg-pressure-instr-threshold", cl::Hidden, cl::init(0),
    cl::desc("Number of region instructions below which premisched should not "
             "track register pressure"));
static cl::opt<bool> EnablePipelinerSchedPropagateIncomingLatencies(
    "aie-pipeliner-propagate-incoming-latencies", cl::Hidden, cl::init(true),
    cl::desc(
        "Move input latency of copy-like instructions to their successors"));

// These are debugging/testing options.

// aie-latency-margin defines the latency that will be given to ExitSU edges.
// If it is not set explicitly, it will be derived from the worst case latency
// of the instruction at the Src of the ExitSU edge.
static cl::opt<unsigned>
    UserLatencyMargin("aie-latency-margin", cl::Hidden, cl::init(0),
                      cl::desc("Define the latency on ExitSU edges"));

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
SDep &getBackwardEdge(const SUnit &SrcSU, const SDep &E) {
  return getInverseEdge(SrcSU, E, /*Backward=*/true);
}

} // namespace

void AIEBaseSubtarget::overrideSchedPolicyBase(MachineSchedPolicy &Policy,
                                               unsigned NumRegionInstrs) const {
  // The default policy is to avoid tracking pressure for "small regions". For
  // AIE, it is critical to estimate the pressure everywhere, especially small
  // loops. Spills are very expensive.
  Policy.ShouldTrackPressure =
      NumRegionInstrs >= RegPressureInstrPreMISchedThreshold;
}

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

// Set latency and declare height/depth dirty if it changes
// return whether anything changed
bool updatePredLatency(SDep &Dep, SUnit &SuccSU, int Latency) {
  if (Latency == Dep.getSignedLatency()) {
    return false;
  }

  // Change the dependence in both directions.
  getForwardEdge(SuccSU, Dep).setSignedLatency(Latency);
  Dep.setSignedLatency(Latency);
  SuccSU.setDepthDirty();
  Dep.getSUnit()->setHeightDirty();
  return true;
}

bool updateSuccLatency(SDep &SuccEdge, SUnit &PredSU, int Latency) {
  SDep &PredEdge = getBackwardEdge(PredSU, SuccEdge);
  return updatePredLatency(PredEdge, *SuccEdge.getSUnit(), Latency);
}

// Set the latency of ordering edges between memory operations and locks.
// The initial graph will have ordering edges induced by hasSideEffects of the
// locks
class LockDelays : public ScheduleDAGMutation {
  void apply(ScheduleDAGInstrs *DAG) override {
    // FIXME: Delays for locks to reach the core aren't completely described in
    // the ISA. The numbers are therefore conservative.
    const int CoreStallCycle = 2;
    const int CoreResumeCycle = 8;
    const auto *TII = static_cast<const AIEBaseInstrInfo *>(DAG->TII);

    // Iterate over all the predecessors and successors of Lock instructions
    // to increase the edge latency.
    // Note that scalar streams are kept away from locks using
    // a reserved FuncUnit instead.  See AIE2Schedule.td
    for (auto &SU : DAG->SUnits) {
      MachineInstr *Lock = SU.getInstr();
      if (!Lock || !TII->isLock(Lock->getOpcode())) {
        continue;
      }
      for (auto &PredEdge : SU.Preds) {
        MachineInstr *LdSt = PredEdge.getSUnit()->getInstr();
        if (PredEdge.getKind() != SDep::Order || !LdSt->mayLoadOrStore()) {
          continue;
        }
        // Ensure memory operation happens before the core stalls
        int Delay = *TII->getLastMemoryCycle(LdSt->getDesc().SchedClass) -
                    CoreStallCycle + 1;
        updatePredLatency(PredEdge, SU, Delay);
      }
      for (auto &SuccEdge : SU.Succs) {
        MachineInstr *LdSt = SuccEdge.getSUnit()->getInstr();
        if (SuccEdge.getKind() != SDep::Order || !LdSt->mayLoadOrStore()) {
          continue;
        }
        // Ensure memory operation happens after the core resumes
        int Delay = CoreResumeCycle -
                    *TII->getFirstMemoryCycle(LdSt->getDesc().SchedClass) + 1;
        updateSuccLatency(SuccEdge, SU, Delay);
      }
    }
  };
};

#undef DEBUG_TYPE
#define DEBUG_TYPE "machine-scheduler"

class RegionEndEdges : public ScheduleDAGMutation {
  void removeExitSUPreds(ScheduleDAGInstrs *DAG) {
    SUnit &ExitSU = DAG->ExitSU;
    while (!ExitSU.Preds.empty()) {
      ExitSU.removePred(ExitSU.Preds.back());
    }
  }
  void apply(ScheduleDAGInstrs *DAG) override {
    AIE::MaxLatencyFinder MaxLatency(static_cast<AIEScheduleDAGMI *>(DAG));

    // Default edges to ExitSU are conservative, and can't be shrunk.
    // We really should know what we're doing here, so just remove and
    // recompute all of them.
    removeExitSUPreds(DAG);

    const auto *TII = static_cast<const AIEBaseInstrInfo *>(DAG->TII);
    bool UserSetLatencyMargin = UserLatencyMargin.getNumOccurrences() > 0;
    for (SUnit &SU : DAG->SUnits) {
      MachineInstr &MI = *SU.getInstr();

      SDep ExitDep(&SU, SDep::Artificial);

      // By using IgnoreBundle, we can safely apply this mutation to already
      // bundled instructions without causing misclassification of instructions
      // that are bundled with control flow ones. Otherwise, the assertion
      // below can be triggered for correct cases.
      unsigned DelaySlots =
          TII->getNumDelaySlots(MI, MachineInstr::QueryType::IgnoreBundle);
      unsigned EdgeLatency = !DelaySlots && UserSetLatencyMargin
                                 ? UserLatencyMargin
                                 : MaxLatency(MI);
      // Extend the edge latency if MI requires delay slots. This makes sure
      // there are at least getNumDelaySlots() cycles between MI and ExitSU.
      if (DelaySlots) {
        assert(EdgeLatency < DelaySlots);
        EdgeLatency = DelaySlots + 1;
      }
      // Between writing Registers (lc, le, ls) and the end of the loop,
      // there must be a distance of 112 bytes in terms of PM addresses.
      // 112 bytes correspond to 7 fully-expanded 128-bit instructions and
      // hence adding a latency of 8 from LoopStart to the ExitSU.
      if (TII->isZeroOverheadLoopSetupInstr(MI))
        EdgeLatency = 8;

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

/// Collect all "weak" edges in a separate vector. This allows modifying
/// \p SU.Preds without invalidating iterators.
SmallVector<SDep, 4> getWeakPreds(SUnit &SU) {
  SmallVector<SDep, 4> WeakPreds;
  copy_if(SU.Preds, std::back_inserter(WeakPreds),
          [](SDep &PredEdge) { return PredEdge.isWeak(); });
  return WeakPreds;
}

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
      for (SDep &PredEdge : getWeakPreds(*SU)) {
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

class PropagateIncomingLatencies : public ScheduleDAGMutation {
  bool OnlyCopyLike;

public:
  PropagateIncomingLatencies(bool OnlyCopyLike = true)
      : OnlyCopyLike(OnlyCopyLike) {}
  void apply(ScheduleDAGInstrs *DAG) override {
    auto IsData = [](const SDep &D) { return D.getKind() == SDep::Data; };
    for (SUnit &SU : DAG->SUnits) {
      MachineInstr &MI = *SU.getInstr();

      // Only look at COPY and REG_SEQUENCE if requested
      if (OnlyCopyLike && !MI.isCopy() &&
          MI.getOpcode() != TargetOpcode::REG_SEQUENCE)
        continue;

      // Do not extend live ranges of phys regs
      if (any_of(MI.defs(), [](const MachineOperand &MO) {
            return MO.isReg() && MO.getReg().isPhysical();
          }))
        continue;

      // Find the common latency for all predecessors that can be
      // "moved" to successors.
      SDep *MinLatencyDep = nullptr;
      for (SDep &PredEdge : make_filter_range(SU.Preds, IsData)) {
        if (!MinLatencyDep ||
            PredEdge.getLatency() < MinLatencyDep->getLatency())
          MinLatencyDep = &PredEdge;
      }
      if (!MinLatencyDep)
        continue;

      int PropagatableSrcLatency = MinLatencyDep->getLatency();
      for (SDep &PredEdge : make_filter_range(SU.Preds, IsData)) {
        updatePredLatency(PredEdge, SU,
                          PredEdge.getLatency() - PropagatableSrcLatency);
      }
      for (SDep &SuccEdge : make_filter_range(SU.Succs, IsData)) {
        updateSuccLatency(SuccEdge, SU,
                          SuccEdge.getLatency() + PropagatableSrcLatency);
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
    // Query individual instruction behavior. This is because we might create
    // dependencies with already-scheduled blocks where Bundles have been
    // created.
    const auto QueryType = MachineInstr::QueryType::IgnoreBundle;
    // Run over all instructions that may load or store, and correct the
    // latencies for all their memory dependencies.
    for (SUnit &SU : DAG->SUnits) {
      MachineInstr &MI = *SU.getInstr();
      if (!MI.mayLoadOrStore(QueryType)) {
        continue;
      }

      for (auto &PredEdge : SU.Preds) {
        MachineInstr &SrcMI = *PredEdge.getSUnit()->getInstr();

        // Ignore non-memory dependencies. Locks or other instructions with side
        // effects aren't handled with MemInstrItinData itineraries.
        if (!PredEdge.isNormalMemoryOrBarrier() ||
            !SrcMI.mayLoadOrStore(QueryType)) {
          continue;
        }

        // Ignore Load-Load (RAR) dependencies.
        // TODO: Those should probably be removed altogether.
        if (!SrcMI.mayStore(QueryType) && !MI.mayStore(QueryType)) {
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
        updatePredLatency(PredEdge, SU, *MemLat);
      }
    }
  };
};

void dumpDependencies(ScheduleDAGInstrs *DAG, SDep::Kind depType,
                      const char *DepName) {
  const TargetRegisterInfo *TRI = DAG->MF.getSubtarget().getRegisterInfo();
  dbgs() << (DAG->MF).getName() << " " << DepName << " dependencies\n";
  for (SUnit &SU : DAG->SUnits) {
    for (const SDep &Dep : SU.Succs) {
      if (Dep.getKind() != depType)
        continue;
      dbgs() << "SU(" << SU.NodeNum << ")->SU(" << Dep.getSUnit()->NodeNum
             << ") ";
      Dep.dump();
      dbgs() << " " << printReg(Dep.getReg(), TRI) << "\n";
    }
  }
}

/// Prevent WAW dependencies on physical register writes. Instructions that
/// write a register have very limited scheduler freedom. That could be improved
/// by ignoring the writes that don't reach a read. Algorithm starts with the
/// live set of MBB, backtrack the DAG and update the live set. Whenever an edge
/// points to a non-live write, it is updated to the subsequent live write.
class WAWEdges : public ScheduleDAGMutation {
  // Collect all edges in a separate vector. This allows modifying SU.Preds
  // without invalidating iterators.
  SmallVector<SDep, 4> getPreds(SUnit &SU) {
    SmallVector<SDep, 4> Preds;
    copy(SU.Preds, std::back_inserter(Preds));
    return Preds;
  }
  // Updates the dependency to the instruction with last live write of the same
  // register
  void updateOutputDeps(SUnit *SU, Register Reg,
                        std::map<Register, SUnit *> &PhysRegWriters) {
    for (const SDep &Dep : getPreds(*SU)) {
      if (Dep.getKind() == SDep::Output && Dep.getReg() == Reg) {
        auto It = PhysRegWriters.find(Dep.getReg());
        if (It != PhysRegWriters.end()) {
          It->second->addPred(Dep);
        }
        SU->removePred(Dep);
      }
    }
  }
  void apply(ScheduleDAGInstrs *DAG) override {
    MachineFunction &MF = DAG->MF;
    MachineRegisterInfo &MRI = MF.getRegInfo();
    const TargetRegisterInfo *TRI = MF.getSubtarget().getRegisterInfo();
    auto *RI = static_cast<const AIEBaseRegisterInfo *>(TRI);
    LivePhysRegs LiveRegs;
    LiveRegs.init(*TRI);
    // Reserved registers are considered always live
    for (MCPhysReg PhysReg : MRI.getReservedRegs().set_bits()) {
      if (RI->isSimplifiableReservedReg(PhysReg))
        LiveRegs.addReg(PhysReg);
    }
    // Stores latest live write of physical register.
    std::map<Register, SUnit *> PhysRegWriters;
    for (SUnit &SU : reverse(DAG->SUnits)) {
      MachineInstr &MI = *SU.getInstr();
      for (MIBundleOperands MO(MI); MO.isValid(); ++MO) {
        // Checks if operand is a Physical register and it is written in MI
        if (MO->isReg() && MO->isDef() &&
            RI->isSimplifiableReservedReg(MO->getReg())) {
          Register PhysReg = MO->getReg();
          if (!LiveRegs.contains(PhysReg)) {
            // The physical register isn't live, simplify WAW dependencies that
            // are internal to the region
            updateOutputDeps(&SU, PhysReg, PhysRegWriters);
          } else {
            PhysRegWriters[PhysReg] = &SU;
          }
        }
      }
      LiveRegs.stepBackward(MI);
    }
    LLVM_DEBUG(dumpDependencies(DAG, SDep::Output, "WAW"));
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
    Mutations.emplace_back(std::make_unique<WAWEdges>());
  }
  return Mutations;
}

// List the Mutations that apply to the interblock DAG construction.
std::vector<std::unique_ptr<ScheduleDAGMutation>>
AIEBaseSubtarget::getInterBlockMutationsImpl(const Triple &TT) {
  std::vector<std::unique_ptr<ScheduleDAGMutation>> Mutations;
  Mutations.emplace_back(std::make_unique<LockDelays>());
  if (!TT.isAIE1()) {
    Mutations.emplace_back(std::make_unique<RegionEndEdges>());
    Mutations.emplace_back(std::make_unique<MemoryEdges>());
    Mutations.emplace_back(std::make_unique<WAWEdges>());
  }
  return Mutations;
}

std::vector<std::unique_ptr<ScheduleDAGMutation>>
AIEBaseSubtarget::getPreRAMutationsImpl(const Triple &TT) {
  std::vector<std::unique_ptr<ScheduleDAGMutation>> Mutations;
  if (EnablePreMISchedPropagateIncomingLatencies)
    Mutations.emplace_back(std::make_unique<PropagateIncomingLatencies>());
  if (EnableStrongCopyEdges)
    Mutations.emplace_back(std::make_unique<EnforceCopyEdges>());
  return Mutations;
}

std::vector<std::unique_ptr<ScheduleDAGMutation>>
AIEBaseSubtarget::getSMSMutationsImpl(const Triple &TT) {
  std::vector<std::unique_ptr<ScheduleDAGMutation>> Mutations;
  if (!TT.isAIE1()) {
    Mutations.emplace_back(std::make_unique<WAWEdges>());
    if (EnablePipelinerSchedPropagateIncomingLatencies)
      Mutations.emplace_back(std::make_unique<PropagateIncomingLatencies>());
  }
  return Mutations;
}
