//===- aiev2_ups.h ----------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV2_UPS_H__
#define __AIEV2_UPS_H__

INTRINSIC(v8acc64) lups(v8int32 a, int shft, int sign) {
  return __builtin_aiev2_acc64_v8_I256_ups(a, shft, sign);
}
inline v8acc64 lups(v8int32 a, int shft) {
  return lups(a, shft, __SIGN_SIGNED);
}

INTRINSIC(v8acc64) lups(v8uint32 a, int shft, int sign) {
  return __builtin_aiev2_acc64_v8_I256_ups(a, shft, sign);
}
inline v8acc64 lups(v8uint32 a, int shft) {
  return lups(a, shft, __SIGN_UNSIGNED);
}

INTRINSIC(v16acc32) sups(v16int16 a, int shft, int sign) {
  return __builtin_aiev2_acc32_v16_I256_ups(a, shft, sign);
}
inline v16acc32 sups(v16int16 a, int shft) {
  return sups(a, shft, __SIGN_SIGNED);
}

INTRINSIC(v16acc32) sups(v16uint16 a, int shft, int sign) {
  return __builtin_aiev2_acc32_v16_I256_ups(a, shft, sign);
}
inline v16acc32 sups(v16uint16 a, int shft) {
  return sups(a, shft, __SIGN_UNSIGNED);
}

#if 0
inline v4cacc64 lups(v4cint32 a, int shft, int sign) {
   uint6_t sft = (uint6_t)(shft);
   bool of;
   v4cacc64 result = me_primitive::ups_w2b(a, sft,(uint1_t)acc_mode_64, (uint1_t)sign, get_satmode(), of); return result;
}

inline v4cacc64 lups(v4cint32 a, int shft) {
   uint6_t sft = (uint6_t)(shft);
   bool of;
   v4cacc64 result = me_primitive::ups_w2b(a, sft, (uint1_t)acc_mode_64 (uint1_t)SIGN_SIGNED, get_satmode(), of); return result;
 }
#endif

INTRINSIC(v32acc32) sups(v32int8 a, int shft, int sign) {
  return __builtin_aiev2_acc32_v32_I256_ups(a, shft, sign);
}
inline v32acc32 sups(v32int8 a, int shft) {
  return sups(a, shft, __SIGN_SIGNED);
}

INTRINSIC(v32acc32) sups(v32uint8 a, int shft, int sign) {
  return __builtin_aiev2_acc32_v32_I256_ups(a, shft, sign);
}
inline v32acc32 sups(v32uint8 a, int shft) {
  return sups(a, shft, __SIGN_UNSIGNED);
}

INTRINSIC(v16acc64) lups(v16int16 a, int shft, int sign) {
  return __builtin_aiev2_acc64_v16_I256_ups(a, shft, sign);
}
inline v16acc64 lups(v16int16 a, int shft) {
  return lups(a, shft, __SIGN_SIGNED);
}

INTRINSIC(v16acc64) lups(v16uint16 a, int shft, int sign) {
  return __builtin_aiev2_acc64_v16_I256_ups(a, shft, sign);
}
inline v16acc64 lups(v16uint16 a, int shft) {
  return lups(a, shft, __SIGN_UNSIGNED);
}

#if 0
inline v8cacc64 lups(v8cint16 a, int shft, int sign) {
   uint6_t sft = (uint6_t)(shft);
   bool of;
   v8cacc64 result = me_primitive::ups_w2c(a, sft,(uint1_t)acc_mode_64, (uint1_t)sign, get_satmode(), of); return result;
}
inline v8cacc64 lups(v8cint16 a, int shft) { uint6_t sft = (uint6_t)(shft);
 bool of;
 v8cacc64 result = me_primitive::ups_w2c(a, sft, (uint1_t)acc_mode_64,(uint1_t)SIGN_SIGNED, get_satmode(), of); return result;
}
#endif

inline v16acc64 lups(v16int32 a, int shft, int sign) {
  v16acc64 result = set_v16acc64(0, lups(extract_v8int32(a, 0), shft, sign));
  result = insert(result, 1, lups(extract_v8int32(a, 1), shft, sign));
  return result;
}
inline v16acc64 lups(v16int32 a, int shft) {
  return lups(a, shft, __SIGN_SIGNED);
}

