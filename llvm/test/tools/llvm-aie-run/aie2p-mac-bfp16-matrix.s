//===- aie2p-mac-bfp16-matrix.s -----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vmac.f EX_EX, the bfp16 8x8 by 8x8-transposed matrix accumulate -- 512 MACs
// in one instruction, and the shape the shipped GEMM runs on.
//
// WHAT THIS PINS THAT A VALUE CHECK NORMALLY CANNOT: the transpose. The shape
// bits of this conf (bmode 1, variant 0) are the same ones the int8 8x8_8x8
// carries, and that form is indexed B[k][j]; this one is B[j][k]. So the two
// readings are chosen between here rather than assumed, by operands built so
// they disagree on the SAME lanes:
//
//   A[0][3] = 2      B[5][3] = 3   ->  lane 5  = 6   ([k][j] would give 14,
//                    B[3][5] = 7        from B[3][5], which this reading
//                                       never reaches)
//   A[1][0] = 1      B[2][0] = 4   ->  lane 10 = 4 + 2*(-0.5) = 3
//   A[1][1] = 2      B[2][1] = -0.5     ([k][j] reads rows 0 and 1 of B,
//                                        both zero, and gives nothing)
//   A[4][2] = 1      B[7][2] = 5   ->  lane 39 = 5  ([k][j] would put 4 and
//                                       -0.5 in lanes 32 and 33 instead)
//
// The wrong reading produces {5: 14, 32: 4, 33: -0.5} against this one's
// {5: 6, 10: 3, 39: 5} -- disjoint but for lane 5, where the value differs.
// Lane 39 also puts a result in the accumulator's high half, so a model that
// only filled the low one fails rather than passing on a subset.
//
// The operands are built leaf by leaf rather than through
// vconv.bfp16ebs8.fp32, so this tests the multiply against the format
// directly instead of against the quantiser's reading of it. The encoding is
// the one that instruction emits: mantissa m and block exponent e denote
// m * 2^(e - 127) / 64, so 2.0 in a block whose max exponent is 128 is 0x40,
// and -0.5 three exponents down from 129 is 0xF8.
//
// Only ONE rounding is reachable in this instruction and it is not exercised
// here. Every term of a lane shares one scale -- the exponent is per block,
// and block i of A and block j of B are fixed across k -- so the dot product
// is an integer of at most 2^17 times a power of two, exact whatever order
// the hardware sums in. The accumulator add is the single place a result can
// round, and zero_acc is set here, so these values are exact.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --coverage --scratch=0x10000:128 \
// RUN:   | FileCheck %s
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:128 \
// RUN:   | FileCheck %s --check-prefix=ABSENT

// The shape gate: the same operands under an amode this does not model must
// refuse rather than fall back on the int8 reading.
// RUN: llvm-mc -triple aie2p -filetype=obj --defsym BAD_SHAPE=1 %s -o %t.shape.o
// RUN: not llvm-aie-run %t.shape.o --scratch=0x10000:128 2>&1 \
// RUN:   | FileCheck %s --check-prefix=BAD-SHAPE

// The sign gate: a bfp16 mantissa is two's complement, and what an unsigned
// reading of one would mean is stated nowhere.
// RUN: llvm-mc -triple aie2p -filetype=obj --defsym UNSIGNED=1 %s -o %t.sgn.o
// RUN: not llvm-aie-run %t.sgn.o --scratch=0x10000:128 2>&1 \
// RUN:   | FileCheck %s --check-prefix=UNSIGNED

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10020
	movxm	p2, #0x10040
	movxm	p3, #0x10068
	// A's mantissas: lane 3 = 0x40, lanes 8 and 9 = 0x20 0x40, lane 34 =
	// 0x40. Everything else is the zeroed scratch.
	movxm	r0, #0x40000000
	movxm	r1, #0x00004020
	movxm	r2, #0x00400000
	// B's mantissas: lanes 16 and 17 = 0x40 0xF8, lane 29 = 0x70, lane 43 =
	// 0x60, lane 58 = 0x50.
	movxm	r3, #0x0000F840
	movxm	r4, #0x00007000
	movxm	r5, #0x60000000
	movxm	r7, #0x00500000
	st	r0, [p0, #0]
	st	r1, [p0, #8]
	st	r2, [p1, #0]
	st	r3, [p2, #16]
	st	r4, [p2, #28]
	st	r5, [p3, #0]
	st	r7, [p3, #16]
	nop
	nop
	nop
	nop
	nop
	nop
	// The mantissa leaves of ex0 and ex1.
	vlda	x0, [p0, #0]
	vlda	x1, [p2, #0]
	// The exponent leaves, one byte per block of eight lanes. A's blocks 0
	// and 1 are 128 and its block 4 is 127; B's blocks 2, 3 and 7 are 129
	// and its block 5 is 128. Blocks whose lanes are all zero keep exponent
	// zero, which a zero mantissa makes unobservable.
	movxm	r8, #0x00008080
	movxm	r9, #0x0000007F
	movxm	r10, #0x81810000
	movxm	r11, #0x81008000
	mov	el0, r8
	mov	eh0, r9
	mov	el1, r10
	mov	eh1, r11
	// zero_acc, amode 2, bmode 1, variant 0, both signs set -- the conf
	// every bfp wrapper in aie2p_vmult.h builds for this shape.
.ifdef BAD_SHAPE
	movxm	r6, #0x309
.else
.ifdef UNSIGNED
	movxm	r6, #0x10D
.else
	movxm	r6, #0x30D
.endif
.endif
	nop
	nop
	nop
	nop
	nop
	nop
	vmac.f	dm2, dm2, ex0, ex1, r6
	nop
	nop
	nop
	nop
	nop
	nop
	done

// CHECK-DAG: modelled   VMAC_f_vmac_bfp_vmul_bfp_core_EX_EX

// Lane 10 = 3.0 then lane 5 = 6.0, low half. Anchored because the lanes below
// 5 are zero and leading zeros are not printed.
// CHECK-DAG: bmll2 = 0x{{404000000000000000000000000000000000000040C000000000000000000000000000000000000000000000$}}

// Lane 39 = 5.0, high half.
// CHECK-DAG: bmhl2 = 0x{{40A0000000000000000000000000000000000000000000000000000000000000$}}

// Lanes 16-31 and 48-63 stay zero, and an all-zero accumulator leaf is not
// printed at all, so their absence is what checks that nothing outside the
// three computed lanes was written. It runs as its own pass because a
// CHECK-NOT placed among the CHECK-DAGs above would bound them to the region
// before it, and --print-regs emits these leaves before the coverage lines.
// ABSENT-NOT: bmlh2 =
// ABSENT-NOT: bmhh2 =

// BAD-SHAPE: conf amode=0 bmode=1 variant=0 is not the 8x8_8x8T bfp16 shape
// UNSIGNED: conf clears sgn_x/sgn_y
