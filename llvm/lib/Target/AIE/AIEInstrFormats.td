//===-- AIEInstrFormats.td - AIE Instruction Formats ---*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//
//  These instruction format definitions are structured to match the
//  description in the AIEngine User-Level ISA specification as closely as
//  possible. For instance, the specification describes instructions with the
//  MSB (31st bit) on the left and the LSB (0th bit) on the right. This is
//  reflected in the order of parameters to each instruction class.
//
//  One area of divergence is in the description of immediates. The
//  specification describes immediate encoding in terms of bit-slicing
//  operations on the logical value represented. The immediate argument to
//  these instruction formats instead represents the bit sequence that will be
//  inserted into the instruction. e.g. although JAL's immediate is logically
//  a 21-bit value (where the LSB is always zero), we describe it as an imm20
//  to match how it is encoded.
//
//===----------------------------------------------------------------------===//

// Slot definitions for describing high-level instruction formats. This
// architecture uses a hierarchical decomposition in which more complex VLIW
// instructions are defined in terms of simpler instructions. At the MC level,
// Toplevel instructions may have simpler instructions attached to them as
// operands. This avoids describing the same instruction in multiple places
// and also avoids a cross-product explosion of instruction specifications for
// VLIW formats.

include "AIEBaseInstrFormats.td"

let Namespace = "AIE" in
{
  def unknown_slot : InstSlot<"unknown", 0, true>;

  // Temporary slot (until all instructions instr16 are rewritten in a
  // canonical format)
  // FIXME: rewrite instructions and remove me
  def instr16_slot : InstSlot<"instr16", 14> {
    // Explicit name of the field to which the slot binds in the hierarchy
    let FieldToFind = "instr16";
  }

  def alu_slot     : InstSlot<"alu", 19> {
    let DecoderMethod = "decodeAluInstruction";
  }

  def lng_slot     : InstSlot<"lng", 28> {
    let DecoderMethod = "decodeLngInstruction";
  }

  def mv1ssb_slot  : InstSlot<"mv1ssb", 7> {
    let DecoderMethod = "decodeMv1ssbInstruction";
  }

  def mv0_slot     : InstSlot<"mv0", 16> {
    let DecoderMethod = "decodeMv0Instruction";
  }

  def mv1_slot     : InstSlot<"mv1", 13> {
    let DecoderMethod = "decodeMv1Instruction";
  }

  def lda_slot     : InstSlot<"lda", 18> {
    let DecoderMethod = "decodeLdaInstruction";
  }

  def ldb_slot     : InstSlot<"ldb", 17> {
    let DecoderMethod = "decodeLdbInstruction";
  }

  def st_slot      : InstSlot<"st", 20> {
    let DecoderMethod = "decodeStInstruction";
  }

  def vecshft_slot : InstSlot<"vecshft",  6> {
    let DecoderMethod = "decodeVecShftInstruction";
  }

  def vecstrm_slot : InstSlot<"vecstrm",  5> {
    let DecoderMethod = "decodeVecStrmInstruction";
  }

  def vecshrt_slot : InstSlot<"vecshrt", 11> {
    let DecoderMethod = "decodeVecShrtInstruction";
  }

  def vecm_slot    : InstSlot<"vecm", 25> {
    let DecoderMethod = "decodeVecMedInstruction";
  }

  def veca_slot    : InstSlot<"veca", 37> {
    let DecoderMethod = "decodeVecAllInstruction";
  }
}

class AIEOpcode_ALU_R_RR<bits<4> val> {
  bits<4> Value = val;
}

// Most of these instructions read and write in E1.
def OPC_ALU_R_RR_ADD      : AIEOpcode_ALU_R_RR<0b0000>;
def OPC_ALU_R_RR_SUB      : AIEOpcode_ALU_R_RR<0b0001>;
def OPC_ALU_R_RR_ADDX     : AIEOpcode_ALU_R_RR<0b0010>;
def OPC_ALU_R_RR_SUBX     : AIEOpcode_ALU_R_RR<0b0011>;
def OPC_ALU_R_RR_AND      : AIEOpcode_ALU_R_RR<0b0100>;
def OPC_ALU_R_RR_OR       : AIEOpcode_ALU_R_RR<0b0101>;
def OPC_ALU_R_RR_XOR      : AIEOpcode_ALU_R_RR<0b0110>;
def OPC_ALU_R_RR_MUL      : AIEOpcode_ALU_R_RR<0b0111>; // read in E1, write in E3.
def OPC_ALU_R_RR_SEQ      : AIEOpcode_ALU_R_RR<0b1000>;
def OPC_ALU_R_RR_SNE      : AIEOpcode_ALU_R_RR<0b1001>;
def OPC_ALU_R_RR_SGE      : AIEOpcode_ALU_R_RR<0b1010>;
def OPC_ALU_R_RR_SLT      : AIEOpcode_ALU_R_RR<0b1011>;
def OPC_ALU_R_RR_SGEU     : AIEOpcode_ALU_R_RR<0b1100>;
def OPC_ALU_R_RR_SLTU     : AIEOpcode_ALU_R_RR<0b1101>;
def OPC_ALU_R_RR_UNS      : AIEOpcode_ALU_R_RR<0b1110>;
def OPC_ALU_R_RR_SGN      : AIEOpcode_ALU_R_RR<0b1111>;

class AIEOpcode_ALU_R_R<bits<4> val> {
  bits<4> Value = val;
}
def OPC_ALU_R_R_CLB      : AIEOpcode_ALU_R_R<0b0000>;
def OPC_ALU_R_R_CLBU      : AIEOpcode_ALU_R_R<0b0001>;
def OPC_ALU_R_R_SE8     : AIEOpcode_ALU_R_R<0b0010>;
def OPC_ALU_R_R_SE16     : AIEOpcode_ALU_R_R<0b0011>;
def OPC_ALU_R_R_ZE8      : AIEOpcode_ALU_R_R<0b0100>;
def OPC_ALU_R_R_ZE16       : AIEOpcode_ALU_R_R<0b0101>;
def OPC_ALU_R_R_SEQ0      : AIEOpcode_ALU_R_R<0b0110>;
def OPC_ALU_R_R_SNE0      : AIEOpcode_ALU_R_R<0b0111>;
def OPC_ALU_R_R_ABS      : AIEOpcode_ALU_R_R<0b1000>;

class AIEInst<dag outs, dag ins, string opcodestr, string argstr,
              list<dag> pattern>
    : AIEBaseInst<outs, ins, pattern>
{
  let Namespace = "AIE";

  let AsmString = opcodestr # "\t" # argstr;

  // Default value of the slot
  let Slot = unknown_slot;
}

// Pseudo instructions
class Pseudo<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIEInst<outs, ins, opcodestr, argstr, pattern> {
  let isPseudo = 1;
  let isCodeGenOnly = 1;
}

//////////////////////////
// Instruction classes

// SoftFail is a field the disassembler can use to provide a way for
// instructions to not match without killing the whole decode process. It is
// mainly used for ARM, but Tablegen expects this field to exist or it fails
// to build the decode table.

// Note that some information about these instruction formats is duplicated in
// AIEDisassembler.cpp. This should probably be consolidated here.
class AIE_instr16<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIEInst<outs, ins, opcodestr, argstr, pattern> {
  field bits<16> SoftFail = 0;
  field bits<16> Inst;
  field bits<14> instr16;
  let Size = 2; // 2 Byte instruction
  let Inst = {instr16, 0b01};
  // FIXME: remove me when all instr16 will be rewritten into their canonical
  // form
  let Slot = instr16_slot;
}

class AIE_instr32<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIEInst<outs, ins, opcodestr, argstr, pattern> {
  field bits<32> SoftFail = 0;
  field bits<32> Inst;
  field bits<29> instr32;
  let Inst = {instr32, 0b011};
}

class AIE_instr64<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIEInst<outs, ins, opcodestr, argstr, pattern> {
  field bits<64> SoftFail = 0;
  field bits<64> Inst;
  field bits<60> instr64;
  let Size = 8; // 8 Byte instruction
  let Inst = {instr64, 0b0111};
}

class AIE_instr96<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIEInst<outs, ins, opcodestr, argstr, pattern> {
  field bits<96> SoftFail = 0;
  field bits<96> Inst;
  field bits<92> instr96;
  let Size = 12; // 12 Byte instruction
  let Inst = {instr96, 0b1111};
}

class AIE_instr128<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIEInst<outs, ins, opcodestr, argstr, pattern> {
  field bits<128> SoftFail = 0;
  field bits<128> Inst;
  field bits<127> instr128;
  let Size = 16; // 12 Byte instruction
  let Inst = {instr128, 0b0};
}

//////////////////////////
// 16-bit instructions

class AIE_alu_r_r<bits<4> opcode, string opcodestr = "", string argstr = "$d0, $s0">
    : AIE_instr16<(outs GPR:$d0), (ins GPR:$s0), [], opcodestr, argstr> {
   bits<4> ALU_opcode = opcode;
   bits<4> s0;
   bits<4> d0;
   let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
   let instr16 = {opcode, s0, d0, 0b11};
}

class AIE_ja_instr<dag outs, dag ins, list<dag> pattern, string opcodestr = "ja", string argstr = "$dst">
    : AIE_instr16<outs, ins, pattern, opcodestr, argstr> {
   bits<3> dst;
   let instr16 = {dst, 0b00000 /*XXXXX*/, 0b100000};
   let isIndirectBranch = 1;
}

class AIE_jal_ind_instr<dag outs, dag ins, list<dag> pattern, string opcodestr = "ja", string argstr = "$reg">
    : AIE_instr16<outs, ins, pattern, opcodestr, argstr> {
   bits<3> reg;
   let instr16 = {reg, 0b00000 /*XXXXX*/, 0b010000};
   let isIndirectBranch = 1;
}

class AIE_mv_special2r_instr16<dag outs, dag ins, list<dag> pattern, string opcodestr = "mov", string argstr = "$d0, $s0">
    : AIE_instr16<outs, ins, pattern, opcodestr, argstr> {
   bits<6> s0;
   bits<4> d0;
   let instr16 = {s0, 0b01, d0, 0b10};
}

class AIE_mv_r2special_instr16<dag outs, dag ins, list<dag> pattern, string opcodestr = "mov", string argstr = "$d0, $s0">
    : AIE_instr16<outs, ins, pattern, opcodestr, argstr> {
   bits<4> s0;
   bits<6> d0;
   let instr16 = {s0, d0, 0b1000};
}

class AIE_Accumulator_mv_a<dag outs, dag ins, list<dag> pattern, string opcodestr = "vmov", string argstr = "$d0, $s0">
    : AIE_instr16<outs, ins, pattern, opcodestr, argstr> {
   bits<3> s0;
   bits<3> d0;
   let instr16 = {0b010 /*X1X*/, s0{2}, s0{1-0}, 0b000 /*00X*/, d0{2}, d0{1-0}, 0b10};
}

class AIE_lda_ptr_inc_instr16<dag outs, dag ins, list<dag> pattern, string opcodestr = " ", string argstr = " ">
    : AIE_instr16<outs, ins, pattern, opcodestr, argstr> {
  bits<11> lda_ptr_inc;
	bits<11> ag_a_standalone;
	let lda_ptr_inc = ag_a_standalone;
	let instr16 = {lda_ptr_inc, 0b100};
}


//////////////////////////
// 32-bit instructions

class AIE_i32_shrt<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_instr32<outs, ins, pattern, opcodestr, argstr> {
   field bits<21> i32_shrt;
   let instr32 = {i32_shrt, 0b00000000};
}
// AKA 'slotalu'
class AIE_alu_format<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_i32_shrt<outs, ins, pattern, opcodestr, argstr> {
   field bits<19> alu_base;
   let i32_shrt = {0b0, alu_base, 0b0};
   let Slot = alu_slot;
}
class AIE_event<string opcodestr = "event", string argstr = "$val">
    : AIE_alu_format<(outs), (ins t02u:$val), [], opcodestr, argstr> {
  field bits<10> unused;
  bits<2> val;
  let hasSideEffects = 1;
  let mayLoad = 0;
	let mayStore = 0;
  let unused = 0;
  let alu_base = {0b0000100, unused, val};
}
class AIE_fx2flt<string opcodestr = "mov", string argstr = "$d0, ${s0}.FX2FLT, s2">
    : AIE_alu_format<(outs GPR:$d0), (ins GPR:$s0, S2:$s1), [], opcodestr, argstr> {
  bits<4> s0;
  bits<0> s1;
  bits<4> d0;
	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
  let alu_base = {0b00011000000,s0,s1,d0};
}
class AIEOpcode_nlf_combo<bits<2> val, string opcode> {
  bits<2> Value = val;
  string op = opcode;
}
def OPC_NLF_COMBO_SQRT    : AIEOpcode_nlf_combo<0b00, "sqrt">;
def OPC_NLF_COMBO_INVSQRT : AIEOpcode_nlf_combo<0b01, "invsqrt">;
def OPC_NLF_COMBO_INV     : AIEOpcode_nlf_combo<0b10, "inv">;

class AIE_nlf_combo<dag ins, AIEOpcode_nlf_combo opcode, string argstr = "$d0, $s0">
    : AIE_alu_format<(outs GPR1:$d0), ins, [], opcode.op, argstr> {
  field bits<5> unused;
	bits<0> d0;
  bits<4> s0;
  field bits<2> op = opcode.Value;
  field bits<1> flt2fix;
  field bits<1> fix2flt;
  let hasSideEffects = 0;
  let mayLoad = 0;
	let mayStore = 0;
  let unused = 0;
  let alu_base = {0b000111, unused, d0, s0, op, flt2fix, fix2flt};
}
class AIE_nlf_combo_flt_flt<AIEOpcode_nlf_combo opcode, string argstr = "$d0, $s0">
    : AIE_nlf_combo<(ins GPR:$s0), opcode, argstr> {
  let flt2fix = 0;
  let fix2flt = 0;
}
class AIE_nlf_combo_fix_flt<AIEOpcode_nlf_combo opcode, string argstr = "${d0}.FLT2FX, $s3, $s0">
    : AIE_nlf_combo<(ins S3:$s3, GPR:$s0), opcode, argstr> {
  let flt2fix = 0;
  let fix2flt = 1;
  bits<0> s3;
  let alu_base = {0b000111, unused, d0, s0, s3, op, flt2fix, fix2flt};
}
class AIE_nlf_combo_flt_fix<AIEOpcode_nlf_combo opcode, string argstr = "$d0, ${s0}.FX2FLT, $s2">
    : AIE_nlf_combo<(ins GPR:$s0, S2:$s2), opcode, argstr> {
  let flt2fix = 1;
  let fix2flt = 0;
  bits<0> s2;
  let alu_base = {0b000111, unused, d0, s0, s2, op, flt2fix, fix2flt};
}
class AIE_nlf_combo_fix_fix<AIEOpcode_nlf_combo opcode, string argstr = "${d0}.FLT2FX, $s3, ${s0}.FX2FLT, $s2">
    : AIE_nlf_combo<(ins S3:$s3, GPR:$s0, S2:$s2), opcode, argstr> {
  let flt2fix = 1;
  let fix2flt = 1;
  bits<0> s2;
  bits<0> s3;
  let alu_base = {0b000111, unused, d0, s0, s2, s3, op, flt2fix, fix2flt};
}
class AIE_lock_reg<bits<1> releasenotacquire, string opcodestr = "", string argstr = "$s0, $use_val, $s1">
    : AIE_alu_format<(outs), (ins GPR:$s0, GPR:$s1, uimm1:$use_val), [], opcodestr, argstr> {
   bits<4> s1;
   bits<4> s0;
   bits<1> use_val;
   let hasSideEffects = 1;
	let mayLoad = 1;
	let mayStore = 1;
   let alu_base = {0b00101, releasenotacquire, 0b0, s1, s0, 0b001, use_val};
}
class AIE_lock_imm<bits<1> releasenotacquire, string opcodestr = "", string argstr = "$imm, $use_val, $s1">
    : AIE_alu_format<(outs), (ins uimm6:$imm, GPR:$s1, uimm1:$use_val), [], opcodestr, argstr> {
   bits<4> s1;
   bits<6> imm;
   bits<1> use_val;
   let hasSideEffects = 1;
	let mayLoad = 1;
	let mayStore = 1;
   let alu_base = {0b00101, releasenotacquire, 0b0, s1, imm, 0b0, use_val};
}
// Note that condition code is *LAST*.
class AIE_select_r_rr<bits<1> opcode, string opcodestr = "", string argstr = "$d0, $s0, $s1, $s2">
    : AIE_alu_format<(outs GPR:$d0), (ins GPR:$s0, GPR:$s1, GPR0:$s2), [], opcodestr, argstr> {
	 bits<0> s2;
   bits<4> s1;
   bits<4> s0;
   bits<4> d0;
   let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
   let alu_base = {0b001111, opcode, s2, s1, s0, d0};
}
class AIE_add_r_ri<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_alu_format<outs, ins, pattern, opcodestr, argstr> {
   bits<7> imm;
   bits<4> s0;
   bits<4> d0;
   let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
   let alu_base = {0b0100, imm, s0, d0};
}
class AIE_add_s_ri<bits<2> opcode, dag outs, dag ins, list<dag> pattern,
		string opcodestr = "", string argstr = "">
    : AIE_alu_format<outs, ins, pattern, opcodestr, argstr> {
   bits<6> imm;
   bits<4> s0;
   bits<3> d0;
   let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
   let alu_base = {0b0101, imm{5-1}, opcode, s0, imm{0}, d0};
}
class AIE_alu_r_rr<bits<4> opcode, string opcodestr = "", string argstr = "$d0, $s0, $s1">
    : AIE_alu_format<(outs GPR:$d0), (ins GPR:$s0, GPR:$s1), [], opcodestr, argstr> {
   bits<4> ALU_opcode = opcode;
   bits<4> s1;
   bits<4> s0;
   bits<4> d0;
   let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
   let alu_base = {0b011, opcode, s1, s0, d0};
}
class AIE_cg_s12<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_alu_format<outs, ins, pattern, opcodestr, argstr> {
   bits<12> imm;
	bits<6> dst; // mAluCg12
   let alu_base = {0b1, imm, dst};
}


class AIE_vec_med<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_instr32<outs, ins, pattern, opcodestr, argstr> {
   field bits<25> vecm_base;
	field bits<2> unknown2;
   let instr32 = {vecm_base, 0b00, 0b10};

   let Slot = vecm_slot;
}

// FIXME: Same problem as AIE_mv1ssb_base (disassembler_ambiguity.mir)
// AIE_vec_dpd must be rewritten as a separate 3-byte instruction.
class AIE_vec_dpd<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_med<outs, ins, pattern, opcodestr, argstr> {

  bits<22> vec_dpd;
  bits<5>  vecstrm_base = 0;
  bits<6>  vecshft_base = 0;
  bits<11> vecshrt_base = 0;

  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vecstrm_base, vecshft_base, vecshrt_base};
}

class AIE_vec_strm<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_dpd<outs, ins, pattern, opcodestr, argstr> {
  let Slot = vecstrm_slot;
}

class AIE_vec_shft<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_dpd<outs, ins, pattern, opcodestr, argstr> {
  let Slot = vecshft_slot;
}

class AIE_vec_short<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_dpd<outs, ins, pattern, opcodestr, argstr> {
  let Slot = vecshrt_slot;
}

class AIE_vec_compare<
  bits<2> xsz, /* i16, i32, u8, i8 */
dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_med<outs, ins, pattern, opcodestr, argstr> {
  bits<1> dst; // xa, xc
  bits<3> cmp; // GPR0_7

  bits<1> sx; // ya, yd
  bits<4> xstart; // GPR
  bits<3> xoffs; // mC
  bits<3> ystart; // GPR0_7
  bits<3> yoffs; // mC
  bits<3> conf; // mC
  field bits<23> vec_compare = {cmp{2}, xsz, xstart, ystart, xoffs, yoffs, conf, dst, sx, cmp{1-0} };
  let vecm_base = {0b01, vec_compare};
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
}
class AIE_vec_select<
  bits<2> xsz, /* i16, i32, u8, i8 */
dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_med<outs, ins, pattern, opcodestr, argstr> {
  bits<1> dst; // xa, xc
  bits<4> sel; // GPR

  bits<1> sx; // ya, yd
  bits<4> xstart; // GPR
  bits<3> xoffs; // mC
  bits<3> ystart; // GPR0_7
  bits<3> yoffs; // mC
  bits<3> conf; // mC
  field bits<24> vec_select = {sel{3-2}, xsz, xstart, ystart, xoffs, yoffs, conf, dst, sx, sel{1-0} };
  let vecm_base = {0b1, vec_select};
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
}


class AIE_lng<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_instr32<outs, ins, pattern, opcodestr, argstr> {
   field bits<28> lng_base;
   let instr32 = {lng_base, 0b1};
   let Slot = lng_slot;
}
class AIE_ba_instr<dag outs, dag ins, list<dag> pattern, bits<1> opcode, string opcodestr, string argstr>
    : AIE_lng<outs, ins, pattern, opcodestr, argstr> {
   bits<4> reg;
   bits<20> addr;

   field bits<25> ba_instr;
   let ba_instr{24} = opcode;
   let ba_instr{23-21} = addr{19-17};
   let ba_instr{20-17} = reg;
   let ba_instr{16-0} = addr{16-0};

   let lng_base = {0b100, ba_instr};
}

class AIE_j_instr<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_lng<outs, ins, pattern, opcodestr, argstr> {
   bits<20> addr;

   field bits<24> j_instr;
   let j_instr{23-21} = addr{19-17};
   let j_instr{20-17} = 0x0000;
   let j_instr{16-0} = addr{16-0};

   let lng_base = {0b1010, j_instr};
}
class AIE_jal_instr<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_lng<outs, ins, pattern, opcodestr, argstr> {
	bits<20> addr;

   field bits<24> j_instr;
   let j_instr{23-21} = addr{19-17};
   let j_instr{20-17} = 0x0001;
   let j_instr{16-0} = addr{16-0};

   let lng_base = {0b1010, j_instr};
}
class AIE_mov20s<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_lng<outs, ins, pattern, opcodestr, argstr> {
   bits<20> imm;
   bits<6> reg;

   field bits<26> mov = {imm{19-14}, reg, imm{13-0}};
   let lng_base = {0b00, mov};
}
class AIE_mov20<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_lng<outs, ins, pattern, opcodestr, argstr> {
    bits<20> imm;
	 bits<6> reg;

    field bits<26> mov = {imm{19-14}, reg, imm{13-0}};
    let lng_base = {0b01, mov};
}
class AIE_movu12<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_lng<outs, ins, pattern, opcodestr, argstr> {
    bits<12> imm;
    bits<6> reg; // mRCm

	 field bits<20> mov = {reg, 0b00, imm};
    let lng_base = {0b00, 0b000001, mov};
}

class AIE_dms_lda_spil<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_lng<outs, ins, pattern, opcodestr, argstr> {
    bits<14> imm;
	 bits<6> reg; //mLdaScl
	 field bits<20> dms_lda_spil = {imm{13-11}, reg, imm{10-0}};
    let lng_base = {0b11, /*XXX*/0b000, dms_lda_spil, 0b110};
	 let hasSideEffects = 0;
	 let mayLoad = 1;
	 let mayStore = 0;
}

class AIE_dmv_lda_spil<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_lng<outs, ins, pattern, opcodestr, argstr> {
    bits<14> imm;
    bits<4> reg; //mVa
    field bits<20> dmv_lda_spil = {imm{13-11}, /*XX*/0b00, reg, imm{10-0}};
    let lng_base = {0b11, /*XX*/0b00, 0b1, dmv_lda_spil, 0b010};
    let hasSideEffects = 0;
    let mayLoad = 1;
    let mayStore = 0;
}

class AIE_dmw_lda_spil<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_lng<outs, ins, pattern, opcodestr, argstr> {
    bits<14> imm;
    field bits<6> dst;
    field bits<20> dmw_lda_spil = {imm{13-11}, dst, imm{10-0}};
    let lng_base = {0b11, /*XX*/0b00, 0b0, dmw_lda_spil, 0b010};
    let hasSideEffects = 0;
    let mayLoad = 1;
    let mayStore = 0;
}

class AIE_dmw_lda_spil_mWa<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_dmw_lda_spil<outs, ins, pattern, opcodestr, argstr> {
  bits<3> reg; //mWa
  let dst = {0b1, /*X*/0b0, reg, /*X*/0b0};
}

//*select subregister class sd{2} -> {aml0, ..., amh3}, 0->{aml0,...,aml3}, 1->{amh0,..., amh3}*/
class AIE_dmw_lda_spil_mLoadAcc_AM<
  bits<1> hilo, /* lo or hi? */
  bits<1> osz,  /* 48 or 80 bits? */
  dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_dmw_lda_spil<outs, ins, pattern, opcodestr, argstr> {
  bits<3> sd; //mAM
  let dst = {0b0, hilo, osz, sd{2}, sd{1-0}};
}

class AIE_dms_sts_spil<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_lng<outs, ins, pattern, opcodestr, argstr> {
    bits<14> imm;
	 bits<6> reg; // mSclSt
	 field bits<20> dms_sts_spil = {reg, imm{13-0}};
    let lng_base = {0b11, dms_sts_spil, /*XXX*/0b000, 0b100};
	 let hasSideEffects = 0;
	 let mayLoad = 0;
	 let mayStore = 1;
}

class AIE_dmv_sts_spil<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_lng<outs, ins, pattern, opcodestr, argstr> {
    bits<14> imm;
    bits<4> reg; // mVa
    // Note that this is inconsistent with the processor spec.  But it is
    // more self-consistent than the processor spec is!
    field bits<20> dmv_sts_spil = {/*XX*/0b00, reg, imm{13-0}};
    let lng_base = {0b11, dmv_sts_spil, /*XX*/0b00, 0b1000};
    let hasSideEffects = 0;
    let mayLoad = 0;
    let mayStore = 1;
}

class AIE_dmw_sts_spil<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_lng<outs, ins, pattern, opcodestr, argstr> {
  bits<14> imm;
  field bits<6> dst;
  field bits<20> dmw_sts_spil = {dst, imm{13-0}};
  let lng_base = {0b11, dmw_sts_spil, /*XX*/0b00, 0b0000};
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 1;
}

class AIE_dmw_sts_spil_mWs<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_dmw_sts_spil<outs, ins, pattern, opcodestr, argstr> {
  bits<3> reg; //mWa
  let dst = {0b1, /*X*/0b0, reg, /*X*/0b0};
}

//*select subregister class sd{2} -> {aml0, ..., amh3}, 0->{aml0,...,aml3}, 1->{amh0,..., amh3}*/
class AIE_dmw_sts_spil_mStoreAcc_AM<
  bits<1> hilo, /* lo or hi? */
  bits<1> osz,  /* 48 or 80 bits? */
  dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_dmw_sts_spil<outs, ins, pattern, opcodestr, argstr> {
  bits<3> sd; //mAM
  let dst = {0b0, hilo, osz, sd{2}, sd{1-0}};
}

//////////////////////////////////////
// Possibilities for an ag_short field.
// pointer indirect register [ptr], mod
class ag_short_pstm_cyc {
	 bits<3> cyc;
	 bits<3> mod;
	 bits<3> ptr;

    field bits<10> ag_short;
	 let ag_short = {0b0, cyc, mod, ptr};
}
class ag_short_pstm_nrm {
	 bits<3> mod;
	 bits<3> ptr;

    field bits<10> ag_short;
	 let ag_short = {0b1000, mod, ptr};
}
// pointer indirect [ptr], #imm
class ag_short_pstm_nrm_imm {
	 bits<4> imm;
	 bits<3> ptr;

    field bits<10> ag_short;
	 let ag_short = {0b101, imm, ptr};
}
// Stack pointer register [sp], mod
class ag_short_pstm_sp {
	 bits<3> mod;
	 field bits<2> unknown;

	 field bits<10> ag_short;
	 let ag_short = {0b1001, mod, unknown, 0b0};
}
// Stack pointer immediate [sp], #imm
class ag_short_pstm_sp_imm {
	 bits<5> imm;

	 field bits<10> ag_short;
	 let ag_short = {0b1001, imm, 0b1};
}
// Indexed addressing [ptr, cs]
class ag_short_idx {
	 bits<3> cs;
	 bits<3> ptr;

	 field bits<10> ag_short;
	 let ag_short = {0b1100, cs, ptr};
}
// Indirect addressing [ptr]
class ag_short_ind {
	 bits<3> ptr;
	 field bits<10> ag_short;
	 let ag_short = {0b1101, 0b000/*xxx*/, ptr};
}
// Stack pointer immediate [sp, #imm]
class ag_short_pstm_spis { // FIXME: name?
	 bits<7> imm;

	 field bits<10> ag_short;
    let ag_short = {0b111, imm};
}


// A standalone field is slightly longer than an ag_short field,
// but adds the fft_data indexing.
class ag_a_standalone {
	 field bits<11> ag_a_standalone;
	 field bits<10> ag_short;
	 let ag_a_standalone = {0b0, ag_short};
}
class aga_all {
	 field bits<11> aga_all;
	 field bits<10> ag_short;
	 let aga_all = {0b0, ag_short};
}

//// AKA 'slota'
class AIE_i32_lda<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_i32_shrt<outs, ins, pattern, opcodestr, argstr> {
    field bits<18> lda_base;
    let i32_shrt = {0b0, lda_base, 0b01};
	let hasSideEffects = 0;
	let mayLoad = 1;
	let mayStore = 0;

  let Slot = lda_slot;
}

class AIE_cg_s10<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_i32_lda<outs, ins, pattern, opcodestr, argstr> {
   bits<10> imm;
   field bits<5> mAluCg10;
   let lda_base = {0b001, mAluCg10, imm};
}
class AIE_lda_ptr_inc<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_i32_lda<outs, ins, pattern, opcodestr, argstr> {
    bits<11> lda_ptr_inc;
    bits<11> ag_a_standalone;
    let mayLoad = 0;
    let lda_ptr_inc = ag_a_standalone;
    let lda_base = {0b0000100, lda_ptr_inc};
}

class AIE_dmhb_lda<bits<2> op, dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_i32_lda<outs, ins, pattern, opcodestr, argstr> {
    bits<3> reg; // mRSa
    field bits<10> ag_short;

    field bits<15> dmhb_lda = {op, reg, ag_short};
    let lda_base = {0b100, dmhb_lda};
}

class AIE_dms_lda<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_i32_lda<outs, ins, pattern, opcodestr, argstr> {
    bits<6> reg;
    field bits<10> ag_short;

    field bits<16> dms_lda = {reg, ag_short};
    let lda_base = {0b01, dms_lda};
}

class AIE_dmv_lda<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_i32_lda<outs, ins, pattern, opcodestr, argstr> {
    bits<4> reg; // mV
    field bits<10> ag_short;
    field bits<11> ag_all = {0b0, ag_short};

    field bits<15> dmv_lda = {reg, ag_all};
    let lda_base = {0b101, dmv_lda};
}

class AIE_dmw_lda_W<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_i32_lda<outs, ins, pattern, opcodestr, argstr> {
    bits<3> reg; // mV
    field bits<10> ag_short;
    field bits<11> ag_all = {0b0, ag_short};

    field bits<15> dmw_lda = {reg, 0b1, ag_all};
    let lda_base = {0b000, dmw_lda};
}

class AIE_dmw_lda_AM<
  bits<1> hilo, /* lo or hi? */
  bits<1> osz,  /* 48 or 80 bits? */
  dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_i32_lda<outs, ins, pattern, opcodestr, argstr> {
  bits<3> reg;
  field bits<3> sd = {reg}; //mAM
  field bits<10> ag_short;

  field bits<15> dmw_lda_AM = {hilo, osz, sd{2}, sd{1-0}, ag_short};
  let lda_base = {0b111, dmw_lda_AM};
}

//// aka 'slotm0b'
class AIE_mv0_ldb<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_i32_shrt<outs, ins, pattern, opcodestr, argstr> {
    field bits<18> mv0_ldb;
    let i32_shrt = {0b0, mv0_ldb, 0b11};
}

// mv0_base AKA 'slotm0'
class AIE_mv0_base<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_mv0_ldb<outs, ins, pattern, opcodestr, argstr> {
    field bits<16> mv0_base;

    let mv0_ldb = {mv0_base, 0b00};
    let Slot = mv0_slot;
}

class AIE_mv_special2r<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
	: AIE_mv0_base<outs, ins, pattern, opcodestr, argstr> {
	bits<6> s0;
	bits<4> d0;

	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
	let mv0_base = {0b1000, s0, 0b01, d0};
}

class AIE_mv_r2special<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
	: AIE_mv0_base<outs, ins, pattern, opcodestr, argstr> {
	bits<4> s0;
	bits<6> d0;

	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
	let mv0_base = {0b000010, s0, d0};
}


class AIE_mv_scl<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
	: AIE_mv0_base<outs, ins, pattern, opcodestr, argstr> {
	bits<6> src;
	bits<6> dst;

	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
	let mv0_base = {0b0001, src, dst};
}

// "Accumulator Move"
class AIE_mv_vecam<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
	: AIE_mv0_base<outs, ins, pattern, opcodestr, argstr> {
	bits<3> src;
	bits<3> dst;

	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
	let mv0_base = {0b1000010, src, 0b000, dst};
}

// "Vector Move"
class AIE_mv_vec<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
	: AIE_mv0_base<outs, ins, pattern, opcodestr, argstr> {
	bits<4> src;
	bits<4> dst;

	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
	let mv0_base = {0b100110, src, 0b01, dst};
}

// "Wide Vector Move"
class AIE_mv_vecw<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
	: AIE_mv0_base<outs, ins, pattern, opcodestr, argstr> {
	bits<3> src;
	bits<3> dst;

	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
	let mv0_base = {0b100100, src, 0b101, dst, 0b1};
}

class AIE_mv_vecx<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
	: AIE_mv0_base<outs, ins, pattern, opcodestr, argstr> {
	bits<2> src;
	bits<2> dst;

	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
	let mv0_base = {0b100100, src, 0b0101, dst, 0b00};
}

// Note: the modifier register md0 is always an input to this
// instruction, even though it is implicit and doesn't appear in the
// encoding.
class AIE_mv_ssa2scl<bits<1> nb, bits<1> ssa, string opcodestr, string srcstr>
	: AIE_mv0_base<(outs OP_mMvScl:$dst), (ins MD0:$mod), [], opcodestr, "$dst, " # srcstr> {
	bits<6> dst;

	let hasSideEffects = 1;
	let mayLoad = 0;
	let mayStore = 0;
	let mv0_base = {0b0000001, /*x*/0b0, nb, ssa, dst};
}
class AIE_flt2fx<string opcodestr = "mov", string argstr = "$d0, ${s0}.FLT2FX, s3">
	: AIE_mv0_base<(outs GPR:$d0), (ins GPR:$s0, S3:$s1), [], opcodestr, argstr> {
  bits<4> s0;
	bits<0> s1;
  bits<4> d0;
	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
  let mv0_base = {0b000001,s0,s1,0b00,d0};
}
class AIE_s2v_shiftw0<bits<1> size, dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
	: AIE_mv0_base<outs, ins, pattern, opcodestr, argstr> {
	bits<3> vreg; // mVa
	bits<4> reg;

	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
	let mv0_base = {0b10100, size, reg, 0b00, vreg, 0b0};
}

class AIE_s2v_shiftv0<bits<1> size, dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
	: AIE_mv0_base<outs, ins, pattern, opcodestr, argstr> {
	bits<4> vreg; // mVa
	bits<4> reg;

	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
	let mv0_base = {0b10100, size, reg, 0b01, vreg};
}

class AIE_s2v_upd<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
	: AIE_mv0_base<outs, ins, pattern, opcodestr, argstr> {
	field bits<14> s2v_upd;

	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
	let mv0_base = {0b01, s2v_upd};
}
class AIE_s2v_upd_r16<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
		: AIE_s2v_upd<outs, ins, pattern, opcodestr, argstr> {
        bits<3> idx;
        bits<4> vreg; // mVa
        bits<4> reg;
        let s2v_upd = {idx, 0b0, reg, 0b11, vreg};
}
class AIE_s2w_upd_r16<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
		: AIE_s2v_upd<outs, ins, pattern, opcodestr, argstr> {
        bits<4> idx;
        bits<3> vreg;
        bits<4> reg;
        let s2v_upd = {idx{2-0}, 0b0, reg, 0b11, vreg, idx{3}};
        bit isCodeGenOnly = true; // The canonical instruction is defined on a 128 bit register
}
class AIE_s2x_upd_r16<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
		: AIE_s2v_upd<outs, ins, pattern, opcodestr, argstr> {
        bits<5> idx;
        bits<2> vreg;
        bits<4> reg;
        let s2v_upd = {idx{2-0}, 0b0, reg, 0b11, vreg, idx{4-3}};
        bit isCodeGenOnly = true; // The canonical instruction is defined on a 128 bit register
}

class AIE_s2v_upd_r32<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
		: AIE_s2v_upd<outs, ins, pattern, opcodestr, argstr> {
        bits<2> idx;
        bits<4> vreg; // mVa
        bits<4> reg;
        let s2v_upd = {idx, 0b01, reg, 0b11, vreg};
}

class AIE_s2w_upd_r32<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
		: AIE_s2v_upd<outs, ins, pattern, opcodestr, argstr> {
        bits<3> idx;
        bits<3> vreg; // mVa
        bits<4> reg;
        let s2v_upd = {idx{1-0}, 0b01, reg, 0b11, vreg, idx{2}};
        bit isCodeGenOnly = true; // The canonical instruction is defined on a 128 bit register
}

class AIE_s2x_upd_r32<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
		: AIE_s2v_upd<outs, ins, pattern, opcodestr, argstr> {
        bits<4> idx;
        bits<2> vreg; // mVa
        bits<4> reg;
        let s2v_upd = {idx{1-0}, 0b01, reg, 0b11, vreg, idx{3-2}};
        bit isCodeGenOnly = true; // The canonical instruction is defined on a 128 bit register
}

class AIE_bitget<bits<1> srcIsGPR, dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
	: AIE_mv0_base<outs, ins, pattern, opcodestr, argstr> {
	bits<5> idx;
	bits<3> dst; // mRSm (r0-7)
	bits<4> src; // mBitGetSet  (r0-15, mc, md)

   field bits<13> bitget = {idx{4-3}, srcIsGPR, src, idx{2-0}, dst};

	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
	let mv0_base = {0b001, bitget};
}
class AIE_bitset<bits<1> dstIsGPR, dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
	: AIE_mv0_base<outs, ins, pattern, opcodestr, argstr> {
	bits<5> idx;
	bits<3> src; // mRSb (r0-7)
	bits<4> dst; // mBitGetSet  (r0-15, mc, md)

   field bits<14> bitset = {idx, src, 0b1, dstIsGPR, dst};

	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
	let mv0_base = {0b10, bitset};
}

// ldb_base AKA 'slotb'
class AIE_ldb_base<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_mv0_ldb<outs, ins, pattern, opcodestr, argstr> {
    field bits<17> ldb_base;

    let mv0_ldb = {ldb_base, 0b1};
	let hasSideEffects = 0;
	let mayLoad = 1;
	let mayStore = 0;

  let Slot = ldb_slot;
}
class AIE_dms_ldb<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_ldb_base<outs, ins, pattern, opcodestr, argstr> {
    bits<6> reg; // mLdbScl
    field bits<10> ag_short;

	 field bits<16> dms_ldb = {reg, ag_short};
    let ldb_base = {0b1, dms_ldb};
}
class AIE_dmv_ldb<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_ldb_base<outs, ins, pattern, opcodestr, argstr> {
    bits<4> reg; // mV
	 field bits<10> ag_short;
    field bits<11> ag_all = {0b0, ag_short};

    field bits<15> dmv_ldb = {reg, ag_all};
    let ldb_base = {0b01, dmv_ldb};
}
class AIE_dmw_ldb<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_ldb_base<outs, ins, pattern, opcodestr, argstr> {
    bits<3> reg; // mV
    field bits<10> ag_short;
    field bits<11> ag_all = {0b0, ag_short};

    field bits<15> dmv_ldb = {reg, 0b1, ag_all};
    let ldb_base = {0b00, dmv_ldb};
}
class AIE_ldb_ptr_inc<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_ldb_base<outs, ins, pattern, opcodestr, argstr> {
	 bits<11> ldb_ptr_inc;
	 bits<11> ag_a_standalone;
	 field bits<1> unknown1;

	 let ldb_ptr_inc = {ag_a_standalone};
    let ldb_base = {0b00, unknown1, 0b100, ldb_ptr_inc};
}


//// AKA 'slots'
class AIE_st_base<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_i32_shrt<outs, ins, pattern, opcodestr, argstr> {
    field bits<20> st_base;
    let i32_shrt = {0b1, st_base};
	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 1;

  let Slot = st_slot;
}

class AIE_gen_pckt_header<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_st_base<outs, ins, pattern, opcodestr, argstr> {
    bits<4> reg;
	 bits<3> type;
	 field bits<3> msa;

    field bits<14> gen_pckt_header = {reg, 0b1, type, /*xxx*/0b000, msa};
    let st_base = {0b0000, /*xx*/ 0b00, gen_pckt_header};
	 let hasSideEffects = 1;
}
// The instruction always reads md0.  We make this explicit in graph to make it
// easy to connect the associated bitset instruction to this one.
class AIE_gen_pckt_header_reg<bits<1> dest, string opcodestr, string deststr>
	 : AIE_gen_pckt_header<(outs), (ins MD0:$mod, GPR:$reg, t03u:$type), [], opcodestr, deststr # ", $reg, $type, r3"> {
	 let msa = {dest, 0b1, /*x*/ 0b0};
}
class AIE_gen_pckt_header_imm<bits<1> dest, string opcodestr, string deststr>
	 : AIE_gen_pckt_header<(outs), (ins MD0:$mod, GPR:$reg, t03u:$type, uimm1:$tlast), [], opcodestr, deststr # ", $reg, $type, $tlast"> {
	 bits<1> tlast;
	 let msa = {dest, 0b0, tlast};
}
class AIE_gen_ctrl_pckt_header<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_st_base<outs, ins, pattern, opcodestr, argstr> {
	 bits<2> op;
	 bits<4> reg;
	 bits<3> nw;
	 bits<3> addr; // mMs

