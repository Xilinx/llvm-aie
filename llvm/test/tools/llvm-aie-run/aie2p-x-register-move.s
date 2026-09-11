//===- aie2p-x-register-move.s -------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// VMOV_alu_mv_mv_x, the "vmov $dst, $src" whose class (OP_mMvBMXDst/Src) spans
// X, BM and FIFO registers. This models the X/BM arm -- a plain 512-bit copy,
// what real kernels use it for (e.g. "vmov x8, x4" in every disassembled
// GEMV/MHA core) -- and declines the FIFO arm rather than guessing at
// push/pop semantics no header states.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --coverage --scratch=0x10000:512 \
// RUN:   | FileCheck %s

// The FIFO arm of the same opcode: declined, not guessed.
// RUN: llvm-mc -triple aie2p -filetype=obj --defsym FIFO=1 %s -o %t.fifo.o
// RUN: not llvm-aie-run %t.fifo.o 2>&1 | FileCheck %s --check-prefix=FIFO

	.text
	.globl _start
_start:
.ifdef FIFO
	vmov	x4, sfl
	done
.else
	movxm	p0, #0x10000
	movxm	r0, #0x11111111
	movxm	r1, #0x22222222
	nop
	nop
	nop
	nop
	nop
	nop
	vbcst.32	x4, r0
	vbcst.32	x5, r1
	nop
	nop
	nop
	nop
	nop
	nop
	// bmll1 has no broadcast of its own -- round-trip x5 through memory,
	// the same load VMOV_alu_mv_mv_x's BM arm would see from a real kernel.
	vst	x5, [p0, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	bmll1, [p0, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	// X to X.
	vmov	x8, x4
	// BM to X: the composed-register arm the same opcode also carries.
	vmov	x9, bmll1
	nop
	nop
	nop
	nop
	nop
	nop
	done
.endif

// CHECK-DAG: modelled   VMOV_alu_mv_mv_x

// The sources, so the copies below are read against something visible. x is
// printed as its wh/wl halves, not by its own name.
// CHECK-DAG: wh4 = 0x{{(11111111){8}$}}
// CHECK-DAG: wl4 = 0x{{(11111111){8}$}}
// CHECK-DAG: bmll1 = 0x{{(22222222){16}$}}

// Both copies land whole and unmixed: x8 got x4's pattern, x9 got bmll1's.
// CHECK-DAG: wh8 = 0x{{(11111111){8}$}}
// CHECK-DAG: wl8 = 0x{{(11111111){8}$}}
// CHECK-DAG: wh9 = 0x{{(22222222){8}$}}
// CHECK-DAG: wl9 = 0x{{(22222222){8}$}}

// FIFO: error: fault at 0x0: VMOV_alu_mv_mv_x: sfl is a load/store FIFO port; this model has no queue to move it through
