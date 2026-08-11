//===- aie2p-vector-shuffle.s -------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vshuffle transposes the 1024-bit (s1, s2) pair and writes one 512-bit half.
// The mode selects the shape, and the modes below are named in aie2p_enums.h,
// so each expected value here is read off the NAME rather than off the
// implementation:
//
//  - 22/23 are T512_1x2_lo/_hi, aie_api's bypass. lo must be s1 exactly and hi
//    must be s2 exactly. This is what fixes the order of the pair: if s2 were
//    taken as the low half the two would swap.
//
//  - 4 is T32_16x2_lo, alias DINTLV_lo_32o64. Deinterleave takes the even
//    32-bit words, so s1's even words land in the low half of the result and
//    s2's in the high half.
//
//  - 16 is T32_2x16_lo, alias INTLV_lo_32o64. Interleave zips instead, so the
//    words alternate s1, s2, s1, s2.
//
//  - 0 is T8_64x2_lo, alias DINTLV_lo_8o16 -- the same deinterleave one
//    granularity down, which is why the inputs are broadcasts of a word whose
//    four BYTES differ. A byte-granularity mode reads F0,F2 where the 32-bit
//    one reads whole words.
//
// Both destination forms are exercised: x for vec_shuffle_x, and an accumulator
// quarter for vec_shuffle_bm, which shares the body.
//
// Every byte of both inputs has its high nibble set, so no permutation of them
// can produce a leading zero -- the register dump would drop it.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --coverage | FileCheck %s

// Mode 24 is T16_16x4_lo, the first of the NL/FFT/permute-reduction shapes,
// whose R x C does not tile the pair. It is not extrapolated from the
// transposes, so it says which mode it wanted and stops.
// RUN: llvm-mc -triple aie2p -filetype=obj --defsym BADMODE=1 %s -o %t.bad.o
// RUN: not llvm-aie-run %t.bad.o 2>&1 | FileCheck %s --check-prefix=BADMODE

	.text
	.globl _start
_start:
	movxm	r5, #0xF3F2F1F0
	movxm	r6, #0xF7F6F5F4
	movxm	r0, #0
	movxm	r1, #4
	movxm	r2, #16
	movxm	r3, #22
	movxm	r4, #23
	nop
	nop
	nop
	nop
	nop
	nop
	vbcst.32	x0, r5
	vbcst.32	x1, r6
	nop
	nop
	nop
	nop
	nop
	nop
.ifdef BADMODE
	movxm	r0, #24
	nop
	nop
	nop
	nop
	nop
	nop
	vshuffle	x2, x0, x1, r0
.else
	vshuffle	x2, x0, x1, r0
	vshuffle	x3, x0, x1, r1
	vshuffle	x4, x0, x1, r2
	vshuffle	x5, x0, x1, r3
	vshuffle	x6, x0, x1, r4
	// The bm form of the same bypass: it must reproduce s1 like x5 does.
	vshuffle	bmll0, x0, x1, r3
.endif
	nop
	nop
	nop
	nop
	nop
	nop
	done

// CHECK-DAG: modelled   VSHUFFLE_vec_shuffle_x
// CHECK-DAG: modelled   VSHUFFLE_vec_shuffle_bm

// The dump names a 512-bit x register by its two 256-bit halves, wl<N> and
// wh<N>, so each result below is two lines of eight words.

// The inputs, so the permutations below are read against something visible.
// CHECK-DAG: wl0 = 0x{{(F3F2F1F0){8}$}}
// CHECK-DAG: wh0 = 0x{{(F3F2F1F0){8}$}}
// CHECK-DAG: wl1 = 0x{{(F7F6F5F4){8}$}}
// CHECK-DAG: wh1 = 0x{{(F7F6F5F4){8}$}}

// mode 0, DINTLV_lo_8o16: even bytes. s1 contributes F0,F2 repeating to the low
// half and s2 contributes F4,F6 to the high.
// CHECK-DAG: wl2 = 0x{{(F2F0){16}$}}
// CHECK-DAG: wh2 = 0x{{(F6F4){16}$}}

// mode 4, DINTLV_lo_32o64: even words, s1's low and s2's high.
// CHECK-DAG: wl3 = 0x{{(F3F2F1F0){8}$}}
// CHECK-DAG: wh3 = 0x{{(F7F6F5F4){8}$}}

// mode 16, INTLV_lo_32o64: alternating, so both halves read the same pair.
// CHECK-DAG: wl4 = 0x{{(F7F6F5F4F3F2F1F0){4}$}}
// CHECK-DAG: wh4 = 0x{{(F7F6F5F4F3F2F1F0){4}$}}

// modes 22 and 23, bypass: exactly s1, and exactly s2.
// CHECK-DAG: wl5 = 0x{{(F3F2F1F0){8}$}}
// CHECK-DAG: wh5 = 0x{{(F3F2F1F0){8}$}}
// CHECK-DAG: wl6 = 0x{{(F7F6F5F4){8}$}}
// CHECK-DAG: wh6 = 0x{{(F7F6F5F4){8}$}}
// CHECK-DAG: bmll0 = 0x{{(F3F2F1F0){16}$}}

// BADMODE: mode 24 is not one of the transpose shapes this models
