//===- AIE2GenFixupInstrInfo.td ----------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// Naming scheme:
// 1. If there is a single instruction (after rule expansion) with a given asm,
// string, use that (in uppercase, and translating special characters like `.`
// to `_`). E.g. VSHIFT, or VLDA_128, VST_SRS...
//
// 2. If the ASM string isn't enough to get unique opcodes, use the name of the
// instruction rule as a suffix. E.g. VMOV_mv_w and VMOV_mv_x
//
// 3. If the names still aren't unique, run these sub-steps:
//  - Create extra suffixes by concatenating the names of operand rules, the
//    order being given by a DFS traversal. One should limit the depth of the
//    DFS (and then iteratively increment it) until the names are unique for
//    each instruction. Note that if a rule appears multiple times as an
//    operand, the DFS should only visit it once.
//  - If there are suffixes that are common to all the instructions to
//    be uniqued, they are stripped from the final names.
//  - Find the minimum amount of suffixes needed to make the names unique.
//    E.g. VST_dmw_sts_w_ag_idx_mWs and VST_dmw_sts_am_ag_idx_imm_mAMs become
//    VST_dmw_sts_w_ag_idx and VST_dmw_sts_am_ag_idx_imm. The mWs and mAMs
//    suffixes weren't needed.
//  - Note that rule names are atomic in the sense that we never eliminate
//    substrings that are common to e.g. sibling suffixes.
//
// E.g. for solving the VMUL conflict (similar for VNEGMUL):
// After step 2. there are still multiple instructions matching VMUL_vmul.
// Apply step 3.:
// Try DFS with depth=1:
//  VMUL_vmul_vmac_cm_core_mCMa_addsub_add_vmac_cm1_0_vmac_cm2_0
//  VMUL_vmul_vmac_cm_core_mCMa_addsub_add_vmac_cm1_0_vmac_cm2_0
//  VMUL_vmul_vmac_cm_core_mCMa_addsub_add_vmac_cm1_0_vmac_cm2_0
// Try DFS with depth=2:
//  VMUL_vmul_vmac_cm_core_dense_mCMa_addsub_add_vmac_cm1_0_vmac_cm2_0
//  VMUL_vmul_vmac_cm_core_sparse_wide_mCMa_addsub_add_vmac_cm1_0_vmac_cm2_0
//  VMUL_vmul_vmac_cm_core_sparse_narrow_mCMa_addsub_add_vmac_cm1_0_vmac_cm2_0
// Names are now unique, clean up common suffixes:
//  [vmul,vmac_cm_core_dense,mCMa,addsub_add,vmac_cm1_0,vmac_cm2_0]
//  [vmul,vmac_cm_core_sparse_wide,mCMa,addsub_add,vmac_cm1_0,vmac_cm2_0]
//  [vmul,vmac_cm_core_sparse_narrow,mCMa,addsub_add,vmac_cm1_0,vmac_cm2_0]
//  --becomes-->
//  [vmac_cm_core_dense]
//  [vmac_cm_core_sparse_wide]
//  [vmac_cm_core_sparse_narrow]
// Final:
//  VMUL_vmac_cm_core_dense
//  VMUL_vmac_cm_core_sparse_wide
//  VMUL_vmac_cm_core_sparse_narrow
//
// E.g. for solving the VST conflict:
// Try step 2.
//  VST_dmw_sts_w
//  VST_dmw_sts_am
//  VST_dmv_sts_q
// Try step 3. (still multiple addressing modes):
// Try DFS with depth=1:
//  VST_dmw_sts_w_ags_memw_mWs
//  VST_dmw_sts_am_ags_memw_mAMs
//  VST_dmv_sts_q_ags_memv_mQQs
// Try DFS with depth=2 (still multiple addressing modes):
//  ...
// Try DFS with depth=3 (still multiple addressing modes):
// Note: not expanding "register class" operands like mWs, mAMs, mQQs
//  VST_dmw_sts_w_mem_nrm_ag_idx_mWs
//  VST_dmw_sts_w_mem_nrm_ag_idx_imm_mWs
//  ...
//  VST_dmw_sts_am_mem_nrm_ag_idx_mAMs
//  VST_dmw_sts_am_mem_nrm_ag_idx_imm_mAMs
//  ...
//  VST_dmv_sts_q_mem_nrm_ag_idx_mQQs
//  VST_dmv_sts_q_mem_nrm_ag_idx_imm_mQQs
//  ...
// Names are now unique, clean up common suffixes:
//  VST_dmw_sts_w_ag_idx_mWs
//  VST_dmw_sts_w_ag_idx_imm_mWs
//  ...
//  VST_dmw_sts_am_ag_idx_mAMs
//  VST_dmw_sts_am_ag_idx_imm_mAMs
//  ...
//  VST_dmv_sts_q_ag_idx_mQQs
//  VST_dmv_sts_q_ag_idx_imm_mQQs
// Find the minimum amount of suffixes:
//  VST_dmw_sts_w_ag_idx
//  VST_dmw_sts_w_ag_idx_imm
//  ...
//  VST_dmw_sts_am_ag_idx
//  VST_dmw_sts_am_ag_idx_imm
//  ...
//  VST_dmv_sts_q_ag_idx
//  VST_dmv_sts_q_ag_idx_imm
// Done.

/////
// vmac_bm_core: multiply/accumulate modes for bfloat vectors.
class vmac_bm_core {
  bits<5> vmac_bm_core_0 = 0;
  bits<8> vmac_bm_core_1 = 0;
}

class vmac_bm_core_dense : vmac_bm_core {
  bits<5> c;
  bits<4> s1;
  bits<4> s2;
  let vmac_bm_core_0 = {c};
  let vmac_bm_core_1 = {s1, s2};
}

class vmac_bm_core_sparse_wide : vmac_bm_core {
  bits<5> c;
  bits<2> ys1;
  bits<2> qxs2;
  let vmac_bm_core_0 = {c};
  let vmac_bm_core_1 = {0b11, ys1, 0b11, qxs2};
}

class vmac_bm_core_sparse_narrow : vmac_bm_core {
  bits<5> c;
  bits<4> xs1;
  bits<2> qxs2;
  let vmac_bm_core_0 = {c};
  let vmac_bm_core_1 = {xs1, 0b11, qxs2};
}

// vmac_cm_core: multiply/accumulate modes for vectors.
class vmac_cm_core {
  bits<5> vmac_cm_core_0 = 0;
  bits<8> vmac_cm_core_1 = 0;
}

class vmac_cm_core_dense : vmac_cm_core {
  bits<5> c;
  bits<4> s1;
  bits<4> s2;
  let vmac_cm_core_0 = {c};
  let vmac_cm_core_1 = {s1, s2};
}

class vmac_cm_core_sparse_wide : vmac_cm_core {
  bits<5> c;
  bits<2> ys1;
  bits<2> qxs2;
  let vmac_cm_core_0 = {c};
  let vmac_cm_core_1 = {0b11, ys1, 0b11, qxs2};
}

class vmac_cm_core_sparse_narrow : vmac_cm_core {
  bits<5> c;
  bits<4> xs1;
  bits<2> qxs2;
  let vmac_cm_core_0 = {c};
  let vmac_cm_core_1 = {xs1, 0b11, qxs2};
}

class mWsrs {
  bits<10> mWsrs;
}
class mWdssrs<bits<1> md, bits<1> sgn> : mWsrs {
  bits<2> shft;
  bits<4> src;
  let mWsrs = {shft, md, sgn, src, 0b00};
}
class mWlsrsl<bits<1> md, bits<1> sgn> : mWsrs {
  bits<2> shft;
  bits<5> src;
  let mWsrs = {shft, md, sgn, src, 0b1};
}
class mXdlsrs<bits<1> md, bits<1> sgn> {
  bits<8> mXdlsrs;
  bits<2> shft;
  bits<4> src;
  let mXdlsrs = {shft, md, sgn, src};
}
class mWlsrsh<bits<1> md, bits<1> sgn> {
  bits<9> mWlsrsh;
  bits<2> shft;
  bits<5> src;
  let mWlsrsh = {shft, src, md, sgn};
}
class mv_ups_base<bits<1> md, bits<1> sgn> {
  bits<4> mv_ups_base;
  bits<2> shft;
  let mv_ups_base = {shft, md, sgn};
}
class mv_pack_src<bits<1> sgn, bits<1> sz> {
  bits<8> mv_pack_src;
  bits<4> src;
  let mv_pack_src = {sgn, sz, 0b00, src};
}
// Insert modes for vectors
class vec_insert_src {
  bits<2> vec_insert_src_0;
  bits<5> vec_insert_src_1;
}
class vec_insert_src_R<bits<2> sz> : vec_insert_src {
  bits<5> s0;
  let vec_insert_src_0 = {sz};
  let vec_insert_src_1 = {s0};
}
class vec_insert_src_L<bits<2> sz> : vec_insert_src {
  bits<3> s0;
  field bits<2> dontcare2;
  let vec_insert_src_0 = {sz};
  let vec_insert_src_1 = {s0, dontcare2};
}

// Push modes for vectors
class vec_push_lo {
  bits<20> vec_push_lo;
}
class vec_push_lo_R<bits<2> sz> : vec_push_lo {
  bits<4> d;
  bits<5> s1;
  bits<4> s2;
  field bits<2> dontcare2;
  field bits<1> dontcare1;
  let vec_push_lo = {d, sz, dontcare1, s1, 0b11, dontcare2, s2};
}
class vec_push_lo_L<bits<2> sz> : vec_push_lo {
  bits<4> d;
  bits<3> s1;
  bits<4> s2;
  field bits<2> dontcare2;
  field bits<1> dontcare1;
  let vec_push_lo = {d, sz, dontcare1, s1, dontcare2, 0b11, dontcare2, s2};
}
class vec_push_hi {
  bits<16> vec_push_hi;
}
class vec_push_hi_R<bits<2> sz> : vec_push_hi {
  bits<4> d;
  bits<4> s1;
  bits<5> s2;
  let vec_push_hi = {d, sz, 0b1, s2, s1};
}
class vec_push_hi_L<bits<2> sz> : vec_push_hi {
  bits<4> d;
  bits<4> s1;
  bits<3> s2;
  field bits<2> dontcare2;
  let vec_push_hi = {d, sz, 0b1, s2, dontcare2, s1};
}

// VEXTRACT modes
class vec_extract_dest {
  bits<2> vec_extract_dest_0;
  bits<7> vec_extract_dest_1;
}
class vec_extract_dest_R<bits<2> sz> : vec_extract_dest {
  bits<7> dst;
  let vec_extract_dest_0 = {sz};
  let vec_extract_dest_1 = {dst};
}
class vec_extract_dest_L<bits<2> sz> : vec_extract_dest {
  bits<3> dst;
  field bits<4> dontcare4;
  let vec_extract_dest_0 = {sz};
  let vec_extract_dest_1 = {dst,dontcare4};
}

// VEXTBCST : mExtractIdx
class mExtractIdx {
  bits<6> mExtractIdx;
}
class mExtractIdx_reg : mExtractIdx {
  bits<5> idx;
  let mExtractIdx = {0b0, idx};
}
class mExtractIdx_imm : mExtractIdx {
  bits<5> idx;
  let mExtractIdx = {0b1, idx};
}

// mFl2FxSrc sources for mv_float_to_int
class mv_mFl2FxSrc {
  bits<1> mFl2FxSrc_0;
  bits<6> mFl2FxSrc_1;
}
class mv_mFl2FxSrc_AM : mv_mFl2FxSrc {
  bits<6> src;
  let mFl2FxSrc_0 = {0b1};
  let mFl2FxSrc_1 = {src};
}
class mv_mFl2FxSrc_W : mv_mFl2FxSrc {
  bits<5> src;
  field bits<1> dontcare1;
  let mFl2FxSrc_0 = {0b0};
  let mFl2FxSrc_1 = {src, dontcare1};
}

