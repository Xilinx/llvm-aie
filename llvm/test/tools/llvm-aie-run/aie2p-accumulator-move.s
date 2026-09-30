//===- aie2p-accumulator-move.s -----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vmov.d and the cm form of vmov: whole-register copies of the 2048- and
// 1024-bit accumulators. There is no arithmetic in either, so what is actually
// under test is the register file -- how wide each class is, and where its
// pieces sit.
//
// The two questions a copy can get wrong, and the inputs that answer them:
//
//  - HOW MUCH. dm0 is four distinct 512-bit quarters, so a vmov.d that moved
//    only the low 1024 bits leaves the top two zeroed rather than copied.
//
//  - WHICH END. The quarters differ from each other, so a copy that composed
//    the halves the other way round puts 0xCCCCCCCC where 0xAAAAAAAA belongs
//    instead of landing on an indistinguishable pattern.
//
//  - AND NOT MORE. dm2 is written twice: 0xEEEEEEEE and 0xFFFFFFFF into its
//    high half first, then a cm-form vmov into its low half. cml2 and cmh2 are
//    the two halves of one dm2, so a cm move that ran at 2048 bits would erase
//    the markers. This is the only check that separates the two widths.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --coverage --scratch=0x10000:512 \
// RUN:   | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10040
	movxm	p2, #0x10080
	movxm	p3, #0x100C0
	movxm	p4, #0x10100
	movxm	p5, #0x10140
	movxm	r0, #0xAAAAAAAA
	movxm	r1, #0xBBBBBBBB
	movxm	r2, #0xCCCCCCCC
	movxm	r3, #0xDDDDDDDD
	movxm	r4, #0xEEEEEEEE
	movxm	r5, #0xFFFFFFFF
	nop
	nop
	nop
	nop
	nop
	nop
	vbcst.32	x0, r0
	vbcst.32	x1, r1
	vbcst.32	x2, r2
	vbcst.32	x3, r3
	vbcst.32	x4, r4
	vbcst.32	x5, r5
	nop
	nop
	nop
	nop
	nop
	nop
	vst	x0, [p0, #0]
	vst	x1, [p1, #0]
	vst	x2, [p2, #0]
	vst	x3, [p3, #0]
	vst	x4, [p4, #0]
	vst	x5, [p5, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	// dm0 = [A, B, C, D] over its four 512-bit quarters, low to high.
	vlda	bmll0, [p0, #0]
	vlda	bmlh0, [p1, #0]
	vlda	bmhl0, [p2, #0]
	vlda	bmhh0, [p3, #0]
	// The markers that the 1024-bit move must not reach.
	vlda	bmhl2, [p4, #0]
	vlda	bmhh2, [p5, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	vmov.d	dm1, dm0
	nop
	nop
	nop
	nop
	nop
	nop
	vmov	cml2, cml0
	nop
	nop
	nop
	nop
	nop
	nop
	done

// CHECK-DAG: modelled   VMOV_D
// CHECK-DAG: modelled   VMOV_alu_mv_mv_cm

// The source, so the copies below are read against something visible.
// CHECK-DAG: bmll0 = 0x{{(AAAAAAAA){16}$}}
// CHECK-DAG: bmlh0 = 0x{{(BBBBBBBB){16}$}}
// CHECK-DAG: bmhl0 = 0x{{(CCCCCCCC){16}$}}
// CHECK-DAG: bmhh0 = 0x{{(DDDDDDDD){16}$}}

// All 2048 bits, quarter for quarter. A 1024-bit move drops the last two
// lines; a swapped composition reorders all four.
// CHECK-DAG: bmll1 = 0x{{(AAAAAAAA){16}$}}
// CHECK-DAG: bmlh1 = 0x{{(BBBBBBBB){16}$}}
// CHECK-DAG: bmhl1 = 0x{{(CCCCCCCC){16}$}}
// CHECK-DAG: bmhh1 = 0x{{(DDDDDDDD){16}$}}

// The cm form moved cml0 into the low half of dm2 and stopped there. The
// markers are the discriminator: at 2048 bits they would read C and D.
// CHECK-DAG: bmll2 = 0x{{(AAAAAAAA){16}$}}
// CHECK-DAG: bmlh2 = 0x{{(BBBBBBBB){16}$}}
// CHECK-DAG: bmhl2 = 0x{{(EEEEEEEE){16}$}}
// CHECK-DAG: bmhh2 = 0x{{(FFFFFFFF){16}$}}
