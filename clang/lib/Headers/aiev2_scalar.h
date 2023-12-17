//===- aiev2_scalar.h -------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV2_SCALAR_H__
#define __AIEV2_SCALAR_H__

INTRINSIC(int) get_coreid() { return __builtin_aiev2_get_coreid(); }

INTRINSIC(unsigned) clb(unsigned x) { return __builtin_clz(x); }
INTRINSIC(unsigned) clb(unsigned long long x) { return __builtin_clzll(x); }
INTRINSIC(unsigned) clb(int x) { return __builtin_aiev2_clb(x); }
INTRINSIC(unsigned) clb(long long x) {
  return x >= 0 ? __builtin_clzll(x) : __builtin_clzll(~x);
}

#endif //__AIEV2_SCALAR_H__
