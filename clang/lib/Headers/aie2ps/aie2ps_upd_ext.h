//===- aie2ps_upd_ext.h ------------------------------------------*- C++
//-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2PS_UPD_EXT_H__
#define __AIE2PS_UPD_EXT_H__

// Small vector datatypes
inline unsigned int set_w32(int idx, unsigned int val, unsigned int elems,
                            int step, unsigned int elem_mask) {
  idx = idx & (elems - 1);
  return ((val & elem_mask) << (idx * step));
}

inline mask64 set_w64(int idx, unsigned int val, unsigned int elems, int step,
                      unsigned int elem_mask) {
  idx = idx & (elems - 1);
  return (v2uint32)(((unsigned long long)(val & elem_mask)) << (idx * step));
}

inline unsigned int upd_w32(unsigned int a, int idx, unsigned int val,
                            unsigned int elems, int step,
                            unsigned int elem_mask) {
  idx = idx & (elems - 1);
  unsigned int mask = unsigned(elem_mask) << (idx * step);
  return (a & ~mask) | ((val & elem_mask) << (idx * step));
}

inline mask64 upd_w64(mask64 a_, int idx, unsigned int val, unsigned int elems,
                      int step, unsigned int elem_mask) {
  unsigned long long a = (unsigned long long)(a_);
  idx = idx & (elems - 1);
  unsigned long long mask = ((unsigned long long)(elem_mask)) << (idx * step);
  return (v2uint32)((a & ~mask) |
                    (((unsigned long long)(val & elem_mask)) << (idx * step)));
}

inline int ext_w32(int a, int idx, unsigned int elems, int step,
                   unsigned int elem_mask) {
  idx = idx & (elems - 1);
  return ((a << (32 - (idx + 1) * step)) >> (32 - step));
}

inline int ext_w64(mask64 a_, int idx, unsigned int elems, int step,
                   unsigned int elem_mask) {
  unsigned long long a = (unsigned long long)(a_);
  idx = idx & (elems - 1);
  return ((v2int32)((a >> (idx * step)) & elem_mask))[0];
}

inline unsigned int ext_u32(unsigned int a, int idx, unsigned int elems,
                            int step, unsigned int elem_mask) {
  idx = idx & (elems - 1);
  return (a >> (idx * step)) & elem_mask;
}

inline mask64 ext_u64(mask64 a_, int idx, unsigned int elems, int step,
                      unsigned int elem_mask) {
  unsigned long long a = (unsigned long long)(a_);
  idx = idx & (elems - 1);
  return (v2uint32)((a >> (idx * step)) & elem_mask);
}

inline unsigned int set_v2w4(int idx, unsigned int val) {
  return set_w32(idx, val, 2, 4, 0xf);
}
inline unsigned int set_v4w4(int idx, unsigned int val) {
  return set_w32(idx, val, 4, 4, 0xf);
}
inline unsigned int set_v8w4(int idx, unsigned int val) {
  return set_w32(idx, val, 8, 4, 0xf);
}
inline mask64 set_v16w4(int idx, unsigned int val) {
  return set_w64(idx, val, 16, 4, 0xf);
}

inline unsigned int set_v2w8(int idx, unsigned int val) {
  return set_w32(idx, val, 2, 8, 0xff);
}
inline unsigned int set_v4w8(int idx, unsigned int val) {
  return set_w32(idx, val, 4, 8, 0xff);
}
inline mask64 set_v8w8(int idx, unsigned int val) {
  return set_w64(idx, val, 8, 8, 0xff);
}

inline unsigned int set_v2w16(int idx, unsigned int val) {
  return set_w32(idx, val, 2, 16, 0xffff);
}
inline mask64 set_v4w16(int idx, unsigned int val) {
  return set_w64(idx, val, 4, 16, 0xffff);
}

inline mask64 set_v2w32(int idx, unsigned int val) {
  return set_w64(idx, val, 2, 32, 0xffffffff);
}

inline unsigned int upd_v2w4(unsigned int a, int idx, unsigned int val) {
  return upd_w32(a, idx, val, 2, 4, 0xf);
}
inline unsigned int upd_v4w4(unsigned int a, int idx, unsigned int val) {
  return upd_w32(a, idx, val, 4, 4, 0xf);
}
inline unsigned int upd_v8w4(unsigned int a, int idx, unsigned int val) {
  return upd_w32(a, idx, val, 8, 4, 0xf);
}
inline mask64 upd_v16w4(mask64 a, int idx, unsigned int val) {
  return upd_w64(a, idx, val, 16, 4, 0xf);
}

inline unsigned int upd_v2w8(unsigned int a, int idx, unsigned int val) {
  return upd_w32(a, idx, val, 2, 8, 0xff);
}
inline unsigned int upd_v4w8(unsigned int a, int idx, unsigned int val) {
  return upd_w32(a, idx, val, 4, 8, 0xff);
}
inline mask64 upd_v8w8(mask64 a, int idx, unsigned int val) {
  return upd_w64(a, idx, val, 8, 8, 0xff);
}

inline unsigned int upd_v2w16(unsigned int a, int idx, unsigned int val) {
  return upd_w32(a, idx, val, 2, 16, 0xffff);
}
inline mask64 upd_v4w16(mask64 a, int idx, unsigned int val) {
  return upd_w64(a, idx, val, 4, 16, 0xffff);
}

inline mask64 upd_v2w32(mask64 a, int idx, unsigned int val) {
  return upd_w64(a, idx, val, 2, 32, 0xffffffff);
}

inline int ext_v2w4(unsigned int a, int idx) {
  return ext_w32(a, idx, 2, 4, 0xf);
}
inline int ext_v4w4(unsigned int a, int idx) {
  return ext_w32(a, idx, 4, 4, 0xf);
}
inline int ext_v8w4(unsigned int a, int idx) {
  return ext_w32(a, idx, 8, 4, 0xf);
}
inline int ext_v16w4(mask64 a, int idx) { return ext_w64(a, idx, 16, 4, 0xf); }

inline int ext_v2w8(unsigned int a, int idx) {
  return ext_w32(a, idx, 2, 8, 0xff);
}
inline int ext_v4w8(unsigned int a, int idx) {
  return ext_w32(a, idx, 4, 8, 0xff);
}
inline int ext_v8w8(mask64 a, int idx) { return ext_w64(a, idx, 8, 8, 0xff); }

inline int ext_v2w16(unsigned int a, int idx) {
  return ext_w32(a, idx, 2, 16, 0xffff);
}
inline int ext_v4w16(mask64 a, int idx) {
  return ext_w64(a, idx, 4, 16, 0xffff);
}

inline int ext_v2w32(mask64 a, int idx) {
  return ext_w64(a, idx, 2, 32, 0xffffffff);
}

inline unsigned int ext_v2u4(unsigned int a, int idx) {
  return ext_u32(a, idx, 2, 4, 0xf);
}
inline unsigned int ext_v4u4(unsigned int a, int idx) {
  return ext_u32(a, idx, 4, 4, 0xf);
}
inline unsigned int ext_v8u4(unsigned int a, int idx) {
  return ext_u32(a, idx, 8, 4, 0xf);
}
inline unsigned int ext_v16u4(mask64 a, int idx) {
  return (ext_u64(a, idx, 16, 4, 0xf))[0];
}

inline unsigned int ext_v2u8(unsigned int a, int idx) {
  return ext_u32(a, idx, 2, 8, 0xff);
}
inline unsigned int ext_v4u8(unsigned int a, int idx) {
  return ext_u32(a, idx, 4, 8, 0xff);
}
inline unsigned int ext_v8u8(mask64 a, int idx) {
  return (ext_u64(a, idx, 8, 8, 0xff))[0];
}

inline unsigned int ext_v2u16(unsigned int a, int idx) {
  return ext_u32(a, idx, 2, 16, 0xffff);
}
inline unsigned int ext_v4u16(mask64 a, int idx) {
  return (ext_u64(a, idx, 4, 16, 0xffff))[0];
}

inline unsigned int ext_v2u32(mask64 a, int idx) {
  return (ext_u64(a, idx, 2, 32, 0xffffffff))[0];
}

INTRINSIC(v2int4) set_v2int4(int idx, int val) { return set_v2w4(idx, val); }
INTRINSIC(v4int4) set_v4int4(int idx, int val) {
  return (v4int4)(short)set_v4w4(idx, val);
}
INTRINSIC(v8int4) set_v8int4(int idx, int val) {
  return (v8int4)set_v8w4(idx, val);
}
INTRINSIC(v16int4) set_v16int4(int idx, int val) { return set_v16w4(idx, val); }

INTRINSIC(v2int8) set_v2int8(int idx, int val) {
  return (v2int8)(short)set_v2w8(idx, val);
}
INTRINSIC(v4int8) set_v4int8(int idx, int val) {
  return (v4int8)set_v4w8(idx, val);
}
INTRINSIC(v8int8) set_v8int8(int idx, int val) { return set_v8w8(idx, val); }

INTRINSIC(v2int16) set_v2int16(int idx, int val) {
  return (v2int16)set_v2w16(idx, val);
}
INTRINSIC(v4int16) set_v4int16(int idx, int val) { return set_v4w16(idx, val); }

INTRINSIC(v2int32) set_v2int32(int idx, int val) { return set_v2w32(idx, val); }

INTRINSIC(v2uint4) set_v2uint4(int idx, unsigned int val) {
  return set_v2w4(idx, val);
}
INTRINSIC(v4uint4) set_v4uint4(int idx, unsigned int val) {
  return (v4uint4)(short)set_v4w4(idx, val);
}
INTRINSIC(v8uint4) set_v8uint4(int idx, unsigned int val) {
  return (v8uint4)set_v8w4(idx, val);
}
INTRINSIC(v16uint4) set_v16uint4(int idx, unsigned int val) {
  return set_v16w4(idx, val);
}

