//===-- AIE2PInstrInfo.td - Target Description for AIE2p --*- tablegen --*-==//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file describes the AIE2p instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// AIE2p specific DAG Nodes.
//===----------------------------------------------------------------------===//

include "AIEBaseInstrInfo.td"
include "aie2p/AIE2PInstrFormats.td"
include "aie2p/AIE2PCompositeFormats.td"
include "aie2p/AIE2PImmOperands.td"
include "AIE2PRegOperandDef.td"
include "aie2p/AIE2PGenInstrInfo.td"

let isMeta = true, hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
  def DelayedSchedBarrier : Pseudo<(outs), (ins)>;
}

// Pessimistically assume the stack pointer will be clobbered
let Defs = [sp], Uses = [sp],
    hasSideEffects=false, mayLoad=false, mayStore=false in {
  def ADJCALLSTACKUP   : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2)>;
  def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2)>;
}


// Pre-scheduling pseudo instruction for RET
let Uses = [lr],
    hasDelaySlot = 1, isBarrier = 1, isReturn = 1, isTerminator = 1,
    hasSideEffects = false, mayLoad = 0, mayStore = 0 in
def PseudoRET : Pseudo<(outs), (ins)>,
                PreSchedInstExpansion<RET, DelayedSchedBarrier>;

// Pre-scheduling pseudo instructions for calls.
let Defs = [lr],
    hasDelaySlot = true, isCall = true, isTerminator = false,
    hasSideEffects = true, mayLoad = false, mayStore = false  in {
  def PseudoJL : Pseudo<(outs), (ins c20u:$cpmaddr)>,
      PreSchedInstExpansion<JL_lng, DelayedSchedBarrier>;
  def PseudoJL_IND : Pseudo<(outs), (ins eP:$mPm)>,
      PreSchedInstExpansion<JL_alumv_or, DelayedSchedBarrier>;
}

//NOP
let hasSideEffects = 0, mayLoad = 0, mayStore = 0, isSlotNOP = true in {
  def NOP : AIE2P_nop_nop_inst_nop<(outs), (ins), "nop">;
}

let hasNoSchedulingInfo = true, hasSideEffects = true, mayLoad = false, mayStore = false, isConvergent = true, isMeta = true in {
  def SCHED_BARRIER : Pseudo<(outs), (ins)>;
}

// Loop related Pseudos
// JNZD loops
let hasSideEffects = 0, mayLoad = 0, mayStore = 0, Defs = [srCarry] in
def LoopDec :
    Pseudo<(outs eR:$dst), (ins eR:$src), "loop_dec", "${dst}, ${src}">;

let hasSideEffects = 0,  mayLoad = 0, mayStore = 0, isBranch = 1, isIndirectBranch = 1, isTerminator = true in
def LoopJNZ :
    Pseudo<(outs), (ins eR:$tc, eP:$target), "loop_jnz", "${tc}, ${target}">;

// Zero overhead loops
let hasSideEffects = true, mayLoad = false, mayStore = false,
    isNotDuplicable = true in {
  def LoopStart : Pseudo<(outs), (ins eR:$src, c6s:$adj), "loop_start", "${src}, ${adj}">;

  // PseudoLoopEnd represents the terminator of a ZOL. You can view it as a
  // conditional branch to the loop body. We make this a meta instruction,
  // since it doesn't need to be emitted itself, it only carries info for
  // its predecessor bundle
  let isBranch = true, isTerminator = true, isMeta = true, Uses = [ls, le] in
    def PseudoLoopEnd : Pseudo<(outs), (ins cpmaddr:$lastInstr, cpmaddr:$target),
      "pseudo_loop_end", "${lastInstr}, ${target}">;
}
// Pseudo Copy
// Target specific impl. of isCopyInstrImp causes the MOVE to be deleted, the Pseudo is
// kind of work around so that its not deleted
let Itinerary = II_MOV_alu_mv_mv_mv_scl, hasSideEffects = 0,  mayLoad = 0, mayStore = 0 in {
  def PseudoMove :
      Pseudo<(outs OP_mMvSclDst:$mMvSclDst), (ins OP_mMvSclSrc:$mMvSclSrc), "copy","$mMvSclDst, $mMvSclSrc">;
}

