//===- aie2p-mac-register-pairings.s ------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// The three DENSE MAC register pairings beyond X_X: X_Y, Y_X and Y_Y. A y is
// 1024 bits, so a shape that wants 512 reads half of it, and this pins WHICH
// half by loading the two x registers a y is built from with different values.
//
// All three run 8x2_2x8 -- conf 792, amode 0, bmode 3, variant 0 plus 768 for
// two signed operands -- which wants 16 int16 of A and 16 of B, so 256 bits of
// each 512-bit half.
//
//   y0 = {x0, x1}   A: low 2, high 5
//   y1 = {x2, x3}   B: low 3, high 7
//
// Reading the low halves gives 2*3 + 2*3 = 12 in every lane. Any other pairing
// of halves gives 30, 28 or 70, so a wrong half cannot pass.
//
// The Q* pairings are absent on purpose: their $s2 is a sparse class, which is
// a different instruction rather than a wider one.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs | FileCheck %s

	.text
	.globl _start
_start:	movxm	r0, #0x00020002
	vbcst.32	x0, r0
	movxm	r1, #0x00050005
	vbcst.32	x1, r1
	movxm	r2, #0x00030003
	vbcst.32	x2, r2
	movxm	r3, #0x00070007
	vbcst.32	x3, r3
	movxm	r4, #792
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vmul	dm0, y0, x2, r4
	vmul	dm1, x0, y1, r4
	vmul	dm2, y0, y1, r4
	done

// Y_X
// CHECK-DAG: bmll0 = 0xC0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C
// CHECK-DAG: bmhh0 = 0xC0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C

// X_Y
// CHECK-DAG: bmll1 = 0xC0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C
// CHECK-DAG: bmhh1 = 0xC0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C

// Y_Y
// CHECK-DAG: bmll2 = 0xC0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C
// CHECK-DAG: bmhh2 = 0xC0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C0000000C
