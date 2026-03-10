//===- AIERegMemEventTracker.h - Register event tracker -..------*- C++ -*-===//
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

void AIERegMemEventTracker::updateUseDefMaxCycle(Register Reg, int EventCycle,
                                                 bool IsDef) {
  auto [It, Inserted] =
      getRegToCycleMap(IsDef).emplace(Reg.asMCReg(), EventCycle);
  if (!Inserted)
    It->second = std::max(It->second, EventCycle);
}

void AIERegMemEventTracker::updateLastMemCycle(int Cycle, bool IsStore) {
  if (IsStore)
    LastStoreCycle = std::max(LastStoreCycle, Cycle);
  else
    LastLoadCycle = std::max(LastLoadCycle, Cycle);
}

int AIERegMemEventTracker::getFirstMemCycle(bool IsStore) const {
  return IsStore ? FirstStoreCycle : FirstLoadCycle;
}

int AIERegMemEventTracker::getLastMemCycle(bool IsStore) const {
  return IsStore ? LastStoreCycle : LastLoadCycle;
}

int AIERegMemEventTracker::getLastMemoryAccessCycle() const {
  return std::max(LastStoreCycle, LastLoadCycle);
}

int AIERegMemEventTracker::getFirstMemoryAccessCycle() const {
  return std::max(FirstStoreCycle, FirstLoadCycle);
}

