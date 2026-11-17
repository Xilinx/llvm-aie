// --- aie-target-data.c ----------------------------------------------------///
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
// --------------------------------------------------------------------------///

// Clang takes the AIE data layout from Triple::computeDataLayout, which the
// backend and opt also use.

// RUN: %clang_cc1 -triple aie -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -triple aie2 -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -triple aie2p -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -triple aie2ps -emit-llvm -o - %s | FileCheck %s

// CHECK: target datalayout = "e-m:e-p:20:32-i1:8:32-i8:8:32-i16:16:32-i32:32:32-f32:32:32-i64:32-f64:32-a:0:32-n32"
