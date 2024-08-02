//===-------------------- AIEngine AIE2 intrinsics ------------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

//* Automatically generated file, do not edit! *
//

#ifndef __AIEV2_UPD_EXT_H__
#define __AIEV2_UPD_EXT_H__

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
  return (v2uint32)(a & ~mask) |
         (((unsigned long long)(val & elem_mask)) << (idx * step));
}

inline int ext_w32(int a, int idx, unsigned int elems, int step,
                   unsigned int elem_mask) {
  idx = idx & (elems - 1);
  return ((a << (32 - (idx + 1) * step)) >> (32 - step));
}

inline int ext_w64(mask64 a_, int idx, unsigned int elems, int step,
                   unsigned int elem_mask) {
  long long a = (unsigned long long)(a_);
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
  return ((v2int32)((a >> (idx * step)) & elem_mask));
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
  return ext_u64(a, idx, 16, 4, 0xf)[0];
}

inline unsigned int ext_v2u8(unsigned int a, int idx) {
  return ext_u32(a, idx, 2, 8, 0xff);
}
inline unsigned int ext_v4u8(unsigned int a, int idx) {
  return ext_u32(a, idx, 4, 8, 0xff);
}
inline unsigned int ext_v8u8(mask64 a, int idx) {
  return ext_u64(a, idx, 8, 8, 0xff)[0];
}

inline unsigned int ext_v2u16(unsigned int a, int idx) {
  return ext_u32(a, idx, 2, 16, 0xffff);
}
inline unsigned int ext_v4u16(mask64 a, int idx) {
  return ext_u64(a, idx, 4, 16, 0xffff)[0];
}

inline unsigned int ext_v2u32(mask64 a, int idx) {
  return ext_u64(a, idx, 2, 32, 0xffffffff)[0];
}

// Vector datatypes

// Scalar updates and extracts
INTRINSIC(unsigned long long)
insert(unsigned long long a, int idx, unsigned int b) {
  if (idx == 0)
    return (unsigned long long)__builtin_aiev2_upd_I64_I32((v2uint32)a, b, 0);
  else
    return (unsigned long long)__builtin_aiev2_upd_I64_I32((v2uint32)a, b, 1);
}
INTRINSIC(unsigned long long) set_u64(int idx, unsigned int b) {
  if (idx == 0)
    return (unsigned long long)__builtin_aiev2_set_I64_I32(b, 0);
  else
    return (unsigned long long)__builtin_aiev2_set_I64_I32(b, 1);
}
INTRINSIC(unsigned int) extract_uint32(unsigned long long a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I32_I64((v2uint32)a, 0);
  else
    return __builtin_aiev2_ext_I32_I64((v2uint32)a, 1);
}
INTRINSIC(unsigned long long) concat(unsigned int a, unsigned int b) {
  return insert(set_u64(a, 0), 1, b);
}

// Generic extract primitives

inline v8int32 extract_256_512(v16int32 a, int idx) {
  if (idx % 2 == 0) {
    return __builtin_shufflevector(a, a, 0, 1, 2, 3, 4, 5, 6, 7);
  } else {
    return __builtin_shufflevector(a, a, 8, 9, 10, 11, 12, 13, 14, 15);
  }
}

inline v16int32 extract_512_1024(v32int32 a, int idx) {
  if (idx % 2 == 0) {
    return __builtin_shufflevector(a, a, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                   12, 13, 14, 15);
  } else {
    return __builtin_shufflevector(a, a, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                                   26, 27, 28, 29, 30, 31);
  }
}

inline v8int32 extract_256_1024(v32int32 a, int idx) {
  if (idx % 4 == 0) {
    return __builtin_shufflevector(a, a, 0, 1, 2, 3, 4, 5, 6, 7);
  }
  if (idx % 4 == 1) {
    return __builtin_shufflevector(a, a, 8, 9, 10, 11, 12, 13, 14, 15);
  }
  if (idx % 4 == 2) {
    return __builtin_shufflevector(a, a, 16, 17, 18, 19, 20, 21, 22, 23);
  } else {
    return __builtin_shufflevector(a, a, 24, 25, 26, 27, 28, 29, 30, 31);
  }
}

// Generic insert primitives

inline v16int32 insert_256_512(v16int32 a, int idx, v8int32 b) {
  v8int32 undef_256;

  v16int32 tmp_512;

  tmp_512 = __builtin_shufflevector(b, undef_256, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                    10, 11, 12, 13, 14, 15);

  if (idx % 2 == 0) {
    return __builtin_shufflevector(tmp_512, a, 0, 1, 2, 3, 4, 5, 6, 7, 24, 25,
                                   26, 27, 28, 29, 30, 31);
  } else {
    return __builtin_shufflevector(tmp_512, a, 16, 17, 18, 19, 20, 21, 22, 23,
                                   0, 1, 2, 3, 4, 5, 6, 7);
  }
}

inline v32int32 insert_512_1024(v32int32 a, int idx, v16int32 b) {
  v16int32 undef_512;

  v32int32 tmp_1024;

  tmp_1024 = __builtin_shufflevector(
      b, undef_512, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
      17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);

  if (idx % 2 == 0) {
    return __builtin_shufflevector(tmp_1024, a, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                   10, 11, 12, 13, 14, 15, 48, 49, 50, 51, 52,
                                   53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
  } else {
    return __builtin_shufflevector(tmp_1024, a, 32, 33, 34, 35, 36, 37, 38, 39,
                                   40, 41, 42, 43, 44, 45, 46, 47, 0, 1, 2, 3,
                                   4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  }
}

inline v32int32 insert_256_1024(v32int32 a, int idx, v8int32 b) {
  v8int32 undef_256;
  v16int32 undef_512;

  v16int32 tmp_512;
  v32int32 tmp_1024;

  tmp_512 = __builtin_shufflevector(b, undef_256, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                    10, 11, 12, 13, 14, 15);
  tmp_1024 = __builtin_shufflevector(
      tmp_512, undef_512, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
      16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);

  if (idx % 4 == 0) {
    return __builtin_shufflevector(tmp_1024, a, 0, 1, 2, 3, 4, 5, 6, 7, 40, 41,
                                   42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
                                   53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
  }
  if (idx % 4 == 1) {
    return __builtin_shufflevector(tmp_1024, a, 32, 33, 34, 35, 36, 37, 38, 39,
                                   0, 1, 2, 3, 4, 5, 6, 7, 48, 49, 50, 51, 52,
                                   53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
  }
  if (idx % 4 == 2) {
    return __builtin_shufflevector(tmp_1024, a, 32, 33, 34, 35, 36, 37, 38, 39,
                                   40, 41, 42, 43, 44, 45, 46, 47, 0, 1, 2, 3,
                                   4, 5, 6, 7, 56, 57, 58, 59, 60, 61, 62, 63);
  } else {
    return __builtin_shufflevector(tmp_1024, a, 32, 33, 34, 35, 36, 37, 38, 39,
                                   40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                                   51, 52, 53, 54, 55, 0, 1, 2, 3, 4, 5, 6, 7);
  }
}

// Generic set primitives

inline v16int32 set_256_512(int idx, v8int32 b) {
  v8int32 tmp0;
  if (idx % 2 == 0) {
    return __builtin_shufflevector(b, tmp0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15);
  } else {

    return __builtin_shufflevector(tmp0, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15);
  }
}

inline v32int32 set_512_1024(int idx, v16int32 b) {
  v16int32 tmp0;
  if (idx % 2 == 0) {
    return __builtin_shufflevector(b, tmp0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                   22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  } else {

    return __builtin_shufflevector(tmp0, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                   22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  }
}

inline v32int32 set_256_1024(int idx, v8int32 b) {
  v8int32 tmp0;
  v16int32 tmp1;
  v16int32 undef1;
  if (idx % 4 == 0) {
    tmp1 = __builtin_shufflevector(b, tmp0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15);
    return __builtin_shufflevector(tmp1, undef1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                   10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                   21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  }
  if (idx % 4 == 1) {
    tmp1 = __builtin_shufflevector(tmp0, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15);
    return __builtin_shufflevector(tmp1, undef1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                   10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                   21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  }
  if (idx % 4 == 2) {
    tmp1 = __builtin_shufflevector(b, tmp0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15);
    return __builtin_shufflevector(undef1, tmp1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                   10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                   21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  } else {

    tmp1 = __builtin_shufflevector(tmp0, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15);
    return __builtin_shufflevector(undef1, tmp1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                   10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                   21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  }
}

// Generic concat primitives

inline v16int32 concat_256_512(v8int32 a0, v8int32 a1) {

  return __builtin_shufflevector(a0, a1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                 12, 13, 14, 15);
}

inline v32int32 concat_512_1024(v16int32 a0, v16int32 a1) {

  return __builtin_shufflevector(a0, a1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
                                 24, 25, 26, 27, 28, 29, 30, 31);
}

inline v32int32 concat_256_1024(v8int32 a0, v8int32 a1, v8int32 a2,
                                v8int32 a3) {
  v16int32 tmp0;
  v16int32 tmp1;

  tmp0 = __builtin_shufflevector(a0, a1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                 12, 13, 14, 15);
  tmp1 = __builtin_shufflevector(a2, a3, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                 12, 13, 14, 15);

  return __builtin_shufflevector(tmp0, tmp1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                                 23, 24, 25, 26, 27, 28, 29, 30, 31);
}

// Conversions

// v128uint4

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v64uint4) extract_v64uint4(v128uint4 a, int idx) {
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v128uint4) insert(v128uint4 a, int idx, v64uint4 b) {
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v128uint4) set_v128uint4(int idx, v64uint4 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v128uint4) concat(v64uint4 a0, v64uint4 a1) {
  return concat_256_512(a0, a1);
}

// v128int4

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v64int4) extract_v64int4(v128int4 a, int idx) {
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v128int4) insert(v128int4 a, int idx, v64int4 b) {
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v128int4) set_v128int4(int idx, v64int4 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v128int4) concat(v64int4 a0, v64int4 a1) {
  return concat_256_512(a0, a1);
}

// v64uint8

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v32uint8) extract_v32uint8(v64uint8 a, int idx) {
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v64uint8) insert(v64uint8 a, int idx, v32uint8 b) {
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v64uint8) set_v64uint8(int idx, v32uint8 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v64uint8) concat(v32uint8 a0, v32uint8 a1) {
  return concat_256_512(a0, a1);
}

// v64int8

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v32int8) extract_v32int8(v64int8 a, int idx) {
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v64int8) insert(v64int8 a, int idx, v32int8 b) {
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v64int8) set_v64int8(int idx, v32int8 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v64int8) concat(v32int8 a0, v32int8 a1) {
  return concat_256_512(a0, a1);
}

// v32uint16

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v16uint16) extract_v16uint16(v32uint16 a, int idx) {
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v32uint16) insert(v32uint16 a, int idx, v16uint16 b) {
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v32uint16) set_v32uint16(int idx, v16uint16 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v32uint16) concat(v16uint16 a0, v16uint16 a1) {
  return concat_256_512(a0, a1);
}

// v32int16

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v16int16) extract_v16int16(v32int16 a, int idx) {
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v32int16) insert(v32int16 a, int idx, v16int16 b) {
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v32int16) set_v32int16(int idx, v16int16 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v32int16) concat(v16int16 a0, v16int16 a1) {
  return concat_256_512(a0, a1);
}

// v16cint16

#if 0

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v8cint16) extract_v8cint16 (v16cint16 a, int idx)
{
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v16cint16) insert (v16cint16 a, int idx, v8cint16 b)
{
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v16cint16) set_v16cint16 (int idx, v8cint16 b)
{
  return set_256_512(idx, b);
}

INTRINSIC(v16cint16) concat (v8cint16 a0, v8cint16 a1)
{
  return concat_256_512(a0, a1);
}

#endif

// v16uint32

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v8uint32) extract_v8uint32(v16uint32 a, int idx) {
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v16uint32) insert(v16uint32 a, int idx, v8uint32 b) {
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v16uint32) set_v16uint32(int idx, v8uint32 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v16uint32) concat(v8uint32 a0, v8uint32 a1) {
  return concat_256_512(a0, a1);
}

// v16int32

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v8int32) extract_v8int32(v16int32 a, int idx) {
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v16int32) insert(v16int32 a, int idx, v8int32 b) {
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v16int32) set_v16int32(int idx, v8int32 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v16int32) concat(v8int32 a0, v8int32 a1) {
  return concat_256_512(a0, a1);
}

// v8cint32

#if 0

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v4cint32) extract_v4cint32 (v8cint32 a, int idx)
{
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v8cint32) insert (v8cint32 a, int idx, v4cint32 b)
{
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v8cint32) set_v8cint32 (int idx, v4cint32 b)
{
  return set_256_512(idx, b);
}

INTRINSIC(v8cint32) concat (v4cint32 a0, v4cint32 a1)
{
  return concat_256_512(a0, a1);
}

#endif

// v32bfloat16

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v16bfloat16) extract_v16bfloat16(v32bfloat16 a, int idx) {
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v32bfloat16) insert(v32bfloat16 a, int idx, v16bfloat16 b) {
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v32bfloat16) set_v32bfloat16(int idx, v16bfloat16 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v32bfloat16) concat(v16bfloat16 a0, v16bfloat16 a1) {
  return concat_256_512(a0, a1);
}

// v16accfloat

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v8accfloat) extract_v8accfloat(v16accfloat a, int idx) {
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v16accfloat) insert(v16accfloat a, int idx, v8accfloat b) {
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v16accfloat) set_v16accfloat(int idx, v8accfloat b) {
  return set_256_512(idx, b);
}

INTRINSIC(v16accfloat) concat(v8accfloat a0, v8accfloat a1) {
  return concat_256_512(a0, a1);
}

// v16float

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v8float) extract_v8float(v16float a, int idx) {
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v16float) insert(v16float a, int idx, v8float b) {
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v16float) set_v16float(int idx, v8float b) {
  return set_256_512(idx, b);
}

INTRINSIC(v16float) concat(v8float a0, v8float a1) {
  return concat_256_512(a0, a1);
}

// v16acc32

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v8acc32) extract_v8acc32(v16acc32 a, int idx) {
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v16acc32) insert(v16acc32 a, int idx, v8acc32 b) {
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v16acc32) set_v16acc32(int idx, v8acc32 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v16acc32) concat(v8acc32 a0, v8acc32 a1) {
  return concat_256_512(a0, a1);
}

// v8acc64

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v4acc64) extract_v4acc64(v8acc64 a, int idx) {
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v8acc64) insert(v8acc64 a, int idx, v4acc64 b) {
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v8acc64) set_v8acc64(int idx, v4acc64 b) {
  return set_256_512(idx, b);
}

INTRINSIC(v8acc64) concat(v4acc64 a0, v4acc64 a1) {
  return concat_256_512(a0, a1);
}

// v8cfloat

#if 0

//! @name Extract 256-bit portion from 512-bit register
inline INTRINSIC(v4cfloat) extract_v4cfloat (v8cfloat a, int idx)
{
  return extract_256_512(a, idx);
}

//! @name Insert 256-bit in 512-bit register
inline INTRINSIC(v8cfloat) insert (v8cfloat a, int idx, v4cfloat b)
{
  return insert_256_512(a, idx, b);
}

//! @name Set 256-bit portion of 512-bit register
inline INTRINSIC(v8cfloat) set_v8cfloat (int idx, v4cfloat b)
{
  return set_256_512(idx, b);
}

INTRINSIC(v8cfloat) concat (v4cfloat a0, v4cfloat a1)
{
  return concat_256_512(a0, a1);
}

#endif

// v256uint4

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v64uint4) extract_v64uint4(v256uint4 a, int idx) {
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v256uint4) insert(v256uint4 a, int idx, v64uint4 b) {
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v256uint4) set_v256uint4(int idx, v64uint4 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v256uint4)
concat(v64uint4 a0, v64uint4 a1, v64uint4 a2, v64uint4 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v256uint4

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v128uint4) extract_v128uint4(v256uint4 a, int idx) {
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v256uint4) insert(v256uint4 a, int idx, v128uint4 b) {
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v256uint4) set_v256uint4(int idx, v128uint4 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v256uint4) concat(v128uint4 a0, v128uint4 a1) {
  return concat_512_1024(a0, a1);
}

// v256int4

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v64int4) extract_v64int4(v256int4 a, int idx) {
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v256int4) insert(v256int4 a, int idx, v64int4 b) {
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v256int4) set_v256int4(int idx, v64int4 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v256int4) concat(v64int4 a0, v64int4 a1, v64int4 a2, v64int4 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v256int4

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v128int4) extract_v128int4(v256int4 a, int idx) {
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v256int4) insert(v256int4 a, int idx, v128int4 b) {
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v256int4) set_v256int4(int idx, v128int4 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v256int4) concat(v128int4 a0, v128int4 a1) {
  return concat_512_1024(a0, a1);
}

// v128uint8

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v32uint8) extract_v32uint8(v128uint8 a, int idx) {
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v128uint8) insert(v128uint8 a, int idx, v32uint8 b) {
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v128uint8) set_v128uint8(int idx, v32uint8 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v128uint8)
concat(v32uint8 a0, v32uint8 a1, v32uint8 a2, v32uint8 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v128uint8

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v64uint8) extract_v64uint8(v128uint8 a, int idx) {
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v128uint8) insert(v128uint8 a, int idx, v64uint8 b) {
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v128uint8) set_v128uint8(int idx, v64uint8 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v128uint8) concat(v64uint8 a0, v64uint8 a1) {
  return concat_512_1024(a0, a1);
}

// v128int8

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v32int8) extract_v32int8(v128int8 a, int idx) {
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v128int8) insert(v128int8 a, int idx, v32int8 b) {
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v128int8) set_v128int8(int idx, v32int8 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v128int8) concat(v32int8 a0, v32int8 a1, v32int8 a2, v32int8 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v128int8

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v64int8) extract_v64int8(v128int8 a, int idx) {
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v128int8) insert(v128int8 a, int idx, v64int8 b) {
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v128int8) set_v128int8(int idx, v64int8 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v128int8) concat(v64int8 a0, v64int8 a1) {
  return concat_512_1024(a0, a1);
}

// v64uint16

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v16uint16) extract_v16uint16(v64uint16 a, int idx) {
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v64uint16) insert(v64uint16 a, int idx, v16uint16 b) {
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v64uint16) set_v64uint16(int idx, v16uint16 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v64uint16)
concat(v16uint16 a0, v16uint16 a1, v16uint16 a2, v16uint16 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v64uint16

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v32uint16) extract_v32uint16(v64uint16 a, int idx) {
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v64uint16) insert(v64uint16 a, int idx, v32uint16 b) {
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v64uint16) set_v64uint16(int idx, v32uint16 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v64uint16) concat(v32uint16 a0, v32uint16 a1) {
  return concat_512_1024(a0, a1);
}

// v64int16

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v16int16) extract_v16int16(v64int16 a, int idx) {
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v64int16) insert(v64int16 a, int idx, v16int16 b) {
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v64int16) set_v64int16(int idx, v16int16 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v64int16) concat(v16int16 a0, v16int16 a1, v16int16 a2, v16int16 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v64int16

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v32int16) extract_v32int16(v64int16 a, int idx) {
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v64int16) insert(v64int16 a, int idx, v32int16 b) {
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v64int16) set_v64int16(int idx, v32int16 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v64int16) concat(v32int16 a0, v32int16 a1) {
  return concat_512_1024(a0, a1);
}

// v32cint16

#if 0

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v8cint16) extract_v8cint16 (v32cint16 a, int idx)
{
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v32cint16) insert (v32cint16 a, int idx, v8cint16 b)
{
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v32cint16) set_v32cint16 (int idx, v8cint16 b)
{
  return set_256_1024(idx, b);
}

INTRINSIC(v32cint16) concat (v8cint16 a0, v8cint16 a1, v8cint16 a2, v8cint16 a3)
{
  return concat_256_1024(a0, a1, a2, a3);
}

#endif

// v32cint16

#if 0

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v16cint16) extract_v16cint16 (v32cint16 a, int idx)
{
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v32cint16) insert (v32cint16 a, int idx, v16cint16 b)
{
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v32cint16) set_v32cint16 (int idx, v16cint16 b)
{
  return set_512_1024(idx, b);
}

INTRINSIC(v32cint16) concat (v16cint16 a0, v16cint16 a1)
{
  return concat_512_1024(a0, a1);
}

#endif

// v32uint32

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v8uint32) extract_v8uint32(v32uint32 a, int idx) {
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v32uint32) insert(v32uint32 a, int idx, v8uint32 b) {
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v32uint32) set_v32uint32(int idx, v8uint32 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v32uint32)
concat(v8uint32 a0, v8uint32 a1, v8uint32 a2, v8uint32 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v32uint32

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v16uint32) extract_v16uint32(v32uint32 a, int idx) {
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v32uint32) insert(v32uint32 a, int idx, v16uint32 b) {
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v32uint32) set_v32uint32(int idx, v16uint32 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v32uint32) concat(v16uint32 a0, v16uint32 a1) {
  return concat_512_1024(a0, a1);
}

// v32int32

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v8int32) extract_v8int32(v32int32 a, int idx) {
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v32int32) insert(v32int32 a, int idx, v8int32 b) {
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v32int32) set_v32int32(int idx, v8int32 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v32int32) concat(v8int32 a0, v8int32 a1, v8int32 a2, v8int32 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v32int32

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v16int32) extract_v16int32(v32int32 a, int idx) {
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v32int32) insert(v32int32 a, int idx, v16int32 b) {
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v32int32) set_v32int32(int idx, v16int32 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v32int32) concat(v16int32 a0, v16int32 a1) {
  return concat_512_1024(a0, a1);
}

// v16cint32

#if 0

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v4cint32) extract_v4cint32 (v16cint32 a, int idx)
{
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v16cint32) insert (v16cint32 a, int idx, v4cint32 b)
{
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v16cint32) set_v16cint32 (int idx, v4cint32 b)
{
  return set_256_1024(idx, b);
}

INTRINSIC(v16cint32) concat (v4cint32 a0, v4cint32 a1, v4cint32 a2, v4cint32 a3)
{
  return concat_256_1024(a0, a1, a2, a3);
}

#endif

// v16cint32

#if 0

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v8cint32) extract_v8cint32 (v16cint32 a, int idx)
{
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v16cint32) insert (v16cint32 a, int idx, v8cint32 b)
{
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v16cint32) set_v16cint32 (int idx, v8cint32 b)
{
  return set_512_1024(idx, b);
}

INTRINSIC(v16cint32) concat (v8cint32 a0, v8cint32 a1)
{
  return concat_512_1024(a0, a1);
}

#endif

// v64bfloat16

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v16bfloat16) extract_v16bfloat16(v64bfloat16 a, int idx) {
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v64bfloat16) insert(v64bfloat16 a, int idx, v16bfloat16 b) {
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v64bfloat16) set_v64bfloat16(int idx, v16bfloat16 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v64bfloat16)
concat(v16bfloat16 a0, v16bfloat16 a1, v16bfloat16 a2, v16bfloat16 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v64bfloat16

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v32bfloat16) extract_v32bfloat16(v64bfloat16 a, int idx) {
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v64bfloat16) insert(v64bfloat16 a, int idx, v32bfloat16 b) {
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v64bfloat16) set_v64bfloat16(int idx, v32bfloat16 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v64bfloat16) concat(v32bfloat16 a0, v32bfloat16 a1) {
  return concat_512_1024(a0, a1);
}

// v32accfloat

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v8accfloat) extract_v8accfloat(v32accfloat a, int idx) {
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v32accfloat) insert(v32accfloat a, int idx, v8accfloat b) {
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v32accfloat) set_v32accfloat(int idx, v8accfloat b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v32accfloat)
concat(v8accfloat a0, v8accfloat a1, v8accfloat a2, v8accfloat a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v32accfloat

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v16accfloat) extract_v16accfloat(v32accfloat a, int idx) {
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v32accfloat) insert(v32accfloat a, int idx, v16accfloat b) {
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v32accfloat) set_v32accfloat(int idx, v16accfloat b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v32accfloat) concat(v16accfloat a0, v16accfloat a1) {
  return concat_512_1024(a0, a1);
}

// v32float

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v8float) extract_v8float(v32float a, int idx) {
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v32float) insert(v32float a, int idx, v8float b) {
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v32float) set_v32float(int idx, v8float b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v32float) concat(v8float a0, v8float a1, v8float a2, v8float a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v32float

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v16float) extract_v16float(v32float a, int idx) {
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v32float) insert(v32float a, int idx, v16float b) {
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v32float) set_v32float(int idx, v16float b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v32float) concat(v16float a0, v16float a1) {
  return concat_512_1024(a0, a1);
}

// v32acc32

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v8acc32) extract_v8acc32(v32acc32 a, int idx) {
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v32acc32) insert(v32acc32 a, int idx, v8acc32 b) {
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v32acc32) set_v32acc32(int idx, v8acc32 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v32acc32) concat(v8acc32 a0, v8acc32 a1, v8acc32 a2, v8acc32 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v32acc32

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v16acc32) extract_v16acc32(v32acc32 a, int idx) {
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v32acc32) insert(v32acc32 a, int idx, v16acc32 b) {
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v32acc32) set_v32acc32(int idx, v16acc32 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v32acc32) concat(v16acc32 a0, v16acc32 a1) {
  return concat_512_1024(a0, a1);
}

// v16acc64

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v4acc64) extract_v4acc64(v16acc64 a, int idx) {
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v16acc64) insert(v16acc64 a, int idx, v4acc64 b) {
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v16acc64) set_v16acc64(int idx, v4acc64 b) {
  return set_256_1024(idx, b);
}

INTRINSIC(v16acc64) concat(v4acc64 a0, v4acc64 a1, v4acc64 a2, v4acc64 a3) {
  return concat_256_1024(a0, a1, a2, a3);
}

// v16acc64

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v8acc64) extract_v8acc64(v16acc64 a, int idx) {
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v16acc64) insert(v16acc64 a, int idx, v8acc64 b) {
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v16acc64) set_v16acc64(int idx, v8acc64 b) {
  return set_512_1024(idx, b);
}

INTRINSIC(v16acc64) concat(v8acc64 a0, v8acc64 a1) {
  return concat_512_1024(a0, a1);
}

// v16cfloat

#if 0

//! @name Extract 256-bit portion from 1024-bit register
inline INTRINSIC(v4cfloat) extract_v4cfloat (v16cfloat a, int idx)
{
  return extract_256_1024(a, idx);
}

//! @name Insert 256-bit in 1024-bit register
inline INTRINSIC(v16cfloat) insert (v16cfloat a, int idx, v4cfloat b)
{
  return insert_256_1024(a, idx, b);
}

//! @name Set 256-bit portion of 1024-bit register
inline INTRINSIC(v16cfloat) set_v16cfloat (int idx, v4cfloat b)
{
  return set_256_1024(idx, b);
}

INTRINSIC(v16cfloat) concat (v4cfloat a0, v4cfloat a1, v4cfloat a2, v4cfloat a3)
{
  return concat_256_1024(a0, a1, a2, a3);
}

#endif

// v16cfloat

#if 0

//! @name Extract 512-bit portion from 1024-bit register
inline INTRINSIC(v8cfloat) extract_v8cfloat (v16cfloat a, int idx)
{
  return extract_512_1024(a, idx);
}

//! @name Insert 512-bit in 1024-bit register
inline INTRINSIC(v16cfloat) insert (v16cfloat a, int idx, v8cfloat b)
{
  return insert_512_1024(a, idx, b);
}

//! @name Set 512-bit portion of 1024-bit register
inline INTRINSIC(v16cfloat) set_v16cfloat (int idx, v8cfloat b)
{
  return set_512_1024(idx, b);
}

INTRINSIC(v16cfloat) concat (v8cfloat a0, v8cfloat a1)
{
  return concat_512_1024(a0, a1);
}

#endif

// Generic 128-bit extract primitives

inline v4int32 extract_128_256(v8int32 a, int idx) {
  if (idx % 2 == 0) {
    return __builtin_shufflevector(a, a, 0, 1, 2, 3);
  } else {
    return __builtin_shufflevector(a, a, 4, 5, 6, 7);
  }
}

inline v4int32 extract_128_512(v16int32 a, int idx) {
  if (idx % 4 == 0) {
    return __builtin_shufflevector(a, a, 0, 1, 2, 3);
  }
  if (idx % 4 == 1) {
    return __builtin_shufflevector(a, a, 4, 5, 6, 7);
  }
  if (idx % 4 == 2) {
    return __builtin_shufflevector(a, a, 8, 9, 10, 11);
  } else {
    return __builtin_shufflevector(a, a, 12, 13, 14, 15);
  }
}

// Generic 128-bit insert primitives

inline v8int32 insert_128_256(v8int32 a, int idx, v4int32 b) {
  v4int32 undef_128;

  v8int32 tmp_256;

  tmp_256 = __builtin_shufflevector(b, undef_128, 0, 1, 2, 3, 4, 5, 6, 7);

  if (idx % 2 == 0) {
    return __builtin_shufflevector(tmp_256, a, 0, 1, 2, 3, 12, 13, 14, 15);
  } else {
    return __builtin_shufflevector(tmp_256, a, 8, 9, 10, 11, 0, 1, 2, 3);
  }
}

inline v16int32 insert_128_512(v16int32 a, int idx, v4int32 b) {
  v4int32 undef_128;
  v8int32 undef_256;

  v8int32 tmp_256;
  v16int32 tmp_512;

  tmp_256 = __builtin_shufflevector(b, undef_128, 0, 1, 2, 3, 4, 5, 6, 7);
  tmp_512 = __builtin_shufflevector(tmp_256, undef_256, 0, 1, 2, 3, 4, 5, 6, 7,
                                    8, 9, 10, 11, 12, 13, 14, 15);

  if (idx % 4 == 0) {
    return __builtin_shufflevector(tmp_512, a, 0, 1, 2, 3, 20, 21, 22, 23, 24,
                                   25, 26, 27, 28, 29, 30, 31);
  }
  if (idx % 4 == 1) {
    return __builtin_shufflevector(tmp_512, a, 16, 17, 18, 19, 0, 1, 2, 3, 24,
                                   25, 26, 27, 28, 29, 30, 31);
  }
  if (idx % 4 == 2) {
    return __builtin_shufflevector(tmp_512, a, 16, 17, 18, 19, 20, 21, 22, 23,
                                   0, 1, 2, 3, 28, 29, 30, 31);
  } else {
    return __builtin_shufflevector(tmp_512, a, 16, 17, 18, 19, 20, 21, 22, 23,
                                   24, 25, 26, 27, 0, 1, 2, 3);
  }
}

// Generic 128-bit set primitives

inline v8int32 set_128_256(int idx, v4int32 b) {
  v4int32 tmp0;
  if (idx % 2 == 0) {
    return __builtin_shufflevector(b, tmp0, 0, 1, 2, 3, 4, 5, 6, 7);
  } else {

    return __builtin_shufflevector(tmp0, b, 0, 1, 2, 3, 4, 5, 6, 7);
  }
}

inline v16int32 set_128_512(int idx, v4int32 b) {
  v4int32 tmp0;
  v8int32 tmp1;
  v8int32 undef1;
  if (idx % 4 == 0) {
    tmp1 = __builtin_shufflevector(b, tmp0, 0, 1, 2, 3, 4, 5, 6, 7);
    return __builtin_shufflevector(tmp1, undef1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                   10, 11, 12, 13, 14, 15);
  }
  if (idx % 4 == 1) {
    tmp1 = __builtin_shufflevector(tmp0, b, 0, 1, 2, 3, 4, 5, 6, 7);
    return __builtin_shufflevector(tmp1, undef1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                   10, 11, 12, 13, 14, 15);
  }
  if (idx % 4 == 2) {
    tmp1 = __builtin_shufflevector(b, tmp0, 0, 1, 2, 3, 4, 5, 6, 7);
    return __builtin_shufflevector(undef1, tmp1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                   10, 11, 12, 13, 14, 15);
  } else {

    tmp1 = __builtin_shufflevector(tmp0, b, 0, 1, 2, 3, 4, 5, 6, 7);
    return __builtin_shufflevector(undef1, tmp1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                   10, 11, 12, 13, 14, 15);
  }
}

// Generic 128-bit concat primitives

inline v8int32 concat_128_256(v4int32 a0, v4int32 a1) {

  return __builtin_shufflevector(a0, a1, 0, 1, 2, 3, 4, 5, 6, 7);
}

inline v16int32 concat_128_512(v4int32 a0, v4int32 a1, v4int32 a2, v4int32 a3) {
  v8int32 tmp0;
  v8int32 tmp1;

  tmp0 = __builtin_shufflevector(a0, a1, 0, 1, 2, 3, 4, 5, 6, 7);
  tmp1 = __builtin_shufflevector(a2, a3, 0, 1, 2, 3, 4, 5, 6, 7);

  return __builtin_shufflevector(tmp0, tmp1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                 11, 12, 13, 14, 15);
}

#define DIAGNOSE_EXTRACT_IDX(MAX)                                              \
  __attribute__((diagnose_if(idx < 0 || idx > MAX,                             \
                             "index out of range [0," #MAX "]", "error")))

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v32uint4)
extract_v32uint4(v128uint4 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v32int4)
extract_v32int4(v128int4 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v16uint8)
extract_v16uint8(v64uint8 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v16int8) extract_v16int8(v64int8 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v8uint16)
extract_v8uint16(v32uint16 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v8int16)
extract_v8int16(v32int16 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v4uint32)
extract_v4uint32(v16uint32 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v4int32)
extract_v4int32(v16int32 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v8bfloat16)
extract_v8bfloat16(v32bfloat16 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v4float)
extract_v4float(v16float a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

//! @name Set 128-bit portion of 256-bit register
//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v128uint4)
set_v128uint4(int idx, v32uint4 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v128int4) set_v128int4(int idx, v32int4 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v64uint8) set_v64uint8(int idx, v16uint8 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v64int8) set_v64int8(int idx, v16int8 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v32uint16)
set_v32uint16(int idx, v8uint16 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v32int16) set_v32int16(int idx, v8int16 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v16uint32)
set_v16uint32(int idx, v4uint32 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v16int32) set_v16int32(int idx, v4int32 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v32bfloat16)
set_v32bfloat16(int idx, v8bfloat16 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v16float) set_v16float(int idx, v4float a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v32uint4)
extract_v32uint4(v64uint4 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v32int4) extract_v32int4(v64int4 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v16uint8)
extract_v16uint8(v32uint8 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v16int8) extract_v16int8(v32int8 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v8uint16)
extract_v8uint16(v16uint16 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v8int16)
extract_v8int16(v16int16 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v4uint32)
extract_v4uint32(v8uint32 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v4int32) extract_v4int32(v8int32 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v8bfloat16)
extract_v8bfloat16(v16bfloat16 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

//! @name Extract 128-bit portion from 256-bit register
INTRINSIC(v4float) extract_v4float(v8float a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v64uint4) set_v64uint4(int idx, v32uint4 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v64int4) set_v64int4(int idx, v32int4 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v32uint8) set_v32uint8(int idx, v16uint8 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v32int8) set_v32int8(int idx, v16int8 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v16uint16)
set_v16uint16(int idx, v8uint16 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v16int16) set_v16int16(int idx, v8int16 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v8uint32) set_v8uint32(int idx, v4uint32 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v8int32) set_v8int32(int idx, v4int32 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v16bfloat16)
set_v16bfloat16(int idx, v8bfloat16 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}

//! @name Set 128-bit portion of 256-bit register
INTRINSIC(v8float) set_v8float(int idx, v4float a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}

//! @name Insert 128-bit in 512-bit register
//! @name Insert 128-bit in 512-bit register
INTRINSIC(v128uint4)
insert(v128uint4 v, int idx, v32uint4 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}

//! @name Insert 128-bit in 512-bit register
//! @name Insert 128-bit in 512-bit register
INTRINSIC(v128int4)
insert(v128int4 v, int idx, v32int4 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}

//! @name Insert 128-bit in 512-bit register
//! @name Insert 128-bit in 512-bit register
INTRINSIC(v64uint8)
insert(v64uint8 v, int idx, v16uint8 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}

//! @name Insert 128-bit in 512-bit register
//! @name Insert 128-bit in 512-bit register
INTRINSIC(v64int8)
insert(v64int8 v, int idx, v16int8 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}

//! @name Insert 128-bit in 512-bit register
//! @name Insert 128-bit in 512-bit register
INTRINSIC(v32uint16)
insert(v32uint16 v, int idx, v8uint16 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}

//! @name Insert 128-bit in 512-bit register
//! @name Insert 128-bit in 512-bit register
INTRINSIC(v32int16)
insert(v32int16 v, int idx, v8int16 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}

//! @name Insert 128-bit in 512-bit register
//! @name Insert 128-bit in 512-bit register
INTRINSIC(v16uint32)
insert(v16uint32 v, int idx, v4uint32 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}

//! @name Insert 128-bit in 512-bit register
//! @name Insert 128-bit in 512-bit register
INTRINSIC(v16int32)
insert(v16int32 v, int idx, v4int32 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}

//! @name Insert 128-bit in 512-bit register
//! @name Insert 128-bit in 512-bit register
INTRINSIC(v32bfloat16)
insert(v32bfloat16 v, int idx, v8bfloat16 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}

//! @name Insert 128-bit in 512-bit register
//! @name Insert 128-bit in 512-bit register
INTRINSIC(v16float)
insert(v16float v, int idx, v4float b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}

//! @name Insert 128-bit in 512-bit register
INTRINSIC(v64uint4)
insert(v64uint4 a, int idx, v32uint4 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}

//! @name Insert 128-bit in 512-bit register
INTRINSIC(v64int4)
insert(v64int4 a, int idx, v32int4 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}

//! @name Insert 128-bit in 512-bit register
INTRINSIC(v32uint8)
insert(v32uint8 a, int idx, v16uint8 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}

//! @name Insert 128-bit in 512-bit register
INTRINSIC(v32int8)
insert(v32int8 a, int idx, v16int8 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}

//! @name Insert 128-bit in 512-bit register
INTRINSIC(v16uint16)
insert(v16uint16 a, int idx, v8uint16 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}

//! @name Insert 128-bit in 512-bit register
INTRINSIC(v16int16)
insert(v16int16 a, int idx, v8int16 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}

//! @name Insert 128-bit in 512-bit register
INTRINSIC(v8uint32)
insert(v8uint32 a, int idx, v4uint32 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}

//! @name Insert 128-bit in 512-bit register
INTRINSIC(v8int32)
insert(v8int32 a, int idx, v4int32 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}

//! @name Insert 128-bit in 512-bit register
INTRINSIC(v16bfloat16)
insert(v16bfloat16 a, int idx, v8bfloat16 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}

//! @name Insert 128-bit in 512-bit register
INTRINSIC(v8float)
insert(v8float a, int idx, v4float b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}

INTRINSIC(v128uint4)
concat(v32uint4 v0, v32uint4 v1, v32uint4 v2, v32uint4 v3) {
  return concat_128_512(v0, v1, v2, v3);
}

INTRINSIC(v128int4) concat(v32int4 v0, v32int4 v1, v32int4 v2, v32int4 v3) {
  return concat_128_512(v0, v1, v2, v3);
}

INTRINSIC(v64uint8) concat(v16uint8 v0, v16uint8 v1, v16uint8 v2, v16uint8 v3) {
  return concat_128_512(v0, v1, v2, v3);
}

INTRINSIC(v64int8) concat(v16int8 v0, v16int8 v1, v16int8 v2, v16int8 v3) {
  return concat_128_512(v0, v1, v2, v3);
}

INTRINSIC(v32uint16)
concat(v8uint16 v0, v8uint16 v1, v8uint16 v2, v8uint16 v3) {
  return concat_128_512(v0, v1, v2, v3);
}

INTRINSIC(v32int16) concat(v8int16 v0, v8int16 v1, v8int16 v2, v8int16 v3) {
  return concat_128_512(v0, v1, v2, v3);
}

INTRINSIC(v16uint32)
concat(v4uint32 v0, v4uint32 v1, v4uint32 v2, v4uint32 v3) {
  return concat_128_512(v0, v1, v2, v3);
}

INTRINSIC(v16int32) concat(v4int32 v0, v4int32 v1, v4int32 v2, v4int32 v3) {
  return concat_128_512(v0, v1, v2, v3);
}

INTRINSIC(v32bfloat16)
concat(v8bfloat16 v0, v8bfloat16 v1, v8bfloat16 v2, v8bfloat16 v3) {
  return concat_128_512(v0, v1, v2, v3);
}

INTRINSIC(v16float) concat(v4float v0, v4float v1, v4float v2, v4float v3) {
  return concat_128_512(v0, v1, v2, v3);
}

INTRINSIC(v64uint4) concat(v32uint4 v0, v32uint4 v1) {
  return concat_128_256(v0, v1);
}

INTRINSIC(v64int4) concat(v32int4 v0, v32int4 v1) {
  return concat_128_256(v0, v1);
}

INTRINSIC(v32uint8) concat(v16uint8 v0, v16uint8 v1) {
  return concat_128_256(v0, v1);
}

INTRINSIC(v32int8) concat(v16int8 v0, v16int8 v1) {
  return concat_128_256(v0, v1);
}

INTRINSIC(v16uint16) concat(v8uint16 v0, v8uint16 v1) {
  return concat_128_256(v0, v1);
}

INTRINSIC(v16int16) concat(v8int16 v0, v8int16 v1) {
  return concat_128_256(v0, v1);
}

INTRINSIC(v8uint32) concat(v4uint32 v0, v4uint32 v1) {
  return concat_128_256(v0, v1);
}

INTRINSIC(v8int32) concat(v4int32 v0, v4int32 v1) {
  return concat_128_256(v0, v1);
}

INTRINSIC(v16bfloat16) concat(v8bfloat16 v0, v8bfloat16 v1) {
  return concat_128_256(v0, v1);
}

INTRINSIC(v8float) concat(v4float v0, v4float v1) {
  return concat_128_256(v0, v1);
}

#endif // __AIEV2_UPD_EXT_H__