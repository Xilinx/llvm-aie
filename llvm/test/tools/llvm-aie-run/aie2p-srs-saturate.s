//===- aie2p-srs-saturate.s ---------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// crSat, whose values are aie_api's saturation_mode: 0 none, 1 saturate,
// 3 symmetric. The accumulator lanes here are +/-65536, which does not fit a
// signed 16-bit result, so all three differ -- and symmetric is the one that
// needs a distinct case, since it clamps the low end to -32767 rather than
// -32768 so the two limits have equal magnitude.
//
// Also covers the remaining vsrs conversions, whose geometry differs but whose
// lane arithmetic is the same.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs | FileCheck %s

	.text
	.globl _start
_start:
	movxm	r0, #0xFFFF0001
	vbcst.32	x0, r0
	movxm	r2, #0
	mov	crupsmode, r2
	mov	crsrsmode, r2
	mov	crrnd, r2
	mov	s0, r2
	movxm	r3, #16
	mov	s1, r3
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	// Lanes of +1 and -1 shifted up by 16 give +/-65536.
	vups.2x	bmll0, wl0, s1, upssign1
	vups.2x	cml2, x0, s1, upssign1
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// none: the value simply wraps, and 65536 truncated to 16 bits is zero,
	// so the whole register reads back zero and --print-regs omits it.
	movxm	r4, #1
	mov	crsat, r2
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vsrs.2x	wl1, bmll0, s0, srssign1

	// saturate: the ordinary signed limits.
	mov	crsat, r4
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vsrs.2x	wl3, bmll0, s0, srssign1
	// Unsigned saturation clamps the negative lane to zero instead.
	vsrs.2x	wl4, bmll0, s0, srssign0
	// The 512-bit source conversion, same arithmetic.
	vsrs.2x	x5, cml2, s0, srssign1

	// symmetric: the low limit becomes -32767.
	movxm	r5, #3
	mov	crsat, r5
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vsrs.2x	wl7, bmll0, s0, srssign1
	done

// CHECK-NOT: wl1 =

// 65536 and -65536 clamped to the signed 16-bit limits.
// CHECK-DAG: wl3 = 0x{{(80007FFF){8}$}}

// Unsigned: the negative lane clamps to ZERO, not to the maximum -- the
// accumulator is signed however the result is read. The top lane is one of
// the zeroed ones, so its leading digits are not printed.
// CHECK-DAG: wl4 = 0x{{FFFF(0000FFFF){7}$}}

// Same again through the 1024-bit accumulator into a 512-bit vector.
// CHECK-DAG: wl5 = 0x{{(80007FFF){8}$}}
// CHECK-DAG: wh5 = 0x{{(80007FFF){8}$}}

// Symmetric keeps the limits equal in magnitude, so the low one is -32767.
// CHECK-DAG: wl7 = 0x{{(80017FFF){8}$}}
