//===- AIE2PCompositeFormatsInclude.td -----------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

class AIE2P_instr16_Composite<dag slot_ins> : AIE2PCompositeInst<slot_ins>
{
  bits<16> Inst;
  bits<1> instr16;
  let Inst = {instr16, dontcare{11-1}, 0b0000};
  let Size = 2;
}

class AIE2P__instr16__nop<dag slot_ins>
  : AIE2P_instr16_Composite<slot_ins>
{
  // Only slot
  bits<1> nop16;
  // There is just one insn which is nop
  let instr16 = {nop16};
}

class AIE2P__instr64__nop__alu__vec<dag slot_ins>
  : AIE2P_instr64_Composite<slot_ins>
{
  // Base Slots
  bits<20> alu;
  bits<1> nop;
  bits<26> vec;
  // Intermediate fields
  bits<60> inst_alu_vec;
  // Hierarchical definition
  let instr64 = {inst_alu_vec};
  let inst_alu_vec = {nop, 0b000000000, alu, 0b01, vec, 0b10};
}

// nop_slot has all don't care bits
def I16_NOP : AIE2P__instr16__nop<(ins nop_slot:$nop16)>;
