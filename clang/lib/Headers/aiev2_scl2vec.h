//===- aiev2_scl2vec.h ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV2_SCL2VEC_H__
#define __AIEV2_SCL2VEC_H__

INTRINSIC(v128int4) shiftx(v128int4 a, v128int4 b, int step, int shift) {
  return __builtin_aiev2_vshift_I512_I512(a, b, step, shift);
}
INTRINSIC(v64int8) shiftx(v64int8 a, v64int8 b, int step, int shift) {
  return __builtin_aiev2_vshift_I512_I512(a, b, step, shift);
}
INTRINSIC(v32int16) shiftx(v32int16 a, v32int16 b, int step, int shift) {
  return __builtin_aiev2_vshift_I512_I512(a, b, step, shift);
}
INTRINSIC(v16int32) shiftx(v16int32 a, v16int32 b, int step, int shift) {
  return __builtin_aiev2_vshift_I512_I512(a, b, step, shift);
}
INTRINSIC(v128uint4) shiftx(v128uint4 a, v128uint4 b, int step, int shift) {
  return __builtin_aiev2_vshift_I512_I512(a, b, step, shift);
}
INTRINSIC(v64uint8) shiftx(v64uint8 a, v64uint8 b, int step, int shift) {
  return __builtin_aiev2_vshift_I512_I512(a, b, step, shift);
}
INTRINSIC(v32uint16) shiftx(v32uint16 a, v32uint16 b, int step, int shift) {
  return __builtin_aiev2_vshift_I512_I512(a, b, step, shift);
}
INTRINSIC(v16uint32) shiftx(v16uint32 a, v16uint32 b, int step, int shift) {
  return __builtin_aiev2_vshift_I512_I512(a, b, step, shift);
}
#if 0
INTRINSIC(v16cint16) shiftx(v16cint16 a, v16cint16 b, int step, int shift) {
    return __builtin_aiev2_vshift_I512_I512(a, b, step, shift);
}
INTRINSIC(v8cint32) shiftx(v8cint32 a, v8cint32 b, int step, int shift) {
    return __builtin_aiev2_vshift_I512_I512(a, b, step, shift);
}
#endif
INTRINSIC(v32bfloat16)
shiftx(v32bfloat16 a, v32bfloat16 b, int step, int shift) {
  return __builtin_aiev2_vshift_bf512_bf512(a, b, step, shift);
}
INTRINSIC(v16accfloat)
shiftx(v16accfloat a, v16accfloat b, int step, int shift) {
  return __builtin_aiev2_vshift_I512_I512(a, b, step, shift);
}
#if 0
INTRINSIC(v16float) shiftx(v16float a, v16float b, int step, int shift) {
    return __builtin_aiev2_vshift_I512_I512(a, b, step, shift);
}
#endif

INTRINSIC(v128int4) shift_bytes(v128int4 a, v128int4 b, int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v64int8) shift_bytes(v64int8 a, v64int8 b, int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v32int16) shift_bytes(v32int16 a, v32int16 b, int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v16int32) shift_bytes(v16int32 a, v16int32 b, int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v128uint4) shift_bytes(v128uint4 a, v128uint4 b, int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v64uint8) shift_bytes(v64uint8 a, v64uint8 b, int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v32uint16) shift_bytes(v32uint16 a, v32uint16 b, int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v16uint32) shift_bytes(v16uint32 a, v16uint32 b, int shift) {
  return shiftx(a, b, 0, shift);
}
#if 0
INTRINSIC(v16cint16) shift_bytes(v16cint16 a, v16cint16 b, int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v8cint32) shift_bytes(v8cint32 a, v8cint32 b, int shift) {
  return shiftx(a, b, 0, shift);
}
#endif
INTRINSIC(v32bfloat16) shift_bytes(v32bfloat16 a, v32bfloat16 b, int shift) {
  return shiftx(a, b, 0, shift);
}
INTRINSIC(v16accfloat) shift_bytes(v16accfloat a, v16accfloat b, int shift) {
  return shiftx(a, b, 0, shift);
}
#if 0
INTRINSIC(v16float) shift_bytes(v16float a, v16float b, int shift) {
  return shiftx(a, b, 0, shift);
}
#endif

INTRINSIC(v64int8) shift(v64int8 a, v64int8 b, int shift) {
  return shiftx(a, b, 0, shift * 1);
}
INTRINSIC(v32int16) shift(v32int16 a, v32int16 b, int shift) {
  return shiftx(a, b, 0, shift * 2);
}
INTRINSIC(v16int32) shift(v16int32 a, v16int32 b, int shift) {
  return shiftx(a, b, 0, shift * 4);
}
INTRINSIC(v64uint8) shift(v64uint8 a, v64uint8 b, int shift) {
  return shiftx(a, b, 0, shift * 1);
}
INTRINSIC(v32uint16) shift(v32uint16 a, v32uint16 b, int shift) {
  return shiftx(a, b, 0, shift * 2);
}
INTRINSIC(v16uint32) shift(v16uint32 a, v16uint32 b, int shift) {
  return shiftx(a, b, 0, shift * 4);
}
#if 0
INTRINSIC(v16cint16) shift(v16cint16 a, v16cint16 b, int shift) {
  return shiftx(a, b, 0, shift * 4);
}
INTRINSIC(v8cint32) shift(v8cint32 a, v8cint32 b, int shift) {
  return shiftx(a, b, 0, shift * 8);
}
#endif
INTRINSIC(v32bfloat16) shift(v32bfloat16 a, v32bfloat16 b, int shift) {
  return shiftx(a, b, 0, shift * 2);
}
INTRINSIC(v16accfloat) shift(v16accfloat a, v16accfloat b, int shift) {
  return shiftx(a, b, 0, shift * 4);
}
#if 0
INTRINSIC(v16float) shift(v16float a, v16float b, int shift) {
  return shiftx(a, b, 0, shift * 4);
}
#endif

INTRINSIC(v64int8) upd_elem(v64int8 v, int idx, char b) {
  return __builtin_aiev2_vinsert8_I512(v, idx, b);
}
INTRINSIC(v32int16) upd_elem(v32int16 v, int idx, short b) {
  return __builtin_aiev2_vinsert16_I512(v, idx, b);
}
INTRINSIC(v16int32) upd_elem(v16int32 v, int idx, int b) {
  return __builtin_aiev2_vinsert32_I512(v, idx, b);
}
INTRINSIC(v64uint8) upd_elem(v64uint8 v, int idx, unsigned char b) {
  return __builtin_aiev2_vinsert8_I512(v, idx, b);
}
INTRINSIC(v32uint16) upd_elem(v32uint16 v, int idx, unsigned short b) {
  return __builtin_aiev2_vinsert16_I512(v, idx, b);
}
INTRINSIC(v16uint32) upd_elem(v16uint32 v, int idx, unsigned int b) {
  return __builtin_aiev2_vinsert32_I512(v, idx, b);
}
INTRINSIC(v32bfloat16) upd_elem(v32bfloat16 v, int idx, bfloat16 b) {
  return __builtin_aiev2_vinsert_bf16_bf512(v, idx, b);
}
#if 0
INTRINSIC(v16cint16) upd_elem(v16cint16 v, int idx, cint16 b) {
  return __builtin_aiev2_vinsert32_I512(v, idx, b);
}
INTRINSIC(v8cint32) upd_elem(v8cint32 v, int idx, cint32 b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, b);
}
#endif

