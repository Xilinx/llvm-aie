//===- decode-zerobit-operand.td ---------------------------*- tablegen -*-===//
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

// For instruction operands in singleton register classes (here, R1), there
// might be no bits needed to encode the register in the instruction encoding.
// -gen-disassembler will typically ignore operands without encoding bits, and
// actually create an MCInst with less operands than the definition in .td
// states.
//
// This is usually fine if the "zero-bit" operand is last. However, when it
// isn't, it will offset the indices of the operands that come afterwards.
// E.g. in the example below, InstA will have $s3 as its second operand in
// its MCInst, whereas it should be third.
// The problem is fixed in InstB, where $s2 still gets decoded, even though
// it has no encoding bits.

def R : ArchRegisterClass<(add r0,r1)>;
def R1_NoDecode : ArchRegisterClass<(add r1)> {
  let DecodeZeroBitOperand = false;
}
def R1_Decode : ArchRegisterClass<(add r1)> {
  let DecodeZeroBitOperand = true;
}

def InstA : TestInstruction {
  bits<2> s1;
  bits<2> s3;
  let Inst = {s1,s3,0b1000};
  let InOperandList = (ins R:$s1, R1_NoDecode:$s2, R:$s3);
  let AsmString = "InstA";
}

def InstB : TestInstruction {
  bits<2> s1;
  bits<2> s3;
  let Inst = {s1,s3,0b1100};
  let InOperandList = (ins R:$s1, R1_Decode:$s2, R:$s3);
  let AsmString = "InstB";
}

// CHECK:      static const uint8_t DecoderTable8[] = {
// CHECK-NEXT: /* 0 */       MCD::OPC_ExtractField, 0, 4,  // Inst{3-0} ...
// CHECK-NEXT: /* 3 */       MCD::OPC_FilterValue, 8, 4, 0, 0, // Skip to: 12
// CHECK-NEXT: /* 8 */       MCD::OPC_Decode, {{[0-9]+}}, {{[0-9]+}}, 0, // Opcode: InstA
// CHECK-NEXT: /* 12 */      MCD::OPC_FilterValue, 12, 4, 0, 0, // Skip to: 21
// CHECK-NEXT: /* 17 */      MCD::OPC_Decode, {{[0-9]+}}, {{[0-9]+}}, 1, // Opcode: InstB
// CHECK-NEXT: /* 21 */      MCD::OPC_Fail,

// CHECK-LABEL: static DecodeStatus decodeToMCInst
// CHECK:       case 0:
// CHECK-NEXT:    tmp = fieldFromInstruction(insn, 6, 2);
// CHECK-NEXT:    if (!Check(S, DecodeRRegisterClass(MI, tmp, Address, Decoder))) { return MCDisassembler::Fail; }
// CHECK-NEXT:    tmp = fieldFromInstruction(insn, 4, 2);
// CHECK-NEXT:    if (!Check(S, DecodeRRegisterClass(MI, tmp, Address, Decoder))) { return MCDisassembler::Fail; }
// CHECK-NEXT:    return S;
// CHECK-NEXT:  case 1:
// CHECK-NEXT:    tmp = fieldFromInstruction(insn, 6, 2);
// CHECK-NEXT:    if (!Check(S, DecodeRRegisterClass(MI, tmp, Address, Decoder))) { return MCDisassembler::Fail; }
// CHECK-NEXT:    if (!Check(S, DecodeR1_DecodeRegisterClass(MI, insn, Address, Decoder))) { return MCDisassembler::Fail; }
// CHECK-NEXT:    tmp = fieldFromInstruction(insn, 4, 2);
// CHECK-NEXT:    if (!Check(S, DecodeRRegisterClass(MI, tmp, Address, Decoder))) { return MCDisassembler::Fail; }
// CHECK-NEXT:    return S;
