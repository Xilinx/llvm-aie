//===- aie2p_defines.h ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2P_DEFINES_H__
#define __AIE2P_DEFINES_H__

// Natural alignment of vectors
#define AIE_VECTOR_LDST_ALIGN 64

// scalar compare
#define cmp_seq 0
#define cmp_sne 1
#define cmp_sge 2
#define cmp_slt 3
#define cmp_sgeu 6
#define cmp_sltu 7

// Rounding Modes

// No rounding - Truncate LSB, always round down (towards negative
//! infinity)
#define rnd_floor 0
// No rounding - Always round up (towards positive infinity)
#define rnd_ceil 1
// No rounding - Truncate LSB, always round towards 0
#define rnd_sym_floor 2
// No rounding - Always round up towards infinity
#define rnd_sym_ceil 3
// Round halfway towards negative infinity
#define rnd_neg_inf 8
// Round halfway towards positive infinity
#define rnd_pos_inf 9
// Round halfway towards zero (away from infinity)
#define rnd_sym_zero 10
// Round halfway towards infinity (away from zero)
#define rnd_sym_inf 11
// Round halfway towards nearest even number
#define rnd_conv_even 12
// Round halfway towards nearest odd number
#define rnd_conv_odd 13

#define crSat 0
#define crRnd 1
#define crFPMask 2
#define crF2IMask 3
#define crF2FMask 4
#define crF2BMask 5
#define crSRSMode 6
#define crUPSMode 7
#define crUnpackSize 8
#define crPackSize 9

#define srsSign0 10
#define srsSign1 11
#define upsSign0 12
#define upsSign1 13
#define packSign0 14
#define packSign1 15
#define unpackSign0 16
#define unpackSign1 17
#define vaddSign0 18
#define vaddSign1 19

#define crSCDEn 20
#define crMCDEn 21

#define crFPNlfMask 22
#define crFPCnvFx2FlMask 23
#define crFPCnvFl2FxMask 24

#define srCarry 25
#define srSS0 26
#define srMS0 27
#define srSRS_of 28
#define srUPS_of 29
#define srSparse_of 30
#define srFifo_of 31
#define srFifo_uf 32
#define srFPFlags 33
#define srF2IFlags 34
#define srF2FFlags 35
#define srF2BFlags 36
#define srFPNlf 37
#define srFPCnvFx2Fl 38
#define srFPCnvFl2Fx 39
// Term negation used in standard complex operations
#define OP_TERM_NEG_COMPLEX 0x0A
// Term negation used in complex operations with real part conjugate
#define OP_TERM_NEG_COMPLEX_CONJUGATE_X 0xA0
// Term negation used in complex operations with imaginary part conjugate
#define OP_TERM_NEG_COMPLEX_CONJUGATE_Y 0x50
// Term negation used in complex operations with real and imaginary parts
// conjugate
#define OP_TERM_NEG_COMPLEX_CONJUGATE_X_Y 0xFA
// Term negation used in butterfly complex with real and imaginary parts
// conjugate operations (only for complex 16-bit * 16-bit)
#define OP_TERM_NEG_COMPLEX_CONJUGATE_BUTTERFLY 0xC6
// Term negation used in butterfly complex operations (only for complex 16-bit *
// 16-bit)
#define OP_TERM_NEG_COMPLEX_BUTTERFLY 0x9C

// ups-size
#define ups_shrt 0
#define ups_long 1
// assign high part of UPS
#define ups_nohi 0
#define ups_dohi 1

#endif // __AIE2P_DEFINES_H__
