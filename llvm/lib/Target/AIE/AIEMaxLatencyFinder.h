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
#include "llvm/CodeGen/MachineRegisterInfo.h"

using namespace llvm;

namespace llvm::AIE {

// operand and the memory latency. Include the stage latency if requested.
int maxLatency(const MachineInstr *MI, const AIEBaseInstrInfo &InstrInfo,
               const InstrItineraryData &Itineraries, bool IncludeStages);

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

  // Check whether SrcOp and DstOp might refer to the same value
  bool overlap(MachineOperand &SrcOp, MachineOperand &DstOp) const;

  // Check whether Dst depends on Src
  bool depends(MachineInstr &Src, MachineInstr &Dst) const;

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

  // Find the first dependence on SrcMI in Bundles or Prune,
  // whichever is less
  int findEarliestRef(MachineInstr &SrcMI,
                      const std::vector<llvm::AIE::MachineBundle> &Bundles,
                      int Prune);
};

} // namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_MAXLATENCYFINDER_H
