//===- aie2p-vector-bitwise.s -------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:64 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10020
	movxm	r0, #0xAAAAAAAA
	movxm	r1, #0xBBBBBBBB
	st	r0, [p0, #0]
	st	r1, [p1, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	x0, [p0, #0]
	movxm	r2, #0xF0F0F0F0
	vbcst.32	x1, r2
	nop
	nop
	nop
	nop
	nop
	nop
	vband	x2, x0, x1
	vbor	x3, x0, x1
	done

// One operand is loaded and asymmetric, the other broadcast and uniform, so
// each half of the result depends on a different half of x0. An op that read
// one half twice, or crossed the halves, would show up here.

// AAAAAAAA & F0F0F0F0 in the low word of each half; the rest of x0 is the
// zeroed scratch, so it masks to zero and is not printed.
// CHECK-DAG: wl2 = 0x{{A0A0A0A0$}}
// CHECK-DAG: wh2 = 0x{{B0B0B0B0$}}

// Or against the same mask leaves F0F0F0F0 everywhere x0 was zero, and
// AAAAAAAA|F0F0F0F0 = FAFAFAFA in the one word it was not.
// CHECK-DAG: wl3 = 0x{{(F0F0F0F0){7}FAFAFAFA$}}
// CHECK-DAG: wh3 = 0x{{(F0F0F0F0){7}FBFBFBFB$}}
