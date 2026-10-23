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
#include "AIEBaseInstrInfo.h"
#include "AIEBaseSubtarget.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/ScheduleDAGInstrs.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/MC/MCInstrItineraries.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>

#define DEBUG_TYPE "aie-datadependencehelper"

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

void DataDependenceHelper::addPhysRegDataDeps(SUnit *SU, unsigned OperIdx) {
  const MachineOperand &MO = SU->getInstr()->getOperand(OperIdx);
  assert(MO.isDef() && "expect physreg def");
  const Register Reg = MO.getReg();
  for (const MCRegUnit Unit : TRI->regunits(Reg)) {
    for (RegUnit2SUnitsMap::iterator I = Uses.find(Unit); I != Uses.end();
         ++I) {
      SUnit *UseSU = I->SU;
      if (UseSU == SU)
        continue;
      const int UseOpIdx = I->OpIdx;
      SDep Dep;
      if (UseOpIdx < 0) {
        Dep = SDep(SU, SDep::Artificial);
      } else {
        SU->hasPhysRegDefs = true;
        const Register UseReg =
            UseSU->getInstr()->getOperand(UseOpIdx).getReg();
        Dep = SDep(SU, SDep::Data, UseReg);
      }
      addDep(SU, OperIdx, UseSU, UseOpIdx, Dep);
    }
  }
}

void DataDependenceHelper::addPhysRegDeps(SUnit *SU, unsigned OperIdx) {
  MachineInstr *MI = SU->getInstr();
  MachineOperand &MO = MI->getOperand(OperIdx);
  const Register Reg = MO.getReg();
  if (MRI.isConstantPhysReg(Reg))
    return;

  const SDep::Kind Kind = MO.isUse() ? SDep::Anti : SDep::Output;
  for (const MCRegUnit Unit : TRI->regunits(Reg)) {
    for (RegUnit2SUnitsMap::iterator I = Defs.find(Unit); I != Defs.end();
         ++I) {
      SUnit *DefSU = I->SU;
      if (DefSU == &ExitSU)
        continue;
      MachineInstr *DefInstr = DefSU->getInstr();
      MachineOperand &DefMO = DefInstr->getOperand(I->OpIdx);
      if (DefSU != SU &&
          (Kind != SDep::Output || !MO.isDead() || !DefMO.isDead())) {
        SDep Dep(SU, Kind, DefMO.getReg());
        addDep(SU, OperIdx, DefSU, I->OpIdx, Dep);
      }
    }
  }

  if (MO.isUse()) {
    SU->hasPhysRegUses = true;
    for (const MCRegUnit Unit : TRI->regunits(Reg))
      Uses.insert(PhysRegSUOper(SU, OperIdx, Unit));
    if (RemoveKillFlags)
      MO.setIsKill(false);
  } else {
    addPhysRegDataDeps(SU, OperIdx);
    // Clear previous uses and defs of this register and its subregisters,
    // unless the def is in a fixed node: its cycle is set externally and need
    // not respect the order that transitive pruning relies on.
    if (!(RetainFixedPhysRegDeps && isFixedNode(*SU))) {
      for (const MCRegUnit Unit : TRI->regunits(Reg)) {
        Uses.eraseAll(Unit);
        if (!MO.isDead())
          Defs.eraseAll(Unit);
      }
    }
    if (MO.isDead() && SU->isCall) {
      for (const MCRegUnit Unit : TRI->regunits(Reg)) {
        RegUnit2SUnitsMap::RangePair P = Defs.equal_range(Unit);
        RegUnit2SUnitsMap::iterator B = P.first;
        RegUnit2SUnitsMap::iterator It = P.second;
        for (bool isBegin = It == B; !isBegin;) {
          isBegin = (--It) == B;
          if (!It->SU->isCall)
            break;
          It = Defs.erase(It);
        }
      }
    }
    for (const MCRegUnit Unit : TRI->regunits(Reg))
      Defs.insert(PhysRegSUOper(SU, OperIdx, Unit));
  }
}

