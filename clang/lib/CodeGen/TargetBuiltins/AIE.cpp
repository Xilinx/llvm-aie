//===---- AIE.cpp - Emit LLVM Code for builtins ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its
// affiliates
//
//===----------------------------------------------------------------------===//
//
// This contains code to emit Builtin calls as LLVM code.
//
//===----------------------------------------------------------------------===//

#include "ABIInfo.h"
#include "CGBuiltin.h"
#include "CodeGenFunction.h"
#include "CodeGenModule.h"
#include "TargetInfo.h"
#include "clang/Basic/TargetBuiltins.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IntrinsicsAIE.h"
#include "llvm/IR/IntrinsicsAIE2.h"
#include "llvm/IR/IntrinsicsAIE2P.h"
#include "llvm/IR/IntrinsicsAIE2PS.h"

using namespace clang;
using namespace CodeGen;
using namespace llvm;

static llvm::Intrinsic::ID getAIE1IntrinsicFunction(unsigned BuiltinID) {
  switch (BuiltinID) {
  case AIE::BI__builtin_aie_ctrl_packet_header:
    return Intrinsic::aie_ctrl_packet_header;
  default:
    break;
  }
  return Intrinsic::not_intrinsic;
}

static llvm::Intrinsic::ID getAIE2IntrinsicFunction(unsigned BuiltinID) {
  switch (BuiltinID) {
  case AIE::BI__builtin_aiev2_vabs_gtz8:
    return Intrinsic::aie2_vabs_gtz8;
  case AIE::BI__builtin_aiev2_vabs_gtz16:
    return Intrinsic::aie2_vabs_gtz16;
  case AIE::BI__builtin_aiev2_vabs_gtz32:
    return Intrinsic::aie2_vabs_gtz32;
  case AIE::BI__builtin_aiev2_vbneg_ltz8:
    return Intrinsic::aie2_vbneg_ltz8;
  case AIE::BI__builtin_aiev2_vbneg_ltz16:
    return Intrinsic::aie2_vbneg_ltz16;
  case AIE::BI__builtin_aiev2_vbneg_ltz32:
    return Intrinsic::aie2_vbneg_ltz32;
  case AIE::BI__builtin_aiev2_vmaxdiff_lt8:
    return Intrinsic::aie2_vmaxdiff_lt8;
  case AIE::BI__builtin_aiev2_vmaxdiff_lt16:
    return Intrinsic::aie2_vmaxdiff_lt16;
  case AIE::BI__builtin_aiev2_vmaxdiff_lt32:
    return Intrinsic::aie2_vmaxdiff_lt32;
  case AIE::BI__builtin_aiev2_vmax_lt8:
    return Intrinsic::aie2_vmax_lt8;
  case AIE::BI__builtin_aiev2_vmax_lt16:
    return Intrinsic::aie2_vmax_lt16;
  case AIE::BI__builtin_aiev2_vmax_lt32:
    return Intrinsic::aie2_vmax_lt32;
  case AIE::BI__builtin_aiev2_vmax_ltbf16:
    return Intrinsic::aie2_vmax_ltbf16;
  case AIE::BI__builtin_aiev2_vmin_ge8:
    return Intrinsic::aie2_vmin_ge8;
  case AIE::BI__builtin_aiev2_vmin_ge16:
    return Intrinsic::aie2_vmin_ge16;
  case AIE::BI__builtin_aiev2_vmin_ge32:
    return Intrinsic::aie2_vmin_ge32;
  case AIE::BI__builtin_aiev2_vmin_gebf16:
    return Intrinsic::aie2_vmin_gebf16;
  case AIE::BI__builtin_aiev2_vneg_gtz8:
    return Intrinsic::aie2_vneg_gtz8;
  case AIE::BI__builtin_aiev2_vneg_gtz16:
    return Intrinsic::aie2_vneg_gtz16;
  case AIE::BI__builtin_aiev2_vneg_gtz32:
    return Intrinsic::aie2_vneg_gtz32;
  case AIE::BI__builtin_aiev2_vsub_ge8:
    return Intrinsic::aie2_vsub_ge8;
  case AIE::BI__builtin_aiev2_vsub_ge16:
    return Intrinsic::aie2_vsub_ge16;
  case AIE::BI__builtin_aiev2_vsub_ge32:
    return Intrinsic::aie2_vsub_ge32;
  case AIE::BI__builtin_aiev2_vsub_lt8:
    return Intrinsic::aie2_vsub_lt8;
  case AIE::BI__builtin_aiev2_vsub_lt16:
    return Intrinsic::aie2_vsub_lt16;
  case AIE::BI__builtin_aiev2_vsub_lt32:
    return Intrinsic::aie2_vsub_lt32;
  case AIE::BI__builtin_aiev2_add_2d:
    return Intrinsic::aie2_add_2d;
  case AIE::BI__builtin_aiev2_add_3d:
    return Intrinsic::aie2_add_3d;
  case AIE::BI__builtin_aiev2_get_ss:
    return Intrinsic::aie2_get_ss;
  case AIE::BI__builtin_aiev2_get_ss_nb:
    return Intrinsic::aie2_get_ss_nb;
  case AIE::BI__builtin_aiev2_put_ms_nb:
    return Intrinsic::aie2_put_ms_nb;
  case AIE::BI__builtin_aiev2_put_ms_nb_packet_header:
    return Intrinsic::aie2_put_ms_nb_packet_header;
  case AIE::BI__builtin_aiev2_put_ms_nb_ctrl_packet_header:
    return Intrinsic::aie2_put_ms_nb_ctrl_packet_header;
  case AIE::BI__builtin_aiev2_divstep:
    return Intrinsic::aie2_divs;
  case AIE::BI__builtin_aiev2_sparse_pop_16_and_get_pointer:
    return Intrinsic::aie2_sparse_pop_16_and_get_pointer;
  case AIE::BI__builtin_aiev2_sparse_pop_16_set_lo:
    return Intrinsic::aie2_sparse_pop_16_set_lo;
  case AIE::BI__builtin_aiev2_sparse_pop_16_insert_hi:
    return Intrinsic::aie2_sparse_pop_16_insert_hi;
  case AIE::BI__builtin_aiev2_sparse_peek_16_and_get_pointer:
    return Intrinsic::aie2_sparse_peek_16_and_get_pointer;
  case AIE::BI__builtin_aiev2_sparse_peek_16_set_lo:
    return Intrinsic::aie2_sparse_peek_16_set_lo;
  case AIE::BI__builtin_aiev2_sparse_peek_16_insert_hi:
    return Intrinsic::aie2_sparse_peek_16_insert_hi;
  case AIE::BI__builtin_aiev2_sparse_pop_8_and_get_pointer:
    return Intrinsic::aie2_sparse_pop_8_and_get_pointer;
  case AIE::BI__builtin_aiev2_sparse_pop_8_set_lo:
    return Intrinsic::aie2_sparse_pop_8_set_lo;
  case AIE::BI__builtin_aiev2_sparse_pop_8_insert_hi:
    return Intrinsic::aie2_sparse_pop_8_insert_hi;
  case AIE::BI__builtin_aiev2_sparse_peek_8_and_get_pointer:
    return Intrinsic::aie2_sparse_peek_8_and_get_pointer;
  case AIE::BI__builtin_aiev2_sparse_peek_8_set_lo:
    return Intrinsic::aie2_sparse_peek_8_set_lo;
  case AIE::BI__builtin_aiev2_sparse_peek_8_insert_hi:
    return Intrinsic::aie2_sparse_peek_8_insert_hi;
  case AIE::BI__builtin_aiev2_sparse_pop_4_and_get_pointer:
    return Intrinsic::aie2_sparse_pop_4_and_get_pointer;
  case AIE::BI__builtin_aiev2_sparse_pop_4_set_lo:
    return Intrinsic::aie2_sparse_pop_4_set_lo;
  case AIE::BI__builtin_aiev2_sparse_pop_4_insert_hi:
    return Intrinsic::aie2_sparse_pop_4_insert_hi;
  case AIE::BI__builtin_aiev2_sparse_peek_4_and_get_pointer:
    return Intrinsic::aie2_sparse_peek_4_and_get_pointer;
  case AIE::BI__builtin_aiev2_sparse_peek_4_set_lo:
    return Intrinsic::aie2_sparse_peek_4_set_lo;
  case AIE::BI__builtin_aiev2_sparse_peek_4_insert_hi:
    return Intrinsic::aie2_sparse_peek_4_insert_hi;
  case AIE::BI__builtin_aiev2_sparse_reset_16:
    return Intrinsic::aie2_sparse_reset_16;
  case AIE::BI__builtin_aiev2_sparse_reset_16_and_get_pointer:
    return Intrinsic::aie2_sparse_reset_16_and_get_pointer;
  case AIE::BI__builtin_aiev2_sparse_reset_8:
    return Intrinsic::aie2_sparse_reset_8;
  case AIE::BI__builtin_aiev2_sparse_reset_8_and_get_pointer:
    return Intrinsic::aie2_sparse_reset_8_and_get_pointer;
  case AIE::BI__builtin_aiev2_sparse_reset_4:
    return Intrinsic::aie2_sparse_reset_4;
  case AIE::BI__builtin_aiev2_sparse_reset_4_and_get_pointer:
    return Intrinsic::aie2_sparse_reset_4_and_get_pointer;
  case AIE::BI__builtin_aiev2_sparse_fill_16:
    return Intrinsic::aie2_sparse_fill_16;
  case AIE::BI__builtin_aiev2_sparse_fill_16_and_get_pointer:
    return Intrinsic::aie2_sparse_fill_16_and_get_pointer;
  case AIE::BI__builtin_aiev2_sparse_fill_8:
    return Intrinsic::aie2_sparse_fill_8;
  case AIE::BI__builtin_aiev2_sparse_fill_8_and_get_pointer:
    return Intrinsic::aie2_sparse_fill_8_and_get_pointer;
  case AIE::BI__builtin_aiev2_sparse_fill_4:
    return Intrinsic::aie2_sparse_fill_4;
  case AIE::BI__builtin_aiev2_sparse_fill_4_and_get_pointer:
    return Intrinsic::aie2_sparse_fill_4_and_get_pointer;
  case AIE::BI__builtin_aiev2_sparse_pop_16_bfloat_and_get_pointer:
    return Intrinsic::aie2_sparse_pop_16_bfloat_and_get_pointer;
  case AIE::BI__builtin_aiev2_sparse_pop_16_bfloat_set_lo:
    return Intrinsic::aie2_sparse_pop_16_bfloat_set_lo;
  case AIE::BI__builtin_aiev2_sparse_pop_16_bfloat_insert_hi:
    return Intrinsic::aie2_sparse_pop_16_bfloat_insert_hi;
  case AIE::BI__builtin_aiev2_sparse_peek_16_bfloat_and_get_pointer:
    return Intrinsic::aie2_sparse_peek_16_bfloat_and_get_pointer;
  case AIE::BI__builtin_aiev2_sparse_peek_16_bfloat_set_lo:
    return Intrinsic::aie2_sparse_peek_16_bfloat_set_lo;
  case AIE::BI__builtin_aiev2_sparse_peek_16_bfloat_insert_hi:
    return Intrinsic::aie2_sparse_peek_16_bfloat_insert_hi;
  default:
    break;
  }
  return Intrinsic::not_intrinsic;
}

