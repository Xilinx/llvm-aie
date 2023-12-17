//===- aiev2_core.h ---------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV2_CORE_H
#define __AIEV2_CORE_H
#ifdef mul_elem_16
#undef mul_elem_16
#endif

#ifdef mac_elem_16
#undef mac_elem_16
#endif

#ifdef msc_elem_16
#undef msc_elem_16
#endif

#ifdef negmul_elem_16
#undef negmul_elem_16
#endif

#if defined(AIE2_FP32_EMULATION_SET_RND_MODE)
inline v16accfloat mul_elem_16_accuracy_low(v16float v1, v16float v2) {
  int rnd = get_rnd();
  set_rnd(0);

  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  v32bfloat16 c = broadcast_zero_to_v32bfloat16();
  v32bfloat16 d = broadcast_zero_to_v32bfloat16();
  v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();
  a = insert(a, 0, to_v16bfloat16((v16accfloat)v1));
  b = insert(b, 0, to_v16bfloat16(msc_elem_16_2(a, dummy0, (v16accfloat)v1)));
  c = insert(c, 0, to_v16bfloat16((v16accfloat)v2));
  d = insert(d, 0, to_v16bfloat16(msc_elem_16_2(c, dummy0, (v16accfloat)v2)));
  set_rnd(rnd);
  return mac_elem_16_2(a, c, mac_elem_16_2(a, d, mul_elem_16_2(b, c)));
}
#if 0
inline v8caccfloat mul_elem_8_accuracy_low(v8float v1, v8cfloat v2)
{
	v16float w3 = shuffle(set_v16float(0, v1), set_v16float(0, v1),T32_2x16_lo);
	v16float v22 = (v16float)v2;
        v16accfloat v3 = mul_elem_16_accuracy_low(w3,v22);
	return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mul_elem_8_accuracy_low(v8cfloat v1, v8float v2)
{
	v16float w3 = shuffle(set_v16float(0, v2), set_v16float(0, v2), T32_2x16_lo);
        v16float v11 = (v16float)v1;
        v16accfloat v3 = mul_elem_16_accuracy_low(v11,w3);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mul_elem_8_accuracy_low(v8cfloat v1, v8cfloat v2)
{

	v16float v2_real = shuffle((v16float)v2, T32_16x2_lo);
        v16float v2_imag = shuffle((v16float)v2, T32_16x2_hi);
        v16float v2_flip = shuffle(v2_imag, v2_real, T32_2x16_lo);

        v16accfloat x = mul_elem_16_accuracy_low((v16float)v1,(v16float)v2);
        v16accfloat y = mul_elem_16_accuracy_low((v16float)v1,(v16float)v2_flip);
        v16float x_shifted = shiftl_elem((v16float)x,0);
        v16float y_shifted = shiftr_elem((v16float)y,0);
        v16accfloat real_vec = sub(x,(v16accfloat)x_shifted);
        v16accfloat imag_vec = add(y,(v16accfloat)y_shifted);

        return (v8caccfloat)sel((v16float)real_vec, (v16float)imag_vec, 0xaaaaaaaa);
}
#endif

inline v16accfloat mac_elem_16_accuracy_low(v16float v1, v16float v2,
                                            v16accfloat acc) {
  int rnd = get_rnd();
  set_rnd(0);

  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  v32bfloat16 c = broadcast_zero_to_v32bfloat16();
  v32bfloat16 d = broadcast_zero_to_v32bfloat16();
  v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();
  a = insert(a, 0, to_v16bfloat16((v16accfloat)v1));
  b = insert(b, 0, to_v16bfloat16(msc_elem_16_2(a, dummy0, (v16accfloat)v1)));
  c = insert(c, 0, to_v16bfloat16((v16accfloat)v2));
  d = insert(d, 0, to_v16bfloat16(msc_elem_16_2(c, dummy0, (v16accfloat)v2)));
  set_rnd(rnd);
  return addmac_elem_16_2(a, c, mac_elem_16_2(a, d, mul_elem_16_2(b, c)), acc);
}

#if 0
inline v8caccfloat mac_elem_8_accuracy_low(v8float v1, v8cfloat v2, v8caccfloat acc)
{
	v16float w3 = shuffle(set_v16float(0, v1), set_v16float(0, v1),T32_2x16_lo);
	v16float v22 = (v16float)v2;
        v16accfloat v3 = mac_elem_16_accuracy_low(w3,v22,(v16accfloat)acc);
	return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mac_elem_8_accuracy_low(v8cfloat v1, v8float v2, v8caccfloat acc)
{
	v16float w3 = shuffle(set_v16float(0, v2), set_v16float(0, v2), T32_2x16_lo);
        v16float v11 = (v16float)v1;
        v16accfloat v3 = mac_elem_16_accuracy_low(v11,w3,(v16accfloat)acc);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mac_elem_8_accuracy_low(v8cfloat v1, v8cfloat v2, v8caccfloat acc)
{

	v16float v2_real = shuffle((v16float)v2, T32_16x2_lo);
        v16float v2_imag = shuffle((v16float)v2, T32_16x2_hi);
        v16float v2_flip = shuffle(v2_imag, v2_real, T32_2x16_lo);

        v16accfloat x = mul_elem_16_accuracy_low((v16float)v1,(v16float)v2);
        v16accfloat y = mul_elem_16_accuracy_low((v16float)v1,(v16float)v2_flip);
        v16float x_shifted = shiftl_elem((v16float)x,0);
        v16float y_shifted = shiftr_elem((v16float)y,0);
        v16accfloat real_vec = sub(x,(v16accfloat)x_shifted);
        v16accfloat imag_vec = add(y,(v16accfloat)y_shifted);

        return add(acc, (v8caccfloat)sel((v16float)real_vec, (v16float)imag_vec, 0xaaaaaaaa));
}
#endif

inline v16accfloat mul_elem_16_accuracy_fast(v16float v1, v16float v2) {
  int rnd = get_rnd();
  set_rnd(0);

  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  v32bfloat16 c = broadcast_zero_to_v32bfloat16();
  v32bfloat16 d = broadcast_zero_to_v32bfloat16();
  v32bfloat16 e = broadcast_zero_to_v32bfloat16();
  v32bfloat16 f = broadcast_zero_to_v32bfloat16();
  v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();
  a = insert(a, 0, to_v16bfloat16((v16accfloat)v1));
  v16accfloat acc0 = msc_elem_16_2(a, dummy0, (v16accfloat)v1);
  b = insert(b, 0, to_v16bfloat16(acc0));
  c = insert(c, 0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
  d = insert(d, 0, to_v16bfloat16((v16accfloat)v2));
  v16accfloat acc1 = msc_elem_16_2(d, dummy0, (v16accfloat)v2);
  e = insert(e, 0, to_v16bfloat16(acc1));
  f = insert(f, 0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
  set_rnd(rnd);
  return mac_elem_16_2(
      a, d,
      mac_elem_16_2(
          a, e,
          mac_elem_16_2(
              b, d,
              mac_elem_16_2(d, c, mac_elem_16_2(b, e, mul_elem_16_2(a, f))))));
}

#if 0
inline v8caccfloat mul_elem_8_accuracy_fast(v8float v1, v8cfloat v2)
{
	v16float w3 = shuffle(set_v16float(0, v1), set_v16float(0, v1),T32_2x16_lo);
        v16float v22 = (v16float)v2;
        v16accfloat v3 = mul_elem_16_accuracy_fast(w3,v22);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mul_elem_8_accuracy_fast(v8cfloat v1, v8float v2)
{
	v16float w3 = shuffle(set_v16float(0, v2), set_v16float(0, v2), T32_2x16_lo);
        v16float v11 = (v16float)v1;
        v16accfloat v3 = mul_elem_16_accuracy_fast(v11,w3);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mul_elem_8_accuracy_fast(v8cfloat v1, v8cfloat v2)
{

	v16float v2_real = shuffle((v16float)v2, T32_16x2_lo);
        v16float v2_imag = shuffle((v16float)v2, T32_16x2_hi);
        v16float v2_flip = shuffle(v2_imag, v2_real, T32_2x16_lo);

	v16accfloat x = mul_elem_16_accuracy_fast((v16float)v1,(v16float)v2);
        v16accfloat y = mul_elem_16_accuracy_fast((v16float)v1,(v16float)v2_flip);
        v16float x_shifted = shiftl_elem((v16float)x,0);
        v16float y_shifted = shiftr_elem((v16float)y,0);
        v16accfloat real_vec = sub(x,(v16accfloat)x_shifted);
        v16accfloat imag_vec = add(y,(v16accfloat)y_shifted);

        return (v8caccfloat)sel((v16float)real_vec, (v16float)imag_vec, 0xaaaaaaaa);
}

#endif

inline v16accfloat mac_elem_16_accuracy_fast(v16float v1, v16float v2,
                                             v16accfloat acc) {
  int rnd = get_rnd();
  set_rnd(0);

  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  v32bfloat16 c = broadcast_zero_to_v32bfloat16();
  v32bfloat16 d = broadcast_zero_to_v32bfloat16();
  v32bfloat16 e = broadcast_zero_to_v32bfloat16();
  v32bfloat16 f = broadcast_zero_to_v32bfloat16();
  v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();
  a = insert(a, 0, to_v16bfloat16((v16accfloat)v1));
  v16accfloat acc0 = msc_elem_16_2(a, dummy0, (v16accfloat)v1);
  b = insert(b, 0, to_v16bfloat16(acc0));
  c = insert(c, 0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
  d = insert(d, 0, to_v16bfloat16((v16accfloat)v2));
  v16accfloat acc1 = msc_elem_16_2(d, dummy0, (v16accfloat)v2);
  e = insert(e, 0, to_v16bfloat16(acc1));
  f = insert(f, 0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
  set_rnd(rnd);
  return addmac_elem_16_2(
      a, d,
      mac_elem_16_2(
          a, e,
          mac_elem_16_2(
              b, d,
              mac_elem_16_2(d, c, mac_elem_16_2(b, e, mul_elem_16_2(a, f))))),
      acc);
}

#if 0
inline v8caccfloat mac_elem_8_accuracy_fast(v8float v1, v8cfloat v2, v8caccfloat acc)
{
	v16float w3 = shuffle(set_v16float(0, v1), set_v16float(0, v1),T32_2x16_lo);
	v16float v22 = (v16float)v2;
        v16accfloat v3 = mac_elem_16_accuracy_fast(w3,v22,(v16accfloat)acc);
	return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mac_elem_8_accuracy_fast(v8cfloat v1, v8float v2, v8caccfloat acc)
{
	v16float w3 = shuffle(set_v16float(0, v2), set_v16float(0, v2), T32_2x16_lo);
        v16float v11 = (v16float)v1;
        v16accfloat v3 = mac_elem_16_accuracy_fast(v11,w3,(v16accfloat)acc);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mac_elem_8_accuracy_fast(v8cfloat v1, v8cfloat v2, v8caccfloat acc)
{

	v16float v2_real = shuffle((v16float)v2, T32_16x2_lo);
        v16float v2_imag = shuffle((v16float)v2, T32_16x2_hi);
        v16float v2_flip = shuffle(v2_imag, v2_real, T32_2x16_lo);

        v16accfloat x = mul_elem_16_accuracy_fast((v16float)v1,(v16float)v2);
        v16accfloat y = mul_elem_16_accuracy_fast((v16float)v1,(v16float)v2_flip);
        v16float x_shifted = shiftl_elem((v16float)x,0);
        v16float y_shifted = shiftr_elem((v16float)y,0);
        v16accfloat real_vec = sub(x,(v16accfloat)x_shifted);
        v16accfloat imag_vec = add(y,(v16accfloat)y_shifted);


        return add(acc, (v8caccfloat)sel((v16float)real_vec, (v16float)imag_vec, 0xaaaaaaaa));
}
#endif

inline v16accfloat mul_elem_16_accuracy_safe(v16float v1, v16float v2) {
  int rnd = get_rnd();
  set_rnd(0);

  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  v32bfloat16 c = broadcast_zero_to_v32bfloat16();
  v32bfloat16 d = broadcast_zero_to_v32bfloat16();
  v32bfloat16 e = broadcast_zero_to_v32bfloat16();
  v32bfloat16 f = broadcast_zero_to_v32bfloat16();
  v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();
  a = insert(a, 0, to_v16bfloat16((v16accfloat)v1));
  v16accfloat acc0 = msc_elem_16_2(a, dummy0, (v16accfloat)v1);
  b = insert(b, 0, to_v16bfloat16(acc0));
  c = insert(c, 0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
  d = insert(d, 0, to_v16bfloat16((v16accfloat)v2));
  v16accfloat acc1 = msc_elem_16_2(d, dummy0, (v16accfloat)v2);
  e = insert(e, 0, to_v16bfloat16(acc1));
  f = insert(f, 0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
  set_rnd(rnd);
  return mac_elem_16_2(
      a, d,
      mac_elem_16_2(
          a, e,
          mac_elem_16_2(
              b, d,
              mac_elem_16_2(
                  d, c,
                  mac_elem_16_2(
                      b, e,
                      mac_elem_16_2(
                          a, f,
                          mac_elem_16_2(
                              b, f,
                              mac_elem_16_2(c, e, mul_elem_16_2(c, f)))))))));
}

#if 0
inline v8caccfloat mul_elem_8_accuracy_safe(v8float v1, v8cfloat v2)
{
        v16float w3 = shuffle(set_v16float(0, v1), set_v16float(0, v1),T32_2x16_lo);
	v16float v22 = (v16float)v2;
        v16accfloat v3 = mul_elem_16_accuracy_safe(w3,v22);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mul_elem_8_accuracy_safe(v8cfloat v1, v8float v2)
{
	v16float w3 = shuffle(set_v16float(0, v2), set_v16float(0, v2), T32_2x16_lo);
        v16float v11 = (v16float)v1;
        v16accfloat v3 = mul_elem_16_accuracy_safe(v11,w3);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mul_elem_8_accuracy_safe(v8cfloat v1, v8cfloat v2)
{
	v16float v2_real = shuffle((v16float)v2, T32_16x2_lo);
        v16float v2_imag = shuffle((v16float)v2, T32_16x2_hi);
        v16float v2_flip = shuffle(v2_imag, v2_real, T32_2x16_lo);

        v16accfloat x = mul_elem_16_accuracy_safe((v16float)v1,(v16float)v2);
        v16accfloat y = mul_elem_16_accuracy_safe((v16float)v1,(v16float)v2_flip);
        v16float x_shifted = shiftl_elem((v16float)x,0);
        v16float y_shifted = shiftr_elem((v16float)y,0);
        v16accfloat real_vec = sub(x,(v16accfloat)x_shifted);
        v16accfloat imag_vec = add(y,(v16accfloat)y_shifted);


        return (v8caccfloat)sel((v16float)real_vec, (v16float)imag_vec, 0xaaaaaaaa);
}
#endif

inline v16accfloat mac_elem_16_accuracy_safe(v16float v1, v16float v2,
                                             v16accfloat acc) {
  int rnd = get_rnd();
  set_rnd(0);

  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  v32bfloat16 c = broadcast_zero_to_v32bfloat16();
  v32bfloat16 d = broadcast_zero_to_v32bfloat16();
  v32bfloat16 e = broadcast_zero_to_v32bfloat16();
  v32bfloat16 f = broadcast_zero_to_v32bfloat16();
  v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();
  a = insert(a, 0, to_v16bfloat16((v16accfloat)v1));
  v16accfloat acc0 = msc_elem_16_2(a, dummy0, (v16accfloat)v1);
  b = insert(b, 0, to_v16bfloat16(acc0));
  c = insert(c, 0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
  d = insert(d, 0, to_v16bfloat16((v16accfloat)v2));
  v16accfloat acc1 = msc_elem_16_2(d, dummy0, (v16accfloat)v2);
  e = insert(e, 0, to_v16bfloat16(acc1));
  f = insert(f, 0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
  set_rnd(rnd);
  return addmac_elem_16_2(
      a, d,
      mac_elem_16_2(
          a, e,
          mac_elem_16_2(
              b, d,
              mac_elem_16_2(
                  d, c,
                  mac_elem_16_2(
                      b, e,
                      mac_elem_16_2(
                          a, f,
                          mac_elem_16_2(
                              b, f,
                              mac_elem_16_2(c, e, mul_elem_16_2(c, f)))))))),
      acc);
}

#if 0
inline v8caccfloat mac_elem_8_accuracy_safe(v8float v1, v8cfloat v2, v8caccfloat acc)
{
	v16float w3 = shuffle(set_v16float(0, v1), set_v16float(0, v1),T32_2x16_lo);
	v16float v22 = (v16float)v2;
        v16accfloat v3 = mac_elem_16_accuracy_safe(w3,v22,(v16accfloat)acc);
	return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mac_elem_8_accuracy_safe(v8cfloat v1, v8float v2, v8caccfloat acc)
{
	v16float w3 = shuffle(set_v16float(0, v2), set_v16float(0, v2), T32_2x16_lo);
        v16float v11 = (v16float)v1;
        v16accfloat v3 = mac_elem_16_accuracy_safe(v11,w3,(v16accfloat)acc);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mac_elem_8_accuracy_safe(v8cfloat v1, v8cfloat v2, v8caccfloat acc)
{

	v16float v2_real = shuffle((v16float)v2, T32_16x2_lo);
        v16float v2_imag = shuffle((v16float)v2, T32_16x2_hi);
        v16float v2_flip = shuffle(v2_imag, v2_real, T32_2x16_lo);
        v16accfloat x = mul_elem_16_accuracy_safe((v16float)v1,(v16float)v2);
        v16accfloat y = mul_elem_16_accuracy_safe((v16float)v1,(v16float)v2_flip);
        v16float x_shifted = shiftl_elem((v16float)x,0);
        v16float y_shifted = shiftr_elem((v16float)y,0);
        v16accfloat real_vec = sub(x,(v16accfloat)x_shifted);
        v16accfloat imag_vec = add(y,(v16accfloat)y_shifted);


        return add(acc, (v8caccfloat)sel((v16float)real_vec, (v16float)imag_vec, 0xaaaaaaaa));
}

inline v16accfloat mul_4x8_8x4_accuracy_safe(v32float v1, v32float v2)
{
    int rnd = get_rnd();
    set_rnd(0);

    v32bfloat16 a;
    v32bfloat16 b;
    v32bfloat16 c;
    v32bfloat16 d;
    v32bfloat16 e;
    v32bfloat16 f;
    v32bfloat16 dummy0 = ::shuffle(broadcast_one_to_v32bfloat16(), broadcast_zero_to_v32bfloat16(), INTLV_lo_256o512);
    v32bfloat16 dummy1 = ::shuffle(broadcast_zero_to_v32bfloat16(), broadcast_one_to_v32bfloat16(), INTLV_lo_256o512);

    v16accfloat temp0 = (v16accfloat)extract_v16float(v1, 0);
    v16accfloat temp1 = (v16accfloat)extract_v16float(v2, 0);
    v16accfloat temp2 = (v16accfloat)extract_v16float(v1, 1);
    v16accfloat temp3 = (v16accfloat)extract_v16float(v2, 1);

    a = set_v32bfloat16(0, to_v16bfloat16(temp0));
    a = insert(a,       1, to_v16bfloat16(temp2));

    v16accfloat acc0 = msc_elem_16_2(a, dummy0, temp0);
    v16accfloat acc2 = msc_elem_16_2(a, dummy1, temp2);

    b = set_v32bfloat16(0, to_v16bfloat16(acc0));
    b = insert(b,       1, to_v16bfloat16(acc2));

    c = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
    c = insert(c,       1, to_v16bfloat16(msc_elem_16_2(b, dummy1, acc2)));

    d = set_v32bfloat16(0, to_v16bfloat16(temp1));
    d = insert(d,       1, to_v16bfloat16(temp3));

    v16accfloat acc1 = msc_elem_16_2(d, dummy0, temp1);
    v16accfloat acc3 = msc_elem_16_2(d, dummy1, temp3);

    e = set_v32bfloat16(0, to_v16bfloat16(acc1));
    e = insert(e,       1, to_v16bfloat16(acc3));

    f = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
    f = insert(f,       1, to_v16bfloat16(msc_elem_16_2(e, dummy1, acc3)));
    set_rnd(rnd);
    return mac_4x8_8x4(a,d,mac_4x8_8x4(a,e,mac_4x8_8x4(b,d,mac_4x8_8x4(d,c,mac_4x8_8x4(b,e,mac_4x8_8x4(a,f,mac_4x8_8x4(b,f,mac_4x8_8x4(c,e,mul_4x8_8x4(c,f)))))))));
}
inline v4caccfloat mul_2x8_8x2_accuracy_safe(v16float v1, v16cfloat v2)
{
	v32float    w3    = concat(v1,undef_v16float());
	v16accfloat w4    = mul_4x8_8x4_accuracy_safe(w3, (v32float)v2);
	v8float     out   = extract_v8float((v16float)w4,0);
	return (v4caccfloat)out;
}
inline v16accfloat mac_4x8_8x4_accuracy_safe(v32float v1, v32float v2, v16accfloat acc)
{
    int rnd = get_rnd();
    set_rnd(0);

    v32bfloat16 a;
    v32bfloat16 b;
    v32bfloat16 c;
    v32bfloat16 d;
    v32bfloat16 e;
    v32bfloat16 f;
    v32bfloat16 dummy0 = ::shuffle(broadcast_one_to_v32bfloat16(), broadcast_zero_to_v32bfloat16(), INTLV_lo_256o512);
    v32bfloat16 dummy1 = ::shuffle(broadcast_zero_to_v32bfloat16(), broadcast_one_to_v32bfloat16(), INTLV_lo_256o512);

    v16accfloat temp0 = (v16accfloat)extract_v16float(v1, 0);
    v16accfloat temp1 = (v16accfloat)extract_v16float(v2, 0);
    v16accfloat temp2 = (v16accfloat)extract_v16float(v1, 1);
    v16accfloat temp3 = (v16accfloat)extract_v16float(v2, 1);

    a = set_v32bfloat16(0, to_v16bfloat16(temp0));
    a = insert(a,       1, to_v16bfloat16(temp2));

    v16accfloat acc0 = msc_elem_16_2(a, dummy0, temp0);
    v16accfloat acc2 = msc_elem_16_2(a, dummy1, temp2);

    b = set_v32bfloat16(0, to_v16bfloat16(acc0));
    b = insert(b,       1, to_v16bfloat16(acc2));

    c = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
    c = insert(c,       1, to_v16bfloat16(msc_elem_16_2(b, dummy1, acc2)));

    d = set_v32bfloat16(0, to_v16bfloat16(temp1));
    d = insert(d,       1, to_v16bfloat16(temp3));

    v16accfloat acc1 = msc_elem_16_2(d, dummy0, temp1);
    v16accfloat acc3 = msc_elem_16_2(d, dummy1, temp3);

    e = set_v32bfloat16(0, to_v16bfloat16(acc1));
    e = insert(e,       1, to_v16bfloat16(acc3));

    f = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
    f = insert(f,       1, to_v16bfloat16(msc_elem_16_2(e, dummy1, acc3)));
    set_rnd(rnd);
    return addmac_4x8_8x4(a,d,mac_4x8_8x4(a,e,mac_4x8_8x4(b,d,mac_4x8_8x4(d,c,mac_4x8_8x4(b,e,mac_4x8_8x4(a,f,mac_4x8_8x4(b,f,mac_4x8_8x4(c,e,mul_4x8_8x4(c,f)))))))),acc);
}
inline v16accfloat mul_4x8_8x4_accuracy_fast(v32float v1, v32float v2)
{
    int rnd = get_rnd();
    set_rnd(0);

    v32bfloat16 a;
    v32bfloat16 b;
    v32bfloat16 c;
    v32bfloat16 d;
    v32bfloat16 e;
    v32bfloat16 f;
    v32bfloat16 dummy0 = ::shuffle(broadcast_one_to_v32bfloat16(), broadcast_zero_to_v32bfloat16(), INTLV_lo_256o512);
    v32bfloat16 dummy1 = ::shuffle(broadcast_zero_to_v32bfloat16(), broadcast_one_to_v32bfloat16(), INTLV_lo_256o512);

    v16accfloat temp0 = (v16accfloat)extract_v16float(v1, 0);
    v16accfloat temp1 = (v16accfloat)extract_v16float(v2, 0);
    v16accfloat temp2 = (v16accfloat)extract_v16float(v1, 1);
    v16accfloat temp3 = (v16accfloat)extract_v16float(v2, 1);

    a = set_v32bfloat16(0, to_v16bfloat16(temp0));
    a = insert(a,       1, to_v16bfloat16(temp2));

    v16accfloat acc0 = msc_elem_16_2(a, dummy0, temp0);
    v16accfloat acc2 = msc_elem_16_2(a, dummy1, temp2);

    b = set_v32bfloat16(0, to_v16bfloat16(acc0));
    b = insert(b,       1, to_v16bfloat16(acc2));

    c = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
    c = insert(c,       1, to_v16bfloat16(msc_elem_16_2(b, dummy1, acc2)));

    d = set_v32bfloat16(0, to_v16bfloat16(temp1));
    d = insert(d,       1, to_v16bfloat16(temp3));

    v16accfloat acc1 = msc_elem_16_2(d, dummy0, temp1);
    v16accfloat acc3 = msc_elem_16_2(d, dummy1, temp3);

    e = set_v32bfloat16(0, to_v16bfloat16(acc1));
    e = insert(e,       1, to_v16bfloat16(acc3));

    f = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
    f = insert(f,       1, to_v16bfloat16(msc_elem_16_2(e, dummy1, acc3)));

    set_rnd(rnd);
    return mac_4x8_8x4(a, d, mac_4x8_8x4(a, e, mac_4x8_8x4(b, d, mac_4x8_8x4(d, c, mac_4x8_8x4(b, e, mul_4x8_8x4(a, f))))));
}
inline v4caccfloat mul_2x8_8x2_accuracy_fast(v16float v1, v16cfloat v2)
{
        v32float    w3    = concat(v1,undef_v16float());
        v16accfloat w4    = mul_4x8_8x4_accuracy_fast(w3, (v32float)v2);
        v8float     out   = extract_v8float((v16float)w4,0);
        return (v4caccfloat)out;
}
inline v16accfloat mac_4x8_8x4_accuracy_fast(v32float v1, v32float v2, v16accfloat acc)
{
    int rnd = get_rnd();
    set_rnd(0);

    v32bfloat16 a;
    v32bfloat16 b;
    v32bfloat16 c;
    v32bfloat16 d;
    v32bfloat16 e;
    v32bfloat16 f;
    v32bfloat16 dummy0 = ::shuffle(broadcast_one_to_v32bfloat16(), broadcast_zero_to_v32bfloat16(), INTLV_lo_256o512);
    v32bfloat16 dummy1 = ::shuffle(broadcast_zero_to_v32bfloat16(), broadcast_one_to_v32bfloat16(), INTLV_lo_256o512);

    v16accfloat temp0 = (v16accfloat)extract_v16float(v1, 0);
    v16accfloat temp1 = (v16accfloat)extract_v16float(v2, 0);
    v16accfloat temp2 = (v16accfloat)extract_v16float(v1, 1);
    v16accfloat temp3 = (v16accfloat)extract_v16float(v2, 1);

    a = set_v32bfloat16(0, to_v16bfloat16(temp0));
    a = insert(a,       1, to_v16bfloat16(temp2));

    v16accfloat acc0 = msc_elem_16_2(a, dummy0, temp0);
    v16accfloat acc2 = msc_elem_16_2(a, dummy1, temp2);

    b = set_v32bfloat16(0, to_v16bfloat16(acc0));
    b = insert(b,       1, to_v16bfloat16(acc2));

    c = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
    c = insert(c,       1, to_v16bfloat16(msc_elem_16_2(b, dummy1, acc2)));

    d = set_v32bfloat16(0, to_v16bfloat16(temp1));
    d = insert(d,       1, to_v16bfloat16(temp3));

    v16accfloat acc1 = msc_elem_16_2(d, dummy0, temp1);
    v16accfloat acc3 = msc_elem_16_2(d, dummy1, temp3);

    e = set_v32bfloat16(0, to_v16bfloat16(acc1));
    e = insert(e,       1, to_v16bfloat16(acc3));

    f = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
    f = insert(f,       1, to_v16bfloat16(msc_elem_16_2(e, dummy1, acc3)));

    set_rnd(rnd);
    return addmac_4x8_8x4(a, d, mac_4x8_8x4(a, e, mac_4x8_8x4(b, d, mac_4x8_8x4(d, c, mac_4x8_8x4(b, e, mul_4x8_8x4(a, f))))),acc);
}
inline v16accfloat mul_4x8_8x4_accuracy_low(v32float v1, v32float v2)
{
    int rnd = get_rnd();
    set_rnd(0);

    v32bfloat16 a;
    v32bfloat16 b;
    v32bfloat16 d;
    v32bfloat16 e;
    v32bfloat16 dummy0 = ::shuffle(broadcast_one_to_v32bfloat16(), broadcast_zero_to_v32bfloat16(), INTLV_lo_256o512);
    v32bfloat16 dummy1 = ::shuffle(broadcast_zero_to_v32bfloat16(), broadcast_one_to_v32bfloat16(), INTLV_lo_256o512);

    v16accfloat temp0 = (v16accfloat)extract_v16float(v1, 0);
    v16accfloat temp1 = (v16accfloat)extract_v16float(v2, 0);
    v16accfloat temp2 = (v16accfloat)extract_v16float(v1, 1);
    v16accfloat temp3 = (v16accfloat)extract_v16float(v2, 1);

    a = set_v32bfloat16(0, to_v16bfloat16(temp0));
    a = insert(a,       1, to_v16bfloat16(temp2));

    v16accfloat acc0 = msc_elem_16_2(a, dummy0, temp0);
    v16accfloat acc2 = msc_elem_16_2(a, dummy1, temp2);

    b = set_v32bfloat16(0, to_v16bfloat16(acc0));
    b = insert(b,       1, to_v16bfloat16(acc2));

    d = set_v32bfloat16(0, to_v16bfloat16(temp1));
    d = insert(d,       1, to_v16bfloat16(temp3));

    v16accfloat acc1 = msc_elem_16_2(d, dummy0, temp1);
    v16accfloat acc3 = msc_elem_16_2(d, dummy1, temp3);

    e = set_v32bfloat16(0, to_v16bfloat16(acc1));
    e = insert(e,       1, to_v16bfloat16(acc3));

    set_rnd(rnd);
    return mac_4x8_8x4(a,d,mac_4x8_8x4(a,e,mul_4x8_8x4(b,d)));
}
inline v4caccfloat mul_2x8_8x2_accuracy_low(v16float v1, v16cfloat v2)
{
        v32float    w3    = concat(v1,undef_v16float());
        v16accfloat w4    = mul_4x8_8x4_accuracy_low(w3, (v32float)v2);
        v8float     out   = extract_v8float((v16float)w4,0);
        return (v4caccfloat)out;
}
inline v16accfloat mac_4x8_8x4_accuracy_low(v32float v1, v32float v2, v16accfloat acc)
{
    int rnd = get_rnd();
    set_rnd(0);

    v32bfloat16 a;
    v32bfloat16 b;
    v32bfloat16 d;
    v32bfloat16 e;
    v32bfloat16 dummy0 = ::shuffle(broadcast_one_to_v32bfloat16(), broadcast_zero_to_v32bfloat16(), INTLV_lo_256o512);
    v32bfloat16 dummy1 = ::shuffle(broadcast_zero_to_v32bfloat16(), broadcast_one_to_v32bfloat16(), INTLV_lo_256o512);

    v16accfloat temp0 = (v16accfloat)extract_v16float(v1, 0);
    v16accfloat temp1 = (v16accfloat)extract_v16float(v2, 0);
    v16accfloat temp2 = (v16accfloat)extract_v16float(v1, 1);
    v16accfloat temp3 = (v16accfloat)extract_v16float(v2, 1);

    a = set_v32bfloat16(0, to_v16bfloat16(temp0));
    a = insert(a,       1, to_v16bfloat16(temp2));

    v16accfloat acc0 = msc_elem_16_2(a, dummy0, temp0);
    v16accfloat acc2 = msc_elem_16_2(a, dummy1, temp2);

    b = set_v32bfloat16(0, to_v16bfloat16(acc0));
    b = insert(b,       1, to_v16bfloat16(acc2));

    d = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
    d = insert(d,       1, to_v16bfloat16(msc_elem_16_2(b, dummy1, acc2)));

    e = set_v32bfloat16(0, to_v16bfloat16(temp1));
    e = insert(e,       1, to_v16bfloat16(temp3));

    set_rnd(rnd);
    return addmac_4x8_8x4(a,d,mac_4x8_8x4(a,e,mul_4x8_8x4(b,d)),acc);
}
#endif

#else // defined(AIE2_FP32_EMULATION_SET_RND_MODE)

inline v16accfloat mul_elem_16_accuracy_low(v16float v1, v16float v2) {
  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  v32bfloat16 c = broadcast_zero_to_v32bfloat16();
  v32bfloat16 d = broadcast_zero_to_v32bfloat16();
  v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();
  a = insert(a, 0, to_v16bfloat16((v16accfloat)v1));
  b = insert(b, 0, to_v16bfloat16(msc_elem_16_2(a, dummy0, (v16accfloat)v1)));
  c = insert(c, 0, to_v16bfloat16((v16accfloat)v2));
  d = insert(d, 0, to_v16bfloat16(msc_elem_16_2(c, dummy0, (v16accfloat)v2)));
  return mac_elem_16_2(a, c, mac_elem_16_2(a, d, mul_elem_16_2(b, c)));
}

#if 0
inline v8caccfloat mul_elem_8_accuracy_low(v8float v1, v8cfloat v2)
{
        v16float w3 = shuffle(set_v16float(0, v1), set_v16float(0, v1),T32_2x16_lo);
	v16float v22 = (v16float)v2;
        v16accfloat v3 = mul_elem_16_accuracy_low(w3,v22);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mul_elem_8_accuracy_low(v8cfloat v1, v8float v2)
{
	v16float w3 = shuffle(set_v16float(0, v2), set_v16float(0, v2), T32_2x16_lo);
        v16float v11 = (v16float)v1;
        v16accfloat v3 = mul_elem_16_accuracy_low(v11,w3);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mul_elem_8_accuracy_low(v8cfloat v1, v8cfloat v2)
{

	v16float v2_real = shuffle((v16float)v2, T32_16x2_lo);
        v16float v2_imag = shuffle((v16float)v2, T32_16x2_hi);
        v16float v2_flip = shuffle(v2_imag, v2_real, T32_2x16_lo);

        v16accfloat x = mul_elem_16_accuracy_low((v16float)v1,(v16float)v2);
        v16accfloat y = mul_elem_16_accuracy_low((v16float)v1,(v16float)v2_flip);
        v16float x_shifted = shiftl_elem((v16float)x,0);
        v16float y_shifted = shiftr_elem((v16float)y,0);
        v16accfloat real_vec = sub(x,(v16accfloat)x_shifted);
        v16accfloat imag_vec = add(y,(v16accfloat)y_shifted);

        return (v8caccfloat)sel((v16float)real_vec, (v16float)imag_vec, 0xaaaaaaaa);
}
#endif

inline v16accfloat mac_elem_16_accuracy_low(v16float v1, v16float v2,
                                            v16accfloat acc) {
  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  v32bfloat16 c = broadcast_zero_to_v32bfloat16();
  v32bfloat16 d = broadcast_zero_to_v32bfloat16();
  v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();
  a = insert(a, 0, to_v16bfloat16((v16accfloat)v1));
  b = insert(b, 0, to_v16bfloat16(msc_elem_16_2(a, dummy0, (v16accfloat)v1)));
  c = insert(c, 0, to_v16bfloat16((v16accfloat)v2));
  d = insert(d, 0, to_v16bfloat16(msc_elem_16_2(c, dummy0, (v16accfloat)v2)));
  return addmac_elem_16_2(a, c, mac_elem_16_2(a, d, mul_elem_16_2(b, c)), acc);
}

#if 0
inline v8caccfloat mac_elem_8_accuracy_low(v8float v1, v8cfloat v2, v8caccfloat acc)
{
	v16float w3 = shuffle(set_v16float(0, v1), set_v16float(0, v1),T32_2x16_lo);
	v16float v22 = (v16float)v2;
        v16accfloat v3 = mac_elem_16_accuracy_low(w3,v22,(v16accfloat)acc);
	return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mac_elem_8_accuracy_low(v8cfloat v1, v8float v2, v8caccfloat acc)
{
	v16float w3 = shuffle(set_v16float(0, v2), set_v16float(0, v2), T32_2x16_lo);
        v16float v11 = (v16float)v1;
        v16accfloat v3 = mac_elem_16_accuracy_low(v11,w3,(v16accfloat)acc);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mac_elem_8_accuracy_low(v8cfloat v1, v8cfloat v2, v8caccfloat acc)
{

	v16float v2_real = shuffle((v16float)v2, T32_16x2_lo);
        v16float v2_imag = shuffle((v16float)v2, T32_16x2_hi);
        v16float v2_flip = shuffle(v2_imag, v2_real, T32_2x16_lo);

        v16accfloat x = mul_elem_16_accuracy_low((v16float)v1,(v16float)v2);
        v16accfloat y = mul_elem_16_accuracy_low((v16float)v1,(v16float)v2_flip);
        v16float x_shifted = shiftl_elem((v16float)x,0);
        v16float y_shifted = shiftr_elem((v16float)y,0);
        v16accfloat real_vec = sub(x,(v16accfloat)x_shifted);
        v16accfloat imag_vec = add(y,(v16accfloat)y_shifted);


        return add(acc, (v8caccfloat)sel((v16float)real_vec, (v16float)imag_vec, 0xaaaaaaaa));
}
#endif

inline v16accfloat mul_elem_16_accuracy_fast(v16float v1, v16float v2) {
  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  v32bfloat16 c = broadcast_zero_to_v32bfloat16();
  v32bfloat16 d = broadcast_zero_to_v32bfloat16();
  v32bfloat16 e = broadcast_zero_to_v32bfloat16();
  v32bfloat16 f = broadcast_zero_to_v32bfloat16();
  v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();
  a = insert(a, 0, to_v16bfloat16((v16accfloat)v1));
  v16accfloat acc0 = msc_elem_16_2(a, dummy0, (v16accfloat)v1);
  b = insert(b, 0, to_v16bfloat16(acc0));
  c = insert(c, 0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
  d = insert(d, 0, to_v16bfloat16((v16accfloat)v2));
  v16accfloat acc1 = msc_elem_16_2(d, dummy0, (v16accfloat)v2);
  e = insert(e, 0, to_v16bfloat16(acc1));
  f = insert(f, 0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
  return mac_elem_16_2(
      a, d,
      mac_elem_16_2(
          a, e,
          mac_elem_16_2(
              b, d,
              mac_elem_16_2(d, c, mac_elem_16_2(b, e, mul_elem_16_2(a, f))))));
}

#if 0
inline v8caccfloat mul_elem_8_accuracy_fast(v8float v1, v8cfloat v2)
{
        v16float w3 = shuffle(set_v16float(0, v1), set_v16float(0, v1),T32_2x16_lo);
	v16float v22 = (v16float)v2;
        v16accfloat v3 = mul_elem_16_accuracy_fast(w3,v22);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mul_elem_8_accuracy_fast(v8cfloat v1, v8float v2)
{
	v16float w3 = shuffle(set_v16float(0, v2), set_v16float(0, v2), T32_2x16_lo);
        v16float v11 = (v16float)v1;
        v16accfloat v3 = mul_elem_16_accuracy_fast(v11,w3);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mul_elem_8_accuracy_fast(v8cfloat v1, v8cfloat v2)
{

	v16float v2_real = shuffle((v16float)v2, T32_16x2_lo);
        v16float v2_imag = shuffle((v16float)v2, T32_16x2_hi);
        v16float v2_flip = shuffle(v2_imag, v2_real, T32_2x16_lo);

        v16accfloat x = mul_elem_16_accuracy_fast((v16float)v1,(v16float)v2);
        v16accfloat y = mul_elem_16_accuracy_fast((v16float)v1,(v16float)v2_flip);
        v16float x_shifted = shiftl_elem((v16float)x,0);
        v16float y_shifted = shiftr_elem((v16float)y,0);
        v16accfloat real_vec = sub(x,(v16accfloat)x_shifted);
        v16accfloat imag_vec = add(y,(v16accfloat)y_shifted);

        return (v8caccfloat)sel((v16float)real_vec, (v16float)imag_vec, 0xaaaaaaaa);
}
#endif

inline v16accfloat mac_elem_16_accuracy_fast(v16float v1, v16float v2,
                                             v16accfloat acc) {
  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  v32bfloat16 c = broadcast_zero_to_v32bfloat16();
  v32bfloat16 d = broadcast_zero_to_v32bfloat16();
  v32bfloat16 e = broadcast_zero_to_v32bfloat16();
  v32bfloat16 f = broadcast_zero_to_v32bfloat16();
  v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();
  a = insert(a, 0, to_v16bfloat16((v16accfloat)v1));
  v16accfloat acc0 = msc_elem_16_2(a, dummy0, (v16accfloat)v1);
  b = insert(b, 0, to_v16bfloat16(acc0));
  c = insert(c, 0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
  d = insert(d, 0, to_v16bfloat16((v16accfloat)v2));
  v16accfloat acc1 = msc_elem_16_2(d, dummy0, (v16accfloat)v2);
  e = insert(e, 0, to_v16bfloat16(acc1));
  f = insert(f, 0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
  return addmac_elem_16_2(
      a, d,
      mac_elem_16_2(
          a, e,
          mac_elem_16_2(
              b, d,
              mac_elem_16_2(d, c, mac_elem_16_2(b, e, mul_elem_16_2(a, f))))),
      acc);
}

#if 0
inline v8caccfloat mac_elem_8_accuracy_fast(v8float v1, v8cfloat v2, v8caccfloat acc)
{
	v16float w3 = shuffle(set_v16float(0, v1), set_v16float(0, v1),T32_2x16_lo);
	v16float v22 = (v16float)v2;
        v16accfloat v3 = mac_elem_16_accuracy_fast(w3,v22,(v16accfloat)acc);
	return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mac_elem_8_accuracy_fast(v8cfloat v1, v8float v2, v8caccfloat acc)
{
	v16float w3 = shuffle(set_v16float(0, v2), set_v16float(0, v2), T32_2x16_lo);
        v16float v11 = (v16float)v1;
        v16accfloat v3 = mac_elem_16_accuracy_fast(v11,w3,(v16accfloat)acc);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mac_elem_8_accuracy_fast(v8cfloat v1, v8cfloat v2, v8caccfloat acc)
{

	v16float v2_real = shuffle((v16float)v2, T32_16x2_lo);
        v16float v2_imag = shuffle((v16float)v2, T32_16x2_hi);
        v16float v2_flip = shuffle(v2_imag, v2_real, T32_2x16_lo);

        v16accfloat x = mul_elem_16_accuracy_fast((v16float)v1,(v16float)v2);
        v16accfloat y = mul_elem_16_accuracy_fast((v16float)v1,(v16float)v2_flip);
        v16float x_shifted = shiftl_elem((v16float)x,0);
        v16float y_shifted = shiftr_elem((v16float)y,0);
        v16accfloat real_vec = sub(x,(v16accfloat)x_shifted);
        v16accfloat imag_vec = add(y,(v16accfloat)y_shifted);


        return add(acc, (v8caccfloat)sel((v16float)real_vec, (v16float)imag_vec, 0xaaaaaaaa));
}
#endif

inline v16accfloat mul_elem_16_accuracy_safe(v16float v1, v16float v2) {
  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  v32bfloat16 c = broadcast_zero_to_v32bfloat16();
  v32bfloat16 d = broadcast_zero_to_v32bfloat16();
  v32bfloat16 e = broadcast_zero_to_v32bfloat16();
  v32bfloat16 f = broadcast_zero_to_v32bfloat16();
  v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();
  a = insert(a, 0, to_v16bfloat16((v16accfloat)v1));
  v16accfloat acc0 = msc_elem_16_2(a, dummy0, (v16accfloat)v1);
  b = insert(b, 0, to_v16bfloat16(acc0));
  c = insert(c, 0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
  d = insert(d, 0, to_v16bfloat16((v16accfloat)v2));
  v16accfloat acc1 = msc_elem_16_2(d, dummy0, (v16accfloat)v2);
  e = insert(e, 0, to_v16bfloat16(acc1));
  f = insert(f, 0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
  return mac_elem_16_2(
      a, d,
      mac_elem_16_2(
          a, e,
          mac_elem_16_2(
              b, d,
              mac_elem_16_2(
                  d, c,
                  mac_elem_16_2(
                      b, e,
                      mac_elem_16_2(
                          a, f,
                          mac_elem_16_2(
                              b, f,
                              mac_elem_16_2(c, e, mul_elem_16_2(c, f)))))))));
}

#if 0
inline v8caccfloat mul_elem_8_accuracy_safe(v8float v1, v8cfloat v2)
{
        v16float w3 = shuffle(set_v16float(0, v1), set_v16float(0, v1),T32_2x16_lo);
	v16float v22 = (v16float)v2;
        v16accfloat v3 = mul_elem_16_accuracy_safe(w3,v22);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mul_elem_8_accuracy_safe(v8cfloat v1, v8float v2)
{
	v16float w3 = shuffle(set_v16float(0, v2), set_v16float(0, v2), T32_2x16_lo);
        v16float v11 = (v16float)v1;
        v16accfloat v3 = mul_elem_16_accuracy_safe(v11,w3);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mul_elem_8_accuracy_safe(v8cfloat v1, v8cfloat v2)
{

	v16float v2_real = shuffle((v16float)v2, T32_16x2_lo);
        v16float v2_imag = shuffle((v16float)v2, T32_16x2_hi);
        v16float v2_flip = shuffle(v2_imag, v2_real, T32_2x16_lo);

        v16accfloat x = mul_elem_16_accuracy_safe((v16float)v1,(v16float)v2);
        v16accfloat y = mul_elem_16_accuracy_safe((v16float)v1,(v16float)v2_flip);
        v16float x_shifted = shiftl_elem((v16float)x,0);
        v16float y_shifted = shiftr_elem((v16float)y,0);
        v16accfloat real_vec = sub(x,(v16accfloat)x_shifted);
        v16accfloat imag_vec = add(y,(v16accfloat)y_shifted);


        return (v8caccfloat)sel((v16float)real_vec, (v16float)imag_vec, 0xaaaaaaaa);
}
#endif

inline v16accfloat mac_elem_16_accuracy_safe(v16float v1, v16float v2,
                                             v16accfloat acc) {
  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  v32bfloat16 c = broadcast_zero_to_v32bfloat16();
  v32bfloat16 d = broadcast_zero_to_v32bfloat16();
  v32bfloat16 e = broadcast_zero_to_v32bfloat16();
  v32bfloat16 f = broadcast_zero_to_v32bfloat16();
  v32bfloat16 dummy0 = broadcast_one_to_v32bfloat16();
  a = insert(a, 0, to_v16bfloat16((v16accfloat)v1));
  v16accfloat acc0 = msc_elem_16_2(a, dummy0, (v16accfloat)v1);
  b = insert(b, 0, to_v16bfloat16(acc0));
  c = insert(c, 0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
  d = insert(d, 0, to_v16bfloat16((v16accfloat)v2));
  v16accfloat acc1 = msc_elem_16_2(d, dummy0, (v16accfloat)v2);
  e = insert(e, 0, to_v16bfloat16(acc1));
  f = insert(f, 0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
  return addmac_elem_16_2(
      a, d,
      mac_elem_16_2(
          a, e,
          mac_elem_16_2(
              b, d,
              mac_elem_16_2(
                  d, c,
                  mac_elem_16_2(
                      b, e,
                      mac_elem_16_2(
                          a, f,
                          mac_elem_16_2(
                              b, f,
                              mac_elem_16_2(c, e, mul_elem_16_2(c, f)))))))),
      acc);
}

#if 0
inline v8caccfloat mac_elem_8_accuracy_safe(v8float v1, v8cfloat v2, v8caccfloat acc)
{
	v16float w3 = shuffle(set_v16float(0, v1), set_v16float(0, v1),T32_2x16_lo);
	v16float v22 = (v16float)v2;
        v16accfloat v3 = mac_elem_16_accuracy_safe(w3,v22,(v16accfloat)acc);
	return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mac_elem_8_accuracy_safe(v8cfloat v1, v8float v2, v8caccfloat acc)
{
	v16float w3 = shuffle(set_v16float(0, v2), set_v16float(0, v2), T32_2x16_lo);
        v16float v11 = (v16float)v1;
        v16accfloat v3 = mac_elem_16_accuracy_safe(v11,w3,(v16accfloat)acc);
        return (v8caccfloat)((v16float)v3);
}
inline v8caccfloat mac_elem_8_accuracy_safe(v8cfloat v1, v8cfloat v2, v8caccfloat acc)
{

	v16float v2_real = shuffle((v16float)v2, T32_16x2_lo);
        v16float v2_imag = shuffle((v16float)v2, T32_16x2_hi);
        v16float v2_flip = shuffle(v2_imag, v2_real, T32_2x16_lo);

        v16accfloat x = mul_elem_16_accuracy_safe((v16float)v1,(v16float)v2);
        v16accfloat y = mul_elem_16_accuracy_safe((v16float)v1,(v16float)v2_flip);
        v16float x_shifted = shiftl_elem((v16float)x,0);
        v16float y_shifted = shiftr_elem((v16float)y,0);
        v16accfloat real_vec = sub(x,(v16accfloat)x_shifted);
        v16accfloat imag_vec = add(y,(v16accfloat)y_shifted);

	return add(acc, (v8caccfloat)sel((v16float)real_vec, (v16float)imag_vec, 0xaaaaaaaa));
}

inline v16accfloat mul_4x8_8x4_accuracy_safe(v32float v1, v32float v2)
{
    v32bfloat16 a;
    v32bfloat16 b;
    v32bfloat16 c;
    v32bfloat16 d;
    v32bfloat16 e;
    v32bfloat16 f;
    v32bfloat16 dummy0 = ::shuffle(broadcast_one_to_v32bfloat16(), broadcast_zero_to_v32bfloat16(), INTLV_lo_256o512);
    v32bfloat16 dummy1 = ::shuffle(broadcast_zero_to_v32bfloat16(), broadcast_one_to_v32bfloat16(), INTLV_lo_256o512);

    v16accfloat temp0 = (v16accfloat)extract_v16float(v1, 0);
    v16accfloat temp1 = (v16accfloat)extract_v16float(v2, 0);
    v16accfloat temp2 = (v16accfloat)extract_v16float(v1, 1);
    v16accfloat temp3 = (v16accfloat)extract_v16float(v2, 1);

    a = set_v32bfloat16(0, to_v16bfloat16(temp0));
    a = insert(a,       1, to_v16bfloat16(temp2));

    v16accfloat acc0 = msc_elem_16_2(a, dummy0, temp0);
    v16accfloat acc2 = msc_elem_16_2(a, dummy1, temp2);

    b = set_v32bfloat16(0, to_v16bfloat16(acc0));
    b = insert(b,       1, to_v16bfloat16(acc2));

    c = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
    c = insert(c,       1, to_v16bfloat16(msc_elem_16_2(b, dummy1, acc2)));

    d = set_v32bfloat16(0, to_v16bfloat16(temp1));
    d = insert(d,       1, to_v16bfloat16(temp3));

    v16accfloat acc1 = msc_elem_16_2(d, dummy0, temp1);
    v16accfloat acc3 = msc_elem_16_2(d, dummy1, temp3);

    e = set_v32bfloat16(0, to_v16bfloat16(acc1));
    e = insert(e,       1, to_v16bfloat16(acc3));

    f = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
    f = insert(f,       1, to_v16bfloat16(msc_elem_16_2(e, dummy1, acc3)));
    return mac_4x8_8x4(a,d,mac_4x8_8x4(a,e,mac_4x8_8x4(b,d,mac_4x8_8x4(d,c,mac_4x8_8x4(b,e,mac_4x8_8x4(a,f,mac_4x8_8x4(b,f,mac_4x8_8x4(c,e,mul_4x8_8x4(c,f)))))))));
}
inline v4caccfloat mul_2x8_8x2_accuracy_safe(v16float v1, v16cfloat v2)
{
        v32float    w3    = concat(v1,undef_v16float());
        v16accfloat w4    = mul_4x8_8x4_accuracy_safe(w3, (v32float)v2);
        v8float     out   = extract_v8float((v16float)w4,0);
        return (v4caccfloat)out;
}
inline v16accfloat mac_4x8_8x4_accuracy_safe(v32float v1, v32float v2, v16accfloat acc)
{
    v32bfloat16 a;
    v32bfloat16 b;
    v32bfloat16 c;
    v32bfloat16 d;
    v32bfloat16 e;
    v32bfloat16 f;
    v32bfloat16 dummy0 = ::shuffle(broadcast_one_to_v32bfloat16(), broadcast_zero_to_v32bfloat16(), INTLV_lo_256o512);
    v32bfloat16 dummy1 = ::shuffle(broadcast_zero_to_v32bfloat16(), broadcast_one_to_v32bfloat16(), INTLV_lo_256o512);

    v16accfloat temp0 = (v16accfloat)extract_v16float(v1, 0);
    v16accfloat temp1 = (v16accfloat)extract_v16float(v2, 0);
    v16accfloat temp2 = (v16accfloat)extract_v16float(v1, 1);
    v16accfloat temp3 = (v16accfloat)extract_v16float(v2, 1);

    a = set_v32bfloat16(0, to_v16bfloat16(temp0));
    a = insert(a,       1, to_v16bfloat16(temp2));

    v16accfloat acc0 = msc_elem_16_2(a, dummy0, temp0);
    v16accfloat acc2 = msc_elem_16_2(a, dummy1, temp2);

    b = set_v32bfloat16(0, to_v16bfloat16(acc0));
    b = insert(b,       1, to_v16bfloat16(acc2));

    c = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
    c = insert(c,       1, to_v16bfloat16(msc_elem_16_2(b, dummy1, acc2)));

    d = set_v32bfloat16(0, to_v16bfloat16(temp1));
    d = insert(d,       1, to_v16bfloat16(temp3));

    v16accfloat acc1 = msc_elem_16_2(d, dummy0, temp1);
    v16accfloat acc3 = msc_elem_16_2(d, dummy1, temp3);

    e = set_v32bfloat16(0, to_v16bfloat16(acc1));
    e = insert(e,       1, to_v16bfloat16(acc3));

    f = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
    f = insert(f,       1, to_v16bfloat16(msc_elem_16_2(e, dummy1, acc3)));
    return addmac_4x8_8x4(a,d,mac_4x8_8x4(a,e,mac_4x8_8x4(b,d,mac_4x8_8x4(d,c,mac_4x8_8x4(b,e,mac_4x8_8x4(a,f,mac_4x8_8x4(b,f,mac_4x8_8x4(c,e,mul_4x8_8x4(c,f)))))))),acc);
}
inline v16accfloat mul_4x8_8x4_accuracy_fast(v32float v1, v32float v2)
{
    v32bfloat16 a;
    v32bfloat16 b;
    v32bfloat16 c;
    v32bfloat16 d;
    v32bfloat16 e;
    v32bfloat16 f;
    v32bfloat16 dummy0 = ::shuffle(broadcast_one_to_v32bfloat16(), broadcast_zero_to_v32bfloat16(), INTLV_lo_256o512);
    v32bfloat16 dummy1 = ::shuffle(broadcast_zero_to_v32bfloat16(), broadcast_one_to_v32bfloat16(), INTLV_lo_256o512);

    v16accfloat temp0 = (v16accfloat)extract_v16float(v1, 0);
    v16accfloat temp1 = (v16accfloat)extract_v16float(v2, 0);
    v16accfloat temp2 = (v16accfloat)extract_v16float(v1, 1);
    v16accfloat temp3 = (v16accfloat)extract_v16float(v2, 1);

    a = set_v32bfloat16(0, to_v16bfloat16(temp0));
    a = insert(a,       1, to_v16bfloat16(temp2));

    v16accfloat acc0 = msc_elem_16_2(a, dummy0, temp0);
    v16accfloat acc2 = msc_elem_16_2(a, dummy1, temp2);

    b = set_v32bfloat16(0, to_v16bfloat16(acc0));
    b = insert(b,       1, to_v16bfloat16(acc2));

    c = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
    c = insert(c,       1, to_v16bfloat16(msc_elem_16_2(b, dummy1, acc2)));

    d = set_v32bfloat16(0, to_v16bfloat16(temp1));
    d = insert(d,       1, to_v16bfloat16(temp3));

    v16accfloat acc1 = msc_elem_16_2(d, dummy0, temp1);
    v16accfloat acc3 = msc_elem_16_2(d, dummy1, temp3);

    e = set_v32bfloat16(0, to_v16bfloat16(acc1));
    e = insert(e,       1, to_v16bfloat16(acc3));

    f = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
    f = insert(f,       1, to_v16bfloat16(msc_elem_16_2(e, dummy1, acc3)));

    return mac_4x8_8x4(a, d, mac_4x8_8x4(a, e, mac_4x8_8x4(b, d, mac_4x8_8x4(d, c, mac_4x8_8x4(b, e, mul_4x8_8x4(a, f))))));
}
inline v4caccfloat mul_2x8_8x2_accuracy_fast(v16float v1, v16cfloat v2)
{
        v32float    w3    = concat(v1,undef_v16float());
        v16accfloat w4    = mul_4x8_8x4_accuracy_fast(w3, (v32float)v2);
        v8float     out   = extract_v8float((v16float)w4,0);
        return (v4caccfloat)out;
}
inline v16accfloat mac_4x8_8x4_accuracy_fast(v32float v1, v32float v2, v16accfloat acc)
{
    v32bfloat16 a;
    v32bfloat16 b;
    v32bfloat16 c;
    v32bfloat16 d;
    v32bfloat16 e;
    v32bfloat16 f;
    v32bfloat16 dummy0 = ::shuffle(broadcast_one_to_v32bfloat16(), broadcast_zero_to_v32bfloat16(), INTLV_lo_256o512);
    v32bfloat16 dummy1 = ::shuffle(broadcast_zero_to_v32bfloat16(), broadcast_one_to_v32bfloat16(), INTLV_lo_256o512);

    v16accfloat temp0 = (v16accfloat)extract_v16float(v1, 0);
    v16accfloat temp1 = (v16accfloat)extract_v16float(v2, 0);
    v16accfloat temp2 = (v16accfloat)extract_v16float(v1, 1);
    v16accfloat temp3 = (v16accfloat)extract_v16float(v2, 1);

    a = set_v32bfloat16(0, to_v16bfloat16(temp0));
    a = insert(a,       1, to_v16bfloat16(temp2));

    v16accfloat acc0 = msc_elem_16_2(a, dummy0, temp0);
    v16accfloat acc2 = msc_elem_16_2(a, dummy1, temp2);

    b = set_v32bfloat16(0, to_v16bfloat16(acc0));
    b = insert(b,       1, to_v16bfloat16(acc2));

    c = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
    c = insert(c,       1, to_v16bfloat16(msc_elem_16_2(b, dummy1, acc2)));

    d = set_v32bfloat16(0, to_v16bfloat16(temp1));
    d = insert(d,       1, to_v16bfloat16(temp3));

    v16accfloat acc1 = msc_elem_16_2(d, dummy0, temp1);
    v16accfloat acc3 = msc_elem_16_2(d, dummy1, temp3);

    e = set_v32bfloat16(0, to_v16bfloat16(acc1));
    e = insert(e,       1, to_v16bfloat16(acc3));

    f = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(e, dummy0, acc1)));
    f = insert(f,       1, to_v16bfloat16(msc_elem_16_2(e, dummy1, acc3)));

    return addmac_4x8_8x4(a, d, mac_4x8_8x4(a, e, mac_4x8_8x4(b, d, mac_4x8_8x4(d, c, mac_4x8_8x4(b, e, mul_4x8_8x4(a, f))))),acc);
}
inline v16accfloat mul_4x8_8x4_accuracy_low(v32float v1, v32float v2)
{
    v32bfloat16 a;
    v32bfloat16 b;
    v32bfloat16 d;
    v32bfloat16 e;
    v32bfloat16 dummy0 = ::shuffle(broadcast_one_to_v32bfloat16(), broadcast_zero_to_v32bfloat16(), INTLV_lo_256o512);
    v32bfloat16 dummy1 = ::shuffle(broadcast_zero_to_v32bfloat16(), broadcast_one_to_v32bfloat16(), INTLV_lo_256o512);

    v16accfloat temp0 = (v16accfloat)extract_v16float(v1, 0);
    v16accfloat temp1 = (v16accfloat)extract_v16float(v2, 0);
    v16accfloat temp2 = (v16accfloat)extract_v16float(v1, 1);
    v16accfloat temp3 = (v16accfloat)extract_v16float(v2, 1);

    a = set_v32bfloat16(0, to_v16bfloat16(temp0));
    a = insert(a,       1, to_v16bfloat16(temp2));

    v16accfloat acc0 = msc_elem_16_2(a, dummy0, temp0);
    v16accfloat acc2 = msc_elem_16_2(a, dummy1, temp2);

    b = set_v32bfloat16(0, to_v16bfloat16(acc0));
    b = insert(b,       1, to_v16bfloat16(acc2));

    d = set_v32bfloat16(0, to_v16bfloat16(temp1));
    d = insert(d,       1, to_v16bfloat16(temp3));

    v16accfloat acc1 = msc_elem_16_2(d, dummy0, temp1);
    v16accfloat acc3 = msc_elem_16_2(d, dummy1, temp3);

    e = set_v32bfloat16(0, to_v16bfloat16(acc1));
    e = insert(e,       1, to_v16bfloat16(acc3));

    return mac_4x8_8x4(a,d,mac_4x8_8x4(a,e,mul_4x8_8x4(b,d)));
}
inline v4caccfloat mul_2x8_8x2_accuracy_low(v16float v1, v16cfloat v2)
{
        v32float    w3    = concat(v1,undef_v16float());
        v16accfloat w4    = mul_4x8_8x4_accuracy_low(w3, (v32float)v2);
        v8float     out   = extract_v8float((v16float)w4,0);
        return (v4caccfloat)out;
}
inline v16accfloat mac_4x8_8x4_accuracy_low(v32float v1, v32float v2, v16accfloat acc)
{

    v32bfloat16 a;
    v32bfloat16 b;
    v32bfloat16 d;
    v32bfloat16 e;
    v32bfloat16 dummy0 = ::shuffle(broadcast_one_to_v32bfloat16(), broadcast_zero_to_v32bfloat16(), INTLV_lo_256o512);
    v32bfloat16 dummy1 = ::shuffle(broadcast_zero_to_v32bfloat16(), broadcast_one_to_v32bfloat16(), INTLV_lo_256o512);

    v16accfloat temp0 = (v16accfloat)extract_v16float(v1, 0);
    v16accfloat temp1 = (v16accfloat)extract_v16float(v2, 0);
    v16accfloat temp2 = (v16accfloat)extract_v16float(v1, 1);
    v16accfloat temp3 = (v16accfloat)extract_v16float(v2, 1);

    a = set_v32bfloat16(0, to_v16bfloat16(temp0));
    a = insert(a,       1, to_v16bfloat16(temp2));

    v16accfloat acc0 = msc_elem_16_2(a, dummy0, temp0);
    v16accfloat acc2 = msc_elem_16_2(a, dummy1, temp2);

    b = set_v32bfloat16(0, to_v16bfloat16(acc0));
    b = insert(b,       1, to_v16bfloat16(acc2));

    d = set_v32bfloat16(0, to_v16bfloat16(msc_elem_16_2(b, dummy0, acc0)));
    d = insert(d,       1, to_v16bfloat16(msc_elem_16_2(b, dummy1, acc2)));

    e = set_v32bfloat16(0, to_v16bfloat16(temp1));
    e = insert(e,       1, to_v16bfloat16(temp3));

    return addmac_4x8_8x4(a,d,mac_4x8_8x4(a,e,mul_4x8_8x4(b,d)),acc);
}
#endif // #if 0

#endif

#if 0
inline v16accfloat negmul_elem_16_accuracy_low(v16float v1, v16float v2)
{
    return neg(mul_elem_16_accuracy_low(v1,v2));
}

inline v8caccfloat negmul_elem_8_accuracy_low(v8float v1, v8cfloat v2)
{
    return neg(mul_elem_8_accuracy_low(v1,v2));
}
inline v8caccfloat negmul_elem_8_accuracy_low(v8cfloat v1, v8float v2)
{
    return neg(mul_elem_8_accuracy_low(v1,v2));
}
inline v8caccfloat negmul_elem_8_accuracy_low(v8cfloat v1, v8cfloat v2)
{
    return neg(mul_elem_8_accuracy_low(v1,v2));
}
inline v16accfloat negmul_elem_16_accuracy_fast(v16float v1, v16float v2)
{
    return neg(mul_elem_16_accuracy_fast(v1,v2));
}
inline v8caccfloat negmul_elem_8_accuracy_fast(v8float v1, v8cfloat v2)
{
    return neg(mul_elem_8_accuracy_fast(v1,v2));
}
inline v8caccfloat negmul_elem_8_accuracy_fast(v8cfloat v1, v8float v2)
{
    return neg(mul_elem_8_accuracy_fast(v1,v2));
}
inline v8caccfloat negmul_elem_8_accuracy_fast(v8cfloat v1, v8cfloat v2)
{
    return neg(mul_elem_8_accuracy_fast(v1,v2));
}
inline v16accfloat negmul_elem_16_accuracy_safe(v16float v1, v16float v2)
{
    return neg(mul_elem_16_accuracy_safe(v1,v2));
}
inline v8caccfloat negmul_elem_8_accuracy_safe(v8float v1, v8cfloat v2)
{
    return neg(mul_elem_8_accuracy_safe(v1,v2));
}
inline v8caccfloat negmul_elem_8_accuracy_safe(v8cfloat v1, v8float v2)
{
    return neg(mul_elem_8_accuracy_safe(v1,v2));
}
inline v8caccfloat negmul_elem_8_accuracy_safe(v8cfloat v1, v8cfloat v2)
{
    return neg(mul_elem_8_accuracy_safe(v1,v2));
}
#endif

inline v16accfloat addmac_elem_16_accuracy_safe(v16float v1, v16float v2,
                                                v16accfloat acc1,
                                                v16accfloat acc2) {
  return add(mac_elem_16_accuracy_safe(v1, v2, acc1), acc2);
}
inline v16accfloat addmac_elem_16_accuracy_fast(v16float v1, v16float v2,
                                                v16accfloat acc1,
                                                v16accfloat acc2) {
  return add(mac_elem_16_accuracy_fast(v1, v2, acc1), acc2);
}
inline v16accfloat addmac_elem_16_accuracy_low(v16float v1, v16float v2,
                                               v16accfloat acc1,
                                               v16accfloat acc2) {
  return add(mac_elem_16_accuracy_low(v1, v2, acc1), acc2);
}
inline v16accfloat msc_elem_16_accuracy_safe(v16float v1, v16float v2,
                                             v16accfloat acc) {
  return sub(acc, mul_elem_16_accuracy_safe(v1, v2));
}

#if 0
inline v8caccfloat msc_elem_8_accuracy_safe(v8float v1, v8cfloat v2, v8caccfloat acc)
{
    return sub(acc,mul_elem_8_accuracy_safe(v1,v2));
}
inline v8caccfloat msc_elem_8_accuracy_safe(v8cfloat v1, v8float v2, v8caccfloat acc)
{
    return sub(acc,mul_elem_8_accuracy_safe(v1,v2));
}
inline v8caccfloat msc_elem_8_accuracy_safe(v8cfloat v1, v8cfloat v2, v8caccfloat acc)
{
    return sub(acc,mul_elem_8_accuracy_safe(v1,v2));
}
#endif

inline v16accfloat msc_elem_16_accuracy_fast(v16float v1, v16float v2,
                                             v16accfloat acc) {
  return sub(acc, mul_elem_16_accuracy_fast(v1, v2));
}

#if 0
inline v8caccfloat msc_elem_8_accuracy_fast(v8float v1, v8cfloat v2, v8caccfloat acc)
{
    return sub(acc,mul_elem_8_accuracy_fast(v1,v2));
}
inline v8caccfloat msc_elem_8_accuracy_fast(v8cfloat v1, v8float v2, v8caccfloat acc)
{
    return sub(acc,mul_elem_8_accuracy_fast(v1,v2));
}
inline v8caccfloat msc_elem_8_accuracy_fast(v8cfloat v1, v8cfloat v2, v8caccfloat acc)
{
    return sub(acc,mul_elem_8_accuracy_fast(v1,v2));
}
#endif

inline v16accfloat msc_elem_16_accuracy_low(v16float v1, v16float v2,
                                            v16accfloat acc) {
  return sub(acc, mul_elem_16_accuracy_low(v1, v2));
}

#if 0
inline v8caccfloat msc_elem_8_accuracy_low(v8float v1, v8cfloat v2, v8caccfloat acc)
{
    return sub(acc,mul_elem_8_accuracy_low(v1,v2));
}
inline v8caccfloat msc_elem_8_accuracy_low(v8cfloat v1, v8float v2, v8caccfloat acc)
{
    return sub(acc,mul_elem_8_accuracy_low(v1,v2));
}
inline v8caccfloat msc_elem_8_accuracy_low(v8cfloat v1, v8cfloat v2, v8caccfloat acc)
{
    return sub(acc,mul_elem_8_accuracy_low(v1,v2));
}
#endif

inline v16accfloat addmsc_elem_16_accuracy_safe(v16float v1, v16float v2,
                                                v16accfloat acc1,
                                                v16accfloat acc2) {
  return sub(add(acc1, acc2), mul_elem_16_accuracy_safe(v1, v2));
}
inline v16accfloat addmsc_elem_16_accuracy_fast(v16float v1, v16float v2,
                                                v16accfloat acc1,
                                                v16accfloat acc2) {
  return sub(add(acc1, acc2), mul_elem_16_accuracy_fast(v1, v2));
}
inline v16accfloat addmsc_elem_16_accuracy_low(v16float v1, v16float v2,
                                               v16accfloat acc1,
                                               v16accfloat acc2) {
  return sub(add(acc1, acc2), mul_elem_16_accuracy_low(v1, v2));
}

#if 0
inline v16accfloat negmul_4x8_8x4_accuracy_safe(v32float v1, v32float v2)
{
    return neg(mul_4x8_8x4_accuracy_safe(v1,v2));
}
inline v16accfloat negmul_4x8_8x4_accuracy_fast(v32float v1, v32float v2)
{
    return neg(mul_4x8_8x4_accuracy_fast(v1,v2));
}
inline v16accfloat negmul_4x8_8x4_accuracy_low(v32float v1, v32float v2)
{
    return neg(mul_4x8_8x4_accuracy_low(v1,v2));
}
inline v16accfloat addmac_4x8_8x4_accuracy_safe(v32float v1, v32float v2, v16accfloat acc1, v16accfloat acc2)
{
    return add(add(acc1, acc2),mul_4x8_8x4_accuracy_safe(v1,v2));
}
inline v16accfloat addmac_4x8_8x4_accuracy_fast(v32float v1, v32float v2, v16accfloat acc1, v16accfloat acc2)
{
    return add(add(acc1, acc2),mul_4x8_8x4_accuracy_fast(v1,v2));
}
inline v16accfloat addmac_4x8_8x4_accuracy_low(v32float v1, v32float v2, v16accfloat acc1, v16accfloat acc2)
{
    return add(add(acc1, acc2),mul_4x8_8x4_accuracy_low(v1,v2));
}
inline v16accfloat msc_4x8_8x4_accuracy_safe(v32float v1, v32float v2, v16accfloat acc)
{
    return sub(acc, mul_4x8_8x4_accuracy_safe(v1,v2));
}
inline v16accfloat msc_4x8_8x4_accuracy_fast(v32float v1, v32float v2, v16accfloat acc)
{
    return sub(acc, mul_4x8_8x4_accuracy_fast(v1,v2));
}
inline v16accfloat msc_4x8_8x4_accuracy_low(v32float v1, v32float v2, v16accfloat acc)
{
    return sub(acc, mul_4x8_8x4_accuracy_low(v1,v2));
}
inline v16accfloat addmsc_4x8_8x4_accuracy_safe(v32float v1, v32float v2, v16accfloat acc1, v16accfloat acc2)
{
    return sub(add(acc1, acc2),mul_4x8_8x4_accuracy_safe(v1,v2));
}
inline v16accfloat addmsc_4x8_8x4_accuracy_fast(v32float v1, v32float v2, v16accfloat acc1, v16accfloat acc2)
{
    return sub(add(acc1, acc2),mul_4x8_8x4_accuracy_fast(v1,v2));
}
inline v16accfloat addmsc_4x8_8x4_accuracy_low(v32float v1, v32float v2, v16accfloat acc1, v16accfloat acc2)
{
    return sub(add(acc1, acc2),mul_4x8_8x4_accuracy_low(v1,v2));
}
#endif

#if defined(AIE2_FP32_EMULATION_ACCURACY_FAST)
#define mul_elem_16 mul_elem_16_accuracy_fast
#define negmul_elem_16 negmul_elem_16_accuracy_fast
#define mac_elem_16 mac_elem_16_accuracy_fast
#define msc_elem_16 msc_elem_16_accuracy_fast
#define addmsc_elem_16 addmsc_elem_16_accuracy_fast
#define addmac_elem_16 addmac_elem_16_accuracy_fast

#if 0
inline v8caccfloat mul_elem_8(v8float v1, v8cfloat v2)  { return mul_elem_8_accuracy_fast(v1,v2);}
inline v8caccfloat mul_elem_8(v8cfloat v1, v8float v2)  { return mul_elem_8_accuracy_fast(v1,v2);}
inline v8caccfloat mul_elem_8(v8cfloat v1, v8cfloat v2) { return mul_elem_8_accuracy_fast(v1,v2);}
inline v8caccfloat negmul_elem_8(v8float v1, v8cfloat v2)  { return negmul_elem_8_accuracy_fast(v1,v2);}
inline v8caccfloat negmul_elem_8(v8cfloat v1, v8float v2)  { return negmul_elem_8_accuracy_fast(v1,v2);}
inline v8caccfloat negmul_elem_8(v8cfloat v1, v8cfloat v2) { return negmul_elem_8_accuracy_fast(v1,v2);}
inline v8caccfloat msc_elem_8(v8float v1, v8cfloat v2,v8caccfloat acc)  { return msc_elem_8_accuracy_fast(v1,v2,acc);}
inline v8caccfloat msc_elem_8(v8cfloat v1, v8float v2,v8caccfloat acc)  { return msc_elem_8_accuracy_fast(v1,v2,acc);}
inline v8caccfloat msc_elem_8(v8cfloat v1, v8cfloat v2,v8caccfloat acc) { return msc_elem_8_accuracy_fast(v1,v2,acc);}
inline v8caccfloat mac_elem_8(v8float v1, v8cfloat v2, v8caccfloat acc)  { return mac_elem_8_accuracy_fast(v1,v2,acc);}
inline v8caccfloat mac_elem_8(v8cfloat v1, v8float v2, v8caccfloat acc)  { return mac_elem_8_accuracy_fast(v1,v2,acc);}
inline v8caccfloat mac_elem_8(v8cfloat v1, v8cfloat v2, v8caccfloat acc) { return mac_elem_8_accuracy_fast(v1,v2,acc);}
inline v16accfloat mul_4x8_8x4(    v32float v1, v32float v2                                     ) { return mul_4x8_8x4_accuracy_fast(    v1, v2             );}
inline v4caccfloat mul_2x8_8x2(    v16float v1, v16cfloat v2                                    ) { return mul_2x8_8x2_accuracy_fast(    v1, v2             );}
inline v16accfloat negmul_4x8_8x4( v32float v1, v32float v2                                     ) { return negmul_4x8_8x4_accuracy_fast( v1, v2             );}
inline v16accfloat mac_4x8_8x4(    v32float v1, v32float v2, v16accfloat acc                    ) { return mac_4x8_8x4_accuracy_fast(    v1, v2, acc        );}
inline v16accfloat msc_4x8_8x4(    v32float v1, v32float v2, v16accfloat acc                    ) { return msc_4x8_8x4_accuracy_fast(    v1, v2, acc        );}
inline v16accfloat addmac_4x8_8x4( v32float v1, v32float v2, v16accfloat acc1, v16accfloat acc2 ) { return addmac_4x8_8x4_accuracy_fast( v1, v2, acc1, acc2 );}
inline v16accfloat addmsc_4x8_8x4( v32float v1, v32float v2, v16accfloat acc1, v16accfloat acc2 ) { return addmsc_4x8_8x4_accuracy_fast( v1, v2, acc1, acc2 );}
#endif

#elif defined(AIE2_FP32_EMULATION_ACCURACY_LOW)
#define mul_elem_16 mul_elem_16_accuracy_low
#define negmul_elem_16 negmul_elem_16_accuracy_low
#define mac_elem_16 mac_elem_16_accuracy_low
#define msc_elem_16 msc_elem_16_accuracy_low
#define addmsc_elem_16 addmsc_elem_16_accuracy_low
#define addmac_elem_16 addmac_elem_16_accuracy_low

#if 0
inline v8caccfloat mul_elem_8(v8float v1, v8cfloat v2)  { return mul_elem_8_accuracy_low(v1,v2);}
inline v8caccfloat mul_elem_8(v8cfloat v1, v8float v2)  { return mul_elem_8_accuracy_low(v1,v2);}
inline v8caccfloat mul_elem_8(v8cfloat v1, v8cfloat v2) { return mul_elem_8_accuracy_low(v1,v2);}
inline v8caccfloat negmul_elem_8(v8float v1, v8cfloat v2)  { return negmul_elem_8_accuracy_low(v1,v2);}
inline v8caccfloat negmul_elem_8(v8cfloat v1, v8float v2)  { return negmul_elem_8_accuracy_low(v1,v2);}
inline v8caccfloat negmul_elem_8(v8cfloat v1, v8cfloat v2) { return negmul_elem_8_accuracy_low(v1,v2);}
inline v8caccfloat msc_elem_8(v8float v1, v8cfloat v2,v8caccfloat acc)  { return msc_elem_8_accuracy_low(v1,v2,acc);}
inline v8caccfloat msc_elem_8(v8cfloat v1, v8float v2,v8caccfloat acc)  { return msc_elem_8_accuracy_low(v1,v2,acc);}
inline v8caccfloat msc_elem_8(v8cfloat v1, v8cfloat v2,v8caccfloat acc) { return msc_elem_8_accuracy_low(v1,v2,acc);}
inline v8caccfloat mac_elem_8(v8float v1, v8cfloat v2, v8caccfloat acc)  { return mac_elem_8_accuracy_low(v1,v2,acc);}
inline v8caccfloat mac_elem_8(v8cfloat v1, v8float v2, v8caccfloat acc)  { return mac_elem_8_accuracy_low(v1,v2,acc);}
inline v8caccfloat mac_elem_8(v8cfloat v1, v8cfloat v2, v8caccfloat acc) { return mac_elem_8_accuracy_low(v1,v2,acc);}
inline v16accfloat mul_4x8_8x4(    v32float v1, v32float v2                                     ) { return mul_4x8_8x4_accuracy_low(    v1, v2             );}
inline v4caccfloat mul_2x8_8x2(    v16float v1, v16cfloat v2                                    ) { return mul_2x8_8x2_accuracy_low(    v1, v2             );}
inline v16accfloat negmul_4x8_8x4( v32float v1, v32float v2                                     ) { return negmul_4x8_8x4_accuracy_low( v1, v2             );}
inline v16accfloat mac_4x8_8x4(    v32float v1, v32float v2, v16accfloat acc                    ) { return mac_4x8_8x4_accuracy_low(    v1, v2, acc        );}
inline v16accfloat msc_4x8_8x4(    v32float v1, v32float v2, v16accfloat acc                    ) { return msc_4x8_8x4_accuracy_low(    v1, v2, acc        );}
inline v16accfloat addmac_4x8_8x4( v32float v1, v32float v2, v16accfloat acc1, v16accfloat acc2 ) { return addmac_4x8_8x4_accuracy_low( v1, v2, acc1, acc2 );}
inline v16accfloat addmsc_4x8_8x4( v32float v1, v32float v2, v16accfloat acc1, v16accfloat acc2 ) { return addmsc_4x8_8x4_accuracy_low( v1, v2, acc1, acc2 );}
#endif

#else
#define mul_elem_16 mul_elem_16_accuracy_safe
#define negmul_elem_16 negmul_elem_16_accuracy_safe
#define mac_elem_16 mac_elem_16_accuracy_safe
#define msc_elem_16 msc_elem_16_accuracy_safe
#define addmsc_elem_16 addmsc_elem_16_accuracy_safe
#define addmac_elem_16 addmac_elem_16_accuracy_safe

#if 0
inline v8caccfloat mul_elem_8(v8float v1, v8cfloat v2) { return mul_elem_8_accuracy_safe(v1,v2);}
inline v8caccfloat mul_elem_8(v8cfloat v1, v8float v2) { return mul_elem_8_accuracy_safe(v1,v2);}
inline v8caccfloat mul_elem_8(v8cfloat v1, v8cfloat v2){ return mul_elem_8_accuracy_safe(v1,v2);}
inline v8caccfloat negmul_elem_8(v8float v1, v8cfloat v2)  { return negmul_elem_8_accuracy_safe(v1,v2);}
inline v8caccfloat negmul_elem_8(v8cfloat v1, v8float v2)  { return negmul_elem_8_accuracy_safe(v1,v2);}
inline v8caccfloat negmul_elem_8(v8cfloat v1, v8cfloat v2) { return negmul_elem_8_accuracy_safe(v1,v2);}
inline v8caccfloat msc_elem_8(v8float v1, v8cfloat v2,v8caccfloat acc)  { return msc_elem_8_accuracy_safe(v1,v2,acc);}
inline v8caccfloat msc_elem_8(v8cfloat v1, v8float v2,v8caccfloat acc)  { return msc_elem_8_accuracy_safe(v1,v2,acc);}
inline v8caccfloat msc_elem_8(v8cfloat v1, v8cfloat v2,v8caccfloat acc) { return msc_elem_8_accuracy_safe(v1,v2,acc);}
inline v8caccfloat mac_elem_8(v8float v1, v8cfloat v2, v8caccfloat acc)  { return mac_elem_8_accuracy_safe(v1,v2,acc);}
inline v8caccfloat mac_elem_8(v8cfloat v1, v8float v2, v8caccfloat acc)  { return mac_elem_8_accuracy_safe(v1,v2,acc);}
inline v8caccfloat mac_elem_8(v8cfloat v1, v8cfloat v2, v8caccfloat acc) { return mac_elem_8_accuracy_safe(v1,v2,acc);}
inline v16accfloat mul_4x8_8x4(    v32float v1, v32float v2                                     ) { return mul_4x8_8x4_accuracy_safe(    v1, v2             );}
inline v4caccfloat mul_2x8_8x2(    v16float v1, v16cfloat v2                                    ) { return mul_2x8_8x2_accuracy_safe(    v1, v2             );}
inline v16accfloat negmul_4x8_8x4( v32float v1, v32float v2                                     ) { return negmul_4x8_8x4_accuracy_safe( v1, v2             );}
inline v16accfloat mac_4x8_8x4(    v32float v1, v32float v2, v16accfloat acc                    ) { return mac_4x8_8x4_accuracy_safe(    v1, v2, acc        );}
inline v16accfloat msc_4x8_8x4(    v32float v1, v32float v2, v16accfloat acc                    ) { return msc_4x8_8x4_accuracy_safe(    v1, v2, acc        );}
inline v16accfloat addmac_4x8_8x4( v32float v1, v32float v2, v16accfloat acc1, v16accfloat acc2 ) { return addmac_4x8_8x4_accuracy_safe( v1, v2, acc1, acc2 );}
inline v16accfloat addmsc_4x8_8x4( v32float v1, v32float v2, v16accfloat acc1, v16accfloat acc2 ) { return addmsc_4x8_8x4_accuracy_safe( v1, v2, acc1, acc2 );}
#endif

#endif

#endif // __AIEV2_CORE_H
