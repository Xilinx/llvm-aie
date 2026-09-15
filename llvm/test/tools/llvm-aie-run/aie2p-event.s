//===- aie2p-event.s ----------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// event reaches the embedder's trace port. Unlike acq/rel it defaults to a
// no-op rather than a fault: the intrinsic is IntrHasSideEffects + IntrNoMem,
// so nothing in the datapath can read the event back and a core that raises
// one into no sink still computes the same result.
//
// aie2p splits over four opcodes what AIE2 spelled as one opcode over a 2-bit
// operand, so the event number is the opcode. Which number belongs to which
// opcode is fixed four ways over: the ISel patterns select them from
// (i32 0)..(i32 3), the tablegen encodings differ only in the top two bits of
// alu, llc lowers the intrinsic to this order, and the assembled words below
// differ only in bits [26:25]. Nothing here rests on the mnemonics.
//
// Only the first two words are raw: "#0" and "#1" are part of the AsmString
// rather than an operand, and the AIE2P parser has no special case for them
// the way AIE2PSAsmParser does. The .warning and .error forms carry no such
// operand and assemble from their mnemonics, which also pins the two raw
// words -- all four encodings agree outside bits [26:25].
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o | FileCheck %s
// RUN: llvm-aie-run %t.o --coverage | FileCheck %s --check-prefix=COV

	.text
	.globl _start
_start:
	.long	0x10100018	// event #0
	.long	0x12100018	// event #1
	event.warning
	event.error
	done

// The port is reached once per instruction, in program order, and the number
// carried is the opcode's own.
// CHECK: event 0
// CHECK-NEXT: event 1
// CHECK-NEXT: event 2
// CHECK-NEXT: event 3

// Naming the opcodes separately, so an id that is right for the wrong reason
// -- four decodes of one opcode, say -- cannot pass.
// COV-DAG: EVENT_event0
// COV-DAG: EVENT_event1
// COV-DAG: EVENT_WARNING
// COV-DAG: EVENT_ERROR
