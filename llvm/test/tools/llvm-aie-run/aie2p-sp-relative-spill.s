//===- aie2p-sp-relative-spill.s -----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// ST_dms_sts_spill/LDA_dms_lda_spill -- what the register allocator's
// ST_R_SPILL/LDA_R_SPILL pseudos actually expand to
// (AIE2PInstrInfo::getSpillPseudoExpandInfoByOpcode) -- encode sp as a fixed
// base, not a $ptr operand the way the general idx_imm forms do (sp is
// AIE2PSPLReg<5,"sp">, a separate special-register class from the eP
// pointer class those use). The offset field (c12n_step4) only encodes
// [-2048,-4], confirmed by llvm-mc rejecting a positive one: spill slots
// live below sp, never above.

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --scratch=0x10000:64 \
// RUN:   --dump-mem=0x1003c:4 | FileCheck %s

	.text
	.globl _start
_start:
	movxm	sp, #0x10040
	mov	r0, #100
	st	r0, [sp, #-4]
	lda	r1, [sp, #-4]
	done

// CHECK-DAG: r0 = 0x64
// CHECK-DAG: r1 = 0x64

// The store landed at sp-4, not wherever a wrong base would have put it.
// CHECK: mem[0x1003c] = 64 00 00 00
