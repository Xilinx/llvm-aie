//===- bundles.s ------------------------------------------------*- C++ -*-===//
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

// CHECK: padda   [p1], #4; add     r0, r0, r1
// CHECK: padda   [p0], #4; mov     r0, r1
// CHECK: padda   [p0], #4; st      r0, [p1, #0]
// CHECK: padda   [p1], #4; add     r0, r0, r1;     mov     r2, r1
// CHECK: padda   [p1], #4; movxm   r2, #64
// CHECK: padda   [p1], #4; st      r0, [p0, #0];        add     r0, r0, r1;     mov     r2, r1

padda   [p1], #4;   add     r0, r0, r1
padda   [p0], #4;   mov     r0, r1
padda   [p0], #4;   st      r0, [p1, #0]
padda   [p1], #4;   add     r0, r0, r1;     mov     r2, r1
padda   [p1], #4;   movxm   r2, #64
padda   [p1], #4;   st      r0, [p0, #0];        add     r0, r0, r1;     mov     r2, r1
