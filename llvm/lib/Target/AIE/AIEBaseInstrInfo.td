//===-- AIEBaseInstrInfo.td - Target Description for AIE ---*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file constains geneirc utility classes for describing one of the
// the AIEngine ISAs in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// AIEngine specific DAG Nodes.
//===----------------------------------------------------------------------===//

// Note: decode*Operand functions are implemented in AIEDisassembler.
// Node: getSImmOpValue* functions are implemented in AIEMCCodeEmitter.

// Regular unsigned immediates.
class txxu <int n, ValueType vt> : Operand<vt>, ImmLeaf<vt, [{return isUInt<}] # n # [{>(Imm);}]> {
// let ParserMatchClass = UImmAsmOperand<" # n # ">;
  let DecoderMethod = "decodeUImmOperand<" # n # ">";
}

// Regular signed immediates.
class txxs <int n, ValueType vt> : Operand<vt>, ImmLeaf<vt, [{return isInt<}] # n # [{>(Imm);}]> {
// let ParserMatchClass = SImmAsmOperand<" # n # ">;
  let DecoderMethod = "decodeSImmOperand<" # n # ">";
}
// Regular unsigned target immediates.
class timm_txxu <int n> : Operand<i32>, TImmLeaf<i32, [{return isUInt<}] # n # [{>(Imm);}]> {
// let ParserMatchClass = UImmAsmOperand<" # n # ">;
  let DecoderMethod = "decodeUImmOperand<" # n # ">";
}
// Regular signed target immediates.
class timm_txxs <int n> : Operand<i32>, TImmLeaf<i32, [{return isInt<}] # n # [{>(Imm);}]> {
// let ParserMatchClass = SImmAsmOperand<" # n # ">;
  let DecoderMethod = "decodeSImmOperand<" # n # ">";
}

// A signed immediate with n bits, with value always negative
class immx1_negative <int n, ValueType vt> : Operand<vt>, ImmLeaf<vt, [{return isInt<}] # n # [{+1>(Imm);}]> {
  let DecoderMethod = "decodeSImmOperandX1<" # n # ">";
}

// A signed (or signed negative) immediate with n+log2(step) + 1 (if negative) bits
// and the low-order log2(step) bits as 0. This results in an n-bit encoded number
class immxstep <int n, int step, int fixedEncodedBits, ValueType vt = i32>
    : Operand<vt>, ImmLeaf<vt, [{return isInt<}] # n # [{+}] # fixedEncodedBits # [{>(Imm) && (Imm % }] # step # [{) == 0;}]> {
  bits<1> isNegative = false;
  let DecoderMethod = "decodeSImmOperandXStep<" # n # ", "#step#", "# isNegative #">";
  let EncoderMethod = "getSImmOpValueX"#step#"<" # n #", /*offset=*/0, "# isNegative #">";
}

// A signed immediate with n+2 bits and the low-order 2 bits as 0.
// This results in an n-bit encoded number
class immx4 <int n, ValueType vt>
    : immxstep<n, 4 /*step*/, 2 /*lower-order as 0*/, vt>
{
  let PrintMethod = "printImmOffset</*offset=*/0>";
}

// A signed negative immediate with n+3 bits and the low-order 2 bits as 0 and sign-bit as 1.
// This results in an n-bit encoded number
class immx4_negative <int n, ValueType vt>
    : immxstep<n, 4 /*step*/, 3 /*1 sign-bit + 2 lower-order bits as 0*/, vt>
{
  let isNegative = true;
  let PrintMethod = "printImmOffset</*offset=*/0>";
}

// A signed immediate with n+4 bits and the low-order 4 bits as 0.
// This results in an n-bit encoded number
class immx16 <int n, ValueType vt = i32> : immxstep<n, 16 /*step*/, 4 /**lower-order as 0**/, vt>;

// A signed negative immediate with n+5 bits and the low-order 4 bits as 0 and sign-bit as 1.
// This results in an n-bit encoded number
class immx16_negative <int n> : immxstep<n, 16 /*step*/, 5 /**1 sign-bit + 4 lower-order bits as 0**/>
{
  let isNegative = true;
}

// A signed immediate with n+5 bits and the low-order 5 bits as 0.
// This results in an n-bit encoded number
class immx32 <int n, ValueType vt = i32> : immxstep<n, 32 /**step**/, 5 /**lower-order as 0**/, vt>;

// A signed negative immediate with n+6 bits and the low-order 5 bits as 0 and sign-bit as 1.
// This results in an n-bit encoded number
class immx32_negative <int n> : immxstep<n, 32 /*step*/, 6 /**1 sign-bit + 5 lower-order bits as 0**/>
{
  let isNegative = true;
}

// A signed immediate which fits in n bits after being negated.
class txxsn <int n, ValueType vt> : Operand<vt>, ImmLeaf<vt, [{return isInt<}] # n # [{>(-Imm);}]> {
  let DecoderMethod = "decodeSImmOperand<" # n # ">";
}

// Instructions that have tied subregister and have to set
// hasExtraSrcRegAllocReq and hasExtraDefRegAllocReq to prevent the registers
// from being renamed post-RA.
// Note: Since this class has to override the two values it has to be inherited
// after AIEBaseInstr or one of the classes that inherits from it.
class AIE_HasTiedSubregister {
  bit hasExtraSrcRegAllocReq = true;
  bit hasExtraDefRegAllocReq = true;
}
