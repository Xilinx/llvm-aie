//===- aie2p-mac-matrix.s -----------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vmul, the 8x8_8x8 matrix shape -- amode 0, bmode 1, variant 0, so a conf of
// (1<<3) = 8, plus (1<<9)|(1<<8) = 768 for two signed operands: 776.
//
// The shape name gives M, K and N directly, and aie_api confirms it:
// mmul_8_4<8, 8, 8, TypeA, TypeB, 32> instantiates this exact intrinsic over
// 64-element operands, and its template parameters are <M, K, N, ..., accum
// bits>. Row-major throughout -- A[i][k] at i*K+k, B[k][j] at k*N+j, C[i][j]
// at lane i*N+j.
//
// The operands are chosen to discriminate all three indices, which a broadcast
// alone cannot: a plain vbcst repeats every four bytes, so every ROW would be
// identical and a wrong i would still pass. Here two words are overwritten in
// memory before the load, giving rows 0 and 4 different contents, and B has
// non-zero entries in two different columns. The result has three distinct row
// patterns.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:128 | FileCheck %s

	.text
	.globl _start
_start:	movxm	p0, #0x10000
	movxm	p1, #0x10020
	movxm	p2, #0x10000
	movxm	r0, #0x04030201
	vbcst.32	x0, r0
	movxm	r1, #0x00030001
	vbcst.32	x1, r1
	movxm	r2, #0x08070605
	movxm	r3, #0x0C0B0A09
	movxm	r4, #776
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vst	x0, [p0, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	st	r2, [p0, #0]
	st	r3, [p1, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	x2, [p2, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vmul	dm0, x2, x1, r4
	done

// Computed independently from the matrix definition, not read back from the
// model: rows give 20/60, 36/108 and 52/156 in the two non-zero columns.
// CHECK-DAG: bmll0 = 0x3C0000000000000014000000000000003C0000000000000014000000000000006C0000000000000024000000000000006C0000000000000024
// CHECK-DAG: bmlh0 = 0x3C0000000000000014000000000000003C0000000000000014000000000000003C0000000000000014000000000000003C0000000000000014
