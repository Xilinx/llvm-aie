//===-------------------- AIEngine AIE2p intrinsics ------------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef AIE2P_UPS_H
#define AIE2P_UPS_H

//* Automatically generated file, do not edit! *
//

typedef unsigned int crsat_t;

INTRINSIC(v8acc64) lups(v8int32 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v8_I256_ups(a, shft, sign);
}
INTRINSIC(v8acc64) lups_conf(v8int32 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v8acc64 val = lups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v8acc64) lups(v8int32 a, int shft) {
  return lups(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v8acc64) lups_conf(v8int32 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v8acc64 val = lups(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v8acc64) lups(v8uint32 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v8_I256_ups(a, shft, sign);
}
INTRINSIC(v8acc64) lups_conf(v8uint32 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v8acc64 val = lups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v8acc64) lups(v8uint32 a, int shft) {
  return lups(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v8acc64) lups_conf(v8uint32 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v8acc64 val = lups(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc32) sups(v16int16 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v16_I256_ups(a, shft, sign);
}
INTRINSIC(v16acc32) sups_conf(v16int16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc32 val = sups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc32) sups(v16int16 a, int shft) {
  return sups(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v16acc32) sups_conf(v16int16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc32 val = sups(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc32) sups(v16uint16 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v16_I256_ups(a, shft, sign);
}
INTRINSIC(v16acc32) sups_conf(v16uint16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc32 val = sups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc32) sups(v16uint16 a, int shft) {
  return sups(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16acc32) sups_conf(v16uint16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc32 val = sups(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) sups(v32int8 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v32_I256_ups(a, shft, sign);
}
INTRINSIC(v32acc32) sups_conf(v32int8 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = sups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) sups(v32int8 a, int shft) {
  return sups(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v32acc32) sups_conf(v32int8 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = sups(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) sups(v32uint8 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v32_I256_ups(a, shft, sign);
}
INTRINSIC(v32acc32) sups_conf(v32uint8 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = sups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) sups(v32uint8 a, int shft) {
  return sups(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32acc32) sups_conf(v32uint8 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = sups(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) lups(v16int16 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v16_I256_ups(a, shft, sign);
}
INTRINSIC(v16acc64) lups_conf(v16int16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = lups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) lups(v16int16 a, int shft) {
  return lups(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v16acc64) lups_conf(v16int16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = lups(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) lups(v16uint16 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v16_I256_ups(a, shft, sign);
}
INTRINSIC(v16acc64) lups_conf(v16uint16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = lups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) lups(v16uint16 a, int shft) {
  return lups(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16acc64) lups_conf(v16uint16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = lups(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) lups(v16int32 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v16_I512_ups(a, shft, sign);
}
INTRINSIC(v16acc64) lups_conf(v16int32 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = lups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) lups(v16int32 a, int shft) {
  return lups(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v16acc64) lups_conf(v16int32 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = lups(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) lups(v16uint32 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v16_I512_ups(a, shft, sign);
}
INTRINSIC(v16acc64) lups_conf(v16uint32 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = lups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) lups(v16uint32 a, int shft) {
  return lups(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16acc64) lups_conf(v16uint32 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = lups(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) sups(v32int16 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v32_I512_ups(a, shft, sign);
}
INTRINSIC(v32acc32) sups_conf(v32int16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = sups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) sups(v32int16 a, int shft) {
  return sups(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v32acc32) sups_conf(v32int16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = sups(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) sups(v32uint16 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v32_I512_ups(a, shft, sign);
}
INTRINSIC(v32acc32) sups_conf(v32uint16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = sups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) sups(v32uint16 a, int shft) {
  return sups(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32acc32) sups_conf(v32uint16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = sups(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64acc32) sups(v64int8 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v64_I512_ups(a, shft, sign);
}
INTRINSIC(v64acc32) sups_conf(v64int8 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64acc32 val = sups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64acc32) sups(v64int8 a, int shft) {
  return sups(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v64acc32) sups_conf(v64int8 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64acc32 val = sups(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64acc32) sups(v64uint8 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v64_I512_ups(a, shft, sign);
}
INTRINSIC(v64acc32) sups_conf(v64uint8 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64acc32 val = sups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64acc32) sups(v64uint8 a, int shft) {
  return sups(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v64acc32) sups_conf(v64uint8 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64acc32 val = sups(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc64) lups(v32int16 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v32_I512_ups(a, shft, sign);
}
INTRINSIC(v32acc64) lups_conf(v32int16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc64 val = lups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc64) lups(v32int16 a, int shft) {
  return lups(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v32acc64) lups_conf(v32int16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc64 val = lups(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc64) lups(v32uint16 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v32_I512_ups(a, shft, sign);
}
INTRINSIC(v32acc64) lups_conf(v32uint16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc64 val = lups(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc64) lups(v32uint16 a, int shft) {
  return lups(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32acc64) lups_conf(v32uint16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc64 val = lups(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v8acc64) ups_to_v8acc64(v8int32 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v8_I256_ups(a, shft, sign);
}
INTRINSIC(v8acc64)
ups_to_v8acc64_conf(v8int32 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v8acc64 val = ups_to_v8acc64(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v8acc64) ups_to_v8acc64(v8int32 a, int shft) {
  return ups_to_v8acc64(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v8acc64) ups_to_v8acc64_conf(v8int32 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v8acc64 val = ups_to_v8acc64(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v8acc64) ups_to_v8acc64(v8uint32 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v8_I256_ups(a, shft, sign);
}
INTRINSIC(v8acc64)
ups_to_v8acc64_conf(v8uint32 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v8acc64 val = ups_to_v8acc64(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v8acc64) ups_to_v8acc64(v8uint32 a, int shft) {
  return ups_to_v8acc64(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v8acc64) ups_to_v8acc64_conf(v8uint32 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v8acc64 val = ups_to_v8acc64(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc32) ups_to_v16acc32(v16int16 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v16_I256_ups(a, shft, sign);
}
INTRINSIC(v16acc32)
ups_to_v16acc32_conf(v16int16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc32 val = ups_to_v16acc32(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc32) ups_to_v16acc32(v16int16 a, int shft) {
  return ups_to_v16acc32(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v16acc32) ups_to_v16acc32_conf(v16int16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc32 val = ups_to_v16acc32(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc32) ups_to_v16acc32(v16uint16 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v16_I256_ups(a, shft, sign);
}
INTRINSIC(v16acc32)
ups_to_v16acc32_conf(v16uint16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc32 val = ups_to_v16acc32(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc32) ups_to_v16acc32(v16uint16 a, int shft) {
  return ups_to_v16acc32(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16acc32) ups_to_v16acc32_conf(v16uint16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc32 val = ups_to_v16acc32(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) ups_to_v32acc32(v32int8 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v32_I256_ups(a, shft, sign);
}
INTRINSIC(v32acc32)
ups_to_v32acc32_conf(v32int8 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = ups_to_v32acc32(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) ups_to_v32acc32(v32int8 a, int shft) {
  return ups_to_v32acc32(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v32acc32) ups_to_v32acc32_conf(v32int8 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = ups_to_v32acc32(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) ups_to_v32acc32(v32uint8 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v32_I256_ups(a, shft, sign);
}
INTRINSIC(v32acc32)
ups_to_v32acc32_conf(v32uint8 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = ups_to_v32acc32(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) ups_to_v32acc32(v32uint8 a, int shft) {
  return ups_to_v32acc32(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32acc32) ups_to_v32acc32_conf(v32uint8 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = ups_to_v32acc32(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) ups_to_v16acc64(v16int16 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v16_I256_ups(a, shft, sign);
}
INTRINSIC(v16acc64)
ups_to_v16acc64_conf(v16int16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = ups_to_v16acc64(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) ups_to_v16acc64(v16int16 a, int shft) {
  return ups_to_v16acc64(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v16acc64) ups_to_v16acc64_conf(v16int16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = ups_to_v16acc64(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) ups_to_v16acc64(v16uint16 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v16_I256_ups(a, shft, sign);
}
INTRINSIC(v16acc64)
ups_to_v16acc64_conf(v16uint16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = ups_to_v16acc64(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) ups_to_v16acc64(v16uint16 a, int shft) {
  return ups_to_v16acc64(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16acc64) ups_to_v16acc64_conf(v16uint16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = ups_to_v16acc64(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) ups_to_v16acc64(v16int32 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v16_I512_ups(a, shft, sign);
}
INTRINSIC(v16acc64)
ups_to_v16acc64_conf(v16int32 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = ups_to_v16acc64(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) ups_to_v16acc64(v16int32 a, int shft) {
  return ups_to_v16acc64(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v16acc64) ups_to_v16acc64_conf(v16int32 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = ups_to_v16acc64(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) ups_to_v16acc64(v16uint32 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v16_I512_ups(a, shft, sign);
}
INTRINSIC(v16acc64)
ups_to_v16acc64_conf(v16uint32 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = ups_to_v16acc64(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v16acc64) ups_to_v16acc64(v16uint32 a, int shft) {
  return ups_to_v16acc64(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16acc64) ups_to_v16acc64_conf(v16uint32 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v16acc64 val = ups_to_v16acc64(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) ups_to_v32acc32(v32int16 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v32_I512_ups(a, shft, sign);
}
INTRINSIC(v32acc32)
ups_to_v32acc32_conf(v32int16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = ups_to_v32acc32(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) ups_to_v32acc32(v32int16 a, int shft) {
  return ups_to_v32acc32(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v32acc32) ups_to_v32acc32_conf(v32int16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = ups_to_v32acc32(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) ups_to_v32acc32(v32uint16 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v32_I512_ups(a, shft, sign);
}
INTRINSIC(v32acc32)
ups_to_v32acc32_conf(v32uint16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = ups_to_v32acc32(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc32) ups_to_v32acc32(v32uint16 a, int shft) {
  return ups_to_v32acc32(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32acc32) ups_to_v32acc32_conf(v32uint16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc32 val = ups_to_v32acc32(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64acc32) ups_to_v64acc32(v64int8 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v64_I512_ups(a, shft, sign);
}
INTRINSIC(v64acc32)
ups_to_v64acc32_conf(v64int8 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64acc32 val = ups_to_v64acc32(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64acc32) ups_to_v64acc32(v64int8 a, int shft) {
  return ups_to_v64acc32(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v64acc32) ups_to_v64acc32_conf(v64int8 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64acc32 val = ups_to_v64acc32(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64acc32) ups_to_v64acc32(v64uint8 a, int shft, int sign) {
  return __builtin_aie2p_acc32_v64_I512_ups(a, shft, sign);
}
INTRINSIC(v64acc32)
ups_to_v64acc32_conf(v64uint8 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64acc32 val = ups_to_v64acc32(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64acc32) ups_to_v64acc32(v64uint8 a, int shft) {
  return ups_to_v64acc32(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v64acc32) ups_to_v64acc32_conf(v64uint8 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64acc32 val = ups_to_v64acc32(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc64) ups_to_v32acc64(v32int16 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v32_I512_ups(a, shft, sign);
}
INTRINSIC(v32acc64)
ups_to_v32acc64_conf(v32int16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc64 val = ups_to_v32acc64(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc64) ups_to_v32acc64(v32int16 a, int shft) {
  return ups_to_v32acc64(a, shft, __SIGN_SIGNED);
}
INTRINSIC(v32acc64) ups_to_v32acc64_conf(v32int16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc64 val = ups_to_v32acc64(a, shft, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc64) ups_to_v32acc64(v32uint16 a, int shft, int sign) {
  return __builtin_aie2p_acc64_v32_I512_ups(a, shft, sign);
}
INTRINSIC(v32acc64)
ups_to_v32acc64_conf(v32uint16 a, int shft, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc64 val = ups_to_v32acc64(a, shft, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32acc64) ups_to_v32acc64(v32uint16 a, int shft) {
  return ups_to_v32acc64(a, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32acc64) ups_to_v32acc64_conf(v32uint16 a, int shft, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32acc64 val = ups_to_v32acc64(a, shft, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}

INTRINSIC(v16accfloat) ups_to_v16accfloat(v16bfloat16 a) {
  return __builtin_aie2p_v16bf16_to_v16accfloat(a);
}
INTRINSIC(v16accfloat) ups(v16bfloat16 a) { return ups_to_v16accfloat(a); }

INTRINSIC(v16accfloat) ups_to_v16accfloat(v8float a) {
  return __builtin_aie2p_v8float_to_v16accfloat(a);
}
INTRINSIC(v16accfloat) ups(v8float a) { return ups_to_v16accfloat(a); }

INTRINSIC(v32accfloat) ups_to_v32accfloat(v32bfloat16 a) {
  return __builtin_aie2p_v32bf16_to_v32accfloat(a);
}
INTRINSIC(v32accfloat) ups(v32bfloat16 a) { return ups_to_v32accfloat(a); }

INTRINSIC(v32accfloat) ups_to_v32accfloat(v16float a) {
  return __builtin_aie2p_v16float_to_v32accfloat(a);
}
INTRINSIC(v32accfloat) ups(v16float a) { return ups_to_v32accfloat(a); }

#endif // AIE2P_UPS_H
