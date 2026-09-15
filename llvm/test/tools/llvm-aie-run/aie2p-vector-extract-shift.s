//===- aie2p-vector-extract-shift.s -------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vextract.N: one lane widened into a scalar register. vaddSign is the
// widening, which AIEBaseInstrPatterns.td states rather than implies -- its
// Extract_512 multiclass takes (UnsignedOpc, SignedOpc) and binds
// vextract_zext to the first and vextract_sext to the second, and aie2p
// passes vaddSign0 then vaddSign1. Lane 4 here is 0xAB, whose top bit is set,
// so the two widenings disagree; the lane is not 0, so an implementation that
// ignored the index would print the 0xFF lane instead.
//
// vshift: the 64-byte window of the 1024-bit concatenation {s2 : s1} at a
// BYTE offset. Both of those are pinned by an opcode already tested here --
// AIE2PInstrPatterns.td combines vshift(s0, bcst(s1), 0x0, 0x8) into
// VPUSH_hi_64, which drops one 64-bit element off s0's bottom and appends s1.
// That holds only if 8 counts bytes and s1 is the LOW half. Shifting by 4
// below moves x0 down one word and pulls exactly one word of x1 into the top,
// so a byte/element mixup or a swapped pair changes the answer.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:128 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10004
	movxm	r0, #0xFFFFFFFF
	movxm	r1, #0x000000AB
	st	r0, [p0, #0]
	st	r1, [p1, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	// x0 = [0xFFFFFFFF, 0x000000AB, 0, ...]; byte 4 is 0xAB.
	vlda	x0, [p0, #0]
	movxm	r5, #0x11111111
	vbcst.32	x1, r5
	movxm	r6, #0x00000004
	nop
	nop
	nop
	nop
	nop
	vextract.8	r2, x0, #4, vaddsign1
	vextract.8	r3, x0, #4, vaddsign0
	vextract.16	r4, x0, #0, vaddsign1
	nop
	nop
	nop
	nop
	nop
	nop
	vshift	x2, x0, x1, r6
	nop
	nop
	nop
	nop
	nop
	nop
	done

// Lane 4 read as SIGNED: 0xAB widens to 0xFFFFFFAB.
// CHECK-DAG: r2 = 0xFFFFFFAB{{$}}

// The same lane read as UNSIGNED is the discriminating pair.
// CHECK-DAG: r3 = 0xAB{{$}}

// Half-word lane 0 is 0xFFFF, signed, so it fills the register. This is the
// width check: a .8 reading would give 0xFFFFFFFF too, but an off-by-one in
// the lane stride would land on 0x00AB.
// CHECK-DAG: r4 = 0xFFFFFFFF{{$}}

// vshift by 4 bytes. The window starts one word into x0, so word 0 becomes
// x0's 0xAB and the 0xFFFFFFFF falls off the bottom -- which is what says x0
// is the LOW half. Exactly one word of x1 is pulled in, at the very top, so
// the high half carries a single 0x11111111 above seven zero words.
// CHECK-DAG: wl2 = 0xAB{{$}}
// CHECK-DAG: wh2 = 0x11111111{{0{56}$}}