void DataDependenceHelper::addVRegDefDeps(SUnit *SU, unsigned OperIdx) {
  MachineInstr *MI = SU->getInstr();
  MachineOperand &MO = MI->getOperand(OperIdx);
  const Register Reg = MO.getReg();

  LaneBitmask DefLaneMask;
  LaneBitmask KillLaneMask;
  if (TrackLaneMasks) {
    const bool IsKill = MO.getSubReg() == 0 || MO.isUndef();
    DefLaneMask = getLaneMaskForMO(MO);
    KillLaneMask = IsKill ? LaneBitmask::getAll() : DefLaneMask;
    if (MO.getSubReg() != 0 && MO.isUndef()) {
      for (const MachineOperand &OtherMO :
           llvm::drop_begin(MI->operands(), OperIdx + 1))
        if (OtherMO.isReg() && OtherMO.isDef() && OtherMO.getReg() == Reg)
          KillLaneMask &= ~getLaneMaskForMO(OtherMO);
    }
    MO.setIsUndef(false);
  } else {
    DefLaneMask = LaneBitmask::getAll();
    KillLaneMask = LaneBitmask::getAll();
  }

  if (MO.isDead()) {
    assert(deadDefHasNoUse(MO) && "Dead defs should have no uses");
  } else {
    for (VReg2SUnitOperIdxMultiMap::iterator I = CurrentVRegUses.find(Reg),
                                             E = CurrentVRegUses.end();
         I != E;) {
      LaneBitmask LaneMask = I->LaneMask;
      if ((LaneMask & KillLaneMask).none()) {
        ++I;
        continue;
      }
      if ((LaneMask & DefLaneMask).any()) {
        SUnit *UseSU = I->SU;
        SDep Dep(SU, SDep::Data, Reg);
        addDep(SU, OperIdx, UseSU, I->OperandIndex, Dep);
      }
      LaneMask &= ~KillLaneMask;
      if (LaneMask.any()) {
        I->LaneMask = LaneMask;
        ++I;
      } else {
        I = CurrentVRegUses.erase(I);
      }
    }
  }

  if (false && MRI.hasOneDef(Reg))
    return;

  LaneBitmask LaneMask = DefLaneMask;
  for (VReg2SUnitOperIdx &V2SU :
       make_range(CurrentVRegDefs.find(Reg), CurrentVRegDefs.end())) {
    if ((V2SU.LaneMask & LaneMask).none())
      continue;
    SUnit *DefSU = V2SU.SU;
    if (DefSU == SU)
      continue;
    SDep Dep(SU, SDep::Output, Reg);
    addDep(SU, OperIdx, DefSU, V2SU.OperandIndex, Dep);
    const LaneBitmask OverlapMask = V2SU.LaneMask & LaneMask;
    const LaneBitmask NonOverlapMask = V2SU.LaneMask & ~LaneMask;
    V2SU.SU = SU;
    V2SU.LaneMask = OverlapMask;
    V2SU.OperandIndex = OperIdx;
    if (NonOverlapMask.any())
      CurrentVRegDefs.insert(
          VReg2SUnitOperIdx(Reg, NonOverlapMask, V2SU.OperandIndex, DefSU));
  }
  if (LaneMask.any())
    CurrentVRegDefs.insert(VReg2SUnitOperIdx(Reg, LaneMask, OperIdx, SU));
}

void DataDependenceHelper::addVRegUseDeps(SUnit *SU, unsigned OperIdx) {
  const MachineInstr *MI = SU->getInstr();
  const MachineOperand &MO = MI->getOperand(OperIdx);
  const Register Reg = MO.getReg();
  const LaneBitmask LaneMask =
      TrackLaneMasks ? getLaneMaskForMO(MO) : LaneBitmask::getAll();
  CurrentVRegUses.insert(VReg2SUnitOperIdx(Reg, LaneMask, OperIdx, SU));
  for (VReg2SUnitOperIdx &V2SU :
       make_range(CurrentVRegDefs.find(Reg), CurrentVRegDefs.end())) {
    if ((V2SU.LaneMask & LaneMask).none())
      continue;
    if (V2SU.SU == SU)
      continue;
    SDep Dep(SU, SDep::Anti, Reg);
    addDep(SU, OperIdx, V2SU.SU, V2SU.OperandIndex, Dep);
  }
}

