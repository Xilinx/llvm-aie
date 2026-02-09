//===-------------------- AIEngine AIE2ps intrinsics ------------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2PS_VMULT_EMULATED_H__
#define __AIE2PS_VMULT_EMULATED_H__

#define EMUL_32x16_MAC_F(ACC, ELEM, DATAX, DATAY, SIGNX, SIGNY, NAME, GROUP,   \
                         A1_DOC, BRIEF_DOC)                                    \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mul_, NAME)                                                           \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() DATAY b) {             \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(mul_, NAME)(hi, b);                                     \
    acc = PP_CAT3(mac_, NAME, _conf)(lo, false, b, SIGNY, acc, 0, 1, 0, 0);    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(negmul_, NAME)                                                        \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() DATAY b) {             \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(negmul_, NAME)(hi, b);                                  \
    acc = PP_CAT3(msc_, NAME, _conf)(lo, false, b, SIGNY, acc, 0, 1, 0, 0);    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mac_, NAME)                                                           \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() DATAY b, ACC acc1) {   \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(mul_, NAME)(hi, b);                                     \
    acc = PP_CAT3(addmac_, NAME, _conf)(lo, false, b, SIGNY, acc, acc1, 0, 1,  \
                                        0, 0, 0);                              \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(msc_, NAME)                                                           \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() DATAY b, ACC acc1) {   \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(negmul_, NAME)(hi, b); /*-hi*/                          \
    acc = PP_CAT3(addmsc_, NAME, _conf)(lo, false, b, SIGNY, acc, acc1, 0, 1,  \
                                        0, 0, 0); /*( +(-hi) + acc1 - lo */    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mul_, NAME)                                                           \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() int sgn_x, DATAY b,    \
   int sgn_y) {                                                                \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(mul_, NAME)(hi, sgn_x, b, sgn_y);                       \
    acc = PP_CAT3(mac_, NAME, _conf)(lo, false, b, sgn_y, acc, 0, 1, 0, 0);    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(negmul_, NAME)                                                        \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() int sgn_x, DATAY b,    \
   int sgn_y) {                                                                \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(negmul_, NAME)(hi, sgn_x, b, sgn_y);                    \
    acc = PP_CAT3(msc_, NAME, _conf)(lo, false, b, sgn_y, acc, 0, 1, 0, 0);    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mac_, NAME)                                                           \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() int sgn_x, DATAY b,    \
   int sgn_y, ACC acc1) {                                                      \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(mul_, NAME)(hi, sgn_x, b, sgn_y);                       \
    acc = PP_CAT3(addmac_, NAME, _conf)(lo, false, b, sgn_y, acc, acc1, 0, 1,  \
                                        0, 0, 0);                              \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(msc_, NAME)                                                           \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() int sgn_x, DATAY b,    \
   int sgn_y, ACC acc1) {                                                      \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(negmul_, NAME)(hi, sgn_x, b, sgn_y); /*-hi*/            \
    acc = PP_CAT3(addmsc_, NAME, _conf)(lo, false, b, sgn_y, acc, acc1, 0, 1,  \
                                        0, 0, 0); /*( +(-hi) + acc1 - lo */    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _conf)                                                   \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() DATAY b,               \
   int sub_mul) {                                                              \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(hi, b, sub_mul);                    \
    acc = PP_CAT3(mac_, NAME, _conf)(lo, false, b, SIGNY, acc, 0, 1, sub_mul,  \
                                     0);                                       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(negmul_, NAME, _conf)                                                \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() DATAY b,               \
   int sub_mul) {                                                              \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(hi, b, sub_mul);                 \
    acc = PP_CAT3(msc_, NAME, _conf)(lo, false, b, SIGNY, acc, 0, 1, sub_mul,  \
                                     0);                                       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() DATAY b, ACC acc1,     \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(hi, b, sub_mul);                    \
    acc = PP_CAT3(addmac_, NAME, _conf)(lo, false, b, SIGNY, acc, acc1, 0, 1,  \
                                        sub_mul, 0, sub_acc1);                 \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() DATAY b, ACC acc1,     \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(hi, b, sub_mul);                 \
    acc = PP_CAT3(addmsc_, NAME, _conf)(lo, false, b, SIGNY, acc, acc1, 0, 1,  \
                                        sub_mul, 0, sub_acc1);                 \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _conf)                                                   \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() int sgn_x, DATAY b,    \
   int sgn_y, int sub_mul) {                                                   \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(hi, sgn_x, b, sgn_y, sub_mul);      \
    acc = PP_CAT3(mac_, NAME, _conf)(lo, false, b, sgn_y, acc, 0, 1, sub_mul,  \
                                     0);                                       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(negmul_, NAME, _conf)                                                \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() int sgn_x, DATAY b,    \
   int sgn_y, int sub_mul) {                                                   \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(hi, sgn_x, b, sgn_y, sub_mul);   \
    acc = PP_CAT3(msc_, NAME, _conf)(lo, false, b, sgn_y, acc, 0, 1, sub_mul,  \
                                     0);                                       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() int sgn_x, DATAY b,    \
   int sgn_y, ACC acc1, int sub_mul, int sub_acc1) {                           \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(hi, sgn_x, b, sgn_y, sub_mul);      \
    acc = PP_CAT3(addmac_, NAME, _conf)(lo, false, b, sgn_y, acc, acc1, 0, 1,  \
                                        sub_mul, 0, sub_acc1);                 \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() int sgn_x, DATAY b,    \
   int sgn_y, ACC acc1, int sub_mul, int sub_acc1) {                           \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(hi, sgn_x, b, sgn_y, sub_mul);   \
    acc = PP_CAT3(addmsc_, NAME, _conf)(lo, false, b, sgn_y, acc, acc1, 0, 1,  \
                                        sub_mul, 0, sub_acc1);                 \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() DATAY b, ACC acc1,     \
   int zero_acc1, int sub_mul, int sub_acc1) {                                 \
    if (__builtin_constant_p(zero_acc1) && (zero_acc1 == 0)) {                 \
      return PP_CAT3(mac_, NAME,                                               \
                     _conf)(a0, PP_IF(ELEM, a1 PP_COMMA, PP_EMPTY)() b, acc1,  \
                            sub_mul, sub_acc1);                                \
    }                                                                          \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(hi, b, sub_mul);                    \
    acc = ::PP_CAT3(mac_, NAME, _conf)(lo, false, b, SIGNY, acc, 0, 1,         \
                                       sub_mul, 0);                            \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() DATAY b, ACC acc1,     \
   int zero_acc1, int sub_mul, int sub_acc1) {                                 \
    if (__builtin_constant_p(zero_acc1) && (zero_acc1 == 0)) {                 \
      return PP_CAT3(msc_, NAME,                                               \
                     _conf)(a0, PP_IF(ELEM, a1 PP_COMMA, PP_EMPTY)() b, acc1,  \
                            sub_mul, sub_acc1);                                \
    }                                                                          \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(hi, b, sub_mul);                 \
    acc = ::PP_CAT3(mac_, NAME, _conf)(lo, false, b, SIGNY, acc, 0, 1,         \
                                       sub_mul, 0);                            \
    acc = ::sub_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() int sgn_x, DATAY b,    \
   int sgn_y, ACC acc1, int zero_acc1, int sub_mul, int sub_acc1) {            \
    if (__builtin_constant_p(zero_acc1) && (zero_acc1 == 0)) {                 \
      return PP_CAT3(mac_, NAME,                                               \
                     _conf)(a0, PP_IF(ELEM, a1 PP_COMMA, PP_EMPTY)() sgn_x, b, \
                            sgn_y, acc1, sub_mul, sub_acc1);                   \
    }                                                                          \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(hi, sgn_x, b, sgn_y, sub_mul);      \
    acc = ::PP_CAT3(mac_, NAME, _conf)(lo, false, b, sgn_y, acc, 0, 1,         \
                                       sub_mul, 0);                            \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, PP_IF(ELEM, DATAX a1 PP_COMMA, PP_EMPTY)() int sgn_x, DATAY b,    \
   int sgn_y, ACC acc1, int zero_acc1, int sub_mul, int sub_acc1) {            \
    if (__builtin_constant_p(zero_acc1) && (zero_acc1 == 0)) {                 \
      return PP_CAT3(msc_, NAME,                                               \
                     _conf)(a0, PP_IF(ELEM, a1 PP_COMMA, PP_EMPTY)() sgn_x, b, \
                            sgn_y, acc1, sub_mul, sub_acc1);                   \
    }                                                                          \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_lo);            \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                           \
        a0, PP_IF(ELEM, a1, PP_CAT(undef_, DATAX)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(hi, sgn_x, b, sgn_y, sub_mul);   \
    acc = ::PP_CAT3(mac_, NAME, _conf)(lo, false, b, sgn_y, acc, 0, 1,         \
                                       sub_mul, 0);                            \
    acc = ::sub_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }

#define EMUL_32x16_SIGNED_MAC_F(ACC, ELEM, DATAX_1, DATAX_2, DATAY_1, DATAY_2, \
                                NAME, GROUP, A1_DOC, BRIEF_DOC)                \
  EMUL_32x16_MAC_F(ACC, ELEM, PP_CAT3(DATAX_1, u, DATAX_2),                    \
                   PP_CAT3(DATAY_1, u, DATAY_2), 0, 0, NAME, GROUP, A1_DOC,    \
                   BRIEF_DOC);                                                 \
  EMUL_32x16_MAC_F(ACC, ELEM, PP_CAT3(DATAX_1, u, DATAX_2),                    \
                   PP_CAT3(DATAY_1, , DATAY_2), 0, 1, NAME, GROUP, A1_DOC,     \
                   BRIEF_DOC);                                                 \
  EMUL_32x16_MAC_F(ACC, ELEM, PP_CAT3(DATAX_1, , DATAX_2),                     \
                   PP_CAT3(DATAY_1, u, DATAY_2), 1, 0, NAME, GROUP, A1_DOC,    \
                   BRIEF_DOC);                                                 \
  EMUL_32x16_MAC_F(ACC, ELEM, PP_CAT3(DATAX_1, , DATAX_2),                     \
                   PP_CAT3(DATAY_1, , DATAY_2), 1, 1, NAME, GROUP, A1_DOC,     \
                   BRIEF_DOC);

