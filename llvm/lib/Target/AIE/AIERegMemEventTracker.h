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

#ifndef LLVM_LIB_TARGET_AIE_AIEREGISTEREVENTTRACKER_H
#define LLVM_LIB_TARGET_AIE_AIEREGISTEREVENTTRACKER_H

#include "AIEBaseInstrInfo.h"
#include "AIEBundle.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/MC/MCInstrItineraries.h"
#include <map>

namespace llvm {
// Helper class to track the last read and write cycles of every register. Those
// cycles can then be used to create dependencies to unscheduled successor
// instructions.
//
// Each instance is used in a single direction (forward or backward). Forward
// tracking records events from EntrySU; backward tracking records events from
// ExitSU. The member variables are shared between directions because a given
// instance only ever calls computeUseDef in one direction.
class AIERegMemEventTracker {
public:
  AIERegMemEventTracker(const InstrItineraryData *Itins,
                        const TargetRegisterInfo *TRI,
                        const AIEBaseInstrInfo *TII, AAResults *AA = nullptr)
      : InstrItins(Itins), TRI(TRI), TII(TII), AA(AA) {};

  // This function calculates the cycles in which each register will be last
  // materialized or used, taking into account a specified sequence of timed
  // bundles. InSeparateRegion is utilized to project the availability cycle
  // to the subsequent region.
  void computeUseDefForward(ArrayRef<AIE::MachineBundle> Bundles,
                            bool InSeparateRegion);

  // This function calculates the cycles from the end in which each register
  // will be first materialized or used, taking into account a specified
  // sequence of timed bundles in reverse order. InSeparateRegion is utilized
  // to project the availability cycle to the preceding region.
  void computeUseDefBackward(ArrayRef<AIE::MachineBundle> Bundles,
                             bool InSeparateRegion);

  /// Return the minimum number of cycles MI must be placed away from the
  /// top (EntrySU) of the fixed region to avoid hazards.
  unsigned getSafeOperandsDistanceFromTop(const MachineInstr &MI) const;

  /// Return the minimum number of cycles MI must be placed away from the
  /// bottom (ExitSU) of the fixed region to avoid hazards.
  unsigned getSafeOperandsDistanceFromBottom(const MachineInstr &MI) const;

private:
  const InstrItineraryData *InstrItins;
  const TargetRegisterInfo *TRI;
  const AIEBaseInstrInfo *TII;
  /// Alias analysis results, or nullptr for conservative (all-alias) mode.
  AAResults *AA;

  /// Maximum event cycle at which each register is defined or used.
  /// Keyed by physical register; values are cycle positions relative to the
  /// tracking anchor (EntrySU for forward, ExitSU for backward).
  std::map<MCRegister, int> RegisterToCycleDef;
  std::map<MCRegister, int> RegisterToCycleUse;

  /// Maximum cycle at which any store/load touches memory.
  /// Initialized to INT_MIN (meaning "no event recorded yet").
  int StoreCycle = INT_MIN;
  int LoadCycle = INT_MIN;

  /// Per-instruction memory event maps, used for alias-aware dependency
  /// checking. Maps memory-access cycle → list of instructions that access
  /// memory at that cycle.
  std::map<int, std::vector<MachineInstr *>> MemoryCycleToStoreInstrs;
  std::map<int, std::vector<MachineInstr *>> MemoryCycleToLoadInstrs;

  /// Number of cycles in the fixed (already-scheduled) region. Used by
  /// checkEventLikeInstruction to ensure event-like instructions are placed
  /// outside the entire fixed region.
  unsigned FixedRegionSize = 0;

  /// Return the def or use register-to-cycle map.
  const std::map<MCRegister, int> &getRegToCycleMap(bool IsDef) const;
  std::map<MCRegister, int> &getRegToCycleMap(bool IsDef);

  /// Memory cycle APIs (getFirstMemoryCycle/getLastMemoryCycle) return 1-based
  /// pipeline stage numbers: stage 1 is the first execution cycle after issue.
  /// These helpers convert to 0-based delays from the issue cycle, consistent
  /// with the register operand convention (getOperandCycle returns 0-based).
  /// E.g., a memory operation at stage 5 touches memory 4 cycles after issue.
  int firstMemoryDelay(unsigned SchedClass) const;
  int lastMemoryDelay(unsigned SchedClass) const;
  int minFirstMemoryDelay() const;
  int minLastMemoryDelay() const;

