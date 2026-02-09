//===- aie2ps_enums.h -------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

#ifndef __AIE2PS_ENUMS_H__
#define __AIE2PS_ENUMS_H__

// ------------------------------------------------------------
// Transpose/shuffle modes

// previous interleaving and deinterleaving modes

#define T512_1x2_lo 0
#define T512_1x2_hi 1
// 1024 bit transpose modes
#define T256_2x2_lo 18
#define T256_2x2_hi 19
#define T128_4x2_lo 20
#define T128_4x2_hi 21
#define T64_8x2_lo 22
#define T64_8x2_hi 23
#define T32_16x2_lo 24
#define T32_16x2_hi 25
#define T16_32x2_lo 26
#define T16_32x2_hi 27
#define T8_64x2_lo 28
#define T8_64x2_hi 29
#define T128_2x4_lo 34
#define T128_2x4_hi 35
#define T64_4x4_lo 36
#define T64_4x4_hi 37
#define T32_8x4_lo 38
#define T32_8x4_hi 39
#define T16_16x4_lo 40
#define T16_16x4_hi 41
// #define T8_32x4_lo 42
// #define T8_32x4_hi 43
#define T64_2x8_lo 50
#define T64_2x8_hi 51
#define T32_4x8_lo 52
#define T32_4x8_hi 53
#define T16_8x8_lo 54
#define T16_8x8_hi 55
// #define T8_16x8_lo 56
// #define T8_16x8_hi 57
#define T32_2x16_lo 66
#define T32_2x16_hi 67
#define T16_4x16_lo 68
#define T16_4x16_hi 69
// #define T8_8x16_lo 70
// #define T8_8x16_hi 71
#define T16_2x32_lo 82
#define T16_2x32_hi 83
// #define T8_4x32_lo 84
// #define T8_4x32_hi 85
#define T8_2x64_lo 98
#define T8_2x64_hi 99
// 512 bit transpose modes
// #define T128_2x2 108
#define T64_4x2 106
#define T32_8x2 104
#define T16_16x2 102
// #define T8_32x2 100
#define T64_2x4 92
#define T32_4x4 90
#define T16_8x4 88
#define T8_16x4 86
#define T32_2x8 76
#define T16_4x8 74
#define T8_8x8 72
#define T16_2x16 60
#define T8_4x16 58
// define T8_2x32 44
//  256 bit transpose modes
#define T16_8x2 105
#define T16_4x4 91
#define T8_8x4 89
#define T16_2x8 77
#define T8_4x8 75
// #define T64_2x2 109
// #define T32_4x2 107
// #define T8_16x2 103
// #define T32_2x4 93
// #define T8_2x16 61
//  128 bit transpose modes
#define T16_4x2 87
#define T16_2x4 101
// 1024 bit transpose modes
#define T8_1x4_flip 45
#define T16_1x2_flip 59
#define T32_1x16_flip 73

// Emulated modes for sparse MAC porting
#define T8_8x16_hilo -1
#define T8_16x8_hilo -2

#define T8_64x2 T8_64x2_lo
#define T16_32x2 T16_32x2_lo
#define T32_16x2 T32_16x2_lo
#define T64_8x2 T64_8x2_lo
#define T128_4x2 T128_4x2_lo
#define T256_2x2 T256_2x2_lo
#define T128_2x4 T128_2x4_lo
#define T64_2x8 T64_2x8_lo
#define T32_2x16 T32_2x16_lo
#define T16_2x32 T16_2x32_lo
#define T8_2x64 T8_2x64_lo
#define T512_1x2 T512_1x2_lo
#define T16_16x4 T16_16x4_lo
#define T16_4x16 T16_4x16_lo
#define T32_8x4 T32_8x4_lo
#define T32_4x8 T32_4x8_lo

#define DINTLV_lo_8o16 T8_64x2_lo
#define DINTLV_lo_16o32 T16_32x2_lo
#define DINTLV_lo_32o64 T32_16x2_lo
#define DINTLV_lo_64o128 T64_8x2_lo
#define DINTLV_lo_128o256 T128_4x2_lo
#define DINTLV_lo_256o512 T256_2x2_lo
#define DINTLV_lo_512o1024 T512_1x2_lo
#define DINTLV_hi_8o16 T8_64x2_hi
#define DINTLV_hi_16o32 T16_32x2_hi
#define DINTLV_hi_32o64 T32_16x2_hi
#define DINTLV_hi_64o128 T64_8x2_hi
#define DINTLV_hi_128o256 T128_4x2_hi
#define DINTLV_hi_256o512 T256_2x2_hi
#define DINTLV_hi_512o1024 T512_1x2_hi

