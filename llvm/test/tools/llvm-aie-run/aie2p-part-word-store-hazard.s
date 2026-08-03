//===- aie2p-part-word-store-hazard.s -----------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// The part-word-store structural hazard, which is the one real stall AIE2P's
// itineraries describe.
//
//   II_ST_s16_idx_imm       IsPartWordStore    = InstrStage<7, [PART_WORD_STORE], 0>
//   II_LDA_dms_lda_idx_imm  AvoidPartWordStore = InstrStage<1, [PART_WORD_STORE], 0, Reserved>
//
// Twelve sub-word store forms take that unit as Required for 7 cycles, and 596
// memory forms take it as Reserved, so a load issued inside the window has to
// wait for it. Compiled code never does -- the scheduler models this, and every
// aievec test in this directory runs with cycles equal to bundles -- so it takes
// hand-written assembly to reach.
//
// The load here is offered the cycle after the store, so it waits 6 for the
// 7-cycle window to close. `nop` reserves nothing, which is what makes the
// second half a control: the same store followed by nops costs nothing.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --scratch=0x10000:64 --dump-mem=0x10000:8 | FileCheck %s

	.text
	.globl _start
_start:	movxm	p0, #0x10000
	movxm	p1, #0x10010
	mov	r0, #0x11
	mov	r1, #0x22
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	st.s16	r0, [p0, #0]
	lda	r2, [p0, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	st.s16	r1, [p1, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	done

// 6 cycles for the one load that had to wait, and 6 rather than 12 is what
// says the second store's nops cost nothing.
// CHECK: stall-cycles: 6

// The store still lands, so the stall did not cost correctness.
// CHECK: mem[0x10000] = 11 00 00 00 00 00 00 00