#define EMUL_32x16_MAC_F_1KB(ACC, ELEM, DATAX, DATAY, SIGNX, SIGNY, NAME,      \
                             GROUP, A1_DOC, BRIEF_DOC)                         \
  INTRINSIC(ACC) PP_CAT(mul_, NAME)(DATAX a0, DATAX a1, DATAY b) {             \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(mul_, NAME)(concat(hi0, hi1), b);                       \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(lo0, lo1), false, b, SIGNY, acc,   \
                                     0, 1, 0, 0);                              \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(negmul_, NAME)(DATAX a0, DATAX a1, DATAY b) {          \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(negmul_, NAME)(concat(hi0, hi1), b);                    \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(lo0, lo1), false, b, SIGNY, acc,   \
                                     0, 1, 0, 0);                              \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(mac_, NAME)(DATAX a0, DATAX a1, DATAY b, ACC acc1) {   \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(mul_, NAME)(concat(hi0, hi1), b);                       \
    acc = PP_CAT3(addmac_, NAME, _conf)(concat(lo0, lo1), false, b, SIGNY,     \
                                        acc, acc1, 0, 1, 0, 0, 0);             \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(msc_, NAME)(DATAX a0, DATAX a1, DATAY b, ACC acc1) {   \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(negmul_, NAME)(concat(hi0, hi1), b); /*-hi*/            \
    acc = PP_CAT3(addmsc_, NAME, _conf)(concat(lo0, lo1), false, b, SIGNY,     \
                                        acc, acc1, 0, 1, 0, 0,                 \
                                        0); /*( +(-hi) + acc1 - lo */          \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mul_, NAME)(DATAX a0, DATAX a1, int sgn_x, DATAY b, int sgn_y) {      \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(mul_, NAME)(concat(hi0, hi1), sgn_x, b, sgn_y);         \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(lo0, lo1), false, b, sgn_y, acc,   \
                                     0, 1, 0, 0);                              \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(negmul_, NAME)(DATAX a0, DATAX a1, int sgn_x, DATAY b, int sgn_y) {   \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(negmul_, NAME)(concat(hi0, hi1), sgn_x, b, sgn_y);      \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(lo0, lo1), false, b, sgn_y, acc,   \
                                     0, 1, 0, 0);                              \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mac_, NAME)                                                           \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b, int sgn_y, ACC acc1) {              \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(mul_, NAME)(concat(hi0, hi1), sgn_x, b, sgn_y);         \
    acc = PP_CAT3(addmac_, NAME, _conf)(concat(lo0, lo1), false, b, sgn_y,     \
                                        acc, acc1, 0, 1, 0, 0, 0);             \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(msc_, NAME)                                                           \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b, int sgn_y, ACC acc1) {              \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc =                                                                  \
        ::PP_CAT(negmul_, NAME)(concat(hi0, hi1), sgn_x, b, sgn_y); /*-hi*/    \
    acc = PP_CAT3(addmsc_, NAME, _conf)(concat(lo0, lo1), false, b, sgn_y,     \
                                        acc, acc1, 0, 1, 0, 0,                 \
                                        0); /*( +(-hi) + acc1 - lo */          \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _conf)(DATAX a0, DATAX a1, DATAY b, int sub_mul) {       \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(concat(hi0, hi1), b, sub_mul);      \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(lo0, lo1), false, b, SIGNY, acc,   \
                                     0, 1, sub_mul, 0);                        \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(negmul_, NAME, _conf)(DATAX a0, DATAX a1, DATAY b, int sub_mul) {    \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(concat(hi0, hi1), b, sub_mul);   \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(lo0, lo1), false, b, SIGNY, acc,   \
                                     0, 1, sub_mul, 0);                        \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, DATAY b, ACC acc1, int sub_mul, int sub_acc1) {         \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(concat(hi0, hi1), b, sub_mul);      \
    acc =                                                                      \
        PP_CAT3(addmac_, NAME, _conf)(concat(lo0, lo1), false, b, SIGNY, acc,  \
                                      acc1, 0, 1, sub_mul, 0, sub_acc1);       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, DATAY b, ACC acc1, int sub_mul, int sub_acc1) {         \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(concat(hi0, hi1), b, sub_mul);   \
    acc =                                                                      \
        PP_CAT3(addmsc_, NAME, _conf)(concat(lo0, lo1), false, b, SIGNY, acc,  \
                                      acc1, 0, 1, sub_mul, 0, sub_acc1);       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b, int sgn_y, int sub_mul) {           \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(concat(hi0, hi1), sgn_x, b, sgn_y,  \
                                           sub_mul);                           \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(lo0, lo1), false, b, sgn_y, acc,   \
                                     0, 1, sub_mul, 0);                        \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(negmul_, NAME, _conf)                                                \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b, int sgn_y, int sub_mul) {           \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(concat(hi0, hi1), sgn_x, b,      \
                                              sgn_y, sub_mul);                 \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(lo0, lo1), false, b, sgn_y, acc,   \
                                     0, 1, sub_mul, 0);                        \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b, int sgn_y, ACC acc1, int sub_mul,   \
   int sub_acc1) {                                                             \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(concat(hi0, hi1), sgn_x, b, sgn_y,  \
                                           sub_mul);                           \
    acc =                                                                      \
        PP_CAT3(addmac_, NAME, _conf)(concat(lo0, lo1), false, b, sgn_y, acc,  \
                                      acc1, 0, 1, sub_mul, 0, sub_acc1);       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b, int sgn_y, ACC acc1, int sub_mul,   \
   int sub_acc1) {                                                             \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(concat(hi0, hi1), sgn_x, b,      \
                                              sgn_y, sub_mul);                 \
    acc =                                                                      \
        PP_CAT3(addmsc_, NAME, _conf)(concat(lo0, lo1), false, b, sgn_y, acc,  \
                                      acc1, 0, 1, sub_mul, 0, sub_acc1);       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, DATAY b, ACC acc1, int zero_acc1, int sub_mul,          \
   int sub_acc1) {                                                             \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(concat(hi0, hi1), b, sub_mul);      \
    acc = ::PP_CAT3(mac_, NAME, _conf)(concat(lo0, lo1), false, b, SIGNY, acc, \
                                       0, 1, sub_mul, 0);                      \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, DATAY b, ACC acc1, int zero_acc1, int sub_mul,          \
   int sub_acc1) {                                                             \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(concat(hi0, hi1), b, sub_mul);   \
    acc = ::PP_CAT3(mac_, NAME, _conf)(concat(lo0, lo1), false, b, SIGNY, acc, \
                                       0, 1, sub_mul, 0);                      \
    acc = ::sub_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b, int sgn_y, ACC acc1, int zero_acc1, \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(concat(hi0, hi1), sgn_x, b, sgn_y,  \
                                           sub_mul);                           \
    acc = ::PP_CAT3(mac_, NAME, _conf)(concat(lo0, lo1), false, b, sgn_y, acc, \
                                       0, 1, sub_mul, 0);                      \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b, int sgn_y, ACC acc1, int zero_acc1, \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(concat(hi0, hi1), sgn_x, b,      \
                                              sgn_y, sub_mul);                 \
    acc = ::PP_CAT3(mac_, NAME, _conf)(concat(lo0, lo1), false, b, sgn_y, acc, \
                                       0, 1, sub_mul, 0);                      \
    acc = ::sub_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }

#define EMUL_32x16_SIGNED_MAC_F_1KB(ACC, ELEM, DATAX_1, DATAX_2, DATAY_1,      \
                                    DATAY_2, NAME, GROUP, A1_DOC, BRIEF_DOC)   \
  EMUL_32x16_MAC_F_1KB(ACC, ELEM, PP_CAT3(DATAX_1, u, DATAX_2),                \
                       PP_CAT3(DATAY_1, u, DATAY_2), 0, 0, NAME, GROUP,        \
                       A1_DOC, BRIEF_DOC);                                     \
  EMUL_32x16_MAC_F_1KB(ACC, ELEM, PP_CAT3(DATAX_1, u, DATAX_2),                \
                       PP_CAT3(DATAY_1, , DATAY_2), 0, 1, NAME, GROUP, A1_DOC, \
                       BRIEF_DOC);                                             \
  EMUL_32x16_MAC_F_1KB(ACC, ELEM, PP_CAT3(DATAX_1, , DATAX_2),                 \
                       PP_CAT3(DATAY_1, u, DATAY_2), 1, 0, NAME, GROUP,        \
                       A1_DOC, BRIEF_DOC);                                     \
  EMUL_32x16_MAC_F_1KB(ACC, ELEM, PP_CAT3(DATAX_1, , DATAX_2),                 \
                       PP_CAT3(DATAY_1, , DATAY_2), 1, 1, NAME, GROUP, A1_DOC, \
                       BRIEF_DOC);

EMUL_32x16_SIGNED_MAC_F(v64acc32, 0, v16, int32, v32, int16, 8x2_2x8,
                        intr_gpvectorop_emul_32bx16b, ,
                        Emulated Multiplication of(8x2) with(2x8)
                            Matrices(32b * 16b))
    // EMUL_32x16_SIGNED_MAC_F(v64acc32, 0, v32, int32, v64, int16, elem_64
    // ,intr_gpvectorop_emul_32bx16b, , Emulated Channel by channel
    // multiplication of (1x1) with (1x1) (32b * 16b))

    EMUL_32x16_SIGNED_MAC_F(
        v32acc64, 0, v16, int32, v32, int16, 4x4_4x8,
        intr_gpvectorop_emul_32bx16b, ,
        Emulated Channel by channel multiplication of(1x1) with(1x1)(
            32b * 16b)) EMUL_32x16_SIGNED_MAC_F(v32acc64, 1, v16, int32, v32,
                                                int16, elem_32,
                                                intr_gpvectorop_emul_32bx16b, ,
                                                Emulated Channel by channel
                                                    multiplication of(1x1)
                                                        with(1x1)(32b * 16b))

        EMUL_32x16_SIGNED_MAC_F_1KB(v32acc64, 1, v32, int32, v64, int16,
                                    elem_32_2, intr_gpvectorop_emul_32bx16b, ,
                                    Emulated Channel by channel multiplication
                                        of(1x2) with(2x1)(32b * 16b))
            EMUL_32x16_SIGNED_MAC_F_1KB(
                v32acc64, 0, v32, int32, v32, int16, conv_4x4_8ch,
                intr_gpvectorop_emul_32bx16b, ,
                Emulated Channel by channel multiplication of(1x1)
                    with(1x1)(32b * 16b))

#undef EMUL_32x16_MAC_F
#undef EMUL_32x16_SIGNED_MAC_F

#undef EMUL_32x16_SIGNED_MAC_F_1KB
#undef EMUL_32x16_MAC_F_1KB

