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
  std::map<int, std::vector<MachineInstr *>> MemoryCycleToStoreInstrs;
  std::map<int, std::vector<MachineInstr *>> MemoryCycleToLoadInstrs;

  unsigned BotFixedRegionSize = 0;
  unsigned TopFixedRegionSize = 0;

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

  int getFirstMemCycle(bool IsStore) const;

  int getLastMemCycle(bool IsStore) const;

  void updateUseDefMaxCycle(Register Reg, int EventCycle, bool IsDef);

  void updateLastMemCycle(int Cycle, bool IsStore);

  void updateFirstMemCycle(int Cycle, bool IsStore);

  int getFirstMemoryAccessCycle() const;

  int getLastMemoryAccessCycle() const;

  void addPerInstructionMemCycle(int MemCycle, MachineInstr *MI);

  int getMaxAliasingMemCycle(const MachineInstr &MI, bool IsStore) const;

  int getMinSafeDistance(int CurrSafeDistance, int StoredCycle, int EventDelay,
                         bool IsBackward) const;

  int checkRegisterDependencies(int CurrSafeDistance, const MachineInstr &MI,
                                bool IsBackward) const;

  int checkEventLikeInstruction(int CurrSafeDistance, const MachineInstr &MI,
                                bool IsBackward) const;

  int checkLoadStoreDependencies(int CurrSafeDistance, const MachineInstr &MI,
                                 bool IsBackward) const;

  int checkLockDependency(int CurrSafeDistance, const MachineInstr &MI,
                          bool IsBackward) const;

  int getSafeOperandsDistance(const MachineInstr &MI, bool IsBackward) const;

  unsigned getBotFixedRegionSize() const { return BotFixedRegionSize; }
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEREGISTEREVENTTRACKER_H
