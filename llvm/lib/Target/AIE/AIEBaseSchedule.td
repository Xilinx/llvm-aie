//=- AIEBaseSchedule.td - Common AIE Scheduling Model ---------*- tablegen -*-=//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Settings shared by the scheduling models of AIE2 and later architectures.
// A concrete model only needs to link its own itineraries.
//
//===----------------------------------------------------------------------===//

class AIEBaseSchedModel<ProcessorItineraries Itins> : SchedMachineModel {
  // Issue limit is anyway limited by VLIW formats, but there is no universal
  // syntax for "no issue limit".
  int IssueWidth = 1000;

  // Allow some kind of controlled out-order execution through negative latencies
  // This is mostly to convince the scheduler that it doesn't need to bump the
  // current cycle by `x` after scheduling an instruction in cycle `CurrCycle + x`.
  // However, in pre-RA mode, MicroOpBufferSize also enables the heuristics for
  // acyclic latency of the local region vs cyclic critical path latency of the
  // enclosing loop in pre-RA mode. See checkAcyclicLatency().
  // I haven't seen any effect on QoR at this point.
  int MicroOpBufferSize = 1000;

  let Itineraries = Itins;

  // LoadLatency, HighLatency and MispredictPenalty are deliberately not set:
  // the latencies come from the itineraries, and the generic fallbacks are not
  // used for AIE.
  // The default def latency used when the itineraries have no data
  // is defined in DataDependenceHelper::defaultDefLatency.
  // MispredictPenalty is not used. Instead, we override getCriticalPathLimit().

  // CompleteModel = 1 makes TableGen reject every instruction that has neither
  // an itinerary class nor hasNoSchedulingInfo. Pseudo instructions
  // deliberately have no itinerary class. They get their latency from
  // DataDependenceHelper::defaultDefLatency, so the model is not complete by
  // design.
  let CompleteModel = 0;
  let PostRAScheduler = 1;
}
