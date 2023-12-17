//===- aiev2_defines.h ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV2_DEFINES_H__
#define __AIEV2_DEFINES_H__

#define __AIE2__
#define __AIE_ARCH__ 20
#define __AIE_ARCH_NAME__ AIE2
#define __AIE_ARCH_NAME_STR__ "AIE2"

// Natural alignment of vectors
#define AIE_VECTOR_LDST_ALIGN 32

// Control Registers
#define crVaddSign 0
#define crF2FMask 1
#define crF2IMask 2
#define crFPMask 3
#define crMCDEn 4
#define crPackSign 5
#define crRnd 6
#define crSCDEn 7
#define crSRSSign 8
#define crSat 9
#define crUPSSign 10
#define crUnpackSign 11

// Rounding Modes
// No rounding - Truncate LSB, always round down (towards negative infinity)
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

// Scalar Compare
#define cmp_seq 0
#define cmp_sne 1
#define cmp_sge 2
#define cmp_slt 3
#define cmp_sgeu 6
#define cmp_sltu 7

// ups-size
#define ups_shrt 0
#define ups_long 1

// assign high part of UPS
#define ups_nohi 0
#define ups_dohi 1

#endif // __AIEV2_DEFINES_H__
