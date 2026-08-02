//===- aie2p-register-offset-memory.s ------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// The `[$ptr, $dj]` load/store forms, which take the offset from a register
// rather than the encoding. Neither the def (`(outs eR:$dst), (ins eP:$ptr,
// eDJ:$dj)`, no ptr_out) nor the name (no `pstm`) modifies the pointer, so p0
// is checked unchanged after every access below. dj is a byte offset, not an
// element index.
//
// These are what a compiled loop indexes with: every scalar aievec_tests dut
// lowers its subscript to `movs dj, <i>` followed by one of these.
//
// The narrow stores also cover source narrowing: r3 holds 0xFFFFFF85 and
// `st.s8` must commit one byte of it. Taking the store value at the store's
// width straight from the 32-bit register asserts inside APInt.

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:64 \
// RUN:   --dump-mem=0x10000:16 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	sp, #0x10040
	movxm	p0, #0x10000

	movx	r1, #8
	movx	r4, #4
	movx	r2, #6
	movs	dj0, r1
	movs	dj1, r4
	movs	dj2, r2
	nop

	// One store per width, each addressed through dj.
	movxm	r0, #0x11223344
	movxm	r3, #0xFFFFFF85
	movxm	r10, #0xBEEF
	nop
	st	r0, [p0, #0]
	st	r0, [p0, dj0]
	st.s8	r3, [p0, dj1]
	st.s16	r10, [p0, dj2]
	nop
	nop

	// One load per width and signedness, all through dj.
	lda	r6, [p0, dj0]
	lda.s8	r5, [p0, dj1]
	lda.u8	r9, [p0, dj1]
	lda.s16	r7, [p0, dj2]
	lda.u16	r8, [p0, dj2]
	nop
	nop
	nop
	nop
	nop
	nop

	// paddxm [sp], #imm: the frame adjust, sp implicit in the encoding.
	paddxm	[sp], #-64
	nop
	nop
	done

// The pointer is untouched by every [p0, dj] access above, and sp moved by
// exactly the frame amount.
// CHECK-DAG: p0 = 0x10000
// CHECK-DAG: sp = 0x10000

// Sign- and zero-extending loads of the same bytes disagree, which is what
// makes them distinguishable at all.
// CHECK-DAG: r5 = 0xFFFFFF85
// CHECK-DAG: r9 = 0x85
// CHECK-DAG: r7 = 0xFFFFBEEF
// CHECK-DAG: r8 = 0xBEEF
// CHECK-DAG: r6 = 0x11223344

// Byte 4 is 0x85 alone and byte 5 stayed zero: the s8 store took the low byte
// of r3 and wrote no more than one.
// CHECK: mem[0x10000] = 44 33 22 11 85 00 ef be 44 33 22 11 00 00 00 00
