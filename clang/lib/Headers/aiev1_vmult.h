//===- aiev1_vmult.h --------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

//* Automatically generated file, do not edit! *

#ifndef __AIEV1MULT_H
#define __AIEV1MULT_H

INTRINSIC(v16acc48)
mul16(v32int32 xbuff, int xstart, unsigned int xoffsets,
      unsigned int xoffsets_hi, v16int16 zbuff, int zstart,
      unsigned int zoffsets, unsigned int zoffsets_hi) {
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  xoffs[1] = xoffsets_hi;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  zoffs[1] = zoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = (((0) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  return __builtin_aie_mul16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
}
INTRINSIC(v16acc48)
mul16_abs(v32int32 xbuff, int xstart, unsigned int xoffsets,
          unsigned int xoffsets_hi, v16int16 zbuff, int zstart,
          unsigned int zoffsets, unsigned int zoffsets_hi) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  xoffs[1] = xoffsets_hi;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  zoffs[1] = zoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((6) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v16acc48 result;
  result = __builtin_aie_mul16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mul16_sym(v32int32 xbuff, int xstart, unsigned int xoffsets,
          unsigned int xoffsets_hi, int ystart, v16int16 zbuff, int zstart,
          unsigned int zoffsets, unsigned int zoffsets_hi) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  xoffs[1] = xoffsets_hi;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  zoffs[1] = zoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (((0) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v16acc48 result;
  result = __builtin_aie_mul16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mul16_antisym(v32int32 xbuff, int xstart, unsigned int xoffsets,
              unsigned int xoffsets_hi, int ystart, v16int16 zbuff, int zstart,
              unsigned int zoffsets, unsigned int zoffsets_hi) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  xoffs[1] = xoffsets_hi;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  zoffs[1] = zoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((0) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v16acc48 result;
  result = __builtin_aie_mul16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mul16_max(v32int32 xbuff, int xstart, unsigned int xoffsets,
          unsigned int xoffsets_hi, int ystart, v16int16 zbuff, int zstart,
          unsigned int zoffsets, unsigned int zoffsets_hi) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  xoffs[1] = xoffsets_hi;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  zoffs[1] = zoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((5) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v16acc48 result;
  result = __builtin_aie_mul16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mul16_min(v32int32 xbuff, int xstart, unsigned int xoffsets,
          unsigned int xoffsets_hi, int ystart, v16int16 zbuff, int zstart,
          unsigned int zoffsets, unsigned int zoffsets_hi) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  xoffs[1] = xoffsets_hi;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  zoffs[1] = zoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((4) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v16acc48 result;
  result = __builtin_aie_mul16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mul16_maxdiff(v32int32 xbuff, int xstart, unsigned int xoffsets,
              unsigned int xoffsets_hi, int ystart, v16int16 zbuff, int zstart,
              unsigned int zoffsets, unsigned int zoffsets_hi) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  xoffs[1] = xoffsets_hi;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  zoffs[1] = zoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((7) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v16acc48 result;
  result = __builtin_aie_mul16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
negmul16(v32int32 xbuff, int xstart, unsigned int xoffsets,
         unsigned int xoffsets_hi, v16int16 zbuff, int zstart,
         unsigned int zoffsets, unsigned int zoffsets_hi) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  xoffs[1] = xoffsets_hi;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  zoffs[1] = zoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = (((0) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v16acc48 result;
  result = __builtin_aie_mul16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
negmul16_abs(v32int32 xbuff, int xstart, unsigned int xoffsets,
             unsigned int xoffsets_hi, v16int16 zbuff, int zstart,
             unsigned int zoffsets, unsigned int zoffsets_hi) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  xoffs[1] = xoffsets_hi;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  zoffs[1] = zoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((6) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v16acc48 result;
  result = __builtin_aie_mul16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
negmul16_sym(v32int32 xbuff, int xstart, unsigned int xoffsets,
             unsigned int xoffsets_hi, int ystart, v16int16 zbuff, int zstart,
             unsigned int zoffsets, unsigned int zoffsets_hi) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  xoffs[1] = xoffsets_hi;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  zoffs[1] = zoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (((0) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v16acc48 result;
  result = __builtin_aie_mul16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
negmul16_antisym(v32int32 xbuff, int xstart, unsigned int xoffsets,
                 unsigned int xoffsets_hi, int ystart, v16int16 zbuff,
                 int zstart, unsigned int zoffsets, unsigned int zoffsets_hi) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  xoffs[1] = xoffsets_hi;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  zoffs[1] = zoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((0) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v16acc48 result;
  result = __builtin_aie_mul16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
negmul16_max(v32int32 xbuff, int xstart, unsigned int xoffsets,
             unsigned int xoffsets_hi, int ystart, v16int16 zbuff, int zstart,
             unsigned int zoffsets, unsigned int zoffsets_hi) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  xoffs[1] = xoffsets_hi;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  zoffs[1] = zoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((5) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v16acc48 result;
  result = __builtin_aie_mul16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
negmul16_min(v32int32 xbuff, int xstart, unsigned int xoffsets,
             unsigned int xoffsets_hi, int ystart, v16int16 zbuff, int zstart,
             unsigned int zoffsets, unsigned int zoffsets_hi) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  xoffs[1] = xoffsets_hi;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  zoffs[1] = zoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((4) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v16acc48 result;
  result = __builtin_aie_mul16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
negmul16_maxdiff(v32int32 xbuff, int xstart, unsigned int xoffsets,
                 unsigned int xoffsets_hi, int ystart, v16int16 zbuff,
                 int zstart, unsigned int zoffsets, unsigned int zoffsets_hi) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  xoffs[1] = xoffsets_hi;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  zoffs[1] = zoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((7) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v16acc48 result;
  result = __builtin_aie_mul16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mul16(v128int8 xbuff, int xstart, unsigned int xoffsets, int xstep,
      unsigned int xsquare, v32int8 zbuff, unsigned int zstart,
      unsigned int zoffsets, int zstep, unsigned int zsquare) {
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (((0) & 0x0F) << 23) | (((zstep)&0x3F) << 8) |
            (((xstep >> 1) & 0x3F) << 0) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17) |
            (((xsquare)&0x0003) | (((xsquare)&0x0030) >> 2) |
             (((xsquare)&0x0300) >> 4) | (((xsquare)&0x3000) >> 6)) |
            ((((zsquare)&0x0003) << 8) | (((zsquare)&0x0030) << 6) |
             (((zsquare)&0x0300) << 4) | (((zsquare)&0x3000) << 2));
  return __builtin_aie_mul16_v128int8_v32int8_bm_sw48(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
}
INTRINSIC(v16acc48)
negmul16(v128int8 xbuff, int xstart, unsigned int xoffsets, int xstep,
         unsigned int xsquare, v32int8 zbuff, int zstart, unsigned int zoffsets,
         int zstep, unsigned int zsquare) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (((0) & 0x0F) << 23) | (((zstep)&0x3F) << 8) |
            (((xstep >> 1) & 0x3F) << 0) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17) |
            (((xsquare)&0x0003) | (((xsquare)&0x0030) >> 2) |
             (((xsquare)&0x0300) >> 4) | (((xsquare)&0x3000) >> 6)) |
            ((((zsquare)&0x0003) << 8) | (((zsquare)&0x0030) << 6) |
             (((zsquare)&0x0300) << 4) | (((zsquare)&0x3000) << 2));
  v16acc48 result;
  result = __builtin_aie_mul16_v128int8_v32int8_bm_sw48(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v8acc80)
lmul8(v32int32 xbuff, int xstart, unsigned int xoffsets, v8int32 zbuff,
      int zstart, unsigned int zoffsets) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (((0) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v8acc80 result;
  result = __builtin_aie_mul8_v32int32_v8int32_bm_sw80(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v8acc80)
lmul8_abs(v32int32 xbuff, int xstart, unsigned int xoffsets, v8int32 zbuff,
          int zstart, unsigned int zoffsets) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((6) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v8acc80 result;
  result = __builtin_aie_mul8_v32int32_v8int32_bm_sw80(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v8acc80)
lmul8_sym(v32int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
          v8int32 zbuff, int zstart, unsigned int zoffsets) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (((0) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v8acc80 result;
  result = __builtin_aie_mul8_v32int32_v8int32_bm_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v8acc80)
lmul8_antisym(v32int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
              v8int32 zbuff, int zstart, unsigned int zoffsets) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((0) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v8acc80 result;
  result = __builtin_aie_mul8_v32int32_v8int32_bm_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v8acc80)
lmul8_max(v32int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
          v8int32 zbuff, int zstart, unsigned int zoffsets) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((5) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v8acc80 result;
  result = __builtin_aie_mul8_v32int32_v8int32_bm_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v8acc80)
lmul8_min(v32int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
          v8int32 zbuff, int zstart, unsigned int zoffsets) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((4) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v8acc80 result;
  result = __builtin_aie_mul8_v32int32_v8int32_bm_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v8acc80)
lmul8_maxdiff(v32int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
              v8int32 zbuff, int zstart, unsigned int zoffsets) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((7) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v8acc80 result;
  result = __builtin_aie_mul8_v32int32_v8int32_bm_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v8acc80)
lnegmul8(v32int32 xbuff, int xstart, unsigned int xoffsets, v8int32 zbuff,
         int zstart, unsigned int zoffsets) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (((0) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v8acc80 result;
  result = __builtin_aie_mul8_v32int32_v8int32_bm_sw80(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v8acc80)
lnegmul8_abs(v32int32 xbuff, int xstart, unsigned int xoffsets, v8int32 zbuff,
             int zstart, unsigned int zoffsets) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((6) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v8acc80 result;
  result = __builtin_aie_mul8_v32int32_v8int32_bm_sw80(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v8acc80)
lnegmul8_sym(v32int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
             v8int32 zbuff, int zstart, unsigned int zoffsets) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (((0) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v8acc80 result;
  result = __builtin_aie_mul8_v32int32_v8int32_bm_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v8acc80)
lnegmul8_antisym(v32int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
                 v8int32 zbuff, int zstart, unsigned int zoffsets) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((0) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v8acc80 result;
  result = __builtin_aie_mul8_v32int32_v8int32_bm_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v8acc80)
lnegmul8_max(v32int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
             v8int32 zbuff, int zstart, unsigned int zoffsets) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((5) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v8acc80 result;
  result = __builtin_aie_mul8_v32int32_v8int32_bm_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v8acc80)
lnegmul8_min(v32int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
             v8int32 zbuff, int zstart, unsigned int zoffsets) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((4) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v8acc80 result;
  result = __builtin_aie_mul8_v32int32_v8int32_bm_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v8acc80)
lnegmul8_maxdiff(v32int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
                 v8int32 zbuff, int zstart, unsigned int zoffsets) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] =
      (1 << 15) | (1 << 16) | (((7) & 0x0F) << 23) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v8acc80 result;
  result = __builtin_aie_mul8_v32int32_v8int32_bm_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4(v32int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
      v8int32 zbuff, int zstart, unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (((0) & 0x0F) << 23) | (((zstep)&0x3F) << 8) |
            (((xstep)&0x3F) << 0) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_abs(v32int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
          v8int32 zbuff, int zstart, unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (((6) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_sym(v32int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
          int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
          int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (((0) & 0x0F) << 23) | (((zstep)&0x3F) << 8) |
            (((xstep)&0x3F) << 0) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_antisym(v32int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
              int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
              int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (((0) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_max(v32int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
          int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
          int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (((5) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_min(v32int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
          int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
          int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (((4) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_maxdiff(v32int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
              int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
              int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (((7) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_sym_ct(v32int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
             unsigned int mtap, v8int32 zbuff, int zstart,
             unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (((0) & 0x0F) << 23) | (((zstep)&0x3F) << 8) |
            ((0) << 18) | ((0) << 17) | (((mtap + 1) & 0x1F) << 27);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_antisym_ct(v32int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
                 unsigned int mtap, v8int32 zbuff, int zstart,
                 unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (((0) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | ((0) << 18) | ((0) << 17) |
            (((mtap + 1) & 0x1F) << 27);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4(v32int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
         v8int32 zbuff, int zstart, unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (((0) & 0x0F) << 23) | (((zstep)&0x3F) << 8) |
            (((xstep)&0x3F) << 0) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_abs(v32int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
             v8int32 zbuff, int zstart, unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (((6) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_sym(v32int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
             int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
             int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (((0) & 0x0F) << 23) | (((zstep)&0x3F) << 8) |
            (((xstep)&0x3F) << 0) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_antisym(v32int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
                 int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
                 int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (((0) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_max(v32int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
             int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
             int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (((5) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_min(v32int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
             int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
             int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (((4) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_maxdiff(v32int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
                 int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
                 int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (((7) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_sym_ct(v32int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
                unsigned int mtap, v8int32 zbuff, int zstart,
                unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (((0) & 0x0F) << 23) | (((zstep)&0x3F) << 8) |
            ((0) << 18) | ((0) << 17) | (((mtap + 1) & 0x1F) << 27);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_antisym_ct(v32int32 xbuff, int xstart, unsigned int xoffsets,
                    int ystart, unsigned int mtap, v8int32 zbuff, int zstart,
                    unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (((0) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | ((0) << 18) | ((0) << 17) |
            (((mtap + 1) & 0x1F) << 27);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v32int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4(v16int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
      v8int32 zbuff, int zstart, unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 20) | (((0) & 0x0F) << 23) | (((zstep)&0x3F) << 8) |
            (((xstep)&0x3F) << 0) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_abs(v16int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
          v8int32 zbuff, int zstart, unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (1 << 20) | (((6) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_sym(v16int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
          int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
          int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 20) | (((0) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_antisym(v16int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
              int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
              int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (1 << 20) | (((0) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_max(v16int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
          int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
          int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (1 << 20) | (((5) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_min(v16int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
          int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
          int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (1 << 20) | (((4) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_maxdiff(v16int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
              int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
              int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (1 << 20) | (((7) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_sym_ct(v16int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
             unsigned int mtap, v8int32 zbuff, int zstart,
             unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 20) | (((0) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | ((0) << 18) | ((0) << 17) |
            (((mtap + 1) & 0x1F) << 27);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lmul4_antisym_ct(v16int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
                 unsigned int mtap, v8int32 zbuff, int zstart,
                 unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (1 << 20) | (((0) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | ((0) << 18) | ((0) << 17) |
            (((mtap + 1) & 0x1F) << 27);
  conf[1] = 0 | (0 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4(v16int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
         v8int32 zbuff, int zstart, unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 20) | (((0) & 0x0F) << 23) | (((zstep)&0x3F) << 8) |
            (((xstep)&0x3F) << 0) | ((0) << 18) | ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_abs(v16int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
             v8int32 zbuff, int zstart, unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (1 << 20) | (((6) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_sym(v16int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
             int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
             int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 20) | (((0) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_antisym(v16int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
                 int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
                 int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (1 << 20) | (((0) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_max(v16int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
             int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
             int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (1 << 20) | (((5) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_min(v16int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
             int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
             int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (1 << 20) | (((4) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_maxdiff(v16int32 xbuff, int xstart, unsigned int xoffsets, int xstep,
                 int ystart, v8int32 zbuff, int zstart, unsigned int zoffsets,
                 int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (1 << 20) | (((7) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | (((xstep)&0x3F) << 0) | ((0) << 18) |
            ((0) << 17);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_sym_ct(v16int32 xbuff, int xstart, unsigned int xoffsets, int ystart,
                unsigned int mtap, v8int32 zbuff, int zstart,
                unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 20) | (((0) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | ((0) << 18) | ((0) << 17) |
            (((mtap + 1) & 0x1F) << 27);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v4acc80)
lnegmul4_antisym_ct(v16int32 xbuff, int xstart, unsigned int xoffsets,
                    int ystart, unsigned int mtap, v8int32 zbuff, int zstart,
                    unsigned int zoffsets, int zstep) {
  ;
  v2int32 xoffs = undef_v2int32();
  xoffs[0] = xoffsets;
  v2int32 zoffs = undef_v2int32();
  zoffs[0] = zoffsets;
  v2int32 conf = undef_v2int32();
  conf[0] = (1 << 15) | (1 << 16) | (1 << 20) | (((0) & 0x0F) << 23) |
            (((zstep)&0x3F) << 8) | ((0) << 18) | ((0) << 17) |
            (((mtap + 1) & 0x1F) << 27);
  conf[1] = 0 | (1 << 17);
  v4acc80 result;
  result = __builtin_aie_mul4_v16int32_v8int32_am_sw80(
      xbuff, zbuff, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
#endif /*__AIEV1MULT_H*/
