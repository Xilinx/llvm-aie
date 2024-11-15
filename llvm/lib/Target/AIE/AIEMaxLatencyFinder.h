//===-- AIEMaxLatencyFinder.h - Interblock latency support ----------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares helpers for inter-block latency computations
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MAXLATENCYFINDER_H
#define LLVM_LIB_TARGET_AIE_MAXLATENCYFINDER_H

#include "AIEBaseSubtarget.h"
#include "AIEMachineScheduler.h"
#include "llvm/CodeGen/MachineInstr.h"

using namespace llvm;

namespace llvm::AIE {

// operand and the memory latency. Include the stage latency if requested.
int maxLatency(const MachineInstr *MI, const AIEBaseInstrInfo &InstrInfo,
               const InstrItineraryData &Itineraries, bool IncludeStages);

struct InstrAndCycle {
  MachineInstr *MI = nullptr;
  int Cycle;
};

/// Find the first dependence on SrcMI in Bundles[0,Prune)
/// \returns the Cycle in which the dependence happens or a conservative lower
///          bound and the instruction responsible for the dependency if it is
///          found.
InstrAndCycle findEarliestRef(const MachineInstr &SrcMI,
                              ArrayRef<MachineBundle> Bundles, int Prune);

class MaxLatencyFinder {
  const AIEPostRASchedStrategy *const Scheduler;
  const AIEBaseInstrInfo *const TII;
  const InstrItineraryData *const Itineraries;
  const MCRegisterInfo *const TRI;
  MachineBasicBlock *const CurBB;
  const bool InterBlock;

  // Check whether this region connects to the successor blocks
  //
  bool isBottomRegion(MachineInstr *ExitMI);

public:
  // Constructors
  MaxLatencyFinder(const AIEPostRASchedStrategy *const Scheduler,
                   const AIEBaseInstrInfo *const TII,
                   const InstrItineraryData *const Itineraries,
                   const MCRegisterInfo *const TRI,
                   MachineBasicBlock *const CurBB);

  MaxLatencyFinder(ScheduleDAGInstrs *DAG);

  // Find the maximum latency of MI taking  successors into account
  unsigned operator()(MachineInstr &MI);
};

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_MAXLATENCYFINDER_H
