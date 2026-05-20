//===-- AIEMaxLatencyFinder.h - Interblock latency support ----------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares helpers for inter-block latency computations
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MAXLATENCYFINDER_H
#define LLVM_LIB_TARGET_AIE_MAXLATENCYFINDER_H

#include "AIEBaseSubtarget.h"
#include "AIEDataDependenceHelper.h"
#include "AIEMachineScheduler.h"
#include "llvm/CodeGen/MachineInstr.h"
#include <memory>
#include <vector>

using namespace llvm;

namespace llvm::AIE {

// operand and the memory latency. Include the stage latency if requested.
int maxLatency(const MachineInstr *MI, const AIEBaseInstrInfo &InstrInfo,
               const InstrItineraryData &Itineraries, bool IncludeStages);

class MaxLatencyFinder {
  AIEPostRASchedStrategy *const Scheduler;
  const AIEBaseInstrInfo *const TII;
  const InstrItineraryData *const Itineraries;
  const MCRegisterInfo *const TRI;
  MachineBasicBlock *const CurBB;

  const bool IsBottomRegion;

  const bool SuccessorsAreScheduled;

  /// True when CurBB has no CFG successors (e.g. a return block), requiring
  /// the conservative raw latency as a floor.
  bool HasUnknownSuccessors = false;

  /// Reflects opportunity to reduce maxLatency and wires in the commandline
  /// flag
  bool ReduceLatency;

  // Check whether this region connects to the successor blocks.
  bool isBottomRegion(MachineInstr *ExitMI);

  // Use the interblock edges to get an sharper bound on maxlatency
  int computeEffectiveLatency(MachineInstr &MI);

public:
  explicit MaxLatencyFinder(ScheduleDAGInstrs *DAG);

  // Find the maximum latency of MI taking successors into account.
  unsigned operator()(MachineInstr &MI);
};

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_MAXLATENCYFINDER_H
