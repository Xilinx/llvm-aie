//===- aiev2_undef.h --------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV2_UNDEF_H__
#define __AIEV2_UNDEF_H__

INTRINSIC(v32int4) undef_v32int4() { return __builtin_aiev2_v4int32(); }
INTRINSIC(v32uint4) undef_v32uint4() { return __builtin_aiev2_v4int32(); }
INTRINSIC(v16int8) undef_v16int8() { return __builtin_aiev2_v16int8(); }
INTRINSIC(v16uint8) undef_v16uint8() { return __builtin_aiev2_v16int8(); }
INTRINSIC(v8int16) undef_v8int16() { return __builtin_aiev2_v8int16(); }
INTRINSIC(v8uint16) undef_v8uint16() { return __builtin_aiev2_v8int16(); }
INTRINSIC(v4int32) undef_v4int32() { return __builtin_aiev2_v4int32(); }
INTRINSIC(v4uint32) undef_v4uint32() { return __builtin_aiev2_v4int32(); }
INTRINSIC(v64int4) undef_v64int4() { return __builtin_aiev2_v8int32(); }
INTRINSIC(v64uint4) undef_v64uint4() { return __builtin_aiev2_v8int32(); }
INTRINSIC(v32int8) undef_v32int8() { return __builtin_aiev2_v32int8(); }
INTRINSIC(v32uint8) undef_v32uint8() { return __builtin_aiev2_v32int8(); }
INTRINSIC(v16int16) undef_v16int16() { return __builtin_aiev2_v16int16(); }
INTRINSIC(v16uint16) undef_v16uint16() { return __builtin_aiev2_v16int16(); }
INTRINSIC(v8int32) undef_v8int32() { return __builtin_aiev2_v8int32(); }
INTRINSIC(v8uint32) undef_v8uint32() { return __builtin_aiev2_v8int32(); }
INTRINSIC(v8acc32) undef_v8acc32() { return __builtin_aiev2_v8acc32(); }
INTRINSIC(v4acc64) undef_v4acc64() { return __builtin_aiev2_v4acc64(); }
INTRINSIC(v128int4) undef_v128int4() { return __builtin_aiev2_v16int32(); }
INTRINSIC(v128uint4) undef_v128uint4() { return __builtin_aiev2_v16int32(); }
INTRINSIC(v64int8) undef_v64int8() { return __builtin_aiev2_v64int8(); }
INTRINSIC(v64uint8) undef_v64uint8() { return __builtin_aiev2_v64int8(); }
INTRINSIC(v32int16) undef_v32int16() { return __builtin_aiev2_v32int16(); }
INTRINSIC(v32uint16) undef_v32uint16() { return __builtin_aiev2_v32int16(); }
INTRINSIC(v16int32) undef_v16int32() { return __builtin_aiev2_v16int32(); }
INTRINSIC(v16uint32) undef_v16uint32() { return __builtin_aiev2_v16int32(); }
INTRINSIC(v16acc32) undef_v16acc32() { return __builtin_aiev2_v16acc32(); }
INTRINSIC(v8acc64) undef_v8acc64() { return __builtin_aiev2_v8acc64(); }
INTRINSIC(v256int4) undef_v256int4() { return __builtin_aiev2_v32int32(); }
INTRINSIC(v256uint4) undef_v256uint4() { return __builtin_aiev2_v32int32(); }
INTRINSIC(v128int8) undef_v128int8() { return __builtin_aiev2_v128int8(); }
INTRINSIC(v128uint8) undef_v128uint8() { return __builtin_aiev2_v128int8(); }
INTRINSIC(v64int16) undef_v64int16() { return __builtin_aiev2_v64int16(); }
INTRINSIC(v64uint16) undef_v64uint16() { return __builtin_aiev2_v64int16(); }
INTRINSIC(v32int32) undef_v32int32() { return __builtin_aiev2_v32int32(); }
INTRINSIC(v32uint32) undef_v32uint32() { return __builtin_aiev2_v32int32(); }
INTRINSIC(v32acc32) undef_v32acc32() { return __builtin_aiev2_v32acc32(); }
INTRINSIC(v16acc64) undef_v16acc64() { return __builtin_aiev2_v16acc64(); }
INTRINSIC(v8accfloat) undef_v8accfloat() {
  return __builtin_aiev2_v8accfloat();
}
INTRINSIC(v16accfloat) undef_v16accfloat() {
  return __builtin_aiev2_v16accfloat();
}
INTRINSIC(v32accfloat) undef_v32accfloat() {
  return __builtin_aiev2_v32accfloat();
}
INTRINSIC(v8bfloat16) undef_v8bfloat16() {
  return __builtin_aiev2_v8bfloat16();
}
INTRINSIC(v16bfloat16) undef_v16bfloat16() {
  return __builtin_aiev2_v16bfloat16();
}
INTRINSIC(v32bfloat16) undef_v32bfloat16() {
  return __builtin_aiev2_v32bfloat16();
}
INTRINSIC(v64bfloat16) undef_v64bfloat16() {
  return __builtin_aiev2_v64bfloat16();
}
INTRINSIC(v8float) undef_v8float() { return __builtin_aiev2_v8float(); }
INTRINSIC(v16float) undef_v16float() { return __builtin_aiev2_v16float(); }
INTRINSIC(v32float) undef_v32float() { return __builtin_aiev2_v32float(); }

#endif /*__AIEV2_UNDEF_H__*/
