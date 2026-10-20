//===- aie2p-vector-conv-bf16.s -----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vconv.bf16.fp32: narrow f32 accumulator lanes back to bf16.
//
// The first instruction modelled here whose ROUNDING MODE is load-bearing --
// vmul.f needed none because a bf16 product is exact in f32, while this throws
// away 16 bits of every lane. It is not guessed: the instruction carries
// Uses = [crRnd], the same control register the integer SRS path reads.
//
// The accumulator is produced by vmul.f rather than loaded, which makes the
// inputs exactly representable and tests the pair the kernels actually use.
// 1.5 * (1 + 2^-7) = 1 + 2^-1 + 2^-7 + 2^-8, whose f32 is 0x3FC18000: the
// discarded half-word is exactly 0x8000, a TIE, and the surviving bf16 0x3FC1
// is ODD. 1.75 * (1 + 2^-7) gives 0x3FE1C000, above the halfway point rather
// than on it.
//
// Lane 1 is the same magnitude as lane 0 but NEGATIVE, and that is the point.
// A float is sign-magnitude, so truncation always moves toward zero while the
// integer SRS path's "up" means toward +inf. Under floor the negative lane
// must move AWAY from zero where the positive ones stay put, and under ceil
// the reverse -- so the two modes disagree on every lane here, in opposite
// directions per sign. Getting that backwards prints the other mode's answer.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:128 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10004
	movxm	p2, #0x10040
	movxm	p3, #0x10044
	// x0 lanes 0-2: 1.5, -1.5, 1.75
	movxm	r0, #0xBFC03FC0
	movxm	r1, #0x00003FE0
	// x1 lanes 0-2: all 1 + 2^-7
	movxm	r3, #0x3F813F81
	movxm	r4, #0x00003F81
	st	r0, [p0, #0]
	st	r1, [p1, #0]
	st	r3, [p2, #0]
	st	r4, [p3, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	x0, [p0, #0]
	vlda	x1, [p2, #0]
	movxm	r2, #60
	nop
	nop
	nop
	nop
	nop
	nop
	vmul.f	dm0, x0, x1, r2
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// floor -- toward -inf
	movxm	r5, #0
	mov	crrnd, r5
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vconv.bf16.fp32	wl1, bmll0
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// ceil -- toward +inf
	movxm	r5, #1
	mov	crrnd, r5
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vconv.bf16.fp32	wl2, bmll0
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	done

// floor: the positives truncate (0x3FC1, 0x3FE1) and the NEGATIVE grows in
// magnitude to 0xBFC2, because more negative is toward -inf.
// CHECK-DAG: wl1 = 0x3FE1BFC23FC1{{$}}

// ceil: exactly the opposite on every lane.
// CHECK-DAG: wl2 = 0x3FE2BFC13FC2{{$}}
