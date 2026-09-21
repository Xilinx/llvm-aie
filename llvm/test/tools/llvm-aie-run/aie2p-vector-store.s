//===- aie2p-vector-store.s ---------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:192 \
// RUN:   --dump-mem=0x10040:68 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10020
	movxm	p2, #0x10040
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
	nop
	nop
	nop
	nop
	nop
	nop
	vst	x0, [p2, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	x1, [p2, #0]
	done

// Loaded from one place and stored to another, so the round trip has to
// preserve both the width and the order of the halves.
// CHECK-DAG: wl1 = 0x{{AAAAAAAA$}}
// CHECK-DAG: wh1 = 0x{{BBBBBBBB$}}

// The bytes themselves, which is the check the registers cannot be: it pins
// where in memory each half landed, not merely that the pair survived.
// The store writes exactly 64 bytes -- the four past the end stay zero, which
// is what says the width came from the register class and not from a
// hardcoded scalar size.
// CHECK: mem[0x10040] = aa aa aa aa 00 00 00 00 00 00 00 00 00 00 00 00
// CHECK-SAME: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// CHECK-SAME: bb bb bb bb 00 00 00 00 00 00 00 00 00 00 00 00
// CHECK-SAME: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// CHECK-SAME: 00 00 00 00