	 bits<3> msa;

    field bits<16> gen_ctrl_pckt_header = {op, reg, 0b1, nw, addr, msa};
    let st_base = {0b0001, gen_ctrl_pckt_header};
	 let hasSideEffects = 1;
}
// nw == numWords
class AIE_gen_ctrl_pckt_header_reg<bits<1> dest, string opcodestr, string deststr>
	 : AIE_gen_ctrl_pckt_header<(outs), (ins MD0:$mod, MOD:$addr, t03u:$nw, t02u:$op, GPR:$reg), [], opcodestr, deststr # ", $addr, $nw, $op, $reg, r3"> {
	 let msa = {dest, 0b1, /*x*/ 0b0};
}
class AIE_gen_ctrl_pckt_header_imm<bits<1> dest, string opcodestr, string deststr>
	 : AIE_gen_ctrl_pckt_header<(outs), (ins MD0:$mod, MOD:$addr, t03u:$nw, t02u:$op, GPR:$reg, uimm1:$tlast), [], opcodestr, deststr # ", $addr, $nw, $op, $reg, $tlast"> {
	 bits<1> tlast;
	 let msa = {dest, 0b0, tlast};
}
// stream write
class AIE_mv_scl2msa<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_st_base<outs, ins, pattern, opcodestr, argstr> {
    bits<5> reg; // mSclMSa
	 field bits<4> nb_msa;

    field bits<15> mv_scl2msa = {reg, 0b11, /*xxxx*/0b0000, nb_msa};
    let st_base = {0b0011, /*x*/ 0b0, mv_scl2msa};
	 let hasSideEffects = 1;
}
class AIE_mv_scl2msa_reg<bits<1> dest, bits<1> nb, string opcodestr, string deststr>
	 : AIE_mv_scl2msa<(outs), (ins MD0:$mod, GPRPTRMOD:$reg), [], opcodestr, deststr # ", $reg, r3"> {
	 let nb_msa = {nb, 0b1, /*x*/ 0b0, dest};
}
class AIE_mv_scl2msa_imm<bits<1> dest, bits<1> nb, string opcodestr, string deststr>
	 : AIE_mv_scl2msa<(outs), (ins MD0:$mod, GPRPTRMOD:$reg, uimm1:$tlast), [], opcodestr, deststr # ", $reg, $tlast"> {
	 bits<1> tlast;
	 let nb_msa = {nb, 0b0, tlast, dest};
}

// shift-round-saturate
class AIE_mv_v_srs0<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_st_base<outs, ins, pattern, opcodestr, argstr> {
  bits<4> dst; /*mVs*/
  field bits<8> mVsrs0;
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
  let st_base = {0b00, mVsrs0, 0b0101, /*x*/0b00, dst};
}
class AIE_mv_v_srs0_am<bits<1> sz /*48 or 80-bits*/, dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_mv_v_srs0<outs, ins, pattern, opcodestr, argstr> {
  bits<3> src; /*mAm*/
  bits<3> shift;
  let mVsrs0 = {0b1, shift, sz, src};
}
class AIE_mv_v_srs0_bm<bits<1> sg /*uns or sgn*/, dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_mv_v_srs0<outs, ins, pattern, opcodestr, argstr> {
  bits<2> src; /*mBm*/
  bits<3> shift;
  let mVsrs0 = {0b0, shift, sg, 0b0, src};
}

class AIE_mv_w_srs0<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_st_base<outs, ins, pattern, opcodestr, argstr> {
  bits<3> dst; /*mWs*/
  field bits<8> mWsrs0;
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
  let st_base = {0b00, mWsrs0, 0b0110, /*x*/0b00, dst, /*x*/0b0};
}
class AIE_mv_w_srs0_am<bits<1> sz, dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_mv_w_srs0<outs, ins, pattern, opcodestr, argstr> {
  bits<3> src; /*mAm*/
  bits<3> shift;
  let mWsrs0 = {0b1, shift, sz, src};
}
class AIE_mv_w_srs0_bm<bits<1> sz, dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_mv_w_srs0<outs, ins, pattern, opcodestr, argstr> {
  bits<2> src; /*mBm*/
  bits<3> shift;
  let mWsrs0 = {0b0, shift, sz, 0b1, src};
}

class AIE_dmhb_sts<bits<1> op, dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_st_base<outs, ins, pattern, opcodestr, argstr> {
    field bits<10> ag_short;
    bits<4> reg;

    field bits<15> dmhb_sts = {op, reg, ag_short};
    let st_base = {0b01001, dmhb_sts};
}
class AIE_dms_sts<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_st_base<outs, ins, pattern, opcodestr, argstr> {
    field bits<10> ag_short;
    bits<6> reg; // mSclSt

    field bits<16> dms_sts = {reg, ag_short};
    let st_base = {0b0101, dms_sts};
}
class AIE_dmv_sts<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_st_base<outs, ins, pattern, opcodestr, argstr> {
    field bits<10> ag_short;
    bits<4> reg;

    field bits<14> dmv_sts = {reg, ag_short};
    let st_base = {0b01101, /*X*/0b0, dmv_sts};
}
class AIE_dmw_sts_WG<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_st_base<outs, ins, pattern, opcodestr, argstr> {
    field bits<10> ag_short;
    bits<3> reg;

    field bits<14> dmw_sts_WG = {reg, /*X*/0b0, ag_short};
    let st_base = {0b011101, dmw_sts_WG};
}
class AIE_dmv_sts_pack<bits<1> sg /*sign*/,dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_st_base<outs, ins, pattern, opcodestr, argstr> {
    field bits<10> ag_short;
    bits<3> reg; /*mWs*/

    field bits<14> dmv_sts_pack = {reg, sg, ag_short};
    let st_base = {0b011000, dmv_sts_pack};
}
class AIE_dmw_sts_pack<bits<1> sg /*sign*/,dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_st_base<outs, ins, pattern, opcodestr, argstr> {
    field bits<10> ag_short;
    bits<2> reg; /*mXs*/

    field bits<14> dmw_sts_pack = {reg, /*X*/0b0, sg, ag_short};
    let st_base = {0b011100, dmw_sts_pack};
}
class AIE_dmv_sts_srs_bm<bits<1> sg /*sign*/, dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_st_base<outs, ins, pattern, opcodestr, argstr> {
    field bits<10> ag_short;
    bits<3> shft; // mS
    bits<2> sd;   // mBM

    field bits<17> dmw_sts_srs = {shft, sg, 0b0, sd, ag_short};
    let st_base = {0b101, dmw_sts_srs};
}
////
// mv1_base AKA 'slotm1'
class AIE_mv1_base<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_mv0_ldb<outs, ins, pattern, opcodestr, argstr> {
    field bits<13> mv1_base;

    let mv0_ldb = {mv1_base, /*xxx*/0b000, 0b10};

    let Slot = mv1_slot;
}

// stream write
class AIE_mv_scl2msb<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_mv1_base<outs, ins, pattern, opcodestr, argstr> {
    bits<5> reg; // mSclMSa
	 field bits<4> nb_msb;

    field bits<9> mv_scl2msb = {reg, nb_msb};
    let mv1_base = {mv_scl2msb, 0b1100};
	 let hasSideEffects = 1;
	 let mayLoad = 0;
	 let mayStore = 1;
}
// FIXME: not just GPR: MOD and PTR too.
class AIE_mv_scl2msb_reg<bits<1> dest, bits<1> nb, string opcodestr, string deststr>
	 : AIE_mv_scl2msb<(outs), (ins MD0:$mod, GPR:$reg), [], opcodestr, deststr # ", $reg, r3"> {
	 let nb_msb = {nb, 0b1, /*x*/ 0b0, dest};
}
class AIE_mv_scl2msb_imm<bits<1> dest, bits<1> nb, string opcodestr, string deststr>
	 : AIE_mv_scl2msb<(outs), (ins MD0:$mod, GPR:$reg, uimm1:$tlast), [], opcodestr, deststr # ", $reg, $tlast"> {
	 bits<1> tlast;
	 let nb_msb = {nb, 0b0, tlast, dest};
}

// Stream write
// FIXME: due to the error showcased by disassembler_ambiguity.mir, it should
// be rewritten in such a way that there are differents DecoderTable between
// mv1_base and mv1ssb_base instructions. A simple way to achieve that is to
// use a different encoding size (Size attribute). Hence, we could define
// AIE_mv1ssb_base independently of AIE_mv1_base, on a separate 8-bit
// instruction.
class AIE_mv1ssb_base<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_mv1_base<outs, ins, pattern, opcodestr, argstr> {
    bits<7> mv1ssb_base;
    let mv1_base = {mv1ssb_base, /*x*/0b0, 0b00000};
    let Slot = mv1ssb_slot;
}