inline v16acc64 lups(v16uint32 a, int shft, int sign) {
  v16acc64 result = set_v16acc64(0, lups(extract_v8uint32(a, 0), shft, sign));
  result = insert(result, 1, lups(extract_v8uint32(a, 1), shft, sign));
  return result;
}
inline v16acc64 lups(v16uint32 a, int shft) {
  return lups(a, shft, __SIGN_UNSIGNED);
}

inline v32acc32 sups(v32int16 a, int shft, int sign) {
  v32acc32 result = set_v32acc32(0, sups(extract_v16int16(a, 0), shft, sign));
  result = insert(result, 1, sups(extract_v16int16(a, 1), shft, sign));
  return result;
}
inline v32acc32 sups(v32int16 a, int shft) {
  return sups(a, shft, __SIGN_SIGNED);
}

inline v32acc32 sups(v32uint16 a, int shft, int sign) {
  v32acc32 result = set_v32acc32(0, sups(extract_v16uint16(a, 0), shft, sign));
  result = insert(result, 1, sups(extract_v16uint16(a, 1), shft, sign));
  return result;
}
inline v32acc32 sups(v32uint16 a, int shft) {
  return sups(a, shft, __SIGN_UNSIGNED);
}

#if 0
inline v8cacc64 lups(v8cint32 a, int shft, int sign) {
   uint6_t sft = (uint6_t)(shft); bool of;
   v8cacc64 result = set_v8cacc64( 0, me_primitive::ups_w2b( extract_v4cint32 (a, 0), sft, (uint1_t)acc_mode_64, (uint1_t)sign, get_satmode(), of));
   result = insert ( result, 1, me_primitive::ups_w2b( extract_v4cint32 (a, 1), sft, (uint1_t)acc_mode_64,(uint1_t)sign, get_satmode(), of)); return result;
 }

inline v8cacc64 lups(v8cint32 a, int shft) {
   uint6_t sft = (uint6_t)(shft); bool of;
   v8cacc64 result = set_v8cacc64( 0, me_primitive::ups_w2b( extract_v4cint32 (a,
0), sft, (uint1_t)acc_mode_64, (uint1_t)SIGN_SIGNED, get_satmode(), of));
   result = insert ( result, 1, me_primitive::ups_w2b( extract_v4cint32 (a, 1), sft,
(uint1_t)acc_mode_64, (uint1_t)SIGN_SIGNED, get_satmode(), of)); return result;
}
#endif

inline v8acc64 ups_to_v8acc64(v8int32 a, int shft, int sign) {
  return lups(a, shft, sign);
}
inline v8acc64 ups_to_v8acc64(v8int32 a, int shft) {
  return lups(a, shft, __SIGN_SIGNED);
}
inline v8acc64 ups_to_v8acc64(v8uint32 a, int shft, int sign) {
  return lups(a, shft, sign);
}
inline v8acc64 ups_to_v8acc64(v8uint32 a, int shft) {
  return lups(a, shft, __SIGN_UNSIGNED);
}
inline v16acc32 ups_to_v16acc32(v16int16 a, int shft, int sign) {
  return sups(a, shft, sign);
}
inline v16acc32 ups_to_v16acc32(v16int16 a, int shft) {
  return sups(a, shft, __SIGN_SIGNED);
}
inline v16acc32 ups_to_v16acc32(v16uint16 a, int shft, int sign) {
  return sups(a, shft, sign);
}
inline v16acc32 ups_to_v16acc32(v16uint16 a, int shft) {
  return sups(a, shft, __SIGN_UNSIGNED);
}
#if 0
inline v4cacc64 ups_to_v4cacc64(v4cint32 a, int shft, int sign) {
   uint6_t sft = (uint6_t)(shft); bool of;
   v4cacc64 result = me_primitive::ups_w2b(a, sft, (uint1_t)acc_mode_64, (uint1_t)sign, get_satmode(), of); return result;
}
inline v4cacc64 ups_to_v4cacc64(v4cint32 a, int shft) {
   uint6_t sft = (uint6_t)(shft); bool of;
   v4cacc64 result = me_primitive::ups_w2b(a, sft,(uint1_t)acc_mode_64, (uint1_t)SIGN_SIGNED, get_satmode(), of); return result;
 }
#endif