INTRINSIC(v128int4) insert(v128int4 v, int idx, v2int4 b) {
  return __builtin_aiev2_vinsert8_I512(v, idx, b);
}
INTRINSIC(v128int4) insert(v128int4 v, int idx, v4int4 b) {
  return __builtin_aiev2_vinsert16_I512(v, idx, (short)b);
}
INTRINSIC(v128int4) insert(v128int4 v, int idx, v8int4 b) {
  return __builtin_aiev2_vinsert32_I512(v, idx, (int)b);
}
INTRINSIC(v128int4) insert(v128int4 v, int idx, v16int4 b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, b);
}
INTRINSIC(v64int8) insert(v64int8 v, int idx, char b) {
  return __builtin_aiev2_vinsert8_I512(v, idx, b);
}
INTRINSIC(v64int8) insert(v64int8 v, int idx, signed char b) {
  return __builtin_aiev2_vinsert8_I512(v, idx, b);
}
INTRINSIC(v64int8) insert(v64int8 v, int idx, v2int8 b) {
  return __builtin_aiev2_vinsert16_I512(v, idx, (short)b);
}
INTRINSIC(v64int8) insert(v64int8 v, int idx, v4int8 b) {
  return __builtin_aiev2_vinsert32_I512(v, idx, (int)b);
}
INTRINSIC(v64int8) insert(v64int8 v, int idx, v8int8 b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, b);
}
INTRINSIC(v32int16) insert(v32int16 v, int idx, short b) {
  return __builtin_aiev2_vinsert16_I512(v, idx, b);
}
INTRINSIC(v32int16) insert(v32int16 v, int idx, v2int16 b) {
  return __builtin_aiev2_vinsert32_I512(v, idx, (int)b);
}
INTRINSIC(v32int16) insert(v32int16 v, int idx, v4int16 b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, b);
}
INTRINSIC(v16int32) insert(v16int32 v, int idx, int b) {
  return __builtin_aiev2_vinsert32_I512(v, idx, b);
}
INTRINSIC(v16int32) insert(v16int32 v, int idx, v2int32 b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, b);
}
INTRINSIC(v128uint4) insert(v128uint4 v, int idx, v2uint4 b) {
  return __builtin_aiev2_vinsert8_I512(v, idx, b);
}
INTRINSIC(v128uint4) insert(v128uint4 v, int idx, v4uint4 b) {
  return __builtin_aiev2_vinsert16_I512(v, idx, (unsigned short)b);
}
INTRINSIC(v128uint4) insert(v128uint4 v, int idx, v8uint4 b) {
  return __builtin_aiev2_vinsert32_I512(v, idx, (unsigned int)b);
}
INTRINSIC(v128uint4) insert(v128uint4 v, int idx, v16uint4 b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, b);
}
INTRINSIC(v64uint8) insert(v64uint8 v, int idx, unsigned char b) {
  return __builtin_aiev2_vinsert8_I512(v, idx, b);
}
INTRINSIC(v64uint8) insert(v64uint8 v, int idx, v2uint8 b) {
  return __builtin_aiev2_vinsert16_I512(v, idx, (unsigned short)b);
}
INTRINSIC(v64uint8) insert(v64uint8 v, int idx, v4uint8 b) {
  return __builtin_aiev2_vinsert32_I512(v, idx, (unsigned int)b);
}
INTRINSIC(v64uint8) insert(v64uint8 v, int idx, v8uint8 b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, b);
}
INTRINSIC(v32uint16) insert(v32uint16 v, int idx, unsigned short b) {
  return __builtin_aiev2_vinsert16_I512(v, idx, b);
}
INTRINSIC(v32uint16) insert(v32uint16 v, int idx, v2uint16 b) {
  return __builtin_aiev2_vinsert32_I512(v, idx, (unsigned int)b);
}
INTRINSIC(v32uint16) insert(v32uint16 v, int idx, v4uint16 b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, b);
}
INTRINSIC(v16uint32) insert(v16uint32 v, int idx, unsigned int b) {
  return __builtin_aiev2_vinsert32_I512(v, idx, b);
}
INTRINSIC(v16uint32) insert(v16uint32 v, int idx, v2uint32 b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, b);
}
#if 0
INTRINSIC(v16cint16) insert(v16cint16 v, int idx, cint16 b) {
  return __builtin_aiev2_vinsert32_I512(v, idx, b);
}
INTRINSIC(v16cint16) insert(v16cint16 v, int idx, v2cint16 b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, b);
}
INTRINSIC(v16float) insert(v16float v, int idx, float b) {
  return __builtin_aiev2_vinsert32_I512(v, idx, b);
}
#endif
INTRINSIC(v32bfloat16) insert(v32bfloat16 v, int idx, bfloat16 b) {
  return __builtin_aiev2_vinsert_bf16_bf512(v, idx, b);
}
INTRINSIC(v32bfloat16) insert(v32bfloat16 v, int idx, v2bfloat16 b) {
  return __builtin_aiev2_vinsert_bf32_bf512(v, idx, b);
}
INTRINSIC(v32bfloat16) insert(v32bfloat16 v, int idx, v4bfloat16 b) {
  return __builtin_aiev2_vinsert_bf64_bf512(v, idx, b);
}
INTRINSIC(v128int4) insert(v128int4 v, int idx, unsigned long long b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, (v2uint32)b);
}
INTRINSIC(v64int8) insert(v64int8 v, int idx, unsigned long long b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, (v2uint32)b);
}
INTRINSIC(v32int16) insert(v32int16 v, int idx, unsigned long long b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, (v2uint32)b);
}
INTRINSIC(v16int32) insert(v16int32 v, int idx, unsigned long long b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, (v2uint32)b);
}
INTRINSIC(v128uint4) insert(v128uint4 v, int idx, unsigned long long b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, (v2uint32)b);
}
INTRINSIC(v64uint8) insert(v64uint8 v, int idx, unsigned long long b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, (v2uint32)b);
}
INTRINSIC(v32uint16) insert(v32uint16 v, int idx, unsigned long long b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, (v2uint32)b);
}
INTRINSIC(v16uint32) insert(v16uint32 v, int idx, unsigned long long b) {
  return __builtin_aiev2_vinsert64_I512(v, idx, (v2uint32)b);
}
#if 0
INTRINSIC(v16cint16) insert(v16cint16 v, int idx, unsigned long long b) {
  return __builtin_aiev2_vinsert64_I512(v, idx,
                                        (v2uint32)b);
}
INTRINSIC(v8cint32) insert(v8cint32 v, int idx, cint32 b) {
  return __builtin_aiev2_vinsert64_I512(v, idx,
                                        (v2uint32)b);
}
#endif
INTRINSIC(v32bfloat16) insert(v32bfloat16 v, int idx, unsigned long long b) {
  return __builtin_aiev2_vinsert_bf64_bf512(v, idx, (v4bfloat16)b);
}

// broadcast from scalar (alternative syntax to broadcast to vector)
INTRINSIC(v64int8)
broadcast_s8(char b) { return __builtin_aiev2_vbroadcast8_I512(b); }

INTRINSIC(v32int16)
broadcast_s16(short b) { return __builtin_aiev2_vbroadcast16_I512(b); }

