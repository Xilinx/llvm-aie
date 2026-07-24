//===-- generic-loop-hint-template-nonexpr.cpp ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its
// affiliates
//
//===----------------------------------------------------------------------===//

// Generic loop hints with a non-dependent value (key-only, string, identifier,
// or constant) must survive template instantiation unchanged.

// RUN: %clang_cc1 -triple aie2ps -std=c++20 -emit-llvm -o - %s | FileCheck %s

template <int N> void key_only(int *p) {
#pragma clang loop hint(no_predication)
  for (int i = 0; i != 4; ++i)
    p[i] += i;
}

template <int N> void string_valued(int *p) {
#pragma clang loop hint(config, "pipelinesolve(NS=4,II=7)")
  for (int i = 0; i != 4; ++i)
    p[i] += i;
}

template <int N> void ident_valued(int *p) {
#pragma clang loop hint(use_pipeliner, pre)
  for (int i = 0; i != 4; ++i)
    p[i] += i;
}

template <int N> void nondependent(int *p) {
#pragma clang loop hint(swp_ii, (3 + 5))
  for (int i = 0; i != 4; ++i)
    p[i] += i;
}

template void key_only<1>(int *);
template void string_valued<1>(int *);
template void ident_valued<1>(int *);
template void nondependent<1>(int *);

// CHECK-DAG: !{!"llvm.loop.hint.no_predication", i64 1}
// CHECK-DAG: !{!"llvm.loop.hint.config", !"pipelinesolve(NS=4,II=7)"}
// CHECK-DAG: !{!"llvm.loop.hint.use_pipeliner", !"pre"}
// CHECK-DAG: !{!"llvm.loop.hint.swp_ii", i64 8}
