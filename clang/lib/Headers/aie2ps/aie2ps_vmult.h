//===-------------------- AIEngine AIE2ps intrinsics ------------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2PS_VMULT_H__
#define __AIE2PS_VMULT_H__
#include "aie2ps_defines.h"
#include "aie2ps_enums.h"
#include "aie2ps_pp.h"

// ------------------------------------------------------------
// MUL/MAC Intrinsics
// ------------------------------------------------------------

#define AMODE(acc_cmb) PP_CAT(AMODE_, acc_cmb)

static inline int aie2ps_compute_control(int sgn_x, int sgn_y, int amode,
                                         int bmode, int cmode_sel_x,
                                         int cmode_sel_y, int variant,
                                         int zero_acc, int shift16, int sub0,
                                         int sub1, int sub2, int sub_mask) {
  return ((unsigned)sub_mask << 16) | ((unsigned)shift16 << 10) |
         ((unsigned)sub0 << 11) | ((unsigned)sub1 << 12) |
         ((unsigned)sub2 << 13) | ((unsigned)(amode & 3) << 1) |
         ((unsigned)(cmode_sel_x & 1) << 24) |
         ((unsigned)(cmode_sel_y & 1) << 25) | ((unsigned)bmode << 3) |
         ((unsigned)variant << 5) |
         ((unsigned)(sgn_x << 9) | ((unsigned)sgn_y << 8)) |
         ((unsigned)zero_acc << 0);
}