INTRINSIC(v16int32)
broadcast_s32(int b) { return __builtin_aiev2_vbroadcast32_I512(b); }

INTRINSIC(v16int32)
broadcast_s64(long long b) {
  return __builtin_aiev2_vbroadcast64_I512((v2int32)b);
}

INTRINSIC(v16int32)
broadcast_v2s32(v2int32 b) { return __builtin_aiev2_vbroadcast64_I512(b); }

INTRINSIC(v64uint8)
broadcast_u8(unsigned char b) { return __builtin_aiev2_vbroadcast8_I512(b); }

INTRINSIC(v32uint16)
broadcast_u16(unsigned short b) { return __builtin_aiev2_vbroadcast16_I512(b); }

INTRINSIC(v16uint32)
broadcast_u32(unsigned int b) { return __builtin_aiev2_vbroadcast32_I512(b); }

INTRINSIC(v16uint32)
broadcast_u64(unsigned long long b) {
  return __builtin_aiev2_vbroadcast64_I512((v2uint32)b);
}

INTRINSIC(v16uint32)
broadcast_v2u32(v2uint32 b) { return __builtin_aiev2_vbroadcast64_I512(b); }

INTRINSIC(v32bfloat16)
broadcast_bfloat16(bfloat16 b) {
  return __builtin_aiev2_vbroadcast_bf16_bf512(b);
}

INTRINSIC(v16float)
broadcast_float(float b) {
  int as_int = *reinterpret_cast<int *>(&b);
  return __builtin_aiev2_vbroadcast32_I512(as_int);
}

#if 0
INTRINSIC(v16cint16)
broadcast_c16 (cint16 b) { return __builtin_aiev2_vbroadcast32_I512(b);}

INTRINSIC(v8cint32)
broadcast_c32 (cint32 b) { return __builtin_aiev2_vbroadcast64_I512(b);}
#endif

// broadcast to vector (alternative syntax to broadcast from scalar)
INTRINSIC(v128int4)
broadcast_to_v128int4(v2int4 b) { return __builtin_aiev2_vbroadcast8_I512(b); }

INTRINSIC(v128int4)
broadcast_to_v128int4(v4int4 b) {
  return __builtin_aiev2_vbroadcast16_I512((short)b);
}

INTRINSIC(v128int4)
broadcast_to_v128int4(v8int4 b) {
  return __builtin_aiev2_vbroadcast32_I512((int)b);
}

INTRINSIC(v128int4)
broadcast_to_v128int4(v16int4 b) {
  return __builtin_aiev2_vbroadcast64_I512(b);
}

INTRINSIC(v64int8)
broadcast_to_v64int8(char b) { return __builtin_aiev2_vbroadcast8_I512(b); }

INTRINSIC(v64int8)
broadcast_to_v64int8(v2int8 b) {
  return __builtin_aiev2_vbroadcast16_I512((short)b);
}

INTRINSIC(v64int8)
broadcast_to_v64int8(v4int8 b) {
  return __builtin_aiev2_vbroadcast32_I512((int)b);
}

INTRINSIC(v64int8)
broadcast_to_v64int8(v8int8 b) { return __builtin_aiev2_vbroadcast64_I512(b); }

INTRINSIC(v32int16)
broadcast_to_v32int16(short b) { return __builtin_aiev2_vbroadcast16_I512(b); }

INTRINSIC(v32int16)
broadcast_to_v32int16(v2int16 b) {
  return __builtin_aiev2_vbroadcast32_I512((int)b);
}

INTRINSIC(v32int16)
broadcast_to_v32int16(v4int16 b) {
  return __builtin_aiev2_vbroadcast64_I512(b);
}

INTRINSIC(v16int32)
broadcast_to_v16int32(int b) { return __builtin_aiev2_vbroadcast32_I512(b); }

INTRINSIC(v16int32)
broadcast_to_v16int32(unsigned long long b) {
  return __builtin_aiev2_vbroadcast64_I512((v2uint32)b);
}

INTRINSIC(v16int32)
broadcast_to_v16int32(v2int32 b) {
  return __builtin_aiev2_vbroadcast64_I512(b);
}

INTRINSIC(v128uint4)
broadcast_to_v128uint4(v2uint4 b) {
  return __builtin_aiev2_vbroadcast8_I512(b);
}

INTRINSIC(v128uint4)
broadcast_to_v128uint4(v4uint4 b) {
  return __builtin_aiev2_vbroadcast16_I512((unsigned short)b);
}

INTRINSIC(v128uint4)
broadcast_to_v128uint4(v8uint4 b) {
  return __builtin_aiev2_vbroadcast32_I512((unsigned int)b);
}

INTRINSIC(v128uint4)
broadcast_to_v128uint4(v16uint4 b) {
  return __builtin_aiev2_vbroadcast64_I512(b);
}

INTRINSIC(v64uint8)
broadcast_to_v64uint8(unsigned char b) {
  return __builtin_aiev2_vbroadcast8_I512(b);
}

INTRINSIC(v64uint8)
broadcast_to_v64uint8(v2uint8 b) {
  return __builtin_aiev2_vbroadcast16_I512((unsigned short)b);
}

INTRINSIC(v64uint8)
broadcast_to_v64uint8(v4uint8 b) {
  return __builtin_aiev2_vbroadcast32_I512((unsigned int)b);
}

INTRINSIC(v64uint8)
broadcast_to_v64uint8(v8uint8 b) {
  return __builtin_aiev2_vbroadcast64_I512(b);
}

INTRINSIC(v32uint16)
broadcast_to_v32uint16(unsigned short b) {
  return __builtin_aiev2_vbroadcast16_I512(b);
}

INTRINSIC(v32uint16)
broadcast_to_v32uint16(v2uint16 b) {
  return __builtin_aiev2_vbroadcast32_I512((unsigned int)b);
}

INTRINSIC(v32uint16)
broadcast_to_v32uint16(v4uint16 b) {
  return __builtin_aiev2_vbroadcast64_I512(b);
}

INTRINSIC(v16uint32)
broadcast_to_v16uint32(unsigned int b) {
  return __builtin_aiev2_vbroadcast32_I512(b);
}

INTRINSIC(v16uint32)
broadcast_to_v16uint32(unsigned long long b) {
  return __builtin_aiev2_vbroadcast64_I512((v2uint32)b);
}

INTRINSIC(v16uint32)
broadcast_to_v16uint32(v2uint32 b) {
  return __builtin_aiev2_vbroadcast64_I512(b);
}

#if 0
INTRINSIC(v16cint16)
broadcast_to_v16cint16 (cint16 b) { return __builtin_aiev2_vbroadcast32_I512(b); }

INTRINSIC(v16cint16)
broadcast_to_v16cint16 (v2cint16 b) { return __builtin_aiev2_vbroadcast64_I512(b); }

INTRINSIC(v8cint32)
broadcast_to_v8cint32 (cint32 b) { return __builtin_aiev2_vbroadcast64_I512(b); }
#endif

INTRINSIC(v32bfloat16)
broadcast_to_v32bfloat16(bfloat16 b) {
  return __builtin_aiev2_vbroadcast_bf16_bf512(b);
}

