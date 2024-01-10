//===--AIE2InstrFormats.td -AIEngine V2 Instruction Formats--*-tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

include "AIEBaseInstrFormats.td"
include "AIE2Slots.td"
include "AIE2GenInstrFormats.td"


// Pseudo instructions
class Pseudo<dag outs, dag ins, string opcodestr = "",
             string argstr = "">
    : AIE2Inst<outs, ins, opcodestr, argstr> {
  let isPseudo = 1;
  let isCodeGenOnly = 1;
  let isComposite = 0;
}

// MultiSlot Pseudo instruction: Use this to define Pseudo for multi slot instructions
// This instruction could be materialized to different VLIW slots.
class MultiSlot_Pseudo<dag outs, dag ins, string opcodestr = "",
             string argstr = "", list<AIE2Inst> insts = []>
    : AIE2Inst<outs, ins, opcodestr, argstr>{
  let isPseudo = 1;
  bits<1> isMultiSlotPseudo = 1;
  list<AIE2Inst> materializableInto = insts;
  let isCodeGenOnly = 1;
  let isComposite = 0;
}

class SplitPseudo<dag outs, dag ins> : Pseudo<outs, ins>, AIE_HasTiedSubregister {}
