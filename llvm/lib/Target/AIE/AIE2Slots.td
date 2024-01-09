//===-- AIE2Slots.td
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Slot definitions for describing high-level instruction formats. This
// architecture uses a hierarchical decomposition in which more complex VLIW
// instructions are defined in terms of simpler instructions. At the MC level,
// Toplevel instructions may have simpler instructions attached to them as
// operands. This avoids describing the same instruction in multiple places
// and also avoids a cross-product explosion of instruction specifications for
// VLIW formats.
//===----------------------------------------------------------------------===//

let Namespace = "AIE2" in
{
  def unknown_slot : InstSlot<"unknown", 0, true>;

  def lda_slot     : InstSlot<"lda", 21> {
    let DecoderMethod = "decodeLdaSlot";
    let FieldToFind = "lda";
  }

  def ldb_slot     : InstSlot<"ldb", 16> {
    let DecoderMethod = "decodeLdbSlot";
    let FieldToFind = "ldb";
  }

  def alu_slot     : InstSlot<"alu", 20> {
    let DecoderMethod = "decodeAluSlot";
    let FieldToFind = "alu";
  }

  def mv_slot     : InstSlot<"mv", 22> {
    let DecoderMethod = "decodeMvSlot";
    let FieldToFind = "mv";
  }

  def st_slot      : InstSlot<"st", 21> {
    let DecoderMethod = "decodeStSlot";
    let FieldToFind = "st";
  }

  def vec_slot : InstSlot<"vec", 26> {
    let DecoderMethod = "decodeVecSlot";
    let FieldToFind = "vec";
  }

  def lng_slot     : InstSlot<"lng", 42> {
    let DecoderMethod = "decodeLngSlot";
    let FieldToFind = "lng";
  }

  // Cooked up Slot for 16-bit InsnFormat.
  // In AIE2 this has a single instruction nop.
  def nop16_slot     : InstSlot<"nop16", 12> {
    let DecoderMethod = "decodeNopSlot";
    let FieldToFind = "nop16";
  }
}


class AIE2Inst<dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
    : AIEBaseInst <outs, ins, pattern> {
  field bits<32> dontcare = 0;
  let Namespace = "AIE2";
  let AsmString = opcodestr # "\t" # argstr;
}

class AIE2_inst_alu_instr32 <dag outs, dag ins, list<dag> pattern,
      string opcodestr = "", string argstr = "">
    : AIE2Inst <outs, ins, opcodestr, argstr, pattern> {
  bits<20> alu;
  field bits<20> Inst;

  let Inst = alu;
  let DecoderNamespace = "Alu";
  let Slot = alu_slot;
}

class AIE2_inst_lda_instr32 < dag outs, dag ins, list<dag> pattern,
      string opcodestr = "", string argstr = "">
    : AIE2Inst<outs, ins, opcodestr, argstr, pattern> {
  bits<21> lda;
  field bits<21> Inst;

  let Inst = lda;
  let DecoderNamespace = "Lda";
  let Slot = lda_slot;
}

class AIE2_inst_st_instr32 < dag outs, dag ins, list<dag> pattern,
      string opcodestr = "", string argstr = "">
    : AIE2Inst <outs, ins, opcodestr, argstr, pattern> {
  bits<21> st;
  field bits<21> Inst;

  let Inst = st;
  let DecoderNamespace = "St";
  let Slot = st_slot;
}

class AIE2_inst_ldb_instr32 < dag outs, dag ins, list<dag> pattern,
      string opcodestr = "", string argstr = "">
    : AIE2Inst<outs, ins, opcodestr, argstr, pattern> {
  bits<16> ldb;
  field bits<16> Inst;

  let Inst = ldb;
  let DecoderNamespace = "Ldb";
  let Slot = ldb_slot; 
}

class AIE2_inst_lng_instr48 < dag outs, dag ins, list<dag> pattern,
      string opcodestr = "", string argstr = "">
    : AIE2Inst <outs, ins, opcodestr, argstr, pattern> {
  bits<42> lng;
  field bits<42> Inst;

  let Inst = lng;
  let DecoderNamespace = "Lng";
  let Size = 6;
  let Slot = lng_slot;
}

class AIE2_inst_mv_instr32 < dag outs, dag ins, list<dag> pattern,
      string opcodestr = "", string argstr = "">
    : AIE2Inst <outs, ins, opcodestr, argstr, pattern> {
  bits<22> mv;
  field bits<22> Inst;

  let Inst = mv;
  let DecoderNamespace = "Mv";
  let Slot = mv_slot;
}

class AIE2_inst_vec_instr32 <dag outs, dag ins, list<dag> pattern,
      string opcodestr = "", string argstr = "">
    : AIE2Inst <outs, ins, opcodestr, argstr, pattern> {
  bits<26> vec;
  bits<26> Inst;

  let Inst = vec;
  let DecoderNamespace = "Vec";
  let Slot = vec_slot;
}

class AIE2_inst_nop_instr16 < dag outs, dag ins, list<dag> pattern,
      string opcodestr = "", string argstr = "">
  : AIE2Inst <outs, ins, opcodestr, argstr, pattern> {
  field bits<12> Inst;

  bits<12> nop16;
  field bits<12> Inst;
  
  let Size = 2;
  let Inst = nop16;
  let DecoderNamespace = "Nop";
  let Slot = nop16_slot;
}