#define EMUL_32x32_ELEM_MAC_F(ACC, ELEM, DATAX, DATAY, SIGNX, SIGNY, NAME,     \
                              GROUP, BRIEF_DOC)                                \
  INTRINSIC(ACC) PP_CAT(mul_, NAME)(DATAX a0, DATAX a1, DATAY b0, DATAY b1) {  \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    /*Doing ( ( hi*hi << 16 ) + hi * lo ) + lo * hi ) << 16 + lo*lo */         \
    ACC acc = ::PP_CAT(mul_, NAME)(a_hi, b_hi);                                \
    acc = PP_CAT3(mac_, NAME, _conf)(a_hi, SIGNX, b_lo, false, acc, 0, 1, 0,   \
                                     0); /*shift high part by 16 */            \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_hi, SIGNY, acc, 0, 0, 0,   \
                                     0); /* no shift */                        \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_lo, false, acc, 0, 1, 0,   \
                                     0); /* shift by 16 */                     \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(negmul_, NAME)(DATAX a0, DATAX a1, DATAY b0, DATAY b1) {              \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT(negmul_, NAME)(a_hi, b_hi);                             \
    acc = PP_CAT3(msc_, NAME, _conf)(a_hi, SIGNX, b_lo, false, acc, 0, 1, 0,   \
                                     0); /*shift high part by 16 */            \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_hi, SIGNY, acc, 0, 0, 0,   \
                                     0); /* no shift */                        \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_lo, false, acc, 0, 1, 0,   \
                                     0); /* shift by 16 */                     \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mac_, NAME)(DATAX a0, DATAX a1, DATAY b0, DATAY b1, ACC acc1) {       \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT(mul_, NAME)(a_hi, b_hi);                                \
    acc = PP_CAT3(mac_, NAME, _conf)(a_hi, SIGNX, b_lo, false, acc, 0, 1, 0,   \
                                     0); /*shift high part by 16 */            \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_hi, SIGNY, acc, 0, 0, 0,   \
                                     0); /* no shift */                        \
    acc = PP_CAT3(addmac_, NAME, _conf)(a_lo, false, b_lo, false, acc, acc1,   \
                                        0, 1, 0, 0, 0); /* shift by 16 */      \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(msc_, NAME)(DATAX a0, DATAX a1, DATAY b0, DATAY b1, ACC acc1) {       \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT(negmul_, NAME)(a_hi, b_hi);                             \
    acc = PP_CAT3(msc_, NAME, _conf)(a_hi, SIGNX, b_lo, false, acc, 0, 1, 0,   \
                                     0); /*shift high part by 16 */            \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_hi, SIGNY, acc, 0, 0, 0,   \
                                     0); /* no shift */                        \
    acc = PP_CAT3(addmsc_, NAME, _conf)(a_lo, false, b_lo, false, acc, acc1,   \
                                        0, 1, 0, 0, 0); /* shift by 16 */      \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mul_, NAME)                                                           \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y) {             \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT(mul_, NAME)(a_hi, sgn_x, b_hi, sgn_y);                  \
    acc = PP_CAT3(mac_, NAME, _conf)(a_hi, sgn_x, b_lo, false, acc, 0, 1, 0,   \
                                     0); /*shift high part by 16 */            \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_hi, sgn_y, acc, 0, 0, 0,   \
                                     0); /* no shift */                        \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_lo, false, acc, 0, 1, 0,   \
                                     0); /* shift by 16 */                     \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(negmul_, NAME)                                                        \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y) {             \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT(negmul_, NAME)(a_hi, sgn_x, b_hi, sgn_y);               \
    acc = PP_CAT3(msc_, NAME, _conf)(a_hi, sgn_x, b_lo, false, acc, 0, 1, 0,   \
                                     0); /*shift high part by 16 */            \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_hi, sgn_y, acc, 0, 0, 0,   \
                                     0); /* no shift */                        \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_lo, false, acc, 0, 1, 0,   \
                                     0); /* shift by 16 */                     \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mac_, NAME)                                                           \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1) {   \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT(mul_, NAME)(a_hi, sgn_x, b_hi, sgn_y);                  \
    acc = PP_CAT3(mac_, NAME, _conf)(a_hi, sgn_x, b_lo, false, acc, 0, 1, 0,   \
                                     0); /*shift high part by 16 */            \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_hi, sgn_y, acc, 0, 0, 0,   \
                                     0); /* no shift */                        \
    acc = PP_CAT3(addmac_, NAME, _conf)(a_lo, false, b_lo, false, acc, acc1,   \
                                        0, 1, 0, 0, 0); /* shift by 16 */      \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(msc_, NAME)                                                           \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1) {   \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT(negmul_, NAME)(a_hi, sgn_x, b_hi, sgn_y);               \
    acc = PP_CAT3(msc_, NAME, _conf)(a_hi, sgn_x, b_lo, false, acc, 0, 1, 0,   \
                                     0); /*shift high part by 16 */            \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_hi, sgn_y, acc, 0, 0, 0,   \
                                     0); /* no shift */                        \
    acc = PP_CAT3(addmsc_, NAME, _conf)(a_lo, false, b_lo, false, acc, acc1,   \
                                        0, 1, 0, 0, 0); /* shift by 16 */      \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, DATAY b0, DATAY b1, int sub_mul) {                      \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a_hi, b_hi, sub_mul);               \
    acc = PP_CAT3(mac_, NAME, _conf)(a_hi, SIGNX, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_hi, SIGNY, acc, 0, 0,      \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /* shift by 16 */            \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(negmul_, NAME, _conf)                                                \
  (DATAX a0, DATAX a1, DATAY b0, DATAY b1, int sub_mul) {                      \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a_hi, b_hi, sub_mul);            \
    acc = PP_CAT3(msc_, NAME, _conf)(a_hi, SIGNX, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_hi, SIGNY, acc, 0, 0,      \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /* shift by 16 */            \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, DATAY b0, DATAY b1, ACC acc1, int sub_mul,              \
   int sub_acc1) {                                                             \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a_hi, b_hi, sub_mul);               \
    acc = PP_CAT3(mac_, NAME, _conf)(a_hi, SIGNX, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_hi, SIGNY, acc, 0, 0,      \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(addmac_, NAME, _conf)(a_lo, false, b_lo, false, acc, acc1,   \
                                        0, 1, sub_mul, 0,                      \
                                        sub_acc1); /* shift by 16 */           \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, DATAY b0, DATAY b1, ACC acc1, int sub_mul,              \
   int sub_acc1) {                                                             \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a_hi, b_hi, sub_mul);            \
    acc = PP_CAT3(msc_, NAME, _conf)(a_hi, SIGNX, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_hi, SIGNY, acc, 0, 0,      \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(addmsc_, NAME, _conf)(a_lo, false, b_lo, false, acc, acc1,   \
                                        0, 1, sub_mul, 0,                      \
                                        sub_acc1); /* shift by 16 */           \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y,               \
   int sub_mul) {                                                              \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a_hi, sgn_x, b_hi, sgn_y, sub_mul); \
    acc = PP_CAT3(mac_, NAME, _conf)(a_hi, sgn_x, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_hi, sgn_y, acc, 0, 0,      \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /* shift by 16 */            \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(negmul_, NAME, _conf)                                                \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y,               \
   int sub_mul) {                                                              \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc =                                                                  \
        ::PP_CAT3(negmul_, NAME, _conf)(a_hi, sgn_x, b_hi, sgn_y, sub_mul);    \
    acc = PP_CAT3(msc_, NAME, _conf)(a_hi, sgn_x, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_hi, sgn_y, acc, 0, 0,      \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /* shift by 16 */            \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1,     \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a_hi, sgn_x, b_hi, sgn_y, sub_mul); \
    acc = PP_CAT3(mac_, NAME, _conf)(a_hi, sgn_x, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_hi, sgn_y, acc, 0, 0,      \
                                     sub_mul, 0); /* no shift */               \
    acc =                                                                      \
        PP_CAT3(addmac_, NAME, _conf)(a_lo, false, b_lo, false, acc, acc1, 0,  \
                                      1, sub_mul, 0, sub_acc1); /* shift 16 */ \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1,     \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc =                                                                  \
        ::PP_CAT3(negmul_, NAME, _conf)(a_hi, sgn_x, b_hi, sgn_y, sub_mul);    \
    acc = PP_CAT3(msc_, NAME, _conf)(a_hi, sgn_x, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_hi, sgn_y, acc, 0, 0,      \
                                     sub_mul, 0); /* no shift */               \
    acc =                                                                      \
        PP_CAT3(addmsc_, NAME, _conf)(a_lo, false, b_lo, false, acc, acc1, 0,  \
                                      1, sub_mul, 0, sub_acc1); /* shift 16 */ \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, DATAY b0, DATAY b1, ACC acc1, int zero_acc1,            \
   int sub_mul, int sub_acc1) {                                                \
    if (__builtin_constant_p(zero_acc1) && (zero_acc1 == 0)) {                 \
      return PP_CAT3(mac_, NAME, _conf)(a0, a1, b0, b1, acc1, sub_mul,         \
                                        sub_acc1);                             \
    }                                                                          \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a_hi, b_hi, sub_mul);               \
    acc = PP_CAT3(mac_, NAME, _conf)(a_hi, SIGNX, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_hi, SIGNY, acc, 0, 0,      \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0);                              \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, DATAY b0, DATAY b1, ACC acc1, int zero_acc1,            \
   int sub_mul, int sub_acc1) {                                                \
    if (__builtin_constant_p(zero_acc1) && (zero_acc1 == 0)) {                 \
      return PP_CAT3(msc_, NAME, _conf)(a0, a1, b0, b1, acc1, sub_mul,         \
                                        sub_acc1);                             \
    }                                                                          \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a_hi, b_hi, sub_mul);            \
    acc = PP_CAT3(msc_, NAME, _conf)(a_hi, SIGNX, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /* shift high part by 16 */  \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_hi, SIGNY, acc, 0, 0,      \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0);                              \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1,     \
   int zero_acc1, int sub_mul, int sub_acc1) {                                 \
    if (__builtin_constant_p(zero_acc1) && (zero_acc1 == 0)) {                 \
      return PP_CAT3(mac_, NAME, _conf)(a0, a1, sgn_x, b0, b1, sgn_y, acc1,    \
                                        sub_mul, sub_acc1);                    \
    }                                                                          \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a_hi, sgn_x, b_hi, sgn_y, sub_mul); \
    acc = PP_CAT3(mac_, NAME, _conf)(a_hi, sgn_x, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /* shift high part by 16 */  \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_hi, sgn_y, acc, 0, 0,      \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(mac_, NAME, _conf)(a_lo, false, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0);                              \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1,     \
   int zero_acc1, int sub_mul, int sub_acc1) {                                 \
    if (__builtin_constant_p(zero_acc1) && (zero_acc1 == 0)) {                 \
      return PP_CAT3(msc_, NAME, _conf)(a0, a1, sgn_x, b0, b1, sgn_y, acc1,    \
                                        sub_mul, sub_acc1);                    \
    }                                                                          \
    v32uint16 a_lo = (v32uint16)shuffle(a0, a1, T16_32x2_lo);                  \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(a0, a1, T16_32x2_hi);    \
    v32uint16 b_lo = (v32uint16)shuffle(b0, b1, T16_32x2_lo);                  \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(b0, b1, T16_32x2_hi);    \
    ACC acc =                                                                  \
        ::PP_CAT3(negmul_, NAME, _conf)(a_hi, sgn_x, b_hi, sgn_y, sub_mul);    \
    acc = PP_CAT3(msc_, NAME, _conf)(a_hi, sgn_x, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_hi, sgn_y, acc, 0, 0,      \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(msc_, NAME, _conf)(a_lo, false, b_lo, false, acc, 0, 1,      \
                                     sub_mul, 0);                              \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }

#define EMUL_32x32_ELEM_SIGNED_MAC_F(ACC, ELEM, DATAX_1, DATAX_2, DATAY_1,     \
                                     DATAY_2, NAME, GROUP, BRIEF_DOC)          \
  EMUL_32x32_ELEM_MAC_F(ACC, ELEM, PP_CAT3(DATAX_1, u, DATAX_2),               \
                        PP_CAT3(DATAY_1, u, DATAY_2), 0, 0, NAME, GROUP,       \
                        BRIEF_DOC);                                            \
  EMUL_32x32_ELEM_MAC_F(ACC, ELEM, PP_CAT3(DATAX_1, u, DATAX_2),               \
                        PP_CAT3(DATAY_1, , DATAY_2), 0, 1, NAME, GROUP,        \
                        BRIEF_DOC);                                            \
  EMUL_32x32_ELEM_MAC_F(ACC, ELEM, PP_CAT3(DATAX_1, , DATAX_2),                \
                        PP_CAT3(DATAY_1, u, DATAY_2), 1, 0, NAME, GROUP,       \
                        BRIEF_DOC);                                            \
  EMUL_32x32_ELEM_MAC_F(ACC, ELEM, PP_CAT3(DATAX_1, , DATAX_2),                \
                        PP_CAT3(DATAY_1, , DATAY_2), 1, 1, NAME, GROUP,        \
                        BRIEF_DOC);

