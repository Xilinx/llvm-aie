//===- aie2p-memory.s ---------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:64 \
// RUN:   --dump-mem=0x10000:12 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #65536
	mov	r0, #42
	mov	r1, #7
	st	r0, [p0], #4
	nop
	st	r1, [p0], #4
	nop
	lda	r2, [p0, #-8]
	lda	r3, [p0, #-4]
	nop
	nop
	nop
	nop
	nop
	nop
	add	r4, r2, r3
	st	r4, [p0, #0]
	done

// CHECK-DAG: p0 = 0x10008
// CHECK-DAG: r2 = 0x2A
// CHECK-DAG: r3 = 0x7
// CHECK-DAG: r4 = 0x31

// Each post-increment store wrote at its pre-increment address.
// CHECK: mem[0x10000] = 2a 00 00 00 07 00 00 00 31 00 00 00
