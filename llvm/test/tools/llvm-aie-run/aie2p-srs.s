//===- aie2p-srs.s ------------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vsrs.Nx -- shift an accumulator lane right, round, saturate, narrow. Reads
// three control registers: crSRSMode for the lane width (as vups reads
// crUPSMode), and crRnd/crSat for the arithmetic. Encodings are llvm-aie's
// own, from clang/lib/Headers/aie2p/aie2p_defines.h and aie_api's
// saturation_mode.
//
// The lanes are 5, 6, 7 and -6 at a shift of 2, chosen so one lane sits below
// the half, one exactly on it, one above, and one is a negative tie. That is
// what separates the directional modes from the nearest ones -- with only ties
// present, floor and neg_inf agree and the test proves nothing.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs | FileCheck %s

	.text
	.globl _start
_start:
	movxm	r0, #0x00060005
	movxm	r1, #0xFFFA0007
	vbcst.64	x0, r1:r0
	movxm	r2, #0
	mov	crupsmode, r2
	mov	crsrsmode, r2
	mov	crsat, r2
	mov	s0, r2
	movxm	r3, #2
	mov	s1, r3
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vups.2x	bmll0, wl0, s0, upssign1
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// floor -- toward -inf
	movxm	r4, #0
	mov	crrnd, r4
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vsrs.2x	wl1, bmll0, s1, srssign1

	// ceil -- toward +inf
	movxm	r4, #1
	mov	crrnd, r4
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vsrs.2x	wl2, bmll0, s1, srssign1

	// sym_floor -- toward zero
	movxm	r4, #2
	mov	crrnd, r4
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vsrs.2x	wl3, bmll0, s1, srssign1

	// sym_ceil -- away from zero
	movxm	r4, #3
	mov	crrnd, r4
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vsrs.2x	wl4, bmll0, s1, srssign1

	// neg_inf -- tie -> -inf
	movxm	r4, #8
	mov	crrnd, r4
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vsrs.2x	wl5, bmll0, s1, srssign1

	// pos_inf -- tie -> +inf
	movxm	r4, #9
	mov	crrnd, r4
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vsrs.2x	wl6, bmll0, s1, srssign1

	// sym_zero -- tie -> zero
	movxm	r4, #10
	mov	crrnd, r4
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vsrs.2x	wl7, bmll0, s1, srssign1

	// sym_inf -- tie -> away
	movxm	r4, #11
	mov	crrnd, r4
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vsrs.2x	wl8, bmll0, s1, srssign1

	// conv_even -- tie -> even
	movxm	r4, #12
	mov	crrnd, r4
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vsrs.2x	wl9, bmll0, s1, srssign1

	// conv_odd -- tie -> odd
	movxm	r4, #13
	mov	crrnd, r4
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vsrs.2x	wl10, bmll0, s1, srssign1
	done

// floor      toward -inf
// CHECK-DAG: wl1 = 0x{{(FFFE000100010001){4}$}}
// ceil       toward +inf
// CHECK-DAG: wl2 = 0x{{(FFFF000200020002){4}$}}
// sym_floor  toward zero
// CHECK-DAG: wl3 = 0x{{(FFFF000100010001){4}$}}
// sym_ceil   away from zero
// CHECK-DAG: wl4 = 0x{{(FFFE000200020002){4}$}}
// neg_inf    tie -> -inf
// CHECK-DAG: wl5 = 0x{{(FFFE000200010001){4}$}}
// pos_inf    tie -> +inf
// CHECK-DAG: wl6 = 0x{{(FFFF000200020001){4}$}}
// sym_zero   tie -> zero
// CHECK-DAG: wl7 = 0x{{(FFFF000200010001){4}$}}
// sym_inf    tie -> away
// CHECK-DAG: wl8 = 0x{{(FFFE000200020001){4}$}}
// conv_even  tie -> even
// CHECK-DAG: wl9 = 0x{{(FFFE000200020001){4}$}}
// conv_odd   tie -> odd
// CHECK-DAG: wl10 = 0x{{(FFFF000200010001){4}$}}
