//===-- generic-loop-hint-template-errors.cpp ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its
// affiliates
//
//===----------------------------------------------------------------------===//

// Reproduce the crash: malformed dependent generic loop hint values stay
// dependent without the fix and the evaluator asserts instead of diagnosing.

// RUN: not --crash %clang_cc1 -triple aie2ps -std=c++20 -emit-llvm -o - %s 2>&1 | FileCheck %s --check-prefix=CRASH

template <int N> void bad_type(int *p) {
#pragma clang loop hint(swp_ii, ((N == 0) ? 1.5 : 0.5))
  for (int i = 0; i != 4; ++i)
    p[i] += i;
}
template void bad_type<0>(int *);

template <int N> void negative(int *p) {
#pragma clang loop hint(swp_ii, ((N) - 5))
  for (int i = 0; i != 4; ++i)
    p[i] += i;
}
template void negative<0>(int *);

struct S {};
template <typename T> void subst_fail(int *p) {
#pragma clang loop hint(swp_ii, (T::nonexistent))
  for (int i = 0; i != 4; ++i)
    p[i] += i;
}
template void subst_fail<S>(int *);

template <int N> void split(int *p) {
#pragma clang loop hint(swp_ii, ((N) - 3))
  for (int i = 0; i != 4; ++i)
    p[i] += i;
}
template void split<5>(int *);
template void split<1>(int *);

// CRASH: Expression evaluator can't be called on a dependent expression.
