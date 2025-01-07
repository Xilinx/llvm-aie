//===- aie2p_streams.h -----------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

#ifndef __AIE2P_STREAMS_H__
#define __AIE2P_STREAMS_H__

INTRINSIC(v16acc32) get_scd_v16acc32(int en) {
  return __builtin_aie2p_scd_read_acc32(en);
}
INTRINSIC(v8acc64) get_scd_v8acc64(int en) {
  return (v8acc64)__builtin_aie2p_scd_read_acc32(en);
}
INTRINSIC(v16accfloat) get_scd_v16accfloat(int en) {
  return (v16accfloat)get_scd_v16acc32(en);
}
[[deprecated("Function 'v16acc32 get_scd(int)' is deprecated. Please use "
             "'v16acc32 get_scd_v16acc32(int)' instead.")]] INTRINSIC(v16acc32)
    get_scd(int en) {
  return get_scd_v16acc32(en);
}
[[deprecated("Function 'v8acc64  getl_scd(int)' is deprecated. Please use "
             "'v8acc64 get_scd_v8acc64(int)' instead.")]] INTRINSIC(v8acc64)
    getl_scd(int en) {
  return get_scd_v8acc64(en);
}
[[deprecated(
    "Function 'v16accfloat getf_scd(int)' is deprecated. Please use "
    "'v16accfloat get_scd_v16accfloat(int)' instead.")]] INTRINSIC(v16accfloat)
    getf_scd(int en) {
  return get_scd_v16accfloat(en);
}
INTRINSIC(v128int4) get_scd_v128int4(int en) {
  return (v128int4)__builtin_aie2p_scd_read_vec(en);
}
INTRINSIC(v128uint4) get_scd_v128uint4(int en) {
  return (v128uint4)__builtin_aie2p_scd_read_vec(en);
}
INTRINSIC(v64int8) get_scd_v64int8(int en) {
  return (v64int8)__builtin_aie2p_scd_read_vec(en);
}
INTRINSIC(v64uint8) get_scd_v64uint8(int en) {
  return (v64uint8)__builtin_aie2p_scd_read_vec(en);
}
INTRINSIC(v32int16) get_scd_v32int16(int en) {
  return (v32int16)__builtin_aie2p_scd_read_vec(en);
}
INTRINSIC(v32uint16) get_scd_v32uint16(int en) {
  return (v32uint16)__builtin_aie2p_scd_read_vec(en);
}
INTRINSIC(v16int32) get_scd_v16int32(int en) {
  return (v16int32)__builtin_aie2p_scd_read_vec(en);
}
INTRINSIC(v16uint32) get_scd_v16uint32(int en) {
  return (v16uint32)__builtin_aie2p_scd_read_vec(en);
}
INTRINSIC(v32bfloat16) get_scd_v32bfloat16(int en) {
  return (v32bfloat16)__builtin_aie2p_scd_read_vec(en);
}
INTRINSIC(v16float) get_scd_v16float(int en) {
  return (v16float)__builtin_aie2p_scd_read_vec(en);
}
INTRINSIC(v16acc32) get_scd_v16acc32() { return get_scd_v16acc32(1); }
INTRINSIC(v8acc64) get_scd_v8acc64() { return get_scd_v8acc64(1); }
INTRINSIC(v16accfloat) get_scd_v16accfloat() { return get_scd_v16accfloat(1); }

[[deprecated("Function 'v16acc32 get_scd()' is deprecated. Please use "
             "'v16acc32 get_scd_v16acc32()' instead.")]] INTRINSIC(v16acc32)
    get_scd() {
  return get_scd_v16acc32();
}
[[deprecated("Function 'v8acc64  getl_scd()' is deprecated. Please use "
             "'v8acc64 get_scd_v8acc64()' instead.")]] INTRINSIC(v8acc64)
    getl_scd() {
  return get_scd_v8acc64();
}
[[deprecated(
    "Function 'v16accfloat getf_scd()' is deprecated. Please use "
    "'v16accfloat get_scd_v16accfloat()' instead.")]] INTRINSIC(v16accfloat)
    getf_scd() {
  return get_scd_v16accfloat();
}

