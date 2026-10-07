//===- aie2p-mac-elementwise.s ------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vmul/vmac, elementwise shape.
//
// The lane-feeding layout is not in the opcode -- one opcode serves every
// shape -- and it is not hidden either: it is in the $acc operand, a
// configuration word whose layout is llvm-aie's own (aie2p_compute_control in
// clang/lib/Headers/aie2p/aie2p_vmult.h). elem_64 is amode 0, bmode 1,
// variant 1, which is (1<<3)|(1<<5) = 40, plus (1<<9)|(1<<8) = 768 when both
// operands are signed.
//
// Only that shape is modelled. Every other triple faults WITH ITS NUMBERS, so
// an unimplemented shape says which one it was rather than quietly producing a
// plausible product -- checked below.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs | FileCheck %s

// RUN: llvm-mc -triple aie2p -filetype=obj --defsym BADSHAPE=1 %s -o %t.bad.o
// RUN: not llvm-aie-run %t.bad.o 2>&1 | FileCheck %s --check-prefix=BADSHAPE

	.text
	.globl _start
_start:
	// Bytes 02 00 FF 03, so signed lanes are 2, 0, -1, 3 and unsigned ones
	// are 2, 0, 255, 3. Squaring separates the two readings in exactly the
	// lane holding 0xFF.
	movxm	r0, #0x03FF0002
	vbcst.32	x0, r0
	movxm	r4, #808
	movxm	r5, #40
.ifdef BADSHAPE
	// amode 0, bmode 1, variant 2 -- conv_8x8_8ch, not modelled.
	movxm	r6, #72
.endif
	nop
	nop
	nop
	nop
	nop
	nop
	nop
.ifdef BADSHAPE
	vmul	dm0, x0, x0, r6
	done
.endif
.ifndef BADSHAPE
	// Signed, then unsigned.
	vmul	dm0, x0, x0, r4
	vmul	dm1, x0, x0, r5
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	// And accumulate the signed products onto themselves.
	vmac	dm2, dm0, x0, x0, r4
	done
.endif

// Signed: lanes are 4, 0, 1, 9 low to high, so printed high to low the top
// lane is 9 and its leading zeros go.
// CHECK-DAG: bmll0 = 0x{{9000000010000000000000004(00000009000000010000000000000004){3}$}}

// Unsigned: 255 squared is 65025, so that lane reads 0000FE01 instead of 1.
// CHECK-DAG: bmll1 = 0x{{90000FE010000000000000004(000000090000FE010000000000000004){3}$}}

// Accumulated: every lane doubled.
// CHECK-DAG: bmll2 = 0x{{12000000020000000000000008(00000012000000020000000000000008){3}$}}

// An unmodelled shape names itself rather than inventing a product.
// BADSHAPE: MAC shape amode=0 bmode=1 variant=2 is not modelled
