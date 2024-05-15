//===- aiev2_srs.h ----------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

INTRINSIC(v16bfloat16) to_v16bfloat16(v16accfloat acc) {
  return __builtin_aiev2_v16accfloat_to_v16bf16(acc);
}

INTRINSIC(v32int8) ssrs(v32acc32 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v32_acc32_srs(acc, shft, sign);
}

INTRINSIC(v32int8)
ssrs_conf(v32acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int8 val = __builtin_aiev2_I256_v32_acc32_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v32uint8) ussrs(v32acc32 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v32_acc32_srs(acc, shft, sign);
}

INTRINSIC(v32uint8)
ussrs_conf(v32acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint8 val = __builtin_aiev2_I256_v32_acc32_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v32int16) lsrs(v32acc32 acc, int shft, int sign) {
  return __builtin_aiev2_I512_v32_acc32_srs(acc, shft, sign);
}

INTRINSIC(v32int16)
lsrs_conf(v32acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 val = __builtin_aiev2_I512_v32_acc32_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v32uint16) ulsrs(v32acc32 acc, int shft, int sign) {
  return __builtin_aiev2_I512_v32_acc32_srs(acc, shft, sign);
}

INTRINSIC(v32uint16)
ulsrs_conf(v32acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 val = __builtin_aiev2_I512_v32_acc32_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v16int16) lsrs(v16acc32 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v16_acc32_srs(acc, shft, sign);
}

INTRINSIC(v16int16)
lsrs_conf(v16acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 val = __builtin_aiev2_I256_v16_acc32_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v16uint16) ulsrs(v16acc32 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v16_acc32_srs(acc, shft, sign);
}

INTRINSIC(v16uint16)
ulsrs_conf(v16acc32 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 val = __builtin_aiev2_I256_v16_acc32_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v16int16) ssrs(v16acc64 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v16_acc64_srs(acc, shft, sign);
}

INTRINSIC(v16int16)
ssrs_conf(v16acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 val = __builtin_aiev2_I256_v16_acc64_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v16uint16) ussrs(v16acc64 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v16_acc64_srs(acc, shft, sign);
}

INTRINSIC(v16uint16)
ussrs_conf(v16acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 val = __builtin_aiev2_I256_v16_acc64_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v16int32) lsrs(v16acc64 acc, int shft, int sign) {
  return __builtin_aiev2_I512_v16_acc64_srs(acc, shft, sign);
}

INTRINSIC(v16int32)
lsrs_conf(v16acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int32 val = __builtin_aiev2_I512_v16_acc64_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v16uint32) ulsrs(v16acc64 acc, int shft, int sign) {
  return __builtin_aiev2_I512_v16_acc64_srs(acc, shft, sign);
}

INTRINSIC(v16uint32)
ulsrs_conf(v16acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint32 val = __builtin_aiev2_I512_v16_acc64_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v8int32) lsrs(v8acc64 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v8_acc64_srs(acc, shft, sign);
}

INTRINSIC(v8int32)
lsrs_conf(v8acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8int32 val = __builtin_aiev2_I256_v8_acc64_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v8uint32) ulsrs(v8acc64 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v8_acc64_srs(acc, shft, sign);
}

INTRINSIC(v8uint32)
ulsrs_conf(v8acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8uint32 val = __builtin_aiev2_I256_v8_acc64_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

inline v32int8 ssrs(v32acc32 acc, int shft) {
  return ssrs(acc, shft, __SIGN_SIGNED);
}

inline v32int8 ssrs_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return ssrs_conf(acc, shft, __SIGN_SIGNED, sat, rnd);
}

inline v32uint8 ussrs(v32acc32 acc, int shft) {
  return ussrs(acc, shft, __SIGN_UNSIGNED);
}

inline v32uint8 ussrs_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return ussrs_conf(acc, shft, __SIGN_UNSIGNED, sat, rnd);
}

inline v32int16 lsrs(v32acc32 acc, int shft) {
  return lsrs(acc, shft, __SIGN_SIGNED);
}

inline v32int16 lsrs_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return lsrs_conf(acc, shft, __SIGN_SIGNED, sat, rnd);
}

