//===- aie2p-3d-addressing.s --------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// 3D post-increment -- the 2D walk with one more level around it:
//
//     if      (c1 != size1) { ptr += incr1; c1 += 1; }
//     else if (c2 != size2) { ptr += incr2; c1 = 0; c2 += 1; }
//     else                  { ptr += mod;   c1 = 0; c2 = 0; }
//
// aie_api gives the rule, and gives it next to the 2D one, which is what makes
// the nesting unambiguous:
//
//     add_2d_byte(p, inc.inc2, inc.num1, c1, inc.inc1)
//     add_3d_byte(p, inc.inc3, inc.num1, c1, inc.inc1, inc.num2, c2, inc.inc2)
//
// The descriptor is a PAIR of 80-bit dims -- d0 here is [d0, d4], so the outer
// level's fields are dn4/dj4/dc4 -- and createDSRegSequence fills seven of the
// eight slots: the outer dim's own mod is unused, the single sub_mod being the
// wrap for the whole walk.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:512 | FileCheck %s

	.text
	.globl _start
_start:
	// A 2x3 walk: step 4 twice, step 100 three times, then wrap. One full
	// period displaces 4+100+4+100+4 = 212, so the wrap is -212 and the
	// pointer lands back on the base exactly.
	movxm	p0, #0x10000
	movxm	r0, #-212
	mov	m0, r0
	movxm	r1, #1
	mov	dn0, r1
	movxm	r2, #4
	mov	dj0, r2
	movxm	r3, #0
	mov	dc0, r3
	movxm	r4, #2
	mov	dn4, r4
	movxm	r5, #100
	mov	dj4, r5
	mov	dc4, r3

	// A second descriptor for the loads and stores. Two steps is enough to
	// take one of each branch: the first advances the inner level, the
	// second exhausts it and advances the outer.
	movxm	p4, #0x10100
	movxm	p5, #0x10100
	movxm	r6, #64
	mov	dj1, r6
	mov	dn1, r1
	mov	dc1, r3
	movxm	r7, #32
	mov	dj5, r7
	mov	dn5, r1
	mov	dc5, r3
	movxm	r9, #-96
	mov	m1, r9
	movxm	p6, #0x10200
	movxm	r8, #0x6C6C6C6C
	vbcst.32	x0, r8
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// Six steps is one full period, and the seventh must repeat the first.
	padda.3d	[p0], d0
	padda.3d	[p0], d0
	padda.3d	[p0], d0
	padda.3d	[p0], d0
	padda.3d	[p0], d0
	padda.3d	[p0], d0
	padda.3d	[p0], d0

	// Store at p4's pre-increment address, then read the same address back
	// with a pointer that steps the OTHER branch.
	vst.3d	x0, [p4], d1
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vlda.3d	x1, [p5], d1
	vlda.3d	wl2, [p5], d1

	// By here both levels are exhausted, so the 256-bit store takes the
	// third branch and applies the wrap.
	vst.3d	wl0, [p6], d1
	done

// One period returns to base, so the seventh step is the second slot again.
// CHECK-DAG: p0 = 0x10004
// CHECK-DAG: dc0 = 0x1

// The store took the inner branch (+64); the first load found the inner level
// exhausted and took the outer one (+32), and the second stepped inner again.
// CHECK-DAG: p4 = 0x10140
// CHECK-DAG: p5 = 0x10160

// The wrap branch, reached once both counters are exhausted: 0x10200 - 96.
// CHECK-DAG: p6 = 0x101A0

// What the store wrote is what the load read.
// CHECK-DAG: wl1 = 0x{{(6C6C6C6C){8}$}}
// CHECK-DAG: wh1 = 0x{{(6C6C6C6C){8}$}}