INTRINSIC(v128int4) get_scd_v128int4() { return get_scd_v128int4(1); }
INTRINSIC(v128uint4) get_scd_v128uint4() { return get_scd_v128uint4(1); }
INTRINSIC(v64int8) get_scd_v64int8() { return get_scd_v64int8(1); }
INTRINSIC(v64uint8) get_scd_v64uint8() { return get_scd_v64uint8(1); }
INTRINSIC(v32int16) get_scd_v32int16() { return get_scd_v32int16(1); }
INTRINSIC(v32uint16) get_scd_v32uint16() { return get_scd_v32uint16(1); }
INTRINSIC(v16int32) get_scd_v16int32() { return get_scd_v16int32(1); }
INTRINSIC(v16uint32) get_scd_v16uint32() { return get_scd_v16uint32(1); }
INTRINSIC(v32bfloat16) get_scd_v32bfloat16() { return get_scd_v32bfloat16(1); }
INTRINSIC(v16float) get_scd_v16float() { return get_scd_v16float(1); }

INTRINSIC(v32acc32) get_scd_v32acc32(int en) {
  v32acc32 r;
  r = set_v32acc32(0, get_scd_v16acc32(en));
  r = insert(r, 1, get_scd_v16acc32(en));
  return r;
}
INTRINSIC(v16acc64) get_scd_v16acc64(int en) {
  return (v16acc64)get_scd_v32acc32(en);
}
INTRINSIC(v32accfloat) get_scd_v32accfloat(int en) {
  return (v32accfloat)get_scd_v32acc32(en);
}
INTRINSIC(v32int32) get_scd_v32int32(int en) {
  v32int32 r;
  r = set_v32int32(0, get_scd_v16int32(en));
  r = insert(r, 1, get_scd_v16int32(en));
  return r;
}
INTRINSIC(v256int4) get_scd_v256int4(int en) {
  return (v256int4)get_scd_v32int32(en);
}
INTRINSIC(v256uint4) get_scd_v256uint4(int en) {
  return (v256uint4)get_scd_v32int32(en);
}
INTRINSIC(v128int8) get_scd_v128int8(int en) {
  return (v128int8)get_scd_v32int32(en);
}
INTRINSIC(v128uint8) get_scd_v128uint8(int en) {
  return (v128uint8)get_scd_v32int32(en);
}
INTRINSIC(v64int16) get_scd_v64int16(int en) {
  return (v64int16)get_scd_v32int32(en);
}
INTRINSIC(v64uint16) get_scd_v64uint16(int en) {
  return (v64uint16)get_scd_v32int32(en);
}
INTRINSIC(v32uint32) get_scd_v32uint32(int en) {
  return (v32uint32)get_scd_v32int32(en);
}
INTRINSIC(v32float) get_scd_v32float(int en) {
  return (v32float)get_scd_v32int32(en);
}
INTRINSIC(v32acc32) get_scd_v32acc32() { return get_scd_v32acc32(1); }
INTRINSIC(v16acc64) get_scd_v16acc64() { return get_scd_v16acc64(1); }
INTRINSIC(v256int4) get_scd_v256int4() { return get_scd_v256int4(1); }
INTRINSIC(v256uint4) get_scd_v256uint4() { return get_scd_v256uint4(1); }
INTRINSIC(v128int8) get_scd_v128int8() { return get_scd_v128int8(1); }
INTRINSIC(v128uint8) get_scd_v128uint8() { return get_scd_v128uint8(1); }
INTRINSIC(v64int16) get_scd_v64int16() { return get_scd_v64int16(1); }
INTRINSIC(v64uint16) get_scd_v64uint16() { return get_scd_v64uint16(1); }
INTRINSIC(v32int32) get_scd_v32int32() { return get_scd_v32int32(1); }
INTRINSIC(v32uint32) get_scd_v32uint32() { return get_scd_v32uint32(1); }
INTRINSIC(v32float) get_scd_v32float() { return get_scd_v32float(1); }
INTRINSIC(v64acc32) get_scd_v64acc32(int en) {
  v64acc32 r;
  r = set_v64acc32(0, get_scd_v16acc32(en));
  r = insert(r, 1, get_scd_v16acc32(en));
  r = insert(r, 2, get_scd_v16acc32(en));
  r = insert(r, 3, get_scd_v16acc32(en));
  return r;
}
INTRINSIC(v32acc64) get_scd_v32acc64(int en) {
  return (v32acc64)get_scd_v64acc32(en);
}
INTRINSIC(v64accfloat) get_scd_v64accfloat(int en) {
  return (v64accfloat)get_scd_v64acc32(en);
}
INTRINSIC(v64acc32) get_scd_v64acc32() { return get_scd_v64acc32(1); }
INTRINSIC(v32acc64) get_scd_v32acc64() { return get_scd_v32acc64(1); }
INTRINSIC(v32acc32) get_scd_v32acc32_lo(int en) {
  return __builtin_aie2p_scd_expand_lo(en);
}
INTRINSIC(v32acc32) get_scd_v32acc32_hi(int en) {
  return __builtin_aie2p_scd_expand_hi(en);
}
INTRINSIC(v16acc64) get_scd_v16acc64_lo(int en) {
  return (v16acc64)get_scd_v32acc32_lo(en);
}
INTRINSIC(v16acc64) get_scd_v16acc64_hi(int en) {
  return (v16acc64)get_scd_v32acc32_hi(en);
}
INTRINSIC(v32accfloat) get_scd_v32accfloat_lo(int en) {
  return (v32accfloat)get_scd_v32acc32_lo(en);
}
INTRINSIC(v32accfloat) get_scd_v32accfloat_hi(int en) {
  return (v32accfloat)get_scd_v32acc32_hi(en);
}
[[deprecated(
    "Function 'v32acc32 get_scd_lo(int en)' is deprecated. Please use "
    "'v32acc32 get_scd_v32acc32_lo(int)' instead.")]] INTRINSIC(v32acc32)
    get_scd_lo(int en) {
  return get_scd_v32acc32_lo(en);
}
[[deprecated(
    "Function 'v32acc32 get_scd_hi(int en)' is deprecated. Please use "
    "'v32acc32 get_scd_v32acc32_hi(int)' instead.")]] INTRINSIC(v32acc32)
    get_scd_hi(int en) {
  return get_scd_v32acc32_hi(en);
}
[[deprecated(
    "Function 'v16acc64 getl_scd_lo(int en)' is deprecated. Please "
    "use 'v16acc64 getl_scd_v16acc64_lo(int)' instead.")]] INTRINSIC(v16acc64)
    getl_scd_lo(int en) {
  return get_scd_v16acc64_lo(en);
}
[[deprecated(
    "Function 'v16acc64 getl_scd_hi(int en)' is deprecated. Please "
    "use 'v16acc64 getl_scd_v16acc64_hi(int)' instead.")]] INTRINSIC(v16acc64)
    getl_scd_hi(int en) {
  return get_scd_v16acc64_hi(en);
}

INTRINSIC(v32acc32) get_scd_v32acc32_lo() { return get_scd_v32acc32_lo(1); }
INTRINSIC(v32acc32) get_scd_v32acc32_hi() { return get_scd_v32acc32_hi(1); }
INTRINSIC(v16acc64) get_scd_v16acc64_lo() { return get_scd_v16acc64_lo(1); }
INTRINSIC(v16acc64) get_scd_v16acc64_hi() { return get_scd_v16acc64_hi(1); }
INTRINSIC(v32accfloat) get_scd_v32accfloat_lo() {
  return get_scd_v32accfloat_lo(1);
}
INTRINSIC(v32accfloat) get_scd_v32accfloat_hi() {
  return get_scd_v32accfloat_hi(1);
}
INTRINSIC(v32acc32) get_scd_expand_v32acc32_incr(int en, int &pos) {
  return __builtin_aie2p_scd_expand_ACC1024_incr(en, (void *)pos, pos);
}
INTRINSIC(v16acc64) get_scd_expand_v16acc64_incr(int en, int &pos) {
  return (v16acc64)get_scd_expand_v32acc32_incr(en, pos);
}
INTRINSIC(v32accfloat) get_scd_expand_v32accfloat_incr(int en, int &pos) {
  return (v32accfloat)get_scd_expand_v32acc32_incr(en, pos);
}
INTRINSIC(v32acc32) get_scd_expand_v32acc32_incr(int &pos) {
  return get_scd_expand_v32acc32_incr(1, pos);
}
INTRINSIC(v16acc64) get_scd_expand_v16acc64_incr(int &pos) {
  return get_scd_expand_v16acc64_incr(1, pos);
}
INTRINSIC(v32accfloat) get_scd_expand_v32accfloat_incr(int &pos) {
  return get_scd_expand_v32accfloat_incr(1, pos);
}
INTRINSIC(v32acc32) get_scd_expand_v32acc32(int en, int pos) {
  return __builtin_aie2p_scd_expand_ACC1024(en, pos);
}
INTRINSIC(v16acc64) get_scd_expand_v16acc64(int en, int pos) {
  return (v16acc64)get_scd_expand_v32acc32(en, pos);
}
INTRINSIC(v32accfloat) get_scd_expand_v32accfloat(int en, int pos) {
  return (v32accfloat)get_scd_expand_v32acc32(en, pos);
}
INTRINSIC(v32acc32) get_scd_expand_v32acc32(int pos) {
  return get_scd_expand_v32acc32(1, pos);
}
INTRINSIC(v16acc64) get_scd_expand_v16acc64(int pos) {
  return get_scd_expand_v16acc64(1, pos);
}
INTRINSIC(v32accfloat) get_scd_expand_v32accfloat(int pos) {
  return get_scd_expand_v32accfloat(1, pos);
}
[[deprecated("Function 'v32acc32 get_scd_lo()' is deprecated. Please use "
             "'v32acc32 get_scd_v32acc32_lo()' instead.")]] INTRINSIC(v32acc32)
    get_scd_lo() {
  return get_scd_v32acc32_lo();
}
[[deprecated("Function 'v32acc32 get_scd_hi()' is deprecated. Please use "
             "'v32acc32 get_scd_v32acc32_hi()' instead.")]] INTRINSIC(v32acc32)
    get_scd_hi() {
  return get_scd_v32acc32_hi();
}
[[deprecated("Function 'v16acc64 getl_scd_lo()' is deprecated. Please use "
             "'v16acc64 getl_scd_v16acc64_lo()' instead.")]] INTRINSIC(v16acc64)
    getl_scd_lo() {
  return get_scd_v16acc64_lo();
}
[[deprecated("Function 'v16acc64 getl_scd_hi()' is deprecated. Please use "
             "'v16acc64 getl_scd_v16acc64_hi()' instead.")]] INTRINSIC(v16acc64)
    getl_scd_hi() {
  return get_scd_v16acc64_hi();
}
INTRINSIC(v64acc32) get_scd_v64acc32_0(int en) {
  return __builtin_aie2p_scd_ACC2048(en, 0);
}
INTRINSIC(v64acc32) get_scd_v64acc32_1(int en) {
  return __builtin_aie2p_scd_ACC2048(en, 1);
}
INTRINSIC(v64acc32) get_scd_v64acc32_2(int en) {
  return __builtin_aie2p_scd_ACC2048(en, 2);
}
INTRINSIC(v64acc32) get_scd_v64acc32_3(int en) {
  return __builtin_aie2p_scd_ACC2048(en, 3);
}
INTRINSIC(v32acc64) get_scd_v32acc64_0(int en) {
  return (v32acc64)get_scd_v64acc32_0(en);
}
INTRINSIC(v32acc64) get_scd_v32acc64_1(int en) {
  return (v32acc64)get_scd_v64acc32_1(en);
}
INTRINSIC(v32acc64) get_scd_v32acc64_2(int en) {
  return (v32acc64)get_scd_v64acc32_2(en);
}
INTRINSIC(v32acc64) get_scd_v32acc64_3(int en) {
  return (v32acc64)get_scd_v64acc32_3(en);
}
INTRINSIC(v64accfloat) get_scd_v64accfloat_0(int en) {
  return (v64accfloat)get_scd_v64acc32_0(en);
}
INTRINSIC(v64accfloat) get_scd_v64accfloat_1(int en) {
  return (v64accfloat)get_scd_v64acc32_1(en);
}
INTRINSIC(v64accfloat) get_scd_v64accfloat_2(int en) {
  return (v64accfloat)get_scd_v64acc32_2(en);
}
INTRINSIC(v64accfloat) get_scd_v64accfloat_3(int en) {
  return (v64accfloat)get_scd_v64acc32_3(en);
}