inline v32uint16 ulsrs(v32acc32 acc, int shft) {
  return ulsrs(acc, shft, __SIGN_UNSIGNED);
}

inline v32uint16 ulsrs_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return ulsrs_conf(acc, shft, __SIGN_UNSIGNED, sat, rnd);
}

inline v16int16 lsrs(v16acc32 acc, int shft) {
  return lsrs(acc, shft, __SIGN_SIGNED);
}

inline v16int16 lsrs_conf(v16acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return lsrs_conf(acc, shft, __SIGN_SIGNED, sat, rnd);
}

inline v16uint16 ulsrs(v16acc32 acc, int shft) {
  return ulsrs(acc, shft, __SIGN_UNSIGNED);
}

inline v16uint16 ulsrs_conf(v16acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return ulsrs_conf(acc, shft, __SIGN_UNSIGNED, sat, rnd);
}

inline v16int16 ssrs(v16acc64 acc, int shft) {
  return ssrs(acc, shft, __SIGN_SIGNED);
}

inline v16int16 ssrs_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return ssrs_conf(acc, shft, __SIGN_SIGNED, sat, rnd);
}

inline v16uint16 ussrs(v16acc64 acc, int shft) {
  return ussrs(acc, shft, __SIGN_UNSIGNED);
}

inline v16uint16 ussrs_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return ussrs_conf(acc, shft, __SIGN_UNSIGNED, sat, rnd);
}

inline v16int32 lsrs(v16acc64 acc, int shft) {
  return lsrs(acc, shft, __SIGN_SIGNED);
}

inline v16int32 lsrs_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return lsrs_conf(acc, shft, __SIGN_SIGNED, sat, rnd);
}

inline v16uint32 ulsrs(v16acc64 acc, int shft) {
  return ulsrs(acc, shft, __SIGN_UNSIGNED);
}

inline v16uint32 ulsrs_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return ulsrs_conf(acc, shft, __SIGN_UNSIGNED, sat, rnd);
}

inline v8int32 lsrs(v8acc64 acc, int shft) {
  return lsrs(acc, shft, __SIGN_SIGNED);
}

inline v8int32 lsrs_conf(v8acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return lsrs_conf(acc, shft, __SIGN_SIGNED, sat, rnd);
}

inline v8uint32 ulsrs(v8acc64 acc, int shft) {
  return ulsrs(acc, shft, __SIGN_UNSIGNED);
}

inline v8uint32 ulsrs_conf(v8acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  return ulsrs_conf(acc, shft, __SIGN_UNSIGNED, sat, rnd);
}

INTRINSIC(v32int8) srs_to_v32int8(v32acc32 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v32_acc32_srs(acc, shft, sign);
}

INTRINSIC(v32int8)
srs_to_v32int8_conf(v32acc32 acc, int shft, int sign, crsat_t sat,
                    crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int8 val = __builtin_aiev2_I256_v32_acc32_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v32uint8) srs_to_v32uint8(v32acc32 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v32_acc32_srs(acc, shft, sign);
}

INTRINSIC(v32uint8)
srs_to_v32uint8_conf(v32acc32 acc, int shft, int sign, crsat_t sat,
                     crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint8 val = __builtin_aiev2_I256_v32_acc32_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v32int16) srs_to_v32int16(v32acc32 acc, int shft, int sign) {
  return __builtin_aiev2_I512_v32_acc32_srs(acc, shft, sign);
}

INTRINSIC(v32int16)
srs_to_v32int16_conf(v32acc32 acc, int shft, int sign, crsat_t sat,
                     crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 val = __builtin_aiev2_I512_v32_acc32_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v32uint16) srs_to_v32uint16(v32acc32 acc, int shft, int sign) {
  return __builtin_aiev2_I512_v32_acc32_srs(acc, shft, sign);
}

INTRINSIC(v32uint16)
srs_to_v32uint16_conf(v32acc32 acc, int shft, int sign, crsat_t sat,
                      crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 val = __builtin_aiev2_I512_v32_acc32_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v16int16) srs_to_v16int16(v16acc32 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v16_acc32_srs(acc, shft, sign);
}