  /// Return the memory delay appropriate for the given direction.
  /// Forward uses lastMemoryDelay (tracking completion), backward uses
  /// firstMemoryDelay (tracking start from end).
  int memoryDelay(unsigned SchedClass, bool IsBackward) const;
  int minMemoryDelay(bool IsBackward) const;

  /// Return the maximum recorded memory cycle for stores or loads.
  int getMemCycle(bool IsStore) const;

  /// Update the maximum event cycle for Reg's def or use map.
  void updateUseDefMaxCycle(Register Reg, int EventCycle, bool IsDef);

  /// Update the maximum recorded store or load memory cycle.
  void updateMemCycle(int Cycle, bool IsStore);

  /// Return the maximum memory cycle across both stores and loads.
  int getMemoryAccessCycle() const;

  /// Record MI's memory access at MemCycle for per-instruction alias analysis.
  void addPerInstructionMemCycle(int MemCycle, MachineInstr *MI);

  /// Using alias analysis, find the maximum memory cycle among tracked
  /// store/load instructions that may alias with MI.
  /// Returns 0 if no aliasing instruction is found.
  int getMaxAliasingMemCycle(const MachineInstr &MI, bool IsStore) const;

  /// Compute the minimum safe placement distance given a stored event cycle
  /// and the new instruction's event delay. Returns the larger of
  /// CurrSafeDistance and the computed distance.
  /// See the .cpp implementation for the derivation of the forward/backward
  /// formulas.
  int getMinSafeDistance(int CurrSafeDistance, int StoredCycle, int EventDelay,
                         bool IsBackward) const;

  /// Check all register operands of MI against tracked register events.
  /// Returns the minimum placement distance needed to avoid register hazards.
  int checkRegisterDependencies(int CurrSafeDistance, const MachineInstr &MI,
                                bool IsBackward) const;

  /// Event-like instructions (unmodeled side effects, no reg/mem deps) must
  /// be placed outside the entire fixed region. Returns FixedRegionSize if
  /// MI is event-like, otherwise CurrSafeDistance.
  int checkEventLikeInstruction(int CurrSafeDistance,
                                const MachineInstr &MI) const;

  /// Check MI's memory operands against tracked memory events (loads and
  /// stores). Uses alias analysis when available, otherwise conservatively
  /// assumes all memory operations alias.
  int checkLoadStoreDependencies(int CurrSafeDistance, const MachineInstr &MI,
                                 bool IsBackward) const;

  /// Check if MI is a lock instruction and compute the safe distance so that
  /// the lock's core-stall window does not overlap with any tracked memory
  /// access.
  int checkLockDependency(int CurrSafeDistance, const MachineInstr &MI,
                          bool IsBackward) const;

  /// Aggregate all dependency checks (event-like, load/store, lock, register)
  /// and return the overall minimum safe placement distance.
  int getSafeOperandsDistance(const MachineInstr &MI, bool IsBackward) const;

  /// Compute the cycle at which an event with the given Delay from its issue
  /// cycle occurs, relative to the tracking anchor (EntrySU or ExitSU).
  ///   Forward:  event = Cycle + Delay  (later issue → later event)
  ///   Backward: event = Cycle - Delay  (earlier issue → earlier event)
  /// When processing a separate region (e.g. the loop body projected into an
  /// adjacent block), the event is shifted back by TotalCycles to express it
  /// in the adjacent block's coordinate system.
  int eventCycle(int Cycle, int Delay, bool IsBackward, bool InSeparateRegion,
                 int TotalCycles) const;

  /// Record all register and memory events for a single instruction at the
  /// given Cycle position. Called once per instruction from
  /// computeUseDefForward / computeUseDefBackward. For each event (register
  /// operand read/write or memory access), computes the event cycle and updates
  /// the tracking state.
  void processInstruction(MachineInstr *BundledMI, int Cycle, bool IsBackward,
                          bool InSeparateRegion, int TotalCycles);
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEREGISTEREVENTTRACKER_H
