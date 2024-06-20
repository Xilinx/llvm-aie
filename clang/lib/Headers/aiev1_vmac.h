//===- aiev1_vmac.h ---------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

//* Automatically generated file, do not edit! *

#ifndef __AIEV1MAC_H
#define __AIEV1MAC_H

INTRINSIC(v16acc48)
mac16(v16acc48 acc, v32int32 xbuff, int xstart, unsigned int xoffsets,
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
  conf[1] = 0 | (0 << 17);
  v16acc48 result;
  result = __builtin_aie_mac16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, acc, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mac16_abs(v16acc48 acc, v32int32 xbuff, int xstart, unsigned int xoffsets,
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
  result = __builtin_aie_mac16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, acc, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mac16_sym(v16acc48 acc, v32int32 xbuff, int xstart, unsigned int xoffsets,
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
  result = __builtin_aie_mac16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mac16_antisym(v16acc48 acc, v32int32 xbuff, int xstart, unsigned int xoffsets,
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
  result = __builtin_aie_mac16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mac16_max(v16acc48 acc, v32int32 xbuff, int xstart, unsigned int xoffsets,
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
  result = __builtin_aie_mac16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mac16_min(v16acc48 acc, v32int32 xbuff, int xstart, unsigned int xoffsets,
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
  result = __builtin_aie_mac16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mac16_maxdiff(v16acc48 acc, v32int32 xbuff, int xstart, unsigned int xoffsets,
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
  result = __builtin_aie_mac16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
msc16(v16acc48 acc, v32int32 xbuff, int xstart, unsigned int xoffsets,
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
  result = __builtin_aie_mac16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, acc, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
msc16_abs(v16acc48 acc, v32int32 xbuff, int xstart, unsigned int xoffsets,
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
  result = __builtin_aie_mac16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, acc, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
msc16_sym(v16acc48 acc, v32int32 xbuff, int xstart, unsigned int xoffsets,
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
  result = __builtin_aie_mac16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
msc16_antisym(v16acc48 acc, v32int32 xbuff, int xstart, unsigned int xoffsets,
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
  conf[1] = 0 | (1 << 17);
  v16acc48 result;
  result = __builtin_aie_mac16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
msc16_max(v16acc48 acc, v32int32 xbuff, int xstart, unsigned int xoffsets,
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
  result = __builtin_aie_mac16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
msc16_min(v16acc48 acc, v32int32 xbuff, int xstart, unsigned int xoffsets,
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
  result = __builtin_aie_mac16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
msc16_maxdiff(v16acc48 acc, v32int32 xbuff, int xstart, unsigned int xoffsets,
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
  conf[1] = 0 | (1 << 17);
  v16acc48 result;
  result = __builtin_aie_mac16_v32int32_v16int16_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mac16(v16acc48 acc, v64int16 xbuff, int xstart, unsigned int xoffsets,
      unsigned int xoffsets_hi, v8int32 zbuff, int zstart,
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
  conf[1] = 0 | (0 << 17);
  v16acc48 result;
  result = __builtin_aie_mac16_v64int16_v8int32_bm_sw48(
      xbuff, zbuff, acc, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mac16_abs(v16acc48 acc, v64int16 xbuff, int xstart, unsigned int xoffsets,
          unsigned int xoffsets_hi, v8int32 zbuff, int zstart,
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
  result = __builtin_aie_mac16_v64int16_v8int32_bm_sw48(
      xbuff, zbuff, acc, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mac16_sym(v16acc48 acc, v64int16 xbuff, int xstart, unsigned int xoffsets,
          unsigned int xoffsets_hi, int ystart, v8int32 zbuff, int zstart,
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
  result = __builtin_aie_mac16_v64int16_v8int32_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mac16_antisym(v16acc48 acc, v64int16 xbuff, int xstart, unsigned int xoffsets,
              unsigned int xoffsets_hi, int ystart, v8int32 zbuff, int zstart,
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
  result = __builtin_aie_mac16_v64int16_v8int32_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mac16_max(v16acc48 acc, v64int16 xbuff, int xstart, unsigned int xoffsets,
          unsigned int xoffsets_hi, int ystart, v8int32 zbuff, int zstart,
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
  result = __builtin_aie_mac16_v64int16_v8int32_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mac16_min(v16acc48 acc, v64int16 xbuff, int xstart, unsigned int xoffsets,
          unsigned int xoffsets_hi, int ystart, v8int32 zbuff, int zstart,
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
  result = __builtin_aie_mac16_v64int16_v8int32_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mac16_maxdiff(v16acc48 acc, v64int16 xbuff, int xstart, unsigned int xoffsets,
              unsigned int xoffsets_hi, int ystart, v8int32 zbuff, int zstart,
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
  result = __builtin_aie_mac16_v64int16_v8int32_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
msc16(v16acc48 acc, v64int16 xbuff, int xstart, unsigned int xoffsets,
      unsigned int xoffsets_hi, v8int32 zbuff, int zstart,
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
  result = __builtin_aie_mac16_v64int16_v8int32_bm_sw48(
      xbuff, zbuff, acc, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
msc16_abs(v16acc48 acc, v64int16 xbuff, int xstart, unsigned int xoffsets,
          unsigned int xoffsets_hi, v8int32 zbuff, int zstart,
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
  result = __builtin_aie_mac16_v64int16_v8int32_bm_sw48(
      xbuff, zbuff, acc, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
msc16_sym(v16acc48 acc, v64int16 xbuff, int xstart, unsigned int xoffsets,
          unsigned int xoffsets_hi, int ystart, v8int32 zbuff, int zstart,
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
  result = __builtin_aie_mac16_v64int16_v8int32_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
msc16_antisym(v16acc48 acc, v64int16 xbuff, int xstart, unsigned int xoffsets,
              unsigned int xoffsets_hi, int ystart, v8int32 zbuff, int zstart,
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
  conf[1] = 0 | (1 << 17);
  v16acc48 result;
  result = __builtin_aie_mac16_v64int16_v8int32_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
msc16_max(v16acc48 acc, v64int16 xbuff, int xstart, unsigned int xoffsets,
          unsigned int xoffsets_hi, int ystart, v8int32 zbuff, int zstart,
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
  result = __builtin_aie_mac16_v64int16_v8int32_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
msc16_min(v16acc48 acc, v64int16 xbuff, int xstart, unsigned int xoffsets,
          unsigned int xoffsets_hi, int ystart, v8int32 zbuff, int zstart,
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
  result = __builtin_aie_mac16_v64int16_v8int32_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
msc16_maxdiff(v16acc48 acc, v64int16 xbuff, int xstart, unsigned int xoffsets,
              unsigned int xoffsets_hi, int ystart, v8int32 zbuff, int zstart,
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
  conf[1] = 0 | (1 << 17);
  v16acc48 result;
  result = __builtin_aie_mac16_v64int16_v8int32_bm_sw48(
      xbuff, zbuff, acc, xstart, ystart, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
mac16(v16acc48 acc, v128int8 xbuff, int xstart, unsigned int xoffsets,
      int xstep, unsigned int xsquare, v32int8 zbuff, int zstart,
      unsigned int zoffsets, int zstep, unsigned int zsquare) {
  ;
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
  v16acc48 result;
  result = __builtin_aie_mac16_v128int8_v32int8_bm_sw48(
      xbuff, zbuff, acc, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};
INTRINSIC(v16acc48)
msc16(v16acc48 acc, v128int8 xbuff, int xstart, unsigned int xoffsets,
      int xstep, unsigned int xsquare, v32int8 zbuff, int zstart,
      unsigned int zoffsets, int zstep, unsigned int zsquare) {
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
  result = __builtin_aie_mac16_v128int8_v32int8_bm_sw48(
      xbuff, zbuff, acc, xstart, 0, zstart, xoffs, zoffs, conf);
  return result;
};

#endif /*__AIEV1MAC_H*/
