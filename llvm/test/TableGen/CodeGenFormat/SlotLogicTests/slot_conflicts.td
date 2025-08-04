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
//  corresponding slots on typical AIE architectures.
// We compute conflicts from the totality of formats, and we  we should find conflicts
//  X -> {XM},
//  M -> {XM},
//  XM -> {X, M, XM}
// and nothing more. In particular, ConflictBits of A should be equal to its SlotBits

// CHECK-LABEL:   TESTSlotKind::TEST_SLOT_X,
// CHECK-NEXT:   "X",
// CHECK-NEXT:  10,
// CHECK-NEXT:  1,
// CHECK-NEXT:  5,
// CHECK-LABEL:  TESTSlotKind::TEST_SLOT_A,
// CHECK-NEXT:  "A",
// CHECK-NEXT:  10,
// CHECK-NEXT:  2,
// CHECK-NEXT:  2,
// CHECK-LABEL:  TESTSlotKind::TEST_SLOT_XM,
// CHECK-NEXT:  "XM",
// CHECK-NEXT:  20,
// CHECK-NEXT:  4,
// CHECK-NEXT:  13,
// CHECK-LABEL:  TESTSlotKind::TEST_SLOT_M,
// CHECK-NEXT:  "M",
// CHECK-NEXT:  10,
// CHECK-NEXT:  8,
// CHECK-NEXT:  12,
// CHECK-LABEL:  TESTSlotKind::TEST_SLOT_NOP,
// CHECK-NEXT:  "nop",
// CHECK-NEXT:  14,
// CHECK-NEXT:  16,
// CHECK-NEXT:  31,

include "llvm/Target/Target.td"
include "llvm/Target/CodeGenFormat.td"

let Namespace = "TEST" in {
def nop_slot 	 : InstSlot<"nop", 14> { let Artificial = true; }
def lda_slot     : InstSlot<"A", 10>;
def alu_slot     : InstSlot<"X", 10>;
def mov_slot     : InstSlot<"M", 10>;
def lng_slot     : InstSlot<"XM", 20>;
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

class TESTInst<dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
    : Instruction, InstFormat
{
  let Namespace = "TEST";

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = opcodestr # "\t" # argstr;
  let Pattern = pattern;

  let Slot = default_slot;
}

class TESTSubInst<dag outs, dag ins>
    : TESTInst<outs, ins, "", "", []>
{
  let isComposite = false;
}

class TESTPacketFormat<dag outs, dag ins> : TESTInst<outs, ins,"", "", []> {
  let isComposite = true;
}


class TEST_instr32<dag outs, dag ins> : TESTPacketFormat<outs, ins> {
  field bits<32> Inst;
  field bits<32> instr32;
  let Inst = {instr32};
}

def I64_A_XM : TEST_instr32<(outs), (ins lda_slot:$lda, alu_slot:$alu, lng_slot:$lng)> {
  bits<10> lda;
  bits<20> lng;
  let instr32 = {lda, lng, 0b00};

}

def I64_A_X_M : TEST_instr32<(outs), (ins lda_slot:$lda, alu_slot:$alu, mov_slot:$mov)> {
  bits<10> lda;
  bits<10> alu;
  bits<10> mov;
  let instr32 = {lda, alu, mov, 0b01};
}


