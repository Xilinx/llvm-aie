//===- aie2p-return-in-first-bundle.s -----------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// `ret` as the FIRST instruction at the call target, which is the tight case
// for when the link register becomes readable.
//
// jl writes lr as its branch resolves, and the target's first bundle runs the
// very next cycle. aie2p-call-return.s puts a `mov` before its `ret`, so it
// has a cycle of slack and stays green even if lr lands one cycle late; here
// there is none. Compiler output hits this whenever a callee's entry bundle
// carries the return, so it is not a synthetic corner.

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs | FileCheck %s

	.text
	.globl _start
_start:
	mov	r0, #7
	jl	#28
	nop
	nop
	nop
	nop
	nop
// Return point, byte 20. Reached only through func's ret.
	mov	r1, #99
	done
func:
	ret	lr
	nop
	nop
	nop
	nop
	nop

// r1 is written only after the return, so this is the whole assertion: the
// link register was readable by the instruction the branch jumped to. With it
// landing a cycle late, ret reads a stale lr and control never gets here.
// CHECK-DAG: r0 = 0x7
// CHECK-DAG: r1 = 0x63
// CHECK-DAG: lr = 0x14
