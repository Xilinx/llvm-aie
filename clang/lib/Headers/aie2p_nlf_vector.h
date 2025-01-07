//===-------------------- AIEngine AIE2p intrinsics ------------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2P_NLF_VECTOR_H__
#define __AIE2P_NLF_VECTOR_H__

INTRINSIC(v16bfloat16) exp2(v16accfloat e) { return __builtin_aie2p_exp2(e); }
INTRINSIC(v16bfloat16) tanh(v16accfloat e) { return __builtin_aie2p_tanh(e); }
INTRINSIC(v32bfloat16) tanh(v32accfloat a) {
  v32bfloat16 v = undef_v32bfloat16();
  v = insert(v, 0, tanh(extract_v16accfloat(a, 0)));
  v = insert(v, 1, tanh(extract_v16accfloat(a, 1)));
  return v;
}

INTRINSIC(v32bfloat16) exp2(v32accfloat a) {
  v32bfloat16 v = undef_v32bfloat16();
  v = insert(v, 0, exp2(extract_v16accfloat(a, 0)));
  v = insert(v, 1, exp2(extract_v16accfloat(a, 1)));
  return v;
}

INTRINSIC(v32bfloat16) tanh(v32float a) {
  v32bfloat16 v = undef_v32bfloat16();
  v = insert(v, 0, tanh(extract_v16float(a, 0)));
  v = insert(v, 1, tanh(extract_v16float(a, 1)));
  return v;
}

INTRINSIC(v32bfloat16) exp2(v32float a) {
  v32bfloat16 v = undef_v32bfloat16();
  v = insert(v, 0, exp2(extract_v16float(a, 0)));
  v = insert(v, 1, exp2(extract_v16float(a, 1)));
  return v;
}

#endif // __AIE2P_NLF_VECTOR_H__
