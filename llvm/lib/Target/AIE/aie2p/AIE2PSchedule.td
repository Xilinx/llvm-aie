//=- AIE2PSchedule.td - AIE2p Scheduling Definitions --------*- tablegen -*-=//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

include "aie2p/AIE2PGenSchedule.td"

def AIE2PSchedModel : SchedMachineModel {
  // Issue limit is anyway limited by VLIW formats, but there is no universal
  // syntax for "no issue limit".
  int IssueWidth = 1000;

  // Allow some kind of controlled out-order execution through negative latencies
  // This is mostly to convince the scheduler that it doesn't need to bump the
  // current cycle by `x` after scheduling an instruction in cycle `CurrCycle + x`.
  // However, in pre-RA mode, MicroOpBufferSize also enables the heuristics for
  // acyclic latency of the local region vs cyclic critical path latency of the
  // enclosing loop in pre-RA mode. See checkAcyclicLatency().
  int MicroOpBufferSize = 1000;

  let Itineraries = AIE2PItineraries;
  int LoadLatency = 5; // FIXME
  int MispredictPenalty = 4; // FIXME
  int HighLatency = 37; // FIXME
  let CompleteModel = 0;
  let PostRAScheduler = 1;
}
