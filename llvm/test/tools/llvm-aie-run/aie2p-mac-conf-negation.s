//===- aie2p-mac-conf-negation.s ----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vmul/vmac conf bits sub0 (bit 11) and sub1 (bit 12): two negations carried by
// the configuration word rather than by the opcode.
//
// aie_api establishes both, in the same expression that derives the
// already-modelled zero_acc: sub_mul and sub_acc come from Operation::Neg on
// the source operands and on the accumulator, where zero_acc comes from
// Operation::Zero on the accumulator. So sub0 negates the product and sub1
// negates the accumulator input, each on its own term.
//
// The elementwise and matrix arms are separate runs -- there are only five
// accumulators -- because the product a negation applies to is formed
// differently in each, one lane-wise multiply against a dot product over K.
// Conf words are the shape plus 2048 for sub0 and 4096 for sub1: elem_64
// signed is 808 (amode 0, bmode 1, variant 1, both signs), 8x8_8x8 signed is
// 776 (variant 0).
//
// shift16 (bit 10), sub2 (bit 13) and sub_mask (bit 16) are NOT modelled and
// fault, checked below: nothing in this tree says what shift16 shifts, sub2
// belongs to the second-accumulator wrappers this lambda does not model, and
// sub_mask has no caller at all.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs | FileCheck %s

// RUN: llvm-mc -triple aie2p -filetype=obj --defsym MATRIX=1 %s -o %t.mat.o
// RUN: llvm-aie-run %t.mat.o --print-regs | FileCheck %s --check-prefix=MATRIX

// RUN: llvm-mc -triple aie2p -filetype=obj --defsym SHIFT16=1 %s -o %t.s16.o
// RUN: not llvm-aie-run %t.s16.o 2>&1 | FileCheck %s --check-prefix=UNMODELLED

// RUN: llvm-mc -triple aie2p -filetype=obj --defsym SUB2=1 %s -o %t.sub2.o
// RUN: not llvm-aie-run %t.sub2.o 2>&1 | FileCheck %s --check-prefix=UNMODELLED

// RUN: llvm-mc -triple aie2p -filetype=obj --defsym SUBMASK=1 %s -o %t.smask.o
// RUN: not llvm-aie-run %t.smask.o 2>&1 | FileCheck %s --check-prefix=UNMODELLED

	.text
	.globl _start
_start:
	// Bytes 02 00 FF 03: signed lanes 2, 0, -1, 3, squaring to 4, 0, 1, 9.
	// As the matrix arm's B it is column 2, 0, -1, 3 by j mod 4.
	movxm	r0, #0x03FF0002
	vbcst.32	x0, r0
	// Bytes 01 02 03 04, squaring to 1, 4, 9, 16 -- every lane distinct from
	// the accumulator's, so a dropped sub1 cannot pass as a correct sum. As
	// the matrix arm's A every row sums to 20.
	movxm	r1, #0x04030201
	vbcst.32	x1, r1
	// Bytes 01 00 03 00: column 1, 0, 3, 0 by j mod 4, giving 20 and 60.
	movxm	r2, #0x00030001
	vbcst.32	x2, r2
	movxm	r4, #808
	movxm	r5, #4904
	movxm	r6, #2856
	movxm	r7, #6952
	movxm	r9, #776
	movxm	r10, #2824
	movxm	r11, #4872
.ifdef SHIFT16
	movxm	r8, #1832
.endif
.ifdef SUB2
	movxm	r8, #9000
.endif
.ifdef SUBMASK
	movxm	r8, #66344
.endif
	nop
	nop
	nop
	nop
	nop
	nop
	nop
.ifdef SHIFT16
	vmul	dm4, x0, x0, r8
	done
.endif
.ifdef SUB2
	vmul	dm4, x0, x0, r8
	done
.endif
.ifdef SUBMASK
	vmul	dm4, x0, x0, r8
	done
.endif
.ifdef MATRIX
	vmul	dm0, x1, x2, r9
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	// sub0: the whole dot product is negated.
	vmul	dm1, x1, x2, r10
	// sub1, against x0 as B so the two terms do not cancel to zero.
	vmac	dm2, dm0, x1, x0, r11
	done
.endif
.ifndef MATRIX
.ifndef SHIFT16
.ifndef SUB2
.ifndef SUBMASK
	vmul	dm0, x0, x0, r4
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	// sub1 alone: the accumulator is negated, the product is not.
	vmac	dm1, dm0, x1, x1, r5
	// sub0 alone: the product is negated.
	vmul	dm2, x0, x0, r6
	// Both: each term negated, neither cancelling the other.
	vmac	dm3, dm0, x1, x1, r7
	done
.endif
.endif
.endif
.endif

// The unmodified elementwise product, as the accumulator the negations act on:
// 9, 1, 0, 4 high to low.
// CHECK-DAG: bmll0 = 0x{{9000000010000000000000004(00000009000000010000000000000004){3}$}}

// sub1: -(4, 0, 1, 9) + (1, 4, 9, 16) = -3, 4, 8, 7 low to high.
// CHECK-DAG: bmll1 = 0x{{70000000800000004FFFFFFFD(000000070000000800000004FFFFFFFD){3}$}}

// sub0: -(4, 0, 1, 9), so the lane that was 0 stays 0 and the rest go negative.
// CHECK-DAG: bmll2 = 0x{{FFFFFFF7FFFFFFFF00000000FFFFFFFC(FFFFFFF7FFFFFFFF00000000FFFFFFFC){3}$}}

// sub0 and sub1 together: -(4, 0, 1, 9) - (1, 4, 9, 16) = -5, -4, -10, -25.
// CHECK-DAG: bmll3 = 0x{{FFFFFFE7FFFFFFF6FFFFFFFCFFFFFFFB(FFFFFFE7FFFFFFF6FFFFFFFCFFFFFFFB){3}$}}

// The unmodified matrix product: 20, 0, 60, 0 across each row of eight.
// MATRIX-DAG: bmll0 = 0x{{3C0000000000000014(000000000000003C0000000000000014){3}$}}

// sub0 on the matrix arm: -20 and -60, the zero columns unchanged.
// MATRIX-DAG: bmll1 = 0x{{FFFFFFC400000000FFFFFFEC(00000000FFFFFFC400000000FFFFFFEC){3}$}}

// sub1 on the matrix arm: -(20, 0, 60, 0) + (40, 0, -20, 60) = 20, 0, -80, 60.
// MATRIX-DAG: bmll2 = 0x{{3CFFFFFFB00000000000000014(0000003CFFFFFFB00000000000000014){3}$}}

// A bit whose meaning this tree does not establish names itself rather than
// being silently dropped.
// UNMODELLED: conf carries shift16/sub2/sub_mask bits
