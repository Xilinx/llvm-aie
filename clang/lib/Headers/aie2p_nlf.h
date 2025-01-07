//===-------------------- AIEngine AIE2p intrinsics ------------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2P_NLF_H__
#define __AIE2P_NLF_H__

INTRINSIC(float) _sqrtf(float a) { return __builtin_aie2p_sqrtf(a); }

INTRINSIC(int) sqrt(int a, int sft1, int sft2) {
  return float2fix(_sqrtf(fix2float(a, sft1)), sft2);
}

INTRINSIC(float) invsqrt(float a) { return __builtin_aie2p_invsqrt(a); }

INTRINSIC(int) invsqrt(int a, int sft1, int sft2) {
  return float2fix(invsqrt(fix2float(a, sft1)), sft2);
}

INTRINSIC(float) inv(float a) { return __builtin_aie2p_inv(a); }

INTRINSIC(int) inv(int a, int sft1, int sft2) {
  return float2fix(inv(fix2float(a, sft1)), sft2);
}

#endif // __AIE2P_NLF_H__
