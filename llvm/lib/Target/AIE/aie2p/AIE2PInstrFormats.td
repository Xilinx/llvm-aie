//===-- AIE2PPInsrFormats.td - AIE2p Instruction Formats --*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

include "AIEBaseInstrFormats.td"
include "aie2p/AIE2PSlots.td"

// Pseudo instructions
class Pseudo<dag outs, dag ins, string opcodestr = "",
             string argstr = "">
    : AIE2PInst<outs, ins, opcodestr, argstr>, AIEPseudoInstrAttributes {}

class AIE2P_inst_nop_instr16<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2PSlotInst<nop_slot, outs, ins, opcodestr, argstr> {
  bits<1> nop;
  bits<1> Inst = nop;
  
  let Size = 2;
}

class AIE2P_nop_nop_inst_nop < dag outs, dag ins, string opcodestr = "", string argstr = "">:
    AIE2P_inst_nop_instr16 <outs, ins, opcodestr, argstr> {

  let nop = {0b0};
}

// MultiSlot Pseudo instruction: Use this to define Pseudo for multi slot instructions
// This instruction could be materialized to different VLIW slots.
class MultiSlot_Pseudo<dag outs, dag ins, string opcodestr = "",
             string argstr = "", list<AIE2PInst> insts = []>
    : AIE2PInst<outs, ins, opcodestr, argstr>{
  let isPseudo = 1;
  bits<1> isMultiSlotPseudo = 1;
  list<AIE2PInst> materializableInto = insts;
  let isCodeGenOnly = 1;
  let isComposite = 0;
}
