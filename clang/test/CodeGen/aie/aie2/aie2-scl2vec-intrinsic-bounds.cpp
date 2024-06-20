// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates

// RUN: not %clang -O2 %s --target=aie2 -S -emit-llvm -o - |& FileCheck %s

// CHECK: error: index out of range [0,63]
char test_ext_elem_bounds(v64int8 v, int idx, int sign) {
  return ext_elem(v, 64, sign);
}
