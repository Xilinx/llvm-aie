//===- aie_upd_ext_common.h ------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Generic 128-bit extract / insert / set / concat primitives shared by the
// AIE2P and AIE2PS upd_ext headers. Including translation units must have
// the INTRINSIC macro and the AIE vector typedefs in scope (provided by the
// per-arch top-level intrin header before this file is included).
//
//===----------------------------------------------------------------------===//

#ifndef AIE_VEXTRACT_BROADCAST128_I512
#error                                                                         \
    "Define AIE_VEXTRACT_BROADCAST128_I512(v, idx) before including this file"
#endif

#ifndef __AIE_UPD_EXT_COMMON_H__
#define __AIE_UPD_EXT_COMMON_H__

// Generic 128-bit extract primitives
INTRINSIC(v4int32) extract_128_256(v8int32 a, int idx) {
  if (idx % 2 == 0)
    return __builtin_shufflevector(a, a, 0, 1, 2, 3);
  else
    return __builtin_shufflevector(a, a, 4, 5, 6, 7);
}

// Lower runtime-idx 128-bit extract via the per-arch VEXTBCST.128
// instruction (a single op that extracts the chosen 128-bit lane and
// broadcasts it into a 512-bit vector). The shufflevector then keeps the
// low 128 bits, which the broadcast guarantees equal to the chosen lane.
// This avoids the 4-way switch+shufflevector+phi shape that the previous
// if-chain produced for runtime indices, which blocked HW-loop formation.
INTRINSIC(v4int32) extract_128_512(v16int32 a, int idx) {
  v16int32 bcst = AIE_VEXTRACT_BROADCAST128_I512(a, idx);
  return __builtin_shufflevector(bcst, bcst, 0, 1, 2, 3);
}

// Generic 128-bit insert primitives
INTRINSIC(v8int32) insert_128_256(v8int32 a, int idx, v4int32 b) {
  v4int32 unused;
  v8int32 tmp = __builtin_shufflevector(b, unused, 0, 1, 2, 3, 4, 5, 6, 7);
  if (idx % 2 == 0)
    return __builtin_shufflevector(tmp, a, 0, 1, 2, 3, 12, 13, 14, 15);
  else
    return __builtin_shufflevector(a, tmp, 0, 1, 2, 3, 8, 9, 10, 11);
}

INTRINSIC(v16int32) insert_128_512(v16int32 a, int idx, v4int32 b) {
  v4int32 unused_128;
  v8int32 unused_256;
  v8int32 tmp_256 =
      __builtin_shufflevector(b, unused_128, 0, 1, 2, 3, 4, 5, 6, 7);
  v16int32 tmp_512 =
      __builtin_shufflevector(tmp_256, unused_256, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                              10, 11, 12, 13, 14, 15);
  if (idx % 4 == 0)
    return __builtin_shufflevector(tmp_512, a, 0, 1, 2, 3, 20, 21, 22, 23, 24,
                                   25, 26, 27, 28, 29, 30, 31);
  if (idx % 4 == 1)
    return __builtin_shufflevector(tmp_512, a, 16, 17, 18, 19, 0, 1, 2, 3, 24,
                                   25, 26, 27, 28, 29, 30, 31);
  if (idx % 4 == 2)
    return __builtin_shufflevector(tmp_512, a, 16, 17, 18, 19, 20, 21, 22, 23,
                                   0, 1, 2, 3, 28, 29, 30, 31);
  else
    return __builtin_shufflevector(tmp_512, a, 16, 17, 18, 19, 20, 21, 22, 23,
                                   24, 25, 26, 27, 0, 1, 2, 3);
}

// Generic 128-bit set primitives
INTRINSIC(v8int32) set_128_256(int idx, v4int32 b) {
  v4int32 tmp;
  if (idx % 2 == 0)
    return __builtin_shufflevector(b, tmp, 0, 1, 2, 3, 4, 5, 6, 7);
  else
    return __builtin_shufflevector(tmp, b, 0, 1, 2, 3, 4, 5, 6, 7);
}

