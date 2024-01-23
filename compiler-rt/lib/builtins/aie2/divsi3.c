//===-- divsi3.c - Implement __divsi3 -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements __divsi3 for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#include "../int_lib.h"
#include "aie2_div.h"

// Returns: a / b
COMPILER_RT_ABI si_int __divsi3(si_int a, si_int b) {
  int q, r;
  sdivmod(a, b, &q, &r);
  return q;
}
