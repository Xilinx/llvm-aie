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
    let DecoderMethod = "decodeLdaInstruction";
    let FieldToFind = "lda";
  }

  def ldb_slot     : InstSlot<"ldb", 16> {
    let DecoderMethod = "decodeLdbInstruction";
    let FieldToFind = "ldb";
  }

  def alu_slot     : InstSlot<"alu", 20> {
    let DecoderMethod = "decodeAluInstruction";
    let FieldToFind = "alu";
  }

  def mv_slot     : InstSlot<"mv", 22> {
    let DecoderMethod = "decodeMvInstruction";
    let FieldToFind = "mv";
  }

  def st_slot      : InstSlot<"st", 21> {
    let DecoderMethod = "decodeStInstruction";
    let FieldToFind = "st";
  }

  def vec_slot : InstSlot<"vec", 26> {
    let DecoderMethod = "decodeVecInstruction";
    let FieldToFind = "vec";
  }

  def lng_slot     : InstSlot<"lng", 42> {
    let DecoderMethod = "decodeLngInstruction";
    let FieldToFind = "lng";
  }

  // Cooked up Slot for 16-bit InsnFormat.
  // In AIE2 this has a single instruction nop.
  def nop16_slot     : InstSlot<"nop16", 12> {
    let DecoderMethod = "decodeNopInstruction";
    let FieldToFind = "nop16";
  }
}