let isMeta = true, hasSideEffects = 0, mayLoad = 0, mayStore = 0 in {
  def CYCLE_SEPARATOR : Pseudo<(outs), (ins)>;
}

// Pseudo instructions for branches. Those are used by ISel and carry the right
// attributes for optimizations to work. They get expanded right before post-RA
// scheduling to target instructions with encoding information.
let hasDelaySlot = true, isBranch = true, isTerminator = true,
    hasSideEffects = false, mayLoad = 0, mayStore = 0 in {
  def PseudoJZ : Pseudo<(outs), (ins eR:$mRx, cpmaddr:$cpmaddr)>,
      PreSchedInstExpansion<JZ, DelayedSchedBarrier>;
  def PseudoJNZ : Pseudo<(outs), (ins eR:$mRx, cpmaddr:$cpmaddr)>,
      PreSchedInstExpansion<JNZ, DelayedSchedBarrier>;

  let isIndirectBranch = true, Defs = [srCarry] in
  def PseudoJNZD : Pseudo<(outs eR:$mRx), (ins eR:$mRx0, eP:$mPm)>,
      PreSchedInstExpansion<JNZD, DelayedSchedBarrier>;

  let isBarrier = true in {
    def PseudoJ_jump_imm : Pseudo<(outs), (ins cpmaddr:$cpmaddr)>,
        PreSchedInstExpansion<J_lng, DelayedSchedBarrier>;
    def PseudoJ_jump_ind : Pseudo<(outs), (ins eP:$mPm)>,
        PreSchedInstExpansion<J_alumv_or, DelayedSchedBarrier>;
  }
    let isCall = true, isReturn = true in {
    def PseudoJ_TCO_jump_imm : Pseudo<(outs), (ins cpmaddr:$cpmaddr)>,
        PreSchedInstExpansion<J_lng, DelayedSchedBarrier>;
    def PseudoJ_TCO_jump_ind : Pseudo<(outs), (ins eP:$mPm)>,
        PreSchedInstExpansion<J_alumv_or, DelayedSchedBarrier>;    
    }
}

// Modified OR instruction to mimic MOV operation
// OR $dst, $src, $src is equivalent to MOV $dst, $src 
let Itinerary = II_OR, isCodeGenOnly = 1, hasSideEffects = false,
    mayLoad = 0, mayStore = 0 in {
  def MOV_OR : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0), "or", " $d0, $s0, $s0">{
    bits<5> s0;
    bits<5> d0;
    let alu = {s0, d0, s0, 0b0101, 0b1};
  }    
}

include "aie2p/AIE2PMultiSlotPseudoInstrInfo.td"
// Define _split variants for instructions using 2D registers
class Split2DInstr<Instruction RealInst, int opidx> : SplitPseudo<RealInst,
    opidx, (ins eM:$mod_2d, eDN:$dim_size, eDJ:$dim_stride, eDC:$dim_count)> {}

foreach instr = [PADDA_2D, PADDB_2D, PADDS_2D, PADD_2D_pseudo] in
    def instr # _split : Split2DInstr<instr, /*opidx=*/3>;