#define INTLV_lo_8o16 T8_2x64_lo
#define INTLV_lo_16o32 T16_2x32_lo
#define INTLV_lo_32o64 T32_2x16_lo
#define INTLV_lo_64o128 T64_2x8_lo
#define INTLV_lo_128o256 T128_2x4_lo
#define INTLV_lo_256o512 T256_2x2_lo
#define INTLV_lo_512o1024 T512_1x2_lo
#define INTLV_hi_8o16 T8_2x64_hi
#define INTLV_hi_16o32 T16_2x32_hi
#define INTLV_hi_32o64 T32_2x16_hi
#define INTLV_hi_64o128 T64_2x8_hi
#define INTLV_hi_128o256 T128_2x4_hi
#define INTLV_hi_256o512 T256_2x2_hi
#define INTLV_hi_512o1024 T512_1x2_hi

// ------------------------------------------------------------
// Parameters for UPS and SRS

#define ACC_MODE_32 0
#define ACC_MODE_64 1

#define SCALE_SIZE_HALF 0
#define SCALE_SIZE_FULL 1

// ------------------------------------------------------------
// Accumulator modes

#define ADDSUB_ADD 0
#define ADDSUB_SUB 1

#define ACCMASK_ZERO 1
#define ACCMASK_PASS 0

// ------------------------------------------------------------
// SCD positions

#define SCD_0 0
#define SCD_1 1
#define SCD_2 2
#define SCD_3 3

// ------------------------------------------------------------
// Vector adder modes

#define VADD 0
#define VSUB 1
#define VSEL 2
#define VADDSUB 3
#define VBAND 4
#define VBOR 5
#define VABS_GTZ 6
#define VMAXDIFF_LT 7
#define VSUB_LT 8
#define VSUB_GE 9
#define VBNEG_LTZ 10
#define VEQZ 11
#define VMIN_GE 12
#define VMAX_LT 13
#define VLT 14
#define VNEG_GTZ 15
#define VGE 16
#define VMIN_GE_BF16 20
#define VMIN_GE_FP16 21
#define VMIN_GE_BF8 22
#define VMIN_GE_FP8 23
#define VMAX_LT_BF16 24
#define VMAX_LT_FP16 25
#define VMAX_LT_BF8 26
#define VMAX_LT_FP8 27
#define VLT_BF16 28
#define VLT_FP16 29
#define VLT_BF8 30
#define VLT_FP8 31
#define VGE_BF16 32
#define VGE_FP16 33
#define VGE_BF8 34
#define VGE_FP8 35

// ------------------------------------------------------------
// scalar-to-vector modes

#define S2V_WIDE_8 0
#define S2V_WIDE_16 1
#define S2V_WIDE_32 2
#define S2V_WIDE_64 3
#define S2V_WIDE_128 4

#define S2V_MODE_ZERO 0
#define S2V_MODE_UPD 1
#define S2V_MODE_BCST 2
#define S2V_MODE_SHL 3
#define S2V_MODE_SHR 4
#define S2V_MODE_ONE 7

// ------------------------------------------------------------
// SRS modes

#define SAT_MODE_TRUNC 0
#define SAT_MODE_SAT 1
#define SAT_MODE_SYMSAT 3

// ------------------------------------------------------------
// Packing/Unpacking sizes

#define PACK_SIZE_4 0
#define PACK_SIZE_8 1

// ---------------------------------------------------------------------
// Parameter dynamic sign

#define DYN_SIGN_DYNAMIC 0
#define DYN_SIGN_SIGNED 1

// ---------------------------------------------------------------------
// MAC control

#define AMODE_I32 0
#define AMODE_I64 1
#define AMODE_FP32 2

#define BMODE_FP32_BFP13 0
#define BMODE_FP32_BFP16 1
#define BMODE_FP32_BF9 2
#define BMODE_FP32_BF20 3

