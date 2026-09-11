//===- aie2p-accumulator-memory.s ---------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Loads and stores of the 512-bit accumulator registers. These are the only
// accumulators with plain memory forms -- the 1024- and 2048-bit ones are
// written by the MAC datapath, not loaded -- and they need no arithmetic and
// no rounding mode, which makes them the cheapest thing that exercises the
// accumulator register file end to end.
//
// Every source address below holds the same pattern, so a load that used the
// wrong base reads zeroed scratch and shows up.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:1024 \
// RUN:   --dump-mem=0x10200:8 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	sp, #0x10100
	movxm	p0, #0x10000
	movxm	p1, #0x10040
	movxm	p2, #0x10080
	movxm	p3, #0x100c0
	movxm	r0, #0x9A9A9A9A
	vbcst.32	x0, r0
	movxm	r1, #0
	mov	dj0, r1
	movxm	r2, #64
	mov	m0, r2
	// d0 (2D) and d1 (3D) both step by 64 with room to spare, so their
	// first step behaves like a flat post-increment.
	movxm	r3, #3
	mov	dn0, r3
	mov	dj0, r1
	mov	dc0, r1
	mov	m1, r2
	mov	dn1, r3
	mov	dj1, r2
	mov	dc1, r1
	mov	dn5, r3
	mov	dj5, r2
	mov	dc5, r1
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// Fill four 64-byte slots with the pattern.
	vst	x0, [p0, #0]
	vst	x0, [p1, #0]
	vst	x0, [p2, #0]
	vst	x0, [p3, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// Every flat addressing mode, into a different accumulator.
	vlda	bmll0, [p0, #0]
	vlda	bmlh0, [p0, dj0]
	vlda	bmhl0, [p1], #64
	vlda	bmhh0, [p2], m0
	vlda	bmll1, [sp, #-64]
	// And the dimensioned ones.
	vlda.3d	bmlh1, [p3], d1
	movxm	p4, #0x10000
	nop
	nop
	nop
	nop
	nop
	nop
	vlda.2d	bmhl1, [p4], d0

	// Store back through each mode too. p5..p7 walk the second half.
	movxm	p5, #0x10200
	movxm	p6, #0x10240
	movxm	p7, #0x10280
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vst	bmll0, [p5, #0]
	vst	bmll0, [p6, dj0]
	vst	bmll0, [p7], #64
	vst	bmll0, [p7], m0
	vst	bmll0, [sp, #-64]
	vst.3d	bmll0, [p5], d1
	vst.2d	bmll0, [p6], d0
	done

// Seven loads through seven addressing modes, all of the same pattern.
// CHECK-DAG: bmll0 = 0x{{(9A9A9A9A){16}$}}
// CHECK-DAG: bmlh0 = 0x{{(9A9A9A9A){16}$}}
// CHECK-DAG: bmhl0 = 0x{{(9A9A9A9A){16}$}}
// CHECK-DAG: bmhh0 = 0x{{(9A9A9A9A){16}$}}
// CHECK-DAG: bmll1 = 0x{{(9A9A9A9A){16}$}}
// CHECK-DAG: bmlh1 = 0x{{(9A9A9A9A){16}$}}
// CHECK-DAG: bmhl1 = 0x{{(9A9A9A9A){16}$}}

// And a store landed where it was aimed.
// CHECK: mem[0x10200] = 9a 9a 9a 9a 9a 9a 9a 9a
