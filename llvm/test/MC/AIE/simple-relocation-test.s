//===- simple-relocation-test.s ---------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: llvm-mc -triple aie2 %s -filetype=obj -o %t
// RUN: llvm-objdump --triple aie2 -dr --no-print-imm-hex %t | FileCheck %s

// CHECK: <.entry_point>:
// CHECK: movxm sp, #32
// CHECK: j #0
// CHECK: R_AIE_1 entry_point
// CHECK: nopx

    .global entry_point
.entry_point:
    movxm sp, #32
    j #entry_point