// Note: the modifier register md0 is always an input to this
// instruction, even though it is implicit and doesn't appear in the
// encoding.
class AIE_mv_ssb2scl<bits<1> nb, bits<1> ssb, string opcodestr, string srcstr>
	: AIE_mv1ssb_base<(outs GPR0_7:$dst), (ins MD0:$mod), [], opcodestr, "$dst, " # srcstr> {
	bits<3> dst;

	let hasSideEffects = 1;
	let mayLoad = 0;
	let mayStore = 0;
	let mv1ssb_base = {nb, dst, ssb, 0b11};
}

// shift-round-saturate
class AIE_mv_w_srs1<bits<1> sz, dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_mv1_base<outs, ins, pattern, opcodestr, argstr> {
	bits<3> src;
	bits<3> dst;

	let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
  let mv1_base = {dst, /*x*/0b0, src, sz, 0b01000};
}

// Extract element from vector.
class AIE_s2v_ext_r32_base<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_mv1_base<outs, ins, pattern, opcodestr, argstr> {
  field bits<10> s2v_ext_r32;
  let mv1_base = {s2v_ext_r32, 0b010};
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
}
class AIE_s2v_ext_r32<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_s2v_ext_r32_base<outs, ins, pattern, opcodestr, argstr> {
  bits<4> d0; // mRb
  bits<4> s0; // mVn
  bits<2> idx;

  let s2v_ext_r32 = {d0, s0, idx};
}

class AIE_s2w_ext_r32<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_s2v_ext_r32_base<outs, ins, pattern, opcodestr, argstr> {
  bits<4> d0; // mRb
  bits<3> s0; // W (256 bits)
  bits<3> idx;

  let s2v_ext_r32 = {d0, s0, idx};
  bit isCodeGenOnly = true; // The canonical instruction is defined on a 128 bit register
}
class AIE_s2x_ext_r32<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_s2v_ext_r32_base<outs, ins, pattern, opcodestr, argstr> {
  bits<4> d0; // mRb
  bits<2> s0; // X (512 bits)
  bits<4> idx;

  let s2v_ext_r32 = {d0, s0, idx};
  bit isCodeGenOnly = true; // The canonical instruction is defined on a 128 bit register
}
class AIE_s2v_ext_p32_base<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_mv1_base<outs, ins, pattern, opcodestr, argstr> {
  field bits<9> s2v_ext_p32;
  let mv1_base = {s2v_ext_p32, 0b0110};
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
}
class AIE_s2v_ext_p32<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_s2v_ext_p32_base<outs, ins, pattern, opcodestr, argstr> {
  bits<3> d0; // mPb
  bits<4> s0; // mVn
  bits<2> idx;

  let s2v_ext_p32 = {idx{1-1}, d0, s0, idx{0-0}};
}
class AIE_s2w_ext_p32<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_s2v_ext_p32_base<outs, ins, pattern, opcodestr, argstr> {
  bits<3> d0; // mPb
  bits<3> s0; // W (256 bits)
  bits<3> idx;

  let s2v_ext_p32 = {idx{1-1}, d0, s0, idx{2-2}, idx{0-0}};
  bit isCodeGenOnly = true; // The canonical instruction is defined on a 128 bit register
}
class AIE_s2x_ext_p32<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_s2v_ext_p32_base<outs, ins, pattern, opcodestr, argstr> {
  bits<3> d0; // mPb
  bits<2> s0; // X (512 bits)
  bits<4> idx;

  let s2v_ext_p32 = {idx{1-1}, d0, s0, idx{3-2}, idx{0-0}};
  bit isCodeGenOnly = true; // The canonical instruction is defined on a 128 bit register
}
class AIE_s2v_ext_r16_base<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_mv1_base<outs, ins, pattern, opcodestr, argstr> {
  field bits<10> s2v_ext_r16;
  let mv1_base = {0b1, s2v_ext_r16, 0b01};
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
}
class AIE_s2v_ext_r16<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_s2v_ext_r16_base<outs, ins, pattern, opcodestr, argstr> {
  bits<3> d0; // mRSb (GPR0-7)
  bits<4> s0; // mVn
  bits<3> idx;

  field bits<10> s2v_ext_r16= {d0, s0, idx};
}
class AIE_s2w_ext_r16<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_s2v_ext_r16_base<outs, ins, pattern, opcodestr, argstr> {
  bits<3> d0; // mRSb (GPR0-7)
  bits<3> s0; // W (256-bit)
  bits<4> idx;

  field bits<10> s2v_ext_r16= {d0, s0, idx};
  bit isCodeGenOnly = true; // The canonical instruction is defined on a 128 bit register
}
class AIE_s2x_ext_r16<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_s2v_ext_r16_base<outs, ins, pattern, opcodestr, argstr> {
  bits<3> d0; // mRsb (GPR0-7)
  bits<2> s0; // X (512-bit)
  bits<5> idx;

  field bits<10> s2v_ext_r16= {d0, s0, idx};
  bit isCodeGenOnly = true; // The canonical instruction is defined on a 128 bit register
}
// FIXME: add other extract_element variants
// s2v_ext_2p
// s2v_ext_2p_idx
// s2v_ext_r16_idx
// s2v_ext_r32_idx
// s2v_ext_p32_idx

////////////////////////////
// 64-bit instructions
// A VLIW format doesn't need outputs or patterns.
class AIE_i64_format<dag ins, string opcodestr, string argstr> :
      AIE_instr64<(outs), ins, [], opcodestr, argstr> {
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
}

class AIE_i64_vec<dag ins, string opcodestr, string argstr>
    : AIE_i64_format<ins, opcodestr, argstr> {
   field bits<58> i64_vec;
   let instr64 = {i64_vec, 0b00};
}
class AIE_i64_i32_shrt_vec_all<dag ins, string opcodestr, string argstr>
    : AIE_i64_vec<ins, opcodestr, argstr> {
  field bits<21> i32_shrt;
  field bits<37> veca_base;
  let i64_vec = {i32_shrt, veca_base};
}
// Definition of the veca_base slot, in a standalone way. This slot is the only
// one that doesn't fit in the instr32 format (37-bit).
// As with instr32 standalone definitions, when decoding a composite instruction,
// information for a veca_base slot will be extracted decoded separately.
// In this case the i32_shrt portion is irrelevant and can be ignored.
// Note that we put these standalone instructions in a separate DecoderNamespace,
// to separate the decoding of the standalone instructions from the composite
// instructions.
class AIE_veca_base<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_instr64<outs, ins, pattern, opcodestr, argstr> {
  field bits<37> veca_base;
  field bits<21> i32_shrt = 0;
  let instr64 = {i32_shrt, veca_base, 0b00};
  let Size = 8; // 8 Byte instruction
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
  let Slot = veca_slot;
  let DecoderNamespace = "veca_slot";
}
class AIE_vec_float<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_veca_base<outs, ins, pattern, opcodestr, argstr> {
  field bits<28> vec_float;
  let veca_base = {0b000100000, vec_float};
}
class AIE_vec_float_acc_all<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_float<outs, ins, pattern, opcodestr, argstr> {
  field bits<3> acc_noacc;
  bits<2> d0; // wr2, wr3, wd0, wd1
  bits<3> d1; // r0-r7
  bits<2> fpaddmode; // Literal
  bits<4> fpxstart; // r0-r15
  bits<3> fpzstart; // t03u
  bits<3> fpxoffs; // cl0-cl7
  bits<3> fpzoffs; // cl0-cl7
  bits<3> fpconf; // cl0-cl7
  bits<1> sc; // wc0, wc1
  bits<1> sx; // ya, yd
  let vec_float = {acc_noacc, d0, d1{2}, fpaddmode, fpxstart, fpzstart, fpxoffs, fpzoffs, fpconf, sc, sx, d1{1}, d1{0} };
}
class AIE_vec_float_acc<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_float_acc_all<outs, ins, pattern, opcodestr, argstr> {
  bits<2> acc; // wr2, wr3, wd0, wd1
  let acc_noacc = {0b1, acc};
}
class AIE_vec_float_noacc<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_float_acc_all<outs, ins, pattern, opcodestr, argstr> {
  let acc_noacc = {0b010};
}

class AIE_vec_am<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_veca_base<outs, ins, pattern, opcodestr, argstr> {
  field bits<34> vec_am;
  let veca_base = {0b001, vec_am};
}