#define BMODE_I32_8x8 0   /*           AMODE_I32              */
#define BMODE_I32_16x16 1 /*           AMODE_I32              */

#define BMODE_I64_16x16 1 /*                    AMODE_I64     */
#define BMODE_I64_32x16 0 /*                    AMODE_I64     */

#define BMODE_ACC_DEFAULT_I32 BMODE_I32_8x8
#define BMODE_ACC_DEFAULT_I64 BMODE_I64_32x16
#define BMODE_ACC_DEFAULT_FP32 BMODE_FP32_BF20

#define PMODE_8x8_1_8x8_8x8 0
#define PMODE_8x8_1_4x8_8x16 1
#define PMODE_16x16_1_8x2_2x8 2
#define PMODE_16x16_2_4x4_4x8 3

#define PMODE_8x8_1_elem_2 4
#define PMODE_16x16_1_elem 5
#define PMODE_16x16_2_elem_2 6

#define PMODE_8x8_1_conv_8x8_8ch 7
#define PMODE_8x8_1_conv_64x8 8
#define PMODE_16x16_2_conv_32x4 9
#define PMODE_16x16_2_conv_4x4_8ch 10

#define PMODE_32x16_2_4x2_2x8 11
#define PMODE_32x16_2_elem_cplx 12
#define PMODE_16x16_2_elem_2_cplx 13

#define PMODE_8x8_1_4x8_8x16T 14

#define PMODE_BFP13xBFP13_1_4x16_16x16T 15
#define PMODE_BF9xBF9_1_8x8_8x8 16
#define PMODE_BF20xBF20_1_elem_1 17
#define PMODE_BF20xBF20_1_4x8_8x8 18

// --------
#define PMODE_LAST_NON_SPARSE 18
// --------

#define PMODE_8x8_1_8x16_16x8T_sparse 19
#define PMODE_8x8_1_4x16_16x16T_sparse 20
#define PMODE_16x16_2_4x8_8x8T_sparse 21
#define PMODE_BF9xBF9_1_4x16_16x16T_sparse 22
#define PMODE_BF20xBF20_1_4x16_16x8T_sparse 23

// --------

#define VARIANT_8x8_1_8x8_8x8 0
#define VARIANT_8x8_1_elem_2 1
#define VARIANT_8x8_1_conv_8x8_8ch 2
#define VARIANT_8x8_1_conv_64x8 4
#define VARIANT_8x8_1_8x16_16x8T_sparse 5
#define VARIANT_8x8_1_4x8_8x16 6
#define VARIANT_8x8_1_4x16_16x16T_sparse 7

#define VARIANT_16x16_1_8x2_2x8 0
#define VARIANT_16x16_1_elem 1

#define VARIANT_16x16_2_4x4_4x8 0
#define VARIANT_16x16_2_elem_2 2
#define VARIANT_16x16_2_conv_32x4 3
#define VARIANT_16x16_2_conv_4x4_8ch 1
#define VARIANT_16x16_2_elem_2_cplx 4
#define VARIANT_16x16_2_4x8_8x8T_sparse 5

#define VARIANT_BF20xBF20_1_elem_1 1
#define VARIANT_BF20xBF20_1_4x8_8x8 2
#define VARIANT_BF20xBF20_1_4x16_16x8T_sparse 3

#define VARIANT_BF9xBF9_1_8x8_8x8 0
#define VARIANT_BF9xBF9_1_4x16_16x16T_sparse 1

#define VARIANT_32x16_2_4x2_2x8 0
#define VARIANT_32x16_2_elem_cplx 1

#define VARIANT_BFP16xBFP16_1_4x8_8x16T 6

#define VARIANT_BFP13xBFP13_1_4x16_16x16T 0

#define MUL_BCAST_SZ_8 0
#define MUL_BCAST_SZ_16 1
#define MUL_BCAST_SZ_32 2

#define VCTRL_TYPE_INT 0
#define VCTRL_TYPE_FP 1
#define VCTRL_TYPE_BFP13 2
#define VCTRL_TYPE_BFP16 3

#define VCTRL_INST_VMAC 0
#define VCTRL_INST_VADDMAC 1
#define VCTRL_INST_VMUL 2
#define VCTRL_INST_VACC 3
#define VCTRL_INST_VMOV 4
#define VCTRL_INST_VNEG 5
#define VCTRL_INST_VCLR 6
#define VCTRL_INST_IDLE 7

