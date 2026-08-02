//===- AIE2PSubRegRanges.inc - Sub-register bit ranges ----------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Where each sub-register index sits inside its parent, indexed by the
// sub-register index number.
//
// GENERATED, then checked in, and the duplication is deliberate rather than
// lazy. TableGen already emits exactly this as `SubRegIdxRangeTable` in
// AIE2PGenRegisterInfo.inc -- but `static`, inside GET_REGINFO_TARGET_DESC,
// which is the CodeGen layer. This simulator is MC-only on purpose: it is
// reached through a C ABI because a Peano dist ships libLLVM.so and no LLVM
// headers, so it cannot pull in TargetRegisterInfo to read the table it needs.
//
// Exposing the table through MCRegisterInfo instead would delete this file. It
// is a small change but not a local one -- it touches MCRegisterInfo and the
// InitMCRegisterInfo signature, which every target's generated code calls -- so
// it is an upstream proposal rather than a drive-by, and this stands until that
// is decided.
//
// Regenerate with the extractor in the sub-register composition commit; the
// self-check in AIERegisterFile fails loudly if this drifts from the MC layer
// it is paired with, which is what makes checked-in generated data safe here.
//
// An offset of kNoContiguousRange means the index does not name one contiguous
// run of bits (the composed vecmask indices do this). Those are refused rather
// than approximated.
//
//===----------------------------------------------------------------------===//

