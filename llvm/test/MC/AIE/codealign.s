//===- codealign.s ----------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: llvm-mc -triple=aie2 %s --filetype=obj -o %t
// RUN: llvm-objdump --triple=aie2 -d %t | FileCheck %s

// Check that big alignment paddings use 16 byte bundles

// CHECK-LABEL: bigalign
// CHECK 0: 99 34 82 10 and r1, r2, r3
// CHECK: 4: c0 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 nopb ; nopa ; nops ; nopxm ; nopv
// CHECK: 14: c0 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 nopb ; nopa ; nops ; nopxm ; nopv
// CHECK: 24: c0 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 nopb ; nopa ; nops ; nopxm ; nopv
// CHECK: 34: 37 88 03 00 00 00 00 00 00 00 00 00 nops ; nopb ; nopx ; nopm
// CHECK: 40: 99 35 82 10 or r1, r2, r3

bigalign:
  and r1, r2, r3
  .align 64
  or r1, r2, r3
