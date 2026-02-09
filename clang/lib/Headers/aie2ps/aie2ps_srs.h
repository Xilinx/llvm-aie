//===-------------------- AIEngine AIE2ps intrinsics -----------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef AIE2PS_SRS_H
#define AIE2PS_SRS_H

//* Automatically generated file, do not edit! *
//

typedef uint2_t crsat_t;

typedef uint4_t crrnd_t;

INTRINSIC(v32int8) ssrs(v32acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v32_acc32_srs(acc, shft, sign);
}
INTRINSIC(v32int8)
ssrs_conf(v32acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int8 r = ssrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint8) ussrs(v32acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v32_acc32_srs(acc, shft, sign);
}
INTRINSIC(v32uint8)
ussrs_conf(v32acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint8 r = ussrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v64int8) ssrs(v64acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v64_acc32_srs(acc, shft, sign);
}
INTRINSIC(v64int8)
ssrs_conf(v64acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v64int8 r = ssrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v64uint8) ussrs(v64acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v64_acc32_srs(acc, shft, sign);
}
INTRINSIC(v64uint8)
ussrs_conf(v64acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v64uint8 r = ussrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v32int16) lsrs(v32acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v32_acc32_srs(acc, shft, sign);
}
INTRINSIC(v32int16)
lsrs_conf(v32acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 r = lsrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint16) ulsrs(v32acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v32_acc32_srs(acc, shft, sign);
}
INTRINSIC(v32uint16)
ulsrs_conf(v32acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 r = ulsrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16int16) lsrs(v16acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v16_acc32_srs(acc, shft, sign);
}
INTRINSIC(v16int16)
lsrs_conf(v16acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 r = lsrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint16) ulsrs(v16acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v16_acc32_srs(acc, shft, sign);
}
INTRINSIC(v16uint16)
ulsrs_conf(v16acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 r = ulsrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v16int16) ssrs(v16acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v16_acc64_srs(acc, shft, sign);
}
INTRINSIC(v16int16)
ssrs_conf(v16acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 r = ssrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint16) ussrs(v16acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v16_acc64_srs(acc, shft, sign);
}
INTRINSIC(v16uint16)
ussrs_conf(v16acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 r = ussrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32int16) ssrs(v32acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v32_acc64_srs(acc, shft, sign);
}
INTRINSIC(v32int16)
ssrs_conf(v32acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 r = ssrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint16) ussrs(v32acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v32_acc64_srs(acc, shft, sign);
}
INTRINSIC(v32uint16)
ussrs_conf(v32acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 r = ussrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16int32) lsrs(v16acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v16_acc64_srs(acc, shft, sign);
}
INTRINSIC(v16int32)
lsrs_conf(v16acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int32 r = lsrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint32) ulsrs(v16acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v16_acc64_srs(acc, shft, sign);
}
INTRINSIC(v16uint32)
ulsrs_conf(v16acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint32 r = ulsrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v8int32) lsrs(v8acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v8_acc64_srs(acc, shft, sign);
}
INTRINSIC(v8int32)
lsrs_conf(v8acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8int32 r = lsrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v8uint32) ulsrs(v8acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v8_acc64_srs(acc, shft, sign);
}
INTRINSIC(v8uint32)
ulsrs_conf(v8acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8uint32 r = ulsrs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32int8) ssrs(v32acc32 acc, int shft) {
  return __builtin_aie2ps_I256_v32_acc32_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v32int8) ssrs_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int8 r = ssrs(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint8) ussrs(v32acc32 acc, int shft) {
  return __builtin_aie2ps_I256_v32_acc32_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32uint8)
ussrs_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint8 r = ussrs(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v64int8) ssrs(v64acc32 acc, int shft) {
  return __builtin_aie2ps_I512_v64_acc32_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v64int8) ssrs_conf(v64acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v64int8 r = ssrs(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v64uint8) ussrs(v64acc32 acc, int shft) {
  return __builtin_aie2ps_I512_v64_acc32_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v64uint8)
ussrs_conf(v64acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v64uint8 r = ussrs(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v32int16) lsrs(v32acc32 acc, int shft) {
  return __builtin_aie2ps_I512_v32_acc32_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v32int16)
lsrs_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 r = lsrs(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint16) ulsrs(v32acc32 acc, int shft) {
  return __builtin_aie2ps_I512_v32_acc32_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32uint16)
ulsrs_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 r = ulsrs(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16int16) lsrs(v16acc32 acc, int shft) {
  return __builtin_aie2ps_I256_v16_acc32_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v16int16)
lsrs_conf(v16acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 r = lsrs(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint16) ulsrs(v16acc32 acc, int shft) {
  return __builtin_aie2ps_I256_v16_acc32_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16uint16)
ulsrs_conf(v16acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 r = ulsrs(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v16int16) ssrs(v16acc64 acc, int shft) {
  return __builtin_aie2ps_I256_v16_acc64_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v16int16)
ssrs_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 r = ssrs(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint16) ussrs(v16acc64 acc, int shft) {
  return __builtin_aie2ps_I256_v16_acc64_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16uint16)
ussrs_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 r = ussrs(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32int16) ssrs(v32acc64 acc, int shft) {
  return __builtin_aie2ps_I512_v32_acc64_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v32int16)
ssrs_conf(v32acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 r = ssrs(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint16) ussrs(v32acc64 acc, int shft) {
  return __builtin_aie2ps_I512_v32_acc64_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32uint16)
ussrs_conf(v32acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 r = ussrs(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16int32) lsrs(v16acc64 acc, int shft) {
  return __builtin_aie2ps_I512_v16_acc64_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v16int32)
lsrs_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int32 r = lsrs(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint32) ulsrs(v16acc64 acc, int shft) {
  return __builtin_aie2ps_I512_v16_acc64_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16uint32)
ulsrs_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint32 r = ulsrs(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v8int32) lsrs(v8acc64 acc, int shft) {
  return __builtin_aie2ps_I256_v8_acc64_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v8int32) lsrs_conf(v8acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8int32 r = lsrs(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v8uint32) ulsrs(v8acc64 acc, int shft) {
  return __builtin_aie2ps_I256_v8_acc64_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v8uint32)
ulsrs_conf(v8acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8uint32 r = ulsrs(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32int8) to_v32int8(v32acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v32_acc32_srs(acc, shft, sign);
}
INTRINSIC(v32int8)
to_v32int8_conf(v32acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int8 r = to_v32int8(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint8) to_v32uint8(v32acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v32_acc32_srs(acc, shft, sign);
}
INTRINSIC(v32uint8)
to_v32uint8_conf(v32acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint8 r = to_v32uint8(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v64int8) to_v64int8(v64acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v64_acc32_srs(acc, shft, sign);
}
INTRINSIC(v64int8)
to_v64int8_conf(v64acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v64int8 r = to_v64int8(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v64uint8) to_v64uint8(v64acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v64_acc32_srs(acc, shft, sign);
}
INTRINSIC(v64uint8)
to_v64uint8_conf(v64acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v64uint8 r = to_v64uint8(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
[[deprecated("Function 'srs_to_v32int8 ()' is deprecated. Please use "
             "'to_v32int8 ()' instead.")]] INTRINSIC(v32int8)
    srs_to_v32int8(v32acc32 acc, int shft, int sign) {
  return to_v32int8(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v32int8_conf ()' is deprecated. Please use "
             "'to_v32int8_conf ()' instead.")]] INTRINSIC(v32int8)
    srs_to_v32int8_conf(v32acc32 acc, int shft, int sign, crsat_t sat,
                        crrnd_t rnd) {
  return to_v32int8_conf(acc, shft, sign, sat, rnd);
}
[[deprecated("Function 'srs_to_v32Uint8 ()' is deprecated. Please use "
             "'to_v32uint8 ()' instead.")]] INTRINSIC(v32uint8)
    srs_to_v32uint8(v32acc32 acc, int shft, int sign) {
  return to_v32uint8(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v32uint8_conf ()' is deprecated. Please use "
             "'to_v32uint8_conf ()' instead.")]] INTRINSIC(v32uint8)
    srs_to_v32uint8_conf(v32acc32 acc, int shft, int sign, crsat_t sat,
                         crrnd_t rnd) {
  return to_v32uint8_conf(acc, shft, sign, sat, rnd);
}
[[deprecated("Function 'srs_to_v64int8 ()' is deprecated. Please use "
             "'to_v64int8 ()' instead.")]] INTRINSIC(v64int8)
    srs_to_v64int8(v64acc32 acc, int shft, int sign) {
  return to_v64int8(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v64int8_conf ()' is deprecated. Please use "
             "'to_v64int8_conf ()' instead.")]] INTRINSIC(v64int8)
    srs_to_v64int8_conf(v64acc32 acc, int shft, int sign, crsat_t sat,
                        crrnd_t rnd) {
  return to_v64int8_conf(acc, shft, sign, sat, rnd);
}
[[deprecated("Function 'srs_to_v64uint8 ()' is deprecated. Please use "
             "'to_v64uint8 ()' instead.")]] INTRINSIC(v64uint8)
    srs_to_v64uint8(v64acc32 acc, int shft, int sign) {
  return to_v64uint8(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v64uint8_conf ()' is deprecated. Please use "
             "'to_v64uint8_conf ()' instead.")]] INTRINSIC(v64uint8)
    srs_to_v64uint8_conf(v64acc32 acc, int shft, int sign, crsat_t sat,
                         crrnd_t rnd) {
  return to_v64uint8_conf(acc, shft, sign, sat, rnd);
}

INTRINSIC(v32int16) to_v32int16(v32acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v32_acc32_srs(acc, shft, sign);
}
INTRINSIC(v32int16)
to_v32int16_conf(v32acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 r = to_v32int16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint16) to_v32uint16(v32acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v32_acc32_srs(acc, shft, sign);
}
INTRINSIC(v32uint16)
to_v32uint16_conf(v32acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 r = to_v32uint16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16int16) to_v16int16(v16acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v16_acc32_srs(acc, shft, sign);
}
INTRINSIC(v16int16)
to_v16int16_conf(v16acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 r = to_v16int16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint16) to_v16uint16(v16acc32 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v16_acc32_srs(acc, shft, sign);
}
INTRINSIC(v16uint16)
to_v16uint16_conf(v16acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 r = to_v16uint16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
[[deprecated("Function 'srs_to_v32int16 ()' is deprecated. Please use "
             "'to_v32int16 ()' instead.")]] INTRINSIC(v32int16)
    srs_to_v32int16(v32acc32 acc, int shft, int sign) {
  return to_v32int16(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v32int16_conf ()' is deprecated. Please use "
             "'to_v32int16_conf ()' instead.")]] INTRINSIC(v32int16)
    srs_to_v32int16_conf(v32acc32 acc, int shft, int sign, crsat_t sat,
                         crrnd_t rnd) {
  return to_v32int16_conf(acc, shft, sign, sat, rnd);
}
[[deprecated("Function 'srs_to_v32uint16 ()' is deprecated. Please use "
             "'to_v32uint16 ()' instead.")]] INTRINSIC(v32uint16)
    srs_to_v32uint16(v32acc32 acc, int shft, int sign) {
  return to_v32uint16(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v32uint16_conf ()' is deprecated. Please use "
             "'to_v32uint16_conf ()' instead.")]] INTRINSIC(v32uint16)
    srs_to_v32uint16_conf(v32acc32 acc, int shft, int sign, crsat_t sat,
                          crrnd_t rnd) {
  return to_v32uint16_conf(acc, shft, sign, sat, rnd);
}
[[deprecated("Function 'srs_to_v16int16 ()' is deprecated. Please use "
             "'to_v16int16 ()' instead.")]] INTRINSIC(v16int16)
    srs_to_v16int16(v16acc32 acc, int shft, int sign) {
  return to_v16int16(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v16int16_conf ()' is deprecated. Please use "
             "'to_v16int16_conf ()' instead.")]] INTRINSIC(v16int16)
    srs_to_v16int16_conf(v16acc32 acc, int shft, int sign, crsat_t sat,
                         crrnd_t rnd) {
  return to_v16int16_conf(acc, shft, sign, sat, rnd);
}
[[deprecated("Function 'srs_to_v16uint16 ()' is deprecated. Please use "
             "'to_v16uint16 ()' instead.")]] INTRINSIC(v16uint16)
    srs_to_v16uint16(v16acc32 acc, int shft, int sign) {
  return to_v16uint16(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v16uint16_conf ()' is deprecated. Please use "
             "'to_v16uint16_conf ()' instead.")]] INTRINSIC(v16uint16)
    srs_to_v16uint16_conf(v16acc32 acc, int shft, int sign, crsat_t sat,
                          crrnd_t rnd) {
  return to_v16uint16_conf(acc, shft, sign, sat, rnd);
}

INTRINSIC(v16int16) to_v16int16(v16acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v16_acc64_srs(acc, shft, sign);
}
INTRINSIC(v16int16)
to_v16int16_conf(v16acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 r = to_v16int16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint16) to_v16uint16(v16acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v16_acc64_srs(acc, shft, sign);
}
INTRINSIC(v16uint16)
to_v16uint16_conf(v16acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 r = to_v16uint16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32int16) to_v32int16(v32acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v32_acc64_srs(acc, shft, sign);
}
INTRINSIC(v32int16)
to_v32int16_conf(v32acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 r = to_v32int16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint16) to_v32uint16(v32acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v32_acc64_srs(acc, shft, sign);
}
INTRINSIC(v32uint16)
to_v32uint16_conf(v32acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 r = to_v32uint16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
[[deprecated("Function 'srs_to_v16int16 ()' is deprecated. Please use "
             "'to_v16int16 ()' instead.")]] INTRINSIC(v16int16)
    srs_to_v16int16(v16acc64 acc, int shft, int sign) {
  return to_v16int16(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v16int16_conf ()' is deprecated. Please use "
             "'to_v16int16_conf ()' instead.")]] INTRINSIC(v16int16)
    srs_to_v16int16_conf(v16acc64 acc, int shft, int sign, crsat_t sat,
                         crrnd_t rnd) {
  return to_v16int16_conf(acc, shft, sign, sat, rnd);
}
[[deprecated("Function 'srs_to_v16uint16 ()' is deprecated. Please use "
             "'to_v16uint16 ()' instead.")]] INTRINSIC(v16uint16)
    srs_to_v16uint16(v16acc64 acc, int shft, int sign) {
  return to_v16uint16(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v16uint16_conf ()' is deprecated. Please use "
             "'to_v16uint16_conf ()' instead.")]] INTRINSIC(v16uint16)
    srs_to_v16uint16_conf(v16acc64 acc, int shft, int sign, crsat_t sat,
                          crrnd_t rnd) {
  return to_v16uint16_conf(acc, shft, sign, sat, rnd);
}
[[deprecated("Function 'srs_to_v32int16 ()' is deprecated. Please use "
             "'to_v32int16 ()' instead.")]] INTRINSIC(v32int16)
    srs_to_v32int16(v32acc64 acc, int shft, int sign) {
  return to_v32int16(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v32int16_conf ()' is deprecated. Please use "
             "'to_v32int16_conf ()' instead.")]] INTRINSIC(v32int16)
    srs_to_v32int16_conf(v32acc64 acc, int shft, int sign, crsat_t sat,
                         crrnd_t rnd) {
  return to_v32int16_conf(acc, shft, sign, sat, rnd);
}
[[deprecated("Function 'srs_to_v32uint16 ()' is deprecated. Please use "
             "'to_v32uint16 ()' instead.")]] INTRINSIC(v32uint16)
    srs_to_v32uint16(v32acc64 acc, int shft, int sign) {
  return to_v32uint16(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v32uint16_conf ()' is deprecated. Please use "
             "'to_v32uint16_conf ()' instead.")]] INTRINSIC(v32uint16)
    srs_to_v32uint16_conf(v32acc64 acc, int shft, int sign, crsat_t sat,
                          crrnd_t rnd) {
  return to_v32uint16_conf(acc, shft, sign, sat, rnd);
}
INTRINSIC(v16int32) to_v16int32(v16acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v16_acc64_srs(acc, shft, sign);
}
INTRINSIC(v16int32)
to_v16int32_conf(v16acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int32 r = to_v16int32(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint32) to_v16uint32(v16acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I512_v16_acc64_srs(acc, shft, sign);
}
INTRINSIC(v16uint32)
to_v16uint32_conf(v16acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint32 r = to_v16uint32(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v8int32) to_v8int32(v8acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v8_acc64_srs(acc, shft, sign);
}
INTRINSIC(v8int32)
to_v8int32_conf(v8acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8int32 r = to_v8int32(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v8uint32) to_v8uint32(v8acc64 acc, int shft, int sign) {
  return __builtin_aie2ps_I256_v8_acc64_srs(acc, shft, sign);
}
INTRINSIC(v8uint32)
to_v8uint32_conf(v8acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8uint32 r = to_v8uint32(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

[[deprecated("Function 'srs_to_v16int32 ()' is deprecated. Please use "
             "'to_v16int32 ()' instead.")]] INTRINSIC(v16int32)
    srs_to_v16int32(v16acc64 acc, int shft, int sign) {
  return to_v16int32(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v16int32_conf ()' is deprecated. Please use "
             "'to_v16int32_conf ()' instead.")]] INTRINSIC(v16int32)
    srs_to_v16int32_conf(v16acc64 acc, int shft, int sign, crsat_t sat,
                         crrnd_t rnd) {
  return to_v16int32_conf(acc, shft, sign, sat, rnd);
}
[[deprecated("Function 'srs_to_v16uint32 ()' is deprecated. Please use "
             "'to_v16uint32 ()' instead.")]] INTRINSIC(v16uint32)
    srs_to_v16uint32(v16acc64 acc, int shft, int sign) {
  return to_v16uint32(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v16uint32_conf ()' is deprecated. Please use "
             "'to_v16uint32_conf ()' instead.")]] INTRINSIC(v16uint32)
    srs_to_v16uint32_conf(v16acc64 acc, int shft, int sign, crsat_t sat,
                          crrnd_t rnd) {
  return to_v16uint32_conf(acc, shft, sign, sat, rnd);
}
[[deprecated("Function 'srs_to_v8int32 ()' is deprecated. Please use "
             "'to_v8int32 ()' instead.")]] INTRINSIC(v8int32)
    srs_to_v8int32(v8acc64 acc, int shft, int sign) {
  return to_v8int32(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v8int32_conf ()' is deprecated. Please use "
             "'to_v8int32_conf ()' instead.")]] INTRINSIC(v8int32)
    srs_to_v8int32_conf(v8acc64 acc, int shft, int sign, crsat_t sat,
                        crrnd_t rnd) {
  return to_v8int32_conf(acc, shft, sign, sat, rnd);
}
[[deprecated("Function 'srs_to_v8uint32 ()' is deprecated. Please use "
             "'to_v8uint32 ()' instead.")]] INTRINSIC(v8uint32)
    srs_to_v8uint32(v8acc64 acc, int shft, int sign) {
  return to_v8uint32(acc, shft, sign);
}
[[deprecated("Function 'srs_to_v8uint32_conf ()' is deprecated. Please use "
             "'to_v8uint32_conf ()' instead.")]] INTRINSIC(v8uint32)
    srs_to_v8uint32_conf(v8acc64 acc, int shft, int sign, crsat_t sat,
                         crrnd_t rnd) {
  return to_v8uint32_conf(acc, shft, sign, sat, rnd);
}
INTRINSIC(v32int8) to_v32int8(v32acc32 acc, int shft) {
  return __builtin_aie2ps_I256_v32_acc32_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v32int8)
to_v32int8_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int8 r = to_v32int8(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint8) to_v32uint8(v32acc32 acc, int shft) {
  return __builtin_aie2ps_I256_v32_acc32_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32uint8)
to_v32uint8_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint8 r = to_v32uint8(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v64int8) to_v64int8(v64acc32 acc, int shft) {
  return __builtin_aie2ps_I512_v64_acc32_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v64int8)
to_v64int8_conf(v64acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v64int8 r = to_v64int8(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v64uint8) to_v64uint8(v64acc32 acc, int shft) {
  return __builtin_aie2ps_I512_v64_acc32_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v64uint8)
to_v64uint8_conf(v64acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v64uint8 r = to_v64uint8(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

[[deprecated("Function 'srs_to_v32int8 ()' is deprecated. Please use "
             "'to_v32int8 ()' instead.")]] INTRINSIC(v32int8)
    srs_to_v32int8(v32acc32 acc, int shft) {
  return to_v32int8(acc, shft);
}
[[deprecated("Function 'srs_to_v32int8_conf ()' is deprecated. Please use "
             "'to_v32int8_conf ()' instead.")]] INTRINSIC(v32int8)
    srs_to_v32int8_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v32int8_conf(acc, shft, sat, rnd);
}
[[deprecated("Function 'srs_to_v32uint8 ()' is deprecated. Please use "
             "'to_v32uint8 ()' instead.")]] INTRINSIC(v32uint8)
    srs_to_v32uint8(v32acc32 acc, int shft) {
  return to_v32uint8(acc, shft);
}
[[deprecated("Function 'srs_to_v32uint8_conf ()' is deprecated. Please use "
             "'to_v32uint8_conf ()' instead.")]] INTRINSIC(v32uint8)
    srs_to_v32uint8_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v32uint8_conf(acc, shft, sat, rnd);
}
[[deprecated("Function 'srs_to_v64int8 ()' is deprecated. Please use "
             "'to_v64int8 ()' instead.")]] INTRINSIC(v64int8)
    srs_to_v64int8(v64acc32 acc, int shft) {
  return to_v64int8(acc, shft);
}
[[deprecated("Function 'srs_to_v64int8_conf ()' is deprecated. Please use "
             "'to_v64int8_conf ()' instead.")]] INTRINSIC(v64int8)
    srs_to_v64int8_conf(v64acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v64int8_conf(acc, shft, sat, rnd);
}
[[deprecated("Function 'srs_to_v64uint8 ()' is deprecated. Please use "
             "'to_v64uint8 ()' instead.")]] INTRINSIC(v64uint8)
    srs_to_v64uint8(v64acc32 acc, int shft) {
  return to_v64uint8(acc, shft);
}
[[deprecated("Function 'srs_to_v64uint8_conf ()' is deprecated. Please use "
             "'to_v64uint8_conf ()' instead.")]] INTRINSIC(v64uint8)
    srs_to_v64uint8_conf(v64acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v64uint8_conf(acc, shft, sat, rnd);
}

INTRINSIC(v32int16) to_v32int16(v32acc32 acc, int shft) {
  return __builtin_aie2ps_I512_v32_acc32_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v32int16)
to_v32int16_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 r = to_v32int16(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint16) to_v32uint16(v32acc32 acc, int shft) {
  return __builtin_aie2ps_I512_v32_acc32_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32uint16)
to_v32uint16_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 r = to_v32uint16(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16int16) to_v16int16(v16acc32 acc, int shft) {
  return __builtin_aie2ps_I256_v16_acc32_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v16int16)
to_v16int16_conf(v16acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 r = to_v16int16(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint16) to_v16uint16(v16acc32 acc, int shft) {
  return __builtin_aie2ps_I256_v16_acc32_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16uint16)
to_v16uint16_conf(v16acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 r = to_v16uint16(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

[[deprecated("Function 'srs_to_v32int16 ()' is deprecated. Please use "
             "'to_v32int16 ()' instead.")]] INTRINSIC(v32int16)
    srs_to_v32int16(v32acc32 acc, int shft) {
  return to_v32int16(acc, shft);
}
[[deprecated("Function 'srs_to_v32int16_conf ()' is deprecated. Please use "
             "'to_v32int16_conf ()' instead.")]] INTRINSIC(v32int16)
    srs_to_v32int16_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v32int16_conf(acc, shft, sat, rnd);
}
[[deprecated("Function 'srs_to_v32uint16 ()' is deprecated. Please use "
             "'to_v32uint16 ()' instead.")]] INTRINSIC(v32uint16)
    srs_to_v32uint16(v32acc32 acc, int shft) {
  return to_v32uint16(acc, shft);
}
[[deprecated("Function 'srs_to_v32uint16_conf ()' is deprecated. Please use "
             "'to_v32uint16_conf ()' instead.")]] INTRINSIC(v32uint16)
    srs_to_v32uint16_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v32uint16_conf(acc, shft, sat, rnd);
}
[[deprecated("Function 'srs_to_v16int16 ()' is deprecated. Please use "
             "'to_v16int16 ()' instead.")]] INTRINSIC(v16int16)
    srs_to_v16int16(v16acc32 acc, int shft) {
  return to_v16int16(acc, shft);
}
[[deprecated("Function 'srs_to_v16int16_conf ()' is deprecated. Please use "
             "'to_v16int16_conf ()' instead.")]] INTRINSIC(v16int16)
    srs_to_v16int16_conf(v16acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v16int16_conf(acc, shft, sat, rnd);
}
[[deprecated("Function 'srs_to_v16uint16 ()' is deprecated. Please use "
             "'to_v16uint16 ()' instead.")]] INTRINSIC(v16uint16)
    srs_to_v16uint16(v16acc32 acc, int shft) {
  return to_v16uint16(acc, shft);
}
[[deprecated("Function 'srs_to_v16uint16_conf ()' is deprecated. Please use "
             "'to_v16uint16_conf ()' instead.")]] INTRINSIC(v16uint16)
    srs_to_v16uint16_conf(v16acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v16uint16_conf(acc, shft, sat, rnd);
}

INTRINSIC(v16int16) to_v16int16(v16acc64 acc, int shft) {
  return __builtin_aie2ps_I256_v16_acc64_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v16int16)
to_v16int16_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 r = to_v16int16(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint16) to_v16uint16(v16acc64 acc, int shft) {
  return __builtin_aie2ps_I256_v16_acc64_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16uint16)
to_v16uint16_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 r = to_v16uint16(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32int16) to_v32int16(v32acc64 acc, int shft) {
  return __builtin_aie2ps_I512_v32_acc64_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v32int16)
to_v32int16_conf(v32acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 r = to_v32int16(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint16) to_v32uint16(v32acc64 acc, int shft) {
  return __builtin_aie2ps_I512_v32_acc64_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32uint16)
to_v32uint16_conf(v32acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 r = to_v32uint16(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

[[deprecated("Function 'srs_to_v16int16 ()' is deprecated. Please use "
             "'to_v16int16 ()' instead.")]] INTRINSIC(v16int16)
    srs_to_v16int16(v16acc64 acc, int shft) {
  return to_v16int16(acc, shft);
}
[[deprecated("Function 'srs_to_v16int16_conf ()' is deprecated. Please use "
             "'to_v16int16_conf ()' instead.")]] INTRINSIC(v16int16)
    srs_to_v16int16_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v16int16_conf(acc, shft, sat, rnd);
}
[[deprecated("Function 'srs_to_v16uint16 ()' is deprecated. Please use "
             "'to_v16uint16 ()' instead.")]] INTRINSIC(v16uint16)
    srs_to_v16uint16(v16acc64 acc, int shft) {
  return to_v16uint16(acc, shft);
}
[[deprecated("Function 'srs_to_v16uint16_conf ()' is deprecated. Please use "
             "'to_v16uint16_conf ()' instead.")]] INTRINSIC(v16uint16)
    srs_to_v16uint16_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v16uint16_conf(acc, shft, sat, rnd);
}
[[deprecated("Function 'srs_to_v32int16 ()' is deprecated. Please use "
             "'to_v32int16 ()' instead.")]] INTRINSIC(v32int16)
    srs_to_v32int16(v32acc64 acc, int shft) {
  return to_v32int16(acc, shft);
}
[[deprecated("Function 'srs_to_v32int16_conf ()' is deprecated. Please use "
             "'to_v32int16_conf ()' instead.")]] INTRINSIC(v32int16)
    srs_to_v32int16_conf(v32acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v32int16_conf(acc, shft, sat, rnd);
}
[[deprecated("Function 'srs_to_v32uint16 ()' is deprecated. Please use "
             "'to_v32uint16 ()' instead.")]] INTRINSIC(v32uint16)
    srs_to_v32uint16(v32acc64 acc, int shft) {
  return to_v32uint16(acc, shft);
}
[[deprecated("Function 'srs_to_v32uint16_conf ()' is deprecated. Please use "
             "'to_v32uint16_conf ()' instead.")]] INTRINSIC(v32uint16)
    srs_to_v32uint16_conf(v32acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v32uint16_conf(acc, shft, sat, rnd);
}
INTRINSIC(v16int32) to_v16int32(v16acc64 acc, int shft) {
  return __builtin_aie2ps_I512_v16_acc64_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v16int32)
to_v16int32_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int32 r = to_v16int32(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint32) to_v16uint32(v16acc64 acc, int shft) {
  return __builtin_aie2ps_I512_v16_acc64_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16uint32)
to_v16uint32_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint32 r = to_v16uint32(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v8int32) to_v8int32(v8acc64 acc, int shft) {
  return __builtin_aie2ps_I256_v8_acc64_srs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v8int32)
to_v8int32_conf(v8acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8int32 r = to_v8int32(acc, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v8uint32) to_v8uint32(v8acc64 acc, int shft) {
  return __builtin_aie2ps_I256_v8_acc64_srs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v8uint32)
to_v8uint32_conf(v8acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8uint32 r = to_v8uint32(acc, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

[[deprecated("Function 'srs_to_v16int32 ()' is deprecated. Please use "
             "'to_v16int32 ()' instead.")]] INTRINSIC(v16int32)
    srs_to_v16int32(v16acc64 acc, int shft) {
  return to_v16int32(acc, shft);
}
[[deprecated("Function 'srs_to_v16int32_conf ()' is deprecated. Please use "
             "'to_v16int32_conf ()' instead.")]] INTRINSIC(v16int32)
    srs_to_v16int32_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v16int32_conf(acc, shft, sat, rnd);
}
[[deprecated("Function 'srs_to_v16uint32 ()' is deprecated. Please use "
             "'to_v16uint32 ()' instead.")]] INTRINSIC(v16uint32)
    srs_to_v16uint32(v16acc64 acc, int shft) {
  return to_v16uint32(acc, shft);
}
[[deprecated("Function 'srs_to_v16uint32_conf ()' is deprecated. Please use "
             "'to_v16uint32_conf ()' instead.")]] INTRINSIC(v16uint32)
    srs_to_v16uint32_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v16uint32_conf(acc, shft, sat, rnd);
}
[[deprecated("Function 'srs_to_v8int32 ()' is deprecated. Please use "
             "'to_v8int32 ()' instead.")]] INTRINSIC(v8int32)
    srs_to_v8int32(v8acc64 acc, int shft) {
  return to_v8int32(acc, shft);
}
[[deprecated("Function 'srs_to_v8int32_conf ()' is deprecated. Please use "
             "'to_v8int32_conf ()' instead.")]] INTRINSIC(v8int32)
    srs_to_v8int32_conf(v8acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v8int32_conf(acc, shft, sat, rnd);
}
[[deprecated("Function 'srs_to_v8uint32 ()' is deprecated. Please use "
             "'to_v8uint32 ()' instead.")]] INTRINSIC(v8uint32)
    srs_to_v8uint32(v8acc64 acc, int shft) {
  return to_v8uint32(acc, shft);
}
[[deprecated("Function 'srs_to_v8uint32_conf ()' is deprecated. Please use "
             "'to_v8uint32_conf ()' instead.")]] INTRINSIC(v8uint32)
    srs_to_v8uint32_conf(v8acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return to_v8uint32_conf(acc, shft, sat, rnd);
}
INTRINSIC(v16bfloat16) to_v16bfloat16(v16accfloat a) {
  return __builtin_aie2ps_v16accfloat_to_v16bf16(a);
}
INTRINSIC(v32bfloat16) to_v32bfloat16(v32accfloat a) {
  return __builtin_aie2ps_v32accfloat_to_v32bf16(a);
}
INTRINSIC(v16bfloat16) to_v16bfloat16_conf(v16accfloat a, crrnd_t rnd) {
  crrnd_t prev_rnd = get_rnd();

  set_rnd(rnd);

  v16bfloat16 r = to_v16bfloat16(a);

  set_rnd(prev_rnd);

  return r;
}
INTRINSIC(v32bfloat16) to_v32bfloat16_conf(v32accfloat a, crrnd_t rnd) {
  crrnd_t prev_rnd = get_rnd();

  set_rnd(rnd);

  v32bfloat16 r = to_v32bfloat16(a);

  set_rnd(prev_rnd);

  return r;
}
INTRINSIC(v16float16) to_v16float16(v16accfloat a) {
  return __builtin_aie2ps_v16accfloat_to_v16float16(a);
}
INTRINSIC(v32float16) to_v32float16(v32accfloat a) {
  return __builtin_aie2ps_v32accfloat_to_v32float16(a);
}
INTRINSIC(v16float16) to_v16float16_conf(v16accfloat a, crrnd_t rnd) {
  crrnd_t prev_rnd = get_rnd();

  set_rnd(rnd);

  v16float16 r = to_v16float16(a);

  set_rnd(prev_rnd);

  return r;
}
INTRINSIC(v32float16) to_v32float16_conf(v32accfloat a, crrnd_t rnd) {
  crrnd_t prev_rnd = get_rnd();

  set_rnd(rnd);

  v32float16 r = to_v32float16(a);

  set_rnd(prev_rnd);

  return r;
}

INTRINSIC(v32bfloat8) to_v32bfloat8(v32accfloat a, uint6_t shft) {
  return {__builtin_aie2ps_v32accfloat_to_v32bfloat8(a, shft)};
}
INTRINSIC(v32bfloat8)
to_v32bfloat8_conf(v32accfloat a, uint6_t shft, crrnd_t rnd) {
  crrnd_t prev_rnd = get_rnd();

  set_rnd(rnd);

  v32bfloat8 r = to_v32bfloat8(a, shft);

  set_rnd(prev_rnd);

  return r;
}

INTRINSIC(v32float8) to_v32float8(v32accfloat a, uint6_t shft) {
  return {__builtin_aie2ps_v32accfloat_to_v32float8(a, shft)};
}
INTRINSIC(v32float8)
to_v32float8_conf(v32accfloat a, uint6_t shft, crrnd_t rnd) {
  crrnd_t prev_rnd = get_rnd();

  set_rnd(rnd);

  v32float8 r = to_v32float8(a, shft);

  set_rnd(prev_rnd);

  return r;
}

INTRINSIC(v32float8) to_v32float8(v32accfloat a) { return to_v32float8(a, 0); }
INTRINSIC(v32float8) to_v32float8_conf(v32accfloat a, crrnd_t rnd) {
  return to_v32float8_conf(a, 0, rnd);
}

INTRINSIC(v32bfloat8) to_v32bfloat8(v32accfloat a) {
  return to_v32bfloat8(a, 0);
}
INTRINSIC(v32bfloat8) to_v32bfloat8_conf(v32accfloat a, crrnd_t rnd) {
  return to_v32bfloat8_conf(a, 0, rnd);
}

INTRINSIC(v64mx9) to_v64mx9(v64accfloat a) {
  v64mx9 res;

  __builtin_aie2ps_v64accfloat_to_v64mx9(a, (v16int32 &)res.mantissa,
                                         (v2int32 &)res.tileShift,
                                         (v2int32 &)res.exponent);

  return res;
}
INTRINSIC(v64mx6) to_v64mx6(v64accfloat a) {
  v64mx6 res;

  __builtin_aie2ps_v64accfloat_to_v64mx6(
      a, (v8int32 &)res.mantissa, (v2int32 &)res.sign, (int32 &)res.tileShift,
      (int32 &)res.exponent);

  return res;
}
INTRINSIC(v64mx4) to_v64mx4(v64accfloat a) {
  v64mx4 res;

  __builtin_aie2ps_v64accfloat_to_v64mx4(
      a, (v8int32 &)res.mantissa, (v2int32 &)res.sign, (int32 &)res.tileShift,
      (int32 &)res.exponent);

  return res;
}

INTRINSIC(v64mx9) to_v64mx9_conf(v64accfloat a, crrnd_t rnd) {
  crrnd_t prev_rnd = get_rnd();

  set_rnd(rnd);

  v64mx9 r = to_v64mx9(a);

  set_rnd(prev_rnd);

  return r;
}
INTRINSIC(v64mx6) to_v64mx6_conf(v64accfloat a, crrnd_t rnd) {
  crrnd_t prev_rnd = get_rnd();

  set_rnd(rnd);

  v64mx6 r = to_v64mx6(a);

  set_rnd(prev_rnd);

  return r;
}
INTRINSIC(v64mx4) to_v64mx4_conf(v64accfloat a, crrnd_t rnd) {
  crrnd_t prev_rnd = get_rnd();

  set_rnd(rnd);

  v64mx4 r = to_v64mx4(a);

  set_rnd(prev_rnd);

  return r;
}

[[deprecated("Function 'to_v64bfp16p ()' is deprecated. Please use 'to_v64mx9 "
             "()' instead.")]] INTRINSIC(v64bfp16p)
    to_v64bfp16p(v64accfloat a) {
  return to_v64mx9(a);
}
[[deprecated("Function 'to_v64bfp13p ()' is deprecated. Please use 'to_v64mx6 "
             "()' instead.")]] INTRINSIC(v64bfp13p)
    to_v64bfp13p(v64accfloat a) {
  return to_v64mx6(a);
}
[[deprecated("Function 'to_v64bfp11p ()' is deprecated. Please use 'to_v64mx4 "
             "()' instead.")]] INTRINSIC(v64bfp11p)
    to_v64bfp11p(v64accfloat a) {
  return to_v64mx4(a);
}

#endif // AIE2PS_SRS_H
