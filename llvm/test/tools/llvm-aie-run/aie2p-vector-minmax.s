//===- aie2p-vector-minmax.s --------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vmax_lt.N / vmin_ge.N: the elementwise min or max AND the comparison that
// chose it, in one instruction with two defs.
//
// Both readings the encoding leaves open are pinned here, and the inputs are
// chosen so neither can pass by accident:
//
//  - SIGNEDNESS. The suffix is a 1-bit register (vaddSign0/vaddSign1), and
//    aie2p/AIE2PInstrPatterns.td says which is which: aie_setlt selects
//    VLT_*_vaddSign1 where aie_setult selects VLT_*_vaddSign0. Lane 0 holds
//    0xFFFFFFFF, the LEAST value read signed and the GREATEST read unsigned,
//    so the two readings disagree in both defs at once.
//
//  - MASK POLARITY. vmax_lt's mask is s1 < s2 and vmin_ge's is s1 >= s2, from
//    the standalone VLT/VGE they share a datapath with. Taken at the same
//    signedness the two masks here are exact complements, 0xFFFE and 0x0001,
//    so an inverted reading of either prints the other's answer.
//
// The .8 form is included because its mask does not fit a single register:
// 64 lanes need the 64-bit l8 pair, where .16 and .32 write r16.
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
	nop
	nop
	nop
	nop
	nop
	nop
	// x0 = [0xFFFFFFFF, 0, 0, ...]: lane 0 against the zeroed scratch.
	vlda	x0, [p0, #0]
	movxm	r1, #0x00000007
	vbcst.32	x1, r1
	nop
	nop
	nop
	nop
	nop
	nop
	vmax_lt.32	x2, r16, x0, x1, vaddsign1
	nop
	nop
	nop
	nop
	nop
	nop
	mov	r2, r16
	vmax_lt.32	x3, r16, x0, x1, vaddsign0
	nop
	nop
	nop
	nop
	nop
	nop
	mov	r3, r16
	vmin_ge.32	x4, r16, x0, x1, vaddsign0
	nop
	nop
	nop
	nop
	nop
	nop
	mov	r4, r16
	vmax_lt.8	x5, r17:r16, x0, x1, vaddsign1
	nop
	nop
	nop
	nop
	nop
	nop
	done

// SIGNED max. -1 < 7 in lane 0 and 0 < 7 everywhere else, so every lane takes
// x1's 7 and every mask bit sets.
// CHECK-DAG: wl2 = 0x7{{(00000007){7}$}}
// CHECK-DAG: wh2 = 0x7{{(00000007){7}$}}
// CHECK-DAG: r2 = 0xFFFF{{$}}

// UNSIGNED max on the same inputs is the opposite answer in BOTH defs:
// 0xFFFFFFFF is now the greatest value, so lane 0 keeps it and clears its
// mask bit. Reading vaddsign0 as signed would print the block above instead.
// CHECK-DAG: wl3 = 0x7{{(00000007){6}FFFFFFFF$}}
// CHECK-DAG: wh3 = 0x7{{(00000007){7}$}}
// CHECK-DAG: r3 = 0xFFFE{{$}}

// UNSIGNED min, same signedness as the case above it. 0xFFFFFFFF >= 7 holds
// so lane 0 takes the 7; no other lane's 0 is >= 7, so each keeps its zero.
// The mask is 0x0001, the exact complement of the 0xFFFE above -- which is
// what says the two mnemonics carry the two halves of one comparison rather
// than the same one twice. Only 16 of r16's 32 bits are lanes here; the model
// zeroes the rest.
// CHECK-DAG: wl4 = 0x7{{$}}
// CHECK-DAG: r4 = 0x1{{$}}

// SIGNED max at byte width, which is where the mask needs the l8 pair. x1's
// broadcast 7 is one set byte in four, so lane 4k takes the 7 and the three
// bytes above it stay 0; lanes 0-3 are the 0xFF bytes, all less than what
// they meet. The result is x1 back, and the mask is those two patterns
// interleaved.
// CHECK-DAG: wl5 = 0x7{{(00000007){7}$}}
// CHECK-DAG: r16 = 0x1111111F{{$}}
// CHECK-DAG: r17 = 0x11111111{{$}}