INTRINSIC(v16int32) set_128_512(int idx, v4int32 b) {
  v4int32 tmp;
  v8int32 tmp2;
  v8int32 tmp3;
  if (idx % 4 == 0) {
    tmp2 = __builtin_shufflevector(b, tmp, 0, 1, 2, 3, 4, 5, 6, 7);
    return __builtin_shufflevector(tmp2, tmp3, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15);
  }
  if (idx % 4 == 1) {
    tmp2 = __builtin_shufflevector(tmp, b, 0, 1, 2, 3, 4, 5, 6, 7);
    return __builtin_shufflevector(tmp2, tmp3, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15);
  }
  if (idx % 4 == 2) {
    tmp2 = __builtin_shufflevector(b, tmp, 0, 1, 2, 3, 4, 5, 6, 7);
    return __builtin_shufflevector(tmp3, tmp2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15);
  } else {
    tmp2 = __builtin_shufflevector(tmp, b, 0, 1, 2, 3, 4, 5, 6, 7);
    return __builtin_shufflevector(tmp3, tmp2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                   11, 12, 13, 14, 15);
  }
}
// Generic 128-bit concat primitives
INTRINSIC(v8int32) concat_128_256(v4int32 a0, v4int32 a1) {
  return __builtin_shufflevector(a0, a1, 0, 1, 2, 3, 4, 5, 6, 7);
}
INTRINSIC(v16int32)
concat_128_512(v4int32 a0, v4int32 a1, v4int32 a2, v4int32 a3) {
  v8int32 res_hi;
  v8int32 res_lo;
  res_hi = __builtin_shufflevector(a0, a1, 0, 1, 2, 3, 4, 5, 6, 7);
  res_lo = __builtin_shufflevector(a2, a3, 0, 1, 2, 3, 4, 5, 6, 7);
  return __builtin_shufflevector(res_hi, res_lo, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                 10, 11, 12, 13, 14, 15);
}

