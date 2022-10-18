//===- simple-scalar-instr.s ------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: llvm-mc -triple aie2 %s -o - | FileCheck %s
// RUN: llvm-mc -triple aie2 %s -o %t -filetype=obj
// RUN: llvm-objdump --triple=aie2 -dr --no-print-imm-hex %t | FileCheck %s

// CHECK: add r0, r0, r1
// CHECK: add r1, r2, #3
// CHECK: nop
// CHECK: lshl r0, r0, r1
// CHECK: mova r6, #21
// CHECK: mov p6, sp
// CHECK: movxm r0, #1000000
// CHECK: and r0, r1, r2
// CHECK: mul r0, r0, r2
// CHECK: extend.s8 r0, r0
// CHECK: ltu r1, r2, r1


    add     r0, r0, r1
    add     r1, r2, #3
    nop
    lshl    r0, r0, r1
    mova    r6, #21
    mov     p6, sp
    movxm   r0, #1000000
    and     r0, r1, r2
    mul     r0, r0, r2
    extend.s8   r0, r0
    ltu     r1, r2, r1
