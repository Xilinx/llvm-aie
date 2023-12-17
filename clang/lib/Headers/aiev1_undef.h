//===- aiev1_undef.h --------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV1_UNDEF_H__
#define __AIEV1_UNDEF_H__

INTRINSIC(v4float) undef_v4float() { return __builtin_aie_v4f32undef(); }
INTRINSIC(v8float) undef_v8float() { return __builtin_aie_v8f32undef(); }
INTRINSIC(v16float) undef_v16float() { return __builtin_aie_v16f32undef(); }
INTRINSIC(v32float) undef_v32float() { return __builtin_aie_v32f32undef(); }
INTRINSIC(v2int32) undef_v2int32() { return __builtin_aie_v2i32undef(); }
INTRINSIC(v8acc48) undef_v8acc48() { return __builtin_aie_v8i48undef(); }
INTRINSIC(v8acc80) undef_v8acc80() { return __builtin_aie_v16i48undef(); }
INTRINSIC(v16acc48) undef_v16acc48() { return __builtin_aie_v16i48undef(); }
INTRINSIC(v4acc80) undef_v4acc80() { return __builtin_aie_v8i48undef(); }
INTRINSIC(v16int16) undef_v16int16() { return __builtin_aie_v16i16undef(); }
INTRINSIC(v16int32) undef_v16int32() { return __builtin_aie_v16i32undef(); }
INTRINSIC(v32int16) undef_v32int16() { return __builtin_aie_v32i16undef(); }
INTRINSIC(v32int32) undef_v32int32() { return __builtin_aie_v32i32undef(); }
INTRINSIC(v128int8) undef_v128int8() { return __builtin_aie_v128i8undef(); }

#endif /*__AIEV1_UNDEF_H__*/