INTRINSIC(v64acc32) get_scd_v64acc32_0() { return get_scd_v64acc32_0(1); }
INTRINSIC(v64acc32) get_scd_v64acc32_1() { return get_scd_v64acc32_1(1); }
INTRINSIC(v64acc32) get_scd_v64acc32_2() { return get_scd_v64acc32_2(1); }
INTRINSIC(v64acc32) get_scd_v64acc32_3() { return get_scd_v64acc32_3(1); }
INTRINSIC(v32acc64) get_scd_v32acc64_0() { return get_scd_v32acc64_0(1); }
INTRINSIC(v32acc64) get_scd_v32acc64_1() { return get_scd_v32acc64_1(1); }
INTRINSIC(v32acc64) get_scd_v32acc64_2() { return get_scd_v32acc64_2(1); }
INTRINSIC(v32acc64) get_scd_v32acc64_3() { return get_scd_v32acc64_3(1); }
INTRINSIC(v64accfloat) get_scd_v64accfloat_0() {
  return get_scd_v64accfloat_0(1);
}
INTRINSIC(v64accfloat) get_scd_v64accfloat_1() {
  return get_scd_v64accfloat_1(1);
}
INTRINSIC(v64accfloat) get_scd_v64accfloat_2() {
  return get_scd_v64accfloat_2(1);
}
INTRINSIC(v64accfloat) get_scd_v64accfloat_3() {
  return get_scd_v64accfloat_3(1);
}
INTRINSIC(v64acc32) get_scd_expand_v64acc32_incr(int en, int &pos) {
  return __builtin_aie2p_scd_expand_ACC2048_incr(en, (void *)pos, pos);
}
INTRINSIC(v32acc64) get_scd_expand_v32acc64_incr(int en, int &pos) {
  return (v32acc64)get_scd_expand_v64acc32_incr(en, pos);
}
INTRINSIC(v64accfloat) get_scd_expand_v64accfloat_incr(int en, int &pos) {
  return (v64accfloat)get_scd_expand_v64acc32_incr(en, pos);
}
INTRINSIC(v64acc32) get_scd_expand_v64acc32_incr(int &pos) {
  return get_scd_expand_v64acc32_incr(1, pos);
}
INTRINSIC(v32acc64) get_scd_expand_v32acc64_incr(int &pos) {
  return get_scd_expand_v32acc64_incr(1, pos);
}
INTRINSIC(v64accfloat) get_scd_expand_v64accfloat_incr(int &pos) {
  return get_scd_expand_v64accfloat_incr(1, pos);
}
INTRINSIC(v64acc32) get_scd_expand_v64acc32(int en, int pos) {
  return __builtin_aie2p_scd_expand_ACC2048(en, pos);
}
INTRINSIC(v32acc64) get_scd_expand_v32acc64(int en, int pos) {
  return (v32acc64)get_scd_expand_v64acc32(en, pos);
}
INTRINSIC(v64accfloat) get_scd_expand_v64accfloat(int en, int pos) {
  return (v64accfloat)get_scd_expand_v64acc32(en, pos);
}
INTRINSIC(v64acc32) get_scd_expand_v64acc32(int pos) {
  return get_scd_expand_v64acc32(1, pos);
}
INTRINSIC(v32acc64) get_scd_expand_v32acc64(int pos) {
  return get_scd_expand_v32acc64(1, pos);
}
INTRINSIC(v64accfloat) get_scd_expand_v64accfloat(int pos) {
  return get_scd_expand_v64accfloat(1, pos);
}
INTRINSIC(void) put_mcd(v16acc32 a, int en) {
  __builtin_aie2p_mcd_write_acc32(a, en);
}
INTRINSIC(void) put_mcd(v8acc64 a, int en) {
  __builtin_aie2p_mcd_write_acc32(a, en);
}
INTRINSIC(void) put_mcd(v16accfloat a, int en) { put_mcd((v16acc32)a, en); }
INTRINSIC(void) put_mcd(v128int4 a, int en) {
  __builtin_aie2p_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v128uint4 a, int en) {
  __builtin_aie2p_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v64int8 a, int en) {
  __builtin_aie2p_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v64uint8 a, int en) {
  __builtin_aie2p_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v32int16 a, int en) {
  __builtin_aie2p_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v32uint16 a, int en) {
  __builtin_aie2p_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v16int32 a, int en) {
  __builtin_aie2p_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v16uint32 a, int en) {
  __builtin_aie2p_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v32bfloat16 a, int en) {
  __builtin_aie2p_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v16float a, int en) {
  __builtin_aie2p_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v16acc32 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v8acc64 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v16accfloat a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v128int4 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v128uint4 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v64int8 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v64uint8 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v32int16 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v32uint16 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v16int32 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v16uint32 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v32bfloat16 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v16float a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v32acc32 a, int en) {
  put_mcd(extract_v16acc32(a, 0), en);
  put_mcd(extract_v16acc32(a, 1), en);
}
INTRINSIC(void) put_mcd(v16acc64 a, int en) { put_mcd((v32acc32)a, en); }
INTRINSIC(void) put_mcd(v32int32 a, int en) {
  put_mcd(extract_v16int32(a, 0), en);
  put_mcd(extract_v16int32(a, 1), en);
}
INTRINSIC(void) put_mcd(v256int4 a, int en) { put_mcd((v32int32)a, en); }
INTRINSIC(void) put_mcd(v256uint4 a, int en) { put_mcd((v32int32)a, en); }
INTRINSIC(void) put_mcd(v128int8 a, int en) { put_mcd((v32int32)a, en); }
INTRINSIC(void) put_mcd(v128uint8 a, int en) { put_mcd((v32int32)a, en); }
INTRINSIC(void) put_mcd(v64int16 a, int en) { put_mcd((v32int32)a, en); }
INTRINSIC(void) put_mcd(v64uint16 a, int en) { put_mcd((v32int32)a, en); }
INTRINSIC(void) put_mcd(v32uint32 a, int en) { put_mcd((v32int32)a, en); }
INTRINSIC(void) put_mcd(v32float a, int en) { put_mcd((v32int32)a, en); }
INTRINSIC(void) put_mcd(v32acc32 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v16acc64 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v256int4 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v256uint4 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v128int8 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v128uint8 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v64int16 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v64uint16 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v32int32 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v32uint32 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v32float a) { put_mcd(a, 1); }