INTRINSIC(v2uint8) set_v2uint8(int idx, unsigned int val) {
  return (v2uint8)(short)set_v2w8(idx, val);
}
INTRINSIC(v4uint8) set_v4uint8(int idx, unsigned int val) {
  return (v4uint8)set_v4w8(idx, val);
}
INTRINSIC(v8uint8) set_v8uint8(int idx, unsigned int val) {
  return set_v8w8(idx, val);
}

INTRINSIC(v2uint16) set_v2uint16(int idx, unsigned int val) {
  return (v2uint16)set_v2w16(idx, val);
}
INTRINSIC(v4uint16) set_v4uint16(int idx, unsigned int val) {
  return set_v4w16(idx, val);
}

INTRINSIC(v2uint32) set_v2uint32(int idx, unsigned int val) {
  return set_v2w32(idx, val);
}

INTRINSIC(v2float) set_v2float(int idx, float val) {
  return set_v2w32(idx, val);
}

INTRINSIC(v2int4) insert(v2int4 v, int idx, int val) {
  return upd_v2w4(v, idx, val);
}
INTRINSIC(v4int4) insert(v4int4 v, int idx, int val) {
  return (v4int4)(short)upd_v4w4((short)v, idx, val);
}
INTRINSIC(v8int4) insert(v8int4 v, int idx, int val) {
  return (v8int4)upd_v8w4((int)v, idx, val);
}
INTRINSIC(v16int4) insert(v16int4 v, int idx, int val) {
  return upd_v16w4(v, idx, val);
}

INTRINSIC(v2int8) insert(v2int8 v, int idx, int val) {
  return (v2int8)(unsigned short)upd_v2w8((unsigned short)v, idx, val);
}
INTRINSIC(v4int8) insert(v4int8 v, int idx, int val) {
  return (v4int8)upd_v4w8((int)v, idx, val);
}
INTRINSIC(v8int8) insert(v8int8 v, int idx, int val) {
  return upd_v8w8(v, idx, val);
}

INTRINSIC(v2int16) insert(v2int16 v, int idx, int val) {
  return (v2int16)upd_v2w16((int)v, idx, val);
}
INTRINSIC(v4int16) insert(v4int16 v, int idx, int val) {
  return upd_v4w16(v, idx, val);
}

INTRINSIC(v2int32) insert(v2int32 v, int idx, int val) {
  return upd_v2w32(v, idx, val);
}
INTRINSIC(v2float) insert(v2float v, int idx, float val) {
  return upd_v2w32(v, idx, val);
}

INTRINSIC(v2uint4) insert(v2uint4 v, int idx, unsigned int val) {
  return upd_v2w4(v, idx, val);
}
INTRINSIC(v4uint4) insert(v4uint4 v, int idx, unsigned int val) {
  return (v4uint4)(short)upd_v4w4((unsigned short)v, idx, val);
}
INTRINSIC(v8uint4) insert(v8uint4 v, int idx, unsigned int val) {
  return (v8uint4)upd_v8w4((unsigned int)v, idx, val);
}
INTRINSIC(v16uint4) insert(v16uint4 v, int idx, unsigned int val) {
  return upd_v16w4(v, idx, val);
}

INTRINSIC(v2uint8) insert(v2uint8 v, int idx, unsigned int val) {
  return (v2uint8)(unsigned short)upd_v2w8((unsigned short)v, idx, val);
}
INTRINSIC(v4uint8) insert(v4uint8 v, int idx, unsigned int val) {
  return (v4uint8)upd_v4w8((unsigned int)v, idx, val);
}
INTRINSIC(v8uint8) insert(v8uint8 v, int idx, unsigned int val) {
  return upd_v8w8(v, idx, val);
}

INTRINSIC(v2uint16) insert(v2uint16 v, int idx, unsigned int val) {
  return (v2uint16)upd_v2w16((unsigned int)v, idx, val);
}
INTRINSIC(v4uint16) insert(v4uint16 v, int idx, unsigned int val) {
  return upd_v4w16(v, idx, val);
}

INTRINSIC(v2uint32) insert(v2uint32 v, int idx, unsigned int val) {
  return upd_v2w32(v, idx, val);
}

INTRINSIC(int) extract_elem(v2int4 v, int idx) { return ext_v2w4(v, idx); }
INTRINSIC(int) extract_elem(v4int4 v, int idx) {
  return ext_v4w4((short)v, idx);
}
INTRINSIC(int) extract_elem(v8int4 v, int idx) { return ext_v8w4((int)v, idx); }
INTRINSIC(int) extract_elem(v16int4 v, int idx) { return ext_v16w4(v, idx); }

INTRINSIC(int) extract_elem(v2int8 v, int idx) {
  return ext_v2w8((short)v, idx);
}
INTRINSIC(int) extract_elem(v4int8 v, int idx) { return ext_v4w8((int)v, idx); }
INTRINSIC(int) extract_elem(v8int8 v, int idx) { return ext_v8w8(v, idx); }

INTRINSIC(int) extract_elem(v2int16 v, int idx) {
  return ext_v2w16((int)v, idx);
}
INTRINSIC(int) extract_elem(v4int16 v, int idx) { return ext_v4w16(v, idx); }

INTRINSIC(int) extract_elem(v2int32 v, int idx) { return ext_v2w32(v, idx); }

INTRINSIC(float) extract_elem(v2float v, int idx) { return ext_v2w32(v, idx); }

INTRINSIC(unsigned int) extract_elem(v2uint4 v, int idx) {
  return ext_v2u4(v, idx);
}
INTRINSIC(unsigned int) extract_elem(v4uint4 v, int idx) {
  return ext_v4u4((unsigned short)v, idx);
}
INTRINSIC(unsigned int) extract_elem(v8uint4 v, int idx) {
  return ext_v8u4((unsigned int)v, idx);
}
INTRINSIC(unsigned int) extract_elem(v16uint4 v, int idx) {
  return ext_v16u4(v, idx);
}

INTRINSIC(unsigned int) extract_elem(v2uint8 v, int idx) {
  return ext_v2u8((unsigned short)v, idx);
}
INTRINSIC(unsigned int) extract_elem(v4uint8 v, int idx) {
  return ext_v4u8((unsigned int)v, idx);
}
INTRINSIC(unsigned int) extract_elem(v8uint8 v, int idx) {
  return ext_v8u8(v, idx);
}

INTRINSIC(unsigned int) extract_elem(v2uint16 v, int idx) {
  return ext_v2u16((unsigned int)v, idx);
}
INTRINSIC(unsigned int) extract_elem(v4uint16 v, int idx) {
  return ext_v4u16(v, idx);
}

INTRINSIC(unsigned int) extract_elem(v2uint32 v, int idx) {
  return ext_v2u32(v, idx);
}

/** @defgroup intr_scalarop_updext Scalar updates and extracts
  @ingroup intr_scalarop

*/

// Macro-free primitives shared with aie2p (u64 dispatchers + generic
// 256/512/1024-bit shuffle helpers). The type wrappers below depend on
// these.
#include "../aie_upd_ext_primitives.h"

// Conversions

// v128uint4

INTRINSIC(v64uint4) extract_v64uint4(v128uint4 a, int idx) {
  return extract_256_512(a, idx);
}

INTRINSIC(v128uint4) insert(v128uint4 a, int idx, v64uint4 b) {
  return insert_256_512(a, idx, b);
}

INTRINSIC(v128uint4) set_v128uint4(int idx, v64uint4 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v128uint4) concat(v64uint4 a0, v64uint4 a1) {
  return concat_256_512(a0, a1);
}

// v128int4

INTRINSIC(v64int4) extract_v64int4(v128int4 a, int idx) {
  return extract_256_512(a, idx);
}

INTRINSIC(v128int4) insert(v128int4 a, int idx, v64int4 b) {
  return insert_256_512(a, idx, b);
}

INTRINSIC(v128int4) set_v128int4(int idx, v64int4 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v128int4) concat(v64int4 a0, v64int4 a1) {
  return concat_256_512(a0, a1);
}

// v64uint8

INTRINSIC(v32uint8) extract_v32uint8(v64uint8 a, int idx) {
  return extract_256_512(a, idx);
}

INTRINSIC(v64uint8) insert(v64uint8 a, int idx, v32uint8 b) {
  return insert_256_512(a, idx, b);
}

INTRINSIC(v64uint8) set_v64uint8(int idx, v32uint8 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v64uint8) concat(v32uint8 a0, v32uint8 a1) {
  return concat_256_512(a0, a1);
}

// v64int8

INTRINSIC(v32int8) extract_v32int8(v64int8 a, int idx) {
  return extract_256_512(a, idx);
}

INTRINSIC(v64int8) insert(v64int8 a, int idx, v32int8 b) {
  return insert_256_512(a, idx, b);
}

INTRINSIC(v64int8) set_v64int8(int idx, v32int8 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v64int8) concat(v32int8 a0, v32int8 a1) {
  return concat_256_512(a0, a1);
}

// v32uint16

INTRINSIC(v16uint16) extract_v16uint16(v32uint16 a, int idx) {
  return extract_256_512(a, idx);
}

INTRINSIC(v32uint16) insert(v32uint16 a, int idx, v16uint16 b) {
  return insert_256_512(a, idx, b);
}

INTRINSIC(v32uint16) set_v32uint16(int idx, v16uint16 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v32uint16) concat(v16uint16 a0, v16uint16 a1) {
  return concat_256_512(a0, a1);
}

// v32int16

INTRINSIC(v16int16) extract_v16int16(v32int16 a, int idx) {
  return extract_256_512(a, idx);
}

INTRINSIC(v32int16) insert(v32int16 a, int idx, v16int16 b) {
  return insert_256_512(a, idx, b);
}

INTRINSIC(v32int16) set_v32int16(int idx, v16int16 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v32int16) concat(v16int16 a0, v16int16 a1) {
  return concat_256_512(a0, a1);
}

// v16uint32

INTRINSIC(v8uint32) extract_v8uint32(v16uint32 a, int idx) {
  return extract_256_512(a, idx);
}

INTRINSIC(v16uint32) insert(v16uint32 a, int idx, v8uint32 b) {
  return insert_256_512(a, idx, b);
}

INTRINSIC(v16uint32) set_v16uint32(int idx, v8uint32 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v16uint32) concat(v8uint32 a0, v8uint32 a1) {
  return concat_256_512(a0, a1);
}

// v16int32

INTRINSIC(v8int32) extract_v8int32(v16int32 a, int idx) {
  return extract_256_512(a, idx);
}

INTRINSIC(v16int32) insert(v16int32 a, int idx, v8int32 b) {
  return insert_256_512(a, idx, b);
}

INTRINSIC(v16int32) set_v16int32(int idx, v8int32 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v16int32) concat(v8int32 a0, v8int32 a1) {
  return concat_256_512(a0, a1);
}

// v32bfloat16

INTRINSIC(v16bfloat16) extract_v16bfloat16(v32bfloat16 a, int idx) {
  return extract_256_512(a, idx);
}

INTRINSIC(v32bfloat16) insert(v32bfloat16 a, int idx, v16bfloat16 b) {
  return insert_256_512(a, idx, b);
}

INTRINSIC(v32bfloat16) set_v32bfloat16(int idx, v16bfloat16 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v32bfloat16) concat(v16bfloat16 a0, v16bfloat16 a1) {
  return concat_256_512(a0, a1);
}

// v16float

INTRINSIC(v8float) extract_v8float(v16float a, int idx) {
  return extract_256_512(a, idx);
}

INTRINSIC(v16float) insert(v16float a, int idx, v8float b) {
  return insert_256_512(a, idx, b);
}

INTRINSIC(v16float) set_v16float(int idx, v8float b) {
  return set_256_512(idx, b);
}

INTRINSIC(v16float) concat(v8float a0, v8float a1) {
  return concat_256_512(a0, a1);
}

// v256uint4

INTRINSIC(v64uint4) extract_v64uint4(v256uint4 a, int idx) {
  return extract_256_1024(a, idx);
}

INTRINSIC(v256uint4) insert(v256uint4 a, int idx, v64uint4 b) {
  return insert_256_1024(a, idx, b);
}