#define EMUL_32x32_16x32_MAC_F(ACC, ELEM, DATAX, DATAY, SIGNX, SIGNY, NAME,    \
                               GROUP, B1_DOC, BRIEF_DOC)                       \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mul_, NAME)                                                           \
  (DATAX a,                                                                    \
   DATAY b0 PP_IF(ELEM, PP_COMMA, PP_EMPTY)() PP_IF(ELEM, DATAY b1, )) {       \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(mul_, NAME)(a, hi);                                     \
    acc = PP_CAT3(mac_, NAME, _conf)(a, SIGNX, lo, false, acc, 0, 1, 0, 0);    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(negmul_, NAME)                                                        \
  (DATAX a,                                                                    \
   DATAY b0 PP_IF(ELEM, PP_COMMA, PP_EMPTY)() PP_IF(ELEM, DATAY b1, )) {       \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(negmul_, NAME)(a, hi);                                  \
    acc = PP_CAT3(msc_, NAME, _conf)(a, SIGNX, lo, false, acc, 0, 1, 0, 0);    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mac_, NAME)                                                           \
  (DATAX a, DATAY b0, PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() ACC acc1) {   \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(mul_, NAME)(a, hi);                                     \
    acc = PP_CAT3(addmac_, NAME, _conf)(a, SIGNX, lo, false, acc, acc1, 0, 1,  \
                                        0, 0, 0);                              \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(msc_, NAME)                                                           \
  (DATAX a, DATAY b0, PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() ACC acc1) {   \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(negmul_, NAME)(a, hi); /*-hi*/                          \
    acc = PP_CAT3(addmsc_, NAME, _conf)(a, SIGNX, lo, false, acc, acc1, 0, 1,  \
                                        0, 0, 0); /*( +(-hi) + acc1 - lo */    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mul_, NAME)                                                           \
  (DATAX a, int sgn_x, DATAY b0,                                               \
   PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() int sgn_y) {                     \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(mul_, NAME)(a, sgn_x, hi, sgn_y);                       \
    acc = PP_CAT3(mac_, NAME, _conf)(a, sgn_x, lo, false, acc, 0, 1, 0, 0);    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(negmul_, NAME)                                                        \
  (DATAX a, int sgn_x, DATAY b0,                                               \
   PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() int sgn_y) {                     \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(negmul_, NAME)(a, sgn_x, hi, sgn_y);                    \
    acc = PP_CAT3(msc_, NAME, _conf)(a, sgn_x, lo, false, acc, 0, 1, 0, 0);    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mac_, NAME)                                                           \
  (DATAX a, int sgn_x, DATAY b0,                                               \
   PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() int sgn_y, ACC acc1) {           \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(mul_, NAME)(a, sgn_x, hi, sgn_y);                       \
    acc = PP_CAT3(addmac_, NAME, _conf)(a, sgn_x, lo, false, acc, acc1, 0, 1,  \
                                        0, 0, 0);                              \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(msc_, NAME)                                                           \
  (DATAX a, int sgn_x, DATAY b0,                                               \
   PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() int sgn_y, ACC acc1) {           \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT(negmul_, NAME)(a, sgn_x, hi, sgn_y); /*-hi*/            \
    acc = PP_CAT3(addmsc_, NAME, _conf)(a, sgn_x, lo, false, acc, acc1, 0, 1,  \
                                        0, 0, 0); /*( +(-hi) + acc1 - lo */    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _conf)                                                   \
  (DATAX a, DATAY b0,                                                          \
   PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() int sub_mul) {                   \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, hi, sub_mul);                    \
    acc = PP_CAT3(mac_, NAME, _conf)(a, SIGNX, lo, false, acc, 0, 1, sub_mul,  \
                                     0);                                       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(negmul_, NAME, _conf)                                                \
  (DATAX a, DATAY b0,                                                          \
   PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() int sub_mul) {                   \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, hi, sub_mul);                 \
    acc = PP_CAT3(msc_, NAME, _conf)(a, SIGNX, lo, false, acc, 0, 1, sub_mul,  \
                                     0);                                       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a, DATAY b0, PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() ACC acc1,     \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, hi, sub_mul);                    \
    acc = PP_CAT3(addmac_, NAME, _conf)(a, SIGNX, lo, false, acc, acc1, 0, 1,  \
                                        sub_mul, 0, sub_acc1);                 \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a, DATAY b0, PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() ACC acc1,     \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, hi, sub_mul);                 \
    acc = PP_CAT3(addmsc_, NAME, _conf)(a, SIGNX, lo, false, acc, acc1, 0, 1,  \
                                        sub_mul, 0, sub_acc1);                 \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _conf)                                                   \
  (DATAX a, int sgn_x, DATAY b0,                                               \
   PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() int sgn_y, int sub_mul) {        \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, sgn_x, hi, sgn_y, sub_mul);      \
    acc = PP_CAT3(mac_, NAME, _conf)(a, sgn_x, lo, false, acc, 0, 1, sub_mul,  \
                                     0);                                       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(negmul_, NAME, _conf)                                                \
  (DATAX a, int sgn_x, DATAY b0,                                               \
   PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() int sgn_y, int sub_mul) {        \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, sgn_x, hi, sgn_y, sub_mul);   \
    acc = PP_CAT3(msc_, NAME, _conf)(a, sgn_x, lo, false, acc, 0, 1, sub_mul,  \
                                     0);                                       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a, int sgn_x, DATAY b0,                                               \
   PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() int sgn_y, ACC acc1,             \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, sgn_x, hi, sgn_y, sub_mul);      \
    acc = PP_CAT3(addmac_, NAME, _conf)(a, sgn_x, lo, false, acc, acc1, 0, 1,  \
                                        sub_mul, 0, sub_acc1);                 \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a, int sgn_x, DATAY b0,                                               \
   PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() int sgn_y, ACC acc1,             \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, sgn_x, hi, sgn_y, sub_mul);   \
    acc = PP_CAT3(addmsc_, NAME, _conf)(a, sgn_x, lo, false, acc, acc1, 0, 1,  \
                                        sub_mul, 0, sub_acc1);                 \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a, DATAY b0, PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() ACC acc1,     \
   int zero_acc1, int sub_mul, int sub_acc1) {                                 \
    if (__builtin_constant_p(zero_acc1) && (zero_acc1 == 0)) {                 \
      return PP_CAT3(mac_, NAME,                                               \
                     _conf)(a, b0, PP_IF(ELEM, b1 PP_COMMA, PP_EMPTY)() acc1,  \
                            sub_mul, sub_acc1);                                \
    }                                                                          \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, hi, sub_mul);                    \
    acc = ::PP_CAT3(mac_, NAME, _conf)(a, SIGNX, lo, false, acc, 0, 0x01,      \
                                       sub_mul, 0);                            \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a, DATAY b0, PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() ACC acc1,     \
   int zero_acc1, int sub_mul, int sub_acc1) {                                 \
    if (__builtin_constant_p(zero_acc1) && (zero_acc1 == 0)) {                 \
      return PP_CAT3(msc_, NAME,                                               \
                     _conf)(a, b0, PP_IF(ELEM, b1 PP_COMMA, PP_EMPTY)() acc1,  \
                            sub_mul, sub_acc1);                                \
    }                                                                          \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, hi, sub_mul);                 \
    acc = ::PP_CAT3(mac_, NAME, _conf)(a, SIGNX, lo, false, acc, 0, 0x01,      \
                                       sub_mul, 0);                            \
    acc = ::sub_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a, int sgn_x, DATAY b0,                                               \
   PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() int sgn_y, ACC acc1,             \
   int zero_acc1, int sub_mul, int sub_acc1) {                                 \
    if (__builtin_constant_p(zero_acc1) && (zero_acc1 == 0)) {                 \
      return PP_CAT3(mac_, NAME, _conf)(                                       \
          a, sgn_x, b0, PP_IF(ELEM, b1 PP_COMMA, PP_EMPTY)() sgn_y, acc1,      \
          sub_mul, sub_acc1);                                                  \
    }                                                                          \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, sgn_x, hi, sgn_y, sub_mul);      \
    acc = ::PP_CAT3(mac_, NAME, _conf)(a, sgn_x, lo, false, acc, 0, 1,         \
                                       sub_mul, 0);                            \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a, int sgn_x, DATAY b0,                                               \
   PP_IF(ELEM, DATAY b1 PP_COMMA, PP_EMPTY)() int sgn_y, ACC acc1,             \
   int zero_acc1, int sub_mul, int sub_acc1) {                                 \
    if (__builtin_constant_p(zero_acc1) && (zero_acc1 == 0)) {                 \
      return PP_CAT3(msc_, NAME, _conf)(                                       \
          a, sgn_x, b0, PP_IF(ELEM, b1 PP_COMMA, PP_EMPTY)() sgn_y, acc1,      \
          sub_mul, sub_acc1);                                                  \
    }                                                                          \
    v32uint16 lo = (v32uint16)shuffle(                                         \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_lo);            \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                           \
        b0, PP_IF(ELEM, b1, PP_CAT(undef_, DATAY)()), T16_32x2_hi);            \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, sgn_x, hi, sgn_y, sub_mul);   \
    acc = ::PP_CAT3(mac_, NAME, _conf)(a, sgn_x, lo, false, acc, 0, 1,         \
                                       sub_mul, 0);                            \
    acc = ::sub_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }

#define EMUL_32x32_16x32_SIGNED_MAC_F(ACC, ELEM, DATAX_1, DATAX_2, DATAY_1,    \
                                      DATAY_2, NAME, GROUP, A1_DOC, BRIEF_DOC) \
  EMUL_32x32_16x32_MAC_F(ACC, ELEM, PP_CAT3(DATAX_1, u, DATAX_2),              \
                         PP_CAT3(DATAY_1, u, DATAY_2), 0, 0, NAME, GROUP,      \
                         A1_DOC, BRIEF_DOC);                                   \
  EMUL_32x32_16x32_MAC_F(ACC, ELEM, PP_CAT3(DATAX_1, u, DATAX_2),              \
                         PP_CAT3(DATAY_1, , DATAY_2), 0, 1, NAME, GROUP,       \
                         A1_DOC, BRIEF_DOC);                                   \
  EMUL_32x32_16x32_MAC_F(ACC, ELEM, PP_CAT3(DATAX_1, , DATAX_2),               \
                         PP_CAT3(DATAY_1, u, DATAY_2), 1, 0, NAME, GROUP,      \
                         A1_DOC, BRIEF_DOC);                                   \
  EMUL_32x32_16x32_MAC_F(ACC, ELEM, PP_CAT3(DATAX_1, , DATAX_2),               \
                         PP_CAT3(DATAY_1, , DATAY_2), 1, 1, NAME, GROUP,       \
                         A1_DOC, BRIEF_DOC);

