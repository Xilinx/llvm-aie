//===- aie2p-call-return.s -----------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// jl writes lr with the return address once ITS OWN five delay slots retire,
// not the five belonging to the ret it calls into. Byte offsets (.text sits
// at zero in an object this test never links, -show-encoding confirms sizes:
// mov/done/ret 4 bytes, jl #imm 6 (JL_lng, AIE2P_inst_lng_instr48 -- wider
// than the alu forms, which is exactly the "variable-size bundle" case),
// nop 2): jl at 4 (6 bytes, ends at 10), five 2-byte delay-slot nops at
// 10/12/14/16/18 (ending at 20), so lr must land on 20 -- computed from the
// real encoded sizes, not a fixed per-slot assumption.

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs | FileCheck %s

	.text
	.globl _start
_start:
	mov	r0, #0
	jl	#28
	nop
	nop
	nop
	nop
	nop
// Return point, byte 20: reached only via func's ret (jl is unconditional,
// there is no fallthrough case). r1 proves ret actually landed here rather
// than somewhere jl's own delay slots happened to end up.
	mov	r1, #99
	done
func:
	mov	r0, #42
	ret	lr
	nop
	nop
	nop
	nop
	nop

// mov+jl(2) + 5 delay + func's mov+ret(2) + 5 delay + mov r1(1); done itself
// never reaches advancePC, so it is not counted (matches aie2p-delay-slots.s).
// CHECK: bundles: 15

// CHECK-DAG: r0 = 0x2A
// CHECK-DAG: r1 = 0x63
// CHECK-DAG: lr = 0x14
