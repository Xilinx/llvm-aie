//===-------------------- AIEngine AIE2ps intrinsics -----------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef AIE2PS_SCL2VEC_H
#define AIE2PS_SCL2VEC_H

//* Automatically generated file, do not edit! *
//
INTRINSIC(v128int4)
shiftx(v128int4 a, v128int4 b, int pre, unsigned int shift) {
  return __builtin_aie2ps_vshift_I512_I512(a, b, pre, shift);
}
INTRINSIC(v64int8) shiftx(v64int8 a, v64int8 b, int pre, unsigned int shift) {
  return __builtin_aie2ps_vshift_I512_I512(a, b, pre, shift);
}
INTRINSIC(v32int16)
shiftx(v32int16 a, v32int16 b, int pre, unsigned int shift) {
  return __builtin_aie2ps_vshift_I512_I512(a, b, pre, shift);
}
INTRINSIC(v16int32)
shiftx(v16int32 a, v16int32 b, int pre, unsigned int shift) {
  return __builtin_aie2ps_vshift_I512_I512(a, b, pre, shift);
}
INTRINSIC(v128uint4)
shiftx(v128uint4 a, v128uint4 b, int pre, unsigned int shift) {
  return __builtin_aie2ps_vshift_I512_I512(a, b, pre, shift);
}
INTRINSIC(v64uint8)
shiftx(v64uint8 a, v64uint8 b, int pre, unsigned int shift) {
  return __builtin_aie2ps_vshift_I512_I512(a, b, pre, shift);
}
INTRINSIC(v32uint16)
shiftx(v32uint16 a, v32uint16 b, int pre, unsigned int shift) {
  return __builtin_aie2ps_vshift_I512_I512(a, b, pre, shift);
}
INTRINSIC(v16uint32)
shiftx(v16uint32 a, v16uint32 b, int pre, unsigned int shift) {
  return __builtin_aie2ps_vshift_I512_I512(a, b, pre, shift);
}
INTRINSIC(v32bfloat16)
shiftx(v32bfloat16 a, v32bfloat16 b, int pre, unsigned int shift) {
  return __builtin_aie2ps_vshift_bf512_bf512(a, b, pre, shift);
}
INTRINSIC(v32float16)
shiftx(v32float16 a, v32float16 b, int pre, unsigned int shift) {
  return __builtin_aie2ps_vshift_I512_I512(a, b, pre, shift);
}
INTRINSIC(v64float8)
shiftx(v64float8 a, v64float8 b, int pre, unsigned int shift) {
  return {__builtin_aie2ps_vshift_I512_I512(a.data, b.data, pre, shift)};
}
INTRINSIC(v64bfloat8)
shiftx(v64bfloat8 a, v64bfloat8 b, int pre, unsigned int shift) {
  return {__builtin_aie2ps_vshift_I512_I512(a.data, b.data, pre, shift)};
}
INTRINSIC(v16float)
shiftx(v16float a, v16float b, int pre, unsigned int shift) {
  return __builtin_aie2ps_vshift_I512_I512(a, b, pre, shift);
}
INTRINSIC(v16accfloat)
shiftx(v16accfloat a, v16accfloat b, int pre, unsigned int shift) {
  return __builtin_aie2ps_vshift_I512_I512(a, b, pre, shift);
}
INTRINSIC(v128int4) shift_bytes(v128int4 a, v128int4 b, unsigned int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v64int8) shift_bytes(v64int8 a, v64int8 b, unsigned int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v32int16) shift_bytes(v32int16 a, v32int16 b, unsigned int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v16int32) shift_bytes(v16int32 a, v16int32 b, unsigned int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v128uint4) shift_bytes(v128uint4 a, v128uint4 b, unsigned int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v64uint8) shift_bytes(v64uint8 a, v64uint8 b, unsigned int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v32uint16) shift_bytes(v32uint16 a, v32uint16 b, unsigned int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v16uint32) shift_bytes(v16uint32 a, v16uint32 b, unsigned int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v32bfloat16)
shift_bytes(v32bfloat16 a, v32bfloat16 b, unsigned int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v32float16)
shift_bytes(v32float16 a, v32float16 b, unsigned int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v64float8) shift_bytes(v64float8 a, v64float8 b, unsigned int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v64bfloat8)
shift_bytes(v64bfloat8 a, v64bfloat8 b, unsigned int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v16float) shift_bytes(v16float a, v16float b, unsigned int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v16accfloat)
shift_bytes(v16accfloat a, v16accfloat b, unsigned int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v64int8) shift(v64int8 a, v64int8 b, unsigned int shift) {
  return shiftx(a, b, 0, shift * 1);
}
INTRINSIC(v32int16) shift(v32int16 a, v32int16 b, unsigned int shift) {
  return shiftx(a, b, 0, shift * 2);
}
INTRINSIC(v16int32) shift(v16int32 a, v16int32 b, unsigned int shift) {
  return shiftx(a, b, 0, shift * 4);
}
INTRINSIC(v64uint8) shift(v64uint8 a, v64uint8 b, unsigned int shift) {
  return shiftx(a, b, 0, shift * 1);
}
INTRINSIC(v32uint16) shift(v32uint16 a, v32uint16 b, unsigned int shift) {
  return shiftx(a, b, 0, shift * 2);
}
INTRINSIC(v16uint32) shift(v16uint32 a, v16uint32 b, unsigned int shift) {
  return shiftx(a, b, 0, shift * 4);
}
INTRINSIC(v32bfloat16) shift(v32bfloat16 a, v32bfloat16 b, unsigned int shift) {
  return shiftx(a, b, 0, shift * 2);
}
INTRINSIC(v32float16) shift(v32float16 a, v32float16 b, unsigned int shift) {
  return shiftx(a, b, 0, shift * 2);
}
INTRINSIC(v64float8) shift(v64float8 a, v64float8 b, unsigned int shift) {
  return shiftx(a, b, 0, shift * 1);
}
INTRINSIC(v64bfloat8) shift(v64bfloat8 a, v64bfloat8 b, unsigned int shift) {
  return shiftx(a, b, 0, shift * 1);
}
INTRINSIC(v16float) shift(v16float a, v16float b, unsigned int shift) {
  return shiftx(a, b, 0, shift * 4);
}
INTRINSIC(v16accfloat) shift(v16accfloat a, v16accfloat b, unsigned int shift) {
  return shiftx(a, b, 0, shift * 4);
}
template <typename T> inline auto vector_extract(T a, int idx, int sign) {
  return a[idx];
}
template <typename T>
inline decltype(auto) vector_extract64(T a, int idx, int sign) {
  idx *= 2;

  return sign ? (v2int32){a[idx], a[idx + 1]} : (v2uint32){a[idx], a[idx + 1]};
}

INTRINSIC(v2int4) ext_v2int4(v128int4 v, int idx, int sign) {
  return vector_extract(v, idx, sign);
}
INTRINSIC(v4int4) ext_v4int4(v128int4 v, int idx, int sign) {
  return (v4int4)(short)vector_extract((v32int16)v, idx, sign);
}
INTRINSIC(v8int4) ext_v8int4(v128int4 v, int idx, int sign) {
  return (v8int4)vector_extract((v16int32)v, idx, sign);
}
INTRINSIC(v16int4) ext_v16int4(v128int4 v, int idx, int sign) {
  return vector_extract64((v16int32)v, idx, sign);
}
INTRINSIC(mask64) ext_mask64(v128int4 v, int idx, int sign) {
  return (mask64)vector_extract64((v16int32)v, idx, sign);
}

INTRINSIC(char) ext_elem(v64int8 v, int idx, int sign) {
  return vector_extract(v, idx, sign);
}
INTRINSIC(v2int8) ext_v2int8(v64int8 v, int idx, int sign) {
  return (v2int8)(short)vector_extract((v32int16)v, idx, sign);
}
INTRINSIC(v4int8) ext_v4int8(v64int8 v, int idx, int sign) {
  return (v4int8)vector_extract((v16int32)v, idx, sign);
}
INTRINSIC(v8int8) ext_v8int8(v64int8 v, int idx, int sign) {
  return vector_extract64((v16int32)v, idx, sign);
}
INTRINSIC(mask64) ext_mask64(v64int8 v, int idx, int sign) {
  return (mask64)vector_extract64((v16int32)v, idx, sign);
}

INTRINSIC(short) ext_elem(v32int16 v, int idx, int sign) {
  return vector_extract(v, idx, sign);
}
INTRINSIC(v2int16) ext_v2int16(v32int16 v, int idx, int sign) {
  return (v2int16)vector_extract((v16int32)v, idx, sign);
}
INTRINSIC(v4int16) ext_v4int16(v32int16 v, int idx, int sign) {
  return vector_extract64((v16int32)v, idx, sign);
}
INTRINSIC(mask64) ext_mask64(v32int16 v, int idx, int sign) {
  return (mask64)vector_extract64((v16int32)v, idx, sign);
}

INTRINSIC(int) ext_elem(v16int32 v, int idx, int sign) {
  return vector_extract(v, idx, sign);
}
INTRINSIC(v2int32) ext_v2int32(v16int32 v, int idx, int sign) {
  return vector_extract64((v16int32)v, idx, sign);
}
INTRINSIC(mask64) ext_mask64(v16int32 v, int idx, int sign) {
  return (mask64)vector_extract64((v16int32)v, idx, sign);
}

INTRINSIC(v2uint4) ext_v2uint4(v128uint4 v, int idx, int sign) {
  return vector_extract(v, idx, sign);
}
INTRINSIC(v4uint4) ext_v4uint4(v128uint4 v, int idx, int sign) {
  return (v4uint4)(short)vector_extract((v32uint16)v, idx, sign);
}
INTRINSIC(v8uint4) ext_v8uint4(v128uint4 v, int idx, int sign) {
  return (v8uint4)vector_extract((v16uint32)v, idx, sign);
}
INTRINSIC(v16uint4) ext_v16uint4(v128uint4 v, int idx, int sign) {
  return vector_extract64((v16uint32)v, idx, sign);
}
INTRINSIC(mask64) ext_mask64(v128uint4 v, int idx, int sign) {
  return (mask64)vector_extract64((v16uint32)v, idx, sign);
}

INTRINSIC(unsigned char) ext_elem(v64uint8 v, int idx, int sign) {
  return vector_extract(v, idx, sign);
}
INTRINSIC(v2uint8) ext_v2uint8(v64uint8 v, int idx, int sign) {
  return (v2uint8)(short)vector_extract((v32uint16)v, idx, sign);
}
INTRINSIC(v4uint8) ext_v4uint8(v64uint8 v, int idx, int sign) {
  return (v4uint8)vector_extract((v16uint32)v, idx, sign);
}
INTRINSIC(v8uint8) ext_v8uint8(v64uint8 v, int idx, int sign) {
  return vector_extract64((v16uint32)v, idx, sign);
}
INTRINSIC(mask64) ext_mask64(v64uint8 v, int idx, int sign) {
  return (mask64)vector_extract64((v16uint32)v, idx, sign);
}

INTRINSIC(unsigned short) ext_elem(v32uint16 v, int idx, int sign) {
  return vector_extract(v, idx, sign);
}
INTRINSIC(v2uint16) ext_v2uint16(v32uint16 v, int idx, int sign) {
  return (v2uint16)vector_extract((v16uint32)v, idx, sign);
}
INTRINSIC(v4uint16) ext_v4uint16(v32uint16 v, int idx, int sign) {
  return vector_extract64((v16uint32)v, idx, sign);
}
INTRINSIC(mask64) ext_mask64(v32uint16 v, int idx, int sign) {
  return (mask64)vector_extract64((v16uint32)v, idx, sign);
}

INTRINSIC(unsigned int) ext_elem(v16uint32 v, int idx, int sign) {
  return vector_extract(v, idx, sign);
}
INTRINSIC(v2uint32) ext_v2uint32(v16uint32 v, int idx, int sign) {
  return vector_extract64((v16int32)v, idx, sign);
}
INTRINSIC(mask64) ext_mask64(v16uint32 v, int idx, int sign) {
  return (mask64)vector_extract64((v16uint32)v, idx, sign);
}
INTRINSIC(float) ext_elem(v16float v, int idx, int sign) {
  return vector_extract(v, idx, sign);
}
INTRINSIC(v2float) ext_v2float(v16float v, int idx, int sign) {
  float elem1 = vector_extract(v, idx, sign);

  float elem2 = vector_extract(v, idx + 1, sign);

  v2float val{elem1, elem2};

  return val;
}

INTRINSIC(bfloat16) ext_elem(v32bfloat16 v, int idx, int sign) {
  short elem = vector_extract((v32int16)v, idx, sign);

  return __builtin_bit_cast(bfloat16, elem);
}
INTRINSIC(v2bfloat16) ext_v2bfloat16(v32bfloat16 v, int idx, int sign) {
  int elem = vector_extract((v16int32)v, idx, sign);

  return __builtin_bit_cast(v2bfloat16, elem);
}
INTRINSIC(v4bfloat16) ext_v4bfloat16(v32bfloat16 v, int idx, int sign) {
  v2int32 elem = vector_extract64((v16int32)v, idx, sign);

  return __builtin_bit_cast(v4bfloat16, elem);
}

INTRINSIC(mask64) ext_mask64(v32bfloat16 v, int idx, int sign) {
  return (mask64)vector_extract64((v16int32)v, idx, sign);
}

INTRINSIC(float16) ext_elem(v32float16 v, int idx, int sign) {
  return vector_extract(v, idx, sign);
}
INTRINSIC(v2float16) ext_v2float16(v32float16 v, int idx, int sign) {
  int elem = vector_extract((v16int32)v, idx, sign);

  return __builtin_bit_cast(v2float16, elem);
}
INTRINSIC(v4float16) ext_v4float16(v32float16 v, int idx, int sign) {
  v2int32 elem = vector_extract64((v16int32)v, idx, sign);

  return __builtin_bit_cast(v4float16, elem);
}
INTRINSIC(mask64) ext_mask64(v32float16 v, int idx, int sign) {
  return (mask64)vector_extract64((v16int32)v, idx, sign);
}

INTRINSIC(float8) ext_elem(v64float8 v, int idx, int sign) {
  return {vector_extract(v.data, idx, sign)};
}
INTRINSIC(v2float8) ext_v2float8(v64float8 v, int idx, int sign) {
  return {(v2int8)(short)vector_extract((v32int16)v.data, idx, sign)};
}
INTRINSIC(v4float8) ext_v4float8(v64float8 v, int idx, int sign) {
  return {vector_extract((v16int32)v.data, idx, sign)};
}
INTRINSIC(v8float8) ext_v8float8(v64float8 v, int idx, int sign) {
  return {vector_extract64((v16int32)v.data, idx, sign)};
}
INTRINSIC(mask64) ext_mask64(v64float8 v, int idx, int sign) {
  return (mask64)vector_extract64((v16int32)v.data, idx, sign);
}

INTRINSIC(bfloat8) ext_elem(v64bfloat8 v, int idx, int sign) {
  return {vector_extract(v.data, idx, sign)};
}
INTRINSIC(v2bfloat8) ext_v2bfloat8(v64bfloat8 v, int idx, int sign) {
  return {(v2int8)(short)vector_extract((v32int16)v.data, idx, sign)};
}
INTRINSIC(v4bfloat8) ext_v4bfloat8(v64bfloat8 v, int idx, int sign) {
  return {vector_extract((v16int32)v.data, idx, sign)};
}
INTRINSIC(v8bfloat8) ext_v8bfloat8(v64bfloat8 v, int idx, int sign) {
  return {vector_extract64((v16int32)v.data, idx, sign)};
}
INTRINSIC(mask64) ext_mask64(v64bfloat8 v, int idx, int sign) {
  return (mask64)vector_extract64((v16int32)v.data, idx, sign);
}

INTRINSIC(v2int4) extract_v2int4(v128int4 v, int idx, int sign) {
  return ext_v2int4(v, idx, sign);
}
INTRINSIC(v4int4) extract_v4int4(v128int4 v, int idx, int sign) {
  return ext_v4int4(v, idx, sign);
}
INTRINSIC(v8int4) extract_v8int4(v128int4 v, int idx, int sign) {
  return ext_v8int4(v, idx, sign);
}
INTRINSIC(v16int4) extract_v16int4(v128int4 v, int idx, int sign) {
  return ext_v16int4(v, idx, sign);
}
INTRINSIC(v16int4) extract_mask64(v128int4 v, int idx, int sign) {
  return ext_mask64((v16int32)v, idx, sign);
}

INTRINSIC(char) extract_elem(v64int8 v, int idx, int sign) {
  return ext_elem(v, idx, sign);
}
INTRINSIC(v2int8) extract_v2int8(v64int8 v, int idx, int sign) {
  return ext_v2int8(v, idx, sign);
}
INTRINSIC(v4int8) extract_v4int8(v64int8 v, int idx, int sign) {
  return ext_v4int8(v, idx, sign);
}
INTRINSIC(v8int8) extract_v8int8(v64int8 v, int idx, int sign) {
  return ext_v8int8(v, idx, sign);
}
INTRINSIC(v8int8) extract_mask64(v64int8 v, int idx, int sign) {
  return ext_mask64((v16int32)v, idx, sign);
}

INTRINSIC(short) extract_elem(v32int16 v, int idx, int sign) {
  return ext_elem(v, idx, sign);
}
INTRINSIC(v2int16) extract_v2int16(v32int16 v, int idx, int sign) {
  return ext_v2int16(v, idx, sign);
}
INTRINSIC(v4int16) extract_v4int16(v32int16 v, int idx, int sign) {
  return ext_v4int16(v, idx, sign);
}
INTRINSIC(v4int16) extract_mask64(v32int16 v, int idx, int sign) {
  return ext_mask64((v16int32)v, idx, sign);
}

INTRINSIC(int) extract_elem(v16int32 v, int idx, int sign) {
  return ext_elem(v, idx, sign);
}
INTRINSIC(v2int32) extract_v2int32(v16int32 v, int idx, int sign) {
  return ext_v2int32(v, idx, sign);
}
INTRINSIC(v2int32) extract_mask64(v16int32 v, int idx, int sign) {
  return ext_mask64((v16int32)v, idx, sign);
}

INTRINSIC(v2uint4) extract_v2uint4(v128uint4 v, int idx, int sign) {
  return ext_v2uint4(v, idx, sign);
}
INTRINSIC(v4uint4) extract_v4uint4(v128uint4 v, int idx, int sign) {
  return ext_v4uint4(v, idx, sign);
}
INTRINSIC(v8uint4) extract_v8uint4(v128uint4 v, int idx, int sign) {
  return ext_v8uint4(v, idx, sign);
}
INTRINSIC(v16uint4) extract_v16uint4(v128uint4 v, int idx, int sign) {
  return ext_v16uint4(v, idx, sign);
}
INTRINSIC(v16uint4) extract_mask64(v128uint4 v, int idx, int sign) {
  return ext_mask64((v16uint32)v, idx, sign);
}

INTRINSIC(unsigned char) extract_elem(v64uint8 v, int idx, int sign) {
  return ext_elem(v, idx, sign);
}
INTRINSIC(v2uint8) extract_v2uint8(v64uint8 v, int idx, int sign) {
  return ext_v2uint8(v, idx, sign);
}
INTRINSIC(v4uint8) extract_v4uint8(v64uint8 v, int idx, int sign) {
  return ext_v4uint8(v, idx, sign);
}
INTRINSIC(v8uint8) extract_v8uint8(v64uint8 v, int idx, int sign) {
  return ext_v8uint8(v, idx, sign);
}
INTRINSIC(v8uint8) extract_mask64(v64uint8 v, int idx, int sign) {
  return ext_mask64((v16uint32)v, idx, sign);
}

INTRINSIC(unsigned short) extract_elem(v32uint16 v, int idx, int sign) {
  return ext_elem(v, idx, sign);
}
INTRINSIC(v2uint16) extract_v2uint16(v32uint16 v, int idx, int sign) {
  return ext_v2uint16(v, idx, sign);
}
INTRINSIC(v4uint16) extract_v4uint16(v32uint16 v, int idx, int sign) {
  return ext_v4uint16(v, idx, sign);
}
INTRINSIC(v4uint16) extract_mask64(v32uint16 v, int idx, int sign) {
  return ext_mask64((v16uint32)v, idx, sign);
}

