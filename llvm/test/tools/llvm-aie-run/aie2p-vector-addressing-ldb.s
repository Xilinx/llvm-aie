//===- aie2p-vector-addressing-ldb.s ------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vldb across the four addressing modes, in both the 512-bit and the 256-bit
// destination form. vldb is vlda on the other load port, and the port is not
// architectural state -- mXa and mXb are declared over the same mXm, mWa and
// mWb over the same four w registers -- so what this pins is that the operand
// shapes really do line up, not a second load semantics.
//
// Three patterns 64 bytes apart, and every mode lands on a different one than
// its neighbours: reading the immediate as zero, or confusing an indexed form
// with a post-incrementing one, moves the result onto a pattern that belongs
// to another check. The two m-register forms decrement, which is the case
// worth pinning -- a negative modifier goes through the same 20-bit modular
// add that aie2p-modifier-postincrement.s pins for padda.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:192 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10040
	movxm	p2, #0x10080
	movxm	r0, #0x11111111
	vbcst.32	x0, r0
	movxm	r1, #0x22222222
	vbcst.32	x1, r1
	movxm	r2, #0x33333333
	vbcst.32	x2, r2
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// Lay the three patterns down, one 64-byte region each.
	vst	x0, [p0, #0]
	vst	x1, [p1, #0]
	vst	x2, [p2, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// One pointer per load, so a post-increment cannot disturb a later read.
	movxm	p0, #0x10000
	movxm	p1, #0x10000
	movxm	p2, #0x10000
	movxm	p3, #0x10080
	movxm	p4, #0x10000
	movxm	p5, #0x10000
	movxm	p6, #0x10000
	movxm	p7, #0x10080
	movxm	r3, #128
	mov	dj0, r3
	mov	dj1, r3
	movxm	r4, #-64
	mov	m0, r4
	movxm	r5, #-32
	mov	m1, r5
	nop
	nop
	nop
	nop
	nop
	nop

	// 512-bit: [ptr, #imm], [ptr, dj], [ptr], #imm, [ptr], m.
	vldb	x3, [p0, #64]
	vldb	x4, [p1, dj0]
	vldb	x5, [p2], #64
	vldb	x6, [p3], m0

	// 256-bit, the same four shapes.
	vldb	wl8, [p4, #64]
	vldb	wh8, [p5, dj1]
	vldb	wl9, [p6], #32
	vldb	wh9, [p7], m1
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	done

// 512-bit reads: base+64 is B, base+dj0 is C, the post-incrementing pair read
// their pre-increment address, so A and C respectively.
// CHECK-DAG: wl3 = 0x{{(22222222){8}$}}
// CHECK-DAG: wh3 = 0x{{(22222222){8}$}}
// CHECK-DAG: wl4 = 0x{{(33333333){8}$}}
// CHECK-DAG: wh4 = 0x{{(33333333){8}$}}
// CHECK-DAG: wl5 = 0x{{(11111111){8}$}}
// CHECK-DAG: wh5 = 0x{{(11111111){8}$}}
// CHECK-DAG: wl6 = 0x{{(33333333){8}$}}
// CHECK-DAG: wh6 = 0x{{(33333333){8}$}}

// 256-bit reads, same four addresses.
// CHECK-DAG: wl8 = 0x{{(22222222){8}$}}
// CHECK-DAG: wh8 = 0x{{(33333333){8}$}}
// CHECK-DAG: wl9 = 0x{{(11111111){8}$}}
// CHECK-DAG: wh9 = 0x{{(33333333){8}$}}

// Only the four post-incrementing forms moved a pointer, and the two m forms
// moved theirs backwards.
// CHECK-DAG: p0 = 0x10000
// CHECK-DAG: p1 = 0x10000
// CHECK-DAG: p2 = 0x10040
// CHECK-DAG: p3 = 0x10040
// CHECK-DAG: p4 = 0x10000
// CHECK-DAG: p5 = 0x10000
// CHECK-DAG: p6 = 0x10020
// CHECK-DAG: p7 = 0x10060