INTRINSIC(v32bfloat16)
broadcast_to_v32bfloat16(v2bfloat16 b) {
  return __builtin_aiev2_vbroadcast_bf32_bf512(b);
}

INTRINSIC(v32bfloat16)
broadcast_to_v32bfloat16(v4bfloat16 b) {
  return __builtin_aiev2_vbroadcast_bf64_bf512(b);
}

INTRINSIC(v16accfloat)
broadcast_to_v16accfloat(float b) {
  return __builtin_aiev2_vbroadcastfloat_I512(b);
}

#if 0
INTRINSIC(v16float)
broadcast_to_v16float (float b) { return __builtin_aiev2_vbroadcast32_I512(b); }
#endif

INTRINSIC(v32bfloat16)
broadcast_zero_to_v32bfloat16() { return broadcast_to_v32bfloat16(0); }

INTRINSIC(v32bfloat16)
broadcast_one_to_v32bfloat16() { return broadcast_to_v32bfloat16(1); }

// Right-most insertion (left shift)
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
#if 0
INTRINSIC(v16cint16) shiftl_elem(v16cint16 v, cint16 s) {
  return shift_bytes(v, broadcast_c16(s), 4);
}
INTRINSIC(v8cint32) shiftl_elem(v8cint32 v, cint32_w64 s) {
  return shift_bytes(v, broadcast_c32(s), 8);
}
INTRINSIC(v8cint32) shiftl_elem(v8cint32 v, cint32 s) {
  return shift_bytes(v, broadcast_c32((cint32_w64)s), 8);
}
INTRINSIC(v16float) shiftl_elem(v16float v, float s) {
  return shift_bytes(v, broadcast_float(s), 4);
}
#endif
INTRINSIC(v32bfloat16) shiftl_elem(v32bfloat16 v, bfloat16 s) {
  return shift_bytes(v, broadcast_bfloat16(s), 2);
}

// Left-most insertion (right shift)
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
#if 0
INTRINSIC(v16cint16) shiftr_elem(v16cint16 v, cint16 s) {
  return shift_bytes(broadcast_c16(s), v, 64 - 4);
}
INTRINSIC(v8cint32) shiftr_elem(v8cint32 v, cint32_w64 s) {
  return shift_bytes(broadcast_c32(s), v, 64 - 8);
}
INTRINSIC(v8cint32) shiftr_elem(v8cint32 v, cint32 s) {
  return shift_bytes(broadcast_c32((cint32_w64)s), v, 64 - 8);
}
INTRINSIC(v16float) shiftr_elem(v16float v, float s) {
  return shift_bytes(broadcast_float(s), v, 64 - 4);
}
#endif
INTRINSIC(v32bfloat16) shiftr_elem(v32bfloat16 v, bfloat16 s) {
  return shift_bytes(broadcast_bfloat16(s), v, 64 - 2);
}

// broadcast value one(1) to all vector lanes
INTRINSIC(v64int8) broadcast_one_s8() { return broadcast_s8(1); }

INTRINSIC(v32int16) broadcast_one_s16() { return broadcast_s16(1); }

INTRINSIC(v16int32) broadcast_one_s32() { return broadcast_s32(1); }

INTRINSIC(v64uint8) broadcast_one_u8() { return broadcast_u8(1); }

INTRINSIC(v32uint16) broadcast_one_u16() { return broadcast_u16(1); }

INTRINSIC(v16uint32) broadcast_one_u32() { return broadcast_u32(1); }

INTRINSIC(v32bfloat16) broadcast_one_bfloat16() {
  return broadcast_bfloat16(1);
}

#if 0
INTRINSIC(v16cint16) broadcast_one_c16() { return broadcast_c16(1); }

INTRINSIC(v8cint32) broadcast_one_c32() { return broadcast_c32(1); }

INTRINSIC(v16float) broadcast_one_float() { return broadcast_float(1); }
#endif

// broadcast value zero(0) to all vector lanes
[[deprecated(
    "Function 'broadcast_zero_s8' is deprecated. Please use the "
    "'broadcast_zero_to_v64int8' variant instead.")]] INTRINSIC(v64int8)
    broadcast_zero_s8() {
  return broadcast_s8(0);
}
INTRINSIC(v64int8) broadcast_zero_to_v64int8() { return broadcast_s8(0); }

[[deprecated(
    "Function 'broadcast_zero_s16' is deprecated. Please use the "
    "'broadcast_zero_to_v32int16' variant instead.")]] INTRINSIC(v32int16)
    broadcast_zero_s16() {
  return broadcast_s16(0);
}
INTRINSIC(v32int16) broadcast_zero_to_v32int16() { return broadcast_s16(0); }

[[deprecated(
    "Function 'broadcast_zero_s32' is deprecated. Please use the "
    "'broadcast_zero_to_v16int32' variant instead.")]] INTRINSIC(v16int32)
    broadcast_zero_s32() {
  return broadcast_s32(0);
}
INTRINSIC(v16int32) broadcast_zero_to_v16int32() { return broadcast_s32(0); }

[[deprecated(
    "Function 'broadcast_zero_u8' is deprecated. Please use the "
    "'broadcast_zero_to_v64uint8' variant instead.")]] INTRINSIC(v64uint8)
    broadcast_zero_u8() {
  return broadcast_u8(0);
}
INTRINSIC(v64uint8) broadcast_zero_to_v64uint8() { return broadcast_u8(0); }

[[deprecated(
    "Function 'broadcast_zero_u16' is deprecated. Please use the "
    "'broadcast_zero_to_v32uint16' variant instead.")]] INTRINSIC(v32uint16)
    broadcast_zero_u16() {
  return broadcast_u16(0);
}
INTRINSIC(v32uint16) broadcast_zero_to_v32uint16() { return broadcast_u16(0); }

[[deprecated(
    "Function 'broadcast_zero_u32' is deprecated. Please use the "
    "'broadcast_zero_to_v16uint32' variant instead.")]] INTRINSIC(v16uint32)
    broadcast_zero_u32() {
  return broadcast_u32(0);
}
INTRINSIC(v16uint32) broadcast_zero_to_v16uint32() { return broadcast_u32(0); }

[[deprecated(
    "Function 'broadcast_zero_bfloat16' is deprecated. Please use the "
    "'broadcast_zero_to_v32bfloat16' variant instead.")]] INTRINSIC(v32bfloat16)
    broadcast_zero_bfloat16() {
  return broadcast_bfloat16(0);
}

[[deprecated(
    "Function 'broadcast_zero_float' is deprecated. Please use the "
    "'broadcast_zero_to_v16float' variant instead.")]] INTRINSIC(v16float)
    broadcast_zero_float() {
  return broadcast_float(0);
}
INTRINSIC(v16float) broadcast_zero_to_v16float() { return broadcast_float(0); }

INTRINSIC(v16acc64) broadcast_zero_to_v16acc64() {
  return __builtin_aiev2_vbroadcast_zero_acc1024();
}

[[deprecated("Function 'clr' is deprecated. Please use the 'broadcast_zero_to' "
             "variant instead.")]] INTRINSIC(v16acc64) clr16() {
  return broadcast_zero_to_v16acc64();
}

INTRINSIC(v32acc32) broadcast_zero_to_v32acc32() {
  return __builtin_aiev2_vbroadcast_zero_acc1024();
}