INTRINSIC(unsigned int) extract_elem(v16uint32 v, int idx, int sign) {
  return ext_elem(v, idx, sign);
}
INTRINSIC(v2uint32) extract_v2uint32(v16uint32 v, int idx, int sign) {
  return ext_v2uint32(v, idx, sign);
}
INTRINSIC(v2uint32) extract_mask64(v16uint32 v, int idx, int sign) {
  return ext_mask64((v16uint32)v, idx, sign);
}

INTRINSIC(float) extract_elem(v16float v, int idx, int sign) {
  return ext_elem(v, idx, sign);
}
INTRINSIC(v2float) extract_v2float(v16float v, int idx, int sign) {
  return ext_v2float(v, idx, sign);
}
INTRINSIC(bfloat16) extract_elem(v32bfloat16 v, int idx, int sign) {
  return ext_elem(v, idx, sign);
}
INTRINSIC(v2bfloat16) extract_v2bfloat16(v32bfloat16 v, int idx, int sign) {
  return ext_v2bfloat16(v, idx, sign);
}
INTRINSIC(v4bfloat16) extract_v4bfloat16(v32bfloat16 v, int idx, int sign) {
  return ext_v4bfloat16(v, idx, sign);
}
INTRINSIC(v4bfloat16) extract_mask64(v32bfloat16 v, int idx, int sign) {
  return ext_mask64(v, idx, sign);
}

INTRINSIC(float16) extract_elem(v32float16 v, int idx, int sign) {
  return ext_elem(v, idx, sign);
}
INTRINSIC(v2float16) extract_v2float16(v32float16 v, int idx, int sign) {
  return ext_v2float16(v, idx, sign);
}
INTRINSIC(v4float16) extract_v4float16(v32float16 v, int idx, int sign) {
  return ext_v4float16(v, idx, sign);
}
INTRINSIC(v4float16) extract_mask64(v32float16 v, int idx, int sign) {
  return ext_mask64(v, idx, sign);
}

INTRINSIC(float8) extract_elem(v64float8 v, int idx, int sign) {
  return ext_elem(v, idx, sign);
}
INTRINSIC(v2float8) extract_v2float8(v64float8 v, int idx, int sign) {
  return ext_v2float8(v, idx, sign);
}
INTRINSIC(v4float8) extract_v4float8(v64float8 v, int idx, int sign) {
  return ext_v4float8(v, idx, sign);
}
INTRINSIC(v8float8) extract_v8float8(v64float8 v, int idx, int sign) {
  return ext_v8float8(v, idx, sign);
}
INTRINSIC(v8float8) extract_mask64(v64float8 v, int idx, int sign) {
  return {(v8int8)ext_mask64(v, idx, sign)};
}