let hasSideEffects = false, mayLoad = false, mayStore = false in {


// 3.2 VLD - Vector Loads
let hasSideEffects = false, mayLoad = true, mayStore = false in {
  let Itinerary = II_VLDA_W in {
  def VLDA_dmw_lda_w_ag_idx : AIE2_dmw_lda_w_inst_lda< (outs mWa:$dst) , (ins eP:$ptr, eDJ:$dj),
      "vlda", "$dst, [$ptr, $dj]">,
    AIE2_ag_all__ag_idx;
  def VLDA_dmw_lda_w_ag_idx_imm : AIE2_dmw_lda_w_inst_lda< (outs mWa:$dst) , (ins eP:$ptr, imm6x32:$imm),
      "vlda", "$dst, [$ptr, $imm]">,
    AIE2_ag_all__ag_idx_imm;
  let Uses = [SP] in
  def VLDA_dmw_lda_w_ag_spill : AIE2_dmw_lda_w_inst_lda< (outs mWa:$dst) , (ins imm12x32_neg:$imm),
      "vlda", "$dst, [sp, $imm]">,
    AIE2_ag_all__ag_spill;
  } // let Itinerary = II_VLDA_W

  let Itinerary = II_VLDA_POSTINC_W in
  let Constraints = "$ptr_out = $ptr" in {
  def VLDA_dmw_lda_w_ag_pstm_nrm : AIE2_dmw_lda_w_inst_lda< (outs mWa:$dst, eP:$ptr_out) , (ins eP:$ptr, eM:$mod),
      "vlda", "$dst, [$ptr], $mod">,
  AIE2_ag_all__ag_pstm_nrm;
  def VLDA_dmw_lda_w_ag_pstm_nrm_imm : AIE2_dmw_lda_w_inst_lda< (outs mWa:$dst, eP:$ptr_out) , (ins eP:$ptr, imm7x32:$imm),
      "vlda", "$dst, [$ptr], $imm">,
  AIE2_ag_all__ag_pstm_nrm_imm;
  }

  let Itinerary = II_VLDA_2D_W in
  let DecoderMethod = "DecodeVLDA_2D_WInstruction" in
  let Constraints = "$ptr_out = $ptr" in
  def VLDA_2D_dmw_lda_w : AIE2_dmw_lda_w_inst_lda< (outs mWa:$dst, eP:$ptr_out, eDC:$count_out) , (ins eP:$ptr, eD:$mod),
      "vlda.2d", "$dst, [$ptr], $mod">,
    AIE2_ag_all__ag_pstm_2d;

  let Itinerary = II_VLDA_3D_W in
  let DecoderMethod = "DecodeVLDA_3D_WInstruction" in
  let Constraints = "$ptr_out = $ptr" in
  def VLDA_3D_dmw_lda_w : AIE2_dmw_lda_w_inst_lda<
      (outs mWa:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
      (ins eP:$ptr, eDS:$mod),
      "vlda.3d", "$dst, [$ptr], $mod">,
    AIE2_ag_all__ag_pstm_3d;

  let Itinerary = II_VLDA_AM in {
  def VLDA_dmw_lda_am_ag_idx : AIE2_dmw_lda_am_inst_lda< (outs mAMm:$dst) , (ins eP:$ptr, eDJ:$dj),
      "vlda", "$dst, [$ptr, $dj]">,
    AIE2_ag_all__ag_idx;
  def VLDA_dmw_lda_am_ag_idx_imm : AIE2_dmw_lda_am_inst_lda< (outs mAMm:$dst) , (ins eP:$ptr, imm6x32:$imm),
      "vlda", "$dst, [$ptr, $imm]">,
    AIE2_ag_all__ag_idx_imm;
  let Uses = [SP] in
  def VLDA_dmw_lda_am_ag_spill : AIE2_dmw_lda_am_inst_lda< (outs mAMm:$dst) , (ins imm12x32_neg:$imm),
      "vlda", "$dst, [sp, $imm]">,
    AIE2_ag_all__ag_spill;
  } // let Itinerary = II_VLDA_AM

  let Itinerary = II_VLDA_POSTINC_AM in
  let Constraints = "$ptr_out = $ptr" in {
  def VLDA_dmw_lda_am_ag_pstm_nrm : AIE2_dmw_lda_am_inst_lda< (outs mAMm:$dst, eP:$ptr_out) , (ins eP:$ptr, eM:$mod),
      "vlda", "$dst, [$ptr], $mod">,
  AIE2_ag_all__ag_pstm_nrm;
  def VLDA_dmw_lda_am_ag_pstm_nrm_imm : AIE2_dmw_lda_am_inst_lda< (outs mAMm:$dst, eP:$ptr_out) , (ins eP:$ptr, imm7x32:$imm),
      "vlda", "$dst, [$ptr], $imm">,
  AIE2_ag_all__ag_pstm_nrm_imm;
  }

  let Itinerary = II_VLDA_2D_AM in
  let DecoderMethod = "DecodeVLDA_2D_AMInstruction" in
  let Constraints = "$ptr_out = $ptr" in
  def VLDA_2D_dmw_lda_am : AIE2_dmw_lda_am_inst_lda< (outs mAMm:$dst, eP:$ptr_out, eDC:$count_out) , (ins eP:$ptr, eD:$mod),
      "vlda.2d", "$dst, [$ptr], $mod">,
    AIE2_ag_all__ag_pstm_2d;

  let Itinerary = II_VLDA_3D_AM in
  let DecoderMethod = "DecodeVLDA_3D_AMInstruction" in
  let Constraints = "$ptr_out = $ptr" in
  def VLDA_3D_dmw_lda_am : AIE2_dmw_lda_am_inst_lda<
      (outs mAMm:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
      (ins eP:$ptr, eDS:$mod),
      "vlda.3d", "$dst, [$ptr], $mod">,
    AIE2_ag_all__ag_pstm_3d;

  let Itinerary = II_VLDB in {
  def VLDB_dmw_ldb_ag_idx : AIE2_dmw_ldb_inst_ldb< (outs mWa:$dst) , (ins eP:$ptr, eDJ:$dj),
      "vldb", "$dst, [$ptr, $dj]">,
    AIE2_ag_nospill__ag_idx;
  def VLDB_dmw_ldb_ag_idx_imm : AIE2_dmw_ldb_inst_ldb< (outs mWa:$dst) , (ins eP:$ptr, imm3x32:$imm),
      "vldb", "$dst, [$ptr, $imm]">,
    AIE2_ag_nospill__ag_idx_imm;
  } // let Itinerary = II_VLDB

  let Itinerary = II_VLDB_POSTINC in
  let Constraints = "$ptr_out = $ptr" in {
  def VLDB_dmw_ldb_ag_pstm_nrm : AIE2_dmw_ldb_inst_ldb< (outs mWa:$dst, eP:$ptr_out) , (ins eP:$ptr, eM:$mod),
      "vldb", "$dst, [$ptr], $mod">,
  AIE2_ag_nospill__ag_pstm_nrm;
  def VLDB_dmw_ldb_ag_pstm_nrm_imm : AIE2_dmw_ldb_inst_ldb< (outs mWa:$dst, eP:$ptr_out) , (ins eP:$ptr, imm4x32:$imm),
      "vldb", "$dst, [$ptr], $imm">,
  AIE2_ag_nospill__ag_pstm_nrm_imm;
  }

  let Itinerary = II_VLDB_2D in
  let DecoderMethod = "DecodeVLDB_2DInstruction" in
  let Constraints = "$ptr_out = $ptr" in
  def VLDB_2D : AIE2_dmw_ldb_inst_ldb< (outs mWa:$dst, eP:$ptr_out, eDC:$count_out) , (ins eP:$ptr, eD:$mod),
      "vldb.2d", "$dst, [$ptr], $mod">,
    AIE2_ag_nospill__ag_pstm_2d;

  let Itinerary = II_VLDB_3D in
  let DecoderMethod = "DecodeVLDB_3DInstruction" in
  let Constraints = "$ptr_out = $ptr" in
  def VLDB_3D : AIE2_dmw_ldb_inst_ldb<
      (outs mWa:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
      (ins eP:$ptr, eDS:$mod),
      "vldb.3d", "$dst, [$ptr], $mod">,
    AIE2_ag_nospill__ag_pstm_3d;

  let Itinerary = II_VLDA_W in
  def VLDA_128 : AIE2_dmv_lda_w_ptr_inst_lda< (outs mWa:$dst) , (ins eP:$ptr),
      "vlda.128", "$dst, [$ptr]">;

  let Itinerary = II_VLDB_PTR in {
  def VLDB_128_ag_idx : AIE2_dmv_ldb_ptr_inst_ldb< (outs mWa:$dst) , (ins eP:$ptr, eDJ:$dj),
      "vldb.128", "$dst, [$ptr, $dj]">,
    AIE2_ag_no_imm__ag_idx;
  } // let Itinerary = II_VLDB_PTR

  let Itinerary = II_VLDB_PTR_POSTINC in
  let Constraints = "$ptr_out = $ptr" in {
  def VLDB_128_ag_pstm_nrm : AIE2_dmv_ldb_ptr_inst_ldb< (outs mWa:$dst, eP:$ptr_out) , (ins eP:$ptr, eM:$mod),
      "vldb.128", "$dst, [$ptr], $mod">,
  AIE2_ag_no_imm__ag_pstm_nrm;
  }

  let Itinerary = II_VLDB_PTR_2D in
  let DecoderMethod = "DecodeVLDB_2DInstruction" in
  let Constraints = "$ptr_out = $ptr" in
  def VLDB_2D_128 : AIE2_dmv_ldb_ptr_inst_ldb< (outs mWa:$dst, eP:$ptr_out, eDC:$count_out) , (ins eP:$ptr, eD:$mod),
      "vldb.2d.128", "$dst, [$ptr], $mod">,
    AIE2_ag_no_imm__ag_pstm_2d;

  let Itinerary = II_VLDB_PTR_3D in
  let DecoderMethod = "DecodeVLDB_3DInstruction" in
  let Constraints = "$ptr_out = $ptr" in
  def VLDB_3D_128 : AIE2_dmv_ldb_ptr_inst_ldb<
      (outs mWa:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
      (ins eP:$ptr, eDS:$mod),
      "vldb.3d.128", "$dst, [$ptr], $mod">,
    AIE2_ag_no_imm__ag_pstm_3d;

} // hasSideEffects = false, mayLoad = true, mayStore = false

// 3.3 VLD.UNPACK – Vector Loads with unpacking

// VLDB.UNPACK uses mXs register class which doesn't have a power-of-2 number of registers.
// PADDB uses an immediate value seperated with a fixed {0b11} field. This field coincides with
// mXs register field in VLDB.UNPACK. Because of this, it could happen that we try to decode a
// PADDB instruction as VLDB.UNPACK assuming that the latter has a complete decoder.
// To avoid this, we set hasCompleteDecoder to false in VLDB.UNPACK instructions.
// Setting hasCompleteDecoder to false in the mXs register class is not enough because
// VLDB.2D.UNPACK and VLDB.3D.UNPACK instructions have custom decoder methods
let hasSideEffects = false, mayLoad = true, mayStore = false, hasCompleteDecoder= false in {
  let Itinerary = II_VLDB_UNPACK in {
    class VLDB_ag_idx_dmw_ldb_unpack<bits<1> sgn, bits<1> sz, string opcodestr> :
        AIE2_dmw_ldb_unpack_inst_ldb<sgn, sz, (outs mXs:$dst), (ins eP:$ptr, eDJ:$dj), opcodestr, "$dst, [$ptr, $dj]">;
    def VLDB_UNPACK_S8_S4_ag_idx : VLDB_ag_idx_dmw_ldb_unpack</*sgn*/1, /*sz*/0, "vldb.unpack.s8.s4">, 
      AIE2_ag_no_imm__ag_idx;
    def VLDB_UNPACK_S16_S8_ag_idx : VLDB_ag_idx_dmw_ldb_unpack</*sgn*/1, /*sz*/1, "vldb.unpack.s16.s8">,
      AIE2_ag_no_imm__ag_idx;
    let Uses = [crUnpackSign] in {
      def VLDB_UNPACK_D8_D4_ag_idx : VLDB_ag_idx_dmw_ldb_unpack</*sgn*/0, /*sz*/0, "vldb.unpack.d8.d4">,
        AIE2_ag_no_imm__ag_idx;
      def VLDB_UNPACK_D16_D8_ag_idx : VLDB_ag_idx_dmw_ldb_unpack</*sgn*/0, /*sz*/1, "vldb.unpack.d16.d8">,
        AIE2_ag_no_imm__ag_idx;
    }
  }
  let Itinerary = II_VLDB_UNPACK_POSTINC, Constraints = "$ptr_out = $ptr" in {
    class VLDB_ag_pstm_nrm_dmw_ldb_unpack<bits<1> sgn, bits<1> sz, string opcodestr> :
        AIE2_dmw_ldb_unpack_inst_ldb<sgn, sz, (outs mXs:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), opcodestr, "$dst, [$ptr], $mod">;
    def VLDB_UNPACK_S8_S4_ag_pstm_nrm : VLDB_ag_pstm_nrm_dmw_ldb_unpack</*sgn*/1, /*sz*/0, "vldb.unpack.s8.s4">, 
      AIE2_ag_no_imm__ag_pstm_nrm;
    def VLDB_UNPACK_S16_S8_ag_pstm_nrm : VLDB_ag_pstm_nrm_dmw_ldb_unpack</*sgn*/1, /*sz*/1, "vldb.unpack.s16.s8">,
      AIE2_ag_no_imm__ag_pstm_nrm;
    let Uses = [crUnpackSign] in {
      def VLDB_UNPACK_D8_D4_pstm_nrm : VLDB_ag_pstm_nrm_dmw_ldb_unpack</*sgn*/0, /*sz*/0, "vldb.unpack.d8.d4">,
        AIE2_ag_no_imm__ag_pstm_nrm;
      def VLDB_UNPACK_D16_D8_ag_pstm_nrm : VLDB_ag_pstm_nrm_dmw_ldb_unpack</*sgn*/0, /*sz*/1, "vldb.unpack.d16.d8">,
        AIE2_ag_no_imm__ag_pstm_nrm;
    }
  }
  let Itinerary = II_VLDB_UNPACK_2D, Constraints = "$ptr_out = $ptr" in
    let DecoderMethod = "DecodeVLDB_UNPACK_2DInstruction" in {
      class VLDB_2D_dmw_ldb_unpack<bits<1> sgn, bits<1> sz, string opcodestr> :
          AIE2_dmw_ldb_unpack_inst_ldb<sgn, sz, (outs mXs:$dst, eP:$ptr_out, eDC:$count_out), (ins eP:$ptr, eD:$mod), opcodestr, "$dst, [$ptr], $mod">;
      def VLDB_2D_UNPACK_S8_S4 : VLDB_2D_dmw_ldb_unpack</*sgn*/1, /*sz*/0, "vldb.2d.unpack.s8.s4">, 
        AIE2_ag_no_imm__ag_pstm_2d;
      def VLDB_2D_UNPACK_S16_S8 : VLDB_2D_dmw_ldb_unpack</*sgn*/1, /*sz*/1, "vldb.2d.unpack.s16.s8">,
        AIE2_ag_no_imm__ag_pstm_2d;
      let Uses = [crUnpackSign] in {
        def VLDB_2D_UNPACK_D8_D4 : VLDB_2D_dmw_ldb_unpack</*sgn*/0, /*sz*/0, "vldb.2d.unpack.d8.d4">,
          AIE2_ag_no_imm__ag_pstm_2d;
        def VLDB_2D_UNPACK_D16_D8 : VLDB_2D_dmw_ldb_unpack</*sgn*/0, /*sz*/1, "vldb.2d.unpack.d16.d8">,
          AIE2_ag_no_imm__ag_pstm_2d;
      }
    }
  let Itinerary = II_VLDB_UNPACK_3D, Constraints = "$ptr_out = $ptr" in
    let DecoderMethod = "DecodeVLDB_UNPACK_3DInstruction" in {
      class VLDB_3D_dmw_ldb_unpack<bits<1> sgn, bits<1> sz, string opcodestr> :
          AIE2_dmw_ldb_unpack_inst_ldb<sgn, sz, (outs mXs:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out), (ins eP:$ptr, eDS:$mod), opcodestr, "$dst, [$ptr], $mod">;
      def VLDB_3D_UNPACK_S8_S4 : VLDB_3D_dmw_ldb_unpack</*sgn*/1, /*sz*/0, "vldb.3d.unpack.s8.s4">, 
        AIE2_ag_no_imm__ag_pstm_3d;
      def VLDB_3D_UNPACK_S16_S8 : VLDB_3D_dmw_ldb_unpack</*sgn*/1, /*sz*/1, "vldb.3d.unpack.s16.s8">,
        AIE2_ag_no_imm__ag_pstm_3d;
      let Uses = [crUnpackSign] in {
        def VLDB_3D_UNPACK_D8_D4 : VLDB_3D_dmw_ldb_unpack</*sgn*/0, /*sz*/0, "vldb.3d.unpack.d8.d4">,
          AIE2_ag_no_imm__ag_pstm_3d;
        def VLDB_3D_UNPACK_D16_D8 : VLDB_3D_dmw_ldb_unpack</*sgn*/0, /*sz*/1, "vldb.3d.unpack.d16.d8">,
          AIE2_ag_no_imm__ag_pstm_3d;
      }
    }

} // hasSideEffects = false, mayLoad = true, mayStore = false, hasCompleteDecoder= false

// 3.10 VST - Vector Stores
let hasSideEffects = false, mayLoad = false, mayStore = true in {
  let Itinerary = II_VST_W in {
  def VST_dmw_sts_w_ag_idx : AIE2_dmw_sts_w_inst_st< (outs ) , (ins mWs:$src, eP:$ptr, eDJ:$dj),
      "vst", "$src, [$ptr, $dj]">,
    AIE2_ag_all__ag_idx;
  def VST_dmw_sts_w_ag_idx_imm : AIE2_dmw_sts_w_inst_st< (outs ) , (ins mWs:$src, eP:$ptr, imm6x32:$imm),
      "vst", "$src, [$ptr, $imm]">,
    AIE2_ag_all__ag_idx_imm;
  let Uses = [SP] in
  def VST_dmw_sts_w_ag_spill : AIE2_dmw_sts_w_inst_st< (outs ) , (ins mWs:$src, imm12x32_neg:$imm),
      "vst", "$src, [sp, $imm]">,
    AIE2_ag_all__ag_spill;
  } // let Itinerary = II_VST_W

  let Itinerary = II_VST_POSTINC_W in
  let Constraints = "$ptr_out = $ptr" in {
  def VST_dmw_sts_w_ag_pstm_nrm : AIE2_dmw_sts_w_inst_st< (outs eP:$ptr_out) , (ins mWs:$src, eP:$ptr, eM:$mod),
      "vst", "$src, [$ptr], $mod">,
    AIE2_ag_all__ag_pstm_nrm;
  def VST_dmw_sts_w_ag_pstm_nrm_imm : AIE2_dmw_sts_w_inst_st< (outs eP:$ptr_out) , (ins mWs:$src, eP:$ptr, imm7x32:$imm),
      "vst", "$src, [$ptr], $imm">,
    AIE2_ag_all__ag_pstm_nrm_imm;
  }

  let Itinerary = II_VST_2D_W in let Constraints = "$ptr_out = $ptr" in
  let DecoderMethod = "DecodeVST_W_2DInstruction" in
  def VST_2D_dmw_sts_w : AIE2_dmw_sts_w_inst_st< (outs eP:$ptr_out, eDC:$count_out) , (ins mWs:$src, eP:$ptr, eD:$mod),
      "vst.2d", "$src, [$ptr], $mod">,
    AIE2_ag_all__ag_pstm_2d;

  let Itinerary = II_VST_3D_W in let Constraints = "$ptr_out = $ptr" in
  let DecoderMethod = "DecodeVST_3D_WInstruction" in
  def VST_3D_dmw_sts_w : AIE2_dmw_sts_w_inst_st<
      (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
      (ins mWs:$src, eP:$ptr, eDS:$mod),
      "vst.3d", "$src, [$ptr], $mod">,
    AIE2_ag_all__ag_pstm_3d;

  let Itinerary = II_VST_AM in {
  def VST_dmw_sts_am_ag_idx : AIE2_dmw_sts_am_inst_st< (outs ) , (ins mAMs:$src, eP:$ptr, eDJ:$dj),
      "vst", "$src, [$ptr, $dj]">,
    AIE2_ag_all__ag_idx;
  def VST_dmw_sts_am_ag_idx_imm : AIE2_dmw_sts_am_inst_st< (outs ) , (ins mAMs:$src, eP:$ptr, imm6x32:$imm),
      "vst", "$src, [$ptr, $imm]">,
    AIE2_ag_all__ag_idx_imm;
  let Uses = [SP] in
  def VST_dmw_sts_am_ag_spill : AIE2_dmw_sts_am_inst_st< (outs ) , (ins mAMs:$src, imm12x32_neg:$imm),
      "vst", "$src, [sp, $imm]">,
    AIE2_ag_all__ag_spill;
  } // let Itinerary = II_VST_AM

  let Itinerary = II_VST_POSTINC_AM in
  let Constraints = "$ptr_out = $ptr" in {
  def VST_dmw_sts_am_ag_pstm_nrm : AIE2_dmw_sts_am_inst_st< (outs eP:$ptr_out) , (ins mAMs:$src, eP:$ptr, eM:$mod),
      "vst", "$src, [$ptr], $mod">,
    AIE2_ag_all__ag_pstm_nrm;
  def VST_dmw_sts_am_ag_pstm_nrm_imm : AIE2_dmw_sts_am_inst_st< (outs eP:$ptr_out) , (ins mAMs:$src, eP:$ptr, imm7x32:$imm),
      "vst", "$src, [$ptr], $imm">,
    AIE2_ag_all__ag_pstm_nrm_imm;
  }

  let Itinerary = II_VST_2D_AM in let Constraints = "$ptr_out = $ptr" in
  let DecoderMethod = "DecodeVST_AM_2DInstruction" in
  def VST_2D_dmw_sts_am : AIE2_dmw_sts_am_inst_st< (outs eP:$ptr_out, eDC:$count_out) , (ins mAMs:$src, eP:$ptr, eD:$mod),
      "vst.2d", "$src, [$ptr], $mod">,
    AIE2_ag_all__ag_pstm_2d;

  let Itinerary = II_VST_3D_AM in let Constraints = "$ptr_out = $ptr" in
  let DecoderMethod = "DecodeVST_3D_AMInstruction" in
  def VST_3D_dmw_sts_am : AIE2_dmw_sts_am_inst_st<
      (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
      (ins mAMs:$src, eP:$ptr, eDS:$mod),
      "vst.3d", "$src, [$ptr], $mod">,
    AIE2_ag_all__ag_pstm_3d;

  // Note: careful here! The immediate has a 32-byte step (256 bits), yet the
  // 128-bits vectors are 128-bits aligned in memory.
  let Itinerary = II_VST_W in {
    def VST_128_ag_idx_imm : AIE2_dmv_sts_w_inst_st< (outs ) , (ins mWs:$src, eP:$ptr, imm6x32:$imm),
        "vst.128", "$src, [$ptr, $imm]">,
      AIE2_ag_all__ag_idx_imm;
    def VST_128_ag_idx : AIE2_dmv_sts_w_inst_st< (outs ) , (ins mWs:$src, eP:$ptr, eDJ:$dj),
        "vst.128", "$src, [$ptr, $dj]">,
      AIE2_ag_all__ag_idx;
    def VST_128_ag_spill : AIE2_dmv_sts_w_inst_st< (outs ) , (ins mWs:$src, imm12x32_neg:$imm),
        "vst.128", "$src, [sp, $imm]">,
      AIE2_ag_all__ag_spill;
  }
  let Constraints = "$ptr_out = $ptr" in {
    let Itinerary = II_VST_POSTINC_W in {
      def VST_128_ag_pstm_nrm : AIE2_dmv_sts_w_inst_st< (outs eP:$ptr_out) ,
          (ins mWs:$src, eP:$ptr, eM:$mod),
          "vst.128", "$src, [$ptr], $mod">,
        AIE2_ag_all__ag_pstm_nrm;
      def VST_128_ag_pstm_nrm_imm : AIE2_dmv_sts_w_inst_st< (outs eP:$ptr_out) ,
          (ins mWs:$src, eP:$ptr, imm7x32:$imm),
          "vst.128", "$src, [$ptr], $imm">,
        AIE2_ag_all__ag_pstm_nrm_imm;
    }
    let Itinerary = II_VST_2D_W, DecoderMethod = "DecodeVST_W_2DInstruction" in
        def VST_2D_128 : AIE2_dmv_sts_w_inst_st< (outs eP:$ptr_out, eDC:$count_out) ,
            (ins mWs:$src, eP:$ptr, eD:$mod),
            "vst.2d.128", "$src, [$ptr], $mod">,
          AIE2_ag_all__ag_pstm_2d;

    let Itinerary = II_VST_3D_W, DecoderMethod = "DecodeVST_3D_WInstruction" in
        def VST_3D_128 : AIE2_dmv_sts_w_inst_st< (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out) ,
            (ins mWs:$src, eP:$ptr, eDS:$mod),
            "vst.3d.128", "$src, [$ptr], $mod">,
          AIE2_ag_all__ag_pstm_3d;
    }

// 3.18 ST.TM - Scalar Store to Tile Memory Map
  let Itinerary = II_ST_TM in
  def ST_TM : AIE2_tm_sts_inst_st< (outs) , (ins OP_mSclSt:$mSclSt, eP:$ptr),
      "st.tm", "$mSclSt, [$ptr]">;

//3.19 ST – Mask Store
  let Itinerary = II_ST_Q in {
  def ST_dmv_sts_q_ag_idx : AIE2_dmv_sts_q_inst_st< (outs ) , (ins mQQa:$src, eP:$ptr, eDJ:$dj),
      "st", "$src, [$ptr, $dj]">,
    AIE2_ag_all__ag_idx;
  def ST_dmv_sts_q_ag_idx_imm : AIE2_dmv_sts_q_inst_st< (outs ) , (ins mQQa:$src, eP:$ptr, imm6x16:$imm),
      "st", "$src, [$ptr, $imm]">,
    AIE2_ag_all__ag_idx_imm;
  let Uses = [SP] in
  def ST_dmv_sts_q_ag_spill : AIE2_dmv_sts_q_inst_st< (outs ) , (ins mQQa:$src, imm12x16_neg:$imm),
      "st", "$src, [sp, $imm]">,
    AIE2_ag_all__ag_spill;
  } // let Itinerary = II_ST_Q

  let Itinerary = II_ST_POSTINC_Q in
  let Constraints = "$ptr_out = $ptr" in {
  def ST_dmv_sts_q_ag_pstm_nrm : AIE2_dmv_sts_q_inst_st< (outs eP:$ptr_out) , (ins mQQa:$src, eP:$ptr, eM:$mod),
      "st", "$src, [$ptr], $mod">,
    AIE2_ag_all__ag_pstm_nrm;
  def ST_dmv_sts_q_ag_pstm_nrm_imm : AIE2_dmv_sts_q_inst_st< (outs eP:$ptr_out) , (ins mQQa:$src, eP:$ptr, imm7x16:$imm),
      "st", "$src, [$ptr], $imm">,
    AIE2_ag_all__ag_pstm_nrm_imm;
  }
  let Itinerary = II_ST_2D_Q in let Constraints = "$ptr_out = $ptr" in
  let DecoderMethod = "DecodeST_Q_2DInstruction" in
  def ST_2D_dmv_sts_q : AIE2_dmv_sts_q_inst_st< (outs eP:$ptr_out, eDC:$count_out) , (ins mQQa:$src, eP:$ptr, eD:$mod),
      "st.2d", "$src, [$ptr], $mod">,
    AIE2_ag_all__ag_pstm_2d;

  let Itinerary = II_ST_3D_Q in let Constraints = "$ptr_out = $ptr" in
  let DecoderMethod = "DecodeST_Q_3DInstruction" in
  def ST_3D_dmv_sts_q : AIE2_dmv_sts_q_inst_st<
      (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
      (ins mQQa:$src, eP:$ptr, eDS:$mod),
      "st.3d", "$src, [$ptr], $mod">,
    AIE2_ag_all__ag_pstm_3d;

} // let hasSideEffects = false, mayLoad = false, mayStore = true

// 3.21 LD.TM - Scalar Load from Tile Memory Map
let hasSideEffects = false, mayLoad = true, mayStore = false in {
   let Itinerary = II_LDA_TM in
   def LDA_TM : AIE2_tm_lda_inst_lda< (outs eR:$dst) , (ins eP:$ptr),
      "lda.tm", "$dst, [$ptr]">;
} // let hasSideEffects = false, mayLoad = true, mayStore = false

// 3.22 LD – Mask Load
let hasSideEffects = false, mayLoad = true, mayStore = false in {
  let Itinerary = II_LDA_Q in {
  def LDA_dmv_lda_q_ag_idx : AIE2_dmv_lda_q_inst_lda< (outs mQQa:$dst) , (ins eP:$ptr, eDJ:$dj),
      "lda", "$dst, [$ptr, $dj]">,
    AIE2_ag_all__ag_idx;
  def LDA_dmv_lda_q_ag_idx_imm : AIE2_dmv_lda_q_inst_lda< (outs mQQa:$dst) , (ins eP:$ptr, imm6x16:$imm),
      "lda", "$dst, [$ptr, $imm]">,
    AIE2_ag_all__ag_idx_imm;
  let Uses = [SP] in
  def LDA_dmv_lda_q_ag_spill : AIE2_dmv_lda_q_inst_lda< (outs mQQa:$dst) , (ins imm12x16_neg:$imm),
      "lda", "$dst, [sp, $imm]">,
    AIE2_ag_all__ag_spill;
  } // let Itinerary = II_LDA_Q

  let Itinerary = II_LDA_POSTINC_Q in
  let Constraints = "$ptr_out = $ptr" in {
  def LDA_dmv_lda_q_ag_pstm_nrm : AIE2_dmv_lda_q_inst_lda< (outs mQQa:$dst, eP:$ptr_out) , (ins eP:$ptr, eM:$mod),
      "lda", "$dst, [$ptr], $mod">,
  AIE2_ag_all__ag_pstm_nrm;
  def LDA_dmv_lda_q_ag_pstm_nrm_imm : AIE2_dmv_lda_q_inst_lda< (outs mQQa:$dst, eP:$ptr_out) , (ins eP:$ptr, imm7x16:$imm),
      "lda", "$dst, [$ptr], $imm">,
  AIE2_ag_all__ag_pstm_nrm_imm;
  } // let Itinerary = II_LDA_POSTINC_Q
  let Itinerary = II_LDA_2D_Q in
    let DecoderMethod = "DecodeLDA_Q_2DInstruction" in
    def LDA_2D_dmv_lda_q : AIE2_dmv_lda_q_inst_lda< (outs mQQa:$dst, eP:$ptr_out, eDC:$count_out) , (ins eP:$ptr, eD:$mod),
      "lda.2d", "$dst, [$ptr], $mod">,
    AIE2_ag_all__ag_pstm_2d;
  let Itinerary = II_LDA_3D_Q in
    let DecoderMethod = "DecodeLDA_Q_3DInstruction" in
    def LDA_3D_dmv_lda_q : AIE2_dmv_lda_q_inst_lda< (outs mQQa:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out) , (ins eP:$ptr, eDS:$mod),
      "lda.3d", "$dst, [$ptr], $mod">,
    AIE2_ag_all__ag_pstm_3d;
} // let hasSideEffects = false, mayLoad = true, mayStore = false

//5.3 MOV – Move scalar register (delayed)
let Itinerary = II_MOVd1 in {
  def MOV_D1 : AIE2_mv_delay_inst_mv< /*delay*/0b000, (outs eR:$dst), (ins OP_mMvSclSrc:$src),
      "mov.d1", "$dst, $src">;
}
let Itinerary = II_MOVd2 in {
  def MOV_D2 : AIE2_mv_delay_inst_mv< /*delay*/0b001, (outs eR:$dst), (ins OP_mMvSclSrc:$src),
      "mov.d2", "$dst, $src">;
}
let Itinerary = II_MOVd3 in {
  def MOV_D3 : AIE2_mv_delay_inst_mv< /*delay*/0b010, (outs eR:$dst), (ins OP_mMvSclSrc:$src),
      "mov.d3", "$dst, $src">;
}
let Itinerary = II_MOVd4 in {
  def MOV_D4 : AIE2_mv_delay_inst_mv< /*delay*/0b011, (outs eR:$dst), (ins OP_mMvSclSrc:$src),
      "mov.d4", "$dst, $src">;
}
let Itinerary = II_MOVd5 in {
  def MOV_D5 : AIE2_mv_delay_inst_mv< /*delay*/0b100, (outs eR:$dst), (ins OP_mMvSclSrc:$src),
      "mov.d5", "$dst, $src">;
}
let Itinerary = II_MOVd6 in {
  def MOV_D6 : AIE2_mv_delay_inst_mv< /*delay*/0b101, (outs eR:$dst), (ins OP_mMvSclSrc:$src),
      "mov.d6", "$dst, $src">;
}

// 5.5 MOV – Move tile counter
let Itinerary = II_MOV_CNTR in
  let Uses = [tile_cntr] in {
  def MOV_CNTR : AIE2_mv_cntr2c_inst_mv< (outs eL:$dst), (ins ), "mov", "$dst, cntr">;
}
// 5.6 VMOV - Move vector/accumulator register
let Itinerary = II_VMOV_W in {
  let ItineraryRegPairs = [ItinRegClassPair<II_VMOV_W_CM_CM,[OperandRegClass<0, mAMm>, OperandRegClass<1, mAMm>]>,
                           ItinRegClassPair<II_VMOV_W_CM_WMH,[OperandRegClass<0, mAMm>, OperandRegClass<1, eWH>]>,
                           ItinRegClassPair<II_VMOV_W_WMH_CM,[OperandRegClass<0, eWH>, OperandRegClass<1, mAMm>]>,
                           ItinRegClassPair<II_VMOV_W_CM_WML,[OperandRegClass<0, mAMm>, OperandRegClass<1, eWL>]>,
                           ItinRegClassPair<II_VMOV_W_WML_CM,[OperandRegClass<0, eWL>, OperandRegClass<1, mAMm>]>,
                           ItinRegClassPair<II_VMOV_W_WMH_WMH,[OperandRegClass<0, eWH>, OperandRegClass<1, eWH>]>,
                           ItinRegClassPair<II_VMOV_W_WML_WML,[OperandRegClass<0, eWL>, OperandRegClass<1, eWL>]>,
                           ItinRegClassPair<II_VMOV_W_WMH_WML,[OperandRegClass<0, eWH>, OperandRegClass<1, eWL>]>,						   
                           ItinRegClassPair<II_VMOV_W_WML_WMH,[OperandRegClass<0, eWL>, OperandRegClass<1, eWH>]>,
                           ItinRegClassPair<II_VMOV_W_WML_Q, [OperandRegClass<0, eWL>, OperandRegClass<1, mQQm>]>,
                           ItinRegClassPair<II_VMOV_W_WMH_Q, [OperandRegClass<0, eWH>, OperandRegClass<1, mQQm>]>] in {	
	def VMOV_mv_w : AIE2_mv_w_inst_mv< (outs OP_mMvAMWQDst:$dst), (ins OP_mMvAMWQSrc:$src),
		"vmov", "$dst, $src">;
  }
}
let Itinerary = II_VMOV_X in {
  let ItineraryRegPairs = [ItinRegClassPair<II_VMOV_X_BM_BM,[OperandRegClass<0, mBMm>, OperandRegClass<1, mBMm>]>,
                          ItinRegClassPair<II_VMOV_X_XM_BM,[OperandRegClass<0, mXm>, OperandRegClass<1, mBMm>]>,
                          ItinRegClassPair<II_VMOV_X_BM_XM,[OperandRegClass<0, mBMm>, OperandRegClass<1, mXm>]>,
                          ItinRegClassPair<II_VMOV_X_XM_XM,[OperandRegClass<0, mXm>, OperandRegClass<1, mXm>]>] in {
  def VMOV_mv_x : AIE2_mv_x_inst_mv< (outs OP_mMvBMXDst:$dst), (ins OP_mMvBMXSrc:$src),
      "vmov", "$dst, $src">;
  }
}

let Itinerary = II_VMOV_CM in {
  def VMOV_mv_cm : AIE2_mv_cm_inst_mv< (outs mCMm:$dst), (ins mCMm:$src),
      "vmov", "$dst, $src">;
}

// 5.7 VMOV.D – Move accumulator register delayed
let Itinerary = II_VMOV_D in {
  def VMOV_D : AIE2_vmov_inst_vec< (outs mCMa:$cdst), (ins mCMa:$csrc),
      "vmov.d", "$cdst, $csrc">;
}

// 6.3 VMOV - Cascade stream access
let hasSideEffects = true, Itinerary = II_VMOV_CASCADE_READ, Uses = [crSCDEn] in {
  def VMOV_mv_scd : AIE2_mv_scd_inst_lda< (outs OP_mScdDst:$dst), (ins),
      "vmov", "$dst, SCD">;  
  def VMOV_HI : AIE2_mv_scdl_inst_lda<0b1, (outs mCMs:$dst), (ins),
      "vmov.hi", "$dst, SCD">;
  def VMOV_LO : AIE2_mv_scdl_inst_lda<0b0, (outs mCMs:$dst), (ins),
      "vmov.lo", "$dst, SCD">;
}
let hasSideEffects = true, Itinerary = II_VMOV_CASCADE_WRITE, Uses = [crMCDEn] in {
  def VMOV_mv_mcd : AIE2_mv_mcd_inst_st < (outs), (ins OP_mMcdSrc:$src),
      "vmov", "MCD, $src">;
}


// 7.3 VSRS - Shift-Round-Saturate instructions
let Itinerary = II_VSRS in
let Defs = [srSRS_of] in {
  let Uses = [crSat, crRnd, crSRSSign] in
  def VSRS_D8_S32_mv_w_srs :
      AIE2_mv_w_srs_inst_st<(outs mWa:$dst), (ins mCMs:$src, mSs:$shft),
                            "vsrs.d8.s32", "$dst, $src, $shft">,
      mWdssrs</*md*/0, /*sgn*/0>;
  let Uses = [crSat, crRnd] in
  def VSRS_S8_S32_mv_w_srs :
      AIE2_mv_w_srs_inst_st<(outs mWa:$dst), (ins mCMs:$src, mSs:$shft),
                            "vsrs.s8.s32", "$dst, $src, $shft">,
      mWdssrs</*md*/0, /*sgn*/1>;
  let Uses = [crSat, crRnd, crSRSSign] in
  def VSRS_D16_S64_mv_w_srs :
      AIE2_mv_w_srs_inst_st<(outs mWa:$dst), (ins mCMs:$src, mSs:$shft),
                            "vsrs.d16.s64", "$dst, $src, $shft">,
      mWdssrs</*md*/1, /*sgn*/0>;
  let Uses = [crSat, crRnd] in
  def VSRS_S16_S64_mv_w_srs :
      AIE2_mv_w_srs_inst_st<(outs mWa:$dst), (ins mCMs:$src, mSs:$shft),
                            "vsrs.s16.s64", "$dst, $src, $shft">,
      mWdssrs</*md*/1, /*sgn*/1>;

  let Uses = [crSat, crRnd, crSRSSign] in
  def VSRS_D16_S32_mv_w_srs :
      AIE2_mv_w_srs_inst_st<(outs mWa:$dst), (ins mBMs:$src, mSs:$shft),
                            "vsrs.d16.s32", "$dst, $src, $shft">,
      mWlsrsl</*md*/0, /*sgn*/0>;
  let Uses = [crSat, crRnd] in
  def VSRS_S16_S32_mv_w_srs :
      AIE2_mv_w_srs_inst_st<(outs mWa:$dst), (ins mBMs:$src, mSs:$shft),
                            "vsrs.s16.s32", "$dst, $src, $shft">,
      mWlsrsl</*md*/0, /*sgn*/1>;
  let Uses = [crSat, crRnd, crSRSSign] in
  def VSRS_D32_S64_mv_w_srs :
      AIE2_mv_w_srs_inst_st<(outs mWa:$dst), (ins mBMs:$src, mSs:$shft),
                            "vsrs.d32.s64", "$dst, $src, $shft">,
      mWlsrsl</*md*/1, /*sgn*/0>;
  let Uses = [crSat, crRnd] in
  def VSRS_S32_S64_mv_w_srs :
      AIE2_mv_w_srs_inst_st<(outs mWa:$dst), (ins mBMs:$src, mSs:$shft),
                            "vsrs.s32.s64", "$dst, $src, $shft">,
      mWlsrsl</*md*/1, /*sgn*/1>;

  let Uses = [crSat, crRnd, crSRSSign] in
  def VSRS_D16_S32_mv_x_srs :
      AIE2_mv_x_srs_inst_st<(outs mXa:$dst), (ins mCMs:$src, mSs:$shft),
                            "vsrs.d16.s32", "$dst, $src, $shft">,
      mXdlsrs</*md*/0, /*sgn*/0>;
  let Uses = [crSat, crRnd] in
  def VSRS_S16_S32_mv_x_srs :
      AIE2_mv_x_srs_inst_st<(outs mXa:$dst), (ins mCMs:$src, mSs:$shft),
                            "vsrs.s16.s32", "$dst, $src, $shft">,
      mXdlsrs</*md*/0, /*sgn*/1>;
  let Uses = [crSat, crRnd, crSRSSign] in
  def VSRS_D32_S64_mv_x_srs :
      AIE2_mv_x_srs_inst_st<(outs mXa:$dst), (ins mCMs:$src, mSs:$shft),
                            "vsrs.d32.s64", "$dst, $src, $shft">,
      mXdlsrs</*md*/1, /*sgn*/0>;
  let Uses = [crSat, crRnd] in
  def VSRS_S32_S64_mv_x_srs :
      AIE2_mv_x_srs_inst_st<(outs mXa:$dst), (ins mCMs:$src, mSs:$shft),
                            "vsrs.s32.s64", "$dst, $src, $shft">,
      mXdlsrs</*md*/1, /*sgn*/1>;
}

let Itinerary = II_VSRSM in
let Defs = [srSRS_of] in {
  let Uses = [crSat, crRnd, crSRSSign] in {
    def VSRSM_D16_S32 :
      AIE2_mv_w_srs1_inst_mv<(outs mWm:$dst), (ins mBMm:$src, mSm:$shft),
                            "vsrsm.d16.s32", "$dst, $src, $shft">,
      mWlsrsh</*md*/0, /*sgn*/0>;
    def VSRSM_D32_S64 :
      AIE2_mv_w_srs1_inst_mv<(outs mWm:$dst), (ins mBMm:$src, mSm:$shft),
                            "vsrsm.d32.s64", "$dst, $src, $shft">,
      mWlsrsh</*md*/1, /*sgn*/0>;
  }
  let Uses = [crSat, crRnd] in {
    def VSRSM_S16_S32 :
      AIE2_mv_w_srs1_inst_mv<(outs mWm:$dst), (ins mBMm:$src, mSm:$shft),
                            "vsrsm.s16.s32", "$dst, $src, $shft">,
      mWlsrsh</*md*/0, /*sgn*/1>;
    def VSRSM_S32_S64 :
      AIE2_mv_w_srs1_inst_mv<(outs mWm:$dst), (ins mBMm:$src, mSm:$shft),
                            "vsrsm.s32.s64", "$dst, $src, $shft">,
      mWlsrsh</*md*/1, /*sgn*/1>;
  }
}
// 7.4 VUPS - UPS instructions
// Dynamic UPS instructions read the crUPSSign control register to determine the sign of the source operand.
let Itinerary = II_VUPS in
let Defs = [srUPS_of] in {
   let Uses = [crSat, crUPSSign] in
  def VUPS_S32_D8_mv_ups_w2c:
      AIE2_mv_ups_w2c_inst_mv<(outs mCMs:$dst), (ins OP_mWm_1:$src, mSs:$shft),
                            "vups.s32.d8", "$dst, $src, $shft">,
      mv_ups_base</*md*/0, /*sgn*/0>;
  let Uses = [crSat] in
  def VUPS_S32_S8_mv_ups_w2c:
      AIE2_mv_ups_w2c_inst_mv<(outs mCMs:$dst), (ins OP_mWm_1:$src, mSs:$shft),
                            "vups.s32.s8", "$dst, $src, $shft">,
      mv_ups_base</*md*/0, /*sgn*/1>;
  let Uses = [crSat, crUPSSign] in
  def VUPS_S64_D16_mv_ups_w2c:
      AIE2_mv_ups_w2c_inst_mv<(outs mCMs:$dst), (ins OP_mWm_1:$src, mSs:$shft),
                            "vups.s64.d16", "$dst, $src, $shft">,
      mv_ups_base</*md*/1, /*sgn*/0>;
  let Uses = [crSat] in
  def VUPS_S64_S16_mv_ups_w2c:
      AIE2_mv_ups_w2c_inst_mv<(outs mCMs:$dst), (ins OP_mWm_1:$src, mSs:$shft),
                            "vups.s64.s16", "$dst, $src, $shft">,
      mv_ups_base</*md*/1, /*sgn*/1>;
  let Uses = [crSat, crUPSSign] in
  def VUPS_S32_D16_mv_ups_w2b:
      AIE2_mv_ups_w2b_inst_mv<(outs mBMs:$dst), (ins OP_mWm_1:$src, mSs:$shft),
                            "vups.s32.d16", "$dst, $src, $shft">,
      mv_ups_base</*md*/0, /*sgn*/0>;
  let Uses = [crSat] in
  def VUPS_S32_S16_mv_ups_w2b:
      AIE2_mv_ups_w2b_inst_mv<(outs mBMs:$dst), (ins OP_mWm_1:$src, mSs:$shft),
                            "vups.s32.s16", "$dst, $src, $shft">,
      mv_ups_base</*md*/0, /*sgn*/1>;
  let Uses = [crSat, crUPSSign] in
  def VUPS_S64_D32_mv_ups_w2b:
      AIE2_mv_ups_w2b_inst_mv<(outs mBMs:$dst), (ins OP_mWm_1:$src, mSs:$shft),
                            "vups.s64.d32", "$dst, $src, $shft">,
      mv_ups_base</*md*/1, /*sgn*/0>;
  let Uses = [crSat] in
  def VUPS_S64_S32_mv_ups_w2b:
      AIE2_mv_ups_w2b_inst_mv<(outs mBMs:$dst), (ins OP_mWm_1:$src, mSs:$shft),
                            "vups.s64.s32", "$dst, $src, $shft">,
      mv_ups_base</*md*/1, /*sgn*/1>;
  let Uses = [crSat, crUPSSign] in
  def VUPS_S32_D16_mv_ups_x2c:
      AIE2_mv_ups_x2c_inst_mv<(outs mCMs:$dst), (ins mXa:$src, mSs:$shft),
                            "vups.s32.d16", "$dst, $src, $shft">,
      mv_ups_base</*md*/0, /*sgn*/0>;
  let Uses = [crSat] in
  def VUPS_S32_S16_mv_ups_x2c:
      AIE2_mv_ups_x2c_inst_mv<(outs mCMs:$dst), (ins mXa:$src, mSs:$shft),
                            "vups.s32.s16", "$dst, $src, $shft">,
      mv_ups_base</*md*/0, /*sgn*/1>;
  let Uses = [crSat, crUPSSign] in
  def VUPS_S64_D32_mv_ups_x2c:
      AIE2_mv_ups_x2c_inst_mv<(outs mCMs:$dst), (ins mXa:$src, mSs:$shft),
                            "vups.s64.d32", "$dst, $src, $shft">,
      mv_ups_base</*md*/1, /*sgn*/0>;
  let Uses = [crSat] in
  def VUPS_S64_S32_mv_ups_x2c:
      AIE2_mv_ups_x2c_inst_mv<(outs mCMs:$dst), (ins mXa:$src, mSs:$shft),
                            "vups.s64.s32", "$dst, $src, $shft">,
      mv_ups_base</*md*/1, /*sgn*/1>;
}

// 7.5 vconv.fp32.bf16 – bfloat16 to float Conversion instructions
let Itinerary = II_VCONVfp32bf16, ItineraryRegPairs = [ItinRegClassPair<II_VCONVfp32bf16_WL,[OperandRegClass<1, eWL>]>] in {
  def VCONV_FP32_BF16 :
      AIE2_mv_ups_bf_inst_mv <(outs mBMm:$dst), (ins OP_mWm_1:$src),
                            "vconv.fp32.bf16", "$dst, $src">;
}

// 7.6 vconv.bf16.fp32 – Single Precision to bfloat16 Conversion
let Itinerary = II_VCONV in
let Defs = [srF2FFlags] in {
  let Uses = [crF2FMask, crRnd] in {
    def VCONV_BF16_FP32 :
        AIE2_mv_w_srs_bf_inst_st <(outs mWa:$dst), (ins mBMs:$src),
                              "vconv.bf16.fp32", "$dst, $src">;
  }
}

// 7.7 VFLOOR – Vector bfloat16 to Integer Conversion
let Defs = [srF2IFlags] in {
  let Uses = [crF2IMask] in {
    let Itinerary = II_VFLOORs32bf16_AM in
    def VFLOOR_S32_BF16_mFl2FxSrc_AM :
        AIE2_mv_float_to_int_inst_st <(outs mXa:$dst), (ins mAMs:$src, mSs:$shft),
                              "vfloor.s32.bf16", "$dst, $src, $shft">,
        mv_mFl2FxSrc_AM;
    let Itinerary = II_VFLOORs32bf16_W in
    def VFLOOR_S32_BF16_mFl2FxSrc_W :
        AIE2_mv_float_to_int_inst_st <(outs mXa:$dst), (ins mWs:$src, mSs:$shft),
                              "vfloor.s32.bf16", "$dst, $src, $shft">,
        mv_mFl2FxSrc_W;
  }
}

// 8.1.1 VINSERT - Insert scalar in vector
let Itinerary = II_VINSERT in {
  def VINSERT_8 :
    AIE2_vec_insert_inst_mv<(outs mXm:$dst), (ins mXm:$s1, eR29:$idx, eR:$s0),
                            "vinsert.8", "$dst, $s1, $idx, $s0">,
    vec_insert_src_R</*s2v_wide_8*/0b00>;
  def VINSERT_16 :
    AIE2_vec_insert_inst_mv<(outs mXm:$dst), (ins mXm:$s1, eR29:$idx, eR:$s0),
                            "vinsert.16", "$dst, $s1, $idx, $s0">,
    vec_insert_src_R</*s2v_wide_16*/0b01>;
  def VINSERT_32 :
    AIE2_vec_insert_inst_mv<(outs mXm:$dst), (ins mXm:$s1, eR29:$idx, eR:$s0),
                            "vinsert.32", "$dst, $s1, $idx, $s0">,
    vec_insert_src_R</*s2v_wide_32*/0b10>;
  def VINSERT_64 :
    AIE2_vec_insert_inst_mv<(outs mXm:$dst), (ins mXm:$s1, eR29:$idx, eL:$s0),
                            "vinsert.64", "$dst, $s1, $idx, $s0">,
    vec_insert_src_L</*s2v_wide_64*/0b11>;
}

// 8.1.2 VEXTRACT - Extract scalar from vector
let Itinerary = II_VEXTRACT in {
  let ItineraryRegPairs = [ItinRegClassPair<II_VEXTRACT_RS,[OperandRegClass<0, eR>]>, 
                           ItinRegClassPair<II_VEXTRACT_P,[OperandRegClass<0, eP>]>, 
                           ItinRegClassPair<II_VEXTRACT_M,[OperandRegClass<0, eM>]>,
                           ItinRegClassPair<II_VEXTRACT_RS,[OperandRegClass<0, eS>]>, 
                           ItinRegClassPair<II_VEXTRACT_DC,[OperandRegClass<0, eDC>]>,
                           ItinRegClassPair<II_VEXTRACT_DJ,[OperandRegClass<0, eDJ>]>, 
                           ItinRegClassPair<II_VEXTRACT_DN,[OperandRegClass<0, eDN>]>] in {
  let Uses = [crVaddSign] in {
    def VEXTRACT_D8 :
      AIE2_vec_extract_inst_mv</*sign*/0b0, (outs OP_mMvSclDst:$dst), (ins mXm:$s1, OP_mRS4m:$idx),
                              "vextract.d8", "$dst, $s1, $idx">,
      vec_extract_dest_R</*s2v_wide_8*/0b00>;
    def VEXTRACT_D16 :
      AIE2_vec_extract_inst_mv</*sign*/0b0, (outs OP_mMvSclDst:$dst), (ins mXm:$s1, OP_mRS4m:$idx),
                              "vextract.d16", "$dst, $s1, $idx">,
      vec_extract_dest_R</*s2v_wide_16*/0b01>;
    def VEXTRACT_D32 :
      AIE2_vec_extract_inst_mv</*sign*/0b0,(outs OP_mMvSclDst:$dst), (ins mXm:$s1, OP_mRS4m:$idx),
                              "vextract.d32", "$dst, $s1, $idx">,
      vec_extract_dest_R</*s2v_wide_32*/0b10>;
    def VEXTRACT_D64 :
      AIE2_vec_extract_inst_mv</*sign*/0b0,(outs eL:$dst), (ins mXm:$s1, OP_mRS4m:$idx),
                              "vextract.d64", "$dst, $s1, $idx">,
      vec_extract_dest_L</*s2v_wide_64*/0b11>;
  }
  def VEXTRACT_S8 :
    AIE2_vec_extract_inst_mv</*sign*/0b1, (outs OP_mMvSclDst:$dst), (ins mXm:$s1, OP_mRS4m:$idx),
                            "vextract.s8", "$dst, $s1, $idx">,
    vec_extract_dest_R</*s2v_wide_8*/0b00>;
  def VEXTRACT_S16 :
    AIE2_vec_extract_inst_mv</*sign*/0b1, (outs OP_mMvSclDst:$dst), (ins mXm:$s1, OP_mRS4m:$idx),
                            "vextract.s16", "$dst, $s1, $idx">,
    vec_extract_dest_R</*s2v_wide_16*/0b01>;
  def VEXTRACT_S32 :
    AIE2_vec_extract_inst_mv</*sign*/0b1,(outs OP_mMvSclDst:$dst), (ins mXm:$s1, OP_mRS4m:$idx),
                            "vextract.s32", "$dst, $s1, $idx">,
    vec_extract_dest_R</*s2v_wide_32*/0b10>;
  def VEXTRACT_S64 :
    AIE2_vec_extract_inst_mv</*sign*/0b1,(outs eL:$dst), (ins mXm:$s1, OP_mRS4m:$idx),
                            "vextract.s64", "$dst, $s1, $idx">,
    vec_extract_dest_L</*s2v_wide_64*/0b11>;
  }
}

// 8.1.3 VPUSH - Pushes scalar word into vector
// Note: We cannot use the same itinerary for all VPUSH variants, as their
// operands are ordered differently...
let Itinerary = II_VPUSH_LO in {
  def VPUSH_LO_8 :
    AIE2_vec_push_lo_inst_mv<(outs mXm:$d), (ins eR:$s1, mXn:$s2),
                             "vpush.lo.8", "$d, $s1, $s2">,
    vec_push_lo_R</*s2v_wide_8*/0b00>;
  def VPUSH_LO_16 :
    AIE2_vec_push_lo_inst_mv<(outs mXm:$d), (ins eR:$s1, mXn:$s2),
                             "vpush.lo.16", "$d, $s1, $s2">,
    vec_push_lo_R</*s2v_wide_16*/0b01>;
  def VPUSH_LO_32 :
    AIE2_vec_push_lo_inst_mv<(outs mXm:$d), (ins eR:$s1, mXn:$s2),
                             "vpush.lo.32", "$d, $s1, $s2">,
    vec_push_lo_R</*s2v_wide_32*/0b10>;
  def VPUSH_LO_64 :
    AIE2_vec_push_lo_inst_mv<(outs mXm:$d), (ins eL:$s1, mXn:$s2),
                             "vpush.lo.64", "$d, $s1, $s2">,
    vec_push_lo_L</*s2v_wide_64*/0b11>;
}
let Itinerary = II_VPUSH_HI in {
  def VPUSH_HI_8 :
    AIE2_vec_push_hi_inst_mv<(outs mXm:$d), (ins mXm:$s1, eR:$s2),
                             "vpush.hi.8", "$d, $s1, $s2">,
    vec_push_hi_R</*s2v_wide_8*/0b00>;
  def VPUSH_HI_16 :
    AIE2_vec_push_hi_inst_mv<(outs mXm:$d), (ins mXm:$s1, eR:$s2),
                             "vpush.hi.16", "$d, $s1, $s2">,
    vec_push_hi_R</*s2v_wide_16*/0b01>;
  def VPUSH_HI_32 :
    AIE2_vec_push_hi_inst_mv<(outs mXm:$d), (ins mXm:$s1, eR:$s2),
                             "vpush.hi.32", "$d, $s1, $s2">,
    vec_push_hi_R</*s2v_wide_32*/0b10>;
  def VPUSH_HI_64 :
    AIE2_vec_push_hi_inst_mv<(outs mXm:$d), (ins mXm:$s1, eL:$s2),
                             "vpush.hi.64", "$d, $s1, $s2">,
    vec_push_hi_L</*s2v_wide_64*/0b11>;
}

// 8.1.4 VBCST - Broadcast scalar word to vector
let Itinerary = II_VBCST in {
  def VBCST_8 :
    AIE2_vec_broadcast_inst_mv<(outs mXm:$dst), (ins eR:$s0),
                            "vbcst.8", "$dst, $s0">,
    vec_insert_src_R</*s2v_wide_8*/0b00>;
  def VBCST_16 :
    AIE2_vec_broadcast_inst_mv<(outs mXm:$dst), (ins eR:$s0),
                            "vbcst.16", "$dst, $s0">,
    vec_insert_src_R</*s2v_wide_16*/0b01>;
  def VBCST_32 :
    AIE2_vec_broadcast_inst_mv<(outs mXm:$dst), (ins eR:$s0),
                            "vbcst.32", "$dst, $s0">,
    vec_insert_src_R</*s2v_wide_32*/0b10>;
  def VBCST_64 :
    AIE2_vec_broadcast_inst_mv<(outs mXm:$dst), (ins eL:$s0),
                            "vbcst.64", "$dst, $s0">,
    vec_insert_src_L</*s2v_wide_64*/0b11>;
}

// 8.1.5 VEXTBCST - Extract and broadcast scalar word between two vectors
let Itinerary = II_VEXTBCST in {
   def VEXTBCST_8_mRm : AIE2_vec_extract_broadcast_inst_mv</*word_size*/0b00,
       (outs mXm:$dst), (ins mXm:$src, eR:$idx),
       "vextbcst.8", "$dst, $src, $idx" >, mExtractIdx_reg;
   def VEXTBCST_16_mRm : AIE2_vec_extract_broadcast_inst_mv</*word_size*/0b01,
       (outs mXm:$dst), (ins mXm:$src, eR:$idx),
       "vextbcst.16", "$dst, $src, $idx" >, mExtractIdx_reg;
   def VEXTBCST_32_mRm : AIE2_vec_extract_broadcast_inst_mv</*word_size*/0b10,
       (outs mXm:$dst), (ins mXm:$src, eR:$idx),
       "vextbcst.32", "$dst, $src, $idx" >, mExtractIdx_reg;
   def VEXTBCST_64_mRm : AIE2_vec_extract_broadcast_inst_mv</*word_size*/0b11,
       (outs mXm:$dst), (ins mXm:$src, eR:$idx),
       "vextbcst.64", "$dst, $src, $idx" >, mExtractIdx_reg;

   def VEXTBCST_8_mExtractIdxImm : AIE2_vec_extract_broadcast_inst_mv</*word_size*/0b00,
       (outs mXm:$dst), (ins mXm:$src, imm5:$idx),
       "vextbcst.8", "$dst, $src, $idx" >, mExtractIdx_imm;
   def VEXTBCST_16_mExtractIdxImm : AIE2_vec_extract_broadcast_inst_mv</*word_size*/0b01,
       (outs mXm:$dst), (ins mXm:$src, imm5:$idx),
       "vextbcst.16", "$dst, $src, $idx" >, mExtractIdx_imm;
   def VEXTBCST_32_mExtractIdxImm : AIE2_vec_extract_broadcast_inst_mv</*word_size*/0b10,
       (outs mXm:$dst), (ins mXm:$src, imm5:$idx),
       "vextbcst.32", "$dst, $src, $idx" >, mExtractIdx_imm;
   def VEXTBCST_64_mExtractIdxImm : AIE2_vec_extract_broadcast_inst_mv</*word_size*/0b11,
       (outs mXm:$dst), (ins mXm:$src, imm5:$idx),
       "vextbcst.64", "$dst, $src, $idx" >, mExtractIdx_imm;
}

// 8.1.6 VBCSTSHFL - Broadcast scalar word to vector and shuffle
let Itinerary = II_VBCSTSHFL in {
  def VBCSTSHFL_8 :
    AIE2_vec_broadcast_shuffle_inst_mv<(outs OP_mShflDst:$dst), (ins eR:$s0, eR29:$idx),
                            "vbcstshfl.8", "$dst, $s0, $idx">,
    vec_insert_src_R</*s2v_wide_8*/0b00>;
  def VBCSTSHFL_16 :
    AIE2_vec_broadcast_shuffle_inst_mv<(outs OP_mShflDst:$dst), (ins eR:$s0, eR29:$idx),
                            "vbcstshfl.16", "$dst, $s0, $idx">,
    vec_insert_src_R</*s2v_wide_16*/0b01>;
  def VBCSTSHFL_32 :
    AIE2_vec_broadcast_shuffle_inst_mv<(outs OP_mShflDst:$dst), (ins eR:$s0, eR29:$idx),
                            "vbcstshfl.32", "$dst, $s0, $idx">,
    vec_insert_src_R</*s2v_wide_32*/0b10>;
  def VBCSTSHFL_64 :
    AIE2_vec_broadcast_shuffle_inst_mv<(outs OP_mShflDst:$dst), (ins eL:$s0, eR29:$idx),
                            "vbcstshfl.64", "$dst, $s0, $idx">,
    vec_insert_src_L</*s2v_wide_64*/0b11>;
}

// 8.2.1 VUNPACK - Vector word unpacking
let Itinerary = II_VUNPACK in {
  let Uses = [crUnpackSign] in {
    def VUNPACK_D8_D4  : AIE2_mv_unpack_inst_ldb</*sgn*/0b0, /*sz*/0b0,
        (outs mXm:$dst), (ins mWs:$src), "vunpack.d8.d4", "$dst, $src">;
    def VUNPACK_D16_D8 : AIE2_mv_unpack_inst_ldb</*sgn*/0b0, /*sz*/0b1,
        (outs mXm:$dst), (ins mWs:$src), "vunpack.d16.d8", "$dst, $src">;
  }
    def VUNPACK_S8_S4  : AIE2_mv_unpack_inst_ldb</*sgn*/0b1, /*sz*/0b0,
        (outs mXm:$dst), (ins mWs:$src), "vunpack.s8.s4", "$dst, $src">;
    def VUNPACK_S16_S8 : AIE2_mv_unpack_inst_ldb</*sgn*/0b1, /*sz*/0b1,
        (outs mXm:$dst), (ins mWs:$src), "vunpack.s16.s8", "$dst, $src">;
}

// 8.2.2 VPACK - Vector word packing
let Itinerary = II_VPACK in {
  class VPACK<bits<1> sgn, bits<1> sz, string opcodestr>
      : AIE2_mv_pack_inst_st< sgn, sz, (outs mWa:$dst), (ins mXs:$src),
        opcodestr, "$dst, $src">;
  let Uses = [crSat, crPackSign] in {
    def VPACK_D4_D8  : VPACK</*sgn*/0b0, /*sz*/0b0, "vpack.d4.d8">;
    def VPACK_D8_D16 : VPACK</*sgn*/0b0, /*sz*/0b1, "vpack.d8.d16">;
  }
  let Uses = [crSat] in {
    def VPACK_S4_S8  : VPACK</*sgn*/0b1, /*sz*/0b0, "vpack.s4.s8">;
    def VPACK_S8_S16 : VPACK</*sgn*/0b1, /*sz*/0b1, "vpack.s8.s16">;
  }
}

// 8.2.3 VSHUFFLE - Shuffle vector
let Itinerary = II_VSHUFFLE in {
  def VSHUFFLE : AIE2_vec_shuffle_inst_mv<(outs OP_mShflDst:$dst), (ins mXm:$s1, mXn:$s2, eR:$mod ),
      "vshuffle", "$dst, $s1, $s2, $mod">;
}

// 8.2.4 VSHIFT - Shift vector
let Itinerary = II_VSHIFT in {
  def VSHIFT : AIE2_vec_shift_inst_mv<(outs mXm:$d), (ins mXm:$s1, mXn:$s2, eR:$shift ),
      "vshift", "$d, $s1, $s2, $shift">;
}

// 8.2.5 VSHIFT_ALIGN - Shift vector
let Itinerary = II_VSHIFT_ALIGN in {
  def VSHIFT_ALIGN : AIE2_vec_shift_align_inst_mv<(outs mXm:$d), (ins mXm:$s1, mSs:$pre, mXn:$s2, eR:$shift ),
      "vshift.align", "$d, $s1, $pre, $s2, $shift">;
}

// 8.2.6 VSEL - Select words from two vectors
let Itinerary = II_VSEL in {
  def VSEL_8 : AIE2_vec_add_select_inst_mv< /*op*/0b00100, /*word_size*/0b00,
      (outs mXm:$d), (ins mXm:$s1, mXn:$s2, eL:$sel),
      "vsel.8", "$d, $s1, $s2, $sel" >;
  def VSEL_16 : AIE2_vec_add_select_inst_mv< /*op*/0b00100, /*word_size*/0b01,
      (outs mXm:$d), (ins mXm:$s1, mXn:$s2, eRS8:$sel),
      "vsel.16", "$d, $s1, $s2, $sel" >;
  def VSEL_32 : AIE2_vec_add_select_inst_mv< /*op*/0b00100, /*word_size*/0b10,
      (outs mXm:$d), (ins mXm:$s1, mXn:$s2, eRS8:$sel),
      "vsel.32", "$d, $s1, $s2, $sel" >;
}

// 8.3.1 VADD/VSUB - Vector addition / subtraction
let Itinerary = II_VADD in {
 def VADD_8 : AIE2_vec_add_inst_mv<
    /*op*/0b00000, /*vec_size*/0b00, (outs mXm:$d) , (ins  mXm:$s1 ,  mXn:$s2  ),
    "vadd.8", "$d, $s1, $s2">;
 def VADD_16 : AIE2_vec_add_inst_mv<
    /*op*/0b00000, /*vec_size*/0b01, (outs mXm:$d) , (ins  mXm:$s1 ,  mXn:$s2  ),
    "vadd.16", "$d, $s1, $s2">;
 def VADD_32 : AIE2_vec_add_inst_mv<
    /*op*/0b00000, /*vec_size*/0b10, (outs mXm:$d) , (ins  mXm:$s1 ,  mXn:$s2  ),
    "vadd.32", "$d, $s1, $s2">;
}

let Itinerary = II_VSUB in {
 def VSUB_8 : AIE2_vec_add_inst_mv<
    /*op*/0b00010, /*vec_size*/0b00, (outs mXm:$d) , (ins  mXm:$s1 ,  mXn:$s2  ),
    "vsub.8", "$d, $s1, $s2">;
 def VSUB_16 : AIE2_vec_add_inst_mv<
    /*op*/0b00010, /*vec_size*/0b01, (outs mXm:$d) , (ins  mXm:$s1 ,  mXn:$s2  ),
    "vsub.16", "$d, $s1, $s2">;
 def VSUB_32 : AIE2_vec_add_inst_mv<
    /*op*/0b00010, /*vec_size*/0b10, (outs mXm:$d) , (ins  mXm:$s1 ,  mXn:$s2  ),
    "vsub.32", "$d, $s1, $s2">;
}

// 8.3.2 VADDSUB – Configurable vector addition or subtraction
let Itinerary = II_VADDSUB in {
 def VADDSUB_8 : AIE2_vec_add_select_inst_mv<
    /*op*/0b00101, /*word_size*/0b00,
    (outs mXm:$d), (ins mXm:$s1, mXn:$s2, eL:$sel),
    "vaddsub.8", "$d, $s1, $s2, $sel">;
 def VADDSUB_16 : AIE2_vec_add_select_inst_mv<
    /*op*/0b00101, /*word_size*/0b01,
    (outs mXm:$d), (ins mXm:$s1, mXn:$s2, eRS8:$sel),
    "vaddsub.16", "$d, $s1, $s2, $sel">;
 def VADDSUB_32 : AIE2_vec_add_select_inst_mv<
    /*op*/0b00101, /*word_size*/0b10,
    (outs mXm:$d), (ins mXm:$s1, mXn:$s2, eRS8:$sel),
    "vaddsub.32", "$d, $s1, $s2, $sel">;
}

// 8.3.3 VSUB_LT / VSUB_GE – Combined vector subtraction and comparison
let Itinerary = II_VSUB_CMP in {
  let Uses = [crVaddSign] in {
    def VSUB_LT_D8: AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01010, /*vec_size*/0b00, (outs mXm:$d, eL:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vsub_lt.d8", "$d, $cmp, $s1, $s2">;
    def VSUB_LT_D16: AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01010, /*vec_size*/0b01, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vsub_lt.d16", "$d, $cmp, $s1, $s2">;
    def VSUB_LT_D32: AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01010, /*vec_size*/0b10, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vsub_lt.d32", "$d, $cmp, $s1, $s2">;

    def VSUB_GE_D8: AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01100, /*vec_size*/0b00, (outs mXm:$d, eL:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vsub_ge.d8", "$d, $cmp, $s1, $s2">;
    def VSUB_GE_D16: AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01100, /*vec_size*/0b01, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vsub_ge.d16", "$d, $cmp, $s1, $s2">;
    def VSUB_GE_D32: AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01100, /*vec_size*/0b10, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vsub_ge.d32", "$d, $cmp, $s1, $s2">;
  }

    def VSUB_LT_S8: AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01011, /*vec_size*/0b00, (outs mXm:$d, eL:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vsub_lt.s8", "$d, $cmp, $s1, $s2">;
    def VSUB_LT_S16: AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01011, /*vec_size*/0b01, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vsub_lt.s16", "$d, $cmp, $s1, $s2">;
    def VSUB_LT_S32: AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01011, /*vec_size*/0b10, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vsub_lt.s32", "$d, $cmp, $s1, $s2">;

    def VSUB_GE_S8: AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01101, /*vec_size*/0b00, (outs mXm:$d, eL:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vsub_ge.s8", "$d, $cmp, $s1, $s2">;
    def VSUB_GE_S16: AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01101, /*vec_size*/0b01, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vsub_ge.s16", "$d, $cmp, $s1, $s2">;
    def VSUB_GE_S32: AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01101, /*vec_size*/0b10, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vsub_ge.s32", "$d, $cmp, $s1, $s2">;
}

// 8.3.4 VNEG_GTZ - Combined vector negation and zero comparison
let Itinerary = II_VNEG_GTZ in {
 def VNEG_GTZ8  : AIE2_vec_abs_not_inst_mv</*op*/0b11011, /*word_size*/0b00,
     (outs mXm:$d, eL:$cmp), (ins mXn:$s1), "vneg_gtz8", "$d, $cmp, $s1">;
 def VNEG_GTZ16 : AIE2_vec_abs_not_inst_mv</*op*/0b11011, /*word_size*/0b01,
     (outs mXm:$d, eRS8:$cmp), (ins mXn:$s1), "vneg_gtz16", "$d, $cmp, $s1">;
 def VNEG_GTZ32 : AIE2_vec_abs_not_inst_mv</*op*/0b11011, /*word_size*/0b10,
     (outs mXm:$d, eRS8:$cmp), (ins mXn:$s1), "vneg_gtz32", "$d, $cmp, $s1">;
}

// 8.4.1 VBAND/VBOR - Vector bitwise AND and OR operations
let Itinerary = II_VBLOG in {
 def VBAND : AIE2_vec_log_inst_mv<
    /*op*/0b10011, (outs mXm:$d) , (ins  mXm:$s1 ,  mXn:$s2),
    "vband", "$d, $s1, $s2">;

 def VBOR : AIE2_vec_log_inst_mv<
    /*op*/0b10111, (outs mXm:$d) , (ins  mXm:$s1 ,  mXn:$s2),
    "vbor", "$d, $s1, $s2">;
}

// 8.4.2 VBNEG_LTZ - Combined vector bitwise inversion value and zero comparison
let Itinerary = II_VBNEG_LTZ in {
 def VBNEG_LTZ_S8  : AIE2_vec_abs_not_inst_mv</*op*/0b01110, /*word_size*/0b00,
     (outs mXm:$d, eL:$cmp), (ins mXn:$s1), "vbneg_ltz.s8", "$d, $cmp, $s1">;
 def VBNEG_LTZ_S16 : AIE2_vec_abs_not_inst_mv</*op*/0b01110, /*word_size*/0b01,
     (outs mXm:$d, eRS8:$cmp), (ins mXn:$s1), "vbneg_ltz.s16", "$d, $cmp, $s1">;
 def VBNEG_LTZ_S32 : AIE2_vec_abs_not_inst_mv</*op*/0b01110, /*word_size*/0b10,
     (outs mXm:$d, eRS8:$cmp), (ins mXn:$s1), "vbneg_ltz.s32", "$d, $cmp, $s1">;
}

// 8.5.2 VMAX_LT - Combined vector maximum and comparison
let Itinerary = II_VMAX_LT in {
  let Uses = [crVaddSign] in {
    def VMAX_LT_D8 : AIE2_vec_sub_min_max_inst_mv<
       /*op*/0b10100, /*vec_size*/0b00, (outs mXm:$d, eL:$cmp), (ins  mXm:$s1, mXn:$s2),
       "vmax_lt.d8", "$d, $cmp, $s1, $s2">;
    def VMAX_LT_D16 : AIE2_vec_sub_min_max_inst_mv<
       /*op*/0b10100, /*vec_size*/0b01, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
       "vmax_lt.d16", "$d, $cmp, $s1, $s2">;
    def VMAX_LT_D32 : AIE2_vec_sub_min_max_inst_mv<
       /*op*/0b10100, /*vec_size*/0b10, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
       "vmax_lt.d32", "$d, $cmp, $s1, $s2">;
  }
  def VMAX_LT_S8 : AIE2_vec_sub_min_max_inst_mv<
     /*op*/0b10101, /*vec_size*/0b00, (outs mXm:$d, eL:$cmp), (ins  mXm:$s1, mXn:$s2),
     "vmax_lt.s8", "$d, $cmp, $s1, $s2">;
  def VMAX_LT_S16 : AIE2_vec_sub_min_max_inst_mv<
     /*op*/0b10101, /*vec_size*/0b01, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
     "vmax_lt.s16", "$d, $cmp, $s1, $s2">;
  def VMAX_LT_S32 : AIE2_vec_sub_min_max_inst_mv<
     /*op*/0b10101, /*vec_size*/0b10, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
     "vmax_lt.s32", "$d, $cmp, $s1, $s2">;
  def VMAX_LT_BF16 : AIE2_vec_bf_min_max_inst_mv<
     /*op*/0b10110, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
     "vmax_lt.bf16", "$d, $cmp, $s1, $s2">;
}

// 8.5.4 VABS_GTZ - Combined vector absolute value and zero
let Itinerary = II_VABS_GTZ in {
  let Uses = [crVaddSign] in {
    def VABS_GTZ_D8  : AIE2_vec_abs_not_inst_mv</*op*/0b00110, /*word_size*/0b00,
        (outs mXm:$d, eL:$cmp), (ins mXn:$s1), "vabs_gtz.d8", "$d, $cmp, $s1">;
    def VABS_GTZ_D16 : AIE2_vec_abs_not_inst_mv</*op*/0b00110, /*word_size*/0b01,
        (outs mXm:$d, eRS8:$cmp), (ins mXn:$s1), "vabs_gtz.d16", "$d, $cmp, $s1">;
    def VABS_GTZ_D32 : AIE2_vec_abs_not_inst_mv</*op*/0b00110, /*word_size*/0b10,
        (outs mXm:$d, eRS8:$cmp), (ins mXn:$s1), "vabs_gtz.d32", "$d, $cmp, $s1">;
  }
    def VABS_GTZ_S8  : AIE2_vec_abs_not_inst_mv</*op*/0b00111, /*word_size*/0b00,
        (outs mXm:$d, eL:$cmp), (ins mXn:$s1), "vabs_gtz.s8", "$d, $cmp, $s1">;
    def VABS_GTZ_S16 : AIE2_vec_abs_not_inst_mv</*op*/0b00111, /*word_size*/0b01,
        (outs mXm:$d, eRS8:$cmp), (ins mXn:$s1), "vabs_gtz.s16", "$d, $cmp, $s1">;
    def VABS_GTZ_S32 : AIE2_vec_abs_not_inst_mv</*op*/0b00111, /*word_size*/0b10,
        (outs mXm:$d, eRS8:$cmp), (ins mXn:$s1), "vabs_gtz.s32", "$d, $cmp, $s1">;
}

// 8.5.1 VLT/VGE – Vector comparison
let Itinerary = II_VCMP in {
  class VLT_VGE<bits<5> op, bits<2> vec_size, dag outs, string opcodestr> :
    AIE2_vec_compare_inst_mv< op, vec_size, outs, (ins mXm:$s1, mXn:$s2), opcodestr, "$cmp, $s1, $s2">;

  let Uses = [crVaddSign] in {
    def VLT_D8 : VLT_VGE</*op*/0b11000 ,/*vec_size*/0b00 , (outs eL:$cmp), "vlt.d8">;
    def VLT_D16 : VLT_VGE</*op*/0b11000 ,/*vec_size*/0b01 , (outs eRS8:$cmp), "vlt.d16">;
    def VLT_D32 : VLT_VGE</*op*/0b11000 ,/*vec_size*/0b10 , (outs eRS8:$cmp), "vlt.d32">;

    def VGE_D8 : VLT_VGE</*op*/0b11100 ,/*vec_size*/0b00 , (outs eL:$cmp), "vge.d8">;
    def VGE_D16 : VLT_VGE</*op*/0b11100 ,/*vec_size*/0b01 , (outs eRS8:$cmp), "vge.d16">;
    def VGE_D32 : VLT_VGE</*op*/0b11100 ,/*vec_size*/0b10 , (outs eRS8:$cmp), "vge.d32">;
  }

  def VLT_S8 : VLT_VGE</*op*/0b11001 ,/*vec_size*/0b00 , (outs eL:$cmp), "vlt.s8">;
  def VLT_S16 : VLT_VGE</*op*/0b11001 ,/*vec_size*/0b01 , (outs eRS8:$cmp), "vlt.s16">;
  def VLT_S32 : VLT_VGE</*op*/0b11001 ,/*vec_size*/0b10 , (outs eRS8:$cmp), "vlt.s32">;

  def VGE_S8 : VLT_VGE</*op*/0b11101 ,/*vec_size*/0b00 , (outs eL:$cmp), "vge.s8">;
  def VGE_S16 : VLT_VGE</*op*/0b11101 ,/*vec_size*/0b01 , (outs eRS8:$cmp), "vge.s16">;
  def VGE_S32 : VLT_VGE</*op*/0b11101 ,/*vec_size*/0b10 , (outs eRS8:$cmp), "vge.s32">;
}

// BF16
let Itinerary = II_VCMP in {
  def VLT_BF16 : AIE2_vec_bf_compare_inst_mv</*op*/0b11010,
      (outs eRS8:$cmp), (ins mXm:$s1, mXn:$s2), "vlt.bf16", "$cmp, $s1, $s2">;
  def VGE_BF16 : AIE2_vec_bf_compare_inst_mv</*op*/0b11110,
      (outs eRS8:$cmp), (ins mXm:$s1, mXn:$s2), "vge.bf16", "$cmp, $s1, $s2">;
}

// 8.5.2 VMIN_GE - Combined vector minimum and comparison
let Itinerary = II_VMIN_GE in {
  let Uses = [crVaddSign] in {
    def VMIN_GE_D8 : AIE2_vec_sub_min_max_inst_mv<
       /*op*/0b10000, /*vec_size*/0b00, (outs mXm:$d, eL:$cmp), (ins  mXm:$s1, mXn:$s2),
       "vmin_ge.d8", "$d, $cmp, $s1, $s2">;
    def VMIN_GE_D16 : AIE2_vec_sub_min_max_inst_mv<
       /*op*/0b10000, /*vec_size*/0b01, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
       "vmin_ge.d16", "$d, $cmp, $s1, $s2">;
    def VMIN_GE_D32 : AIE2_vec_sub_min_max_inst_mv<
       /*op*/0b10000, /*vec_size*/0b10, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
       "vmin_ge.d32", "$d, $cmp, $s1, $s2">;
  }
  def VMIN_GE_S8 : AIE2_vec_sub_min_max_inst_mv<
     /*op*/0b10001, /*vec_size*/0b00, (outs mXm:$d, eL:$cmp), (ins  mXm:$s1, mXn:$s2),
     "vmin_ge.s8", "$d, $cmp, $s1, $s2">;
  def VMIN_GE_S16 : AIE2_vec_sub_min_max_inst_mv<
     /*op*/0b10001, /*vec_size*/0b01, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
     "vmin_ge.s16", "$d, $cmp, $s1, $s2">;
  def VMIN_GE_S32 : AIE2_vec_sub_min_max_inst_mv<
     /*op*/0b10001, /*vec_size*/0b10, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
     "vmin_ge.s32", "$d, $cmp, $s1, $s2">;
  def VMIN_GE_BF16 : AIE2_vec_bf_min_max_inst_mv<
     /*op*/0b10010, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
     "vmin_ge.bf16", "$d, $cmp, $s1, $s2">;
}
// 8.5.3 VMAXDIFF_LT – Combined vector maximum difference and less-than comparison
let Itinerary = II_VMAXDIFF_LT in {
    let Uses = [crVaddSign] in {
      def VMAXDIFF_LT_D8 : AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01000, /*vec_size*/0b00, (outs mXm:$d, eL:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vmaxdiff_lt.d8", "$d, $cmp, $s1, $s2">;
      def VMAXDIFF_LT_D16 : AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01000, /*vec_size*/0b01, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vmaxdiff_lt.d16", "$d, $cmp, $s1, $s2">;
      def VMAXDIFF_LT_D32 : AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01000, /*vec_size*/0b10, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vmaxdiff_lt.d32", "$d, $cmp, $s1, $s2">;
    }
      def VMAXDIFF_LT_S8 : AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01001, /*vec_size*/0b00, (outs mXm:$d, eL:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vmaxdiff_lt.s8", "$d, $cmp, $s1, $s2">;
      def VMAXDIFF_LT_S16 : AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01001, /*vec_size*/0b01, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vmaxdiff_lt.s16", "$d, $cmp, $s1, $s2">;
      def VMAXDIFF_LT_S32 : AIE2_vec_sub_min_max_inst_mv<
        /*op*/0b01001, /*vec_size*/0b10, (outs mXm:$d, eRS8:$cmp), (ins  mXm:$s1, mXn:$s2),
        "vmaxdiff_lt.s32", "$d, $cmp, $s1, $s2">;
}

// 8.5.5 VEQZ – Compare Vector Elements to Zero
let Itinerary = II_VEQZ in {
  class VEQZ<bits<2> vec_size, dag outs, string opcodestr> :
    AIE2_vec_eqz_inst_mv< vec_size, outs, (ins mXn:$s2), opcodestr, "$cmp, $s2">;

  def VEQZ_8 : VEQZ</*vec_size*/0b00, (outs eL:$cmp), "veqz.8">;
  def VEQZ_16 : VEQZ</*vec_size*/0b01, (outs eRS8:$cmp), "veqz.16">;
  def VEQZ_32 : VEQZ</*vec_size*/0b10, (outs eRS8:$cmp), "veqz.32">;
}

// 9.2 VADD/VSUB - Accumulator addition/subtraction
let Itinerary = II_VACC in {
   def VADD : AIE2_vacc_inst_vec </*md2*/0b0, /*md1*/0b0, (outs eCM:$dst), 
              (ins eCM:$acc1, eCM:$acc2, eR:$c), 
              "vadd", "$dst, $acc1, $acc2, $c">;
   def VSUB : AIE2_vacc_inst_vec </*md2*/0b1, /*md1*/0b0, (outs eCM:$dst), 
              (ins eCM:$acc1, eCM:$acc2, eR:$c), 
              "vsub", "$dst, $acc1, $acc2, $c">;
}

// 9.3 VADD.f/VSUB.f – Floating-point accumulator addition/subtraction
let Itinerary = II_VACCf in
let Defs = [srFPFlags] in {
  let Uses = [crFPMask] in {
    def VADD_F : AIE2_vacc_bf_inst_vec </*md2*/0b0, /*md1*/0b0, (outs mBMa:$dst),
               (ins mBMa:$acc1, mBMa:$acc2, eR:$c),
               "vadd.f", "$dst, $acc1, $acc2, $c">;
    def VSUB_F : AIE2_vacc_bf_inst_vec </*md2*/0b1, /*md1*/0b0, (outs mBMa:$dst),
               (ins mBMa:$acc1, mBMa:$acc2, eR:$c),
               "vsub.f", "$dst, $acc1, $acc2, $c">;
  }
}

// 9.4 VNEGADD/VNEGSUB - Accumulator negated addition/subtraction
let Itinerary = II_VACC in {
   def VNEGADD : AIE2_vacc_inst_vec </*md2*/0b1, /*md1*/0b1, (outs eCM:$dst), 
                 (ins eCM:$acc1, eCM:$acc2, eR:$c), 
                 "vnegadd", "$dst, $acc1, $acc2, $c">;
   def VNEGSUB : AIE2_vacc_inst_vec </*md2*/0b0, /*md1*/0b1, (outs eCM:$dst), 
                 (ins eCM:$acc1, eCM:$acc2, eR:$c),
                 "vnegsub", "$dst, $acc1, $acc2, $c">;
}

// 9.5 VNEGADD.f / VNEGSUB.f – Floating-point accumulator negated
// addition / subtraction
let Itinerary = II_VACCf in
let Defs = [srFPFlags] in {
  let Uses = [crFPMask] in {
    def VNEGADD_F : AIE2_vacc_bf_inst_vec </*md2*/0b1, /*md1*/0b1, (outs mBMa:$dst),
                  (ins mBMa:$acc1, mBMa:$acc2, eR:$c),
                  "vnegadd.f", "$dst, $acc1, $acc2, $c">;
    def VNEGSUB_F : AIE2_vacc_bf_inst_vec </*md2*/0b0, /*md1*/0b1, (outs mBMa:$dst),
                  (ins mBMa:$acc1, mBMa:$acc2, eR:$c),
                  "vnegsub.f", "$dst, $acc1, $acc2, $c">;
  }
}

// 9.6 VNEG – accumulator word negation
let Itinerary = II_VNEG in {
    def VNEG : AIE2_vneg_inst_vec <(outs mCMa:$dst),
                  (ins mCMa:$acc1, eR:$c),
                  "vneg", "$dst, $acc1, $c">;
}

// 9.7 VNEG.f – Floating-point accumulator word negation
let Itinerary = II_VNEGf in
let Defs = [srFPFlags] in {
  let Uses = [crFPMask] in {
    def VNEG_F : AIE2_vneg_bf_inst_vec <(outs mBMa:$dst),
                  (ins mBMa:$acc1, eR:$c),
                  "vneg.f", "$dst, $acc1, $c">;
  }
}

// 9.8 VCLR – Clears accumulator word
let Itinerary = II_VCLRf in
let Defs = [srFPFlags] in {
  let Uses = [crFPMask] in {
    def VCLR_vclr_BF : AIE2_vclr_bf_inst_vec <(outs mBMa:$dst), (ins),
                  "vclr", "$dst">;
  }
}

let Itinerary = II_VCLR in {
  def VCLR_vclr : AIE2_vclr_inst_vec <(outs eCM:$dst), (ins), "vclr", "$dst">;
}

// 9.9 VMUL / VNEGMUL - Matrix multiplication
let Itinerary = II_VMUL in {
  def VMUL_vmac_cm_core_dense :
    AIE2_vmul_inst_vec< /*mdm*/0b0, (outs eCM:$dst) , (ins mXv:$s1, mXw:$s2, eR:$c ),
                        "vmul", "$dst, $s1, $s2, $c">,
    vmac_cm_core_dense;
  def VMUL_vmac_cm_core_sparse_wide :
    AIE2_vmul_inst_vec< /*mdm*/0b0, (outs eCM:$dst) , (ins eYs:$ys1, mQQXw:$qxs2, eR:$c ),
                        "vmul", "$dst, $ys1, $qxs2, $c">,
    vmac_cm_core_sparse_wide;
  def VMUL_vmac_cm_core_sparse_narrow :
    AIE2_vmul_inst_vec< /*mdm*/0b0, (outs eCM:$dst) , (ins mXv:$xs1, mQQXw:$qxs2, eR:$c ),
                        "vmul", "$dst, $xs1, $qxs2, $c">,
    vmac_cm_core_sparse_narrow;
}

let Itinerary = II_VNEGMUL in {
 def VNEGMUL_vmac_cm_core_dense :
    AIE2_vmul_inst_vec< /*mdm*/0b1, (outs eCM:$dst) , (ins mXv:$s1, mXw:$s2, eR:$c ),
                        "vnegmul", "$dst, $s1, $s2, $c">,
    vmac_cm_core_dense;
  def VNEGMUL_vmac_cm_core_sparse_wide :
    AIE2_vmul_inst_vec< /*mdm*/0b1, (outs eCM:$dst) , (ins eYs:$ys1, mQQXw:$qxs2, eR:$c ),
                        "vnegmul", "$dst, $ys1, $qxs2, $c">,
    vmac_cm_core_sparse_wide;
  def VNEGMUL_vmac_cm_core_sparse_narrow :
    AIE2_vmul_inst_vec< /*mdm*/0b1, (outs eCM:$dst) , (ins mXv:$xs1, mQQXw:$qxs2, eR:$c ),
                        "vnegmul", "$dst, $xs1, $qxs2, $c">,
    vmac_cm_core_sparse_narrow;
}

// 9.10 VMUL.f / VNEGMUL.f - BFloat16 Matrix multiplication
let Itinerary = II_VMULf in
let Defs = [srFPFlags] in {
  let Uses = [crFPMask] in {
    def VMUL_F_vmac_bm_core_dense :
    AIE2_vmul_bf_inst_vec< /*mdm*/0b0, (outs mBMa:$dst) , (ins mXv:$s1, mXw:$s2, eR:$c ),
                        "vmul.f", "$dst, $s1, $s2, $c">,
    vmac_bm_core_dense;

    def VMUL_F_vmac_bm_core_sparse_wide :
      AIE2_vmul_bf_inst_vec< /*mdm*/0b0, (outs mBMa:$dst) , (ins eYs:$ys1, mQQXw:$qxs2, eR:$c ),
                        "vmul.f", "$dst, $ys1, $qxs2, $c">,
      vmac_bm_core_sparse_wide;
    def VMUL_F_vmac_bm_core_sparse_narrow :
      AIE2_vmul_bf_inst_vec< /*mdm*/0b0, (outs mBMa:$dst) , (ins mXv:$xs1, mQQXw:$qxs2, eR:$c ),
                        "vmul.f", "$dst, $xs1, $qxs2, $c">,
      vmac_bm_core_sparse_narrow;
  }
}

let Itinerary = II_VNEGMULf in
let Defs = [srFPFlags] in {
  let Uses = [crFPMask] in {
    def VNEGMUL_F_vmac_bm_core_dense :
    AIE2_vmul_bf_inst_vec< /*mdm*/0b1, (outs mBMa:$dst) , (ins mXv:$s1, mXw:$s2, eR:$c ),
                        "vnegmul.f", "$dst, $s1, $s2, $c">,
    vmac_bm_core_dense;
    def VNEGMUL_F_vmac_bm_core_sparse_wide :
      AIE2_vmul_bf_inst_vec< /*mdm*/0b1, (outs mBMa:$dst) , (ins eYs:$ys1, mQQXw:$qxs2, eR:$c ),
                        "vnegmul.f", "$dst, $ys1, $qxs2, $c">,
      vmac_bm_core_sparse_wide;

    def VNEGMUL_F_vmac_bm_core_sparse_narrow :
      AIE2_vmul_bf_inst_vec< /*mdm*/0b1, (outs mBMa:$dst) , (ins mXv:$xs1, mQQXw:$qxs2, eR:$c ),
                        "vnegmul.f", "$dst, $xs1, $qxs2, $c">,
      vmac_bm_core_sparse_narrow;
  }
}

// 9.11 VMAC / VMSC / VNEGMAC / VNEGMSC - Matrix multiplyaccumulate/subtract
let Itinerary = II_VMAC in {
  def VMAC_vmac_cm_core_dense :
    AIE2_vmac_inst_vec< /*mdm*/0b0, /*md1*/0b0,
                        (outs eCM:$dst) , (ins eCM:$acc1, mXv:$s1, mXw:$s2, eR:$c),
                        "vmac", "$dst, $acc1, $s1, $s2, $c" >,
    vmac_cm_core_dense;
  def VMAC_vmac_cm_core_sparse_wide :
    AIE2_vmac_inst_vec< /*mdm*/0b0, /*md1*/0b0,
                        (outs eCM:$dst) , (ins eCM:$acc1, eYs:$ys1, mQQXw:$qxs2, eR:$c),
                        "vmac", "$dst, $acc1, $ys1, $qxs2, $c" >,
    vmac_cm_core_sparse_wide;
  def VMAC_vmac_cm_core_sparse_narrow :
    AIE2_vmac_inst_vec< /*mdm*/0b0, /*md1*/0b0,
                        (outs eCM:$dst) , (ins eCM:$acc1, mXv:$xs1, mQQXw:$qxs2, eR:$c),
                        "vmac", "$dst, $acc1, $xs1, $qxs2, $c" >,
    vmac_cm_core_sparse_narrow;
}

let Itinerary = II_VMSC in {
  def VMSC_vmac_cm_core_dense :
    AIE2_vmac_inst_vec< /*mdm*/0b1, /*md1*/0b0,
                        (outs eCM:$dst) , (ins eCM:$acc1, mXv:$s1, mXw:$s2, eR:$c),
                        "vmsc", "$dst, $acc1, $s1, $s2, $c" >,
    vmac_cm_core_dense;
  def VMSC_vmac_cm_core_sparse_wide :
    AIE2_vmac_inst_vec< /*mdm*/0b1, /*md1*/0b0,
                        (outs eCM:$dst) , (ins eCM:$acc1, eYs:$ys1, mQQXw:$qxs2, eR:$c),
                        "vmsc", "$dst, $acc1, $ys1, $qxs2, $c" >,
    vmac_cm_core_sparse_wide;
  def VMSC_vmac_cm_core_sparse_narrow :
    AIE2_vmac_inst_vec< /*mdm*/0b1, /*md1*/0b0,
                        (outs eCM:$dst) , (ins eCM:$acc1, mXv:$xs1, mQQXw:$qxs2, eR:$c),
                        "vmsc", "$dst, $acc1, $xs1, $qxs2, $c" >,
    vmac_cm_core_sparse_narrow;
}

let Itinerary = II_VNEGMAC in {
  def VNEGMAC_vmac_cm_core_dense :
    AIE2_vmac_inst_vec< /*mdm*/0b1, /*md1*/0b1,
                        (outs eCM:$dst) , (ins eCM:$acc1, mXv:$s1, mXw:$s2, eR:$c),
                        "vnegmac", "$dst, $acc1, $s1, $s2, $c" >,
    vmac_cm_core_dense;
  def VNEGMAC_vmac_cm_core_sparse_wide :
    AIE2_vmac_inst_vec< /*mdm*/0b1, /*md1*/0b1,
                        (outs eCM:$dst) , (ins eCM:$acc1, eYs:$ys1, mQQXw:$qxs2, eR:$c),
                        "vnegmac", "$dst, $acc1, $ys1, $qxs2, $c" >,
    vmac_cm_core_sparse_wide;
  def VNEGMAC_vmac_cm_core_sparse_narrow :
    AIE2_vmac_inst_vec< /*mdm*/0b1, /*md1*/0b1,
                        (outs eCM:$dst) , (ins eCM:$acc1, mXv:$xs1, mQQXw:$qxs2, eR:$c),
                        "vnegmac", "$dst, $acc1, $xs1, $qxs2, $c" >,
    vmac_cm_core_sparse_narrow;
}

let Itinerary = II_VNEGMSC in {
  def VNEGMSC_vmac_cm_core_dense :
    AIE2_vmac_inst_vec< /*mdm*/0b0, /*md1*/0b1,
                        (outs eCM:$dst) , (ins eCM:$acc1, mXv:$s1, mXw:$s2, eR:$c),
                        "vnegmsc", "$dst, $acc1, $s1, $s2, $c" >,
    vmac_cm_core_dense;
  def VNEGMSC_vmac_cm_core_sparse_wide :
    AIE2_vmac_inst_vec< /*mdm*/0b0, /*md1*/0b1,
                        (outs eCM:$dst) , (ins eCM:$acc1, eYs:$ys1, mQQXw:$qxs2, eR:$c),
                        "vnegmsc", "$dst, $acc1, $ys1, $qxs2, $c" >,
    vmac_cm_core_sparse_wide;
  def VNEGMSC_vmac_cm_core_sparse_narrow :
    AIE2_vmac_inst_vec< /*mdm*/0b0, /*md1*/0b1,
                        (outs eCM:$dst) , (ins eCM:$acc1, mXv:$xs1, mQQXw:$qxs2, eR:$c),
                        "vnegmsc", "$dst, $acc1, $xs1, $qxs2, $c" >,
    vmac_cm_core_sparse_narrow;
}

// 9.12 VMAC.f / VMSC.f – BFloat16 matrix multiply-accumulate/subtract
let Itinerary = II_VMACf in
let Defs = [srFPFlags] in {
  let Uses = [crFPMask] in {
  def VMAC_F_vmac_bm_core_dense :
    AIE2_vmac_bf_inst_vec< /*mdm*/0b0,
                        (outs mBMa:$dst) , (ins mBMa:$acc1, mXv:$s1, mXw:$s2, eR:$c),
                        "vmac.f", "$dst, $acc1, $s1, $s2, $c" >,
    vmac_bm_core_dense;
  def VMAC_F_vmac_bm_core_sparse_wide :
    AIE2_vmac_bf_inst_vec< /*mdm*/0b0,
                        (outs mBMa:$dst) , (ins mBMa:$acc1, eYs:$ys1, mQQXw:$qxs2, eR:$c),
                        "vmac.f", "$dst, $acc1, $ys1, $qxs2, $c" >,
    vmac_bm_core_sparse_wide;
  def VMAC_F_vmac_bm_core_sparse_narrow :
    AIE2_vmac_bf_inst_vec< /*mdm*/0b0,
                        (outs mBMa:$dst) , (ins mBMa:$acc1, mXv:$xs1, mQQXw:$qxs2, eR:$c),
                        "vmac.f", "$dst, $acc1, $xs1, $qxs2, $c" >,
    vmac_bm_core_sparse_narrow;

  }
}

let Itinerary = II_VMSCf in
let Defs = [srFPFlags] in {
  let Uses = [crFPMask] in {
  def VMSC_F_vmac_bm_core_dense :
    AIE2_vmac_bf_inst_vec< /*mdm*/0b1,
                        (outs mBMa:$dst) , (ins mBMa:$acc1, mXv:$s1, mXw:$s2, eR:$c),
                        "vmsc.f", "$dst, $acc1, $s1, $s2, $c" >,
    vmac_bm_core_dense;
  def VMSC_F_vmac_bm_core_sparse_wide :
    AIE2_vmac_bf_inst_vec< /*mdm*/0b1,
                        (outs mBMa:$dst) , (ins mBMa:$acc1, eYs:$ys1, mQQXw:$qxs2, eR:$c),
                        "vmsc.f", "$dst, $acc1, $ys1, $qxs2, $c" >,
    vmac_bm_core_sparse_wide;
  def VMSC_F_vmac_bm_core_sparse_narrow :
    AIE2_vmac_bf_inst_vec< /*mdm*/0b1,
                        (outs mBMa:$dst) , (ins mBMa:$acc1, mXv:$xs1, mQQXw:$qxs2, eR:$c),
                        "vmsc.f", "$dst, $acc1, $xs1, $qxs2, $c" >,
    vmac_bm_core_sparse_narrow;

  }
}

// 9.13 VADDMAC / VADDMSC / VSUBMAC / VSUBMSC - Double accumulator matrix mul-acc/sub
let Itinerary = II_VADDMAC in {
  let Constraints = "$dst = $acc1" in {
    def VADDMAC_vmac_bm_core_dense : AIE2_vaddmac_inst_vec < /*mdm*/0b0, /*md2*/0b0,
                  (outs eCM:$dst) , (ins eCM:$acc1, eCM:$acc2, mXv:$s1, mXw:$s2, eR:$c),
                  "vaddmac", "$dst, $acc1, $acc2, $s1, $s2, $c" >, vmac_cm_core_dense;
    def VADDMAC_vmac_cm_core_sparse_wide : AIE2_vaddmac_inst_vec < /*mdm*/0b0, /*md2*/0b0,
                  (outs eCM:$dst) , (ins eCM:$acc1, eCM:$acc2, eYs:$ys1, mQQXw:$qxs2, eR:$c),
                  "vaddmac", "$dst, $acc1, $acc2, $ys1, $qxs2, $c" >, vmac_cm_core_sparse_wide;
    def VADDMAC_vmac_cm_core_sparse_narrow : AIE2_vaddmac_inst_vec < /*mdm*/0b0, /*md2*/0b0,
                  (outs eCM:$dst) , (ins eCM:$acc1, eCM:$acc2, mXv:$xs1, mQQXw:$qxs2, eR:$c),
                  "vaddmac", "$dst, $acc1, $acc2, $xs1, $qxs2, $c" >, vmac_cm_core_sparse_narrow;
    def VADDMSC_vmac_bm_core_dense : AIE2_vaddmac_inst_vec < /*mdm*/0b1, /*md2*/0b0,
                  (outs eCM:$dst) , (ins eCM:$acc1, eCM:$acc2, mXv:$s1, mXw:$s2, eR:$c),
                  "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2, $c" >, vmac_cm_core_dense;
    def VADDMSC_vmac_cm_core_sparse_wide : AIE2_vaddmac_inst_vec < /*mdm*/0b1, /*md2*/0b0,
                  (outs eCM:$dst) , (ins eCM:$acc1, eCM:$acc2, eYs:$ys1, mQQXw:$qxs2, eR:$c),
                  "vaddmsc", "$dst, $acc1, $acc2, $ys1, $qxs2, $c" >, vmac_cm_core_sparse_wide;
    def VADDMSC_vmac_cm_core_sparse_narrow : AIE2_vaddmac_inst_vec < /*mdm*/0b1, /*md2*/0b0,
                  (outs eCM:$dst) , (ins eCM:$acc1, eCM:$acc2, mXv:$xs1, mQQXw:$qxs2, eR:$c),
                  "vaddmsc", "$dst, $acc1, $acc2, $xs1, $qxs2, $c" >, vmac_cm_core_sparse_narrow;
    def VSUBMAC_vmac_bm_core_dense : AIE2_vaddmac_inst_vec < /*mdm*/0b0, /*md2*/0b1,
                  (outs eCM:$dst) , (ins eCM:$acc1, eCM:$acc2, mXv:$s1, mXw:$s2, eR:$c),
                  "vsubmac", "$dst, $acc1, $acc2, $s1, $s2, $c" >, vmac_cm_core_dense;
    def VSUBMAC_vmac_cm_core_sparse_wide : AIE2_vaddmac_inst_vec < /*mdm*/0b0, /*md2*/0b1,
                  (outs eCM:$dst) , (ins eCM:$acc1, eCM:$acc2, eYs:$ys1, mQQXw:$qxs2, eR:$c),
                  "vsubmac", "$dst, $acc1, $acc2, $ys1, $qxs2, $c" >, vmac_cm_core_sparse_wide;
    def VSUBMAC_vmac_cm_core_sparse_narrow : AIE2_vaddmac_inst_vec < /*mdm*/0b0, /*md2*/0b1,
                  (outs eCM:$dst) , (ins eCM:$acc1, eCM:$acc2, mXv:$xs1, mQQXw:$qxs2, eR:$c),
                  "vsubmac", "$dst, $acc1, $acc2, $xs1, $qxs2, $c" >, vmac_cm_core_sparse_narrow;
    def VSUBMSC_vmac_bm_core_dense : AIE2_vaddmac_inst_vec < /*mdm*/0b1, /*md2*/0b1,
                  (outs eCM:$dst) , (ins eCM:$acc1, eCM:$acc2, mXv:$s1, mXw:$s2, eR:$c),
                  "vsubmsc", "$dst, $acc1, $acc2, $s1, $s2, $c" >, vmac_cm_core_dense;
    def VSUBMSC_vmac_cm_core_sparse_wide : AIE2_vaddmac_inst_vec < /*mdm*/0b1, /*md2*/0b1,
                  (outs eCM:$dst) , (ins eCM:$acc1, eCM:$acc2, eYs:$ys1, mQQXw:$qxs2, eR:$c),
                  "vsubmsc", "$dst, $acc1, $acc2, $ys1, $qxs2, $c" >, vmac_cm_core_sparse_wide;
    def VSUBMSC_vmac_cm_core_sparse_narrow : AIE2_vaddmac_inst_vec < /*mdm*/0b1, /*md2*/0b1,
                  (outs eCM:$dst) , (ins eCM:$acc1, eCM:$acc2, mXv:$xs1, mQQXw:$qxs2, eR:$c),
                  "vsubmsc", "$dst, $acc1, $acc2, $xs1, $qxs2, $c" >, vmac_cm_core_sparse_narrow;
  }
}

// 9.14 VADDMAC.f / VADDMSC.f  Double accumulator matrix bfloat16 matrix multiply-
// accumulate/subtract
let Itinerary = II_VADDMACf in
let Defs = [srFPFlags] in {
  let Uses = [crFPMask] in {
  let Constraints = "$dst = $acc1" in {
    def VADDMAC_F_vmac_bm_core_dense : AIE2_vaddmac_bf_inst_vec < /*mdm*/0b0,
                  (outs mBMa:$dst) , (ins mBMa:$acc1, mBMa:$acc2, mXv:$s1, mXw:$s2, eR:$c),
                  "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $c" >, vmac_bm_core_dense;
    def VADDMAC_F_vmac_bm_core_sparse_narrow : AIE2_vaddmac_bf_inst_vec < /*mdm*/0b0,
                  (outs mBMa:$dst) , (ins mBMa:$acc1, mBMa:$acc2, mXv:$xs1, mQQXw:$qxs2, eR:$c),
                  "vaddmac.f", "$dst, $acc1, $acc2, $xs1, $qxs2, $c" >, vmac_bm_core_sparse_narrow;
    def VADDMAC_F_vmac_bm_core_sparse_wide : AIE2_vaddmac_bf_inst_vec < /*mdm*/0b0,
                  (outs mBMa:$dst) , (ins mBMa:$acc1, mBMa:$acc2, eYs:$ys1, mQQXw:$qxs2, eR:$c),
                  "vaddmac.f", "$dst, $acc1, $acc2, $ys1, $qxs2, $c" >, vmac_bm_core_sparse_wide;
    def VADDMSC_F_vmac_bm_core_dense : AIE2_vaddmac_bf_inst_vec < /*mdm*/0b1,
                  (outs mBMa:$dst) , (ins mBMa:$acc1, mBMa:$acc2, mXv:$s1, mXw:$s2, eR:$c),
                  "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $c" >, vmac_bm_core_dense;
    def VADDMSC_F_vmac_bm_core_sparse_narrow : AIE2_vaddmac_bf_inst_vec < /*mdm*/0b1,
                  (outs mBMa:$dst) , (ins mBMa:$acc1, mBMa:$acc2, mXv:$xs1, mQQXw:$qxs2, eR:$c),
                  "vaddmsc.f", "$dst, $acc1, $acc2, $xs1, $qxs2, $c" >, vmac_bm_core_sparse_narrow;
    def VADDMSC_F_vmac_bm_core_sparse_wide : AIE2_vaddmac_bf_inst_vec < /*mdm*/0b1,
                  (outs mBMa:$dst) , (ins mBMa:$acc1, mBMa:$acc2, eYs:$ys1, mQQXw:$qxs2, eR:$c),
                  "vaddmsc.f", "$dst, $acc1, $acc2, $ys1, $qxs2, $c" >, vmac_bm_core_sparse_wide;
    }
  }
}

// 3.4 VLD.UPS - Vector Loads with upshifting
let mayLoad = true in {
  let Itinerary = II_VLDA_UPS in
  let Defs = [srUPS_of] in {
    let Uses = [crSat, crUPSSign] in {
      def VLDA_UPS_S32_D16_ag_idx_imm : AIE2_dmw_lda_ups_w2b_inst_lda < (outs mBMs:$dst) , (ins mSs:$shft, eP:$ptr, imm3x32:$imm),
        "vlda.ups.s32.d16", "$dst, $shft, [$ptr, $imm]">, AIE2_ag_nospill__ag_idx_imm, mv_ups_base<0, 0>;
      def VLDA_UPS_S64_D32_ag_idx_imm : AIE2_dmw_lda_ups_w2b_inst_lda < (outs mBMs:$dst) , (ins mSs:$shft, eP:$ptr, imm3x32:$imm),
        "vlda.ups.s64.d32", "$dst, $shft, [$ptr, $imm]">, AIE2_ag_nospill__ag_idx_imm, mv_ups_base<1, 0>;
      def VLDA_UPS_S32_D8_ag_idx_imm : AIE2_dmw_lda_ups_w2c_inst_lda < (outs mCMs:$dst) , (ins mSs:$shft, eP:$ptr, imm3x32:$imm),
        "vlda.ups.s32.d8", "$dst, $shft, [$ptr, $imm]">, AIE2_ag_nospill__ag_idx_imm, mv_ups_base<0, 0>;
      def VLDA_UPS_S64_D16_ag_idx_imm : AIE2_dmw_lda_ups_w2c_inst_lda < (outs mCMs:$dst) , (ins mSs:$shft, eP:$ptr, imm3x32:$imm),
        "vlda.ups.s64.d16", "$dst, $shft, [$ptr, $imm]">, AIE2_ag_nospill__ag_idx_imm, mv_ups_base<1, 0>;
      def VLDA_UPS_S32_D16_ag_idx : AIE2_dmw_lda_ups_w2b_inst_lda < (outs mBMs:$dst) , (ins mSs:$shft, eP:$ptr, eDJ:$dj),
        "vlda.ups.s32.d16", "$dst, $shft, [$ptr, $dj]">, AIE2_ag_nospill__ag_idx, mv_ups_base<0, 0>;
      def VLDA_UPS_S64_D32_ag_idx : AIE2_dmw_lda_ups_w2b_inst_lda < (outs mBMs:$dst) , (ins mSs:$shft, eP:$ptr, eDJ:$dj),
        "vlda.ups.s64.d32", "$dst, $shft, [$ptr, $dj]">, AIE2_ag_nospill__ag_idx, mv_ups_base<1, 0>;
      def VLDA_UPS_S32_D8_ag_idx : AIE2_dmw_lda_ups_w2c_inst_lda < (outs mCMs:$dst) , (ins mSs:$shft, eP:$ptr, eDJ:$dj),
        "vlda.ups.s32.d8", "$dst, $shft, [$ptr, $dj]">, AIE2_ag_nospill__ag_idx, mv_ups_base<0, 0>;
      def VLDA_UPS_S64_D16_ag_idx : AIE2_dmw_lda_ups_w2c_inst_lda < (outs mCMs:$dst) , (ins mSs:$shft, eP:$ptr, eDJ:$dj),
        "vlda.ups.s64.d16", "$dst, $shft, [$ptr, $dj]">, AIE2_ag_nospill__ag_idx, mv_ups_base<1, 0>;
    }
    let Uses = [crSat] in {
      def VLDA_UPS_S32_S16_ag_idx_imm : AIE2_dmw_lda_ups_w2b_inst_lda < (outs mBMs:$dst) , (ins mSs:$shft, eP:$ptr, imm3x32:$imm),
        "vlda.ups.s32.s16", "$dst, $shft, [$ptr, $imm]">, AIE2_ag_nospill__ag_idx_imm, mv_ups_base<0, 1>;
      def VLDA_UPS_S64_S32_ag_idx_imm : AIE2_dmw_lda_ups_w2b_inst_lda < (outs mBMs:$dst) , (ins mSs:$shft, eP:$ptr, imm3x32:$imm),
        "vlda.ups.s64.s32", "$dst, $shft, [$ptr, $imm]">, AIE2_ag_nospill__ag_idx_imm, mv_ups_base<1, 1>;
      def VLDA_UPS_S32_S8_ag_idx_imm : AIE2_dmw_lda_ups_w2c_inst_lda < (outs mCMs:$dst) , (ins mSs:$shft, eP:$ptr, imm3x32:$imm),
        "vlda.ups.s32.s8", "$dst, $shft, [$ptr, $imm]">, AIE2_ag_nospill__ag_idx_imm, mv_ups_base<0, 1>;
      def VLDA_UPS_S64_S16_ag_idx_imm : AIE2_dmw_lda_ups_w2c_inst_lda < (outs mCMs:$dst) , (ins mSs:$shft, eP:$ptr, imm3x32:$imm),
        "vlda.ups.s64.s16", "$dst, $shft, [$ptr, $imm]">, AIE2_ag_nospill__ag_idx_imm, mv_ups_base<1, 1>;
      def VLDA_UPS_S32_S16_ag_idx : AIE2_dmw_lda_ups_w2b_inst_lda < (outs mBMs:$dst) , (ins mSs:$shft, eP:$ptr, eDJ:$dj),
        "vlda.ups.s32.s16", "$dst, $shft, [$ptr, $dj]">, AIE2_ag_nospill__ag_idx, mv_ups_base<0, 1>;
      def VLDA_UPS_S64_S32_ag_idx : AIE2_dmw_lda_ups_w2b_inst_lda < (outs mBMs:$dst) , (ins mSs:$shft, eP:$ptr, eDJ:$dj),
        "vlda.ups.s64.s32", "$dst, $shft, [$ptr, $dj]">, AIE2_ag_nospill__ag_idx, mv_ups_base<1, 1>;
      def VLDA_UPS_S32_S8_ag_idx : AIE2_dmw_lda_ups_w2c_inst_lda < (outs mCMs:$dst) , (ins mSs:$shft, eP:$ptr, eDJ:$dj),
        "vlda.ups.s32.s8", "$dst, $shft, [$ptr, $dj]">, AIE2_ag_nospill__ag_idx, mv_ups_base<0, 1>;
      def VLDA_UPS_S64_S16_ag_idx : AIE2_dmw_lda_ups_w2c_inst_lda < (outs mCMs:$dst) , (ins mSs:$shft, eP:$ptr, eDJ:$dj),
        "vlda.ups.s64.s16", "$dst, $shft, [$ptr, $dj]">, AIE2_ag_nospill__ag_idx, mv_ups_base<1, 1>;
    }
  }

  let Itinerary = II_VLDA_POSTINC_UPS in
  let Constraints = "$ptr_out = $ptr" in
  let Defs = [srUPS_of] in {
    let Uses = [crSat, crUPSSign] in {
      def VLDA_UPS_S32_D16_ag_pstm_nrm : AIE2_dmw_lda_ups_w2b_inst_lda <
                        (outs mBMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, eM:$mod),
                        "vlda.ups.s32.d16", "$dst, $shft, [$ptr], $mod">,
                            AIE2_ag_nospill__ag_pstm_nrm, mv_ups_base<0, 0>;
      def VLDA_UPS_S32_D16_ag_pstm_nrm_imm : AIE2_dmw_lda_ups_w2b_inst_lda <
                        (outs mBMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, imm4x32:$imm),
                        "vlda.ups.s32.d16", "$dst, $shft, [$ptr], $imm">,
                            AIE2_ag_nospill__ag_pstm_nrm_imm, mv_ups_base<0, 0>;
      def VLDA_UPS_S64_D32_ag_pstm_nrm : AIE2_dmw_lda_ups_w2b_inst_lda <
                        (outs mBMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, eM:$mod),
                        "vlda.ups.s64.d32", "$dst, $shft, [$ptr], $mod">,
                            AIE2_ag_nospill__ag_pstm_nrm, mv_ups_base<1, 0>;
      def VLDA_UPS_S64_D32_ag_pstm_nrm_imm : AIE2_dmw_lda_ups_w2b_inst_lda <
                        (outs mBMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, imm4x32:$imm),
                        "vlda.ups.s64.d32", "$dst, $shft, [$ptr], $imm">,
                            AIE2_ag_nospill__ag_pstm_nrm_imm, mv_ups_base<1, 0>;
      def VLDA_UPS_S32_D8_ag_pstm_nrm : AIE2_dmw_lda_ups_w2c_inst_lda <
                        (outs mCMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, eM:$mod),
                        "vlda.ups.s32.d8", "$dst, $shft, [$ptr], $mod">,
                            AIE2_ag_nospill__ag_pstm_nrm, mv_ups_base<0, 0>;
      def VLDA_UPS_S32_D8_ag_pstm_nrm_imm : AIE2_dmw_lda_ups_w2c_inst_lda <
                        (outs mCMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, imm4x32:$imm),
                        "vlda.ups.s32.d8", "$dst, $shft, [$ptr], $imm">,
                            AIE2_ag_nospill__ag_pstm_nrm_imm, mv_ups_base<0, 0>;
      def VLDA_UPS_S64_D16_ag_pstm_nrm : AIE2_dmw_lda_ups_w2c_inst_lda <
                        (outs mCMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, eM:$mod),
                        "vlda.ups.s64.d16", "$dst, $shft, [$ptr], $mod">,
                            AIE2_ag_nospill__ag_pstm_nrm, mv_ups_base<1, 0>;
      def VLDA_UPS_S64_D16_ag_pstm_nrm_imm : AIE2_dmw_lda_ups_w2c_inst_lda <
                        (outs mCMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, imm4x32:$imm),
                        "vlda.ups.s64.d16", "$dst, $shft, [$ptr], $imm">,
                            AIE2_ag_nospill__ag_pstm_nrm_imm, mv_ups_base<1, 0>;
    }
    let Uses = [crSat] in {
      def VLDA_UPS_S32_S16_ag_pstm_nrm : AIE2_dmw_lda_ups_w2b_inst_lda <
                        (outs mBMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, eM:$mod),
                        "vlda.ups.s32.s16", "$dst, $shft, [$ptr], $mod">,
                            AIE2_ag_nospill__ag_pstm_nrm, mv_ups_base<0, 1>;
      def VLDA_UPS_S32_S16_ag_pstm_nrm_imm : AIE2_dmw_lda_ups_w2b_inst_lda <
                        (outs mBMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, imm4x32:$imm),
                        "vlda.ups.s32.s16", "$dst, $shft, [$ptr], $imm">,
                            AIE2_ag_nospill__ag_pstm_nrm_imm, mv_ups_base<0, 1>;
      def VLDA_UPS_S64_S32_ag_pstm_nrm : AIE2_dmw_lda_ups_w2b_inst_lda <
                        (outs mBMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, eM:$mod),
                        "vlda.ups.s64.s32", "$dst, $shft, [$ptr], $mod">,
                            AIE2_ag_nospill__ag_pstm_nrm, mv_ups_base<1, 1>;
      def VLDA_UPS_S64_S32_ag_pstm_nrm_imm : AIE2_dmw_lda_ups_w2b_inst_lda <
                        (outs mBMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, imm4x32:$imm),
                        "vlda.ups.s64.s32", "$dst, $shft, [$ptr], $imm">,
                            AIE2_ag_nospill__ag_pstm_nrm_imm, mv_ups_base<1, 1>;
      def VLDA_UPS_S32_S8_ag_pstm_nrm : AIE2_dmw_lda_ups_w2c_inst_lda <
                        (outs mCMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, eM:$mod),
                        "vlda.ups.s32.s8", "$dst, $shft, [$ptr], $mod">,
                            AIE2_ag_nospill__ag_pstm_nrm, mv_ups_base<0, 1>;
      def VLDA_UPS_S32_S8_ag_pstm_nrm_imm : AIE2_dmw_lda_ups_w2c_inst_lda <
                        (outs mCMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, imm4x32:$imm),
                        "vlda.ups.s32.s8", "$dst, $shft, [$ptr], $imm">,
                            AIE2_ag_nospill__ag_pstm_nrm_imm, mv_ups_base<0, 1>;
      def VLDA_UPS_S64_S16_ag_pstm_nrm : AIE2_dmw_lda_ups_w2c_inst_lda <
                        (outs mCMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, eM:$mod),
                        "vlda.ups.s64.s16", "$dst, $shft, [$ptr], $mod">,
                            AIE2_ag_nospill__ag_pstm_nrm, mv_ups_base<1, 1>;
      def VLDA_UPS_S64_S16_ag_pstm_nrm_imm : AIE2_dmw_lda_ups_w2c_inst_lda <
                        (outs mCMs:$dst, eP:$ptr_out) , (ins mSs:$shft, eP:$ptr, imm4x32:$imm),
                        "vlda.ups.s64.s16", "$dst, $shft, [$ptr], $imm">,
                            AIE2_ag_nospill__ag_pstm_nrm_imm, mv_ups_base<1, 1>;
    }
  }

  let Itinerary = II_VLDA_2D_UPS in
  let Constraints = "$ptr_out = $ptr" in
  let Defs = [srUPS_of] in {
    let Uses = [crSat, crUPSSign] in {
      let DecoderMethod = "DecodeVLDA_2D_UPS_BMInstruction" in
      def VLDA_2D_UPS_S32_D16 : AIE2_dmw_lda_ups_w2b_inst_lda < (outs mBMs:$dst, eP:$ptr_out, eDC:$count_out) , (ins mSs:$shft, eP:$ptr, eD:$mod),
        "vlda.2d.ups.s32.d16", "$dst, $shft, [$ptr], $mod">, AIE2_ag_nospill__ag_pstm_2d, mv_ups_base<0, 0>;
      let DecoderMethod = "DecodeVLDA_2D_UPS_BMInstruction" in
      def VLDA_2D_UPS_S64_D32 : AIE2_dmw_lda_ups_w2b_inst_lda < (outs mBMs:$dst, eP:$ptr_out, eDC:$count_out) , (ins mSs:$shft, eP:$ptr, eD:$mod),
        "vlda.2d.ups.s64.d32", "$dst, $shft, [$ptr], $mod">, AIE2_ag_nospill__ag_pstm_2d, mv_ups_base<1, 0>;
      let DecoderMethod = "DecodeVLDA_2D_UPS_CMInstruction" in
      def VLDA_2D_UPS_S32_D8 : AIE2_dmw_lda_ups_w2c_inst_lda < (outs mCMs:$dst, eP:$ptr_out, eDC:$count_out) , (ins mSs:$shft, eP:$ptr, eD:$mod),
        "vlda.2d.ups.s32.d8", "$dst, $shft, [$ptr], $mod">, AIE2_ag_nospill__ag_pstm_2d, mv_ups_base<0, 0>;
      let DecoderMethod = "DecodeVLDA_2D_UPS_CMInstruction" in
      def VLDA_2D_UPS_S64_D16 : AIE2_dmw_lda_ups_w2c_inst_lda < (outs mCMs:$dst, eP:$ptr_out, eDC:$count_out) , (ins mSs:$shft, eP:$ptr, eD:$mod),
        "vlda.2d.ups.s64.d16", "$dst, $shft, [$ptr], $mod">, AIE2_ag_nospill__ag_pstm_2d, mv_ups_base<1, 0>;
    }
    let Uses = [crSat] in {
      let DecoderMethod = "DecodeVLDA_2D_UPS_BMInstruction" in
      def VLDA_2D_UPS_S32_S16 : AIE2_dmw_lda_ups_w2b_inst_lda < (outs mBMs:$dst, eP:$ptr_out, eDC:$count_out) , (ins mSs:$shft, eP:$ptr, eD:$mod),
        "vlda.2d.ups.s32.s16", "$dst, $shft, [$ptr], $mod">, AIE2_ag_nospill__ag_pstm_2d, mv_ups_base<0, 1>;
      let DecoderMethod = "DecodeVLDA_2D_UPS_BMInstruction" in
      def VLDA_2D_UPS_S64_S32 : AIE2_dmw_lda_ups_w2b_inst_lda < (outs mBMs:$dst, eP:$ptr_out, eDC:$count_out) , (ins mSs:$shft, eP:$ptr, eD:$mod),
        "vlda.2d.ups.s64.s32", "$dst, $shft, [$ptr], $mod">, AIE2_ag_nospill__ag_pstm_2d, mv_ups_base<1, 1>;
      let DecoderMethod = "DecodeVLDA_2D_UPS_CMInstruction" in
      def VLDA_2D_UPS_S32_S8 : AIE2_dmw_lda_ups_w2c_inst_lda < (outs mCMs:$dst, eP:$ptr_out, eDC:$count_out) , (ins mSs:$shft, eP:$ptr, eD:$mod),
        "vlda.2d.ups.s32.s8", "$dst, $shft, [$ptr], $mod">, AIE2_ag_nospill__ag_pstm_2d, mv_ups_base<0, 1>;
      let DecoderMethod = "DecodeVLDA_2D_UPS_CMInstruction" in
      def VLDA_2D_UPS_S64_S16 : AIE2_dmw_lda_ups_w2c_inst_lda < (outs mCMs:$dst, eP:$ptr_out, eDC:$count_out) , (ins mSs:$shft, eP:$ptr, eD:$mod),
        "vlda.2d.ups.s64.s16", "$dst, $shft, [$ptr], $mod">, AIE2_ag_nospill__ag_pstm_2d, mv_ups_base<1, 1>;
    }
  }

  let Itinerary = II_VLDA_3D_UPS in
  let Constraints = "$ptr_out = $ptr" in
  let Defs = [srUPS_of] in {
    let Uses = [crSat, crUPSSign] in {
      let DecoderMethod = "DecodeVLDA_3D_UPS_BMInstruction" in
      def VLDA_3D_UPS_S32_D16 : AIE2_dmw_lda_ups_w2b_inst_lda <
          (outs mBMs:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins mSs:$shft, eP:$ptr, eDS:$mod),
          "vlda.3d.ups.s32.d16", "$dst, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mv_ups_base<0, 0>;
      let DecoderMethod = "DecodeVLDA_3D_UPS_BMInstruction" in
      def VLDA_3D_UPS_S64_D32 : AIE2_dmw_lda_ups_w2b_inst_lda <
          (outs mBMs:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins mSs:$shft, eP:$ptr, eDS:$mod),
          "vlda.3d.ups.s64.d32", "$dst, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mv_ups_base<1, 0>;
      let DecoderMethod = "DecodeVLDA_3D_UPS_CMInstruction" in
      def VLDA_3D_UPS_S32_D8 : AIE2_dmw_lda_ups_w2c_inst_lda <
          (outs mCMs:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins mSs:$shft, eP:$ptr, eDS:$mod),
          "vlda.3d.ups.s32.d8", "$dst, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mv_ups_base<0, 0>;
      let DecoderMethod = "DecodeVLDA_3D_UPS_CMInstruction" in
      def VLDA_3D_UPS_S64_D16 : AIE2_dmw_lda_ups_w2c_inst_lda <
          (outs mCMs:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins mSs:$shft, eP:$ptr, eDS:$mod),
          "vlda.3d.ups.s64.d16", "$dst, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mv_ups_base<1, 0>;
    }
    let Uses = [crSat] in {
      let DecoderMethod = "DecodeVLDA_3D_UPS_BMInstruction" in
      def VLDA_3D_UPS_S32_S16 : AIE2_dmw_lda_ups_w2b_inst_lda <
          (outs mBMs:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins mSs:$shft, eP:$ptr, eDS:$mod),
          "vlda.3d.ups.s32.s16", "$dst, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mv_ups_base<0, 1>;
      let DecoderMethod = "DecodeVLDA_3D_UPS_BMInstruction" in
      def VLDA_3D_UPS_S64_S32 : AIE2_dmw_lda_ups_w2b_inst_lda <
          (outs mBMs:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins mSs:$shft, eP:$ptr, eDS:$mod),
          "vlda.3d.ups.s64.s32", "$dst, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mv_ups_base<1, 1>;
      let DecoderMethod = "DecodeVLDA_3D_UPS_CMInstruction" in
      def VLDA_3D_UPS_S32_S8 : AIE2_dmw_lda_ups_w2c_inst_lda <
          (outs mCMs:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins mSs:$shft, eP:$ptr, eDS:$mod),
          "vlda.3d.ups.s32.s8", "$dst, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mv_ups_base<0, 1>;
      let DecoderMethod = "DecodeVLDA_3D_UPS_CMInstruction" in
      def VLDA_3D_UPS_S64_S16 : AIE2_dmw_lda_ups_w2c_inst_lda <
          (outs mCMs:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins mSs:$shft, eP:$ptr, eDS:$mod),
          "vlda.3d.ups.s64.s16", "$dst, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mv_ups_base<1, 1>;
    }
  }
} // let mayLoad = true

// VLD.CONV – Vector Loads with bfloat16 to float Conversion
let mayLoad = true in {
  let Itinerary = II_VLDA_CONV in {
    def VLDA_CONV_FP32_BF16_ag_idx_imm : AIE2_dmw_lda_ups_bf_inst_lda < (outs mBMs:$dst) , (ins eP:$ptr, imm3x32:$imm),
      "vlda.conv.fp32.bf16", "$dst, [$ptr, $imm]">, AIE2_ag_nospill__ag_idx_imm;
    def VLDA_CONV_FP32_BF16_ag_idx : AIE2_dmw_lda_ups_bf_inst_lda < (outs mBMs:$dst) , (ins eP:$ptr, eDJ:$dj),
      "vlda.conv.fp32.bf16", "$dst, [$ptr, $dj]">, AIE2_ag_nospill__ag_idx;
  }

  let Constraints = "$ptr_out = $ptr" in {
    let Itinerary = II_VLDA_POSTINC_CONV in {
      def VLDA_CONV_FP32_BF16_pstm_nrm : AIE2_dmw_lda_ups_bf_inst_lda < (outs mBMs:$dst, eP:$ptr_out) , (ins eP:$ptr, eM:$mod),
        "vlda.conv.fp32.bf16", "$dst, [$ptr], $mod">, AIE2_ag_nospill__ag_pstm_nrm;
      def VLDA_CONV_FP32_BF16_pstm_nrm_imm : AIE2_dmw_lda_ups_bf_inst_lda < (outs mBMs:$dst, eP:$ptr_out) , (ins eP:$ptr, imm4x32:$imm),
        "vlda.conv.fp32.bf16", "$dst, [$ptr], $imm">, AIE2_ag_nospill__ag_pstm_nrm_imm;
    }
    let Itinerary = II_VLDA_2D_CONV in
      let DecoderMethod = "DecodeVLDA_2D_CONV_BMInstruction" in
      def VLDA_2D_CONV_FP32_BF16 : AIE2_dmw_lda_ups_bf_inst_lda < (outs mBMs:$dst, eP:$ptr_out, eDC:$count_out) , (ins eP:$ptr, eD:$mod),
        "vlda.2d.conv.fp32.bf16", "$dst, [$ptr], $mod">, AIE2_ag_nospill__ag_pstm_2d;
    let Itinerary = II_VLDA_3D_CONV in
      let DecoderMethod = "DecodeVLDA_3D_CONV_BMInstruction" in
      def VLDA_3D_CONV_FP32_BF16 : AIE2_dmw_lda_ups_bf_inst_lda < (outs mBMs:$dst, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out) , (ins eP:$ptr, eDS:$mod),
        "vlda.3d.conv.fp32.bf16", "$dst, [$ptr], $mod">, AIE2_ag_nospill__ag_pstm_3d;
  } // let Constraints = "$ptr_out = $ptr"

} // let mayLoad = true

let mayStore = true in {
  let Itinerary = II_VST_SRS in
  let Defs = [srSRS_of] in {
    class VST_SRS_ag_idx_imm_mWdssrs<bits<1> md, bits<1> sgn, string opcodestr>
        : AIE2_dmw_sts_srs_inst_st< (outs ) , (ins eP:$ptr, imm3x32:$imm, mCMs:$src, mSs:$shft),
          opcodestr, "$src, $shft, [$ptr, $imm]">, AIE2_ag_nospill__ag_idx_imm, mWdssrs<md, sgn>;
    class VST_SRS_ag_idx_imm_mWlsrsl<bits<1> md, bits<1> sgn, string opcodestr>
        : AIE2_dmw_sts_srs_inst_st< (outs ) , (ins eP:$ptr, imm3x32:$imm, mBMs:$src, mSs:$shft),
          opcodestr, "$src, $shft, [$ptr, $imm]">, AIE2_ag_nospill__ag_idx_imm, mWlsrsl<md, sgn>;
    let Uses = [crSat, crRnd, crSRSSign] in {
      def VST_SRS_D8_S32_ag_idx_imm : VST_SRS_ag_idx_imm_mWdssrs</*md*/0, /*sgn*/0, "vst.srs.d8.s32">;
      def VST_SRS_D16_S64_ag_idx_imm : VST_SRS_ag_idx_imm_mWdssrs</*md*/1, /*sgn*/0, "vst.srs.d16.s64">;
      def VST_SRS_D16_S32_ag_idx_imm : VST_SRS_ag_idx_imm_mWlsrsl</*md*/0, /*sgn*/0, "vst.srs.d16.s32">;
      def VST_SRS_D32_S64_ag_idx_imm : VST_SRS_ag_idx_imm_mWlsrsl</*md*/1, /*sgn*/0, "vst.srs.d32.s64">;
    }
    let Uses = [crSat, crRnd] in {
      def VST_SRS_S8_S32_ag_idx_imm : VST_SRS_ag_idx_imm_mWdssrs</*md*/0, /*sgn*/1, "vst.srs.s8.s32">;
      def VST_SRS_S16_S64_ag_idx_imm : VST_SRS_ag_idx_imm_mWdssrs</*md*/1, /*sgn*/1, "vst.srs.s16.s64">;
      def VST_SRS_S16_S32_ag_idx_imm : VST_SRS_ag_idx_imm_mWlsrsl</*md*/0, /*sgn*/1, "vst.srs.s16.s32">;
      def VST_SRS_S32_S64_ag_idx_imm : VST_SRS_ag_idx_imm_mWlsrsl</*md*/1, /*sgn*/1, "vst.srs.s32.s64">;
    }
    class VST_SRS_ag_idx_mWdssrs<bits<1> md, bits<1> sgn, string opcodestr>
        : AIE2_dmw_sts_srs_inst_st< (outs ), (ins eP:$ptr, eDJ:$dj, mCMs:$src, mSs:$shft),
          opcodestr, "$src, $shft, [$ptr, $dj]">, AIE2_ag_nospill__ag_idx, mWdssrs<md, sgn>;
    class VST_SRS_ag_idx_mWlsrsl<bits<1> md, bits<1> sgn, string opcodestr>
        : AIE2_dmw_sts_srs_inst_st< (outs ), (ins eP:$ptr, eDJ:$dj, mBMs:$src, mSs:$shft),
          opcodestr, "$src, $shft, [$ptr, $dj]">, AIE2_ag_nospill__ag_idx, mWlsrsl<md, sgn>;
    let Uses = [crSat, crRnd, crSRSSign] in {
      def VST_SRS_D8_S32_ag_idx: VST_SRS_ag_idx_mWdssrs</*md*/0, /*sgn*/0, "vst.srs.d8.s32">;
      def VST_SRS_D16_S64_ag_idx: VST_SRS_ag_idx_mWdssrs</*md*/1, /*sgn*/0, "vst.srs.d16.s64">;
      def VST_SRS_D16_S32_ag_idx: VST_SRS_ag_idx_mWlsrsl</*md*/0, /*sgn*/0, "vst.srs.d16.s32">;
      def VST_SRS_D32_S64_ag_idx: VST_SRS_ag_idx_mWlsrsl</*md*/1, /*sgn*/0, "vst.srs.d32.s64">;
    }
    let Uses = [crSat, crRnd] in {
      def VST_SRS_S8_S32_ag_idx: VST_SRS_ag_idx_mWdssrs</*md*/0, /*sgn*/1, "vst.srs.s8.s32">;
      def VST_SRS_S16_S64_ag_idx: VST_SRS_ag_idx_mWdssrs</*md*/1, /*sgn*/1, "vst.srs.s16.s64">;
      def VST_SRS_S16_S32_ag_idx: VST_SRS_ag_idx_mWlsrsl</*md*/0, /*sgn*/1, "vst.srs.s16.s32">;
      def VST_SRS_S32_S64_ag_idx: VST_SRS_ag_idx_mWlsrsl</*md*/1, /*sgn*/1, "vst.srs.s32.s64">;
    }
  }
  let Itinerary = II_VST_POSTINC_SRS in
  let Constraints = "$ptr_out = $ptr" in
  let Defs = [srSRS_of] in {
    let Uses = [crSat, crRnd, crSRSSign] in {
      def VST_SRS_D8_S32_ag_pstm_nrm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, eM:$mod, mCMs:$src, mSs:$shft), "vst.srs.d8.s32", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_nrm, mWdssrs<0, 0>;
      def VST_SRS_D8_S32_ag_pstm_nrm_imm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, imm4x32:$imm, mCMs:$src, mSs:$shft), "vst.srs.d8.s32", "$src, $shft, [$ptr], $imm">,
          AIE2_ag_nospill__ag_pstm_nrm_imm, mWdssrs<0, 0>;
      def VST_SRS_D16_S64_ag_pstm_nrm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, eM:$mod, mCMs:$src, mSs:$shft), "vst.srs.d16.s64", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_nrm, mWdssrs<1, 0>;
      def VST_SRS_D16_S64_ag_pstm_nrm_imm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, imm4x32:$imm, mCMs:$src, mSs:$shft), "vst.srs.d16.s64", "$src, $shft, [$ptr], $imm">,
          AIE2_ag_nospill__ag_pstm_nrm_imm, mWdssrs<1, 0>;
    }
    let Uses = [crSat, crRnd, crSRSSign] in {
      def VST_SRS_D16_S32_ag_pstm_nrm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, eM:$mod, mBMs:$src, mSs:$shft), "vst.srs.d16.s32", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_nrm, mWlsrsl<0, 0>;
      def VST_SRS_D16_S32_ag_pstm_nrm_imm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, imm4x32:$imm, mBMs:$src, mSs:$shft), "vst.srs.d16.s32", "$src, $shft, [$ptr], $imm">,
          AIE2_ag_nospill__ag_pstm_nrm_imm, mWlsrsl<0, 0>;
      def VST_SRS_D32_S64_ag_pstm_nrm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, eM:$mod, mBMs:$src, mSs:$shft), "vst.srs.d32.s64", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_nrm, mWlsrsl<1, 0>;
      def VST_SRS_D32_S64_ag_pstm_nrm_imm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, imm4x32:$imm, mBMs:$src, mSs:$shft), "vst.srs.d32.s64", "$src, $shft, [$ptr], $imm">,
          AIE2_ag_nospill__ag_pstm_nrm_imm, mWlsrsl<1, 0>;
    }
    let Uses = [crSat, crRnd] in {
      def VST_SRS_S8_S32_ag_pstm_nrm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, eM:$mod, mCMs:$src, mSs:$shft), "vst.srs.s8.s32", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_nrm, mWdssrs<0, 1>;
      def VST_SRS_S8_S32_ag_pstm_nrm_imm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, imm4x32:$imm, mCMs:$src, mSs:$shft), "vst.srs.s8.s32", "$src, $shft, [$ptr], $imm">,
          AIE2_ag_nospill__ag_pstm_nrm_imm, mWdssrs<0, 1>;
      def VST_SRS_S16_S64_ag_pstm_nrm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, eM:$mod, mCMs:$src, mSs:$shft), "vst.srs.s16.s64", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_nrm, mWdssrs<1, 1>;
      def VST_SRS_S16_S64_ag_pstm_nrm_imm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, imm4x32:$imm, mCMs:$src, mSs:$shft), "vst.srs.s16.s64", "$src, $shft, [$ptr], $imm">,
          AIE2_ag_nospill__ag_pstm_nrm_imm, mWdssrs<1, 1>;
    }
    let Uses = [crSat, crRnd] in {
      def VST_SRS_S16_S32_ag_pstm_nrm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, eM:$mod, mBMs:$src, mSs:$shft), "vst.srs.s16.s32", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_nrm, mWlsrsl<0, 1>;
      def VST_SRS_S16_S32_ag_pstm_nrm_imm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, imm4x32:$imm, mBMs:$src, mSs:$shft), "vst.srs.s16.s32", "$src, $shft, [$ptr], $imm">,
          AIE2_ag_nospill__ag_pstm_nrm_imm, mWlsrsl<0, 1>;
      def VST_SRS_S32_S64_ag_pstm_nrm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, eM:$mod, mBMs:$src, mSs:$shft), "vst.srs.s32.s64", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_nrm, mWlsrsl<1, 1>;
      def VST_SRS_S32_S64_ag_pstm_nrm_imm : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out),
          (ins eP:$ptr, imm4x32:$imm, mBMs:$src, mSs:$shft), "vst.srs.s32.s64", "$src, $shft, [$ptr], $imm">,
          AIE2_ag_nospill__ag_pstm_nrm_imm, mWlsrsl<1, 1>;
    }
  }

  let Itinerary = II_VST_2D_SRS in
  let Constraints = "$ptr_out = $ptr" in
  let Defs = [srSRS_of] in {
    let DecoderMethod = "DecodeVST_2D_SRS_CMInstruction" in
    let Uses = [crSat, crRnd, crSRSSign] in {
      def VST_2D_SRS_D8_S32 : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out, eDC:$count_out),
          (ins eP:$ptr, eD:$mod, mCMs:$src, mSs:$shft), "vst.2d.srs.d8.s32", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_2d, mWdssrs<0, 0>;
      def VST_2D_SRS_D16_S64 : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out, eDC:$count_out),
          (ins eP:$ptr, eD:$mod, mCMs:$src, mSs:$shft), "vst.2d.srs.d16.s64", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_2d, mWdssrs<1, 0>;
    }
    let DecoderMethod = "DecodeVST_2D_SRS_BMInstruction" in
    let Uses = [crSat, crRnd, crSRSSign] in {
      def VST_2D_SRS_D16_S32 : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out, eDC:$count_out),
          (ins eP:$ptr, eD:$mod, mBMs:$src, mSs:$shft), "vst.2d.srs.d16.s32", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_2d, mWlsrsl<0, 0>;
      def VST_2D_SRS_D32_S64 : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out, eDC:$count_out),
          (ins eP:$ptr, eD:$mod, mBMs:$src, mSs:$shft), "vst.2d.srs.d32.s64", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_2d, mWlsrsl<1, 0>;
    }
    let DecoderMethod = "DecodeVST_2D_SRS_CMInstruction" in
    let Uses = [crSat, crRnd] in {
      def VST_2D_SRS_S8_S32 : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out, eDC:$count_out),
          (ins eP:$ptr, eD:$mod, mCMs:$src, mSs:$shft), "vst.2d.srs.s8.s32", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_2d, mWdssrs<0, 1>;
      def VST_2D_SRS_S16_S64 : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out, eDC:$count_out),
          (ins eP:$ptr, eD:$mod, mCMs:$src, mSs:$shft), "vst.2d.srs.s16.s64", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_2d, mWdssrs<1, 1>;
    }
    let DecoderMethod = "DecodeVST_2D_SRS_BMInstruction" in
    let Uses = [crSat, crRnd] in {
      def VST_2D_SRS_S16_S32 : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out, eDC:$count_out),
          (ins eP:$ptr, eD:$mod, mBMs:$src, mSs:$shft), "vst.2d.srs.s16.s32", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_2d, mWlsrsl<0, 1>;
      def VST_2D_SRS_S32_S64 : AIE2_dmw_sts_srs_inst_st< (outs eP:$ptr_out, eDC:$count_out),
          (ins eP:$ptr, eD:$mod, mBMs:$src, mSs:$shft), "vst.2d.srs.s32.s64", "$src, $shft, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_2d, mWlsrsl<1, 1>;
    }
  }

  let Itinerary = II_VST_3D_SRS in
  let Constraints = "$ptr_out = $ptr" in
  let Defs = [srSRS_of] in {
    let DecoderMethod = "DecodeVST_3D_SRS_CMInstruction" in
    let Uses = [crSat, crRnd, crSRSSign] in {
      def VST_3D_SRS_D8_S32 : AIE2_dmw_sts_srs_inst_st< (
          outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins eP:$ptr, eDS:$mod, mCMs:$src, mSs:$shft),
          "vst.3d.srs.d8.s32", "$src, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mWdssrs<0, 0>;
      def VST_3D_SRS_D16_S64 : AIE2_dmw_sts_srs_inst_st<
          (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins eP:$ptr, eDS:$mod, mCMs:$src, mSs:$shft),
          "vst.3d.srs.d16.s64", "$src, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mWdssrs<1, 0>;
    }
    let DecoderMethod = "DecodeVST_3D_SRS_BMInstruction" in
    let Uses = [crSat, crRnd, crSRSSign] in {
      def VST_3D_SRS_D16_S32 : AIE2_dmw_sts_srs_inst_st<
          (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins eP:$ptr, eDS:$mod, mBMs:$src, mSs:$shft),
          "vst.3d.srs.d16.s32", "$src, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mWlsrsl<0, 0>;
      def VST_3D_SRS_D32_S64 : AIE2_dmw_sts_srs_inst_st<
          (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins eP:$ptr, eDS:$mod, mBMs:$src, mSs:$shft),
          "vst.3d.srs.d32.s64", "$src, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mWlsrsl<1, 0>;
    }
    let DecoderMethod = "DecodeVST_3D_SRS_CMInstruction" in
    let Uses = [crSat, crRnd] in {
      def VST_3D_SRS_S8_S32 : AIE2_dmw_sts_srs_inst_st< (
          outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins eP:$ptr, eDS:$mod, mCMs:$src, mSs:$shft),
          "vst.3d.srs.s8.s32", "$src, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mWdssrs<0, 1>;
      def VST_3D_SRS_S16_S64 : AIE2_dmw_sts_srs_inst_st<
          (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins eP:$ptr, eDS:$mod, mCMs:$src, mSs:$shft),
          "vst.3d.srs.s16.s64", "$src, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mWdssrs<1, 1>;
    }
    let DecoderMethod = "DecodeVST_3D_SRS_BMInstruction" in
    let Uses = [crSat, crRnd] in {
      def VST_3D_SRS_S16_S32 : AIE2_dmw_sts_srs_inst_st<
          (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins eP:$ptr, eDS:$mod, mBMs:$src, mSs:$shft),
          "vst.3d.srs.s16.s32", "$src, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mWlsrsl<0, 1>;
      def VST_3D_SRS_S32_S64 : AIE2_dmw_sts_srs_inst_st<
          (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins eP:$ptr, eDS:$mod, mBMs:$src, mSs:$shft),
          "vst.3d.srs.s32.s64", "$src, $shft, [$ptr], $mod">,
        AIE2_ag_nospill__ag_pstm_3d, mWlsrsl<1, 1>;
    }
  }
} // let mayStore = true