bool DataDependenceHelper::handleRegEvents(SUnit *SU) {
  MachineInstr &MI = *SU->getInstr();
  bool HasVRegDef = false;
  const unsigned N = MI.getNumOperands();
  for (unsigned OpIdx = 0; OpIdx != N; ++OpIdx) {
    const MachineOperand &MO = MI.getOperand(OpIdx);
    if (!MO.isReg() || !MO.isDef())
      continue;
    const Register Reg = MO.getReg();
    if (Reg.isPhysical()) {
      addPhysRegDeps(SU, OpIdx);
    } else if (Reg.isVirtual()) {
      HasVRegDef = true;
      addVRegDefDeps(SU, OpIdx);
    }
  }
  for (unsigned OpIdx = 0; OpIdx != N; ++OpIdx) {
    const MachineOperand &MO = MI.getOperand(OpIdx);
    if (!MO.isReg() || !MO.isUse())
      continue;
    const Register Reg = MO.getReg();
    if (Reg.isPhysical()) {
      addPhysRegDeps(SU, OpIdx);
    } else if (Reg.isVirtual() && MO.readsReg()) {
      addVRegUseDeps(SU, OpIdx);
    }
  }
  return HasVRegDef;
}

InstrEvents DataDependenceHelper::collectEvents(SUnit *SU) {
  InstrEvents Events;
  Events.SU = SU;
  const MachineInstr &MI = *SU->getInstr();

  // Resolve the AIE register-dependent itinerary class once for this
  // instruction. Subsequent per-operand cycle and forwarding class queries
  // are cheap table lookups.
  const auto &ST = static_cast<const AIEBaseSubtarget &>(MF.getSubtarget());
  const auto *TII = static_cast<const AIEBaseInstrInfo *>(ST.getInstrInfo());
  const InstrItineraryData *Itins = ST.getInstrItineraryData();
  assert(Itins && !Itins->isEmpty() &&
         "AIE always has complete itinerary data");
  const unsigned ItinClass = TII->getSchedClass(
      MI.getDesc(), make_range(MI.operands_begin(), MI.operands_end()), MRI);

  for (unsigned OpIdx = 0, N = MI.getNumOperands(); OpIdx != N; ++OpIdx) {
    const MachineOperand &MO = MI.getOperand(OpIdx);
    if (!MO.isReg())
      continue;
    const Register Reg = MO.getReg();

    int OpCycle = 0;
    if (const auto C = Itins->getOperandCycle(ItinClass, OpIdx))
      OpCycle = static_cast<int>(*C);
    const unsigned FwdClass = Itins->getForwardingClass(ItinClass, OpIdx);

    if (Reg.isPhysical()) {
      if (MRI.isConstantPhysReg(Reg))
        continue;
      if (MO.isDef()) {
        for (const MCRegUnit Unit : TRI->regunits(Reg))
          Events.PhysDefs.emplace_back(Unit, Reg, OpIdx, OpCycle, FwdClass);
      } else {
        for (const MCRegUnit Unit : TRI->regunits(Reg))
          Events.PhysUses.emplace_back(Unit, Reg, OpIdx, OpCycle, FwdClass);
      }
    } else if (Reg.isVirtual()) {
      const LaneBitmask LaneMask = LaneBitmask::getAll();
      if (MO.isDef()) {
        Events.VRegDefs.emplace_back(Reg, LaneMask, OpIdx, OpCycle, FwdClass);
      } else if (MO.isUse() && MO.readsReg()) {
        Events.VRegUses.emplace_back(Reg, LaneMask, OpIdx, OpCycle, FwdClass);
      }
    }
  }
  return Events;
}

