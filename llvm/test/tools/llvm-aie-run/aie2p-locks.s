//===- aie2p-locks.s ----------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// acq and rel reach the embedder's lock ports. This embedder has none, so both
// name the lock they wanted rather than passing silently -- the same contract
// the unmodelled-instruction path holds. A core that acquires a lock nobody
// provides must not appear to have run.
//
// The id comes from the operand either way: #3 as an immediate, r5 as a
// register, and the message repeats it under the opcode's own name. Running an
// array with real locks is the fabric suite's job; what is checkable here is
// that the operands decode and reach the port.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: not llvm-aie-run %t.o 2>&1 | FileCheck %s

// RUN: llvm-mc -triple aie2p -filetype=obj --defsym REG=1 %s -o %t.reg.o
// RUN: not llvm-aie-run %t.reg.o 2>&1 | FileCheck %s --check-prefix=REG

// RUN: llvm-mc -triple aie2p -filetype=obj --defsym RELEASE=1 %s -o %t.rel.o
// RUN: not llvm-aie-run %t.rel.o 2>&1 | FileCheck %s --check-prefix=RELEASE

	.text
	.globl _start
_start:	mov	r0, #-2
	mov	r5, #9
	nop
	nop
	nop
	nop
	nop
	nop
	nop
.ifdef REG
	acq	r5, r0
.endif
.ifdef RELEASE
	rel	#3, r0
.endif
.ifndef REG
.ifndef RELEASE
	acq	#3, r0
.endif
.endif
	done

// CHECK: fault at
// CHECK-SAME: ACQ_mLockId_imm: lock 3 is not provided by this embedder

// REG: fault at
// REG-SAME: ACQ_mLockId_reg: lock 9 is not provided by this embedder

// RELEASE: fault at
// RELEASE-SAME: REL_mLockId_imm: lock 3 is not provided by this embedder
