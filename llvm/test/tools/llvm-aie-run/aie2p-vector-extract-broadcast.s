//===- aie2p-vector-extract-broadcast.s ---------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vextbcst.N: the lane at idx repeated across the destination -- vextract's
// lane select and vbcst's splat in one instruction. Unlike vextract there is
// no vaddsign pair, because source and destination lanes are the same width.
//
// The source words below differ in both halves (0x11112222 rather than
// 0x11111111), which is what makes the widths tell each other apart: an
// implementation that read the wrong N would splat a pattern that appears
// nowhere else in the four expectations. The lane is non-zero on three of the
// four, so ignoring idx fails too, and the .32 case is checked in both the
// imm and the register form.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:128 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10004
	movxm	p2, #0x10008
	movxm	p3, #0x1000C
	movxm	r0, #0x11112222
	movxm	r1, #0x33334444
	movxm	r2, #0x55556666
	movxm	r3, #0x77778888
	st	r0, [p0, #0]
	st	r1, [p1, #0]
	st	r2, [p2, #0]
	st	r3, [p3, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	// x0 = [0x11112222, 0x33334444, 0x55556666, 0x77778888, 0, ...]
	vlda	x0, [p0, #0]
	movxm	r6, #0x00000003
	nop
	nop
	nop
	nop
	nop
	nop
	vextbcst.16	x1, x0, #1
	vextbcst.32	x2, x0, #1
	vextbcst.64	x3, x0, #1
	vextbcst.128	x4, x0, #0
	vextbcst.32	x5, x0, r6
	nop
	nop
	nop
	nop
	nop
	nop
	done

// Half-word lane 1 is the TOP half of word 0. Reading lane 1 as a byte would
// give 0x11 and as a word would give 0x33334444, so this pins the stride.
// CHECK-DAG: wl1 = 0x{{1{64}$}}
// CHECK-DAG: wh1 = 0x{{1{64}$}}

// Word lane 1 is word 1 whole.
// CHECK-DAG: wl2 = 0x{{(33334444){8}$}}
// CHECK-DAG: wh2 = 0x{{(33334444){8}$}}

// Doubleword lane 1 is words 2 and 3, word 2 low -- the same ordering vshift
// pins for the 1024-bit concatenation.
// CHECK-DAG: wl3 = 0x{{(7777888855556666){4}$}}
// CHECK-DAG: wh3 = 0x{{(7777888855556666){4}$}}

// Quadword lane 0 is the whole low 128 bits, so this one also says the splat
// covers the high half of the destination rather than leaving it alone.
// CHECK-DAG: wl4 = 0x{{(77778888555566663333444411112222){2}$}}
// CHECK-DAG: wh4 = 0x{{(77778888555566663333444411112222){2}$}}

// The register index form, taking lane 3 from r6 -- word 3, which no other
// expectation here splats.
// CHECK-DAG: wl5 = 0x{{(77778888){8}$}}
// CHECK-DAG: wh5 = 0x{{(77778888){8}$}}
