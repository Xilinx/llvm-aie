//===-------------------- AIEngine AIE2p intrinsics ------------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef AIE2P_SRS_H
#define AIE2P_SRS_H

//* Automatically generated file, do not edit! *
//

typedef unsigned int crsat_t;

typedef unsigned int crrnd_t;

INTRINSIC(v32int8) ssrs(v32acc32 acc, int shft, int sign) {
  return __builtin_aie2p_I256_v32_acc32_srs(acc, shft, sign);
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
  return __builtin_aie2p_I256_v32_acc32_srs(acc, shft, sign);
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
  return __builtin_aie2p_I512_v64_acc32_srs(acc, shft, sign);
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
  return __builtin_aie2p_I512_v64_acc32_srs(acc, shft, sign);
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
  return __builtin_aie2p_I512_v32_acc32_srs(acc, shft, sign);
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
  return __builtin_aie2p_I512_v32_acc32_srs(acc, shft, sign);
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
  return __builtin_aie2p_I256_v16_acc32_srs(acc, shft, sign);
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
  return __builtin_aie2p_I256_v16_acc32_srs(acc, shft, sign);
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
  return __builtin_aie2p_I256_v16_acc64_srs(acc, shft, sign);
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
  return __builtin_aie2p_I256_v16_acc64_srs(acc, shft, sign);
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
  return __builtin_aie2p_I512_v32_acc64_srs(acc, shft, sign);
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
  return __builtin_aie2p_I512_v32_acc64_srs(acc, shft, sign);
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
  return __builtin_aie2p_I512_v16_acc64_srs(acc, shft, sign);
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
  return __builtin_aie2p_I512_v16_acc64_srs(acc, shft, sign);
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
  return __builtin_aie2p_I256_v8_acc64_srs(acc, shft, sign);
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
  return __builtin_aie2p_I256_v8_acc64_srs(acc, shft, sign);
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
  return ssrs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v32int8) ssrs_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int8 r = ssrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint8) ussrs(v32acc32 acc, int shft) {
  return ussrs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32uint8)
ussrs_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint8 r = ussrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v64int8) ssrs(v64acc32 acc, int shft) {
  return ssrs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v64int8) ssrs_conf(v64acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v64int8 r = ssrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v64uint8) ussrs(v64acc32 acc, int shft) {
  return ussrs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v64uint8)
ussrs_conf(v64acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v64uint8 r = ussrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v32int16) lsrs(v32acc32 acc, int shft) {
  return lsrs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v32int16)
lsrs_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 r = lsrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint16) ulsrs(v32acc32 acc, int shft) {
  return ulsrs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32uint16)
ulsrs_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 r = ulsrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16int16) lsrs(v16acc32 acc, int shft) {
  return lsrs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v16int16)
lsrs_conf(v16acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 r = lsrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint16) ulsrs(v16acc32 acc, int shft) {
  return ulsrs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16uint16)
ulsrs_conf(v16acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 r = ulsrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v16int16) ssrs(v16acc64 acc, int shft) {
  return ssrs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v16int16)
ssrs_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 r = ssrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint16) ussrs(v16acc64 acc, int shft) {
  return ussrs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16uint16)
ussrs_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 r = ussrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v32int16) ssrs(v32acc64 acc, int shft) {
  return ssrs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v32int16)
ssrs_conf(v32acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 r = ssrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint16) ussrs(v32acc64 acc, int shft) {
  return ussrs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32uint16)
ussrs_conf(v32acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 r = ussrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v16int32) lsrs(v16acc64 acc, int shft) {
  return lsrs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v16int32)
lsrs_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int32 r = lsrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint32) ulsrs(v16acc64 acc, int shft) {
  return ulsrs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16uint32)