// 3.6 VLD.COMPR - Vector Loads with decompression
let mayLoad = true, Constraints = "$ptr_out = $ptr" in {
  class VLD_COMPR_INIT<bits<2> op, bits<2> smode, string opcodestr> :
    AIE2_dmw_ldb_compr_init_inst_ldb<op, smode, 
      (outs eP:$ptr_out), 
      (ins eP:$ptr), opcodestr,"[$ptr]">;
  let Itinerary = II_VLDB_COMPR_RESET, Defs = [DP], Uses = [DP] in {
      def VLDB_COMPR_RESET : VLD_COMPR_INIT<0b10, 0b00, "vldb.compr.reset">;
  }
  let Itinerary = II_VLDB_COMPR_FILL, Uses = [DP] in {
      def VLDB_COMPR_FILL : VLD_COMPR_INIT<0b00, 0b00, "vldb.compr.fill">;
  }

  class VLD_COMPR_OP<bits<2> op, bits<2> smode, string opcodestr> :
    AIE2_dmw_ldb_compr_op_inst_ldb<op, smode,
      (outs eP:$ptr_out, mWb:$dst),
      (ins eP:$ptr), opcodestr, "$dst, [$ptr]">;
  let Itinerary = II_VLDB_COMPR_PEEK, Defs = [srCompr_uf], Uses = [DP] in {
    def VLDB_COMPR_PEEK : VLD_COMPR_OP<0b11, 0b00, "vldb.compr.peek">;
  }
  let Itinerary = II_VLDB_COMPR_POP, Defs = [srCompr_uf, DP], Uses = [DP] in {
    def VLDB_COMPR_POP : VLD_COMPR_OP<0b01, 0b00, "vldb.compr.pop">;
  }
} // let mayLoad = true, Constraints = "$ptr_out = $ptr"

