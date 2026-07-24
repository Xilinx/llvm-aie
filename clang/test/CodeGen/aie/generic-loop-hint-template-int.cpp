//===-- generic-loop-hint-template-int.cpp -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its
// affiliates
//
//===----------------------------------------------------------------------===//

// Reproduce the crash: dependent integer-valued generic loop hints (incl.
// nested templates) stay dependent without the fix and the evaluator asserts.

// RUN: not --crash %clang_cc1 -triple aie2ps -std=c++20 -emit-llvm -o - %s 2>&1 | FileCheck %s --check-prefix=CRASH

template <int N> void scaled(int *p) {
#pragma clang loop hint(swp_ii, ((N) * 2))
  for (int i = 0; i != 4; ++i)
    p[i] += i;
}

template <int N> struct Outer {
  template <int M> void inner(int *p) {
#pragma clang loop hint(swp_ii, ((N + M)))
    for (int i = 0; i != 4; ++i)
      p[i] += i;
  }
};

template void scaled<4>(int *);
template void Outer<3>::inner<6>(int *);

// CRASH: Expression evaluator can't be called on a dependent expression.