// {offset, size} per sub-register index. Index 0 is the invalid index.
static constexpr AIESubRegRange AIE2PSubRegRanges[] = {
    {kNoContiguousRange, 65535},
    {256, 256},  // sub_256_hi
    {0, 256},  // sub_256_lo
    {512, 512},  // sub_512_acc_hi
    {0, 512},  // sub_512_acc_lo
    {512, 512},  // sub_512_hi
    {768, 256},  // sub_512_hi_256_hi
    {512, 256},  // sub_512_hi_256_lo
    {0, 512},  // sub_512_lo
    {1024, 1024},  // sub_1024_acc_hi
    {0, 1024},  // sub_1024_acc_lo
    {1044, 32},  // sub_avail
    {512, 64},  // sub_bfp16_e
    {0, 512},  // sub_bfp16_x
    {576, 576},  // sub_bfp576_hi
    {1088, 64},  // sub_bfp576_hi_bfp16_e
    {576, 512},  // sub_bfp576_hi_bfp16_x
    {0, 576},  // sub_bfp576_lo
    {0, 32},  // sub_bfp_exp
    {32, 256},  // sub_bfp_vec
    {60, 20},  // sub_dim_count
    {20, 20},  // sub_dim_size
    {40, 20},  // sub_dim_stride
    {64, 32},  // sub_e_vecmaskexp_352
    {0, 640},  // sub_even_vecmask_640
    {0, 704},  // sub_even_vecmaskexp_704
    {20, 1024},  // sub_fifo
    {80, 80},  // sub_hi_dim
    {32, 32},  // sub_hi_exp
    {512, 512},  // sub_hi_fifo
    {64, 64},  // sub_hi_mask
    {352, 352},  // sub_hi_vecmaskexp_352
    {0, 32},  // sub_l_even
    {32, 32},  // sub_l_odd
    {0, 80},  // sub_lo_dim
    {0, 32},  // sub_lo_exp
    {0, 512},  // sub_lo_fifo
    {0, 64},  // sub_lo_mask
    {0, 352},  // sub_lo_vecmaskexp_352
    {0, 20},  // sub_mod
    {640, 640},  // sub_odd_vecmask_640
    {704, 704},  // sub_odd_vecmaskexp_704
    {0, 20},  // sub_ptr
    {0, 64},  // sub_q
    {0, 64},  // sub_q_vecmaskexp_352
    {512, 128},  // sub_sparse_q
    {0, 512},  // sub_sparse_x
    {64, 256},  // sub_w
    {96, 256},  // sub_w_vecmaskexp_352
    {1536, 512},  // sub_1024_acc_hi_then_sub_512_acc_hi
    {1024, 512},  // sub_1024_acc_hi_then_sub_512_acc_lo
    {832, 256},  // sub_bfp576_hi_bfp16_x_then_sub_256_hi
    {576, 256},  // sub_bfp576_hi_bfp16_x_then_sub_256_lo
    {1120, 32},  // sub_bfp576_hi_bfp16_e_then_sub_hi_exp
    {1088, 32},  // sub_bfp576_hi_bfp16_e_then_sub_lo_exp
    {416, 32},  // sub_hi_vecmaskexp_352_then_sub_e_vecmaskexp_352
    {352, 64},  // sub_hi_vecmaskexp_352_then_sub_q_vecmaskexp_352
    {448, 256},  // sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352
    {768, 32},  // sub_odd_vecmaskexp_704_then_sub_e_vecmaskexp_352
    {1056, 352},  // sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352
    {704, 352},  // sub_odd_vecmaskexp_704_then_sub_lo_vecmaskexp_352
    {704, 64},  // sub_odd_vecmaskexp_704_then_sub_q_vecmaskexp_352
    {800, 256},  // sub_odd_vecmaskexp_704_then_sub_w_vecmaskexp_352
    {1120, 32},  // sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_e_vecmaskexp_352
    {1056, 64},  // sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_q_vecmaskexp_352
    {1152, 256},  // sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352
    {896, 256},  // sub_odd_vecmask_640_then_sub_256_hi
    {640, 256},  // sub_odd_vecmask_640_then_sub_256_lo
    {704, 64},  // sub_odd_vecmask_640_then_sub_hi_mask
    {640, 64},  // sub_odd_vecmask_640_then_sub_lo_mask
    {1152, 128},  // sub_odd_vecmask_640_then_sub_sparse_q
    {640, 512},  // sub_odd_vecmask_640_then_sub_sparse_x
    {140, 20},  // sub_hi_dim_then_sub_dim_count
    {100, 20},  // sub_hi_dim_then_sub_dim_size
    {120, 20},  // sub_hi_dim_then_sub_dim_stride
    {80, 20},  // sub_hi_dim_then_sub_mod
    {kNoContiguousRange, 288},  // sub_hi_exp_sub_256_hi
    {kNoContiguousRange, 288},  // sub_lo_exp_sub_256_lo
    {kNoContiguousRange, 1024},  // sub_256_lo_sub_256_hi_sub_bfp576_hi_bfp16_x
    {kNoContiguousRange, 288},  // sub_bfp576_hi_bfp16_e_then_sub_hi_exp_sub_bfp576_hi_bfp16_x_then_sub_256_hi
    {kNoContiguousRange, 288},  // sub_bfp576_hi_bfp16_e_then_sub_lo_exp_sub_bfp576_hi_bfp16_x_then_sub_256_lo
    {64, 288},  // sub_e_vecmaskexp_352_sub_w_vecmaskexp_352
    {kNoContiguousRange, 320},  // sub_q_vecmaskexp_352_sub_w_vecmaskexp_352
    {kNoContiguousRange, 64},  // sub_e_vecmaskexp_352_sub_hi_vecmaskexp_352_then_sub_e_vecmaskexp_352
    {kNoContiguousRange, 128},  // sub_q_vecmaskexp_352_sub_hi_vecmaskexp_352_then_sub_q_vecmaskexp_352
    {kNoContiguousRange, 512},  // sub_w_vecmaskexp_352_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352
    {416, 288},  // sub_hi_vecmaskexp_352_then_sub_e_vecmaskexp_352_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352
    {kNoContiguousRange, 320},  // sub_hi_vecmaskexp_352_then_sub_q_vecmaskexp_352_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352
    {kNoContiguousRange, 576},  // sub_w_vecmaskexp_352_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352_sub_e_vecmaskexp_352_sub_hi_vecmaskexp_352_then_sub_e_vecmaskexp_352
    {kNoContiguousRange, 640},  // sub_w_vecmaskexp_352_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352_sub_q_vecmaskexp_352_sub_hi_vecmaskexp_352_then_sub_q_vecmaskexp_352
    {kNoContiguousRange, 64},  // sub_odd_vecmaskexp_704_then_sub_e_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_e_vecmaskexp_352
    {768, 288},  // sub_odd_vecmaskexp_704_then_sub_e_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_w_vecmaskexp_352
    {kNoContiguousRange, 128},  // sub_odd_vecmaskexp_704_then_sub_q_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_q_vecmaskexp_352
    {kNoContiguousRange, 320},  // sub_odd_vecmaskexp_704_then_sub_q_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_w_vecmaskexp_352
    {kNoContiguousRange, 512},  // sub_odd_vecmaskexp_704_then_sub_w_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352
    {1120, 288},  // sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_e_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352
    {kNoContiguousRange, 320},  // sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_q_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352
    {kNoContiguousRange, 1024},  // sub_w_vecmaskexp_352_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_w_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352
    {kNoContiguousRange, 576},  // sub_odd_vecmaskexp_704_then_sub_w_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_e_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_e_vecmaskexp_352
    {kNoContiguousRange, 640},  // sub_odd_vecmaskexp_704_then_sub_w_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_q_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_q_vecmaskexp_352
    {kNoContiguousRange, 1152},  // sub_w_vecmaskexp_352_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352_sub_e_vecmaskexp_352_sub_hi_vecmaskexp_352_then_sub_e_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_w_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_e_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_e_vecmaskexp_352
    {kNoContiguousRange, 1280},  // sub_w_vecmaskexp_352_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352_sub_q_vecmaskexp_352_sub_hi_vecmaskexp_352_then_sub_q_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_w_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_w_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_q_vecmaskexp_352_sub_odd_vecmaskexp_704_then_sub_hi_vecmaskexp_352_then_sub_q_vecmaskexp_352
    {kNoContiguousRange, 320},  // sub_hi_mask_sub_256_hi
    {kNoContiguousRange, 320},  // sub_lo_mask_sub_256_lo
    {kNoContiguousRange, 1024},  // sub_256_lo_sub_256_hi_sub_odd_vecmask_640_then_sub_sparse_x
    {kNoContiguousRange, 320},  // sub_odd_vecmask_640_then_sub_hi_mask_sub_odd_vecmask_640_then_sub_256_hi
    {kNoContiguousRange, 320},  // sub_odd_vecmask_640_then_sub_lo_mask_sub_odd_vecmask_640_then_sub_256_lo
};