#define MAC_F(ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, SIGNX, SIGNY, ACC_CMB,     \
              NAME, BMODE, CMODE_SEL_X, CMODE_SEL_Y, VARIANT, GROUP,           \
              TRANSPOSE_B, TRANSPOSE_B_MODE)                                   \
  INTRINSIC(ACC) PP_CAT(mul_, NAME)(DATAX a, DATAY b) {                        \
    int conf = aie2ps_compute_control(                                         \
        SIGNX, SIGNY, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), 0, 0, 0, 0, 0, PP_IF(CMPLX, 10, 0));         \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MUL(_mul, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(negmul_, NAME)(DATAX a, DATAY b) {                     \
    int conf = aie2ps_compute_control(                                         \
        SIGNX, SIGNY, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), 0, 0, 0, 0, 0, PP_IF(CMPLX, 10, 0));         \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MUL(_negmul, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,        \
                  VARIANT)                                                     \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(mac_, NAME)(DATAX a, DATAY b, ACC acc) {               \
    int conf = aie2ps_compute_control(                                         \
        SIGNX, SIGNY, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), 0, 0, 0, 0, 0, PP_IF(CMPLX, 10, 0));         \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MAC(_mac, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(msc_, NAME)(DATAX a, DATAY b, ACC acc) {               \
    int conf = aie2ps_compute_control(                                         \
        SIGNX, SIGNY, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), 0, 0, 0, 0, 0, PP_IF(CMPLX, 10, 0));         \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MAC(_msc, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(addmac_, NAME)(DATAX a, DATAY b, ACC acc1, ACC acc2) { \
    int conf = aie2ps_compute_control(                                         \
        SIGNX, SIGNY, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), 0, 0, 0, 0, 0, PP_IF(CMPLX, 10, 0));         \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_ADDMAC(_addmac, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,     \
                     VARIANT)                                                  \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(addmsc_, NAME)(DATAX a, DATAY b, ACC acc1, ACC acc2) { \
    int conf = aie2ps_compute_control(                                         \
        SIGNX, SIGNY, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), 0, 0, 0, 0, 0, PP_IF(CMPLX, 10, 0));         \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_ADDMAC(_addmsc, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,     \
                     VARIANT)                                                  \
  }                                                                            \
                                                                               \
  INTRINSIC(ACC) PP_CAT(mul_, NAME)(DATAX a, int sgn_x, DATAY b, int sgn_y) {  \
    int conf = aie2ps_compute_control(                                         \
        sgn_x, sgn_y, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), 0, 0, 0, 0, 0, PP_IF(CMPLX, 10, 0));         \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MUL(_mul, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(negmul_, NAME)(DATAX a, int sgn_x, DATAY b, int sgn_y) {              \
    int conf = aie2ps_compute_control(                                         \
        sgn_x, sgn_y, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), 0, 0, 0, 0, 0, PP_IF(CMPLX, 10, 0));         \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MUL(_negmul, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,        \
                  VARIANT)                                                     \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(mac_, NAME)(DATAX a, int sgn_x, DATAY b, int sgn_y, ACC acc) {        \
    int conf = aie2ps_compute_control(                                         \
        sgn_x, sgn_y, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), 0, 0, 0, 0, 0, PP_IF(CMPLX, 10, 0));         \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MAC(_mac, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(msc_, NAME)(DATAX a, int sgn_x, DATAY b, int sgn_y, ACC acc) {        \
    int conf = aie2ps_compute_control(                                         \
        sgn_x, sgn_y, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), 0, 0, 0, 0, 0, PP_IF(CMPLX, 10, 0));         \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MAC(_msc, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(addmac_, NAME)                                                        \
  (DATAX a, int sgn_x, DATAY b, int sgn_y, ACC acc1, ACC acc2) {               \
    int conf = aie2ps_compute_control(                                         \
        sgn_x, sgn_y, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), 0, 0, 0, 0, 0, PP_IF(CMPLX, 10, 0));         \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_ADDMAC(_addmac, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,     \
                     VARIANT)                                                  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT(addmsc_, NAME)                                                        \
  (DATAX a, int sgn_x, DATAY b, int sgn_y, ACC acc1, ACC acc2) {               \
    int conf = aie2ps_compute_control(                                         \
        sgn_x, sgn_y, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), 0, 0, 0, 0, 0, PP_IF(CMPLX, 10, 0));         \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_ADDMAC(_addmsc, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,     \
                     VARIANT)                                                  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _conf)                                                   \
  (DATAX a, DATAY b,                                                           \
   PP_IF(CMPLX, int sub_mask PP_COMMA, PP_EMPTY)() int sub_mul) {              \
    int conf = aie2ps_compute_control(SIGNX, SIGNY, AMODE(ACC_CMB), BMODE,     \
                                      CMODE_SEL_X, CMODE_SEL_Y,                \
                                      VARIANT_TO_CONF(VARIANT), 0, 0, sub_mul, \
                                      0, 0, PP_IF(CMPLX, sub_mask, 0));        \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MUL(_mul, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(negmul_, NAME, _conf)                                                \
  (DATAX a, DATAY b,                                                           \
   PP_IF(CMPLX, int sub_mask PP_COMMA, PP_EMPTY)() int sub_mul) {              \
    int conf = aie2ps_compute_control(SIGNX, SIGNY, AMODE(ACC_CMB), BMODE,     \
                                      CMODE_SEL_X, CMODE_SEL_Y,                \
                                      VARIANT_TO_CONF(VARIANT), 0, 0, sub_mul, \
                                      0, 0, PP_IF(CMPLX, sub_mask, 0));        \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MUL(_negmul, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,        \
                  VARIANT)                                                     \
  }                                                                            \
                                                                               \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a, DATAY b, ACC acc, int zero_acc,                                    \
   PP_IF(PP_NOT(BF), int shift16 PP_COMMA, PP_EMPTY)()                         \
       PP_IF(CMPLX, int sub_mask PP_COMMA, PP_EMPTY)() int sub_mul,            \
   int sub_acc1) {                                                             \
    int conf = aie2ps_compute_control(                                         \
        SIGNX, SIGNY, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), zero_acc, PP_IF(PP_NOT(BF), shift16, 0),     \
        sub_mul, sub_acc1, 0, PP_IF(CMPLX, sub_mask, 0));                      \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MAC(_mac, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a, DATAY b, ACC acc, int zero_acc,                                    \
   PP_IF(PP_NOT(BF), int shift16 PP_COMMA, PP_EMPTY)()                         \
       PP_IF(CMPLX, int sub_mask PP_COMMA, PP_EMPTY)() int sub_mul,            \
   int sub_acc1) {                                                             \
    int conf = aie2ps_compute_control(                                         \
        SIGNX, SIGNY, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), zero_acc, PP_IF(PP_NOT(BF), shift16, 0),     \
        sub_mul, sub_acc1, 0, PP_IF(CMPLX, sub_mask, 0));                      \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MAC(_msc, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(addmac_, NAME, _conf)                                                \
  (DATAX a, DATAY b, ACC acc1, ACC acc2, int zero_acc1,                        \
   PP_IF(PP_NOT(BF), int shift16 PP_COMMA, PP_EMPTY)()                         \
       PP_IF(CMPLX, int sub_mask PP_COMMA, PP_EMPTY)() int sub_mul,            \
   int sub_acc1, int sub_acc2) {                                               \
    int conf = aie2ps_compute_control(                                         \
        SIGNX, SIGNY, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), zero_acc1, PP_IF(PP_NOT(BF), shift16, 0),    \
        sub_mul, sub_acc1, sub_acc2, PP_IF(CMPLX, sub_mask, 0));               \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_ADDMAC(_addmac, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,     \
                     VARIANT)                                                  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(addmsc_, NAME, _conf)                                                \
  (DATAX a, DATAY b, ACC acc1, ACC acc2, int zero_acc1,                        \
   PP_IF(PP_NOT(BF), int shift16 PP_COMMA, PP_EMPTY)()                         \
       PP_IF(CMPLX, int sub_mask PP_COMMA, PP_EMPTY)() int sub_mul,            \
   int sub_acc1, int sub_acc2) {                                               \
    int conf = aie2ps_compute_control(                                         \
        SIGNX, SIGNY, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), zero_acc1, PP_IF(PP_NOT(BF), shift16, 0),    \
        sub_mul, sub_acc1, sub_acc2, PP_IF(CMPLX, sub_mask, 0));               \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_ADDMAC(_addmsc, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,     \
                     VARIANT)                                                  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _conf)                                                   \
  (DATAX a, int sgn_x, DATAY b, int sgn_y,                                     \
   PP_IF(CMPLX, int sub_mask PP_COMMA, PP_EMPTY)() int sub_mul) {              \
    int conf = aie2ps_compute_control(sgn_x, sgn_y, AMODE(ACC_CMB), BMODE,     \
                                      CMODE_SEL_X, CMODE_SEL_Y,                \
                                      VARIANT_TO_CONF(VARIANT), 0, 0, sub_mul, \
                                      0, 0, PP_IF(CMPLX, sub_mask, 0));        \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MUL(_mul, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(negmul_, NAME, _conf)                                                \
  (DATAX a, int sgn_x, DATAY b, int sgn_y,                                     \
   PP_IF(CMPLX, int sub_mask PP_COMMA, PP_EMPTY)() int sub_mul) {              \
    int conf = aie2ps_compute_control(sgn_x, sgn_y, AMODE(ACC_CMB), BMODE,     \
                                      CMODE_SEL_X, CMODE_SEL_Y,                \
                                      VARIANT_TO_CONF(VARIANT), 0, 0, sub_mul, \
                                      0, 0, PP_IF(CMPLX, sub_mask, 0));        \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MUL(_negmul, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,        \
                  VARIANT)                                                     \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _conf)                                                   \
  (DATAX a, int sgn_x, DATAY b, int sgn_y, ACC acc, int zero_acc,              \
   PP_IF(PP_NOT(BF), int shift16 PP_COMMA, PP_EMPTY)()                         \
       PP_IF(CMPLX, int sub_mask PP_COMMA, PP_EMPTY)() int sub_mul,            \
   int sub_acc1) {                                                             \
    int conf = aie2ps_compute_control(                                         \
        sgn_x, sgn_y, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), zero_acc, PP_IF(PP_NOT(BF), shift16, 0),     \
        sub_mul, sub_acc1, 0, PP_IF(CMPLX, sub_mask, 0));                      \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MAC(_mac, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _conf)                                                   \
  (DATAX a, int sgn_x, DATAY b, int sgn_y, ACC acc, int zero_acc,              \
   PP_IF(PP_NOT(BF), int shift16 PP_COMMA, PP_EMPTY)()                         \
       PP_IF(CMPLX, int sub_mask PP_COMMA, PP_EMPTY)() int sub_mul,            \
   int sub_acc1) {                                                             \
    int conf = aie2ps_compute_control(                                         \
        sgn_x, sgn_y, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), zero_acc, PP_IF(PP_NOT(BF), shift16, 0),     \
        sub_mul, sub_acc1, 0, PP_IF(CMPLX, sub_mask, 0));                      \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_MAC(_msc, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(addmac_, NAME, _conf)                                                \
  (DATAX a, int sgn_x, DATAY b, int sgn_y, ACC acc1, ACC acc2, int zero_acc1,  \
   PP_IF(PP_NOT(BF), int shift16 PP_COMMA, PP_EMPTY)()                         \
       PP_IF(CMPLX, int sub_mask PP_COMMA, PP_EMPTY)() int sub_mul,            \
   int sub_acc1, int sub_acc2) {                                               \
    int conf = aie2ps_compute_control(                                         \
        sgn_x, sgn_y, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), zero_acc1, PP_IF(PP_NOT(BF), shift16, 0),    \
        sub_mul, sub_acc1, sub_acc2, PP_IF(CMPLX, sub_mask, 0));               \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_ADDMAC(_addmac, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,     \
                     VARIANT)                                                  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(addmsc_, NAME, _conf)                                                \
  (DATAX a, int sgn_x, DATAY b, int sgn_y, ACC acc1, ACC acc2, int zero_acc1,  \
   PP_IF(PP_NOT(BF), int shift16 PP_COMMA, PP_EMPTY)()                         \
       PP_IF(CMPLX, int sub_mask PP_COMMA, PP_EMPTY)() int sub_mul,            \
   int sub_acc1, int sub_acc2) {                                               \
    int conf = aie2ps_compute_control(                                         \
        sgn_x, sgn_y, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT_TO_CONF(VARIANT), zero_acc1, PP_IF(PP_NOT(BF), shift16, 0),    \
        sub_mul, sub_acc1, sub_acc2, PP_IF(CMPLX, sub_mask, 0));               \
    PP_IF(TRANSPOSE_B, b = shuffle(b, TRANSPOSE_B_MODE);, )                    \
    EMIT_BODY_ADDMAC(_addmsc, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,     \
                     VARIANT)                                                  \
  }
#define MUL_CMPLX_F(ACC, ACCNUM, BF, DATAX, DATAY, ACC_CMB, NAME, BMODE,       \
                    CMODE_SEL_X, CMODE_SEL_Y, VARIANT)                         \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _cmplx_conf)                                             \
  (DATAX a, DATAY b, int sub_mask, int sub_mul) {                              \
    int conf = aie2ps_compute_control(0, 0, AMODE(ACC_CMB), BMODE,             \
                                      CMODE_SEL_X, CMODE_SEL_Y, VARIANT, 0, 0, \
                                      sub_mul, 0, 0, sub_mask);                \
    EMIT_BODY_MUL(_mul, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mul_, NAME, _cmplx_conf)                                             \
  (DATAX a, int sgn_x, DATAY b, int sgn_y, int sub_mask, int sub_mul) {        \
    int conf = aie2ps_compute_control(sgn_x, sgn_y, AMODE(ACC_CMB), BMODE,     \
                                      CMODE_SEL_X, CMODE_SEL_Y, VARIANT, 0, 0, \
                                      sub_mul, 0, 0, sub_mask);                \
    EMIT_BODY_MUL(_mul, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _cmplx_conf)                                             \
  (DATAX a, DATAY b, ACC acc, int zero_acc, int sub_mask, int sub_mul,         \
   int sub_acc1) {                                                             \
    int conf = aie2ps_compute_control(                                         \
        0, 0, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y, VARIANT,        \
        zero_acc, 0, sub_mul, sub_acc1, 0, sub_mask);                          \
    EMIT_BODY_MAC(_mac, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(mac_, NAME, _cmplx_conf)                                             \
  (DATAX a, int sgn_x, DATAY b, int sgn_y, ACC acc, int zero_acc,              \
   int sub_mask, int sub_mul, int sub_acc1) {                                  \
    int conf = aie2ps_compute_control(                                         \
        sgn_x, sgn_y, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT, zero_acc, 0, sub_mul, sub_acc1, 0, sub_mask);                 \
    EMIT_BODY_MAC(_mac, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _cmplx_conf)                                             \
  (DATAX a, DATAY b, ACC acc, int zero_acc, int sub_mask, int sub_mul,         \
   int sub_acc1) {                                                             \
    int conf = aie2ps_compute_control(                                         \
        0, 0, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y, VARIANT,        \
        zero_acc, 0, sub_mul, sub_acc1, 0, sub_mask);                          \
    EMIT_BODY_MAC(_msc, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(msc_, NAME, _cmplx_conf)                                             \
  (DATAX a, int sgn_x, DATAY b, int sgn_y, ACC acc, int zero_acc,              \
   int sub_mask, int sub_mul, int sub_acc1) {                                  \
    int conf = aie2ps_compute_control(                                         \
        sgn_x, sgn_y, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT, zero_acc, 0, sub_mul, sub_acc1, 0, sub_mask);                 \
    EMIT_BODY_MAC(_msc, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE, VARIANT)  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(addmac_, NAME, _cmplx_conf)                                          \
  (DATAX a, DATAY b, ACC acc1, ACC acc2, int zero_acc1, int sub_mask,          \
   int sub_mul, int sub_acc1, int sub_acc2) {                                  \
    int conf = aie2ps_compute_control(                                         \
        0, 0, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y, VARIANT,        \
        zero_acc1, 0, sub_mul, sub_acc1, sub_acc2, sub_mask);                  \
    EMIT_BODY_ADDMAC(_addmac, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,     \
                     VARIANT)                                                  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(addmac_, NAME, _cmplx_conf)                                          \
  (DATAX a, int sgn_x, DATAY b, int sgn_y, ACC acc1, ACC acc2, int zero_acc1,  \
   int sub_mask, int sub_mul, int sub_acc1, int sub_acc2) {                    \
    int conf = aie2ps_compute_control(                                         \
        sgn_x, sgn_y, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT, zero_acc1, 0, sub_mul, sub_acc1, sub_acc2, sub_mask);         \
    EMIT_BODY_ADDMAC(_addmac, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,     \
                     VARIANT)                                                  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(addmsc_, NAME, _cmplx_conf)                                          \
  (DATAX a, DATAY b, ACC acc1, ACC acc2, int zero_acc1, int sub_mask,          \
   int sub_mul, int sub_acc1, int sub_acc2) {                                  \
    int conf = aie2ps_compute_control(                                         \
        0, 0, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y, VARIANT,        \
        zero_acc1, 0, sub_mul, sub_acc1, sub_acc2, sub_mask);                  \
    EMIT_BODY_ADDMAC(_addmsc, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,     \
                     VARIANT)                                                  \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(addmsc_, NAME, _cmplx_conf)                                          \
  (DATAX a, int sgn_x, DATAY b, int sgn_y, ACC acc1, ACC acc2, int zero_acc1,  \
   int sub_mask, int sub_mul, int sub_acc1, int sub_acc2) {                    \
    int conf = aie2ps_compute_control(                                         \
        sgn_x, sgn_y, AMODE(ACC_CMB), BMODE, CMODE_SEL_X, CMODE_SEL_Y,         \
        VARIANT, zero_acc1, 0, sub_mul, sub_acc1, sub_acc2, sub_mask);         \
    EMIT_BODY_ADDMAC(_addmsc, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,     \
                     VARIANT)                                                  \
  }
#define SIZED_MAC_F(ACC, ACCNUM, BF, CMPLX, DATAX_1w, DATAX_1n, DATAX_2,       \
                    DATAY_1w, DATAY_1n, DATAY_2, SIGNX, SIGNY, ACC_CMB, NAME,  \
                    BMODE, CMODE_SEL_X, CMODE_SEL_Y, VARIANT, GROUP,           \
                    TRANSPOSE_B, TRANSPOSE_B_MODE)                             \
  MAC_F(ACC, ACCNUM, BF, CMPLX, PP_CAT(DATAX_1n, DATAX_2),                     \
        PP_CAT(DATAY_1n, DATAY_2), SIGNX, SIGNY, ACC_CMB, NAME, BMODE,         \
        CMODE_SEL_X, CMODE_SEL_Y, VARIANT, GROUP, TRANSPOSE_B,                 \
        TRANSPOSE_B_MODE);                                                     \
  MAC_F(ACC, ACCNUM, BF, CMPLX, PP_CAT(DATAX_1n, DATAX_2),                     \
        PP_CAT(DATAY_1w, DATAY_2), SIGNX, SIGNY, ACC_CMB, NAME, BMODE,         \
        CMODE_SEL_X, CMODE_SEL_Y, VARIANT, GROUP, TRANSPOSE_B,                 \
        TRANSPOSE_B_MODE);                                                     \
  MAC_F(ACC, ACCNUM, BF, CMPLX, PP_CAT(DATAX_1w, DATAX_2),                     \
        PP_CAT(DATAY_1n, DATAY_2), SIGNX, SIGNY, ACC_CMB, NAME, BMODE,         \
        CMODE_SEL_X, CMODE_SEL_Y, VARIANT, GROUP, TRANSPOSE_B,                 \
        TRANSPOSE_B_MODE);                                                     \
  MAC_F(ACC, ACCNUM, BF, CMPLX, PP_CAT(DATAX_1w, DATAX_2),                     \
        PP_CAT(DATAY_1w, DATAY_2), SIGNX, SIGNY, ACC_CMB, NAME, BMODE,         \
        CMODE_SEL_X, CMODE_SEL_Y, VARIANT, GROUP, TRANSPOSE_B,                 \
        TRANSPOSE_B_MODE);

#define SIZE_SIGNED_MAC_F(ACC, ACCNUM, BF, CMPLX, DATAX_1w, DATAX_1n, DATAX_2, \
                          DATAY_1w, DATAY_1n, DATAY_2, ACC_CMB, NAME, BMODE,   \
                          CMODE_SEL_X, CMODE_SEL_Y, VARIANT, GROUP,            \
                          TRANSPOSE_B, TRANSPOSE_B_MODE)                       \
  SIZED_MAC_F(ACC, ACCNUM, BF, CMPLX, DATAX_1w, DATAX_1n, PP_CAT(u, DATAX_2),  \
              DATAY_1w, DATAY_1n, PP_CAT(u, DATAY_2), __SIGN_UNSIGNED,         \
              __SIGN_UNSIGNED, ACC_CMB, NAME, BMODE, CMODE_SEL_X, CMODE_SEL_Y, \
              VARIANT, GROUP, TRANSPOSE_B, TRANSPOSE_B_MODE);                  \
  SIZED_MAC_F(ACC, ACCNUM, BF, CMPLX, DATAX_1w, DATAX_1n, PP_CAT(u, DATAX_2),  \
              DATAY_1w, DATAY_1n, PP_CAT(, DATAY_2), __SIGN_UNSIGNED,          \
              __SIGN_SIGNED, ACC_CMB, NAME, BMODE, CMODE_SEL_X, CMODE_SEL_Y,   \
              VARIANT, GROUP, TRANSPOSE_B, TRANSPOSE_B_MODE);                  \
  SIZED_MAC_F(ACC, ACCNUM, BF, CMPLX, DATAX_1w, DATAX_1n, PP_CAT(, DATAX_2),   \
              DATAY_1w, DATAY_1n, PP_CAT(u, DATAY_2), __SIGN_SIGNED,           \
              __SIGN_UNSIGNED, ACC_CMB, NAME, BMODE, CMODE_SEL_X, CMODE_SEL_Y, \
              VARIANT, GROUP, TRANSPOSE_B, TRANSPOSE_B_MODE);                  \
  SIZED_MAC_F(ACC, ACCNUM, BF, CMPLX, DATAX_1w, DATAX_1n, PP_CAT(, DATAX_2),   \
              DATAY_1w, DATAY_1n, PP_CAT(, DATAY_2), __SIGN_SIGNED,            \
              __SIGN_SIGNED, ACC_CMB, NAME, BMODE, CMODE_SEL_X, CMODE_SEL_Y,   \
              VARIANT, GROUP, TRANSPOSE_B, TRANSPOSE_B_MODE);

#define SIGNED_MAC_F(ACC, ACCNUM, BF, CMPLX, DATAX_1, DATAX_2, DATAY_1,        \
                     DATAY_2, ACC_CMB, NAME, BMODE, CMODE_SEL_X, CMODE_SEL_Y,  \
                     VARIANT, GROUP, TRANSPOSE_B, TRANSPOSE_B_MODE)            \
  MAC_F(ACC, ACCNUM, BF, CMPLX, PP_CAT3(DATAX_1, u, DATAX_2),                  \
        PP_CAT3(DATAY_1, u, DATAY_2), __SIGN_UNSIGNED, __SIGN_UNSIGNED,        \
        ACC_CMB, NAME, BMODE, CMODE_SEL_X, CMODE_SEL_Y, VARIANT, GROUP,        \
        TRANSPOSE_B, TRANSPOSE_B_MODE);                                        \
  MAC_F(ACC, ACCNUM, BF, CMPLX, PP_CAT3(DATAX_1, u, DATAX_2),                  \
        PP_CAT3(DATAY_1, , DATAY_2), __SIGN_UNSIGNED, __SIGN_SIGNED, ACC_CMB,  \
        NAME, BMODE, CMODE_SEL_X, CMODE_SEL_Y, VARIANT, GROUP, TRANSPOSE_B,    \
        TRANSPOSE_B_MODE);                                                     \
  MAC_F(ACC, ACCNUM, BF, CMPLX, PP_CAT3(DATAX_1, , DATAX_2),                   \
        PP_CAT3(DATAY_1, u, DATAY_2), __SIGN_SIGNED, __SIGN_UNSIGNED, ACC_CMB, \
        NAME, BMODE, CMODE_SEL_X, CMODE_SEL_Y, VARIANT, GROUP, TRANSPOSE_B,    \
        TRANSPOSE_B_MODE);                                                     \
  MAC_F(ACC, ACCNUM, BF, CMPLX, PP_CAT3(DATAX_1, , DATAX_2),                   \
        PP_CAT3(DATAY_1, , DATAY_2), __SIGN_SIGNED, __SIGN_SIGNED, ACC_CMB,    \
        NAME, BMODE, CMODE_SEL_X, CMODE_SEL_Y, VARIANT, GROUP, TRANSPOSE_B,    \
        TRANSPOSE_B_MODE);

#define SIGNED_BCAST_MAC_F(ACC, ACCNUM, BF, CMPLX, DATAX_1, DATAX_2, DATAY_2,  \
                           ACC_CMB, NAME, BMODE, CMODE_SEL_X, CMODE_SEL_Y,     \
                           VARIANT, GROUP, TRANSPOSE_B, TRANSPOSE_B_MODE)      \
  MAC_F(ACC, ACCNUM, BF, CMPLX, PP_CAT3(DATAX_1, u, DATAX_2),                  \
        unsigned DATAY_2, __SIGN_UNSIGNED, __SIGN_UNSIGNED, ACC_CMB, NAME,     \
        BMODE, CMODE_SEL_X, CMODE_SEL_Y, VARIANT, GROUP, TRANSPOSE_B,          \
        TRANSPOSE_B_MODE);                                                     \
  MAC_F(ACC, ACCNUM, BF, CMPLX, PP_CAT3(DATAX_1, u, DATAX_2), signed DATAY_2,  \
        __SIGN_UNSIGNED, __SIGN_SIGNED, ACC_CMB, NAME, BMODE, CMODE_SEL_X,     \
        CMODE_SEL_Y, VARIANT, GROUP, TRANSPOSE_B, TRANSPOSE_B_MODE);           \
  MAC_F(ACC, ACCNUM, BF, CMPLX, PP_CAT3(DATAX_1, , DATAX_2), unsigned DATAY_2, \
        __SIGN_SIGNED, __SIGN_UNSIGNED, ACC_CMB, NAME, BMODE, CMODE_SEL_X,     \
        CMODE_SEL_Y, VARIANT, GROUP, TRANSPOSE_B, TRANSPOSE_B_MODE);           \
  MAC_F(ACC, ACCNUM, BF, CMPLX, PP_CAT3(DATAX_1, , DATAX_2), signed DATAY_2,   \
        __SIGN_SIGNED, __SIGN_SIGNED, ACC_CMB, NAME, BMODE, CMODE_SEL_X,       \
        CMODE_SEL_Y, VARIANT, GROUP, TRANSPOSE_B, TRANSPOSE_B_MODE);

#define MAC_CONJ_F(ACC, ACCNUM, CONJ, CONJ_MASK, DATAX, DATAY, SIGNX, SIGNY,   \
                   BITSX, BITSY, ACC_CMB, NAME, BMODE, CMODE_SEL_X,            \
                   CMODE_SEL_Y, VARIANT, GROUP, BRIEF_DOC)                     \
  INTRINSIC(ACC) PP_CAT3(mul_, NAME, CONJ)(DATAX a, DATAY b) {                 \
    int conf = aie2ps_compute_control(SIGNX, SIGNY, AMODE(ACC_CMB), BMODE,     \
                                      CMODE_SEL_X, CMODE_SEL_Y, VARIANT, 0, 0, \
                                      0, 0, 0, CONJ_MASK);                     \
    return PP_CAT3(mul, ACCNUM, _conf)(a, b, conf, 0);                         \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(negmul_, NAME, CONJ)(DATAX a, DATAY b) {              \
    int conf = aie2ps_compute_control(SIGNX, SIGNY, AMODE(ACC_CMB), BMODE,     \
                                      CMODE_SEL_X, CMODE_SEL_Y, VARIANT, 0, 0, \
                                      0, 0, 0, CONJ_MASK);                     \
    return PP_CAT3(mul, ACCNUM, _conf)(a, b, conf, 1);                         \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(mac_, NAME, CONJ)(DATAX a, DATAY b, ACC acc1) {       \
    int conf = aie2ps_compute_control(SIGNX, SIGNY, AMODE(ACC_CMB), BMODE,     \
                                      CMODE_SEL_X, CMODE_SEL_Y, VARIANT, 0, 0, \
                                      0, 0, 0, CONJ_MASK);                     \
    return PP_CAT3(mac, ACCNUM, _conf)(a, b, acc1, conf, 0);                   \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT3(msc_, NAME, CONJ)(DATAX a, DATAY b, ACC acc1) {       \
    int conf = aie2ps_compute_control(SIGNX, SIGNY, AMODE(ACC_CMB), BMODE,     \
                                      CMODE_SEL_X, CMODE_SEL_Y, VARIANT, 0, 0, \
                                      0, 0, 0, CONJ_MASK);                     \
    return PP_CAT3(mac, ACCNUM, _conf)(a, b, acc1, conf, 1);                   \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(addmac_, NAME, CONJ)(DATAX a, DATAY b, ACC acc1, ACC acc2) {         \
    int conf = aie2ps_compute_control(SIGNX, SIGNY, AMODE(ACC_CMB), BMODE,     \
                                      CMODE_SEL_X, CMODE_SEL_Y, VARIANT, 0, 0, \
                                      0, 0, 0, CONJ_MASK);                     \
    return PP_CAT3(addmac, ACCNUM, _conf)(a, b, acc1, acc2, conf, 0);          \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  PP_CAT3(addmsc_, NAME, CONJ)(DATAX a, DATAY b, ACC acc1, ACC acc2) {         \
    int conf = aie2ps_compute_control(SIGNX, SIGNY, AMODE(ACC_CMB), BMODE,     \
                                      CMODE_SEL_X, CMODE_SEL_Y, VARIANT, 0, 0, \
                                      0, 0, 0, CONJ_MASK);                     \
    return PP_CAT3(addmac, ACCNUM, _conf)(a, b, acc1, acc2, conf, 1);          \
  }                                                                            \
// SIZE_SIGNED_MAC_F(v64acc32, 64, 0, 0, v128, v64, int8, v128, v64,  int8, I32,
// 8x8_8x8,       BMODE_I32_8x8, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
// VARIANT_8x8_1_8x8_8x8,           intr_gpvectorop_mul_8bx8b,           0, _);
/* */ SIGNED_MAC_F(v64acc32, 64, 0, 0, v64, int8, v64, int8, I32, 8x8_8x8,
                   BMODE_I32_8x8, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_8x8_1_8x8_8x8, intr_gpvectorop_mul_8bx8b, 0, _);
/* */ SIGNED_MAC_F(v64acc32, 64, 0, 0, v64, int8, v64, int8, I32, elem_64,
                   BMODE_I32_8x8, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_8x8_1_elem_2, intr_gpvectorop_mul_8bx8b, 0, _);
/* */ SIGNED_MAC_F(v64acc32, 64, 0, 0, v128, int8, v128, int8, I32, elem_64_2,
                   BMODE_I32_8x8, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_8x8_1_elem_2, intr_gpvectorop_mul_8bx8b, 0, _);
/* */ SIGNED_MAC_F(v64acc32, 64, 0, 0, v128, int8, v64, int8, I32, conv_8x8_8ch,
                   BMODE_I32_8x8, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_8x8_1_conv_8x8_8ch, intr_gpvectorop_mul_8bx8b, 0, _);
/* */ SIGNED_MAC_F(v64acc32, 64, 0, 0, v128, int8, v64, int8, I32, conv_64x8,
                   BMODE_I32_8x8, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_8x8_1_conv_64x8, intr_gpvectorop_mul_8bx8b, 0, _);
/* */ SIGNED_MAC_F(v64acc32, 64, 0, 0, v64, int8, v128, int8, I32, 4x8_8x16,
                   BMODE_I32_8x8, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_8x8_1_4x8_8x16, intr_gpvectorop_mul_8bx8b, 0, _);
SIGNED_BCAST_MAC_F(v64acc32, 64, 0, 0, v64, int8, char, I32, elem_64,
                   BMODE_I32_8x8, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_8x8_1_elem_2, intr_gpvectorop_mul_elem_scl_8bx8b, 0,
                   _);
/* */ SIGNED_MAC_F(v64acc32, 64, 0, 0, v32, int16, v32, int16, I32, 8x2_2x8,
                   BMODE_I32_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_16x16_1_8x2_2x8, intr_gpvectorop_mul_16bx16b, 0, _);
/* */ SIGNED_MAC_F(v64acc32, 64, 0, 0, v64, int16, v64, int16, I32, elem_64,
                   BMODE_I32_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_16x16_1_elem, intr_gpvectorop_mul_16bx16b, 0, _);
/* */ SIGNED_MAC_F(v32acc32, 16, 0, 0, v32, int16, v32, int16, I32, elem_32_32b,
                   BMODE_I32_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_16x16_1_elem, intr_gpvectorop_mul_16bx16b, 0, _);
/* */ SIGNED_MAC_F(v32acc64, 32, 0, 0, v32, int16, v32, int16, I64, 4x4_4x8,
                   BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_16x16_2_4x4_4x8, intr_gpvectorop_mul_16bx16b, 0, _);
/* */ SIGNED_MAC_F(v32acc64, 32, 0, 0, v32, int16, v32, int16, I64, elem_32,
                   BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_16x16_2_elem_2, intr_gpvectorop_mul_16bx16b, 0, _);
/* */ SIGNED_MAC_F(v32acc64, 32, 0, 0, v64, int16, v64, int16, I64, elem_32_2,
                   BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_16x16_2_elem_2, intr_gpvectorop_mul_16bx16b, 0, _);
/* */ SIGNED_MAC_F(v32acc64, 32, 0, 0, v64, int16, v32, int16, I64, conv_32x4,
                   BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_16x16_2_conv_32x4, intr_gpvectorop_mul_16bx16b, 0,
                   _);
/* */ SIGNED_MAC_F(v32acc64, 32, 0, 0, v64, int16, v32, int16, I64,
                   conv_4x4_8ch, BMODE_I64_16x16, CMODE_SEL_DONT_CARE,
                   CMODE_SEL_DONT_CARE, VARIANT_16x16_2_conv_4x4_8ch,
                   intr_gpvectorop_mul_16bx16b, 0, _);

SIGNED_BCAST_MAC_F(v64acc32, 64, 0, 0, v64, int16, short, I32, elem_64,
                   BMODE_I32_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_16x16_1_elem, intr_gpvectorop_mul_elem_scl_16bx16b,
                   0, _);
SIGNED_BCAST_MAC_F(v32acc64, 32, 0, 0, v32, int16, short, I64, elem_32,
                   BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_16x16_2_elem_2, intr_gpvectorop_mul_elem_scl_16bx16b,
                   0, _);
/* */ SIGNED_MAC_F(v32acc64, 32, 0, 0, v16, int32, v32, int16, I64, 4x2_2x8,
                   BMODE_I64_32x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
                   VARIANT_32x16_2_4x2_2x8, intr_gpvectorop_mul_32bx16b, 0, _);

#if 0  // Complex Types
//! @defgroup intr_gpvectorop_mul_16bx16b_complex Multiply-accumulate of 16b x 16b complex integer datatypes
//! @{
/*  */MAC_F(v16cacc64, 32, 0, 1,      v16cint16,       v16cint16,    1,1,I64, elem_16,          BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE, VARIANT_16x16_2_elem_2_cplx,     intr_gpvectorop_mul_16bx16b_complex, 0, _);
/*  */MAC_F(v16cacc64, 32, 0, 1,      v32cint16,       v32cint16,    1,1,I64, elem_16_2,        BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE, VARIANT_16x16_2_elem_2_cplx,     intr_gpvectorop_mul_16bx16b_complex, 0, _); 
///*  */MAC_F( v8cacc64, 32, 0, 1,      v16cint16,       v16cint16,    1,1,I64, elem_8_2,         BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE, VARIANT_16x16_2_elem_2_cplx,     intr_gpvectorop_mul_16bx16b_complex, 0, _);
//SIZED_MAC_F(v16cacc64, 32, 0, 1, v32,v16,cint16,  v32,v16,cint16,    1,1,I64, elem_16_2,        BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE, VARIANT_16x16_2_elem_2_cplx,     intr_gpvectorop_mul_16bx16b_complex, 0, _);
///* */MAC_CONJ_F(v16cacc64, 32, _cc, OP_TERM_NEG_COMPLEX_CONJUGATE_X_Y, v16cint16,   v16cint16, 1, 1, 16, 16, I64, elem_8_2, BMODE_I64_16x16,CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE, 4, intr_gpvectorop_mul_16bx16b_complex, Channel by channel complex multiplication of (1x2) with (2x1) with a & b conjugate );
///* */MAC_CONJ_F(v16cacc64, 32, _cn, OP_TERM_NEG_COMPLEX_CONJUGATE_X,   v16cint16,   v16cint16, 1, 1, 16, 16, I64, elem_8_2, BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,4, intr_gpvectorop_mul_16bx16b_complex, Channel by channel complex multiplication of (1x2) with (2x1) with a conjugate only);
///* */MAC_CONJ_F(v16cacc64, 32, _nc, OP_TERM_NEG_COMPLEX_CONJUGATE_Y,   v16cint16,   v16cint16, 1, 1, 16, 16, I64, elem_8_2, BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,4, intr_gpvectorop_mul_16bx16b_complex, Channel by channel complex multiplication of (1x2) with (2x1) with b conjugate only);

/* */MAC_CONJ_F(v16cacc64, 32, _cc, OP_TERM_NEG_COMPLEX_CONJUGATE_X_Y, v32cint16,   v32cint16, 1, 1, 16, 16, I64, elem_16_2, BMODE_I64_16x16,CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE, 4, intr_gpvectorop_mul_16bx16b_complex, Channel by channel complex multiplication of (1x2) with (2x1) with a & b conjugate );
/* */MAC_CONJ_F(v16cacc64, 32, _cn, OP_TERM_NEG_COMPLEX_CONJUGATE_X,   v32cint16,   v32cint16, 1, 1, 16, 16, I64, elem_16_2, BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,4, intr_gpvectorop_mul_16bx16b_complex, Channel by channel complex multiplication of (1x2) with (2x1) with a conjugate only);
/* */MAC_CONJ_F(v16cacc64, 32, _nc, OP_TERM_NEG_COMPLEX_CONJUGATE_Y,   v32cint16,   v32cint16, 1, 1, 16, 16, I64, elem_16_2, BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,4, intr_gpvectorop_mul_16bx16b_complex, Channel by channel complex multiplication of (1x2) with (2x1) with b conjugate only);

//! @}
//! @defgroup intr_gpvectorop_mul_elem_scl_16bx16b_cmplx Vector x scalar multiply-accumulate of 16b x 16b complex integer datatypes
//! @{
/*  */MAC_F(v16cacc64, 32, 0, 1, v16cint16,  cint16, 1,1,I64, elem_16,   BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE, VARIANT_16x16_2_elem_2_cplx,     intr_gpvectorop_mul_elem_scl_16bx16b_cmplx, 0, _);
//! @}
//! @defgroup intr_gpvectorop_mul_32bx16b_complex Multiply-accumulate of 32b x 16b complex integer datatypes
//! @{
/*  */MAC_F(v8cacc64,  16, 0, 1,       v8cint32,       v16cint16,    1,1,I64, elem_8,          BMODE_I64_32x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE, VARIANT_32x16_2_elem_cplx,       intr_gpvectorop_mul_32bx16b_complex, 0, _);
/*  */MAC_F(v16cacc64, 32, 0, 1,      v16cint32,       v16cint16,    1,1,I64, elem_16,         BMODE_I64_32x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE, VARIANT_32x16_2_elem_cplx,       intr_gpvectorop_mul_32bx16b_complex, 0, _);
MAC_CONJ_F(v8cacc64, 16, _cc, OP_TERM_NEG_COMPLEX_CONJUGATE_X_Y, v8cint32,  v16cint16, 1, 1, 32, 16, I64,  elem_8, BMODE_I64_32x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,1, intr_gpvectorop_mul_32bx16b_complex, Channel by channel complex multiplication of (1x1) with (1x1) with a & b conjugate);
MAC_CONJ_F(v8cacc64, 16, _cn, OP_TERM_NEG_COMPLEX_CONJUGATE_X,   v8cint32,  v16cint16, 1, 1, 32, 16, I64,  elem_8, BMODE_I64_32x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,1, intr_gpvectorop_mul_32bx16b_complex, Channel by channel complex multiplication of (1x1) with (1x1) with a conjugate only);
MAC_CONJ_F(v8cacc64, 16, _nc, OP_TERM_NEG_COMPLEX_CONJUGATE_Y,   v8cint32,  v16cint16, 1, 1, 32, 16, I64,  elem_8, BMODE_I64_32x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,1, intr_gpvectorop_mul_32bx16b_complex, Channel by channel complex multiplication of (1x1) with (1x1) with b conjugate only);
//! @}
//! @defgroup intr_gpvectorop_mul_elem_scl_32bx16b_cmplx Vector x scalar multiply-accumulate of 32b x 16b complex integer datatypes
//! @{
/*  */MAC_F(v8cacc64,  16, 0, 1,       v8cint32,       cint16,    1,1,I64, elem_8,           BMODE_I64_32x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE, VARIANT_32x16_2_elem_cplx,       intr_gpvectorop_mul_elem_scl_32bx16b_cmplx, 0, _);
///*  */MAC_F(v16cacc64, 32, 0, 1,       v8cint32,       cint16,    1,1,I64, elem_16,          BMODE_I64_32x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE, VARIANT_32x16_2_elem_cplx,       intr_gpvectorop_mul_elem_scl_32bx16b_cmplx, 0, _);
/*  */MAC_F(v16cacc64, 32, 0, 1,      v16cint32,       cint16,    1,1,I64, elem_16,          BMODE_I64_32x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE, VARIANT_32x16_2_elem_cplx,       intr_gpvectorop_mul_elem_scl_32bx16b_cmplx, 0, _);
//! @}
#endif // Comples Types

/*   */ MAC_F(v64accfloat, 64, 1, 0, v64bfloat16, v64bfloat16, 0, 0, FP32,
              elem_64, BMODE_FP32_BF20, CMODE_SEL_BF16, CMODE_SEL_BF16,
              VARIANT_BF20xBF20_1_elem_1, intr_gpvectorop_mul_bf16xbf16, 0, _);
/*   */ MAC_F(v32accfloat, 32, 1, 0, v32bfloat16, v32bfloat16, 0, 0, FP32,
              elem_32, BMODE_FP32_BF20, CMODE_SEL_BF16, CMODE_SEL_BF16,
              VARIANT_BF20xBF20_1_elem_1, intr_gpvectorop_mul_bf16xbf16, 0, _);
/*   */ MAC_F(v32accfloat, 32, 1, 0, v32bfloat16, v64bfloat16, 0, 0, FP32,
              4x8_8x8, BMODE_FP32_BF20, CMODE_SEL_BF16, CMODE_SEL_BF16,
              VARIANT_BF20xBF20_1_4x8_8x8, intr_gpvectorop_mul_bf16xbf16, 0, _);

/*   */ MAC_F(v64accfloat, 64, 1, 0, v64bfloat16, v64float16, 0, 0, FP32,
              elem_64, BMODE_FP32_BF20, CMODE_SEL_BF16, CMODE_SEL_FP16,
              VARIANT_BF20xBF20_1_elem_1, intr_gpvectorop_mul_bf16xbf16, 0, _);
/*   */ MAC_F(v32accfloat, 32, 1, 0, v32bfloat16, v32float16, 0, 0, FP32,
              elem_32, BMODE_FP32_BF20, CMODE_SEL_BF16, CMODE_SEL_FP16,
              VARIANT_BF20xBF20_1_elem_1, intr_gpvectorop_mul_bf16xbf16, 0, _);
/*   */ MAC_F(v32accfloat, 32, 1, 0, v32bfloat16, v64float16, 0, 0, FP32,
              4x8_8x8, BMODE_FP32_BF20, CMODE_SEL_BF16, CMODE_SEL_FP16,
              VARIANT_BF20xBF20_1_4x8_8x8, intr_gpvectorop_mul_bf16xbf16, 0, _);
/*   */ MAC_F(v64accfloat, 64, 1, 0, v64float16, v64bfloat16, 0, 0, FP32,
              elem_64, BMODE_FP32_BF20, CMODE_SEL_FP16, CMODE_SEL_BF16,
              VARIANT_BF20xBF20_1_elem_1, intr_gpvectorop_mul_bf16xbf16, 0, _);
/*   */ MAC_F(v32accfloat, 32, 1, 0, v32float16, v32bfloat16, 0, 0, FP32,
              elem_32, BMODE_FP32_BF20, CMODE_SEL_FP16, CMODE_SEL_BF16,
              VARIANT_BF20xBF20_1_elem_1, intr_gpvectorop_mul_bf16xbf16, 0, _);
/*   */ MAC_F(v32accfloat, 32, 1, 0, v32float16, v64bfloat16, 0, 0, FP32,
              4x8_8x8, BMODE_FP32_BF20, CMODE_SEL_FP16, CMODE_SEL_BF16,
              VARIANT_BF20xBF20_1_4x8_8x8, intr_gpvectorop_mul_bf16xbf16, 0, _);
/*   */ MAC_F(v64accfloat, 64, 1, 0, v64float16, v64float16, 0, 0, FP32,
              elem_64, BMODE_FP32_BF20, CMODE_SEL_FP16, CMODE_SEL_FP16,
              VARIANT_BF20xBF20_1_elem_1, intr_gpvectorop_mul_fp16xfp16, 0, _);
/*   */ MAC_F(v32accfloat, 32, 1, 0, v32float16, v32float16, 0, 0, FP32,
              elem_32, BMODE_FP32_BF20, CMODE_SEL_FP16, CMODE_SEL_FP16,
              VARIANT_BF20xBF20_1_elem_1, intr_gpvectorop_mul_bf16xbf16, 0, _);
/*   */ MAC_F(v32accfloat, 32, 1, 0, v32float16, v64float16, 0, 0, FP32,
              4x8_8x8, BMODE_FP32_BF20, CMODE_SEL_FP16, CMODE_SEL_FP16,
              VARIANT_BF20xBF20_1_4x8_8x8, intr_gpvectorop_mul_fp16xfp16, 0, _);
/*   */ MAC_F(v32accfloat, 32, 1, 0, v32bfloat16, bfloat16, 0, 0, FP32, elem_32,
              BMODE_FP32_BF20, CMODE_SEL_BF16, CMODE_SEL_BF16,
              VARIANT_BF20xBF20_1_elem_1,
              intr_gpvectorop_mul_elem_scl_fp16xfp16, 0, _);
/*   */ MAC_F(v32accfloat, 32, 1, 0, v32bfloat16, float16, 0, 0, FP32, elem_32,
              BMODE_FP32_BF20, CMODE_SEL_BF16, CMODE_SEL_FP16,
              VARIANT_BF20xBF20_1_elem_1,
              intr_gpvectorop_mul_elem_scl_fp16xfp16, 0, _);
/*   */ MAC_F(v32accfloat, 32, 1, 0, v32float16, bfloat16, 0, 0, FP32, elem_32,
              BMODE_FP32_BF20, CMODE_SEL_FP16, CMODE_SEL_BF16,
              VARIANT_BF20xBF20_1_elem_1,
              intr_gpvectorop_mul_elem_scl_fp16xfp16, 0, _);
/*   */ MAC_F(v32accfloat, 32, 1, 0, v32float16, float16, 0, 0, FP32, elem_32,
              BMODE_FP32_BF20, CMODE_SEL_FP16, CMODE_SEL_FP16,
              VARIANT_BF20xBF20_1_elem_1,
              intr_gpvectorop_mul_elem_scl_fp16xfp16, 0, _);
/*   */ MAC_F(v64accfloat, 64, 1, 0, v64bfloat16, bfloat16, 0, 0, FP32, elem_64,
              BMODE_FP32_BF20, CMODE_SEL_BF16, CMODE_SEL_BF16,
              VARIANT_BF20xBF20_1_elem_1,
              intr_gpvectorop_mul_elem_scl_fp16xfp16, 0, _);
/*   */ MAC_F(v64accfloat, 64, 1, 0, v64bfloat16, float16, 0, 0, FP32, elem_64,
              BMODE_FP32_BF20, CMODE_SEL_BF16, CMODE_SEL_FP16,
              VARIANT_BF20xBF20_1_elem_1,
              intr_gpvectorop_mul_elem_scl_fp16xfp16, 0, _);
/*   */ MAC_F(v64accfloat, 64, 1, 0, v64float16, bfloat16, 0, 0, FP32, elem_64,
              BMODE_FP32_BF20, CMODE_SEL_FP16, CMODE_SEL_BF16,
              VARIANT_BF20xBF20_1_elem_1,
              intr_gpvectorop_mul_elem_scl_fp16xfp16, 0, _);
/*   */ MAC_F(v64accfloat, 64, 1, 0, v64float16, float16, 0, 0, FP32, elem_64,
              BMODE_FP32_BF20, CMODE_SEL_FP16, CMODE_SEL_FP16,
              VARIANT_BF20xBF20_1_elem_1,
              intr_gpvectorop_mul_elem_scl_fp16xfp16, 0, _);

/*   */ MAC_F(v64accfloat, 64, 1, 0, v64float8, v64float8, 0, 0, FP32, 8x8_8x8,
              BMODE_FP32_BF9, CMODE_SEL_FP8, CMODE_SEL_FP8,
              VARIANT_BF9xBF9_1_8x8_8x8, intr_gpvectorop_mul_fp8xfp8, 0, _);
/*   */ MAC_F(v64accfloat, 64, 1, 0, v64float8, v64bfloat8, 0, 0, FP32, 8x8_8x8,
              BMODE_FP32_BF9, CMODE_SEL_FP8, CMODE_SEL_BF8,
              VARIANT_BF9xBF9_1_8x8_8x8, intr_gpvectorop_mul_fp8xfp8, 0, _);
/*   */ MAC_F(v64accfloat, 64, 1, 0, v64bfloat8, v64float8, 0, 0, FP32, 8x8_8x8,
              BMODE_FP32_BF9, CMODE_SEL_BF8, CMODE_SEL_FP8,
              VARIANT_BF9xBF9_1_8x8_8x8, intr_gpvectorop_mul_fp8xfp8, 0, _);
/*   */ MAC_F(v64accfloat, 64, 1, 0, v64bfloat8, v64bfloat8, 0, 0, FP32,
              8x8_8x8, BMODE_FP32_BF9, CMODE_SEL_BF8, CMODE_SEL_BF8,
              VARIANT_BF9xBF9_1_8x8_8x8, intr_gpvectorop_mul_fp8xfp8, 0, _);
/*   */ MAC_F(v64accfloat, 64, 1, 0, v64mx9, v256mx9, 1, 1, FP32, 4x16_16x16T,
              BMODE_FP32_BFP16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
              VARIANT_BFP16xBFP16_1_4x8_8x16T_v64mx9_v256mx9,
              intr_gpvectorop_mul_bfp16xbfp16, 0, _);
/*   */ MAC_F(v64accfloat, 64, 1, 0, v128mx6, v256mx6, 1, 1, FP32, 4x16_16x16T,
              BMODE_FP32_BFP13, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
              VARIANT_BFP13xBFP13_1_4x16_16x16T_v128mx6_v256mx6,
              intr_gpvectorop_mul_bfp13xbfp13, 0, _);
/*   */ MAC_F(v64accfloat, 64, 1, 0, v128mx4, v256mx4, 1, 1, FP32, 4x16_16x16T,
              BMODE_FP32_BFP13, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
              VARIANT_BFP13xBFP13_1_4x16_16x16T_v128mx4_v256mx4,
              intr_gpvectorop_mul_bfp11xbfp11, 0, _);
#if 0  // Sparse Types
SIGNED_MAC_F(v64acc32, 64, 0, 0, v128, int8, v128, int8_sparse, I32, 8x16_16x8T,
             BMODE_I32_8x8, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
             VARIANT_8x8_1_8x16_16x8T_sparse, intr_gpvectorop_mul_sparse, 0, _);
SIGNED_MAC_F(v32acc32, 32, 0, 0, v64, int8, v128, int8_sparse, I32, 4x16_16x8T,
             BMODE_I32_8x8, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
             VARIANT_8x8_1_8x16_16x8T_sparse, intr_gpvectorop_mul_sparse, 0, _);
SIGNED_MAC_F(v64acc32, 64, 0, 0, v64, int8, v256, int8_sparse, I32, 4x16_16x16T,
             BMODE_I32_8x8, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
             VARIANT_8x8_1_4x16_16x16T_sparse, intr_gpvectorop_mul_sparse, 0,
             _);
SIGNED_MAC_F(v32acc64, 32, 0, 0, v32, int16, v64, int16_sparse, I64, 4x8_8x8T,
             BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
             VARIANT_16x16_2_4x8_8x8T_sparse, intr_gpvectorop_mul_sparse, 0, _);

/*   */ MAC_F(v32accfloat, 32, 1, 0, v64bfloat16, v128bfloat16_sparse, 1, 1,
              FP32, 4x16_16x8T, BMODE_FP32_BF20, CMODE_SEL_BF16, CMODE_SEL_BF16,
              VARIANT_BF20xBF20_1_4x16_16x8T_sparse, intr_gpvectorop_mul_sparse,
              0, _);
/*   */ MAC_F(v32accfloat, 32, 1, 0, v64bfloat16, v128float16_sparse, 1, 1,
              FP32, 4x16_16x8T, BMODE_FP32_BF20, CMODE_SEL_BF16, CMODE_SEL_FP16,
              VARIANT_BF20xBF20_1_4x16_16x8T_sparse, intr_gpvectorop_mul_sparse,
              0, _);
/*   */ MAC_F(v32accfloat, 32, 1, 0, v64float16, v128bfloat16_sparse, 1, 1,
              FP32, 4x16_16x8T, BMODE_FP32_BF20, CMODE_SEL_FP16, CMODE_SEL_BF16,
              VARIANT_BF20xBF20_1_4x16_16x8T_sparse, intr_gpvectorop_mul_sparse,
              0, _);
/*   */ MAC_F(v32accfloat, 32, 1, 0, v64float16, v128float16_sparse, 1, 1, FP32,
              4x16_16x8T, BMODE_FP32_BF20, CMODE_SEL_FP16, CMODE_SEL_FP16,
              VARIANT_BF20xBF20_1_4x16_16x8T_sparse, intr_gpvectorop_mul_sparse,
              0, _);

/*   */ MAC_F(v64accfloat, 64, 1, 0, v64float8, v256float8_sparse, 1, 1, FP32,
              4x16_16x16T, BMODE_FP32_BF9, CMODE_SEL_FP8, CMODE_SEL_FP8,
              VARIANT_BF9xBF9_1_4x16_16x16T_sparse, intr_gpvectorop_mul_sparse,
              0, _);
/*   */ MAC_F(v64accfloat, 64, 1, 0, v64float8, v256bfloat8_sparse, 1, 1, FP32,
              4x16_16x16T, BMODE_FP32_BF9, CMODE_SEL_FP8, CMODE_SEL_BF8,
              VARIANT_BF9xBF9_1_4x16_16x16T_sparse, intr_gpvectorop_mul_sparse,
              0, _);
/*   */ MAC_F(v64accfloat, 64, 1, 0, v64bfloat8, v256float8_sparse, 1, 1, FP32,
              4x16_16x16T, BMODE_FP32_BF9, CMODE_SEL_BF8, CMODE_SEL_FP8,
              VARIANT_BF9xBF9_1_4x16_16x16T_sparse, intr_gpvectorop_mul_sparse,
              0, _);
/*   */ MAC_F(v64accfloat, 64, 1, 0, v64bfloat8, v256bfloat8_sparse, 1, 1, FP32,
              4x16_16x16T, BMODE_FP32_BF9, CMODE_SEL_BF8, CMODE_SEL_BF8,
              VARIANT_BF9xBF9_1_4x16_16x16T_sparse, intr_gpvectorop_mul_sparse,
              0, _);

SIGNED_MAC_F(v64acc32, 64, 0, 0, v128, int8, v128, int8_sparse, I32, 8x16_16x8,
             BMODE_I32_8x8, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
             VARIANT_8x8_1_8x16_16x8T_sparse, intr_gpvectorop_mul_sparse, 1,
             T8_8x8);
// SIGNED_MAC_F(v32acc32,    32, 0, 0,  v64,  int8, v128,   int8_sparse, I32,
// 8x16_16x8,   BMODE_I32_8x8, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
// VARIANT_8x8_1_8x16_16x8T_sparse,          intr_gpvectorop_mul_sparse, 1,
// T8_8x8);
SIGNED_MAC_F(v64acc32, 64, 0, 0, v64, int8, v256, int8_sparse, I32, 4x16_16x16,
             BMODE_I32_8x8, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
             VARIANT_8x8_1_4x16_16x16T_sparse, intr_gpvectorop_mul_sparse, 1,
             T8_8x16_hilo); /*EMU 16x8*/
SIGNED_MAC_F(v32acc64, 32, 0, 0, v32, int16, v64, int16_sparse, I64, 4x8_8x8,
             BMODE_I64_16x16, CMODE_SEL_DONT_CARE, CMODE_SEL_DONT_CARE,
             VARIANT_16x16_2_4x8_8x8T_sparse, intr_gpvectorop_mul_sparse, 1,
             T16_4x8);
#endif // Sparse Types

/*   */ MUL_CMPLX_F(v64accfloat, 64, 1, v64bfloat16, v64bfloat16, FP32, elem_64,
                    BMODE_FP32_BF20, CMODE_SEL_BF16, CMODE_SEL_BF16,
                    VARIANT_BF20xBF20_1_elem_1);
/*   */ MUL_CMPLX_F(v32accfloat, 32, 1, v32bfloat16, v32bfloat16, FP32, elem_32,
                    BMODE_FP32_BF20, CMODE_SEL_BF16, CMODE_SEL_BF16,
                    VARIANT_BF20xBF20_1_elem_1);
/*   */ MUL_CMPLX_F(v32accfloat, 32, 1, v32bfloat16, v64bfloat16, FP32, 4x8_8x8,
                    BMODE_FP32_BF20, CMODE_SEL_BF16, CMODE_SEL_BF16,
                    VARIANT_BF20xBF20_1_4x8_8x8);

#undef MAC_F
#undef MUL_CMPLX_F
#undef SIZE_SIGNED_MAC_F
#undef SIGNED_MAC_F
#undef SIGNED_BCAST_MAC_F
#undef SIZED_MAC_F

// ---------------------------
// Accumulator adder
// ---------------------------

#define BMODE(acc_cmb) (PP_CAT(BMODE_ACC_DEFAULT_, acc_cmb))

// Select the appropriate accumulator type
#define SELECT_ACC_TYPE(ACCNUM, BF)                                            \
  PP_IF(BF, v64accfloat, PP_IF(PP_EQUAL(ACCNUM, 64), v64acc32, v32acc64))

// Select the appropriate set function
#define SET_ACC_TYPE(ACCNUM, BF)                                               \
  PP_IF(BF, set_v64accfloat,                                                   \
        PP_IF(PP_EQUAL(ACCNUM, 64), set_v64acc32, set_v32acc64))

// Macro to define and initialize op0 and op1
#define DEFINE_AND_INIT_OPS(ACCNUM, BF, acc1, acc2)                            \
  SELECT_ACC_TYPE(ACCNUM, BF) op0 = SET_ACC_TYPE(ACCNUM, BF)(0, acc1);         \
  SELECT_ACC_TYPE(ACCNUM, BF) op1 = SET_ACC_TYPE(ACCNUM, BF)(0, acc2);

#define ACC_F(ACC, ACCNUM, CMPLX, INT, FP32, BF16, ACC_CMB, VARIANT, HALF,     \
              ACC_SIZE)                                                        \
  INTRINSIC(ACC) add(ACC acc1, ACC acc2) {                                     \
    PP_IF(PP_OR(PP_EQUAL(ACC_SIZE, 2048), PP_EQUAL(ACC_SIZE, 1024)),           \
          int conf = aie2ps_compute_control(                                   \
              0, 0, AMODE(ACC_CMB), BMODE(ACC_CMB), CMODE_SEL_DONT_CARE,       \
              CMODE_SEL_DONT_CARE, VARIANT, 0, 0, 0, 0, 0, 0);                 \
          return PP_CAT3(__builtin_aie2ps_, TYPE_NAME(ACC),                    \
                         PP_CAT(PP_IF(PP_OR(BF16, FP32), _accfloat_add, _add), \
                                _conf)(acc1, acc2, conf));                     \
          , DEFINE_AND_INIT_OPS(ACCNUM, PP_OR(BF16, FP32), acc1, acc2)         \
                SELECT_ACC_TYPE(ACCNUM, PP_OR(BF16, FP32)) res =               \
                    add(op0, op1);                                             \
          return PP_CAT(extract_, ACC(res, 0));)                               \
  }                                                                            \
  INTRINSIC(ACC) sub(ACC acc1, ACC acc2) {                                     \
    PP_IF(PP_OR(PP_EQUAL(ACC_SIZE, 2048), PP_EQUAL(ACC_SIZE, 1024)),           \
          int conf = aie2ps_compute_control(                                   \
              0, 0, AMODE(ACC_CMB), BMODE(ACC_CMB), CMODE_SEL_DONT_CARE,       \
              CMODE_SEL_DONT_CARE, VARIANT, 0, 0, 0, 0, 0, 0);                 \
          return PP_CAT3(__builtin_aie2ps_, TYPE_NAME(ACC),                    \
                         PP_CAT(PP_IF(PP_OR(BF16, FP32), _accfloat_sub, _sub), \
                                _conf)(acc1, acc2, conf));                     \
          , DEFINE_AND_INIT_OPS(ACCNUM, PP_OR(BF16, FP32), acc1, acc2)         \
                SELECT_ACC_TYPE(ACCNUM, PP_OR(BF16, FP32)) res =               \
                    sub(op0, op1);                                             \
          return PP_CAT(extract_, ACC(res, 0));)                               \
  }                                                                            \
  INTRINSIC(ACC) neg(ACC acc) {                                                \
    PP_IF(PP_OR(PP_EQUAL(ACC_SIZE, 2048), PP_EQUAL(ACC_SIZE, 1024)),           \
          int conf = aie2ps_compute_control(                                   \
              0, 0, AMODE(ACC_CMB), BMODE(ACC_CMB), CMODE_SEL_DONT_CARE,       \
              CMODE_SEL_DONT_CARE, VARIANT, 0, 0, 0, 0, 0, 0);                 \
          return PP_CAT3(__builtin_aie2ps_, TYPE_NAME(ACC),                    \
                         PP_CAT(PP_IF(PP_OR(BF16, FP32), _accfloat_neg, _neg), \
                                _conf)(acc, conf));                            \
          , SELECT_ACC_TYPE(ACCNUM, PP_OR(BF16, FP32)) op =                    \
                SET_ACC_TYPE(ACCNUM, PP_OR(BF16, FP32))(0, acc);               \
          SELECT_ACC_TYPE(ACCNUM, PP_OR(BF16, FP32)) res = neg(op);            \
          return PP_CAT(extract_, ACC(res, 0));)                               \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  add_conf(ACC acc1, ACC acc2, int zero_acc1,                                  \
           PP_IF(INT, int shift16 PP_COMMA, PP_EMPTY)() int sub_acc1,          \
           int sub_acc2) {                                                     \
    PP_IF(PP_OR(PP_EQUAL(ACC_SIZE, 2048), PP_EQUAL(ACC_SIZE, 1024)),           \
          int conf = aie2ps_compute_control(                                   \
              0, 0, AMODE(ACC_CMB), BMODE(ACC_CMB), CMODE_SEL_DONT_CARE,       \
              CMODE_SEL_DONT_CARE, VARIANT, zero_acc1, PP_IF(INT, shift16, 0), \
              0, sub_acc1, sub_acc2, 0);                                       \
          return PP_CAT3(__builtin_aie2ps_, TYPE_NAME(ACC),                    \
                         PP_CAT(PP_IF(PP_OR(BF16, FP32), _accfloat_add, _add), \
                                _conf)(acc1, acc2, conf));                     \
          , DEFINE_AND_INIT_OPS(ACCNUM, PP_OR(BF16, FP32), acc1, acc2)         \
                SELECT_ACC_TYPE(ACCNUM, PP_OR(BF16, FP32)) res =               \
                    add_conf(op0, op1, zero_acc1,                              \
                             PP_IF(PP_NOT(PP_OR(BF16, FP32)),                  \
                                   shift16 PP_COMMA, PP_EMPTY)() sub_acc1,     \
                             sub_acc2);                                        \
          return PP_CAT(extract_, ACC(res, 0));)                               \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  sub_conf(ACC acc1, ACC acc2, int zero_acc1,                                  \
           PP_IF(INT, int shift16 PP_COMMA, PP_EMPTY)() int sub_acc1,          \
           int sub_acc2) {                                                     \
    PP_IF(PP_OR(PP_EQUAL(ACC_SIZE, 2048), PP_EQUAL(ACC_SIZE, 1024)),           \
          int conf = aie2ps_compute_control(                                   \
              0, 0, AMODE(ACC_CMB), BMODE(ACC_CMB), CMODE_SEL_DONT_CARE,       \
              CMODE_SEL_DONT_CARE, VARIANT, zero_acc1, PP_IF(INT, shift16, 0), \
              0, sub_acc1, sub_acc2, 0);                                       \
          return PP_CAT3(__builtin_aie2ps_, TYPE_NAME(ACC),                    \
                         PP_CAT(PP_IF(PP_OR(BF16, FP32), _accfloat_sub, _sub), \
                                _conf)(acc1, acc2, conf));                     \
          , DEFINE_AND_INIT_OPS(ACCNUM, PP_OR(BF16, FP32), acc1, acc2)         \
                SELECT_ACC_TYPE(ACCNUM, PP_OR(BF16, FP32)) res =               \
                    sub_conf(op0, op1, zero_acc1,                              \
                             PP_IF(PP_NOT(PP_OR(BF16, FP32)),                  \
                                   shift16 PP_COMMA, PP_EMPTY)() sub_acc1,     \
                             sub_acc2);                                        \
          return PP_CAT(extract_, ACC(res, 0));)                               \
  }                                                                            \
  INTRINSIC(ACC)                                                               \
  neg_conf(ACC acc, int zero_acc1,                                             \
           PP_IF(INT, int shift16 PP_COMMA, PP_EMPTY)() int sub_acc) {         \
    PP_IF(PP_OR(PP_EQUAL(ACC_SIZE, 2048), PP_EQUAL(ACC_SIZE, 1024)),           \
          int conf = aie2ps_compute_control(                                   \
              0, 0, AMODE(ACC_CMB), BMODE(ACC_CMB), CMODE_SEL_DONT_CARE,       \
              CMODE_SEL_DONT_CARE, VARIANT, zero_acc1, PP_IF(INT, shift16, 0), \
              0, sub_acc, 0, 0);                                               \
          return PP_CAT3(__builtin_aie2ps_, TYPE_NAME(ACC),                    \
                         PP_CAT(PP_IF(PP_OR(BF16, FP32), _accfloat_neg, _neg), \
                                _conf)(acc, conf));                            \
          , SELECT_ACC_TYPE(ACCNUM, PP_OR(BF16, FP32)) op =                    \
                SET_ACC_TYPE(ACCNUM, PP_OR(BF16, FP32))(0, acc);               \
          SELECT_ACC_TYPE(ACCNUM, PP_OR(BF16, FP32)) res =                     \
              neg_conf(op, zero_acc1,                                          \
                       PP_IF(PP_NOT(PP_OR(BF16, FP32)), shift16 PP_COMMA,      \
                             PP_EMPTY)() sub_acc);                             \
          return PP_CAT(extract_, ACC(res, 0));)                               \
  }

#define CLR_F(ACC, ACCNUM, CMPLX, INT, FP32, BF16, ACC_CMB, VARIANT, HALF,     \
              ACC_SIZE)                                                        \
  [[deprecated("Function 'clr' is deprecated. Please use the "                 \
               "'broadcast_zero_to' variant instead.")]]                       \
  INTRINSIC(ACC) PP_CAT3(clr, _, ACC)() {                                      \
    PP_IF_VA_ARGS(                                                             \
        PP_AND(FP32, PP_EQUAL(ACC_SIZE, 2048)),                                \
        return __builtin_bit_cast(v64accfloat, (float)0 - v64float{0}))        \
    PP_IF_VA_ARGS(                                                             \
        PP_AND(FP32, PP_EQUAL(ACC_SIZE, 1024)),                                \
        return __builtin_bit_cast(v32accfloat, (float)0 - v32float{0}))        \
    PP_IF_VA_ARGS(                                                             \
        PP_AND(FP32, PP_EQUAL(ACC_SIZE, 512)),                                 \
        return __builtin_bit_cast(v16accfloat, (float)0 - v16float{0}))        \
    PP_IF_VA_ARGS(                                                             \
        PP_NOT(FP32),                                                          \
        return PP_CAT6(                                                        \
            __builtin_aie2ps_, clr_, TYPE_NAME(ACC), PP_IF(HALF, l, ),         \
            PP_IF(CMPLX, c, PP_IF(FP32, f, PP_IF(BF16, bf, ))), _conf)());     \
  }                                                                            \
  INTRINSIC(ACC) PP_CAT(broadcast_zero_to_, ACC)() {                           \
    PP_IF_VA_ARGS(                                                             \
        PP_AND(FP32, PP_EQUAL(ACC_SIZE, 2048)),                                \
        return __builtin_bit_cast(v64accfloat, (float)0 - v64float{0}))        \
    PP_IF_VA_ARGS(                                                             \
        PP_AND(FP32, PP_EQUAL(ACC_SIZE, 1024)),                                \
        return __builtin_bit_cast(v32accfloat, (float)0 - v32float{0}))        \
    PP_IF_VA_ARGS(                                                             \
        PP_AND(FP32, PP_EQUAL(ACC_SIZE, 512)),                                 \
        return __builtin_bit_cast(v16accfloat, (float)0 - v16float{0}))        \
    PP_IF_VA_ARGS(                                                             \
        PP_NOT(FP32),                                                          \
        return PP_CAT6(                                                        \
            __builtin_aie2ps_, clr_, TYPE_NAME(ACC), PP_IF(HALF, l, ),         \
            PP_IF(CMPLX, c, PP_IF(FP32, f, PP_IF(BF16, bf, ))), _conf)());     \
  }

ACC_F(v64acc32, 64, 0, 1, 0, 0, I32, VARIANT_8x8_1_8x8_8x8, 0, 2048)
ACC_F(v32acc64, 32, 0, 1, 0, 0, I64, VARIANT_32x16_2_4x2_2x8, 0, 2048)
// ACC_F(v16cacc64, 32, 1, 1, 0, 0, I64, VARIANT_32x16_2_4x2_2x8, 0, 2048)
ACC_F(v64accfloat, 64, 0, 0, 1, 0, FP32, VARIANT_BF20xBF20_1_elem_1, 0, 2048)
ACC_F(v32accfloat, 32, 0, 0, 1, 0, FP32, VARIANT_BF20xBF20_1_elem_1, 0, 1024)
ACC_F(v16accfloat, 16, 0, 0, 1, 0, FP32, VARIANT_BF20xBF20_1_elem_1, 0, 512)
// ACC_F(v8caccfloat, 16, 0, 0, 1, 0, FP32, VARIANT_BF20xBF20_1_elem_1, 0, 512)
// ACC_F(v16caccfloat, 32, 0, 0, 1, 0, FP32, VARIANT_BF20xBF20_1_elem_1, 0,
// 1024) ACC_F(v32caccfloat, 64, 0, 0, 1, 0, FP32, VARIANT_BF20xBF20_1_elem_1,
// 0, 2048)
ACC_F(v32acc32, 32, 0, 1, 0, 0, I32, VARIANT_8x8_1_8x8_8x8, 1, 1024)
ACC_F(v16acc64, 16, 0, 1, 0, 0, I64, VARIANT_32x16_2_4x2_2x8, 1, 1024)
// ACC_F(v8cacc64, 16, 1, 1, 0, 0, I64, VARIANT_32x16_2_4x2_2x8, 1, 1024)
CLR_F(v64acc32, 64, 0, 1, 0, 0, I32, VARIANT_8x8_1_8x8_8x8, 0, 2048)
CLR_F(v32acc64, 32, 0, 1, 0, 0, I64, VARIANT_32x16_2_4x2_2x8, 0, 2048)
// CLR_F(v16cacc64, 32, 1, 1, 0, 0, I64, VARIANT_32x16_2_4x2_2x8, 0, 2048)
CLR_F(v64accfloat, 64, 0, 0, 1, 0, FP32, VARIANT_BF20xBF20_1_elem_1, 0, 2048)
CLR_F(v32accfloat, 32, 0, 0, 1, 0, FP32, VARIANT_BF20xBF20_1_elem_1, 0, 1024)
CLR_F(v16accfloat, 16, 0, 0, 1, 0, FP32, VARIANT_BF20xBF20_1_elem_1, 0, 512)
CLR_F(v32acc32, 32, 0, 1, 0, 0, I32, VARIANT_8x8_1_8x8_8x8, 1, 1024)
CLR_F(v16acc64, 16, 0, 1, 0, 0, I64, VARIANT_32x16_2_4x2_2x8, 1, 2048)
// CLR_F(v8cacc64, 16, 1, 1, 0, 0, I64, VARIANT_32x16_2_4x2_2x8, 1, 1024)

#undef BMODE
#undef AMODE
#undef ACC_F
#undef CLR_F

#include "aie2ps_vmult_emulated.h"
#include "aie2ps_vmult_float_emulated.h"
#endif // __AIE2PS_VMULT_H__
