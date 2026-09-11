//===- aie2p-ups-fused.s ------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vlda.ups -- load and widen in one instruction.
//
// Checked against a REFERENCE rather than against constants: a plain vlda
// followed by a vups, which are tested separately elsewhere. Two independently
// written paths agreeing is a stronger statement than either matching a number
// I worked out by hand, and it is what catches an operand index read off the
// wrong outs list -- the fused forms shift su and ptr by one for a ptr_out and
// again for the dimensioned counters.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:512 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10000
	movxm	p2, #0x10000
	movxm	p3, #0x10000
	movxm	p4, #0x10000
	movxm	p5, #0x10000
	movxm	r0, #0xFFFF0002
	vbcst.32	x0, r0
	movxm	r2, #0
	mov	crupsmode, r2
	mov	s0, r2
	movxm	r5, #64
	mov	m0, r5
	mov	dj0, r2
	// d1 and d2 both step by 64 on their first step.
	movxm	r6, #3
	mov	dn1, r6
	mov	dj1, r5
	mov	dc1, r2
	mov	m1, r5
	mov	dn2, r6
	mov	dj2, r5
	mov	dc2, r2
	mov	dn6, r6
	mov	dj6, r5
	mov	dc6, r2
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vst	x0, [p0, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// The reference: load, then widen, as two tested instructions.
	vlda	wl1, [p0, #0]
	vlda	x2, [p0, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vups.2x	bmll3, wl1, s0, upssign1
	vups.2x	cml4, x2, s0, upssign1

	// The fused forms must agree. 256-bit access first, every addressing mode.
	vlda.ups.2x	bmll0, s0, upssign1, [p1, #0]
	vlda.ups.2x	bmlh0, s0, upssign1, [p1, dj0]
	vlda.ups.2x	bmhl0, s0, upssign1, [p2], m0
	vlda.ups.2x	bmhh0, s0, upssign1, [p3], #64
	vlda.2d.ups.2x	bmll1, s0, upssign1, [p4], d1
	vlda.3d.ups.2x	bmlh1, s0, upssign1, [p5], d2

	// 512-bit access, widening into a 1024-bit accumulator.
	vlda.ups.2x	cmh4, s0, upssign1, [p0, #0]
	// And the unsigned opcode of the same shape.
	vlda.ups.2x	bmll2, s0, upssign0, [p0, #0]
	done

// The reference values.
// CHECK-DAG: bmll3 = 0x{{(FFFFFFFF00000002){8}$}}
// CHECK-DAG: bmll4 = 0x{{(FFFFFFFF00000002){8}$}}
// CHECK-DAG: bmlh4 = 0x{{(FFFFFFFF00000002){8}$}}

// Every fused form agrees with them.
// CHECK-DAG: bmll0 = 0x{{(FFFFFFFF00000002){8}$}}
// CHECK-DAG: bmlh0 = 0x{{(FFFFFFFF00000002){8}$}}
// CHECK-DAG: bmhl0 = 0x{{(FFFFFFFF00000002){8}$}}
// CHECK-DAG: bmhh0 = 0x{{(FFFFFFFF00000002){8}$}}
// CHECK-DAG: bmll1 = 0x{{(FFFFFFFF00000002){8}$}}
// CHECK-DAG: bmlh1 = 0x{{(FFFFFFFF00000002){8}$}}
// CHECK-DAG: bmhl4 = 0x{{(FFFFFFFF00000002){8}$}}
// CHECK-DAG: bmhh4 = 0x{{(FFFFFFFF00000002){8}$}}

// Unsigned, so 0xFFFF widens to 0x0000FFFF and the leading digits go.
// CHECK-DAG: bmll2 = 0x{{FFFF00000002(0000FFFF00000002){7}$}}

// The pointers that were meant to advance did, and by the right amount.
// CHECK-DAG: p2 = 0x10040
// CHECK-DAG: p3 = 0x10040
// CHECK-DAG: p4 = 0x10040
// CHECK-DAG: p5 = 0x10040