[[deprecated("Function 'clr' is deprecated. Please use the 'broadcast_zero_to' "
             "variant instead.")]] INTRINSIC(v32acc32) clr32() {
  return broadcast_zero_to_v32acc32();
}

#if 0
[[deprecated("Function 'broadcast_zero_c16' is deprecated. Please use the 'broadcast_zero_to_v16cint16' variant instead.")]] INTRINSIC(v16cint16) broadcast_zero_c16() { return broadcast_c16(0); }
INTRINSIC(v16cint16) broadcast_zero_to_v16cint16() { return broadcast_c16(0); }

[[deprecated("Function 'broadcast_zero_c32' is deprecated. Please use the 'broadcast_zero_to_v8cint32' variant instead.")]] INTRINSIC(v8cint32) broadcast_zero_c32() { return broadcast_c32(0); }
INTRINSIC(v8cint32) broadcast_zero_to_v8cint32() { return broadcast_c32(0); }
#endif

// Extract and broadcast scalar word between two vectors
INTRINSIC(v64int8)
broadcast_elem(v64int8 v, int idx) {
  return __builtin_aiev2_vextract_broadcast8_I512(v, idx);
}

INTRINSIC(v32int16)
broadcast_elem(v32int16 v, int idx) {
  return __builtin_aiev2_vextract_broadcast16_I512(v, idx);
}

INTRINSIC(v16int32)
broadcast_elem(v16int32 v, int idx) {
  return __builtin_aiev2_vextract_broadcast32_I512(v, idx);
}

INTRINSIC(v16int32)
broadcast_elem_s64(v16int32 v, int idx) {
  return __builtin_aiev2_vextract_broadcast32_I512(v, idx);
}

INTRINSIC(v64uint8)
broadcast_elem(v64uint8 v, int idx) {
  return __builtin_aiev2_vextract_broadcast8_I512(v, idx);
}

INTRINSIC(v32uint16)
broadcast_elem(v32uint16 v, int idx) {
  return __builtin_aiev2_vextract_broadcast16_I512(v, idx);
}

INTRINSIC(v16uint32)
broadcast_elem(v16uint32 v, int idx) {
  return __builtin_aiev2_vextract_broadcast32_I512(v, idx);
}

INTRINSIC(v16uint32)
broadcast_elem_s64(v16uint32 v, int idx) {
  return __builtin_aiev2_vextract_broadcast32_I512(v, idx);
}

INTRINSIC(v32bfloat16)
broadcast_elem(v32bfloat16 v, int idx) {
  return __builtin_aiev2_vextract_broadcast_bf32_bf512(v, idx);
}

#if 0
INTRINSIC(v16cint16)
broadcast_elem (v16cint16 v, int idx) {
  return __builtin_aiev2_vextract_broadcast32_I512(v, idx);
}

INTRINSIC(v8cint32)
broadcast_elem (v8cint32 v, int idx) {
  return ;
}

INTRINSIC(v16float)
broadcast_elem (v16float v, int idx) {
  return __builtin_aiev2_vextract_broadcast32_I512(v, idx);
}
#endif

INTRINSIC(v16int32) shuffle(v16int32 a, v16int32 b, unsigned int mode) {
  return __builtin_aiev2_vshuffle(a, b, mode);
}
INTRINSIC(v32int16) shuffle(v32int16 a, v32int16 b, unsigned int mode) {
  return __builtin_aiev2_vshuffle(a, b, mode);
}
INTRINSIC(v64int8) shuffle(v64int8 a, v64int8 b, unsigned int mode) {
  return __builtin_aiev2_vshuffle(a, b, mode);
}

INTRINSIC(v16uint32) shuffle(v16uint32 a, v16uint32 b, unsigned int mode) {
  return __builtin_aiev2_vshuffle(a, b, mode);
}
INTRINSIC(v32uint16) shuffle(v32uint16 a, v32uint16 b, unsigned int mode) {
  return __builtin_aiev2_vshuffle(a, b, mode);
}
INTRINSIC(v64uint8) shuffle(v64uint8 a, v64uint8 b, unsigned int mode) {
  return __builtin_aiev2_vshuffle(a, b, mode);
}
INTRINSIC(v128uint4) shuffle(v128uint4 a, v128uint4 b, unsigned int mode) {
  return __builtin_aiev2_vshuffle(a, b, mode);
}
INTRINSIC(v128int4) shuffle(v128int4 a, v128int4 b, unsigned int mode) {
  return __builtin_aiev2_vshuffle(a, b, mode);
}