#define EMUL_32x32_ELEM_MAC_F_1KB(ACC, ELEM, DATAX, DATAY, SIGNX, SIGNY, NAME, \
                                  GROUP, BRIEF_DOC)                            \
  INTRINSIC(ACC) PP_CAT(mul_, NAME)(DATAX a0, DATAX a1, DATAY b0, DATAY b1) {  \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    /*Doing ( ( hi*hi << 16 ) + hi * lo ) + lo * hi ) << 16 + lo*lo */         \
    ACC acc =                                                                  \
        ::PP_CAT(mul_, NAME)(concat(a_hi0, a_hi1), concat(b_hi0, b_hi1));      \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_hi0, a_hi1), SIGNX,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     0, 0); /*shift high part by 16 */         \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), SIGNY, acc, 0, 0,   \
                                     0, 0); /* no shift */                     \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     0, 0); /* shift by 16 */                  \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(negmul_, NAME)(DATAX a0, DATAX a1, DATAY b0, DATAY b1) {              \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc =                                                                  \
        ::PP_CAT(negmul_, NAME)(concat(a_hi0, a_hi1), concat(b_hi0, b_hi1));   \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_hi0, a_hi1), SIGNX,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     0, 0); /*shift high part by 16 */         \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), SIGNY, acc, 0, 0,   \
                                     0, 0); /* no shift */                     \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     0, 0); /* shift by 16 */                  \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mac_, NAME)(DATAX a0, DATAX a1, DATAY b0, DATAY b1, ACC acc1) {       \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc =                                                                  \
        ::PP_CAT(mul_, NAME)(concat(a_hi0, a_hi1), concat(b_hi0, b_hi1));      \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_hi0, a_hi1), SIGNX,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     0, 0); /*shift high part by 16 */         \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), SIGNY, acc, 0, 0,   \
                                     0, 0); /* no shift */                     \
    acc = PP_CAT3(addmac_, NAME,                                               \
                  _conf)(concat(a_lo0, a_lo1), false, concat(b_lo0, b_lo1),    \
                         false, acc, acc1, 0, 1, 0, 0, 0); /* shift by 16 */   \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(msc_, NAME)(DATAX a0, DATAX a1, DATAY b0, DATAY b1, ACC acc1) {       \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc =                                                                  \
        ::PP_CAT(negmul_, NAME)(concat(a_hi0, a_hi1), concat(b_hi0, b_hi1));   \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_hi0, a_hi1), SIGNX,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     0, 0); /*shift high part by 16 */         \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), SIGNY, acc, 0, 0,   \
                                     0, 0); /* no shift */                     \
    acc = PP_CAT3(addmsc_, NAME,                                               \
                  _conf)(concat(a_lo0, a_lo1), false, concat(b_lo0, b_lo1),    \
                         false, acc, acc1, 0, 1, 0, 0, 0); /* shift by 16 */   \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mul_, NAME)                                                           \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y) {             \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(mul_, NAME)(concat(a_hi0, a_hi1), sgn_x,                \
                                   concat(b_hi0, b_hi1), sgn_y);               \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_hi0, a_hi1), sgn_x,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     0, 0); /*shift high part by 16 */         \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), sgn_y, acc, 0, 0,   \
                                     0, 0); /* no shift */                     \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     0, 0); /* shift by 16 */                  \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(negmul_, NAME)                                                        \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y) {             \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(negmul_, NAME)(concat(a_hi0, a_hi1), sgn_x,             \
                                      concat(b_hi0, b_hi1), sgn_y);            \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_hi0, a_hi1), sgn_x,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     0, 0); /*shift high part by 16 */         \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), sgn_y, acc, 0, 0,   \
                                     0, 0); /* no shift */                     \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     0, 0); /* shift by 16 */                  \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mac_, NAME)                                                           \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1) {   \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(mul_, NAME)(concat(a_hi0, a_hi1), sgn_x,                \
                                   concat(b_hi0, b_hi1), sgn_y);               \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_hi0, a_hi1), sgn_x,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     0, 0); /*shift high part by 16 */         \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), sgn_y, acc, 0, 0,   \
                                     0, 0); /* no shift */                     \
    acc = PP_CAT3(addmac_, NAME,                                               \
                  _conf)(concat(a_lo0, a_lo1), false, concat(b_lo0, b_lo1),    \
                         false, acc, acc1, 0, 1, 0, 0, 0); /* shift by 16 */   \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(msc_, NAME)                                                           \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1) {   \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(negmul_, NAME)(concat(a_hi0, a_hi1), sgn_x,             \
                                      concat(b_hi0, b_hi1), sgn_y);            \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_hi0, a_hi1), sgn_x,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     0, 0); /*shift high part by 16 */         \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), sgn_y, acc, 0, 0,   \
                                     0, 0); /* no shift */                     \
    acc = PP_CAT3(addmsc_, NAME,                                               \
                  _conf)(concat(a_lo0, a_lo1), false, concat(b_lo0, b_lo1),    \
                         false, acc, acc1, 0, 1, 0, 0, 0); /* shift by 16 */   \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, DATAY b0, DATAY b1, int sub_mul) {                      \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(concat(a_hi0, a_hi1),               \
                                           concat(b_hi0, b_hi1), sub_mul);     \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_hi0, a_hi1), SIGNX,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), SIGNY, acc, 0, 0,   \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /* shift by 16 */            \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(negmul_, NAME, _conf)                                                \
  (DATAX a0, DATAX a1, DATAY b0, DATAY b1, int sub_mul) {                      \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(concat(a_hi0, a_hi1),            \
                                              concat(b_hi0, b_hi1), sub_mul);  \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_hi0, a_hi1), SIGNX,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), SIGNY, acc, 0, 0,   \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /* shift by 16 */            \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, DATAY b0, DATAY b1, ACC acc1, int sub_mul,              \
   int sub_acc1) {                                                             \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(concat(a_hi0, a_hi1),               \
                                           concat(b_hi0, b_hi1), sub_mul);     \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_hi0, a_hi1), SIGNX,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), SIGNY, acc, 0, 0,   \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(addmac_, NAME, _conf)(                                       \
        concat(a_lo0, a_lo1), false, concat(b_lo0, b_lo1), false, acc, acc1,   \
        0, 1, sub_mul, 0, sub_acc1); /* shift by 16 */                         \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, DATAY b0, DATAY b1, ACC acc1, int sub_mul,              \
   int sub_acc1) {                                                             \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(concat(a_hi0, a_hi1),            \
                                              concat(b_hi0, b_hi1), sub_mul);  \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_hi0, a_hi1), SIGNX,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), SIGNY, acc, 0, 0,   \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(addmsc_, NAME, _conf)(                                       \
        concat(a_lo0, a_lo1), false, concat(b_lo0, b_lo1), false, acc, acc1,   \
        0, 1, sub_mul, 0, sub_acc1); /* shift by 16 */                         \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y,               \
   int sub_mul) {                                                              \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(                                    \
        concat(a_hi0, a_hi1), sgn_x, concat(b_hi0, b_hi1), sgn_y, sub_mul);    \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_hi0, a_hi1), sgn_x,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), sgn_y, acc, 0, 0,   \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /* shift by 16 */            \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(negmul_, NAME, _conf)                                                \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y,               \
   int sub_mul) {                                                              \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(                                 \
        concat(a_hi0, a_hi1), sgn_x, concat(b_hi0, b_hi1), sgn_y, sub_mul);    \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_hi0, a_hi1), sgn_x,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), sgn_y, acc, 0, 0,   \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /* shift by 16 */            \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1,     \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(                                    \
        concat(a_hi0, a_hi1), sgn_x, concat(b_hi0, b_hi1), sgn_y, sub_mul);    \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_hi0, a_hi1), sgn_x,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), sgn_y, acc, 0, 0,   \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(addmac_, NAME, _conf)(                                       \
        concat(a_lo0, a_lo1), false, concat(b_lo0, b_lo1), false, acc, acc1,   \
        0, 1, sub_mul, 0, sub_acc1); /* shift 16 */                            \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1,     \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(                                 \
        concat(a_hi0, a_hi1), sgn_x, concat(b_hi0, b_hi1), sgn_y, sub_mul);    \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_hi0, a_hi1), sgn_x,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), sgn_y, acc, 0, 0,   \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(addmsc_, NAME, _conf)(                                       \
        concat(a_lo0, a_lo1), false, concat(b_lo0, b_lo1), false, acc, acc1,   \
        0, 1, sub_mul, 0, sub_acc1); /* shift 16 */                            \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, DATAY b0, DATAY b1, ACC acc1, int zero_acc1,            \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(concat(a_hi0, a_hi1),               \
                                           concat(b_hi0, b_hi1), sub_mul);     \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_hi0, a_hi1), SIGNX,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), SIGNY, acc, 0, 0,   \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0);                              \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, DATAY b0, DATAY b1, ACC acc1, int zero_acc1,            \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(concat(a_hi0, a_hi1),            \
                                              concat(b_hi0, b_hi1), sub_mul);  \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_hi0, a_hi1), SIGNX,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /* shift high part by 16 */  \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), SIGNY, acc, 0, 0,   \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0);                              \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1,     \
   int zero_acc1, int sub_mul, int sub_acc1) {                                 \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(                                    \
        concat(a_hi0, a_hi1), sgn_x, concat(b_hi0, b_hi1), sgn_y, sub_mul);    \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_hi0, a_hi1), sgn_x,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /* shift high part by 16 */  \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), sgn_y, acc, 0, 0,   \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(mac_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0);                              \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a0, DATAX a1, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1,     \
   int zero_acc1, int sub_mul, int sub_acc1) {                                 \
    v32uint16 a_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a0, 0),                  \
                           extract_v16int32((v32int32)a0, 1), T16_32x2_lo);    \
    v32uint16 a_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)a1, 0),                  \
                           extract_v16int32((v32int32)a1, 1), T16_32x2_lo);    \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi0 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a0, 0), extract_v16int32((v32int32)a0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNX, v32int16, v32uint16)                                          \
    a_hi1 = (PP_IF(SIGNX, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)a1, 0), extract_v16int32((v32int32)a1, 1),  \
        T16_32x2_hi);                                                          \
    v32uint16 b_lo0 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 b_lo1 =                                                          \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    b_hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                        \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(                                 \
        concat(a_hi0, a_hi1), sgn_x, concat(b_hi0, b_hi1), sgn_y, sub_mul);    \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_hi0, a_hi1), sgn_x,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0); /*shift high part by 16 */   \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_hi0, b_hi1), sgn_y, acc, 0, 0,   \
                                     sub_mul, 0); /* no shift */               \
    acc = PP_CAT3(msc_, NAME, _conf)(concat(a_lo0, a_lo1), false,              \
                                     concat(b_lo0, b_lo1), false, acc, 0, 1,   \
                                     sub_mul, 0);                              \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }

#define EMUL_32x32_ELEM_SIGNED_MAC_F_1KB(ACC, ELEM, DATAX_1, DATAX_2, DATAY_1, \
                                         DATAY_2, NAME, GROUP, BRIEF_DOC)      \
  EMUL_32x32_ELEM_MAC_F_1KB(ACC, ELEM, PP_CAT3(DATAX_1, u, DATAX_2),           \
                            PP_CAT3(DATAY_1, u, DATAY_2), 0, 0, NAME, GROUP,   \
                            BRIEF_DOC);                                        \
  EMUL_32x32_ELEM_MAC_F_1KB(ACC, ELEM, PP_CAT3(DATAX_1, u, DATAX_2),           \
                            PP_CAT3(DATAY_1, , DATAY_2), 0, 1, NAME, GROUP,    \
                            BRIEF_DOC);                                        \
  EMUL_32x32_ELEM_MAC_F_1KB(ACC, ELEM, PP_CAT3(DATAX_1, , DATAX_2),            \
                            PP_CAT3(DATAY_1, u, DATAY_2), 1, 0, NAME, GROUP,   \
                            BRIEF_DOC);                                        \
  EMUL_32x32_ELEM_MAC_F_1KB(ACC, ELEM, PP_CAT3(DATAX_1, , DATAX_2),            \
                            PP_CAT3(DATAY_1, , DATAY_2), 1, 1, NAME, GROUP,    \
                            BRIEF_DOC);

