//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//

// RUN: llvm-tblgen --gen-instr-format -I %p -I %p/../ -I %p/../../../../include %s 2>&1 | FileCheck %s

// This is a minimal check for the computations of the conflict bits.
// We have the slots A. X, M and XM, which interfere like the
// corresponding slots on typical AIE architectures.
// We compute conflicts from the totality of formats, and we should find
// conflicts
//  X -> {X, XM},
//  M -> {M, XM},
//  XM -> {X, M, XM}
//  A -> {A}
// and nothing more.

// CHECK-LABEL:   "X",
// CHECK-NEXT:  8,
// CHECK-NEXT:  1,
// CHECK-NEXT:  5,
// CHECK-LABEL:  "A",
// CHECK-NEXT:  10,
// CHECK-NEXT:  2,
// CHECK-NEXT:  2,
// CHECK-LABEL:  "XM",
// CHECK-NEXT:  18,
// CHECK-NEXT:  4,
// CHECK-NEXT:  13,
// CHECK-LABEL:  "M",
// CHECK-NEXT:  10,
// CHECK-NEXT:  8,
// CHECK-NEXT:  12,
// CHECK-LABEL:  "nop",
// CHECK-NEXT:  14,
// CHECK-NEXT:  16,
// CHECK-NEXT:  31,

include "llvm/Target/Target.td"
include "llvm/Target/CodeGenFormat.td"

let Namespace = "TEST" in {
def nop_slot 	 : InstSlot<"nop", 14> { let Artificial = true; }
def lda_slot     : InstSlot<"A", 10>;
def alu_slot     : InstSlot<"X", 8>;
def mov_slot     : InstSlot<"M", 10>;
def lng_slot     : InstSlot<"XM", 18>;
}


def TESTInstrInfo : InstrInfo;

def TEST : Target {
  let InstructionSet = TESTInstrInfo;
}

def TESTCodeGenFormat : CodeGenFormat {
  let FormatClassEmitted = "";
}

let Namespace = "TEST" in
def default_slot : InstSlot<"default", 0, true>;

class TESTInst<dag outs, dag ins, string opc, string arg, list<dag> pattern>
    : Instruction, InstFormat
{
  let Namespace = "TEST";

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = opc # "\t" # arg;
  let Pattern = pattern;

  let Slot = default_slot;
}

class TESTSubInst<dag outs, dag ins>
    : TESTInst<outs, ins, "", "", []>
{
  let isComposite = false;
}

class TESTFormat<dag outs, dag ins> : TESTInst<outs, ins, "", "", []> {
  let isComposite = true;
}


class TEST_I32<dag outs, dag ins> : TESTFormat<outs, ins> {
  field bits<32> Inst;
  field bits<32> instr32;
  let Inst = {instr32};
}

def I32_A_XM : TEST_I32<(outs), (ins lda_slot:$lda, lng_slot:$lng)> {
  bits<10> lda;
  bits<18> lng;
  let instr32 = {lda, lng, 0b1001};
}

def I32_A_X : TEST_I32<(outs), (ins lda_slot:$lda, alu_slot:$alu)> {
  bits<10> lda;
  bits<8> alu;
  let instr32 = {lda, alu, 0b0000000000, 0b1001};
}

def I32_X_M : TEST_I32<(outs), (ins alu_slot:$alu, mov_slot:$mov)> {
  bits<8> alu;
  bits<10> mov;
  let instr32 = {0b0000000000, alu, mov, 0b0110};
}

def I32_A_X_M : TEST_I32<(outs), 
        (ins lda_slot:$lda, alu_slot:$alu, mov_slot:$mov)> {
  bits<10> lda;
  bits<8> alu;
  bits<10> mov;
  let instr32 = {lda, alu, mov, 0b1110};
}

