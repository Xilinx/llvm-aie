//===- nops.s ---------------------------------------------------*- C++ -*-===//
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
// RUN: llvm-objdump --triple=aie2 -dr %t --no-print-imm-hex | FileCheck %s

// CHECK: nop
// CHECK: nopa
// CHECK: nopb
// CHECK: nops
// CHECK: nopm
// CHECK: nopv
// CHECK: nopx
// CHECK: nopxm
// CHECK: nopa ; nopb
// CHECK: nopa ; nopb ; nopx
// CHECK: nopa ; nopxm
// CHECK: nopx ; nopm
// CHECK: nopa ; nopb ; nops
// CHECK: nopb ; nopa ; nops ; nopx ; nopm ; nopv
// CHECK: nopb ; nopa ; nops ; nopxm ; nopv

nop
nopa
nopb
nops
nopm
nopv
nopx
nopxm
nopa; nopb
nopa; nopb; nopx
nopa; nopxm
nopm; nopx
nopa; nopb; nops
nopa; nopb; nopx; nops; nopm; nopv
nopa; nopb; nops; nopxm; nopv

// Incomplete bundle formats:

// CHECK: nopb ; nopa ; nops ; nopx ; nopm ; nopv
// CHECK: nopb ; nopa ; nops ; nopx ; nopm ; nopv

nopa; nops; nopx; nopm; nopv
nopa; nopb; nops; nopm; nopv
