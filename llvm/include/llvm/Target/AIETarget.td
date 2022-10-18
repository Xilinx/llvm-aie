//===--- AIETarget.td - AIE extensions to Target.td --------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces which should be implemented by each AIE
// target to use AIE-specific TableGen based code generators.
//
//===----------------------------------------------------------------------===//

// Expansion information for a pre-scheduling pseudo-instruction.
class PreSchedInstExpansion<Instruction Target, Instruction Barrier> {
  Instruction TargetInstr = Target;     // The instruction to generate.
  Instruction BarrierInstr = Barrier;
}

// Scheduling information for instructions that touch memory.
class MemoryCycles<int First, int Last = -1> {
  // Cycle for the first load or store to memory.
  int FirstCycle = First;

  // Cycle for the last load or store to memory. This is typically the same as
  // FirstCycle, except for instructions that are read-modify-write. In that
  // case, the instruction touches memory twice, and LastCycle would be
  // different from FirstCycle.
  int LastCycle = Last;
}

// An extension of InstrItinData that is able to store MemoryCycles
// for accurate "memory" edges for instructions with ordering constraints.
class MemInstrItinData<InstrItinClass Class, list<InstrStage> stages,
                       list<int> operandcycles = [],
                       MemoryCycles MemCycles,
                       list<Bypass> bypasses = [], int uops = 1> :
    InstrItinData<Class, stages, operandcycles, bypasses, uops> {
  int FirstMemCycle = MemCycles.FirstCycle;
  int LastMemCycle = !if(!lt(MemCycles.LastCycle, 0),
                         FirstMemCycle, MemCycles.LastCycle);
}
