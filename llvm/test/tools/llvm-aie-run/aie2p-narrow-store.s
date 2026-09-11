//===- aie2p-narrow-store.s ----------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// A store narrower than its source register. The value has to be narrowed
// before the APInt is built: constructing an 8-bit APInt from a 32-bit
// register value asserts, so this aborts rather than storing anything when the
// masking is missing, and the surrounding words prove the store stayed inside
// its own byte.

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --scratch=0x10000:32 --dump-mem=0x10000:12 \
// RUN:   | FileCheck %s

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	r0, #0xAAAAAAAA
	movxm	r1, #0xFFFFFF85
	movxm	r2, #0x1234BEEF
	st	r0, [p0, #0]
	nop
	st	r0, [p0, #8]
	nop
	st.s8	r1, [p0, #4]
	nop
	st.s16	r2, [p0, #6]
	nop
	done

// Byte 4 took the low byte of r1 and bytes 6-7 the low halfword of r2; the
// neighbouring words are untouched.
// CHECK: mem[0x10000] = aa aa aa aa 85 00 ef be aa aa aa aa
