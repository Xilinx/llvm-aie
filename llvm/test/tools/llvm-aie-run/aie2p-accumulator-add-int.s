//===- aie2p-accumulator-add-int.s --------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vadd and vsub over a whole 2048-bit accumulator. The conf's amode picks the
// lane width, so the same operands are added twice, at I32 and at I64, and the
// two results differ. These defs carry no Uses -- no crSat, no crVaddSign -- so
// the operands are chosen to make that visible rather than to exercise
// arithmetic:
//
//  - IT WRAPS, UNSIGNED. 0xffffffff + 0x80000000 is 0x7fffffff, where a
//    saturating add returns 0xffffffff.
//
//  - IT WRAPS, SIGNED. 0x7fffffff + 1 is 0x80000000, where a signed-saturating
//    add returns 0x7fffffff.
//
//  - THE LANES ARE 32 BITS. 0x80000000 + 0xffffffff carries out of bit 31. At
//    32-bit lanes that carry is discarded and every lane reads 0x7fffffff; at
//    64-bit lanes it propagates and each odd lane reads 0x80000000 instead.
//
//  - THE OPERAND ORDER. 0xf0f0f0f0 - 0x0f0f0f0f is 0xe1e1e1e1, and the other
//    way round is 0x1e1e1e1f.
//
// Every expected lane is non-zero and has bit 31 set, which is a constraint the
// register dump imposes rather than a property of the instruction: it skips a
// register that is entirely zero, and prints the rest through APInt::toString,
// which drops leading zeros.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --coverage --scratch=0x10000:512 \
// RUN:   | FileCheck %s

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
	movxm	r0, #0xFFFFFFFF
	movxm	r1, #0x7FFFFFFF
	movxm	r2, #0x80000000
	movxm	r3, #0xF0F0F0F0
	movxm	r4, #0x80000000
	movxm	r5, #0x00000001
	movxm	r8, #0xFFFFFFFF
	movxm	r9, #0x0F0F0F0F
	// amode I32, and no bit that rewrites an operand.
	movxm	r7, #0x0
	// amode I64: the same bits, added in 32 lanes instead of 64.
	movxm	r10, #0x2
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
	vadd	dm2, dm0, dm1, r7
	vsub	dm3, dm0, dm1, r7
	vadd	dm4, dm0, dm1, r10
	nop
	nop
	nop
	nop
	nop
	nop
	done

// CHECK-DAG: modelled   VADD_vmac_cm2_add_reg
// CHECK-DAG: modelled   VSUB_vmac_cm2_add_reg

// CHECK-DAG: bmll2 = 0x{{(7FFFFFFF){16}$}}
// CHECK-DAG: bmlh2 = 0x{{(80000000){16}$}}
// CHECK-DAG: bmhl2 = 0x{{(7FFFFFFF){16}$}}
// CHECK-DAG: bmhh2 = 0x{{(FFFFFFFF){16}$}}

// CHECK-DAG: bmll3 = 0x{{(7FFFFFFF){16}$}}
// CHECK-DAG: bmlh3 = 0x{{(7FFFFFFE){16}$}}
// CHECK-DAG: bmhl3 = 0x{{(80000001){16}$}}
// CHECK-DAG: bmhh3 = 0x{{(E1E1E1E1){16}$}}

// amode I64 over the same inputs. The low and third quarters are where the two
// widths part: the carry out of bit 31 that 32-bit lanes discard lands in the
// odd lane here, so 0x7fffffff repeated becomes 0x800000007fffffff repeated.
// CHECK-DAG: bmll4 = 0x{{(800000007FFFFFFF){8}$}}
// CHECK-DAG: bmlh4 = 0x{{(8000000080000000){8}$}}
// CHECK-DAG: bmhl4 = 0x{{(800000007FFFFFFF){8}$}}
// CHECK-DAG: bmhh4 = 0x{{(FFFFFFFFFFFFFFFF){8}$}}
