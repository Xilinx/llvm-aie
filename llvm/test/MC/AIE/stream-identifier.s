//===- stream-identifier.s --------------------------------------*- C++ -*-===//
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

// CHECK: vmov MCD, x0
// CHECK: vmov.hi cm0, SCD
// CHECK: vmov.lo cm1, SCD
// CHECK: vmov x0, SCD

vmov MCD, x0
vmov.hi cm0, SCD
vmov.lo cm1, SCD
vmov x0, SCD
