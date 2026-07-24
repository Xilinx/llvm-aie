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

// RUN: not --crash %clang_cc1 -triple aie2ps -std=c++20 -emit-llvm -o - %s 2>&1 | FileCheck %s --check-prefix=CRASH

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

// CRASH: Expression evaluator can't be called on a dependent expression.
