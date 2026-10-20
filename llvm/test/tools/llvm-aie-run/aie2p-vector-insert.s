//===- aie2p-vector-insert.s --------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vinsert.32: one lane of a 512-bit register replaced, the rest of it carried
// through. It is vextract's inverse, so vextract is its oracle and the two
// have to agree on the lane convention or the round trip below stops being an
// identity.
//
// What each input pins down:
//
//  - OPERAND ORDER. Insert512Pat makes the last operand the value and the
//    middle one the index; the model taking them the other way round would
//    read lane 0xDEADBEEF and fault instead of finishing.
//
//  - LANE, NOT BYTE. The index is 15, so a byte reading would write inside
//    lane 3 and leave the top of the register alone. Here the top word is what
//    changes and the low half is untouched.
//
//  - EVERY OTHER LANE IS s1's. Only one word of a 16-lane register differs
//    from the source, checked in both halves rather than only the written one.
//
//  - THE ROUND TRIP. vextract lane 1 and vinsert it straight back reproduces
//    the source exactly, which no off-by-one in either instruction survives.
//
// The mIdxImm0 arm is assembled as a raw word because llvm-mc cannot parse the
// syntax its own printer emits for it -- every AIE2P instruction whose asm
// string carries a literal "#<digit>" (the four vinsert.N imm0 forms and both
// event forms) prints in a form the assembler rejects with "invalid operand
// for instruction". 0x1a003178 is the encoding of "vinsert.32 x4, x0, r29, r1"
// with the index-source bit clear, and --coverage below asserts that it really
// decoded to VINSERT_32_mIdxImm0 rather than to whatever a re-encoding might
// make of those bytes.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --coverage --scratch=0x10000:64 \
// RUN:   | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	// Lane 15 is past the reach of the store's own offset field, which is
	// c6s -- six bits signed, so [-32, 31].
	movxm	p1, #0x1003C
	movxm	r0, #0x11111111
	st	r0, [p0, #0]
	movxm	r0, #0x22222222
	st	r0, [p0, #4]
	movxm	r0, #0x33333333
	st	r0, [p1, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	// x0 = [0x11111111, 0x22222222, 0 ... 0, 0x33333333], lane 15 last.
	vlda	x0, [p0, #0]
	movxm	r29, #15
	movxm	r1, #0xDEADBEEF
	nop
	nop
	nop
	nop
	nop
	nop
	vinsert.32	x2, x0, r29, r1
	nop
	nop
	nop
	nop
	nop
	nop
	// The lane just written, read back out through the inverse instruction.
	vextract.32	r2, x2, r29, vaddsign0
	movxm	r29, #1
	nop
	nop
	nop
	nop
	nop
	nop
	vextract.32	r3, x0, r29, vaddsign0
	nop
	nop
	nop
	nop
	nop
	nop
	// Lane 1 put back where it came from: x3 has to be x0 again.
	vinsert.32	x3, x0, r29, r3
	nop
	nop
	nop
	nop
	nop
	nop
	// vinsert.32 x4, x0, #0, r1 -- see the header for why this is a raw word.
	.long	0x1a003178
	nop
	nop
	nop
	nop
	nop
	nop
	done

// CHECK-DAG: modelled   VINSERT_32_mIdxImm0
// CHECK-DAG: modelled   VINSERT_32_mR29_insert

// Lane 15 is the top word of the HIGH half, and the low half is s1's
// untouched. A byte-offset reading of the index would have left wh2 at
// 0x33333333 and damaged wl2 instead.
// CHECK-DAG: wl2 = 0x2222222211111111{{$}}
// CHECK-DAG: wh2 = 0xDEADBEEF{{0{56}$}}

// The same lane back out. This is also what makes the operand order
// observable: reading the value as the index would have faulted the run.
// CHECK-DAG: r2 = 0xDEADBEEF{{$}}

// Lane 1 of the source, on its way to being put back.
// CHECK-DAG: r3 = 0x22222222{{$}}

// Extract-then-insert at the same lane is the identity, in both halves.
// CHECK-DAG: wl3 = 0x2222222211111111{{$}}
// CHECK-DAG: wh3 = 0x33333333{{0{56}$}}

// The mIdxImm0 arm writes lane 0 and nothing else -- lane 1 keeps 0x22222222
// and the high half keeps its 0x33333333.
// CHECK-DAG: wl4 = 0x22222222DEADBEEF{{$}}
// CHECK-DAG: wh4 = 0x33333333{{0{56}$}}