int DataDependenceHelper::computeDataLatency(const MachineInstr &DefMI,
                                             unsigned DefOpIdx,
                                             const MachineInstr *UseMI,
                                             unsigned UseOpIdx) const {
  const auto &ST = static_cast<const AIEBaseSubtarget &>(MF.getSubtarget());
  const auto *TII = static_cast<const AIEBaseInstrInfo *>(ST.getInstrInfo());
  const InstrItineraryData *Itins = ST.getInstrItineraryData();

  // Mirror TargetSchedModel::computeOperandLatency for itineraries. Negative
  // latencies are capped at 0 by getOperandLatency; adjustSchedDependency
  // restores the signed value for operands covered by the itinerary.
  std::optional<unsigned> OperLatency;
  if (UseMI) {
    OperLatency =
        TII->getOperandLatency(Itins, DefMI, DefOpIdx, *UseMI, UseOpIdx);
  } else {
    const unsigned DefClass = DefMI.getDesc().getSchedClass();
    OperLatency = Itins->getOperandCycle(DefClass, DefOpIdx);
  }
  if (OperLatency)
    return static_cast<int>(*OperLatency);

  const int DefaultDefLatency =
      static_cast<int>(TII->defaultDefLatency(ST.getSchedModel(), DefMI));
  return std::max(computeLatency(&DefMI), DefaultDefLatency);
}

// An operand that was added dynamically, i.e. it is not described by the
// instruction descriptor, carries no latency information.
static bool isImplicitPseudoOperand(const MachineInstr &MI, unsigned OpIdx) {
  const MCInstrDesc &Desc = MI.getDesc();
  const MachineOperand &MO = MI.getOperand(OpIdx);
  const Register Reg = MO.getReg();
  const bool Described = MO.isDef() ? Desc.hasImplicitDefOfPhysReg(Reg)
                                    : Desc.hasImplicitUseOfPhysReg(Reg);
  return OpIdx >= Desc.getNumOperands() && !Described;
}

void DataDependenceHelper::setBaseLatency(SUnit *Src, int SrcOpIdx, SUnit *Dst,
                                          int DstOpIdx, SDep &Dep) const {
  switch (Dep.getKind()) {
  case SDep::Output:
    // Latency of WAW dependences is entirely given by adjustSchedDependency.
    Dep.setSignedLatency(0);
    return;
  case SDep::Data:
    break;
  case SDep::Order:
    // Only artificial edges to a physical register use carry a latency.
    if (!Dep.isArtificial())
      return;
    break;
  default:
    return;
  }

  const MachineInstr &DefMI = *Src->getInstr();
  if (DefMI.getOperand(SrcOpIdx).getReg().isPhysical()) {
    const bool PseudoDef = isImplicitPseudoOperand(DefMI, SrcOpIdx);
    const bool PseudoUse =
        DstOpIdx >= 0 && isImplicitPseudoOperand(*Dst->getInstr(), DstOpIdx);
    if (PseudoDef || PseudoUse) {
      Dep.setLatency(0);
      return;
    }
  }
  // Artificial edges to a physical register use have no use operand.
  const MachineInstr *UseMI = Dep.isArtificial() ? nullptr : Dst->getInstr();
  Dep.setSignedLatency(computeDataLatency(DefMI, SrcOpIdx, UseMI, DstOpIdx));
}

bool DataDependenceHelper::addDep(SUnit *Src, int SrcOpIdx, SUnit *Dst,
                                  int DstOpIdx, SDep Dep) {
  // WAW dependences on virtual registers carry no latency.
  if (Dep.getKind() == SDep::Output && !Dep.getReg().isPhysical()) {
    Dep.setSignedLatency(0);
    Dst->addPred(Dep);
    return true;
  }

  setBaseLatency(Src, SrcOpIdx, Dst, DstOpIdx, Dep);

  // Call the non-virtual AIE-specific latency adjustment directly, bypassing
  // the virtual TargetSubtargetInfo::adjustSchedDependency dispatch.
  const auto &ST = static_cast<const AIEBaseSubtarget &>(MF.getSubtarget());
  ST.adjustSchedDependency(*ST.getInstrItineraryData(), Src, SrcOpIdx, Dst,
                           DstOpIdx, Dep);
  Dst->addPred(Dep);
  return true;
}

