//===- aie2p-bfp16-register-move.s --------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// The ex form of vmov: a 576-bit copy of a bfp16 vector register. As with the
// accumulator moves there is no arithmetic here, so what is under test is the
// register file -- ex0 has no storage of its own and is assembled from the four
// leaves that tile it, [wl0, wh0] via x0 and [el0, eh0] via e0.
//
// ex is the first register whose MC layer offers two decompositions: the
// declared [x0, e0] and an inferred [ewl0, ewh0] that interleaves the same four
// leaves differently. Only the declared one occupies contiguous ranges, so the
// four values below arriving intact is the check that the contiguous partition
// is the one being used.
//
// What this pins, and what it does not. A copy reads and writes through the
// SAME placement map, so any self-consistent map round-trips: this cannot
// discriminate a transposed el0/eh0, and no modelled instruction can, because
// nothing yet views that storage two ways (vlda.pop.576 is gated on the LDA
// FIFO). It does pin that all 576 bits move and that every leaf participates --
// a map that dropped one, or that covered only x0's 512, fails to tile and the
// instruction faults rather than copying part of the register.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --coverage --scratch=0x10000:96 \
// RUN:   | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10020
	movxm	r0, #0xAAAAAAAA
	movxm	r1, #0xBBBBBBBB
	movxm	r2, #0xCCCCCCCC
	movxm	r3, #0xDDDDDDDD
	st	r0, [p0, #0]
	st	r1, [p1, #0]
	nop
	nop
	nop
	nop
	nop
	// The vector half, two 256-bit halves that differ.
	vlda	x0, [p0, #0]
	// The exponent half, distinct again so neither can stand in for the other.
	mov	el0, r2
	mov	eh0, r3
	nop
	nop
	nop
	nop
	nop
	nop
	vmov	ex1, ex0
	nop
	nop
	nop
	nop
	nop
	nop
	// Read the copied exponents back out through the scalar file, which is the
	// only way they are printable.
	mov	r4, el1
	mov	r5, eh1
	nop
	nop
	nop
	nop
	nop
	nop
	done

// CHECK-DAG: modelled   VMOV_alu_mv_mv_ex
// CHECK-DAG: modelled   MOV_alu_mv_mv_mv_e_mv_r_to_el
// CHECK-DAG: modelled   MOV_alu_mv_mv_mv_e_mv_el_to_r

// The source, so the copy below is read against something visible. Anchored
// because the rest of each half is zeroed scratch and leading zeros are not
// printed, so an unanchored match would accept a half holding both patterns.
// CHECK-DAG: wl0 = 0x{{AAAAAAAA$}}
// CHECK-DAG: wh0 = 0x{{BBBBBBBB$}}
// CHECK-DAG: el0 = 0x{{CCCCCCCC$}}
// CHECK-DAG: eh0 = 0x{{DDDDDDDD$}}

// All 576 bits, both files. The vector leaves are visible directly; the
// exponent leaves come back through r4 and r5.
// CHECK-DAG: wl1 = 0x{{AAAAAAAA$}}
// CHECK-DAG: wh1 = 0x{{BBBBBBBB$}}
// CHECK-DAG: r4 = 0x{{CCCCCCCC$}}
// CHECK-DAG: r5 = 0x{{DDDDDDDD$}}
