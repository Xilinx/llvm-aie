//===- aie2p-2d-addressing.s --------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// 2D post-increment. The d register is [mod, size, incr, count] and one step is
//
//     if (count == size) { ptr += mod;  count = 0; }
//     else               { ptr += incr; count += 1; }
//
// Both shapes below are taken from aie_api, which drives this from C and so
// fixes what the fields mean.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:384 | FileCheck %s

	.text
	.globl _start
_start:
	// Shape 1 -- circular_iterator::operator++, which calls
	//   add_2d_ptr(ptr, -(elems - stride), elems/stride - 1, index, stride).
	// With four 64-byte slots: step 64 three times, then jump -192, which
	// lands exactly back on the base.
	movxm	p0, #0x10000
	movxm	r0, #-192
	mov	m0, r0
	movxm	r1, #3
	mov	dn0, r1
	movxm	r2, #64
	mov	dj0, r2
	movxm	r3, #0
	mov	dc0, r3

	// Shape 2 -- fft_dit_radix3, add_2d_ptr(p, 1, r/8-1, cnt, 0). An
	// increment of ZERO with a non-zero modifier: hold the pointer for
	// size+1 iterations, then step it. This is the case that discriminates
	// the rule -- under "always add incr, wrap by mod" it could never move.
	movxm	p1, #0x10080
	movxm	r4, #4
	mov	m1, r4
	mov	dn1, r1
	movxm	r5, #0
	mov	dj1, r5
	mov	dc1, r3
	nop
	nop
	nop
	nop
	nop
	nop

	// One full period of each, plus one more step so the counter is seen to
	// restart rather than merely reach zero.
	padda.2d	[p0], d0
	padda.2d	[p0], d0
	padda.2d	[p0], d0
	padda.2d	[p0], d0
	padda.2d	[p0], d0

	padda.2d	[p1], d1
	padda.2d	[p1], d1
	padda.2d	[p1], d1
	padda.2d	[p1], d1
	padda.2d	[p1], d1

	// The loads and stores take the same step. d2 ping-pongs between two
	// 64-byte slots: step 64, then jump -64 back.
	movxm	p2, #0x10100
	movxm	p3, #0x10100
	movxm	r6, #-64
	mov	m2, r6
	movxm	r7, #1
	mov	dn2, r7
	mov	dj2, r2
	mov	dc2, r3
	movxm	r8, #0x7E7E7E7E
	vbcst.32	x2, r8
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// Two stores walk to the two slots, so both get written.
	vst.2d	x2, [p2], d2
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vst.2d	x2, [p2], d2
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// The loads walk the same pair back.
	vlda.2d	x3, [p3], d2
	vlda.2d	wl4, [p3], d2
	done

// Four steps return to the base, so the fifth is the second slot again.
// CHECK-DAG: p0 = 0x10040
// CHECK-DAG: dc0 = 0x1

// Held for four, stepped by 4 once, then held again.
// CHECK-DAG: p1 = 0x10084
// CHECK-DAG: dc1 = 0x1

// Two steps of a two-slot walk return both pointers to where they started,
// and the pattern round-tripped through both slots.
// CHECK-DAG: p2 = 0x10100
// CHECK-DAG: p3 = 0x10100
// CHECK-DAG: wl3 = 0x{{(7E7E7E7E){8}$}}
// CHECK-DAG: wh3 = 0x{{(7E7E7E7E){8}$}}
// CHECK-DAG: wl4 = 0x{{(7E7E7E7E){8}$}}
