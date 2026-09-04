//===- aie2p-vector-mul-bf16.s ------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vmul.f: elementwise bf16 x bf16 widening into f32 accumulator lanes.
//
// The third operand is an eR but is NOT an accumulator -- the patterns pass
// mulbf16_vecconf.ConfBits, the same configuration word the integer MAC forms
// take. 60 is that word: amode 2 (FP32) at bits 1-2, bmode 3 (16x16) at 3-4,
// cmode 1 (BF16xBF16_1_elem_1) at 5-7.
//
// Lane 1 is the load-bearing one. 1.0078125 is 1 + 2^-7, the smallest step a
// bf16 mantissa can take, and its square is 1 + 2^-6 + 2^-14 -- which needs 15
// mantissa bits and so does NOT fit back in a bf16. Keeping it means the
// product stayed at f32 precision; narrowing to bf16 anywhere would drop the
// 2^-14 term and print 0x3F820000 instead of 0x3F820200.
//
// That is also why this instruction needs no rounding mode, which the model
// checks rather than assumes: a bf16 significand is 8 bits, so a product needs
// at most 16, and f32 holds 24. It is always exact, and the model faults if a
// multiply ever reports otherwise.
//
// Lane 0 carries the sign case, and every other lane is 0.0 x 0.0.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:128 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10040
	// x0: lane0 = -2.0 (0xC000), lane1 = 1.0078125 (0x3F81)
	movxm	r0, #0x3F81C000
	// x1: lane0 =  1.0 (0x3F80), lane1 = 1.0078125 (0x3F81)
	movxm	r1, #0x3F813F80
	st	r0, [p0, #0]
	st	r1, [p1, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	x0, [p0, #0]
	vlda	x1, [p1, #0]
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
	done

// The accumulator leaf holding lanes 0 and 1, low lane last.
//   lane 0: -2.0 * 1.0      = -2.0     -> 0xC0000000
//   lane 1:  1.0078125^2    = 1+2^-6+2^-14 -> 0x3F820200
// A model that narrowed the product back to bf16 would print 0x3F820000 here.
// CHECK-DAG: bmll0 = 0x3F820200C0000000{{$}}
