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
  std::map<int, std::vector<MachineInstr *>> MemoryCycleToStoreInstrs;
  std::map<int, std::vector<MachineInstr *>> MemoryCycleToLoadInstrs;
  int LastMemoryAccessCycle = INT_MIN;
  int FirstMemoryAccessCycle = INT_MIN;

  unsigned BotFixedRegionSize = 0;
  unsigned TopFixedRegionSize = 0;

  const std::map<MCRegister, int> &getRegToCycleMap(bool IsDef) const;

  std::map<MCRegister, int> &getRegToCycleMap(bool IsDef);

  int getFirstMemCycle(bool IsStore) const {
    return IsStore ? FirstStoreCycle : FirstLoadCycle;
  }

  int getLastStoreCycle() const { return LastStoreCycle; }

  void updateUseDefMaxCycle(Register Reg, int Latency, bool IsDef);

  void updateLastStoreCycle(int LastStoreCycle);

  void updateLastGlobalMemoryAccessCycle(int MemAccessCycle);

  void updateFirstGlobalMemoryAccessCycle(int MemAccessCycle);

  void updateFirstMemCycle(int Cycle, bool IsStore);

  int getFirstMemoryAccessCycle() const { return FirstMemoryAccessCycle; }

  int getLastMemoryAccessCycle() const { return LastMemoryAccessCycle; }

  void addPerInstructionLastStoreCycle(int LastStoreCycle, MachineInstr *MI);

  void addPerInstructionFirstMemCycle(int FirstMemCycle, MachineInstr *MI,
                                      bool IsStore);

  int getMaxAliasingMemCycle(const MachineInstr &MI, bool IsStore) const;

  unsigned checkMemoryDependency(unsigned CurrentMax, int MemoryCycle,
                                 int MIMemoryCycle, bool IsBackward) const;

  unsigned checkRegisterDependencies(unsigned CurrentMax,
                                     const MachineInstr &MI,
                                     bool IsBackward) const;

  unsigned checkEventLikeInstruction(unsigned CurrentMax,
                                     const MachineInstr &MI,
                                     bool IsBackward) const;

  unsigned getBotFixedRegionSize() const { return BotFixedRegionSize; }
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEREGISTEREVENTTRACKER_H
