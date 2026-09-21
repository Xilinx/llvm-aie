//===- aie2p-conv-bf16-widen.s ------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vconv.fp32.bf16 and vlda.conv.fp32.bf16 -- widen bf16 lanes to f32.
//
// The source word is 0xFFFF0002, so the two bf16 lanes are 0x0002 and 0xFFFF
// and the f32 lanes must come out 0x00020000 and 0xFFFF0000. That pattern is
// what separates this from the integer widening next door: vups would produce
// 0x00000002 / 0x0000FFFF unsigned or 0xFFFFFFFF signed, so a body that
// sign-extends, zero-extends, or forgets the 16-bit shift lands on a value
// none of these expectations carry.
//
// The fused forms are checked against a REFERENCE -- a plain vlda followed by
// the register conversion -- as aie2p-ups-fused.s does, because two
// independently written paths agreeing catches an operand index read off the
// wrong outs list, which the ptr_out forms shift by one.
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
	movxm	r5, #64
	mov	m0, r5
	mov	dj0, r2
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

	// The reference: load, then widen, as two separately tested steps.
	vlda	wl1, [p0, #0]
	vlda	x2, [p0, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vconv.fp32.bf16	bmll3, wl1
	vconv.fp32.bf16	cml2, x2

	// The fused forms must agree. 256-bit access, every addressing mode.
	vlda.conv.fp32.bf16	bmll0, [p1, #0]
	vlda.conv.fp32.bf16	bmlh0, [p1, dj0]
	vlda.conv.fp32.bf16	bmhl0, [p2], m0
	vlda.conv.fp32.bf16	bmhh0, [p3], #64

	// 512-bit access, widening into a 1024-bit accumulator.
	vlda.conv.fp32.bf16	cml4, [p0, #0]
	vlda.conv.fp32.bf16	cmh4, [p4], #64
	done

// The reference. Lane 0 is 0x00020000 and lane 1 0xFFFF0000, printed high
// lane first, so one 64-bit unit is FFFF000000020000.
// CHECK-DAG: bmll3 = 0x{{(FFFF000000020000){8}$}}
// CHECK-DAG: bmll2 = 0x{{(FFFF000000020000){8}$}}
// CHECK-DAG: bmlh2 = 0x{{(FFFF000000020000){8}$}}

// Every fused form agrees with it.
// CHECK-DAG: bmll0 = 0x{{(FFFF000000020000){8}$}}
// CHECK-DAG: bmlh0 = 0x{{(FFFF000000020000){8}$}}
// CHECK-DAG: bmhl0 = 0x{{(FFFF000000020000){8}$}}
// CHECK-DAG: bmhh0 = 0x{{(FFFF000000020000){8}$}}
// CHECK-DAG: bmll4 = 0x{{(FFFF000000020000){8}$}}
// CHECK-DAG: bmlh4 = 0x{{(FFFF000000020000){8}$}}
// CHECK-DAG: bmhl4 = 0x{{(FFFF000000020000){8}$}}
// CHECK-DAG: bmhh4 = 0x{{(FFFF000000020000){8}$}}

// The pointers that were meant to advance did, and by the right amount.
// CHECK-DAG: p2 = 0x10040
// CHECK-DAG: p3 = 0x10040
// CHECK-DAG: p4 = 0x10040