#define EMUL_32x32_16x32_MAC_F_1KB(ACC, ELEM, DATAX, DATAY, SIGNX, SIGNY,      \
                                   NAME, GROUP, B1_DOC, BRIEF_DOC)             \
  INTRINSIC(ACC) PP_CAT(mul_, NAME)(DATAX a, DATAY b0, DATAY b1) {             \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(mul_, NAME)(a, concat(hi0, hi1));                       \
    acc = PP_CAT3(mac_, NAME, _conf)(a, SIGNX, concat(lo0, lo1), false, acc,   \
                                     0, 1, 0, 0);                              \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(negmul_, NAME)(DATAX a, DATAY b0, DATAY b1) {          \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(negmul_, NAME)(a, concat(hi0, hi1));                    \
    acc = PP_CAT3(msc_, NAME, _conf)(a, SIGNX, concat(lo0, lo1), false, acc,   \
                                     0, 1, 0, 0);                              \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(mac_, NAME)(DATAX a, DATAY b0, DATAY b1, ACC acc1) {   \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(mul_, NAME)(a, concat(hi0, hi1));                       \
    acc = PP_CAT3(addmac_, NAME, _conf)(a, SIGNX, concat(lo0, lo1), false,     \
                                        acc, acc1, 0, 1, 0, 0, 0);             \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(msc_, NAME)(DATAX a, DATAY b0, DATAY b1, ACC acc1) {   \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(negmul_, NAME)(a, concat(hi0, hi1)); /*-hi*/            \
    acc = PP_CAT3(addmsc_, NAME, _conf)(a, SIGNX, concat(lo0, lo1), false,     \
                                        acc, acc1, 0, 1, 0, 0,                 \
                                        0); /*( +(-hi) + acc1 - lo */          \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mul_, NAME)(DATAX a, int sgn_x, DATAY b0, DATAY b1, int sgn_y) {      \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(mul_, NAME)(a, sgn_x, concat(hi0, hi1), sgn_y);         \
    acc = PP_CAT3(mac_, NAME, _conf)(a, sgn_x, concat(lo0, lo1), false, acc,   \
                                     0, 1, 0, 0);                              \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(negmul_, NAME)(DATAX a, int sgn_x, DATAY b0, DATAY b1, int sgn_y) {   \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(negmul_, NAME)(a, sgn_x, concat(hi0, hi1), sgn_y);      \
    acc = PP_CAT3(msc_, NAME, _conf)(a, sgn_x, concat(lo0, lo1), false, acc,   \
                                     0, 1, 0, 0);                              \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mac_, NAME)                                                           \
  (DATAX a, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1) {              \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT(mul_, NAME)(a, sgn_x, concat(hi0, hi1), sgn_y);         \
    acc = PP_CAT3(addmac_, NAME, _conf)(a, sgn_x, concat(lo0, lo1), false,     \
                                        acc, acc1, 0, 1, 0, 0, 0);             \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(msc_, NAME)                                                           \
  (DATAX a, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1) {              \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc =                                                                  \
        ::PP_CAT(negmul_, NAME)(a, sgn_x, concat(hi0, hi1), sgn_y); /*-hi*/    \
    acc = PP_CAT3(addmsc_, NAME, _conf)(a, sgn_x, concat(lo0, lo1), false,     \
                                        acc, acc1, 0, 1, 0, 0,                 \
                                        0); /*( +(-hi) + acc1 - lo */          \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _conf)(DATAX a, DATAY b0, DATAY b1, int sub_mul) {       \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, concat(hi0, hi1), sub_mul);      \
    acc = PP_CAT3(mac_, NAME, _conf)(a, SIGNX, concat(lo0, lo1), false, acc,   \
                                     0, 1, sub_mul, 0);                        \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(negmul_, NAME, _conf)(DATAX a, DATAY b0, DATAY b1, int sub_mul) {    \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, concat(hi0, hi1), sub_mul);   \
    acc = PP_CAT3(msc_, NAME, _conf)(a, SIGNX, concat(lo0, lo1), false, acc,   \
                                     0, 1, sub_mul, 0);                        \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a, DATAY b0, DATAY b1, ACC acc1, int sub_mul, int sub_acc1) {         \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, concat(hi0, hi1), sub_mul);      \
    acc =                                                                      \
        PP_CAT3(addmac_, NAME, _conf)(a, SIGNX, concat(lo0, lo1), false, acc,  \
                                      acc1, 0, 1, sub_mul, 0, sub_acc1);       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a, DATAY b0, DATAY b1, ACC acc1, int sub_mul, int sub_acc1) {         \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, concat(hi0, hi1), sub_mul);   \
    acc =                                                                      \
        PP_CAT3(addmsc_, NAME, _conf)(a, SIGNX, concat(lo0, lo1), false, acc,  \
                                      acc1, 0, 1, sub_mul, 0, sub_acc1);       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _conf)                                                   \
  (DATAX a, int sgn_x, DATAY b0, DATAY b1, int sgn_y, int sub_mul) {           \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, sgn_x, concat(hi0, hi1), sgn_y,  \
                                           sub_mul);                           \
    acc = PP_CAT3(mac_, NAME, _conf)(a, sgn_x, concat(lo0, lo1), false, acc,   \
                                     0, 1, sub_mul, 0);                        \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(negmul_, NAME, _conf)                                                \
  (DATAX a, int sgn_x, DATAY b0, DATAY b1, int sgn_y, int sub_mul) {           \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, sgn_x, concat(hi0, hi1),      \
                                              sgn_y, sub_mul);                 \
    acc = PP_CAT3(msc_, NAME, _conf)(a, sgn_x, concat(lo0, lo1), false, acc,   \
                                     0, 1, sub_mul, 0);                        \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1, int sub_mul,   \
   int sub_acc1) {                                                             \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, sgn_x, concat(hi0, hi1), sgn_y,  \
                                           sub_mul);                           \
    acc =                                                                      \
        PP_CAT3(addmac_, NAME, _conf)(a, sgn_x, concat(lo0, lo1), false, acc,  \
                                      acc1, 0, 1, sub_mul, 0, sub_acc1);       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1, int sub_mul,   \
   int sub_acc1) {                                                             \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, sgn_x, concat(hi0, hi1),      \
                                              sgn_y, sub_mul);                 \
    acc =                                                                      \
        PP_CAT3(addmsc_, NAME, _conf)(a, sgn_x, concat(lo0, lo1), false, acc,  \
                                      acc1, 0, 1, sub_mul, 0, sub_acc1);       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a, DATAY b0, DATAY b1, ACC acc1, int zero_acc1, int sub_mul,          \
   int sub_acc1) {                                                             \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, concat(hi0, hi1), sub_mul);      \
    acc = ::PP_CAT3(mac_, NAME, _conf)(a, SIGNX, concat(lo0, lo1), false, acc, \
                                       0, 0x01, sub_mul, 0);                   \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a, DATAY b0, DATAY b1, ACC acc1, int zero_acc1, int sub_mul,          \
   int sub_acc1) {                                                             \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, concat(hi0, hi1), sub_mul);   \
    acc = ::PP_CAT3(mac_, NAME, _conf)(a, SIGNX, concat(lo0, lo1), false, acc, \
                                       0, 0x01, sub_mul, 0);                   \
    acc = ::sub_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1, int zero_acc1, \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, sgn_x, concat(hi0, hi1), sgn_y,  \
                                           sub_mul);                           \
    acc = ::PP_CAT3(mac_, NAME, _conf)(a, sgn_x, concat(lo0, lo1), false, acc, \
                                       0, 1, sub_mul, 0);                      \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a, int sgn_x, DATAY b0, DATAY b1, int sgn_y, ACC acc1, int zero_acc1, \
   int sub_mul, int sub_acc1) {                                                \
    v32uint16 lo0 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b0, 0),                  \
                           extract_v16int32((v32int32)b0, 1), T16_32x2_lo);    \
    v32uint16 lo1 =                                                            \
        (v32uint16)shuffle(extract_v16int32((v32int32)b1, 0),                  \
                           extract_v16int32((v32int32)b1, 1), T16_32x2_lo);    \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi0 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b0, 0), extract_v16int32((v32int32)b0, 1),  \
        T16_32x2_hi);                                                          \
    PP_IF(SIGNY, v32int16, v32uint16)                                          \
    hi1 = (PP_IF(SIGNY, v32int16, v32uint16))shuffle(                          \
        extract_v16int32((v32int32)b1, 0), extract_v16int32((v32int32)b1, 1),  \
        T16_32x2_hi);                                                          \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, sgn_x, concat(hi0, hi1),      \
                                              sgn_y, sub_mul);                 \
    acc = ::PP_CAT3(mac_, NAME, _conf)(a, sgn_x, concat(lo0, lo1), false, acc, \
                                       0, 1, sub_mul, 0);                      \
    acc = ::sub_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }

#define EMUL_32x32_16x32_SIGNED_MAC_F_1KB(ACC, ELEM, DATAX_1, DATAX_2,         \
                                          DATAY_1, DATAY_2, NAME, GROUP,       \
                                          A1_DOC, BRIEF_DOC)                   \
  EMUL_32x32_16x32_MAC_F_1KB(ACC, ELEM, PP_CAT3(DATAX_1, u, DATAX_2),          \
                             PP_CAT3(DATAY_1, u, DATAY_2), 0, 0, NAME, GROUP,  \
                             A1_DOC, BRIEF_DOC);                               \
  EMUL_32x32_16x32_MAC_F_1KB(ACC, ELEM, PP_CAT3(DATAX_1, u, DATAX_2),          \
                             PP_CAT3(DATAY_1, , DATAY_2), 0, 1, NAME, GROUP,   \
                             A1_DOC, BRIEF_DOC);                               \
  EMUL_32x32_16x32_MAC_F_1KB(ACC, ELEM, PP_CAT3(DATAX_1, , DATAX_2),           \
                             PP_CAT3(DATAY_1, u, DATAY_2), 1, 0, NAME, GROUP,  \
                             A1_DOC, BRIEF_DOC);                               \
  EMUL_32x32_16x32_MAC_F_1KB(ACC, ELEM, PP_CAT3(DATAX_1, , DATAX_2),           \
                             PP_CAT3(DATAY_1, , DATAY_2), 1, 1, NAME, GROUP,   \
                             A1_DOC, BRIEF_DOC);
                EMUL_32x32_16x32_SIGNED_MAC_F(v32acc64, 0, v16, int32, v16,
                                              int32, 4x2_2x8,
                                              intr_gpvectorop_emul_32bx32b, ,
                                              Emulated Multiplication of(4x2)
                                                  with(2x8) Matrices(32b * 32b))
                    EMUL_32x32_ELEM_SIGNED_MAC_F(
                        v32acc64, 1, v16, int32, v16, int32, elem_32,
                        intr_gpvectorop_emul_32bx32b,
                        Emulated Channel by channel multiplication of(1x1)
                            with(1x1)(32b * 32b))

                        EMUL_32x32_ELEM_SIGNED_MAC_F_1KB(
                            v32acc64, 1, v32, int32, v32, int32, elem_32_2,
                            intr_gpvectorop_emul_32bx32b,
                            Emulated Channel by channel multiplication of(1x2)
                                with(2x1)(32b * 32b))

