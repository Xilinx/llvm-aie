//===- aie2p-scalar-mac.s -----------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// mac and msc, the scalar multiply-accumulate. Four operands where the encoding
// carries three registers: $a0 is tied to $d0, so the addend is the
// destination's own prior value.
//
// AIE2PInstrPatterns selects these from (add $rd, (mul $rs1, $rs2)) and the sub
// of the same, at i32 and through plain DAG nodes -- which are modular, so
// nothing here saturates. The operands say so rather than being taken on trust:
//
//  - THE PRODUCT IS THE ADDEND, not an operand. 7 + 3*5 is 0x16, where adding
//    before multiplying gives 0x32 and dropping the accumulate gives 0xF.
//
//  - msc SUBTRACTS IT, in that order. 100 - 3*5 is 0x55; the other way round is
//    0xFFFFFFAB.
//
//  - IT WRAPS. 0xffffffff + 2*2 is 0x3, where an unsigned-saturating add stays
//    at 0xffffffff. 0x80000000 - 1*1 is 0x7fffffff, where a signed-saturating
//    subtract stays at 0x80000000.
//
//  - THE PRODUCT IS TRUNCATED, not widened into the result. 0x10001 squared is
//    0x100020001, and only its low half survives.
//
//  - SIGN DOES NOT REACH IT. 100 + (-3)*5 is 0x55, the same value the unsigned
//    reading of -3 produces, because a 32-bit product shares its low half
//    either way. This confirms the sign-agnostic claim rather than choosing
//    between two readings.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --coverage | FileCheck %s

	.text
	.globl _start
_start:
	// The multiplicands, shared by most of the arms below.
	mov	r1, #3
	mov	r2, #5
	mov	r0, #7
	movxm	r3, #100
	movxm	r4, #0xFFFFFFFF
	mov	r5, #2
	movxm	r6, #0x00010001
	mov	r7, #0
	movxm	r8, #-3
	movxm	r9, #100
	movxm	r10, #0x80000000
	mov	r11, #1
	nop
	nop
	nop
	nop
	nop
	nop
	mac	r0, r0, r1, r2
	msc	r3, r3, r1, r2
	mac	r4, r4, r5, r5
	mac	r7, r7, r6, r6
	mac	r9, r9, r8, r2
	msc	r10, r10, r11, r11
	nop
	nop
	nop
	nop
	nop
	nop
	done

// CHECK-DAG: modelled   MAC
// CHECK-DAG: modelled   MSC

// 7 + 3*5.
// CHECK-DAG: r0 = 0x16
// 100 - 3*5.
// CHECK-DAG: r3 = 0x55
// 0xffffffff + 2*2, wrapped.
// CHECK-DAG: r4 = 0x3
// The low half of 0x10001 squared.
// CHECK-DAG: r7 = 0x20001
// 100 + (-3)*5.
// CHECK-DAG: r9 = 0x55
// 0x80000000 - 1*1, wrapped.
// CHECK-DAG: r10 = 0x7FFFFFFF