#define PUT_MS(TYPE)                                                           \
  INTRINSIC(void) put_ms(TYPE val, int tlast) {                                \
    __builtin_aie2p_put_ms((int)val, tlast);                                   \
  }                                                                            \
  INTRINSIC(void) put_ms(TYPE val) { __builtin_aie2p_put_ms((int)val, 0); }    \
  INTRINSIC(void) put_ms(TYPE val, int en, int tlast) { put_ms(val, tlast); }  \
  INTRINSIC(void) put_ms_nb(TYPE val, int tlast, bool &success) {              \
    int suc;                                                                   \
    __builtin_aie2p_put_ms_nb((int)val, tlast, suc);                           \
    success = suc;                                                             \
  }                                                                            \
  INTRINSIC(void) put_ms_nb(TYPE val, bool &success) {                         \
    int suc;                                                                   \
    __builtin_aie2p_put_ms_nb((int)val, 0, suc);                               \
    success = suc;                                                             \
  }
PUT_MS(int)
PUT_MS(unsigned int);
PUT_MS(float);
PUT_MS(v8int4);
PUT_MS(v8uint4);
PUT_MS(v4int8);
PUT_MS(v4uint8);
PUT_MS(v2int16);
PUT_MS(v2uint16);
PUT_MS(v2bfloat16)
// Note: no non-blocking variants of these.
#define PUT_MS_N(TYPE, TYPECAST, N)                                            \
  INTRINSIC(void) put_ms(TYPE a, int tlast) {                                  \
    v16int32 aa = set_v16int32(0, (TYPECAST)a);                                \
    for (int n = 0; n < N - 1; n++) {                                          \
      put_ms(ext_elem(aa, n), 0);                                              \
    }                                                                          \
    put_ms(ext_elem(aa, N - 1), tlast);                                        \
  }                                                                            \
  INTRINSIC(void) put_ms(TYPE a) {                                             \
    v16int32 aa = set_v16int32(0, (TYPECAST)a);                                \
    for (int n = 0; n < N - 1; n++) {                                          \
      put_ms(ext_elem(aa, n));                                                 \
    }                                                                          \
    put_ms(ext_elem(aa, N - 1));                                               \
  }                                                                            \
  INTRINSIC(void) put_ms(TYPE a, int en, int tlast) { put_ms(a, tlast); }

