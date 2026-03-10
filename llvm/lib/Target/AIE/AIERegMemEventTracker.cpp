//===- AIERegMemEventTracker.cpp - Register event tracker -------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Register event tracker that can be used to track read and write events
// related to instructions (cycles). Memory dependencies related to stores
// are also tracked.
//
//===----------------------------------------------------------------------===//

#include "AIERegMemEventTracker.h"
#include <cassert>

using namespace llvm;

namespace {

// Helper function to identify event-like instructions (side effects without
// register/memory dependencies)
bool isEventLikeInstruction(const MachineInstr &MI,
                            const AIEBaseInstrInfo *TII) {
  auto UseOrDefReg = [](const MachineInstr &MI) {
    return llvm::any_of(MI.operands(),
                        [](const MachineOperand &MO) { return MO.isReg(); });
  };

  return MI.hasUnmodeledSideEffects() && !MI.mayLoadOrStore() &&
         !TII->isLock(MI.getOpcode()) && !UseOrDefReg(MI);
}
} // namespace

const std::map<MCRegister, int> &
AIERegMemEventTracker::getRegToCycleMap(bool IsDef) const {
  return IsDef ? RegisterToCycleDef : RegisterToCycleUse;
}

std::map<MCRegister, int> &AIERegMemEventTracker::getRegToCycleMap(bool IsDef) {
  return const_cast<std::map<MCRegister, int> &>(
      const_cast<const AIERegMemEventTracker *>(this)->getRegToCycleMap(IsDef));
}

static int memoryCycleToDelay(std::optional<int> OneBasedMemoryCycle) {
  assert(OneBasedMemoryCycle.has_value() &&
         "Memory instruction without MemoryCycles");
  assert(*OneBasedMemoryCycle >= 1 && "Memory pipeline stages are 1-based");
  return *OneBasedMemoryCycle - 1;
}

int AIERegMemEventTracker::firstMemoryDelay(unsigned SchedClass) const {
  return memoryCycleToDelay(TII->getFirstMemoryCycle(SchedClass));
}

int AIERegMemEventTracker::lastMemoryDelay(unsigned SchedClass) const {
  return memoryCycleToDelay(TII->getLastMemoryCycle(SchedClass));
}

int AIERegMemEventTracker::minFirstMemoryDelay() const {
  return memoryCycleToDelay(TII->getMinFirstMemoryCycle());
}

int AIERegMemEventTracker::minLastMemoryDelay() const {
  return memoryCycleToDelay(TII->getMinLastMemoryCycle());
}

int AIERegMemEventTracker::memoryDelay(unsigned SchedClass,
                                       bool IsBackward) const {
  return IsBackward ? firstMemoryDelay(SchedClass)
                    : lastMemoryDelay(SchedClass);
}

int AIERegMemEventTracker::minMemoryDelay(bool IsBackward) const {
  return IsBackward ? minFirstMemoryDelay() : minLastMemoryDelay();
}

void AIERegMemEventTracker::updateUseDefMaxCycle(Register Reg, int EventCycle,
                                                 bool IsDef) {
  auto [It, Inserted] =
      getRegToCycleMap(IsDef).emplace(Reg.asMCReg(), EventCycle);
  if (!Inserted)
    It->second = std::max(It->second, EventCycle);
}

void AIERegMemEventTracker::updateMemCycle(int Cycle, bool IsStore) {
  if (IsStore)
    StoreCycle = std::max(StoreCycle, Cycle);
  else
    LoadCycle = std::max(LoadCycle, Cycle);
}

int AIERegMemEventTracker::getMemCycle(bool IsStore) const {
  return IsStore ? StoreCycle : LoadCycle;
}

int AIERegMemEventTracker::getMemoryAccessCycle() const {
  return std::max(StoreCycle, LoadCycle);
}

void AIERegMemEventTracker::addPerInstructionMemCycle(int MemCycle,
                                                      MachineInstr *MI) {
  if (MI->mayStore())
    MemoryCycleToStoreInstrs[MemCycle].push_back(MI);
  else
    MemoryCycleToLoadInstrs[MemCycle].push_back(MI);
}

int AIERegMemEventTracker::getMaxAliasingMemCycle(const MachineInstr &MI,
                                                  bool IsStore) const {
  const auto &MemMap =
      IsStore ? MemoryCycleToStoreInstrs : MemoryCycleToLoadInstrs;
  int MaxCycle = INT_MIN;

  for (const auto &[Cycle, MemOps] : MemMap) {
    for (const MachineInstr *MemOp : MemOps) {
      // Part-word memory operations have special semantics, treat
      // conservatively. For other operations, use AA to check if they may alias
      // with MI.
      if (TII->isPartWordMemoryInst(*MemOp) ||
          MI.mayAlias(AA, *MemOp, /*UseTBAA=*/true)) {
        MaxCycle = std::max(MaxCycle, Cycle);
      }
    }
  }
  return (MaxCycle == INT_MIN) ? 0 : MaxCycle;
}

