//===- aie2p-srs-fused.s ------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vst.srs -- narrow and store in one instruction. Unlike vlda.ups this needed
// a mechanism, not just a case: a store names its source and the executor
// reads it at the sample cycle, so the narrowing had to become something the
// MemWrite carries and the executor applies THERE. Doing it at issue would
// defeat the deferral, which exists because a store's source may be written by
// an instruction issued after it.
//
// crRnd is 12 (conv_even) throughout rather than the default, so the rounding
// really is running inside the deferred narrowing and not being skipped.
//
// Checked against a reference: vsrs into a register, then an ordinary vst.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:2048 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10100
	movxm	p2, #0x10200
	movxm	p3, #0x10300
	movxm	p4, #0x10400
	movxm	p5, #0x10500
	movxm	r0, #0x00060005
	movxm	r1, #0xFFFA0007
	vbcst.64	x0, r1:r0
	movxm	r2, #0
	mov	crupsmode, r2
	mov	crsrsmode, r2
	mov	crsat, r2
	movxm	r6, #12
	mov	crrnd, r6
	mov	s0, r2
	movxm	r3, #2
	mov	s1, r3
	movxm	r7, #256
	mov	m0, r7
	mov	dj0, r2
	movxm	r8, #3
	mov	dn1, r8
	mov	dj1, r7
	mov	dc1, r2
	mov	m1, r7
	mov	dn2, r8
	mov	dj2, r7
	mov	dc2, r2
	mov	dn6, r8
	mov	dj6, r7
	mov	dc6, r2
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	// Two accumulators: 512-bit and 1024-bit, so both narrowings are covered.
	vups.2x	bmll0, wl0, s0, upssign1
	vups.2x	cml1, x0, s0, upssign1
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// The reference: narrow into a register, then an ordinary store.
	vsrs.2x	wl4, bmll0, s1, srssign1
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vst	wl4, [p0, #0]

	// The fused forms, one per address, across the addressing modes.
	vst.srs.2x	bmll0, s1, srssign1, [p1, #0]
	vst.srs.2x	bmll0, s1, srssign1, [p2, dj0]
	vst.srs.2x	bmll0, s1, srssign1, [p3], m0
	vst.2d.srs.2x	bmll0, s1, srssign1, [p4], d1
	vst.3d.srs.2x	bmll0, s1, srssign1, [p5], d2
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// Read them all back.
	vlda	wl5, [p0, #0]
	vlda	wl6, [p1, #0]
	vlda	wl7, [p2, #0]
	movxm	p6, #0x10300
	movxm	p7, #0x10400
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	wl8, [p6, #0]
	vlda	wl9, [p7, #0]
	done

// The reference value, and every fused form agreeing with it. conv_even on
// lanes 5, 6, 7 and -6 at a shift of 2.
// CHECK-DAG: wl4 = 0x{{(FFFE000200020001){4}$}}
// CHECK-DAG: wl5 = 0x{{(FFFE000200020001){4}$}}
// CHECK-DAG: wl6 = 0x{{(FFFE000200020001){4}$}}
// CHECK-DAG: wl7 = 0x{{(FFFE000200020001){4}$}}
// CHECK-DAG: wl8 = 0x{{(FFFE000200020001){4}$}}
// CHECK-DAG: wl9 = 0x{{(FFFE000200020001){4}$}}

// The pointers that post-increment did.
// CHECK-DAG: p3 = 0x10400
// CHECK-DAG: p4 = 0x10500
// CHECK-DAG: p5 = 0x10600
