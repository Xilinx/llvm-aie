//===- aie2p-vector-addressing.s ----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// The addressing modes vlda/vst share with their scalar counterparts, past the
// plain [ptr, #imm] the load and store tests already cover: register offset,
// post-increment, and the sp-relative spill form. Spill offsets are negative
// (slots live below sp), same as the scalar spill test.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:192 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	sp, #0x10080
	movxm	p0, #0x10000
	movxm	p1, #0x10000
	movxm	r0, #0x11111111
	vbcst.32	x0, r0
	movxm	r1, #0x22222222
	vbcst.32	x1, r1
	movxm	r2, #64
	mov	dj0, r2
	nop
	nop
	nop
	nop
	nop
	nop

	// Post-increment store: writes at the pre-increment address, 0x10000.
	vst	x0, [p0], #64
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// Spill store: sp - 64 = 0x10040.
	vst	x1, [sp, #-64]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// Register offset: p1 + dj0 = 0x10040, so this must see x1's pattern.
	vlda	x2, [p1, dj0]
	// Spill load: sp - 128 = 0x10000, so this must see x0's.
	vlda	x3, [sp, #-128]
	// Post-increment load: reads 0x10000, then advances p1.
	vlda	x4, [p1], #64

	// The 256-bit forms take the same three paths. p2 walks the same two
	// patterns, so each lands on a different one.
	movxm	p2, #0x10000
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	wl6, [p2, dj0]
	vlda	wh6, [p2], #32
	// vlda's destination lands at cycle 7 while vst SAMPLES its source at
	// cycle 1 -- unlike the narrow scalar stores, which sample at 7. So a
	// store of a just-loaded vector needs seven bundles of separation, and
	// without them it commits the source's previous value rather than
	// faulting. Compiler output covers this; hand-written assembly does not,
	// and the first draft of this test did not either.
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vst	wl6, [sp, #-32]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	wh7, [sp, #-32]
	done

// CHECK-DAG: p0 = 0x10040
// CHECK-DAG: p1 = 0x10040

// The two stores landed 64 bytes apart, which is what the reads below prove:
// each picked up the other pattern than it would have on a wrong base.
// CHECK-DAG: wl2 = 0x{{(22222222){8}$}}
// CHECK-DAG: wh2 = 0x{{(22222222){8}$}}
// CHECK-DAG: wl3 = 0x{{(11111111){8}$}}
// CHECK-DAG: wh3 = 0x{{(11111111){8}$}}
// CHECK-DAG: wl4 = 0x{{(11111111){8}$}}
// CHECK-DAG: wh4 = 0x{{(11111111){8}$}}

// 256-bit: register offset reaches 0x10040, post-increment reads 0x10000 and
// leaves p2 past it, and the spill store/load round-trips through sp - 32.
// CHECK-DAG: p2 = 0x10020
// CHECK-DAG: wl6 = 0x{{(22222222){8}$}}
// CHECK-DAG: wh6 = 0x{{(11111111){8}$}}
// CHECK-DAG: wh7 = 0x{{(22222222){8}$}}