INTRINSIC(bfloat8) extract_elem(v64bfloat8 v, int idx, int sign) {
  return ext_elem(v, idx, sign);
}
INTRINSIC(v2bfloat8) extract_v2bfloat8(v64bfloat8 v, int idx, int sign) {
  return ext_v2bfloat8(v, idx, sign);
}
INTRINSIC(v4bfloat8) extract_v4bfloat8(v64bfloat8 v, int idx, int sign) {
  return ext_v4bfloat8(v, idx, sign);
}
INTRINSIC(v8bfloat8) extract_v8bfloat8(v64bfloat8 v, int idx, int sign) {
  return ext_v8bfloat8(v, idx, sign);
}
INTRINSIC(v8bfloat8) extract_mask64(v64bfloat8 v, int idx, int sign) {
  return {(v8int8)ext_mask64(v, idx, sign)};
}
INTRINSIC(v2int4) ext_v2int4(v128int4 v, int idx) {
  return ext_v2int4(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v4int4) ext_v4int4(v128int4 v, int idx) {
  return ext_v4int4(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v8int4) ext_v8int4(v128int4 v, int idx) {
  return ext_v8int4(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v16int4) ext_v16int4(v128int4 v, int idx) {
  return ext_v16int4(v, idx, __SIGN_SIGNED);
}

INTRINSIC(char) ext_elem(v64int8 v, int idx) {
  return ext_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2int8) ext_v2int8(v64int8 v, int idx) {
  return ext_v2int8(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v4int8) ext_v4int8(v64int8 v, int idx) {
  return ext_v4int8(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v8int8) ext_v8int8(v64int8 v, int idx) {
  return ext_v8int8(v, idx, __SIGN_SIGNED);
}

INTRINSIC(short) ext_elem(v32int16 v, int idx) {
  return ext_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2int16) ext_v2int16(v32int16 v, int idx) {
  return ext_v2int16(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v4int16) ext_v4int16(v32int16 v, int idx) {
  return ext_v4int16(v, idx, __SIGN_SIGNED);
}

INTRINSIC(int) ext_elem(v16int32 v, int idx) {
  return ext_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2int32) ext_v2int32(v16int32 v, int idx) {
  return ext_v2int32(v, idx, __SIGN_SIGNED);
}

INTRINSIC(float) ext_elem(v16float v, int idx) {
  return ext_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2float) ext_v2float(v16float v, int idx) {
  return ext_v2float(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v2uint4) ext_v2uint4(v128uint4 v, int idx) {
  return ext_v2uint4(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v4uint4) ext_v4uint4(v128uint4 v, int idx) {
  return ext_v4uint4(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v8uint4) ext_v8uint4(v128uint4 v, int idx) {
  return ext_v8uint4(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v16uint4) ext_v16uint4(v128uint4 v, int idx) {
  return ext_v16uint4(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned char) ext_elem(v64uint8 v, int idx) {
  return ext_elem(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v2uint8) ext_v2uint8(v64uint8 v, int idx) {
  return ext_v2uint8(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v4uint8) ext_v4uint8(v64uint8 v, int idx) {
  return ext_v4uint8(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v8uint8) ext_v8uint8(v64uint8 v, int idx) {
  return ext_v8uint8(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned short) ext_elem(v32uint16 v, int idx) {
  return ext_elem(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v2uint16) ext_v2uint16(v32uint16 v, int idx) {
  return ext_v2uint16(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v4uint16) ext_v4uint16(v32uint16 v, int idx) {
  return ext_v4uint16(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned int) ext_elem(v16uint32 v, int idx) {
  return ext_elem(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v2uint32) ext_vu2int32(v16uint32 v, int idx) {
  return ext_v2uint32(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(bfloat16) ext_elem(v32bfloat16 v, int idx) {
  return ext_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2bfloat16) ext_v2bfloat16(v32bfloat16 v, int idx) {
  return ext_v2bfloat16(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v4bfloat16) ext_v4bfloat16(v32bfloat16 v, int idx) {
  return ext_v4bfloat16(v, idx, __SIGN_SIGNED);
}

INTRINSIC(float16) ext_elem(v32float16 v, int idx) {
  return ext_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2float16) ext_v2float16(v32float16 v, int idx) {
  return ext_v2float16(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v4float16) ext_v4float16(v32float16 v, int idx) {
  return ext_v4float16(v, idx, __SIGN_SIGNED);
}

INTRINSIC(float8) ext_elem(v64float8 v, int idx) {
  return ext_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2float8) ext_v2float8(v64float8 v, int idx) {
  return ext_v2float8(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v4float8) ext_v4float8(v64float8 v, int idx) {
  return ext_v4float8(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v8float8) ext_v8float8(v64float8 v, int idx) {
  return ext_v8float8(v, idx, __SIGN_SIGNED);
}

INTRINSIC(bfloat8) ext_elem(v64bfloat8 v, int idx) {
  return ext_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2bfloat8) ext_v2bfloat8(v64bfloat8 v, int idx) {
  return ext_v2bfloat8(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v4bfloat8) ext_v4bfloat8(v64bfloat8 v, int idx) {
  return ext_v4bfloat8(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v8bfloat8) ext_v8bfloat8(v64bfloat8 v, int idx) {
  return ext_v8bfloat8(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v2int4) extract_v2int4(v128int4 v, int idx) {
  return extract_v2int4(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v4int4) extract_v4int4(v128int4 v, int idx) {
  return extract_v4int4(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v8int4) extract_v8int4(v128int4 v, int idx) {
  return extract_v8int4(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v16int4) extract_v16int4(v128int4 v, int idx) {
  return extract_v16int4(v, idx, __SIGN_SIGNED);
}

INTRINSIC(char) extract_elem(v64int8 v, int idx) {
  return extract_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2int8) extract_v2int8(v64int8 v, int idx) {
  return extract_v2int8(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v4int8) extract_v4int8(v64int8 v, int idx) {
  return extract_v4int8(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v8int8) extract_v8int8(v64int8 v, int idx) {
  return extract_v8int8(v, idx, __SIGN_SIGNED);
}

INTRINSIC(short) extract_elem(v32int16 v, int idx) {
  return extract_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2int16) extract_v2int16(v32int16 v, int idx) {
  return extract_v2int16(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v4int16) extract_v4int16(v32int16 v, int idx) {
  return extract_v4int16(v, idx, __SIGN_SIGNED);
}

INTRINSIC(int) extract_elem(v16int32 v, int idx) {
  return extract_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2int32) extract_v2int32(v16int32 v, int idx) {
  return extract_v2int32(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v2uint4) extract_v2uint4(v128uint4 v, int idx) {
  return extract_v2uint4(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v4uint4) extract_v4uint4(v128uint4 v, int idx) {
  return extract_v4uint4(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v8uint4) extract_v8uint4(v128uint4 v, int idx) {
  return extract_v8uint4(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v16uint4) extract_v16uint4(v128uint4 v, int idx) {
  return extract_v16uint4(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned char) extract_elem(v64uint8 v, int idx) {
  return extract_elem(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v2uint8) extract_v2uint8(v64uint8 v, int idx) {
  return extract_v2uint8(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v4uint8) extract_v4uint8(v64uint8 v, int idx) {
  return extract_v4uint8(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v8uint8) extract_v8uint8(v64uint8 v, int idx) {
  return extract_v8uint8(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned short) extract_elem(v32uint16 v, int idx) {
  return extract_elem(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v2uint16) extract_v2uint16(v32uint16 v, int idx) {
  return extract_v2uint16(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v4uint16) extract_v4uint16(v32uint16 v, int idx) {
  return extract_v4uint16(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned int) extract_elem(v16uint32 v, int idx) {
  return extract_elem(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(v2uint32) extract_v2uint32(v16uint32 v, int idx) {
  return extract_v2uint32(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(float) extract_elem(v16float v, int idx) {
  return extract_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2float) extract_v2float(v16float v, int idx) {
  return extract_v2float(v, idx, __SIGN_SIGNED);
}

INTRINSIC(bfloat16) extract_elem(v32bfloat16 v, int idx) {
  return extract_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2bfloat16) extract_v2bfloat16(v32bfloat16 v, int idx) {
  return extract_v2bfloat16(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v4bfloat16) extract_v4bfloat16(v32bfloat16 v, int idx) {
  return extract_v4bfloat16(v, idx, __SIGN_SIGNED);
}

INTRINSIC(float16) extract_elem(v32float16 v, int idx) {
  return extract_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2float16) extract_v2float16(v32float16 v, int idx) {
  return extract_v2float16(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v4float16) extract_v4float16(v32float16 v, int idx) {
  return extract_v4float16(v, idx, __SIGN_SIGNED);
}

INTRINSIC(float8) extract_elem(v64float8 v, int idx) {
  return extract_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2float8) extract_v2float8(v64float8 v, int idx) {
  return extract_v2float8(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v4float8) extract_v4float8(v64float8 v, int idx) {
  return extract_v4float8(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v8float8) extract_v8float8(v64float8 v, int idx) {
  return extract_v8float8(v, idx, __SIGN_SIGNED);
}

INTRINSIC(bfloat8) extract_elem(v64bfloat8 v, int idx) {
  return extract_elem(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v2bfloat8) extract_v2bfloat8(v64bfloat8 v, int idx) {
  return extract_v2bfloat8(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v4bfloat8) extract_v4bfloat8(v64bfloat8 v, int idx) {
  return extract_v4bfloat8(v, idx, __SIGN_SIGNED);
}
INTRINSIC(v8bfloat8) extract_v8bfloat8(v64bfloat8 v, int idx) {
  return extract_v8bfloat8(v, idx, __SIGN_SIGNED);
}
INTRINSIC(void *) ext_address(v64int8 v, int idx) {
  return (void *)vector_extract(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(void *) ext_address(v32int16 v, int idx) {
  return (void *)vector_extract(v, idx, __SIGN_UNSIGNED);
}
INTRINSIC(void *) ext_address(v16int32 v, int idx) {
  return (void *)vector_extract(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(void *) extract_address(v64int8 v, int idx) {
  return ext_address(v, idx);
}
INTRINSIC(void *) extract_address(v32int16 v, int idx) {
  return ext_address(v, idx);
}
INTRINSIC(void *) extract_address(v16int32 v, int idx) {
  return ext_address(v, idx);
}
inline v16int32 vector_broadcast64(v2int32 b) {
  return {b[0], b[1], b[0], b[1], b[0], b[1], b[0], b[1],
          b[0], b[1], b[0], b[1], b[0], b[1], b[0], b[1]};
}
inline v16uint32 vector_broadcast64(v2uint32 b) {
  return {b[0], b[1], b[0], b[1], b[0], b[1], b[0], b[1],
          b[0], b[1], b[0], b[1], b[0], b[1], b[0], b[1]};
}

INTRINSIC(v64int8) broadcast_s8(int b) { return (char)b - v64int8{0}; }
INTRINSIC(v32int16) broadcast_s16(int b) { return (short)b - v32int16{0}; }
INTRINSIC(v16int32) broadcast_s32(int b) { return b - v16int32{0}; }
INTRINSIC(v16int32) broadcast_s64(mask64 b) {
  return vector_broadcast64((v2int32)b);
}
INTRINSIC(v16int32) broadcast_v2s32(v2int32 b) { return vector_broadcast64(b); }
INTRINSIC(v64uint8) broadcast_u8(unsigned int b) {
  return (unsigned char)b - v64uint8{0};
}
INTRINSIC(v32uint16) broadcast_u16(unsigned int b) {
  return (unsigned short)b - v32uint16{0};
}
INTRINSIC(v16uint32) broadcast_u32(unsigned int b) { return b - v16uint32{0}; }
INTRINSIC(v16uint32) broadcast_u64(mask64 b) {
  return vector_broadcast64((v2uint32)b);
}
INTRINSIC(v16uint32) broadcast_v2u32(v2uint32 b) {
  return vector_broadcast64(b);
}
INTRINSIC(v32bfloat16) broadcast_bfloat16(bfloat16 b) {
  return b - v32bfloat16{0};
}
INTRINSIC(v64bfloat8) broadcast_bfloat8(bfloat8 b) {
  char as_char = __builtin_bit_cast(char, b.data);

  return {broadcast_s8(as_char)};
}
INTRINSIC(v32float16) broadcast_float16(float16 b) {
  short as_short = __builtin_bit_cast(short, b);

  return __builtin_bit_cast(v32float16, broadcast_s16(as_short));
}
INTRINSIC(v64float8) broadcast_float8(float8 b) {
  char as_char = __builtin_bit_cast(char, b.data);

  return {broadcast_s8(as_char)};
}
INTRINSIC(v16float) broadcast_float(float b) {
  int as_int = __builtin_bit_cast(int, b);

  return __builtin_bit_cast(v16float, broadcast_s32(as_int));
}
INTRINSIC(v128int4) broadcast_to_v128int4(v2int4 b) { return b - v128int4{0}; }
INTRINSIC(v128int4) broadcast_to_v128int4(v4int4 b) {
  return broadcast_s16((short)b);
}
INTRINSIC(v128int4) broadcast_to_v128int4(v8int4 b) {
  return broadcast_s32((int)b);
}
INTRINSIC(v128int4) broadcast_to_v128int4(v16int4 b) {
  return vector_broadcast64((v2int32)b);
}

INTRINSIC(v64int8) broadcast_to_v64int8(int b) { return (char)b - v64int8{0}; }
INTRINSIC(v64int8) broadcast_to_v64int8(v2int8 b) {
  return broadcast_s16((short)b);
}
INTRINSIC(v64int8) broadcast_to_v64int8(v4int8 b) {
  return broadcast_s32((int)b);
}
INTRINSIC(v64int8) broadcast_to_v64int8(v8int8 b) {
  return vector_broadcast64((v2int32)b);
}

INTRINSIC(v32int16) broadcast_to_v32int16(int b) {
  return (short)b - v32int16{0};
}
INTRINSIC(v32int16) broadcast_to_v32int16(v2int16 b) {
  return broadcast_s32((int)b);
}
INTRINSIC(v32int16) broadcast_to_v32int16(v4int16 b) {
  return vector_broadcast64((v2int32)b);
}

INTRINSIC(v16int32) broadcast_to_v16int32(int b) { return b - v16int32{0}; }
INTRINSIC(v16int32) broadcast_to_v16int32(mask64 b) {
  return vector_broadcast64((v2int32)b);
}
INTRINSIC(v16int32) broadcast_to_v16int32(v2int32 b) {
  return vector_broadcast64(b);
}

INTRINSIC(v128uint4) broadcast_to_v128uint4(v2uint4 b) {
  return b - v128uint4{0};
}
INTRINSIC(v128uint4) broadcast_to_v128uint4(v4uint4 b) {
  return broadcast_u16((unsigned short)b);
}
INTRINSIC(v128uint4) broadcast_to_v128uint4(v8uint4 b) {
  return broadcast_u32((unsigned int)b);
}
INTRINSIC(v128uint4) broadcast_to_v128uint4(v16uint4 b) {
  return vector_broadcast64((v2uint32)b);
}

INTRINSIC(v64uint8) broadcast_to_v64uint8(unsigned int b) {
  return (unsigned char)b - v64uint8{0};
}
INTRINSIC(v64uint8) broadcast_to_v64uint8(v2uint8 b) {
  return broadcast_u16((unsigned short)b);
}
INTRINSIC(v64uint8) broadcast_to_v64uint8(v4uint8 b) {
  return broadcast_u32((unsigned int)b);
}
INTRINSIC(v64uint8) broadcast_to_v64uint8(v8uint8 b) {
  return vector_broadcast64((v2uint32)b);
}

INTRINSIC(v32uint16) broadcast_to_v32uint16(unsigned int b) {
  return (unsigned short)b - v32uint16{0};
}
INTRINSIC(v32uint16) broadcast_to_v32uint16(v2uint16 b) {
  return broadcast_u32((unsigned int)b);
}
INTRINSIC(v32uint16) broadcast_to_v32uint16(v4uint16 b) {
  return vector_broadcast64((v2uint32)b);
}

INTRINSIC(v16uint32) broadcast_to_v16uint32(unsigned int b) {
  return b - v16uint32{0};
}

INTRINSIC(v16uint32) broadcast_to_v16uint32(v2uint32 b) {
  return vector_broadcast64(b);
}
INTRINSIC(v32bfloat16) broadcast_to_v32bfloat16(bfloat16 b) {
  return broadcast_bfloat16(b);
}
INTRINSIC(v32bfloat16) broadcast_to_v32bfloat16(v2bfloat16 b) {
  return __builtin_aie2ps_vbroadcast_bf32_bf512(b);
}
INTRINSIC(v32bfloat16) broadcast_to_v32bfloat16(v4bfloat16 b) {
  return __builtin_aie2ps_vbroadcast_bf64_bf512(b);
}

INTRINSIC(v32float16) broadcast_to_v32float16(float16 b) {
  return broadcast_float16(b);
}
INTRINSIC(v32float16) broadcast_to_v32float16(v2float16 b) {
  int as_int = __builtin_bit_cast(int, b);

  return __builtin_bit_cast(v32float16, broadcast_s32(as_int));
}
INTRINSIC(v32float16) broadcast_to_v32float16(v4float16 b) {
  mask64 as_mask64 = __builtin_bit_cast(mask64, b);

  return broadcast_s64(as_mask64);
}

INTRINSIC(v64float8) broadcast_to_v64float8(float8 b) {
  return broadcast_float8(b);
}
INTRINSIC(v64float8) broadcast_to_v64float8(v2float8 b) {
  return {broadcast_s16((short)b.data)};
}
INTRINSIC(v64float8) broadcast_to_v64float8(v4float8 b) {
  return {broadcast_s32(b.data)};
}
INTRINSIC(v64float8) broadcast_to_v64float8(v8float8 b) {
  return {vector_broadcast64((v2int32)b.data)};
}

INTRINSIC(v64bfloat8) broadcast_to_v64bfloat8(bfloat8 b) {
  return broadcast_bfloat8(b);
}
INTRINSIC(v64bfloat8) broadcast_to_v64bfloat8(v2bfloat8 b) {
  return {broadcast_s16((short)b.data)};
}
INTRINSIC(v64bfloat8) broadcast_to_v64bfloat8(v4bfloat8 b) {
  return {broadcast_s32(b.data)};
}
INTRINSIC(v64bfloat8) broadcast_to_v64bfloat8(v8bfloat8 b) {
  return {vector_broadcast64((v2int32)b.data)};
}

INTRINSIC(v16accfloat) broadcast_to_v16accfloat(float b) {
  int as_int = __builtin_bit_cast(int, b);

  return __builtin_bit_cast(v16accfloat, broadcast_s32(as_int));
}
INTRINSIC(v16accfloat) broadcast_to_v16accfloat(v2float b) {
  mask64 as_mask64 = __builtin_bit_cast(mask64, b);

  return broadcast_s64(as_mask64);
}

INTRINSIC(v16float) broadcast_to_v16float(float b) {
  return broadcast_float(b);
}
INTRINSIC(v16float) broadcast_to_v16float(v2float b) {
  mask64 as_mask64 = __builtin_bit_cast(mask64, b);

  return broadcast_s64(as_mask64);
}

INTRINSIC(v64int8) broadcast_one_to_v64int8() { return broadcast_s8(1); }
INTRINSIC(v32int16) broadcast_one_to_v32int16() { return broadcast_s16(1); }
INTRINSIC(v16int32) broadcast_one_to_v16int32() { return broadcast_s32(1); }
INTRINSIC(v64uint8) broadcast_one_to_v64uint8() { return broadcast_u8(1); }
INTRINSIC(v32uint16) broadcast_one_to_v32uint16() { return broadcast_u16(1); }
INTRINSIC(v16uint32) broadcast_one_to_v16uint32() { return broadcast_u32(1); }
INTRINSIC(v32bfloat16) broadcast_one_to_v32bfloat16() {
  return broadcast_bfloat16(1);
}
INTRINSIC(v32float16) broadcast_one_to_v32float16() {
  return broadcast_float16(1);
}
INTRINSIC(v64float8) broadcast_one_to_v64float8() {
  return broadcast_float8({1});
}
INTRINSIC(v64bfloat8) broadcast_one_to_v64bfloat8() {
  return broadcast_bfloat8({1});
}
INTRINSIC(v16float) broadcast_one_to_v16float() { return broadcast_float16(1); }
[[deprecated("Function 'broadcast_one_s8' is deprecated. Please use the "
             "'broadcast_one_to_v64int8' variant instead.")]] INTRINSIC(v64int8)
    broadcast_one_s8() {
  return broadcast_s8(1);
}

[[deprecated(
    "Function 'broadcast_one_s16' is deprecated. Please use the "
    "'broadcast_one_to_v32int16' variant instead.")]] INTRINSIC(v32int16)
    broadcast_one_s16() {
  return broadcast_s16(1);
}

[[deprecated(
    "Function 'broadcast_one_s32' is deprecated. Please use the "
    "'broadcast_one_to_v16int32' variant instead.")]] INTRINSIC(v16int32)
    broadcast_one_s32() {
  return broadcast_s32(1);
}

[[deprecated(
    "Function 'broadcast_one_u8' is deprecated. Please use the "
    "'broadcast_one_to_v64uint8' variant instead.")]] INTRINSIC(v64uint8)
    broadcast_one_u8() {
  return broadcast_u8(1);
}

[[deprecated(
    "Function 'broadcast_one_u16' is deprecated. Please use the "
    "'broadcast_one_to_v32uint16' variant instead.")]] INTRINSIC(v32uint16)
    broadcast_one_u16() {
  return broadcast_u16(1);
}

[[deprecated(
    "Function 'broadcast_one_u32' is deprecated. Please use the "
    "'broadcast_one_to_v16uint32' variant instead.")]] INTRINSIC(v16uint32)
    broadcast_one_u32() {
  return broadcast_u32(1);
}

[[deprecated(
    "Function 'broadcast_one_bfloat16' is deprecated. Please use the "
    "'broadcast_one_to_v32bfloat16' variant instead.")]] INTRINSIC(v32bfloat16)
    broadcast_one_bfloat16() {
  return (v32bfloat16)broadcast_u16(16256);
}
INTRINSIC(v64int8) broadcast_zero_to_v64int8() { return broadcast_s8(0); }
INTRINSIC(v32int16) broadcast_zero_to_v32int16() { return broadcast_s16(0); }
INTRINSIC(v16int32) broadcast_zero_to_v16int32() { return broadcast_s32(0); }
INTRINSIC(v64uint8) broadcast_zero_to_v64uint8() { return broadcast_u8(0); }
INTRINSIC(v32uint16) broadcast_zero_to_v32uint16() { return broadcast_u16(0); }
INTRINSIC(v16uint32) broadcast_zero_to_v16uint32() { return broadcast_u32(0); }
INTRINSIC(v32bfloat16) broadcast_zero_to_v32bfloat16() {
  return broadcast_bfloat16(0);
}
INTRINSIC(v32float16) broadcast_zero_to_v32float16() {
  return broadcast_float16(0);
}
INTRINSIC(v64float8) broadcast_zero_to_v64float8() {
  return broadcast_float8({0});
}
INTRINSIC(v64bfloat8) broadcast_zero_to_v64bfloat8() {
  return broadcast_bfloat8({0});
}
INTRINSIC(v16float) broadcast_zero_to_v16float() { return broadcast_float(0); }
[[deprecated(
    "Function 'broadcast_zero_s8' is deprecated. Please use the "
    "'broadcast_zero_to_v64int8' variant instead.")]] INTRINSIC(v64int8)
    broadcast_zero_s8() {
  return broadcast_s8(0);
}

[[deprecated(
    "Function 'broadcast_zero_s16' is deprecated. Please use the "
    "'broadcast_zero_to_v32int16' variant instead.")]] INTRINSIC(v32int16)
    broadcast_zero_s16() {
  return broadcast_s16(0);
}

[[deprecated(
    "Function 'broadcast_zero_s32' is deprecated. Please use the "
    "'broadcast_zero_to_v16int32' variant instead.")]] INTRINSIC(v16int32)
    broadcast_zero_s32() {
  return broadcast_s32(0);
}

[[deprecated(
    "Function 'broadcast_zero_u8' is deprecated. Please use the "
    "'broadcast_zero_to_v64uint8' variant instead.")]] INTRINSIC(v64uint8)
    broadcast_zero_u8() {
  return broadcast_u8(0);
}

[[deprecated(
    "Function 'broadcast_zero_u16' is deprecated. Please use the "
    "'broadcast_zero_to_v32uint16' variant instead.")]] INTRINSIC(v32uint16)
    broadcast_zero_u16() {
  return broadcast_u16(0);
}

[[deprecated(
    "Function 'broadcast_zero_u32' is deprecated. Please use the "
    "'broadcast_zero_to_v16uint32' variant instead.")]] INTRINSIC(v16uint32)
    broadcast_zero_u32() {
  return broadcast_u32(0);
}

[[deprecated(
    "Function 'broadcast_zero_bfloat16' is deprecated. Please use the "
    "'broadcast_zero_to_v32bfloat16' variant instead.")]] INTRINSIC(v32bfloat16)
    broadcast_zero_bfloat16() {
  return (v32bfloat16)broadcast_u16(0);
}
INTRINSIC(v64int8) broadcast_elem(v64int8 v, int idx) {
  return broadcast_s8(ext_elem(v, idx, 0));
}
INTRINSIC(v32int16) broadcast_elem(v32int16 v, int idx) {
  return broadcast_s16(ext_elem(v, idx, 0));
}
INTRINSIC(v16int32) broadcast_elem(v16int32 v, int idx) {
  return broadcast_s32(ext_elem(v, idx, 0));
}
INTRINSIC(v16int32) broadcast_elem_s64(v16int32 v, int idx) {
  return vector_broadcast64(ext_v2int32(v, idx, 0));
}
INTRINSIC(v16int32) broadcast_elem_128(v16int32 v, int idx) {
  return __builtin_aie2ps_vextract_broadcast128_I512(v, idx);
}

INTRINSIC(v64uint8) broadcast_elem(v64uint8 v, int idx) {
  return broadcast_u8(ext_elem(v, idx, 0));
}
INTRINSIC(v32uint16) broadcast_elem(v32uint16 v, int idx) {
  return broadcast_u16(ext_elem(v, idx, 0));
}
INTRINSIC(v16uint32) broadcast_elem(v16uint32 v, int idx) {
  return broadcast_u32(ext_elem(v, idx, 0));
}
INTRINSIC(v16uint32) broadcast_elem_s64(v16uint32 v, int idx) {
  return vector_broadcast64(ext_v2uint32(v, idx, 0));
}

INTRINSIC(v32bfloat16) broadcast_elem(v32bfloat16 v, int idx) {
  return broadcast_bfloat16(ext_elem(v, idx, 0));
}
INTRINSIC(v32float16) broadcast_elem(v32float16 v, int idx) {
  return broadcast_float16(ext_elem(v, idx, 0));
}

INTRINSIC(v16float) broadcast_elem(v16float v, int idx) {
  return vector_broadcast64(ext_v2int32(v, idx, 0));
}

INTRINSIC(v64float8) broadcast_elem(v64float8 v, int idx) {
  return broadcast_float8(ext_elem(v, idx, 0));
}
INTRINSIC(v64bfloat8) broadcast_elem(v64bfloat8 v, int idx) {
  return broadcast_bfloat8(ext_elem(v, idx, 0));
}
template <typename T, typename Y> inline T vector_insert(T a, int idx, Y b) {
  a[idx] = b;

  return a;
}
template <typename T, typename Y> inline T vector_insert64(T a, int idx, Y b) {
  v2int32 Temp = (v2int32)b;

  idx *= 2;

  a[idx] = Temp[0];

  a[idx + 1] = Temp[1];

  return a;
}

INTRINSIC(v128int4) upd_elem(v128int4 v, int idx, v2int4 b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v128int4) upd_elem(v128int4 v, int idx, v4int4 b) {
  return vector_insert((v32int16)v, idx, (short)b);
}
INTRINSIC(v128int4) upd_elem(v128int4 v, int idx, v8int4 b) {
  return vector_insert((v16int32)v, idx, (int)b);
}
INTRINSIC(v128int4) upd_elem(v128int4 v, int idx, v16int4 b) {
  return vector_insert64((v16int32)v, idx, b);
}
INTRINSIC(v128int4) upd_elem(v128int4 v, int idx, mask64 b) {
  return vector_insert64((v16int32)v, idx, (v2int32)b);
}

INTRINSIC(v64int8) upd_elem(v64int8 v, int idx, int b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v64int8) upd_elem(v64int8 v, int idx, v2int8 b) {
  return vector_insert((v32int16)v, idx, (short)b);
}
INTRINSIC(v64int8) upd_elem(v64int8 v, int idx, v4int8 b) {
  return vector_insert((v16int32)v, idx, (int)b);
}
INTRINSIC(v64int8) upd_elem(v64int8 v, int idx, v8int8 b) {
  return vector_insert64((v16int32)v, idx, b);
}
INTRINSIC(v64int8) upd_elem(v64int8 v, int idx, mask64 b) {
  return vector_insert64((v16int32)v, idx, (v2int32)b);
}

INTRINSIC(v32int16) upd_elem(v32int16 v, int idx, int b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v32int16) upd_elem(v32int16 v, int idx, v2int16 b) {
  return vector_insert((v16int32)v, idx, (int)b);
}
INTRINSIC(v32int16) upd_elem(v32int16 v, int idx, v4int16 b) {
  return vector_insert64((v16int32)v, idx, b);
}
INTRINSIC(v32int16) upd_elem(v32int16 v, int idx, mask64 b) {
  return vector_insert64((v16int32)v, idx, (v2int32)b);
}

INTRINSIC(v16int32) upd_elem(v16int32 v, int idx, int b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v16int32) upd_elem(v16int32 v, int idx, v2int32 b) {
  return vector_insert64(v, idx, b);
}
INTRINSIC(v16int32) upd_elem(v16int32 v, int idx, mask64 b) {
  return vector_insert64(v, idx, (v2int32)b);
}

INTRINSIC(v128uint4) upd_elem(v128uint4 v, int idx, v2uint4 b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v128uint4) upd_elem(v128uint4 v, int idx, v4uint4 b) {
  return vector_insert((v32int16)v, idx, (unsigned short)b);
}
INTRINSIC(v128uint4) upd_elem(v128uint4 v, int idx, v8uint4 b) {
  return vector_insert((v16int32)v, idx, (unsigned int)b);
}
INTRINSIC(v128uint4) upd_elem(v128uint4 v, int idx, v16uint4 b) {
  return vector_insert64((v16int32)v, idx, b);
}
INTRINSIC(v128uint4) upd_elem(v128uint4 v, int idx, mask64 b) {
  return vector_insert64((v16int32)v, idx, (v2uint32)b);
}

INTRINSIC(v64uint8) upd_elem(v64uint8 v, int idx, unsigned int b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v64uint8) upd_elem(v64uint8 v, int idx, v2uint8 b) {
  return vector_insert((v32int16)v, idx, (unsigned short)b);
}
INTRINSIC(v64uint8) upd_elem(v64uint8 v, int idx, v4uint8 b) {
  return vector_insert((v16int32)v, idx, (unsigned int)b);
}
INTRINSIC(v64uint8) upd_elem(v64uint8 v, int idx, v8uint8 b) {
  return vector_insert64((v16int32)v, idx, b);
}
INTRINSIC(v64uint8) upd_elem(v64uint8 v, int idx, mask64 b) {
  return vector_insert64((v16int32)v, idx, (v2uint32)b);
}

INTRINSIC(v32uint16) upd_elem(v32uint16 v, int idx, unsigned int b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v32uint16) upd_elem(v32uint16 v, int idx, v2uint16 b) {
  return vector_insert((v16int32)v, idx, (unsigned int)b);
}
INTRINSIC(v32uint16) upd_elem(v32uint16 v, int idx, v4uint16 b) {
  return vector_insert64((v16int32)v, idx, b);
}
INTRINSIC(v32uint16) upd_elem(v32uint16 v, int idx, mask64 b) {
  return vector_insert64((v16int32)v, idx, (v2uint32)b);
}

INTRINSIC(v16uint32) upd_elem(v16uint32 v, int idx, unsigned int b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v16uint32) upd_elem(v16uint32 v, int idx, v2uint32 b) {
  return vector_insert64(v, idx, b);
}
INTRINSIC(v32bfloat16) upd_elem(v32bfloat16 v, int idx, bfloat16 b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v32bfloat16) upd_elem(v32bfloat16 v, int idx, v2bfloat16 b) {
  return __builtin_aie2ps_vinsert_bf32_bf512(v, idx, b);
}
INTRINSIC(v32bfloat16) upd_elem(v32bfloat16 v, int idx, v4bfloat16 b) {
  return __builtin_aie2ps_vinsert_bf64_bf512(v, idx, b);
}

INTRINSIC(v32float16) upd_elem(v32float16 v, int idx, float16 b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v32float16) upd_elem(v32float16 v, int idx, v2float16 b) {
  return vector_insert(__builtin_bit_cast(v16float, v), idx,
                       __builtin_bit_cast(float, b));
}
INTRINSIC(v32float16) upd_elem(v32float16 v, int idx, v4float16 b) {
  return vector_insert64(__builtin_bit_cast(v16int32, v), idx,
                         __builtin_bit_cast(v2int32, b));
}

INTRINSIC(v64float8) upd_elem(v64float8 v, int idx, float8 b) {
  return {vector_insert(v.data, idx, b.data)};
}
INTRINSIC(v64float8) upd_elem(v64float8 v, int idx, v2float8 b) {
  return {vector_insert((v32int16)v.data, idx, (short)b.data)};
}
INTRINSIC(v64float8) upd_elem(v64float8 v, int idx, v4float8 b) {
  return {vector_insert((v16int32)v.data, idx, b.data)};
}
INTRINSIC(v64float8) upd_elem(v64float8 v, int idx, v8float8 b) {
  return {vector_insert64((v16int32)v.data, idx, b.data)};
}

INTRINSIC(v64bfloat8) upd_elem(v64bfloat8 v, int idx, bfloat8 b) {
  return {vector_insert(v.data, idx, b.data)};
}
INTRINSIC(v64bfloat8) upd_elem(v64bfloat8 v, int idx, v2bfloat8 b) {
  return {vector_insert((v32int16)v.data, idx, (short)b.data)};
}
INTRINSIC(v64bfloat8) upd_elem(v64bfloat8 v, int idx, v4bfloat8 b) {
  return {vector_insert((v16int32)v.data, idx, b.data)};
}
INTRINSIC(v64bfloat8) upd_elem(v64bfloat8 v, int idx, v8bfloat8 b) {
  return {vector_insert64((v16int32)v.data, idx, b.data)};
}

INTRINSIC(v16float) upd_elem(v16float v, int idx, float b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v16float) upd_elem(v16float v, int idx, v2float b) {
  return vector_insert64(__builtin_bit_cast(v16int32, v), idx,
                         __builtin_bit_cast(v2int32, b));
}

INTRINSIC(v128int4) insert(v128int4 v, int idx, v2int4 b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v128int4) insert(v128int4 v, int idx, v4int4 b) {
  return vector_insert((v32int16)v, idx, (short)b);
}
INTRINSIC(v128int4) insert(v128int4 v, int idx, v8int4 b) {
  return vector_insert((v16int32)v, idx, (int)b);
}
INTRINSIC(v128int4) insert(v128int4 v, int idx, v16int4 b) {
  return vector_insert64((v16int32)v, idx, b);
}
INTRINSIC(v128int4) insert(v128int4 v, int idx, mask64 b) {
  return vector_insert64((v16int32)v, idx, (v2int32)b);
}

INTRINSIC(v64int8) insert(v64int8 v, int idx, char b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v64int8) insert(v64int8 v, int idx, signed char b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v64int8) insert(v64int8 v, int idx, v2int8 b) {
  return vector_insert((v32int16)v, idx, (short)b);
}
INTRINSIC(v64int8) insert(v64int8 v, int idx, v4int8 b) {
  return vector_insert((v16int32)v, idx, (int)b);
}
INTRINSIC(v64int8) insert(v64int8 v, int idx, v8int8 b) {
  return vector_insert64((v16int32)v, idx, b);
}
INTRINSIC(v64int8) insert(v64int8 v, int idx, mask64 b) {
  return vector_insert64((v16int32)v, idx, (v2int32)b);
}

INTRINSIC(v32int16) insert(v32int16 v, int idx, short b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v32int16) insert(v32int16 v, int idx, v2int16 b) {
  return vector_insert((v16int32)v, idx, (int)b);
}
INTRINSIC(v32int16) insert(v32int16 v, int idx, v4int16 b) {
  return vector_insert64((v16int32)v, idx, b);
}
INTRINSIC(v32int16) insert(v32int16 v, int idx, mask64 b) {
  return vector_insert64((v16int32)v, idx, (v2int32)b);
}

INTRINSIC(v16int32) insert(v16int32 v, int idx, int b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v16int32) insert(v16int32 v, int idx, v2int32 b) {
  return vector_insert64(v, idx, b);
}
INTRINSIC(v16int32) insert(v16int32 v, int idx, mask64 b) {
  return vector_insert64(v, idx, (v2int32)b);
}

INTRINSIC(v128uint4) insert(v128uint4 v, int idx, v2uint4 b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v128uint4) insert(v128uint4 v, int idx, v4uint4 b) {
  return vector_insert((v32int16)v, idx, (unsigned short)b);
}
INTRINSIC(v128uint4) insert(v128uint4 v, int idx, v8uint4 b) {
  return vector_insert((v16int32)v, idx, (unsigned int)b);
}
INTRINSIC(v128uint4) insert(v128uint4 v, int idx, v16uint4 b) {
  return vector_insert64((v16int32)v, idx, b);
}
INTRINSIC(v128uint4) insert(v128uint4 v, int idx, mask64 b) {
  return vector_insert64((v16int32)v, idx, (v2uint32)b);
}

INTRINSIC(v64uint8) insert(v64uint8 v, int idx, unsigned char b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v64uint8) insert(v64uint8 v, int idx, v2uint8 b) {
  return vector_insert((v32int16)v, idx, (unsigned short)b);
}
INTRINSIC(v64uint8) insert(v64uint8 v, int idx, v4uint8 b) {
  return vector_insert((v16int32)v, idx, (unsigned int)b);
}
INTRINSIC(v64uint8) insert(v64uint8 v, int idx, v8uint8 b) {
  return vector_insert64((v16int32)v, idx, b);
}
INTRINSIC(v64uint8) insert(v64uint8 v, int idx, mask64 b) {
  return vector_insert64((v16int32)v, idx, (v2uint32)b);
}

INTRINSIC(v32uint16) insert(v32uint16 v, int idx, unsigned short b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v32uint16) insert(v32uint16 v, int idx, v2uint16 b) {
  return vector_insert((v16int32)v, idx, (unsigned int)b);
}
INTRINSIC(v32uint16) insert(v32uint16 v, int idx, v4uint16 b) {
  return vector_insert64((v16int32)v, idx, b);
}
INTRINSIC(v32uint16) insert(v32uint16 v, int idx, mask64 b) {
  return vector_insert64((v16int32)v, idx, (v2uint32)b);
}

INTRINSIC(v16uint32) insert(v16uint32 v, int idx, unsigned int b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v16uint32) insert(v16uint32 v, int idx, v2uint32 b) {
  return vector_insert64(v, idx, b);
}
INTRINSIC(v32bfloat16) insert(v32bfloat16 v, int idx, bfloat16 b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v32bfloat16) insert(v32bfloat16 v, int idx, v2bfloat16 b) {
  return __builtin_aie2ps_vinsert_bf32_bf512(v, idx, b);
}
INTRINSIC(v32bfloat16) insert(v32bfloat16 v, int idx, v4bfloat16 b) {
  return __builtin_aie2ps_vinsert_bf64_bf512(v, idx, b);
}
INTRINSIC(v32bfloat16) insert(v32bfloat16 v, int idx, mask64 b) {
  return __builtin_aie2ps_vinsert_bf64_bf512(v, idx, (v4bfloat16)b);
}

INTRINSIC(v32float16) insert(v32float16 v, int idx, float16 b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v32float16) insert(v32float16 v, int idx, v2float16 b) {
  return vector_insert(__builtin_bit_cast(v16float, v), idx,
                       __builtin_bit_cast(float, b));
}
INTRINSIC(v32float16) insert(v32float16 v, int idx, v4float16 b) {
  return vector_insert64(__builtin_bit_cast(v16int32, v), idx,
                         __builtin_bit_cast(v2int32, b));
}
INTRINSIC(v32float16) insert(v32float16 v, int idx, mask64 b) {
  return vector_insert64(__builtin_bit_cast(v16int32, v), idx, b);
}

INTRINSIC(v64float8) insert(v64float8 v, int idx, float8 b) {
  return {vector_insert(v.data, idx, b.data)};
}
INTRINSIC(v64float8) insert(v64float8 v, int idx, v2float8 b) {
  return {vector_insert((v32int16)v.data, idx, (short)b.data)};
}
INTRINSIC(v64float8) insert(v64float8 v, int idx, v4float8 b) {
  return {vector_insert((v16int32)v.data, idx, b.data)};
}
INTRINSIC(v64float8) insert(v64float8 v, int idx, v8float8 b) {
  return {vector_insert64((v16int32)v.data, idx, b.data)};
}
INTRINSIC(v64float8) insert(v64float8 v, int idx, mask64 b) {
  return {vector_insert64((v16int32)v.data, idx, (v2int32)b)};
}

INTRINSIC(v64bfloat8) insert(v64bfloat8 v, int idx, bfloat8 b) {
  return {vector_insert(v.data, idx, b.data)};
}
INTRINSIC(v64bfloat8) insert(v64bfloat8 v, int idx, v2bfloat8 b) {
  return {vector_insert((v32int16)v.data, idx, (short)b.data)};
}
INTRINSIC(v64bfloat8) insert(v64bfloat8 v, int idx, v4bfloat8 b) {
  return {vector_insert((v16int32)v.data, idx, b.data)};
}
INTRINSIC(v64bfloat8) insert(v64bfloat8 v, int idx, v8bfloat8 b) {
  return {vector_insert64((v16int32)v.data, idx, b.data)};
}
INTRINSIC(v64bfloat8) insert(v64bfloat8 v, int idx, mask64 b) {
  return {vector_insert64((v16int32)v.data, idx, (v2int32)b)};
}

INTRINSIC(v16float) insert(v16float v, int idx, float b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v16float) insert(v16float v, int idx, v2float b) {
  return vector_insert64(__builtin_bit_cast(v16int32, v), idx,
                         __builtin_bit_cast(v2int32, b));
}

INTRINSIC(v32int32) upd_elem(v32int32 v, int idx, int b) {
  return vector_insert(v, idx, b);
}
INTRINSIC(v32acc32) upd_elem(v32acc32 v, int idx, int b) {
  return (v32acc32)vector_insert((v32int32)v, idx, b);
}
INTRINSIC(v16acc64) upd_elem(v16acc64 v, int idx, int b) {
  return (v16acc64)vector_insert64((v32int32)v, idx, (int64_t)b);
}
INTRINSIC(v64int8) shiftl_elem(v64int8 v, int s) {
  return shift_bytes(v, broadcast_s8(s), 1);
}
INTRINSIC(v32int16) shiftl_elem(v32int16 v, int s) {
  return shift_bytes(v, broadcast_s16(s), 2);
}
INTRINSIC(v16int32) shiftl_elem(v16int32 v, int s) {
  return shift_bytes(v, broadcast_s32(s), 4);
}
INTRINSIC(v64uint8) shiftl_elem(v64uint8 v, unsigned int s) {
  return shift_bytes(v, broadcast_u8(s), 1);
}
INTRINSIC(v32uint16) shiftl_elem(v32uint16 v, unsigned int s) {
  return shift_bytes(v, broadcast_u16(s), 2);
}
INTRINSIC(v16uint32) shiftl_elem(v16uint32 v, unsigned int s) {
  return shift_bytes(v, broadcast_u32(s), 4);
}
INTRINSIC(v32bfloat16) shiftl_elem(v32bfloat16 v, bfloat16 s) {
  return shift_bytes(v, broadcast_bfloat16(s), 2);
}
INTRINSIC(v32float16) shiftl_elem(v32float16 v, float16 s) {
  return shift_bytes(v, broadcast_float16(s), 2);
}
INTRINSIC(v64float8) shiftl_elem(v64float8 v, float8 s) {
  return shift_bytes(v, broadcast_float8(s), 1);
}
INTRINSIC(v64bfloat8) shiftl_elem(v64bfloat8 v, bfloat8 s) {
  return shift_bytes(v, broadcast_bfloat8(s), 1);
}
INTRINSIC(v16accfloat) shiftl_elem(v16accfloat v, float s) {
  return shift_bytes(v, (v16accfloat)broadcast_float(s), 4);
}
INTRINSIC(v16float) shiftl_elem(v16float v, float s) {
  return shift_bytes(v, broadcast_float(s), 4);
}
INTRINSIC(v64int8) shiftr_elem(v64int8 v, int s) {
  return shift_bytes(broadcast_s8(s), v, 64 - 1);
}
INTRINSIC(v32int16) shiftr_elem(v32int16 v, int s) {
  return shift_bytes(broadcast_s16(s), v, 64 - 2);
}
INTRINSIC(v16int32) shiftr_elem(v16int32 v, int s) {
  return shift_bytes(broadcast_s32(s), v, 64 - 4);
}
INTRINSIC(v64uint8) shiftr_elem(v64uint8 v, unsigned int s) {
  return shift_bytes(broadcast_u8(s), v, 64 - 1);
}
INTRINSIC(v32uint16) shiftr_elem(v32uint16 v, unsigned int s) {
  return shift_bytes(broadcast_u16(s), v, 64 - 2);
}
INTRINSIC(v16uint32) shiftr_elem(v16uint32 v, unsigned int s) {
  return shift_bytes(broadcast_u32(s), v, 64 - 4);
}
INTRINSIC(v32bfloat16) shiftr_elem(v32bfloat16 v, bfloat16 s) {
  return shift_bytes(broadcast_bfloat16(s), v, 64 - 2);
}
INTRINSIC(v32float16) shiftr_elem(v32float16 v, float16 s) {
  return shift_bytes(broadcast_float16(s), v, 64 - 2);
}
INTRINSIC(v64float8) shiftr_elem(v64float8 v, float8 s) {
  return shift_bytes(broadcast_float8(s), v, 64 - 1);
}
INTRINSIC(v64bfloat8) shiftr_elem(v64bfloat8 v, bfloat8 s) {
  return shift_bytes(broadcast_bfloat8(s), v, 64 - 1);
}
INTRINSIC(v16accfloat) shiftr_elem(v16accfloat v, float s) {
  return shift_bytes((v16accfloat)broadcast_float(s), v, 64 - 4);
}
INTRINSIC(v16float) shiftr_elem(v16float v, float s) {
  return shift_bytes(broadcast_float(s), v, 64 - 4);
}
INTRINSIC(v16acc32) shuffle(v16acc32 a, v16acc32 b, unsigned int mode) {
  return __builtin_aie2ps_vshuffle(a, b, mode);
}
INTRINSIC(v16int32) shuffle(v16int32 a, v16int32 b, unsigned int mode) {
  return __builtin_aie2ps_vshuffle(a, b, mode);
}
INTRINSIC(v32int16) shuffle(v32int16 a, v32int16 b, unsigned int mode) {
  return __builtin_aie2ps_vshuffle(a, b, mode);
}
INTRINSIC(v64int8) shuffle(v64int8 a, v64int8 b, unsigned int mode) {
  return __builtin_aie2ps_vshuffle(a, b, mode);
}
INTRINSIC(v128int4) shuffle(v128int4 a, v128int4 b, unsigned int mode) {
  return __builtin_aie2ps_vshuffle(a, b, mode);
}

INTRINSIC(v16uint32) shuffle(v16uint32 a, v16uint32 b, unsigned int mode) {
  return __builtin_aie2ps_vshuffle(a, b, mode);
}
INTRINSIC(v32uint16) shuffle(v32uint16 a, v32uint16 b, unsigned int mode) {
  return __builtin_aie2ps_vshuffle(a, b, mode);
}
INTRINSIC(v64uint8) shuffle(v64uint8 a, v64uint8 b, unsigned int mode) {
  return __builtin_aie2ps_vshuffle(a, b, mode);
}
INTRINSIC(v128uint4) shuffle(v128uint4 a, v128uint4 b, unsigned int mode) {
  return __builtin_aie2ps_vshuffle(a, b, mode);
}
INTRINSIC(v32bfloat16)
shuffle(v32bfloat16 a, v32bfloat16 b, unsigned int mode) {
  return __builtin_aie2ps_vshuffle(__builtin_bit_cast(v16int32, a),
                                   __builtin_bit_cast(v16int32, b), mode);
}
INTRINSIC(v32float16) shuffle(v32float16 a, v32float16 b, unsigned int mode) {
  return __builtin_aie2ps_vshuffle(__builtin_bit_cast(v16int32, a),
                                   __builtin_bit_cast(v16int32, b), mode);
}
INTRINSIC(v64float8) shuffle(v64float8 a, v64float8 b, unsigned int mode) {
  return {__builtin_aie2ps_vshuffle(a.data, b.data, mode)};
}
INTRINSIC(v64bfloat8) shuffle(v64bfloat8 a, v64bfloat8 b, unsigned int mode) {
  return {__builtin_aie2ps_vshuffle(a.data, b.data, mode)};
}
INTRINSIC(v16float) shuffle(v16float a, v16float b, unsigned int mode) {
  return __builtin_aie2ps_vshuffle(a, b, mode);
}
INTRINSIC(v16int32) shuffle(v16int32 a, unsigned int mode) {
  return shuffle(a, undef_v16int32(), mode);
}
INTRINSIC(v32int16) shuffle(v32int16 a, unsigned int mode) {
  return shuffle(a, undef_v32int16(), mode);
}
INTRINSIC(v64int8) shuffle(v64int8 a, unsigned int mode) {
  return shuffle(a, undef_v64int8(), mode);
}
INTRINSIC(v128int4) shuffle(v128int4 a, unsigned int mode) {
  return shuffle(a, undef_v128int4(), mode);
}

INTRINSIC(v16uint32) shuffle(v16uint32 a, unsigned int mode) {
  return shuffle(a, undef_v16uint32(), mode);
}
INTRINSIC(v32uint16) shuffle(v32uint16 a, unsigned int mode) {
  return shuffle(a, undef_v32uint16(), mode);
}
INTRINSIC(v64uint8) shuffle(v64uint8 a, unsigned int mode) {
  return shuffle(a, undef_v64uint8(), mode);
}
INTRINSIC(v128uint4) shuffle(v128uint4 a, unsigned int mode) {
  return shuffle(a, undef_v128uint4(), mode);
}

INTRINSIC(v32bfloat16) shuffle(v32bfloat16 a, unsigned int mode) {
  return shuffle(a, undef_v32bfloat16(), mode);
}
INTRINSIC(v32float16) shuffle(v32float16 a, unsigned int mode) {
  return shuffle(a, undef_v32float16(), mode);
}
INTRINSIC(v64float8) shuffle(v64float8 a, unsigned int mode) {
  return shuffle(a, undef_v64float8(), mode);
}
INTRINSIC(v64bfloat8) shuffle(v64bfloat8 a, unsigned int mode) {
  return shuffle(a, undef_v64bfloat8(), mode);
}
INTRINSIC(v16float) shuffle(v16float a, unsigned int mode) {
  return shuffle(a, undef_v16float(), mode);
}
INTRINSIC(v64int8) shuffle_s8(int b, unsigned int mode) {
  return __builtin_aie2ps_vbcst_shuffle8(b, mode);
}
INTRINSIC(v32int16) shuffle_s16(int b, unsigned int mode) {
  return __builtin_aie2ps_vbcst_shuffle16(b, mode);
}
INTRINSIC(v16int32) shuffle_s32(int b, unsigned int mode) {
  return __builtin_aie2ps_vbcst_shuffle32(b, mode);
}
INTRINSIC(v16int32) shuffle_s64(mask64 b, unsigned int mode) {
  return __builtin_aie2ps_vbcst_shuffle64((v2int32)b, mode);
}
INTRINSIC(v16int32) shuffle_v2s32(v2int32 b, unsigned int mode) {
  return __builtin_aie2ps_vbcst_shuffle64(b, mode);
}
INTRINSIC(v64uint8) shuffle_u8(unsigned int b, unsigned int mode) {
  return __builtin_aie2ps_vbcst_shuffle8(b, mode);
}
INTRINSIC(v32uint16) shuffle_u16(unsigned int b, unsigned int mode) {
  return __builtin_aie2ps_vbcst_shuffle16(b, mode);
}
INTRINSIC(v16uint32) shuffle_u32(unsigned int b, unsigned int mode) {
  return __builtin_aie2ps_vbcst_shuffle32(b, mode);
}
INTRINSIC(v16uint32) shuffle_u64(mask64 b, unsigned int mode) {
  return __builtin_aie2ps_vbcst_shuffle64((v2uint32)b, mode);
}
INTRINSIC(v16uint32) shuffle_v2u32(v2uint32 b, unsigned int mode) {
  return __builtin_aie2ps_vbcst_shuffle64(b, mode);
}
INTRINSIC(v32bfloat16) shuffle_bfloat16(bfloat16 b, unsigned int mode) {
  return __builtin_aie2ps_vbcst_shuffle16((int)__builtin_bit_cast(short, b),
                                          mode);
}
INTRINSIC(v32float16) shuffle_float16(float16 b, unsigned int mode) {
  return __builtin_aie2ps_vbcst_shuffle16((int)__builtin_bit_cast(short, b),
                                          mode);
}
INTRINSIC(v64float8) shuffle_float8(float8 b, unsigned int mode) {
  return {__builtin_aie2ps_vbcst_shuffle8((int)b.data, mode)};
}
INTRINSIC(v64bfloat8) shuffle_bfloat8(bfloat8 b, unsigned int mode) {
  return {__builtin_aie2ps_vbcst_shuffle8((int)b.data, mode)};
}
INTRINSIC(v16float) shuffle_float(float b, unsigned int mode) {
  return __builtin_aie2ps_vbcst_shuffle32(__builtin_bit_cast(int, b), mode);
}
INTRINSIC(v64mx9) shuffle(v64mx9 a, v64mx9 b, unsigned mode) {
  v64mx9 res;

  __builtin_aie2ps_vshuffle_BFP640_BFP640_BFP640(
      a.mantissa, a.tileShift, a.exponent, b.mantissa, b.tileShift, b.exponent,
      mode, (v16int32 &)res.mantissa, (v2int32 &)res.tileShift,
      (v2int32 &)res.exponent);

  return res;
}
INTRINSIC(v128mx6) shuffle(v128mx6 a, v128mx6 b, unsigned mode) {
  v128mx6 res;

  __builtin_aie2ps_vshuffle_BFP768_BFP768_BFP768(
      a.mantissaX0, a.mantissaX1, a.signF0, a.signF1, a.tileShiftG0,
      a.tileShiftG1, a.exponentE0, a.exponentE1, b.mantissaX0, b.mantissaX1,
      b.signF0, b.signF1, b.tileShiftG0, b.tileShiftG1, b.exponentE0,
      b.exponentE1, mode, (v8int32 &)res.mantissaX0, (v8int32 &)res.mantissaX1,
      (v2int32 &)res.signF0, (v2int32 &)res.signF1, (int32 &)res.tileShiftG0,
      (int32 &)res.tileShiftG1, (int32 &)res.exponentE0,
      (int32 &)res.exponentE1);

  return res;
}
INTRINSIC(v128mx4) shuffle(v128mx4 a, v128mx4 b, unsigned mode) {
  v128mx4 res;

  __builtin_aie2ps_vshuffle_BFP768_BFP768_BFP768(
      a.mantissaX0, a.mantissaX1, a.signF0, a.signF1, a.tileShiftG0,
      a.tileShiftG1, a.exponentE0, a.exponentE1, b.mantissaX0, b.mantissaX1,
      b.signF0, b.signF1, b.tileShiftG0, b.tileShiftG1, b.exponentE0,
      b.exponentE1, mode, (v8int32 &)res.mantissaX0, (v8int32 &)res.mantissaX1,
      (v2int32 &)res.signF0, (v2int32 &)res.signF1, (int32 &)res.tileShiftG0,
      (int32 &)res.tileShiftG1, (int32 &)res.exponentE0,
      (int32 &)res.exponentE1);

  return res;
}
INTRINSIC(v64mx9) shuffle(v64mx9 a, unsigned mode) {
  return shuffle(a, undef_v64mx9(), mode);
}
INTRINSIC(v128mx6) shuffle(v128mx6 a, unsigned mode) {
  return shuffle(a, undef_v128mx6(), mode);
}
INTRINSIC(v128mx4) shuffle(v128mx4 a, unsigned mode) {
  return shuffle(a, undef_v128mx4(), mode);
}

#endif // AIE2PS_SCL2VEC_H