int DataDependenceHelper::computeLatency(const MachineInstr *MI) const {
  const auto &ST = static_cast<const AIEBaseSubtarget &>(MF.getSubtarget());
  const InstrItineraryData *Itins = ST.getInstrItineraryData();
  assert(Itins && !Itins->isEmpty() &&
         "AIE always has complete itinerary data");
  return static_cast<int>(ST.getInstrInfo()->getInstrLatency(Itins, *MI));
}

void DataDependenceHelper::buildEdges() {
  DataDependenceHelper::buildEdges(*this);
  for (auto &M : Mutations) {
    M->apply(this);
  }
}

// Large-region threshold matching the default dag-maps-huge-region option.
static constexpr unsigned AIEHugeRegion = 500;
static unsigned aieGetReductionSize() { return AIEHugeRegion / 2; }

void DataDependenceHelper::buildEdges(ScheduleDAGInstrs &Target,
                                      bool TrackLaneMasks) {
  // AIE-specific edge-building. Operates on Target using *this for context.
  // TrackLaneMasks is propagated to the member so the reg-dep helpers can use
  // it.
  this->TrackLaneMasks = TrackLaneMasks;
  // Propagate scheduling settings from Target so helper methods can use them.
  RemoveKillFlags = Target.getRemoveKillFlags();
  // Sync DDH's ExitSU and BB so addSchedBarrierDeps() operates on the right
  // region.
  BB = Target.getBB();
  ExitSU.setInstr(Target.ExitSU.getInstr());

  // Initialize DDH's alias analysis. DDH's own addChainDependencies() uses
  // DDH's mayAlias() which in turn uses this AAForDep.
  if (auto *A = Context.AA)
    AAForDep.emplace(*A);
  BarrierChain = nullptr;

  ScheduleDAGInstrs::Value2SUsMap Stores, Loads(1 /*TrueMemOrderLatency*/);
  ScheduleDAGInstrs::Value2SUsMap NonAliasStores, NonAliasLoads(1);
  ScheduleDAGInstrs::Value2SUsMap FPExceptions;

  assert(Defs.empty() && Uses.empty() &&
         "Only BuildGraph should update Defs/Uses");
  Defs.setUniverse(TRI->getNumRegUnits());
  Uses.setUniverse(TRI->getNumRegUnits());

  assert(CurrentVRegDefs.empty() && "nobody else should use CurrentVRegDefs");
  assert(CurrentVRegUses.empty() && "nobody else should use CurrentVRegUses");
  const unsigned NumVirtRegs = MRI.getNumVirtRegs();
  CurrentVRegDefs.setUniverse(NumVirtRegs);
  CurrentVRegUses.setUniverse(NumVirtRegs);

  addSchedBarrierDeps();

  for (auto &SUR : llvm::reverse(Target.SUnits)) {
    auto *SU = &SUR;
    MachineInstr &MI = *SU->getInstr();

    assert(!MI.isTerminator() && !MI.isPosition() &&
           "AIE does not schedule terminators or labels!");

    const bool HasVRegDef = handleRegEvents(SU);

    if (SU->NumSuccs == 0 && SU->Latency > 1 && (HasVRegDef || MI.mayLoad())) {
      SDep Dep(SU, SDep::Artificial);
      Dep.setLatency(SU->Latency - 1);
      ExitSU.addPred(Dep);
    }

    const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
    if (TII->isGlobalMemoryObject(&MI)) {
      if (BarrierChain)
        BarrierChain->addPredBarrier(SU);
      BarrierChain = SU;

      LLVM_DEBUG(llvm::dbgs()
                 << "Global memory object and new barrier chain: SU("
                 << BarrierChain->NodeNum << ").\n");

      addBarrierChain(Stores);
      addBarrierChain(Loads);
      addBarrierChain(NonAliasStores);
      addBarrierChain(NonAliasLoads);
      addBarrierChain(FPExceptions);
      continue;
    }

    if (MI.mayRaiseFPException()) {
      if (BarrierChain)
        BarrierChain->addPredBarrier(SU);
      FPExceptions.insert(SU, UnknownValue);
      if (FPExceptions.size() >= AIEHugeRegion) {
        LLVM_DEBUG(llvm::dbgs() << "Reducing FPExceptions map.\n");
        ScheduleDAGInstrs::Value2SUsMap empty;
        reduceHugeMemNodeMaps(FPExceptions, empty, aieGetReductionSize());
      }
    }

    if (!MI.mayStore() &&
        !(MI.mayLoad() && !MI.isDereferenceableInvariantLoad()))
      continue;

    if (BarrierChain)
      BarrierChain->addPredBarrier(SU);

    UnderlyingObjectsVector Objs;
    const bool ObjsFound = ScheduleDAGInstrs::getUnderlyingObjectsForInstr(
        &MI, MF.getFrameInfo(), Objs, MF.getDataLayout());

    if (MI.mayStore()) {
      if (!ObjsFound) {
        addChainDependencies(SU, Stores);
        addChainDependencies(SU, NonAliasStores);
        addChainDependencies(SU, Loads);
        addChainDependencies(SU, NonAliasLoads);
        Stores.insert(SU, UnknownValue);
      } else {
        for (const UnderlyingObject &UnderlObj : Objs) {
          const ValueType V = UnderlObj.getValue();
          const bool ThisMayAlias = UnderlObj.mayAlias();
          addChainDependencies(SU, ThisMayAlias ? Stores : NonAliasStores, V);
          addChainDependencies(SU, ThisMayAlias ? Loads : NonAliasLoads, V);
        }
        for (const UnderlyingObject &UnderlObj : Objs) {
          const ValueType V = UnderlObj.getValue();
          const bool ThisMayAlias = UnderlObj.mayAlias();
          (ThisMayAlias ? Stores : NonAliasStores).insert(SU, V);
        }
        addChainDependencies(SU, Loads, UnknownValue);
        addChainDependencies(SU, Stores, UnknownValue);
      }
    } else {
      if (!ObjsFound) {
        addChainDependencies(SU, Stores);
        addChainDependencies(SU, NonAliasStores);
        Loads.insert(SU, UnknownValue);
      } else {
        for (const UnderlyingObject &UnderlObj : Objs) {
          const ValueType V = UnderlObj.getValue();
          const bool ThisMayAlias = UnderlObj.mayAlias();
          addChainDependencies(SU, ThisMayAlias ? Stores : NonAliasStores, V);
          (ThisMayAlias ? Loads : NonAliasLoads).insert(SU, V);
        }
        addChainDependencies(SU, Stores, UnknownValue);
      }
    }

    if (Stores.size() + Loads.size() >= AIEHugeRegion) {
      LLVM_DEBUG(llvm::dbgs() << "Reducing Stores and Loads maps.\n");
      reduceHugeMemNodeMaps(Stores, Loads, aieGetReductionSize());
    }
    if (NonAliasStores.size() + NonAliasLoads.size() >= AIEHugeRegion) {
      LLVM_DEBUG(llvm::dbgs()
                 << "Reducing NonAliasStores and NonAliasLoads maps.\n");
      reduceHugeMemNodeMaps(NonAliasStores, NonAliasLoads,
                            aieGetReductionSize());
    }
  }

  Defs.clear();
  Uses.clear();
  CurrentVRegDefs.clear();
  CurrentVRegUses.clear();

  // Transfer DDH's ExitSU dependencies to Target's ExitSU. DDH's ExitSU was
  // used as a placeholder during edge building; the edges must live in
  // Target's SUnit graph.
  SmallVector<SDep> ExitPreds(ExitSU.Preds.begin(), ExitSU.Preds.end());
  for (const SDep &Dep : ExitPreds) {
    ExitSU.removePred(Dep);
    Target.ExitSU.addPred(Dep);
  }

  Target.markTopoDirty();
}