static llvm::Intrinsic::ID getAIE2PIntrinsicFunction(unsigned BuiltinID) {
  switch (BuiltinID) {
  case AIE::BI__builtin_aie2p_add_2d:
    return Intrinsic::aie2p_add_2d;
  case AIE::BI__builtin_aie2p_add_3d:
    return Intrinsic::aie2p_add_3d;
  case AIE::BI__builtin_aie2p_get_ss:
    return Intrinsic::aie2p_get_ss;
  case AIE::BI__builtin_aie2p_get_ss_nb:
    return Intrinsic::aie2p_get_ss_nb;
  case AIE::BI__builtin_aie2p_put_ms_nb:
    return Intrinsic::aie2p_put_ms_nb;
  case AIE::BI__builtin_aie2p_scd_expand_ACC1024_incr:
    return Intrinsic::aie2p_scd_expand_ACC1024_incr;
  case AIE::BI__builtin_aie2p_scd_expand_ACC2048_incr:
    return Intrinsic::aie2p_scd_expand_ACC2048_incr;
  case AIE::BI__builtin_aie2p_vabs_gtz8:
    return Intrinsic::aie2p_vabs_gtz8;
  case AIE::BI__builtin_aie2p_vabs_gtz16:
    return Intrinsic::aie2p_vabs_gtz16;
  case AIE::BI__builtin_aie2p_vabs_gtz32:
    return Intrinsic::aie2p_vabs_gtz32;
  case AIE::BI__builtin_aie2p_vbneg_ltz8:
    return Intrinsic::aie2p_vbneg_ltz8;
  case AIE::BI__builtin_aie2p_vbneg_ltz16:
    return Intrinsic::aie2p_vbneg_ltz16;
  case AIE::BI__builtin_aie2p_vbneg_ltz32:
    return Intrinsic::aie2p_vbneg_ltz32;
  case AIE::BI__builtin_aie2p_vmaxdiff_lt8:
    return Intrinsic::aie2p_vmaxdiff_lt8;
  case AIE::BI__builtin_aie2p_vmaxdiff_lt16:
    return Intrinsic::aie2p_vmaxdiff_lt16;
  case AIE::BI__builtin_aie2p_vmaxdiff_lt32:
    return Intrinsic::aie2p_vmaxdiff_lt32;
  case AIE::BI__builtin_aie2p_vmax_lt8:
    return Intrinsic::aie2p_vmax_lt8;
  case AIE::BI__builtin_aie2p_vmax_lt16:
    return Intrinsic::aie2p_vmax_lt16;
  case AIE::BI__builtin_aie2p_vmax_lt32:
    return Intrinsic::aie2p_vmax_lt32;
  case AIE::BI__builtin_aie2p_vmax_ltbf16:
    return Intrinsic::aie2p_vmax_ltbf16;
  case AIE::BI__builtin_aie2p_vmin_ge8:
    return Intrinsic::aie2p_vmin_ge8;
  case AIE::BI__builtin_aie2p_vmin_ge16:
    return Intrinsic::aie2p_vmin_ge16;
  case AIE::BI__builtin_aie2p_vmin_ge32:
    return Intrinsic::aie2p_vmin_ge32;
  case AIE::BI__builtin_aie2p_vmin_gebf16:
    return Intrinsic::aie2p_vmin_gebf16;
  case AIE::BI__builtin_aie2p_vneg_gtz8:
    return Intrinsic::aie2p_vneg_gtz8;
  case AIE::BI__builtin_aie2p_vneg_gtz16:
    return Intrinsic::aie2p_vneg_gtz16;
  case AIE::BI__builtin_aie2p_vneg_gtz32:
    return Intrinsic::aie2p_vneg_gtz32;
  case AIE::BI__builtin_aie2p_vsub_ge8:
    return Intrinsic::aie2p_vsub_ge8;
  case AIE::BI__builtin_aie2p_vsub_ge16:
    return Intrinsic::aie2p_vsub_ge16;
  case AIE::BI__builtin_aie2p_vsub_ge32:
    return Intrinsic::aie2p_vsub_ge32;
  case AIE::BI__builtin_aie2p_vsub_lt8:
    return Intrinsic::aie2p_vsub_lt8;
  case AIE::BI__builtin_aie2p_vsub_lt16:
    return Intrinsic::aie2p_vsub_lt16;
  case AIE::BI__builtin_aie2p_vsub_lt32:
    return Intrinsic::aie2p_vsub_lt32;
  case AIE::BI__builtin_aie2p_divstep:
    return Intrinsic::aie2p_divs;
  case AIE::BI__builtin_aie2p_v64accfloat_to_v64bfp16ebs8:
    return Intrinsic::aie2p_v64accfloat_to_v64bfp16ebs8;
  case AIE::BI__builtin_aie2p_v64accfloat_to_v64bfp16ebs16:
    return Intrinsic::aie2p_v64accfloat_to_v64bfp16ebs16;
  case AIE::BI__builtin_aie2p_v64bfp16ebs8_to_v64bfp16ebs16:
    return Intrinsic::aie2p_v64bfp16ebs8_to_v64bfp16ebs16;
  case AIE::BI__builtin_aie2p_vshuffle_576_bfp16:
    return Intrinsic::aie2p_vshuffle_576_bfp16;
  case AIE::BI__builtin_aie2p_fifo_st_push_512_bfp16:
    return Intrinsic::aie2p_fifo_st_push_512_bfp16;
  case AIE::BI__builtin_aie2p_fifo_st_push_544_bfp16:
    return Intrinsic::aie2p_fifo_st_push_544_bfp16;
  case AIE::BI__builtin_aie2p_fifo_st_push_576_bfp16:
    return Intrinsic::aie2p_fifo_st_push_576_bfp16;
  case AIE::BI__builtin_aie2p_fifo_st_flush:
    return Intrinsic::aie2p_fifo_st_flush;
  case AIE::BI__builtin_aie2p_fifo_st_flush_conv:
    return Intrinsic::aie2p_fifo_st_flush_conv;
  case AIE::BI__builtin_aie2p_fifo_st_flush_1d_byte:
    return Intrinsic::aie2p_fifo_st_flush_1d;
  case AIE::BI__builtin_aie2p_fifo_st_flush_conv_1d_byte:
    return Intrinsic::aie2p_fifo_st_flush_1d_conv;
  case AIE::BI__builtin_aie2p_fifo_st_flush_2d_byte:
    return Intrinsic::aie2p_fifo_st_flush_2d_conv;
  case AIE::BI__builtin_aie2p_fifo_st_flush_conv_2d_byte:
    return Intrinsic::aie2p_fifo_st_flush_2d;
  case AIE::BI__builtin_aie2p_fifo_st_flush_3d_byte:
    return Intrinsic::aie2p_fifo_st_flush_3d;
  case AIE::BI__builtin_aie2p_fifo_st_flush_conv_3d_byte:
    return Intrinsic::aie2p_fifo_st_flush_3d_conv;
  case AIE::BI__builtin_aie2p_fifo_ld_fill:
    return Intrinsic::aie2p_fifo_ld_fill;
  case AIE::BI__builtin_aie2p_fifo_ld_fillx:
    return Intrinsic::aie2p_fifo_ld_fillx;
  case AIE::BI__builtin_aie2p_fifo_ld_popx:
    return Intrinsic::aie2p_fifo_ld_popx;
  case AIE::BI__builtin_aie2p_fifo_ld_pop_512_unaligned:
    return Intrinsic::aie2p_fifo_ld_pop_unaligned;
  case AIE::BI__builtin_aie2p_fifo_ld_pop_576_bfp16:
    return Intrinsic::aie2p_fifo_ld_pop_576_bfp16;
  case AIE::BI__builtin_aie2p_fifo_ld_pop_544_bfp16:
    return Intrinsic::aie2p_fifo_ld_pop_544_bfp16;
  case AIE::BI__builtin_aie2p_fifo_ld_pop_1d_512_unaligned:
    return Intrinsic::aie2p_fifo_ld_pop_1d_unaligned;
  case AIE::BI__builtin_aie2p_fifo_ld_pop_1d_576_bfp16:
    return Intrinsic::aie2p_fifo_ld_pop_576_1d_bfp16;
  case AIE::BI__builtin_aie2p_fifo_ld_pop_1d_544_bfp16:
    return Intrinsic::aie2p_fifo_ld_pop_544_1d_bfp16;
  case AIE::BI__builtin_aie2p_fifo_ld_pop_2d_512_unaligned:
    return Intrinsic::aie2p_fifo_ld_pop_2d_unaligned;
  case AIE::BI__builtin_aie2p_fifo_ld_pop_2d_576_bfp16:
    return Intrinsic::aie2p_fifo_ld_pop_576_2d_bfp16;
  case AIE::BI__builtin_aie2p_fifo_ld_pop_2d_544_bfp16:
    return Intrinsic::aie2p_fifo_ld_pop_544_2d_bfp16;
  case AIE::BI__builtin_aie2p_fifo_ld_pop_3d_512_unaligned:
    return Intrinsic::aie2p_fifo_ld_pop_3d_unaligned;
  case AIE::BI__builtin_aie2p_fifo_ld_pop_3d_576_bfp16:
    return Intrinsic::aie2p_fifo_ld_pop_576_3d_bfp16;
  case AIE::BI__builtin_aie2p_fifo_ld_pop_3d_544_bfp16:
    return Intrinsic::aie2p_fifo_ld_pop_544_3d_bfp16;
  default:
    break;
  }
  return Intrinsic::not_intrinsic;
}