class AIE_vec_acm_am_all<
  bits<1> osz, /* 48 or 80 bits? */
  bits<1> xcx, /* x complex? */
  bits<2> xsz, /* i16, i32, u8, i8 */
  bits<1> zcx, /* z complex? */
  bits<2> zsz, /* i16, i32, u8, i8 */
  dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_am<outs, ins, pattern, opcodestr, argstr> {
  field bits<3> acm_am_all_0;
  field bits<3> acm_am_all_1;

  bits<4> xstart; // GPR
  bits<3> xoffs; // mC
  bits<4> zstart; // t04u
  bits<3> zoffs; // mC
  bits<3> ystart; // GPR0_7
  bits<3> conf; // mC
  bits<1> ipm; // wc0 or wc1
  let vec_am = {acm_am_all_1, zcx, zsz, zstart, osz, xcx, xsz, xstart, ystart, xoffs, zoffs, conf, ipm, acm_am_all_0};
}

class AIE_vec_acm_am_zero<
  bits<1> osz, /* 48 or 80 bits? */
  bits<1> xcx, /* x complex? */
  bits<2> xsz, /* i16, i32, u8, i8 */
  bits<1> zcx, /* z complex? */
  bits<2> zsz, /* i16, i32, u8, i8 */
  dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_acm_am_all<osz, xcx, xsz, zcx, zsz, outs, ins, pattern, opcodestr, argstr> {
  bits<1> pmx; /* ya or yd */
  bits<3> sd; // mAM
  bits<3> acm_am_all_0 = {pmx, sd{1-0}};
  bits<3> acm_am_all_1 = {0b00, sd{2}};
}
class AIE_vec_acm_am_acc<
  bits<1> osz, /* 48 or 80 bits? */
  bits<1> xcx, /* x complex? */
  bits<2> xsz, /* i16, i32, u8, i8 */
  bits<1> zcx, /* z complex? */
  bits<2> zsz, /* i16, i32, u8, i8 */
  dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_acm_am_all<osz, xcx, xsz, zcx, zsz, outs, ins, pattern, opcodestr, argstr> {
  bits<1> pmx; /* ya or yd */
  bits<3> sd; // mAM
  bits<3> acm_am_all_0 = {pmx, sd{1-0}};
  bits<3> acm_am_all_1 = {0b01, sd{2}};
}
class AIE_vec_acm_am_scd<
  bits<1> osz, /* 48 or 80 bits? */
  bits<1> xcx, /* x complex? */
  bits<2> xsz, /* i16, i32, u8, i8 */
  bits<1> zcx, /* z complex? */
  bits<2> zsz, /* i16, i32, u8, i8 */
  dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_acm_am_all<osz, xcx, xsz, zcx, zsz, outs, ins, pattern, opcodestr, argstr> {
  bits<1> pmx; /* ya or yd */
  bits<3> sd; // mAM
  bits<3> acm_am_all_0 = {pmx, sd{1-0}};
  bits<3> acm_am_all_1 = {0b11, sd{2}};
}
// I can't parse the encoding in the ISA document for this one.
// class AIE_vec_acm_am_ups<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
//     : AIE_vec_am_all<osz, xcx, xsz, zcx, zsz, outs, ins, pattern, opcodestr, argstr> {
//   bits<1> s; /* ya or yd */
//   bits<3> mAM;
//   bits<3> acm_am_all_0 = {s, mAM{1-0}};
//   bits<3> acm_am_all_1 = {0b11, mAM{2}};
// }

class AIE_vec_bm<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_veca_base<outs, ins, pattern, opcodestr, argstr> {
  field bits<36> vec_bm;
  let veca_base = {0b1, vec_bm};
}
class AIE_vec_acm_bm_all<
  bits<1> osz, /* 48 or 80 bits? */
  bits<1> xcx, /* x complex? */
  bits<2> xsz, /* i16, i32, u8, i8 */
  bits<1> zcx, /* z complex? */
  bits<2> zsz, /* i16, i32, u8, i8 */
  dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_bm<outs, ins, pattern, opcodestr, argstr> {
  field bits<3> acm_bm_all_0;
  field bits<5> acm_bm_all_1;

  bits<4> xstart; // GPR
  bits<3> xoffs; // mC
  bits<4> zstart; // t04u
  bits<3> zoffs; // mC
  bits<3> ystart; // GPR0_7
  bits<3> conf; // mC
  bits<1> ipm; // wc0 or wc1
  let vec_bm = {acm_bm_all_1, zcx, zsz, zstart, osz, xcx, xsz, xstart, ystart, xoffs, zoffs, conf, ipm, acm_bm_all_0};
}
class AIE_vec_acm_bm_zero<
  bits<1> osz, /* 48 or 80 bits? */
  bits<1> xcx, /* x complex? */
  bits<2> xsz, /* i16, i32, u8, i8 */
  bits<1> zcx, /* z complex? */
  bits<2> zsz, /* i16, i32, u8, i8 */
  dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_acm_bm_all<osz, xcx, xsz, zcx, zsz, outs, ins, pattern, opcodestr, argstr> {
  bits<1> pmx; /* ya or yd */
  bits<2> sd; // mBM
  bits<3> acm_bm_all_0 = {pmx, sd};
  bits<5> acm_bm_all_1 = {0b0, 0b1011};
}
class AIE_vec_acm_bm_acc<
  bits<1> osz, /* 48 or 80 bits? */
  bits<1> xcx, /* x complex? */
  bits<2> xsz, /* i16, i32, u8, i8 */
  bits<1> zcx, /* z complex? */
  bits<2> zsz, /* i16, i32, u8, i8 */
  dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : AIE_vec_acm_bm_all<osz, xcx, xsz, zcx, zsz, outs, ins, pattern, opcodestr, argstr> {
  bits<1> pmx; /* ya or yd */
  bits<2> sd; // mBM
  bits<3> acm_bm_all_0 = {pmx, sd};
  bits<5> acm_bm_all_1 = {0b0, 0b1100};
}

class AIE_i64_novec<dag ins, string opcodestr, string argstr>
    : AIE_i64_format<ins, opcodestr, argstr> {
   field bits<59> i64_novec;
   let instr64 = {i64_novec, 0b1};
}


///
class AIE_i64_mv<dag ins, string opcodestr, string argstr>
    : AIE_i64_novec<ins, opcodestr, argstr> {
   field bits<21> alu_st;
   field bits<31> mv_all;
   let i64_novec = {alu_st, mv_all, 0b0111111};
}

class AIE_i64_nold<dag ins, string opcodestr, string argstr>
    : AIE_i64_novec<ins, opcodestr, argstr> {
   bits<20> st;
   bits<19> alu;
   field bits<17> mv;
   let i64_novec = {st, alu, mv, 0b111};
}

class AIE_i64_lda<dag ins, string opcodestr, string argstr>
    : AIE_i64_novec<ins, opcodestr, argstr> {
   bits<18> lda;
   field bits<41> i64_lda_fmts;
   let i64_novec = {lda, i64_lda_fmts};
}

class AIE_i64_lda_fmts_mv_all<dag ins, string opcodestr, string argstr>
    : AIE_i64_lda<ins, opcodestr, argstr> {
  bits<31> mv_all;
  let i64_lda_fmts = {mv_all, 0b0000011111};
}

class AIE_i64_lda_fmts_i64_ldb<dag ins, string opcodestr, string argstr>
    : AIE_i64_lda<ins, opcodestr, argstr> {
  bits<40> i64_ldb;
  let i64_lda_fmts = {i64_ldb, 0b0};
}

class AIE_i64_nop_lng<dag ins, string opcodestr, string argstr>
    : AIE_i64_mv<ins, opcodestr, argstr> {
	field bits<28> lng_base;
   let alu_st = 0;
   let mv_all = {0b000, lng_base};
}

class AIE_i64_jal_instr<dag ins, string opcodestr, string argstr>
    : AIE_i64_nop_lng<ins, opcodestr, argstr> {
	bits<20> addr;

   field bits<24> j_instr;
   let j_instr{23-21} = addr{19-17};
   let j_instr{20-17} = 0x0001;
   let j_instr{16-0} = addr{16-0};

   let lng_base = {0b1010, j_instr};
}

////////////////////////////
// 96-bit instructions
class AIE_i96_vec<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_instr96<outs, ins, pattern, opcodestr, argstr> {
   field bits<91> i96_novec;
   let instr96 = {i96_novec, 0b0};
}
class AIE_i96_novec<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : AIE_instr96<outs, ins, pattern, opcodestr, argstr> {
   field bits<91> i96_novec;
   let instr96 = {i96_novec, 0b1};
}
