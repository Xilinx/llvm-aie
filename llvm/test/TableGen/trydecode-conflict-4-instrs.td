// RUN: llvm-tblgen -gen-disassembler -I %p/../../include -I %p/Common %s | FileCheck %s
include "arch-with-regs.td"

// It might happen that a sequence of bits can correspond to multiple
// instructions if only looking at the fixed bits. This is problematic
// because llvm assumes that when the fixed bits are matched, then it for
// sure can decode the rest of the instruction (in particular, its operands).
//
// e.g. Inst = {0b0000000000,0b111011111111,0b0000001001};
// can match any of the following instrcutions:
// {0b0000000000, 0b1110, 4bits_reg, 4bits_reg, 0b0000001001}
// {0b0000000000, 4bits_reg, 4bits_reg, 4bits_reg, 0b0000001001}
// {0b0000000000, 0b11, 2bits_reg, 0b11, 2bits_reg, 4bits_reg, 0b0000001001}
// {0b0000000000, 4bits_reg, 0b11, 2bits_reg, 4bits_reg, 0b0000001001}
// To handle that case properly, one needs to mention which operands cannot
// be "completely" decoded, i.e. their decoder function isn't always expected
// to succeed.

// In the example below, all 4 instructions have incomplete decoders for
// their register operands so we expect an OPC_TryDecode for each of the 4
// instructions.


let hasCompleteDecoder = 0 in {
    def AIncomplete : Arch1RegisterClass4Bits<(add a0)>;
    def BIncomplete : Arch1RegisterClass4Bits<(add b0)>;
    def CIncomplete : Arch1RegisterClass2Bits<(add c0)>;
    def DIncomplete : Arch1RegisterClass2Bits<(add d0)>;
}

def Inst1 : Arch1TestInstruction {
  bits<4> acc2;
  bits<4> dst;

  let Inst = {0b000000000, 0b0, 0b1110, acc2, dst, 0b0000001001};
  let InOperandList = (ins AIncomplete:$acc2);
  let OutOperandList = (outs AIncomplete:$dst);
  let AsmString = "Inst1";
}

def Inst2 : Arch1TestInstruction {
  bits<4> s1;
  bits<4> s2;
  bits<4> dst;

  let Inst = {0b000000000, 0b0, s1, s2, dst, 0b0000001001};
  let InOperandList = (ins BIncomplete:$s1, BIncomplete:$s2);
  let OutOperandList = (outs AIncomplete:$dst);
  let AsmString = "Inst2";
}

def Inst3 : Arch1TestInstruction {
  bits<2> ys1;
  bits<2> qxs2;
  bits<4> dst;

  let Inst = {0b000000000, 0b0, 0b11, ys1, 0b11, qxs2, dst, 0b0000001001};
  let InOperandList = (ins DIncomplete:$ys1, CIncomplete:$qxs2);
  let OutOperandList = (outs AIncomplete:$dst);
  let AsmString = "Inst3";
}

def Inst4 : Arch1TestInstruction {
  bits<4> xs1;
  bits<2> qxs2;
  bits<4> dst;

  let Inst = {0b000000000, 0b0, xs1, 0b11, qxs2, dst, 0b0000001001};
  let InOperandList = (ins BIncomplete:$xs1, CIncomplete:$qxs2);
  let OutOperandList = (outs AIncomplete:$dst);
  let AsmString = "Inst4";
}

// First we check the fixed bits that are in common between the 4 insts.
//   First we extract and check the 10 least significant bits (comparison with 9)
//   Second we extract and check the 10 most significant bits (comparison with 0)
//   if any of the above checks fails we skip to OPC_FAIL
// Then we check the first 4 bits in Inst{18-21} using OPC_CheckField (comparison with 14)
// Based on the last check we try to decode Inst1.
// If Inst1 fails, we check the 2 bits in Inst{17-16} (comparison with 3).
//   If this fails we skip to try to decode Inst2.
//   If this succeeds, we try check the 2 bits in Inst{20-21} and we try to decode Inst3
//   then Inst4 if the latter fails. If both fails we try Inst1. If it fails we skip to OPC_FAIL.

// CHECK:      /* 0 */       MCD::OPC_ExtractField, 0, 10,  // Inst{9-0} ...
// CHECK-NEXT: /* 3 */       MCD::OPC_FilterValue, 9, 58, 0, 0, // Skip to: 66
// CHECK-NEXT: /* 8 */       MCD::OPC_ExtractField, 22, 10,  // Inst{31-22} ...
// CHECK-NEXT: /* 11 */      MCD::OPC_FilterValue, 0, 50, 0, 0, // Skip to: 66
// CHECK-NEXT: /* 16 */      MCD::OPC_CheckField, 18, 4, 14, 7, 0, 0, // Skip to: 30
// CHECK-NEXT: /* 23 */      MCD::OPC_TryDecode, {{[0-9]+}}, {{[0-9]+}}, 0, 0, 0, 0, // Opcode: Inst1, skip to: 30
// CHECK-NEXT: /* 30 */      MCD::OPC_ExtractField, 16, 2,  // Inst{17-16} ...
// CHECK-NEXT: /* 33 */      MCD::OPC_FilterValue, 3, 21, 0, 0, // Skip to: 59
// CHECK-NEXT: /* 38 */      MCD::OPC_CheckField, 20, 2, 3, 7, 0, 0, // Skip to: 52
// CHECK-NEXT: /* 45 */      MCD::OPC_TryDecode, {{[0-9]+}}, {{[0-9]+}}, 1, 0, 0, 0, // Opcode: Inst3, skip to: 52
// CHECK-NEXT: /* 52 */      MCD::OPC_TryDecode, {{[0-9]+}}, {{[0-9]+}}, 2, 0, 0, 0, // Opcode: Inst4, skip to: 59
// CHECK-NEXT: /* 59 */      MCD::OPC_TryDecode, {{[0-9]+}}, {{[0-9]+}}, 3, 0, 0, 0, // Opcode: Inst2, skip to: 66
// CHECK-NEXT: /* 66 */      MCD::OPC_Fail,

// CHECK: if (!Check(S, DecodeAIncompleteRegisterClass(MI, tmp, Address, Decoder))) { DecodeComplete = false; return MCDisassembler::Fail; }
// CHECK: if (!Check(S, DecodeDIncompleteRegisterClass(MI, tmp, Address, Decoder))) { DecodeComplete = false; return MCDisassembler::Fail; }
// CHECK: if (!Check(S, DecodeCIncompleteRegisterClass(MI, tmp, Address, Decoder))) { DecodeComplete = false; return MCDisassembler::Fail; }
// CHECK: if (!Check(S, DecodeBIncompleteRegisterClass(MI, tmp, Address, Decoder))) { DecodeComplete = false; return MCDisassembler::Fail; }
