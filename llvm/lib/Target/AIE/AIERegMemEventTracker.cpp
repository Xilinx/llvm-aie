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

void AIERegMemEventTracker::updateUseDefMaxCycle(Register Reg, int Latency,
                                                 bool IsDef) {
  std::map<MCRegister, int> &RegisterToCycle = getRegToCycleMap(IsDef);
  MCRegister MCReg = Reg.asMCReg();
  auto EmplacePair = RegisterToCycle.emplace(MCReg, Latency);
  if (!EmplacePair.second) {
    auto RegCycle = EmplacePair.first;
    RegCycle->second = std::max(RegCycle->second, Latency);
  }
}

void AIERegMemEventTracker::updateLastStoreCycle(int StoreCycle) {
  LastStoreCycle = std::max(LastStoreCycle, StoreCycle);
}

void AIERegMemEventTracker::updateLastLoadCycle(int LoadCycle) {
  LastLoadCycle = std::max(LastLoadCycle, LoadCycle);
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
  if (IsStore) {
    FirstStoreCycle = std::max(FirstStoreCycle, Cycle);
  } else {
    FirstLoadCycle = std::max(FirstLoadCycle, Cycle);
  }
}

void AIERegMemEventTracker::addPerInstructionLastMemCycle(int LastMemCycle,
                                                          MachineInstr *MI) {
  if (MI->mayStore()) {
    MemoryCycleToStoreInstrs[LastMemCycle].push_back(MI);
  } else {
    MemoryCycleToLoadInstrs[LastMemCycle].push_back(MI);
  }
}