#define PUT_MS_4(TYPE) PUT_MS_N(TYPE, v4int32, 4)
PUT_MS_4(v4int32);
PUT_MS_4(v4uint32);
PUT_MS_4(v8int16);
PUT_MS_4(v8uint16);
PUT_MS_4(v16int8);
PUT_MS_4(v16uint8);
PUT_MS_4(v32int4);
PUT_MS_4(v32uint4);
PUT_MS_4(v8bfloat16)
PUT_MS_4(v4float)

#define PUT_MS_8(TYPE) PUT_MS_N(TYPE, v8int32, 8)
PUT_MS_8(v8int32);
PUT_MS_8(v8uint32);
PUT_MS_8(v16int16);
PUT_MS_8(v16uint16);
PUT_MS_8(v32int8);
PUT_MS_8(v32uint8);
PUT_MS_8(v64int4);
PUT_MS_8(v64uint4);
PUT_MS_8(v16bfloat16)
PUT_MS_8(v8float)

#define PUT_MS_16(TYPE)                                                        \
  INTRINSIC(void) put_ms(TYPE a, int tlast) {                                  \
    v16int32 aa = a;                                                           \
    for (int n = 0; n < 16 - 1; n++) {                                         \
      put_ms(ext_elem(aa, n), 0);                                              \
    }                                                                          \
    put_ms(ext_elem(aa, 16 - 1), tlast);                                       \
  }                                                                            \
  INTRINSIC(void) put_ms(TYPE a) {                                             \
    v16int32 aa = a;                                                           \
    for (int n = 0; n < 16 - 1; n++) {                                         \
      put_ms(ext_elem(aa, n));                                                 \
    }                                                                          \
    put_ms(ext_elem(aa, 16 - 1));                                              \
  }                                                                            \
  INTRINSIC(void) put_ms(TYPE a, int en, int tlast) { put_ms(a, tlast); }

