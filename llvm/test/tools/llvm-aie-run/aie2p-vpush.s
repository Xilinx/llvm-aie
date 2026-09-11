//===- aie2p-vpush.s ----------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vpush.hi.32: the vector shifted down one element, with the scalar arriving
// as the new TOP element.
//
// The mnemonic says "push" but not toward which end, and neither does the
// operand list. Two things in aie2p/AIE2PInstrPatterns.td settle it: these
// select from G_AIE_ADD_VECTOR_ELT_HI, and VPUSH_hi_64 is the combine of
// vshift(s0, broadcast(s1), 0, 8) -- one 64-bit element of shift into the
// concatenation, whose window is s0's elements 1.. followed by s1.
//
// The test is built so the other reading cannot pass. x2 is all ones except
// its TOP lane, which is 0xAB, and the pushed scalar is 0xCD. Reading it as a
// low-end push would put 0xCD in lane 0 and leave 0xAB where it was; reading
// it as the high end moves 0xAB down to lane 14 and puts 0xCD in lane 15.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:128 | FileCheck %s

	.text
	.globl _start
_start:	movxm	p0, #0x10000
	movxm	p1, #0x1003C
	movxm	r0, #0x00000001
	vbcst.32	x0, r0
	movxm	r1, #0x000000AB
	movxm	r2, #0x000000CD
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
	st	r1, [p1, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	x2, [p0, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vpush.hi.32	x3, x2, r2
	done

// x3 prints as its two 256-bit halves. The high one carries the answer: lane
// 14 is 0xAB, moved down from the top, and lane 15 is the scalar. The low half
// is untouched ones, which is what says the shift was by ONE element and not
// more.
//
// x2 is checked too, so a failure says whether the input or the push was wrong.
// CHECK-DAG: wh2 = 0xAB00000001000000010000000100000001000000010000000100000001
// CHECK-DAG: wh3 = 0xCD000000AB000000010000000100000001000000010000000100000001
// CHECK-DAG: wl3 = 0x100000001000000010000000100000001000000010000000100000001