ulsrs_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint32 r = ulsrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v8int32) lsrs(v8acc64 acc, int shft) {
  return lsrs(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v8int32) lsrs_conf(v8acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8int32 r = lsrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v8uint32) ulsrs(v8acc64 acc, int shft) {
  return ulsrs(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v8uint32)
ulsrs_conf(v8acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8uint32 r = ulsrs(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32int8) srs_to_v32int8(v32acc32 acc, int shft, int sign) {
  return __builtin_aie2p_I256_v32_acc32_srs(acc, shft, sign);
}
INTRINSIC(v32int8)
srs_to_v32int8_conf(v32acc32 acc, int shft, int sign, crsat_t sat,
                    crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int8 r = srs_to_v32int8(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint8) srs_to_v32uint8(v32acc32 acc, int shft, int sign) {
  return __builtin_aie2p_I256_v32_acc32_srs(acc, shft, sign);
}
INTRINSIC(v32uint8)
srs_to_v32uint8_conf(v32acc32 acc, int shft, int sign, crsat_t sat,
                     crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint8 r = srs_to_v32uint8(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v64int8) srs_to_v64int8(v64acc32 acc, int shft, int sign) {
  return __builtin_aie2p_I512_v64_acc32_srs(acc, shft, sign);
}
INTRINSIC(v64int8)
srs_to_v64int8_conf(v64acc32 acc, int shft, int sign, crsat_t sat,
                    crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v64int8 r = srs_to_v64int8(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v64uint8) srs_to_v64uint8(v64acc32 acc, int shft, int sign) {
  return __builtin_aie2p_I512_v64_acc32_srs(acc, shft, sign);
}
INTRINSIC(v64uint8)
srs_to_v64uint8_conf(v64acc32 acc, int shft, int sign, crsat_t sat,
                     crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v64uint8 r = srs_to_v64uint8(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v32int16) srs_to_v32int16(v32acc32 acc, int shft, int sign) {
  return __builtin_aie2p_I512_v32_acc32_srs(acc, shft, sign);
}
INTRINSIC(v32int16)
srs_to_v32int16_conf(v32acc32 acc, int shft, int sign, crsat_t sat,
                     crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 r = srs_to_v32int16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint16) srs_to_v32uint16(v32acc32 acc, int shft, int sign) {
  return __builtin_aie2p_I512_v32_acc32_srs(acc, shft, sign);
}
INTRINSIC(v32uint16)
srs_to_v32uint16_conf(v32acc32 acc, int shft, int sign, crsat_t sat,
                      crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 r = srs_to_v32uint16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16int16) srs_to_v16int16(v16acc32 acc, int shft, int sign) {
  return __builtin_aie2p_I256_v16_acc32_srs(acc, shft, sign);
}
INTRINSIC(v16int16)
srs_to_v16int16_conf(v16acc32 acc, int shft, int sign, crsat_t sat,
                     crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 r = srs_to_v16int16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint16) srs_to_v16uint16(v16acc32 acc, int shft, int sign) {
  return __builtin_aie2p_I256_v16_acc32_srs(acc, shft, sign);
}
INTRINSIC(v16uint16)
srs_to_v16uint16_conf(v16acc32 acc, int shft, int sign, crsat_t sat,
                      crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 r = srs_to_v16uint16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v16int16) srs_to_v16int16(v16acc64 acc, int shft, int sign) {
  return __builtin_aie2p_I256_v16_acc64_srs(acc, shft, sign);
}
INTRINSIC(v16int16)
srs_to_v16int16_conf(v16acc64 acc, int shft, int sign, crsat_t sat,
                     crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 r = srs_to_v16int16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint16) srs_to_v16uint16(v16acc64 acc, int shft, int sign) {
  return __builtin_aie2p_I256_v16_acc64_srs(acc, shft, sign);
}
INTRINSIC(v16uint16)
srs_to_v16uint16_conf(v16acc64 acc, int shft, int sign, crsat_t sat,
                      crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 r = srs_to_v16uint16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v32int16) srs_to_v32int16(v32acc64 acc, int shft, int sign) {
  return __builtin_aie2p_I512_v32_acc64_srs(acc, shft, sign);
}
INTRINSIC(v32int16)
srs_to_v32int16_conf(v32acc64 acc, int shft, int sign, crsat_t sat,
                     crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 r = srs_to_v32int16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint16) srs_to_v32uint16(v32acc64 acc, int shft, int sign) {
  return __builtin_aie2p_I512_v32_acc64_srs(acc, shft, sign);
}
INTRINSIC(v32uint16)
srs_to_v32uint16_conf(v32acc64 acc, int shft, int sign, crsat_t sat,
                      crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 r = srs_to_v32uint16(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v16int32) srs_to_v16int32(v16acc64 acc, int shft, int sign) {
  return __builtin_aie2p_I512_v16_acc64_srs(acc, shft, sign);
}
INTRINSIC(v16int32)
srs_to_v16int32_conf(v16acc64 acc, int shft, int sign, crsat_t sat,
                     crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int32 r = srs_to_v16int32(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint32) srs_to_v16uint32(v16acc64 acc, int shft, int sign) {
  return __builtin_aie2p_I512_v16_acc64_srs(acc, shft, sign);
}
INTRINSIC(v16uint32)
srs_to_v16uint32_conf(v16acc64 acc, int shft, int sign, crsat_t sat,
                      crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint32 r = srs_to_v16uint32(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v8int32) srs_to_v8int32(v8acc64 acc, int shft, int sign) {
  return __builtin_aie2p_I256_v8_acc64_srs(acc, shft, sign);
}
INTRINSIC(v8int32)
srs_to_v8int32_conf(v8acc64 acc, int shft, int sign, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8int32 r = srs_to_v8int32(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v8uint32) srs_to_v8uint32(v8acc64 acc, int shft, int sign) {
  return __builtin_aie2p_I256_v8_acc64_srs(acc, shft, sign);
}
INTRINSIC(v8uint32)
srs_to_v8uint32_conf(v8acc64 acc, int shft, int sign, crsat_t sat,
                     crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8uint32 r = srs_to_v8uint32(acc, shft, sign);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v32int8) srs_to_v32int8(v32acc32 acc, int shft) {
  return srs_to_v32int8(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v32int8)
srs_to_v32int8_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int8 r = srs_to_v32int8(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint8) srs_to_v32uint8(v32acc32 acc, int shft) {
  return srs_to_v32uint8(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32uint8)
srs_to_v32uint8_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint8 r = srs_to_v32uint8(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v64int8) srs_to_v64int8(v64acc32 acc, int shft) {
  return srs_to_v64int8(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v64int8)
srs_to_v64int8_conf(v64acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v64int8 r = srs_to_v64int8(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v64uint8) srs_to_v64uint8(v64acc32 acc, int shft) {
  return srs_to_v64uint8(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v64uint8)
srs_to_v64uint8_conf(v64acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v64uint8 r = srs_to_v64uint8(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v32int16) srs_to_v32int16(v32acc32 acc, int shft) {
  return srs_to_v32int16(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v32int16)
srs_to_v32int16_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 r = srs_to_v32int16(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint16) srs_to_v32uint16(v32acc32 acc, int shft) {
  return srs_to_v32uint16(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32uint16)
srs_to_v32uint16_conf(v32acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 r = srs_to_v32uint16(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16int16) srs_to_v16int16(v16acc32 acc, int shft) {
  return srs_to_v16int16(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v16int16)
srs_to_v16int16_conf(v16acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 r = srs_to_v16int16(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint16) srs_to_v16uint16(v16acc32 acc, int shft) {
  return srs_to_v16uint16(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16uint16)
srs_to_v16uint16_conf(v16acc32 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 r = srs_to_v16uint16(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v16int16) srs_to_v16int16(v16acc64 acc, int shft) {
  return srs_to_v16int16(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v16int16)
srs_to_v16int16_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int16 r = srs_to_v16int16(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint16) srs_to_v16uint16(v16acc64 acc, int shft) {
  return srs_to_v16uint16(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16uint16)
srs_to_v16uint16_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint16 r = srs_to_v16uint16(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v32int16) srs_to_v32int16(v32acc64 acc, int shft) {
  return srs_to_v32int16(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v32int16)
srs_to_v32int16_conf(v32acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32int16 r = srs_to_v32int16(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v32uint16) srs_to_v32uint16(v32acc64 acc, int shft) {
  return srs_to_v32uint16(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v32uint16)
srs_to_v32uint16_conf(v32acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v32uint16 r = srs_to_v32uint16(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v16int32) srs_to_v16int32(v16acc64 acc, int shft) {
  return srs_to_v16int32(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v16int32)
srs_to_v16int32_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16int32 r = srs_to_v16int32(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v16uint32) srs_to_v16uint32(v16acc64 acc, int shft) {
  return srs_to_v16uint32(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v16uint32)
srs_to_v16uint32_conf(v16acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v16uint32 r = srs_to_v16uint32(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v8int32) srs_to_v8int32(v8acc64 acc, int shft) {
  return srs_to_v8int32(acc, shft, __SIGN_SIGNED);
}
INTRINSIC(v8int32)
srs_to_v8int32_conf(v8acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8int32 r = srs_to_v8int32(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}
INTRINSIC(v8uint32) srs_to_v8uint32(v8acc64 acc, int shft) {
  return srs_to_v8uint32(acc, shft, __SIGN_UNSIGNED);
}
INTRINSIC(v8uint32)
srs_to_v8uint32_conf(v8acc64 acc, int shft, crsat_t sat, crrnd_t rnd) {
  crsat_t prev_sat = get_sat();
  crrnd_t prev_rnd = get_rnd();
  set_satmode(sat);
  set_rnd(rnd);
  v8uint32 r = srs_to_v8uint32(acc, shft);
  set_satmode(prev_sat);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v16bfloat16) to_v16bfloat16(v16accfloat acc) {
  return __builtin_aie2p_v16accfloat_to_v16bf16(acc);
}
INTRINSIC(v32bfloat16) to_v32bfloat16(v32accfloat acc) {
  return __builtin_aie2p_v32accfloat_to_v32bf16(acc);
}

INTRINSIC(v16bfloat16) srs_to_v16bfloat16(v16accfloat acc) {
  return to_v16bfloat16(acc);
}
INTRINSIC(v32bfloat16) srs_to_v32bfloat16(v32accfloat acc) {
  return to_v32bfloat16(acc);
}

INTRINSIC(v8float) to_v8float(v16accfloat acc) {
  return __builtin_aie2p_v16accfloat_to_v8float(acc);
}
INTRINSIC(v16float) to_v16float(v32accfloat acc) {
  return __builtin_aie2p_v32accfloat_to_v16float(acc);
}

INTRINSIC(v8float) srs_to_v8float(v16accfloat acc) { return to_v8float(acc); }
INTRINSIC(v16float) srs_to_v16float(v32accfloat acc) {
  return to_v16float(acc);
}

// Floating-point accumulator to block floating-point vector register
INTRINSIC(v64bfp16ebs8) to_v64bfp16ebs8(v64accfloat a) {
  v64bfp16ebs8 r;
  __builtin_aie2p_v64accfloat_to_v64bfp16ebs8((v64char &)r.mantissa,
                                              (v8char &)r.exponent, a);
  return r;
}

INTRINSIC(v64bfp16ebs16) to_v64bfp16ebs16(v64accfloat a) {
  v64bfp16ebs16 r;
  __builtin_aie2p_v64accfloat_to_v64bfp16ebs16((v64char &)r.mantissa,
                                               (v8char &)r.exponent, a);
  return r;
}

INTRINSIC(v64bfp16ebs16) to_v64bfp16ebs16(v64bfp16ebs8 a) {
  v64int8 mantissa = a.mantissa;
  v8int8 exponent = a.exponent;
  v64bfp16ebs16 r;
  __builtin_aie2p_v64bfp16ebs8_to_v64bfp16ebs16(
      (v64char &)r.mantissa, (v8char &)r.exponent, mantissa, exponent);
  return r;
}

INTRINSIC(v64bfp16ebs8) to_v64bfp16ebs8_conf(v64accfloat a, crrnd_t rnd) {
  crrnd_t prev_rnd = get_rnd();
  set_rnd(rnd);
  v64bfp16ebs8 r = to_v64bfp16ebs8(a);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v64bfp16ebs16) to_v64bfp16ebs16_conf(v64accfloat a, crrnd_t rnd) {
  crrnd_t prev_rnd = get_rnd();
  set_rnd(rnd);
  v64bfp16ebs16 r = to_v64bfp16ebs16(a);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v64bfp16ebs16) to_v64bfp16ebs16_conf(v64bfp16ebs8 a, crrnd_t rnd) {
  crrnd_t prev_rnd = get_rnd();
  set_rnd(rnd);
  v64bfp16ebs16 r = to_v64bfp16ebs16(a);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v64bfp16ebs8) to_v64bfp16ebs8(v64float a) {
  return to_v64bfp16ebs8((v64accfloat)a);
}
INTRINSIC(v64bfp16ebs16) to_v64bfp16ebs16(v64float a) {
  return to_v64bfp16ebs16((v64accfloat)a);
}

INTRINSIC(v64bfp16ebs8) to_v64bfp16ebs8_conf(v64float a, crrnd_t rnd) {
  crrnd_t prev_rnd = get_rnd();
  set_rnd(rnd);
  v64bfp16ebs8 r = to_v64bfp16ebs8((v64accfloat)a);
  set_rnd(prev_rnd);
  return r;
}

INTRINSIC(v64bfp16ebs16) to_v64bfp16ebs16_conf(v64float a, crrnd_t rnd) {
  crrnd_t prev_rnd = get_rnd();
  set_rnd(rnd);
  v64bfp16ebs16 r = to_v64bfp16ebs16((v64accfloat)a);
  set_rnd(prev_rnd);
  return r;
}

#endif // AIE2P_SRS_H
