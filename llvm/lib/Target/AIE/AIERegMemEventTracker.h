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

  unsigned getSafeOperandsDistance(const MachineInstr &MI) const;

private:
  using fixed_iterator = MachineBasicBlock::iterator;
  const InstrItineraryData *InstrItins;
  const TargetRegisterInfo *TRI;
  const AIEBaseInstrInfo *TII;
  AAResults *AA;
  std::map<MCRegister, unsigned> RegisterToCycleDef;
  std::map<MCRegister, unsigned> RegisterToCycleUse;
  int LastStoreCycle = 0;
  std::map<int, std::vector<MachineInstr *>> MemoryCycleToStoreInstrs;
  int LastMemoryAccessCycle = 0;

  const std::map<MCRegister, unsigned> &getRegToCycleMap(bool IsDef) const;

  std::map<MCRegister, unsigned> &getRegToCycleMap(bool IsDef);

  void updateUseDefMaxCycle(Register Reg, unsigned Latency, bool IsDef);

  void updateLastStoreCycle(int LastStoreCycle);

  int getLastStoreCycle() const { return LastStoreCycle; }

  void updateLastMemoryAccessCycle(int MemAccessCycle);

  int getLastMemoryAccessCycle() const { return LastMemoryAccessCycle; }

  void addPerInstructionLastStoreCycle(int LastStoreCycle, MachineInstr *MI);

  int getMaxAliasingStoreCycle(const MachineInstr &MI) const;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEREGISTEREVENTTRACKER_H
