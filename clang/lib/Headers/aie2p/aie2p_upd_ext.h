//===- aie2p_upd_ext.h ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2P_UPD_EXT_H__
#define __AIE2P_UPD_EXT_H__

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

//!   @name Scalar updates and extracts
INTRINSIC(unsigned long long)
insert(unsigned long long a, int idx, unsigned int b) {
  if (idx == 0)
    return (unsigned long long)__builtin_aiev2p_upd_I64_I32((v2uint32)a, b, 0);
  else
    return (unsigned long long)__builtin_aiev2p_upd_I64_I32((v2uint32)a, b, 1);
}
INTRINSIC(unsigned long long) set_uint64(int idx, unsigned int b) {
  if (idx == 0)
    return (unsigned long long)__builtin_aiev2p_set_I64_I32(b, 0);
  else
    return (unsigned long long)__builtin_aiev2p_set_I64_I32(b, 1);
}
INTRINSIC(unsigned int) extract_uint32(unsigned long long a, int idx) {
  if (idx == 0)
    return __builtin_aiev2p_ext_I32_I64((v2uint32)a, 0);
  else
    return __builtin_aiev2p_ext_I32_I64((v2uint32)a, 1);
}
INTRINSIC(unsigned long long) concat(unsigned int a, unsigned int b) {
  return insert(set_uint64(a, 0), 1, b);
}

// Generic extract primitives
INTRINSIC(v8int32) extract_256_512(v16int32 a, int idx) {
  if (idx % 2 == 0)
    return __builtin_shufflevector(a, a, 0, 1, 2, 3, 4, 5, 6, 7);
  else
    return __builtin_shufflevector(a, a, 8, 9, 10, 11, 12, 13, 14, 15);
}

INTRINSIC(v16int32) extract_512_1024(v32int32 a, int idx) {
  if (idx % 2 == 0)
    return __builtin_shufflevector(a, a, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15);
  else
    return __builtin_shufflevector(a, a, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                                   26, 27, 28, 29, 30, 31);
}

INTRINSIC(v8int32) extract_256_1024(v32int32 a, int idx) {
  if (idx % 4 == 0)
    return __builtin_shufflevector(a, a, 0, 1, 2, 3, 4, 5, 6, 7);
  if (idx % 4 == 1)
    return __builtin_shufflevector(a, a, 8, 9, 10, 11, 12, 13, 14, 15);
  if (idx % 4 == 2)
    return __builtin_shufflevector(a, a, 16, 17, 18, 19, 20, 21, 22, 23);
  else
    return __builtin_shufflevector(a, a, 24, 25, 26, 27, 28, 29, 30, 31);
}

INTRINSIC(v16acc64) extract_ACC1024_ACC2048(v32acc64 a, int idx) {
  if (idx % 2 == 0)
    return __builtin_shufflevector(a, a, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15);
  else
    return __builtin_shufflevector(a, a, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                                   26, 27, 28, 29, 30, 31);
}

INTRINSIC(v8acc64) extract_ACC512_ACC2048(v32acc64 a, int idx) {
  if (idx % 4 == 0)
    return __builtin_shufflevector(a, a, 0, 1, 2, 3, 4, 5, 6, 7);
  if (idx % 4 == 1)
    return __builtin_shufflevector(a, a, 8, 9, 10, 11, 12, 13, 14, 15);
  if (idx % 4 == 2)
    return __builtin_shufflevector(a, a, 16, 17, 18, 19, 20, 21, 22, 23);
  else
    return __builtin_shufflevector(a, a, 24, 25, 26, 27, 28, 29, 30, 31);
}

// Generic insert primitives
INTRINSIC(v16int32) insert_256_512(v16int32 a, int idx, v8int32 b) {
  v8int32 undef;
  v16int32 tmp = __builtin_shufflevector(b, undef, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                         10, 11, 12, 13, 14, 15);
  if (idx % 2 == 0)
    return __builtin_shufflevector(tmp, a, 0, 1, 2, 3, 4, 5, 6, 7, 24, 25, 26,
                                   27, 28, 29, 30, 31);
  // insert into upper half
  return __builtin_shufflevector(a, tmp, 0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 18, 19,
                                 20, 21, 22, 23);
}

INTRINSIC(v32int32) insert_512_1024(v32int32 a, int idx, v16int32 b) {
  v16int32 undef;
  v32int32 tmp = __builtin_shufflevector(
      b, undef, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
      18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  if (idx % 2 == 0)
    return __builtin_shufflevector(tmp, a, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15, 48, 49, 50, 51, 52, 53, 54,
                                   55, 56, 57, 58, 59, 60, 61, 62, 63);
  // insert into upper half
  return __builtin_shufflevector(a, tmp, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                 12, 13, 14, 15, 32, 33, 34, 35, 36, 37, 38, 39,
                                 40, 41, 42, 43, 44, 45, 46, 47);
}

INTRINSIC(v32int32) insert_256_1024(v32int32 a, int idx, v8int32 b) {
  v8int32 undef_256;
  v16int32 undef_512;
  v16int32 tmp_512 = __builtin_shufflevector(b, undef_256, 0, 1, 2, 3, 4, 5, 6,
                                             7, 8, 9, 10, 11, 12, 13, 14, 15);
  v32int32 tmp_1024 = __builtin_shufflevector(
      tmp_512, undef_512, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
      16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  if (idx % 4 == 0)
    return __builtin_shufflevector(tmp_1024, a, 0, 1, 2, 3, 4, 5, 6, 7, 40, 41,
                                   42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
                                   53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
  if (idx % 4 == 1)
    return __builtin_shufflevector(tmp_1024, a, 32, 33, 34, 35, 36, 37, 38, 39,
                                   0, 1, 2, 3, 4, 5, 6, 7, 48, 49, 50, 51, 52,
                                   53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
  if (idx % 4 == 2)
    return __builtin_shufflevector(tmp_1024, a, 32, 33, 34, 35, 36, 37, 38, 39,
                                   40, 41, 42, 43, 44, 45, 46, 47, 0, 1, 2, 3,
                                   4, 5, 6, 7, 56, 57, 58, 59, 60, 61, 62, 63);
  else
    return __builtin_shufflevector(tmp_1024, a, 32, 33, 34, 35, 36, 37, 38, 39,
                                   40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                                   51, 52, 53, 54, 55, 0, 1, 2, 3, 4, 5, 6, 7);
}

INTRINSIC(v32acc64) insert_ACC1024_ACC2048(v32acc64 a, int idx, v16acc64 b) {
  v32acc64 tmp = __builtin_shufflevector(
      b, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
  if (idx % 2 == 0)
    return __builtin_shufflevector(tmp, a, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15, 48, 49, 50, 51, 52, 53, 54,
                                   55, 56, 57, 58, 59, 60, 61, 62, 63);

  // insert into upper half
  return __builtin_shufflevector(a, tmp, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                 12, 13, 14, 15, 32, 33, 34, 35, 36, 37, 38, 39,
                                 40, 41, 42, 43, 44, 45, 46, 47);
}

INTRINSIC(v32acc64) insert_ACC512_ACC2048(v32acc64 a, int idx, v8acc64 b) {
  v16acc64 tmp_1024 = __builtin_shufflevector(b, b, 0, 1, 2, 3, 4, 5, 6, 7, -1,
                                              -1, -1, -1, -1, -1, -1, -1);
  v32acc64 tmp_2048 = __builtin_shufflevector(
      tmp_1024, tmp_1024, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
  if (idx % 4 == 0)
    return __builtin_shufflevector(tmp_2048, a, 0, 1, 2, 3, 4, 5, 6, 7, 40, 41,
                                   42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
                                   53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
  if (idx % 4 == 1)
    return __builtin_shufflevector(a, tmp_2048, 0, 1, 2, 3, 4, 5, 6, 7, 32, 33,
                                   34, 35, 36, 37, 38, 39, 16, 17, 18, 19, 20,
                                   21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  if (idx % 4 == 2)
    return __builtin_shufflevector(a, tmp_2048, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                   10, 11, 12, 13, 14, 15, 32, 33, 34, 35, 36,
                                   37, 38, 39, 24, 25, 26, 27, 28, 29, 30, 31);
  else
    return __builtin_shufflevector(a, tmp_2048, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                   10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                   21, 22, 23, 32, 33, 34, 35, 36, 37, 38, 39);
}

// Generic set primitives
INTRINSIC(v16int32) set_256_512(int idx, v8int32 b) {
  v8int32 tmp;
  if (idx % 2 == 0)
    return __builtin_shufflevector(b, tmp, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15);
  else
    return __builtin_shufflevector(tmp, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15);
}

INTRINSIC(v32int32) set_512_1024(int idx, v16int32 b) {
  v16int32 tmp;
  if (idx % 2 == 0)
    return __builtin_shufflevector(b, tmp, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                                   23, 24, 25, 26, 27, 28, 29, 30, 31);
  else
    return __builtin_shufflevector(tmp, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                                   23, 24, 25, 26, 27, 28, 29, 30, 31);
}

INTRINSIC(v32int32) set_256_1024(int idx, v8int32 b) {
  v8int32 tmp;
  v16int32 tmp2;
  v16int32 tmp3;
  if (idx % 4 == 0) {
    tmp2 = __builtin_shufflevector(b, tmp, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15);
    return __builtin_shufflevector(tmp2, tmp3, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                   22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  }
  if (idx % 4 == 1) {
    tmp2 = __builtin_shufflevector(tmp, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15);
    return __builtin_shufflevector(tmp2, tmp3, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                   22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  }
  if (idx % 4 == 2) {
    tmp2 = __builtin_shufflevector(b, tmp, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15);
    return __builtin_shufflevector(tmp3, tmp2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                   22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  } else {
    tmp2 = __builtin_shufflevector(tmp, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15);
    return __builtin_shufflevector(tmp3, tmp2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                   22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  }
}

INTRINSIC(v32acc64) set_ACC1024_ACC2048(int idx, v16acc64 b) {
  if (idx % 2 == 0)
    return __builtin_shufflevector(b, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1);
  else
    return __builtin_shufflevector(b, b, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6,
                                   7, 8, 9, 10, 11, 12, 13, 14, 15);
}

INTRINSIC(v32acc64) set_ACC512_ACC2048(int idx, v8acc64 b) {
  v16acc64 tmp = __builtin_shufflevector(b, b, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1,
                                         -1, -1, -1, -1, -1, -1);

  if (idx % 4 == 0)
    return __builtin_shufflevector(tmp, tmp, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
  if (idx % 4 == 1)
    return __builtin_shufflevector(tmp, tmp, -1, -1, -1, -1, -1, -1, -1, -1, 0,
                                   1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
  if (idx % 4 == 2)
    return __builtin_shufflevector(tmp, tmp, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5,
                                   6, 7, -1, -1, -1, -1, -1, -1, -1, -1);
  else
    return __builtin_shufflevector(tmp, tmp, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                   -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7);
}

// Generic concat primitives
INTRINSIC(v16int32) concat_256_512(v8int32 a0, v8int32 a1) {
  return __builtin_shufflevector(a0, a1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                 12, 13, 14, 15);
}

INTRINSIC(v32int32) concat_512_1024(v16int32 a0, v16int32 a1) {
  return __builtin_shufflevector(a0, a1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                                 24, 25, 26, 27, 28, 29, 30, 31);
}

INTRINSIC(v32int32)
concat_256_1024(v8int32 a0, v8int32 a1, v8int32 a2, v8int32 a3) {
  v16int32 res_hi;
  v16int32 res_lo;
  res_hi = __builtin_shufflevector(a0, a1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15);
  res_lo = __builtin_shufflevector(a2, a3, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15);
  return __builtin_shufflevector(res_hi, res_lo, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
}

INTRINSIC(v32acc64) concat_ACC1024_ACC2048(v16acc64 a0, v16acc64 a1) {
  return __builtin_shufflevector(a0, a1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                                 24, 25, 26, 27, 28, 29, 30, 31);
}

INTRINSIC(v32acc64)
concat_ACC512_ACC2048(v8acc64 a0, v8acc64 a1, v8acc64 a2, v8acc64 a3) {
  v16acc64 res_hi;
  v16acc64 res_lo;
  res_lo = __builtin_shufflevector(a0, a1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15);
  res_hi = __builtin_shufflevector(a2, a3, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15);
  return __builtin_shufflevector(res_lo, res_hi, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
}

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

// shared with aie2ps — see clang/lib/Headers/aie_upd_ext_common.h
#include "../aie_upd_ext_common.h"

INTRINSIC(v64bfp16ebs16) insert(v64bfp16ebs16 v, v64int8 m) {
  return {m, v.exponent};
}

INTRINSIC(v64int8) extract_v64int8(v64bfp16ebs16 v) { return v.mantissa; }

INTRINSIC(v64int8) extract_data(v64bfp16ebs16 v) { return extract_v64int8(v); }

INTRINSIC(int)
extract_exponent(v64bfp16ebs16 v, int idx) {
  v2int32 exp = v.exponent;
  return idx == 0 ? exp[0] : exp[1];
}

INTRINSIC(v64bfp16ebs16) insert(v64bfp16ebs16 v, int idx, int exp32) {
  v2int32 exp64 = v.exponent;
  exp64[idx] = exp32;
  return {v.mantissa, exp64};
}

INTRINSIC(v64bfp16ebs8) insert(v64bfp16ebs8 v, v64int8 m) {
  return {m, v.exponent};
}

INTRINSIC(v64int8) extract_v64int8(v64bfp16ebs8 v) { return v.mantissa; }

INTRINSIC(v64int8) extract_data(v64bfp16ebs8 v) { return extract_v64int8(v); }

INTRINSIC(int) extract_exponent(v64bfp16ebs8 v, int idx) {
  v2int32 exp = v.exponent;
  return idx == 0 ? exp[0] : exp[1];
}

INTRINSIC(v64bfp16ebs8) insert(v64bfp16ebs8 v, int idx, int exp32) {
  v2int32 exp64 = v.exponent;
  exp64[idx] = exp32;
  return {v.mantissa, exp64};
}

INTRINSIC(v32int8) extract_v32int8(v64bfp16ebs16 v, int idx) {
  return extract_256_512(v.mantissa, idx);
}

INTRINSIC(v32int8) extract_v32int8(v64bfp16ebs8 v, int idx) {
  return extract_256_512(v.mantissa, idx);
}

INTRINSIC(v64int8) extract_v64int8(v128bfp16ebs8 v, int idx) {
  if (idx == 0)
    return v.mantissaX0;
  return v.mantissaX1;
}

INTRINSIC(v128bfp16ebs8) insert(v128bfp16ebs8 v, int idx, v64int8 m) {
  if (idx == 0)
    return {m, v.mantissaX1, v.exponentE0, v.exponentE1};
  return {v.mantissaX0, m, v.exponentE0, v.exponentE1};
}

INTRINSIC(v128bfp16ebs8) concat(v64bfp16ebs8 v1, v64bfp16ebs8 v2) {
  return {v1.mantissa, v2.mantissa, v1.exponent, v2.exponent};
}

INTRINSIC(v128bfp16ebs8) insert(v128bfp16ebs8 v, int idx, v64bfp16ebs8 vsub) {
  if (idx == 0)
    return {vsub.mantissa, v.mantissaX1, vsub.exponent, v.exponentE1};
  return {v.mantissaX0, vsub.mantissa, v.exponentE0, vsub.exponent};
}

INTRINSIC(v128bfp16ebs8) set_v128bfp16ebs8(int idx, v64bfp16ebs8 vsub) {
  v64bfp16ebs8 undefValue;
  if (idx == 0)
    return {vsub.mantissa, undefValue.mantissa, vsub.exponent,
            undefValue.exponent};
  return {undefValue.mantissa, vsub.mantissa, undefValue.exponent,
          vsub.exponent};
}

INTRINSIC(v128bfp16ebs8) insert(v128bfp16ebs8 v, int idx, int exp) {
  v64bfp16ebs8 vsub0 = {v.mantissaX0, v.exponentE0};
  v64bfp16ebs8 vsub1 = {v.mantissaX1, v.exponentE1};
  if (idx < 2)
    return insert(v, 0, insert(vsub0, (unsigned)idx % 2, exp));
  return insert(v, 1, insert(vsub1, (unsigned)idx % 2, exp));
}

INTRINSIC(int) extract_exponent(v128bfp16ebs8 v, int idx) {
  v8int8 exp0 = v.exponentE0;
  v8int8 exp1 = v.exponentE1;
  if (idx == 0)
    return (int)__builtin_shufflevector(exp0, exp0, 0, 1, 2, 3);
  else if (idx == 1)
    return (int)__builtin_shufflevector(exp0, exp0, 4, 5, 6, 7);
  else if (idx == 2)
    return (int)__builtin_shufflevector(exp1, exp1, 0, 1, 2, 3);
  return (int)__builtin_shufflevector(exp1, exp1, 4, 5, 6, 7);
}

INTRINSIC(v64int8) extract_v64int8(v128bfp16ebs16 v, int idx) {
  if (idx == 0)
    return v.mantissaX0;
  return v.mantissaX1;
}

INTRINSIC(v128bfp16ebs16) insert(v128bfp16ebs16 v, int idx, v64int8 m) {
  if (idx == 0)
    return {m, v.mantissaX1, v.exponentE0, v.exponentE1};
  return {v.mantissaX0, m, v.exponentE0, v.exponentE1};
}

INTRINSIC(v128bfp16ebs16) concat(v64bfp16ebs16 v1, v64bfp16ebs16 v2) {
  return {v1.mantissa, v2.mantissa, v1.exponent, v2.exponent};
}

INTRINSIC(v128bfp16ebs16)
insert(v128bfp16ebs16 v, int idx, v64bfp16ebs16 vsub) {
  if (idx == 0)
    return {vsub.mantissa, v.mantissaX1, vsub.exponent, v.exponentE1};
  return {v.mantissaX0, vsub.mantissa, v.exponentE0, vsub.exponent};
}

INTRINSIC(v128bfp16ebs16) set_v128bfp16ebs16(int idx, v64bfp16ebs16 vsub) {
  v64bfp16ebs16 undefValue;
  if (idx == 0)
    return {vsub.mantissa, undefValue.mantissa, vsub.exponent,
            undefValue.exponent};
  return {undefValue.mantissa, vsub.mantissa, undefValue.exponent,
          vsub.exponent};
}

INTRINSIC(v128bfp16ebs16) insert(v128bfp16ebs16 v, int idx, int exp) {
  v64bfp16ebs16 vsub0 = {v.mantissaX0, v.exponentE0};
  v64bfp16ebs16 vsub1 = {v.mantissaX1, v.exponentE1};
  if (idx < 2)
    return insert(v, 0, insert(vsub0, (unsigned)idx % 2, exp));
  return insert(v, 1, insert(vsub1, (unsigned)idx % 2, exp));
}

INTRINSIC(int) extract_exponent(v128bfp16ebs16 v, int idx) {
  v8int8 exp0 = v.exponentE0;
  v8int8 exp1 = v.exponentE1;
  if (idx == 0)
    return (int)__builtin_shufflevector(exp0, exp0, 0, 1, 2, 3);
  else if (idx == 1)
    return (int)__builtin_shufflevector(exp0, exp0, 4, 5, 6, 7);
  else if (idx == 2)
    return (int)__builtin_shufflevector(exp1, exp1, 0, 1, 2, 3);
  return (int)__builtin_shufflevector(exp1, exp1, 4, 5, 6, 7);
}

INTRINSIC(v64bfp16ebs16) extract_v64bfp16ebs16(v128bfp16ebs16 m, int idx) {
  if (idx == 0)
    return {m.mantissaX0, m.exponentE0};
  return {m.mantissaX1, m.exponentE1};
}

INTRINSIC(v64bfp16ebs8) extract_v64bfp16ebs8(v128bfp16ebs8 m, int idx) {
  if (idx == 0)
    return {m.mantissaX0, m.exponentE0};
  return {m.mantissaX1, m.exponentE1};
}

#endif // __AIE2P_UPD_EXT_H__
