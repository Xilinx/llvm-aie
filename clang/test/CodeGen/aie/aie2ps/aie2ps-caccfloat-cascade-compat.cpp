//===- aie2ps-caccfloat-cascade-compat.cpp --------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// Verify that caccfloat cascade intrinsic compat declarations exist for
// AIE2PS. The ADF accessors.h header instantiates CASCADE_READ/WRITE for
// caccfloat when __AIE_ARCH__==22, requiring these declarations.

// RUN: %clang -fsyntax-only %s --target=aie2ps -nostdlibinc

// Cascade read -- single-width
v8caccfloat test_get_scd_v8caccfloat(int en) {
  return get_scd_v8caccfloat(en);
}
v8caccfloat test_get_scd_v8caccfloat_default() {
  return get_scd_v8caccfloat();
}

// Cascade read -- double-width
v16caccfloat test_get_scd_v16caccfloat(int en) {
  return get_scd_v16caccfloat(en);
}
v16caccfloat test_get_scd_v16caccfloat_default() {
  return get_scd_v16caccfloat();
}

// Cascade write -- single-width
void test_put_mcd_v8caccfloat(v8caccfloat a, int en) {
  put_mcd(a, en);
}
void test_put_mcd_v8caccfloat_default(v8caccfloat a) {
  put_mcd(a);
}

// Cascade write -- double-width
void test_put_mcd_v16caccfloat(v16caccfloat a, int en) {
  put_mcd(a, en);
}