static llvm::Intrinsic::ID getAIE2PSIntrinsicFunction(unsigned BuiltinID) {
  switch (BuiltinID) {
  case AIE::BI__builtin_aie2ps_get_ss:
    return Intrinsic::aie2ps_get_ss;
  case AIE::BI__builtin_aie2ps_get_ss_nb:
    return Intrinsic::aie2ps_get_ss_nb;
  case AIE::BI__builtin_aie2ps_put_ms_nb:
    return Intrinsic::aie2ps_put_ms_nb;
  case AIE::BI__builtin_aie2ps_scd_expand_ACC1024_incr:
    return Intrinsic::aie2ps_scd_expand_ACC1024_incr;
  case AIE::BI__builtin_aie2ps_scd_expand_ACC2048_incr:
    return Intrinsic::aie2ps_scd_expand_ACC2048_incr;
  case AIE::BI__builtin_aie2ps_add_2d:
    return Intrinsic::aie2ps_add_2d;
  case AIE::BI__builtin_aie2ps_add_3d:
    return Intrinsic::aie2ps_add_3d;
  case AIE::BI__builtin_aie2ps_vshuffle_BFP640_BFP640_BFP640:
    return Intrinsic::aie2ps_vshuffle_BFP640_BFP640_BFP640;
  case AIE::BI__builtin_aie2ps_vshuffle_BFP768_BFP768_BFP768:
    return Intrinsic::aie2ps_vshuffle_BFP768_BFP768_BFP768;
  case AIE::BI__builtin_aie2ps_divstep:
    return Intrinsic::aie2ps_divs;
  case AIE::BI__builtin_aie2ps_v64accfloat_to_v64mx4:
    return Intrinsic::aie2ps_v64accfloat_to_v64mx4;
  case AIE::BI__builtin_aie2ps_v64accfloat_to_v64mx6:
    return Intrinsic::aie2ps_v64accfloat_to_v64mx6;
  case AIE::BI__builtin_aie2ps_v64accfloat_to_v64mx9:
    return Intrinsic::aie2ps_v64accfloat_to_v64mx9;
  case AIE::BI__builtin_aie2ps_vabs_gtz8:
    return Intrinsic::aie2ps_vabs_gtz8;
  case AIE::BI__builtin_aie2ps_vabs_gtz16:
    return Intrinsic::aie2ps_vabs_gtz16;
  case AIE::BI__builtin_aie2ps_vabs_gtz32:
    return Intrinsic::aie2ps_vabs_gtz32;
  case AIE::BI__builtin_aie2ps_vbneg_ltz8:
    return Intrinsic::aie2ps_vbneg_ltz8;
  case AIE::BI__builtin_aie2ps_vbneg_ltz16:
    return Intrinsic::aie2ps_vbneg_ltz16;
  case AIE::BI__builtin_aie2ps_vbneg_ltz32:
    return Intrinsic::aie2ps_vbneg_ltz32;
  case AIE::BI__builtin_aie2ps_vmaxdiff_lt8:
    return Intrinsic::aie2ps_vmaxdiff_lt8;
  case AIE::BI__builtin_aie2ps_vmaxdiff_lt16:
    return Intrinsic::aie2ps_vmaxdiff_lt16;
  case AIE::BI__builtin_aie2ps_vmaxdiff_lt32:
    return Intrinsic::aie2ps_vmaxdiff_lt32;
  case AIE::BI__builtin_aie2ps_vmax_lt8:
    return Intrinsic::aie2ps_vmax_lt8;
  case AIE::BI__builtin_aie2ps_vmax_ltfloat8:
    return Intrinsic::aie2ps_vmax_ltfloat8;
  case AIE::BI__builtin_aie2ps_vmax_ltbfloat8:
    return Intrinsic::aie2ps_vmax_ltbfloat8;
  case AIE::BI__builtin_aie2ps_vmax_lt16:
    return Intrinsic::aie2ps_vmax_lt16;
  case AIE::BI__builtin_aie2ps_vmax_lt32:
    return Intrinsic::aie2ps_vmax_lt32;
  case AIE::BI__builtin_aie2ps_vmax_ltbfloat16:
    return Intrinsic::aie2ps_vmax_ltbfloat16;
  case AIE::BI__builtin_aie2ps_vmax_ltfloat16:
    return Intrinsic::aie2ps_vmax_ltfloat16;
  case AIE::BI__builtin_aie2ps_vmin_ge8:
    return Intrinsic::aie2ps_vmin_ge8;
  case AIE::BI__builtin_aie2ps_vmin_gefloat8:
    return Intrinsic::aie2ps_vmin_gefloat8;
  case AIE::BI__builtin_aie2ps_vmin_gebfloat8:
    return Intrinsic::aie2ps_vmin_gebfloat8;
  case AIE::BI__builtin_aie2ps_vmin_ge16:
    return Intrinsic::aie2ps_vmin_ge16;
  case AIE::BI__builtin_aie2ps_vmin_ge32:
    return Intrinsic::aie2ps_vmin_ge32;
  case AIE::BI__builtin_aie2ps_vmin_gefloat16:
    return Intrinsic::aie2ps_vmin_gefloat16;
  case AIE::BI__builtin_aie2ps_vmin_gebfloat16:
    return Intrinsic::aie2ps_vmin_gebfloat16;
  case AIE::BI__builtin_aie2ps_vneg_gtz8:
    return Intrinsic::aie2ps_vneg_gtz8;
  case AIE::BI__builtin_aie2ps_vneg_gtz16:
    return Intrinsic::aie2ps_vneg_gtz16;
  case AIE::BI__builtin_aie2ps_vneg_gtz32:
    return Intrinsic::aie2ps_vneg_gtz32;
  case AIE::BI__builtin_aie2ps_vsub_ge8:
    return Intrinsic::aie2ps_vsub_ge8;
  case AIE::BI__builtin_aie2ps_vsub_ge16:
    return Intrinsic::aie2ps_vsub_ge16;
  case AIE::BI__builtin_aie2ps_vsub_ge32:
    return Intrinsic::aie2ps_vsub_ge32;
  case AIE::BI__builtin_aie2ps_vsub_lt8:
    return Intrinsic::aie2ps_vsub_lt8;
  case AIE::BI__builtin_aie2ps_vsub_lt16:
    return Intrinsic::aie2ps_vsub_lt16;
  case AIE::BI__builtin_aie2ps_vsub_lt32:
    return Intrinsic::aie2ps_vsub_lt32;
  case AIE::BI__builtin_aie2ps_fifo_ld_fill:
    return Intrinsic::aie2ps_fifo_ld_fill;
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_512_unaligned:
    return Intrinsic::aie2ps_fifo_ld_pop_512_unaligned;
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_BFP640:
    return Intrinsic::aie2ps_fifo_ld_pop_BFP640;
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_BFP768:
    return Intrinsic::aie2ps_fifo_ld_pop_BFP768;
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_1d_512_unaligned:
    return Intrinsic::aie2ps_fifo_ld_pop_1d_unaligned;
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_1d_BFP640:
    return Intrinsic::aie2ps_fifo_ld_pop_1d_BFP640;
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_1d_BFP768:
    return Intrinsic::aie2ps_fifo_ld_pop_1d_BFP768;
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_512_unaligned:
    return Intrinsic::aie2ps_fifo_ld_pop_2d_unaligned;
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_BFP640:
    return Intrinsic::aie2ps_fifo_ld_pop_2d_BFP640;
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_BFP768:
    return Intrinsic::aie2ps_fifo_ld_pop_2d_BFP768;
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_512_unaligned:
    return Intrinsic::aie2ps_fifo_ld_pop_3d_unaligned;
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_BFP640:
    return Intrinsic::aie2ps_fifo_ld_pop_3d_BFP640;
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_BFP768:
    return Intrinsic::aie2ps_fifo_ld_pop_3d_BFP768;
  case AIE::BI__builtin_aie2ps_fifo_ld_fillx:
    return Intrinsic::aie2ps_fifo_ld_fillx;
  case AIE::BI__builtin_aie2ps_fifo_ld_popx:
    return Intrinsic::aie2ps_fifo_ld_popx;
  case AIE::BI__builtin_aie2ps_fifo_st_push_512:
    return Intrinsic::aie2ps_fifo_st_push_512;
  case AIE::BI__builtin_aie2ps_fifo_st_push_BFP384:
    return Intrinsic::aie2ps_fifo_st_push_BFP384;
  case AIE::BI__builtin_aie2ps_fifo_st_push_BFP640:
    return Intrinsic::aie2ps_fifo_st_push_BFP640;
  case AIE::BI__builtin_aie2ps_fifo_st_push_BFP768:
    return Intrinsic::aie2ps_fifo_st_push_BFP768;
  case AIE::BI__builtin_aie2ps_fifo_st_flush:
    return Intrinsic::aie2ps_fifo_st_flush;
  case AIE::BI__builtin_aie2ps_fifo_st_flush_1d_byte:
    return Intrinsic::aie2ps_fifo_st_flush_1d;
  case AIE::BI__builtin_aie2ps_fifo_st_flush_2d_byte:
    return Intrinsic::aie2ps_fifo_st_flush_2d;
  case AIE::BI__builtin_aie2ps_fifo_st_flush_3d_byte:
    return Intrinsic::aie2ps_fifo_st_flush_3d;
  case AIE::BI__builtin_aie2ps_fifo_st_flush_conv:
    return Intrinsic::aie2ps_fifo_st_flush_conv;
  case AIE::BI__builtin_aie2ps_fifo_st_flush_conv_1d_byte:
    return Intrinsic::aie2ps_fifo_st_flush_1d_conv;
  case AIE::BI__builtin_aie2ps_fifo_st_flush_conv_2d_byte:
    return Intrinsic::aie2ps_fifo_st_flush_2d_conv;
  case AIE::BI__builtin_aie2ps_fifo_st_flush_conv_3d_byte:
    return Intrinsic::aie2ps_fifo_st_flush_3d_conv;
  default:
    break;
  }
  return Intrinsic::not_intrinsic;
}

static llvm::Intrinsic::ID
getAIEIntrinsicFunction(unsigned BuiltinID, llvm::Triple::ArchType Arch) {
  switch (Arch) {
  case llvm::Triple::aie:
    return getAIE1IntrinsicFunction(BuiltinID);
  case llvm::Triple::aie2:
    return getAIE2IntrinsicFunction(BuiltinID);
  case llvm::Triple::aie2p:
    return getAIE2PIntrinsicFunction(BuiltinID);
  case llvm::Triple::aie2ps:
    return getAIE2PSIntrinsicFunction(BuiltinID);
  default:
    break;
  }
  return Intrinsic::not_intrinsic;
}

Value *CodeGenFunction::EmitAIE1BuiltinExpr(unsigned BuiltinID,
                                            const CallExpr *E) {

  switch (BuiltinID) {
  // Custom lowering is required for following intrinsics to introduce
  // truncation from i32 to i20 for "addr" paramter
  case AIE::BI__builtin_aie_ctrl_packet_header: {
    SmallVector<Value *, 4> Ops;

    Ops.push_back(
        Builder.CreateTrunc(EmitScalarExpr(E->getArg(0)),
                            llvm::Type::getInt20Ty(getLLVMContext())));
    for (unsigned I = 1; I < E->getNumArgs(); I++) {
      Ops.push_back(EmitScalarExpr(E->getArg(I)));
    }
    llvm::Intrinsic::ID IntrinsicID = getAIE1IntrinsicFunction(BuiltinID);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Val = Builder.CreateCall(F, Ops);
    return Val;
  }
  default:
    break;
  }
  return nullptr;
}

void insertImplicitCasts(MutableArrayRef<llvm::Value *> Args,
                         llvm::FunctionType &FTy, CGBuilderTy &Builder) {
  for (unsigned ArgNum = 0; ArgNum < Args.size(); ++ArgNum) {
    llvm::Value *Arg = Args[ArgNum];
    llvm::Type *RequiredType = FTy.getParamType(ArgNum);
    if (Arg->getType() != RequiredType)
      Args[ArgNum] = Builder.CreateBitCast(Arg, RequiredType);
  }
}

