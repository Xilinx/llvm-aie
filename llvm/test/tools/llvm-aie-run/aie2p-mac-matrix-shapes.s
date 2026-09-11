//===- aie2p-mac-matrix-shapes.s ----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// The three remaining integer matrix shapes, all from ONE memory image read at
// three different element widths -- so a shape that had M and N transposed, or
// the wrong operand width, cannot agree with the others by accident.
//
//   8x4_4x8  conf 784  A 16-bit fills x, B 8-bit reads the low half,  acc32
//   4x4_4x8  conf 794  A 16-bit reads the low half, B 16-bit fills x, acc64
//   4x2_2x8  conf 770  A 32-bit and B 16-bit both read the low half,  acc64
//
// conf is (amode<<1)|(bmode<<3)|(variant<<5) plus 768 for two signed operands.
//
// Both vectors are broadcast-filled and then patched in two words, so each has
// distinct rows near the top and an unpatched upper half. Expected values are
// computed from the matrix definition, not read back.
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
	movxm	r4, #784
	movxm	r6, #794
	movxm	r7, #770
	nop
	nop
	nop
	nop
	vmul	dm0, x2, x3, r4
	vmul	dm1, x2, x3, r6
	vmul	dm2, x2, x3, r7
	done

// 8x4_4x8: rows 0 and 2 are [2,0,13,0,0,0,11,0] and [2,0,17,0,0,0,15,0];
// the rest are [2,0,6,0,0,0,4,0].
// CHECK-DAG: bmll0 = 0x4000000000000000000000000000000060000000000000002000000000000000B0000000000000000000000000000000D0000000000000002
// CHECK-DAG: bmlh0 = 0x4000000000000000000000000000000060000000000000002000000000000000F000000000000000000000000000000110000000000000002
// CHECK-DAG: bmhl0 = 0x40000000000000000000000000000000600000000000000020000000000000004000000000000000000000000000000060000000000000002
// CHECK-DAG: bmhh0 = 0x40000000000000000000000000000000600000000000000020000000000000004000000000000000000000000000000060000000000000002

// 4x4_4x8: rows 0 and 2 are [10,21,0,11,0,11,0,11] and [14,29,0,15,0,15,0,15];
// rows 1 and 3 are [2,6,0,4,0,4,0,4].
// CHECK-DAG: bmll1 = 0xB0000000000000000000000000000000B0000000000000000000000000000000B00000000000000000000000000000015000000000000000A
// CHECK-DAG: bmlh1 = 0x40000000000000000000000000000000400000000000000000000000000000004000000000000000000000000000000060000000000000002
// CHECK-DAG: bmhl1 = 0xF0000000000000000000000000000000F0000000000000000000000000000000F0000000000000000000000000000001D000000000000000E
// CHECK-DAG: bmhh1 = 0x40000000000000000000000000000000400000000000000000000000000000004000000000000000000000000000000060000000000000002

// 4x2_2x8: A is read as 32-bit here, so the same bytes give much larger
// products -- row 0 is [131074,524295,0,393221,0,393221,0,393221].
// CHECK-DAG: bmll2 = 0x600050000000000000000000000000006000500000000000000000000000000060005000000000000000000000000000800070000000000020002
// CHECK-DAG: bmlh2 = 0x200020000000000000000000000000002000200000000000000000000000000020002000000000000000000000000000400040000000000020002
// CHECK-DAG: bmhl2 = 0x800070000000000000000000000000008000700000000000000000000000000080007000000000000000000000000000A00090000000000020002
// CHECK-DAG: bmhh2 = 0x200020000000000000000000000000002000200000000000000000000000000020002000000000000000000000000000400040000000000020002
