//===- aie2p-vector-negmul-msc-bf16.s ------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vnegmul.f and vmsc.f: the negated-product halves of the bf16 multiply grid.
// Same conf decode as vmul.f / vmac.f (AMODE_FP32, BMODE_16x16,
// CMODE_BF16xBF16_1_elem_1 = 60); the negation rides on the opcode, since
// aie2p_vmult.h builds a bit-identical conf for all four.
//
// dm1 is seeded by vmul.f: 2.0 * 3.0 = 6.0. Then vnegmul.f over the same
// sources must give -6.0, and vmsc.f must give dm1 - 1.0 * 1.0 = 5.0. Every
// value (6.0, 1.0, 5.0, -6.0) is exact in f32, so a correct read needs no
// rounding-mode assumption.
//
// The 5.0 is what separates the two orderings: negating the product before the
// accumulate gives 6.0 - 1.0 = 5.0, negating after would give -(6.0 + 1.0).
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:256 | FileCheck %s

// sub_mul (conf bit 11) would negate the product a second time; it is not
// modelled and must fault rather than cancel the opcode's own negation.
// RUN: llvm-mc -triple aie2p -filetype=obj --defsym BADSUB=1 %s -o %t.bad.o
// RUN: not llvm-aie-run %t.bad.o --scratch=0x10000:256 2>&1 | FileCheck %s --check-prefix=BADSUB

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10040
	movxm	p2, #0x10080
	movxm	p3, #0x100c0
	// Lane 0 only; every other lane is 0.0 x 0.0.
	movxm	r0, #0x00004000  // x0 lane0 = 2.0
	movxm	r1, #0x00004040  // x1 lane0 = 3.0
	movxm	r10, #0x00003F80 // x2 lane0 = 1.0
	movxm	r11, #0x00003F80 // x3 lane0 = 1.0
	st	r0, [p0, #0]
	st	r1, [p1, #0]
	st	r10, [p2, #0]
	st	r11, [p3, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	x0, [p0, #0]
	vlda	x1, [p1, #0]
	vlda	x2, [p2, #0]
	vlda	x3, [p3, #0]
.ifdef BADSUB
	movxm	r2, #2108        // 60 | sub_mul<<11
.else
	movxm	r2, #60
.endif
	nop
	nop
	nop
	nop
	nop
	nop
	vmul.f	dm1, x0, x1, r2
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vnegmul.f	dm2, x0, x1, r2
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vmsc.f	dm0, dm1, x2, x3, r2
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	done

// 2.0 * 3.0 - 1.0 * 1.0 = 5.0 -> f32 0x40A00000, in the lane-0 leaf. The idle
// lanes stay +0.0 here (0.0 + -0.0 is +0.0), so print elides them.
// CHECK-DAG: bmll0 = 0x40A00000{{$}}
// The vmul.f that seeds the accumulator, unnegated: 6.0 -> 0x40C00000. This is
// the control for the shared lambda still taking the un-negated path.
// CHECK-DAG: bmll1 = 0x40C00000{{$}}
// -(2.0 * 3.0) = -6.0 -> f32 0xC0C00000 in lane 0. Every OTHER lane is 0x80000000
// -- negative zero, because vnegmul.f negates the 0.0 x 0.0 products too, which
// is why this register prints in full where dm0 does not.
// CHECK-DAG: bmll2 = 0x80000000{{.*}}C0C00000{{$}}

// BADSUB: conf carries shift16/sub bits 800