Value *CodeGenFunction::EmitAIEBuiltinExpr(unsigned BuiltinID,
                                           const CallExpr *E,
                                           llvm::Triple::ArchType Arch) {

  auto CreateStoreToLValue = [&](Value *Val, const Expr *Arg) {
    LValue LV = EmitLValue(Arg);
    return Builder.CreateStore(Val, LV.getAddress(),
                               LV.isVolatileQualified());
  };

  switch (BuiltinID) {
  case AIE::BI__builtin_aiev2_add_2d:
  case AIE::BI__builtin_aie2p_add_2d:
  case AIE::BI__builtin_aie2ps_add_2d: {
    // Custom lowering is used for addr intrinsics to introduce truncation
    // from i32 to i20 and to handle multiple outputs returned by these
    // intrinsics and Zext from i20 back to i32 for count parameter
    SmallVector<Value *, 5> Ops;
    Ops.push_back(EmitScalarExpr(E->getArg(0)));
    for (unsigned i = 1, e = E->getNumArgs(); i != e; i++) {
      Ops.push_back(
          Builder.CreateTrunc(EmitScalarExpr(E->getArg(i)),
                              llvm::Type::getInt20Ty(getLLVMContext())));
    }

    llvm::Intrinsic::ID IntrinsicID = getAIEIntrinsicFunction(BuiltinID, Arch);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Val = Builder.CreateCall(F, Ops);

    Value *Count1 =
        Builder.CreateZExt(Builder.CreateExtractValue(Val, 1),
                           llvm::Type::getInt32Ty(getLLVMContext()));
    Value *Count1Addr = EmitLValue(E->getArg(4)).getPointer(*this);
    Builder.CreateDefaultAlignedStore(Count1, Count1Addr);

    return Builder.CreateExtractValue(Val, 0);
  }
  case AIE::BI__builtin_aiev2_add_3d:
  case AIE::BI__builtin_aie2p_add_3d:
  case AIE::BI__builtin_aie2ps_add_3d: {
    SmallVector<Value *, 8> Ops;
    Ops.push_back(EmitScalarExpr(E->getArg(0)));
    for (unsigned i = 1, e = E->getNumArgs(); i != e; i++) {
      Ops.push_back(
          Builder.CreateTrunc(EmitScalarExpr(E->getArg(i)),
                              llvm::Type::getInt20Ty(getLLVMContext())));
    }

    llvm::Intrinsic::ID IntrinsicID = getAIEIntrinsicFunction(BuiltinID, Arch);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Val = Builder.CreateCall(F, Ops);

    Value *Count1 =
        Builder.CreateZExt(Builder.CreateExtractValue(Val, 1),
                           llvm::Type::getInt32Ty(getLLVMContext()));
    Value *Count1Addr = EmitLValue(E->getArg(5)).getPointer(*this);
    Value *Count2 =
        Builder.CreateZExt(Builder.CreateExtractValue(Val, 2),
                           llvm::Type::getInt32Ty(getLLVMContext()));
    Value *Count2Addr = EmitLValue(E->getArg(7)).getPointer(*this);

    Builder.CreateDefaultAlignedStore(Count1, Count1Addr);
    Builder.CreateDefaultAlignedStore(Count2, Count2Addr);

    return Builder.CreateExtractValue(Val, 0);
  }
  case AIE::BI__builtin_aiev2_put_ms_nb:
  case AIE::BI__builtin_aie2p_put_ms_nb:
  case AIE::BI__builtin_aie2ps_put_ms_nb:
  case AIE::BI__builtin_aiev2_put_ms_nb_packet_header:
  case AIE::BI__builtin_aiev2_put_ms_nb_ctrl_packet_header: {
    SmallVector<Value *, 2> Ops;
    for (unsigned I = 0; I < E->getNumArgs() - 1; I++)
      Ops.push_back(EmitScalarExpr(E->getArg(I)));

    llvm::Intrinsic::ID IntrinsicID = getAIEIntrinsicFunction(BuiltinID, Arch);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Val = Builder.CreateCall(F, Ops);

    Value *SuccAddr =
        EmitLValue(E->getArg(E->getNumArgs() - 1)).getPointer(*this);
    return Builder.CreateDefaultAlignedStore(Val, SuccAddr);
  }
  case AIE::BI__builtin_aiev2_vabs_gtz8:
  case AIE::BI__builtin_aiev2_vabs_gtz16:
  case AIE::BI__builtin_aiev2_vabs_gtz32:
  case AIE::BI__builtin_aiev2_vbneg_ltz8:
  case AIE::BI__builtin_aiev2_vbneg_ltz16:
  case AIE::BI__builtin_aiev2_vbneg_ltz32:
  case AIE::BI__builtin_aiev2_vmaxdiff_lt8:
  case AIE::BI__builtin_aiev2_vmaxdiff_lt16:
  case AIE::BI__builtin_aiev2_vmaxdiff_lt32:
  case AIE::BI__builtin_aiev2_vmax_lt8:
  case AIE::BI__builtin_aiev2_vmax_lt16:
  case AIE::BI__builtin_aiev2_vmax_lt32:
  case AIE::BI__builtin_aiev2_vmax_ltbf16:
  case AIE::BI__builtin_aiev2_vmin_ge8:
  case AIE::BI__builtin_aiev2_vmin_ge16:
  case AIE::BI__builtin_aiev2_vmin_ge32:
  case AIE::BI__builtin_aiev2_vmin_gebf16:
  case AIE::BI__builtin_aiev2_vneg_gtz8:
  case AIE::BI__builtin_aiev2_vneg_gtz16:
  case AIE::BI__builtin_aiev2_vneg_gtz32:
  case AIE::BI__builtin_aiev2_vsub_ge8:
  case AIE::BI__builtin_aiev2_vsub_ge16:
  case AIE::BI__builtin_aiev2_vsub_ge32:
  case AIE::BI__builtin_aiev2_vsub_lt8:
  case AIE::BI__builtin_aiev2_vsub_lt16:
  case AIE::BI__builtin_aiev2_vsub_lt32:
  case AIE::BI__builtin_aiev2_get_ss:
  case AIE::BI__builtin_aiev2_get_ss_nb:
  case AIE::BI__builtin_aie2p_vabs_gtz8:
  case AIE::BI__builtin_aie2p_vabs_gtz16:
  case AIE::BI__builtin_aie2p_vabs_gtz32:
  case AIE::BI__builtin_aie2p_vbneg_ltz8:
  case AIE::BI__builtin_aie2p_vbneg_ltz16:
  case AIE::BI__builtin_aie2p_vbneg_ltz32:
  case AIE::BI__builtin_aie2p_vmaxdiff_lt8:
  case AIE::BI__builtin_aie2p_vmaxdiff_lt16:
  case AIE::BI__builtin_aie2p_vmaxdiff_lt32:
  case AIE::BI__builtin_aie2p_vmax_lt8:
  case AIE::BI__builtin_aie2p_vmax_lt16:
  case AIE::BI__builtin_aie2p_vmax_lt32:
  case AIE::BI__builtin_aie2p_vmax_ltbf16:
  case AIE::BI__builtin_aie2p_vmin_ge8:
  case AIE::BI__builtin_aie2p_vmin_ge16:
  case AIE::BI__builtin_aie2p_vmin_ge32:
  case AIE::BI__builtin_aie2p_vmin_gebf16:
  case AIE::BI__builtin_aie2p_vneg_gtz8:
  case AIE::BI__builtin_aie2p_vneg_gtz16:
  case AIE::BI__builtin_aie2p_vneg_gtz32:
  case AIE::BI__builtin_aie2p_vsub_ge8:
  case AIE::BI__builtin_aie2p_vsub_ge16:
  case AIE::BI__builtin_aie2p_vsub_ge32:
  case AIE::BI__builtin_aie2p_vsub_lt8:
  case AIE::BI__builtin_aie2p_vsub_lt16:
  case AIE::BI__builtin_aie2p_vsub_lt32:
  case AIE::BI__builtin_aie2p_get_ss:
  case AIE::BI__builtin_aie2p_get_ss_nb:
  case AIE::BI__builtin_aie2p_scd_expand_ACC1024_incr:
  case AIE::BI__builtin_aie2p_scd_expand_ACC2048_incr:
  case AIE::BI__builtin_aie2ps_vabs_gtz8:
  case AIE::BI__builtin_aie2ps_vabs_gtz16:
  case AIE::BI__builtin_aie2ps_vabs_gtz32:
  case AIE::BI__builtin_aie2ps_vbneg_ltz8:
  case AIE::BI__builtin_aie2ps_vbneg_ltz16:
  case AIE::BI__builtin_aie2ps_vbneg_ltz32:
  case AIE::BI__builtin_aie2ps_vmaxdiff_lt8:
  case AIE::BI__builtin_aie2ps_vmaxdiff_lt16:
  case AIE::BI__builtin_aie2ps_vmaxdiff_lt32:
  case AIE::BI__builtin_aie2ps_vmax_lt8:
  case AIE::BI__builtin_aie2ps_vmax_ltfloat8:
  case AIE::BI__builtin_aie2ps_vmax_ltbfloat8:
  case AIE::BI__builtin_aie2ps_vmax_lt16:
  case AIE::BI__builtin_aie2ps_vmax_lt32:
  case AIE::BI__builtin_aie2ps_vmax_ltbfloat16:
  case AIE::BI__builtin_aie2ps_vmax_ltfloat16:
  case AIE::BI__builtin_aie2ps_vmin_ge8:
  case AIE::BI__builtin_aie2ps_vmin_gefloat8:
  case AIE::BI__builtin_aie2ps_vmin_gebfloat8:
  case AIE::BI__builtin_aie2ps_vmin_ge16:
  case AIE::BI__builtin_aie2ps_vmin_ge32:
  case AIE::BI__builtin_aie2ps_vmin_gefloat16:
  case AIE::BI__builtin_aie2ps_vmin_gebfloat16:
  case AIE::BI__builtin_aie2ps_vneg_gtz8:
  case AIE::BI__builtin_aie2ps_vneg_gtz16:
  case AIE::BI__builtin_aie2ps_vneg_gtz32:
  case AIE::BI__builtin_aie2ps_vsub_ge8:
  case AIE::BI__builtin_aie2ps_vsub_ge16:
  case AIE::BI__builtin_aie2ps_vsub_ge32:
  case AIE::BI__builtin_aie2ps_vsub_lt8:
  case AIE::BI__builtin_aie2ps_vsub_lt16:
  case AIE::BI__builtin_aie2ps_vsub_lt32:
  case AIE::BI__builtin_aie2ps_get_ss:
  case AIE::BI__builtin_aie2ps_get_ss_nb:
  case AIE::BI__builtin_aie2ps_scd_expand_ACC1024_incr:
  case AIE::BI__builtin_aie2ps_scd_expand_ACC2048_incr: {
    SmallVector<Value *, 3> Ops;
    // Skip the last argument, it's actually an output
    for (unsigned I = 0; I < E->getNumArgs() - 1; I++)
      Ops.push_back(EmitScalarExpr(E->getArg(I)));

    llvm::Intrinsic::ID IntrinsicID = getAIEIntrinsicFunction(BuiltinID, Arch);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Val = Builder.CreateCall(F, Ops);

    // The second member of the returned struct is the compare result,
    // store it to the input reference
    Value *Cmp = Builder.CreateExtractValue(Val, 1);
    Value *CmpAddr =
        EmitLValue(E->getArg(E->getNumArgs() - 1)).getPointer(*this);
    Builder.CreateDefaultAlignedStore(Cmp, CmpAddr);

    return Builder.CreateExtractValue(Val, 0);
  }
  case AIE::BI__builtin_aiev2_divstep:
  case AIE::BI__builtin_aie2p_divstep:
  case AIE::BI__builtin_aie2ps_divstep: {

    SmallVector<Value *, 3> Ops;
    for (unsigned I = 0; I < E->getNumArgs(); I++)
      Ops.push_back(EmitScalarExpr(E->getArg(I)));

    llvm::Intrinsic::ID IntrinsicID = getAIEIntrinsicFunction(BuiltinID, Arch);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Val = Builder.CreateCall(F, Ops);

    // The second member of the returned struct is the division result,
    // store it to the first input reference
    Value *Rem = Builder.CreateExtractValue(Val, 1);
    Value *RemAddr = EmitLValue(E->getArg(0)).getPointer(*this);
    Builder.CreateDefaultAlignedStore(Rem, RemAddr);

    // The first member of the returned struct is the remainder result,
    // store it to the second input reference
    Value *Div = Builder.CreateExtractValue(Val, 0);
    Value *DivAddr = EmitLValue(E->getArg(1)).getPointer(*this);
    return Builder.CreateDefaultAlignedStore(Div, DivAddr);
  }
  case AIE::BI__builtin_aie2p_v64accfloat_to_v64bfp16ebs8:
  case AIE::BI__builtin_aie2p_v64accfloat_to_v64bfp16ebs16:
  case AIE::BI__builtin_aie2p_v64bfp16ebs8_to_v64bfp16ebs16: {
    SmallVector<Value *, 3> Ops;
    if (BuiltinID == AIE::BI__builtin_aie2p_v64accfloat_to_v64bfp16ebs8 ||
        BuiltinID == AIE::BI__builtin_aie2p_v64accfloat_to_v64bfp16ebs16)
      Ops.push_back(EmitScalarExpr(E->getArg(E->getNumArgs() - 1)));
    if (BuiltinID == AIE::BI__builtin_aie2p_v64bfp16ebs8_to_v64bfp16ebs16) {
      Ops.push_back(EmitScalarExpr(E->getArg(E->getNumArgs() - 2)));
      Ops.push_back(EmitScalarExpr(E->getArg(E->getNumArgs() - 1)));
    }

    llvm::Intrinsic::ID IntrinsicID = getAIEIntrinsicFunction(BuiltinID, Arch);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Val = Builder.CreateCall(F, Ops);

    Value *Mant = Builder.CreateExtractValue(Val, 0);
    Value *MantAddr = EmitLValue(E->getArg(0)).getPointer(*this);
    Builder.CreateDefaultAlignedStore(Mant, MantAddr);

    Value *Exp = Builder.CreateExtractValue(Val, 1);
    Value *ExpAddr = EmitLValue(E->getArg(1)).getPointer(*this);

    return Builder.CreateDefaultAlignedStore(Exp, ExpAddr);
  }
  case AIE::BI__builtin_aie2p_vshuffle_576_bfp16: {
    SmallVector<Value *, 3> Ops;
    for (unsigned I = 0; I < E->getNumArgs() - 2; I++)
      Ops.push_back(EmitScalarExpr(E->getArg(I)));

    llvm::Intrinsic::ID IntrinsicID = getAIEIntrinsicFunction(BuiltinID, Arch);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Val = Builder.CreateCall(F, Ops);

    // The first member of the returned struct is the mantissa part of bfp16,
    // store it to the first input reference
    Value *Mant = Builder.CreateExtractValue(Val, 0);
    Value *MantAddr =
        EmitLValue(E->getArg(E->getNumArgs() - 2)).getPointer(*this);
    Builder.CreateDefaultAlignedStore(Mant, MantAddr);

    // The second member of the returned struct is the exponent part of bfp16
    // store it to the second input reference
    Value *Exp = Builder.CreateExtractValue(Val, 1);
    Value *ExpAddr =
        EmitLValue(E->getArg(E->getNumArgs() - 1)).getPointer(*this);
    return Builder.CreateDefaultAlignedStore(Exp, ExpAddr);
  }
  case AIE::BI__builtin_aie2p_fifo_st_push_512_bfp16:
  case AIE::BI__builtin_aie2p_fifo_st_push_544_bfp16:
  case AIE::BI__builtin_aie2p_fifo_st_push_576_bfp16:
  case AIE::BI__builtin_aie2p_fifo_st_flush_conv:
  case AIE::BI__builtin_aie2p_fifo_st_flush:
  case AIE::BI__builtin_aie2ps_fifo_st_push_512:
  case AIE::BI__builtin_aie2ps_fifo_st_push_BFP384:
  case AIE::BI__builtin_aie2ps_fifo_st_push_BFP640:
  case AIE::BI__builtin_aie2ps_fifo_st_push_BFP768:
  case AIE::BI__builtin_aie2ps_fifo_st_flush_conv:
  case AIE::BI__builtin_aie2ps_fifo_st_flush: {
    SmallVector<Value *, 3> Ops;
    for (unsigned I = 0; I < E->getNumArgs(); I++)
      Ops.push_back(EmitScalarExpr(E->getArg(I)));

    llvm::Type *OverloadedTy = Ops[0]->getType();
    llvm::Intrinsic::ID IntrinsicID = getAIEIntrinsicFunction(BuiltinID, Arch);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID, {OverloadedTy, OverloadedTy});

    insertImplicitCasts(Ops, *F->getFunctionType(), Builder);
    Value *Val = Builder.CreateCall(F, Ops);

    Value *Ptr = Builder.CreateExtractValue(Val, 0);
    Value *Fifo = Builder.CreateExtractValue(Val, 1);
    Value *Pos = Builder.CreateExtractValue(Val, 2);

    unsigned FifoArgNo;
    unsigned PosArgNo;
    switch (BuiltinID) {
    case AIE::BI__builtin_aie2ps_fifo_st_push_BFP384:
    case AIE::BI__builtin_aie2ps_fifo_st_push_BFP640:
    case AIE::BI__builtin_aie2ps_fifo_st_push_BFP768: {
      FifoArgNo = 1;
      PosArgNo = 2;
      break;
    }
    case AIE::BI__builtin_aie2p_fifo_st_flush:
    case AIE::BI__builtin_aie2p_fifo_st_flush_conv:
    case AIE::BI__builtin_aie2p_fifo_st_push_512_bfp16:
    case AIE::BI__builtin_aie2p_fifo_st_push_576_bfp16:
    case AIE::BI__builtin_aie2p_fifo_st_push_544_bfp16:
    case AIE::BI__builtin_aie2ps_fifo_st_push_512:
    case AIE::BI__builtin_aie2ps_fifo_st_flush:
    case AIE::BI__builtin_aie2ps_fifo_st_flush_conv:
      FifoArgNo = E->getNumArgs() - 2;
      PosArgNo = E->getNumArgs() - 1;
      break;
    default:
      llvm_unreachable("Unexpected BuiltinID");
    }

    CreateStoreToLValue(Fifo, E->getArg(FifoArgNo));
    CreateStoreToLValue(Pos, E->getArg(PosArgNo));
    return CreateStoreToLValue(Ptr, E->getArg(0));
  }
  case AIE::BI__builtin_aie2p_fifo_st_flush_conv_1d_byte:
  case AIE::BI__builtin_aie2p_fifo_st_flush_1d_byte:
  case AIE::BI__builtin_aie2p_fifo_st_flush_conv_2d_byte:
  case AIE::BI__builtin_aie2p_fifo_st_flush_2d_byte:
  case AIE::BI__builtin_aie2p_fifo_st_flush_conv_3d_byte:
  case AIE::BI__builtin_aie2p_fifo_st_flush_3d_byte:
  case AIE::BI__builtin_aie2ps_fifo_st_flush_conv_1d_byte:
  case AIE::BI__builtin_aie2ps_fifo_st_flush_1d_byte:
  case AIE::BI__builtin_aie2ps_fifo_st_flush_conv_2d_byte:
  case AIE::BI__builtin_aie2ps_fifo_st_flush_2d_byte:
  case AIE::BI__builtin_aie2ps_fifo_st_flush_conv_3d_byte:
  case AIE::BI__builtin_aie2ps_fifo_st_flush_3d_byte: {
    SmallVector<Value *, 3> Ops;
    unsigned NumAddrIncs = 0;
    switch (BuiltinID) {
    case AIE::BI__builtin_aie2p_fifo_st_flush_1d_byte:
    case AIE::BI__builtin_aie2p_fifo_st_flush_conv_1d_byte:
    case AIE::BI__builtin_aie2ps_fifo_st_flush_1d_byte:
    case AIE::BI__builtin_aie2ps_fifo_st_flush_conv_1d_byte:
      NumAddrIncs = 1;
      break;
    case AIE::BI__builtin_aie2p_fifo_st_flush_2d_byte:
    case AIE::BI__builtin_aie2p_fifo_st_flush_conv_2d_byte:
    case AIE::BI__builtin_aie2ps_fifo_st_flush_2d_byte:
    case AIE::BI__builtin_aie2ps_fifo_st_flush_conv_2d_byte:
      NumAddrIncs = 4;
      break;
    case AIE::BI__builtin_aie2p_fifo_st_flush_3d_byte:
    case AIE::BI__builtin_aie2p_fifo_st_flush_conv_3d_byte:
    case AIE::BI__builtin_aie2ps_fifo_st_flush_3d_byte:
    case AIE::BI__builtin_aie2ps_fifo_st_flush_conv_3d_byte:
      NumAddrIncs = 7;
      break;
    default:
      llvm_unreachable("Unexpected BuiltinID");
    }

    for (unsigned I = 0; I < E->getNumArgs() - NumAddrIncs; I++)
      Ops.push_back(EmitScalarExpr(E->getArg(I)));

    for (unsigned I = E->getNumArgs() - NumAddrIncs, NA = E->getNumArgs();
         I < NA; I++) {
      Ops.push_back(
          Builder.CreateTrunc(EmitScalarExpr(E->getArg(I)),
                              llvm::Type::getInt20Ty(getLLVMContext())));
    }

    llvm::Type *OverloadedTy = Ops[0]->getType();
    llvm::Intrinsic::ID IntrinsicID = getAIEIntrinsicFunction(BuiltinID, Arch);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID, {OverloadedTy, OverloadedTy});

    insertImplicitCasts(Ops, *F->getFunctionType(), Builder);
    Value *Val = Builder.CreateCall(F, Ops);

    Value *Ptr = Builder.CreateExtractValue(Val, 0);
    Value *Fifo = Builder.CreateExtractValue(Val, 1);
    Value *Pos = Builder.CreateExtractValue(Val, 2);

    switch (BuiltinID) {
    case AIE::BI__builtin_aie2p_fifo_st_flush_2d_byte:
    case AIE::BI__builtin_aie2p_fifo_st_flush_conv_2d_byte:
    case AIE::BI__builtin_aie2ps_fifo_st_flush_2d_byte:
    case AIE::BI__builtin_aie2ps_fifo_st_flush_conv_2d_byte: {
      Value *Count1 =
          Builder.CreateZExt(Builder.CreateExtractValue(Val, 3),
                             llvm::Type::getInt32Ty(getLLVMContext()));
      CreateStoreToLValue(Count1, E->getArg(5));
      break;
    }
    case AIE::BI__builtin_aie2p_fifo_st_flush_3d_byte:
    case AIE::BI__builtin_aie2p_fifo_st_flush_conv_3d_byte:
    case AIE::BI__builtin_aie2ps_fifo_st_flush_3d_byte:
    case AIE::BI__builtin_aie2ps_fifo_st_flush_conv_3d_byte: {
      Value *Count1 =
          Builder.CreateZExt(Builder.CreateExtractValue(Val, 3),
                             llvm::Type::getInt32Ty(getLLVMContext()));
      Value *Count2 =
          Builder.CreateZExt(Builder.CreateExtractValue(Val, 4),
                             llvm::Type::getInt32Ty(getLLVMContext()));
      CreateStoreToLValue(Count1, E->getArg(E->getNumArgs() - 5));
      CreateStoreToLValue(Count2, E->getArg(E->getNumArgs() - 2));
      break;
    }
    }

    CreateStoreToLValue(Fifo, E->getArg(1));
    CreateStoreToLValue(Pos, E->getArg(2));
    return CreateStoreToLValue(Ptr, E->getArg(0));
  }
  case AIE::BI__builtin_aie2p_fifo_ld_fillx:
  case AIE::BI__builtin_aie2p_fifo_ld_fill:
  case AIE::BI__builtin_aie2ps_fifo_ld_fillx:
  case AIE::BI__builtin_aie2ps_fifo_ld_fill: {
    SmallVector<Value *, 3> Ops;
    for (unsigned I = 0; I < E->getNumArgs(); I++)
      Ops.push_back(EmitScalarExpr(E->getArg(I)));

    llvm::Type *OverloadedTy = Ops[0]->getType();
    llvm::Intrinsic::ID IntrinsicID = getAIEIntrinsicFunction(BuiltinID, Arch);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID, {OverloadedTy, OverloadedTy});

    insertImplicitCasts(Ops, *F->getFunctionType(), Builder);
    Value *Val = Builder.CreateCall(F, Ops);

    Value *Ptr = Builder.CreateExtractValue(Val, 0);
    Value *Fifo = Builder.CreateExtractValue(Val, 1);
    Value *Pos = Builder.CreateExtractValue(Val, 2);

    if (BuiltinID == AIE::BI__builtin_aie2p_fifo_ld_fillx ||
        BuiltinID == AIE::BI__builtin_aie2ps_fifo_ld_fillx) {
      Value *Extra = Builder.CreateExtractValue(Val, 3);
      CreateStoreToLValue(Extra, E->getArg(3));
    }

    CreateStoreToLValue(Fifo, E->getArg(1));
    CreateStoreToLValue(Pos, E->getArg(2));
    return CreateStoreToLValue(Ptr, E->getArg(0));
  }
  case AIE::BI__builtin_aie2p_fifo_ld_pop_512_unaligned:
  case AIE::BI__builtin_aie2p_fifo_ld_popx:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_1d_512_unaligned:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_2d_512_unaligned:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_3d_512_unaligned:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_512_unaligned:
  case AIE::BI__builtin_aie2ps_fifo_ld_popx:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_1d_512_unaligned:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_512_unaligned:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_512_unaligned: {
    SmallVector<Value *, 3> Ops;
    unsigned NumAddrIncs = 0;
    if (BuiltinID == AIE::BI__builtin_aie2p_fifo_ld_pop_1d_512_unaligned ||
        BuiltinID == AIE::BI__builtin_aie2ps_fifo_ld_pop_1d_512_unaligned) {
      NumAddrIncs = 1;
    } else if (BuiltinID ==
                   AIE::BI__builtin_aie2p_fifo_ld_pop_2d_512_unaligned ||
               BuiltinID ==
                   AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_512_unaligned) {
      NumAddrIncs = 4;
    } else if (BuiltinID ==
                   AIE::BI__builtin_aie2p_fifo_ld_pop_3d_512_unaligned ||
               BuiltinID ==
                   AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_512_unaligned) {
      NumAddrIncs = 7;
    }

    for (unsigned I = 0; I < E->getNumArgs() - NumAddrIncs; I++)
      Ops.push_back(EmitScalarExpr(E->getArg(I)));

    for (unsigned I = E->getNumArgs() - NumAddrIncs, NA = E->getNumArgs();
         I < NA; I++) {
      Ops.push_back(
          Builder.CreateTrunc(EmitScalarExpr(E->getArg(I)),
                              llvm::Type::getInt20Ty(getLLVMContext())));
    }

    llvm::Type *OverloadedTy = Ops[0]->getType();
    llvm::Intrinsic::ID IntrinsicID = getAIEIntrinsicFunction(BuiltinID, Arch);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID, {OverloadedTy, OverloadedTy});

    insertImplicitCasts(Ops, *F->getFunctionType(), Builder);
    Value *Val = Builder.CreateCall(F, Ops);
    unsigned ExtractValIter = 0;

    Value *Vec = Builder.CreateExtractValue(Val, ExtractValIter++);
    Value *Ptr = Builder.CreateExtractValue(Val, ExtractValIter++);
    Value *Fifo = Builder.CreateExtractValue(Val, ExtractValIter++);
    Value *Pos = Builder.CreateExtractValue(Val, ExtractValIter++);

    if (BuiltinID == AIE::BI__builtin_aie2p_fifo_ld_popx ||
        BuiltinID == AIE::BI__builtin_aie2ps_fifo_ld_popx) {
      Value *Extra = Builder.CreateExtractValue(Val, ExtractValIter++);
      CreateStoreToLValue(Extra, E->getArg(3));
    }

    if (BuiltinID == AIE::BI__builtin_aie2p_fifo_ld_pop_2d_512_unaligned ||
        BuiltinID == AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_512_unaligned) {
      Value *Count1 =
          Builder.CreateZExt(Builder.CreateExtractValue(Val, ExtractValIter++),
                             llvm::Type::getInt32Ty(getLLVMContext()));
      CreateStoreToLValue(Count1, E->getArg(5));
    } else if (BuiltinID ==
                   AIE::BI__builtin_aie2p_fifo_ld_pop_3d_512_unaligned ||
               BuiltinID ==
                   AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_512_unaligned) {
      Value *Count1 =
          Builder.CreateZExt(Builder.CreateExtractValue(Val, ExtractValIter++),
                             llvm::Type::getInt32Ty(getLLVMContext()));
      Value *Count2 =
          Builder.CreateZExt(Builder.CreateExtractValue(Val, ExtractValIter++),
                             llvm::Type::getInt32Ty(getLLVMContext()));
      CreateStoreToLValue(Count1, E->getArg(E->getNumArgs() - 5));
      CreateStoreToLValue(Count2, E->getArg(E->getNumArgs() - 2));
    }

    CreateStoreToLValue(Fifo, E->getArg(1));
    CreateStoreToLValue(Pos, E->getArg(2));
    CreateStoreToLValue(Ptr, E->getArg(0));
    return Vec;
  }
  case AIE::BI__builtin_aie2p_fifo_ld_pop_1d_544_bfp16:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_1d_576_bfp16:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_2d_576_bfp16:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_2d_544_bfp16:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_3d_576_bfp16:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_3d_544_bfp16:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_544_bfp16:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_576_bfp16:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_BFP640:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_BFP768:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_1d_BFP640:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_1d_BFP768:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_BFP640:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_BFP768:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_BFP640:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_BFP768: {
    SmallVector<Value *, 3> Ops;
    unsigned NumAddrIncs = 0;
    unsigned NumOutIncs = 0;
    unsigned MXStructCount = 0;
    switch (BuiltinID) {
    case AIE::BI__builtin_aie2p_fifo_ld_pop_1d_544_bfp16:
    case AIE::BI__builtin_aie2p_fifo_ld_pop_1d_576_bfp16:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_1d_BFP640:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_1d_BFP768: {
      NumAddrIncs = 1;
      NumOutIncs = 0;
      break;
    }
    case AIE::BI__builtin_aie2p_fifo_ld_pop_2d_576_bfp16:
    case AIE::BI__builtin_aie2p_fifo_ld_pop_2d_544_bfp16:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_BFP640:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_BFP768: {
      NumAddrIncs = 4;
      NumOutIncs = 1;
      break;
    }
    case AIE::BI__builtin_aie2p_fifo_ld_pop_3d_576_bfp16:
    case AIE::BI__builtin_aie2p_fifo_ld_pop_3d_544_bfp16:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_BFP640:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_BFP768: {
      NumAddrIncs = 7;
      NumOutIncs = 2;
      break;
    }
    }
    // Determine number of MX struct elements the intrinsic returns
    switch (BuiltinID) {
    case AIE::BI__builtin_aie2p_fifo_ld_pop_1d_544_bfp16:
    case AIE::BI__builtin_aie2p_fifo_ld_pop_1d_576_bfp16:
    case AIE::BI__builtin_aie2p_fifo_ld_pop_2d_576_bfp16:
    case AIE::BI__builtin_aie2p_fifo_ld_pop_2d_544_bfp16:
    case AIE::BI__builtin_aie2p_fifo_ld_pop_3d_576_bfp16:
    case AIE::BI__builtin_aie2p_fifo_ld_pop_3d_544_bfp16:
    case AIE::BI__builtin_aie2p_fifo_ld_pop_544_bfp16:
    case AIE::BI__builtin_aie2p_fifo_ld_pop_576_bfp16:
      MXStructCount = 2;
      break;
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_BFP640:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_1d_BFP640:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_BFP640:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_BFP640:
      MXStructCount = 3;
      break;
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_BFP768:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_1d_BFP768:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_BFP768:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_BFP768:
      MXStructCount = 8;
      break;
    default:
      llvm_unreachable("Unexpected BuiltinID");
    }

    for (unsigned I = 0; I < E->getNumArgs() - NumAddrIncs - MXStructCount; I++)
      Ops.push_back(EmitScalarExpr(E->getArg(I)));
    for (unsigned I = E->getNumArgs() - NumAddrIncs - MXStructCount,
                  NA = E->getNumArgs() - MXStructCount;
         I < NA; I++) {
      Ops.push_back(
          Builder.CreateTrunc(EmitScalarExpr(E->getArg(I)),
                              llvm::Type::getInt20Ty(getLLVMContext())));
    }

    llvm::Type *OverloadedTy = Ops[0]->getType();
    llvm::Intrinsic::ID IntrinsicID = getAIEIntrinsicFunction(BuiltinID, Arch);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID, {OverloadedTy, OverloadedTy});

    insertImplicitCasts(Ops, *F->getFunctionType(), Builder);
    Value *Val = Builder.CreateCall(F, Ops);

    for (unsigned I = 0; I < E->getNumArgs() - NumAddrIncs - MXStructCount;
         I++) {
      Value *EltVal = Builder.CreateExtractValue(Val, I);
      CreateStoreToLValue(EltVal, E->getArg(I));
    }

    switch (BuiltinID) {
    case AIE::BI__builtin_aie2p_fifo_ld_pop_2d_576_bfp16:
    case AIE::BI__builtin_aie2p_fifo_ld_pop_2d_544_bfp16:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_BFP640:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_BFP768: {
      Value *Count1 =
          Builder.CreateZExt(Builder.CreateExtractValue(Val, 3),
                             llvm::Type::getInt32Ty(getLLVMContext()));
      CreateStoreToLValue(
          Count1, E->getArg(E->getNumArgs() - 2 - MXStructCount));
      break;
    }
    case AIE::BI__builtin_aie2p_fifo_ld_pop_3d_576_bfp16:
    case AIE::BI__builtin_aie2p_fifo_ld_pop_3d_544_bfp16:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_BFP640:
    case AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_BFP768: {
      Value *Count1 =
          Builder.CreateZExt(Builder.CreateExtractValue(Val, 3),
                             llvm::Type::getInt32Ty(getLLVMContext()));
      Value *Count2 =
          Builder.CreateZExt(Builder.CreateExtractValue(Val, 4),
                             llvm::Type::getInt32Ty(getLLVMContext()));
      CreateStoreToLValue(
          Count1, E->getArg(E->getNumArgs() - 5 - MXStructCount));
      CreateStoreToLValue(
          Count2, E->getArg(E->getNumArgs() - 2 - MXStructCount));
      break;
    }
    }

    // Tracks the last store to return
    Value *LastStore = nullptr;
    // The intrinsic returns an struct of MX type. So, extract each element
    // and store it in corresponding addr.
    for (unsigned I = 0; I < MXStructCount; I++) {
      Value *EltVal = Builder.CreateExtractValue(Val, 3 + NumOutIncs + I);
      unsigned ArgInd = E->getNumArgs() - MXStructCount + I;
      LastStore = CreateStoreToLValue(EltVal, E->getArg(ArgInd));
    }
    return LastStore;
  }
  case AIE::BI__builtin_aie2ps_vshuffle_BFP640_BFP640_BFP640:
  case AIE::BI__builtin_aie2ps_vshuffle_BFP768_BFP768_BFP768:
  case AIE::BI__builtin_aie2ps_v64accfloat_to_v64mx4:
  case AIE::BI__builtin_aie2ps_v64accfloat_to_v64mx6:
  case AIE::BI__builtin_aie2ps_v64accfloat_to_v64mx9: {
    SmallVector<Value *, 3> Ops;
    unsigned MXStructCount = 0;
    // Determine number of MX struct elements the intrinsic returns
    if (BuiltinID == AIE::BI__builtin_aie2ps_vshuffle_BFP640_BFP640_BFP640 ||
        BuiltinID == AIE::BI__builtin_aie2ps_v64accfloat_to_v64mx9) {
      MXStructCount = 3;
    } else if (BuiltinID == AIE::BI__builtin_aie2ps_v64accfloat_to_v64mx4 ||
               BuiltinID == AIE::BI__builtin_aie2ps_v64accfloat_to_v64mx6) {
      MXStructCount = 4;
    } else if (BuiltinID ==
               AIE::BI__builtin_aie2ps_vshuffle_BFP768_BFP768_BFP768) {
      MXStructCount = 8;
    }

    for (unsigned I = 0; I < E->getNumArgs() - MXStructCount; I++)
      Ops.push_back(EmitScalarExpr(E->getArg(I)));

    llvm::Intrinsic::ID IntrinsicID = getAIEIntrinsicFunction(BuiltinID, Arch);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Val = Builder.CreateCall(F, Ops);
    // Tracks the last store to return
    Value *LastStore = nullptr;

    // The intrinsic returns an struct of MX type. So, extract each element and
    // store it in corresponding addr.
    for (unsigned I = 0; I < MXStructCount; I++) {
      Value *EltVal = Builder.CreateExtractValue(Val, I);
      unsigned ArgInd = E->getNumArgs() - MXStructCount + I;
      Value *EltAddr = EmitLValue(E->getArg(ArgInd)).getPointer(*this);
      LastStore = Builder.CreateDefaultAlignedStore(EltVal, EltAddr);
    }
    return LastStore;
  }
  default:
    break;
  }
  return nullptr;
}

