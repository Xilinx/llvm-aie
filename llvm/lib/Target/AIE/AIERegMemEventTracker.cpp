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

using namespace llvm;

const std::map<MCRegister, unsigned> &
AIERegMemEventTracker::getRegToCycleMap(bool IsDef) const {
  return IsDef ? RegisterToCycleDef : RegisterToCycleUse;
}

std::map<MCRegister, unsigned> &
AIERegMemEventTracker::getRegToCycleMap(bool IsDef) {
  return const_cast<std::map<MCRegister, unsigned> &>(
      const_cast<const AIERegMemEventTracker *>(this)->getRegToCycleMap(IsDef));
}

void AIERegMemEventTracker::updateUseDefMaxCycle(Register Reg, unsigned Latency,
                                                 bool IsDef) {
  std::map<MCRegister, unsigned> &RegisterToCycle = getRegToCycleMap(IsDef);
  MCRegister MCReg = Reg.asMCReg();
  auto EmplacePair = RegisterToCycle.emplace(MCReg, Latency);
  if (!EmplacePair.second) {
    auto RegCycle = EmplacePair.first;
    RegCycle->second = std::max(RegCycle->second, Latency);
  }
}

void AIERegMemEventTracker::computeUseDefForward(
    ArrayRef<AIE::MachineBundle> Bundles, bool InSeparateRegion) {
  int Cycle = 0;
  const int TotalCycles = Bundles.size();
  for (const auto &Bundle : Bundles) {
    for (MachineInstr *BundledMI : Bundle.getInstrs()) {
      const unsigned SchedClass = BundledMI->getDesc().getSchedClass();

      // We need to be conservative with memory access/dependencies.
      if (BundledMI->mayStore()) {
        // Record getLastMemoryCycle of this instruction if this is a
        // store.
        auto OptLastStoreCycle = TII->getLastMemoryCycle(SchedClass);
        assert(OptLastStoreCycle && "Store instruction without MemoryCycles");
        const int LastMemoryCycle = *OptLastStoreCycle;
        int LastStoreCycle = Cycle + LastMemoryCycle - 1;
        if (InSeparateRegion)
          LastStoreCycle = LastStoreCycle - TotalCycles;
        // This is used when we have no AA information of partword store.
        updateLastStoreCycle(LastStoreCycle);
        // Track memory instructions by their completion cycle. This is
        // used when we have AA information.
        addPerInstructionLastStoreCycle(LastStoreCycle, BundledMI);
      }

      // Track all memory operations (loads and stores) for lock dependencies.
      // Locks must wait for all preceding memory accesses to complete.
      if (BundledMI->mayLoadOrStore()) {
        auto OptLastMemCycle = TII->getLastMemoryCycle(SchedClass);
        if (OptLastMemCycle) {
          int MemAccessCycle = Cycle + *OptLastMemCycle - 1;
          if (InSeparateRegion)
            MemAccessCycle = MemAccessCycle - TotalCycles;
          updateLastMemoryAccessCycle(MemAccessCycle);
        }
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

unsigned
AIERegMemEventTracker::getSafeOperandsDistance(const MachineInstr &MI) const {
  unsigned MaxLatency = 0;

  // Firstly, estimate safe memory distancies in case of load and stores
  // operations.
  if (MI.mayLoadOrStore()) {
    auto MemoryPipelineStage =
        MI.isBundle() ? TII->getMinFirstMemoryCycle()
                      : TII->getFirstMemoryCycle(MI.getDesc().getSchedClass());
    assert(MemoryPipelineStage.value() >= 1 && "Execution stages start at 0");
    // Here, we calculate this initial latency.
    // For example, if we have LastStoreCycle = 21 (absolute) and this
    // instruction FirstMemoryCycle = 5 (relative to the start cycle of this
    // instruction), then this instruction should start earliest at cycle 18,
    // to be able to start the FirstMemoryCycle (absolute) at cycle 22. In
    // this case: 21 - (5 - 1) + 1 = 18. Be aware that we need to cap to zero
    // when it is negative.
    const int MIMemoryCycle = MemoryPipelineStage.value() - 1;

    if (AA) {
      // Precise: Only consider stores that may alias with MI
      const int MaxAliasingStoreCycle = getMaxAliasingStoreCycle(MI);
      const int MemoryDep = MaxAliasingStoreCycle - MIMemoryCycle + 1;
      MaxLatency = std::max(MemoryDep, 0);
    } else {
      // Conservative: Use global LastStoreCycle
      const int MemoryDep = getLastStoreCycle() - MIMemoryCycle + 1;
      MaxLatency = std::max(MemoryDep, 0);
    }
  }

  // Lock instructions stall the core. All preceding memory operations must
  // complete before the core stalls.
  if (TII->isLock(MI.getOpcode())) {
    const int CoreStallCycle = TII->getCoreStallCycleAfterLock();
    const int MIStallCycle = CoreStallCycle - 1;
    const int MemDep = getLastMemoryAccessCycle() - MIStallCycle + 1;
    MaxLatency =
        std::max(MaxLatency, static_cast<unsigned>(std::max(MemDep, 0)));
  }

  for (const MachineOperand &MO : MI.operands()) {
    if (!MO.isReg())
      continue;

    auto SafeDistance = [&](const std::map<MCRegister, unsigned> &RegToCycle) {
      unsigned CurrMaxLatency = 0;
      for (MCRegAliasIterator Ali(MO.getReg(), TRI, true); Ali.isValid();
           ++Ali) {
        auto RegCycle = RegToCycle.find(*Ali);
        if (RegCycle != RegToCycle.end()) {
          CurrMaxLatency = std::max(RegCycle->second, CurrMaxLatency);
        }
      }
      return CurrMaxLatency;
    };

    // Here, we need to take care of two scenarios:
    // * For MO.isDef() == true:
    //  - We look to RegDef cycles to detect WAW deps.
    //  - We look to RegUse cycles to detect WAR deps.
    // * For MO.isDef() == false we look to RegDef cycles to detect RAW
    // deps.
    // For each instruction, we can have all deps, but we are interested
    // in the worst one in terms of cycles.
    const unsigned DistFromLastWrite =
        SafeDistance(getRegToCycleMap(/*IsDef*/ true));
    // When we have an use, we don't care about RAR deps.
    const unsigned DistFromLastRead =
        MO.isDef() ? SafeDistance(getRegToCycleMap(/*IsDef*/ false)) : 0;
    MaxLatency = std::max({MaxLatency, DistFromLastWrite, DistFromLastRead});
  }
  return MaxLatency;
}

void AIERegMemEventTracker::updateLastStoreCycle(int StoreCycle) {
  LastStoreCycle = std::max(LastStoreCycle, StoreCycle);
}

void AIERegMemEventTracker::addPerInstructionLastStoreCycle(int LastStoreCycle,
                                                            MachineInstr *MI) {
  MemoryCycleToStoreInstrs[LastStoreCycle].push_back(MI);
}

int AIERegMemEventTracker::getMaxAliasingStoreCycle(
    const MachineInstr &MI) const {
  int MaxAliasingStoreCycle = 0;
  for (const auto &[Cycle, Stores] : MemoryCycleToStoreInstrs) {
    for (const MachineInstr *Store : Stores) {
      // Part-word stores (e.g., byte/half-word) have read-modify-write
      // behavior and must be treated conservatively. For other stores,
      // use alias analysis to check if they may alias with MI. If either
      // condition is true, we need to enforce a dependency.
      if (TII->isPartWordMemoryInst(*Store) ||
          MI.mayAlias(AA, *Store, /*UseTBAA=*/true)) {
        MaxAliasingStoreCycle = std::max(MaxAliasingStoreCycle, Cycle);
      }
    }
  }
  return MaxAliasingStoreCycle;
}

void AIERegMemEventTracker::updateLastMemoryAccessCycle(int MemAccessCycle) {
  LastMemoryAccessCycle = std::max(LastMemoryAccessCycle, MemAccessCycle);
}
