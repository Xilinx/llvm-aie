//===- aie2p-delay-slots.s ----------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// A branch becomes visible five bundles after it issues, so everything between
// jnz and the target runs on every iteration, taken or not.

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs | FileCheck %s

	.text
	.globl _start
_start:
	mov	r0, #3
	mov	r1, #0
	mov	r2, #0
// The loop body starts at byte 12, which is what jnz names below: .text sits
// at zero in an object this test never links.
	add	r1, r1, r0
	add	r0, r0, #-1
	jnz	r0, #12
	add	r2, r2, #1
	nop
	nop
	nop
	nop
	done

// 3 setup + 3 iterations of (3 body + 5 delay)
// CHECK: bundles: 27

// 3 + 2 + 1
// CHECK-DAG: r1 = 0x6

// The delay slot ran once per branch, including the one that fell through.
// CHECK-DAG: r2 = 0x3