int AIERegMemEventTracker::getMinSafeDistance(int CurrSafeDistance,
                                              int StoredCycle, int EventDelay,
                                              bool IsBackward) const {
  if (StoredCycle <= INT_MIN || StoredCycle >= INT_MAX)
    return CurrSafeDistance;

  // Both StoredCycle and EventDelay are distances from the same anchor
  // (EntrySU for forward, ExitSU for backward). Two events at cycles A and B
  // overlap when |A - B| <= 0, so the first non-overlapping placement is at
  // distance |A - B| + 1.
  //
  // Forward: the free instruction is placed *after* the fixed region, so its
  //   event is at (placement + EventDelay). The gap from the stored event
  //   is (placement + EventDelay) - StoredCycle, which must be >= 1.
  //   Solving: placement >= StoredCycle - EventDelay + 1.
  //
  // Backward: the free instruction is placed *before* the fixed region, so
  //   its event is at (placement - EventDelay). The gap from the stored
  //   event is StoredCycle - (placement - EventDelay), which must be >= 1.
  //   Solving: placement >= StoredCycle + EventDelay + 1.
  const int Dep = IsBackward ? (StoredCycle + EventDelay + 1)
                             : (StoredCycle - EventDelay + 1);
  return std::max(CurrSafeDistance, std::max(0, Dep));
}

int AIERegMemEventTracker::checkRegisterDependencies(int CurrSafeDistance,
                                                     const MachineInstr &MI,
                                                     bool IsBackward) const {

  for (unsigned OpNum = 0; OpNum < MI.getNumOperands(); OpNum++) {
    const MachineOperand &MO = MI.getOperand(OpNum);
    if (!MO.isReg())
      continue;
    auto OptCycle =
        InstrItins->getOperandCycle(MI.getDesc().getSchedClass(), OpNum);
    unsigned OperandCycle = OptCycle ? *OptCycle : 0 /*implicit-def*/;

    auto MaxRegSafeDistance =
        [this, OperandCycle, IsBackward](const MachineOperand &MO, bool IsDef) {
          const std::map<MCRegister, int> &RegToCycle =
              IsDef ? RegisterToCycleDef : RegisterToCycleUse;
          int MaxDistance = 0;
          for (MCRegAliasIterator Ali(MO.getReg(), TRI, true); Ali.isValid();
               ++Ali) {
            auto RegCycle = RegToCycle.find(*Ali);
            if (RegCycle != RegToCycle.end())
              MaxDistance = getMinSafeDistance(MaxDistance, RegCycle->second,
                                               OperandCycle, IsBackward);
          }
          return MaxDistance;
        };

    const int DistFromLastWrite = MaxRegSafeDistance(MO, /*IsDef*/ true);
    const int DistFromLastRead =
        MO.isDef() ? MaxRegSafeDistance(MO, /*IsDef*/ false) : 0;

    CurrSafeDistance =
        std::max({CurrSafeDistance, DistFromLastWrite, DistFromLastRead});
  }
  return CurrSafeDistance;
}

int AIERegMemEventTracker::checkEventLikeInstruction(
    int CurrSafeDistance, const MachineInstr &MI) const {
  if (!isEventLikeInstruction(MI, TII))
    return CurrSafeDistance;
  return std::max(CurrSafeDistance, static_cast<int>(FixedRegionSize));
}

int AIERegMemEventTracker::checkLoadStoreDependencies(int CurrSafeDistance,
                                                      const MachineInstr &MI,
                                                      bool IsBackward) const {
  if (!MI.mayLoadOrStore())
    return CurrSafeDistance;

  const int MIMemoryDelay = [&]() {
    if (MI.isBundle())
      return minMemoryDelay(IsBackward);
    return memoryDelay(MI.getDesc().getSchedClass(), IsBackward);
  }();

  if (MI.mayStore()) {
    const int LdCycle = AA ? getMaxAliasingMemCycle(MI, /*IsStore=*/false)
                           : getMemCycle(/*IsStore=*/false);
    CurrSafeDistance = getMinSafeDistance(CurrSafeDistance, LdCycle,
                                          MIMemoryDelay, IsBackward);
  }

  const int StCycle = AA ? getMaxAliasingMemCycle(MI, /*IsStore=*/true)
                         : getMemCycle(/*IsStore=*/true);
  CurrSafeDistance =
      getMinSafeDistance(CurrSafeDistance, StCycle, MIMemoryDelay, IsBackward);

  return CurrSafeDistance;
}

int AIERegMemEventTracker::checkLockDependency(int CurrSafeDistance,
                                               const MachineInstr &MI,
                                               bool IsBackward) const {
  if (!TII->isLock(MI.getOpcode()))
    return CurrSafeDistance;

  const int MemAccessCycle = getMemoryAccessCycle();
  if (MemAccessCycle <= INT_MIN)
    return CurrSafeDistance;

  // Lock stalls the core from CoreStallCycle to CoreResumeCycle.
  // Backward: memory must finish before the core resumes.
  // Forward: memory must start after the core stalls.
  const int LockCycle = IsBackward ? TII->getCoreResumeCycleAfterLock()
                                   : TII->getCoreStallCycleAfterLock() - 1;
  return getMinSafeDistance(CurrSafeDistance, MemAccessCycle, LockCycle,
                            IsBackward);
}

