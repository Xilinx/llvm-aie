//===- AIE2PMultiSlotPseudoInstrInfo.td -------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

// Define Instructions that can be materialized to different slots using 
// MulitSlot_Pseudo.The last parameter to the MulitSlot_Pseudo is 
// instruction into which the Multi-Slot Pseudo could be materialize to. 
// Priority to Slot/Instruction is based on the sequence in which 
// the real instructions are passed

// Multi-Slot Pseudo PADD
let mayLoad = false, mayStore = false, hasSideEffects = false, Itinerary = II_PADDA_pstm_nrm_imm in {
  let Constraints = "$ptr = $res" in {
    def PADD_mod_pseudo : MultiSlot_Pseudo< (outs eP:$res), (ins eP:$ptr, eM:$mod),
                          "padd_mod_pseudo", "[$ptr], $mod", 
                          [PADDA_pstm_nrm, PADDB_pstm_nrm, PADDS_pstm_nrm] >;
    def PADD_imm_pseudo : MultiSlot_Pseudo< (outs eP:$res), (ins eP:$ptr, c10s_step64:$imm),
                          "padd_imm_pseudo", "[$ptr], $imm", 
                          [PADDA_pstm_nrm_imm, PADDB_pstm_nrm_imm, PADDS_pstm_nrm_imm] >;
  }
}

// Pseudo MOV
let isMoveImm = 1, isReMaterializable = 1, isAsCheapAsAMove = 1, Itinerary = II_MOVX_mvx_cr_r,
    hasSideEffects = false, mayLoad = false, mayStore = false in {

  // To move 11bit imm to R/lc type reg.
  def MOV_RLC_imm11_pseudo : MultiSlot_Pseudo< (outs mAluCg:$dst) , (ins c11s:$i),
      "mov_rlc_imm11_pseudo ", "$dst, $i", [MOVA, MOVX_alu_cg, MOV_alu_mv_mv_mv_cg, MOVXM]>;
  // To move 11bit imm to P/D type reg.
  def MOV_PD_imm11_pseudo : MultiSlot_Pseudo< (outs mAguDst:$dst) , (ins c11s:$i),
      "mov_pd_imm11_pseudo ", "$dst, $i", [MOVA, MOV_alu_mv_mv_mv_cg, MOVXM]>;
  // To move 11bit imm to S type reg.
  def MOV_S_imm11_pseudo : MultiSlot_Pseudo< (outs eS:$dst) , (ins c11s:$i),
      "mov_s_imm11_pseudo ", "$dst, $i", [MOV_alu_mv_mv_mv_cg, MOVXM]>;
  // To move 11bit imm to Any type reg. (try to use the above first before using the following)
  def MOV_scalar_imm11_pseudo : MultiSlot_Pseudo< (outs mMvSclDst:$dst) , (ins c11s:$i),
      "mov_scalar_imm11_pseudo ", "$dst, $i", [MOV_alu_mv_mv_mv_cg, MOVXM]>;
}