// 3.7 VLD.SPARSE - Vector Loads with sparse decompression
let mayLoad = true, Constraints = "$ptr_out = $ptr", Uses = [DP] in {
  class VLD_SPARSE_INIT<bits<2> op, bits<2> smode, string opcodestr> :
    AIE2_dmw_ldb_sparse_init_inst_ldb<op, smode, 
      (outs eP:$ptr_out), 
      (ins eP:$ptr), opcodestr,"[$ptr]">;

  let Defs = [srSparse_of, DP] in
    let Itinerary = II_VLDB_SPARSE_RESET in {
      def VLDB_SPARSE_RESET_4 : VLD_SPARSE_INIT<0b10, 0b01, "vldb.sparse.reset.4">;
      def VLDB_SPARSE_RESET_8 : VLD_SPARSE_INIT<0b10, 0b10, "vldb.sparse.reset.8">;
      def VLDB_SPARSE_RESET_16 : VLD_SPARSE_INIT<0b10, 0b11, "vldb.sparse.reset.16">;
  } //  let Defs = [srSparse_of, DP]
  let Defs = [srSparse_of] in
    let Itinerary = II_VLDB_SPARSE_FILL in {
      def VLDB_SPARSE_FILL_4 : VLD_SPARSE_INIT<0b00, 0b01, "vldb.sparse.fill.4">;
      def VLDB_SPARSE_FILL_8 : VLD_SPARSE_INIT<0b00, 0b10, "vldb.sparse.fill.8">;
      def VLDB_SPARSE_FILL_16 : VLD_SPARSE_INIT<0b00, 0b11, "vldb.sparse.fill.16">;
  } //  let Defs = [srSparse_of]

  let Defs = [srSparse_of, srCompr_uf] in
    let Itinerary = II_VLDB_SPARSE_PEEK in {
        class VLD_SPARSE_OP<bits<2> op, bits<2> smode, string opcodestr> :
          AIE2_dmw_ldb_sparse_op_inst_ldb<op, smode,
            (outs eP:$ptr_out, OP_mQXHLb:$dst),
            (ins eP:$ptr), opcodestr,"$dst, [$ptr]">;
        def VLDB_SPARSE_PEEK_4 : VLD_SPARSE_OP<0b11, 0b01, "vldb.sparse.peek.4">;
        def VLDB_SPARSE_PEEK_8 : VLD_SPARSE_OP<0b11, 0b10, "vldb.sparse.peek.8">;
        def VLDB_SPARSE_PEEK_16 : VLD_SPARSE_OP<0b11, 0b11, "vldb.sparse.peek.16">;
    } // let Defs = [srSparse_of, srCompr_uf]

  let Defs = [srSparse_of, srCompr_uf, DP] in
    let Itinerary = II_VLDB_SPARSE_POP in {
      def VLDB_SPARSE_POP_4 : VLD_SPARSE_OP<0b01, 0b01, "vldb.sparse.pop.4">;
      def VLDB_SPARSE_POP_8 : VLD_SPARSE_OP<0b01, 0b10, "vldb.sparse.pop.8">;
      def VLDB_SPARSE_POP_16 : VLD_SPARSE_OP<0b01, 0b11, "vldb.sparse.pop.16">;
    } // let Defs = [srSparse_of, srCompr_uf, DP]

} // let mayLoad = true, Constraints = "$ptr_out = $ptr", Uses = [DP]

