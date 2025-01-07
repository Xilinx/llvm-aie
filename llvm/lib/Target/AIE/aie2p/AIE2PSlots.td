//===-AIE2PSlots.td ---------------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//
//* Automatically generated file, do not edit! *


let Namespace = "AIE2P" in
{
  def ldb_slot : InstSlot<"Ldb", 17> {
   let FieldToFind = "ldb";
  }

  def alu_slot : InstSlot<"Alu", 20> {
   let FieldToFind = "alu";
  }

  def lng_slot : InstSlot<"Lng", 42> {
   let FieldToFind = "lng";
  }

  def lda_slot : InstSlot<"Lda", 20> {
   let FieldToFind = "lda";
  }

  def mv_slot : InstSlot<"Mv", 22> {
   let FieldToFind = "mv";
  }

  def st_slot : InstSlot<"St", 20> {
   let FieldToFind = "st";
  }

  def vec_slot : InstSlot<"Vec", 26> {
   let FieldToFind = "vec";
  }
}

// Include manually written file defining a target's slot instruction
include "AIE2PSlotInclude.td"

class AIE2PInst<dag outs, dag ins, string opcodestr, string argstr>
    : AIEBaseInst <outs, ins, [], opcodestr, argstr, "AIE2P"> {}

class AIE2PSlotInst<InstSlot slot, dag outs, dag ins, string opcodestr, string argstr>
    : AIE2PInst <outs, ins, opcodestr, argstr> {

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


class AIE2P_inst_ldb_instr32<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2PSlotInst<ldb_slot, outs, ins, opcodestr, argstr> {
  bits<17> ldb;
  bits<17> Inst = ldb;
}

class AIE2P_inst_alu_instr32<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2PSlotInst<alu_slot, outs, ins, opcodestr, argstr> {
  bits<20> alu;
  bits<20> Inst = alu;
}

class AIE2P_inst_lng_instr48<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2PSlotInst<lng_slot, outs, ins, opcodestr, argstr> {
  bits<42> lng;
  bits<42> Inst = lng;

  let Size = 6;
}

class AIE2P_inst_lda_instr32<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2PSlotInst<lda_slot, outs, ins, opcodestr, argstr> {
  bits<20> lda;
  bits<20> Inst = lda;
}

class AIE2P_inst_mv_instr32<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2PSlotInst<mv_slot, outs, ins, opcodestr, argstr> {
  bits<22> mv;
  bits<22> Inst = mv;
}

class AIE2P_inst_st_instr32<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2PSlotInst<st_slot, outs, ins, opcodestr, argstr> {
  bits<20> st;
  bits<20> Inst = st;
}

class AIE2P_inst_vec_instr32<dag outs, dag ins, string opcodestr, string argstr>
  : AIE2PSlotInst<vec_slot, outs, ins, opcodestr, argstr> {
  bits<26> vec;
  bits<26> Inst = vec;
}
