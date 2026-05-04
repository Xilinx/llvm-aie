//===- aie_upd_ext_primitives.h ----------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Macro-free primitives shared by AIE2P and AIE2PS upd_ext headers.
// Includes:
//   - 64-bit scalar update/extract dispatchers (insert/set_uint64/...).
//   - Generic 256/512/1024-bit shuffle primitives (extract_*, insert_*,
//     set_*, concat_*).
// Each helper is branchless: a single `vsel` for runtime idx and a
// constant-folded single `mov`/`vmov`/`shufflevector` for compile-time
// idx. The per-arch upd_ext header includes this early so its later
// type wrappers can reference the primitives. The 128-bit
// extract/insert/set/concat helpers and type wrappers live in
// aie_upd_ext_common.h, which is included after the type wrappers
// because its `// Conversions` block references per-arch type wrappers
// like `extract_v8int32`.
//
//===----------------------------------------------------------------------===//

#ifndef __AIE_UPD_EXT_PRIMITIVES_H__
#define __AIE_UPD_EXT_PRIMITIVES_H__

// Scalar 64-bit updates and extracts.
// Branchless across both arches: a single vsel (runtime) or a single mov
// (constant idx). Replaces a per-arch if/else dispatch over an
// immediate-only intrinsic that previously failed ISel for runtime idx
// on AIE2P.
INTRINSIC(unsigned long long)
insert(unsigned long long a, int idx, unsigned int b) {
  v2uint32 temp = (v2uint32)a;
  temp[idx] = b;
  return (unsigned long long)temp;
}
INTRINSIC(unsigned long long) set_uint64(int idx, unsigned int b) {
  // Initialize the placeholder vector via a shufflevector with an all -1
  // mask: the C-level idiom that emits a poison value for the unwritten
  // lane (asm-equivalent to leaving the lane uninitialized; codegen DCEs
  // the unwritten lane either way).
  v2uint32 zero = {0, 0};
  v2uint32 temp = __builtin_shufflevector(zero, zero, -1, -1);
  temp[idx] = b;
  return (unsigned long long)temp;
}
INTRINSIC(unsigned int) extract_uint32(unsigned long long a, int idx) {
  v2uint32 temp = (v2uint32)a;
  return temp[idx];
}
INTRINSIC(unsigned long long) concat(unsigned int a, unsigned int b) {
  return insert(set_uint64(a, 0), 1, b);
}

// Generic 256-bit / 512-bit / 1024-bit extract / insert / set / concat
// primitives. Each helper precomputes the alternatives once and uses a
// vector `select` on bits of `idx`, instead of a runtime if/else cascade
// over distinct shufflevectors. For runtime idx this collapses to a single
// `vsel`/`sel.nez` (often in the delay slots of `ret`); for constant idx
// the speculative shufflevectors fold away leaving a single `mov` /
// `vmov`.

// Extract primitives
INTRINSIC(v8int32) extract_256_512(v16int32 a, int idx) {
  v8int32 lo = __builtin_shufflevector(a, a, 0, 1, 2, 3, 4, 5, 6, 7);
  v8int32 hi =
      __builtin_shufflevector(a, a, 8, 9, 10, 11, 12, 13, 14, 15);
  return (idx & 1) ? hi : lo;
}

INTRINSIC(v16int32) extract_512_1024(v32int32 a, int idx) {
  v16int32 lo = __builtin_shufflevector(a, a, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                        10, 11, 12, 13, 14, 15);
  v16int32 hi =
      __builtin_shufflevector(a, a, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                              26, 27, 28, 29, 30, 31);
  return (idx & 1) ? hi : lo;
}

INTRINSIC(v8int32) extract_256_1024(v32int32 a, int idx) {
  v8int32 q0 = __builtin_shufflevector(a, a, 0, 1, 2, 3, 4, 5, 6, 7);
  v8int32 q1 =
      __builtin_shufflevector(a, a, 8, 9, 10, 11, 12, 13, 14, 15);
  v8int32 q2 =
      __builtin_shufflevector(a, a, 16, 17, 18, 19, 20, 21, 22, 23);
  v8int32 q3 =
      __builtin_shufflevector(a, a, 24, 25, 26, 27, 28, 29, 30, 31);
  v8int32 lo = (idx & 1) ? q1 : q0;
  v8int32 hi = (idx & 1) ? q3 : q2;
  return (idx & 2) ? hi : lo;
}

INTRINSIC(v16acc64) extract_ACC1024_ACC2048(v32acc64 a, int idx) {
  v16acc64 lo = __builtin_shufflevector(a, a, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                        10, 11, 12, 13, 14, 15);
  v16acc64 hi =
      __builtin_shufflevector(a, a, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                              26, 27, 28, 29, 30, 31);
  return (idx & 1) ? hi : lo;
}

INTRINSIC(v8acc64) extract_ACC512_ACC2048(v32acc64 a, int idx) {
  v8acc64 q0 = __builtin_shufflevector(a, a, 0, 1, 2, 3, 4, 5, 6, 7);
  v8acc64 q1 =
      __builtin_shufflevector(a, a, 8, 9, 10, 11, 12, 13, 14, 15);
  v8acc64 q2 =
      __builtin_shufflevector(a, a, 16, 17, 18, 19, 20, 21, 22, 23);
  v8acc64 q3 =
      __builtin_shufflevector(a, a, 24, 25, 26, 27, 28, 29, 30, 31);
  v8acc64 lo = (idx & 1) ? q1 : q0;
  v8acc64 hi = (idx & 1) ? q3 : q2;
  return (idx & 2) ? hi : lo;
}

// Insert primitives
INTRINSIC(v16int32) insert_256_512(v16int32 a, int idx, v8int32 b) {
  v16int32 b_w = __builtin_shufflevector(b, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                         10, 11, 12, 13, 14, 15);
  v16int32 lo_inserted =
      __builtin_shufflevector(b_w, a, 0, 1, 2, 3, 4, 5, 6, 7, 24, 25, 26,
                              27, 28, 29, 30, 31);
  v16int32 hi_inserted = __builtin_shufflevector(
      a, b_w, 0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 18, 19, 20, 21, 22, 23);
  return (idx & 1) ? hi_inserted : lo_inserted;
}

