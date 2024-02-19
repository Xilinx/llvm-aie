// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates

// RUN: not %clang -O2 %s --target=aie2 -S -emit-llvm -o - |& FileCheck %s

// CHECK: error: index out of range [0,3]
v32uint4 test_extract_v32uint4(v128uint4 a, int idx) {
    return extract_v32uint4(a, 4);
}
