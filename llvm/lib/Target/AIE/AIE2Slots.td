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

  def lda_slot     : InstSlot<"Lda", 21> {
    let FieldToFind = "lda";
  }

  def ldb_slot     : InstSlot<"Ldb", 16> {
    let FieldToFind = "ldb";
  }

  def alu_slot     : InstSlot<"Alu", 20> {
    let FieldToFind = "alu";
  }

  def mv_slot     : InstSlot<"Mv", 22> {
    let FieldToFind = "mv";
  }

  def st_slot      : InstSlot<"St", 21> {
    let FieldToFind = "st";
  }

  def vec_slot : InstSlot<"Vec", 26> {
    let FieldToFind = "vec";
  }

  def lng_slot     : InstSlot<"Lng", 42> {
    let FieldToFind = "lng";
  }

  // Cooked up Slot for 16-bit InsnFormat.
  // In AIE2 this has a single instruction nop. We keep the size small, so that
  // we can reuse it as artificial nop slot in other formats. This is another
  // trick, used to disambiguate equivalent formats with different size. 
  // As prime example, XM fits stand-alone in 48 bit and in 64 bits, where we
  // want to use the bigger one for alignmment purposes. We tell the encoder
  // which one to use by adding a nop instruction in the bundle. The encoder
  // will find the smallest format covering these two slots, and end up at the
  // 64 bit one. 
  def nop_slot     : InstSlot<"Nop", 1> {
    let FieldToFind = "nop";
  }
}


class AIE2Inst<dag outs, dag ins, string opcodestr, string argstr>
    : AIEBaseInst <outs, ins, []> {
  field bits<32> dontcare = 0;
  let Namespace = "AIE2";
  let AsmString = opcodestr # "\t" # argstr;
}

class AIE2SlotInst<InstSlot slot,
      dag outs, dag ins, string opcodestr, string argstr>
    : AIE2Inst <outs, ins, opcodestr, argstr> {

  let Slot = slot;

  // We use the slotname for the decode namespace, so that we will not have
  // any conflicts between the coding of distinct slots.
  let DecoderNamespace = slot.SlotName; 
  
  // The Size of a slot is mostly irrelevant, except in instructions that
  // need relocation. There we make a search for the smallest format in which
  // it fits, and then translate it to the slot where we need it.
  // We code the smallest size of a format that contains the slot, which are
  // always the stand-alone instructions. The default Size here covers most
  // slots; outliers are 2 for the 16 bit Nop slot and 6 for the Lng slot. 
  // Please also note that the slot decoder table name will include the bitsize
  // as a suffix.
  let Size = 4;
}

class AIE2_inst_alu_instr32<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2SlotInst<alu_slot, outs, ins, opcodestr, argstr> {
  bits<20> alu;
  bits<20> Inst = alu;
}

class AIE2_inst_lda_instr32<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2SlotInst<lda_slot, outs, ins, opcodestr, argstr> {
  bits<21> lda;
  bits<21> Inst = lda;
}

class AIE2_inst_st_instr32<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2SlotInst<st_slot, outs, ins, opcodestr, argstr> {
  bits<21> st;
  bits<21> Inst = st;
}

class AIE2_inst_ldb_instr32<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2SlotInst<ldb_slot, outs, ins, opcodestr, argstr> {
  bits<16> ldb;
  bits<16> Inst = ldb;
}

class AIE2_inst_lng_instr48<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2SlotInst<lng_slot, outs, ins, opcodestr, argstr> {
  bits<42> lng;
  bits<42> Inst = lng;
  
  let Size = 6;
}

class AIE2_inst_mv_instr32<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2SlotInst<mv_slot, outs, ins, opcodestr, argstr> {
  bits<22> mv;
  bits<22> Inst = mv;
}

class AIE2_inst_vec_instr32<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2SlotInst<vec_slot, outs, ins, opcodestr, argstr> {
  bits<26> vec;
  bits<26> Inst = vec;
}

class AIE2_inst_nop_instr16<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2SlotInst<nop_slot, outs, ins, opcodestr, argstr> {
  bits<1> nop;
  bits<1> Inst = nop;
  
  let Size = 2;
}
