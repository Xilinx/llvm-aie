//===- bundle-slot-symbol-reloc.s --------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// A symbol referenced from a bundle slot must be named by its relocation; an
// index of 0 links the call to address 0 and hangs with no diagnostic.

// RUN: llvm-mc -triple aie2 -filetype=obj %s -o %t2.o
// RUN: llvm-readobj -r --symbols %t2.o | FileCheck %s
// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t2p.o
// RUN: llvm-readobj -r --symbols %t2p.o | FileCheck %s
// RUN: llvm-mc -triple aie2ps -filetype=obj %s -o %t2ps.o
// RUN: llvm-readobj -r --symbols %t2ps.o | FileCheck %s

	.text
	.globl _start
_start:
	jl	#undeclared_callee
	nop

// CHECK: Relocations [
// CHECK: R_AIE_{{[0-9]+}} undeclared_callee

// CHECK: Symbols [
// CHECK: Name: undeclared_callee
// CHECK: Section: Undefined
