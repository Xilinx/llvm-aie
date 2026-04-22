//===- aie2ps_scalar.h ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

#ifndef __AIE2PS_SCALAR_H__
#define __AIE2PS_SCALAR_H__

INTRINSIC(int) get_coreid() { return __builtin_aie2ps_get_coreid(); }

INTRINSIC(unsigned) clb(unsigned x) { return __builtin_clz(x); }
INTRINSIC(unsigned) clb(unsigned long long x) { return __builtin_clzll(x); }
INTRINSIC(unsigned) clb(int x) { return __builtin_aie2ps_clb(x); }
INTRINSIC(unsigned) clb(long long x) {
  return x >= 0 ? __builtin_clzll(x) : __builtin_clzll(~x);
}

INTRINSIC(unsigned) population_count(int x) { return __builtin_popcount(x); }

INTRINSIC(unsigned) population_count(unsigned x) {
  return __builtin_popcount(x);
}

#endif //__AIE2PS_SCALAR_H__