let mayStore = true in {
  let Defs = [srF2FFlags] in {
    let Uses = [crRnd, crF2FMask] in {
      let Itinerary = II_VST_CONV in {
        def VST_CONV_BF16_FP32_ag_idx_imm :
        AIE2_dmw_sts_srs_bf_inst_st < (outs ) , (ins eP:$ptr, imm3x32:$imm, mBMs:$src),
                              "vst.conv.bf16.fp32", "$src, [$ptr, $imm]">,
        AIE2_ag_nospill__ag_idx_imm;

        def VST_CONV_BF16_FP32_ag_idx :
        AIE2_dmw_sts_srs_bf_inst_st < (outs ) , (ins eP:$ptr, eDJ:$dj, mBMs:$src),
                              "vst.conv.bf16.fp32", "$src, [$ptr, $dj]">,
        AIE2_ag_nospill__ag_idx;
      }

      let Itinerary = II_VST_CONV_POSTINC in
      let Constraints = "$ptr_out = $ptr" in {
          def VST_CONV_BF16_FP32_ag_pstm_nrm : AIE2_dmw_sts_srs_bf_inst_st< (outs eP:$ptr_out),
              (ins eP:$ptr, eM:$mod, mBMs:$src), "vst.conv.bf16.fp32", "$src, [$ptr], $mod">,
              AIE2_ag_nospill__ag_pstm_nrm;

          def VST_CONV_BF16_FP32_ag_pstm_nrm_imm : AIE2_dmw_sts_srs_bf_inst_st< (outs eP:$ptr_out),
              (ins eP:$ptr, imm4x32:$imm, mBMs:$src), "vst.conv.bf16.fp32", "$src, [$ptr], $imm">,
              AIE2_ag_nospill__ag_pstm_nrm_imm;
      }

      let Itinerary = II_VST_2D_CONV in
      let Constraints = "$ptr_out = $ptr" in
      let DecoderMethod = "DecodeVST_2D_CONV_BMInstruction" in {
          def VST_CONV_2D_BF16_FP32 : AIE2_dmw_sts_srs_bf_inst_st< (outs eP:$ptr_out, eDC:$count_out),
              (ins eP:$ptr, eD:$mod, mBMs:$src), "vst.2d.conv.bf16.fp32", "$src, [$ptr], $mod">,
              AIE2_ag_nospill__ag_pstm_2d;
      }

      let Itinerary = II_VST_3D_CONV in
      let Constraints = "$ptr_out = $ptr" in
      let DecoderMethod = "DecodeVST_3D_CONV_BMInstruction" in {
          def VST_CONV_3D_BF16_FP32 : AIE2_dmw_sts_srs_bf_inst_st< (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
              (ins eP:$ptr, eDS:$mod, mBMs:$src), "vst.3d.conv.bf16.fp32", "$src, [$ptr], $mod">,
              AIE2_ag_nospill__ag_pstm_3d;
      }
    } // let Uses = [crRnd, crF2FMask]
  } // let Defs = [srF2FFlags]
} // let mayStore = true

let mayStore = true in {
  let Itinerary = II_VST_PACK in {
    class VST_PACK_ag_idx_imm_mv_pack_src<bits<1> sgn, bits<1> sz, string opcodestr>
        : AIE2_dmw_sts_pack_inst_st< (outs ) , (ins eP:$ptr, imm3x32:$imm, mXs:$src),
          opcodestr, "$src, [$ptr, $imm]">, AIE2_ag_nospill__ag_idx_imm, mv_pack_src<sgn, sz>;
    let Uses = [crSat, crPackSign] in {
      def VST_PACK_D4_D8_ag_idx_imm : VST_PACK_ag_idx_imm_mv_pack_src</*sgn*/0, /*sz*/0, "vst.pack.d4.d8">;
      def VST_PACK_D8_D16_ag_idx_imm : VST_PACK_ag_idx_imm_mv_pack_src</*sgn*/0, /*sz*/1, "vst.pack.d8.d16">;
    }
    let Uses = [crSat] in {
      def VST_PACK_S4_S8_ag_idx_imm : VST_PACK_ag_idx_imm_mv_pack_src</*sgn*/1, /*sz*/0, "vst.pack.s4.s8">;
      def VST_PACK_S8_S16_ag_idx_imm : VST_PACK_ag_idx_imm_mv_pack_src</*sgn*/1, /*sz*/1, "vst.pack.s8.s16">;
    }

    class VST_PACK_ag_idx_mv_pack_src<bits<1> sgn, bits<1> sz, string opcodestr>
        : AIE2_dmw_sts_pack_inst_st< (outs ) , (ins eP:$ptr, eDJ:$dj, mXs:$src),
          opcodestr, "$src, [$ptr, $dj]">, AIE2_ag_nospill__ag_idx, mv_pack_src<sgn, sz>;
    let Uses = [crSat, crPackSign] in {
      def VST_PACK_D4_D8_ag_idx : VST_PACK_ag_idx_mv_pack_src</*sgn*/0, /*sz*/0, "vst.pack.d4.d8">;
      def VST_PACK_D8_D16_ag_idx : VST_PACK_ag_idx_mv_pack_src</*sgn*/0, /*sz*/1, "vst.pack.d8.d16">;
    }
    let Uses = [crSat] in {
      def VST_PACK_S4_S8_ag_idx : VST_PACK_ag_idx_mv_pack_src</*sgn*/1, /*sz*/0, "vst.pack.s4.s8">;
      def VST_PACK_S8_S16_ag_idx : VST_PACK_ag_idx_mv_pack_src</*sgn*/1, /*sz*/1, "vst.pack.s8.s16">;
    }
  }


  let Itinerary = II_VST_PACK_POSTINC, Constraints = "$ptr_out = $ptr" in {
    class VST_PACK_ag_pstm_nrm_mv_pack_src<bits<1> sgn, bits<1> sz, string opcodestr>
        : AIE2_dmw_sts_pack_inst_st<
            (outs eP:$ptr_out),
            (ins eP:$ptr, eM:$mod, mXs:$src),
            opcodestr, "$src, [$ptr], $mod">,
          AIE2_ag_nospill__ag_pstm_nrm, mv_pack_src<sgn, sz>;
      let Uses = [crSat, crPackSign] in {
        def VST_PACK_D4_D8_ag_pstm_nrm :
          VST_PACK_ag_pstm_nrm_mv_pack_src</*sgn*/0, /*sz*/0, "vst.pack.d4.d8">;
        def VST_PACK_D8_D16_ag_pstm_nrm :
          VST_PACK_ag_pstm_nrm_mv_pack_src</*sgn*/0, /*sz*/1, "vst.pack.d8.d16">;
      }
      let Uses = [crSat] in {
        def VST_PACK_S4_S8_ag_pstm_nrm :
          VST_PACK_ag_pstm_nrm_mv_pack_src</*sgn*/1, /*sz*/0, "vst.pack.s4.s8">;
        def VST_PACK_S8_S16_ag_pstm_nrm :
          VST_PACK_ag_pstm_nrm_mv_pack_src</*sgn*/1, /*sz*/1, "vst.pack.s8.s16">;
      }
      class VST_PACK_ag_pstm_nrm_imm_mv_pack_src<bits<1> sgn, bits<1> sz, string opcodestr>
          : AIE2_dmw_sts_pack_inst_st< (outs eP:$ptr_out) , (ins eP:$ptr, imm4x32:$imm, mXs:$src),
            opcodestr, "$src, [$ptr], $imm">, AIE2_ag_nospill__ag_pstm_nrm_imm, mv_pack_src<sgn, sz>;
      let Uses = [crSat, crPackSign] in {
        def VST_PACK_D4_D8_ag_pstm_nrm_imm :
          VST_PACK_ag_pstm_nrm_imm_mv_pack_src</*sgn*/0, /*sz*/0, "vst.pack.d4.d8">;
        def VST_PACK_D8_D16_ag_pstm_nrm_imm :
          VST_PACK_ag_pstm_nrm_imm_mv_pack_src</*sgn*/0, /*sz*/1, "vst.pack.d8.d16">;
      }
      let Uses = [crSat] in {
        def VST_PACK_S4_S8_ag_pstm_nrm_imm :
          VST_PACK_ag_pstm_nrm_imm_mv_pack_src</*sgn*/1, /*sz*/0, "vst.pack.s4.s8">;
        def VST_PACK_S8_S16_ag_pstm_nrm_imm :
          VST_PACK_ag_pstm_nrm_imm_mv_pack_src</*sgn*/1, /*sz*/1, "vst.pack.s8.s16">;
      }
    }
  let Itinerary = II_VST_2D_PACK, Constraints = "$ptr_out = $ptr" in
    let DecoderMethod = "DecodeVST_PACK_2DInstruction" in {
      class VST_2D_PACK_ag_pstm_mv_pack_src<bits<1> sgn, bits<1> sz, string opcodestr>
          : AIE2_dmw_sts_pack_inst_st<
              (outs eP:$ptr_out, eDC:$count_out),
              (ins eP:$ptr, eD:$mod, mXs:$src),
              opcodestr, "$src, [$ptr], $mod">,
            AIE2_ag_nospill__ag_pstm_2d, mv_pack_src<sgn, sz>;
      let Uses = [crSat, crPackSign] in {
        def VST_2D_PACK_D4_D8 :
          VST_2D_PACK_ag_pstm_mv_pack_src</*sgn*/0, /*sz*/0, "vst.2d.pack.d4.d8">;
        def VST_2D_PACK_D8_D16 :
          VST_2D_PACK_ag_pstm_mv_pack_src</*sgn*/0, /*sz*/1, "vst.2d.pack.d8.d16">;
      }
      let Uses = [crSat] in {
        def VST_2D_PACK_S4_S8 :
          VST_2D_PACK_ag_pstm_mv_pack_src</*sgn*/1, /*sz*/0, "vst.2d.pack.s4.s8">;
        def VST_2D_PACK_S8_S16 :
          VST_2D_PACK_ag_pstm_mv_pack_src</*sgn*/1, /*sz*/1, "vst.2d.pack.s8.s16">;
      }
    }
  let Itinerary = II_VST_3D_PACK, Constraints = "$ptr_out = $ptr" in
    let DecoderMethod = "DecodeVST_PACK_3DInstruction" in {
      class VST_3D_PACK_ag_pstm_mv_pack_src<bits<1> sgn, bits<1> sz, string opcodestr>
          : AIE2_dmw_sts_pack_inst_st<
              (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
              (ins eP:$ptr, eDS:$mod, mXs:$src),
              opcodestr, "$src, [$ptr], $mod">,
            AIE2_ag_nospill__ag_pstm_3d, mv_pack_src<sgn, sz>;
      let Uses = [crSat, crPackSign] in {
        def VST_3D_PACK_D4_D8 :
          VST_3D_PACK_ag_pstm_mv_pack_src</*sgn*/0, /*sz*/0, "vst.3d.pack.d4.d8">;
        def VST_3D_PACK_D8_D16 :
          VST_3D_PACK_ag_pstm_mv_pack_src</*sgn*/0, /*sz*/1, "vst.3d.pack.d8.d16">;
      }
      let Uses = [crSat] in {
        def VST_3D_PACK_S4_S8 :
          VST_3D_PACK_ag_pstm_mv_pack_src</*sgn*/1, /*sz*/0, "vst.3d.pack.s4.s8">;
        def VST_3D_PACK_S8_S16 :
          VST_3D_PACK_ag_pstm_mv_pack_src</*sgn*/1, /*sz*/1, "vst.3d.pack.s8.s16">;
      }
    }
} // let mayStore = true

let mayLoad = true in {
  let Itinerary = II_VLDB_4X in {
    class VLDB_dmv_ldb_4x<bits<2> sz, bits<1> off, string opcodestr> :
          AIE2_dmv_ldb_4x_inst_ldb<sz, off, (outs mWs:$dst), (ins mWb:$src), opcodestr, "$dst, $src">;
    def VLDB_4x16_LO : VLDB_dmv_ldb_4x<0b00, 0b0, "vldb.4x16.lo">;
    def VLDB_4x16_HI : VLDB_dmv_ldb_4x<0b00, 0b1, "vldb.4x16.hi">;
    def VLDB_4x32_LO : VLDB_dmv_ldb_4x<0b01, 0b0, "vldb.4x32.lo">;
    def VLDB_4x32_HI : VLDB_dmv_ldb_4x<0b01, 0b1, "vldb.4x32.hi">;
    def VLDB_4x64_LO : VLDB_dmv_ldb_4x<0b10, 0b0, "vldb.4x64.lo">;
    def VLDB_4x64_HI : VLDB_dmv_ldb_4x<0b10, 0b1, "vldb.4x64.hi">;
  }
} // mayLoad = true

} //let hasSideEffects = false, mayLoad = false, mayStore = false
