//===-------------------- AIEngine AIE2ps intrinsics ------------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2PS_NLF_VECTOR_H__
#define __AIE2PS_NLF_VECTOR_H__

INTRINSIC(v16bfloat16) exp2(v16accfloat a) {
  v16accfloat c = (v16accfloat)broadcast_float(895.001);
  v32accfloat b = add(set_v32accfloat(0, a), set_v32accfloat(0, c));
  return __builtin_aie2ps_exp2(extract_v16accfloat(b, 0));
}
INTRINSIC(v16bfloat16) exp2(v16float a) {
  v16accfloat c = (v16accfloat)broadcast_float(895.001);
  v32accfloat b = add(set_v32accfloat(0, (v16accfloat)a),
                      set_v32accfloat(0, (v16accfloat)c));
  return __builtin_aie2ps_exp2(extract_v16accfloat(b, 0));
}
INTRINSIC(v32bfloat16) exp2(v32accfloat a) {
  v16accfloat c = (v16accfloat)broadcast_float(895.001);
  v32accfloat b = add(a, concat(c, c));
  v32bfloat16 v = undef_v32bfloat16();
  v = insert(v, 0, __builtin_aie2ps_exp2(extract_v16accfloat(b, 0)));
  v = insert(v, 1, __builtin_aie2ps_exp2(extract_v16accfloat(b, 1)));
  return v;
}
INTRINSIC(v32bfloat16) exp2(v32float a) {
  v16accfloat c = (v16accfloat)broadcast_float(895.001);
  v32accfloat b = add((v32accfloat)a, concat(c, c));

  v32bfloat16 v = undef_v32bfloat16();
  v = insert(v, 0, __builtin_aie2ps_exp2(extract_v16accfloat(b, 0)));
  v = insert(v, 1, __builtin_aie2ps_exp2(extract_v16accfloat(b, 1)));
  return v;
}
INTRINSIC(v64bfloat16) exp2(v64accfloat a) {
  v16accfloat c = (v16accfloat)broadcast_float(895.001);
  v64accfloat b = add(a, concat(c, c, c, c));
  v64bfloat16 v = undef_v64bfloat16();
  v = insert(v, 0, __builtin_aie2ps_exp2(extract_v16accfloat(b, 0)));
  v = insert(v, 1, __builtin_aie2ps_exp2(extract_v16accfloat(b, 1)));
  v = insert(v, 2, __builtin_aie2ps_exp2(extract_v16accfloat(b, 2)));
  v = insert(v, 3, __builtin_aie2ps_exp2(extract_v16accfloat(b, 3)));
  return v;
}
INTRINSIC(v64bfloat16) exp2(v64float a) {
  v16accfloat c = (v16accfloat)broadcast_float(895.001);
  v64accfloat b = add((v64accfloat)a, concat(c, c, c, c));
  v64bfloat16 v = undef_v64bfloat16();
  v = insert(v, 0, __builtin_aie2ps_exp2(extract_v16accfloat(b, 0)));
  v = insert(v, 1, __builtin_aie2ps_exp2(extract_v16accfloat(b, 1)));
  v = insert(v, 2, __builtin_aie2ps_exp2(extract_v16accfloat(b, 2)));
  v = insert(v, 3, __builtin_aie2ps_exp2(extract_v16accfloat(b, 3)));
  return v;
}

INTRINSIC(v16accfloat) exp2_bf20(v16accfloat a) {
  v16accfloat c = (v16accfloat)broadcast_float(895.001);
  v32accfloat b = add(set_v32accfloat(0, a), set_v32accfloat(0, c));
  return __builtin_aie2ps_exp2_bf20(extract_v16accfloat(b, 0));
}
INTRINSIC(v16accfloat) exp2_bf20(v16float a) {
  v16accfloat c = (v16accfloat)broadcast_float(895.001);
  v32accfloat b =
      add(set_v32accfloat(0, (v16accfloat)a), set_v32accfloat(0, c));
  return __builtin_aie2ps_exp2_bf20(extract_v16accfloat(b, 0));
}

INTRINSIC(v32accfloat) exp2_bf20(v32accfloat a) {
  v16accfloat c = (v16accfloat)broadcast_float(895.001);
  v32accfloat b = add(a, concat(c, c));
  v32accfloat v = undef_v32accfloat();
  v = insert(v, 0, __builtin_aie2ps_exp2_bf20(extract_v16accfloat(b, 0)));
  v = insert(v, 1, __builtin_aie2ps_exp2_bf20(extract_v16accfloat(b, 1)));
  return v;
}
INTRINSIC(v32accfloat) exp2_bf20(v32float a) {
  v16accfloat c = (v16accfloat)broadcast_float(895.001);
  v32accfloat b = add((v32accfloat)a, concat(c, c));
  v32accfloat v = undef_v32accfloat();
  v = insert(v, 0, __builtin_aie2ps_exp2_bf20(extract_v16accfloat(b, 0)));
  v = insert(v, 1, __builtin_aie2ps_exp2_bf20(extract_v16accfloat(b, 1)));
  return v;
}
INTRINSIC(v64accfloat) exp2_bf20(v64accfloat a) {
  v16accfloat c = (v16accfloat)broadcast_float(895.001);
  v64accfloat b = add(a, concat(c, c, c, c));
  v64accfloat v = undef_v64accfloat();
  v = insert(v, 0, __builtin_aie2ps_exp2_bf20(extract_v16accfloat(b, 0)));
  v = insert(v, 1, __builtin_aie2ps_exp2_bf20(extract_v16accfloat(b, 1)));
  v = insert(v, 2, __builtin_aie2ps_exp2_bf20(extract_v16accfloat(b, 2)));
  v = insert(v, 3, __builtin_aie2ps_exp2_bf20(extract_v16accfloat(b, 3)));
  return v;
}
INTRINSIC(v64accfloat) exp2_bf20(v64float a) {
  v16accfloat c = (v16accfloat)broadcast_float(895.001);
  v64accfloat b = add((v64accfloat)a, concat(c, c, c, c));
  v64accfloat v = undef_v64accfloat();
  v = insert(v, 0, __builtin_aie2ps_exp2_bf20(extract_v16accfloat(b, 0)));
  v = insert(v, 1, __builtin_aie2ps_exp2_bf20(extract_v16accfloat(b, 1)));
  v = insert(v, 2, __builtin_aie2ps_exp2_bf20(extract_v16accfloat(b, 2)));
  v = insert(v, 3, __builtin_aie2ps_exp2_bf20(extract_v16accfloat(b, 3)));
  return v;
}

INTRINSIC(v16bfloat16) tanh(v16accfloat e) { return __builtin_aie2ps_tanh(e); }
INTRINSIC(v16bfloat16) tanh(v16float e) {
  return __builtin_aie2ps_tanh((v16accfloat)e);
}
INTRINSIC(v32bfloat16) tanh(v32accfloat a) {
  v32bfloat16 v = undef_v32bfloat16();
  v = insert(v, 0, tanh(extract_v16accfloat(a, 0)));
  v = insert(v, 1, tanh(extract_v16accfloat(a, 1)));
  return v;
}
INTRINSIC(v32bfloat16) tanh(v32float a) {
  v32bfloat16 v = undef_v32bfloat16();
  v = insert(v, 0, tanh(extract_v16float(a, 0)));
  v = insert(v, 1, tanh(extract_v16float(a, 1)));
  return v;
}

#endif // __AIE2PS_NLF_VECTOR_H__
