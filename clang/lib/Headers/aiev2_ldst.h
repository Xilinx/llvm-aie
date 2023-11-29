//===- aiev2_ldst.h ---------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV2_PACK_UNPACK_H__
#define __AIEV2_PACK_UNPACK_H__

INTRINSIC(v32int8) pack(v32int16 v, int sign) {
  return __builtin_aiev2_pack_I8_I16(v, sign,
                                     __builtin_aiev2_get_ctrl_reg(crSat));
}

INTRINSIC(v32uint8) pack(v32uint16 v, int sign) {
  return __builtin_aiev2_pack_I8_I16(v, sign,
                                     __builtin_aiev2_get_ctrl_reg(crSat));
}

INTRINSIC(v64int4) pack(v64int8 v, int sign) {
  return __builtin_aiev2_pack_I4_I8(v, sign,
                                    __builtin_aiev2_get_ctrl_reg(crSat));
}

INTRINSIC(v64uint4) pack(v64uint8 v, int sign) {
  return __builtin_aiev2_pack_I4_I8(v, sign,
                                    __builtin_aiev2_get_ctrl_reg(crSat));
}

INTRINSIC(v32int8) pack(v32int16 v) {
  return __builtin_aiev2_pack_I8_I16(v, __SIGN_SIGNED,
                                     __builtin_aiev2_get_ctrl_reg(crSat));
}

INTRINSIC(v32uint8) pack(v32uint16 v) {
  return __builtin_aiev2_pack_I8_I16(v, __SIGN_UNSIGNED,
                                     __builtin_aiev2_get_ctrl_reg(crSat));
}

INTRINSIC(v64int4) pack(v64int8 v) {
  return __builtin_aiev2_pack_I4_I8(v, __SIGN_SIGNED,
                                    __builtin_aiev2_get_ctrl_reg(crSat));
}

INTRINSIC(v64uint4) pack(v64uint8 v) {
  return __builtin_aiev2_pack_I4_I8(v, __SIGN_UNSIGNED,
                                    __builtin_aiev2_get_ctrl_reg(crSat));
}

INTRINSIC(v32int16) unpack(v32int8 v, bool sign) {
  return __builtin_aiev2_unpack(v, sign, 0x1);
}

INTRINSIC(v32uint16) unpack(v32uint8 v, bool sign) {
  return __builtin_aiev2_unpack(v, sign, 0x1);
}

INTRINSIC(v64int8) unpack(v64int4 v, bool sign) {
  return __builtin_aiev2_unpack(v, sign, 0x0);
}

INTRINSIC(v64uint8) unpack(v64uint4 v, bool sign) {
  return __builtin_aiev2_unpack(v, sign, 0x0);
}

INTRINSIC(v32int16) unpack(v32int8 v) {
  return __builtin_aiev2_unpack(v, __SIGN_SIGNED, 0x1);
}

INTRINSIC(v32uint16) unpack(v32uint8 v) {
  return __builtin_aiev2_unpack(v, __SIGN_UNSIGNED, 0x1);
}

INTRINSIC(v64int8) unpack(v64int4 v) {
  return __builtin_aiev2_unpack(v, __SIGN_SIGNED, 0x0);
}

INTRINSIC(v64uint8) unpack(v64uint4 v) {
  return __builtin_aiev2_unpack(v, __SIGN_UNSIGNED, 0x0);
}

#endif // __AIEV2_PACK_UNPACK_H__