#define DIAGNOSE_EXTRACT_IDX(MAX)                                              \
  __attribute__((diagnose_if(idx < 0 || idx > MAX,                             \
                             "index out of range [0," #MAX "]", "error")))

INTRINSIC(v32uint4)
extract_v32uint4(v128uint4 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

INTRINSIC(v32int4)
extract_v32int4(v128int4 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

INTRINSIC(v16uint8)
extract_v16uint8(v64uint8 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

INTRINSIC(v16int8) extract_v16int8(v64int8 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

INTRINSIC(v8uint16)
extract_v8uint16(v32uint16 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

INTRINSIC(v8int16)
extract_v8int16(v32int16 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

INTRINSIC(v4uint32)
extract_v4uint32(v16uint32 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

INTRINSIC(v4int32)
extract_v4int32(v16int32 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

INTRINSIC(v8bfloat16)
extract_v8bfloat16(v32bfloat16 a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

INTRINSIC(v4float)
extract_v4float(v16float a, int idx) DIAGNOSE_EXTRACT_IDX(3) {
  return extract_128_512(a, idx);
}

INTRINSIC(v128uint4)
set_v128uint4(int idx, v32uint4 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

INTRINSIC(v128int4) set_v128int4(int idx, v32int4 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

INTRINSIC(v64uint8) set_v64uint8(int idx, v16uint8 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

INTRINSIC(v64int8) set_v64int8(int idx, v16int8 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

INTRINSIC(v32uint16)
set_v32uint16(int idx, v8uint16 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

INTRINSIC(v32int16) set_v32int16(int idx, v8int16 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

INTRINSIC(v16uint32)
set_v16uint32(int idx, v4uint32 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

INTRINSIC(v16int32) set_v16int32(int idx, v4int32 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

INTRINSIC(v32bfloat16)
set_v32bfloat16(int idx, v8bfloat16 a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

INTRINSIC(v16float) set_v16float(int idx, v4float a) DIAGNOSE_EXTRACT_IDX(3) {
  return set_128_512(idx, a);
}

INTRINSIC(v32uint4)
extract_v32uint4(v64uint4 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

INTRINSIC(v32int4) extract_v32int4(v64int4 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

INTRINSIC(v16uint8)
extract_v16uint8(v32uint8 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

INTRINSIC(v16int8) extract_v16int8(v32int8 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

INTRINSIC(v8uint16)
extract_v8uint16(v16uint16 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

INTRINSIC(v8int16)
extract_v8int16(v16int16 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

INTRINSIC(v4uint32)
extract_v4uint32(v8uint32 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

INTRINSIC(v4int32) extract_v4int32(v8int32 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

INTRINSIC(v8bfloat16)
extract_v8bfloat16(v16bfloat16 a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

INTRINSIC(v4float) extract_v4float(v8float a, int idx) DIAGNOSE_EXTRACT_IDX(1) {
  return extract_128_256(a, idx);
}

INTRINSIC(v64uint4) set_v64uint4(int idx, v32uint4 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}
INTRINSIC(v64int4) set_v64int4(int idx, v32int4 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}
INTRINSIC(v32uint8) set_v32uint8(int idx, v16uint8 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}
INTRINSIC(v32int8) set_v32int8(int idx, v16int8 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}
INTRINSIC(v16uint16)
set_v16uint16(int idx, v8uint16 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}
INTRINSIC(v16int16) set_v16int16(int idx, v8int16 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}
INTRINSIC(v8uint32) set_v8uint32(int idx, v4uint32 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}
INTRINSIC(v8int32) set_v8int32(int idx, v4int32 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}
INTRINSIC(v16bfloat16)
set_v16bfloat16(int idx, v8bfloat16 a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}
INTRINSIC(v8float) set_v8float(int idx, v4float a) DIAGNOSE_EXTRACT_IDX(1) {
  return set_128_256(idx, a);
}

INTRINSIC(v128uint4)
insert(v128uint4 v, int idx, v32uint4 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}
INTRINSIC(v128int4)
insert(v128int4 v, int idx, v32int4 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}
INTRINSIC(v64uint8)
insert(v64uint8 v, int idx, v16uint8 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}
INTRINSIC(v64int8)
insert(v64int8 v, int idx, v16int8 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}
INTRINSIC(v32uint16)
insert(v32uint16 v, int idx, v8uint16 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}
INTRINSIC(v32int16)
insert(v32int16 v, int idx, v8int16 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}
INTRINSIC(v16uint32)
insert(v16uint32 v, int idx, v4uint32 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}
INTRINSIC(v16int32)
insert(v16int32 v, int idx, v4int32 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}
INTRINSIC(v32bfloat16)
insert(v32bfloat16 v, int idx, v8bfloat16 b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}
INTRINSIC(v16float)
insert(v16float v, int idx, v4float b) DIAGNOSE_EXTRACT_IDX(3) {
  return insert_128_512(v, idx, b);
}

INTRINSIC(v64uint4)
insert(v64uint4 a, int idx, v32uint4 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}
INTRINSIC(v64int4)
insert(v64int4 a, int idx, v32int4 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}
INTRINSIC(v32uint8)
insert(v32uint8 a, int idx, v16uint8 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}
INTRINSIC(v32int8)
insert(v32int8 a, int idx, v16int8 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}
INTRINSIC(v16uint16)
insert(v16uint16 a, int idx, v8uint16 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}
INTRINSIC(v16int16)
insert(v16int16 a, int idx, v8int16 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}
INTRINSIC(v8uint32)
insert(v8uint32 a, int idx, v4uint32 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}
INTRINSIC(v8int32)
insert(v8int32 a, int idx, v4int32 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}
INTRINSIC(v16bfloat16)
insert(v16bfloat16 a, int idx, v8bfloat16 b) DIAGNOSE_EXTRACT_IDX(1) {
  return insert_128_256(a, idx, b);
}
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

// Conversions
// v8accfloat
INTRINSIC(v8accfloat) extract_v8accfloat(v16accfloat a, int idx) {
  return (v8accfloat)extract_v8int32((v16int32)a, idx);
}

INTRINSIC(v16accfloat) insert(v16accfloat a, int idx, v8accfloat b) {
  return (v16accfloat)insert((v16int32)a, idx, (v8int32)b);
}

INTRINSIC(v16accfloat) set_v16accfloat(int idx, v8accfloat b) {
  return (v16accfloat)set_v16int32(idx, (v8int32)b);
}

INTRINSIC(v16accfloat) concat(v8accfloat a, v8accfloat b) {
  return (v16accfloat)concat((v8int32)a, (v8int32)b);
}
// v8acc32
INTRINSIC(v8acc32) extract_v8acc32(v16acc32 a, int idx) {
  return (v8acc32)extract_v8int32((v16int32)a, idx);
}

INTRINSIC(v16acc32) insert(v16acc32 a, int idx, v8acc32 b) {
  return (v16acc32)insert((v16int32)a, idx, (v8int32)b);
}

INTRINSIC(v16acc32) set_v16acc32(int idx, v8acc32 b) {
  return (v16acc32)set_v16int32(idx, (v8int32)b);
}

INTRINSIC(v16acc32) concat(v8acc32 a, v8acc32 b) {
  return (v16acc32)concat((v8int32)a, (v8int32)b);
}
// v4acc64
INTRINSIC(v4acc64) extract_v4acc64(v8acc64 a, int idx) {
  return (v4acc64)extract_v8int32((v16int32)a, idx);
}

INTRINSIC(v8acc64) insert(v8acc64 a, int idx, v4acc64 b) {
  return (v8acc64)insert((v16int32)a, idx, (v8int32)b);
}

INTRINSIC(v8acc64) set_v8acc64(int idx, v4acc64 b) {
  return (v8acc64)set_v16int32(idx, (v8int32)b);
}

INTRINSIC(v8acc64) concat(v4acc64 a, v4acc64 b) {
  return (v8acc64)concat((v8int32)a, (v8int32)b);
}
// v8accfloat
INTRINSIC(v8accfloat) extract_v8accfloat(v32accfloat a, int idx) {
  return (v8accfloat)extract_v8int32((v32int32)a, idx);
}

INTRINSIC(v32accfloat) insert(v32accfloat a, int idx, v8accfloat b) {
  return (v32accfloat)insert((v32int32)a, idx, (v8int32)b);
}

INTRINSIC(v32accfloat) set_v32accfloat(int idx, v8accfloat b) {
  return (v32accfloat)set_v32int32(idx, (v8int32)b);
}

INTRINSIC(v32accfloat)
concat(v8accfloat a, v8accfloat b, v8accfloat c, v8accfloat d) {
  return (v32accfloat)concat((v8int32)a, (v8int32)b, (v8int32)c, (v8int32)d);
}
// v8acc32
INTRINSIC(v8acc32) extract_v8acc32(v32acc32 a, int idx) {
  return (v8acc32)extract_v8int32((v32int32)a, idx);
}

INTRINSIC(v32acc32) insert(v32acc32 a, int idx, v8acc32 b) {
  return (v32acc32)insert((v32int32)a, idx, (v8int32)b);
}

INTRINSIC(v32acc32) set_v32acc32(int idx, v8acc32 b) {
  return (v32acc32)set_v32int32(idx, (v8int32)b);
}

INTRINSIC(v32acc32) concat(v8acc32 a, v8acc32 b, v8acc32 c, v8acc32 d) {
  return (v32acc32)concat((v8int32)a, (v8int32)b, (v8int32)c, (v8int32)d);
}
// v4acc64
INTRINSIC(v4acc64) extract_v4acc64(v16acc64 a, int idx) {
  return (v4acc64)extract_v8int32((v32int32)a, idx);
}

INTRINSIC(v16acc64) insert(v16acc64 a, int idx, v4acc64 b) {
  return (v16acc64)insert((v32int32)a, idx, (v8int32)b);
}

INTRINSIC(v16acc64) set_v16acc64(int idx, v4acc64 b) {
  return (v16acc64)set_v32int32(idx, (v8int32)b);
}

INTRINSIC(v16acc64) concat(v4acc64 a, v4acc64 b, v4acc64 c, v4acc64 d) {
  return (v16acc64)concat((v8int32)a, (v8int32)b, (v8int32)c, (v8int32)d);
}

#endif // __AIE_UPD_EXT_COMMON_H__
