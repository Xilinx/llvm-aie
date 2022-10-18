//===- AIE2MultiSlotPseudoInstrInfo.td ---------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// Define Instructions that can be materialized to different slots using MulitSlot_Pseudo.
// The last parameter to the MulitSlot_Pseudo is instruction into which the Multi-Slot Pseudo could be materialize to.
// Priority to Slot/Instruction is based on the sequence in which the real instructions are passed

// Multi-Slot Pseudo PADD
let mayLoad = false, mayStore = false, hasSideEffects = false, Itinerary = II_PADD in {
  let Constraints = "$ptr = $res" in {
    def PADD_mod_pseudo : MultiSlot_Pseudo< (outs eP:$res), (ins eP:$ptr, eM:$mod),
                          "padd_mod_pseudo", "[$ptr], $mod", [PADDB_ldb_ptr_inc_nospill_nrm, PADDA_lda_ptr_inc_idx] >;
    def PADD_imm_pseudo : MultiSlot_Pseudo< (outs eP:$res), (ins eP:$ptr, imm9x4:$imm),
                          "padd_imm_pseudo", "[$ptr], $imm", [PADDB_ldb_ptr_inc_nrm_imm, PADDA_lda_ptr_inc_idx_imm] >;
  }
  let Defs = [SP], Uses = [SP] in {
    def PADD_sp_imm_pseudo : MultiSlot_Pseudo< (outs ), (ins imm12x32:$imm),
                             "padd_sp_imm_pseudo", "[sp], $imm", [PADDB_sp_imm, PADDA_sp_imm] >;
  }
}

// Pseudo MOV
let isMoveImm = 1, isReMaterializable = 1, isAsCheapAsAMove = 1, Itinerary = II_MOV,
    hasSideEffects = false, mayLoad = false, mayStore = false in {

  // To move 10bit imm to R/lc type reg.
  def MOV_RLC_imm10_pseudo : MultiSlot_Pseudo< (outs eRLC:$dst) , (ins simm10:$i),
      "mov_rlc_imm10_pseudo ", "$dst, $i", [MOVA_lda_cg, MOVX_alu_cg, MOV_mv_cg, MOVXM_lng_cg]>;
  // To move 10bit imm to P/D type reg.
  def MOV_PD_imm10_pseudo : MultiSlot_Pseudo< (outs ePmDm:$dst) , (ins simm10:$i),
      "mov_pd_imm10_pseudo ", "$dst, $i", [MOVA_lda_cg, MOV_mv_cg, MOVXM_lng_cg]>;
  // To move 10bit imm to S type reg.
  def MOV_S_imm10_pseudo : MultiSlot_Pseudo< (outs eS:$dst) , (ins simm10:$i),
      "mov_s_imm10_pseudo ", "$dst, $i", [MOV_mv_cg, MOVXM_lng_cg]>;
  // To move 10bit imm to Any type reg. (try to use the above first before using the following)
  def MOV_scalar_imm10_pseudo : MultiSlot_Pseudo< (outs mMvSclDst:$dst) , (ins simm10:$i),
      "mov_scalar_imm10_pseudo ", "$dst, $i", [MOV_mv_cg, MOVXM_lng_cg]>;


  // To move 11bit imm to R/lc type reg.
  def MOV_RLC_imm11_pseudo : MultiSlot_Pseudo< (outs eRLC:$dst) , (ins simm11:$i),
      "mov_rlc_imm11_pseudo ", "$dst, $i", [MOVA_lda_cg, MOVX_alu_cg, MOVXM_lng_cg]>;
  // To move 11bit imm to P/D type reg.
  def MOV_PD_imm11_pseudo : MultiSlot_Pseudo< (outs ePmDm:$dst) , (ins simm11:$i),
      "mov_PD_imm11_pseudo ", "$dst, $i", [MOVA_lda_cg, MOVXM_lng_cg]>;
}
