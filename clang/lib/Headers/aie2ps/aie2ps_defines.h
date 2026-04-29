//===- aie2ps_defines.h -----------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2PS_DEFINES_H__
#define __AIE2PS_DEFINES_H__

/// Natural alignment of vectors
#define AIE_VECTOR_LDST_ALIGN 64

// ------------------------------------------------------------
#ifndef __AIE_CORE_BUILTIN_CLZ__
#define __AIE_CORE_BUILTIN_CLZ__ 1
#endif
// ------------------------------------------------------------

// scalar compare
#define cmp_seq 0
#define cmp_sne 1
#define cmp_sge 2
#define cmp_slt 3
#define cmp_max 4
#define cmp_min 5
#define cmp_sgeu 10
#define cmp_sltu 11
#define cmp_maxu 12
#define cmp_minu 13

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

// Term negation used in standard complex operations
#define OP_TERM_NEG_COMPLEX 0x0A
// Term negation used in complex operations with first input conjugate: a-bi *
// c+di
#define OP_TERM_NEG_COMPLEX_CONJUGATE_X 0xA0
// Term negation used in complex operations with second input conjugate: a+bi *
// c-di
#define OP_TERM_NEG_COMPLEX_CONJUGATE_Y 0x50
// Term negation used in complex operations with both inputs conjugate: a-bi *
// c-di
#define OP_TERM_NEG_COMPLEX_CONJUGATE_X_Y 0xFA
// Term negation used in butterfly complex with real and imaginary parts
// conjugate operations (only for complex 16-bit * 16-bit integer)
#define OP_TERM_NEG_COMPLEX_CONJUGATE_BUTTERFLY 0xC6
// Term negation used in butterfly complex operations (only for complex 16-bit *
// 16-bit integer)
#define OP_TERM_NEG_COMPLEX_BUTTERFLY 0x9C

// ups-size
#define ups_shrt 0
#define ups_long 1

// assign high part of UPS
#define ups_nohi 0
#define ups_dohi 1

#ifdef DOXYGEN_SHOULD_SKIP_THIS
#define property(x)
#endif

#ifdef __programmers_view__
#define PV(a) a
#define HV(a)
// #define PHV(a,b) a
#else
#define PV(a)
#define HV(a) a
// #define PHV(a,b) b
#endif

#ifdef __go__
#define GO(a) a
#else
#define GO(a)
#endif

#ifdef NML_MODE
#define NML(a) a
#else
#define NML(a)
#endif

#endif // __AIE2PS_DEFINES_H__
