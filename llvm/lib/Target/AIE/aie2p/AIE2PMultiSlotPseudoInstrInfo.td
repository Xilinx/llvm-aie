//===- AIE2PMultiSlotPseudoInstrInfo.td -------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2025 Advanced Micro Devices, Inc. or its affiliates
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

// Pseudo VLD
let hasSideEffects = false, mayLoad = true, mayStore = false in {
  // Fifo fill.
  let Itinerary = II_VLDB_FILL_512 in {
  def VLD_FILL_512_pseudo : MultiSlot_Pseudo< (outs ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos),
      "vld_fill_512_pseudo", "[$ptr, $fifo_reg, $pos]",
      [VLDB_FILL_512, VLDA_FILL_512]>, AIE_HasTiedSubregister;
  }

  // Fifo normal pop.
  let Itinerary = II_VLDB_POP_512_normal_pop in
  let Defs = [srFifo_uf] in {
  def VLD_POP_512_normal_pop_pseudo : MultiSlot_Pseudo< (outs mXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos),
      "vld_pop_512_normal_pop_pseudo", "$dst, [$ptr, $fifo_reg, $pos]",
      [VLDB_POP_512_normal_pop, VLDA_POP_512_normal_pop]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_544_normal_pop in
  let Defs = [srFifo_uf] in {
  def VLD_POP_544_normal_pop_pseudo : MultiSlot_Pseudo< (outs mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos),
      "vld_pop_544_normal_pop_pseudo", "$dst, [$ptr, $fifo_reg, $pos]",
      [VLDB_POP_544_normal_pop, VLDA_POP_544_normal_pop]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_576_normal_pop in
  let Defs = [srFifo_uf] in {
  def VLD_POP_576_normal_pop_pseudo : MultiSlot_Pseudo<(outs mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos),
      "vld_pop_576_normal_pop_pseudo", "$dst, [$ptr, $fifo_reg, $pos]",
      [VLDB_POP_576_normal_pop, VLDA_POP_576_normal_pop]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_640_normal_pop in
  let Defs = [srFifo_uf] in {
  def VLD_POP_640_normal_pop_pseudo : MultiSlot_Pseudo<(outs mQXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos),
      "vld_pop_640_normal_pop_pseudo", "$dst, [$ptr, $fifo_reg, $pos]",
      [VLDB_POP_640_normal_pop, VLDA_POP_640_normal_pop]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_704_normal_pop in
  let Defs = [srFifo_uf] in {
  def VLD_POP_704_normal_pop_pseudo : MultiSlot_Pseudo<(outs mQEXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos),
      "vld_pop_704_normal_pop_pseudo", "$dst, [$ptr, $fifo_reg, $pos]",
      [VLDB_POP_704_normal_pop, VLDA_POP_704_normal_pop]>, AIE_HasTiedSubregister;
  }

  // Fifo 1D variants.
  let Itinerary = II_VLDB_POP_512_fifo_1d_pop in
  let Defs = [srFifo_uf] in {
  def VLD_POP_512_fifo_1d_pop_pseudo : MultiSlot_Pseudo<(outs mXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eM:$mod),
      "vld_pop_512_fifo_1d_pop_pseudo", "$dst, [$ptr, $fifo_reg, $pos, $mod]",
      [VLDB_POP_512_fifo_1d_pop, VLDA_POP_512_fifo_1d_pop]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_544_fifo_1d_pop in
  let Defs = [srFifo_uf] in {
  def VLD_POP_544_fifo_1d_pop_pseudo : MultiSlot_Pseudo<(outs mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eM:$mod),
      "vld_pop_544_fifo_1d_pop_pseudo", "$dst, [$ptr, $fifo_reg, $pos, $mod]",
      [VLDB_POP_544_fifo_1d_pop, VLDA_POP_544_fifo_1d_pop]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_576_fifo_1d_pop in
  let Defs = [srFifo_uf] in {
  def VLD_POP_576_fifo_1d_pop_pseudo : MultiSlot_Pseudo<(outs mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eM:$mod),
      "vld_pop_576_fifo_1d_pop_pseudo", "$dst, [$ptr, $fifo_reg, $pos, $mod]",
      [VLDB_POP_576_fifo_1d_pop, VLDA_POP_576_fifo_1d_pop]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_640_fifo_1d_pop in
  let Defs = [srFifo_uf] in {
  def VLD_POP_640_fifo_1d_pop_pseudo : MultiSlot_Pseudo<(outs mQXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eM:$mod),
      "vld_pop_640_fifo_1d_pop_pseudo", "$dst, [$ptr, $fifo_reg, $pos, $mod]",
      [VLDB_POP_640_fifo_1d_pop, VLDA_POP_640_fifo_1d_pop]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_704_fifo_1d_pop in
  let Defs = [srFifo_uf] in {
  def VLD_POP_704_fifo_1d_pop_pseudo : MultiSlot_Pseudo<(outs mQEXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eM:$mod),
      "vld_pop_704_fifo_1d_pop_pseudo", "$dst, [$ptr, $fifo_reg, $pos, $mod]",
      [VLDB_POP_704_fifo_1d_pop, VLDA_POP_704_fifo_1d_pop]>, AIE_HasTiedSubregister;
  }

  // Fifo 2D variants.
  let Itinerary = II_VLDB_POP_512_2D in
  let Defs = [srFifo_uf] in {
  def VLD_POP_512_2D_pseudo : MultiSlot_Pseudo<(outs OP_mXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDC:$dc),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eD:$mod),
      "vld_pop_512_2d_pseudo", "$dst, [$ptr, $fifo_reg, $pos, $mod]",
      [VLDB_POP_512_2D, VLDA_POP_512_2D]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_544_2D in
  let Defs = [srFifo_uf] in {
  def VLD_POP_544_2D_pseudo : MultiSlot_Pseudo<(outs mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDC:$dc),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eD:$mod),
      "vld_pop_544_2d_pseudo", "$dst, [$ptr, $fifo_reg, $pos, $mod]",
      [VLDB_POP_544_2D, VLDA_POP_544_2D]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_576_2D in
  let Defs = [srFifo_uf] in {
  def VLD_POP_576_2D_pseudo : MultiSlot_Pseudo<(outs mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDC:$dc),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eD:$mod),
      "vld_pop_576_2d_pseudo", "$dst, [$ptr, $fifo_reg, $pos, $mod]",
      [VLDB_POP_576_2D, VLDA_POP_576_2D]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_640_2D in
  let Defs = [srFifo_uf] in {
  def VLD_POP_640_2D_pseudo : MultiSlot_Pseudo<(outs mQXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDC:$dc),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eD:$mod),
      "vld_pop_640_2d_pseudo", "$dst, [$ptr, $fifo_reg, $pos, $mod]",
      [VLDB_POP_640_2D, VLDA_POP_640_2D]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_704_2D in
  let Defs = [srFifo_uf] in {
  def VLD_POP_704_2D_pseudo : MultiSlot_Pseudo<(outs mQEXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDC:$dc),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eD:$mod),
      "vld_pop_704_2d_pseudo", "$dst, [$ptr, $fifo_reg, $pos, $mod]",
      [VLDB_POP_704_2D, VLDA_POP_704_2D]>, AIE_HasTiedSubregister;
  }

  // Fifo 3D variants.
  let Itinerary = II_VLDB_POP_512_3D in
  let Defs = [srFifo_uf] in {
  def VLD_POP_512_3D_pseudo : MultiSlot_Pseudo<(outs OP_mXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDCL:$dcl, eDCH:$dch),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eDS:$mod),
      "vld_pop_512_3d_pseudo", "$dst, [$ptr, $fifo_reg, $pos, $mod]",
      [VLDB_POP_512_3D, VLDA_POP_512_3D]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_544_3D in
  let Defs = [srFifo_uf] in {
  def VLD_POP_544_3D_pseudo : MultiSlot_Pseudo<(outs mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDCL:$dcl, eDCH:$dch),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eDS:$mod),
      "vld_pop_544_3d_pseudo", "$dst, [$ptr, $fifo_reg, $pos, $mod]",
      [VLDB_POP_544_3D, VLDA_POP_544_3D]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_576_3D in
  let Defs = [srFifo_uf] in {
  def VLD_POP_576_3D_pseudo : MultiSlot_Pseudo<(outs mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDCL:$dcl, eDCH:$dch),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eDS:$mod),
      "vld_pop_576_3d_pseudo", "$dst, [$ptr, $fifo_reg, $pos, $mod]",
      [VLDB_POP_576_3D, VLDA_POP_576_3D]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_640_3D in
  let Defs = [srFifo_uf] in {
  def VLD_POP_640_3D_pseudo : MultiSlot_Pseudo<(outs mQXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDCL:$dcl, eDCH:$dch),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eDS:$mod),
      "vld_pop_640_3d_pseudo", "$dst, [$ptr, $fifo_reg, $pos, $mod]",
      [VLDB_POP_640_3D, VLDA_POP_640_3D]>, AIE_HasTiedSubregister;
  }

  let Itinerary = II_VLDB_POP_704_3D in
  let Defs = [srFifo_uf] in {
  def VLD_POP_704_3D_pseudo : MultiSlot_Pseudo<(outs mQEXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDCL:$dcl, eDCH:$dch),
                                          (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eDS:$mod),
      "vld_pop_704_3d_pseudo", "$dst, [$ptr, $fifo_reg, $pos, $mod]",
      [VLDB_POP_704_3D, VLDA_POP_704_3D]>, AIE_HasTiedSubregister;
  }
}