void AIERegMemEventTracker::addPerInstructionFirstMemCycle(int FirstMemCycle,
                                                           MachineInstr *MI) {
  if (MI->mayStore()) {
    MemoryCycleToStoreInstrs[FirstMemCycle].push_back(MI);
  } else {
    MemoryCycleToLoadInstrs[FirstMemCycle].push_back(MI);
  }
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

int AIERegMemEventTracker::checkMemoryDependency(int CurrentMax,
                                                 int MemoryCycle,
                                                 int MIMemoryCycle,
                                                 bool IsBackward) const {
  if (MemoryCycle > INT_MIN && MemoryCycle < INT_MAX) {
    // The +1 converts from "last cycle of overlap" to "first safe cycle".
    // In instruction scheduling.
    const int MemoryDep = IsBackward ? (MemoryCycle + MIMemoryCycle + 1)
                                     : (MemoryCycle - MIMemoryCycle + 1);
    return std::max(CurrentMax, std::max(0, MemoryDep));
  }
  return CurrentMax;
}

int AIERegMemEventTracker::checkRegisterDependencies(int CurrentMax,
                                                     const MachineInstr &MI,
                                                     bool IsBackward) const {
  int MaxLatency = CurrentMax;

  for (unsigned OpNum = 0; OpNum < MI.getNumOperands(); OpNum++) {
    const MachineOperand &MO = MI.getOperand(OpNum);
    if (!MO.isReg())
      continue;
    // Get operand cycle if needed
    auto OptCycle =
        InstrItins->getOperandCycle(MI.getDesc().getSchedClass(), OpNum);
    unsigned OperandCycle = OptCycle ? *OptCycle : 0 /*implicit-def*/;

    auto SafeDistance = [this, OperandCycle,
                         IsBackward](const MachineOperand &MO, bool IsDef) {
      const std::map<MCRegister, int> &RegToCycle =
          IsDef ? RegisterToCycleDef : RegisterToCycleUse;
      int CurrMaxLatency = 0;
      for (MCRegAliasIterator Ali(MO.getReg(), TRI, true); Ali.isValid();
           ++Ali) {
        auto RegCycle = RegToCycle.find(*Ali);
        if (RegCycle != RegToCycle.end()) {
          const int RegCycleValue = RegCycle->second;
          // The +1 converts from "last cycle of overlap" to "first safe cycle".
          // In instruction scheduling.
          const int ThisOperandLatency = IsBackward
                                             ? RegCycleValue + OperandCycle + 1
                                             : RegCycleValue - OperandCycle + 1;
          CurrMaxLatency = std::max(ThisOperandLatency, CurrMaxLatency);
        }
      }
      return CurrMaxLatency;
    };

    const int DistFromLastWrite = SafeDistance(MO, /*IsDef*/ true);
    const int DistFromLastRead =
        MO.isDef() ? SafeDistance(MO, /*IsDef*/ false) : 0;

    MaxLatency = std::max({MaxLatency, DistFromLastWrite, DistFromLastRead});
  }
  return MaxLatency;
}

int AIERegMemEventTracker::checkEventLikeInstruction(int CurrentMax,
                                                     const MachineInstr &MI,
                                                     bool IsBackward) const {
  if (isEventLikeInstruction(MI, TII)) {
    unsigned RegionSize = IsBackward ? BotFixedRegionSize : TopFixedRegionSize;
    return std::max(CurrentMax, static_cast<int>(RegionSize));
  }
  return CurrentMax;
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
        auto OptLastMemCycle = TII->getLastMemoryCycle(SchedClass);
        assert(OptLastMemCycle && "Memory instruction without MemoryCycles");
        const int LastMemoryCycle = *OptLastMemCycle;
        int LastMemCycle = Cycle + LastMemoryCycle - 1;
        if (InSeparateRegion)
          LastMemCycle = LastMemCycle - TotalCycles;

        // Update appropriate cycle tracker(s)
        if (BundledMI->mayLoad())
          updateLastLoadCycle(LastMemCycle);
        if (BundledMI->mayStore())
          updateLastStoreCycle(LastMemCycle);

        // Track memory instructions by their completion cycle for AA
        addPerInstructionLastMemCycle(LastMemCycle, BundledMI);
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
        if (InSeparateRegion) {
          const int EventCycle = Cycle + OperandCycle;
          const int EventCycleInNextRegion = EventCycle - TotalCycles;
          if (EventCycleInNextRegion > 0)
            updateUseDefMaxCycle(MO.getReg(), EventCycleInNextRegion, IsDef);
        } else {
          const int EventCycle = Cycle + OperandCycle;
          updateUseDefMaxCycle(MO.getReg(), EventCycle, IsDef);
        }
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
        auto OptFirstMemCycle = TII->getFirstMemoryCycle(SchedClass);
        assert(OptFirstMemCycle && "Memory instruction without MemoryCycles");
        const int FirstMemCycle = *OptFirstMemCycle;
        int FirstMemFromEnd = Cycle - FirstMemCycle + 1;
        if (InSeparateRegion)
          FirstMemFromEnd = FirstMemFromEnd - TotalCycles;

        // Update appropriate cycle tracker(s)
        if (BundledMI->mayLoad())
          updateFirstMemCycle(FirstMemFromEnd, /*IsStore=*/false);
        if (BundledMI->mayStore())
          updateFirstMemCycle(FirstMemFromEnd, /*IsStore=*/true);

        // Track memory instructions by their start cycle for AA
        addPerInstructionFirstMemCycle(FirstMemFromEnd, BundledMI);
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

        // Calculate when the operand event occurs
        const int EventCycle = Cycle - OperandCycle;

        if (InSeparateRegion) {
          const int EventCycleInPrevRegion = EventCycle - TotalCycles;
          // Track even if negative - represents events in the previous region
          updateUseDefMaxCycle(MO.getReg(), EventCycleInPrevRegion, IsDef);
        } else {
          // Track even if negative - represents events very close to ExitSU
          updateUseDefMaxCycle(MO.getReg(), EventCycle, IsDef);
        }
      }
    }
    Cycle++; // Increment as we go backward through bundles
  }
}