INTRINSIC(v32int32) insert_512_1024(v32int32 a, int idx, v16int32 b) {
  v32int32 b_w = __builtin_shufflevector(
      b, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
      18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  v32int32 lo_inserted = __builtin_shufflevector(
      b_w, a, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 48, 49,
      50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
  v32int32 hi_inserted = __builtin_shufflevector(
      a, b_w, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 32, 33,
      34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47);
  return (idx & 1) ? hi_inserted : lo_inserted;
}

INTRINSIC(v32int32) insert_256_1024(v32int32 a, int idx, v8int32 b) {
  // Widen b once into each of the 4 quadrants of a v32int32.
  v32int32 b_q0 = __builtin_shufflevector(
      b, b, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
  v32int32 b_q1 = __builtin_shufflevector(
      b, b, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
  v32int32 b_q2 = __builtin_shufflevector(
      b, b, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1);
  v32int32 b_q3 = __builtin_shufflevector(
      b, b, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7);
  v32int32 q0_inserted = __builtin_shufflevector(
      b_q0, a, 0, 1, 2, 3, 4, 5, 6, 7, 40, 41, 42, 43, 44, 45, 46, 47, 48,
      49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
  v32int32 q1_inserted = __builtin_shufflevector(
      b_q1, a, 32, 33, 34, 35, 36, 37, 38, 39, 8, 9, 10, 11, 12, 13, 14, 15,
      48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
  v32int32 q2_inserted = __builtin_shufflevector(
      b_q2, a, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
      47, 16, 17, 18, 19, 20, 21, 22, 23, 56, 57, 58, 59, 60, 61, 62, 63);
  v32int32 q3_inserted = __builtin_shufflevector(
      b_q3, a, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
      47, 48, 49, 50, 51, 52, 53, 54, 55, 24, 25, 26, 27, 28, 29, 30, 31);
  v32int32 lo = (idx & 1) ? q1_inserted : q0_inserted;
  v32int32 hi = (idx & 1) ? q3_inserted : q2_inserted;
  return (idx & 2) ? hi : lo;
}

INTRINSIC(v32acc64) insert_ACC1024_ACC2048(v32acc64 a, int idx,
                                           v16acc64 b) {
  v32acc64 b_w = __builtin_shufflevector(
      b, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
  v32acc64 lo_inserted = __builtin_shufflevector(
      b_w, a, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 48, 49,
      50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
  v32acc64 hi_inserted = __builtin_shufflevector(
      a, b_w, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 32, 33,
      34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47);
  return (idx & 1) ? hi_inserted : lo_inserted;
}

INTRINSIC(v32acc64) insert_ACC512_ACC2048(v32acc64 a, int idx, v8acc64 b) {
  // Widen b into each quadrant of a v32acc64.
  v32acc64 b_q0 = __builtin_shufflevector(
      b, b, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
  v32acc64 b_q1 = __builtin_shufflevector(
      b, b, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
  v32acc64 b_q2 = __builtin_shufflevector(
      b, b, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1);
  v32acc64 b_q3 = __builtin_shufflevector(
      b, b, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7);
  v32acc64 q0_inserted = __builtin_shufflevector(
      b_q0, a, 0, 1, 2, 3, 4, 5, 6, 7, 40, 41, 42, 43, 44, 45, 46, 47, 48,
      49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
  v32acc64 q1_inserted = __builtin_shufflevector(
      a, b_q1, 0, 1, 2, 3, 4, 5, 6, 7, 40, 41, 42, 43, 44, 45, 46, 47, 16,
      17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  v32acc64 q2_inserted = __builtin_shufflevector(
      a, b_q2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 48, 49,
      50, 51, 52, 53, 54, 55, 24, 25, 26, 27, 28, 29, 30, 31);
  v32acc64 q3_inserted = __builtin_shufflevector(
      a, b_q3, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
      18, 19, 20, 21, 22, 23, 56, 57, 58, 59, 60, 61, 62, 63);
  v32acc64 lo = (idx & 1) ? q1_inserted : q0_inserted;
  v32acc64 hi = (idx & 1) ? q3_inserted : q2_inserted;
  return (idx & 2) ? hi : lo;
}

// Set primitives — same shape as insert_* but with poison in the
// non-target lanes (caller commits a follow-up insert to populate the
// other lanes).
INTRINSIC(v16int32) set_256_512(int idx, v8int32 b) {
  v16int32 lo = __builtin_shufflevector(b, b, 0, 1, 2, 3, 4, 5, 6, 7, -1,
                                        -1, -1, -1, -1, -1, -1, -1);
  v16int32 hi = __builtin_shufflevector(b, b, -1, -1, -1, -1, -1, -1, -1,
                                        -1, 0, 1, 2, 3, 4, 5, 6, 7);
  return (idx & 1) ? hi : lo;
}

INTRINSIC(v32int32) set_512_1024(int idx, v16int32 b) {
  v32int32 lo = __builtin_shufflevector(
      b, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
  v32int32 hi = __builtin_shufflevector(
      b, b, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  return (idx & 1) ? hi : lo;
}

INTRINSIC(v32int32) set_256_1024(int idx, v8int32 b) {
  v32int32 q0 = __builtin_shufflevector(
      b, b, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
  v32int32 q1 = __builtin_shufflevector(
      b, b, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
  v32int32 q2 = __builtin_shufflevector(
      b, b, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1);
  v32int32 q3 = __builtin_shufflevector(
      b, b, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7);
  v32int32 lo = (idx & 1) ? q1 : q0;
  v32int32 hi = (idx & 1) ? q3 : q2;
  return (idx & 2) ? hi : lo;
}

INTRINSIC(v32acc64) set_ACC1024_ACC2048(int idx, v16acc64 b) {
  v32acc64 lo = __builtin_shufflevector(
      b, b, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
  v32acc64 hi = __builtin_shufflevector(
      b, b, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  return (idx & 1) ? hi : lo;
}

INTRINSIC(v32acc64) set_ACC512_ACC2048(int idx, v8acc64 b) {
  v32acc64 q0 = __builtin_shufflevector(
      b, b, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
  v32acc64 q1 = __builtin_shufflevector(
      b, b, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
  v32acc64 q2 = __builtin_shufflevector(
      b, b, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1);
  v32acc64 q3 = __builtin_shufflevector(
      b, b, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7);
  v32acc64 lo = (idx & 1) ? q1 : q0;
  v32acc64 hi = (idx & 1) ? q3 : q2;
  return (idx & 2) ? hi : lo;
}

// Concat primitives — already branchless (single shufflevector).
INTRINSIC(v16int32) concat_256_512(v8int32 a0, v8int32 a1) {
  return __builtin_shufflevector(a0, a1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                 11, 12, 13, 14, 15);
}

INTRINSIC(v32int32) concat_512_1024(v16int32 a0, v16int32 a1) {
  return __builtin_shufflevector(a0, a1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
}

INTRINSIC(v32int32)
concat_256_1024(v8int32 a0, v8int32 a1, v8int32 a2, v8int32 a3) {
  v16int32 res_hi = __builtin_shufflevector(
      a0, a1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  v16int32 res_lo = __builtin_shufflevector(
      a2, a3, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  return __builtin_shufflevector(res_hi, res_lo, 0, 1, 2, 3, 4, 5, 6, 7, 8,
                                 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
                                 31);
}

INTRINSIC(v32acc64) concat_ACC1024_ACC2048(v16acc64 a0, v16acc64 a1) {
  return __builtin_shufflevector(a0, a1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
}

INTRINSIC(v32acc64)
concat_ACC512_ACC2048(v8acc64 a0, v8acc64 a1, v8acc64 a2, v8acc64 a3) {
  v16acc64 res_lo = __builtin_shufflevector(
      a0, a1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  v16acc64 res_hi = __builtin_shufflevector(
      a2, a3, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  return __builtin_shufflevector(res_lo, res_hi, 0, 1, 2, 3, 4, 5, 6, 7, 8,
                                 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
                                 31);
}

#endif // __AIE_UPD_EXT_PRIMITIVES_H__
