//===-- generic-loop-hint-template.cpp -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its
// affiliates
//
//===----------------------------------------------------------------------===//

// RUN: %clang_cc1 -triple aie2ps -std=c++20 -emit-llvm -o - %s | FileCheck %s

enum out_mode_t { FULL, PARTIAL };

template <out_mode_t OutMode, bool EnablePsumInt32>
void gemm_like(int *p) {
#pragma clang loop hint(aie-olp-war-rename, ((OutMode == FULL && EnablePsumInt32) ? 1 : 0))
  for (int i = 0; i != 4; ++i)
    p[i] += i;
}

template void gemm_like<FULL, false>(int *);
template void gemm_like<FULL, true>(int *);
template void gemm_like<PARTIAL, false>(int *);
template void gemm_like<PARTIAL, true>(int *);

// CHECK-LABEL: define weak_odr void @_Z9gemm_likeIL10out_mode_t0ELb0EEvPi(
// CHECK: br label %for.cond, !llvm.loop [[FULL_FALSE_LOOP:![0-9]+]]
// CHECK-LABEL: define weak_odr void @_Z9gemm_likeIL10out_mode_t0ELb1EEvPi(
// CHECK: br label %for.cond, !llvm.loop [[FULL_TRUE_LOOP:![0-9]+]]
// CHECK-LABEL: define weak_odr void @_Z9gemm_likeIL10out_mode_t1ELb0EEvPi(
// CHECK: br label %for.cond, !llvm.loop [[PARTIAL_FALSE_LOOP:![0-9]+]]
// CHECK-LABEL: define weak_odr void @_Z9gemm_likeIL10out_mode_t1ELb1EEvPi(
// CHECK: br label %for.cond, !llvm.loop [[PARTIAL_TRUE_LOOP:![0-9]+]]
// CHECK: [[FULL_FALSE_LOOP]] = distinct !{[[FULL_FALSE_LOOP]], !{{[0-9]+}}, [[ZERO_HINT:![0-9]+]]}
// CHECK: [[ZERO_HINT]] = !{!"llvm.loop.hint.aie-olp-war-rename", i64 0}
// CHECK: [[FULL_TRUE_LOOP]] = distinct !{[[FULL_TRUE_LOOP]], !{{[0-9]+}}, [[ONE_HINT:![0-9]+]]}
// CHECK: [[ONE_HINT]] = !{!"llvm.loop.hint.aie-olp-war-rename", i64 1}
// CHECK: [[PARTIAL_FALSE_LOOP]] = distinct !{[[PARTIAL_FALSE_LOOP]], !{{[0-9]+}}, [[ZERO_HINT]]}
// CHECK: [[PARTIAL_TRUE_LOOP]] = distinct !{[[PARTIAL_TRUE_LOOP]], !{{[0-9]+}}, [[ZERO_HINT]]}
