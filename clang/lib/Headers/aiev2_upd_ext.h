//===- aiev2_upd_ext.h ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV2_UPD_EXT_H__
#define __AIEV2_UPD_EXT_H__

inline int get_idx(int idx, unsigned int elems) { return (idx & (elems - 1)); }

// Small vector datatypes
inline unsigned int set_w32(int idx, unsigned int val, unsigned int elems,
                            int step, unsigned int elem_mask) {
  idx = get_idx(idx, elems);
  return ((val & elem_mask) << (idx * step));
}

inline mask64 set_w64(int idx, unsigned int val, unsigned int elems, int step,
                      unsigned int elem_mask) {
  idx = get_idx(idx, elems);
  return (v2uint32)(((unsigned long long)(val & elem_mask)) << (idx * step));
}

inline unsigned int upd_w32(unsigned int a, int idx, unsigned int val,
                            unsigned int elems, int step,
                            unsigned int elem_mask) {
  idx = get_idx(idx, elems);
  unsigned int mask = unsigned(elem_mask) << (idx * step);
  return (a & ~mask) | ((val & elem_mask) << (idx * step));
}

inline mask64 upd_w64(mask64 a_, int idx, unsigned int val, unsigned int elems,
                      int step, unsigned int elem_mask) {
  unsigned long long a = (unsigned long long)(a_);
  idx = get_idx(idx, elems);
  unsigned long long mask = ((unsigned long long)(elem_mask)) << (idx * step);
  return (v2uint32)((a & ~mask) |
                    (((unsigned long long)(val & elem_mask)) << (idx * step)));
}

inline int ext_w32(int a, int idx, unsigned int elems, int step,
                   unsigned int elem_mask) {
  idx = get_idx(idx, elems);
  return ((a << (32 - (idx + 1) * step)) >> (32 - step));
}

inline int ext_w64(mask64 a_, int idx, unsigned int elems, int step,
                   unsigned int elem_mask) {
  long long a = (long long)(a_);
  idx = get_idx(idx, elems);
  return ((v2int32)((a >> (idx * step)) & elem_mask))[0];
}

inline unsigned int ext_u32(unsigned int a, int idx, unsigned int elems,
                            int step, unsigned int elem_mask) {
  idx = get_idx(idx, elems);
  return (a >> (idx * step)) & elem_mask;
}

inline mask64 ext_u64(mask64 a_, int idx, unsigned int elems, int step,
                      unsigned int elem_mask) {
  unsigned long long a = (unsigned long long)(a_);
  idx = get_idx(idx, elems);
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

// Extract 256-bit portion from 512-bit register
INTRINSIC(v64uint4) extract_v64uint4(v128uint4 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I512(a, 0);
  else
    return __builtin_aiev2_ext_I256_I512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v128uint4) insert(v128uint4 a, int idx, v64uint4 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I512_I256(a, b, 0);
  else
    return __builtin_aiev2_upd_I512_I256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v128uint4) set_v128uint4(int idx, v64uint4 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I512_I256(b, 0);
  else
    return __builtin_aiev2_set_I512_I256(b, 1);
}

INTRINSIC(v128uint4) concat(v64uint4 a0, v64uint4 a1) {
  return __builtin_aiev2_concat_I512_I256(a0, a1);
}

// Extract 256-bit portion from 512-bit register
INTRINSIC(v64int4) extract_v64int4(v128int4 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I512(a, 0);
  else
    return __builtin_aiev2_ext_I256_I512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v128int4) insert(v128int4 a, int idx, v64int4 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I512_I256(a, b, 0);
  else
    return __builtin_aiev2_upd_I512_I256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v128int4) set_v128int4(int idx, v64int4 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I512_I256(b, 0);
  else
    return __builtin_aiev2_set_I512_I256(b, 1);
}

INTRINSIC(v128int4) concat(v64int4 a0, v64int4 a1) {
  return __builtin_aiev2_concat_I512_I256(a0, a1);
}

// Extract 256-bit portion from 512-bit register
INTRINSIC(v32uint8) extract_v32uint8(v64uint8 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I512(a, 0);
  else
    return __builtin_aiev2_ext_I256_I512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v64uint8) insert(v64uint8 a, int idx, v32uint8 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I512_I256(a, b, 0);
  else
    return __builtin_aiev2_upd_I512_I256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v64uint8) set_v64uint8(int idx, v32uint8 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I512_I256(b, 0);
  else
    return __builtin_aiev2_set_I512_I256(b, 1);
}

INTRINSIC(v64uint8) concat(v32uint8 a0, v32uint8 a1) {
  return __builtin_aiev2_concat_I512_I256(a0, a1);
}

// Extract 256-bit portion from 512-bit register
INTRINSIC(v32int8) extract_v32int8(v64int8 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I512(a, 0);
  else
    return __builtin_aiev2_ext_I256_I512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v64int8) insert(v64int8 a, int idx, v32int8 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I512_I256(a, b, 0);
  else
    return __builtin_aiev2_upd_I512_I256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v64int8) set_v64int8(int idx, v32int8 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I512_I256(b, 0);
  else
    return __builtin_aiev2_set_I512_I256(b, 1);
}

INTRINSIC(v64int8) concat(v32int8 a0, v32int8 a1) {
  return __builtin_aiev2_concat_I512_I256(a0, a1);
}

// Extract 256-bit portion from 512-bit register
INTRINSIC(v16uint16) extract_v16uint16(v32uint16 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I512(a, 0);
  else
    return __builtin_aiev2_ext_I256_I512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v32uint16) insert(v32uint16 a, int idx, v16uint16 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I512_I256(a, b, 0);
  else
    return __builtin_aiev2_upd_I512_I256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v32uint16) set_v32uint16(int idx, v16uint16 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I512_I256(b, 0);
  else
    return __builtin_aiev2_set_I512_I256(b, 1);
}

INTRINSIC(v32uint16) concat(v16uint16 a0, v16uint16 a1) {
  return __builtin_aiev2_concat_I512_I256(a0, a1);
}

// Extract 256-bit portion from 512-bit register
INTRINSIC(v16int16) extract_v16int16(v32int16 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I512(a, 0);
  else
    return __builtin_aiev2_ext_I256_I512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v32int16) insert(v32int16 a, int idx, v16int16 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I512_I256(a, b, 0);
  else
    return __builtin_aiev2_upd_I512_I256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v32int16) set_v32int16(int idx, v16int16 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I512_I256(b, 0);
  else
    return __builtin_aiev2_set_I512_I256(b, 1);
}

INTRINSIC(v32int16) concat(v16int16 a0, v16int16 a1) {
  return __builtin_aiev2_concat_I512_I256(a0, a1);
}

#if 0
// Extract 256-bit portion from 512-bit register
INTRINSIC(v8cint16) extract_v8cint16(v16cint16 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I512(a, 0);
  else
    return __builtin_aiev2_ext_I256_I512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v16cint16) insert(v16cint16 a, int idx, v8cint16 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I512_I256(a, b, 0);
  else
    return __builtin_aiev2_upd_I512_I256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v16cint16) set_v16cint16(int idx, v8cint16 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I512_I256(b, 0);
  else
    return __builtin_aiev2_set_I512_I256(b, 1);
}

INTRINSIC(v16cint16) concat(v8cint16 a0, v8cint16 a1) {
  return __builtin_aiev2_concat_I512_I256(a0, a1);
}
#endif

// Extract 256-bit portion from 512-bit register
INTRINSIC(v8uint32) extract_v8uint32(v16uint32 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I512(a, 0);
  else
    return __builtin_aiev2_ext_I256_I512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v16uint32) insert(v16uint32 a, int idx, v8uint32 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I512_I256(a, b, 0);
  else
    return __builtin_aiev2_upd_I512_I256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v16uint32) set_v16uint32(int idx, v8uint32 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I512_I256(b, 0);
  else
    return __builtin_aiev2_set_I512_I256(b, 1);
}

INTRINSIC(v16uint32) concat(v8uint32 a0, v8uint32 a1) {
  return __builtin_aiev2_concat_I512_I256(a0, a1);
}

// Extract 256-bit portion from 512-bit register
INTRINSIC(v8int32) extract_v8int32(v16int32 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I512(a, 0);
  else
    return __builtin_aiev2_ext_I256_I512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v16int32) insert(v16int32 a, int idx, v8int32 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I512_I256(a, b, 0);
  else
    return __builtin_aiev2_upd_I512_I256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v16int32) set_v16int32(int idx, v8int32 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I512_I256(b, 0);
  else
    return __builtin_aiev2_set_I512_I256(b, 1);
}

INTRINSIC(v16int32) concat(v8int32 a0, v8int32 a1) {
  return __builtin_aiev2_concat_I512_I256(a0, a1);
}

#if 0
// Extract 256-bit portion from 512-bit register
INTRINSIC(v4cint32) extract_v4cint32(v8cint32 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I512(a, 0);
  else
    return __builtin_aiev2_ext_I256_I512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v8cint32) insert(v8cint32 a, int idx, v4cint32 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I512_I256(a, b, 0);
  else
    return __builtin_aiev2_upd_I512_I256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v8cint32) set_v8cint32(int idx, v4cint32 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I512_I256(b, 0);
  else
    return __builtin_aiev2_set_I512_I256(b, 1);
}

INTRINSIC(v8cint32) concat(v4cint32 a0, v4cint32 a1) {
  return __builtin_aiev2_concat_I512_I256(a0, a1);
}
#endif
// Extract 256-bit portion from 512-bit register
INTRINSIC(v16bfloat16) extract_v16bfloat16(v32bfloat16 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_bf256_bf512(a, 0);
  else
    return __builtin_aiev2_ext_bf256_bf512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v32bfloat16) insert(v32bfloat16 a, int idx, v16bfloat16 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_bf512_bf256(a, b, 0);
  else
    return __builtin_aiev2_upd_bf512_bf256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v32bfloat16) set_v32bfloat16(int idx, v16bfloat16 b) {
  if (idx == 0)
    return __builtin_aiev2_set_bf512_bf256(b, 0);
  else
    return __builtin_aiev2_set_bf512_bf256(b, 1);
}

INTRINSIC(v32bfloat16) concat(v16bfloat16 a0, v16bfloat16 a1) {
  return __builtin_aiev2_concat_bf512_bf256(a0, a1);
}

// Extract 256-bit portion from 512-bit register
INTRINSIC(v8accfloat) extract_v8accfloat(v16accfloat a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_ACC256_ACC512(a, 0);
  else
    return __builtin_aiev2_ext_ACC256_ACC512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v16accfloat) insert(v16accfloat a, int idx, v8accfloat b) {
  if (idx == 0)
    return __builtin_aiev2_upd_ACC512_ACC256(a, b, 0);
  else
    return __builtin_aiev2_upd_ACC512_ACC256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v16accfloat) set_v16accfloat(int idx, v8accfloat b) {
  if (idx == 0)
    return __builtin_aiev2_set_ACC512_ACC256(b, 0);
  else
    return __builtin_aiev2_set_ACC512_ACC256(b, 1);
}

INTRINSIC(v16accfloat) concat(v8accfloat a0, v8accfloat a1) {
  return __builtin_aiev2_concat_ACC512_ACC256_acc32(a0, a1);
}

// Extract 256-bit portion from 512-bit register
INTRINSIC(v8float) extract_v8float(v16float a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I512(a, 0);
  else
    return __builtin_aiev2_ext_I256_I512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v16float) insert(v16float a, int idx, v8float b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I512_I256(a, b, 0);
  else
    return __builtin_aiev2_upd_I512_I256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v16float) set_v16float(int idx, v8float b) {
  if (idx == 0)
    return __builtin_aiev2_set_I512_I256(b, 0);
  else
    return __builtin_aiev2_set_I512_I256(b, 1);
}

INTRINSIC(v16float) concat(v8float a0, v8float a1) {
  return __builtin_aiev2_concat_I512_I256(a0, a1);
}

// Extract 256-bit portion from 512-bit register
INTRINSIC(v8acc32) extract_v8acc32(v16acc32 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_ACC256_ACC512(a, 0);
  else
    return __builtin_aiev2_ext_ACC256_ACC512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v16acc32) insert(v16acc32 a, int idx, v8acc32 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_ACC512_ACC256(a, b, 0);
  else
    return __builtin_aiev2_upd_ACC512_ACC256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v16acc32) set_v16acc32(int idx, v8acc32 b) {
  if (idx == 0)
    return __builtin_aiev2_set_ACC512_ACC256(b, 0);
  else
    return __builtin_aiev2_set_ACC512_ACC256(b, 1);
}

INTRINSIC(v16acc32) concat(v8acc32 a0, v8acc32 a1) {
  return __builtin_aiev2_concat_ACC512_ACC256_acc32(a0, a1);
}

// Extract 256-bit portion from 512-bit register
INTRINSIC(v4acc64) extract_v4acc64(v8acc64 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_ACC256_ACC512(a, 0);
  else
    return __builtin_aiev2_ext_ACC256_ACC512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v8acc64) insert(v8acc64 a, int idx, v4acc64 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_ACC512_ACC256(a, b, 0);
  else
    return __builtin_aiev2_upd_ACC512_ACC256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v8acc64) set_v8acc64(int idx, v4acc64 b) {
  if (idx == 0)
    return __builtin_aiev2_set_ACC512_ACC256(b, 0);
  else
    return __builtin_aiev2_set_ACC512_ACC256(b, 1);
}

INTRINSIC(v8acc64) concat(v4acc64 a0, v4acc64 a1) {
  return __builtin_aiev2_concat_ACC512_ACC256_acc64(a0, a1);
}

#if 0
// Extract 256-bit portion from 512-bit register
INTRINSIC(v2cacc64) extract_v2cacc64(v4cacc64 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_b_256_512(a, 0);
  else
    return __builtin_aiev2_ext_b_256_512(a, 1);
}

// Insert 256-bit in 512-bit register
INTRINSIC(v4cacc64) insert(v4cacc64 a, int idx, v2cacc64 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_b_512_256(a, b, 0);
  else
    return __builtin_aiev2_upd_b_512_256(a, b, 1);
}

// Set 256-bit portion of 512-bit register
INTRINSIC(v4cacc64) set_v4cacc64(int idx, v2cacc64 b) {
  if (idx == 0)
    return __builtin_aiev2_set_b_512_256(b, 0);
  else
    return __builtin_aiev2_set_b_512_256(b, 1);
}

INTRINSIC(v4cacc64) concat(v2cacc64 a0, v2cacc64 a1) {
  return __builtin_aiev2_concat_bm_am(a0, a1);
}
#endif

// Extract 256-bit portion from 1024-bit register
INTRINSIC(v64uint4) extract_v64uint4(v256uint4 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_I256_I1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_I256_I1024(a, 2);
  else
    return __builtin_aiev2_ext_I256_I1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v256uint4) insert(v256uint4 a, int idx, v64uint4 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_I1024_I256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_I1024_I256(a, b, 2);
  else
    return __builtin_aiev2_upd_I1024_I256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v256uint4) set_v256uint4(int idx, v64uint4 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_I1024_I256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_I1024_I256(b, 2);
  else
    return __builtin_aiev2_set_I1024_I256(b, 3);
}

INTRINSIC(v256uint4)
concat(v64uint4 a0, v64uint4 a1, v64uint4 a2, v64uint4 a3) {
  return __builtin_aiev2_concat_I1024_I256(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v128uint4) extract_v128uint4(v256uint4 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I512_I1024(a, 0);
  else
    return __builtin_aiev2_ext_I512_I1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v256uint4) insert(v256uint4 a, int idx, v128uint4 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I512(a, b, 0);
  else
    return __builtin_aiev2_upd_I1024_I512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v256uint4) set_v256uint4(int idx, v128uint4 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I512(b, 0);
  else
    return __builtin_aiev2_set_I1024_I512(b, 1);
}

INTRINSIC(v256uint4) concat(v128uint4 a0, v128uint4 a1) {
  return __builtin_aiev2_concat_I1024_I512(a0, a1);
}

// Extract 256-bit portion from 1024-bit register
INTRINSIC(v64int4) extract_v64int4(v256int4 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_I256_I1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_I256_I1024(a, 2);
  else
    return __builtin_aiev2_ext_I256_I1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v256int4) insert(v256int4 a, int idx, v64int4 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_I1024_I256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_I1024_I256(a, b, 2);
  else
    return __builtin_aiev2_upd_I1024_I256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v256int4) set_v256int4(int idx, v64int4 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_I1024_I256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_I1024_I256(b, 2);
  else
    return __builtin_aiev2_set_I1024_I256(b, 3);
}

INTRINSIC(v256int4) concat(v64int4 a0, v64int4 a1, v64int4 a2, v64int4 a3) {
  return __builtin_aiev2_concat_I1024_I256(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v128int4) extract_v128int4(v256int4 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I512_I1024(a, 0);
  else
    return __builtin_aiev2_ext_I512_I1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v256int4) insert(v256int4 a, int idx, v128int4 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I512(a, b, 0);
  else
    return __builtin_aiev2_upd_I1024_I512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v256int4) set_v256int4(int idx, v128int4 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I512(b, 0);
  else
    return __builtin_aiev2_set_I1024_I512(b, 1);
}

INTRINSIC(v256int4) concat(v128int4 a0, v128int4 a1) {
  return __builtin_aiev2_concat_I1024_I512(a0, a1);
}

// Extract 256-bit portion from 1024-bit register
INTRINSIC(v32uint8) extract_v32uint8(v128uint8 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_I256_I1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_I256_I1024(a, 2);
  else
    return __builtin_aiev2_ext_I256_I1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v128uint8) insert(v128uint8 a, int idx, v32uint8 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_I1024_I256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_I1024_I256(a, b, 2);
  else
    return __builtin_aiev2_upd_I1024_I256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v128uint8) set_v128uint8(int idx, v32uint8 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_I1024_I256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_I1024_I256(b, 2);
  else
    return __builtin_aiev2_set_I1024_I256(b, 3);
}

INTRINSIC(v128uint8)
concat(v32uint8 a0, v32uint8 a1, v32uint8 a2, v32uint8 a3) {
  return __builtin_aiev2_concat_I1024_I256(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v64uint8) extract_v64uint8(v128uint8 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I512_I1024(a, 0);
  else
    return __builtin_aiev2_ext_I512_I1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v128uint8) insert(v128uint8 a, int idx, v64uint8 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I512(a, b, 0);
  else
    return __builtin_aiev2_upd_I1024_I512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v128uint8) set_v128uint8(int idx, v64uint8 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I512(b, 0);
  else
    return __builtin_aiev2_set_I1024_I512(b, 1);
}

INTRINSIC(v128uint8) concat(v64uint8 a0, v64uint8 a1) {
  return __builtin_aiev2_concat_I1024_I512(a0, a1);
}

// Extract 256-bit portion from 1024-bit register
INTRINSIC(v32int8) extract_v32int8(v128int8 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_I256_I1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_I256_I1024(a, 2);
  else
    return __builtin_aiev2_ext_I256_I1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v128int8) insert(v128int8 a, int idx, v32int8 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_I1024_I256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_I1024_I256(a, b, 2);
  else
    return __builtin_aiev2_upd_I1024_I256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v128int8) set_v128int8(int idx, v32int8 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_I1024_I256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_I1024_I256(b, 2);
  else
    return __builtin_aiev2_set_I1024_I256(b, 3);
}

INTRINSIC(v128int8) concat(v32int8 a0, v32int8 a1, v32int8 a2, v32int8 a3) {
  return __builtin_aiev2_concat_I1024_I256(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v64int8) extract_v64int8(v128int8 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I512_I1024(a, 0);
  else
    return __builtin_aiev2_ext_I512_I1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v128int8) insert(v128int8 a, int idx, v64int8 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I512(a, b, 0);
  else
    return __builtin_aiev2_upd_I1024_I512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v128int8) set_v128int8(int idx, v64int8 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I512(b, 0);
  else
    return __builtin_aiev2_set_I1024_I512(b, 1);
}

INTRINSIC(v128int8) concat(v64int8 a0, v64int8 a1) {
  return __builtin_aiev2_concat_I1024_I512(a0, a1);
}

// Extract 256-bit portion from 1024-bit register
INTRINSIC(v16uint16) extract_v16uint16(v64uint16 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_I256_I1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_I256_I1024(a, 2);
  else
    return __builtin_aiev2_ext_I256_I1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v64uint16) insert(v64uint16 a, int idx, v16uint16 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_I1024_I256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_I1024_I256(a, b, 2);
  else
    return __builtin_aiev2_upd_I1024_I256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v64uint16) set_v64uint16(int idx, v16uint16 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_I1024_I256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_I1024_I256(b, 2);
  else
    return __builtin_aiev2_set_I1024_I256(b, 3);
}

INTRINSIC(v64uint16)
concat(v16uint16 a0, v16uint16 a1, v16uint16 a2, v16uint16 a3) {
  return __builtin_aiev2_concat_I1024_I256(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v32uint16) extract_v32uint16(v64uint16 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I512_I1024(a, 0);
  else
    return __builtin_aiev2_ext_I512_I1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v64uint16) insert(v64uint16 a, int idx, v32uint16 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I512(a, b, 0);
  else
    return __builtin_aiev2_upd_I1024_I512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v64uint16) set_v64uint16(int idx, v32uint16 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I512(b, 0);
  else
    return __builtin_aiev2_set_I1024_I512(b, 1);
}

INTRINSIC(v64uint16) concat(v32uint16 a0, v32uint16 a1) {
  return __builtin_aiev2_concat_I1024_I512(a0, a1);
}

// Extract 256-bit portion from 1024-bit register
INTRINSIC(v16int16) extract_v16int16(v64int16 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_I256_I1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_I256_I1024(a, 2);
  else
    return __builtin_aiev2_ext_I256_I1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v64int16) insert(v64int16 a, int idx, v16int16 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_I1024_I256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_I1024_I256(a, b, 2);
  else
    return __builtin_aiev2_upd_I1024_I256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v64int16) set_v64int16(int idx, v16int16 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_I1024_I256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_I1024_I256(b, 2);
  else
    return __builtin_aiev2_set_I1024_I256(b, 3);
}

INTRINSIC(v64int16) concat(v16int16 a0, v16int16 a1, v16int16 a2, v16int16 a3) {
  return __builtin_aiev2_concat_I1024_I256(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v32int16) extract_v32int16(v64int16 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I512_I1024(a, 0);
  else
    return __builtin_aiev2_ext_I512_I1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v64int16) insert(v64int16 a, int idx, v32int16 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I512(a, b, 0);
  else
    return __builtin_aiev2_upd_I1024_I512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v64int16) set_v64int16(int idx, v32int16 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I512(b, 0);
  else
    return __builtin_aiev2_set_I1024_I512(b, 1);
}

INTRINSIC(v64int16) concat(v32int16 a0, v32int16 a1) {
  return __builtin_aiev2_concat_I1024_I512(a0, a1);
}

#if 0
// Extract 256-bit portion from 1024-bit register
INTRINSIC(v8cint16) extract_v8cint16(v32cint16 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_I256_I1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_I256_I1024(a, 2);
  else
    return __builtin_aiev2_ext_I256_I1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v32cint16) insert(v32cint16 a, int idx, v8cint16 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_I1024_I256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_I1024_I256(a, b, 2);
  else
    return __builtin_aiev2_upd_I1024_I256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v32cint16) set_v32cint16(int idx, v8cint16 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_I1024_I256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_I1024_I256(b, 2);
  else
    return __builtin_aiev2_set_I1024_I256(b, 3);
}

INTRINSIC(v32cint16)
concat(v8cint16 a0, v8cint16 a1, v8cint16 a2, v8cint16 a3) {
  return __builtin_aiev2_concat_I1024_I256(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v16cint16) extract_v16cint16(v32cint16 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I512_I1024(a, 0);
  else
    return __builtin_aiev2_ext_I512_I1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v32cint16) insert(v32cint16 a, int idx, v16cint16 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I512(a, b, 0);
  else
    return __builtin_aiev2_upd_I1024_I512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v32cint16) set_v32cint16(int idx, v16cint16 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I512(b, 0);
  else
    return __builtin_aiev2_set_I1024_I512(b, 1);
}

INTRINSIC(v32cint16) concat(v16cint16 a0, v16cint16 a1) {
  return __builtin_aiev2_concat_I1024_I512(a0, a1);
}
#endif

// Extract 256-bit portion from 1024-bit register
INTRINSIC(v8uint32) extract_v8uint32(v32uint32 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_I256_I1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_I256_I1024(a, 2);
  else
    return __builtin_aiev2_ext_I256_I1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v32uint32) insert(v32uint32 a, int idx, v8uint32 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_I1024_I256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_I1024_I256(a, b, 2);
  else
    return __builtin_aiev2_upd_I1024_I256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v32uint32) set_v32uint32(int idx, v8uint32 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_I1024_I256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_I1024_I256(b, 2);
  else
    return __builtin_aiev2_set_I1024_I256(b, 3);
}

INTRINSIC(v32uint32)
concat(v8uint32 a0, v8uint32 a1, v8uint32 a2, v8uint32 a3) {
  return __builtin_aiev2_concat_I1024_I256(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v16uint32) extract_v16uint32(v32uint32 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I512_I1024(a, 0);
  else
    return __builtin_aiev2_ext_I512_I1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v32uint32) insert(v32uint32 a, int idx, v16uint32 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I512(a, b, 0);
  else
    return __builtin_aiev2_upd_I1024_I512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v32uint32) set_v32uint32(int idx, v16uint32 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I512(b, 0);
  else
    return __builtin_aiev2_set_I1024_I512(b, 1);
}

INTRINSIC(v32uint32) concat(v16uint32 a0, v16uint32 a1) {
  return __builtin_aiev2_concat_I1024_I512(a0, a1);
}

// Extract 256-bit portion from 1024-bit register
INTRINSIC(v8int32) extract_v8int32(v32int32 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_I256_I1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_I256_I1024(a, 2);
  else
    return __builtin_aiev2_ext_I256_I1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v32int32) insert(v32int32 a, int idx, v8int32 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_I1024_I256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_I1024_I256(a, b, 2);
  else
    return __builtin_aiev2_upd_I1024_I256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v32int32) set_v32int32(int idx, v8int32 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_I1024_I256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_I1024_I256(b, 2);
  else
    return __builtin_aiev2_set_I1024_I256(b, 3);
}

INTRINSIC(v32int32) concat(v8int32 a0, v8int32 a1, v8int32 a2, v8int32 a3) {
  return __builtin_aiev2_concat_I1024_I256(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v16int32) extract_v16int32(v32int32 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I512_I1024(a, 0);
  else
    return __builtin_aiev2_ext_I512_I1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v32int32) insert(v32int32 a, int idx, v16int32 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I512(a, b, 0);
  else
    return __builtin_aiev2_upd_I1024_I512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v32int32) set_v32int32(int idx, v16int32 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I512(b, 0);
  else
    return __builtin_aiev2_set_I1024_I512(b, 1);
}

INTRINSIC(v32int32) concat(v16int32 a0, v16int32 a1) {
  return __builtin_aiev2_concat_I1024_I512(a0, a1);
}

#if 0
// Extract 256-bit portion from 1024-bit register
INTRINSIC(v4cint32) extract_v4cint32(v16cint32 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_I256_I1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_I256_I1024(a, 2);
  else
    return __builtin_aiev2_ext_I256_I1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v16cint32) insert(v16cint32 a, int idx, v4cint32 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_I1024_I256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_I1024_I256(a, b, 2);
  else
    return __builtin_aiev2_upd_I1024_I256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v16cint32) set_v16cint32(int idx, v4cint32 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_I1024_I256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_I1024_I256(b, 2);
  else
    return __builtin_aiev2_set_I1024_I256(b, 3);
}

INTRINSIC(v16cint32)
concat(v4cint32 a0, v4cint32 a1, v4cint32 a2, v4cint32 a3) {
  return __builtin_aiev2_concat_I1024_I256(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v8cint32) extract_v8cint32(v16cint32 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I512_I1024(a, 0);
  else
    return __builtin_aiev2_ext_I512_I1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v16cint32) insert(v16cint32 a, int idx, v8cint32 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I512(a, b, 0);
  else
    return __builtin_aiev2_upd_I1024_I512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v16cint32) set_v16cint32(int idx, v8cint32 b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I512(b, 0);
  else
    return __builtin_aiev2_set_I1024_I512(b, 1);
}

INTRINSIC(v16cint32) concat(v8cint32 a0, v8cint32 a1) {
  return __builtin_aiev2_concat_I1024_I512(a0, a1);
}
#endif

// Extract 256-bit portion from 1024-bit register
INTRINSIC(v16bfloat16) extract_v16bfloat16(v64bfloat16 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_bf256_bf1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_bf256_bf1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_bf256_bf1024(a, 2);
  else
    return __builtin_aiev2_ext_bf256_bf1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v64bfloat16) insert(v64bfloat16 a, int idx, v16bfloat16 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_bf1024_bf256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_bf1024_bf256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_bf1024_bf256(a, b, 2);
  else
    return __builtin_aiev2_upd_bf1024_bf256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v64bfloat16) set_v64bfloat16(int idx, v16bfloat16 b) {
  if (idx == 0)
    return __builtin_aiev2_set_bf1024_bf256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_bf1024_bf256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_bf1024_bf256(b, 2);
  else
    return __builtin_aiev2_set_bf1024_bf256(b, 3);
}

INTRINSIC(v64bfloat16)
concat(v16bfloat16 a0, v16bfloat16 a1, v16bfloat16 a2, v16bfloat16 a3) {
  return __builtin_aiev2_concat_bf1024_bf256(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v32bfloat16) extract_v32bfloat16(v64bfloat16 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_bf512_bf1024(a, 0);
  else
    return __builtin_aiev2_ext_bf512_bf1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v64bfloat16) insert(v64bfloat16 a, int idx, v32bfloat16 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_bf1024_bf512(a, b, 0);
  else
    return __builtin_aiev2_upd_bf1024_bf512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v64bfloat16) set_v64bfloat16(int idx, v32bfloat16 b) {
  if (idx == 0)
    return __builtin_aiev2_set_bf1024_bf512(b, 0);
  else
    return __builtin_aiev2_set_bf1024_bf512(b, 1);
}

INTRINSIC(v64bfloat16) concat(v32bfloat16 a0, v32bfloat16 a1) {
  return __builtin_aiev2_concat_bf1024_bf512(a0, a1);
}

// Extract 256-bit portion from 1024-bit register
INTRINSIC(v8accfloat) extract_v8accfloat(v32accfloat a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_ACC256_ACC1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_ACC256_ACC1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_ACC256_ACC1024(a, 2);
  else
    return __builtin_aiev2_ext_ACC256_ACC1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v32accfloat) insert(v32accfloat a, int idx, v8accfloat b) {
  if (idx == 0)
    return __builtin_aiev2_upd_ACC1024_ACC256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_ACC1024_ACC256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_ACC1024_ACC256(a, b, 2);
  else
    return __builtin_aiev2_upd_ACC1024_ACC256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v32accfloat) set_v32accfloat(int idx, v8accfloat b) {
  if (idx == 0)
    return __builtin_aiev2_set_ACC1024_ACC256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_ACC1024_ACC256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_ACC1024_ACC256(b, 2);
  else
    return __builtin_aiev2_set_ACC1024_ACC256(b, 3);
}

INTRINSIC(v32accfloat)
concat(v8accfloat a0, v8accfloat a1, v8accfloat a2, v8accfloat a3) {
  return __builtin_aiev2_concat_ACC1024_ACC256_acc32(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v16accfloat) extract_v16accfloat(v32accfloat a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_ACC512_ACC1024(a, 0);
  else
    return __builtin_aiev2_ext_ACC512_ACC1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v32accfloat) insert(v32accfloat a, int idx, v16accfloat b) {
  if (idx == 0)
    return __builtin_aiev2_upd_ACC1024_ACC512(a, b, 0);
  else
    return __builtin_aiev2_upd_ACC1024_ACC512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v32accfloat) set_v32accfloat(int idx, v16accfloat b) {
  if (idx == 0)
    return __builtin_aiev2_set_ACC1024_ACC512(b, 0);
  else
    return __builtin_aiev2_set_ACC1024_ACC512(b, 1);
}

INTRINSIC(v32accfloat) concat(v16accfloat a0, v16accfloat a1) {
  return __builtin_aiev2_concat_ACC1024_ACC512_acc32(a0, a1);
}

// Extract 256-bit portion from 1024-bit register
INTRINSIC(v8float) extract_v8float(v32float a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I256_I1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_I256_I1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_I256_I1024(a, 2);
  else
    return __builtin_aiev2_ext_I256_I1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v32float) insert(v32float a, int idx, v8float b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_I1024_I256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_I1024_I256(a, b, 2);
  else
    return __builtin_aiev2_upd_I1024_I256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v32float) set_v32float(int idx, v8float b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_I1024_I256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_I1024_I256(b, 2);
  else
    return __builtin_aiev2_set_I1024_I256(b, 3);
}

INTRINSIC(v32float) concat(v8float a0, v8float a1, v8float a2, v8float a3) {
  return __builtin_aiev2_concat_I1024_I256(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v16float) extract_v16float(v32float a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_I512_I1024(a, 0);
  else
    return __builtin_aiev2_ext_I512_I1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v32float) insert(v32float a, int idx, v16float b) {
  if (idx == 0)
    return __builtin_aiev2_upd_I1024_I512(a, b, 0);
  else
    return __builtin_aiev2_upd_I1024_I512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v32float) set_v32float(int idx, v16float b) {
  if (idx == 0)
    return __builtin_aiev2_set_I1024_I512(b, 0);
  else
    return __builtin_aiev2_set_I1024_I512(b, 1);
}

INTRINSIC(v32float) concat(v16float a0, v16float a1) {
  return __builtin_aiev2_concat_I1024_I512(a0, a1);
}

// Extract 256-bit portion from 1024-bit register
INTRINSIC(v8acc32) extract_v8acc32(v32acc32 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_ACC256_ACC1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_ACC256_ACC1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_ACC256_ACC1024(a, 2);
  else
    return __builtin_aiev2_ext_ACC256_ACC1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v32acc32) insert(v32acc32 a, int idx, v8acc32 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_ACC1024_ACC256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_ACC1024_ACC256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_ACC1024_ACC256(a, b, 2);
  else
    return __builtin_aiev2_upd_ACC1024_ACC256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v32acc32) set_v32acc32(int idx, v8acc32 b) {
  if (idx == 0)
    return __builtin_aiev2_set_ACC1024_ACC256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_ACC1024_ACC256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_ACC1024_ACC256(b, 2);
  else
    return __builtin_aiev2_set_ACC1024_ACC256(b, 3);
}

INTRINSIC(v32acc32) concat(v8acc32 a0, v8acc32 a1, v8acc32 a2, v8acc32 a3) {
  return __builtin_aiev2_concat_ACC1024_ACC256_acc32(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v16acc32) extract_v16acc32(v32acc32 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_ACC512_ACC1024(a, 0);
  else
    return __builtin_aiev2_ext_ACC512_ACC1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v32acc32) insert(v32acc32 a, int idx, v16acc32 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_ACC1024_ACC512(a, b, 0);
  else
    return __builtin_aiev2_upd_ACC1024_ACC512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v32acc32) set_v32acc32(int idx, v16acc32 b) {
  if (idx == 0)
    return __builtin_aiev2_set_ACC1024_ACC512(b, 0);
  else
    return __builtin_aiev2_set_ACC1024_ACC512(b, 1);
}

INTRINSIC(v32acc32) concat(v16acc32 a0, v16acc32 a1) {
  return __builtin_aiev2_concat_ACC1024_ACC512_acc32(a0, a1);
}

// Extract 256-bit portion from 1024-bit register
INTRINSIC(v4acc64) extract_v4acc64(v16acc64 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_ACC256_ACC1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_ACC256_ACC1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_ACC256_ACC1024(a, 2);
  else
    return __builtin_aiev2_ext_ACC256_ACC1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v16acc64) insert(v16acc64 a, int idx, v4acc64 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_ACC1024_ACC256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_ACC1024_ACC256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_ACC1024_ACC256(a, b, 2);
  else
    return __builtin_aiev2_upd_ACC1024_ACC256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v16acc64) set_v16acc64(int idx, v4acc64 b) {
  if (idx == 0)
    return __builtin_aiev2_set_ACC1024_ACC256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_ACC1024_ACC256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_ACC1024_ACC256(b, 2);
  else
    return __builtin_aiev2_set_ACC1024_ACC256(b, 3);
}

INTRINSIC(v16acc64) concat(v4acc64 a0, v4acc64 a1, v4acc64 a2, v4acc64 a3) {
  return __builtin_aiev2_concat_ACC1024_ACC256_acc64(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v8acc64) extract_v8acc64(v16acc64 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_ACC512_ACC1024(a, 0);
  else
    return __builtin_aiev2_ext_ACC512_ACC1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v16acc64) insert(v16acc64 a, int idx, v8acc64 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_ACC1024_ACC512(a, b, 0);
  else
    return __builtin_aiev2_upd_ACC1024_ACC512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v16acc64) set_v16acc64(int idx, v8acc64 b) {
  if (idx == 0)
    return __builtin_aiev2_set_ACC1024_ACC512(b, 0);
  else
    return __builtin_aiev2_set_ACC1024_ACC512(b, 1);
}

INTRINSIC(v16acc64) concat(v8acc64 a0, v8acc64 a1) {
  return __builtin_aiev2_concat_ACC1024_ACC512_acc64(a0, a1);
}

#if 0
// Extract 256-bit portion from 1024-bit register
INTRINSIC(v2cacc64) extract_v2cacc64(v8cacc64 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_c_256_1024(a, 0);
  else if (idx == 1)
    return __builtin_aiev2_ext_c_256_1024(a, 1);
  else if (idx == 2)
    return __builtin_aiev2_ext_c_256_1024(a, 2);
  else
    return __builtin_aiev2_ext_c_256_1024(a, 3);
}

// Insert 256-bit in 1024-bit register
INTRINSIC(v8cacc64) insert(v8cacc64 a, int idx, v2cacc64 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_c_1024_256(a, b, 0);
  else if (idx == 1)
    return __builtin_aiev2_upd_c_1024_256(a, b, 1);
  else if (idx == 2)
    return __builtin_aiev2_upd_c_1024_256(a, b, 2);
  else
    return __builtin_aiev2_upd_c_1024_256(a, b, 3);
}

// Set 256-bit portion of 1024-bit register
INTRINSIC(v8cacc64) set_v8cacc64(int idx, v2cacc64 b) {
  if (idx == 0)
    return __builtin_aiev2_set_c_1024_256(b, 0);
  else if (idx == 1)
    return __builtin_aiev2_set_c_1024_256(b, 1);
  else if (idx == 2)
    return __builtin_aiev2_set_c_1024_256(b, 2);
  else
    return __builtin_aiev2_set_c_1024_256(b, 3);
}

INTRINSIC(v8cacc64) concat(v2cacc64 a0, v2cacc64 a1, v2cacc64 a2, v2cacc64 a3) {
  return __builtin_aiev2_concat_cm_am(a0, a1, a2, a3);
}

// Extract 512-bit portion from 1024-bit register
INTRINSIC(v4cacc64) extract_v4cacc64(v8cacc64 a, int idx) {
  if (idx == 0)
    return __builtin_aiev2_ext_c_512_1024(a, 0);
  else
    return __builtin_aiev2_ext_c_512_1024(a, 1);
}

// Insert 512-bit in 1024-bit register
INTRINSIC(v8cacc64) insert(v8cacc64 a, int idx, v4cacc64 b) {
  if (idx == 0)
    return __builtin_aiev2_upd_c_1024_512(a, b, 0);
  else
    return __builtin_aiev2_upd_c_1024_512(a, b, 1);
}

// Set 512-bit portion of 1024-bit register
INTRINSIC(v8cacc64) set_v8cacc64(int idx, v4cacc64 b) {
  if (idx == 0)
    return __builtin_aiev2_set_c_1024_512(b, 0);
  else
    return __builtin_aiev2_set_c_1024_512(b, 1);
}

INTRINSIC(v8cacc64) concat(v4cacc64 a0, v4cacc64 a1) {
  return __builtin_aiev2_concat_cm_bm(a0, a1);
}
#endif

constexpr void handle_assertion(bool value, int max) {
  if (value) {
    // FIXME : Find a way to handle parameterize __error__
    // AND use message : idx must be in range [0, max]
    extern __attribute__((__error__("idx is out of allowed range"))) void
    aie_static_assert(void);
    aie_static_assert();
  }
}

#define __EXT_128B_CHECK(idx, max)                                             \
  handle_assertion((__builtin_constant_p(idx) && (idx < 0 || idx > max)), max);

// Extract 128-bit portion from 256-bit register
INTRINSIC(v32uint4) extract_v32uint4(v128uint4 a, int idx) {
  __EXT_128B_CHECK(idx, 3)
  v128uint4 dest =
      __builtin_aiev2_vshift_I512_I512(a, undef_v128uint4(), 0x0, 16 * idx);
  // extract 128-bit from 512-bit dest.
  return __builtin_aiev2_extract_I128_I512(dest);
}

INTRINSIC(v32int4) extract_v32int4(v128int4 a, int idx) {
  __EXT_128B_CHECK(idx, 3)
  v128int4 dest =
      __builtin_aiev2_vshift_I512_I512(a, undef_v128int4(), 0x0, 16 * idx);
  // extract 128-bit from 512-bit dest.
  return __builtin_aiev2_extract_I128_I512(dest);
}

INTRINSIC(v16uint8) extract_v16uint8(v64uint8 a, int idx) {
  __EXT_128B_CHECK(idx, 3)
  v64uint8 dest =
      __builtin_aiev2_vshift_I512_I512(a, undef_v64uint8(), 0x0, 16 * idx);
  // extract 128-bit from 512-bit dest.
  return __builtin_aiev2_extract_I128_I512(dest);
}

INTRINSIC(v16int8) extract_v16int8(v64int8 a, int idx) {
  __EXT_128B_CHECK(idx, 3)
  v64int8 dest =
      __builtin_aiev2_vshift_I512_I512(a, undef_v64int8(), 0x0, 16 * idx);
  // extract 128-bit from 512-bit dest.
  return __builtin_aiev2_extract_I128_I512(dest);
}

INTRINSIC(v8uint16) extract_v8uint16(v32uint16 a, int idx) {
  __EXT_128B_CHECK(idx, 3)
  v32uint16 dest =
      __builtin_aiev2_vshift_I512_I512(a, undef_v32uint16(), 0x0, 16 * idx);
  // extract 128-bit from 512-bit dest.
  return __builtin_aiev2_extract_I128_I512(dest);
}

INTRINSIC(v8int16) extract_v8int16(v32int16 a, int idx) {
  __EXT_128B_CHECK(idx, 3)
  v32int16 dest =
      __builtin_aiev2_vshift_I512_I512(a, undef_v32int16(), 0x0, 16 * idx);
  // extract 128-bit from 512-bit dest.
  return __builtin_aiev2_extract_I128_I512(dest);
}

#if 0
INTRINSIC(v4cint16) extract_v4cint16(v16cint16 a, int idx) {
  __EXT_128B_CHECK(idx, 3)
  v16cint16 dest =  __builtin_aiev2_vshift_I512_I512(a, undef_v16cint16(), 0x0, 16 * idx);
  // extract 128-bit from 512-bit dest.
  return __builtin_aiev2_extract_I128_I512(dest);
}
#endif

INTRINSIC(v4uint32) extract_v4uint32(v16uint32 a, int idx) {
  __EXT_128B_CHECK(idx, 3)
  v16uint32 dest =
      __builtin_aiev2_vshift_I512_I512(a, undef_v16uint32(), 0x0, 16 * idx);
  // extract 128-bit from 512-bit dest.
  return __builtin_aiev2_extract_I128_I512(dest);
}

INTRINSIC(v4int32) extract_v4int32(v16int32 a, int idx) {
  __EXT_128B_CHECK(idx, 3)
  v16int32 dest =
      __builtin_aiev2_vshift_I512_I512(a, undef_v16int32(), 0x0, 16 * idx);
  // extract 128-bit from 512-bit dest.
  return __builtin_aiev2_extract_I128_I512(dest);
}

#if 0
INTRINSIC(v2cint32) extract_v2cint32(v8cint32 a, int idx) {
  __EXT_128B_CHECK(idx, 3)
  v8cint32 dest =  __builtin_aiev2_vshift_I512_I512(a, undef_v8cint32(),
                                                    0x0, 16 * idx);
  // extract 128-bit from 512-bit dest.
  return __builtin_aiev2_extract_I128_I512(dest);
}
#endif

INTRINSIC(v4float) extract_v4float(v16float a, int idx) {
  __EXT_128B_CHECK(idx, 3)
  v16float dest =
      __builtin_aiev2_vshift_I512_I512(a, undef_v16float(), 0x0, 16 * idx);
  // extract 128-bit from 512-bit dest.
  return __builtin_aiev2_extract_I128_I512(dest);
}

INTRINSIC(v8bfloat16) extract_v8bfloat16(v32bfloat16 a, int idx) {
  __EXT_128B_CHECK(idx, 3)
  v32bfloat16 dest =
      __builtin_aiev2_vshift_I512_I512(a, undef_v32bfloat16(), 0x0, 16 * idx);
  // extract 128-bit from 512-bit dest.
  return __builtin_aiev2_extract_I128_I512(dest);
}

// Set 128-bit portion of 256-bit register
INTRINSIC(v128uint4) set_v128uint4(int idx, v32uint4 a) {
  __EXT_128B_CHECK(idx, 3)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_set_I512_I128(a);
  else {
    // set 512-bit source from 128-bit src
    v128uint4 src = __builtin_aiev2_set_I512_I128(a);
    return __builtin_aiev2_vshift_I512_I512(undef_v128uint4(), src, 0x0,
                                            (4 - idx) * 16);
  }
}

INTRINSIC(v128int4) set_v128int4(int idx, v32int4 a) {
  __EXT_128B_CHECK(idx, 3)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_set_I512_I128(a);
  else {
    // set 512-bit source from 128-bit src
    v128int4 src = __builtin_aiev2_set_I512_I128(a);
    return __builtin_aiev2_vshift_I512_I512(undef_v128int4(), src, 0x0,
                                            (4 - idx) * 16);
  }
}

INTRINSIC(v64uint8) set_v64uint8(int idx, v16uint8 a) {
  __EXT_128B_CHECK(idx, 3)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_set_I512_I128(a);
  else {
    // set 512-bit source from 128-bit src
    v64uint8 src = __builtin_aiev2_set_I512_I128(a);
    return __builtin_aiev2_vshift_I512_I512(undef_v64uint8(), src, 0x0,
                                            (4 - idx) * 16);
  }
}

INTRINSIC(v64int8) set_v64int8(int idx, v16int8 a) {
  __EXT_128B_CHECK(idx, 3)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_set_I512_I128(a);
  else {
    // set 512-bit source from 128-bit src
    v64int8 src = __builtin_aiev2_set_I512_I128(a);
    return __builtin_aiev2_vshift_I512_I512(undef_v64int8(), src, 0x0,
                                            (4 - idx) * 16);
  }
}

INTRINSIC(v32uint16) set_v32uint16(int idx, v8uint16 a) {
  __EXT_128B_CHECK(idx, 3)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_set_I512_I128(a);
  else {
    // set 512-bit source from 128-bit src
    v32uint16 src = __builtin_aiev2_set_I512_I128(a);
    return __builtin_aiev2_vshift_I512_I512(undef_v32uint16(), src, 0x0,
                                            (4 - idx) * 16);
  }
}

INTRINSIC(v32int16) set_v32int16(int idx, v8int16 a) {
  __EXT_128B_CHECK(idx, 3)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_set_I512_I128(a);
  else {
    // set 512-bit source from 128-bit src
    v32int16 src = __builtin_aiev2_set_I512_I128(a);
    return __builtin_aiev2_vshift_I512_I512(undef_v32int16(), src, 0x0,
                                            (4 - idx) * 16);
  }
}

#if 0
INTRINSIC(v16cint16) set_v16cint16(int idx, v4cint16 a) {
  __EXT_128B_CHECK(idx, 3)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_set_I512_I128(a);
  else {
    // set 512-bit source from 128-bit src
    v16cint16 src =  __builtin_aiev2_set_I512_I128(a);
    return __builtin_aiev2_vshift_I512_I512(undef_v16cint16(), src, 0x0, (4 - idx) * 16);
  }
}
#endif

INTRINSIC(v16uint32) set_v16uint32(int idx, v4uint32 a) {
  __EXT_128B_CHECK(idx, 3)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_set_I512_I128(a);
  else {
    // set 512-bit source from 128-bit src
    v16uint32 src = __builtin_aiev2_set_I512_I128(a);
    return __builtin_aiev2_vshift_I512_I512(undef_v16uint32(), src, 0x0,
                                            (4 - idx) * 16);
  }
}

INTRINSIC(v16int32) set_v16int32(int idx, v4int32 a) {
  __EXT_128B_CHECK(idx, 3)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_set_I512_I128(a);
  else {
    // set 512-bit source from 128-bit src
    v16int32 src = __builtin_aiev2_set_I512_I128(a);
    return __builtin_aiev2_vshift_I512_I512(undef_v16int32(), src, 0x0,
                                            (4 - idx) * 16);
  }
}

INTRINSIC(v32bfloat16) set_v32bfloat16(int idx, v8bfloat16 a) {
  __EXT_128B_CHECK(idx, 3)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_set_I512_I128(a);
  else {
    // set 512-bit source from 128-bit src
    v32bfloat16 src = __builtin_aiev2_set_I512_I128(a);
    return __builtin_aiev2_vshift_I512_I512(undef_v32bfloat16(), src, 0x0,
                                            (4 - idx) * 16);
  }
}

#if 0
INTRINSIC(v8cint32) set_v8cint32(int idx, v2cint32 a) {
  __EXT_128B_CHECK(idx, 3)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_set_I512_I128(a);
  else {
    // set 512-bit source from 128-bit src
    v8cint32 src =  __builtin_aiev2_set_I512_I128(a);
    return __builtin_aiev2_vshift_I512_I512(undef_v8cint32(), src, 0x0,
                                            (4 - idx) * 16);
  }
}
#endif

INTRINSIC(v16float) set_v16float(int idx, v4float a) {
  __EXT_128B_CHECK(idx, 3)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_set_I512_I128(a);
  else {
    // set 512-bit source from 128-bit src
    v16float src = __builtin_aiev2_set_I512_I128(a);
    return __builtin_aiev2_vshift_I512_I512(undef_v16float(), src, 0x0,
                                            (4 - idx) * 16);
  }
}

// Extract 128-bit portion from 512-bit register
INTRINSIC(v32uint4) extract_v32uint4(v64uint4 a, int idx) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v32uint4(set_v128uint4(0, a), idx);
}

// Extract 128-bit portion from 512-bit register
INTRINSIC(v32int4) extract_v32int4(v64int4 a, int idx) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v32int4(set_v128int4(0, a), idx);
}

// Extract 128-bit portion from 512-bit register
INTRINSIC(v16uint8) extract_v16uint8(v32uint8 a, int idx) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v16uint8(set_v64uint8(0, a), idx);
}

// Extract 128-bit portion from 512-bit register
INTRINSIC(v16int8) extract_v16int8(v32int8 a, int idx) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v16int8(set_v64int8(0, a), idx);
}

// Extract 128-bit portion from 512-bit register
INTRINSIC(v8uint16) extract_v8uint16(v16uint16 a, int idx) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v8uint16(set_v32uint16(0, a), idx);
}

// Extract 128-bit portion from 512-bit register
INTRINSIC(v8int16) extract_v8int16(v16int16 a, int idx) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v8int16(set_v32int16(0, a), idx);
}

#if 0
// Extract 128-bit portion from 512-bit register
INTRINSIC(v4cint16) extract_v4cint16(v8cint16 a, int idx) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v4cint16(set_v16cint16(0, a), idx);
}
#endif

// Extract 128-bit portion from 512-bit register
INTRINSIC(v4uint32) extract_v4uint32(v8uint32 a, int idx) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v4uint32(set_v16uint32(0, a), idx);
}

// Extract 128-bit portion from 512-bit register
INTRINSIC(v4int32) extract_v4int32(v8int32 a, int idx) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v4int32(set_v16int32(0, a), idx);
}

// Extract 128-bit portion from 512-bit register
INTRINSIC(v8bfloat16) extract_v8bfloat16(v16bfloat16 a, int idx) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v8bfloat16(set_v32bfloat16(0, a), idx);
}

#if 0
// Extract 128-bit portion from 512-bit register
INTRINSIC(v2cint32) extract_v2cint32(v4cint32 a, int idx) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v2cint32(set_v8cint32(0, a), idx);
}

// Extract 128-bit portion from 512-bit register
INTRINSIC(v4float) extract_v4float(v8float a, int idx) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v4float(set_v16float(0, a), idx);
}
#endif

// Set 128-bit portion of 512-bit register
INTRINSIC(v64uint4) set_v64uint4(int idx, v32uint4 a) {
  __EXT_128B_CHECK(idx, 1)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_get_I256_I128(a);
  else
    return extract_v64uint4(set_v128uint4(idx, a), 0);
}

// Set 128-bit portion of 512-bit register
INTRINSIC(v64int4) set_v64int4(int idx, v32int4 a) {
  __EXT_128B_CHECK(idx, 1)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_get_I256_I128(a);
  else
    return extract_v64int4(set_v128int4(idx, a), 0);
}

// Set 128-bit portion of 512-bit register
INTRINSIC(v32uint8) set_v32uint8(int idx, v16uint8 a) {
  __EXT_128B_CHECK(idx, 1)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_get_I256_I128(a);
  else
    return extract_v32uint8(set_v64uint8(idx, a), 0);
}

// Set 128-bit portion of 512-bit register
INTRINSIC(v32int8) set_v32int8(int idx, v16int8 a) {
  __EXT_128B_CHECK(idx, 1)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_get_I256_I128(a);
  else
    return extract_v32int8(set_v64int8(idx, a), 0);
}

// Set 128-bit portion of 512-bit register
INTRINSIC(v16uint16) set_v16uint16(int idx, v8uint16 a) {
  __EXT_128B_CHECK(idx, 1)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_get_I256_I128(a);
  else
    return extract_v16uint16(set_v32uint16(idx, a), 0);
}

// Set 128-bit portion of 512-bit register
INTRINSIC(v16int16) set_v16int16(int idx, v8int16 a) {
  __EXT_128B_CHECK(idx, 1)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_get_I256_I128(a);
  else
    return extract_v16int16(set_v32int16(idx, a), 0);
}

#if 0
// Set 128-bit portion of 512-bit register
INTRINSIC(v8cint16) set_v8cint16(int idx, v4cint16 a) {
  __EXT_128B_CHECK(idx, 1)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_get_I256_I128(a);
  else
    return extract_v8cint16(set_v16cint16(idx, a), 0);
}
#endif

// Set 128-bit portion of 512-bit register
INTRINSIC(v8uint32) set_v8uint32(int idx, v4uint32 a) {
  __EXT_128B_CHECK(idx, 1)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_get_I256_I128(a);
  else
    return extract_v8uint32(set_v16uint32(idx, a), 0);
}

// Set 128-bit portion of 512-bit register
INTRINSIC(v8int32) set_v8int32(int idx, v4int32 a) {
  __EXT_128B_CHECK(idx, 1)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_get_I256_I128(a);
  else
    return extract_v8int32(set_v16int32(idx, a), 0);
}

// Set 128-bit portion of 512-bit register
INTRINSIC(v16bfloat16) set_v16bfloat16(int idx, v8bfloat16 a) {
  return set_v16int16(idx, a);
}

#if 0
// Set 128-bit portion of 512-bit register
INTRINSIC(v4cint32) set_v4cint32(int idx, v2cint32 a) {
  __EXT_128B_CHECK(idx, 1)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_get_I256_I128(a);
  else
    return extract_v4cint32(set_v8cint32(idx, a), 0);
}

// Set 128-bit portion of 512-bit register
INTRINSIC(v8float) set_v8float(int idx, v4float a) {
  __EXT_128B_CHECK(idx, 1)
  if (__builtin_constant_p(idx) && (idx == 0))
    return __builtin_aiev2_get_I256_I128(a);
  else
    return extract_v8float(set_v16float(idx, a), 0);
}
#endif

// Insert 128-bit in 512-bit register
INTRINSIC(v128uint4) insert(v128uint4 v, int idx, v32uint4 b) {
  __EXT_128B_CHECK(idx, 3)
  v16int32 tmp = (v16int32)set_v128uint4(idx, b);

  const unsigned mask_elems = 4;
  const unsigned mask_base = (1u << mask_elems) - 1u;
  const unsigned mask = mask_base << (mask_elems * idx);

  return (v128uint4)__builtin_aiev2_vsel32(v, tmp, mask);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v128int4) insert(v128int4 v, int idx, v32int4 b) {
  __EXT_128B_CHECK(idx, 3)
  v16int32 tmp = (v16int32)set_v128int4(idx, b);

  const unsigned mask_elems = 4;
  const unsigned mask_base = (1u << mask_elems) - 1u;
  const unsigned mask = mask_base << (mask_elems * idx);

  return (v128int4)__builtin_aiev2_vsel32(v, tmp, mask);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v64uint8) insert(v64uint8 v, int idx, v16uint8 b) {
  __EXT_128B_CHECK(idx, 3)
  v16int32 tmp = (v16int32)set_v64uint8(idx, b);

  const unsigned mask_elems = 4;
  const unsigned mask_base = (1u << mask_elems) - 1u;
  const unsigned mask = mask_base << (mask_elems * idx);

  return (v64uint8)__builtin_aiev2_vsel32(v, tmp, mask);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v64int8) insert(v64int8 v, int idx, v16int8 b) {
  __EXT_128B_CHECK(idx, 3)
  v16int32 tmp = (v16int32)set_v64int8(idx, b);

  const unsigned mask_elems = 4;
  const unsigned mask_base = (1u << mask_elems) - 1u;
  const unsigned mask = mask_base << (mask_elems * idx);

  return (v64int8)__builtin_aiev2_vsel32(v, tmp, mask);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v32uint16) insert(v32uint16 v, int idx, v8uint16 b) {
  __EXT_128B_CHECK(idx, 3)
  v16int32 tmp = (v16int32)set_v32uint16(idx, b);

  const unsigned mask_elems = 4;
  const unsigned mask_base = (1u << mask_elems) - 1u;
  const unsigned mask = mask_base << (mask_elems * idx);

  return (v32uint16)__builtin_aiev2_vsel32(v, tmp, mask);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v32int16) insert(v32int16 v, int idx, v8int16 b) {
  __EXT_128B_CHECK(idx, 3)
  v16int32 tmp = (v16int32)set_v32int16(idx, b);

  const unsigned mask_elems = 4;
  const unsigned mask_base = (1u << mask_elems) - 1u;
  const unsigned mask = mask_base << (mask_elems * idx);

  return (v32int16)__builtin_aiev2_vsel32(v, tmp, mask);
}

#if 0
// Insert 128-bit in 512-bit register
INTRINSIC(v16cint16) insert(v16cint16 v, int idx, v4cint16 b) {
  __EXT_128B_CHECK(idx, 3)
  v16int32 tmp = (v16int32)set_v16cint16(idx, b);

  const unsigned mask_elems = 4;
  const unsigned mask_base = (1u << mask_elems) - 1u;
  const unsigned mask = mask_base << (mask_elems * idx);

  return (v16cint16)__builtin_aiev2_vsel32(v, tmp, mask);
}
#endif

// Insert 128-bit in 512-bit register
INTRINSIC(v16uint32) insert(v16uint32 v, int idx, v4uint32 b) {
  __EXT_128B_CHECK(idx, 3)
  v16int32 tmp = (v16int32)set_v16uint32(idx, b);

  const unsigned mask_elems = 4;
  const unsigned mask_base = (1u << mask_elems) - 1u;
  const unsigned mask = mask_base << (mask_elems * idx);

  return (v16uint32)__builtin_aiev2_vsel32(v, tmp, mask);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v16int32) insert(v16int32 v, int idx, v4int32 b) {
  __EXT_128B_CHECK(idx, 3)
  v16int32 tmp = (v16int32)set_v16int32(idx, b);

  const unsigned mask_elems = 4;
  const unsigned mask_base = (1u << mask_elems) - 1u;
  const unsigned mask = mask_base << (mask_elems * idx);

  return (v16int32)__builtin_aiev2_vsel32(v, tmp, mask);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v32bfloat16) insert(v32bfloat16 v, int idx, v8bfloat16 b) {
  __EXT_128B_CHECK(idx, 3)
  v16int32 tmp = (v16int32)set_v32bfloat16(idx, b);

  const unsigned mask_elems = 4;
  const unsigned mask_base = (1u << mask_elems) - 1u;
  const unsigned mask = mask_base << (mask_elems * idx);

  return (v32bfloat16)__builtin_aiev2_vsel32(v, tmp, mask);
}

#if 0
// Insert 128-bit in 512-bit register
INTRINSIC(v8cint32) insert(v8cint32 v, int idx, v2cint32 b) {
  __EXT_128B_CHECK(idx, 3)
  v16int32 tmp = (v16int32)set_v8cint32(idx, b);

  const unsigned mask_elems = 4;
  const unsigned mask_base = (1u << mask_elems) - 1u;
  const unsigned mask = mask_base << (mask_elems * idx);

  return (v8cint32)__builtin_aiev2_vsel32(v, tmp, mask);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v16float) insert(v16float v, int idx, v4float b) {
  __EXT_128B_CHECK(idx, 3)
  v16int32 tmp = (v16int32)set_v16float(idx, b);

  const unsigned mask_elems = 4;
  const unsigned mask_base = (1u << mask_elems) - 1u;
  const unsigned mask = mask_base << (mask_elems * idx);

  return (v16float)__builtin_aiev2_vsel32(v, tmp, mask);
}
#endif

// Insert 128-bit in 512-bit register
INTRINSIC(v64uint4) insert(v64uint4 a, int idx, v32uint4 b) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v64uint4(insert(set_v128uint4(0, a), idx, b), 0);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v64int4) insert(v64int4 a, int idx, v32int4 b) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v64int4(insert(set_v128int4(0, a), idx, b), 0);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v32uint8) insert(v32uint8 a, int idx, v16uint8 b) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v32uint8(insert(set_v64uint8(0, a), idx, b), 0);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v32int8) insert(v32int8 a, int idx, v16int8 b) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v32int8(insert(set_v64int8(0, a), idx, b), 0);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v16uint16) insert(v16uint16 a, int idx, v8uint16 b) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v16uint16(insert(set_v32uint16(0, a), idx, b), 0);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v16int16) insert(v16int16 a, int idx, v8int16 b) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v16int16(insert(set_v32int16(0, a), idx, b), 0);
}

#if 0
// Insert 128-bit in 512-bit register
INTRINSIC(v8cint16) insert(v8cint16 a, int idx, v4cint16 b) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v8cint16(insert(set_v16cint16(0, a), idx, b), 0);
}
#endif

// Insert 128-bit in 512-bit register
INTRINSIC(v8uint32) insert(v8uint32 a, int idx, v4uint32 b) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v8uint32(insert(set_v16uint32(0, a), idx, b), 0);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v8int32) insert(v8int32 a, int idx, v4int32 b) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v8int32(insert(set_v16int32(0, a), idx, b), 0);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v16bfloat16) insert(v16bfloat16 a, int idx, v8bfloat16 b) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v16bfloat16(insert(set_v32bfloat16(0, a), idx, b), 0);
}

#if 0
// Insert 128-bit in 512-bit register
INTRINSIC(v4cint32) insert(v4cint32 a, int idx, v2cint32 b) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v4cint32(insert(set_v8cint32(0, a), idx, b), 0);
}

// Insert 128-bit in 512-bit register
INTRINSIC(v8float) insert(v8float a, int idx, v4float b) {
  __EXT_128B_CHECK(idx, 1)
  return extract_v8float(insert(set_v16float(0, a), idx, b), 0);
}
#endif

INTRINSIC(v128uint4)
concat(v32uint4 v0, v32uint4 v1, v32uint4 v2, v32uint4 v3) {
  v128uint4 r = set_v128uint4(1, v1);
  r = insert(r, 2, v2);
  r = insert(r, 3, v3);
  r = insert(r, 0, v0);
  return r;
}
INTRINSIC(v128int4) concat(v32int4 v0, v32int4 v1, v32int4 v2, v32int4 v3) {
  v128int4 r = set_v128int4(1, v1);
  r = insert(r, 2, v2);
  r = insert(r, 3, v3);
  r = insert(r, 0, v0);
  return r;
}
INTRINSIC(v64uint8) concat(v16uint8 v0, v16uint8 v1, v16uint8 v2, v16uint8 v3) {
  v64uint8 r = set_v64uint8(1, v1);
  r = insert(r, 2, v2);
  r = insert(r, 3, v3);
  r = insert(r, 0, v0);
  return r;
}
INTRINSIC(v64int8) concat(v16int8 v0, v16int8 v1, v16int8 v2, v16int8 v3) {
  v64int8 r = set_v64int8(1, v1);
  r = insert(r, 2, v2);
  r = insert(r, 3, v3);
  r = insert(r, 0, v0);
  return r;
}
INTRINSIC(v32uint16)
concat(v8uint16 v0, v8uint16 v1, v8uint16 v2, v8uint16 v3) {
  v32uint16 r = set_v32uint16(1, v1);
  r = insert(r, 2, v2);
  r = insert(r, 3, v3);
  r = insert(r, 0, v0);
  return r;
}
INTRINSIC(v32int16) concat(v8int16 v0, v8int16 v1, v8int16 v2, v8int16 v3) {
  v32int16 r = set_v32int16(1, v1);
  r = insert(r, 2, v2);
  r = insert(r, 3, v3);
  r = insert(r, 0, v0);
  return r;
}

#if 0
INTRINSIC(v16cint16)
concat(v4cint16 v0, v4cint16 v1, v4cint16 v2, v4cint16 v3) {
  v16cint16 r = set_v16cint16(1, v1);
  r = insert(r, 2, v2);
  r = insert(r, 3, v3);
  r = insert(r, 0, v0);
  return r;
}
#endif

INTRINSIC(v16uint32)
concat(v4uint32 v0, v4uint32 v1, v4uint32 v2, v4uint32 v3) {
  v16uint32 r = set_v16uint32(1, v1);
  r = insert(r, 2, v2);
  r = insert(r, 3, v3);
  r = insert(r, 0, v0);
  return r;
}
INTRINSIC(v16int32) concat(v4int32 v0, v4int32 v1, v4int32 v2, v4int32 v3) {
  v16int32 r = set_v16int32(1, v1);
  r = insert(r, 2, v2);
  r = insert(r, 3, v3);
  r = insert(r, 0, v0);
  return r;
}
INTRINSIC(v32bfloat16)
concat(v8bfloat16 v0, v8bfloat16 v1, v8bfloat16 v2, v8bfloat16 v3) {
  v32bfloat16 r = set_v32bfloat16(1, v1);
  r = insert(r, 2, v2);
  r = insert(r, 3, v3);
  r = insert(r, 0, v0);
  return r;
}

#if 0
INTRINSIC(v8cint32) concat(v2cint32 v0, v2cint32 v1, v2cint32 v2, v2cint32 v3) {
  v8cint32 r = set_v8cint32(1, v1);
  r = insert(r, 2, v2);
  r = insert(r, 3, v3);
  r = insert(r, 0, v0);
  return r;
}

INTRINSIC(v16float) concat(v4float v0, v4float v1, v4float v2, v4float v3) {
  v16float r = set_v16float(1, v1);
  r = insert(r, 2, v2);
  r = insert(r, 3, v3);
  r = insert(r, 0, v0);
  return r;
}
#endif

INTRINSIC(v64uint4) concat(v32uint4 v0, v32uint4 v1) {
  v64uint4 r = set_v64uint4(1, v1);
  r = insert(r, 0, v0);
  return r;
}
INTRINSIC(v64int4) concat(v32int4 v0, v32int4 v1) {
  v64int4 r = set_v64int4(1, v1);
  r = insert(r, 0, v0);
  return r;
}
INTRINSIC(v32uint8) concat(v16uint8 v0, v16uint8 v1) {
  v32uint8 r = set_v32uint8(1, v1);
  r = insert(r, 0, v0);
  return r;
}
INTRINSIC(v32int8) concat(v16int8 v0, v16int8 v1) {
  v32int8 r = set_v32int8(1, v1);
  r = insert(r, 0, v0);
  return r;
}
INTRINSIC(v16uint16) concat(v8uint16 v0, v8uint16 v1) {
  v16uint16 r = set_v16uint16(1, v1);
  r = insert(r, 0, v0);
  return r;
}
INTRINSIC(v16int16) concat(v8int16 v0, v8int16 v1) {
  v16int16 r = set_v16int16(1, v1);
  r = insert(r, 0, v0);
  return r;
}

#if 0
INTRINSIC(v8cint16) concat(v4cint16 v0, v4cint16 v1) {
  v8cint16 r = set_v8cint16(1, v1);
  r = insert(r, 0, v0);
  return r;
}
#endif

INTRINSIC(v8uint32) concat(v4uint32 v0, v4uint32 v1) {
  v8uint32 r = set_v8uint32(1, v1);
  r = insert(r, 0, v0);
  return r;
}
INTRINSIC(v8int32) concat(v4int32 v0, v4int32 v1) {
  v8int32 r = set_v8int32(1, v1);
  r = insert(r, 0, v0);
  return r;
}
INTRINSIC(v16bfloat16) concat(v8bfloat16 v0, v8bfloat16 v1) {
  v16bfloat16 r = set_v16bfloat16(1, v1);
  r = insert(r, 0, v0);
  return r;
}

#if 0
INTRINSIC(v4cint32) concat(v2cint32 v0, v2cint32 v1) {
  v4cint32 r = set_v4cint32(1, v1);
  r = insert(r, 0, v0);
  return r;
}
INTRINSIC(v8float) concat(v4float v0, v4float v1) {
  v8float r = set_v8float(1, v1);
  r = insert(r, 0, v0);
  return r;
}

// TODO/FIXME : support sparse data type
// ext_sparse
INTRINSIC(v128uint4) extract_v128uint4(v256uint4_sparse v) {
  return __builtin_aiev2_ext_qx(v);
}
INTRINSIC(v64uint8) extract_v64uint8(v128uint8_sparse v) {
  return __builtin_aiev2_ext_qx(v);
}
INTRINSIC(v32uint16) extract_v32uint16(v64uint16_sparse v) {
  return __builtin_aiev2_ext_qx(v);
}
INTRINSIC(v128int4) extract_v128int4(v256int4_sparse v) {
  return __builtin_aiev2_ext_qx(v);
}
INTRINSIC(v64int8) extract_v64int8(v128int8_sparse v) {
  return __builtin_aiev2_ext_qx(v);
}
INTRINSIC(v32int16) extract_v32int16(v64int16_sparse v) {
  return __builtin_aiev2_ext_qx(v);
}
INTRINSIC(v32bfloat16) extract_v32bfloat16(v64bfloat16_sparse v) {
  return __builtin_aiev2_ext_qx(v);
}

INTRINSIC(v128uint4) extract_sparse_data(v256uint4_sparse v) {
  return __builtin_aiev2_ext_qx(v);
}
INTRINSIC(v64uint8) extract_sparse_data(v128uint8_sparse v) {
  return __builtin_aiev2_ext_qx(v);
}
INTRINSIC(v32uint16) extract_sparse_data(v64uint16_sparse v) {
  return __builtin_aiev2_ext_qx(v);
}
INTRINSIC(v128int4) extract_sparse_data(v256int4_sparse v) {
  return __builtin_aiev2_ext_qx(v);
}
INTRINSIC(v64int8) extract_sparse_data(v128int8_sparse v) {
  return __builtin_aiev2_ext_qx(v);
}
INTRINSIC(v32int16) extract_sparse_data(v64int16_sparse v) {
  return __builtin_aiev2_ext_qx(v);
}
INTRINSIC(v32bfloat16) extract_sparse_data(v64bfloat16_sparse v) {
  return __builtin_aiev2_ext_qx(v);
}

INTRINSIC(sparsity_t) extract_sparsity(v256uint4_sparse v) {
  return __builtin_aiev2_ext_qxm(v);
}
INTRINSIC(sparsity_t) extract_sparsity(v128uint8_sparse v) {
  return __builtin_aiev2_ext_qxm(v);
}
INTRINSIC(sparsity_t) extract_sparsity(v64uint16_sparse v) {
  return __builtin_aiev2_ext_qxm(v);
}
INTRINSIC(sparsity_t) extract_sparsity(v256int4_sparse v) {
  return __builtin_aiev2_ext_qxm(v);
}
INTRINSIC(sparsity_t) extract_sparsity(v128int8_sparse v) {
  return __builtin_aiev2_ext_qxm(v);
}
INTRINSIC(sparsity_t) extract_sparsity(v64int16_sparse v) {
  return __builtin_aiev2_ext_qxm(v);
}
INTRINSIC(sparsity_t) extract_sparsity(v64bfloat16_sparse v) {
  return __builtin_aiev2_ext_qxm(v);
}

// ext_update_sparse

INTRINSIC(v256uint4_sparse) update(v256uint4_sparse m, v128uint4 v) {
  return __builtin_aiev2_upd_qx(m, v);
}
INTRINSIC(v128uint8_sparse) update(v128uint8_sparse m, v64uint8 v) {
  return __builtin_aiev2_upd_qx(m, v);
}
INTRINSIC(v64uint16_sparse) update(v64uint16_sparse m, v32uint16 v) {
  return __builtin_aiev2_upd_qx(m, v);
}
INTRINSIC(v256int4_sparse) update(v256int4_sparse m, v128int4 v) {
  return __builtin_aiev2_upd_qx(m, v);
}
INTRINSIC(v128int8_sparse) update(v128int8_sparse m, v64int8 v) {
  return __builtin_aiev2_upd_qx(m, v);
}
INTRINSIC(v64int16_sparse) update(v64int16_sparse m, v32int16 v) {
  return __builtin_aiev2_upd_qx(m, v);
}
INTRINSIC(v64bfloat16_sparse) update(v64bfloat16_sparse m, v32bfloat16 v) {
  return __builtin_aiev2_upd_qx(m, v);
}

INTRINSIC(v256uint4_sparse) update(v256uint4_sparse m, sparsity_t v) {
  return __builtin_aiev2_upd_qxm(m, v);
}
INTRINSIC(v128uint8_sparse) update(v128uint8_sparse m, sparsity_t v) {
  return __builtin_aiev2_upd_qxm(m, v);
}
INTRINSIC(v64uint16_sparse) update(v64uint16_sparse m, sparsity_t v) {
  return __builtin_aiev2_upd_qxm(m, v);
}
INTRINSIC(v256int4_sparse) update(v256int4_sparse m, sparsity_t v) {
  return __builtin_aiev2_upd_qxm(m, v);
}
INTRINSIC(v128int8_sparse) update(v128int8_sparse m, sparsity_t v) {
  return __builtin_aiev2_upd_qxm(m, v);
}
INTRINSIC(v64int16_sparse) update(v64int16_sparse m, sparsity_t v) {
  return __builtin_aiev2_upd_qxm(m, v);
}
INTRINSIC(v64bfloat16_sparse) update(v64bfloat16_sparse m, sparsity_t v) {
  return __builtin_aiev2_upd_qxm(m, v);
}
#endif

// ext_elem
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
INTRINSIC(int) extract_elem(v2int32 v, int idx) { return ext_v2w32(v, idx); }
INTRINSIC(int) extract_elem(v4int16 v, int idx) { return ext_v4w16(v, idx); }
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

// insert_elem
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
  return (v2int8)(short)upd_v2w8((short)v, idx, val);
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

// set_elem
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

#endif // __AIEV2_UPD_EXT_H__
