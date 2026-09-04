//===- aie2p-scalar-alu.s -----------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs \
// RUN:   | FileCheck %s --implicit-check-not={{'^r(8|11) ='}}

	.text
	.globl _start
_start:
	mov	r0, #12
	mov	r1, #5
	add	r2, r0, r1
	sub	r3, r0, r1
	and	r4, r0, r1
	or	r5, r0, r1
	xor	r6, r0, r1
	lt	r7, r1, r0
	ge	r8, r1, r0
	eq	r9, r0, r0
	ne	r10, r0, r1
	eqz	r11, r1
	nez	r12, r1
	clz	r13, r1
	popcount r14, r0
	mov	r15, #-9
	abs	r16, r15
	extend.s8 r17, r15
	lshl	r18, r0, r1
	mov	r19, #-2
	lshl	r20, r0, r19
	done

// CHECK-DAG: r2 = 0x11
// CHECK-DAG: r3 = 0x7
// CHECK-DAG: r4 = 0x4
// CHECK-DAG: r5 = 0xD
// CHECK-DAG: r6 = 0x9
// CHECK-DAG: r7 = 0x1
// CHECK-DAG: r9 = 0x1
// CHECK-DAG: r10 = 0x1
// CHECK-DAG: r12 = 0x1
// CHECK-DAG: r13 = 0x1D
// CHECK-DAG: r14 = 0x2
// CHECK-DAG: r16 = 0x9
// CHECK-DAG: r17 = 0xFFFFFFF7
// CHECK-DAG: r18 = 0x180

// A negative shift amount is a right shift: 12 >> 2.
// CHECK-DAG: r20 = 0x3

// ge and eqz are false here, and --print-regs omits zeroed registers, which is
// what the implicit-check-not on the RUN line pins down.
