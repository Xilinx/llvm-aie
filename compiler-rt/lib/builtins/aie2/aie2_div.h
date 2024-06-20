//===-- aie2_div.h - Implement div/mod ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements signed/unsigned div/mod for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

static void udivmod(unsigned a, unsigned b, unsigned *q, unsigned *r) {
  unsigned int rem = 0;
  for (int i = 0; i < 32; i++)
    __builtin_aiev2_divstep(a, rem, b);
  *q = a;
  *r = rem;
}

static void sdivmod(int a, int b, int *q, int *r) {
  unsigned ua = a < 0 ? -a : a;
  unsigned ub = b < 0 ? -b : b;
  unsigned uq, ur;
  udivmod(ua, ub, &uq, &ur);
  *q = (a ^ b) < 0 ? -uq : uq;
  *r = a < 0 ? -ur : ur;
}