foreach instr = [
  LDA_2D_dms_lda,
  LDA_2D_s8,
  LDA_2D_u16,
  LDA_2D_u8,
  LDA_TM_2D,
  ST_2D_dms_sts,
  ST_2D_dmv_sts_q,
  ST_2D_s16,
  ST_2D_s8,
  ST_TM_2D,
  VLDA_2D_128,
  VLDA_2D_CONV_fp32_bf16_dmw_lda_ups_bf,
  VLDA_2D_CONV_fp32_bf16_dmx_lda_ups_bf,
  VLDA_2D_dmw_lda_w,
  VLDA_2D_dmx_lda_bm,
  VLDA_2D_dmx_lda_fifohl,
  VLDA_2D_dmx_lda_x,
  VLDB_2D_128,
  VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign0,
  VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign1,
  VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign0,
  VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign1,
  VLDB_2D_dmw_ldb,
  VLDB_2D_dmx_ldb_x,
  VST_2D_128,
  VST_2D_CONV_bf16_fp32_dmw_sts_srs_bf,
  VST_2D_CONV_bf16_fp32_dmx_sts_srs_bf,
  VST_2D_PACK_dmw_sts_pack_packSign0,
  VST_2D_PACK_dmw_sts_pack_packSign1,
  VST_2D_PACK_dmx_sts_pack_packSign0,
  VST_2D_PACK_dmx_sts_pack_packSign1,
  VST_2D_dmw_sts_w,
  VST_2D_dmx_sts_bm,
  VST_2D_dmx_sts_fifohl,
  VST_2D_dmx_sts_x, VLD_2D_128_pseudo, VLD_2D_w_pseudo, VLD_2D_x_pseudo] in
    def instr # _split : Split2DInstr<instr, /*opidx=*/4>;

foreach instr = [
  VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign0,
  VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign1,
  VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign0,
  VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign1,
  VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign0,
  VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign1, 
  VST_2D_SRS_2x_dm_sts_srs_cm_srsSign0,
  VST_2D_SRS_2x_dm_sts_srs_cm_srsSign1, 
  VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign0,
  VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign1, 
  VST_2D_SRS_4x_dm_sts_srs_cm_srsSign0,
  VST_2D_SRS_4x_dm_sts_srs_cm_srsSign1, 
  VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign0,
  VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign1] in
    def instr # _split : Split2DInstr<instr, /*opidx=*/5>;

foreach instr = [
     VST_FLUSH_512_2D, VST_FLUSH_512_CONV_2D] in
    def instr # _split : Split2DInstr<instr, /*opidx=*/7>;

foreach instr = [
    VLDA_POP_512_2D, VLDA_POP_544_2D, VLDA_POP_576_2D, VLDA_POP_640_2D,
    VLDA_POP_704_2D, VLDB_POP_512_2D, VLDB_POP_544_2D, VLDB_POP_576_2D,
    VLDB_POP_640_2D, VLDB_POP_704_2D,
    VLD_POP_512_2D_pseudo, VLD_POP_544_2D_pseudo, VLD_POP_576_2D_pseudo, 
    VLD_POP_640_2D_pseudo, VLD_POP_704_2D_pseudo] in
    def instr # _split : Split2DInstr<instr, /*opidx=*/8>;

// Define _split variants for instructions using 3D registers
class Split3DInstr<Instruction RealInst, int opidx> : SplitPseudo<RealInst,
    opidx, (ins eM:$mod1, eDN:$dim_size1, eDJ:$dim_stride1, eDC:$dim_count1,
                eM:$mod2, eDN:$dim_size2, eDJ:$dim_stride2, eDC:$dim_count2)> {}

foreach instr = [PADDA_3D, PADDB_3D, PADDS_3D, PADD_3D_pseudo] in
    def instr # _split : Split3DInstr<instr, /*opidx=*/4>;