#undef EMUL_32x32_ELEM_SIGNED_MAC_F_1KB
#undef EMUL_32x32_ELEM_MAC_F_1KB
#undef EMUL_32x32_ELEM_MAC_F
/*
#define EMUL_c32xc32_MAC_F(ACC, DATAX, DATAY, NAME, GROUP, BRIEF_DOC)          \
  INTRINSIC(ACC) PP_CAT(mul_, NAME)(DATAX a, DATAY b) { \
    v16cint16 lo =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_lo);           \
    v16cint16 hi =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_hi);           \
    ACC acc = ::PP_CAT(mul_, NAME)(a, hi);                                     \
    acc = PP_CAT3(mac_, NAME, _conf)(a, 1, lo, false, acc, 0, 1,               \
                                     OP_TERM_NEG_COMPLEX, 0, 0);               \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(negmul_, NAME)(DATAX a, DATAY b) { \
    v16cint16 lo =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_lo);           \
    v16cint16 hi =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_hi);           \
    ACC acc = ::PP_CAT(negmul_, NAME)(a, hi);                                  \
    acc = PP_CAT3(msc_, NAME, _conf)(a, 1, lo, false, acc, 0, 1,               \
                                     OP_TERM_NEG_COMPLEX, 0, 0);               \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(mac_, NAME)(DATAX a, DATAY b, ACC acc1) { \
    v16cint16 lo =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_lo);           \
    v16cint16 hi =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_hi);           \
    ACC acc = ::PP_CAT(mul_, NAME)(a, hi);                                     \
    acc = PP_CAT3(addmac_, NAME, _conf)(a, 1, lo, false, acc, acc1, 0, 1,      \
                                        OP_TERM_NEG_COMPLEX, 0, 0, 0);         \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(msc_, NAME)(DATAX a, DATAY b, ACC acc1) { \
    v16cint16 lo =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_lo);           \
    v16cint16 hi =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_hi);           \
    ACC acc = ::PP_CAT(negmul_, NAME)(a, hi);                                  \
    acc = PP_CAT3(addmsc_, NAME, _conf)(a, 1, lo, false, acc, acc1, 0, 1,      \
                                        OP_TERM_NEG_COMPLEX, 0, 0,             \
                                        0);                                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(mul_, NAME, _conf)(DATAX a, DATAY b, int sub_mul) { \
    v16cint16 lo =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_lo);           \
    v16cint16 hi =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_hi);           \
    ACC acc =                                                                  \
        ::PP_CAT3(mul_, NAME, _conf)(a, hi, OP_TERM_NEG_COMPLEX, sub_mul);     \
    acc = PP_CAT3(mac_, NAME, _conf)(a, 1, lo, false, acc, 0, 1,               \
                                     OP_TERM_NEG_COMPLEX, sub_mul, 0);         \
    return acc;                                                                \
  }                                                                          \
  INTRINSIC(ACC) PP_CAT3(negmul_, NAME, _conf)(DATAX a, DATAY b, int sub_mul) {
\
    v16cint16 lo =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_lo);           \
    v16cint16 hi =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_hi);           \
    ACC acc =                                                                  \
        ::PP_CAT3(negmul_, NAME, _conf)(a, hi, OP_TERM_NEG_COMPLEX, sub_mul);  \
    acc = PP_CAT3(msc_, NAME, _conf)(a, 1, lo, false, acc, 0, 1,               \
                                     OP_TERM_NEG_COMPLEX, sub_mul, 0);         \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(mac_, NAME, _conf)(DATAX a, DATAY b, ACC acc1, \
                                        int sub_mul, int sub_acc1) {           \
    v16cint16 lo =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_lo);           \
    v16cint16 hi =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_hi);           \
    ACC acc =                                                                  \
        ::PP_CAT3(mul_, NAME, _conf)(a, hi, OP_TERM_NEG_COMPLEX, sub_mul);     \
    acc = PP_CAT3(addmac_, NAME, _conf)(a, 1, lo, false, acc, acc1, 0, 1,      \
                                        OP_TERM_NEG_COMPLEX, sub_mul, 0,       \
                                        sub_acc1);                             \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(msc_, NAME, _conf)(DATAX a, DATAY b, ACC acc1, \
                                        int sub_mul, int sub_acc1) {           \
    v16cint16 lo =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_lo);           \
    v16cint16 hi =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_hi);           \
    ACC acc =                                                                  \
        ::PP_CAT3(negmul_, NAME, _conf)(a, hi, OP_TERM_NEG_COMPLEX, sub_mul);  \
    acc = PP_CAT3(addmsc_, NAME, _conf)(a, 1, lo, false, acc, acc1, 0, 1,      \
                                        OP_TERM_NEG_COMPLEX, sub_mul, 0,       \
                                        sub_acc1);                             \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(mac_, NAME, _conf)(DATAX a, DATAY b, ACC acc1, \
                                        int zero_acc1, int sub_mask,           \
                                        int sub_mul, int sub_acc1) {           \
    v16cint16 lo =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_lo);           \
    v16cint16 hi =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_hi);           \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, hi, sub_mask, sub_mul);          \
    acc = ::PP_CAT3(mac_, NAME, _conf)(a, 1, lo, 0, acc, 0, 1, sub_mask,       \
                                       sub_mul, 0);                            \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(msc_, NAME, _conf)(DATAX a, DATAY b, ACC acc1, \
                                        int zero_acc1, int sub_mask,           \
                                        int sub_mul, int sub_acc1) {           \
    v16cint16 lo =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_lo);           \
    v16cint16 hi =                                                             \
        (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_hi);           \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, hi, sub_mask, sub_mul);          \
    acc = ::PP_CAT3(msc_, NAME, _conf)(a, 1, lo, 0, acc, 0, 1, sub_mask,       \
                                       sub_mul, 0);                            \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }                                                                            \
  */
