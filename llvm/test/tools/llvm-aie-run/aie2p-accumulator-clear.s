//===- aie2p-accumulator-clear.s ----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vclr over a whole 2048-bit accumulator. The def takes no source operand, so
// the only thing to establish is the constant it writes and how far that write
// reaches.
//
// The register dump cannot show the result directly: it skips a register that
// is entirely zero, which is precisely the state under test. So the zero is
// read out through an addition instead, and the absence is checked separately:
//
//  - IT IS ZERO, EVERYWHERE. dm0 is cleared and then added to dm1, whose four
//    quarters differ from each other. Zero is the additive identity, so dm2
//    must come out equal to dm1 quarter for quarter. A clear that missed the
//    top half, or wrote anything but zero, moves the quarter it touched.
//
//  - IT REALLY CLEARED, rather than the sum merely looking right. dm0 is
//    pre-loaded with four non-zero quarters first, so a vclr modelled as a
//    no-op leaves those in place and every quarter of dm2 is wrong.
//
//  - AND NOT MORE. bmll3 is loaded and never named again. It is not part of
//    dm0, so it must survive; it is also what keeps the CLEARED-NOT lines
//    below from passing vacuously, by proving the dump does print registers of
//    this class under these names.
//
// The addend's lanes all have their top nibble set, which the dump imposes
// rather than the instruction: it prints through APInt::toString, which drops
// leading zeros.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --coverage --scratch=0x10000:512 > %t.out
// RUN: FileCheck %s < %t.out
// RUN: FileCheck %s --check-prefix=CLEARED < %t.out

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10040
	movxm	p2, #0x10080
	movxm	p3, #0x100C0
	movxm	p4, #0x10100
	movxm	p5, #0x10140
	movxm	p6, #0x10180
	movxm	p7, #0x101C0
	// The four quarters vclr has to reach, none of them zero.
	movxm	r0, #0xD5D5D5D5
	movxm	r1, #0xE6E6E6E6
	movxm	r2, #0xF7F7F7F7
	movxm	r3, #0x88888888
	// The addend, which the sum has to reproduce unchanged.
	movxm	r4, #0x91919191
	movxm	r5, #0xA2A2A2A2
	movxm	r8, #0xB3B3B3B3
	movxm	r9, #0xC4C4C4C4
	// amode I32, and no bit that rewrites an operand.
	movxm	r7, #0x0
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
	vbcst.32	x6, r8
	vbcst.32	x7, r9
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
	vst	x6, [p6, #0]
	vst	x7, [p7, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	bmll0, [p0, #0]
	vlda	bmlh0, [p1, #0]
	vlda	bmhl0, [p2, #0]
	vlda	bmhh0, [p3, #0]
	vlda	bmll1, [p4, #0]
	vlda	bmlh1, [p5, #0]
	vlda	bmhl1, [p6, #0]
	vlda	bmhh1, [p7, #0]
	// The witness vclr must leave alone.
	vlda	bmll3, [p0, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	vclr	dm0
	nop
	nop
	nop
	nop
	nop
	nop
	vadd	dm2, dm0, dm1, r7
	nop
	nop
	nop
	nop
	nop
	nop
	done

// CHECK-DAG: modelled   VCLR
// CHECK-DAG: modelled   VADD_vmac_cm2_add_reg

// The addend, so the sum below is read against something visible.
// CHECK-DAG: bmll1 = 0x{{(91919191){16}$}}
// CHECK-DAG: bmlh1 = 0x{{(A2A2A2A2){16}$}}
// CHECK-DAG: bmhl1 = 0x{{(B3B3B3B3){16}$}}
// CHECK-DAG: bmhh1 = 0x{{(C4C4C4C4){16}$}}

// dm0 + dm1 with dm0 cleared, so every quarter is the addend's. Any quarter
// vclr left behind lands here instead.
// CHECK-DAG: bmll2 = 0x{{(91919191){16}$}}
// CHECK-DAG: bmlh2 = 0x{{(A2A2A2A2){16}$}}
// CHECK-DAG: bmhl2 = 0x{{(B3B3B3B3){16}$}}
// CHECK-DAG: bmhh2 = 0x{{(C4C4C4C4){16}$}}

// Outside dm0 and untouched.
// CHECK-DAG: bmll3 = 0x{{(D5D5D5D5){16}$}}

// The clear itself, as an absence: a register that is entirely zero is skipped
// by the dump. These run before the coverage line, which the dump precedes, so
// they cover the whole register listing.
// CLEARED-NOT: bmll0 =
// CLEARED-NOT: bmlh0 =
// CLEARED-NOT: bmhl0 =
// CLEARED-NOT: bmhh0 =
// CLEARED: modelled   VCLR
