//===- aie2p-conv-bfp16ebs8.s -------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vconv.bfp16ebs8.fp32: quantise 64 f32 accumulator lanes into block floating
// point -- eight blocks of eight, each a shared exponent and eight
// two's-complement mantissas.
//
// The accumulator is built by two exact bf16 widenings rather than a multiply,
// which keeps every input lane a bit pattern chosen here: vconv.fp32.bf16
// carries no Uses at all, so nothing between the constants below and the
// quantiser can round.
//
// WHAT THIS PINS THAT THE REGISTER MOVE COULD NOT. aie2p-bfp16-register-move.s
// says in as many words that a copy cannot discriminate a transposed el0/eh0,
// because read and write go through the same placement map. This instruction
// is the first that views the storage two ways -- it COMPUTES the mantissas
// and the exponents into named halves -- so the two exponent leaves are told
// apart here by giving block 0 and block 4 different shared exponents, 0x80
// against 0x7E. Swap the leaves and both checks fail.
//
// Block 0 (lanes 0-7) has max exponent 128, so every lane aligns down to it:
//
//   lane  value          exp  after >>17  align  mantissa
//   0     1.0            127  0x40        1      0x20
//   1     -1.0           127  0xC0        1      0xE0
//   2     2.0            128  0x40        0      0x40
//   3     0.5            126  0x40        2      0x10
//   4     -(1 + 2^-7)    127  0xBF        1      0xDF
//   5     +(1 + 2^-7)    127  0x40        1      0x20
//   6     2^-10          117  0x40        7      0x00
//   7     -2^-10         117  0xC0        7      0xFF
//
// Lane 4 is the one that earns its place: it is the FLOOR discriminator. Its
// magnitude needs 64.5 and then 32.5 mantissa steps, and floor takes a
// negative lane AWAY from zero both times -- -65 then -33, printing 0xDF.
// Truncation toward zero would print 0xE0. Lane 5 is the same magnitude
// positive and cannot tell the two apart, which is why both are here.
//
// Lanes 6 and 7 are the saturation the reference spells out as 0x00 / 0xff:
// their exponent is 11 below the block's, and an 8-bit arithmetic shift stops
// at all-sign-bits, so the positive one vanishes and the negative one sticks
// at -1 rather than reaching zero.
//
// Block 4 (lanes 32-39) is a second, smaller block whose max exponent is 126,
// carrying 0.5 and 0.25 with both signs. Its lanes lose no bits, so it checks
// the alignment arithmetic where nothing is discarded.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --coverage --scratch=0x10000:128 \
// RUN:   | FileCheck %s

// The gate itself, which no value check can reach: away from floor the two
// sources conflict, so the instruction must refuse rather than pick one.
// RUN: llvm-mc -triple aie2p -filetype=obj --defsym RND_CEIL=1 %s -o %t.ceil.o
// RUN: not llvm-aie-run %t.ceil.o --scratch=0x10000:128 2>&1 \
// RUN:   | FileCheck %s --check-prefix=NOT-FLOOR

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10004
	movxm	p2, #0x10008
	movxm	p3, #0x1000C
	movxm	p4, #0x10040
	movxm	p5, #0x10044
	// Block 0, as bf16 pairs: 1.0 -1.0 | 2.0 0.5 | -(1+2^-7) +(1+2^-7) |
	// 2^-10 -2^-10.
	movxm	r0, #0xBF803F80
	movxm	r1, #0x3F004000
	movxm	r2, #0x3F81BF81
	movxm	r3, #0xBA803A80
	// Block 4: 0.5 -0.5 | 0.25 -0.25.
	movxm	r6, #0xBF003F00
	movxm	r7, #0xBE803E80
	st	r0, [p0, #0]
	st	r1, [p1, #0]
	st	r2, [p2, #0]
	st	r3, [p3, #0]
	st	r6, [p4, #0]
	st	r7, [p5, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	x2, [p0, #0]
	vlda	x3, [p4, #0]
	// crRnd 0 is floor, which is what the reference calls truncation: an
	// arithmetic shift of a two's-complement mantissa. Ceil is the arm the
	// negative control takes.
.ifdef RND_CEIL
	movxm	r5, #1
.else
	movxm	r5, #0
.endif
	mov	crrnd, r5
	nop
	nop
	nop
	nop
	nop
	nop
	// Widen to the two halves of dm0, filling all 64 f32 lanes.
	vconv.fp32.bf16	cml0, x2
	vconv.fp32.bf16	cmh0, x3
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vconv.bfp16ebs8.fp32	ex0, dm0
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	done

// CHECK-DAG: modelled   VCONV_bfp16ebs8_fp32

// Block 0's mantissas, lane 7 down to lane 0. Lanes 8-31 are zero and their
// leading zeros are not printed, so the anchor is what keeps this from
// matching a wider value that happens to end the same way.
// CHECK-DAG: wl0 = 0x{{FF0020DF1040E020$}}

// Block 4's mantissas sit at the bottom of the high half, blocks 5-7 zero.
// CHECK-DAG: wh0 = 0x{{E020C040$}}

// The shared exponents, one byte per block: 128 for block 0 in the low leaf,
// 126 for block 4 in the high one. Distinct on purpose -- this pair is what
// tells el0 and eh0 apart.
// CHECK-DAG: el0 = 0x{{80$}}
// CHECK-DAG: eh0 = 0x{{7E$}}

// NOT-FLOOR: VCONV_bfp16ebs8_fp32: crRnd holds 1
