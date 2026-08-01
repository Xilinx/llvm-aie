//===- aie2p-fault-contract.s -------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// An instruction with no semantics stops the run by name, and so does a read of
// a register some earlier instruction defined without the model computing it.
// Neither may quietly produce a number.

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: not llvm-aie-run %t.o --coverage 2>&1 | FileCheck %s

// RUN: llvm-mc -triple aie2p -filetype=obj --defsym CARRY=1 %s -o %t.carry.o
// RUN: not llvm-aie-run %t.carry.o 2>&1 | FileCheck %s --check-prefix=CARRY

// RUN: llvm-mc -triple aie2p -filetype=obj --defsym NOCARRY=1 %s -o %t.ok.o
// RUN: llvm-aie-run %t.ok.o --print-regs | FileCheck %s --check-prefix=OK

	.text
	.globl _start
_start:
	mov	r0, #1
	mov	r1, #2
.ifdef CARRY
	add	r2, r0, r1
	mov	r3, srcarry
.endif
.ifdef NOCARRY
	mov	r3, srcarry
.endif
.ifndef CARRY
.ifndef NOCARRY
	adc	r2, r0, r1
.endif
.endif
	done

// CHECK: error: fault at 0x8: ADC: no semantics
// CHECK: unmodelled ADC
// CHECK: modelled   MOV_alu_mv_mv_mv_cg

// add defines srCarry, which this model does not compute.
// CARRY: error: fault at 0xc: MOV_alu_mv_mv_mv_scl: srCarry holds no value

// Untouched, srCarry reads as its reset value.
// OK: bundles: 3