int AIERegMemEventTracker::eventCycle(int Cycle, int Delay, bool IsBackward,
                                      bool InSeparateRegion,
                                      int TotalCycles) const {
  // Cycle is the instruction's distance from the tracking anchor (EntrySU for
  // forward, ExitSU for backward). Delay is how many cycles after issue the
  // event occurs.
  //   Forward:  the event is Delay cycles further from EntrySU → add.
  //   Backward: the event is Delay cycles closer to ExitSU → subtract.
  int Event = IsBackward ? (Cycle - Delay) : (Cycle + Delay);
  // When projecting events from a separate region (e.g. a loop body into an
  // adjacent block), shift by -TotalCycles so that cycle 0 represents the
  // boundary between the two regions. Positive values extend into the adjacent
  // block; negative values remain inside the source region.
  if (InSeparateRegion)
    Event -= TotalCycles;
  return Event;
}

void AIERegMemEventTracker::processInstruction(MachineInstr *BundledMI,
                                               int Cycle, bool IsBackward,
                                               bool InSeparateRegion,
                                               int TotalCycles) {
  const unsigned SchedClass = BundledMI->getDesc().getSchedClass();

  // Track when this instruction touches memory. Forward tracking records the
  // last (completion) cycle; backward tracking records the first (start) cycle.
  if (BundledMI->mayLoadOrStore()) {
    const int MemEvent = eventCycle(Cycle, memoryDelay(SchedClass, IsBackward),
                                    IsBackward, InSeparateRegion, TotalCycles);

    if (BundledMI->mayLoad())
      updateMemCycle(MemEvent, /*IsStore=*/false);
    if (BundledMI->mayStore())
      updateMemCycle(MemEvent, /*IsStore=*/true);

    addPerInstructionMemCycle(MemEvent, BundledMI);
  }

  // Track when each register operand is read or written.
  for (unsigned OpNum = 0; OpNum < BundledMI->getNumOperands(); OpNum++) {
    const MachineOperand &MO = BundledMI->getOperand(OpNum);
    if (!MO.isReg())
      continue;
    std::optional<unsigned> OptMOCycle =
        InstrItins->getOperandCycle(SchedClass, OpNum);
    assert(OptMOCycle);
    const int RegEvent = eventCycle(Cycle, static_cast<int>(*OptMOCycle),
                                    IsBackward, InSeparateRegion, TotalCycles);
    // When processing a separate region, eventCycle shifts events so that
    // cycle 0 is the region boundary:
    //   Forward (projecting into epilogue): events with RegEvent <= 0 completed
    //     before the epilogue starts and cannot cause hazards — skip them.
    //   Backward (projecting into preheader): events with negative RegEvent are
    //     close to the boundary and still constrain the preheader — keep them.
    if (IsBackward || !InSeparateRegion || RegEvent > 0)
      updateUseDefMaxCycle(MO.getReg(), RegEvent, MO.isDef());
  }
}

void AIERegMemEventTracker::computeUseDefForward(
    ArrayRef<AIE::MachineBundle> Bundles, bool InSeparateRegion) {
  const int TotalCycles = Bundles.size();

  if (!InSeparateRegion)
    FixedRegionSize = TotalCycles;

  int Cycle = 0;
  for (const auto &Bundle : Bundles) {
    for (MachineInstr *BundledMI : Bundle.getInstrs())
      processInstruction(BundledMI, Cycle, /*IsBackward=*/false,
                         InSeparateRegion, TotalCycles);
    Cycle++;
  }
}

void AIERegMemEventTracker::computeUseDefBackward(
    ArrayRef<AIE::MachineBundle> Bundles, bool InSeparateRegion) {
  const int TotalCycles = Bundles.size();

  if (!InSeparateRegion)
    FixedRegionSize = TotalCycles;

  int Cycle = 0;
  for (const auto &Bundle : reverse(Bundles)) {
    for (MachineInstr *BundledMI : Bundle.getInstrs())
      processInstruction(BundledMI, Cycle, /*IsBackward=*/true,
                         InSeparateRegion, TotalCycles);
    Cycle++;
  }
}

int AIERegMemEventTracker::getSafeOperandsDistance(const MachineInstr &MI,
                                                   bool IsBackward) const {
  int CurrSafeDistance = 0;

  CurrSafeDistance = checkEventLikeInstruction(CurrSafeDistance, MI);
  CurrSafeDistance =
      checkLoadStoreDependencies(CurrSafeDistance, MI, IsBackward);
  CurrSafeDistance = checkLockDependency(CurrSafeDistance, MI, IsBackward);
  CurrSafeDistance =
      checkRegisterDependencies(CurrSafeDistance, MI, IsBackward);

  return std::max(0, CurrSafeDistance);
}

unsigned AIERegMemEventTracker::getSafeOperandsDistanceFromTop(
    const MachineInstr &MI) const {
  return getSafeOperandsDistance(MI, /*IsBackward=*/false);
}

unsigned AIERegMemEventTracker::getSafeOperandsDistanceFromBottom(
    const MachineInstr &MI) const {
  return getSafeOperandsDistance(MI, /*IsBackward=*/true);
}
