//===-- udivsi3.c - Implement __udivsi3 -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements __udivsi3 for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#include "../int_lib.h"
#include "aie2_div.h"

// Returns: a / b
COMPILER_RT_ABI su_int __udivsi3(su_int a, su_int b) {
  unsigned int q, r;
  udivmod(a, b, &q, &r);
  return q;
}
