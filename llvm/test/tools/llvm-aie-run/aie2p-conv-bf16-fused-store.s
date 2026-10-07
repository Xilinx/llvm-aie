//===- aie2p-conv-bf16-fused-store.s ------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vst.conv.bf16.fp32 -- narrow f32 lanes to bf16 and store, in one
// instruction. The narrowing is deferred into MemWrite::Narrow the way
// vst.srs is, because a fused store's source may be written by an instruction
// issued after it.
//
// Every lane is 0x3FC18000, which is a TIE: the discarded half-word is exactly
// 0x8000 and the surviving bf16 0x3FC1 is ODD. crRnd is 12 (conv_even), so the
// answer is 0x3FC2 and NOT the truncation. A deferred narrowing that skipped
// the rounding, or read crRnd as the default, would store 0x3FC1 -- so this
// says the rounding really runs inside the callback rather than being dropped
// on the way through.
//
// The accumulators are LOADED rather than computed, which is what lets every
// lane carry a chosen bit pattern; vlda into a bm register is covered by
// aie2p-accumulator-memory.s. Checked against a reference, as
// aie2p-srs-fused.s does: vconv.bf16.fp32 into a register, then an ordinary
// vst.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:2048 | FileCheck %s

// A lane the narrowing DECLINES must fault rather than store. This is the
// case the deferral makes awkward -- the value is not known until SampleAt,
// so the callback has to be able to say no, and before it could the only
// options were to store an infinity or to check the wrong value at issue.
// RUN: llvm-mc -triple aie2p -filetype=obj --defsym NAN_LANE=1 %s -o %t.nan.o
// RUN: not llvm-aie-run %t.nan.o --scratch=0x10000:2048 2>&1 | \
// RUN:   FileCheck %s --check-prefix=NANFAULT

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10040
.ifdef NAN_LANE
	// Exponent all ones with the mantissa living only in the discarded low
	// half, so a truncation would quietly produce an infinity.
	movxm	r0, #0x7F800001
.else
	movxm	r0, #0x3FC18000
.endif
	vbcst.32	x0, r0
	movxm	r2, #0
	movxm	r6, #12
	mov	crrnd, r6
	mov	crsrsmode, r2
	mov	crsat, r2
	mov	dj0, r2
	movxm	r7, #256
	mov	m0, r7
	nop
	nop
	nop
	nop
	nop
	nop
	vst	x0, [p0, #0]
	vst	x0, [p1, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	// Both halves of cml0, so the 1024-bit source is wholly defined.
	vlda	bmll0, [p0, #0]
	vlda	bmlh0, [p1, #0]
	movxm	p2, #0x10100
	movxm	p3, #0x10200
	movxm	p4, #0x10300
	movxm	p5, #0x10400
	movxm	p6, #0x10500
	movxm	p7, #0x10600
	nop
	nop
	nop
	nop
	nop
	nop

.ifdef NAN_LANE
	// Only the fused store. The register form would fault first, at issue,
	// and that is a different path from the one under test here.
	vst.conv.bf16.fp32	bmll0, [p4, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	done
.else

	// The reference: narrow into a register, then an ordinary store. 512-bit
	// accumulator first, then the 1024-bit one.
	vconv.bf16.fp32	wl5, bmll0
	vconv.bf16.fp32	x1, cml0
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vst	wl5, [p2, #0]
	vst	x1, [p3, #0]

	// The fused forms, one address each, across the addressing modes.
	vst.conv.bf16.fp32	bmll0, [p4, #0]
	vst.conv.bf16.fp32	bmll0, [p5, dj0]
	vst.conv.bf16.fp32	bmll0, [p6], m0
	vst.conv.bf16.fp32	bmll0, [p7], #64
	// And the 512-bit access, from the 1024-bit accumulator.
	movxm	p0, #0x10700
	nop
	nop
	nop
	nop
	nop
	nop
	vst.conv.bf16.fp32	cml0, [p0, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// Read them all back.
	movxm	p1, #0x10300
	movxm	p2, #0x10400
	movxm	p3, #0x10500
	movxm	p4, #0x10600
	movxm	p5, #0x10700
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	wl0, [p1, #0]
	vlda	wl2, [p2, #0]
	vlda	wl3, [p3, #0]
	vlda	wl4, [p4, #0]
	vlda	x6, [p5, #0]
	done
.endif

// The reference. A tie under conv_even rounds to the EVEN neighbour, so every
// lane is 0x3FC2 -- truncation would leave the odd 0x3FC1.
// The 512-bit destinations print as their two 256-bit halves.
// CHECK-DAG: wl5 = 0x{{(3FC2){16}$}}
// CHECK-DAG: wl1 = 0x{{(3FC2){16}$}}
// CHECK-DAG: wh1 = 0x{{(3FC2){16}$}}

// Every fused form agrees with it.
// CHECK-DAG: wl0 = 0x{{(3FC2){16}$}}
// CHECK-DAG: wl2 = 0x{{(3FC2){16}$}}
// CHECK-DAG: wl3 = 0x{{(3FC2){16}$}}
// CHECK-DAG: wl4 = 0x{{(3FC2){16}$}}
// CHECK-DAG: wl6 = 0x{{(3FC2){16}$}}
// CHECK-DAG: wh6 = 0x{{(3FC2){16}$}}

// The pointers that were meant to advance did, and by the right amount.
// CHECK-DAG: p6 = 0x10600
// CHECK-DAG: p7 = 0x10640

// The declined lane names the store rather than storing an infinity.
// NANFAULT: fused store of bmll0 holds a value this model does not convert