INTRINSIC(v16int16)
srs_to_v16int16_conf(v16acc32 acc, int shft, int sign, crsat_t sat,
                     crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 val = __builtin_aiev2_I256_v16_acc32_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v16uint16) srs_to_v16uint16(v16acc32 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v16_acc32_srs(acc, shft, sign);
}

INTRINSIC(v16uint16)
srs_to_v16uint16_conf(v16acc32 acc, int shft, int sign, crsat_t sat,
                      crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 val = __builtin_aiev2_I256_v16_acc32_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v16int16) srs_to_v16int16(v16acc64 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v16_acc64_srs(acc, shft, sign);
}

INTRINSIC(v16int16)
srs_to_v16int16_conf(v16acc64 acc, int shft, int sign, crsat_t sat,
                     crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 val = __builtin_aiev2_I256_v16_acc64_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v16uint16) srs_to_v16uint16(v16acc64 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v16_acc64_srs(acc, shft, sign);
}

INTRINSIC(v16uint16)
srs_to_v16uint16_conf(v16acc64 acc, int shft, int sign, crsat_t sat,
                      crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 val = __builtin_aiev2_I256_v16_acc64_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v16int32) srs_to_int32(v16acc64 acc, int shft, int sign) {
  return __builtin_aiev2_I512_v16_acc64_srs(acc, shft, sign);
}

INTRINSIC(v16int32)
srs_to_int32_conf(v16acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int32 val = __builtin_aiev2_I512_v16_acc64_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v16uint32) srs_to_uint32(v16acc64 acc, int shft, int sign) {
  return __builtin_aiev2_I512_v16_acc64_srs(acc, shft, sign);
}

INTRINSIC(v16uint32)
srs_to_uint32_conf(v16acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint32 val = __builtin_aiev2_I512_v16_acc64_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v8int32) srs_to_v8int32(v8acc64 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v8_acc64_srs(acc, shft, sign);
}

INTRINSIC(v8int32)
srs_to_v8int32_conf(v8acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8int32 val = __builtin_aiev2_I256_v8_acc64_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v8uint32) srs_to_v8uint32(v8acc64 acc, int shft, int sign) {
  return __builtin_aiev2_I256_v8_acc64_srs(acc, shft, sign);
}

INTRINSIC(v8uint32)
srs_to_v8uint32_conf(v8acc64 acc, int shft, int sign, crsat_t sat,
                     crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8uint32 val = __builtin_aiev2_I256_v8_acc64_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v16int32) srs_to_v16int32(v16acc64 acc, int shft, int sign) {
  return __builtin_aiev2_I512_v16_acc64_srs(acc, shft, sign);
}