Value *CodeGenFunction::EmitAIE2BuiltinExpr(unsigned BuiltinID,
                                            const CallExpr *E,
                                            llvm::Triple::ArchType Arch) {

  switch (BuiltinID) {
  case AIE::BI__builtin_aiev2_vabs_gtz8:
  case AIE::BI__builtin_aiev2_vabs_gtz16:
  case AIE::BI__builtin_aiev2_vabs_gtz32:
  case AIE::BI__builtin_aiev2_vbneg_ltz8:
  case AIE::BI__builtin_aiev2_vbneg_ltz16:
  case AIE::BI__builtin_aiev2_vbneg_ltz32:
  case AIE::BI__builtin_aiev2_vmaxdiff_lt8:
  case AIE::BI__builtin_aiev2_vmaxdiff_lt16:
  case AIE::BI__builtin_aiev2_vmaxdiff_lt32:
  case AIE::BI__builtin_aiev2_vmax_lt8:
  case AIE::BI__builtin_aiev2_vmax_lt16:
  case AIE::BI__builtin_aiev2_vmax_lt32:
  case AIE::BI__builtin_aiev2_vmax_ltbf16:
  case AIE::BI__builtin_aiev2_vmin_ge8:
  case AIE::BI__builtin_aiev2_vmin_ge16:
  case AIE::BI__builtin_aiev2_vmin_ge32:
  case AIE::BI__builtin_aiev2_vmin_gebf16:
  case AIE::BI__builtin_aiev2_vneg_gtz8:
  case AIE::BI__builtin_aiev2_vneg_gtz16:
  case AIE::BI__builtin_aiev2_vneg_gtz32:
  case AIE::BI__builtin_aiev2_vsub_ge8:
  case AIE::BI__builtin_aiev2_vsub_ge16:
  case AIE::BI__builtin_aiev2_vsub_ge32:
  case AIE::BI__builtin_aiev2_vsub_lt8:
  case AIE::BI__builtin_aiev2_vsub_lt16:
  case AIE::BI__builtin_aiev2_vsub_lt32:
  case AIE::BI__builtin_aiev2_get_ss:
  case AIE::BI__builtin_aiev2_get_ss_nb:
  case AIE::BI__builtin_aiev2_add_2d:
  case AIE::BI__builtin_aiev2_add_3d:
  case AIE::BI__builtin_aiev2_put_ms_nb:
  case AIE::BI__builtin_aiev2_put_ms_nb_packet_header:
  case AIE::BI__builtin_aiev2_put_ms_nb_ctrl_packet_header:
  case AIE::BI__builtin_aiev2_divstep: {
    return this->EmitAIEBuiltinExpr(BuiltinID, E, Arch);
  }
  case AIE::BI__builtin_aiev2_sparse_peek_4_and_get_pointer:
  case AIE::BI__builtin_aiev2_sparse_peek_4_set_lo:
  case AIE::BI__builtin_aiev2_sparse_pop_4_and_get_pointer:
  case AIE::BI__builtin_aiev2_sparse_pop_4_set_lo:
  case AIE::BI__builtin_aiev2_sparse_peek_8_and_get_pointer:
  case AIE::BI__builtin_aiev2_sparse_peek_8_set_lo:
  case AIE::BI__builtin_aiev2_sparse_pop_8_and_get_pointer:
  case AIE::BI__builtin_aiev2_sparse_pop_8_set_lo:
  case AIE::BI__builtin_aiev2_sparse_peek_16_and_get_pointer:
  case AIE::BI__builtin_aiev2_sparse_peek_16_set_lo:
  case AIE::BI__builtin_aiev2_sparse_pop_16_and_get_pointer:
  case AIE::BI__builtin_aiev2_sparse_pop_16_set_lo:
  case AIE::BI__builtin_aiev2_sparse_peek_16_bfloat_and_get_pointer:
  case AIE::BI__builtin_aiev2_sparse_peek_16_bfloat_set_lo:
  case AIE::BI__builtin_aiev2_sparse_pop_16_bfloat_and_get_pointer:
  case AIE::BI__builtin_aiev2_sparse_pop_16_bfloat_set_lo: {
    SmallVector<Value *, 3> Ops;
    for (unsigned I = 0; I < E->getNumArgs(); I++)
      Ops.push_back(EmitScalarExpr(E->getArg(I)));

    llvm::Intrinsic::ID IntrinsicID = getAIE2IntrinsicFunction(BuiltinID);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Val = Builder.CreateCall(F, Ops);

    Value *Ptr = Builder.CreateExtractValue(Val, 0);
    Value *Vec = Builder.CreateExtractValue(Val, 1);
    Value *Mask = Builder.CreateExtractValue(Val, 2);

    Value *VecAddr = EmitLValue(E->getArg(0)).getPointer(*this);
    Value *MaskAddr = EmitLValue(E->getArg(1)).getPointer(*this);
    Value *PtrAddr = EmitLValue(E->getArg(2)).getPointer(*this);

    Builder.CreateDefaultAlignedStore(Vec, VecAddr);
    Builder.CreateDefaultAlignedStore(Mask, MaskAddr);
    return Builder.CreateDefaultAlignedStore(Ptr, PtrAddr);
  }
  case AIE::BI__builtin_aiev2_sparse_peek_4_insert_hi:
  case AIE::BI__builtin_aiev2_sparse_pop_4_insert_hi:
  case AIE::BI__builtin_aiev2_sparse_peek_8_insert_hi:
  case AIE::BI__builtin_aiev2_sparse_pop_8_insert_hi:
  case AIE::BI__builtin_aiev2_sparse_peek_16_insert_hi:
  case AIE::BI__builtin_aiev2_sparse_pop_16_insert_hi:
  case AIE::BI__builtin_aiev2_sparse_peek_16_bfloat_insert_hi:
  case AIE::BI__builtin_aiev2_sparse_pop_16_bfloat_insert_hi: {
    SmallVector<Value *, 3> Ops;
    for (unsigned I = 0; I < E->getNumArgs(); I++)
      Ops.push_back(EmitScalarExpr(E->getArg(I)));

    llvm::Intrinsic::ID IntrinsicID = getAIE2IntrinsicFunction(BuiltinID);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Val = Builder.CreateCall(F, Ops);

    Value *Ptr = Builder.CreateExtractValue(Val, 0);
    Value *Vec = Builder.CreateExtractValue(Val, 1);
    Value *Mask = Builder.CreateExtractValue(Val, 2);

    Value *VecAddr = EmitLValue(E->getArg(0)).getPointer(*this);
    Value *MaskAddr = EmitLValue(E->getArg(1)).getPointer(*this);
    Value *PtrAddr = EmitLValue(E->getArg(4)).getPointer(*this);

    Builder.CreateDefaultAlignedStore(Vec, VecAddr);
    Builder.CreateDefaultAlignedStore(Mask, MaskAddr);
    return Builder.CreateDefaultAlignedStore(Ptr, PtrAddr);
  }
  case AIE::BI__builtin_aiev2_sparse_reset_16:
  case AIE::BI__builtin_aiev2_sparse_reset_16_and_get_pointer:
  case AIE::BI__builtin_aiev2_sparse_reset_8:
  case AIE::BI__builtin_aiev2_sparse_reset_8_and_get_pointer:
  case AIE::BI__builtin_aiev2_sparse_reset_4:
  case AIE::BI__builtin_aiev2_sparse_reset_4_and_get_pointer:
  case AIE::BI__builtin_aiev2_sparse_fill_16:
  case AIE::BI__builtin_aiev2_sparse_fill_16_and_get_pointer:
  case AIE::BI__builtin_aiev2_sparse_fill_8:
  case AIE::BI__builtin_aiev2_sparse_fill_8_and_get_pointer:
  case AIE::BI__builtin_aiev2_sparse_fill_4:
  case AIE::BI__builtin_aiev2_sparse_fill_4_and_get_pointer: {
    SmallVector<Value *, 1> Ops;
    for (unsigned I = 0; I < E->getNumArgs(); I++)
      Ops.push_back(EmitScalarExpr(E->getArg(I)));
    llvm::Intrinsic::ID IntrinsicID = getAIE2IntrinsicFunction(BuiltinID);
    assert(IntrinsicID != Intrinsic::not_intrinsic);
    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Val = Builder.CreateCall(F, Ops);
    Value *PtrAddr = EmitLValue(E->getArg(0)).getPointer(*this);
    return Builder.CreateDefaultAlignedStore(Val, PtrAddr);
  }
  default:
    break;
  }
  return nullptr;
}

