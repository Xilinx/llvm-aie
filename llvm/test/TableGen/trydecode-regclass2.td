//===- trydecode-regclass2.td ------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: llvm-tblgen -gen-disassembler -I %p/../../include -I %p/Common %s | FileCheck %s

include "arch-with-regs.td"

// Similar to trydecode-regclass.td, expect here InstB has the incomplete
// decoder for its register operand. Notice that that the instructions are
// still ordered the same way, so the OPC_TryDecode isn't strictly needed
// since InstA is decoded first.

def RCComplete : ArchRegisterClass<(add r0,r1)>;
def RCIncomplete : ArchRegisterClass<(add r0,r1)> {
  let hasCompleteDecoder = 0;
}

def InstA : TestInstruction {
  bits<2> twobits_reg;
  field bits<3> sub1 = {twobits_reg,0b0};
  let Inst = {sub1,0b01000};
  let OutOperandList = (outs RCComplete:$twobits_reg);
  let AsmString = "InstA";
}

def InstB : TestInstruction {
  bits<2> twobits_reg;
  field bits<3> sub1 = {0b0,twobits_reg};
  let Inst = {sub1,0b01000};
  let InOperandList = (ins RCIncomplete:$twobits_reg);
  let AsmString = "InstB";
}

// CHECK:      OPC_CheckField, 0, 5, 8,                   //  0: check Inst[4:0] == 0x8
// CHECK-NEXT:   OPC_Scope, 8,                              //  4: try {
// CHECK-NEXT:   OPC_CheckField, 5, 1, 0,                   //  6:  check Inst[5] == 0x0
// CHECK-NEXT:   OPC_Decode, {{[0-9, ]+}}, 0,                     // 10:  decode to InstA using decoder 0
// CHECK-NEXT:                                              // 10: } else try {
// CHECK-NEXT:   OPC_CheckField, 7, 1, 0,                   // 14:  check Inst[7] == 0x0
// CHECK-NEXT:   OPC_Decode, {{[0-9, ]+}}, 1,                     // 18:  decode to InstB using decoder 1
// CHECK-NEXT:                                              // 18: }

// CHECK: if (!Check(S, DecodeRCCompleteRegisterClass(MI, tmp, Address, Decoder))) { return MCDisassembler::Fail; }
// CHECK: if (!Check(S, DecodeRCIncompleteRegisterClass(MI, tmp, Address, Decoder))) { DecodeComplete = false; return MCDisassembler::Fail; }