INTRINSIC(v16int32)
srs_to_v16int32_conf(v16acc64 acc, int shft, int sign, crsat_t sat,
                     crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int32 val = __builtin_aiev2_I512_v16_acc64_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

INTRINSIC(v16uint32) srs_to_v16uint32(v16acc64 acc, int shft, int sign) {
  return __builtin_aiev2_I512_v16_acc64_srs(acc, shft, sign);
}

INTRINSIC(v16uint32)
srs_to_v16uint32_conf(v16acc64 acc, int shft, int sign, crsat_t sat,
                      crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint32 val = __builtin_aiev2_I512_v16_acc64_srs(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return val;
}

inline v32int8 srs_to_v32int8(v32acc32 acc, int shft) {
  return ssrs(acc, shft, __SIGN_SIGNED);
}

inline v32int8 srs_to_v32int8_conf(v32acc32 acc, int shft, crsat_t sat,
                                   crrnd_t rnd) {
  return ssrs_conf(acc, shft, __SIGN_SIGNED, sat, rnd);
}

inline v32uint8 srs_to_v32uint8(v32acc32 acc, int shft) {
  return ussrs(acc, shft, __SIGN_UNSIGNED);
}

inline v32uint8 srs_to_v32uint8_conf(v32acc32 acc, int shft, crsat_t sat,
                                     crrnd_t rnd) {
  return ussrs_conf(acc, shft, __SIGN_UNSIGNED, sat, rnd);
}

inline v32int16 srs_to_v32int16(v32acc32 acc, int shft) {
  return lsrs(acc, shft, __SIGN_SIGNED);
}

inline v32int16 srs_to_v32int16_conf(v32acc32 acc, int shft, crsat_t sat,
                                     crrnd_t rnd) {
  return lsrs_conf(acc, shft, __SIGN_SIGNED, sat, rnd);
}

inline v32uint16 srs_to_v32uint16(v32acc32 acc, int shft) {
  return ulsrs(acc, shft, __SIGN_UNSIGNED);
}

inline v32uint16 srs_to_v32uint16_conf(v32acc32 acc, int shft, crsat_t sat,
                                       crrnd_t rnd) {
  return ulsrs_conf(acc, shft, __SIGN_UNSIGNED, sat, rnd);
}

inline v16int16 srs_to_v16int16(v16acc32 acc, int shft) {
  return lsrs(acc, shft, __SIGN_SIGNED);
}

inline v16int16 srs_to_v16int16_conf(v16acc32 acc, int shft, crsat_t sat,
                                     crrnd_t rnd) {
  return lsrs_conf(acc, shft, __SIGN_SIGNED, sat, rnd);
}

inline v16uint16 srs_to_v16uint16(v16acc32 acc, int shft) {
  return ulsrs(acc, shft, __SIGN_UNSIGNED);
}

inline v16uint16 srs_to_v16uint16_conf(v16acc32 acc, int shft, crsat_t sat,
                                       crrnd_t rnd) {
  return ulsrs_conf(acc, shft, __SIGN_UNSIGNED, sat, rnd);
}

inline v16int16 srs_to_v16int16(v16acc64 acc, int shft) {
  return ssrs(acc, shft, __SIGN_SIGNED);
}

inline v16int16 srs_to_v16int16_conf(v16acc64 acc, int shft, crsat_t sat,
                                     crrnd_t rnd) {
  return ssrs_conf(acc, shft, __SIGN_SIGNED, sat, rnd);
}

inline v16uint16 srs_to_v16uint16(v16acc64 acc, int shft) {
  return ussrs(acc, shft, __SIGN_UNSIGNED);
}

inline v16uint16 srs_to_v16uint16_conf(v16acc64 acc, int shft, crsat_t sat,
                                       crrnd_t rnd) {
  return ussrs_conf(acc, shft, __SIGN_UNSIGNED, sat, rnd);
}

inline v16int32 srs_to_v16int32(v16acc64 acc, int shft) {
  return lsrs(acc, shft, __SIGN_SIGNED);
}

inline v16int32 srs_to_v16int32_conf(v16acc64 acc, int shft, crsat_t sat,
                                     crrnd_t rnd) {
  return lsrs_conf(acc, shft, __SIGN_SIGNED, sat, rnd);
}

inline v16uint32 srs_to_v16uint32(v16acc64 acc, int shft) {
  return ulsrs(acc, shft, __SIGN_UNSIGNED);
}

inline v16uint32 srs_to_v16uint32_conf(v16acc64 acc, int shft, crsat_t sat,
                                       crrnd_t rnd) {
  return ulsrs_conf(acc, shft, __SIGN_UNSIGNED, sat, rnd);
}

inline v8int32 srs_to_v8int32(v8acc64 acc, int shft) {
  return lsrs(acc, shft, __SIGN_SIGNED);
}

inline v8int32 srs_to_v8int32_conf(v8acc64 acc, int shft, crsat_t sat,
                                   crrnd_t rnd) {
  return lsrs_conf(acc, shft, __SIGN_SIGNED, sat, rnd);
}

inline v8uint32 srs_to_v8uint32(v8acc64 acc, int shft) {
  return ulsrs(acc, shft, __SIGN_UNSIGNED);
}

inline v8uint32 srs_to_v8uint32_conf(v8acc64 acc, int shft, crsat_t sat,
                                     crrnd_t rnd) {
  return ulsrs_conf(acc, shft, __SIGN_UNSIGNED, sat, rnd);
}

inline v32bfloat16 to_v32bfloat16(v32accfloat acc) {
  v16bfloat16 x0 = to_v16bfloat16(extract_v16accfloat(acc, 0));

  v16bfloat16 x1 = to_v16bfloat16(extract_v16accfloat(acc, 1));

  return concat(x0, x1);
}
