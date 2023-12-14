//===- trydecode-regclass.td -------------------------------*- tablegen -*-===//
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

// It might happen that a sequence of bits can correspond to multiple
// instructions if only looking at the fixed bits. This is problematic
// because llvm assumes that when the fixed bits are matched, then it for
// sure can decode the rest of the instruction (in particular, its operands).
//
// e.g. Inst = {0b000,0b01000};
// can match both {2bits_reg,0b0,0b01000} and {0b0,2bits_reg,0b01000}.
// To handle that case properly, one needs to mention which operands cannot
// be "completely" decoded, i.e. their decoder function isn't always expected
// to succeed.

// In the example below, InstA has an incomplete decoder for its register
// operand, so we expect an OPC_TryDecode for InstA, with a fallback to
// the InstB's decoder.

def RCComplete : ArchRegisterClass<(add r0,r1)>;
def RCIncomplete : ArchRegisterClass<(add r0,r1)> {
  let hasCompleteDecoder = 0;
}

def InstA : TestInstruction {
  bits<2> twobits_reg;
  field bits<3> sub1 = {twobits_reg,0b0};
  let Inst = {sub1,0b01000};
  let OutOperandList = (outs RCIncomplete:$twobits_reg);
  let AsmString = "InstA";
}

def InstB : TestInstruction {
  bits<2> twobits_reg;
  field bits<3> sub1 = {0b0,twobits_reg};
  let Inst = {sub1,0b01000};
  let InOperandList = (ins RCComplete:$twobits_reg);
  let AsmString = "InstB";
}

// CHECK:      /* 0 */       MCD::OPC_ExtractField, 0, 5,  // Inst{4-0} ...
// CHECK-NEXT: /* 3 */       MCD::OPC_FilterValue, 8, 25, 0, 0, // Skip to: 33
// CHECK-NEXT: /* 8 */       MCD::OPC_CheckField, 5, 1, 0, 7, 0, 0, // Skip to: 22
// CHECK-NEXT: /* 15 */      MCD::OPC_TryDecode, {{[0-9]+}}, {{[0-9]+}}, 0, 0, 0, 0, // Opcode: InstA, skip to: 22
// CHECK-NEXT: /* 22 */      MCD::OPC_CheckField, 7, 1, 0, 4, 0, 0, // Skip to: 33
// CHECK-NEXT: /* 29 */      MCD::OPC_Decode, {{[0-9]+}}, {{[0-9]+}}, 1, // Opcode: InstB
// CHECK-NEXT: /* 33 */      MCD::OPC_Fail,

// CHECK: if (!Check(S, DecodeRCIncompleteRegisterClass(MI, tmp, Address, Decoder))) { DecodeComplete = false; return MCDisassembler::Fail; }
// CHECK: if (!Check(S, DecodeRCCompleteRegisterClass(MI, tmp, Address, Decoder))) { return MCDisassembler::Fail; }
