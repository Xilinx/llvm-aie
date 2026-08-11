//===- aie2p-accumulator-add-float.s ------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vadd.f and vsub.f over a whole 2048-bit accumulator: 64 f32 lanes, no
// multiply. The conf is accfp32_vecconf's 0x3c, the word the fadd/fsub
// patterns emit.
//
// Each 512-bit quarter carries one broadcast value, so a quarter's expected
// result is one repeated word and the four quarters differ from each other:
//
//  - THE OPERATION. The addends are a power of two and a smaller power of two,
//    so every sum and difference is exact and distinct. Subtracting the other
//    way round turns all four vsub quarters negative.
//
//  - THE LANE TYPE. An integer add of the same bits gives 0x7e800000 in the
//    first quarter rather than 1.5's 0x3fc00000, so a float op that fell
//    through to the integer body cannot pass.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --coverage --scratch=0x10000:512 \
// RUN:   | FileCheck %s

// A conf bit that rewrites an operand says so and stops, rather than returning
// a sum that ignored it. 0x3d is accfp32_vecconf with dynZeroAccum set.
// RUN: llvm-mc -triple aie2p -filetype=obj --defsym ZEROACC=1 %s -o %t.bad.o
// RUN: not llvm-aie-run %t.bad.o --scratch=0x10000:512 2>&1 \
// RUN:   | FileCheck %s --check-prefix=ZEROACC

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10040
	movxm	p2, #0x10080
	movxm	p3, #0x100C0
	movxm	p4, #0x10100
	movxm	p5, #0x10140
	movxm	p6, #0x10180
	movxm	p7, #0x101C0
	// dm0 = 1.0, 2.0, 4.0, 8.0 over its four quarters, low to high.
	movxm	r0, #0x3F800000
	movxm	r1, #0x40000000
	movxm	r2, #0x40800000
	movxm	r3, #0x41000000
	// dm1 = 0.5, 0.25, 0.125, 0.0625.
	movxm	r4, #0x3F000000
	movxm	r5, #0x3E800000
	movxm	r8, #0x3E000000
	movxm	r9, #0x3D800000
.ifdef ZEROACC
	movxm	r6, #0x3D
.else
	// accfp32_vecconf: amode FP32, and nothing that rewrites an operand.
	movxm	r6, #0x3C
.endif
	nop
	nop
	nop
	nop
	nop
	nop
	vbcst.32	x0, r0
	vbcst.32	x1, r1
	vbcst.32	x2, r2
	vbcst.32	x3, r3
	vbcst.32	x4, r4
	vbcst.32	x5, r5
	vbcst.32	x6, r8
	vbcst.32	x7, r9
	nop
	nop
	nop
	nop
	nop
	nop
	vst	x0, [p0, #0]
	vst	x1, [p1, #0]
	vst	x2, [p2, #0]
	vst	x3, [p3, #0]
	vst	x4, [p4, #0]
	vst	x5, [p5, #0]
	vst	x6, [p6, #0]
	vst	x7, [p7, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	bmll0, [p0, #0]
	vlda	bmlh0, [p1, #0]
	vlda	bmhl0, [p2, #0]
	vlda	bmhh0, [p3, #0]
	vlda	bmll1, [p4, #0]
	vlda	bmlh1, [p5, #0]
	vlda	bmhl1, [p6, #0]
	vlda	bmhh1, [p7, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	vadd.f	dm2, dm0, dm1, r6
	vsub.f	dm3, dm0, dm1, r6
	nop
	nop
	nop
	nop
	nop
	nop
	done

// CHECK-DAG: modelled   VADD_f_vmac_cm2_add_reg
// CHECK-DAG: modelled   VSUB_f_vmac_cm2_add_reg

// 1.5, 2.25, 4.125, 8.0625.
// CHECK-DAG: bmll2 = 0x{{(3FC00000){16}$}}
// CHECK-DAG: bmlh2 = 0x{{(40100000){16}$}}
// CHECK-DAG: bmhl2 = 0x{{(40840000){16}$}}
// CHECK-DAG: bmhh2 = 0x{{(41010000){16}$}}

// 0.5, 1.75, 3.875, 7.9375. Reversing the operands makes every one of these
// negative, which is the sign bit of the first hex digit.
// CHECK-DAG: bmll3 = 0x{{(3F000000){16}$}}
// CHECK-DAG: bmlh3 = 0x{{(3FE00000){16}$}}
// CHECK-DAG: bmhl3 = 0x{{(40780000){16}$}}
// CHECK-DAG: bmhh3 = 0x{{(40FE0000){16}$}}

// ZEROACC: conf 3d asks for a zero-accumulate, an accumulator shift or a negation