foreach instr = [
  LDA_3D_dms_lda,
  LDA_3D_dmv_lda_q,
  LDA_3D_s16,
  LDA_3D_s8,
  LDA_3D_u16,
  LDA_3D_u8,
  LDA_TM_3D,
  ST_3D_dms_sts,
  ST_3D_dmv_sts_q,
  ST_3D_s16,
  ST_3D_s8,
  ST_TM_3D,
  VLDA_3D_128,
  VLDA_3D_CONV_fp32_bf16_dmw_lda_ups_bf,
  VLDA_3D_CONV_fp32_bf16_dmx_lda_ups_bf,
  VLDA_3D_dmw_lda_w,
  VLDA_3D_dmx_lda_bm,
  VLDA_3D_dmx_lda_fifohl,
  VLDA_3D_dmx_lda_x,
  VLDB_3D_128,
  VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign0,
  VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign1,
  VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign0,
  VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign1,
  VLDB_3D_dmw_ldb,
  VLDB_3D_dmx_ldb_x,
  VST_3D_128,
  VST_3D_CONV_bf16_fp32_dmw_sts_srs_bf,
  VST_3D_CONV_bf16_fp32_dmx_sts_srs_bf,
  VST_3D_PACK_dmw_sts_pack_packSign0,
  VST_3D_PACK_dmw_sts_pack_packSign1,
  VST_3D_PACK_dmx_sts_pack_packSign0,
  VST_3D_PACK_dmx_sts_pack_packSign1,
  VST_3D_dmw_sts_w,
  VST_3D_dmx_sts_bm,
  VST_3D_dmx_sts_fifohl,
  VST_3D_dmx_sts_x,
  VLD_3D_w_pseudo,
  VLD_3D_x_pseudo,
  VLD_3D_128_pseudo] in       
    def instr # _split : Split3DInstr<instr, /*opidx=*/5>;

foreach instr = [ VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign0,
  VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign1, 
  VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign0, 
  VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign1, 
  VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign0, 
  VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign1, 
  VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign0, 
  VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign1, 
  VST_3D_SRS_2x_dm_sts_srs_cm_srsSign0, 
  VST_3D_SRS_2x_dm_sts_srs_cm_srsSign1, 
  VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign0, 
  VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign1, 
  VST_3D_SRS_4x_dm_sts_srs_cm_srsSign0, 
  VST_3D_SRS_4x_dm_sts_srs_cm_srsSign1, 
  VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign0, 
  VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign1] in   
    def instr # _split : Split3DInstr<instr, /*opidx=*/6>;

foreach instr = [
    VST_FLUSH_512_3D, VST_FLUSH_512_CONV_3D] in
    def instr # _split : Split3DInstr<instr, /*opidx=*/8>;

foreach instr = [
    VLDA_POP_512_3D, VLDA_POP_544_3D, VLDA_POP_576_3D, VLDA_POP_640_3D,
    VLDA_POP_704_3D, VLDB_POP_512_3D, VLDB_POP_544_3D, VLDB_POP_576_3D,
    VLDB_POP_640_3D, VLDB_POP_704_3D,
    VLD_POP_512_3D_pseudo, VLD_POP_544_3D_pseudo, VLD_POP_576_3D_pseudo,
    VLD_POP_640_3D_pseudo, VLD_POP_704_3D_pseudo] in
    def instr # _split : Split3DInstr<instr, /*opidx=*/9>;