unsigned AIERegMemEventTracker::getSafeOperandsDistanceFromTop(
    const MachineInstr &MI) const {
  int MaxLatency = 0;

  // Check for event-like instructions
  MaxLatency = checkEventLikeInstruction(MaxLatency, MI, /*IsTop=*/false);

  // Memory dependencies from top
  if (MI.mayLoadOrStore()) {
    auto MemoryPipelineStage =
        MI.isBundle() ? TII->getMinFirstMemoryCycle()
                      : TII->getFirstMemoryCycle(MI.getDesc().getSchedClass());
    assert(MemoryPipelineStage.value() >= 1 && "Execution stages start at 0");
    // Here, we calculate this initial latency.
    // For example, if we have LastStoreCycle = 21 (absolute) and this
    // instruction FirstMemoryCycle = 5 (relative to the start cycle of this
    // instruction), then this instruction should start earliest at cycle 18,
    // to be able to start the FirstMemoryCycle (absolute) at cycle 22.
    // this case: 21 - (5 - 1) + 1 = 18. Be aware that we need to cap to zero
    // when it is negative.
    const int MIMemoryCycle = MemoryPipelineStage.value() - 1;

    if (MI.mayStore()) {
      // Free store: check loads in top-fixed (WAR dependency)
      const int LoadCycle = AA ? getMaxAliasingMemCycle(MI, /*IsStore=*/false)
                               : getLastMemCycle(/*IsStore=*/false);
      MaxLatency =
          checkMemoryDependency(MaxLatency, LoadCycle, MIMemoryCycle, false);
    }

    // Free load/store: check stores in top-fixed (RAW for loads, WAW for
    // stores)
    const int StoreCycle = AA ? getMaxAliasingMemCycle(MI, /*IsStore=*/true)
                              : getLastMemCycle(/*IsStore=*/true);
    MaxLatency =
        checkMemoryDependency(MaxLatency, StoreCycle, MIMemoryCycle, false);
  }

  // Lock instructions stall the core. All preceding memory operations must
  // complete before the core stalls.
  if (TII->isLock(MI.getOpcode())) {
    if (getLastMemoryAccessCycle() > INT_MIN) {
      const int CoreStallCycle = TII->getCoreStallCycleAfterLock();
      const int MIStallCycle = CoreStallCycle - 1;
      const int MemDep = getLastMemoryAccessCycle() - MIStallCycle + 1;
      MaxLatency = std::max(MaxLatency, std::max(0, MemDep));
    }
  }

  // Register dependencies from top
  MaxLatency =
      checkRegisterDependencies(MaxLatency, MI, /*AddOperandCycle=*/false);

  return static_cast<unsigned>(std::max(0, MaxLatency));
}

unsigned AIERegMemEventTracker::getSafeOperandsDistanceFromBottom(
    const MachineInstr &MI) const {
  int MaxLatency = 0;

  // Check for event-like instructions
  MaxLatency = checkEventLikeInstruction(MaxLatency, MI, /*IsBackward=*/true);

  // Memory dependencies from bottom
  if (MI.mayLoadOrStore()) {
    auto MemoryPipelineStage =
        MI.isBundle() ? TII->getMinLastMemoryCycle()
                      : TII->getLastMemoryCycle(MI.getDesc().getSchedClass());
    assert(MemoryPipelineStage.value() >= 1 && "Execution stages start at 0");
    const int MIMemoryCycle = MemoryPipelineStage.value() - 1;

    if (MI.mayStore()) {
      // Free store: check loads in bot-fixed (WAR dependency)
      const int LoadCycle = AA ? getMaxAliasingMemCycle(MI, /*IsStore=*/false)
                               : getFirstMemCycle(/*IsStore=*/false);
      MaxLatency =
          checkMemoryDependency(MaxLatency, LoadCycle, MIMemoryCycle, true);
    }

    // Free load/store: check stores in bot-fixed (RAW dependency)
    const int LoadStoreCycle = AA ? getMaxAliasingMemCycle(MI, /*IsStore=*/true)
                                  : getFirstMemCycle(/*IsStore=*/true);
    MaxLatency =
        checkMemoryDependency(MaxLatency, LoadStoreCycle, MIMemoryCycle, true);
  }
  // Lock instructions: all subsequent memory operations must wait.
  // lock no_fence: for targets that support it, no forward latency is
  // needed — LockDelays already enforces MEM->lock gaps large enough to drain
  // the pipeline before the lock issues, so no memory is ever in-flight at
  // lock issue time.
  if (TII->isLock(MI.getOpcode()) && !TII->hasLockNoFenceSemantics()) {
    if (getFirstMemoryAccessCycle() > INT_MIN) {
      const int CoreResumeCycle = TII->getCoreResumeCycleAfterLock();
      const int MemCycle = getFirstMemoryAccessCycle();
      MaxLatency =
          std::max(MaxLatency, std::max(0, MemCycle + CoreResumeCycle + 1));
    }
  }

  // Register dependencies from bottom
  MaxLatency =
      checkRegisterDependencies(MaxLatency, MI, /*AddOperandCycle=*/true);

  return static_cast<unsigned>(std::max(0, MaxLatency));
}
