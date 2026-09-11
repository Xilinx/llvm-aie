//===- aie2p-vector-load.s ----------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:96 | FileCheck %s

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
	vlda	wl2, [p1, #0]
	done

// The 512-bit load spans both halves of a composed register, and the two
// halves of memory differ, so this is the check the broadcast tests could not
// be: a broadcast repeats every 64 bits at most, so its halves always match
// and swapping them changes nothing. Verified to produce these two the other
// way round when sub_256_hi and sub_256_lo are transposed, so it discriminates
// rather than decorates.
// Anchored: the rest of each half is the zeroed scratch, and leading zeros are
// not printed, so an unanchored match would also accept a half that had
// swallowed both patterns.
// CHECK-DAG: wl0 = 0x{{AAAAAAAA$}}
// CHECK-DAG: wh0 = 0x{{BBBBBBBB$}}

// The 256-bit form lands in a register that has its own storage, so it takes
// the other path out of the register file. Same bytes p1 points at.
// CHECK-DAG: wl2 = 0x{{BBBBBBBB$}}
