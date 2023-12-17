//===- aiev1_vfloat.h -------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV1_VFLOAT_H__
#define __AIEV1_VFLOAT_H__

enum __cmpmode {
  __fpmul_lt = 0,
  __fpmul_ge = 1,
  __fpmul_nrm = 2,
};

enum __addmode {
  __fpmul_add = 0,
  __fpmul_sub = 1,
  __fpmul_mixadd = 2,
  __fpmul_mixsub = 3,
};

INTRINSIC(uint32_t)
__fpconf(bool ones, bool abs, unsigned char addmask, __cmpmode cmpmode) {
  uint32_t retval = 0;
  retval |= (abs ? 1 << 19 : 0);
  retval |= (1 << 20);
  retval |= (ones ? 1 << 21 : 0);
  retval |= (cmpmode & 0x3) << 22;
  retval |= addmask << 24;
  return retval;
}

INTRINSIC(v8float)
fpmul(v32float xbuf, int xstart, unsigned int xoffs, v8float zbuf, int zstart,
      unsigned int zoffs) {
  return __builtin_aie_vfpmul(xbuf, xstart, xoffs, zbuf, zstart, zoffs,
                              __fpmul_add,
                              __fpconf(false, false, 0, __fpmul_nrm));
}

INTRINSIC(v8float)
fpmac(v8float acc, v32float xbuf, int xstart, unsigned int xoffs, v8float zbuf,
      int zstart, unsigned int zoffs) {
  return __builtin_aie_vfpmac(acc, xbuf, xstart, xoffs, zbuf, zstart, zoffs,
                              __fpmul_add,
                              __fpconf(false, false, 0, __fpmul_nrm));
}

INTRINSIC(v8float)
fpadd(v8float acc, v32float xbuf, int xstart, unsigned int xoffs) {
  return __builtin_aie_vfpmac(acc, xbuf, xstart, xoffs, undef_v8float(), 0, 0,
                              __fpmul_add,
                              __fpconf(true, false, 0, __fpmul_nrm));
}

INTRINSIC(v8float)
fpmin(v8float acc, v32float xbuf, int xstart, unsigned int xoffs) {
  return __builtin_aie_vfpmac(acc, xbuf, xstart, xoffs, undef_v8float(), 0, 0,
                              __fpmul_sub,
                              __fpconf(true, false, 0, __fpmul_ge));
}

INTRINSIC(v8float)
fpmax(v8float acc, v32float xbuf, int xstart, unsigned int xoffs) {
  return __builtin_aie_vfpmac(acc, xbuf, xstart, xoffs, undef_v8float(), 0, 0,
                              __fpmul_sub,
                              __fpconf(true, false, 0, __fpmul_lt));
}

INTRINSIC(v8float) fpabs(v32float xbuf, int xstart, unsigned int xoffs) {
  return __builtin_aie_vfpmul(xbuf, xstart, xoffs, undef_v8float(), 0, 0,
                              __fpmul_add,
                              __fpconf(true, true, 0, __fpmul_nrm));
}

INTRINSIC(v8float) fpshuffle(v32float xbuf, int xstart, unsigned int xoffs) {
  return __builtin_aie_vfpmul(xbuf, xstart, xoffs, undef_v8float(), 0, 0,
                              __fpmul_add,
                              __fpconf(true, false, 0, __fpmul_nrm));
}

// Simple floating point builtins
INTRINSIC(v8float) fpmul(v8float xbuf, v8float zbuf) {
  return __builtin_aie_vfpsimplemul(xbuf, zbuf, __fpmul_add,
                                    __fpconf(false, false, 0, __fpmul_nrm));
}

INTRINSIC(v8float) fpmac(v8float acc, v8float xbuf, v8float zbuf) {
  return __builtin_aie_vfpsimplemac(acc, xbuf, zbuf, __fpmul_add,
                                    __fpconf(false, false, 0, __fpmul_nrm));
}

INTRINSIC(v8float) fpadd(v8float acc, v8float xbuf) {
  return __builtin_aie_vfpsimplemac(acc, xbuf, undef_v8float(), __fpmul_add,
                                    __fpconf(true, false, 0, __fpmul_nrm));
}

INTRINSIC(v8float) fpmin(v8float acc, v8float xbuf) {
  return __builtin_aie_vfpsimplemac(acc, xbuf, undef_v8float(), __fpmul_sub,
                                    __fpconf(true, false, 0, __fpmul_ge));
}

INTRINSIC(v8float) fpmax(v8float acc, v8float xbuf) {
  return __builtin_aie_vfpsimplemac(acc, xbuf, undef_v8float(), __fpmul_sub,
                                    __fpconf(true, false, 0, __fpmul_lt));
}

INTRINSIC(v8float) fpabs(v8float xbuf) {
  return __builtin_aie_vfpsimplemul(xbuf, undef_v8float(), __fpmul_add,
                                    __fpconf(true, true, 0, __fpmul_nrm));
}

INTRINSIC(v8float) fpshuffle(v8float xbuf) {
  return __builtin_aie_vfpsimplemul(xbuf, undef_v8float(), __fpmul_add,
                                    __fpconf(true, false, 0, __fpmul_nrm));
}

#endif //__AIEV1_VFLOAT_H__
