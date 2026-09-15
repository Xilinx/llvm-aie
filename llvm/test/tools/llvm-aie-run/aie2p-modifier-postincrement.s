//===- aie2p-modifier-postincrement.s -----------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// `[$ptr], $mod` -- post-increment by an m register. An m register in this
// mode holds a plain signed byte count, not a structured descriptor, which is
// what test/CodeGen/AIE/dyn-stackalloc.ll shows the compiler relying on: it
// lowers `alloca i32, %n` to `mov m0, r0` / `padda [p1], m0` where r0 is
// n*4 rounded up to 64. The name is shared with a FIELD of the 2D descriptor
// (sub_mod), which is why this is worth pinning rather than assuming.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:192 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10040
	movxm	p2, #0x10000
	movxm	p3, #0x10000
	movxm	r0, #64
	mov	m0, r0
	movxm	r1, #-64
	mov	m1, r1
	movxm	r2, #0x33333333
	vbcst.32	x0, r2
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// Pointer arithmetic both ways. The negative one is the case that pins
	// the arithmetic: -64 lives in a 20-bit register as 0xFFFC0, and the
	// address is masked back to 20 bits, so p - 64 comes out of a modular
	// add without anything being sign-extended.
	padda	[p0], m0
	padda	[p1], m1

	// Vector store, then reload from where the pointer ended up.
	vst	x0, [p2], m0
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	x1, [p3], m0

	// The scalar and 256-bit forms take the same operand shape.
	movxm	p4, #0x10080
	movxm	p5, #0x10080
	movxm	p6, #0x100a0
	movxm	p7, #0x100a0
	movxm	r4, #4
	mov	m2, r4
	movxm	r5, #32
	mov	m3, r5
	movxm	r6, #0x5A5A5A5A
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	st	r6, [p4], m2
	vst	wl0, [p6], m3
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	lda	r7, [p5], m2
	vlda	wh3, [p7], m3
	done

// CHECK-DAG: p0 = 0x10040
// CHECK-DAG: p1 = 0x10000

// The store wrote at p2's PRE-increment address and left p2 past it; the load
// then read that same address and advanced too.
// CHECK-DAG: p2 = 0x10040
// CHECK-DAG: p3 = 0x10040
// CHECK-DAG: wl1 = 0x{{(33333333){8}$}}
// CHECK-DAG: wh1 = 0x{{(33333333){8}$}}

// Scalar and 256-bit, same shape: each pointer advanced by its own m register
// and the value round-tripped through memory.
// CHECK-DAG: p4 = 0x10084
// CHECK-DAG: p5 = 0x10084
// CHECK-DAG: r7 = 0x5A5A5A5A
// CHECK-DAG: p6 = 0x100C0
// CHECK-DAG: p7 = 0x100C0
// CHECK-DAG: wh3 = 0x{{(33333333){8}$}}
