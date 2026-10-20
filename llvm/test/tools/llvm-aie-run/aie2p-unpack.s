//===- aie2p-unpack.s ---------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vunpack -- widen every lane of a packed vector to twice its width.
//
// Two things vary and neither is in the mnemonic. crUnpackSize picks the
// SOURCE lane width, 1 for 8-bit and 0 for 4-bit, and the opcode suffix picks
// the extension, unpacksign1 signed and unpacksign0 unsigned. All four
// combinations run here over ONE source word, 0x81F27334, chosen so that each
// combination lands on a value none of the other three carry:
//
//   8-bit lanes 34 73 F2 81   signed -> 0034 0073 FFF2 FF81
//                           unsigned -> 0034 0073 00F2 0081
//   4-bit lanes 4 3 3 7 2 F 1 8
//                             signed -> 04 03 03 07 02 FF 01 F8
//                           unsigned -> 04 03 03 07 02 0F 01 08
//
// So a body that reads the size bit the wrong way round, or sign-extends
// where it should zero-extend, cannot land on the expected value of the case
// it is running -- the low lanes 34/73 and 4/3/3/7 agree across all four,
// which is what forces the check onto the lanes that actually discriminate.
//
// The 512-bit destinations print as their wl/wh halves, and llvm-aie-run
// prints an APInt with leading zeros dropped, so the unsigned checks match a
// first unit that is short by its leading zeros. That is the print, not the
// value. crUnpackSize itself is not checked for the same reason: the printer
// skips a register that is entirely zero, so its final state has no line. The
// 4-bit cases landing away from the 8-bit ones is what proves it was read.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs | FileCheck %s

	.text
	.globl _start
_start:
	movxm	r0, #0x81F27334
	vbcst.32	x0, r0
	movxm	r2, #1
	mov	crunpacksize, r2
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// 8-bit source lanes, both extensions, 256 bits in and 512 out.
	vunpack	x2, wl0, unpacksign1
	vunpack	x3, wl0, unpacksign0

	// The wide form, 512 bits in and 1024 out, over the same word. y2 is
	// x4:x5, so its four printed halves must each repeat the same unit the
	// narrow signed case produced -- the two forms differ in width only.
	vunpack	y2, x0, unpacksign1

	movxm	r3, #0
	mov	crunpacksize, r3
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	// 4-bit source lanes, both extensions.
	vunpack	x6, wl0, unpacksign1
	vunpack	x7, wl0, unpacksign0
	done

// 8-bit lanes, signed: FFF2 and FF81 are the discriminating lanes.
// CHECK-DAG: wl2 = 0x{{(FF81FFF200730034){4}$}}
// CHECK-DAG: wh2 = 0x{{(FF81FFF200730034){4}$}}

// 8-bit lanes, unsigned: the same low lanes, 00F2 and 0081 above them.
// CHECK-DAG: wl3 = 0x{{8100F200730034(008100F200730034){3}$}}
// CHECK-DAG: wh3 = 0x{{8100F200730034(008100F200730034){3}$}}

// The wide form agrees with the narrow signed case across all four halves.
// CHECK-DAG: wl4 = 0x{{(FF81FFF200730034){4}$}}
// CHECK-DAG: wh4 = 0x{{(FF81FFF200730034){4}$}}
// CHECK-DAG: wl5 = 0x{{(FF81FFF200730034){4}$}}
// CHECK-DAG: wh5 = 0x{{(FF81FFF200730034){4}$}}

// 4-bit lanes, signed: FF and F8 where the unsigned case has 0F and 08.
// CHECK-DAG: wl6 = 0x{{(F801FF0207030304){4}$}}
// CHECK-DAG: wh6 = 0x{{(F801FF0207030304){4}$}}

// 4-bit lanes, unsigned.
// CHECK-DAG: wl7 = 0x{{8010F0207030304(08010F0207030304){3}$}}
// CHECK-DAG: wh7 = 0x{{8010F0207030304(08010F0207030304){3}$}}
