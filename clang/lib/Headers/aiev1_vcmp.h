//===- aiev1_vcmp.h ---------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV1_VCMP_H__
#define __AIEV1_VCMP_H__

namespace aiev1_compute_conf {
inline int compute_conf0(unsigned pramode, bool antisym) {
  return (0 | (1 << 14) | (1 << 15) | (1 << 20) | 0 |
          (antisym ? (1 << 16) : 0) | (((pramode)&0x0F) << 23));
}
inline int compute_conf1(unsigned int xsquare, unsigned int ysquare) {
  return (0 | (1 << 16) |
          (((xsquare)&0x0003) | (((xsquare)&0x0030) >> 2) |
           (((xsquare)&0x0300) >> 4) | (((xsquare)&0x3000) >> 6)) |
          ((((ysquare)&0x0003) << 21) | (((ysquare)&0x0030) << 19) |
           (((ysquare)&0x0300) << 17) | (((ysquare)&0x3000) << 15)));
}
} // namespace aiev1_compute_conf

inline v32int16 maxdiff32(v32int16 xbuff, int xstart, unsigned int xoffsets,
                          unsigned int xoffsets_hi, unsigned int xsquare,
                          int ystart, unsigned int yoffsets,
                          unsigned int yoffsets_hi, unsigned int ysquare) {
  v2int32 xo = undef_v2int32();
  xo[0] = xoffsets;
  xo[1] = xoffsets_hi;
  v2int32 yo = undef_v2int32();
  yo[0] = yoffsets;
  yo[1] = yoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = aiev1_compute_conf::compute_conf0(7, true);
  conf[1] = aiev1_compute_conf::compute_conf1(xsquare, ysquare);
  return __builtin_aie_prim_v32int16(xbuff, xstart, ystart, xo, yo, conf);
}
inline v32int16 max32(v32int16 xbuff, int xstart, unsigned int xoffsets,
                      unsigned int xoffsets_hi, unsigned int xsquare,
                      int ystart, unsigned int yoffsets,
                      unsigned int yoffsets_hi, unsigned int ysquare) {
  v2int32 xo = undef_v2int32();
  xo[0] = xoffsets;
  xo[1] = xoffsets_hi;
  v2int32 yo = undef_v2int32();
  yo[0] = yoffsets;
  yo[1] = yoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = aiev1_compute_conf::compute_conf0(5, true);
  conf[1] = aiev1_compute_conf::compute_conf1(xsquare, ysquare);
  return __builtin_aie_prim_v32int16(xbuff, xstart, ystart, xo, yo, conf);
}
inline v32int16 min32(v32int16 xbuff, int xstart, unsigned int xoffsets,
                      unsigned int xoffsets_hi, unsigned int xsquare,
                      int ystart, unsigned int yoffsets,
                      unsigned int yoffsets_hi, unsigned int ysquare) {
  v2int32 xo = undef_v2int32();
  xo[0] = xoffsets;
  xo[1] = xoffsets_hi;
  v2int32 yo = undef_v2int32();
  yo[0] = yoffsets;
  yo[1] = yoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = aiev1_compute_conf::compute_conf0(4, true);
  conf[1] = aiev1_compute_conf::compute_conf1(xsquare, ysquare);
  return __builtin_aie_prim_v32int16(xbuff, xstart, ystart, xo, yo, conf);
}
inline v32int16 maxdiffcmp32(v32int16 xbuff, int xstart, unsigned int xoffsets,
                             unsigned int xoffsets_hi, unsigned int xsquare,
                             int ystart, unsigned int yoffsets,
                             unsigned int yoffsets_hi, unsigned int ysquare,
                             unsigned int &cmp) {
  v2int32 xo = undef_v2int32();
  xo[0] = xoffsets;
  xo[1] = xoffsets_hi;
  v2int32 yo = undef_v2int32();
  yo[0] = yoffsets;
  yo[1] = yoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = aiev1_compute_conf::compute_conf0(7, true);
  conf[1] = aiev1_compute_conf::compute_conf1(xsquare, ysquare);
  return __builtin_aie_prim_v32int16_cmp(xbuff, xstart, ystart, xo, yo, conf,
                                         cmp);
}
inline v32int16 maxcmp32(v32int16 xbuff, int xstart, unsigned int xoffsets,
                         unsigned int xoffsets_hi, unsigned int xsquare,
                         int ystart, unsigned int yoffsets,
                         unsigned int yoffsets_hi, unsigned int ysquare,
                         unsigned int &cmp) {
  v2int32 xo = undef_v2int32();
  xo[0] = xoffsets;
  xo[1] = xoffsets_hi;
  v2int32 yo = undef_v2int32();
  yo[0] = yoffsets;
  yo[1] = yoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = aiev1_compute_conf::compute_conf0(5, true);
  conf[1] = aiev1_compute_conf::compute_conf1(xsquare, ysquare);
  return __builtin_aie_prim_v32int16_cmp(xbuff, xstart, ystart, xo, yo, conf,
                                         cmp);
}
inline v32int16 mincmp32(v32int16 xbuff, int xstart, unsigned int xoffsets,
                         unsigned int xoffsets_hi, unsigned int xsquare,
                         int ystart, unsigned int yoffsets,
                         unsigned int yoffsets_hi, unsigned int ysquare,
                         unsigned int &cmp) {
  v2int32 xo = undef_v2int32();
  xo[0] = xoffsets;
  xo[1] = xoffsets_hi;
  v2int32 yo = undef_v2int32();
  yo[0] = yoffsets;
  yo[1] = yoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = aiev1_compute_conf::compute_conf0(4, true);
  conf[1] = aiev1_compute_conf::compute_conf1(xsquare, ysquare);
  return __builtin_aie_prim_v32int16_cmp(xbuff, xstart, ystart, xo, yo, conf,
                                         cmp);
}
inline v32int16 add32(v32int16 xbuff, int xstart, unsigned int xoffsets,
                      unsigned int xoffsets_hi, unsigned int xsquare,
                      int ystart, unsigned int yoffsets,
                      unsigned int yoffsets_hi, unsigned int ysquare) {
  v2int32 xo = undef_v2int32();
  xo[0] = xoffsets;
  xo[1] = xoffsets_hi;
  v2int32 yo = undef_v2int32();
  yo[0] = yoffsets;
  yo[1] = yoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = aiev1_compute_conf::compute_conf0(0, false);
  conf[1] = aiev1_compute_conf::compute_conf1(xsquare, ysquare);
  return __builtin_aie_prim_v32int16(xbuff, xstart, ystart, xo, yo, conf);
}
inline v32int16 sub32(v32int16 xbuff, int xstart, unsigned int xoffsets,
                      unsigned int xoffsets_hi, unsigned int xsquare,
                      int ystart, unsigned int yoffsets,
                      unsigned int yoffsets_hi, unsigned int ysquare) {
  v2int32 xo = undef_v2int32();
  xo[0] = xoffsets;
  xo[1] = xoffsets_hi;
  v2int32 yo = undef_v2int32();
  yo[0] = yoffsets;
  yo[1] = yoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = aiev1_compute_conf::compute_conf0(0, true);
  conf[1] = aiev1_compute_conf::compute_conf1(xsquare, ysquare);
  return __builtin_aie_prim_v32int16(xbuff, xstart, ystart, xo, yo, conf);
}
inline v32int16 subcmp32(v32int16 xbuff, int xstart, unsigned int xoffsets,
                         unsigned int xoffsets_hi, unsigned int xsquare,
                         int ystart, unsigned int yoffsets,
                         unsigned int yoffsets_hi, unsigned int ysquare,
                         unsigned int &cmp) {
  v2int32 xo = undef_v2int32();
  xo[0] = xoffsets;
  xo[1] = xoffsets_hi;
  v2int32 yo = undef_v2int32();
  yo[0] = yoffsets;
  yo[1] = yoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = aiev1_compute_conf::compute_conf0(0, true);
  conf[1] = aiev1_compute_conf::compute_conf1(xsquare, ysquare);
  return __builtin_aie_prim_v32int16_cmp(xbuff, xstart, ystart, xo, yo, conf,
                                         cmp);
}
inline v32int16 select32(unsigned int select, v32int16 xbuff, int xstart,
                         unsigned int xoffsets, unsigned int xoffsets_hi,
                         unsigned int xsquare, int ystart,
                         unsigned int yoffsets, unsigned int yoffsets_hi,
                         unsigned int ysquare) {
  v2int32 xo = undef_v2int32();
  xo[0] = xoffsets;
  xo[1] = xoffsets_hi;
  v2int32 yo = undef_v2int32();
  yo[0] = yoffsets;
  yo[1] = yoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = aiev1_compute_conf::compute_conf0(3, true);
  conf[1] = aiev1_compute_conf::compute_conf1(xsquare, ysquare);
  return __builtin_aie_prim_v32int16_select(xbuff, select, xstart, ystart, xo,
                                            yo, conf);
}
inline v32int16 abs32(v32int16 xbuff, int xstart, unsigned int xoffsets,
                      unsigned int xoffsets_hi, unsigned int xsquare) {
  v2int32 xo = undef_v2int32();
  xo[0] = xoffsets;
  xo[1] = xoffsets_hi;
  v2int32 yo = undef_v2int32();
  yo[0] = xoffsets;
  yo[1] = xoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = aiev1_compute_conf::compute_conf0(6, true);
  conf[1] = aiev1_compute_conf::compute_conf1(xsquare, xsquare);
  return __builtin_aie_prim_v32int16(xbuff, xstart, xstart, xo, yo, conf);
}
inline v32int16 abscmp32(v32int16 xbuff, int xstart, unsigned int xoffsets,
                         unsigned int xoffsets_hi, unsigned int xsquare,
                         unsigned int &cmp) {
  v2int32 xo = undef_v2int32();
  xo[0] = xoffsets;
  xo[1] = xoffsets_hi;
  v2int32 yo = undef_v2int32();
  yo[0] = xoffsets;
  yo[1] = xoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = aiev1_compute_conf::compute_conf0(6, true);
  conf[1] = aiev1_compute_conf::compute_conf1(xsquare, xsquare);
  return __builtin_aie_prim_v32int16_cmp(xbuff, xstart, xstart, xo, yo, conf,
                                         cmp);
}
inline v32int16 shuffle32(v32int16 xbuff, int xstart, unsigned int xoffsets,
                          unsigned int xoffsets_hi, unsigned int xsquare) {
  v2int32 xo = undef_v2int32();
  xo[0] = xoffsets;
  xo[1] = xoffsets_hi;
  v2int32 yo = undef_v2int32();
  yo[0] = xoffsets;
  yo[1] = xoffsets_hi;
  v2int32 conf = undef_v2int32();
  conf[0] = aiev1_compute_conf::compute_conf0(3, true);
  conf[1] = aiev1_compute_conf::compute_conf1(xsquare, xsquare);
  unsigned int select = 0x00000000;
  return __builtin_aie_prim_v32int16_select(xbuff, select, xstart, xstart, xo,
                                            yo, conf);
}

#endif /*__AIEV1_VCMP_H__*/
