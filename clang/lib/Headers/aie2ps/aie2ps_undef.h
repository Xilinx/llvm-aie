//===- aie2ps_undef.h -------------------------------------------*- C++ -*-===//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2PS_UNDEF_H__
#define __AIE2PS_UNDEF_H__

INTRINSIC(v2float) undef_v2float() {
  v2float val;
  return val;
}

INTRINSIC(v32int4) undef_v32int4() {
  v32int4 val;
  return val;
}
INTRINSIC(v32uint4) undef_v32uint4() {
  v32uint4 val;
  return val;
}
INTRINSIC(v16int8) undef_v16int8() {
  v16int8 val;
  return val;
}
INTRINSIC(v16uint8) undef_v16uint8() {
  v16uint8 val;
  return val;
}
INTRINSIC(v8int16) undef_v8int16() {
  v8int16 val;
  return val;
}
INTRINSIC(v8uint16) undef_v8uint16() {
  v8uint16 val;
  return val;
}
INTRINSIC(v4int32) undef_v4int32() {
  v4int32 val;
  return val;
}
INTRINSIC(v4uint32) undef_v4uint32() {
  v4uint32 val;
  return val;
}
INTRINSIC(v8bfloat16) undef_v8bfloat16() {
  v8bfloat16 val;
  return val;
}
INTRINSIC(v8float16) undef_v8float16() {
  v8float16 val;
  return val;
}
INTRINSIC(v16bfloat8) undef_v16bfloat8() {
  v16bfloat8 val;
  return val;
}
INTRINSIC(v16float8) undef_v16float8() {
  v16float8 val;
  return val;
}
INTRINSIC(v4float) undef_v4float() {
  v4float val;
  return val;
}

INTRINSIC(v64int4) undef_v64int4() {
  v64int4 val;
  return val;
}
INTRINSIC(v64uint4) undef_v64uint4() {
  v64uint4 val;
  return val;
}
INTRINSIC(v32int8) undef_v32int8() {
  v32int8 val;
  return val;
}
INTRINSIC(v32uint8) undef_v32uint8() {
  v32uint8 val;
  return val;
}
INTRINSIC(v16int16) undef_v16int16() {
  v16int16 val;
  return val;
}
INTRINSIC(v16uint16) undef_v16uint16() {
  v16uint16 val;
  return val;
}
INTRINSIC(v8int32) undef_v8int32() {
  v8int32 val;
  return val;
}
INTRINSIC(v8uint32) undef_v8uint32() {
  v8uint32 val;
  return val;
}
INTRINSIC(v8accfloat) undef_v8accfloat() {
  v8accfloat val;
  return val;
}
INTRINSIC(v8acc32) undef_v8acc32() {
  v8acc32 val;
  return val;
}
INTRINSIC(v4acc64) undef_v4acc64() {
  v4acc64 val;
  return val;
}
INTRINSIC(v16bfloat16) undef_v16bfloat16() {
  v16bfloat16 val;
  return val;
}
INTRINSIC(v16float16) undef_v16float16() {
  v16float16 val;
  return val;
}
INTRINSIC(v32bfloat8) undef_v32bfloat8() {
  v32bfloat8 val;
  return val;
}
INTRINSIC(v32float8) undef_v32float8() {
  v32float8 val;
  return val;
}
INTRINSIC(v8float) undef_v8float() {
  v8float val;
  return val;
}

INTRINSIC(v32mx9) undef_v32mx9() {
  v32mx9 val;
  return val;
}
[[deprecated("Function undef_v32bfp16p()' is deprecated. Please use "
             "undef_v32mx9()' instead.")]] INTRINSIC(v32bfp16p)
    undef_v32bfp16p() {
  return undef_v32mx9();
}

INTRINSIC(v64mx6) undef_v64mx6() {
  v64mx6 val;
  return val;
}
[[deprecated("Function undef_v64bfp13p()' is deprecated. Please use "
             "undef_v64mx6()' instead.")]] INTRINSIC(v64bfp13p)
    undef_v64bfp13p() {
  return undef_v64mx6();
}
INTRINSIC(v64mx4) undef_v64mx4() {
  v64mx4 val;
  return val;
}
[[deprecated("Function undef_v64bfp11p()' is deprecated. Please use "
             "undef_v64mx4()' instead.")]] INTRINSIC(v64bfp11p)
    undef_v64bfp11p() {
  return undef_v64mx4();
}

INTRINSIC(v128int4) undef_v128int4() {
  v128int4 val;
  return val;
}
INTRINSIC(v128uint4) undef_v128uint4() {
  v128uint4 val;
  return val;
}
INTRINSIC(v64int8) undef_v64int8() {
  v64int8 val;
  return val;
}
INTRINSIC(v64uint8) undef_v64uint8() {
  v64uint8 val;
  return val;
}
INTRINSIC(v32int16) undef_v32int16() {
  v32int16 val;
  return val;
}
INTRINSIC(v32uint16) undef_v32uint16() {
  v32uint16 val;
  return val;
}
INTRINSIC(v16int32) undef_v16int32() {
  v16int32 val;
  return val;
}
INTRINSIC(v16uint32) undef_v16uint32() {
  v16uint32 val;
  return val;
}
INTRINSIC(v16accfloat) undef_v16accfloat() {
  v16accfloat val;
  return val;
}
INTRINSIC(v16acc32) undef_v16acc32() {
  v16acc32 val;
  return val;
}
INTRINSIC(v8acc64) undef_v8acc64() {
  v8acc64 val;
  return val;
}
INTRINSIC(v32bfloat16) undef_v32bfloat16() {
  v32bfloat16 val;
  return val;
}
INTRINSIC(v32float16) undef_v32float16() {
  v32float16 val;
  return val;
}
INTRINSIC(v64bfloat8) undef_v64bfloat8() {
  v64bfloat8 val;
  return val;
}
INTRINSIC(v64float8) undef_v64float8() {
  v64float8 val;
  return val;
}
INTRINSIC(v16float) undef_v16float() {
  v16float val;
  return val;
}