inline v32acc32 ups_to_v32acc32(v32int8 a, int shft, int sign) {
  return sups(a, shft, sign);
}
inline v32acc32 ups_to_v32acc32(v32int8 a, int shft) {
  return sups(a, shft, __SIGN_SIGNED);
}
inline v32acc32 ups_to_v32acc32(v32uint8 a, int shft, int sign) {
  return sups(a, shft, sign);
}
inline v32acc32 ups_to_v32acc32(v32uint8 a, int shft) {
  return sups(a, shft, __SIGN_UNSIGNED);
}
inline v16acc64 ups_to_v16acc64(v16int16 a, int shft, int sign) {
  return lups(a, shft, sign);
}
inline v16acc64 ups_to_v16acc64(v16int16 a, int shft) {
  return lups(a, shft, __SIGN_SIGNED);
}
inline v16acc64 ups_to_v16acc64(v16uint16 a, int shft, int sign) {
  return lups(a, shft, sign);
}
inline v16acc64 ups_to_v16acc64(v16uint16 a, int shft) {
  return lups(a, shft, __SIGN_UNSIGNED);
}

#if 0
inline v8cacc64 ups_to_v8cacc64(v8cint16 a, int shft, int sign) {
   uint6_t sft = (uint6_t)(shft); bool of;
   v8cacc64 result = me_primitive::ups_w2c(a, sft,(uint1_t)acc_mode_64, (uint1_t)sign, get_satmode(), of); return result;
 }
inline v8cacc64 ups_to_v8cacc64(v8cint16 a, int shft) {
   uint6_t sft = (uint6_t)(shft); bool of;
   v8cacc64 result = me_primitive::ups_w2c(a, sft,(uint1_t)acc_mode_64, (uint1_t)SIGN_SIGNED, get_satmode(), of); return result;
 }
inline v8cacc64 ups_to_v8cacc64(v8cint32 a, int shft, int sign) {
   uint6_t sft = (uint6_t)(shft); bool of;
   v8cacc64 result = me_primitive::ups_x2c(a, sft,(uint1_t)acc_mode_64, (uint1_t)sign, get_satmode(), of); return result;
}
inline v8cacc64 ups_to_v8cacc64(v8cint32 a, int shft) {
   uint6_t sft = (uint6_t)(shft); bool of;
   v8cacc64 result = me_primitive::ups_x2c(a, sft,(uint1_t)acc_mode_64, (uint1_t)SIGN_SIGNED, get_satmode(), of); return result;
}
#endif

INTRINSIC(v16acc64) ups_to_v16acc64(v16int32 a, int shft, int sign) {
  return __builtin_aiev2_acc64_v16_I512_ups(a, shft, sign);
}
inline v16acc64 ups_to_v16acc64(v16int32 a, int shft) {
  return ups_to_v16acc64(a, shft, __SIGN_SIGNED);
}

INTRINSIC(v16acc64) ups_to_v16acc64(v16uint32 a, int shft, int sign) {
  return __builtin_aiev2_acc64_v16_I512_ups(a, shft, sign);
}
inline v16acc64 ups_to_v16acc64(v16uint32 a, int shft) {
  return ups_to_v16acc64(a, shft, __SIGN_UNSIGNED);
}

INTRINSIC(v32acc32) ups_to_v32acc32(v32int16 a, int shft, int sign) {
  return __builtin_aiev2_acc32_v32_I512_ups(a, shft, sign);
}
inline v32acc32 ups_to_v32acc32(v32int16 a, int shft) {
  return ups_to_v32acc32(a, shft, __SIGN_SIGNED);
}

INTRINSIC(v16accfloat) ups_to_v16accfloat(v16bfloat16 a) {
  return __builtin_aiev2_v16bf16_to_v16accfloat(a);
}

INTRINSIC(v16accfloat) ups(v16bfloat16 a) { return ups_to_v16accfloat(a); }

INTRINSIC(v32acc32) ups_to_v32acc32(v32uint16 a, int shft, int sign) {
  return __builtin_aiev2_acc32_v32_I512_ups(a, shft, sign);
}

INTRINSIC(v32acc32) ups_to_v32acc32(v32uint16 a, int shft) {
  return ups_to_v32acc32(a, shft, __SIGN_UNSIGNED);
}

INTRINSIC(v32accfloat) ups_to_v32accfloat(v32bfloat16 a) {
  v16accfloat x0 = ups_to_v16accfloat(extract_v16bfloat16(a, 0));
  v16accfloat x1 = ups_to_v16accfloat(extract_v16bfloat16(a, 1));
  return concat(x0, x1);
}

#endif //__AIEV2_UPS_H__