/* //Complex Types
#define EMUL_c32xc32_2_MAC_F(ACC, DATAX, DATAY, NAME, GROUP, BRIEF_DOC)        \
  INTRINSIC(ACC) PP_CAT(mul_, NAME)(DATAX a, DATAY b0, DATAY b1) { \
    v16cint16 lo = (v16cint16)shuffle(b0, b1, T16_32x2_lo);                    \
    v16cint16 hi = (v16cint16)shuffle(b0, b1, T16_32x2_hi);                    \
    ACC acc = ::PP_CAT(mul_, NAME)(a, hi);                                     \
    acc = PP_CAT3(mac_, NAME, _conf)(a, 1, lo, false, acc, 0, 1,               \
                                     OP_TERM_NEG_COMPLEX, 0, 0);               \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(negmul_, NAME)(DATAX a, DATAY b0, DATAY b1) { \
    v16cint16 lo = (v16cint16)shuffle(b0, b1, T16_32x2_lo);                    \
    v16cint16 hi = (v16cint16)shuffle(b0, b1, T16_32x2_hi);                    \
    ACC acc = ::PP_CAT(negmul_, NAME)(a, hi);                                  \
    acc = PP_CAT3(msc_, NAME, _conf)(a, 1, lo, false, acc, 0, 1,               \
                                     OP_TERM_NEG_COMPLEX, 0, 0);               \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(mac_, NAME)(DATAX a, DATAY b0, DATAY b1, ACC acc1) { \
    v16cint16 lo = (v16cint16)shuffle(b0, b1, T16_32x2_lo);                    \
    v16cint16 hi = (v16cint16)shuffle(b0, b1, T16_32x2_hi);                    \
    ACC acc = ::PP_CAT(mul_, NAME)(a, hi);                                     \
    acc = PP_CAT3(addmac_, NAME, _conf)(a, 1, lo, false, acc, acc1, 0, 1,      \
                                        OP_TERM_NEG_COMPLEX, 0, 0, 0);         \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(msc_, NAME)(DATAX a, DATAY b0, DATAY b1, ACC acc1) { \
    v16cint16 lo = (v16cint16)shuffle(b0, b1, T16_32x2_lo);                    \
    v16cint16 hi = (v16cint16)shuffle(b0, b1, T16_32x2_hi);                    \
    ACC acc = ::PP_CAT(negmul_, NAME)(a, hi);                                  \
    acc = PP_CAT3(addmsc_, NAME, _conf)(a, 1, lo, false, acc, acc1, 0, 1,      \
                                        OP_TERM_NEG_COMPLEX, 0, 0,             \
                                        0);                                    \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(mul_, NAME, _conf)(DATAX a, DATAY b0, DATAY b1, \
                                        int sub_mul) {                         \
    v16cint16 lo = (v16cint16)shuffle(b0, b1, T16_32x2_lo);                    \
    v16cint16 hi = (v16cint16)shuffle(b0, b1, T16_32x2_hi);                    \
    ACC acc =                                                                  \
        ::PP_CAT3(mul_, NAME, _conf)(a, hi, OP_TERM_NEG_COMPLEX, sub_mul);     \
    acc = PP_CAT3(mac_, NAME, _conf)(a, 1, lo, false, acc, 0, 1,               \
                                     OP_TERM_NEG_COMPLEX, sub_mul, 0);         \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(negmul_, NAME, _conf)(DATAX a, DATAY b0, DATAY b1, \
                                           int sub_mul) {                      \
    v16cint16 lo = (v16cint16)shuffle(b0, b1, T16_32x2_lo);                    \
    v16cint16 hi = (v16cint16)shuffle(b0, b1, T16_32x2_hi);                    \
    ACC acc =                                                                  \
        ::PP_CAT3(negmul_, NAME, _conf)(a, hi, OP_TERM_NEG_COMPLEX, sub_mul);  \
    acc = PP_CAT3(msc_, NAME, _conf)(a, 1, lo, false, acc, 0, 1,               \
                                     OP_TERM_NEG_COMPLEX, sub_mul, 0);         \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(mac_, NAME, _conf)(DATAX a, DATAY b0, DATAY b1, ACC
acc1, \
                                        int sub_mul, int sub_acc1) {           \
    v16cint16 lo = (v16cint16)shuffle(b0, b1, T16_32x2_lo);                    \
    v16cint16 hi = (v16cint16)shuffle(b0, b1, T16_32x2_hi);                    \
    ACC acc =                                                                  \
        ::PP_CAT3(mul_, NAME, _conf)(a, hi, OP_TERM_NEG_COMPLEX, sub_mul);     \
    acc = PP_CAT3(addmac_, NAME, _conf)(a, 1, lo, false, acc, acc1, 0, 1,      \
                                        OP_TERM_NEG_COMPLEX, sub_mul, 0,       \
                                        sub_acc1);                             \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(msc_, NAME, _conf)(DATAX a, DATAY b0, DATAY b1, ACC
acc1, \
                                        int sub_mul, int sub_acc1) {           \
    v16cint16 lo = (v16cint16)shuffle(b0, b1, T16_32x2_lo);                    \
    v16cint16 hi = (v16cint16)shuffle(b0, b1, T16_32x2_hi);                    \
    ACC acc =                                                                  \
        ::PP_CAT3(negmul_, NAME, _conf)(a, hi, OP_TERM_NEG_COMPLEX, sub_mul);  \
    acc = PP_CAT3(addmsc_, NAME, _conf)(a, 1, lo, false, acc, acc1, 0, 1,      \
                                        OP_TERM_NEG_COMPLEX, sub_mul, 0,       \
                                        sub_acc1);                             \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(mac_, NAME, _conf)(DATAX a, DATAY b0, DATAY b1, ACC
acc1, \
                                        int zero_acc1, int sub_mask,           \
                                        int sub_mul, int sub_acc1) {           \
    v16cint16 lo = (v16cint16)shuffle(b0, b1, T16_32x2_lo);                    \
    v16cint16 hi = (v16cint16)shuffle(b0, b1, T16_32x2_hi);                    \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, hi, sub_mask, sub_mul);          \
    acc = ::PP_CAT3(mac_, NAME, _conf)(a, 1, lo, 0, acc, 0, 1, sub_mask,       \
                                       sub_mul, 0);                            \
    acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);                    \
    return acc;                                                                \
  }

                INTRINSIC(ACC)
    PP_CAT3(msc_, NAME, _conf)(DATAX a, DATAY b0, DATAY b1, ACC acc1,
                               int zero_acc1, int sub_mask, int sub_mul,
                               int sub_acc1) {
  v16cint16 lo = (v16cint16)shuffle(b0, b1, T16_32x2_lo);
  v16cint16 hi = (v16cint16)shuffle(b0, b1, T16_32x2_hi);
  ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, hi, sub_mask, sub_mul);
  acc = ::PP_CAT3(msc_, NAME, _conf)(a, 1, lo, 0, acc, 0, 1, sub_mask, sub_mul,
                                     0);
  acc = ::add_conf(acc1, acc, zero_acc1, 0, sub_acc1, 0);
  return acc;
}
#define EMUL_c32xc32_MAC_CONJ_F(ACC, CONJ, CONJ_MASK, DATAX, DATAY, NAME, GROUP,
BRIEF_DOC) INTRINSIC(ACC) PP_CAT3(mul_, NAME, CONJ)(DATAX a, DATAY b) {
  v16cint16 lo = (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_lo);
  v16cint16 hi = (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_hi);
  ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, hi, CONJ_MASK, 0);
  acc = PP_CAT3(mac_, NAME, _conf)(a, 1, lo, false, acc, 0, 1, CONJ_MASK, 0, 0);
  return acc;
}
INTRINSIC(ACC) PP_CAT3(negmul_, NAME, CONJ)(DATAX a, DATAY b) {
  v16cint16 lo = (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_lo);
  v16cint16 hi = (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_hi);
  ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, hi, CONJ_MASK, 0);
  acc = PP_CAT3(msc_, NAME, _conf)(a, 1, lo, false, acc, 0, 1, CONJ_MASK, 0, 0);
  return acc;
}
INTRINSIC(ACC) PP_CAT3(mac_, NAME, CONJ)(DATAX a, DATAY b, ACC acc1) {
  v16cint16 lo = (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_lo);
  v16cint16 hi = (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_hi);
  ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, hi, CONJ_MASK, 0);
  acc = PP_CAT3(addmac_, NAME, _conf)(a, 1, lo, false, acc, acc1, 0, 1,
                                      CONJ_MASK, 0, 0, 0);
  return acc;
}
INTRINSIC(ACC) PP_CAT3(msc_, NAME, CONJ)(DATAX a, DATAY b, ACC acc1) {
  v16cint16 lo = (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_lo);
  v16cint16 hi = (v16cint16)shuffle(b, PP_CAT(undef_, DATAY)(), T16_32x2_hi);
  ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, hi, CONJ_MASK, 0);
  acc = PP_CAT3(addmsc_, NAME, _conf)(a, 1, lo, false, acc, acc1, 0, 1,
                                      OP_TERM_NEG_COMPLEX, 0, 0, 0);
  return acc;
}
#define EMUL_c32xc32_2_MAC_CONJ_F(ACC, CONJ, CONJ_MASK, DATAX, DATAY, NAME,    \
                                  GROUP, BRIEF_DOC)                            \
  INTRINSIC(ACC) PP_CAT3(mul_, NAME, CONJ)(DATAX a, DATAY b0, DATAY b1) { \
    v16cint16 lo = (v16cint16)shuffle(b0, b1, T16_32x2_lo);                    \
    v16cint16 hi = (v16cint16)shuffle(b0, b1, T16_32x2_hi);                    \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, hi, CONJ_MASK, 0);               \
    acc = PP_CAT3(mac_, NAME, _conf)(a, 1, lo, false, acc, 0, 1, CONJ_MASK, 0, \
                                     0);                                       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(negmul_, NAME, CONJ)(DATAX a, DATAY b0, DATAY b1) { \
    v16cint16 lo = (v16cint16)shuffle(b0, b1, T16_32x2_lo);                    \
    v16cint16 hi = (v16cint16)shuffle(b0, b1, T16_32x2_hi);                    \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, hi, CONJ_MASK, 0);            \
    acc = PP_CAT3(msc_, NAME, _conf)(a, 1, lo, false, acc, 0, 1, CONJ_MASK, 0, \
                                     0);                                       \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(mac_, NAME, CONJ)(DATAX a, DATAY b0, DATAY b1, \
                                       ACC acc1) {                             \
    v16cint16 lo = (v16cint16)shuffle(b0, b1, T16_32x2_lo);                    \
    v16cint16 hi = (v16cint16)shuffle(b0, b1, T16_32x2_hi);                    \
    ACC acc = ::PP_CAT3(mul_, NAME, _conf)(a, hi, CONJ_MASK, 0);               \
    acc = PP_CAT3(addmac_, NAME, _conf)(a, 1, lo, false, acc, acc1, 0, 1,      \
                                        CONJ_MASK, 0, 0, 0);                   \
    return acc;                                                                \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(msc_, NAME, CONJ)(DATAX a, DATAY b0, DATAY b1, \
                                       ACC acc1) {                             \
    v16cint16 lo = (v16cint16)shuffle(b0, b1, T16_32x2_lo);                    \
    v16cint16 hi = (v16cint16)shuffle(b0, b1, T16_32x2_hi);                    \
    ACC acc = ::PP_CAT3(negmul_, NAME, _conf)(a, hi, CONJ_MASK, 0);            \
    acc = PP_CAT3(addmsc_, NAME, _conf)(a, 1, lo, false, acc, acc1, 0, 1,      \
                                        OP_TERM_NEG_COMPLEX, 0, 0, 0);         \
    return acc;                                                                \
  }                     */  //Complex Types                                    \
/*EMUL_c32xc32_MAC_F(v8cacc64, v8cint32,  v8cint32,  elem_8,                   \
 * intr_gpvectorop_emul_c32bxc32b, Emulated Multiplication of Channel by       \
 * channel complex multiplication of (1x1) with (1x1))*/
#if 0 // Complex Types
EMUL_c32xc32_MAC_CONJ_F(v8cacc64, _cc, OP_TERM_NEG_COMPLEX_CONJUGATE_X_Y, v8cint32,  v8cint32,  elem_8, intr_gpvectorop_emul_c32bxc32b, Emulated Multiplication of Channel by channel complex multiplication of (1x2) with (2x1) with a & b conjugate);
EMUL_c32xc32_MAC_CONJ_F(v8cacc64, _cn, OP_TERM_NEG_COMPLEX_CONJUGATE_X,   v8cint32,  v8cint32,  elem_8, intr_gpvectorop_emul_c32bxc32b, Emulated Multiplication of Channel by channel complex multiplication of (1x2) with (2x1) with a & b conjugate);
EMUL_c32xc32_MAC_CONJ_F(v8cacc64, _nc, OP_TERM_NEG_COMPLEX_CONJUGATE_Y,   v8cint32,  v8cint32,  elem_8, intr_gpvectorop_emul_c32bxc32b, Emulated Multiplication of Channel by channel complex multiplication of (1x2) with (2x1) with a & b conjugate);
/* */EMUL_c32xc32_2_MAC_F(v16cacc64,                                         v16cint32,  v8cint32,  elem_16, intr_gpvectorop_emul_c32bxc32b, Emulated Multiplication of Channel by channel complex multiplication of (1x1) with (1x1))
EMUL_c32xc32_2_MAC_CONJ_F(v16cacc64, _cc, OP_TERM_NEG_COMPLEX_CONJUGATE_X_Y, v16cint32,  v8cint32,  elem_16, intr_gpvectorop_emul_c32bxc32b, Emulated Multiplication of Channel by channel complex multiplication of (1x2) with (2x1) with a & b conjugate);
EMUL_c32xc32_2_MAC_CONJ_F(v16cacc64, _cn, OP_TERM_NEG_COMPLEX_CONJUGATE_X,   v16cint32,  v8cint32,  elem_16, intr_gpvectorop_emul_c32bxc32b, Emulated Multiplication of Channel by channel complex multiplication of (1x2) with (2x1) with a & b conjugate);
EMUL_c32xc32_2_MAC_CONJ_F(v16cacc64, _nc, OP_TERM_NEG_COMPLEX_CONJUGATE_Y,   v16cint32,  v8cint32,  elem_16, intr_gpvectorop_emul_c32bxc32b, Emulated Multiplication of Channel by channel complex multiplication of (1x2) with (2x1) with a & b conjugate);

#endif // Complex Types

                            EMUL_32x32_16x32_SIGNED_MAC_F(
                                v64acc32, 0, v32, int16, v16, int32, 8x2_2x8,
                                intr_gpvectorop_emul_16bx32b, ,
                                Emulated Multiplication of(2x4) with(4x8)
                                    Matrices(16b * 32b))
                                EMUL_32x32_16x32_SIGNED_MAC_F(
                                    v32acc64, 1, v32, int16, v16, int32,
                                    elem_32, intr_gpvectorop_emul_16bx32b,
                                    @param b1 Matrix B(Second half),
                                    Emulated Channel by channel multiplication
                                        of(1x1) with(1x1)(16b * 32b))
                                    EMUL_32x32_16x32_SIGNED_MAC_F_1KB(
                                        v32acc64, 1, v64, int16, v32, int32,
                                        elem_32_2, intr_gpvectorop_emul_16bx32b,
                                        @param b1 Matrix B(Second half),
                                        Emulated Channel by channel
                                            multiplication of(1x2)
                                                with(2x1)(16b * 32b))
// EMUL_32x32_16x32_SIGNED_MAC_F(v32acc64, 1, v32, int16, v16, int32,
// elem_32_2,intr_gpvectorop_emul_16bx32b, @param  b1          Matrix B (Second
// half), Emulated Channel by channel multiplication of (1x2) with (2x1) (16b *
// 32b)) EMUL_32x32_16x32_SIGNED_MAC_F(v16acc64, 1, v32, int16, v16, int32,
// elem_16_2,intr_gpvectorop_emul_16bx32b, @param  b1          Matrix B (Second
// half), Emulated Channel by channel multiplication of (1x2) with (2x1) (16b *
// 32b))

#undef EMUL_32x32_16x32_MAC_F
#undef EMUL_32x32_16x32_SIGNED_MAC_F

#undef EMUL_32x32_16x32_SIGNED_MAC_F_1KB
#undef EMUL_32x32_16x32_MAC_F_1KB

#endif // __AIE2PS_VMULT_EMULATED_H__