#if 0
INTRINSIC(v8cint32)    shuffle(v8cint32 a,    v8cint32 b,    unsigned int mode) { return __builtin_aiev2_vshuffle(a, b, mode); }
INTRINSIC(v16cint16)   shuffle(v16cint16 a,   v16cint16 b,   unsigned int mode) { return __builtin_aiev2_vshuffle(a, b, mode); }
INTRINSIC(v16float)    shuffle(v16float a,    v16float b,    unsigned int mode) { return __builtin_aiev2_vshuffle(a, b, mode); }
#endif
INTRINSIC(v32bfloat16)
shuffle(v32bfloat16 a, v32bfloat16 b, unsigned int mode) {
  return __builtin_aiev2_vshuffle_bf16(a, b, mode);
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
INTRINSIC(v16uint32) shuffle(v16uint32 a, unsigned int mode) {
  return shuffle(a, undef_v16uint32(), mode);
}
INTRINSIC(v32uint16) shuffle(v32uint16 a, unsigned int mode) {
  return shuffle(a, undef_v32uint16(), mode);
}
INTRINSIC(v64uint8) shuffle(v64uint8 a, unsigned int mode) {
  return shuffle(a, undef_v64uint8(), mode);
}

INTRINSIC(v128int4) shuffle(v128int4 a, unsigned int mode) {
  return shuffle(a, undef_v128int4(), mode);
}
INTRINSIC(v128uint4) shuffle(v128uint4 a, unsigned int mode) {
  return shuffle(a, undef_v128uint4(), mode);
}
INTRINSIC(v32bfloat16) shuffle(v32bfloat16 a, unsigned int mode) {
  return shuffle(a, undef_v32bfloat16(), mode);
}

#if 0
INTRINSIC(v8cint32)    shuffle(v8cint32 a,    unsigned int mode) { return shuffle(a, undef_v8cint32(),    mode); }
INTRINSIC(v16cint16)   shuffle(v16cint16 a,   unsigned int mode) { return shuffle(a, undef_v16cint16(),   mode); }
INTRINSIC(v16float)    shuffle(v16float a,    unsigned int mode) { return shuffle(a, undef_v16float(),    mode); }
#endif

INTRINSIC(v64int8) shuffle_s8(int b, unsigned int m) {
  return __builtin_aiev2_vbcst_shuffle8(b, m);
}
INTRINSIC(v32int16) shuffle_s16(int b, unsigned int m) {
  return __builtin_aiev2_vbcst_shuffle16(b, m);
}
INTRINSIC(v16int32) shuffle_s32(int b, unsigned int m) {
  return __builtin_aiev2_vbcst_shuffle32(b, m);
}
INTRINSIC(v16int32) shuffle_v2s32(v2int32 b, unsigned int m) {
  return __builtin_aiev2_vbcst_shuffle64(b, m);
}
INTRINSIC(v64uint8) shuffle_u8(unsigned int b, unsigned int m) {
  return __builtin_aiev2_vbcst_shuffle8(b, m);
}
INTRINSIC(v32uint16) shuffle_u16(unsigned int b, unsigned int m) {
  return __builtin_aiev2_vbcst_shuffle16(b, m);
}
INTRINSIC(v16uint32) shuffle_u32(unsigned int b, unsigned int m) {
  return __builtin_aiev2_vbcst_shuffle32(b, m);
}
INTRINSIC(v16uint32) shuffle_v2u32(v2uint32 b, unsigned int m) {
  return __builtin_aiev2_vbcst_shuffle64(b, m);
}

#if 0
INTRINSIC(v16cint16)   shuffle_c16     (      cint16 b, unsigned int m) {  return __builtin_aiev2_vbcst_shuffle32(b,m) ;}
INTRINSIC(v16float)    shuffle_float   (float        b, unsigned int m) {  return __builtin_aiev2_vbcst_shuffle32(b,m) ;}
#endif
INTRINSIC(v32bfloat16) shuffle_bfloat16(bfloat16 b, unsigned int m) {
  return __builtin_aiev2_vbcst_shuffle_bf16(b, m);
}

INTRINSIC(v16int32) shuffle_s64(long long b, unsigned int m) {
  return __builtin_aiev2_vbcst_shuffle64((v2int32)b, m);
}
INTRINSIC(v16uint32) shuffle_u64(unsigned long long b, unsigned int m) {
  return __builtin_aiev2_vbcst_shuffle64((v2uint32)b, m);
}
#if 0
INTRINSIC(v8cint32)  shuffle_c32 ( cint32 b, unsigned int m) { return __builtin_aiev2_vbcst_shuffle64((cint32_w64)b, m);}
#endif

#if 0
INTRINSIC(v256int4_sparse) shuffle(v256int4_sparse qx, int itlv) {
    v128int4 x = extract_v128int4(qx);
    x = shuffle( x, undef_v128int4(), itlv );
    return update(qx, x);
}
INTRINSIC(v128int8_sparse) shuffle(v128int8_sparse qx, int itlv) {
    v64int8 x = extract_v64int8(qx);
    x = shuffle( x, undef_v64int8(), itlv );
    return update(qx, x);
}
INTRINSIC(v64int16_sparse) shuffle(v64int16_sparse qx, int itlv) {
    v32int16 x = extract_v32int16(qx);
    x = shuffle( x, undef_v32int16(), itlv );
    return update(qx, x);
}

INTRINSIC(v256uint4_sparse) shuffle(v256uint4_sparse qx, int itlv) {
    v128uint4 x = extract_v128uint4(qx);
    x = shuffle( x, undef_v128uint4(), itlv );
    return update(qx, x);
}
INTRINSIC(v128uint8_sparse) shuffle(v128uint8_sparse qx, int itlv) {
    v64uint8 x = extract_v64uint8(qx);
    x = shuffle( x, undef_v64uint8(), itlv );
    return update(qx, x);
}
INTRINSIC(v64uint16_sparse) shuffle(v64uint16_sparse qx, int itlv) {
    v32uint16 x = extract_v32uint16(qx);
    x = shuffle( x, undef_v32uint16(), itlv );
    return update(qx, x);
}

INTRINSIC(v64bfloat16_sparse) shuffle(v64bfloat16_sparse qx, int itlv) {
    v32bfloat16 x = extract_v32bfloat16(qx);
    x = shuffle( x, undef_v32bfloat16(), itlv );
    return update(qx, x);
#endif

// Extract an element from a vector with dynamic sign
#define DIAGNOSE_EXT_IDX                                                       \
  __attribute__((                                                              \
      diagnose_if(idx < 0 || idx > 63, "index out of range [0,63]", "error")))

INTRINSIC(v2int4)
ext_v2int4(v128int4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem8_I512(v, idx, sign);
}

INTRINSIC(v4int4)
ext_v4int4(v128int4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return (v4int4)(short)__builtin_aiev2_vextract_elem16_I512(v, idx, sign);
}

INTRINSIC(v8int4)
ext_v8int4(v128int4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return (v8int4)__builtin_aiev2_vextract_elem32_I512(v, idx, sign);
}

INTRINSIC(v16int4)
ext_v16int4(v128int4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem64_I512(v, idx, sign);
}

INTRINSIC(char)
ext_elem(v64int8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem8_I512(v, idx, sign);
}

INTRINSIC(v2int8)
ext_v2int8(v64int8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return (v2int8)(short)__builtin_aiev2_vextract_elem16_I512(v, idx, sign);
}

INTRINSIC(v4int8)
ext_v4int8(v64int8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return (v4int8)__builtin_aiev2_vextract_elem32_I512(v, idx, sign);
}

INTRINSIC(v8int8)
ext_v8int8(v64int8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem64_I512(v, idx, sign);
}

INTRINSIC(short)
ext_elem(v32int16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem16_I512(v, idx, sign);
}

INTRINSIC(v2int16)
ext_v2int16(v32int16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return (v2int16)__builtin_aiev2_vextract_elem32_I512(v, idx, sign);
}

INTRINSIC(v4int16)
ext_v4int16(v32int16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem64_I512(v, idx, sign);
}

INTRINSIC(int)
ext_elem(v16int32 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem32_I512(v, idx, sign);
}

INTRINSIC(v2int32)
ext_v2int32(v16int32 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem64_I512(v, idx, sign);
}

INTRINSIC(v2uint4)
ext_v2uint4(v128uint4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem8_I512(v, idx, sign);
}

INTRINSIC(v4uint4)
ext_v4uint4(v128uint4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return (v4uint4)(short)__builtin_aiev2_vextract_elem16_I512(v, idx, sign);
}

INTRINSIC(v8uint4)
ext_v8uint4(v128uint4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return (v8uint4)__builtin_aiev2_vextract_elem32_I512(v, idx, sign);
}

INTRINSIC(v16uint4)
ext_v16uint4(v128uint4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem64_I512(v, idx, sign);
}

INTRINSIC(unsigned char)
ext_elem(v64uint8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem8_I512(v, idx, sign);
}

INTRINSIC(v2uint8)
ext_v2uint8(v64uint8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return (v2uint8)(short)__builtin_aiev2_vextract_elem16_I512(v, idx, sign);
}

INTRINSIC(v4uint8)
ext_v4uint8(v64uint8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return (v4uint8)__builtin_aiev2_vextract_elem32_I512(v, idx, sign);
}

INTRINSIC(v8uint8)
ext_v8uint8(v64uint8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem64_I512(v, idx, sign);
}

INTRINSIC(unsigned short)
ext_elem(v32uint16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem16_I512(v, idx, sign);
}

INTRINSIC(v2uint16)
ext_v2uint16(v32uint16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return (v2uint16)__builtin_aiev2_vextract_elem32_I512(v, idx, sign);
}

INTRINSIC(v4uint16)
ext_v4uint16(v32uint16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem64_I512(v, idx, sign);
}

INTRINSIC(unsigned int)
ext_elem(v16uint32 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem32_I512(v, idx, sign);
}

INTRINSIC(v2uint32)
ext_v2uint32(v16uint32 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return __builtin_aiev2_vextract_elem64_I512(v, idx, sign);
}

#if 0
INTRINSIC(cint16)
ext_elem(v16cint16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
   return __builtin_aiev2_vextract_elem32_I512(v, idx, sign);
}

INTRINSIC(v2cint16)
ext_v2cint16(v16cint16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
   return __builtin_aiev2_vextract_elem64_I512(v, idx, sign);
}
#endif

INTRINSIC(unsigned long long)
ext_u64(v128int4 v, int idx, int sign) DIAGNOSE_EXT_IDX {

  return (
      unsigned long long)(__builtin_aiev2_vextract_elem64_I512(v, idx, sign));
}

INTRINSIC(unsigned long long)
ext_u64(v64int8 v, int idx, int sign) DIAGNOSE_EXT_IDX {

  return (
      unsigned long long)(__builtin_aiev2_vextract_elem64_I512(v, idx, sign));
}

INTRINSIC(unsigned long long)
ext_u64(v32int16 v, int idx, int sign) DIAGNOSE_EXT_IDX {

  return (
      unsigned long long)(__builtin_aiev2_vextract_elem64_I512(v, idx, sign));
}

INTRINSIC(unsigned long long)
ext_u64(v16int32 v, int idx, int sign) DIAGNOSE_EXT_IDX {

  return (
      unsigned long long)(__builtin_aiev2_vextract_elem64_I512(v, idx, sign));
}

INTRINSIC(unsigned long long)
ext_u64(v128uint4 v, int idx, int sign) DIAGNOSE_EXT_IDX {

  return (
      unsigned long long)(__builtin_aiev2_vextract_elem64_I512(v, idx, sign));
}

INTRINSIC(unsigned long long)
ext_u64(v64uint8 v, int idx, int sign) DIAGNOSE_EXT_IDX {

  return (
      unsigned long long)(__builtin_aiev2_vextract_elem64_I512(v, idx, sign));
}

INTRINSIC(unsigned long long)
ext_u64(v32uint16 v, int idx, int sign) DIAGNOSE_EXT_IDX {

  return (
      unsigned long long)(__builtin_aiev2_vextract_elem64_I512(v, idx, sign));
}

INTRINSIC(unsigned long long)
ext_u64(v16uint32 v, int idx, int sign) DIAGNOSE_EXT_IDX {

  return (
      unsigned long long)(__builtin_aiev2_vextract_elem64_I512(v, idx, sign));
}

INTRINSIC(bfloat16)
ext_elem(v32bfloat16 v, int idx, int sign) DIAGNOSE_EXT_IDX {

  int elem = __builtin_aiev2_vextract_elem16_I512((v32int16)v, idx, sign);
  return *reinterpret_cast<bfloat16 *>(&elem);
}

#if 0
INTRINSIC(unsigned long long)
ext_u64(v16cint16 v, int idx, int sign) DIAGNOSE_EXT_IDX {

  return __builtin_aiev2_vextract_elem64_I512(v, idx, sign);
}

INTRINSIC(cint32)
ext_elem(v8cint32 v, int idx, int sign) DIAGNOSE_EXT_IDX {
   return (cint32)__builtin_aiev2_vextract_elem64_I512(v, idx, sign);
}
#endif
INTRINSIC(unsigned long long)
ext_u64(v32bfloat16 v, int idx, int sign) DIAGNOSE_EXT_IDX {

  return (unsigned long long)__builtin_aiev2_vextract_elem64_I512((v32int16)v,
                                                                  idx, sign);
}

INTRINSIC(v2bfloat16)
ext_v2bfloat16(v32bfloat16 v, int idx, int sign) DIAGNOSE_EXT_IDX {

  int elem = __builtin_aiev2_vextract_elem32_I512((v32int16)v, idx, sign);
  return *reinterpret_cast<v2bfloat16 *>(&elem);
}

INTRINSIC(v4bfloat16)
ext_v4bfloat16(v32bfloat16 v, int idx, int sign) DIAGNOSE_EXT_IDX {

  v2int32 elem = __builtin_aiev2_vextract_elem64_I512((v32int16)v, idx, sign);
  return *reinterpret_cast<v4bfloat16 *>(&elem);
}

INTRINSIC(v2int4)
extract_v2int4(v128int4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v2int4(v, idx, sign);
}

INTRINSIC(v4int4)
extract_v4int4(v128int4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v4int4(v, idx, sign);
}

INTRINSIC(v8int4)
extract_v8int4(v128int4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v8int4(v, idx, sign);
}

INTRINSIC(v16int4)
extract_v16int4(v128int4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v16int4(v, idx, sign);
}

INTRINSIC(char) extract_elem(v64int8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_elem(v, idx, sign);
}

INTRINSIC(v2int8)
extract_v2int8(v64int8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v2int8(v, idx, sign);
}

INTRINSIC(v4int8)
extract_v4int8(v64int8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v4int8(v, idx, sign);
}

INTRINSIC(v8int8)
extract_v8int8(v64int8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v8int8(v, idx, sign);
}

INTRINSIC(short) extract_elem(v32int16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_elem(v, idx, sign);
}

INTRINSIC(v2int16)
extract_v2int16(v32int16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v2int16(v, idx, sign);
}

INTRINSIC(v4int16)
extract_v4int16(v32int16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v4int16(v, idx, sign);
}

INTRINSIC(int) extract_elem(v16int32 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_elem(v, idx, sign);
}

INTRINSIC(v2int32)
extract_v2int32(v16int32 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v2int32(v, idx, sign);
}

INTRINSIC(v2uint4)
extract_v2uint4(v128uint4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v2uint4(v, idx, sign);
}

INTRINSIC(v4uint4)
extract_v4uint4(v128uint4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v4uint4(v, idx, sign);
}

INTRINSIC(v8uint4)
extract_v8uint4(v128uint4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v8uint4(v, idx, sign);
}

INTRINSIC(v16uint4)
extract_v16uint4(v128uint4 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v16uint4(v, idx, sign);
}

INTRINSIC(unsigned char)
extract_elem(v64uint8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_elem(v, idx, sign);
}

INTRINSIC(v2uint8)
extract_v2uint8(v64uint8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v2uint8(v, idx, sign);
}

INTRINSIC(v4uint8)
extract_v4uint8(v64uint8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v4uint8(v, idx, sign);
}

INTRINSIC(v8uint8)
extract_v8uint8(v64uint8 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v8uint8(v, idx, sign);
}

INTRINSIC(unsigned short)
extract_elem(v32uint16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_elem(v, idx, sign);
}

INTRINSIC(v2uint16)
extract_v2uint16(v32uint16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v2uint16(v, idx, sign);
}

INTRINSIC(v4uint16)
extract_v4uint16(v32uint16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v4uint16(v, idx, sign);
}

INTRINSIC(unsigned)
int extract_elem(v16uint32 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_elem(v, idx, sign);
}

INTRINSIC(v2uint32)
extract_v2uint32(v16uint32 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v2uint32(v, idx, sign);
}

INTRINSIC(bfloat16)
extract_elem(v32bfloat16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_elem(v, idx, sign);
}

INTRINSIC(v2bfloat16)
extract_v2bfloat16(v32bfloat16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v2bfloat16(v, idx, sign);
}

INTRINSIC(v4bfloat16)
extract_v4bfloat16(v32bfloat16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
  return ext_v4bfloat16(v, idx, sign);
}

INTRINSIC(float) extract_elem(v16float v, int idx, int sign) DIAGNOSE_EXT_IDX {
  int elem = __builtin_aiev2_vextract_elem32_I512(v, idx, sign);
  return *reinterpret_cast<float *>(&elem);
}

#if 0
INTRINSIC(cint32) extract_elem(v8cint32 v, int idx, int sign) DIAGNOSE_EXT_IDX {
   return __builtin_aiev2_vextract_elem64_I512(v, idx, sign);
}

INTRINSIC(cint16) extract_elem(v16cint16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
   return ext_elem(v, idx, sign);
}

INTRINSIC(v2cint16) extract_v2cint16(v16cint16 v, int idx, int sign) DIAGNOSE_EXT_IDX {
   return ext_v2cint16(v, idx, sign);
}
#endif

// Extract an element from a vector
INTRINSIC(v2int4) ext_v2int4(v128int4 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v2int4(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v4int4) ext_v4int4(v128int4 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v4int4(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v8int4) ext_v8int4(v128int4 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v8int4(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v16int4) ext_v16int4(v128int4 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v16int4(v, idx, __SIGN_SIGNED);
}

INTRINSIC(char) ext_elem(v64int8 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_elem(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v2int8) ext_v2int8(v64int8 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v2int8(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v4int8) ext_v4int8(v64int8 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v4int8(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v8int8) ext_v8int8(v64int8 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v8int8(v, idx, __SIGN_SIGNED);
}

INTRINSIC(short) ext_elem(v32int16 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_elem(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v2int16) ext_v2int16(v32int16 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v2int16(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v4int16) ext_v4int16(v32int16 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v4int16(v, idx, __SIGN_SIGNED);
}

INTRINSIC(int) ext_elem(v16int32 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_elem(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v2int32) ext_v2int32(v16int32 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v2int32(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v2uint4) ext_v2uint4(v128uint4 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v2uint4(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v4uint4) ext_v4uint4(v128uint4 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v4uint4(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v8uint4) ext_v8uint4(v128uint4 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v8uint4(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v16uint4) ext_v16uint4(v128uint4 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v16uint4(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned char) ext_elem(v64uint8 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_elem(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v2uint8) ext_v2uint8(v64uint8 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v2uint8(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v4uint8) ext_v4uint8(v64uint8 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v4uint8(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v8uint8) ext_v8uint8(v64uint8 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v8uint8(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned short) ext_elem(v32uint16 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_elem(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v2uint16) ext_v2uint16(v32uint16 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v2uint16(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v4uint16) ext_v4uint16(v32uint16 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v4uint16(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned) int ext_elem(v16uint32 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_elem(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v2uint32) ext_v2uint32(v16uint32 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v2uint32(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(bfloat16) ext_elem(v32bfloat16 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_elem(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v2bfloat16) ext_v2bfloat16(v32bfloat16 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v2bfloat16(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v4bfloat16) ext_v4bfloat16(v32bfloat16 v, int idx) DIAGNOSE_EXT_IDX {
  return ext_v4bfloat16(v, idx, __SIGN_SIGNED);
}

#if 0
INTRINSIC(cint16) ext_elem(v16cint16 v, int idx) DIAGNOSE_EXT_IDX {
   return ext_elem(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v2cint16) ext_v2cint16(v16cint16 v, int idx) DIAGNOSE_EXT_IDX {
   return ext_v2cint16(v, idx, __SIGN_SIGNED);
}

INTRINSIC(cint32) ext_elem(v8cint32 v, int idx) DIAGNOSE_EXT_IDX {
   return ext_elem(v, idx, __SIGN_SIGNED);
}
#endif

INTRINSIC(v2int4) extract_v2int4(v128int4 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v2int4(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v4int4) extract_v4int4(v128int4 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v4int4(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v8int4) extract_v8int4(v128int4 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v8int4(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v16int4) extract_v16int4(v128int4 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v16int4(v, idx, __SIGN_SIGNED);
}

INTRINSIC(char) extract_elem(v64int8 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_elem(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v2int8) extract_v2int8(v64int8 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v2int8(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v4int8) extract_v4int8(v64int8 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v4int8(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v8int8) extract_v8int8(v64int8 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v8int8(v, idx, __SIGN_SIGNED);
}

INTRINSIC(short) extract_elem(v32int16 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_elem(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v2int16) extract_v2int16(v32int16 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v2int16(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v4int16) extract_v4int16(v32int16 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v4int16(v, idx, __SIGN_SIGNED);
}

INTRINSIC(int) extract_elem(v16int32 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_elem(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v2int32) extract_v2int32(v16int32 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v2int32(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v2uint4) extract_v2uint4(v128uint4 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v2uint4(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v4uint4) extract_v4uint4(v128uint4 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v4uint4(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v8uint4) extract_v8uint4(v128uint4 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v8uint4(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v16uint4) extract_v16uint4(v128uint4 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v16uint4(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned char) extract_elem(v64uint8 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_elem(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v2uint8) extract_v2uint8(v64uint8 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v2uint8(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v4uint8) extract_v4uint8(v64uint8 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v4uint8(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v8uint8) extract_v8uint8(v64uint8 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v8uint8(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned short) extract_elem(v32uint16 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_elem(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v2uint16) extract_v2uint16(v32uint16 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v2uint16(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v4uint16) extract_v4uint16(v32uint16 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v4uint16(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned int) extract_elem(v16uint32 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_elem(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(v2uint32) extract_v2uint32(v16uint32 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v2uint32(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(bfloat16) extract_elem(v32bfloat16 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_elem(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v2bfloat16)
extract_v2bfloat16(v32bfloat16 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v2bfloat16(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v4bfloat16)
extract_v4bfloat16(v32bfloat16 v, int idx) DIAGNOSE_EXT_IDX {
  return extract_v4bfloat16(v, idx, __SIGN_SIGNED);
}

INTRINSIC(float) extract_elem(v16float v, int idx) DIAGNOSE_EXT_IDX {
  return extract_elem(v, idx, __SIGN_SIGNED);
}

#if 0
INTRINSIC(cint16) extract_elem(v16cint16 v, int idx) DIAGNOSE_EXT_IDX {
   return extract_elem(v, idx, __SIGN_SIGNED);
}

INTRINSIC(v2cint16) extract_v2cint16(v16cint16 v, int idx) DIAGNOSE_EXT_IDX {
   return extract_v2cint16(v, idx, __SIGN_SIGNED);
}

INTRINSIC(cint32) extract_elem(v8cint32 v, int idx) DIAGNOSE_EXT_IDX {
   return extract_elem(v, idx, __SIGN_SIGNED);
}
#endif

// Extract 20 bit element (pointer) from a vector
INTRINSIC(void *) ext_address(v64int8 v, int idx) {
  return (void *)__builtin_aiev2_vextract_elem8_I512(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(void *) ext_address(v32int16 v, int idx) {
  return (void *)__builtin_aiev2_vextract_elem16_I512(v, idx, __SIGN_UNSIGNED);
}

INTRINSIC(void *) ext_address(v16int32 v, int idx) {
  return (void *)__builtin_aiev2_vextract_elem32_I512(v, idx, __SIGN_UNSIGNED);
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

#endif /*__AIEV2_SCL2VEC_H__*/
