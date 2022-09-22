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

// CHECK:      /* 0 */       MCD::OPC_ExtractField, 0, 5,  // Inst{4-0} ...
// CHECK-NEXT: /* 3 */       MCD::OPC_FilterValue, 8, 25, 0, 0, // Skip to: 33
// CHECK-NEXT: /* 8 */       MCD::OPC_CheckField, 5, 1, 0, 4, 0, 0, // Skip to: 19
// CHECK-NEXT: /* 15 */      MCD::OPC_Decode, {{[0-9]+}}, {{[0-9]+}}, 0, // Opcode: InstA
// CHECK-NEXT: /* 19 */      MCD::OPC_CheckField, 7, 1, 0, 7, 0, 0, // Skip to: 33
// CHECK-NEXT: /* 26 */      MCD::OPC_TryDecode, {{[0-9]+}}, {{[0-9]+}}, 1, 0, 0, 0, // Opcode: InstB, skip to: 33
// CHECK-NEXT: /* 33 */      MCD::OPC_Fail,

// CHECK: if (!Check(S, DecodeRCCompleteRegisterClass(MI, tmp, Address, Decoder))) { return MCDisassembler::Fail; }
// CHECK: if (!Check(S, DecodeRCIncompleteRegisterClass(MI, tmp, Address, Decoder))) { DecodeComplete = false; return MCDisassembler::Fail; }
