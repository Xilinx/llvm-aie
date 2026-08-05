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

// Malformed dependent generic loop hint values are value-dependent, so their
// checks are skipped at parse time and run at instantiation instead.

// RUN: not %clang_cc1 -triple aie2ps -std=c++20 -fsyntax-only %s 2>&1 | FileCheck %s

template <int N> void bad_type(int *p) {
#pragma clang loop hint(swp_ii, ((N == 0) ? 1.5 : 0.5))
  for (int i = 0; i != 4; ++i)
    p[i] += i;
}
// CHECK: error: invalid argument of type 'double'; expected an integer type
// CHECK: note: in instantiation of function template specialization 'bad_type<0>' requested here
template void bad_type<0>(int *);

template <int N> void negative(int *p) {
#pragma clang loop hint(swp_ii, ((N) - 5))
  for (int i = 0; i != 4; ++i)
    p[i] += i;
}
// CHECK: error: invalid value '-5'; must be positive
// CHECK: note: in instantiation of function template specialization 'negative<0>' requested here
template void negative<0>(int *);

struct S {};
template <typename T> void subst_fail(int *p) {
#pragma clang loop hint(swp_ii, (T::nonexistent))
  for (int i = 0; i != 4; ++i)
    p[i] += i;
}
// CHECK: error: no member named 'nonexistent' in 'S'
// CHECK: note: in instantiation of function template specialization 'subst_fail<S>' requested here
template void subst_fail<S>(int *);

// A valid sibling instantiation is unaffected; only the bad one is diagnosed.
template <int N> void split(int *p) {
#pragma clang loop hint(swp_ii, ((N) - 3))
  for (int i = 0; i != 4; ++i)
    p[i] += i;
}
template void split<5>(int *); // ok: value 2
// CHECK: error: invalid value '-2'; must be positive
// CHECK: note: in instantiation of function template specialization 'split<1>' requested here
template void split<1>(int *);
