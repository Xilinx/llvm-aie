//===- aie2p-vector-store-128.s -----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --scratch=0x10000:256 \
// RUN:   --dump-mem=0x10040:80 | FileCheck %s

// vst.128 names a 256-bit W and writes half of it. Memory is the only oracle
// that can say WHICH half, so this gates on --dump-mem rather than on regs.

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10020
	movxm	p2, #0x10040
	movxm	r0, #0xAAAAAAAA
	movxm	r1, #0xBBBBBBBB
	movxm	r2, #0xCCCCCCCC
	movxm	r3, #0xDDDDDDDD
	// One marker word per 16-byte quarter of x0, so each half of each W is
	// distinguishable from the other three. Two base pointers because the
	// scalar store's own offset does not reach 32.
	st	r0, [p0, #0]
	st	r1, [p0, #16]
	st	r2, [p1, #0]
	st	r3, [p1, #16]
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
	vst.128	wl0, [p2, #16]
	vst.128	wh0, [p2, #48]
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	done

// wl0 holds aa..(16 bytes)..bb.., wh0 holds cc..(16)..dd.. -- so the marker
// that lands is the one that says which half went out.
//
// 0x10040: untouched, the store went to [p2, #16] and not to p2 itself, which
// is what pins the immediate as a byte offset. The encoded field is 4 bits of
// c8s_step16, so a model that missed the step would land here.
// CHECK: mem[0x10040] = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 0x10050: the LOW half of wl0. bb would be the high half.
// CHECK-SAME: aa aa aa aa 00 00 00 00 00 00 00 00 00 00 00 00
// 0x10060: still zero, so the store wrote 16 bytes and not the 32 its source
// register holds -- the width came from the opcode, not the register class.
// CHECK-SAME: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// 0x10070: the low half of wh0, so the source operand is read where it is
// named and the second store is not a copy of the first.
// CHECK-SAME: cc cc cc cc 00 00 00 00 00 00 00 00 00 00 00 00
// 0x10080: past both stores.
// CHECK-SAME: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
