//===- aie2p-hardware-loop.s --------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// A zero-overhead loop has no branch in the body: le names the last bundle and
// the back edge is hardware behaviour. lc is the iteration count, which is why
// the compiler emits "add.nc lc, rN, #0" for a loop of rN iterations.

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs | FileCheck %s

	.text
	.globl _start
_start:
// The body sits at byte 24 and its last bundle at byte 28; .text is at zero in
// an object this test never links, so ls and le take those as constants.
	mov	r0, #4
	movxm	ls, #24
	movxm	le, #28
	add.nc	lc, r0, #0
	mov	r1, #0
	add	r1, r1, #1
	add	r1, r1, #10
	done

// 5 setup + 4 iterations of 2 bundles
// CHECK: bundles: 13

// 4 * (1 + 10)
// CHECK-DAG: r1 = 0x2C

// The loop drained.
// CHECK-NOT: lc =
