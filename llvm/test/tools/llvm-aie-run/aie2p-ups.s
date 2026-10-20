//===- aie2p-ups.s ------------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vups.Nx -- widen each vector lane into an accumulator lane, shifted left.
//
// The lane width is NOT in the opcode. vups.2x w2b is the selection target for
// both acc32_v16_I256_ups (16 lanes, i16 -> i32) and acc64_v8_I256_ups
// (8 lanes, i32 -> i64), which produce different bits from the same input.
// crUPSMode picks: 0 for the acc32 forms, 1 for the acc64 ones. The first two
// checks below are the same instruction on the same input under each mode, so
// they are what pins that.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs | FileCheck %s

	.text
	.globl _start
_start:
	movxm	r0, #0xFFFF0002
	vbcst.32	x0, r0
	movxm	r1, #0
	mov	s0, r1
	movxm	r2, #4
	mov	s1, r2
	mov	crupsmode, r1
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// acc32: 16-bit lanes widened to 32.
	vups.2x	bmll0, wl0, s0, upssign1
	// Unsigned, so the negative half zero-extends instead.
	vups.2x	bmlh0, wl0, s0, upssign0
	// Signed with a shift of 4.
	vups.2x	bmhl0, wl0, s1, upssign1

	// The other three conversions, all acc32 so the lane arithmetic is the
	// same and only the geometry differs.
	vups.2x	cml1, x0, s0, upssign1
	vups.4x	cmh1, wl0, s0, upssign1
	vups.4x	dm2, x0, s0, upssign1
	vups.4x	cml3, wl0, s0, upssign0
	vups.2x	cmh3, x0, s0, upssign0

	// Now the SAME 2x w2b instruction under acc64, which must widen 32-bit
	// lanes to 64 instead.
	movxm	r3, #1
	mov	crupsmode, r3
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vups.2x	bmhh0, wl0, s0, upssign1
	done

// 0x0002 and 0xFFFF as signed 16-bit lanes, widened to 32.
// CHECK-DAG: bmll0 = 0x{{(FFFFFFFF00000002){8}$}}

// Same lanes unsigned: 0xFFFF becomes 0x0000FFFF, so the top nibbles vanish.
// CHECK-DAG: bmlh0 = 0x{{FFFF00000002(0000FFFF00000002){7}$}}

// Signed, then shifted left by 4.
// CHECK-DAG: bmhl0 = 0x{{(FFFFFFF000000020){8}$}}

// 512 bits in, 1024 out, still 16-bit lanes -- twice as many of them.
// CHECK-DAG: bmll1 = 0x{{(FFFFFFFF00000002){8}$}}
// CHECK-DAG: bmlh1 = 0x{{(FFFFFFFF00000002){8}$}}

// 4x from 256 bits: 8-bit lanes now, so the source reads as 02 00 FF FF.
// CHECK-DAG: bmhl1 = 0x{{(FFFFFFFFFFFFFFFF0000000000000002){4}$}}
// CHECK-DAG: bmhh1 = 0x{{(FFFFFFFFFFFFFFFF0000000000000002){4}$}}

// 4x from 512 bits into the 2048-bit accumulator: same 8-bit lanes, 64 of them.
// CHECK-DAG: bmll2 = 0x{{(FFFFFFFFFFFFFFFF0000000000000002){4}$}}
// CHECK-DAG: bmhh2 = 0x{{(FFFFFFFFFFFFFFFF0000000000000002){4}$}}

// The unsigned 4x and 2x forms, for the sign0 opcodes of those conversions.
// Unsigned 8-bit lanes: 0xFF becomes 0x000000FF rather than 0xFFFFFFFF.
// CHECK-DAG: bmll3 = 0x{{FF000000FF0000000000000002(000000FF000000FF0000000000000002){3}$}}
// CHECK-DAG: bmhl3 = 0x{{FFFF00000002(0000FFFF00000002){7}$}}

// Same instruction as bmll0, acc64 mode: 32-bit lanes widened to 64.
// CHECK-DAG: bmhh0 = 0x{{(FFFFFFFFFFFF0002){8}$}}
