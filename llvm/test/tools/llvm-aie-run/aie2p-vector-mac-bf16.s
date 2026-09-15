//===- aie2p-vector-mac-bf16.s -------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vmac.f: elementwise bf16 x bf16 into f32, accumulating a prior f32 lane in.
// Same conf decode and multiply as vmul.f (AMODE_FP32, BMODE_16x16,
// CMODE_BF16xBF16_1_elem_1 = 60), with $acc1 added to the product.
//
// dm1 is seeded by vmul.f: 2.0 * 3.0 = 6.0, exact in f32. The vmac.f under
// test then computes 1.0 * 1.0 + dm1 = 7.0. All three values (6.0, 1.0, 7.0)
// are exact in f32, so a correct read needs no rounding-mode assumption --
// the accumulate add is not otherwise checked for exactness, unlike the
// multiply.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:256 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10040
	movxm	p2, #0x10080
	movxm	p3, #0x100c0
	// Lane 0 only; every other lane is 0.0 x 0.0.
	movxm	r0, #0x00004000  // x0 lane0 = 2.0
	movxm	r1, #0x00004040  // x1 lane0 = 3.0
	movxm	r10, #0x00003F80 // x2 lane0 = 1.0
	movxm	r11, #0x00003F80 // x3 lane0 = 1.0
	st	r0, [p0, #0]
	st	r1, [p1, #0]
	st	r10, [p2, #0]
	st	r11, [p3, #0]
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	x0, [p0, #0]
	vlda	x1, [p1, #0]
	vlda	x2, [p2, #0]
	vlda	x3, [p3, #0]
	movxm	r2, #60
	nop
	nop
	nop
	nop
	nop
	nop
	vmul.f	dm1, x0, x1, r2
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	vmac.f	dm0, dm1, x2, x3, r2
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	done

// 2.0 * 3.0 + 1.0 * 1.0 = 7.0 -> f32 0x40E00000, in the lane-0 leaf.
// CHECK-DAG: bmll0 = 0x40E00000{{$}}