INTRINSIC(v64mx9) undef_v64mx9() {
  v64mx9 val;
  return val;
}
[[deprecated("Function undef_v64bfp16p()' is deprecated. Please use "
             "undef_v64mx9()' instead.")]] INTRINSIC(v64bfp16p)
    undef_v64bfp16p() {
  return undef_v64mx9();
}

INTRINSIC(v128mx6) undef_v128mx6() {
  v128mx6 val;
  return val;
}
[[deprecated("Function undef_v128bfp13p()' is deprecated. Please use "
             "undef_v128mx6()' instead.")]] INTRINSIC(v128bfp13p)
    undef_v128bfp13p() {
  return undef_v128mx6();
}
INTRINSIC(v128mx4) undef_v128mx4() {
  v128mx4 val;
  return val;
}
[[deprecated("Function undef_v128bfp11p()' is deprecated. Please use "
             "undef_v128mx4()' instead.")]] INTRINSIC(v128bfp11p)
    undef_v128bfp11p() {
  return undef_v128mx4();
}

INTRINSIC(v256int4) undef_v256int4() {
  v256int4 val;
  return val;
}
INTRINSIC(v256uint4) undef_v256uint4() {
  v256uint4 val;
  return val;
}
INTRINSIC(v128int8) undef_v128int8() {
  v128int8 val;
  return val;
}
INTRINSIC(v128uint8) undef_v128uint8() {
  v128uint8 val;
  return val;
}
INTRINSIC(v64int16) undef_v64int16() {
  v64int16 val;
  return val;
}
INTRINSIC(v64uint16) undef_v64uint16() {
  v64uint16 val;
  return val;
}
INTRINSIC(v32int32) undef_v32int32() {
  v32int32 val;
  return val;
}
INTRINSIC(v32uint32) undef_v32uint32() {
  v32uint32 val;
  return val;
}
INTRINSIC(v32accfloat) undef_v32accfloat() {
  v32accfloat val;
  return val;
}
INTRINSIC(v32acc32) undef_v32acc32() {
  v32acc32 val;
  return val;
}
INTRINSIC(v16acc64) undef_v16acc64() {
  v16acc64 val;
  return val;
}
INTRINSIC(v64bfloat16) undef_v64bfloat16() {
  v64bfloat16 val;
  return val;
}
INTRINSIC(v64float16) undef_v64float16() {
  v64float16 val;
  return val;
}
INTRINSIC(v128bfloat8) undef_v128bfloat8() {
  v128bfloat8 val;
  return val;
}
INTRINSIC(v128float8) undef_v128float8() {
  v128float8 val;
  return val;
}
INTRINSIC(v32float) undef_v32float() {
  v32float val;
  return val;
}

INTRINSIC(v128mx9) undef_v128mx9() {
  v128mx9 val;
  return val;
}
[[deprecated("Function undef_v128bfp16p()' is deprecated. Please use "
             "undef_v128mx9()' instead.")]] INTRINSIC(v128bfp16p)
    undef_v128bfp16p() {
  return undef_v128mx9();
}

INTRINSIC(v256mx6) undef_v256mx6() {
  v256mx6 val;
  return val;
}
[[deprecated("Function undef_v256bfp13p()' is deprecated. Please use "
             "undef_v256mx6()' instead.")]] INTRINSIC(v256bfp13p)
    undef_v256bfp13p() {
  return undef_v256mx6();
}
INTRINSIC(v256mx4) undef_v256mx4() {
  v256mx4 val;
  return val;
}
[[deprecated("Function undef_v256bfp11p()' is deprecated. Please use "
             "undef_v256mx4()' instead.")]] INTRINSIC(v256bfp11p)
    undef_v256bfp11p() {
  return undef_v256mx4();
}

INTRINSIC(v64accfloat) undef_v64accfloat() {
  v64accfloat val;
  return val;
}
INTRINSIC(v64acc32) undef_v64acc32() {
  v64acc32 val;
  return val;
}
INTRINSIC(v32acc64) undef_v32acc64() {
  v32acc64 val;
  return val;
}
INTRINSIC(v64float) undef_v64float() {
  v64float val;
  return val;
}

INTRINSIC(v256mx9) undef_v256mx9() {
  v256mx9 val;
  return val;
}
[[deprecated("Function undef_v256bfp16p()' is deprecated. Please use "
             "undef_v256mx9()' instead.")]] INTRINSIC(v256bfp16p)
    undef_v256bfp16p() {
  return undef_v256mx9();
}

#endif // __AIE2PS_UNDEF_H__
