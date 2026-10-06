//===- bfp16.s --------------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: llvm-mc -triple aie2ps %s -o - | FileCheck %s
// RUN: llvm-mc -triple aie2ps %s -filetype=obj -o %t
// RUN: llvm-objdump --triple=aie2ps -dr --no-print-imm-hex %t | FileCheck %s

// CHECK: vmul.f dm0, ex0, ey0, ey1, r8
// CHECK: vnegmul.f dm0, ex11, ey2, ey3, r9
// CHECK: vmac.f dm5, dm5, ex11, ey0, ey1, r13
// CHECK: vmsc.f dm5, dm5, ex1, ey2, ey3, r13
// CHECK: vaddmac.f dm5, dm5, dm0, ex5, ey2, ey2, r14
// CHECK: vaddmsc.f dm2, dm2, dm4, ex11, ey0, ey0, r8

	vmul.f		dm0, ex0, ey0, ey1, r8
	vnegmul.f	dm0, ex11, ey2, ey3, r9
	vmac.f		dm5, dm5, ex11, ey0, ey1, r13
	vmsc.f		dm5, dm5, ex1, ey2, ey3, r13
	vaddmac.f	dm5, dm5, dm0, ex5, ey2, ey2, r14
	vaddmsc.f	dm2, dm2, dm4, ex11, ey0, ey0, r8