PUT_MS_16(v16int32);
PUT_MS_16(v16uint32);
PUT_MS_16(v32int16);
PUT_MS_16(v32uint16);
PUT_MS_16(v64int8);
PUT_MS_16(v64uint8);
PUT_MS_16(v128int4);
PUT_MS_16(v128uint4);
PUT_MS_16(v32bfloat16)
PUT_MS_16(v16float)

#define PUT_MS_32(TYPE)                                                        \
  INTRINSIC(void) put_ms(TYPE a, int en, int tlast) {                          \
    put_ms(extract_v16int32(a, 0), en, 0);                                     \
    put_ms(extract_v16int32(a, 1), en, tlast);                                 \
  }

PUT_MS_32(v32int32);
PUT_MS_32(v32uint32);
PUT_MS_32(v64int16);
PUT_MS_32(v64uint16);
PUT_MS_32(v128int8);
PUT_MS_32(v128uint8);
PUT_MS_32(v256int4);
PUT_MS_32(v256uint4);
PUT_MS_32(v64bfloat16)
PUT_MS_32(v32float)

#define GET_SS(T, PREFIX, SUFFIX)                                              \
  INTRINSIC(T) PREFIX##_ss##SUFFIX() {                                         \
    int status;                                                                \
    return (T)__builtin_aie2p_get_ss(status);                                  \
  }                                                                            \
  INTRINSIC(T) PREFIX##_ss##SUFFIX(bool &tlast) {                              \
    int status;                                                                \
    T val = (T)__builtin_aie2p_get_ss(status);                                 \
    tlast = status & 1;                                                        \
    return val;                                                                \
  }                                                                            \
  INTRINSIC(T) PREFIX##_ss_nb##SUFFIX(bool &success) {                         \
    int status;                                                                \
    T val = (T)__builtin_aie2p_get_ss_nb(status);                              \
    success = status & 2;                                                      \
    return val;                                                                \
  }                                                                            \
  INTRINSIC(T) PREFIX##_ss_nb##SUFFIX(bool &success, bool &tlast) {            \
    int status;                                                                \
    T val = (T)__builtin_aie2p_get_ss_nb(status);                              \
    tlast = status & 1;                                                        \
    success = status & 2;                                                      \
    return val;                                                                \
  }
