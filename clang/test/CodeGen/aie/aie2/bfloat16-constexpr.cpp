//===- bfloat16-constexpr.cpp -----------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: %clang --target=aie2 -S -emit-llvm %s -O2 -o - | FileCheck %s

constexpr bfloat16 Cst = -0.3761263890318252f;

// CHECK-LABEL: @_Z19test_bf16_constexprv
// CHECK: bfloat 0xRBEC1
bfloat16 test_bf16_constexpr() {
  return Cst;
}