Value *CodeGenFunction::EmitAIE2PBuiltinExpr(unsigned BuiltinID,
                                             const CallExpr *E,
                                             llvm::Triple::ArchType Arch) {
  switch (BuiltinID) {
  case AIE::BI__builtin_aie2p_add_2d:
  case AIE::BI__builtin_aie2p_add_3d:
  case AIE::BI__builtin_aie2p_put_ms_nb:
  case AIE::BI__builtin_aie2p_scd_expand_ACC1024_incr:
  case AIE::BI__builtin_aie2p_scd_expand_ACC2048_incr:
  case AIE::BI__builtin_aie2p_get_ss:
  case AIE::BI__builtin_aie2p_get_ss_nb:
  case AIE::BI__builtin_aie2p_vabs_gtz8:
  case AIE::BI__builtin_aie2p_vabs_gtz16:
  case AIE::BI__builtin_aie2p_vabs_gtz32:
  case AIE::BI__builtin_aie2p_vbneg_ltz8:
  case AIE::BI__builtin_aie2p_vbneg_ltz16:
  case AIE::BI__builtin_aie2p_vbneg_ltz32:
  case AIE::BI__builtin_aie2p_vmaxdiff_lt8:
  case AIE::BI__builtin_aie2p_vmaxdiff_lt16:
  case AIE::BI__builtin_aie2p_vmaxdiff_lt32:
  case AIE::BI__builtin_aie2p_vmax_lt8:
  case AIE::BI__builtin_aie2p_vmax_lt16:
  case AIE::BI__builtin_aie2p_vmax_lt32:
  case AIE::BI__builtin_aie2p_vmax_ltbf16:
  case AIE::BI__builtin_aie2p_vmin_ge8:
  case AIE::BI__builtin_aie2p_vmin_ge16:
  case AIE::BI__builtin_aie2p_vmin_ge32:
  case AIE::BI__builtin_aie2p_vmin_gebf16:
  case AIE::BI__builtin_aie2p_vneg_gtz8:
  case AIE::BI__builtin_aie2p_vneg_gtz16:
  case AIE::BI__builtin_aie2p_vneg_gtz32:
  case AIE::BI__builtin_aie2p_vsub_ge8:
  case AIE::BI__builtin_aie2p_vsub_ge16:
  case AIE::BI__builtin_aie2p_vsub_ge32:
  case AIE::BI__builtin_aie2p_vsub_lt8:
  case AIE::BI__builtin_aie2p_vsub_lt16:
  case AIE::BI__builtin_aie2p_vsub_lt32:
  case AIE::BI__builtin_aie2p_divstep:
  case AIE::BI__builtin_aie2p_v64accfloat_to_v64bfp16ebs8:
  case AIE::BI__builtin_aie2p_v64accfloat_to_v64bfp16ebs16:
  case AIE::BI__builtin_aie2p_v64bfp16ebs8_to_v64bfp16ebs16:
  case AIE::BI__builtin_aie2p_vshuffle_576_bfp16:
  case AIE::BI__builtin_aie2p_fifo_st_push_576_bfp16:
  case AIE::BI__builtin_aie2p_fifo_st_push_512_bfp16:
  case AIE::BI__builtin_aie2p_fifo_st_push_544_bfp16:
  case AIE::BI__builtin_aie2p_fifo_st_flush:
  case AIE::BI__builtin_aie2p_fifo_st_flush_conv:
  case AIE::BI__builtin_aie2p_fifo_st_flush_1d_byte:
  case AIE::BI__builtin_aie2p_fifo_st_flush_conv_1d_byte:
  case AIE::BI__builtin_aie2p_fifo_st_flush_2d_byte:
  case AIE::BI__builtin_aie2p_fifo_st_flush_conv_2d_byte:
  case AIE::BI__builtin_aie2p_fifo_st_flush_3d_byte:
  case AIE::BI__builtin_aie2p_fifo_st_flush_conv_3d_byte:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_512_unaligned:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_576_bfp16:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_544_bfp16:
  case AIE::BI__builtin_aie2p_fifo_ld_fill:
  case AIE::BI__builtin_aie2p_fifo_ld_fillx:
  case AIE::BI__builtin_aie2p_fifo_ld_popx:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_1d_512_unaligned:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_1d_576_bfp16:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_1d_544_bfp16:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_2d_512_unaligned:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_2d_576_bfp16:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_2d_544_bfp16:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_3d_512_unaligned:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_3d_576_bfp16:
  case AIE::BI__builtin_aie2p_fifo_ld_pop_3d_544_bfp16: {
    return this->EmitAIEBuiltinExpr(BuiltinID, E, Arch);
  }
  default:
    break;
  }

  return nullptr;
}

