//===-- AIEBaseTarget.td - Shared AIE target definition ----*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Shared TableGen compilation for the canonical AIE:: opcode namespace.
// All architecture-specific AIE targets are expected to produce identical
// enum values for:
//   1. LLVM generic opcodes from TargetOpcodes.def
//   2. AIE generic opcodes from AIEInstrGISel.td
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"
include "llvm/Target/AIETarget.td"
include "llvm/Target/CodeGenFormat.td"

// Minimal register definition required by the gen-instr-info backend.
// Only enum values from this compilation are consumed.
def AIEBaseTargetReg : Register<"r0">;
def AIEBaseTargetGPR
    : RegisterClass<"AIE", [i32], 32, (add AIEBaseTargetReg)>;

class AIEGenericInstruction : GenericInstruction {
  let Namespace = "AIEBase";
}
include "AIEInstrGISel.td"

// Shared pseudo instructions present in all AIE targets.
// Must be defined here so AIEBase enum values match the subtargets.
let Namespace = "AIEBase", isPseudo = true, isCodeGenOnly = true in {
  def ADJCALLSTACKDOWN : Instruction {
    let hasSideEffects = false;
    let mayLoad = false;
    let mayStore = false;
    let OutOperandList = (outs);
    let InOperandList = (ins i32imm:$amt1, i32imm:$amt2);
    let AsmString = "";
    let Pattern = [];
  }
  def ADJCALLSTACKUP : Instruction {
    let hasSideEffects = false;
    let mayLoad = false;
    let mayStore = false;
    let OutOperandList = (outs);
    let InOperandList = (ins i32imm:$amt1, i32imm:$amt2);
    let AsmString = "";
    let Pattern = [];
  }
  def CYCLE_SEPARATOR : Instruction {
    let hasSideEffects = false;
    let mayLoad = false;
    let mayStore = false;
    let OutOperandList = (outs);
    let InOperandList = (ins);
    let AsmString = "";
    let Pattern = [];
    let isMeta = true;
  }
  def DelayedSchedBarrier : Instruction {
    let hasSideEffects = false;
    let mayLoad = false;
    let mayStore = false;
    let OutOperandList = (outs);
    let InOperandList = (ins);
    let AsmString = "";
    let Pattern = [];
    let isMeta = true;
  }
}

def AIEBaseTargetInstrInfo : InstrInfo {
  let guessInstructionProperties = 0;
}

def AIEBaseTarget : Target {
  let InstructionSet = AIEBaseTargetInstrInfo;
}