INTRINSIC(v256uint4) set_v256uint4(int idx, v64uint4 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v256uint4)
concat(v64uint4 a0, v64uint4 a1, v64uint4 a2, v64uint4 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v256uint4

INTRINSIC(v128uint4) extract_v128uint4(v256uint4 a, int idx) {
  return extract_512_1024(a, idx);
}

INTRINSIC(v256uint4) insert(v256uint4 a, int idx, v128uint4 b) {
  return insert_512_1024(a, idx, b);
}

INTRINSIC(v256uint4) set_v256uint4(int idx, v128uint4 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v256uint4) concat(v128uint4 a0, v128uint4 a1) {
  return concat_512_1024(a0, a1);
}

// v256int4

INTRINSIC(v64int4) extract_v64int4(v256int4 a, int idx) {
  return extract_256_1024(a, idx);
}

INTRINSIC(v256int4) insert(v256int4 a, int idx, v64int4 b) {
  return insert_256_1024(a, idx, b);
}

INTRINSIC(v256int4) set_v256int4(int idx, v64int4 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v256int4) concat(v64int4 a0, v64int4 a1, v64int4 a2, v64int4 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v256int4

INTRINSIC(v128int4) extract_v128int4(v256int4 a, int idx) {
  return extract_512_1024(a, idx);
}

INTRINSIC(v256int4) insert(v256int4 a, int idx, v128int4 b) {
  return insert_512_1024(a, idx, b);
}

INTRINSIC(v256int4) set_v256int4(int idx, v128int4 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v256int4) concat(v128int4 a0, v128int4 a1) {
  return concat_512_1024(a0, a1);
}

// v128uint8

INTRINSIC(v32uint8) extract_v32uint8(v128uint8 a, int idx) {
  return extract_256_1024(a, idx);
}

INTRINSIC(v128uint8) insert(v128uint8 a, int idx, v32uint8 b) {
  return insert_256_1024(a, idx, b);
}

INTRINSIC(v128uint8) set_v128uint8(int idx, v32uint8 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v128uint8)
concat(v32uint8 a0, v32uint8 a1, v32uint8 a2, v32uint8 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v128uint8

INTRINSIC(v64uint8) extract_v64uint8(v128uint8 a, int idx) {
  return extract_512_1024(a, idx);
}

INTRINSIC(v128uint8) insert(v128uint8 a, int idx, v64uint8 b) {
  return insert_512_1024(a, idx, b);
}

INTRINSIC(v128uint8) set_v128uint8(int idx, v64uint8 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v128uint8) concat(v64uint8 a0, v64uint8 a1) {
  return concat_512_1024(a0, a1);
}

// v128int8

INTRINSIC(v32int8) extract_v32int8(v128int8 a, int idx) {
  return extract_256_1024(a, idx);
}

INTRINSIC(v128int8) insert(v128int8 a, int idx, v32int8 b) {
  return insert_256_1024(a, idx, b);
}

INTRINSIC(v128int8) set_v128int8(int idx, v32int8 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v128int8) concat(v32int8 a0, v32int8 a1, v32int8 a2, v32int8 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v128int8

INTRINSIC(v64int8) extract_v64int8(v128int8 a, int idx) {
  return extract_512_1024(a, idx);
}

INTRINSIC(v128int8) insert(v128int8 a, int idx, v64int8 b) {
  return insert_512_1024(a, idx, b);
}

INTRINSIC(v128int8) set_v128int8(int idx, v64int8 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v128int8) concat(v64int8 a0, v64int8 a1) {
  return concat_512_1024(a0, a1);
}

// v64uint16

INTRINSIC(v16uint16) extract_v16uint16(v64uint16 a, int idx) {
  return extract_256_1024(a, idx);
}

INTRINSIC(v64uint16) insert(v64uint16 a, int idx, v16uint16 b) {
  return insert_256_1024(a, idx, b);
}

INTRINSIC(v64uint16) set_v64uint16(int idx, v16uint16 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v64uint16)
concat(v16uint16 a0, v16uint16 a1, v16uint16 a2, v16uint16 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v64uint16

INTRINSIC(v32uint16) extract_v32uint16(v64uint16 a, int idx) {
  return extract_512_1024(a, idx);
}

INTRINSIC(v64uint16) insert(v64uint16 a, int idx, v32uint16 b) {
  return insert_512_1024(a, idx, b);
}

INTRINSIC(v64uint16) set_v64uint16(int idx, v32uint16 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v64uint16) concat(v32uint16 a0, v32uint16 a1) {
  return concat_512_1024(a0, a1);
}

// v64int16

INTRINSIC(v16int16) extract_v16int16(v64int16 a, int idx) {
  return extract_256_1024(a, idx);
}

INTRINSIC(v64int16) insert(v64int16 a, int idx, v16int16 b) {
  return insert_256_1024(a, idx, b);
}

INTRINSIC(v64int16) set_v64int16(int idx, v16int16 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v64int16) concat(v16int16 a0, v16int16 a1, v16int16 a2, v16int16 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v64int16

INTRINSIC(v32int16) extract_v32int16(v64int16 a, int idx) {
  return extract_512_1024(a, idx);
}

INTRINSIC(v64int16) insert(v64int16 a, int idx, v32int16 b) {
  return insert_512_1024(a, idx, b);
}

INTRINSIC(v64int16) set_v64int16(int idx, v32int16 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v64int16) concat(v32int16 a0, v32int16 a1) {
  return concat_512_1024(a0, a1);
}

// v32uint32

INTRINSIC(v8uint32) extract_v8uint32(v32uint32 a, int idx) {
  return extract_256_1024(a, idx);
}

INTRINSIC(v32uint32) insert(v32uint32 a, int idx, v8uint32 b) {
  return insert_256_1024(a, idx, b);
}

INTRINSIC(v32uint32) set_v32uint32(int idx, v8uint32 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v32uint32)
concat(v8uint32 a0, v8uint32 a1, v8uint32 a2, v8uint32 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v32uint32

INTRINSIC(v16uint32) extract_v16uint32(v32uint32 a, int idx) {
  return extract_512_1024(a, idx);
}

INTRINSIC(v32uint32) insert(v32uint32 a, int idx, v16uint32 b) {
  return insert_512_1024(a, idx, b);
}

INTRINSIC(v32uint32) set_v32uint32(int idx, v16uint32 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v32uint32) concat(v16uint32 a0, v16uint32 a1) {
  return concat_512_1024(a0, a1);
}

// v32int32

INTRINSIC(v8int32) extract_v8int32(v32int32 a, int idx) {
  return extract_256_1024(a, idx);
}

INTRINSIC(v32int32) insert(v32int32 a, int idx, v8int32 b) {
  return insert_256_1024(a, idx, b);
}

INTRINSIC(v32int32) set_v32int32(int idx, v8int32 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v32int32) concat(v8int32 a0, v8int32 a1, v8int32 a2, v8int32 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v32int32

INTRINSIC(v16int32) extract_v16int32(v32int32 a, int idx) {
  return extract_512_1024(a, idx);
}

INTRINSIC(v32int32) insert(v32int32 a, int idx, v16int32 b) {
  return insert_512_1024(a, idx, b);
}

INTRINSIC(v32int32) set_v32int32(int idx, v16int32 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v32int32) concat(v16int32 a0, v16int32 a1) {
  return concat_512_1024(a0, a1);
}

// v64bfloat16

INTRINSIC(v16bfloat16) extract_v16bfloat16(v64bfloat16 a, int idx) {
  return extract_256_1024(a, idx);
}

INTRINSIC(v64bfloat16) insert(v64bfloat16 a, int idx, v16bfloat16 b) {
  return insert_256_1024(a, idx, b);
}

INTRINSIC(v64bfloat16) set_v64bfloat16(int idx, v16bfloat16 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v64bfloat16)
concat(v16bfloat16 a0, v16bfloat16 a1, v16bfloat16 a2, v16bfloat16 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v64bfloat16

INTRINSIC(v32bfloat16) extract_v32bfloat16(v64bfloat16 a, int idx) {
  return extract_512_1024(a, idx);
}

INTRINSIC(v64bfloat16) insert(v64bfloat16 a, int idx, v32bfloat16 b) {
  return insert_512_1024(a, idx, b);
}

INTRINSIC(v64bfloat16) set_v64bfloat16(int idx, v32bfloat16 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v64bfloat16) concat(v32bfloat16 a0, v32bfloat16 a1) {
  return concat_512_1024(a0, a1);
}

// v32accfloat

INTRINSIC(v16accfloat) extract_v16accfloat(v32accfloat a, int idx) {
  return extract_512_1024(a, idx);
}

INTRINSIC(v32accfloat) insert(v32accfloat a, int idx, v16accfloat b) {
  return insert_512_1024(a, idx, b);
}

INTRINSIC(v32accfloat) set_v32accfloat(int idx, v16accfloat b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v32accfloat) concat(v16accfloat a0, v16accfloat a1) {
  return concat_512_1024(a0, a1);
}

// v32float

INTRINSIC(v8float) extract_v8float(v32float a, int idx) {
  return extract_256_1024(a, idx);
}

INTRINSIC(v32float) insert(v32float a, int idx, v8float b) {
  return insert_256_1024(a, idx, b);
}

INTRINSIC(v32float) set_v32float(int idx, v8float b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v32float) concat(v8float a0, v8float a1, v8float a2, v8float a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v32float

INTRINSIC(v16float) extract_v16float(v32float a, int idx) {
  return extract_512_1024(a, idx);
}

INTRINSIC(v32float) insert(v32float a, int idx, v16float b) {
  return insert_512_1024(a, idx, b);
}

INTRINSIC(v32float) set_v32float(int idx, v16float b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v32float) concat(v16float a0, v16float a1) {
  return concat_512_1024(a0, a1);
}

// v32acc32

INTRINSIC(v16acc32) extract_v16acc32(v32acc32 a, int idx) {
  return extract_512_1024(a, idx);
}

INTRINSIC(v32acc32) insert(v32acc32 a, int idx, v16acc32 b) {
  return insert_512_1024(a, idx, b);
}

INTRINSIC(v32acc32) set_v32acc32(int idx, v16acc32 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v32acc32) concat(v16acc32 a0, v16acc32 a1) {
  return concat_512_1024(a0, a1);
}

// v16acc64

INTRINSIC(v8acc64) extract_v8acc64(v16acc64 a, int idx) {
  return extract_512_1024(a, idx);
}

INTRINSIC(v16acc64) insert(v16acc64 a, int idx, v8acc64 b) {
  return insert_512_1024(a, idx, b);
}

INTRINSIC(v16acc64) set_v16acc64(int idx, v8acc64 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v16acc64) concat(v8acc64 a0, v8acc64 a1) {
  return concat_512_1024(a0, a1);
}

// v64accfloat

INTRINSIC(v16accfloat) extract_v16accfloat(v64accfloat a, int idx) {
  return extract_ACC512_ACC2048(a, idx);
}

INTRINSIC(v64accfloat) insert(v64accfloat a, int idx, v16accfloat b) {
  return insert_ACC512_ACC2048(a, idx, b);
}

INTRINSIC(v64accfloat) set_v64accfloat(int idx, v16accfloat b) {
  return set_ACC512_ACC2048(idx, b);
}

INTRINSIC(v64accfloat)
concat(v16accfloat a0, v16accfloat a1, v16accfloat a2, v16accfloat a3) {
  return concat_ACC512_ACC2048(a0, a1, a2, a3);
}

// v64accfloat

INTRINSIC(v32accfloat) extract_v32accfloat(v64accfloat a, int idx) {
  return extract_ACC1024_ACC2048(a, idx);
}

INTRINSIC(v64accfloat) insert(v64accfloat a, int idx, v32accfloat b) {
  return insert_ACC1024_ACC2048(a, idx, b);
}

INTRINSIC(v64accfloat) set_v64accfloat(int idx, v32accfloat b) {
  return set_ACC1024_ACC2048(idx, b);
}

INTRINSIC(v64accfloat) concat(v32accfloat a0, v32accfloat a1) {
  return concat_ACC1024_ACC2048(a0, a1);
}

// v64acc32

INTRINSIC(v16acc32) extract_v16acc32(v64acc32 a, int idx) {
  return extract_ACC512_ACC2048(a, idx);
}

INTRINSIC(v64acc32) insert(v64acc32 a, int idx, v16acc32 b) {
  return insert_ACC512_ACC2048(a, idx, b);
}

INTRINSIC(v64acc32) set_v64acc32(int idx, v16acc32 b) {
  return set_ACC512_ACC2048(idx, b);
}

INTRINSIC(v64acc32) concat(v16acc32 a0, v16acc32 a1, v16acc32 a2, v16acc32 a3) {
  return concat_ACC512_ACC2048(a0, a1, a2, a3);
}

// v64acc32

INTRINSIC(v32acc32) extract_v32acc32(v64acc32 a, int idx) {
  return extract_ACC1024_ACC2048(a, idx);
}

INTRINSIC(v64acc32) insert(v64acc32 a, int idx, v32acc32 b) {
  return insert_ACC1024_ACC2048(a, idx, b);
}

INTRINSIC(v64acc32) set_v64acc32(int idx, v32acc32 b) {
  return set_ACC1024_ACC2048(idx, b);
}

INTRINSIC(v64acc32) concat(v32acc32 a0, v32acc32 a1) {
  return concat_ACC1024_ACC2048(a0, a1);
}

// v32acc64

INTRINSIC(v8acc64) extract_v8acc64(v32acc64 a, int idx) {
  return extract_ACC512_ACC2048(a, idx);
}

INTRINSIC(v32acc64) insert(v32acc64 a, int idx, v8acc64 b) {
  return insert_ACC512_ACC2048(a, idx, b);
}

INTRINSIC(v32acc64) set_v32acc64(int idx, v8acc64 b) {
  return set_ACC512_ACC2048(idx, b);
}

INTRINSIC(v32acc64) concat(v8acc64 a0, v8acc64 a1, v8acc64 a2, v8acc64 a3) {
  return concat_ACC512_ACC2048(a0, a1, a2, a3);
}

// v32acc64

INTRINSIC(v16acc64) extract_v16acc64(v32acc64 a, int idx) {
  return extract_ACC1024_ACC2048(a, idx);
}

INTRINSIC(v32acc64) insert(v32acc64 a, int idx, v16acc64 b) {
  return insert_ACC1024_ACC2048(a, idx, b);
}

INTRINSIC(v32acc64) set_v32acc64(int idx, v16acc64 b) {
  return set_ACC1024_ACC2048(idx, b);
}

INTRINSIC(v32acc64) concat(v16acc64 a0, v16acc64 a1) {
  return concat_ACC1024_ACC2048(a0, a1);
}

// 128-bit extract/insert/set/concat helpers + type wrappers + Conversions
// section live in aie_upd_ext_common.h. This is included AFTER the
// per-arch type wrappers above because the Conversions block in common
// references those (e.g. extract_v8int32). extract_128_512 needs the
// per-arch VEXTBCST.128 builtin, so define the macro before #include.
#define AIE_VEXTRACT_BROADCAST128_I512(v, idx)                                 \
  __builtin_aie2ps_vextract_broadcast128_I512((v), (idx))
#include "../aie_upd_ext_common.h"
#undef AIE_VEXTRACT_BROADCAST128_I512

// v64bfloat8

INTRINSIC(v32bfloat8) extract_v32bfloat8(v64bfloat8 a, int idx) {
  return {extract_256_512(a.data, idx)};
}

INTRINSIC(v64bfloat8) insert(v64bfloat8 a, int idx, v32bfloat8 b) {
  return {insert_256_512(a.data, idx, b.data)};
}

INTRINSIC(v64bfloat8) set_v64bfloat8(int idx, v32bfloat8 b) {
  return {set_256_512(idx, b.data)};
}

INTRINSIC(v64bfloat8) concat(v32bfloat8 a0, v32bfloat8 a1) {
  return {concat_256_512(a0.data, a1.data)};
}

// v64float8

INTRINSIC(v32float8) extract_v32float8(v64float8 a, int idx) {
  return {extract_256_512(a.data, idx)};
}

INTRINSIC(v64float8) insert(v64float8 a, int idx, v32float8 b) {
  return {insert_256_512(a.data, idx, b.data)};
}

INTRINSIC(v64float8) set_v64float8(int idx, v32float8 b) {
  return {set_256_512(idx, b.data)};
}

INTRINSIC(v64float8) concat(v32float8 a0, v32float8 a1) {
  return {concat_256_512(a0.data, a1.data)};
}

// v32float16

INTRINSIC(v16float16) extract_v16float16(v32float16 a, int idx) {
  return extract_256_512(a, idx);
}

INTRINSIC(v32float16) insert(v32float16 a, int idx, v16float16 b) {
  return insert_256_512(a, idx, b);
}

INTRINSIC(v32float16) set_v32float16(int idx, v16float16 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v32float16) concat(v16float16 a0, v16float16 a1) {
  return concat_256_512(a0, a1);
}

// v64float16

INTRINSIC(v16float16) extract_v16float16(v64float16 a, int idx) {
  return extract_256_1024(a, idx);
}

INTRINSIC(v64float16) insert(v64float16 a, int idx, v16float16 b) {
  return insert_256_1024(a, idx, b);
}

INTRINSIC(v64float16) set_v64float16(int idx, v16float16 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v64float16)
concat(v16float16 a0, v16float16 a1, v16float16 a2, v16float16 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v128float8

INTRINSIC(v32float8) extract_v32float8(v128float8 a, int idx) {
  return {extract_256_1024(a.data, idx)};
}

INTRINSIC(v128float8) insert(v128float8 a, int idx, v32float8 b) {
  return {insert_256_1024(a.data, idx, b.data)};
}

INTRINSIC(v128float8) set_v128float8(int idx, v32float8 b) {
  return {set_256_1024(idx, b.data)};
}

INTRINSIC(v128float8)
concat(v32float8 a0, v32float8 a1, v32float8 a2, v32float8 a3) {
  return {concat_256_1024(a0.data, a1.data, a2.data, a3.data)};
}

// v128bfloat8

INTRINSIC(v32bfloat8) extract_v32bfloat8(v128bfloat8 a, int idx) {
  return {extract_256_1024(a.data, idx)};
}

INTRINSIC(v128bfloat8) insert(v128bfloat8 a, int idx, v32bfloat8 b) {
  return {insert_256_1024(a.data, idx, b.data)};
}

INTRINSIC(v128bfloat8) set_v128bfloat8(int idx, v32bfloat8 b) {
  return {set_256_1024(idx, b.data)};
}

INTRINSIC(v128bfloat8)
concat(v32bfloat8 a0, v32bfloat8 a1, v32bfloat8 a2, v32bfloat8 a3) {
  return {concat_256_1024(a0.data, a1.data, a2.data, a3.data)};
}

// v128float8

INTRINSIC(v64float8) extract_v64float8(v128float8 a, int idx) {
  return {extract_512_1024(a.data, idx)};
}

INTRINSIC(v128float8) insert(v128float8 a, int idx, v64float8 b) {
  return {insert_512_1024(a.data, idx, b.data)};
}

INTRINSIC(v128float8) set_v128float8(int idx, v64float8 b) {
  return {set_512_1024(idx, b.data)};
}

INTRINSIC(v128float8) concat(v64float8 a0, v64float8 a1) {
  return {concat_512_1024(a0.data, a1.data)};
}

// v128bfloat8

INTRINSIC(v64bfloat8) extract_v64bfloat8(v128bfloat8 a, int idx) {
  return {extract_512_1024(a.data, idx)};
}

INTRINSIC(v128bfloat8) insert(v128bfloat8 a, int idx, v64bfloat8 b) {
  return {insert_512_1024(a.data, idx, b.data)};
}

INTRINSIC(v128bfloat8) set_v128bfloat8(int idx, v64bfloat8 b) {
  return {set_512_1024(idx, b.data)};
}

INTRINSIC(v128bfloat8) concat(v64bfloat8 a0, v64bfloat8 a1) {
  return {concat_512_1024(a0.data, a1.data)};
}

// v64float16

INTRINSIC(v32float16) extract_v32float16(v64float16 a, int idx) {
  return extract_512_1024(a, idx);
}

INTRINSIC(v64float16) insert(v64float16 a, int idx, v32float16 b) {
  return insert_512_1024(a, idx, b);
}

INTRINSIC(v64float16) set_v64float16(int idx, v32float16 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v64float16) concat(v32float16 a0, v32float16 a1) {
  return concat_512_1024(a0, a1);
}

INTRINSIC(v8float16)
extract_v8float16(v32float16 a, int idx) { return extract_128_512(a, idx); }

INTRINSIC(v32float16)
set_v32float16(int idx, v8float16 a) { return set_128_512(idx, a); }

INTRINSIC(v8float16)
extract_v8float16(v16float16 a, int idx) { return extract_128_256(a, idx); }

INTRINSIC(v16float16)
set_v16float16(int idx, v8float16 a) { return set_128_256(idx, a); }

INTRINSIC(v32float16)
insert(v32float16 v, int idx, v8float16 b) { return insert_128_512(v, idx, b); }

INTRINSIC(v16float16)
insert(v16float16 a, int idx, v8float16 b) { return insert_128_256(a, idx, b); }

INTRINSIC(v32float16)
concat(v8float16 v0, v8float16 v1, v8float16 v2, v8float16 v3) {
  return concat_128_512(v0, v1, v2, v3);
}
INTRINSIC(v16float16) concat(v8float16 v0, v8float16 v1) {
  return concat_128_256(v0, v1);
}

INTRINSIC(v16bfloat8)
extract_v16bfloat8(v64bfloat8 a, int idx) {
  return {extract_128_512(a.data, idx)};
}

INTRINSIC(v16bfloat8)
extract_v16bfloat8(v32bfloat8 a, int idx) {
  return {extract_128_256(a.data, idx)};
}
INTRINSIC(v16float8)
extract_v16float8(v64float8 a, int idx) {
  return {extract_128_512(a.data, idx)};
}

INTRINSIC(v16float8)
extract_v16float8(v32float8 a, int idx) {
  return {extract_128_256(a.data, idx)};
}

INTRINSIC(v64bfloat8) set_v64bfloat8(int idx, v16bfloat8 a) {
  return {set_128_512(idx, a.data)};
}
INTRINSIC(v32bfloat8) set_v32bfloat8(int idx, v16bfloat8 a) {
  return {set_128_256(idx, a.data)};
}
INTRINSIC(v64float8) set_v64float8(int idx, v16float8 a) {
  return {set_128_512(idx, a.data)};
}
INTRINSIC(v32float8) set_v32float8(int idx, v16float8 a) {
  return {set_128_256(idx, a.data)};
}

// v64bfloat8

INTRINSIC(v64bfloat8) insert(v64bfloat8 a, int idx, v16bfloat8 b) {
  return {insert_128_512(a.data, idx, b.data)};
}

INTRINSIC(v64bfloat8)
concat(v16bfloat8 a0, v16bfloat8 a1, v16bfloat8 a2, v16bfloat8 a3) {
  return {concat_128_512(a0.data, a1.data, a2.data, a3.data)};
}

// v64float8

INTRINSIC(v64float8) insert(v64float8 a, int idx, v16float8 b) {
  return {insert_128_512(a.data, idx, b.data)};
}

INTRINSIC(v64float8)
concat(v16float8 a0, v16float8 a1, v16float8 a2, v16float8 a3) {
  return {concat_128_512(a0.data, a1.data, a2.data, a3.data)};
}

// v32bfloat8

INTRINSIC(v32bfloat8) insert(v32bfloat8 a, int idx, v16bfloat8 b) {
  return {insert_128_256(a.data, idx, b.data)};
}

INTRINSIC(v32bfloat8)
concat(v16bfloat8 a0, v16bfloat8 a1) {
  return {concat_128_256(a0.data, a1.data)};
}
// v32float8

INTRINSIC(v32float8) insert(v32float8 a, int idx, v16float8 b) {
  return {insert_128_256(a.data, idx, b.data)};
}

INTRINSIC(v32float8)
concat(v16float8 a0, v16float8 a1) {
  return {concat_128_256(a0.data, a1.data)};
}

// bfp16
// v64mx9

INTRINSIC(v64mx9) insert(v64mx9 v, int idx, int exp32) {
  v2int32 exp64 = v.exponent;
  exp64[idx] = exp32;
  return {v.mantissa, v.tileShift, exp64};
}

INTRINSIC(v64mx9) insert(v64mx9 v, int idx, v4int8 prime) {
  v2int32 prime64 = v.tileShift;
  prime64[idx] = (int)prime;
  return {v.mantissa, prime64, v.exponent};
}

INTRINSIC(int) extract_exponent(v64mx9 m, int idx) { return m.exponent[idx]; }
INTRINSIC(v4int8) extract_prime(v64mx9 m, int idx) {
  return (v4int8)m.tileShift[idx];
}

INTRINSIC(v32int8) extract_v32int8(v64mx9 v, int idx) {
  return extract_256_512(v.mantissa, idx);
}

INTRINSIC(v64mx9) insert(v64mx9 v, v64int8 m) {
  return {m, v.tileShift, v.exponent};
}

INTRINSIC(v64int8) extract_v64int8(v64mx9 v) { return v.mantissa; }

// v128mx9
INTRINSIC(v64int8) extract_v64int8(v128mx9 v, int idx) {
  if (idx == 0)
    return v.mantissaX0;
  return v.mantissaX1;
}

INTRINSIC(v128mx9) insert(v128mx9 v, int idx, v64int8 m) {
  if (idx == 0)
    return {
        m,           v.mantissaX1, v.tileShiftG0, v.tileShiftG1, v.exponentE0,
        v.exponentE1};
  return {v.mantissaX0, m,           v.tileShiftG0, v.tileShiftG1,
          v.exponentE0, v.exponentE1};
}

INTRINSIC(v128mx9) concat(v64mx9 v1, v64mx9 v2) {
  return {v1.mantissa,  v2.mantissa, v1.tileShift,
          v2.tileShift, v1.exponent, v2.exponent};
}

INTRINSIC(v128mx9)
insert(v128mx9 v, int idx, v64mx9 vsub) {
  if (idx == 0)
    return {vsub.mantissa, v.mantissaX1,  vsub.tileShift,
            v.tileShiftG1, vsub.exponent, v.exponentE1};
  return {v.mantissaX0,   vsub.mantissa, v.tileShiftG0,
          vsub.tileShift, v.exponentE0,  vsub.exponent};
}

INTRINSIC(v128mx9) set_v128mx9(int idx, v64mx9 vsub) {
  v64mx9 undefValue;
  if (idx == 0)
    return {vsub.mantissa,        undefValue.mantissa, vsub.tileShift,
            undefValue.tileShift, vsub.exponent,       undefValue.exponent};
  return {undefValue.mantissa, vsub.mantissa,       undefValue.tileShift,
          vsub.tileShift,      undefValue.exponent, vsub.exponent};
}

INTRINSIC(v128bfp16p) set_v128bfp16p(int idx, v64bfp16p v) {
  return set_v128mx9(idx, v);
}

INTRINSIC(v128mx9) insert(v128mx9 v, int idx, int exp) {
  v64mx9 vsub0 = {v.mantissaX0, v.tileShiftG0, v.exponentE0};
  v64mx9 vsub1 = {v.mantissaX1, v.tileShiftG1, v.exponentE1};
  if (idx < 2)
    return insert(v, 0, insert(vsub0, (unsigned)idx % 2, exp));
  return insert(v, 1, insert(vsub1, (unsigned)idx % 2, exp));
}

INTRINSIC(v128mx9) insert(v128mx9 v, int idx, v4int8 prime) {
  v64mx9 vsub0 = {v.mantissaX0, v.tileShiftG0, v.exponentE0};
  v64mx9 vsub1 = {v.mantissaX1, v.tileShiftG1, v.exponentE1};
  if (idx < 2)
    return insert(v, 0, insert(vsub0, (unsigned)idx % 2, prime));
  return insert(v, 1, insert(vsub1, (unsigned)idx % 2, prime));
}

INTRINSIC(int) extract_exponent(v128mx9 v, int idx) {
  return idx < 2 ? v.exponentE0[idx % 2] : v.exponentE1[idx % 2];
}

INTRINSIC(v4int8) extract_prime(v128mx9 v, int idx) {
  return (v4int8)(idx < 2 ? v.tileShiftG0[idx % 2] : v.tileShiftG1[idx % 2]);
}

INTRINSIC(v64mx9) extract_v64mx9(v128mx9 m, int idx) {
  if (idx == 0)
    return {m.mantissaX0, m.tileShiftG0, m.exponentE0};
  return {m.mantissaX1, m.tileShiftG1, m.exponentE1};
}

INTRINSIC(v64bfp16p) extract_v64bfp16p(v128bfp16p a, int idx) {
  return extract_v64mx9(a, idx);
}

// v256mx9
INTRINSIC(v256mx9) insert(v256mx9 v, int idx, v128mx9 vsub) {
  if (idx == 0)
    return {vsub, v.h};
  return {v.l, vsub};
}
INTRINSIC(v128mx9) extract_v128mx9(v256mx9 v, int idx) {
  if (idx == 0)
    return v.l;
  return v.h;
}
INTRINSIC(v128bfp16p) extract_v128bfp16p(v256bfp16p a, int idx) {
  return extract_v128mx9(a, idx);
}
INTRINSIC(v256mx9) insert(v256mx9 m, int idx, v64mx9 a) {
  insert(m, idx / 2, insert(extract_v128mx9(m, idx / 2), idx % 2, a));
}

// BFP13 and BFP11

INTRINSIC(int) extract_expo(v64mx6 m, int idx) {
  if (idx == 0)
    return m.exponent;
  else
    return m.tileShift;
}

INTRINSIC(v64mx6) insert(v64mx6 m, int idx, int exp32) {
  if (idx == 0)
    m.exponent = exp32;
  else
    m.tileShift = exp32;
  return m;
}

INTRINSIC(int) extract_expo(v64mx4 m, int idx) {
  if (idx == 0)
    return m.exponent;
  else
    return m.tileShift;
}

INTRINSIC(v64mx4) insert(v64mx4 m, int idx, int exp32) {
  if (idx == 0)
    m.exponent = exp32;
  else
    m.tileShift = exp32;
  return m;
}

INTRINSIC(v16uint4) extract_msb_v16uint4(v256mx6 m, int idx) {
  v2int32 temp;
  if (idx == 0)
    temp = m.signF0;
  else if (idx == 1)
    temp = m.signF1;
  else if (idx == 2)
    temp = m.signF2;
  else
    temp = m.signF3;

  return (v16uint4)temp;
}

INTRINSIC(v32uint4) extract_msb_v32uint4(v256mx6 m, int idx) {
  if (idx == 0)
    return (v32uint4){m.signF0[0], m.signF0[1], m.signF1[0], m.signF1[1]};
  else
    return (v32uint4){m.signF2[0], m.signF2[1], m.signF3[0], m.signF3[1]};
}

INTRINSIC(v16uint4) extract_msb_v16uint4(v256mx4 m, int idx) {
  v2int32 temp;
  if (idx == 0)
    temp = m.signF0;
  else if (idx == 1)
    temp = m.signF1;
  else if (idx == 2)
    temp = m.signF2;
  else
    temp = m.signF3;

  return (v16uint4)temp;
}

INTRINSIC(v32uint4) extract_msb_v32uint4(v256mx4 m, int idx) {
  if (idx == 0)
    return (v32uint4){m.signF0[0], m.signF0[1], m.signF1[0], m.signF1[1]};
  else
    return (v32uint4){m.signF2[0], m.signF2[1], m.signF3[0], m.signF3[1]};
}

INTRINSIC(v256mx6) update(v256mx6 s, int idx, v16uint4 m) {
  if (idx == 0) {
    s.signF0 = (v2int32)m;
  } else if (idx == 1) {
    s.signF1 = (v2int32)m;
  } else if (idx == 2) {
    s.signF2 = (v2int32)m;
  } else {
    s.signF3 = (v2int32)m;
  }
  return s;
}

INTRINSIC(v256mx6) update(v256mx6 s, int idx, v32uint4 m) {
  v2int32 tempM0 = {((v4int32)m)[0], ((v4int32)m)[1]};
  v2int32 tempM1 = {((v4int32)m)[2], ((v4int32)m)[3]};
  if (idx == 0) {
    s.signF0 = tempM0;
    s.signF1 = tempM1;
  } else {
    s.signF2 = tempM0;
    s.signF3 = tempM1;
  }
  return s;
}

INTRINSIC(v256mx6) update(v256mx6 s, int idx, int m) {
  switch (idx) {
  case 0:
    s.exponentE0 = m;
    break;
  case 1:
    s.exponentE1 = m;
    break;
  case 2:
    s.exponentE2 = m;
    break;
  case 3:
    s.exponentE3 = m;
    break;
  case 4:
    s.tileShiftG0 = m;
    break;
  case 5:
    s.tileShiftG1 = m;
    break;
  case 6:
    s.tileShiftG2 = m;
    break;
  default:
    s.tileShiftG3 = m;
  }
  return s;
}

INTRINSIC(v256mx6) update(v256mx6 s, int idx, v8int8 m) {
  switch (idx) {
  case 0:
    s.exponentE0 = ((v2int32)m)[0];
    s.exponentE1 = ((v2int32)m)[1];
    break;
  case 1:
    s.exponentE2 = ((v2int32)m)[0];
    s.exponentE3 = ((v2int32)m)[1];
    break;
  case 2:
    s.tileShiftG0 = ((v2int32)m)[0];
    s.tileShiftG1 = ((v2int32)m)[1];
    break;
  default:
    s.tileShiftG2 = ((v2int32)m)[0];
    s.tileShiftG3 = ((v2int32)m)[1];
    break;
  }
  return s;
}

INTRINSIC(v256mx4) update(v256mx4 s, int idx, v16uint4 m) {
  if (idx == 0) {
    s.signF0 = (v2int32)m;
  } else if (idx == 1) {
    s.signF1 = (v2int32)m;
  } else if (idx == 2) {
    s.signF2 = (v2int32)m;
  } else {
    s.signF3 = (v2int32)m;
  }
  return s;
}

INTRINSIC(v256mx4) update(v256mx4 s, int idx, v32uint4 m) {
  v2int32 tempM0 = {((v4int32)m)[0], ((v4int32)m)[1]};
  v2int32 tempM1 = {((v4int32)m)[2], ((v4int32)m)[3]};
  if (idx == 0) {
    s.signF0 = tempM0;
    s.signF1 = tempM1;
  } else {
    s.signF2 = tempM0;
    s.signF3 = tempM1;
  }
  return s;
}

INTRINSIC(v256mx4) update(v256mx4 s, int idx, int m) {
  switch (idx) {
  case 0:
    s.exponentE0 = m;
    break;
  case 1:
    s.exponentE1 = m;
    break;
  case 2:
    s.exponentE2 = m;
    break;
  case 3:
    s.exponentE3 = m;
    break;
  case 4:
    s.tileShiftG0 = m;
    break;
  case 5:
    s.tileShiftG1 = m;
    break;
  case 6:
    s.tileShiftG2 = m;
    break;
  default:
    s.tileShiftG3 = m;
  }
  return s;
}

INTRINSIC(v256mx4) update(v256mx4 s, int idx, v8int8 m) {
  switch (idx) {
  case 0:
    s.exponentE0 = ((v2int32)m)[0];
    s.exponentE1 = ((v2int32)m)[1];
    break;
  case 1:
    s.exponentE2 = ((v2int32)m)[0];
    s.exponentE3 = ((v2int32)m)[1];
    break;
  case 2:
    s.tileShiftG0 = ((v2int32)m)[0];
    s.tileShiftG1 = ((v2int32)m)[1];
    break;
  default:
    s.tileShiftG2 = ((v2int32)m)[0];
    s.tileShiftG3 = ((v2int32)m)[1];
  }
  return s;
}

INTRINSIC(int) extract_expo_int(v256mx6 s, int idx) {
  switch (idx) {
  case 0:
    return s.exponentE0;
  case 1:
    return s.exponentE1;
  case 2:
    return s.exponentE2;
  case 3:
    return s.exponentE3;
  case 4:
    return s.tileShiftG0;
  case 5:
    return s.tileShiftG1;
  case 6:
    return s.tileShiftG2;
  default:
    return s.tileShiftG3;
  }
}

INTRINSIC(v8int8) extract_expo_v8int8(v256mx6 s, int idx) {
  switch (idx) {
  case 0:
    return {s.exponentE0, s.exponentE1};
  case 1:
    return {s.exponentE2, s.exponentE3};
  case 2:
    return {s.tileShiftG0, s.tileShiftG1};
  default:
    return {s.tileShiftG2, s.tileShiftG3};
  }
}

INTRINSIC(int) extract_expo_int(v256mx4 s, int idx) {
  switch (idx) {
  case 0:
    return s.exponentE0;
  case 1:
    return s.exponentE1;
  case 2:
    return s.exponentE2;
  case 3:
    return s.exponentE3;
  case 4:
    return s.tileShiftG0;
  case 5:
    return s.tileShiftG1;
  case 6:
    return s.tileShiftG2;
  default:
    return s.tileShiftG3;
  }
}

INTRINSIC(v8int8) extract_expo_v8int8(v256mx4 s, int idx) {
  switch (idx) {
  case 0:
    return {s.exponentE0, s.exponentE1};
  case 1:
    return {s.exponentE2, s.exponentE3};
  case 2:
    return {s.tileShiftG0, s.tileShiftG1};
  default:
    return {s.tileShiftG2, s.tileShiftG3};
  }
}

INTRINSIC(v64uint4) extract_v64uint4(v256mx6 v, int idx) {
  v8int32 temp;
  if (idx == 0)
    temp = v.mantissaX0;
  else if (idx == 1)
    temp = v.mantissaX1;
  else if (idx == 2)
    temp = v.mantissaX2;
  else
    temp = v.mantissaX3;

  return (v64uint4)temp;
}

INTRINSIC(v128uint4) extract_v128uint4(v256mx6 v, int idx) {
  v8int32 temp0;
  v8int32 temp1;
  v16int32 res;
  if (idx == 0) {
    temp0 = v.mantissaX0;
    temp1 = v.mantissaX1;
  } else {
    temp0 = v.mantissaX2;
    temp1 = v.mantissaX3;
  }

  res = concat(temp0, temp1);
  return (v128uint4)res;
}

INTRINSIC(v64uint4) extract_v64uint4(v256mx4 v, int idx) {
  v8int32 temp;
  if (idx == 0)
    temp = v.mantissaX0;
  else if (idx == 1)
    temp = v.mantissaX1;
  else if (idx == 2)
    temp = v.mantissaX2;
  else
    temp = v.mantissaX3;

  return (v64uint4)temp;
}

INTRINSIC(v128uint4) extract_v128uint4(v256mx4 v, int idx) {
  v8int32 temp0;
  v8int32 temp1;
  v16int32 res;
  if (idx == 0) {
    temp0 = v.mantissaX0;
    temp1 = v.mantissaX1;
  } else {
    temp0 = v.mantissaX2;
    temp1 = v.mantissaX3;
  }
  res = concat(temp0, temp1);

  return (v128uint4)res;
}

INTRINSIC(v256mx6) update(v256mx6 s, int idx, v128uint4 m) {
  v8int32 temp0 = extract_256_512((v16int32)m, 0);
  v8int32 temp1 = extract_256_512((v16int32)m, 1);
  s.mantissaX0 = idx == 0 ? temp0 : s.mantissaX0;
  s.mantissaX1 = idx == 0 ? temp1 : s.mantissaX1;
  s.mantissaX2 = idx == 0 ? s.mantissaX2 : temp0;
  s.mantissaX3 = idx == 0 ? s.mantissaX3 : temp1;
  return s;
}

INTRINSIC(v256mx6) update(v256mx6 s, int idx, v64uint4 m) {

  if (idx == 0)
    s.mantissaX0 = (v64uint4)m;
  else if (idx == 1)
    s.mantissaX1 = (v64uint4)m;
  else if (idx == 2)
    s.mantissaX2 = (v64uint4)m;
  else
    s.mantissaX3 = (v64uint4)m;
  return s;
}

INTRINSIC(v256mx4) update(v256mx4 s, int idx, v128uint4 m) {
  v8int32 temp0 = extract_256_512((v16int32)m, 0);
  v8int32 temp1 = extract_256_512((v16int32)m, 1);
  s.mantissaX0 = idx == 0 ? temp0 : s.mantissaX0;
  s.mantissaX1 = idx == 0 ? temp1 : s.mantissaX1;
  s.mantissaX2 = idx == 0 ? s.mantissaX2 : temp0;
  s.mantissaX3 = idx == 0 ? s.mantissaX3 : temp1;
  return s;
}

INTRINSIC(v256mx4) update(v256mx4 s, int idx, v64uint4 m) {
  if (idx == 0)
    s.mantissaX0 = (v64uint4)m;
  else if (idx == 1)
    s.mantissaX1 = (v64uint4)m;
  else if (idx == 2)
    s.mantissaX2 = (v64uint4)m;
  else
    s.mantissaX3 = (v64uint4)m;
  return s;
}

INTRINSIC(v64mx6) extract_v64mx6(v256mx6 m, int idx) {
  if (idx == 0)
    return {m.mantissaX0, m.signF0, m.tileShiftG0, m.exponentE0};
  else if (idx == 1)
    return {m.mantissaX1, m.signF1, m.tileShiftG1, m.exponentE1};

  else if (idx == 2)
    return {m.mantissaX2, m.signF2, m.tileShiftG2, m.exponentE2};
  else
    return {m.mantissaX3, m.signF3, m.tileShiftG3, m.exponentE3};
}

INTRINSIC(v64bfp13p) extract_v64bfp13p(v256bfp13p m, int idx) {
  return extract_v64mx6(m, idx);
}
INTRINSIC(v64mx4) extract_v64mx4(v256mx4 m, int idx) {
  if (idx == 0)
    return {m.mantissaX0, m.signF0, m.tileShiftG0, m.exponentE0};
  else if (idx == 1)
    return {m.mantissaX1, m.signF1, m.tileShiftG1, m.exponentE1};

  else if (idx == 2)
    return {m.mantissaX2, m.signF2, m.tileShiftG2, m.exponentE2};
  else
    return {m.mantissaX3, m.signF3, m.tileShiftG3, m.exponentE3};
}

INTRINSIC(v64bfp11p) extract_v64bfp11p(v256bfp11p m, int idx) {
  return extract_v64mx4(m, idx);
}

INTRINSIC(v256mx6) update(v256mx6 s, int idx, v64mx6 m) {
  if (idx == 0) {
    s.mantissaX0 = m.mantissa;
    s.signF0 = m.sign;
    s.tileShiftG0 = m.tileShift;
    s.exponentE0 = m.exponent;
  } else if (idx == 1) {
    s.mantissaX1 = m.mantissa;
    s.signF1 = m.sign;
    s.tileShiftG1 = m.tileShift;
    s.exponentE1 = m.exponent;
  } else if (idx == 2) {
    s.mantissaX2 = m.mantissa;
    s.signF2 = m.sign;
    s.tileShiftG2 = m.tileShift;
    s.exponentE2 = m.exponent;
  } else {
    s.mantissaX3 = m.mantissa;
    s.signF3 = m.sign;
    s.tileShiftG3 = m.tileShift;
    s.exponentE3 = m.exponent;
  }
  return s;
}
INTRINSIC(v256mx6) update(v256mx6 s, int idx, v128mx6 m) {
  if (idx == 0) {
    s.mantissaX0 = m.mantissaX0;
    s.signF0 = m.signF0;
    s.tileShiftG0 = m.tileShiftG0;
    s.exponentE0 = m.exponentE0;
    s.mantissaX1 = m.mantissaX1;
    s.signF1 = m.signF1;
    s.tileShiftG1 = m.tileShiftG1;
    s.exponentE1 = m.exponentE1;
  } else if (idx == 1) {
    s.mantissaX2 = m.mantissaX0;
    s.signF2 = m.signF0;
    s.tileShiftG2 = m.tileShiftG0;
    s.exponentE2 = m.exponentE0;
    s.mantissaX3 = m.mantissaX1;
    s.signF3 = m.signF1;
    s.tileShiftG3 = m.tileShiftG1;
    s.exponentE3 = m.exponentE1;
  }
  return s;
}

INTRINSIC(v256mx4) update(v256mx4 s, int idx, v64mx4 m) {
  if (idx == 0) {
    s.mantissaX0 = m.mantissa;
    s.signF0 = m.sign;
    s.tileShiftG0 = m.tileShift;
    s.exponentE0 = m.exponent;
  } else if (idx == 1) {
    s.mantissaX1 = m.mantissa;
    s.signF1 = m.sign;
    s.tileShiftG1 = m.tileShift;
    s.exponentE1 = m.exponent;
  } else if (idx == 2) {
    s.mantissaX2 = m.mantissa;
    s.signF2 = m.sign;
    s.tileShiftG2 = m.tileShift;
    s.exponentE2 = m.exponent;
  } else {
    s.mantissaX3 = m.mantissa;
    s.signF3 = m.sign;
    s.tileShiftG3 = m.tileShift;
    s.exponentE3 = m.exponent;
  }
  return s;
}
INTRINSIC(v256mx4) update(v256mx4 s, int idx, v128mx4 m) {
  if (idx == 0) {
    s.mantissaX0 = m.mantissaX0;
    s.signF0 = m.signF0;
    s.tileShiftG0 = m.tileShiftG0;
    s.exponentE0 = m.exponentE0;
    s.mantissaX1 = m.mantissaX1;
    s.signF1 = m.signF1;
    s.tileShiftG1 = m.tileShiftG1;
    s.exponentE1 = m.exponentE1;
  } else if (idx == 1) {
    s.mantissaX2 = m.mantissaX0;
    s.signF2 = m.signF0;
    s.tileShiftG2 = m.tileShiftG0;
    s.exponentE2 = m.exponentE0;
    s.mantissaX3 = m.mantissaX1;
    s.signF3 = m.signF1;
    s.tileShiftG3 = m.tileShiftG1;
    s.exponentE3 = m.exponentE1;
  }
  return s;
}

INTRINSIC(v128mx6) extract_v128mx6(v256mx6 s, int idx) {
  if (idx == 0) {
    return {s.mantissaX0,  s.mantissaX1,  s.signF0,     s.signF1,
            s.tileShiftG0, s.tileShiftG1, s.exponentE0, s.exponentE1};
  } else if (idx == 1) {
    return {s.mantissaX2,  s.mantissaX3,  s.signF2,     s.signF3,
            s.tileShiftG2, s.tileShiftG3, s.exponentE2, s.exponentE3};
  }
}

INTRINSIC(v128bfp13p) extract_v128bfp13p(v256bfp13p m, int idx) {
  return extract_v128mx6(m, idx);
}

INTRINSIC(v128mx4) extract_v128mx4(v256mx4 s, int idx) {
  if (idx == 0) {
    return {s.mantissaX0,  s.mantissaX1,  s.signF0,     s.signF1,
            s.tileShiftG0, s.tileShiftG1, s.exponentE0, s.exponentE1};
  } else if (idx == 1) {
    return {s.mantissaX2,  s.mantissaX3,  s.signF2,     s.signF3,
            s.tileShiftG2, s.tileShiftG3, s.exponentE2, s.exponentE3};
  }
}

INTRINSIC(v128bfp11p) extract_v128bfp11p(v256bfp11p m, int idx) {
  return extract_v128mx4(m, idx);
}

INTRINSIC(v256mx6) set_v256mx6(int idx, v128mx6 m) {
  v256mx6 s;
  if (idx == 0) {
    s.mantissaX0 = m.mantissaX0;
    s.signF0 = m.signF0;
    s.tileShiftG0 = m.tileShiftG0;
    s.exponentE0 = m.exponentE0;
    s.mantissaX1 = m.mantissaX1;
    s.signF1 = m.signF1;
    s.tileShiftG1 = m.tileShiftG1;
    s.exponentE1 = m.exponentE1;
  } else if (idx == 1) {
    s.mantissaX2 = m.mantissaX0;
    s.signF2 = m.signF0;
    s.tileShiftG2 = m.tileShiftG0;
    s.exponentE2 = m.exponentE0;
    s.mantissaX3 = m.mantissaX1;
    s.signF3 = m.signF1;
    s.tileShiftG3 = m.tileShiftG1;
    s.exponentE3 = m.exponentE1;
  }
  return s;
}
INTRINSIC(v256bfp13p) set_v256bfp13p(int idx, v128bfp13p m) {
  return set_v256mx6(idx, m);
}
INTRINSIC(v256mx6) concat(v128mx6 v0, v128mx6 v1) {
  return {v0.mantissaX0,  v0.mantissaX1,  v1.mantissaX0,  v1.mantissaX1,
          v0.signF0,      v0.signF1,      v1.signF0,      v1.signF1,
          v0.tileShiftG0, v0.tileShiftG1, v1.tileShiftG0, v1.tileShiftG1,
          v0.exponentE0,  v0.exponentE1,  v1.exponentE0,  v1.exponentE1};
}

INTRINSIC(v256mx4) set_v256mx4(int idx, v128mx4 m) {
  v256mx4 s;
  if (idx == 0) {
    s.mantissaX0 = m.mantissaX0;
    s.signF0 = m.signF0;
    s.tileShiftG0 = m.tileShiftG0;
    s.exponentE0 = m.exponentE0;
    s.mantissaX1 = m.mantissaX1;
    s.signF1 = m.signF1;
    s.tileShiftG1 = m.tileShiftG1;
    s.exponentE1 = m.exponentE1;
  } else if (idx == 1) {
    s.mantissaX2 = m.mantissaX0;
    s.signF2 = m.signF0;
    s.tileShiftG2 = m.tileShiftG0;
    s.exponentE2 = m.exponentE0;
    s.mantissaX3 = m.mantissaX1;
    s.signF3 = m.signF1;
    s.tileShiftG3 = m.tileShiftG1;
    s.exponentE3 = m.exponentE1;
  }
  return s;
}
INTRINSIC(v256bfp11p) set_v256bfp11p(int idx, v128bfp11p m) {
  return set_v256mx4(idx, m);
}
INTRINSIC(v256mx4) concat(v128mx4 v0, v128mx4 v1) {
  return {v0.mantissaX0,  v0.mantissaX1,  v1.mantissaX0,  v1.mantissaX1,
          v0.signF0,      v0.signF1,      v1.signF0,      v1.signF1,
          v0.tileShiftG0, v0.tileShiftG1, v1.tileShiftG0, v1.tileShiftG1,
          v0.exponentE0,  v0.exponentE1,  v1.exponentE0,  v1.exponentE1};
}

INTRINSIC(v64mx6) extract_v64mx6(v128mx6 s, int idx) {
  if (idx == 0) {
    return {s.mantissaX0, s.signF0, s.tileShiftG0, s.exponentE0};
  } else if (idx == 1) {
    return {s.mantissaX1, s.signF1, s.tileShiftG1, s.exponentE1};
  }
}
INTRINSIC(v64bfp13p) extract_v64bfp13p(v128bfp13p m, int idx) {
  return extract_v64mx6(m, idx);
}
INTRINSIC(v128mx6) insert(v128mx6 s, int idx, v64mx6 m) {
  if (idx == 0) {
    return {m.mantissa,  s.mantissaX1,  m.sign,     s.signF1,
            m.tileShift, s.tileShiftG1, m.exponent, s.exponentE1};
  } else if (idx == 1) {
    return {s.mantissaX0,  m.mantissa,  s.signF0,     m.sign,
            s.tileShiftG0, m.tileShift, s.exponentE0, m.exponent};
  }
}
INTRINSIC(v128mx6) set_v128mx6(int idx, v64mx6 m) {
  v128mx6 s;
  if (idx == 0) {
    return {m.mantissa,  s.mantissaX1,  m.sign,     s.signF1,
            m.tileShift, s.tileShiftG1, m.exponent, s.exponentE1};
  } else if (idx == 1) {
    return {s.mantissaX0,  m.mantissa,  s.signF0,     m.sign,
            s.tileShiftG0, m.tileShift, s.exponentE0, m.exponent};
  }
}
INTRINSIC(v128bfp13p) set_v128bfp13p(int idx, v64bfp13p m) {
  return set_v128mx6(idx, m);
}
INTRINSIC(v128mx6) concat(v64mx6 v0, v64mx6 v1) {
  return {v0.mantissa,  v1.mantissa,  v0.sign,     v1.sign,
          v0.tileShift, v1.tileShift, v0.exponent, v1.exponent};
}

INTRINSIC(v64mx4) extract_v64mx4(v128mx4 s, int idx) {
  if (idx == 0) {
    return {s.mantissaX0, s.signF0, s.tileShiftG0, s.exponentE0};
  } else if (idx == 1) {
    return {s.mantissaX1, s.signF1, s.tileShiftG1, s.exponentE1};
  }
}
INTRINSIC(v64bfp11p) extract_v64bfp11p(v128bfp11p m, int idx) {
  return extract_v64mx4(m, idx);
}
INTRINSIC(v128mx4) insert(v128mx4 s, int idx, v64mx4 m) {
  if (idx == 0) {
    return {m.mantissa,  s.mantissaX1,  m.sign,     s.signF1,
            m.tileShift, s.tileShiftG1, m.exponent, s.exponentE1};
  } else if (idx == 1) {
    return {s.mantissaX0,  m.mantissa,  s.signF0,     m.sign,
            s.tileShiftG0, m.tileShift, s.exponentE0, m.exponent};
  }
}
INTRINSIC(v128mx4) set_v128mx4(int idx, v64mx4 m) {
  v128mx4 s;
  if (idx == 0) {
    return {m.mantissa,  s.mantissaX1,  m.sign,     s.signF1,
            m.tileShift, s.tileShiftG1, m.exponent, s.exponentE1};
  } else if (idx == 1) {
    return {s.mantissaX0,  m.mantissa,  s.signF0,     m.sign,
            s.tileShiftG0, m.tileShift, s.exponentE0, m.exponent};
  }
}
INTRINSIC(v128bfp11p) set_v128bfp11p(int idx, v64bfp11p m) {
  return set_v128mx4(idx, m);
}
INTRINSIC(v128mx4) concat(v64mx4 v0, v64mx4 v1) {
  return {v0.mantissa,  v1.mantissa,  v0.sign,     v1.sign,
          v0.tileShift, v1.tileShift, v0.exponent, v1.exponent};
}

#endif // __AIE2PS_UPD_EXT_H__