Value *CodeGenFunction::EmitAIE2PSBuiltinExpr(unsigned BuiltinID,
                                              const CallExpr *E,
                                              llvm::Triple::ArchType Arch) {
  switch (BuiltinID) {
  case AIE::BI__builtin_aie2ps_get_ss:
  case AIE::BI__builtin_aie2ps_get_ss_nb:
  case AIE::BI__builtin_aie2ps_put_ms_nb:
  case AIE::BI__builtin_aie2ps_scd_expand_ACC1024_incr:
  case AIE::BI__builtin_aie2ps_scd_expand_ACC2048_incr:
  case AIE::BI__builtin_aie2ps_add_2d:
  case AIE::BI__builtin_aie2ps_add_3d:
  case AIE::BI__builtin_aie2ps_vshuffle_BFP640_BFP640_BFP640:
  case AIE::BI__builtin_aie2ps_vshuffle_BFP768_BFP768_BFP768:
  case AIE::BI__builtin_aie2ps_v64accfloat_to_v64mx4:
  case AIE::BI__builtin_aie2ps_v64accfloat_to_v64mx6:
  case AIE::BI__builtin_aie2ps_v64accfloat_to_v64mx9:
  case AIE::BI__builtin_aie2ps_vabs_gtz8:
  case AIE::BI__builtin_aie2ps_vabs_gtz16:
  case AIE::BI__builtin_aie2ps_vabs_gtz32:
  case AIE::BI__builtin_aie2ps_vbneg_ltz8:
  case AIE::BI__builtin_aie2ps_vbneg_ltz16:
  case AIE::BI__builtin_aie2ps_vbneg_ltz32:
  case AIE::BI__builtin_aie2ps_vmaxdiff_lt8:
  case AIE::BI__builtin_aie2ps_vmaxdiff_lt16:
  case AIE::BI__builtin_aie2ps_vmaxdiff_lt32:
  case AIE::BI__builtin_aie2ps_vmax_lt8:
  case AIE::BI__builtin_aie2ps_vmax_ltfloat8:
  case AIE::BI__builtin_aie2ps_vmax_ltbfloat8:
  case AIE::BI__builtin_aie2ps_vmax_lt16:
  case AIE::BI__builtin_aie2ps_vmax_lt32:
  case AIE::BI__builtin_aie2ps_vmax_ltbfloat16:
  case AIE::BI__builtin_aie2ps_vmax_ltfloat16:
  case AIE::BI__builtin_aie2ps_vmin_ge8:
  case AIE::BI__builtin_aie2ps_vmin_gefloat8:
  case AIE::BI__builtin_aie2ps_vmin_gebfloat8:
  case AIE::BI__builtin_aie2ps_vmin_ge16:
  case AIE::BI__builtin_aie2ps_vmin_ge32:
  case AIE::BI__builtin_aie2ps_vmin_gefloat16:
  case AIE::BI__builtin_aie2ps_vmin_gebfloat16:
  case AIE::BI__builtin_aie2ps_vneg_gtz8:
  case AIE::BI__builtin_aie2ps_vneg_gtz16:
  case AIE::BI__builtin_aie2ps_vneg_gtz32:
  case AIE::BI__builtin_aie2ps_vsub_ge8:
  case AIE::BI__builtin_aie2ps_vsub_ge16:
  case AIE::BI__builtin_aie2ps_vsub_ge32:
  case AIE::BI__builtin_aie2ps_vsub_lt8:
  case AIE::BI__builtin_aie2ps_vsub_lt16:
  case AIE::BI__builtin_aie2ps_vsub_lt32:
  case AIE::BI__builtin_aie2ps_divstep:
  case AIE::BI__builtin_aie2ps_fifo_ld_fill:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_512_unaligned:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_BFP640:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_BFP768:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_1d_512_unaligned:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_1d_BFP640:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_1d_BFP768:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_512_unaligned:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_BFP640:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_2d_BFP768:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_512_unaligned:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_BFP640:
  case AIE::BI__builtin_aie2ps_fifo_ld_pop_3d_BFP768:
  case AIE::BI__builtin_aie2ps_fifo_ld_fillx:
  case AIE::BI__builtin_aie2ps_fifo_ld_popx:
  case AIE::BI__builtin_aie2ps_fifo_st_push_512:
  case AIE::BI__builtin_aie2ps_fifo_st_push_BFP384:
  case AIE::BI__builtin_aie2ps_fifo_st_push_BFP640:
  case AIE::BI__builtin_aie2ps_fifo_st_push_BFP768:
  case AIE::BI__builtin_aie2ps_fifo_st_flush:
  case AIE::BI__builtin_aie2ps_fifo_st_flush_1d_byte:
  case AIE::BI__builtin_aie2ps_fifo_st_flush_2d_byte:
  case AIE::BI__builtin_aie2ps_fifo_st_flush_3d_byte:
  case AIE::BI__builtin_aie2ps_fifo_st_flush_conv:
  case AIE::BI__builtin_aie2ps_fifo_st_flush_conv_1d_byte:
  case AIE::BI__builtin_aie2ps_fifo_st_flush_conv_2d_byte:
  case AIE::BI__builtin_aie2ps_fifo_st_flush_conv_3d_byte: {
    return this->EmitAIEBuiltinExpr(BuiltinID, E, Arch);
  }
  default:
    break;
  }
  return nullptr;
}