bool DataDependenceHelper::mayAlias(SUnit *SUa, SUnit *SUb, bool TBAA) {
  if (!MemDeps)
    return false;
  // All memory accesses in the region are proved non-aliasing; skip chain deps.
  if (SafeToIgnoreMemDeps)
    return false;
  // When building a pipelined schedule, the DAG contains two copies of the
  // loop body. Instructions with NodeNum >= PipeliningNInstr belong to the
  // second copy (the "next iteration"). aliasAcrossVirtualUnrolls() uses the
  // per-copy iteration index to determine whether two accesses across copies
  // can provably not alias.
  if (PipeliningNInstr > 0) {
    const unsigned IterA = SUa->NodeNum / PipeliningNInstr;
    const unsigned IterB = SUb->NodeNum / PipeliningNInstr;
    if (aliasAcrossVirtualUnrolls(SUa->getInstr(), SUb->getInstr(), IterA,
                                  IterB) == AliasResult::NoAlias)
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

void InterBlockEdges::BoundaryDistances::allocate(unsigned BaseNodeNum,
                                                  unsigned NumNodes) {
  Base = BaseNodeNum;
  Distances.assign(NumNodes, std::nullopt);
  RegionMax = 0;
}

void InterBlockEdges::BoundaryDistances::clear() {
  RegionMax = 0;
  std::fill(Distances.begin(), Distances.end(), std::nullopt);
}

void InterBlockEdges::BoundaryDistances::reset() {
  Base = 0;
  Distances.clear();
  RegionMax = 0;
}

void InterBlockEdges::BoundaryDistances::record(MachineInstr *MI, int Dist) {
  RegionMax = std::max(RegionMax, Dist);
  const auto Found = Nodes.find(MI);
  if (Found == Nodes.end())
    return;
  assert(Found->second >= Base &&
         "NodeNum below BoundaryDistances region base");
  const unsigned Idx = Found->second - Base;
  assert(Idx < Distances.size() &&
         "BoundaryDistances not allocated for this NodeNum");
  Distances[Idx] = Dist;
}

std::optional<int>
InterBlockEdges::BoundaryDistances::getDistance(const SUnit *SU) const {
  // ExitSU and other DAG boundary nodes have NodeNums outside the allocated
  // side; treat them as unrecorded so callers can supply a default.
  if (SU->NodeNum < Base)
    return std::nullopt;
  const unsigned Idx = SU->NodeNum - Base;
  if (Idx >= Distances.size())
    return std::nullopt;
  return Distances[Idx];
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

void InterBlockEdges::allocateDepths() {
  assert(Boundary.has_value() && "Boundary required to size depth vectors");
  PreDepths.allocate(/*BaseNodeNum=*/0, *Boundary);
  PostDepths.allocate(*Boundary, SUnits.size() - *Boundary);
}

void InterBlockEdges::markFreeBegin() {
  FreeRange &Free = currentFreeRange();
  assert(!Free.Begin && "Free range already started on this side");
  Free.Begin = SUnits.size();
}

void InterBlockEdges::markFreeEnd() {
  FreeRange &Free = currentFreeRange();
  assert(Free.Begin && "Free range must start before it ends");
  assert(!Free.End && "Free range already ended on this side");
  Free.End = SUnits.size();
}

bool InterBlockEdges::isFixedNode(const SUnit &SU) const {
  return freeRangeOf(SU).isFixed(SU.NodeNum);
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

bool InterBlockEdges::isPredecessorNode(const SUnit *SU) const {
  return Boundary ? SU->NodeNum < *Boundary : true;
}

bool InterBlockEdges::isFixedPreBoundaryNode(const SUnit *SU) const {
  return PreFree.End && SU->NodeNum >= *PreFree.End && isPredecessorNode(SU);
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
  PostDepths.reset();
  PreDepths.reset();
  PreFree.clear();
  PostFree.clear();
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
