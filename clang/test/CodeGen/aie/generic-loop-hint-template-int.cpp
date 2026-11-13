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

// Dependent integer-valued generic loop hints are instantiated and preserved
// verbatim (no unroll 0/1 rewrite), including across nested templates.

// RUN: %clang_cc1 -triple aie2ps -std=c++20 -emit-llvm -o - %s | FileCheck %s

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

// A value naming a constexpr local of the enclosing function, from a template
// lambda. The hint is instantiated after its loop, so the reference must not be
// left pending an odr-use check or turned into a capture of the local.
template <int N> void local_in_lambda(int *p) {
  constexpr int kII = N + 1;
  auto body = [&]<bool B>() {
#pragma clang loop hint(swp_ii, (kII))
    for (int i = 0; i != 4; ++i)
      p[i] += i;
  };
  body.template operator()<true>();
}

template void scaled<4>(int *);
template void Outer<3>::inner<6>(int *);
template void local_in_lambda<4>(int *);

// CHECK-DAG: !{!"llvm.loop.hint.swp_ii", i64 8}
// CHECK-DAG: !{!"llvm.loop.hint.swp_ii", i64 9}
// CHECK-DAG: !{!"llvm.loop.hint.swp_ii", i64 5}
