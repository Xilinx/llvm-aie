//===- aie-builtins.cpp -----------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: %clang -O2 -ffast-math --target=aie-none-elf %s -S -o %t
// RUN: cat %t | FileCheck %s
// RUN: %clang -O2 -ffast-math --target=aie-none-elf %s -o %t.o
// RUN: llvm-objdump -dr --triple=aie %t.o | FileCheck %s --check-prefix=CHECK-LINK

// We should automatically link against the builtins.

#include<stdint.h>

int64_t test_mul(int64_t x, int64_t y) {
// CHECK-LABEL: test_mul
// CHECK: jal __muldi3
// CHECK-LINK-DAG: <__muldi3>
  return x*y;
}

extern "C" float sqrtf(float x);
float test_sqrtf(float x) {
// CHECK-LABEL: test_sqrtf
// CHECK: sqrt
  return sqrtf(x);
}

extern "C" float rsqrtf(float x);
float test_rsqrtf(float x) {
// CHECK-LABEL: test_rsqrtf
// CHECK: invsqrt
  return rsqrtf(x);
}

float test_oneover(float x) {
// CHECK-LABEL: test_oneover
// CHECK: jal __divsf3
// CHECK-LINK-DAG: <__divsf3>
  return 1.0f/x;
}

float test_inv(float x) {
// CHECK-LABEL: test_inv
// CHECK:  inv    r1, [[REG:r[0-9]+]]
  return inv(x);
}

float test_builtin_sqrt_flt_flt(float x) {
// CHECK-LABEL: test_builtin_sqrt_flt_flt
// CHECK:  sqrt    r1, [[REG:r[0-9]+]]
  return _sqrtf(x);
}

int32_t test_builtin_sqrt_fix_flt(float x) {
// CHECK-LABEL: test_builtin_sqrt_fix_flt
// CHECK:  mov.u20 [[REG:r[0-9]+]], #4
// CHECK:  mov     s3, [[REG]]
// CHECK:  sqrt    r1.FLT2FX, s3, r6
  return __builtin_aie_sqrt_fix_flt(4, x);
}

float test_builtin_sqrt_flt_fix(int32_t x) {
// CHECK-LABEL: test_builtin_sqrt_flt_fix
// CHECK:  mov.u20 [[REG:r[0-9]+]], #4
// CHECK:  mov     s2, [[REG]]
// CHECK:  sqrt    r1, r6.FX2FLT, s2
  return __builtin_aie_sqrt_flt_fix(x, 4);
}

int32_t test_builtin_sqrt_fix_fix(int32_t x) {
// CHECK-LABEL: test_builtin_sqrt_fix_fix
// CHECK:  mov.u20 [[REG:r[0-9]+]], #4
// CHECK:  mov     s2, [[REG]]
// CHECK:  mov     s3, [[REG]]
// CHECK:  sqrt    r1.FLT2FX, s3, r6.FX2FLT, s2
  return sqrt(x, 4, 4);
}

float test_builtin_invsqrt_flt_flt(float x) {
// CHECK-LABEL: test_builtin_invsqrt_flt_flt
// CHECK:  invsqrt    r1, [[REG:r[0-9]+]]
  return __builtin_aie_invsqrt_flt_flt(x);
}

int32_t test_builtin_invsqrt_fix_flt(float x) {
// CHECK-LABEL: test_builtin_invsqrt_fix_flt
// CHECK:  mov.u20 [[REG:r[0-9]+]], #4
// CHECK:  mov     s3, [[REG]]
// CHECK:  invsqrt r1.FLT2FX, s3, r6
  return __builtin_aie_invsqrt_fix_flt(4, x);
}

float test_builtin_invsqrt_flt_fix(int32_t x) {
// CHECK-LABEL: test_builtin_invsqrt_flt_fix
// CHECK:  mov.u20 [[REG:r[0-9]+]], #4
// CHECK:  mov     s2, [[REG]]
// CHECK:  invsqrt    r1, r6.FX2FLT, s2
  return __builtin_aie_invsqrt_flt_fix(x, 4);
}

int32_t test_builtin_invsqrt_fix_fix(int32_t x) {
// CHECK-LABEL: test_builtin_invsqrt_fix_fix
// CHECK:  mov.u20 [[REG:r[0-9]+]], #4
// CHECK:  mov     s2, [[REG]]
// CHECK:  mov     s3, [[REG]]
// CHECK:  invsqrt r1.FLT2FX, s3, r6.FX2FLT, s2
  return __builtin_aie_invsqrt_fix_fix(4, x, 4);
}

float test_builtin_inv_flt_flt(float x) {
// CHECK-LABEL: test_builtin_inv_flt_flt
// CHECK:  inv    r1, [[REG:r[0-9]+]]
  return __builtin_aie_inv_flt_flt(x);
}

int32_t test_builtin_inv_fix_flt(float x) {
// CHECK-LABEL: test_builtin_inv_fix_flt
// CHECK:  mov.u20 [[REG:r[0-9]+]], #4
// CHECK:  mov     s3, [[REG]]
// CHECK:  inv     r1.FLT2FX, s3, r6
  return __builtin_aie_inv_fix_flt(4, x);
}

float test_builtin_inv_flt_fix(int32_t x) {
// CHECK-LABEL: test_builtin_inv_flt_fix
// CHECK:  mov.u20 [[REG:r[0-9]+]], #4
// CHECK:  mov     s2, [[REG]]
// CHECK:  inv     r1, r6.FX2FLT, s2
  return __builtin_aie_inv_flt_fix(x, 4);
}

int32_t test_builtin_inv_fix_fix(int32_t x) {
// CHECK-LABEL: test_builtin_inv_fix_fix
// CHECK:  mov.u20 [[REG:r[0-9]+]], #4
// CHECK:  mov     s2, [[REG]]
// CHECK:  mov     s3, [[REG]]
// CHECK:  inv     r1.FLT2FX, s3, r6.FX2FLT, s2
  return __builtin_aie_inv_fix_fix(4, x, 4);
}
