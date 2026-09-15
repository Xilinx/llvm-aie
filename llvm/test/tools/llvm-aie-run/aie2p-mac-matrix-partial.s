//===- aie2p-mac-matrix-partial.s ---------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vmul, the 8x2_2x8 shape -- amode 0, bmode 3, variant 0, so a conf of
// (3<<3) = 24, plus 768 for two signed operands: 792.
//
// What this shape adds is that NEITHER operand fills its register: A is
// 8x2 and B is 2x8, so each wants 16 int16 -- 256 bits -- of a 512-bit x.
// The read window is the LOW half, and this test is built to fail if it were
// the high one. Both x registers are broadcast-filled first, so the upper 256
// bits hold the unpatched base pattern while the lower 256 carry the matrix;
// reading high would lose A's two distinct rows and B's patched second row.
//
// A is all ones except rows 0 and 4, so three distinct row patterns separate
// the i index. B alternates 0 and 1 down each row, which separates j, and its
// row 1 is patched in columns 0 and 1, which separates the k summation from a
// swapped B.
//
// Expected values computed from the matrix definition, not read back.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:128 | FileCheck %s

	.text
	.globl _start
_start:	movxm	p0, #0x10000
	movxm	p1, #0x10040
	movxm	p2, #0x10010
	movxm	p3, #0x10050
	movxm	r0, #0x00010001
	vbcst.32	x0, r0
	movxm	r1, #0x00010000
	vbcst.32	x1, r1
	movxm	r2, #0x00050004
	movxm	r3, #0x00070006
	movxm	r5, #0x00030002
	movxm	r4, #792
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vst	x0, [p0, #0]
	vst	x1, [p1, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	st	r2, [p0, #0]
	st	r3, [p2, #0]
	st	r5, [p3, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	x2, [p0, #0]
	vlda	x3, [p1, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vmul	dm0, x2, x3, r4
	done

// Rows 1-3 and 5-7 are [2, 4, 0, 2, 0, 2, 0, 2]; row 0 is
// [10, 19, 0, 9, 0, 9, 0, 9] and row 4 is [14, 27, 0, 13, 0, 13, 0, 13].
// CHECK-DAG: bmll0 = 0x200000000000000020000000000000002000000000000000400000002000000090000000000000009000000000000000900000000000000130000000A
// CHECK-DAG: bmlh0 = 0x2000000000000000200000000000000020000000000000004000000020000000200000000000000020000000000000002000000000000000400000002
// CHECK-DAG: bmhl0 = 0x2000000000000000200000000000000020000000000000004000000020000000D000000000000000D000000000000000D000000000000001B0000000E
// CHECK-DAG: bmhh0 = 0x2000000000000000200000000000000020000000000000004000000020000000200000000000000020000000000000002000000000000000400000002
