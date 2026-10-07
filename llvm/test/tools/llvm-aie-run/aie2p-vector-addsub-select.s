//===- aie2p-vector-addsub-select.s -------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vadd.N / vsub.N / vsel.N: the elementwise integer ALU, which unlike the
// min/max family it shares a datapath with carries no sign or saturation
// register at all.
//
// Every reading the encoding leaves open is pinned by an input that fails
// loudly under the alternative:
//
//  - WRAPPING, not saturating. Lane 0 is 0xFFFFFFFF and lane 1 is 0x7FFFFFFF,
//    so vadd.32 of 7 overflows lane 0 unsigned and lane 1 signed. Either
//    saturating reading holds one of them at its bound instead of wrapping.
//
//  - SUBTRACT DIRECTION. Lane 2 is 5 against a broadcast 7, so s1 - s2 is
//    0xFFFFFFFE where s2 - s1 would be 2.
//
//  - LANE WIDTH. vadd.16 of a broadcast 1 clears lane 0's 0xFFFF to zero and
//    carries nothing into lane 1; a .32 reading leaves 0x00010000 in that word
//    and a .8 reading writes 0x7F00FF00 into the next one.
//
//  - SELECT POLARITY. Set bit takes s2, clear takes s1, which is what
//    AIE2InstrPatterns.td's (select cond, $rs2, $rs3) -> VSEL_32($rs2, $rs3,
//    cond - 1) requires. Checked twice: against a hand-set mask, and by
//    feeding vmax_lt.32's own compare output back in, which must reproduce
//    the max it came from.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:64 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	r0, #0xFFFFFFFF
	st	r0, [p0, #0]
	movxm	r0, #0x7FFFFFFF
	st	r0, [p0, #4]
	movxm	r0, #0x00000005
	st	r0, [p0, #8]
	nop
	nop
	nop
	nop
	nop
	nop
	// x0 = [0xFFFFFFFF, 0x7FFFFFFF, 5, 0 ...] against the zeroed scratch.
	vlda	x0, [p0, #0]
	movxm	r1, #0x00000007
	vbcst.32	x1, r1
	movxm	r2, #0x00000001
	vbcst.16	x5, r2
	nop
	nop
	nop
	nop
	nop
	nop
	vadd.32	x2, x0, x1
	vsub.32	x3, x0, x1
	vadd.16	x4, x0, x5
	nop
	nop
	nop
	nop
	nop
	nop
	vmax_lt.32	x6, r16, x0, x1, vaddsign1
	nop
	nop
	nop
	nop
	nop
	nop
	// The mask vmax_lt just wrote, fed straight back as vsel's selector.
	vsel.32	x7, x0, x1, r16
	mov	r3, r16
	// Lanes 0 and 15, with every bit above the 16 lanes also set: the surplus
	// bits have to change nothing, and lane 15 is the top of the range.
	movxm	r20, #0xFFFF8001
	nop
	nop
	nop
	nop
	nop
	nop
	vsel.32	x8, x0, x1, r20
	// Byte lanes 0, 1 and 60 from s2. The 64 lanes need the whole pair, so
	// lane 60 only lands if the high half of it is read at all.
	movxm	r18, #0x00000003
	movxm	r19, #0x10000000
	nop
	nop
	nop
	nop
	nop
	nop
	vsel.8	x9, x0, x1, r19:r18
	nop
	nop
	nop
	nop
	nop
	nop
	done

// vadd.32. Lane 0 wraps to 6 rather than sticking at 0xFFFFFFFF, and lane 1
// wraps past the signed bound to 0x80000006 rather than sticking at
// 0x7FFFFFFF. Lane 2 is 5 + 7 and the rest are 0 + 7.
// CHECK-DAG: wl2 = 0x7{{(00000007){4}0000000C8000000600000006$}}
// CHECK-DAG: wh2 = 0x7{{(00000007){7}$}}

// vsub.32, same operands in the same order. Lane 2's 0xFFFFFFFE is 5 - 7; the
// reversed reading would print 2 there and 0x80000008 in lane 1.
// CHECK-DAG: wl3 = 0x{{(FFFFFFF9){5}FFFFFFFE7FFFFFF8FFFFFFF8$}}
// CHECK-DAG: wh3 = 0x{{(FFFFFFF9){8}$}}

// vadd.16 of a broadcast 1. Word 0 is two 0xFFFF lanes that each wrap to zero
// with no carry crossing between them, so the whole word is zero -- a .32 add
// would leave 0x00010000. Word 1's 0x7FFF lane becomes 0x8000, where a .8 add
// would have written 0x7F00FF00.
// CHECK-DAG: wl4 = 0x1{{0001(00010001){4}000100068000000000000000$}}
// CHECK-DAG: wh4 = 0x1{{0001(00010001){7}$}}

// SIGNED max: 0xFFFFFFFF is -1 so lane 0 takes the 7, and only lane 1's
// 0x7FFFFFFF beats it. The mask is that answer as one bit per lane.
// CHECK-DAG: wl6 = 0x7{{(00000007){5}7FFFFFFF00000007$}}
// CHECK-DAG: wh6 = 0x7{{(00000007){7}$}}
// CHECK-DAG: r3 = 0xFFFD{{$}}

// vsel.32 driven by that mask reproduces the max exactly -- the two
// instructions agree on both the bit-per-lane layout and which source a set
// bit names. An inverted polarity would print the min here instead.
// CHECK-DAG: wl7 = 0x7{{(00000007){5}7FFFFFFF00000007$}}
// CHECK-DAG: wh7 = 0x7{{(00000007){7}$}}

// vsel.32 with 0xFFFF8001: lanes 0 and 15 from s2, every other lane from s1,
// which is x0 back with a 7 at each end. The 16 surplus bits changed nothing.
// CHECK-DAG: wl8 = 0x57FFFFFFF00000007{{$}}
// CHECK-DAG: wh8 = 0x7{{0{56}$}}

// vsel.8 with bits 0, 1 and 60. The low two BYTES come from s2, whose
// broadcast 7 puts 0x07 in the first and 0x00 in the second, leaving
// 0xFFFF0007 -- a mask read at 16-bit lanes would have replaced that whole
// word with 7. Byte lane 60 is the low byte of the last word, and it is bit 28
// of the pair's HIGH register, so it lands only if all 64 bits are read.
// CHECK-DAG: wl9 = 0x57FFFFFFFFFFF0007{{$}}
// CHECK-DAG: wh9 = 0x7{{0{56}$}}
