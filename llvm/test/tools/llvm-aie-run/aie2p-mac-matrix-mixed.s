//===- aie2p-mac-matrix-mixed.s -----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vmul, the 4x8_8x8 shape -- amode 1, bmode 2, variant 0, so a conf of
// (1<<1)|(2<<3) = 18, plus 768 for two signed operands: 786.
//
// Two things this covers that 8x8_8x8 does not. The operands have DIFFERENT
// element widths -- A is 16-bit and B is 8-bit -- and the accumulator lane is
// 64 bits rather than 32. Both come from aie_api, which instantiates this
// intrinsic as mmul_16_8<4, 8, 8, TypeA, TypeB, AccumBits> over
// vector<TypeA, 32> and vector<TypeB, 64> with C_block<..., 64, 32, 1>: 32
// A elements of 16 bits, 64 B elements of 8, and 32 accumulator lanes of 64.
//
// That instantiation is also why this shape is modelled while 8x4_4x8 and
// friends are not: here both operands exactly FILL their 512-bit registers,
// so there is no question of which half is read.
//
// A's first two elements and elements 8-9 are overwritten in memory before the
// load, so rows 0 and 1 differ and a wrong row index cannot pass.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:128 | FileCheck %s

	.text
	.globl _start
_start:	movxm	p0, #0x10000
	movxm	p1, #0x10010
	movxm	p2, #0x10000
	movxm	r0, #0x00020001
	vbcst.32	x0, r0
	movxm	r1, #0x01000100
	vbcst.32	x1, r1
	movxm	r2, #0x00050004
	movxm	r3, #0x00070006
	movxm	r4, #786
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

// Computed from the matrix definition, not read back: rows give 18 and 22 in
// the odd columns and zero in the even ones.
// CHECK-DAG: bmll0 = 0x120000000000000000000000000000001200000000000000000000000000000012000000000000000000000000000000120000000000000000
// CHECK-DAG: bmlh0 = 0x160000000000000000000000000000001600000000000000000000000000000016000000000000000000000000000000160000000000000000