void AIERegMemEventTracker::updateFirstMemCycle(int Cycle, bool IsStore) {
  if (IsStore)
    FirstStoreCycle = std::max(FirstStoreCycle, Cycle);
  else
    FirstLoadCycle = std::max(FirstLoadCycle, Cycle);
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
  int MaxCycle = IsStore ? 0 : INT_MIN;

  for (const auto &[Cycle, MemOps] : MemMap) {
    for (const MachineInstr *MemOp : MemOps) {
      // Part-word memory operations have special semantics, treat
      // conservatively For other operations, use AA to check if they may alias
      // with MI
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
    // Get operand cycle if needed
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

int AIERegMemEventTracker::checkEventLikeInstruction(int CurrSafeDistance,
                                                     const MachineInstr &MI,
                                                     bool IsBackward) const {
  if (!isEventLikeInstruction(MI, TII))
    return CurrSafeDistance;

  unsigned RegionSize = IsBackward ? BotFixedRegionSize : TopFixedRegionSize;
  return std::max(CurrSafeDistance, static_cast<int>(RegionSize));
}

int AIERegMemEventTracker::checkLoadStoreDependencies(int CurrSafeDistance,
                                                      const MachineInstr &MI,
                                                      bool IsBackward) const {
  if (!MI.mayLoadOrStore())
    return CurrSafeDistance;

  // Forward (epilogue): use FirstMemoryCycle - the free instruction's earliest
  // memory touch must come after the stored last memory event.
  // Backward (prologue): use LastMemoryCycle - the free instruction's latest
  // memory touch must be farther from ExitSU than the stored first memory
  // event.
  const int MIMemoryDelay = [&]() {
    if (MI.isBundle())
      return IsBackward ? minLastMemoryDelay() : minFirstMemoryDelay();
    unsigned SchedClass = MI.getDesc().getSchedClass();
    return IsBackward ? lastMemoryDelay(SchedClass)
                      : firstMemoryDelay(SchedClass);
  }();

  // Get the relevant stored cycle for a given memory type.
  // Forward: stored values are last-touch cycles (getLastMemCycle).
  // Backward: stored values are first-touch cycles (getFirstMemCycle).
  auto GetFixedMemCycle = [this, IsBackward](bool IsStore) {
    return IsBackward ? getFirstMemCycle(IsStore) : getLastMemCycle(IsStore);
  };

  if (MI.mayStore()) {
    // Free store: check WAR dependency against loads in the fixed region
    const int LoadCycle = AA ? getMaxAliasingMemCycle(MI, /*IsStore=*/false)
                             : GetFixedMemCycle(/*IsStore=*/false);
    CurrSafeDistance = getMinSafeDistance(CurrSafeDistance, LoadCycle,
                                          MIMemoryDelay, IsBackward);
  }

  // Free load/store: check RAW (for loads) or WAW (for stores) against stores
  // in the fixed region
  const int StoreCycle = AA ? getMaxAliasingMemCycle(MI, /*IsStore=*/true)
                            : GetFixedMemCycle(/*IsStore=*/true);
  CurrSafeDistance = getMinSafeDistance(CurrSafeDistance, StoreCycle,
                                        MIMemoryDelay, IsBackward);

  return CurrSafeDistance;
}

int AIERegMemEventTracker::checkLockDependency(int CurrSafeDistance,
                                               const MachineInstr &MI,
                                               bool IsBackward) const {
  if (!TII->isLock(MI.getOpcode()))
    return CurrSafeDistance;

  const int MemAccessCycle =
      IsBackward ? getFirstMemoryAccessCycle() : getLastMemoryAccessCycle();
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

void AIERegMemEventTracker::computeUseDefForward(
    ArrayRef<AIE::MachineBundle> Bundles, bool InSeparateRegion) {
  int Cycle = 0;
  const int TotalCycles = Bundles.size();

  // Track top-fixed region size (only for the first call, not separate region)
  if (!InSeparateRegion) {
    TopFixedRegionSize = TotalCycles;
  }

  for (const auto &Bundle : Bundles) {
    for (MachineInstr *BundledMI : Bundle.getInstrs()) {
      const unsigned SchedClass = BundledMI->getDesc().getSchedClass();

      // Track memory operations (loads and stores)
      if (BundledMI->mayLoadOrStore()) {
        int LastMemCycle = Cycle + lastMemoryDelay(SchedClass);
        if (InSeparateRegion)
          LastMemCycle = LastMemCycle - TotalCycles;

        // Update appropriate cycle tracker(s)
        if (BundledMI->mayLoad())
          updateLastMemCycle(LastMemCycle, /*IsStore=*/false);
        if (BundledMI->mayStore())
          updateLastMemCycle(LastMemCycle, /*IsStore=*/true);

        // Track memory instructions by their completion cycle for AA
        addPerInstructionMemCycle(LastMemCycle, BundledMI);
      }

      for (unsigned OpNum = 0; OpNum < BundledMI->getNumOperands(); OpNum++) {
        const MachineOperand &MO = BundledMI->getOperand(OpNum);
        if (!MO.isReg())
          continue;
        const bool IsDef = MO.isDef();
        std::optional<unsigned> OptMOCycle =
            InstrItins->getOperandCycle(SchedClass, OpNum);
        assert(OptMOCycle);
        const int OperandCycle = *OptMOCycle;
        const int EventCycle =
            Cycle + OperandCycle - (InSeparateRegion ? TotalCycles : 0);
        if (!InSeparateRegion || EventCycle > 0)
          updateUseDefMaxCycle(MO.getReg(), EventCycle, IsDef);
      }
    }
    Cycle++;
  }
}

void AIERegMemEventTracker::computeUseDefBackward(
    ArrayRef<AIE::MachineBundle> Bundles, bool InSeparateRegion) {
  const int TotalCycles = Bundles.size();

  // Track bot-fixed region size (only for the first call, not separate region)
  if (!InSeparateRegion) {
    BotFixedRegionSize = TotalCycles;
  }

  // Count progressively from 0 as we iterate backward through bundles
  // Cycle represents the distance from ExitSU for each bundle
  int Cycle = 0;

  for (const auto &Bundle : reverse(Bundles)) {
    // First bundle processed (last in forward order) is at Cycle = 0 (closest
    // to ExitSU) Last bundle processed (first in forward order) is at Cycle =
    // TotalCycles - 1 (farthest from ExitSU)

    for (MachineInstr *BundledMI : Bundle.getInstrs()) {
      const unsigned SchedClass = BundledMI->getDesc().getSchedClass();

      // Track memory operations (loads and stores)
      if (BundledMI->mayLoadOrStore()) {
        int FirstMemFromEnd = Cycle - firstMemoryDelay(SchedClass);
        if (InSeparateRegion)
          FirstMemFromEnd = FirstMemFromEnd - TotalCycles;

        // Update appropriate cycle tracker(s)
        if (BundledMI->mayLoad())
          updateFirstMemCycle(FirstMemFromEnd, /*IsStore=*/false);
        if (BundledMI->mayStore())
          updateFirstMemCycle(FirstMemFromEnd, /*IsStore=*/true);

        // Track memory instructions by their start cycle for AA
        addPerInstructionMemCycle(FirstMemFromEnd, BundledMI);
      }

      // Track register operands
      for (unsigned OpNum = 0; OpNum < BundledMI->getNumOperands(); OpNum++) {
        const MachineOperand &MO = BundledMI->getOperand(OpNum);
        if (!MO.isReg())
          continue;
        const bool IsDef = MO.isDef();
        std::optional<unsigned> OptMOCycle =
            InstrItins->getOperandCycle(SchedClass, OpNum);
        assert(OptMOCycle);
        const int OperandCycle = *OptMOCycle;

        const int EventCycle =
            Cycle - OperandCycle - (InSeparateRegion ? TotalCycles : 0);
        updateUseDefMaxCycle(MO.getReg(), EventCycle, IsDef);
      }
    }
    Cycle++; // Increment as we go backward through bundles
  }
}

int AIERegMemEventTracker::getSafeOperandsDistance(const MachineInstr &MI,
                                                   bool IsBackward) const {
  int CurrSafeDistance = 0;

  CurrSafeDistance =
      checkEventLikeInstruction(CurrSafeDistance, MI, IsBackward);
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
