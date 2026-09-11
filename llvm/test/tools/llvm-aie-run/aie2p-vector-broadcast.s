//===- aie2p-vector-broadcast.s -----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs | FileCheck %s

	.text
	.globl _start
_start:
	movxm	r0, #0x1234ABCD
	vbcst.16	x0, r0
	vbcst.8	x1, r0
	vbcst.32	x2, r0
	movxm	r4, #0xAAAAAAAA
	movxm	r5, #0xBBBBBBBB
	vbcst.64	x3, r5:r4
	movxm	r6, #0x0000007F
	vbcst.16	x5, r6
	done

// A vector register has no storage of its own -- x0 is wl0 and wh0 -- so a
// result appears here only if the 512-bit write was split across both halves.
// Checking the halves rather than x0 is deliberate: the split is what is
// under test, and --print-regs reports registers that hold their own bits.

// Only the low half-word: 1234 above it is dropped.
// CHECK-DAG: wl0 = 0x{{ABCD(ABCD){15}$}}
// CHECK-DAG: wh0 = 0x{{ABCD(ABCD){15}$}}

// CHECK-DAG: wl1 = 0x{{CD(CD){31}$}}
// CHECK-DAG: wh1 = 0x{{CD(CD){31}$}}

// CHECK-DAG: wl2 = 0x{{1234ABCD(1234ABCD){7}$}}
// CHECK-DAG: wh2 = 0x{{1234ABCD(1234ABCD){7}$}}

// vbcst.64 is the only one of the four that READS through composition: its
// source is an eL pair, itself made of two r registers. Two different words
// make the 64-bit lane asymmetric, so this is also the one check here that
// would notice sub_l_even and sub_l_odd transposed -- verified by doing it,
// which yields AAAAAAAABBBBBBBB instead. l2 is r5:r4, odd half high.
// CHECK-DAG: wl3 = 0x{{BBBBBBBBAAAAAAAA(BBBBBBBBAAAAAAAA){3}$}}
// CHECK-DAG: wh3 = 0x{{BBBBBBBBAAAAAAAA(BBBBBBBBAAAAAAAA){3}$}}

// A pattern with a zero high byte, to catch a lane assembled from the wrong
// end. Leading zeros are not printed, so this one is 62 nibbles.
// CHECK-DAG: wl5 = 0x{{7F(007F){15}$}}
// CHECK-DAG: wh5 = 0x{{7F(007F){15}$}}

// What none of these can catch: a broadcast repeats every 64 bits at most, so
// the two 256-bit halves always hold the same value and swapping them changes
// nothing. sub_256_hi against sub_256_lo rests on AIE2PSubRegRanges.inc being
// generated, not on this test.
