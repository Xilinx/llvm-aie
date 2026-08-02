//===- aie2p-exposed-pipeline.s -----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// A register holds two values at once, to instructions a few bundles apart.
//
// AIE has no interlocks: an instruction's result appears at its own def cycle
// and a consumer samples at its own use cycle, so a consumer scheduled inside
// the producer's window reads what the register held BEFORE. That is not a
// corner case -- it is how the compiler packs these loops, and it is the
// property this executor got wrong until it was measured on silicon.
//
// `lda` writes its destination at cycle 7. The `mov` two bundles later reads
// at cycle 1, i.e. absolute +3, so it still sees the OLD r0. The second `mov`
// is placed past the load's landing and sees the loaded value. One register,
// two readers, two answers.

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:64 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	r0, #0xAAAA
	nop
	// Seed memory with a value distinguishable from r0's.
	st	r0, [p0, #0]
	movxm	r1, #0x5555
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	st	r1, [p0, #4]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// r0 = mem[+4] = 0x5555, landing 7 cycles from here.
	lda	r0, [p0, #4]
	nop
	// Inside the load's window: still the old r0.
	mov	r2, r0
	nop
	nop
	nop
	nop
	nop
	nop
	// Past it: the loaded value.
	mov	r3, r0
	nop
	nop
	done

// The same register, read twice, two different answers -- which is the whole
// point. r2 is what r0 held before the load landed.
// CHECK-DAG: r2 = 0xAAAA
// CHECK-DAG: r3 = 0x5555