GET_SS(int, get, )
GET_SS(float, getf, )
GET_SS(int, get, _int)
GET_SS(unsigned int, get, _uint)
GET_SS(float, get, _float)
GET_SS(v8int4, get, _v8int4)
GET_SS(v8uint4, get, _v8uint4)
GET_SS(v4int8, get, _v4int8)
GET_SS(v4uint8, get, _v4uint8)
GET_SS(v2int16, get, _v2int16)
GET_SS(v2uint16, get, _v2uint16)
GET_SS(v2bfloat16, get, _v2bfloat16)
#define GET_SS_N(TYPE, N)                                                      \
  INTRINSIC(TYPE) get_ss_##TYPE() {                                            \
    v16int32 r = undef_v16int32();                                             \
    for (int n = 0; n < N - 1; n++) {                                          \
      r = upd_elem(r, n, get_ss_int());                                        \
    }                                                                          \
    r = upd_elem(r, N - 1, get_ss_int());                                      \
    return extract_##TYPE(r, 0);                                               \
  }

#define GET_SS_4(TYPE) GET_SS_N(TYPE, 4)
GET_SS_4(v4int32);
GET_SS_4(v4uint32);
GET_SS_4(v8int16);
GET_SS_4(v8uint16);
GET_SS_4(v16int8);
GET_SS_4(v16uint8);
GET_SS_4(v32int4);
GET_SS_4(v32uint4);
GET_SS_4(v8bfloat16)
GET_SS_4(v4float)

#define GET_SS_8(TYPE) GET_SS_N(TYPE, 8)
GET_SS_8(v8int32);
GET_SS_8(v8uint32);
GET_SS_8(v16int16);
GET_SS_8(v16uint16);
GET_SS_8(v32int8);
GET_SS_8(v32uint8);
GET_SS_8(v64int4);
GET_SS_8(v64uint4);
GET_SS_8(v16bfloat16)
GET_SS_8(v8float)

#define GET_SS_16(TYPE)                                                        \
  INTRINSIC(TYPE) get_ss_##TYPE() {                                            \
    v16int32 r = undef_v16int32();                                             \
    for (int n = 0; n < 16 - 1; n++) {                                         \
      r = upd_elem(r, n, get_ss_int());                                        \
    }                                                                          \
    r = upd_elem(r, 16 - 1, get_ss_int());                                     \
    return r;                                                                  \
  }
GET_SS_16(v16int32);
GET_SS_16(v16uint32);
GET_SS_16(v32int16);
GET_SS_16(v32uint16);
GET_SS_16(v64int8);
GET_SS_16(v64uint8);
GET_SS_16(v128int4);
GET_SS_16(v128uint4);
GET_SS_16(v32bfloat16)
GET_SS_16(v16float)

#define GET_SS_32(TYPE)                                                        \
  INTRINSIC(TYPE) get_ss_##TYPE() {                                            \
    v32int32 r;                                                                \
    r = set_v32int32(0, get_ss_v16int32());                                    \
    r = insert(r, 1, get_ss_v16int32());                                       \
    return r;                                                                  \
  }
GET_SS_32(v32int32);
GET_SS_32(v32uint32);
GET_SS_32(v64int16);
GET_SS_32(v64uint16);
GET_SS_32(v128int8);
GET_SS_32(v128uint8);
GET_SS_32(v256int4);
GET_SS_32(v256uint4);
GET_SS_32(v64bfloat16)
GET_SS_32(v32float)

#endif // __AIE2P_STREAMS_H__
