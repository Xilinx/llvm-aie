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
	movxm	r1, #0x0000007F
	vbcst.16	x3, r1
	done

// x0 has no storage of its own -- it is wl0 and wh0 -- so a result only
// appears here if the 512-bit write was split across both halves. Checking
// the halves rather than x0 is deliberate: it is the split that is under
// test, and --print-regs reports registers that hold their own bits.
//
// What this cannot catch: a broadcast makes every lane equal, so the halves
// hold identical bits and swapping them changes nothing here. Which half is
// which rests on AIE2PSubRegRanges.inc being generated, not on this test.
// Every lane is ABCD and none is 1234ABCD, which is the same check as "only
// the low half-word is broadcast" -- a whole-word splat would not fit 32
// lanes and would read back with 1234 in it.
// CHECK-DAG: wl0 = 0x{{ABCD(ABCD){15}$}}
// CHECK-DAG: wh0 = 0x{{ABCD(ABCD){15}$}}

// A pattern with a zero high byte, to catch a lane assembled from the wrong
// end. Leading zeros are not printed, so this one is 62 nibbles.
// CHECK-DAG: wl3 = 0x{{7F(007F){15}$}}
// CHECK-DAG: wh3 = 0x{{7F(007F){15}$}}
