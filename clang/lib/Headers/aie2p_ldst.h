//===-------------------- AIEngine AIE2p intrinsics ------------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef AIE2P_LDST_H
#define AIE2P_LDST_H

//* Automatically generated file, do not edit! *
//
INTRINSIC(v32int8) pack(v32int16 v, int sign) {
  return __builtin_aie2p_pack_I512_I8_I16(v, sign);
}
INTRINSIC(v32uint8) pack(v32uint16 v, int sign) {
  return __builtin_aie2p_pack_I512_I8_I16(v, sign);
}
INTRINSIC(v64int4) pack(v64int8 v, int sign) {
  return __builtin_aie2p_pack_I512_I4_I8(v, sign);
  ;
}
INTRINSIC(v64uint4) pack(v64uint8 v, int sign) {
  return __builtin_aie2p_pack_I512_I4_I8(v, sign);
}
INTRINSIC(v64int8) pack(v64int16 v, int sign) {
  return __builtin_aie2p_pack_I1024_I8_I16(v, sign);
}
INTRINSIC(v64uint8) pack(v64uint16 v, int sign) {
  return __builtin_aie2p_pack_I1024_I8_I16(v, sign);
}
INTRINSIC(v128int4) pack(v128int8 v, int sign) {
  return __builtin_aie2p_pack_I1024_I4_I8(v, sign);
}
INTRINSIC(v128uint4) pack(v128uint8 v, int sign) {
  return __builtin_aie2p_pack_I1024_I4_I8(v, sign);
}

INTRINSIC(v32int8) pack(v32int16 v) { return pack(v, __SIGN_SIGNED); }
INTRINSIC(v32uint8) pack(v32uint16 v) { return pack(v, __SIGN_UNSIGNED); }
INTRINSIC(v64int4) pack(v64int8 v) { return pack(v, __SIGN_SIGNED); }
INTRINSIC(v64uint4) pack(v64uint8 v) { return pack(v, __SIGN_UNSIGNED); }
INTRINSIC(v64int8) pack(v64int16 v) { return pack(v, __SIGN_SIGNED); }
INTRINSIC(v64uint8) pack(v64uint16 v) { return pack(v, __SIGN_UNSIGNED); }
INTRINSIC(v128int4) pack(v128int8 v) { return pack(v, __SIGN_SIGNED); }
INTRINSIC(v128uint4) pack(v128uint8 v) { return pack(v, __SIGN_UNSIGNED); }

INTRINSIC(v32int16) unpack(v32int8 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I512_I16_I8(v, sign);
}
INTRINSIC(v32uint16) unpack(v32uint8 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I512_I16_I8(v, sign);
}
INTRINSIC(v64int8) unpack(v64int4 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I512_I8_I4(v, sign);
}
INTRINSIC(v64uint8) unpack(v64uint4 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I512_I8_I4(v, sign);
}
INTRINSIC(v64int16) unpack(v64int8 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I1024_I16_I8(v, sign);
}
INTRINSIC(v64uint16) unpack(v64uint8 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I1024_I16_I8(v, sign);
}
INTRINSIC(v128int8) unpack(v128int4 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I1024_I8_I4(v, sign);
}
INTRINSIC(v128uint8) unpack(v128uint4 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I1024_I8_I4(v, sign);
}

INTRINSIC(v32int16) unpack(v32int8 v) { return unpack(v, __SIGN_SIGNED); }
INTRINSIC(v32uint16) unpack(v32uint8 v) { return unpack(v, __SIGN_UNSIGNED); }
INTRINSIC(v64int8) unpack(v64int4 v) { return unpack(v, __SIGN_SIGNED); }
INTRINSIC(v64uint8) unpack(v64uint4 v) { return unpack(v, __SIGN_UNSIGNED); }
INTRINSIC(v64int16) unpack(v64int8 v) { return unpack(v, __SIGN_SIGNED); }
INTRINSIC(v64uint16) unpack(v64uint8 v) { return unpack(v, __SIGN_UNSIGNED); }
INTRINSIC(v128int8) unpack(v128int4 v) { return unpack(v, __SIGN_SIGNED); }
INTRINSIC(v128uint8) unpack(v128uint4 v) { return unpack(v, __SIGN_UNSIGNED); }

#endif // AIE2P_LDST_H
