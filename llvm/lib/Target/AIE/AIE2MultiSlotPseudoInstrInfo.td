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

// Note : The itineraries of all instructions covered by a multi-slot pseudo should be equivalent. 
// We need to make sure the operand latency is same for all the possible instructions.

// Multi-Slot Pseudo PADD
let mayLoad = false, mayStore = false, hasSideEffects = false, Itinerary = II_PADD in {
  let Constraints = "$ptr = $res" in {
    def PADD_mod_pseudo : MultiSlot_Pseudo< (outs eP:$res), (ins eP:$ptr, eM:$mod),
                          "padd_mod_pseudo", "[$ptr], $mod", [PADDB_ldb_ptr_inc_nospill_nrm, PADDS_st_ptr_inc_idx, PADDA_lda_ptr_inc_idx] >;
    def PADD_imm9_pseudo : MultiSlot_Pseudo< (outs eP:$res), (ins eP:$ptr, imm9x4:$imm),
                          "padd_imm_pseudo", "[$ptr], $imm", [PADDB_ldb_ptr_inc_nrm_imm, PADDS_st_ptr_inc_idx_imm, PADDA_lda_ptr_inc_idx_imm] >;
    def PADD_imm10_pseudo : MultiSlot_Pseudo< (outs eP:$res), (ins eP:$ptr, imm10x4:$imm),
                          "padd_imm_pseudo", "[$ptr], $imm", [PADDA_lda_ptr_inc_idx_imm, PADDS_st_ptr_inc_idx_imm] >;
  }
  let Defs = [SP], Uses = [SP] in {
    def PADD_sp_imm_pseudo : MultiSlot_Pseudo< (outs ), (ins imm12x32:$imm),
                             "padd_sp_imm_pseudo", "[sp], $imm",
                             [PADDB_sp_imm, PADDA_sp_imm] >;
  }
}

// We use OR to mimic MOV behavior, and not ADD with 0 because ADD uses $srCarry register
let Itinerary = II_MOV_SCL_RS, isMoveReg = 1, hasSideEffects = false, mayLoad = false, mayStore = false in {
    def MOV_SCL_pseudo : MultiSlot_Pseudo<(outs eR:$mRx), (ins eR:$mRx0), 
                                          "mov_scl_pseudo", "$mRx, $mRx0",
                                          [MOV_mv_scl, MOV_OR]>;
}

// Pseudo MOV
let isMoveImm = 1, isReMaterializable = 1, isAsCheapAsAMove = 1, Itinerary = II_MOV,
    hasSideEffects = false, mayLoad = false, mayStore = false in {

  // To move 10bit imm to R/lc type reg.
  def MOV_RLC_imm10_pseudo : MultiSlot_Pseudo< (outs eRLC:$dst), (ins simm10:$i),
      "mov_rlc_imm10_pseudo ", "$dst, $i",
      [MOVA_lda_cg, MOVX_alu_cg, MOV_mv_cg, MOVXM_lng_cg]>;
  // To move 10bit imm to P/D type reg.
  def MOV_PD_imm10_pseudo : MultiSlot_Pseudo< (outs ePmDm:$dst), (ins simm10:$i),
      "mov_pd_imm10_pseudo ", "$dst, $i",
      [MOVA_lda_cg, MOV_mv_cg, MOVXM_lng_cg]>;
  // To move 10bit imm to S type reg.
  def MOV_S_imm10_pseudo : MultiSlot_Pseudo< (outs eS:$dst), (ins simm10:$i),
      "mov_s_imm10_pseudo ", "$dst, $i",
      [MOV_mv_cg, MOVXM_lng_cg]>;
  // To move 10bit imm to Any type reg. (try to use the above first before using the following)
  def MOV_scalar_imm10_pseudo : MultiSlot_Pseudo< (outs mMvSclDst:$dst), (ins simm10:$i),
      "mov_scalar_imm10_pseudo ", "$dst, $i",
      [MOV_mv_cg, MOVXM_lng_cg]>;


  // To move 11bit imm to R/lc type reg.
  def MOV_RLC_imm11_pseudo : MultiSlot_Pseudo< (outs eRLC:$dst), (ins simm11:$i),
      "mov_rlc_imm11_pseudo ", "$dst, $i",
      [MOVA_lda_cg, MOVX_alu_cg, MOVXM_lng_cg]>;
  // To move 11bit imm to P/D type reg.
  def MOV_PD_imm11_pseudo : MultiSlot_Pseudo< (outs ePmDm:$dst), (ins simm11:$i),
      "mov_PD_imm11_pseudo ", "$dst, $i",
      [MOVA_lda_cg, MOVXM_lng_cg]>;
}

// Pseudo VLD
let hasSideEffects = false, mayLoad = true, mayStore = false in {
  let Itinerary = II_VLDA_W in {
  def VLD_idx_pseudo : MultiSlot_Pseudo< (outs mWa:$dst), 
                                         (ins eP:$ptr, eDJ:$dj),
      "vld_idx_pseudo", "$dst, [$ptr, $dj]", 
      [VLDB_dmw_ldb_ag_idx, VLDA_dmw_lda_w_ag_idx]>;
  def VLD_idx_imm_3x32_pseudo : MultiSlot_Pseudo< (outs mWa:$dst), 
                                                  (ins eP:$ptr, imm3x32:$imm),
      "vld_idx_imm_imm3x32_pseudo", "$dst, [$ptr, $imm]", 
      [VLDB_dmw_ldb_ag_idx_imm, VLDA_dmw_lda_w_ag_idx_imm]>;
  }
  let Itinerary = II_VLDA_POSTINC_W in
  let Constraints = "$ptr_out = $ptr" in {
  def VLD_pstm_pseudo : MultiSlot_Pseudo< (outs mWa:$dst, eP:$ptr_out), 
                                          (ins eP:$ptr, eM:$mod),
      "vld_pstm_pseudo", "$dst, [$ptr], $mod", 
      [VLDB_dmw_ldb_ag_pstm_nrm, VLDA_dmw_lda_w_ag_pstm_nrm]>;
  def VLD_pstm_imm_4x32_pseudo : MultiSlot_Pseudo< (outs mWa:$dst, eP:$ptr_out),
                                                   (ins eP:$ptr, imm4x32:$imm),
      "vld_pstm_imm_4x32_pseudo", "$dst, [$ptr], $imm", 
      [VLDB_dmw_ldb_ag_pstm_nrm_imm, VLDA_dmw_lda_w_ag_pstm_nrm_imm]>;
  }
  let Itinerary = II_VLDA_2D_W in
  let Constraints = "$ptr_out = $ptr" in {
  def VLD_2D_pseudo : MultiSlot_Pseudo< (outs mWa:$dst, eP:$ptr_out, eDC:$count_out), 
                                        (ins eP:$ptr, eD:$mod),
      "vld.2d_pseudo", "$dst, [$ptr], $mod", 
      [VLDB_2D, VLDA_2D_dmw_lda_w]>, AIE_HasTiedSubregister;
  }
  let Itinerary = II_VLDA_3D_W in
  let Constraints = "$ptr_out = $ptr" in {
  def VLD_3D_pseudo : MultiSlot_Pseudo< (outs mWa:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out), 
                                        (ins eP:$ptr, eDS:$mod),
      "vld.3d_pseudo", "$dst, [$ptr], $mod", 
      [VLDB_3D, VLDA_3D_dmw_lda_w]>, AIE_HasTiedSubregister;
  }
}