let hasSideEffects = 0, Uses = [sp] in {
let mayLoad = false, mayStore = true in {
// Although it is ideal to avoid the use of ST_R_SPILL and LDA_R_SPILL,
// these instructions are included to simplify handling in certain
// scenarios. For example, they help eliminate the need for custom handling
// in places like eliminateFrameIndex. By including these instructions, we
// can streamline the code and improve readability.
def ST_R_SPILL : Pseudo<(outs ), (ins OP_mSclSt:$src, c12n_step4:$imm), "vst_r_spill", "$src, [sp, $imm]">;
def VST_L_SPILL : Pseudo<(outs ), (ins eL:$src, c12n_step4:$imm), "vst_l_spill", "$src, [sp, $imm]">;
def VST_Y_SPILL : Pseudo<(outs ), (ins VEC1024:$src, c16n_step64:$imm), "vst_y_spill", "${src}, [sp, $imm]">;
def VST_CM_SPILL : Pseudo<(outs ), (ins ACC1024:$src, c16n_step64:$imm), "vst_cm_spill", "${src}, [sp, $imm]">;
def VST_DM_SPILL : Pseudo<(outs ), (ins ACC2048:$src, c16n_step64:$imm), "vst_dm_spill", "${src}, [sp, $imm]">;
def ST_D_SPILL : Pseudo<(outs ), (ins eD:$src, c12n_step4:$imm), "st_d_spill", "$src, [sp, $imm]">;
def ST_DS_SPILL : Pseudo<(outs ), (ins eDS:$src, c12n_step4:$imm), "st_ds_spill", "$src, [sp, $imm]">;
def VST_FIFO_SPILL : Pseudo<(outs ), (ins FIFO1024:$src, c16n_step64:$imm), "vst_fifo_spill", "${src}, [sp, $imm]">;
def VST_PLFR_SPILL : Pseudo<(outs ), (ins ePSRFLdF:$src, c16n_step64:$imm), "vst_plfr_spill", "${src}, [sp, $imm]">;
def VST_EX_SPILL : Pseudo<(outs ), (ins VEC576:$src, c16n_step64:$imm), "vst_ex_spill", "${src}, [sp, $imm]">;
def VST_E_SPILL : Pseudo<(outs ), (ins EXPVEC64:$src, c12n_step4:$imm), "vst_e_spill", "$src, [sp, $imm]">;
def VST_512_COMPOSED_REG_SPILL : Pseudo<(outs ), (ins spill_vec512_to_composite:$src, c16n_step64:$imm), "vst_512_composed_reg_spill", "${src}, [sp, $imm]">;
}

let mayLoad = true, mayStore = false in {
def LDA_R_SPILL : Pseudo<(outs OP_mLdaScl:$dst), (ins c12n_step4:$imm), "vlda_r_spill", "$dst, [sp, $imm]">;
def VLDA_L_SPILL : Pseudo<(outs eL:$dst), (ins c12n_step4:$imm), "vlda_l_spill", "$dst, [sp, $imm]">;
def VLDA_Y_SPILL : Pseudo<(outs VEC1024:$dst), (ins c16n_step64:$imm), "vlda_y_spill", "${dst}, [sp, $imm]">;
def VLDA_CM_SPILL : Pseudo<(outs ACC1024:$dst), (ins c16n_step64:$imm), "vlda_cm_spill", "${dst}, [sp, $imm]">;
def VLDA_FIFO_SPILL : Pseudo<(outs FIFO1024:$dst), (ins c16n_step64:$imm), "vlda_fifo_spill", "${dst}, [sp, $imm]">;
def VLDA_PLFR_SPILL : Pseudo<(outs ePSRFLdF:$dst), (ins c16n_step64:$imm), "vlda_plfr_spill", "${dst}, [sp, $imm]">;
def VLDA_DM_SPILL : Pseudo<(outs ACC2048:$dst), (ins c16n_step64:$imm), "vlda_dm_spill", "${dst}, [sp, $imm]">;
def LDA_D_SPILL : Pseudo<(outs eD:$dst), (ins c12n_step4:$imm), "lda_d_spill", "${dst}, [sp, $imm]">;
def LDA_DS_SPILL : Pseudo<(outs eDS:$dst), (ins c12n_step4:$imm), "lda_ds_spill", "${dst}, [sp, $imm]">;
def VLDA_EX_SPILL : Pseudo<(outs VEC576:$dst), (ins c16n_step64:$imm), "vlda_ex_spill", "${dst}, [sp, $imm]">;
def VLDA_E_SPILL : Pseudo<(outs EXPVEC64:$dst), (ins c12n_step4:$imm), "vlda_e_spill", "${dst}, [sp, $imm]">;
def VLDA_512_COMPOSED_REG_SPILL : Pseudo<(outs spill_vec512_to_composite:$dst), (ins c16n_step64:$imm), "vlda_512_composed_reg_spill", "${dst}, [sp, $imm]">;
}
}


include "aie2p/AIE2PInstrPatterns.td"
