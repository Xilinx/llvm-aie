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

  unsigned getSafeOperandsDistanceFromTop(const MachineInstr &MI) const;

  unsigned getSafeOperandsDistanceFromBottom(const MachineInstr &MI) const;

private:
  using fixed_iterator = MachineBasicBlock::iterator;
  const InstrItineraryData *InstrItins;
  const TargetRegisterInfo *TRI;
  const AIEBaseInstrInfo *TII;
  AAResults *AA;

  // Forward tracking data structures (use int for consistency with backward)
  std::map<MCRegister, int> RegisterToCycleDef;
  std::map<MCRegister, int> RegisterToCycleUse;
  int FirstLoadCycle = INT_MIN;
  int FirstStoreCycle = INT_MIN;
  int LastStoreCycle = INT_MIN;
  int LastLoadCycle = INT_MIN;
  // For load-only instructions (mayLoad && !mayStore), the lock-ordering cycle
  // extends to the register write-back stage rather than just the
  // memory-access stage.
  int LastLoadWriteBackCycle = INT_MIN;
  std::map<int, std::vector<MachineInstr *>> MemoryCycleToStoreInstrs;
  std::map<int, std::vector<MachineInstr *>> MemoryCycleToLoadInstrs;
  // Load-only lock-ordering cycles (write-back stage) keyed by completion
  // cycle.
  std::map<int, std::vector<MachineInstr *>> MemoryCycleToLoadLockOrdInstrs;

  unsigned BotFixedRegionSize = 0;
  unsigned TopFixedRegionSize = 0;

  const std::map<MCRegister, int> &getRegToCycleMap(bool IsDef) const;

  std::map<MCRegister, int> &getRegToCycleMap(bool IsDef);

  int getFirstMemCycle(bool IsStore) const;

  int getLastMemCycle(bool IsStore) const;

  void updateUseDefMaxCycle(Register Reg, int Latency, bool IsDef);

  void updateLastStoreCycle(int LastStoreCycle);

  void updateLastLoadCycle(int LastLoadCycle);

  void updateLastLoadWriteBackCycle(int Cycle);

  int getLastMemoryAccessCycleForLockOrdering() const;

  void updateFirstMemCycle(int Cycle, bool IsStore);

  int getFirstMemoryAccessCycle() const;

  int getLastMemoryAccessCycle() const;

  void addPerInstructionLastMemCycle(int LastMemCycle, MachineInstr *MI);

  void addPerInstructionFirstMemCycle(int FirstMemCycle, MachineInstr *MI);

  int getMaxAliasingMemCycle(const MachineInstr &MI, bool IsStore) const;

  // Among aliasing mem ops keyed by LAST mem stage (backward coords: max Cycle
  // = earliest in program). Used for ACQ stall (mem must finish before lock).
  int getMaxAliasingLastMemCycleForLock(const MachineInstr &Lock, bool IsStore,
                                        AAResults *AA) const;

  // Among aliasing mem ops keyed by FIRST mem stage (backward coords: max Cycle
  // = earliest in program). Used for lock resume (mem must start after resume).
  int getMaxAliasingFirstMemCycleForLock(const MachineInstr &Lock,
                                         AAResults *AA) const;

  // Record a load-only mem op at its write-back completion cycle (LockOrdCycle)
  // in MemoryCycleToLoadLockOrdInstrs. Consumed by
  // getMaxAliasingLastMemCycleForLock for ACQ stall; distinct from
  // addPerInstructionLastMemCycle (memory-access cycle).
  void addPerInstructionLockOrdMemCycle(int LockOrdCycle, MachineInstr *MI);

  int checkMemoryDependency(int CurrentMax, int MemoryCycle, int MIMemoryCycle,
                            bool IsBackward) const;

  int checkRegisterDependencies(int CurrentMax, const MachineInstr &MI,
                                bool IsBackward) const;

  int checkEventLikeInstruction(int CurrentMax, const MachineInstr &MI,
                                bool IsBackward) const;

  unsigned getBotFixedRegionSize() const { return BotFixedRegionSize; }
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEREGISTEREVENTTRACKER_H
