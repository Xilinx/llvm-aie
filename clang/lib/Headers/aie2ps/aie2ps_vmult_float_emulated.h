//===-------------------- AIEngine AIE2ps intrinsics ------------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2PS_VMULT_FLOAT_EMULATED_H__
#define __AIE2PS_VMULT_FLOAT_EMULATED_H__
// -----------------------------------------------------------------------------
// Elementwise
// -----------------------------------------------------------------------------
// Vector float - multiplication
// 32 lanes
INTRINSIC(v32accfloat)
mul_elem_32_accuracy_low_inner(v32float v1, v32float v2, int sub_mask,
                               int sub_mul) {
  v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();
  v32bfloat16 a = to_v32bfloat16((v32accfloat)v1);
  v32bfloat16 b = to_v32bfloat16(msc_elem_32(a, dummy0, (v32accfloat)v1));
  v32bfloat16 c = to_v32bfloat16((v32accfloat)v2);
  v32bfloat16 d = to_v32bfloat16(msc_elem_32(c, dummy0, (v32accfloat)v2));
  return mac_elem_32_cmplx_conf(
      a, c,
      mac_elem_32_cmplx_conf(a, d,
                             mul_elem_32_cmplx_conf(b, c, sub_mask, sub_mul), 0,
                             sub_mask, sub_mul, 0),
      0, sub_mask, sub_mul, 0);
}
INTRINSIC(v32accfloat)
mul_elem_32_accuracy_fast_inner(v32float v1, v32float v2, int sub_mask,
                                int sub_mul) {
  v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();
  v32bfloat16 a = to_v32bfloat16((v32accfloat)v1);
  v32accfloat acc0 = msc_elem_32(a, dummy0, (v32accfloat)v1);
  v32bfloat16 b = to_v32bfloat16(acc0);
  v32bfloat16 c = to_v32bfloat16(msc_elem_32(b, dummy0, acc0));
  v32bfloat16 d = to_v32bfloat16((v32accfloat)v2);
  v32accfloat acc1 = msc_elem_32(d, dummy0, (v32accfloat)v2);
  v32bfloat16 e = to_v32bfloat16(acc1);
  v32bfloat16 f = to_v32bfloat16(msc_elem_32(e, dummy0, acc1));
  return mac_elem_32_cmplx_conf(
      a, d,
      mac_elem_32_cmplx_conf(
          a, e,
          mac_elem_32_cmplx_conf(
              b, d,
              mac_elem_32_cmplx_conf(
                  d, c,
                  mac_elem_32_cmplx_conf(
                      b, e, mul_elem_32_cmplx_conf(a, f, sub_mask, sub_mul), 0,
                      sub_mask, sub_mul, 0),
                  0, sub_mask, sub_mul, 0),
              0, sub_mask, sub_mul, 0),
          0, sub_mask, sub_mul, 0),
      0, sub_mask, sub_mul, 0);
}
INTRINSIC(v32accfloat)
mul_elem_32_accuracy_safe_inner(v32float v1, v32float v2, int sub_mask,
                                int sub_mul) {
  v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();
  v32bfloat16 a = to_v32bfloat16((v32accfloat)v1);
  v32accfloat acc0 = msc_elem_32(a, dummy0, (v32accfloat)v1);
  v32bfloat16 b = to_v32bfloat16(acc0);
  v32bfloat16 c = to_v32bfloat16(msc_elem_32(b, dummy0, acc0));
  v32bfloat16 d = to_v32bfloat16((v32accfloat)v2);
  v32accfloat acc1 = msc_elem_32(d, dummy0, (v32accfloat)v2);
  v32bfloat16 e = to_v32bfloat16(acc1);
  v32bfloat16 f = to_v32bfloat16(msc_elem_32(e, dummy0, acc1));
  v32accfloat __aie_register_keep() tmp =
      mul_elem_32_cmplx_conf(c, f, sub_mask, sub_mul);
  tmp += mul_elem_32_cmplx_conf(c, e, sub_mask, sub_mul);
  tmp += mul_elem_32_cmplx_conf(b, f, sub_mask, sub_mul);
  tmp += mul_elem_32_cmplx_conf(a, f, sub_mask, sub_mul);
  tmp += mul_elem_32_cmplx_conf(b, e, sub_mask, sub_mul);
  tmp += mul_elem_32_cmplx_conf(d, c, sub_mask, sub_mul);
  tmp += mul_elem_32_cmplx_conf(b, d, sub_mask, sub_mul);
  tmp += mul_elem_32_cmplx_conf(a, e, sub_mask, sub_mul);
  tmp += mul_elem_32_cmplx_conf(a, d, sub_mask, sub_mul);
  return tmp;
}

// 64 lanes
INTRINSIC(v64accfloat)
mul_elem_64_accuracy_low_inner(v64float v1, v64float v2, int sub_mask,
                               int sub_mul) {
  v64bfloat16 a = undef_v64bfloat16();
  v64bfloat16 b = undef_v64bfloat16();
  v64bfloat16 c = undef_v64bfloat16();
  v64bfloat16 d = undef_v64bfloat16();
  v64bfloat16 dummy0 = undef_v64bfloat16();
  dummy0 = insert(dummy0, 0, broadcast_one_to_v32bfloat16());
  dummy0 = insert(dummy0, 1, broadcast_one_to_v32bfloat16());
  a = insert(a, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v1, 0)));
  a = insert(a, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v1, 1)));
  b = insert(b, 0,
             to_v32bfloat16(extract_v32accfloat(
                 msc_elem_64(a, dummy0, (v64accfloat)v1), 0)));
  b = insert(b, 1,
             to_v32bfloat16(extract_v32accfloat(
                 msc_elem_64(a, dummy0, (v64accfloat)v1), 1)));
  c = insert(c, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 0)));
  c = insert(c, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 1)));
  d = insert(d, 0,
             to_v32bfloat16(extract_v32accfloat(
                 msc_elem_64(c, dummy0, (v64accfloat)v2), 0)));
  d = insert(d, 1,
             to_v32bfloat16(extract_v32accfloat(
                 msc_elem_64(c, dummy0, (v64accfloat)v2), 1)));
  return mac_elem_64_cmplx_conf(
      a, c,
      mac_elem_64_cmplx_conf(a, d,
                             mul_elem_64_cmplx_conf(b, c, sub_mask, sub_mul), 0,
                             sub_mask, sub_mul, 0),
      0, sub_mask, sub_mul, 0);
}
INTRINSIC(v64accfloat)
mul_elem_64_accuracy_fast_inner(v64float v1, v64float v2, int sub_mask,
                                int sub_mul) {
  v64bfloat16 a = undef_v64bfloat16();
  v64bfloat16 b = undef_v64bfloat16();
  v64bfloat16 c = undef_v64bfloat16();
  v64bfloat16 d = undef_v64bfloat16();
  v64bfloat16 e = undef_v64bfloat16();
  v64bfloat16 f = undef_v64bfloat16();
  v64bfloat16 dummy0 = undef_v64bfloat16();
  dummy0 = insert(dummy0, 0, broadcast_one_to_v32bfloat16());
  dummy0 = insert(dummy0, 1, broadcast_one_to_v32bfloat16());
  a = insert(a, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v1, 0)));
  a = insert(a, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v1, 1)));
  v64accfloat acc0 = msc_elem_64(a, dummy0, (v64accfloat)v1);
  b = insert(b, 0, to_v32bfloat16(extract_v32accfloat(acc0, 0)));
  b = insert(b, 1, to_v32bfloat16(extract_v32accfloat(acc0, 1)));
  c = insert(
      c, 0,
      to_v32bfloat16(extract_v32accfloat(msc_elem_64(b, dummy0, acc0), 0)));
  c = insert(
      c, 1,
      to_v32bfloat16(extract_v32accfloat(msc_elem_64(b, dummy0, acc0), 1)));
  d = insert(d, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 0)));
  d = insert(d, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 1)));
  v64accfloat acc1 = msc_elem_64(d, dummy0, (v64accfloat)v2);
  e = insert(e, 0, to_v32bfloat16(extract_v32accfloat(acc1, 0)));
  e = insert(e, 1, to_v32bfloat16(extract_v32accfloat(acc1, 1)));
  f = insert(
      f, 0,
      to_v32bfloat16(extract_v32accfloat(msc_elem_64(e, dummy0, acc1), 0)));
  f = insert(
      f, 1,
      to_v32bfloat16(extract_v32accfloat(msc_elem_64(e, dummy0, acc1), 1)));
  return mac_elem_64_cmplx_conf(
      a, d,
      mac_elem_64_cmplx_conf(
          a, e,
          mac_elem_64_cmplx_conf(
              b, d,
              mac_elem_64_cmplx_conf(
                  d, c,
                  mac_elem_64_cmplx_conf(
                      b, e, mul_elem_64_cmplx_conf(a, f, sub_mask, sub_mul), 0,
                      sub_mask, sub_mul, 0),
                  0, sub_mask, sub_mul, 0),
              0, sub_mask, sub_mul, 0),
          0, sub_mask, sub_mul, 0),
      0, sub_mask, sub_mul, 0);
}
INTRINSIC(v64accfloat)
mul_elem_64_accuracy_safe_inner(v64float v1, v64float v2, int sub_mask,
                                int sub_mul) {
  v64bfloat16 a = undef_v64bfloat16();
  v64bfloat16 b = undef_v64bfloat16();
  v64bfloat16 c = undef_v64bfloat16();
  v64bfloat16 d = undef_v64bfloat16();
  v64bfloat16 e = undef_v64bfloat16();
  v64bfloat16 f = undef_v64bfloat16();
  v64bfloat16 dummy0 = undef_v64bfloat16();
  dummy0 = insert(dummy0, 0, broadcast_one_to_v32bfloat16());
  dummy0 = insert(dummy0, 1, broadcast_one_to_v32bfloat16());
  a = insert(a, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v1, 0)));
  a = insert(a, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v1, 1)));
  v64accfloat acc0 = msc_elem_64(a, dummy0, (v64accfloat)v1);
  b = insert(b, 0, to_v32bfloat16(extract_v32accfloat(acc0, 0)));
  b = insert(b, 1, to_v32bfloat16(extract_v32accfloat(acc0, 1)));
  c = insert(
      c, 0,
      to_v32bfloat16(extract_v32accfloat(msc_elem_64(b, dummy0, acc0), 0)));
  c = insert(
      c, 1,
      to_v32bfloat16(extract_v32accfloat(msc_elem_64(b, dummy0, acc0), 1)));
  d = insert(d, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 0)));
  d = insert(d, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 1)));
  v64accfloat acc1 = msc_elem_64(d, dummy0, (v64accfloat)v2);
  e = insert(e, 0, to_v32bfloat16(extract_v32accfloat(acc1, 0)));
  e = insert(e, 1, to_v32bfloat16(extract_v32accfloat(acc1, 1)));
  f = insert(
      f, 0,
      to_v32bfloat16(extract_v32accfloat(msc_elem_64(e, dummy0, acc1), 0)));
  f = insert(
      f, 1,
      to_v32bfloat16(extract_v32accfloat(msc_elem_64(e, dummy0, acc1), 1)));
  v64accfloat __aie_register_keep() tmp =
      mul_elem_64_cmplx_conf(c, f, sub_mask, sub_mul);
  tmp += mul_elem_64_cmplx_conf(c, e, sub_mask, sub_mul);
  tmp += mul_elem_64_cmplx_conf(b, f, sub_mask, sub_mul);
  tmp += mul_elem_64_cmplx_conf(a, f, sub_mask, sub_mul);
  tmp += mul_elem_64_cmplx_conf(b, e, sub_mask, sub_mul);
  tmp += mul_elem_64_cmplx_conf(d, c, sub_mask, sub_mul);
  tmp += mul_elem_64_cmplx_conf(b, d, sub_mask, sub_mul);
  tmp += mul_elem_64_cmplx_conf(a, e, sub_mask, sub_mul);
  tmp += mul_elem_64_cmplx_conf(a, d, sub_mask, sub_mul);
  return tmp;
}

// -----------------------------------------------------------------------------
// Matrix mode: 4x8 x 8x8
// -----------------------------------------------------------------------------
INTRINSIC(v32accfloat)
mul_4x8_8x8_accuracy_safe_inner(v32float v1, v64float v2, int sub_mask,
                                int sub_mul) {
  v32bfloat16 v32dummy0 = broadcast_one_to_v32bfloat16();
  v32bfloat16 a = to_v32bfloat16((v32accfloat)v1);
  v32accfloat acc0 = msc_elem_32(a, v32dummy0, (v32accfloat)v1);
  v32bfloat16 b = to_v32bfloat16(acc0);
  v32bfloat16 c = to_v32bfloat16(msc_elem_32(b, v32dummy0, acc0));

  v64bfloat16 v64dummy0 = undef_v64bfloat16();
  v64bfloat16 d = undef_v64bfloat16();
  v64bfloat16 e = undef_v64bfloat16();
  v64bfloat16 f = undef_v64bfloat16();
  v64dummy0 = insert(v64dummy0, 0, broadcast_one_to_v32bfloat16());
  v64dummy0 = insert(v64dummy0, 1, broadcast_one_to_v32bfloat16());
  d = insert(d, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 0)));
  d = insert(d, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 1)));
  v64accfloat acc1 = msc_elem_64(d, v64dummy0, (v64accfloat)v2);
  e = insert(e, 0, to_v32bfloat16(extract_v32accfloat(acc1, 0)));
  e = insert(e, 1, to_v32bfloat16(extract_v32accfloat(acc1, 1)));
  f = insert(
      f, 0,
      to_v32bfloat16(extract_v32accfloat(msc_elem_64(e, v64dummy0, acc1), 0)));
  f = insert(
      f, 1,
      to_v32bfloat16(extract_v32accfloat(msc_elem_64(e, v64dummy0, acc1), 1)));

  v32accfloat __aie_register_keep() tmp = mul_4x8_8x8_conf(c, f, sub_mul);
  tmp += mul_4x8_8x8_conf(c, e, sub_mul);
  tmp += mul_4x8_8x8_conf(b, f, sub_mul);
  tmp += mul_4x8_8x8_conf(a, f, sub_mul);
  tmp += mul_4x8_8x8_conf(b, e, sub_mul);
  tmp += mul_4x8_8x8_conf(c, d, sub_mul);
  tmp += mul_4x8_8x8_conf(b, d, sub_mul);
  tmp += mul_4x8_8x8_conf(a, e, sub_mul);
  tmp += mul_4x8_8x8_conf(a, d, sub_mul);

  return tmp;
}

INTRINSIC(v32accfloat)
mul_4x8_8x8_accuracy_fast_inner(v32float v1, v64float v2, int sub_mask,
                                int sub_mul) {
  v32bfloat16 v32dummy0 = broadcast_one_to_v32bfloat16();
  v32bfloat16 a = to_v32bfloat16((v32accfloat)v1);
  v32accfloat acc0 = msc_elem_32(a, v32dummy0, (v32accfloat)v1);
  v32bfloat16 b = to_v32bfloat16(acc0);
  v32bfloat16 c = to_v32bfloat16(msc_elem_32(b, v32dummy0, acc0));

  v64bfloat16 v64dummy0 = undef_v64bfloat16();
  v64bfloat16 d = undef_v64bfloat16();
  v64bfloat16 e = undef_v64bfloat16();
  v64bfloat16 f = undef_v64bfloat16();
  v64dummy0 = insert(v64dummy0, 0, broadcast_one_to_v32bfloat16());
  v64dummy0 = insert(v64dummy0, 1, broadcast_one_to_v32bfloat16());
  d = insert(d, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 0)));
  d = insert(d, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 1)));
  v64accfloat acc1 = msc_elem_64(d, v64dummy0, (v64accfloat)v2);
  e = insert(e, 0, to_v32bfloat16(extract_v32accfloat(acc1, 0)));
  e = insert(e, 1, to_v32bfloat16(extract_v32accfloat(acc1, 1)));
  f = insert(
      f, 0,
      to_v32bfloat16(extract_v32accfloat(msc_elem_64(e, v64dummy0, acc1), 0)));
  f = insert(
      f, 1,
      to_v32bfloat16(extract_v32accfloat(msc_elem_64(e, v64dummy0, acc1), 1)));

  return mac_4x8_8x8_conf(
      a, d,
      mac_4x8_8x8_conf(
          a, e,
          mac_4x8_8x8_conf(
              b, d,
              mac_4x8_8x8_conf(c, d,
                               mac_4x8_8x8_conf(b, e,
                                                mul_4x8_8x8_conf(a, f, sub_mul),
                                                0, sub_mul, 0),
                               0, sub_mul, 0),
              0, sub_mul, 0),
          0, sub_mul, 0),
      0, sub_mul, 0);
}

INTRINSIC(v32accfloat)
mul_4x8_8x8_accuracy_low_inner(v32float v1, v64float v2, int sub_mask,
                               int sub_mul) {
  v32bfloat16 v32dummy0 = broadcast_one_to_v32bfloat16();
  v32bfloat16 a = to_v32bfloat16((v32accfloat)v1);
  v32bfloat16 b = to_v32bfloat16(msc_elem_32(a, v32dummy0, (v32accfloat)v1));
  v64bfloat16 c = undef_v64bfloat16();
  v64bfloat16 d = undef_v64bfloat16();
  v64bfloat16 v64dummy0 = undef_v64bfloat16();
  v64dummy0 = insert(v64dummy0, 0, broadcast_one_to_v32bfloat16());
  v64dummy0 = insert(v64dummy0, 1, broadcast_one_to_v32bfloat16());
  c = insert(c, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 0)));
  c = insert(c, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 1)));
  d = insert(d, 0,
             to_v32bfloat16(extract_v32accfloat(
                 msc_elem_64(c, v64dummy0, (v64accfloat)v2), 0)));
  d = insert(d, 1,
             to_v32bfloat16(extract_v32accfloat(
                 msc_elem_64(c, v64dummy0, (v64accfloat)v2), 1)));

  return mac_4x8_8x8_conf(
      a, d,
      mac_4x8_8x8_conf(a, c, mul_4x8_8x8_conf(b, d, sub_mul), 0, sub_mul, 0), 0,
      sub_mul, 0);
}

#define _TEMPLATE_MAC_MSC_ELEM(TYPE, REDUCTION, ADDSUB, ACC_NEG2)              \
  INTRINSIC(v32accfloat)                                                       \
  TYPE##_elem_32_accuracy_low_inner(v32float v1, v32float v2, v32accfloat acc, \
                                    int zero_acc, int sub_mask, int sub_mul,   \
                                    int sub_acc1) {                            \
    v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();                       \
    v32bfloat16 a = to_v32bfloat16((v32accfloat)v1);                           \
    v32bfloat16 b = to_v32bfloat16(msc_elem_32(a, dummy0, (v32accfloat)v1));   \
    v32bfloat16 c = to_v32bfloat16((v32accfloat)v2);                           \
    v32bfloat16 d = to_v32bfloat16(msc_elem_32(c, dummy0, (v32accfloat)v2));   \
    return REDUCTION##_elem_32_cmplx_conf(                                     \
        a, c, acc,                                                             \
        mac_elem_32_cmplx_conf(                                                \
            a, d, mul_elem_32_cmplx_conf(b, c, sub_mask, sub_mul), 0,          \
            sub_mask, sub_mul, 0),                                             \
        zero_acc, sub_mask, sub_mul, sub_acc1, ACC_NEG2);                      \
  }                                                                            \
  INTRINSIC(v32accfloat)                                                       \
  TYPE##_elem_32_accuracy_fast_inner(                                          \
      v32float v1, v32float v2, v32accfloat acc, int zero_acc, int sub_mask,   \
      int sub_mul, int sub_acc1) {                                             \
    v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();                       \
    v32bfloat16 a = to_v32bfloat16((v32accfloat)v1);                           \
    v32accfloat acc0 = msc_elem_32(a, dummy0, (v32accfloat)v1);                \
    v32bfloat16 b = to_v32bfloat16(acc0);                                      \
    v32bfloat16 c = to_v32bfloat16(msc_elem_32(b, dummy0, acc0));              \
    v32bfloat16 d = to_v32bfloat16((v32accfloat)v2);                           \
    v32accfloat acc1 = msc_elem_32(d, dummy0, (v32accfloat)v2);                \
    v32bfloat16 e = to_v32bfloat16(acc1);                                      \
    v32bfloat16 f = to_v32bfloat16(msc_elem_32(e, dummy0, acc1));              \
    return REDUCTION##_elem_32_cmplx_conf(                                     \
        a, d, acc,                                                             \
        mac_elem_32_cmplx_conf(                                                \
            a, e,                                                              \
            mac_elem_32_cmplx_conf(                                            \
                b, d,                                                          \
                mac_elem_32_cmplx_conf(                                        \
                    d, c,                                                      \
                    mac_elem_32_cmplx_conf(                                    \
                        b, e,                                                  \
                        mac_elem_32_cmplx_conf(                                \
                            a, f,                                              \
                            mac_elem_32_cmplx_conf(                            \
                                b, f,                                          \
                                mac_elem_32_cmplx_conf(                        \
                                    c, e,                                      \
                                    mul_elem_32_cmplx_conf(c, f, sub_mask,     \
                                                           sub_mul),           \
                                    0, sub_mask, sub_mul, 0),                  \
                                0, sub_mask, sub_mul, 0),                      \
                            0, sub_mask, sub_mul, 0),                          \
                        0, sub_mask, sub_mul, 0),                              \
                    0, sub_mask, sub_mul, 0),                                  \
                0, sub_mask, sub_mul, 0),                                      \
            0, sub_mask, sub_mul, 0),                                          \
        zero_acc, sub_mask, sub_mul, sub_acc1, ACC_NEG2);                      \
  }                                                                            \
  INTRINSIC(v32accfloat)                                                       \
  TYPE##_elem_32_accuracy_safe_inner(                                          \
      v32float v1, v32float v2, v32accfloat acc, int zero_acc, int sub_mask,   \
      int sub_mul, int sub_acc1) {                                             \
    v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();                       \
    v32bfloat16 a = to_v32bfloat16((v32accfloat)v1);                           \
    v32accfloat acc0 = msc_elem_32(a, dummy0, (v32accfloat)v1);                \
    v32bfloat16 b = to_v32bfloat16(acc0);                                      \
    v32bfloat16 c = to_v32bfloat16(msc_elem_32(b, dummy0, acc0));              \
    v32bfloat16 d = to_v32bfloat16((v32accfloat)v2);                           \
    v32accfloat acc1 = msc_elem_32(d, dummy0, (v32accfloat)v2);                \
    v32bfloat16 e = to_v32bfloat16(acc1);                                      \
    v32bfloat16 f = to_v32bfloat16(msc_elem_32(e, dummy0, acc1));              \
    v32accfloat __aie_register_keep() tmp =                                    \
        mul_elem_32_cmplx_conf(c, f, sub_mask, sub_mul);                       \
    tmp += mul_elem_32_cmplx_conf(c, e, sub_mask, sub_mul);                    \
    tmp += mul_elem_32_cmplx_conf(b, f, sub_mask, sub_mul);                    \
    tmp += mul_elem_32_cmplx_conf(a, f, sub_mask, sub_mul);                    \
    tmp += mul_elem_32_cmplx_conf(b, e, sub_mask, sub_mul);                    \
    tmp += mul_elem_32_cmplx_conf(d, c, sub_mask, sub_mul);                    \
    tmp += mul_elem_32_cmplx_conf(b, d, sub_mask, sub_mul);                    \
    tmp += mul_elem_32_cmplx_conf(a, e, sub_mask, sub_mul);                    \
    tmp += mul_elem_32_cmplx_conf(a, d, sub_mask, sub_mul);                    \
    return ADDSUB##_conf(acc, tmp, zero_acc, sub_acc1, 0);                     \
  }                                                                            \
  INTRINSIC(v64accfloat)                                                       \
  TYPE##_elem_64_accuracy_low_inner(v64float v1, v64float v2, v64accfloat acc, \
                                    int zero_acc, int sub_mask, int sub_mul,   \
                                    int sub_acc1) {                            \
    v64bfloat16 a = undef_v64bfloat16();                                       \
    v64bfloat16 b = undef_v64bfloat16();                                       \
    v64bfloat16 c = undef_v64bfloat16();                                       \
    v64bfloat16 d = undef_v64bfloat16();                                       \
    v64bfloat16 dummy0 = undef_v64bfloat16();                                  \
    dummy0 = insert(dummy0, 0, broadcast_one_to_v32bfloat16());                \
    dummy0 = insert(dummy0, 1, broadcast_one_to_v32bfloat16());                \
    a = insert(a, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v1, 0))); \
    a = insert(a, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v1, 1))); \
    b = insert(b, 0,                                                           \
               to_v32bfloat16(extract_v32accfloat(                             \
                   msc_elem_64(a, dummy0, (v64accfloat)v1), 0)));              \
    b = insert(b, 1,                                                           \
               to_v32bfloat16(extract_v32accfloat(                             \
                   msc_elem_64(a, dummy0, (v64accfloat)v1), 1)));              \
    c = insert(c, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 0))); \
    c = insert(c, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 1))); \
    d = insert(d, 0,                                                           \
               to_v32bfloat16(extract_v32accfloat(                             \
                   msc_elem_64(c, dummy0, (v64accfloat)v2), 0)));              \
    d = insert(d, 1,                                                           \
               to_v32bfloat16(extract_v32accfloat(                             \
                   msc_elem_64(c, dummy0, (v64accfloat)v2), 1)));              \
    return REDUCTION##_elem_64_cmplx_conf(                                     \
        a, c, acc,                                                             \
        mac_elem_64_cmplx_conf(                                                \
            a, d, mul_elem_64_cmplx_conf(b, c, sub_mask, sub_mul), 0,          \
            sub_mask, sub_mul, 0),                                             \
        zero_acc, sub_mask, sub_mul, sub_acc1, ACC_NEG2);                      \
  }                                                                            \
  INTRINSIC(v64accfloat)                                                       \
  TYPE##_elem_64_accuracy_fast_inner(                                          \
      v64float v1, v64float v2, v64accfloat acc, int zero_acc, int sub_mask,   \
      int sub_mul, int sub_acc1) {                                             \
    v64bfloat16 a = undef_v64bfloat16();                                       \
    v64bfloat16 b = undef_v64bfloat16();                                       \
    v64bfloat16 c = undef_v64bfloat16();                                       \
    v64bfloat16 d = undef_v64bfloat16();                                       \
    v64bfloat16 e = undef_v64bfloat16();                                       \
    v64bfloat16 f = undef_v64bfloat16();                                       \
    v64bfloat16 dummy0 = undef_v64bfloat16();                                  \
    dummy0 = insert(dummy0, 0, broadcast_one_to_v32bfloat16());                \
    dummy0 = insert(dummy0, 1, broadcast_one_to_v32bfloat16());                \
    a = insert(a, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v1, 0))); \
    a = insert(a, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v1, 1))); \
    v64accfloat acc0 = msc_elem_64(a, dummy0, (v64accfloat)v1);                \
    b = insert(b, 0, to_v32bfloat16(extract_v32accfloat(acc0, 0)));            \
    b = insert(b, 1, to_v32bfloat16(extract_v32accfloat(acc0, 1)));            \
    c = insert(                                                                \
        c, 0,                                                                  \
        to_v32bfloat16(extract_v32accfloat(msc_elem_64(b, dummy0, acc0), 0))); \
    c = insert(                                                                \
        c, 1,                                                                  \
        to_v32bfloat16(extract_v32accfloat(msc_elem_64(b, dummy0, acc0), 1))); \
    d = insert(d, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 0))); \
    d = insert(d, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 1))); \
    v64accfloat acc1 = msc_elem_64(d, dummy0, (v64accfloat)v2);                \
    e = insert(e, 0, to_v32bfloat16(extract_v32accfloat(acc1, 0)));            \
    e = insert(e, 1, to_v32bfloat16(extract_v32accfloat(acc1, 1)));            \
    f = insert(                                                                \
        f, 0,                                                                  \
        to_v32bfloat16(extract_v32accfloat(msc_elem_64(e, dummy0, acc1), 0))); \
    f = insert(                                                                \
        f, 1,                                                                  \
        to_v32bfloat16(extract_v32accfloat(msc_elem_64(e, dummy0, acc1), 1))); \
    return REDUCTION##_elem_64_cmplx_conf(                                     \
        a, d, acc,                                                             \
        mac_elem_64_cmplx_conf(                                                \
            a, e,                                                              \
            mac_elem_64_cmplx_conf(                                            \
                b, d,                                                          \
                mac_elem_64_cmplx_conf(                                        \
                    d, c,                                                      \
                    mac_elem_64_cmplx_conf(                                    \
                        b, e, mul_elem_64_cmplx_conf(a, f, sub_mask, sub_mul), \
                        0, sub_mask, sub_mul, 0),                              \
                    0, sub_mask, sub_mul, 0),                                  \
                0, sub_mask, sub_mul, 0),                                      \
            0, sub_mask, sub_mul, 0),                                          \
        zero_acc, sub_mask, sub_mul, sub_acc1, ACC_NEG2);                      \
  }                                                                            \
  INTRINSIC(v64accfloat)                                                       \
  TYPE##_elem_64_accuracy_safe_inner(                                          \
      v64float v1, v64float v2, v64accfloat acc, int zero_acc, int sub_mask,   \
      int sub_mul, int sub_acc1) {                                             \
    v64bfloat16 a = undef_v64bfloat16();                                       \
    v64bfloat16 b = undef_v64bfloat16();                                       \
    v64bfloat16 c = undef_v64bfloat16();                                       \
    v64bfloat16 d = undef_v64bfloat16();                                       \
    v64bfloat16 e = undef_v64bfloat16();                                       \
    v64bfloat16 f = undef_v64bfloat16();                                       \
    v64bfloat16 dummy0 = undef_v64bfloat16();                                  \
    dummy0 = insert(dummy0, 0, broadcast_one_to_v32bfloat16());                \
    dummy0 = insert(dummy0, 1, broadcast_one_to_v32bfloat16());                \
    a = insert(a, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v1, 0))); \
    a = insert(a, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v1, 1))); \
    v64accfloat acc0 = msc_elem_64(a, dummy0, (v64accfloat)v1);                \
    b = insert(b, 0, to_v32bfloat16(extract_v32accfloat(acc0, 0)));            \
    b = insert(b, 1, to_v32bfloat16(extract_v32accfloat(acc0, 1)));            \
    c = insert(                                                                \
        c, 0,                                                                  \
        to_v32bfloat16(extract_v32accfloat(msc_elem_64(b, dummy0, acc0), 0))); \
    c = insert(                                                                \
        c, 1,                                                                  \
        to_v32bfloat16(extract_v32accfloat(msc_elem_64(b, dummy0, acc0), 1))); \
    d = insert(d, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 0))); \
    d = insert(d, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 1))); \
    v64accfloat acc1 = msc_elem_64(d, dummy0, (v64accfloat)v2);                \
    e = insert(e, 0, to_v32bfloat16(extract_v32accfloat(acc1, 0)));            \
    e = insert(e, 1, to_v32bfloat16(extract_v32accfloat(acc1, 1)));            \
    f = insert(                                                                \
        f, 0,                                                                  \
        to_v32bfloat16(extract_v32accfloat(msc_elem_64(e, dummy0, acc1), 0))); \
    f = insert(                                                                \
        f, 1,                                                                  \
        to_v32bfloat16(extract_v32accfloat(msc_elem_64(e, dummy0, acc1), 1))); \
    v64accfloat __aie_register_keep() tmp =                                    \
        mul_elem_64_cmplx_conf(c, f, sub_mask, sub_mul);                       \
    tmp += mul_elem_64_cmplx_conf(c, e, sub_mask, sub_mul);                    \
    tmp += mul_elem_64_cmplx_conf(b, f, sub_mask, sub_mul);                    \
    tmp += mul_elem_64_cmplx_conf(a, f, sub_mask, sub_mul);                    \
    tmp += mul_elem_64_cmplx_conf(b, e, sub_mask, sub_mul);                    \
    tmp += mul_elem_64_cmplx_conf(d, c, sub_mask, sub_mul);                    \
    tmp += mul_elem_64_cmplx_conf(b, d, sub_mask, sub_mul);                    \
    tmp += mul_elem_64_cmplx_conf(a, e, sub_mask, sub_mul);                    \
    tmp += mul_elem_64_cmplx_conf(a, d, sub_mask, sub_mul);                    \
    return ADDSUB##_conf(acc, tmp, zero_acc, sub_acc1, 0);                     \
  }

_TEMPLATE_MAC_MSC_ELEM(mac, addmac, add, 0)
_TEMPLATE_MAC_MSC_ELEM(msc, addmsc, sub, 1)

#undef _TEMPLATE_MAC_MSC_ELEM

#define _TEMPLATE_MAC_MSC_MATMUL(TYPE, REDUCTION, ADDSUB, ACC_NEG2)            \
  INTRINSIC(v32accfloat)                                                       \
  TYPE##_4x8_8x8_accuracy_low_inner(v32float v1, v64float v2, v32accfloat acc, \
                                    int zero_acc, int sub_mask, int sub_mul,   \
                                    int sub_acc1) {                            \
    v32bfloat16 v32dummy0 = broadcast_one_to_v32bfloat16();                    \
    v32bfloat16 a = to_v32bfloat16((v32accfloat)v1);                           \
    v32bfloat16 b =                                                            \
        to_v32bfloat16(msc_elem_32(a, v32dummy0, (v32accfloat)v1));            \
    v64bfloat16 c = undef_v64bfloat16();                                       \
    v64bfloat16 d = undef_v64bfloat16();                                       \
    v64bfloat16 v64dummy0 = undef_v64bfloat16();                               \
    v64dummy0 = insert(v64dummy0, 0, broadcast_one_to_v32bfloat16());          \
    v64dummy0 = insert(v64dummy0, 1, broadcast_one_to_v32bfloat16());          \
    c = insert(c, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 0))); \
    c = insert(c, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 1))); \
    d = insert(d, 0,                                                           \
               to_v32bfloat16(extract_v32accfloat(                             \
                   msc_elem_64(c, v64dummy0, (v64accfloat)v2), 0)));           \
    d = insert(d, 1,                                                           \
               to_v32bfloat16(extract_v32accfloat(                             \
                   msc_elem_64(c, v64dummy0, (v64accfloat)v2), 1)));           \
    return REDUCTION##_4x8_8x8_conf(                                           \
        a, c, acc,                                                             \
        mac_4x8_8x8_conf(a, d, mul_4x8_8x8_conf(b, c, sub_mul), 0, sub_mul,    \
                         0),                                                   \
        zero_acc, sub_mul, sub_acc1, ACC_NEG2);                                \
  }                                                                            \
  INTRINSIC(v32accfloat)                                                       \
  TYPE##_4x8_8x8_accuracy_fast_inner(                                          \
      v32float v1, v64float v2, v32accfloat acc, int zero_acc, int sub_mask,   \
      int sub_mul, int sub_acc1) {                                             \
    v32bfloat16 v32dummy0 = broadcast_one_to_v32bfloat16();                    \
    v32bfloat16 a = to_v32bfloat16((v32accfloat)v1);                           \
    v32accfloat acc0 = msc_elem_32(a, v32dummy0, (v32accfloat)v1);             \
    v32bfloat16 b = to_v32bfloat16(acc0);                                      \
    v32bfloat16 c = to_v32bfloat16(msc_elem_32(b, v32dummy0, acc0));           \
    v64bfloat16 v64dummy0 = undef_v64bfloat16();                               \
    v64bfloat16 d = undef_v64bfloat16();                                       \
    v64bfloat16 e = undef_v64bfloat16();                                       \
    v64bfloat16 f = undef_v64bfloat16();                                       \
    v64dummy0 = insert(v64dummy0, 0, broadcast_one_to_v32bfloat16());          \
    v64dummy0 = insert(v64dummy0, 1, broadcast_one_to_v32bfloat16());          \
    d = insert(d, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 0))); \
    d = insert(d, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 1))); \
    v64accfloat acc1 = msc_elem_64(d, v64dummy0, (v64accfloat)v2);             \
    e = insert(e, 0, to_v32bfloat16(extract_v32accfloat(acc1, 0)));            \
    e = insert(e, 1, to_v32bfloat16(extract_v32accfloat(acc1, 1)));            \
    f = insert(f, 0,                                                           \
               to_v32bfloat16(                                                 \
                   extract_v32accfloat(msc_elem_64(e, v64dummy0, acc1), 0)));  \
    f = insert(f, 1,                                                           \
               to_v32bfloat16(                                                 \
                   extract_v32accfloat(msc_elem_64(e, v64dummy0, acc1), 1)));  \
    return REDUCTION##_4x8_8x8_conf(                                           \
        a, d, acc,                                                             \
        mac_4x8_8x8_conf(                                                      \
            a, e,                                                              \
            mac_4x8_8x8_conf(                                                  \
                b, d,                                                          \
                mac_4x8_8x8_conf(                                              \
                    c, d,                                                      \
                    mac_4x8_8x8_conf(                                          \
                        b, e,                                                  \
                        mac_4x8_8x8_conf(                                      \
                            a, f,                                              \
                            mac_4x8_8x8_conf(                                  \
                                b, f,                                          \
                                mac_4x8_8x8_conf(                              \
                                    c, e, mul_4x8_8x8_conf(c, f, sub_mul), 0,  \
                                    sub_mul, 0),                               \
                                0, sub_mul, 0),                                \
                            0, sub_mul, 0),                                    \
                        0, sub_mul, 0),                                        \
                    0, sub_mul, 0),                                            \
                0, sub_mul, 0),                                                \
            0, sub_mul, 0),                                                    \
        zero_acc, sub_mul, sub_acc1, ACC_NEG2);                                \
  }                                                                            \
  INTRINSIC(v32accfloat)                                                       \
  TYPE##_4x8_8x8_accuracy_safe_inner(                                          \
      v32float v1, v64float v2, v32accfloat acc, int zero_acc, int sub_mask,   \
      int sub_mul, int sub_acc1) {                                             \
    v32bfloat16 v32dummy0 = broadcast_one_to_v32bfloat16();                    \
    v32bfloat16 a = to_v32bfloat16((v32accfloat)v1);                           \
    v32accfloat acc0 = msc_elem_32(a, v32dummy0, (v32accfloat)v1);             \
    v32bfloat16 b = to_v32bfloat16(acc0);                                      \
    v32bfloat16 c = to_v32bfloat16(msc_elem_32(b, v32dummy0, acc0));           \
    v64bfloat16 v64dummy0 = undef_v64bfloat16();                               \
    v64bfloat16 d = undef_v64bfloat16();                                       \
    v64bfloat16 e = undef_v64bfloat16();                                       \
    v64bfloat16 f = undef_v64bfloat16();                                       \
    v64dummy0 = insert(v64dummy0, 0, broadcast_one_to_v32bfloat16());          \
    v64dummy0 = insert(v64dummy0, 1, broadcast_one_to_v32bfloat16());          \
    d = insert(d, 0, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 0))); \
    d = insert(d, 1, to_v32bfloat16(extract_v32accfloat((v64accfloat)v2, 1))); \
    v64accfloat acc1 = msc_elem_64(d, v64dummy0, (v64accfloat)v2);             \
    e = insert(e, 0, to_v32bfloat16(extract_v32accfloat(acc1, 0)));            \
    e = insert(e, 1, to_v32bfloat16(extract_v32accfloat(acc1, 1)));            \
    f = insert(f, 0,                                                           \
               to_v32bfloat16(                                                 \
                   extract_v32accfloat(msc_elem_64(e, v64dummy0, acc1), 0)));  \
    f = insert(f, 1,                                                           \
               to_v32bfloat16(                                                 \
                   extract_v32accfloat(msc_elem_64(e, v64dummy0, acc1), 1)));  \
    v32accfloat __aie_register_keep() tmp = mul_4x8_8x8_conf(c, f, sub_mul);   \
    tmp += mul_4x8_8x8_conf(c, e, sub_mul);                                    \
    tmp += mul_4x8_8x8_conf(b, f, sub_mul);                                    \
    tmp += mul_4x8_8x8_conf(a, f, sub_mul);                                    \
    tmp += mul_4x8_8x8_conf(b, e, sub_mul);                                    \
    tmp += mul_4x8_8x8_conf(c, d, sub_mul);                                    \
    tmp += mul_4x8_8x8_conf(b, d, sub_mul);                                    \
    tmp += mul_4x8_8x8_conf(a, e, sub_mul);                                    \
    tmp += mul_4x8_8x8_conf(a, d, sub_mul);                                    \
    return ADDSUB##_conf(acc, tmp, zero_acc, sub_acc1, 0);                     \
  }

_TEMPLATE_MAC_MSC_MATMUL(mac, addmac, add, 0)
_TEMPLATE_MAC_MSC_MATMUL(msc, addmsc, sub, 1)

#undef _TEMPLATE_MAC_MSC_MATMUL

#define _MUL_MAC_TYPE(RET, DATAX, DATAY, MODE, ACCURACY)                       \
  INTRINSIC(RET) mul_##MODE##_accuracy_##ACCURACY(DATAX v1, DATAY v2) {        \
    RET o = mul_##MODE##_accuracy_##ACCURACY##_inner(v1, v2, 0, 0);            \
    return o;                                                                  \
  }                                                                            \
  INTRINSIC(RET)                                                               \
  mul_##MODE##_accuracy_##ACCURACY(DATAX v1, DATAY v2, int sub_mul) {          \
    RET o = mul_##MODE##_accuracy_##ACCURACY##_inner(v1, v2, 0, sub_mul);      \
    return o;                                                                  \
  }                                                                            \
  INTRINSIC(RET)                                                               \
  mac_##MODE##_accuracy_##ACCURACY(DATAX v1, DATAY v2, RET acc, int zero_acc,  \
                                   int sub_mul, int sub_acc1) {                \
    RET o = mac_##MODE##_accuracy_##ACCURACY##_inner(v1, v2, acc, zero_acc, 0, \
                                                     sub_mul, sub_acc1);       \
    return o;                                                                  \
  }                                                                            \
  INTRINSIC(RET)                                                               \
  msc_##MODE##_accuracy_##ACCURACY(DATAX v1, DATAY v2, RET acc, int zero_acc,  \
                                   int sub_mul, int sub_acc1) {                \
    RET o = msc_##MODE##_accuracy_##ACCURACY##_inner(v1, v2, acc, zero_acc, 0, \
                                                     sub_mul, sub_acc1);       \
    return o;                                                                  \
  }

#if defined(AIE_FP32_EMULATION_SET_RND_MODE)

#undef _MUL_MAC_TYPE
#define _MUL_MAC_TYPE(RET, DATAX, DATAY, MODE, TYPE)                           \
  INTRINSIC(RET) mul_##MODE##_accuracy_##ACCURACY(DATAX v1, DATAY v2) {        \
    int rnd = get_rnd();                                                       \
    set_rnd(0);                                                                \
    RET o = mul_##MODE##_accuracy_##ACCURACY##_inner(v1, v2, 0, 0);            \
    set_rnd(rnd);                                                              \
    return o;                                                                  \
  }                                                                            \
  INTRINSIC(RET)                                                               \
  mul_##MODE##_accuracy_##ACCURACY(DATAX v1, DATAY v2, int sub_mul) {          \
    int rnd = get_rnd();                                                       \
    set_rnd(0);                                                                \
    RET o = mul_##MODE##_accuracy_##ACCURACY##_inner(v1, v2, 0, sub_mul);      \
    set_rnd(rnd);                                                              \
    return o;                                                                  \
  }                                                                            \
  INTRINSIC(RET)                                                               \
  mac_##MODE##_accuracy_##ACCURACY(DATAX v1, DATAY v2, RET acc, int zero_acc,  \
                                   int sub_mul, int sub_acc1) {                \
    int rnd = get_rnd();                                                       \
    set_rnd(0);                                                                \
    RET o = mac_##MODE##_accuracy_##ACCURACY##_inner(v1, v2, acc, zero_acc, 0, \
                                                     sub_mul, sub_acc1);       \
    set_rnd(rnd);                                                              \
    return o;                                                                  \
  }                                                                            \
  INTRINSIC(RET)                                                               \
  msc_##MODE##_accuracy_##ACCURACY(DATAX v1, DATAY v2, RET acc, int zero_acc,  \
                                   int sub_mul, int sub_acc1) {                \
    int rnd = get_rnd();                                                       \
    set_rnd(0);                                                                \
    RET o = msc_##MODE##_accuracy_##ACCURACY##_inner(v1, v2, acc, zero_acc, 0, \
                                                     sub_mul, sub_acc1);       \
    set_rnd(rnd);                                                              \
    return o;                                                                  \
  }

#endif

#define _INSTANTIATE_ACCURACY(ACCURACY)                                        \
  _MUL_MAC_TYPE(v32accfloat, v32float, v32float, elem_32, ACCURACY)            \
  _MUL_MAC_TYPE(v64accfloat, v64float, v64float, elem_64, ACCURACY)            \
  _MUL_MAC_TYPE(v32accfloat, v32float, v64float, 4x8_8x8, ACCURACY)

_INSTANTIATE_ACCURACY(low)
_INSTANTIATE_ACCURACY(safe)
_INSTANTIATE_ACCURACY(fast)

#undef _MUL_MAC_TYPE
#undef _INSTANTIATE_ACCURACY

#define _MAC_FLOAT_F(RET, DATAX, DATAY, MODE, ACCURACY)                        \
  INTRINSIC(RET) mul_##MODE(DATAX v1, DATAY v2) {                              \
    return mul_##MODE##_accuracy_##ACCURACY(v1, v2, 0);                        \
  }                                                                            \
  INTRINSIC(RET) negmul_##MODE(DATAX v1, DATAY v2) {                           \
    return neg(mul_##MODE##_accuracy_##ACCURACY(v1, v2, 0));                   \
  }                                                                            \
  INTRINSIC(RET) mac_##MODE(DATAX v1, DATAY v2, RET acc) {                     \
    return mac_##MODE##_accuracy_##ACCURACY(v1, v2, acc, 0, 0, 0);             \
  }                                                                            \
  INTRINSIC(RET) msc_##MODE(DATAX v1, DATAY v2, RET acc) {                     \
    return msc_##MODE##_accuracy_##ACCURACY(v1, v2, acc, 0, 0, 0);             \
  }                                                                            \
  INTRINSIC(RET) addmac_##MODE(DATAX v1, DATAY v2, RET acc1, RET acc2) {       \
    return add(mac_##MODE##_accuracy_##ACCURACY(v1, v2, acc1, 0, 0, 0), acc2); \
  }                                                                            \
  INTRINSIC(RET) addmsc_##MODE(DATAX v1, DATAY v2, RET acc1, RET acc2) {       \
    return add(msc_##MODE##_accuracy_##ACCURACY(v1, v2, acc2, 0, 0, 0), acc1); \
  }                                                                            \
  INTRINSIC(RET) mul_##MODE##_conf(DATAX v1, DATAY v2, int sub_mul) {          \
    return mul_##MODE##_accuracy_##ACCURACY(v1, v2, sub_mul);                  \
  }                                                                            \
  INTRINSIC(RET)                                                               \
  mac_##MODE##_conf(DATAX v1, DATAY v2, RET acc, int zero_acc, int sub_mul,    \
                    int sub_acc1) {                                            \
    return mac_##MODE##_accuracy_##ACCURACY(v1, v2, acc, zero_acc, sub_mul,    \
                                            sub_acc1);                         \
  }                                                                            \
  INTRINSIC(RET)                                                               \
  msc_##MODE##_conf(DATAX v1, DATAY v2, RET acc, int zero_acc, int sub_mul,    \
                    int sub_acc1) {                                            \
    return msc_##MODE##_accuracy_##ACCURACY(v1, v2, acc, zero_acc, sub_mul,    \
                                            sub_acc1);                         \
  }                                                                            \
  INTRINSIC(RET)                                                               \
  addmac_##MODE##_conf(DATAX v1, DATAY v2, RET acc1, RET acc2, int zero_acc,   \
                       int sub_mul, int sub_acc1, int sub_acc2) {              \
    return add_conf(mac_##MODE##_accuracy_##ACCURACY(v1, v2, acc1, zero_acc,   \
                                                     sub_mul, sub_acc1),       \
                    acc2, 0, 0, sub_acc2);                                     \
  }                                                                            \
  INTRINSIC(RET)                                                               \
  addmsc_##MODE##_conf(DATAX v1, DATAY v2, RET acc1, RET acc2, int zero_acc,   \
                       int sub_mul, int sub_acc1, int sub_acc2) {              \
    return add_conf(msc_##MODE##_accuracy_##ACCURACY(v1, v2, acc2, zero_acc,   \
                                                     sub_mul, sub_acc2),       \
                    acc1, 0, 0, sub_acc1);                                     \
  }

#define _ALL_OPS_FLOAT_F(ACCURACY)                                             \
  _MAC_FLOAT_F(v32accfloat, v32float, v32float, elem_32, ACCURACY)             \
  _MAC_FLOAT_F(v64accfloat, v64float, v64float, elem_64, ACCURACY)             \
  _MAC_FLOAT_F(v32accfloat, v32float, v64float, 4x8_8x8, ACCURACY)

#if defined(AIE_FP32_EMULATION_ACCURACY_LOW)
_ALL_OPS_FLOAT_F(low)
#elif defined(AIE_FP32_EMULATION_ACCURACY_FAST)
_ALL_OPS_FLOAT_F(fast)
#else
_ALL_OPS_FLOAT_F(safe)
#endif
#undef _MAC_FLOAT_F
#undef _MAC_FLOAT_CONF_F
#undef _ALL_OPS_FLOAT_F

#endif //__AIE2PS_VMULT_FLOAT_EMULATED_H__
