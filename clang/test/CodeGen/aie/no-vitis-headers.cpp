//===- no-vitis-headers.cpp -------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// RUN: %clang -O1 %s --target=aie -mno-vitis-headers -S -emit-llvm -o %t

// non-standard defines should be defined.
#ifdef chess_copy
#error("present define")
#endif

int f() {
  return 1;
}