#define VCTRL_SIZE_FULL 0
#define VCTRL_SIZE_HALF 1

#define VCTRL_A_X 0
#define VCTRL_A_Y 1

#define VCTRL_B_X 0
#define VCTRL_B_Y 1
#define VCTRL_B_R 2
#define VCTRL_B_QX 3
#define VCTRL_B_QY 4
// ------------------------------------------------------------
// Load 4x sizes

#define LOAD_4x16 0
#define LOAD_4x32 1
#define LOAD_4x64 2

// ------------------------------------------------------------
// Load 4x pointer offset

#define LOAD_4x_POINTER_LO 0
#define LOAD_4x_POINTER_HI 1

// ------------------------------------------------------------
// Load FIFO

#define _FIFO_LD_FILL 0
#define _FIFO_LD_POP 1
#define _FIFO_LD_FILLX 2
#define _FIFO_LD_POPX 3

#define _FIFO_LD_X 0
#define _FIFO_LD_QX 1
#define _FIFO_LD_EX 2
#define _FIFO_LD_FEX 3
#define _FIFO_LD_FEX_BFP11 4

#define FIFO_STEP_INT_1B 0
#define FIFO_STEP_INT_2B 1
#define FIFO_STEP_INT_4B 2

// ------------------------------------------------------------
// Store FIFO

#define _FIFO_ST_FLUSH 0
#define _FIFO_ST_PUSH 1

#define _FIFO_ST_X 0
#define _FIFO_ST_EX 1
#define _FIFO_ST_FEX 2
#define _FIFO_ST_FEW 3
#define _FIFO_ST_FEX_BFP11 4
#define _FIFO_ST_FEW_BFP11 5

// ------------------------------------------------------------
// BFP accuracy modes

#define BFP_ACC_MODE_16 0
#define BFP_ACC_MODE_13 1
#define BFP_ACC_MODE_11 2

// ------------------------------------------------------------
// Convert from fp32 modes

#define FP32_TO_MODE_BF16 0
#define FP32_TO_MODE_FP16 1
#define FP32_TO_MODE_BF8 2
#define FP32_TO_MODE_FP8 3

#define FP32_FROM_MODE_BF16 0
#define FP32_FROM_MODE_FP16 1

#define SCD_RANGE_2 0
#define SCD_RANGE_4 1

#define FP8_FORMAT_IEEE 0
#define FP8_FORMAT_EXT1 1
#define FP8_FORMAT_EXT2 2

#define FP8_OVF_MODE_NAN 0
#define FP8_OVF_MODE_SAT 1

#define FP8_HUGE_OFFSET 0
#define FP8_INF_OFFSET 1

#define ACTIVITY_BFP16 0
#define ACTIVITY_BFP13 1
#define ACTIVITY_INT 2
#define ACTIVITY_FP 3

// ------------------------------------------------------------
// Control and Status Registers

#define crSat 0
#define crRnd 1
#define crFPMask 2
#define crSCDEn 3
#define crMCDEn 4
#define crUnpackSize 5
#define crPackSize 6
#define crUPSMode 7
#define crSRSMode 8
#define crF2IMask 9
#define crF2FMask 10

#define vaddSign0 11
#define vaddSign1 12
#define unpackSign0 13
#define unpackSign1 14
#define packSign0 15
#define packSign1 16
#define upsSign0 17
#define upsSign1 18
#define srsSign0 19
#define srsSign1 20
#define crBF8conf 21
#define crFP8conf 22
#define crFPConvSat 23

#define crFPNlfMask 24
#define crFPCnvFx2FlMask 25
#define crFPCnvFl2FxMask 26
#define crF2BMask 27

#define srCarry 28
#define srSS0 29
#define srMS0 30
#define srSRS_of 31
#define srUPS_of 32
#define srSparse_of 33
#define srFifo_of 34
#define srFifo_uf 35
#define srFPFlags 36
#define srF2IFlags 37
#define srF2FFlags 38
#define srF2BFlags 39
#define srFPNlf 40
#define srFPCnvFx2Fl 41
#define srFPCnvFl2Fx 42

#endif // __AIE2PS_ENUMS_H__
